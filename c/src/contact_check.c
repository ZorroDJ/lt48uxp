#include <labtool48/labtool48.h>
#include "internal.h"
#include "../../NewDeviceLib/sources/pic-common/source_contact_masks.h"
#include "../../NewDeviceLib/sources/pic-common/source_contact_capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int payload_digest(const unsigned char *data,size_t size,const char *expected) {
    unsigned char digest[32];char hex[65];lt48_sha256(data,size,digest);
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    return !strcmp(hex,expected);
}
/* Only the exact previously audited dispatcher is accepted. It is read from
 * the owner's file or extracted from a descriptor in their executable. */
lt48_status lt48_contact_dispatcher_load_internal(const char *root,unsigned char out[476]) {
    if(!root||!out)return LT48_INVALID_ARGUMENT;
    const char *names[]={"contact-dispatcher.bin","WDATA48UXP.exe","WDATA48UXP.EXE","WLT48UXP.exe","WLT48UXP.EXE"};
    for(unsigned i=0;i<sizeof(names)/sizeof(*names);i++) {
        size_t n=strlen(root)+strlen(names[i])+2;char *path=malloc(n);
        if(!path)return LT48_NO_MEMORY;
        snprintf(path,n,"%s/%s",root,names[i]);FILE *f=fopen(path,"rb");free(path);
        if(!f)continue;
        if(fseek(f,0,SEEK_END)){fclose(f);return LT48_FILE_ERROR;}
        long end=ftell(f);
        if(end<0||end>32*1024*1024||fseek(f,0,SEEK_SET)){fclose(f);return LT48_FORMAT_ERROR;}
        size_t size=(size_t)end;unsigned char *bytes=malloc(size?size:1);
        if(!bytes){fclose(f);return LT48_NO_MEMORY;}
        int valid=fread(bytes,1,size,f)==size&&!ferror(f);fclose(f);
        if(!valid){free(bytes);return LT48_FILE_ERROR;}
        size_t offset=i?4:0;int found=0;
        for(;offset<=size&&size-offset>=476;offset++) {
            if(i&&memcmp(bytes+offset-4,"\0\xb0\xdc\1",4))continue;
            if(payload_digest(bytes+offset,476,"6c298f402055ea4fd0c90a728992a09062bbc05964d47b3e9e463da9732af788")) {
                memcpy(out,bytes+offset,476);found=1;break;
            }
            if(!i)break;
        }
        free(bytes);if(found&&(!i?size==476:1))return LT48_OK;
        if(!i)return LT48_SAFETY_REJECTED;
    }
    return LT48_NOT_FOUND;
}

lt48_status lt48_installation_contact_check_profile(const lt48_installation *installation,
    size_t device,lt48_contact_check_profile *out) {
    if(!installation||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    lt48_device_spec spec;lt48_rez *rez=NULL;lt48_pin_code code;size_t index;
    lt48_status st=lt48_installation_resolve_device(installation,device,&spec);
    if(st!=LT48_OK)return st;
    const char *base=lt48_installation_path(installation);
    if(!base||!spec.definition_stem)return LT48_FORMAT_ERROR;
    size_t size=strlen(base)+strlen(spec.definition_stem)+7;
    char *path=malloc(size);if(!path)return LT48_NO_MEMORY;
    snprintf(path,size,"%s/%s.rez",base,spec.definition_stem);
    st=lt48_rez_open(path,&rez);free(path);
    char resource[24];snprintf(resource,sizeof(resource),"P%x",spec.pinmap);
    if(st==LT48_OK)st=lt48_rez_find_resource(rez,resource,&index);
    if(st==LT48_OK)st=lt48_rez_get_pin_code(rez,index,&code);
    lt48_pic_contact_masks masks={0};
    if(st==LT48_OK&&code.pin_count!=spec.pin_count)st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK)st=lt48_pic_source_contact_masks(code.pin_count,code.data,code.data_size,&masks);
    if(st==LT48_OK) {
        out->pin_count=code.pin_count;out->alternate=masks.alternate;
        memcpy(out->required,masks.required,sizeof(out->required));
        memcpy(out->forbidden,masks.forbidden,sizeof(out->forbidden));
        memcpy(out->auxiliary,masks.auxiliary,sizeof(out->auxiliary));
        if(!(out->required[0]|out->required[1]))st=LT48_SAFETY_REJECTED;
    }
    lt48_rez_close(rez);
    if(st==LT48_OK) {
        unsigned char *idle=NULL;size_t count=0;
        st=lt48_load_verified_idle_fpga_internal(base,&idle,&count);
        if(st==LT48_OK&&count!=sizeof(out->idle_fpga))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK){memcpy(out->idle_fpga,idle,count);out->idle_size=count;}free(idle);
    }
    if(st==LT48_OK&&out->alternate) {
        st=lt48_contact_dispatcher_load_internal(base,out->alternate_payload);
        if(st==LT48_OK)out->alternate_size=sizeof(out->alternate_payload);
    }
    return st;
}

