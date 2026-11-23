#include "at89_s2051.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
/* Exact S2051/4051 operation owner; source-audited, hardware qualification pending. */
lt48_status lt48_s2051_validate(const lt48_s2051_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r || !length || start>=(r->flash+32u) || length>(r->flash+32u)-start)return LT48_SAFETY_REJECTED;
    if((op<1 || op>8) && op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)
        if(start || length!=(r->flash+32u))return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_CONFIGURATION) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=7 || (image[0]&0xc0))return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<6;i++)if((image[0]&(1u<<i)) && image[i+1]>1)return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_SECURE) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=1 || !image[0] || image[0]>2)return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE) {
        if(!image || size!=length)return LT48_INVALID_ARGUMENT;
    }
    return LT48_OK;
}
typedef struct s2051_session {
    const lt48_s2051_resources *r;const lt48_protocol_transport *api;void *ctx;
    lt48_runtime_transport_session *wire;lt48_compile_variable vars[6];
    unsigned char buffers[32][32];lt48_runtime_configuration_value config[6];size_t config_count;
    uint32_t volts[4];unsigned mode,restore,ready;unsigned char saved[6];
} s2051_session;
static void address_s2051(s2051_session *s,unsigned image) {
    uint32_t physical=0,remaining=0;lt48_s2051_address(s->r,image,&physical,&remaining);
    for(unsigned i=0;i<4;i++){s->buffers[0][i]=(unsigned char)(physical>>(8*i));s->buffers[1][i]=(unsigned char)(image>>(8*i));}
}
static lt48_status run_s2051(s2051_session *s,unsigned op) {
    lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[31];size_t n=0;
    for(unsigned i=0;i<32;i++)if(i!=8)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    o.mode=s->mode;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=8;o.parameter_buffer_id=3;
    o.initial_buffers=seeds;o.initial_buffer_count=n;o.configuration=s->config;o.configuration_count=s->config_count;
    memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);o.root_timeout_seconds=10;
    lt48_status st=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
    if(st==LT48_OK)st=lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,6,&o,s->r->addresses,4,s->buffers,s->config,6,&s->config_count,s->volts);
    return st;
}
static lt48_status transfer_s2051(s2051_session *s,int input,unsigned ep,unsigned char *p,size_t n) {
    if(s->api->cancelled && s->api->cancelled(s->ctx))return LT48_CANCELLED;
    size_t done=0;lt48_status st=input?s->api->read(s->ctx,(uint8_t)ep,p,n,&done):s->api->write(s->ctx,(uint8_t)ep,p,n,&done);
    return st==LT48_OK && done!=n?LT48_PROTOCOL_ERROR:st;
}
static lt48_status command_s2051(s2051_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    lt48_status st=lt48_runtime_transport_session_flush(s->wire);
    if(st==LT48_OK)st=transfer_s2051(s,0,2,(unsigned char *)p,n);
    if(st==LT48_OK && reply)st=transfer_s2051(s,1,0x88,reply,512);
    return st;
}
static lt48_status stop_s2051(s2051_session *s) {
    const unsigned char stop[]={0,0};unsigned char reply[512]={0};
    lt48_status st=command_s2051(s,stop,sizeof stop,reply);
    return st==LT48_OK && !(reply[0]|reply[1])?LT48_PROTOCOL_ERROR:st;
}
static lt48_status block_s2051(s2051_session *s,int writing,unsigned char *data,size_t n) {
    unsigned char h[]={writing?0x48:0x43,(unsigned char)(n/512),(unsigned char)(n/512>>8),(unsigned char)(n%512),(unsigned char)(n%512>>8),writing?1:0},reply[512]={0};
    lt48_status st=command_s2051(s,h,sizeof h,NULL);
    if(st==LT48_OK)st=transfer_s2051(s,!writing,writing?4:0x88,data,n);
    if(st==LT48_OK && writing) {
        st=transfer_s2051(s,1,0x88,reply,512);if(st==LT48_OK && !(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;
        const unsigned char end[]={0x0c,1};if(st==LT48_OK)st=command_s2051(s,end,sizeof end,reply);
        if(st==LT48_OK && (!(reply[0]|reply[1]) || !(reply[2]&1)))st=LT48_DEVICE_MISMATCH;
    }
    return st;
}
static unsigned count_s2051(const s2051_session *s){(void)s;return 6;}
static void configure_s2051(s2051_session *s,const unsigned char *v) {
    s->config_count=6;
    for(unsigned i=0;i<6;i++)s->config[i]=(lt48_runtime_configuration_value){-1,0,i,v?v[i]:UINT32_MAX};
}
static lt48_status settings_read_s2051(s2051_session *s,unsigned char out[6]) {
    s->mode=13;address_s2051(s,0);lt48_status st=run_s2051(s,12);configure_s2051(s,NULL);
    if(st==LT48_OK)st=run_s2051(s,6);
    for(unsigned i=0;st==LT48_OK && i<count_s2051(s);i++) {
        if(s->config[i].value>1u)st=LT48_DEVICE_MISMATCH;
        else out[i]=(unsigned char)s->config[i].value;
    }
    if(st==LT48_OK)st=run_s2051(s,13);
    return st;
}
static lt48_status settings_write_s2051(s2051_session *s,const unsigned char *desired,int extra) {
    /* Parallel hardware has one combined fuse/lock byte. Commit fuse changes
     * with existing locks, then commit locks only after independent readback. */
    unsigned char target[6],actual[6];lt48_status st=settings_read_s2051(s,target);
    if(st!=LT48_OK)return st;
    for(unsigned i=extra?2u:0;i<(extra?6u:2u);i++)target[i]=desired[i];
    s->mode=13;address_s2051(s,0);st=run_s2051(s,12);configure_s2051(s,target);
    if(st==LT48_OK)st=run_s2051(s,11);
    if(st==LT48_OK)st=run_s2051(s,13);
    if(st==LT48_OK)st=settings_read_s2051(s,actual);
    if(st==LT48_OK && memcmp(actual,target,6))st=LT48_DEVICE_MISMATCH;
    return st;
}
static lt48_status restore_s2051(s2051_session *s) {
    lt48_status st=settings_write_s2051(s,s->saved,1);
    if(st==LT48_OK)s->restore=0;
    return st;
}
static lt48_status read_s2051(s2051_session *s,unsigned start,size_t n,unsigned char *out,unsigned vcc) {
    /* Source setup starts at wire address zero. Read and discard prefixes
     * for partial ranges; preserve the two independent image regions. */
    s->vars[0].value=vcc;s->mode=1;size_t done=0;lt48_status st=LT48_OK;unsigned char block[4096];
    while(st==LT48_OK && done<n) {
        unsigned a=start+(unsigned)done;uint32_t wire,remaining;
        st=lt48_s2051_address(s->r,a,&wire,&remaining);
        if(st!=LT48_OK)break;
        unsigned base=a-wire,boundary=a+remaining;
        size_t take=n-done;if(take>boundary-a)take=boundary-a;
        address_s2051(s,base);st=run_s2051(s,12);
        if(st==LT48_OK)st=run_s2051(s,5);
        size_t offset=0,total=a-base+take;
        while(st==LT48_OK && offset<total) {
            size_t chunk=total-offset;if(chunk>sizeof block)chunk=sizeof block;
            st=block_s2051(s,0,block,chunk);
            size_t skip=a-base>offset?a-base-offset:0;if(skip>chunk)skip=chunk;
            if(st==LT48_OK && skip<chunk)memcpy(out+done+offset+skip-(a-base),block+skip,chunk-skip);
            offset+=chunk;
        }
        if(st==LT48_OK)st=stop_s2051(s);
        if(st==LT48_OK)st=run_s2051(s,13);
        done+=take;
    }
    s->vars[0].value=500;return st;
}
static lt48_status write_s2051_region(s2051_session *s,unsigned base,unsigned end,const unsigned char *image) {
    s->mode=2;address_s2051(s,base);lt48_status st=run_s2051(s,12);
    if(st==LT48_OK)st=run_s2051(s,8);
    for(unsigned a=base;st==LT48_OK && a<end;) {
        size_t n=end-a;if(n>4096)n=4096;
        st=block_s2051(s,1,(unsigned char *)image+a,n);a+=(unsigned)n;
    }
    if(st==LT48_OK)st=stop_s2051(s);
    if(st==LT48_OK)st=run_s2051(s,13);
    return st;
}
static lt48_status write_s2051(s2051_session *s,const unsigned char *image) {
    unsigned bases[]={0,s->r->flash},ends[]={s->r->flash,s->r->flash+32};
    lt48_status st=LT48_OK;
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=write_s2051_region(s,bases[i],ends[i],image);
    return st;
}
static lt48_status mismatch_s2051(lt48_serial_page_result *out,unsigned base,const unsigned char *a,const unsigned char *b,size_t n,int blank) {
    for(size_t i=0;i<n;i++)if(a[i]!=(blank?255:b[i])) {
        if(!out->mismatch_count)out->first_mismatch=base+(unsigned)i;
        out->mismatch_count++;
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status erase_once_s2051(s2051_session *s) {
    s->mode=7;address_s2051(s,0);lt48_status st=run_s2051(s,12);
    if(st==LT48_OK)st=run_s2051(s,10);
    if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=run_s2051(s,13);
    return st;
}
static lt48_status erase_s2051(s2051_session *s,unsigned char *saved,unsigned char *actual,lt48_serial_page_result *out) {
    /* Erase first to remove locks; only then can the user-row fuse be enabled.
     * A second erase covers the row even when its original fuse was disabled. */
    saved[0]=saved[1]=0;memcpy(s->saved,saved,6);s->restore=1;
    lt48_status st=erase_once_s2051(s);unsigned char enabled[6];memcpy(enabled,saved,6);enabled[4]=1;
    if(st==LT48_OK)st=settings_write_s2051(s,enabled,1);
    if(st==LT48_OK)st=erase_once_s2051(s);
    if(st==LT48_OK)st=restore_s2051(s);
    if(st==LT48_OK)st=read_s2051(s,0,s->r->flash+32,actual,500);
    if(st==LT48_OK)st=mismatch_s2051(out,0,actual,NULL,s->r->flash+32,1);
    return st;
}
static lt48_status cleanup_s2051(void *v,unsigned phase) {
    s2051_session *s=v;
    if(phase==0)return stop_s2051(s);
    if(phase==1)return s->ready?run_s2051(s,13):LT48_OK;
    if(phase==2)return run_s2051(s,1);
    if(phase==4)return s->restore?restore_s2051(s):LT48_OK;
    return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
}
static lt48_status setup_s2051(s2051_session *s) {
    lt48_status st=LT48_OK;
    for(unsigned i=0;st==LT48_OK && i<4;i++) {
        const lt48_wave_code *a=&s->r->applet[i];
        st=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
        if(st==LT48_OK)st=lt48_owner_load_applet_internal(s->wire,a->load_address,a->data,a->data_size);
    }
    if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s->wire,s->r->fpga,2778);
    if(st==LT48_OK)st=run_s2051(s,0);
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);
    const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof busy);
    return st;
}
lt48_status lt48_s2051_execute(const lt48_s2051_resources *r,const lt48_protocol_transport *api,void *ctx,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void *decision_ctx,lt48_serial_page_result *out) {
    if(!out || !api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_s2051_validate(r,op,start,length,image,size);
    if(st==LT48_OK && (type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
    unsigned fields=r?6u:0;
    if(st==LT48_OK && ((op==LT48_OPERATION_READ && (!read || read_size<length)) || (op==LT48_OPERATION_CONFIGURATION && (!read || read_size<fields))))st=LT48_BUFFER_TOO_SMALL;
    if(st!=LT48_OK){out->operation_status=st;return st;}
    if(api->cancelled && api->cancelled(ctx)){out->operation_status=LT48_CANCELLED;out->cleanup_completed=1;return LT48_CANCELLED;}
    unsigned char *actual=malloc((r->flash+32u)),saved[6],desired[6];
    if(!actual){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
    s2051_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=13;memcpy(s.vars,r->vars,sizeof s.vars);
    lt48_protocol_transport clean=*api;clean.cancelled=NULL;int setup_uncertain=0,setup_started=0;
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
    if(st==LT48_OK) {
        setup_started=1;s.api=&clean;st=setup_s2051(&s);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
        setup_uncertain=st!=LT48_OK;s.ready=st==LT48_OK;
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;s.api=api;
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
    for(unsigned attempt=0;st==LT48_OK && attempt<64;attempt++) {
        st=run_s2051(&s,4);if(st!=LT48_OK)break;
        unsigned manufacturer=s.buffers[7][0],device=s.buffers[5][0];
        if(manufacturer==r->manufacturer && device==r->device)break;
        lt48_m37_id_choice choice=decide?decide(decision_ctx,r->manufacturer,r->device,manufacturer,device):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)break;
        if(choice!=LT48_M37_ID_RETRY || attempt==63)st=LT48_DEVICE_MISMATCH;
    }
    if(st==LT48_OK)st=settings_read_s2051(&s,saved);
    int programming=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if(st==LT48_OK && (op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE)) {
        memcpy(desired,saved,fields);
        if(op==LT48_OPERATION_SECURE){unsigned level=image?image[0]:2;desired[0]=1;if(level==2)desired[1]=1;}
        else if(image)for(unsigned i=0;i<6;i++)if(image[0]&(1u<<i))desired[i]=image[i+1];
        int fuses=memcmp(desired+2,saved+2,4)!=0,locks=memcmp(desired,saved,2)!=0;
        if(desired[0]<saved[0] || desired[1]<saved[1] || (fuses && (saved[0] || saved[1])))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && fuses)st=settings_write_s2051(&s,desired,1);
        if(st==LT48_OK && locks)st=settings_write_s2051(&s,desired,0);
        if(st==LT48_OK)memcpy(saved,desired,fields);
    } else if(st==LT48_OK) {
        if(op==LT48_OPERATION_ERASE || op==LT48_OPERATION_PROGRAM)st=erase_s2051(&s,saved,actual,out);
        else if(saved[1] || (programming && saved[0]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && programming) {
            if(op==LT48_OPERATION_OVER_PROGRAM) {
                st=read_s2051(&s,0,r->flash+32,actual,500);
                for(unsigned i=0;st==LT48_OK && i<r->flash+32;i++)if((actual[i]&image[i])!=image[i]){out->first_mismatch=i;out->mismatch_count=1;st=LT48_DEVICE_MISMATCH;}
            }
            if(st==LT48_OK) {
                memcpy(s.saved,saved,6);s.restore=1;memcpy(desired,saved,6);
                desired[4]=1; /* User-row writes require its enable fuse. */
                st=settings_write_s2051(&s,desired,1);
            }
            if(st==LT48_OK)st=write_s2051(&s,image);
            unsigned delta=type?50:25,volts[]={500-delta,500+delta};
            for(unsigned i=0;st==LT48_OK && i<2;i++) {
                st=read_s2051(&s,0,(r->flash+32u),actual,volts[i]);
                if(st==LT48_OK)st=mismatch_s2051(out,0,actual,image,(r->flash+32u),0);
            }
        } else if(st==LT48_OK && op!=LT48_OPERATION_ERASE) {
            unsigned delta=type?50:25,volts[]={500-delta,500+delta};
            unsigned passes=op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_ALL?2:1;
            if(op!=LT48_OPERATION_VERIFY || margin==LT48_VERIFY_MARGIN_NOMINAL)volts[0]=500;
            else if(margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
            for(unsigned i=0;st==LT48_OK && i<passes;i++) {
                st=read_s2051(&s,start,length,actual,volts[i]);
                if(st==LT48_OK && op!=LT48_OPERATION_READ)st=mismatch_s2051(out,start,actual,image,length,op==LT48_OPERATION_BLANK_CHECK);
            }
        }
    }
    if(st==LT48_OK && s.restore)st=restore_s2051(&s);
    if(st==LT48_OK)st=settings_read_s2051(&s,desired);
    if(st==LT48_OK && memcmp(saved,desired,fields))st=LT48_DEVICE_MISMATCH;
    out->operation_status=st;
    if(!setup_started){lt48_runtime_transport_session_close(s.wire);out->cleanup_completed=1;free(actual);return st;}
    s.api=&clean;const unsigned phases[]={0,1,4,2,3};
    st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_s2051,&s,phases,5,st,&out->cleanup_status,&out->cleanup_completed);
    if(setup_uncertain){out->cleanup_status=LT48_CLEANUP_FAILED;out->cleanup_completed=0;}
    if(st==LT48_OK) {
        if(op==LT48_OPERATION_READ)memcpy(read,actual,length);
        if(op==LT48_OPERATION_CONFIGURATION)memcpy(read,desired,fields);
        out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?fields:length;
    }
    free(actual);return st;
}
