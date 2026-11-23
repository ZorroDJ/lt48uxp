#include "at89_ed2.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
/* Exact ED2/ID2/CC03 operation owner; source-audited, hardware qualification pending. */
lt48_status lt48_ed2_validate(const lt48_ed2_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r || !length || start>=r->layout.capacity || length>r->layout.capacity-start)return LT48_SAFETY_REJECTED;
    if((op<1 || op>8) && op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)
        if(start || length!=r->layout.capacity)return LT48_SAFETY_REJECTED;
    unsigned fields=r->layout.flags+1+r->layout.numbers;
    if(op==LT48_OPERATION_CONFIGURATION) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=fields+1 || image[0]>>fields)return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<fields;i++)if((image[0]&(1u<<i)) && image[i+1]>(i<r->layout.flags?1:i==r->layout.flags?3:255))return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_SECURE) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=1 || !image[0] || image[0]>3)return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE) {
        if(!image || size!=length)return LT48_INVALID_ARGUMENT;
    }
    return LT48_OK;
}
typedef struct ed2_session {
    const lt48_ed2_resources *r;const lt48_protocol_transport *api;void *ctx;
    lt48_runtime_transport_session *wire;lt48_compile_variable vars[6];
    unsigned char buffers[32][32];lt48_runtime_configuration_value config[8];size_t config_count;
    uint32_t volts[4];unsigned mode,restore;unsigned char saved[8];
} ed2_session;
static void address_ed2(ed2_session *s,unsigned image) {
    uint32_t physical=0,remaining=0;lt48_ed2_address(&s->r->layout,image,&physical,&remaining);
    for(unsigned i=0;i<4;i++){s->buffers[0][i]=(unsigned char)(physical>>(8*i));s->buffers[1][i]=(unsigned char)(image>>(8*i));}
}
static lt48_status run_ed2(ed2_session *s,unsigned op) {
    lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[31];size_t n=0;
    for(unsigned i=0;i<32;i++)if(i!=8)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    o.mode=s->mode;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=8;o.parameter_buffer_id=3;
    o.initial_buffers=seeds;o.initial_buffer_count=n;o.configuration=s->config;o.configuration_count=s->config_count;
    memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);o.root_timeout_seconds=10;
    lt48_status st=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
    if(st==LT48_OK)st=lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&o,s->r->addresses,4,s->buffers,s->config,8,&s->config_count,s->volts);
    return st;
}
static lt48_status transfer_ed2(ed2_session *s,int input,unsigned ep,unsigned char *p,size_t n) {
    if(s->api->cancelled && s->api->cancelled(s->ctx))return LT48_CANCELLED;
    size_t done=0;lt48_status st=input?s->api->read(s->ctx,(uint8_t)ep,p,n,&done):s->api->write(s->ctx,(uint8_t)ep,p,n,&done);
    return st==LT48_OK && done!=n?LT48_PROTOCOL_ERROR:st;
}
static lt48_status command_ed2(ed2_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    lt48_status st=lt48_runtime_transport_session_flush(s->wire);
    if(st==LT48_OK)st=transfer_ed2(s,0,2,(unsigned char *)p,n);
    if(st==LT48_OK && reply)st=transfer_ed2(s,1,0x88,reply,512);
    return st;
}
static lt48_status stop_ed2(ed2_session *s) {
    const unsigned char stop[]={0,0};unsigned char reply[512]={0};
    lt48_status st=command_ed2(s,stop,sizeof stop,reply);
    return st==LT48_OK && !(reply[0]|reply[1])?LT48_PROTOCOL_ERROR:st;
}
static lt48_status block_ed2(ed2_session *s,int writing,unsigned char *data,size_t n) {
    unsigned char h[]={writing?0x48:0x43,(unsigned char)(n/512),(unsigned char)(n/512>>8),(unsigned char)(n%512),(unsigned char)(n%512>>8),writing?1:0},reply[512]={0};
    lt48_status st=command_ed2(s,h,sizeof h,NULL);
    if(st==LT48_OK)st=transfer_ed2(s,!writing,writing?4:0x88,data,n);
    if(st==LT48_OK && writing) {
        st=transfer_ed2(s,1,0x88,reply,512);if(st==LT48_OK && !(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;
        const unsigned char end[]={0x0c,1};if(st==LT48_OK)st=command_ed2(s,end,sizeof end,reply);
        if(st==LT48_OK && (!(reply[0]|reply[1]) || !(reply[2]&1)))st=LT48_DEVICE_MISMATCH;
    }
    return st;
}
static unsigned count_ed2(const ed2_session *s){return s->r->layout.flags+1+s->r->layout.numbers;}
static void configure_ed2(ed2_session *s,const unsigned char *v) {
    unsigned flags=s->r->layout.flags;s->config_count=count_ed2(s);
    for(unsigned i=0;i<s->config_count;i++)s->config[i]=(lt48_runtime_configuration_value){-1,i<flags?0:i==flags?1:2,i<flags?i:i==flags?0:i-flags-1,v?v[i]:UINT32_MAX};
}
static lt48_status settings_read_ed2(ed2_session *s,unsigned char out[8]) {
    s->mode=13;address_ed2(s,0);lt48_status st=run_ed2(s,2);configure_ed2(s,NULL);
    if(st==LT48_OK)st=run_ed2(s,6);
    unsigned flags=s->r->layout.flags;
    for(unsigned i=0;st==LT48_OK && i<count_ed2(s);i++) {
        if(s->config[i].value>(i<flags?1u:i==flags?3u:255u))st=LT48_DEVICE_MISMATCH;
        else out[i]=(unsigned char)s->config[i].value;
    }
    if(st==LT48_OK)st=run_ed2(s,3);
    return st;
}
static lt48_status settings_write_ed2(ed2_session *s,const unsigned char *desired,int extra) {
    s->mode=13;address_ed2(s,0);lt48_status st=run_ed2(s,2);configure_ed2(s,desired);
    if(st==LT48_OK)st=run_ed2(s,extra?13:14);
    if(st==LT48_OK)st=run_ed2(s,3);
    unsigned char actual[8];if(st==LT48_OK)st=settings_read_ed2(s,actual);
    if(st==LT48_OK)for(unsigned i=extra?s->r->layout.flags+1:0;i<(extra?count_ed2(s):s->r->layout.flags+1);i++)
        if(actual[i]!=desired[i])st=LT48_DEVICE_MISMATCH;
    return st;
}
static lt48_status restore_ed2(ed2_session *s) {
    lt48_status st=settings_write_ed2(s,s->saved,1);
    if(st==LT48_OK)st=settings_write_ed2(s,s->saved,0);
    if(st==LT48_OK)s->restore=0;
    return st;
}
static lt48_status read_ed2(ed2_session *s,unsigned start,size_t n,unsigned char *out,unsigned vcc) {
    /* Start each selected region at its source-defined base. In particular,
     * selector 1 overwrites the EEPROM high address latch during setup.
     * Discard the prefix for arbitrary ranges; never assume those bytes can
     * instead be addressed by flattening the host image into physical pins. */
    s->vars[0].value=vcc;s->mode=1;size_t done=0;lt48_status st=LT48_OK;unsigned char block[4096];
    while(st==LT48_OK && done<n) {
        unsigned a=start+(unsigned)done,base=a<65536?0:a<67584?65536:67584;
        unsigned boundary=base==0?65536:base==65536?67584:s->r->layout.capacity;
        size_t take=n-done;if(take>boundary-a)take=boundary-a;
        address_ed2(s,base);st=run_ed2(s,2);
        if(st==LT48_OK)st=run_ed2(s,5);
        size_t offset=0,total=a-base+take;
        while(st==LT48_OK && offset<total) {
            size_t chunk=total-offset;if(chunk>sizeof block)chunk=sizeof block;
            st=block_ed2(s,0,block,chunk);
            size_t skip=a-base>offset?a-base-offset:0;if(skip>chunk)skip=chunk;
            if(st==LT48_OK && skip<chunk)memcpy(out+done+offset+skip-(a-base),block+skip,chunk-skip);
            offset+=chunk;
        }
        if(st==LT48_OK)st=stop_ed2(s);
        if(st==LT48_OK)st=run_ed2(s,3);
        done+=take;
    }
    s->vars[0].value=500;return st;
}
static lt48_status write_ed2(ed2_session *s,const unsigned char *image) {
    s->mode=2;lt48_status st=LT48_OK;
    for(unsigned base=0;st==LT48_OK && base<s->r->layout.capacity;) {
        unsigned end=base==0?65536:base==65536?67584:s->r->layout.capacity;
        address_ed2(s,base);st=run_ed2(s,12);
        /* Explicit non-erasing entry; unexpected protection prevents writes. */
        if(st==LT48_OK && (s->buffers[15][0]&7)!=7)st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK)st=run_ed2(s,8);
        for(unsigned a=base;st==LT48_OK && a<end;) {
            size_t n=end-a;if(n>4096)n=4096;
            st=block_ed2(s,1,(unsigned char *)image+a,n);a+=(unsigned)n;
        }
        if(st==LT48_OK)st=stop_ed2(s);
        if(st==LT48_OK)st=run_ed2(s,3);
        base=end;
    }
    return st;
}
static lt48_status mismatch_ed2(lt48_serial_page_result *out,unsigned base,const unsigned char *a,const unsigned char *b,size_t n,int blank) {
    for(size_t i=0;i<n;i++)if(a[i]!=(blank?255:b[i])) {
        if(!out->mismatch_count)out->first_mismatch=base+(unsigned)i;
        out->mismatch_count++;
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status erase_ed2(ed2_session *s,unsigned char *saved,unsigned char *actual,lt48_serial_page_result *out) {
    s->mode=7;address_ed2(s,0);lt48_status st=run_ed2(s,2);
    saved[s->r->layout.flags]=0;memcpy(s->saved,saved,count_ed2(s));
    if(st==LT48_OK){s->restore=1;st=run_ed2(s,10);}
    if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=run_ed2(s,3);
    if(st==LT48_OK)st=restore_ed2(s);
    if(st==LT48_OK)st=read_ed2(s,0,s->r->layout.capacity,actual,500);
    if(st==LT48_OK)st=mismatch_ed2(out,0,actual,NULL,s->r->layout.capacity,1);
    return st;
}
static lt48_status cleanup_ed2(void *v,unsigned phase) {
    ed2_session *s=v;
    if(phase==0)return stop_ed2(s);
    if(phase==1)return run_ed2(s,3);
    if(phase==2)return run_ed2(s,1);
    if(phase==4)return s->restore?restore_ed2(s):LT48_OK;
    return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
}
static lt48_status setup_ed2(ed2_session *s) {
    lt48_status st=LT48_OK;
    for(unsigned i=0;st==LT48_OK && i<4;i++) {
        const lt48_wave_code *a=&s->r->applet[i];
        st=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
        if(st==LT48_OK)st=lt48_owner_load_applet_internal(s->wire,a->load_address,a->data,a->data_size);
    }
    if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s->wire,s->r->fpga,2778);
    if(st==LT48_OK)st=run_ed2(s,0);
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);
    const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof busy);
    return st;
}
lt48_status lt48_ed2_execute(const lt48_ed2_resources *r,const lt48_protocol_transport *api,void *ctx,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void *decision_ctx,lt48_serial_page_result *out) {
    if(!out || !api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_ed2_validate(r,op,start,length,image,size);
    if(st==LT48_OK && (type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
    unsigned fields=r?r->layout.flags+1+r->layout.numbers:0;
    if(st==LT48_OK && ((op==LT48_OPERATION_READ && (!read || read_size<length)) || (op==LT48_OPERATION_CONFIGURATION && (!read || read_size<fields))))st=LT48_BUFFER_TOO_SMALL;
    if(st!=LT48_OK){out->operation_status=st;return st;}
    unsigned char *actual=malloc(r->layout.capacity),saved[8],desired[8];
    if(!actual){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
    ed2_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=13;memcpy(s.vars,r->vars,sizeof s.vars);
    lt48_protocol_transport clean=*api;clean.cancelled=NULL;int setup_uncertain=0;
    if(api->cancelled && api->cancelled(ctx))st=LT48_CANCELLED;
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
    if(st==LT48_OK) {
        s.api=&clean;st=setup_ed2(&s);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
        setup_uncertain=st!=LT48_OK;
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;s.api=api;
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
    /* These catalogue rows carry no expected ID (0/0). Do not treat zero
     * as a real identity or let it gate manual device selection. */
    for(unsigned attempt=0;st==LT48_OK && (r->manufacturer || r->device) && attempt<64;attempt++) {
        address_ed2(&s,0);st=run_ed2(&s,4);if(st!=LT48_OK)break;
        if(s.buffers[5][0]==r->device)break;
        lt48_m37_id_choice choice=decide?decide(decision_ctx,0,r->device,0,s.buffers[5][0]):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)break;
        if(choice!=LT48_M37_ID_RETRY || attempt==63)st=LT48_DEVICE_MISMATCH;
    }
    if(st==LT48_OK)st=settings_read_ed2(&s,saved);
    unsigned lock=r->layout.flags;
    int programming=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if(st==LT48_OK && (op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE)) {
        memcpy(desired,saved,fields);
        if(op==LT48_OPERATION_SECURE){unsigned level=image?image[0]:2;if(level>desired[lock])desired[lock]=(unsigned char)level;}
        else if(image)for(unsigned i=0;i<fields;i++)if(image[0]&(1u<<i))desired[i]=image[i+1];
        int extra=memcmp(desired+lock+1,saved+lock+1,r->layout.numbers)!=0;
        int flags=memcmp(desired,saved,lock)!=0;
        if(desired[lock]<saved[lock] || ((extra || flags) && saved[lock]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && extra)st=settings_write_ed2(&s,desired,1);
        if(st==LT48_OK && (flags || desired[lock]!=saved[lock]))st=settings_write_ed2(&s,desired,0);
        if(st==LT48_OK)memcpy(saved,desired,fields);
    } else if(st==LT48_OK) {
        if(op==LT48_OPERATION_ERASE || op==LT48_OPERATION_PROGRAM)st=erase_ed2(&s,saved,actual,out);
        else if(saved[lock]>=2 || (programming && saved[lock]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && programming) {
            if(op==LT48_OPERATION_OVER_PROGRAM) {
                st=read_ed2(&s,0,r->layout.capacity,actual,500);
                for(unsigned i=0;st==LT48_OK && i<r->layout.capacity;i++)if((actual[i]&image[i])!=image[i]) {out->first_mismatch=i;out->mismatch_count=1;st=LT48_DEVICE_MISMATCH;}
            }
            if(st==LT48_OK)st=write_ed2(&s,image);
            unsigned delta=type?50:25,volts[]={500-delta,500+delta};
            for(unsigned i=0;st==LT48_OK && i<2;i++) {
                st=read_ed2(&s,0,r->layout.capacity,actual,volts[i]);
                if(st==LT48_OK)st=mismatch_ed2(out,0,actual,image,r->layout.capacity,0);
            }
        } else if(st==LT48_OK && op!=LT48_OPERATION_ERASE) {
            unsigned delta=type?50:25,volts[]={500-delta,500+delta};
            unsigned passes=op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_ALL?2:1;
            if(op!=LT48_OPERATION_VERIFY || margin==LT48_VERIFY_MARGIN_NOMINAL)volts[0]=500;
            else if(margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
            for(unsigned i=0;st==LT48_OK && i<passes;i++) {
                st=read_ed2(&s,start,length,actual,volts[i]);
                if(st==LT48_OK && op!=LT48_OPERATION_READ)st=mismatch_ed2(out,start,actual,image,length,op==LT48_OPERATION_BLANK_CHECK);
            }
        }
    }
    if(st==LT48_OK)st=settings_read_ed2(&s,desired);
    if(st==LT48_OK && memcmp(saved,desired,fields))st=LT48_DEVICE_MISMATCH;
    out->operation_status=st;s.api=&clean;const unsigned phases[]={0,1,4,2,3};
    st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_ed2,&s,phases,5,st,&out->cleanup_status,&out->cleanup_completed);
    if(setup_uncertain){out->cleanup_status=LT48_CLEANUP_FAILED;out->cleanup_completed=0;}
    if(st==LT48_OK) {
        if(op==LT48_OPERATION_READ)memcpy(read,actual,length);
        if(op==LT48_OPERATION_CONFIGURATION)memcpy(read,desired,fields);
        out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?fields:length;
    }
    free(actual);return st;
}