typedef struct contact_io {lt48_runtime_transport_session *session;} contact_io;
static lt48_status contact_write(void *p,const unsigned char *bytes,size_t size) {
    return lt48_runtime_transport_session_controller_write(((contact_io*)p)->session,bytes,size);
}
static lt48_status contact_read(void *p,unsigned char *bytes,size_t size) {
    return lt48_runtime_transport_session_controller_capture_internal(((contact_io*)p)->session,bytes,(unsigned)size);
}
static lt48_status contact_restore(void *p) {
    const unsigned char zero=0;return contact_write(p,&zero,1);
}
static lt48_status contact_timeout(void *p,unsigned milliseconds) {
    return lt48_runtime_transport_session_wave_watchdog(((contact_io*)p)->session,milliseconds,0);
}

lt48_status lt48_contact_check_execute(const lt48_contact_check_profile *profile,
    const lt48_protocol_transport *api,void *context,lt48_contact_check_result *out) {
    if(!profile||!api||!api->write||!api->read||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->operation_status=LT48_SAFETY_REJECTED;
    out->cleanup_status=LT48_OK;
    if(profile->alternate>1)return LT48_INVALID_ARGUMENT;
    if(!profile->pin_count||profile->pin_count>48||
       !(profile->required[0]|profile->required[1]))return LT48_SAFETY_REJECTED;
    if(profile->idle_size!=sizeof(profile->idle_fpga)||
       !payload_digest(profile->idle_fpga,profile->idle_size,"1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab"))
        return LT48_SAFETY_REJECTED;
    if(profile->alternate&&(profile->alternate_size!=476||
       !payload_digest(profile->alternate_payload,476,"6c298f402055ea4fd0c90a728992a09062bbc05964d47b3e9e463da9732af788"))) {
        out->operation_status=LT48_UNSUPPORTED;return LT48_UNSUPPORTED;
    }
    for(unsigned i=0;i<2;i++)if((profile->required[i]|profile->forbidden[i]|profile->auxiliary[i])&0xff000000u)
        return LT48_INVALID_ARGUMENT;
    if(api->cancelled&&api->cancelled(context)){out->operation_status=LT48_CANCELLED;return LT48_CANCELLED;}
    contact_io io={0};lt48_status st=lt48_runtime_transport_session_create(api,context,&io.session);
    if(st!=LT48_OK){out->operation_status=st;return st;}
    lt48_pic_contact_io callbacks={contact_write,contact_read,contact_timeout,contact_restore,&io};
    unsigned char raw[12];out->attempted=1;
    const unsigned char reset[]={0x49,0xaa,0x55},configure[]={0x38,0xaa,0x55};
    st=contact_timeout(&io,5000);
    if(st==LT48_OK)st=contact_write(&io,reset,sizeof(reset));
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(io.session);
    if(st==LT48_OK)st=contact_write(&io,configure,sizeof(configure));
    if(st==LT48_OK)st=contact_write(&io,profile->idle_fpga,profile->idle_size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(io.session);
    if(st==LT48_OK)st=lt48_pic_source_contact_capture(&callbacks,profile->alternate,0xb000,
        profile->alternate_payload,profile->alternate_size,raw);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(io.session);
    if(st==LT48_OK){memcpy(out->raw,raw,sizeof(raw));out->capture_completed=1;}
    lt48_runtime_transport_session_close(io.session);io.session=NULL;
    /* A separate, noncancellable finalizer can run even after a poisoned session. */
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
    lt48_status cs=lt48_runtime_transport_session_create(&cleanup,context,&io.session);
    const unsigned char finalizer[]={0x49,0xaa,0x55};
    if(cs==LT48_OK&&profile->alternate)cs=contact_restore(&io);
    if(cs==LT48_OK)cs=contact_write(&io,finalizer,sizeof(finalizer));
    if(cs==LT48_OK)cs=lt48_runtime_transport_session_flush(io.session);
    lt48_runtime_transport_session_close(io.session);
    out->cleanup_status=cs;out->cleanup_completed=cs==LT48_OK;
    if(st==LT48_OK) {
        lt48_pic_contact_masks masks={0};lt48_pic_contact_result decoded;
        masks.alternate=profile->alternate;
        memcpy(masks.required,profile->required,sizeof(masks.required));
        memcpy(masks.forbidden,profile->forbidden,sizeof(masks.forbidden));
        memcpy(masks.auxiliary,profile->auxiliary,sizeof(masks.auxiliary));
        out->matched=lt48_pic_source_contact_match(&masks,raw,&decoded);
        memcpy(out->observed,decoded.observed,sizeof(out->observed));
        memcpy(out->missing,decoded.missing,sizeof(out->missing));
        memcpy(out->unexpected,decoded.unexpected,sizeof(out->unexpected));
        if(!out->matched)st=LT48_DEVICE_MISMATCH;
    }
    out->operation_status=st;
    return cs!=LT48_OK?LT48_CLEANUP_FAILED:st;
}
