#include "at89_s8253.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
/* Exact S8253 operation owner; source-audited, hardware qualification pending. */
lt48_status lt48_s8253_validate(const lt48_s8253_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(r && r->kind==51)return lt48_r8c8_validate(r,op,start,length,image,size);
    if(r && r->kind==19)return lt48_t89six_validate(r,op,start,length,image,size);
    if(r && r->kind==18)return lt48_at91nine_validate(r,op,start,length,image,size);
    if(r && r->kind==17)return lt48_ramtron6_validate(r,op,start,length,image,size);
    if(r && r->kind==23)return lt48_sy80_validate(r,op,start,length,image,size);
    if(r && r->kind==24)return lt48_sy76_validate(r,op,start,length,image,size);
    if(r && r->kind==25)return lt48_mw36_validate(r,op,start,length,image,size);
    if(r && r->kind==28)return lt48_sx30_validate(r,op,start,length,image,size);
    if(r && r->kind==29)return lt48_mg15_validate(r,op,start,length,image,size);
    if(r && r->kind==30)return lt48_sn9_validate(r,op,start,length,image,size);
    if(r && r->kind==33)return lt48_ob16_validate(r,op,start,length,image,size);
    if(r && r->kind==34)return lt48_wt10_validate(r,op,start,length,image,size);
    if(r && r->kind==36)return lt48_wt8_validate(r,op,start,length,image,size);
    if(r && r->kind==39)return lt48_hy14_validate(r,op,start,length,image,size);
    if(r && r->kind==41)return lt48_mw6_validate(r,op,start,length,image,size);
    if(r && r->kind==42)return lt48_mw8_validate(r,op,start,length,image,size);
    if(r && r->kind==43)return lt48_my7_validate(r,op,start,length,image,size);
    if(r && r->kind==44)return lt48_my4_validate(r,op,start,length,image,size);
    if(r && r->kind==47)return lt48_sc2_validate(r,op,start,length,image,size);
    if(r && r->kind==49)return lt48_sc4g_validate(r,op,start,length,image,size);
    if(r && r->kind==48)return lt48_sc4a_validate(r,op,start,length,image,size);
    if(r && r->kind==45)return lt48_tp4_validate(r,op,start,length,image,size);
    if(r && r->kind==46)return lt48_tp6_validate(r,op,start,length,image,size);
    if(r && r->kind==40)return lt48_gc8_validate(r,op,start,length,image,size);
    if(r && r->kind==37)return lt48_ab11_validate(r,op,start,length,image,size);
    if(r && r->kind==38)return lt48_su16_validate(r,op,start,length,image,size);
    if(r && r->kind==50)return lt48_ite2_validate(r,op,start,length,image,size);
    if(r && r->kind==35)return lt48_so30_validate(r,op,start,length,image,size);
    if(r && r->kind==32)return lt48_sm3_validate(r,op,start,length,image,size);
    if(r && r->kind==31)return lt48_sm6_validate(r,op,start,length,image,size);
    if(r && (r->kind==26||r->kind==27))return lt48_sm30_validate(r,op,start,length,image,size);
    if(r && r->kind==22)return lt48_mw27_validate(r,op,start,length,image,size);
    if(r && (r->kind==20||r->kind==21))return lt48_mf74_validate(r,op,start,length,image,size);
    if(r && r->kind==16)return lt48_at28c64b1_validate(r,op,start,length,image,size);
    if(r && r->kind==15)return lt48_stm8s4_validate(r,op,start,length,image,size);
    if(r && r->kind==14)return lt48_cc254x5_validate(r,op,start,length,image,size);
    if(r && r->kind==13)return lt48_novatek25_validate(r,op,start,length,image,size);
    if(r && r->kind==12)return lt48_xmega7_validate(r,op,start,length,image,size);
    if(r && r->kind==11)return lt48_psoc29_validate(r,op,start,length,image,size);
    if(r && r->kind==10)return lt48_sstmx16_validate(r,op,start,length,image,size);
    if(r && r->kind==9)return lt48_n79w79_21_validate(r,op,start,length,image,size);
    if(r && r->kind==8)return lt48_nuvoton30_validate(r,op,start,length,image,size);
    if(r && r->kind==7)return lt48_msp43076_validate(r,op,start,length,image,size);
    if(r && r->kind==6)return lt48_numicro81_validate(r,op,start,length,image,size);
    if(r && r->kind==5)return lt48_n79w79_validate(r,op,start,length,image,size);
    if(r && r->kind==4)return lt48_w78bank147_validate(r,op,start,length,image,size);
    if(r && r->kind==3)return lt48_w78d24_validate(r,op,start,length,image,size);
    if(r && r->kind==2)return lt48_sst_validate(r,op,start,length,image,size);
    if(r && r->kind)return lt48_p89v_validate(r,op,start,length,image,size);
    if(!r || !length || start>=(14400u) || length>(14400u)-start)return LT48_SAFETY_REJECTED;
    if((op<1 || op>8) && op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)
        if(start || length!=(14400u))return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_CONFIGURATION) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=6 || (image[0]&0xe0))return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<5;i++)if((image[0]&(1u<<i)) && image[i+1]>(i<4?1:3))return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_SECURE) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=1 || !image[0] || image[0]>3)return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE) {
        if(!image || size!=length)return LT48_INVALID_ARGUMENT;
    }
    return LT48_OK;
}
typedef struct s8253_session {
    const lt48_s8253_resources *r;const lt48_protocol_transport *api;void *ctx;
    lt48_runtime_transport_session *wire;lt48_compile_variable vars[8];
    unsigned char buffers[32][32];lt48_runtime_configuration_value config[32];size_t config_count;
    uint32_t volts[4];unsigned mode,restore,ready;unsigned char saved[5];uint32_t mf74_saved[16];uint32_t sy80_saved[32];unsigned sy80_restore;unsigned mf74_restore;const unsigned char*device_image;size_t device_image_size;unsigned char*device_image_update;
} s8253_session;
static void address_s8253(s8253_session *s,unsigned image) {
    uint32_t physical=0,remaining=0;lt48_s8253_address(s->r,image,&physical,&remaining);
    for(unsigned i=0;i<4;i++){s->buffers[0][i]=(unsigned char)(physical>>(8*i));s->buffers[1][i]=(unsigned char)(image>>(8*i));}
}
static lt48_status run_s8253(s8253_session *s,unsigned op) {
    lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[31];size_t n=0;
    int image_result=(s->r->kind==51&&op==11)||s->r->kind==38||(s->r->kind==35&&(op==2||(s->r->definition==9&&(op==6||op==11))))||s->r->kind==14||(s->r->kind==19&&(op==8||op==11));
    if(s->r->legacy_w79e201)for(unsigned i=0;i<4;i++)s->buffers[2][i]=(unsigned char)(s->r->flash>>(8*i));
    unsigned mode_buffer=s->r->n76one?7:s->r->kind==19?4:((s->r->kind==38||s->r->kind==46||s->r->kind==45||s->r->kind==48||s->r->kind==49||s->r->kind==47||s->r->kind==44||s->r->kind==43||s->r->kind==42||s->r->kind==41||s->r->kind==39||s->r->kind==40||(s->r->kind==50||s->r->kind==51)||s->r->kind==35||s->r->kind==37||s->r->kind==36||s->r->kind==34||s->r->kind==33||s->r->kind==32||s->r->kind==31||s->r->kind==30||s->r->kind==29||s->r->kind==28||s->r->kind==26||s->r->kind==25||s->r->kind==22||s->r->kind==20||s->r->kind==5||s->r->kind==16||s->r->kind==17||s->r->kind==18)&&s->r->n79_xp)||((s->r->kind==10&&s->r->n79_xp)||s->r->kind==11)?7:8;
    for(unsigned i=0;i<32;i++)if(i!=mode_buffer)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    o.host_staged_shutdown=s->r->kind==13&&!s->r->novatek25_main_only?1:s->r->kind==18?4:s->r->kind==16?1:(s->r->kind==7||s->r->kind==12)?4:(s->r->kind==38||s->r->kind==46||s->r->kind==45||s->r->kind==48||s->r->kind==49||s->r->kind==47||s->r->kind==44||s->r->kind==43||s->r->kind==42||s->r->kind==41||s->r->kind==39||s->r->kind==40||(s->r->kind==50||s->r->kind==51)||s->r->kind==35||s->r->kind==37||s->r->kind==36||s->r->kind==34||s->r->kind==33||s->r->kind==32||s->r->kind==31||s->r->kind==30||s->r->kind==29||s->r->kind==28||s->r->kind==26||s->r->kind==25||s->r->kind==22||s->r->kind==20||s->r->kind==5||s->r->kind==9||s->r->kind==10||s->r->kind==11||s->r->kind==17)?3:0;o.mode=s->mode;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=mode_buffer;o.parameter_buffer_id=s->r->kind==19?1:((s->r->kind==38||s->r->kind==46||s->r->kind==45||s->r->kind==48||s->r->kind==49||s->r->kind==47||s->r->kind==44||s->r->kind==43||s->r->kind==42||s->r->kind==41||s->r->kind==39||s->r->kind==40||(s->r->kind==50||s->r->kind==51)||s->r->kind==35||s->r->kind==37||s->r->kind==36||s->r->kind==34||s->r->kind==33||s->r->kind==32||s->r->kind==31||s->r->kind==30||s->r->kind==29||s->r->kind==28||s->r->kind==26||s->r->kind==25||s->r->kind==22||s->r->kind==20||s->r->kind==5||s->r->kind==16||s->r->kind==17||s->r->kind==18)&&s->r->n79_xp)||((s->r->kind==10&&s->r->n79_xp)||s->r->kind==11)?2:3;
    /* Reuse the original float-to-centivolt conversion for this XP route.
     * The other flag effect (staged shutdown) is already enabled above. */
    if(s->r->kind==22||s->r->kind==25||s->r->kind==26||s->r->kind==28||s->r->kind==29||s->r->kind==30||s->r->kind==46||s->r->kind==45||s->r->kind==48||s->r->kind==49||s->r->kind==47||s->r->kind==44||s->r->kind==43||s->r->kind==42||s->r->kind==41||s->r->kind==39||s->r->kind==40||(s->r->kind==50||s->r->kind==51)||s->r->kind==35||s->r->kind==37||s->r->kind==36||s->r->kind==34||s->r->kind==33||s->r->kind==32||s->r->kind==31)o.host_avr_parallel=1;
    if(s->r->n76one){o.mode_buffer=7;o.parameter_buffer_id=2;}
    o.initial_buffers=seeds;o.initial_buffer_count=n;o.configuration=s->config;o.configuration_count=s->config_count;
    lt48_runtime_nested_wave nested[17];size_t nn=0;if(s->r->kind>=5){for(unsigned i=0;i<(s->r->n76one?17u:((s->r->kind==15||s->r->kind==19||s->r->kind==38||s->r->kind==46||s->r->kind==45||s->r->kind==48||s->r->kind==49||s->r->kind==47||s->r->kind==44||s->r->kind==43||s->r->kind==42||s->r->kind==41||s->r->kind==39||s->r->kind==40||(s->r->kind==50||s->r->kind==51)||s->r->kind==35||s->r->kind==37||s->r->kind==36||s->r->kind==34||s->r->kind==33||s->r->kind==32||s->r->kind==31||s->r->kind==30||s->r->kind==29||s->r->kind==28||s->r->kind==26||s->r->kind==25||s->r->kind==21||s->r->kind==22)?17u:12u));i++)if(s->r->wave[i])nested[nn++]=(lt48_runtime_nested_wave){i,s->r->wave[i]};o.nested_waves=nested;o.nested_wave_count=nn;}
    if(s->r->kind==11||image_result){o.device_image=s->device_image;o.device_image_size=s->device_image_size;o.device_element_bytes=s->r->kind==35?2:1;}
    memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);o.root_timeout_seconds=s->r->kind==51?60:10;
    lt48_status st=lt48_runtime_transport_session_wave_watchdog(s->wire,s->r->kind==51?60000:10000,1);
    if(st==LT48_OK&&!image_result)st=lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,s->r->kind>=4?s->r->variable_count:4,&o,s->r->addresses,(s->r->white3_page||s->r->nuvoton8||s->r->kind==10||s->r->kind==11||s->r->kind==12||s->r->kind==13||s->r->kind==14||s->r->kind==15||s->r->kind==18||s->r->kind==23||s->r->kind==24||s->r->kind==27||s->r->kind==38||s->r->kind==46||s->r->kind==45||s->r->kind==48||s->r->kind==49||s->r->kind==47||s->r->kind==44||s->r->kind==43||s->r->kind==42||s->r->kind==41||s->r->kind==39||s->r->kind==40||(s->r->kind==50||s->r->kind==51)||s->r->kind==35||s->r->kind==37||s->r->kind==36||s->r->kind==34||s->r->kind==33||s->r->kind==32||s->r->kind==31||s->r->kind==30||s->r->kind==29||s->r->kind==28||s->r->kind==26||s->r->kind==25||s->r->kind==22||s->r->kind==20||s->r->kind==21)?s->r->sstmx_applets:s->r->kind==7?7:4,s->buffers,s->config,(s->r->kind==15||s->r->kind==23||s->r->kind==24||s->r->kind==27||s->r->kind==25||s->r->kind==26||s->r->kind==28||s->r->kind==29||s->r->kind==30||s->r->kind==38||s->r->kind==46||s->r->kind==45||s->r->kind==48||s->r->kind==49||s->r->kind==47||s->r->kind==44||s->r->kind==43||s->r->kind==42||s->r->kind==41||s->r->kind==39||s->r->kind==40||(s->r->kind==50||s->r->kind==51)||s->r->kind==35||s->r->kind==37||s->r->kind==36||s->r->kind==34||s->r->kind==33||s->r->kind==32||s->r->kind==31)?32:s->r->kind>=5?16:8,&s->config_count,s->volts);
    if(st==LT48_OK&&image_result){lt48_runtime_result*result=NULL;st=lt48_owner_run_result_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,s->r->variable_count,&o,s->r->addresses,s->r->sstmx_applets,s->buffers,s->config,16,&s->config_count,s->volts,&result);if(!st&&s->device_image_update){const unsigned char*p;size_t n;st=lt48_runtime_get_image(result,&p,&n);if(!st&&n!=s->device_image_size)st=LT48_RUNTIME_ERROR;if(!st)memcpy(s->device_image_update,p,n);}lt48_runtime_result_close(result);}
    return st;
}
static lt48_status transfer_s8253(s8253_session *s,int input,unsigned ep,unsigned char *p,size_t n) {
    if(s->api->cancelled && s->api->cancelled(s->ctx))return LT48_CANCELLED;
    size_t done=0;lt48_status st=input?s->api->read(s->ctx,(uint8_t)ep,p,n,&done):s->api->write(s->ctx,(uint8_t)ep,p,n,&done);
    return st==LT48_OK && done!=n?LT48_PROTOCOL_ERROR:st;
}
static lt48_status command_s8253(s8253_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    lt48_status st=lt48_runtime_transport_session_flush(s->wire);
    if(st==LT48_OK)st=transfer_s8253(s,0,2,(unsigned char *)p,n);
    if(st==LT48_OK && reply)st=transfer_s8253(s,1,0x88,reply,512);
    return st;
}
static lt48_status stop_s8253(s8253_session *s) {
    const unsigned char stop[]={0,0};unsigned char reply[512]={0};
    lt48_status st=command_s8253(s,stop,sizeof stop,reply);
    return st==LT48_OK && !(reply[0]|reply[1])?LT48_PROTOCOL_ERROR:st;
}
static lt48_status block_s8253(s8253_session *s,int writing,unsigned char *data,size_t n) {
    unsigned char h[]={writing?0x48:0x43,(unsigned char)(n/512),(unsigned char)(n/512>>8),(unsigned char)(n%512),(unsigned char)(n%512>>8),writing?1:0},reply[512]={0};
    lt48_status st=command_s8253(s,h,sizeof h,NULL);
    if(st==LT48_OK)st=transfer_s8253(s,!writing,writing?4:0x88,data,n);
    if(st==LT48_OK && writing) {
        st=transfer_s8253(s,1,0x88,reply,512);if(st==LT48_OK && !(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;
        const unsigned char end[]={0x0c,1};if(st==LT48_OK)st=command_s8253(s,end,sizeof end,reply);
        if(st==LT48_OK && (!(reply[0]|reply[1]) || !(reply[2]&1)))st=LT48_DEVICE_MISMATCH;
    }
    return st;
}
static void configure_s8253(s8253_session *s,const unsigned char *v) {
    s->config_count=5;
    for(unsigned i=0;i<5;i++)s->config[i]=(lt48_runtime_configuration_value){-1,i<4?0:1,i<4?i:0,v?v[i]:UINT32_MAX};
}
static lt48_status settings_read_s8253(s8253_session *s,unsigned char out[5]) {
    s->mode=13;address_s8253(s,0);lt48_status st=run_s8253(s,12);configure_s8253(s,NULL);
    if(st==LT48_OK)st=run_s8253(s,6);
    for(unsigned i=0;st==LT48_OK && i<5;i++) {
        if(s->config[i].value>(i<4?1u:3u))st=LT48_DEVICE_MISMATCH;
        else out[i]=(unsigned char)s->config[i].value;
    }
    if(st==LT48_OK)st=run_s8253(s,13);
    return st;
}
static lt48_status settings_write_s8253(s8253_session *s,const unsigned char *desired,int fuses) {
    s->mode=13;address_s8253(s,0);lt48_status st=run_s8253(s,12);configure_s8253(s,desired);
    if(st==LT48_OK)st=run_s8253(s,fuses?14:15);
    if(st==LT48_OK)st=run_s8253(s,13);
    unsigned char actual[5];if(st==LT48_OK)st=settings_read_s8253(s,actual);
    if(st==LT48_OK)for(unsigned i=fuses?0u:4;i<(fuses?4u:5u);i++)
        if(actual[i]!=desired[i])st=LT48_DEVICE_MISMATCH;
    return st;
}
static lt48_status restore_s8253(s8253_session *s) {
    lt48_status st=settings_write_s8253(s,s->saved,1);
    if(st==LT48_OK)s->restore=0;
    return st;
}
static lt48_status read_s8253(s8253_session *s,unsigned start,size_t n,unsigned char *out,unsigned vcc) {
    /* Source setup starts at wire address zero. Read and discard prefixes
     * for partial ranges; preserve the three independent image regions. */
    s->vars[0].value=vcc;s->mode=1;size_t done=0;lt48_status st=LT48_OK;unsigned char block[4096];
    while(st==LT48_OK && done<n) {
        unsigned a=start+(unsigned)done;uint32_t wire,remaining;
        st=lt48_s8253_address(s->r,a,&wire,&remaining);
        if(st!=LT48_OK)break;
        unsigned base=a-wire,boundary=a+remaining;
        size_t take=n-done;if(take>boundary-a)take=boundary-a;
        address_s8253(s,base);st=run_s8253(s,12);
        if(st==LT48_OK)st=run_s8253(s,5);
        size_t offset=0,total=a-base+take;
        while(st==LT48_OK && offset<total) {
            size_t chunk=total-offset;if(chunk>sizeof block)chunk=sizeof block;
            st=block_s8253(s,0,block,chunk);
            size_t skip=a-base>offset?a-base-offset:0;if(skip>chunk)skip=chunk;
            if(st==LT48_OK && skip<chunk)memcpy(out+done+offset+skip-(a-base),block+skip,chunk-skip);
            offset+=chunk;
        }
        if(st==LT48_OK)st=stop_s8253(s);
        if(st==LT48_OK)st=run_s8253(s,13);
        done+=take;
    }
    s->vars[0].value=500;return st;
}
static lt48_status write_s8253_region(s8253_session *s,unsigned base,unsigned end,const unsigned char *image) {
    s->mode=2;address_s8253(s,base);lt48_status st=run_s8253(s,12);
    if(st==LT48_OK)st=run_s8253(s,8);
    for(unsigned a=base;st==LT48_OK && a<end;) {
        size_t n=end-a;if(n>4096)n=4096;
        st=block_s8253(s,1,(unsigned char *)image+a,n);a+=(unsigned)n;
    }
    if(st==LT48_OK)st=stop_s8253(s);
    if(st==LT48_OK)st=run_s8253(s,13);
    return st;
}
static lt48_status write_s8253(s8253_session *s,const unsigned char *image) {
    unsigned bases[]={0,12288,14336},ends[]={12288,14336,14400};
    lt48_status st=LT48_OK;
    for(unsigned i=0;st==LT48_OK && i<3;i++)st=write_s8253_region(s,bases[i],ends[i],image);
    return st;
}
static lt48_status mismatch_s8253(lt48_serial_page_result *out,unsigned base,const unsigned char *a,const unsigned char *b,size_t n,int blank) {
    for(size_t i=0;i<n;i++)if(a[i]!=(blank?255:b[i])) {
        if(!out->mismatch_count)out->first_mismatch=base+(unsigned)i;
        out->mismatch_count++;
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status erase_once_s8253(s8253_session *s) {
    s->mode=7;address_s8253(s,0);lt48_status st=run_s8253(s,12);
    if(st==LT48_OK)st=run_s8253(s,16);
    if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=run_s8253(s,13);
    return st;
}
static lt48_status erase_s8253(s8253_session *s,unsigned char *saved,unsigned char *actual,lt48_serial_page_result *out) {
    /* Erase first to remove locks; only then can the user-row fuse be enabled.
     * A second erase covers the row even when its original fuse was disabled. */
    saved[4]=0;memcpy(s->saved,saved,5);s->restore=1;
    lt48_status st=erase_once_s8253(s);unsigned char enabled[5];memcpy(enabled,saved,5);enabled[2]=1;
    if(st==LT48_OK)st=settings_write_s8253(s,enabled,1);
    if(st==LT48_OK)st=erase_once_s8253(s);
    if(st==LT48_OK)st=restore_s8253(s);
    if(st==LT48_OK)st=read_s8253(s,0,14400,actual,500);
    if(st==LT48_OK)st=mismatch_s8253(out,0,actual,NULL,14400,1);
    return st;
}
static lt48_status cleanup_s8253(void *v,unsigned phase) {
    s8253_session *s=v;
    if(phase==0)return stop_s8253(s);
    if(phase==1)return s->ready?run_s8253(s,13):LT48_OK;
    if(phase==2)return run_s8253(s,1);
    if(phase==4)return s->restore?restore_s8253(s):LT48_OK;
    return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
}
static lt48_status setup_s8253(s8253_session *s) {
    lt48_status st=LT48_OK;
    for(unsigned i=0;st==LT48_OK && i<((s->r->white3_page||s->r->nuvoton8||s->r->kind==10||s->r->kind==11||s->r->kind==12||s->r->kind==13||s->r->kind==14||s->r->kind==15||s->r->kind==18||s->r->kind==23||s->r->kind==24||s->r->kind==27||(s->r->kind==50||s->r->kind==51)||s->r->kind==35||s->r->kind==38||s->r->kind==37||s->r->kind==36||s->r->kind==34||s->r->kind==33||s->r->kind==32||s->r->kind==31||s->r->kind==30||s->r->kind==29||s->r->kind==28||s->r->kind==26||s->r->kind==25||s->r->kind==22||s->r->kind==20||s->r->kind==21)?s->r->sstmx_applets:s->r->kind==7?7u:4u);i++) {
        const lt48_wave_code *a=&s->r->applet[i];
        st=lt48_runtime_transport_session_wave_watchdog(s->wire,s->r->kind==51?60000:10000,1);
        if(st==LT48_OK)st=lt48_owner_load_applet_internal(s->wire,a->load_address,a->data,a->data_size);
    }
    if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s->wire,s->r->fpga,2778);
    if(st==LT48_OK)st=run_s8253(s,0);
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);
    const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof busy);
    return st;
}
#include "../../NewDeviceLib/sources/p89v51/owner.inc"
#include "../../NewDeviceLib/sources/sst89rd/owner.inc"
#include "../../NewDeviceLib/sources/w78d24/owner.inc"
#include "../../NewDeviceLib/sources/w78bank147/owner.inc"
#include "../../NewDeviceLib/sources/n79w79_88/owner.inc"
#include "../../NewDeviceLib/sources/nuvoton8/range.inc"
#include "../../NewDeviceLib/sources/numicro81/owner.inc"
#include "../../NewDeviceLib/sources/nuvoton30/owner.inc"
#include "../../NewDeviceLib/sources/n79w79_21/owner.inc"
#include "../../NewDeviceLib/sources/sstmx16/owner.inc"
#include "../../NewDeviceLib/sources/sst58rc2/owner.inc"
#include "../../NewDeviceLib/sources/cc254x5/owner.inc"
#include "../../NewDeviceLib/sources/psoc29/owner.inc"
#include "../../NewDeviceLib/sources/msp430_76/owner.inc"
#include "../../NewDeviceLib/sources/xmega7/owner.inc"
#include "../../NewDeviceLib/sources/nuvoton8/owner.inc"
#include "../../NewDeviceLib/sources/novatek-aux12/owner.inc"
#include "../../NewDeviceLib/sources/novatek25/owner.inc"
#include "../../NewDeviceLib/sources/stm8s4/owner.inc"
#include "../../NewDeviceLib/sources/at28c64b1/owner.inc"
#include "../../NewDeviceLib/sources/ramtron6/owner.inc"
#include "../../NewDeviceLib/sources/at91nine/owner.inc"
#include "../../NewDeviceLib/sources/atmel-t89six/owner.inc"
#include "../../NewDeviceLib/sources/megawin-fudan74/owner.inc"
#include "../../NewDeviceLib/sources/megawin27/owner.inc"
#include "../../NewDeviceLib/sources/syncmos80/owner.inc"
#include "../../NewDeviceLib/sources/syncmos76/owner.inc"
#include "../../NewDeviceLib/sources/megawin36/owner.inc"
#include "../../NewDeviceLib/sources/syncmos30/owner.inc"
#include "../../NewDeviceLib/sources/syncmos595x30/owner.inc"
#include "../../NewDeviceLib/sources/magna15/owner.inc"
#include "../../NewDeviceLib/sources/syntek9/owner.inc"
#include "../../NewDeviceLib/sources/snas4/owner.inc"
#include "../../NewDeviceLib/sources/syncmos6/owner.inc"
#include "../../NewDeviceLib/sources/syncmos3/owner.inc"
#include "../../NewDeviceLib/sources/onbright16/owner.inc"
#include "../../NewDeviceLib/sources/weltrend10/owner.inc"
#include "../../NewDeviceLib/sources/weltrend8/owner.inc"
#include "../../NewDeviceLib/sources/abov11/owner.inc"
#include "../../NewDeviceLib/sources/hyundai14/owner.inc"
#include "../../NewDeviceLib/sources/megawin6/owner.inc"
#include "../../NewDeviceLib/sources/megawin8/owner.inc"
#include "../../NewDeviceLib/sources/myson7/owner.inc"
#include "../../NewDeviceLib/sources/myson4/owner.inc"
#include "../../NewDeviceLib/sources/syncmos2/owner.inc"
#include "../../NewDeviceLib/sources/syncmos4g/owner.inc"
#include "../../NewDeviceLib/sources/syncmos4a/owner.inc"
#include "../../NewDeviceLib/sources/topro4/owner.inc"
#include "../../NewDeviceLib/sources/topro6/owner.inc"
#include "../../NewDeviceLib/sources/gencore8/owner.inc"
#include "../../NewDeviceLib/sources/sunplus16/owner.inc"
#include "../../NewDeviceLib/sources/sonix30/owner.inc"
#include "../../NewDeviceLib/sources/ite2/owner.inc"
#include "../../NewDeviceLib/sources/r8c8/owner.inc"
lt48_status lt48_s8253_execute(const lt48_s8253_resources *r,const lt48_protocol_transport *api,void *ctx,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void *decision_ctx,lt48_serial_page_result *out) {
    if(r && r->kind==19)return execute_t89six(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==18)return execute_at91nine(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==17)return execute_ramtron6(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==23)return execute_sy80(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==24)return execute_sy76(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==25)return execute_mw36(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==28)return execute_sx30(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==29)return execute_mg15(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r&&r->kind==30&&lt48_snas4_array_internal(r->definition))return execute_snas4(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==30)return execute_sn9(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==33)return execute_ob16(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==34)return execute_wt10(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==36)return execute_wt8(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==39)return execute_hy14(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==41)return execute_mw6(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==42)return execute_mw8(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==43)return execute_my7(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==44)return execute_my4(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==47)return execute_sc2(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==49)return execute_sc4g(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==48)return execute_sc4a(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==45)return execute_tp4(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==46)return execute_tp6(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==40)return execute_gc8(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==37)return execute_ab11(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==38)return execute_su16(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==51)return execute_r8c8(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,out);
    if(r && r->kind==50)return execute_ite2(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,out);
    if(r && r->kind==35)return execute_so30(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==32)return execute_sm3(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==31)return execute_sm6(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && (r->kind==26||r->kind==27))return execute_sm30(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==22)return execute_mw27(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && (r->kind==20||r->kind==21))return execute_mf74(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==16)return execute_at28c64b1(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==15)return execute_stm8s4(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==14)return execute_cc254x5(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==13)return execute_novatek25(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->nuvoton8)return execute_nuvoton8(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==12)return execute_xmega7(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==11)return execute_psoc29(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==10&&!r->n79_xp)return execute_sst58rc2(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==10)return execute_sstmx16(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==9)return execute_n79w79_21(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==8)return execute_nuvoton30(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==7)return execute_msp43076(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==6)return execute_numicro81(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==5)return execute_n79w79(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==4)return execute_w78bank147(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==3)return execute_w78d24(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind==2)return execute_sst(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(r && r->kind)return execute_p89v(r,api,ctx,op,start,length,image,size,read,read_size,margin,type,decide,decision_ctx,out);
    if(!out || !api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_s8253_validate(r,op,start,length,image,size);
    if(st==LT48_OK && (type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
    unsigned fields=r?5u:0;
    if(st==LT48_OK && ((op==LT48_OPERATION_READ && (!read || read_size<length)) || (op==LT48_OPERATION_CONFIGURATION && (!read || read_size<fields))))st=LT48_BUFFER_TOO_SMALL;
    if(st!=LT48_OK){out->operation_status=st;return st;}
    if(api->cancelled && api->cancelled(ctx)){out->operation_status=LT48_CANCELLED;out->cleanup_completed=1;return LT48_CANCELLED;}
    unsigned char *actual=malloc((14400u)),saved[5],desired[5];
    if(!actual){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
    s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=13;memcpy(s.vars,r->vars,sizeof s.vars);
    lt48_protocol_transport clean=*api;clean.cancelled=NULL;int setup_uncertain=0,setup_started=0;
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
    if(st==LT48_OK) {
        setup_started=1;s.api=&clean;st=setup_s8253(&s);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
        setup_uncertain=st!=LT48_OK;s.ready=st==LT48_OK;
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;s.api=api;
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
    for(unsigned attempt=0;st==LT48_OK && attempt<64;attempt++) {
        st=run_s8253(&s,4);if(st!=LT48_OK)break;
        unsigned manufacturer=s.buffers[7][0],device=s.buffers[5][0];
        if(manufacturer==r->manufacturer && device==r->device)break;
        lt48_m37_id_choice choice=decide?decide(decision_ctx,r->manufacturer,r->device,manufacturer,device):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)break;
        if(choice!=LT48_M37_ID_RETRY || attempt==63)st=LT48_DEVICE_MISMATCH;
    }
    if(st==LT48_OK)st=settings_read_s8253(&s,saved);
    int programming=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if(st==LT48_OK && (op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE)) {
        memcpy(desired,saved,fields);
        if(op==LT48_OPERATION_SECURE){unsigned level=image?image[0]:3;if(level>desired[4])desired[4]=(unsigned char)level;}
        else if(image)for(unsigned i=0;i<5;i++)if(image[0]&(1u<<i))desired[i]=image[i+1];
        int fuses=memcmp(desired,saved,4)!=0,locks=desired[4]!=saved[4];
        if(desired[4]<saved[4] || (fuses && saved[4]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && fuses)st=settings_write_s8253(&s,desired,1);
        if(st==LT48_OK && locks)st=settings_write_s8253(&s,desired,0);
        if(st==LT48_OK)memcpy(saved,desired,fields);
    } else if(st==LT48_OK) {
        if(op==LT48_OPERATION_ERASE || op==LT48_OPERATION_PROGRAM)st=erase_s8253(&s,saved,actual,out);
        else if(saved[4]>=2 || (programming && saved[4]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && programming) {
            if(op==LT48_OPERATION_OVER_PROGRAM) {
                st=read_s8253(&s,0,14400,actual,500);
                for(unsigned i=0;st==LT48_OK && i<14400;i++)if((actual[i]&image[i])!=image[i]){out->first_mismatch=i;out->mismatch_count=1;st=LT48_DEVICE_MISMATCH;}
            }
            if(st==LT48_OK) {
                memcpy(s.saved,saved,5);s.restore=1;memcpy(desired,saved,5);
                desired[2]=1; /* User-row writes require its enable fuse. */
                st=settings_write_s8253(&s,desired,1);
            }
            if(st==LT48_OK)st=write_s8253(&s,image);
            unsigned delta=type?50:25,volts[]={500-delta,500+delta};
            for(unsigned i=0;st==LT48_OK && i<2;i++) {
                st=read_s8253(&s,0,(14400u),actual,volts[i]);
                if(st==LT48_OK)st=mismatch_s8253(out,0,actual,image,(14400u),0);
            }
        } else if(st==LT48_OK && op!=LT48_OPERATION_ERASE) {
            unsigned delta=type?50:25,volts[]={500-delta,500+delta};
            unsigned passes=op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_ALL?2:1;
            if(op!=LT48_OPERATION_VERIFY || margin==LT48_VERIFY_MARGIN_NOMINAL)volts[0]=500;
            else if(margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
            for(unsigned i=0;st==LT48_OK && i<passes;i++) {
                st=read_s8253(&s,start,length,actual,volts[i]);
                if(st==LT48_OK && op!=LT48_OPERATION_READ)st=mismatch_s8253(out,start,actual,image,length,op==LT48_OPERATION_BLANK_CHECK);
            }
        }
    }
    if(st==LT48_OK && s.restore)st=restore_s8253(&s);
    if(st==LT48_OK)st=settings_read_s8253(&s,desired);
    if(st==LT48_OK && memcmp(saved,desired,fields))st=LT48_DEVICE_MISMATCH;
    out->operation_status=st;
    if(!setup_started){lt48_runtime_transport_session_close(s.wire);out->cleanup_completed=1;free(actual);return st;}
    s.api=&clean;const unsigned phases[]={0,1,4,2,3};
    st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_s8253,&s,phases,5,st,&out->cleanup_status,&out->cleanup_completed);
    if(setup_uncertain){out->cleanup_status=LT48_CLEANUP_FAILED;out->cleanup_completed=0;}
    if(st==LT48_OK) {
        if(op==LT48_OPERATION_READ)memcpy(read,actual,length);
        if(op==LT48_OPERATION_CONFIGURATION)memcpy(read,desired,fields);
        out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?fields:length;
    }
    free(actual);return st;
}
