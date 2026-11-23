/* Authored experimental RE2 error reporting, revision 3.
 * Exact user-supplied firmware only; no original firmware distributed here.
 * Preserve original replies and pin recovery. Latch timeout at SRAM 5466.
 * Applet command 1 reports that latch (1=clear, 0=failed); command 2 clears it.
 * Neither setup nor query clears failure. The host must clear before a logical
 * transaction and query through status-checked USB 2C after its final capture.
 * 5466 is the final reserved byte of the extended W363 image in known RAM.
 * Original W363/W364 use neither commands 1/2 nor XRAM 5466 for device work.
 * The original unused command handlers only consumed their command byte.
 */
#include <labtool48/labtool48.h>
#include <stdio.h>
#include <string.h>
lt48_status lt48_re2_guard_applet(unsigned address,const unsigned char *source,size_t size,unsigned char *out,size_t capacity,size_t *out_size) {
    const char *expected;size_t length,branch,old_failure;
    if(!source || !out || !out_size)return LT48_INVALID_ARGUMENT;
    *out_size=0;
    if(address==0x5000){length=1084;branch=0x3c0;old_failure=0x5412;expected="65e8e9b670cc9d0a8518a219722aec932ac280fe2347835fb94237e54b8e0a97";}
    else if(address==0x6000){length=1319;branch=0x474;old_failure=0x6521;expected="e7793b50ffaacab0312d45ff209589cbb4bc6a3e91e76dad27cbe39cfccfd93f";}
    else return LT48_UNSUPPORTED;
    if(size!=length)return LT48_SAFETY_REJECTED;
    size_t extra=address==0x5000?43:42;
    if(capacity<length+extra)return LT48_BUFFER_TOO_SMALL;
    unsigned char digest[32];char hash[65];lt48_sha256(source,size,digest);
    for(unsigned i=0;i<32;i++)snprintf(hash+i*2,3,"%02x",digest[i]);
    if(strcmp(hash,expected))return LT48_SAFETY_REJECTED;
    unsigned start=address+(unsigned)length,query=start+11,clear=query+15,dispatch=address+6;
    unsigned char tail[]={
        0x90,0x54,0x66,0xe4,0xf0,0x90,0xf0,0,0x02,(unsigned char)(old_failure>>8),(unsigned char)old_failure,
        0x75,0xa0,0xc8,0xe2,0x90,0x54,0x66,0xe0,0xf2,0x90,0xf0,0,0x02,(unsigned char)(dispatch>>8),(unsigned char)dispatch,
        0x75,0xa0,0xc8,0xe2,0x90,0x54,0x66,0x74,1,0xf0,0x90,0xf0,0,0x02,(unsigned char)(dispatch>>8),(unsigned char)dispatch};
    memmove(out,source,length);memcpy(out+length,tail,sizeof tail);
    out[branch+1]=(unsigned char)(start>>8);out[branch+2]=(unsigned char)start;
    out[0x5d]=(unsigned char)(query>>8);out[0x5e]=(unsigned char)query;
    out[0x61]=(unsigned char)(clear>>8);out[0x62]=(unsigned char)clear;
    if(address==0x5000)out[length+sizeof tail]=1; /* Reserved latch, never executed. */
    *out_size=length+extra;return LT48_OK;
}
