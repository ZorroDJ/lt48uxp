#include "at89_re2_guard.h"

static lt48_status exchange(const lt48_protocol_transport *api,void *ctx,
                           const unsigned char *command,size_t length,
                           unsigned char reply[512]) {
    size_t done=0;
    if(api->cancelled && api->cancelled(ctx))return LT48_CANCELLED;
    lt48_status st=api->write(ctx,2,command,length,&done);
    if(st!=LT48_OK)return st;
    if(done!=length)return LT48_PROTOCOL_ERROR;
    if(api->cancelled && api->cancelled(ctx))return LT48_CANCELLED;
    st=api->read(ctx,0x88,reply,512,&done);
    if(st!=LT48_OK)return st;
    /* On bridge failure FC02 is a diagnostic bit and can equal one. The
     * completion prefix must be validated before interpreting the latch. */
    if(done!=512 || reply[0]!=1 || reply[1]!=0)return LT48_PROTOCOL_ERROR;
    return LT48_OK;
}

lt48_status lt48_re2_guard_check(const lt48_protocol_transport *api,void *ctx,int clear) {
    if(!api || !api->write || !api->read || (clear!=0 && clear!=1))return LT48_INVALID_ARGUMENT;
    const unsigned char reset[]={0x2c,1,2,0,0};
    const unsigned char query[]={0x2c,1,1,3,0,0,0};
    unsigned char reply[512]={0};
    lt48_status st=LT48_OK;
    if(clear)st=exchange(api,ctx,reset,sizeof reset,reply);
    if(st==LT48_OK)st=exchange(api,ctx,query,sizeof query,reply);
    if(st==LT48_OK && reply[2]!=1)st=LT48_PROTOCOL_ERROR;
    return st;
}
