#include "at89_c5132_guard.h"
lt48_status lt48_c5132_guard_check(const lt48_protocol_transport *api,void *ctx) {
    if(!api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
    /* Q1 72 dispatches the RAM helper at 4200; queue opcode3 captures its byte.
     * This is a status-bearing 2C response, unlike unframed bulk read43. */
    const unsigned char query[]={0x2c,1,0x72,3,0,0,0};
    unsigned char reply[512]={0};size_t done=0;
    if(api->cancelled && api->cancelled(ctx))return LT48_CANCELLED;
    lt48_status st=api->write(ctx,2,query,sizeof query,&done);
    if(st!=LT48_OK)return st;
    if(done!=sizeof query)return LT48_PROTOCOL_ERROR;
    if(api->cancelled && api->cancelled(ctx))return LT48_CANCELLED;
    st=api->read(ctx,0x88,reply,sizeof reply,&done);
    if(st!=LT48_OK)return st;
    if(done!=sizeof reply || reply[0]!=1 || reply[1]!=0 || reply[2]!=1)return LT48_PROTOCOL_ERROR;
    return LT48_OK;
}
