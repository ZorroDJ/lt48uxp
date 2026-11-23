#ifndef LT48_PIC_MIDRANGE14_CONTROLLER_H
#define LT48_PIC_MIDRANGE14_CONTROLLER_H
#include "midrange14_protocol.h"
#include <string.h>
/* Encode the entire timing-critical span for one controller_write call.
 * This prevents the protocol queue flushing between Begin/Delay/End.
 * Actual clock duration, short transfers and source entry remain separate. */
static inline lt48_status lt48_pic_midrange_commit_packet(const lt48_pic_midrange_policy *p,unsigned address,unsigned data,unsigned clock,unsigned char *out,size_t capacity,size_t *size){
 if(!size)return LT48_INVALID_ARGUMENT;
 *size=0;
 if(!lt48_pic_midrange_valid(p)||!lt48_pic_midrange_address(p,address,1)||!out||capacity<17||data>188||clock>188||(data&3)||(clock&3)||data==clock)return LT48_INVALID_ARGUMENT;
 int internal=p->internal_configuration&&address>=0x2000;
 unsigned us=internal?(address==0x2007?p->internal_config_us:p->internal_data_us):p->external_us;
 if(us>6553500)return LT48_INVALID_ARGUMENT;
 unsigned char begin[]={0x58,(unsigned char)data,(unsigned char)clock,5,(unsigned char)(internal?8:24)};
 memcpy(out,begin,sizeof(begin));size_t n=sizeof(begin);
 if(us<=255){out[n++]=0x4e;out[n++]=(unsigned char)us;}
 else {unsigned units=us/100;out[n++]=0x4f;out[n++]=(unsigned char)units;out[n++]=(unsigned char)(units>>8);
  if(us%100){out[n++]=0x4e;out[n++]=(unsigned char)(us%100);}}
 if(!internal){out[n++]=0x58;out[n++]=(unsigned char)data;out[n++]=(unsigned char)clock;out[n++]=5;out[n++]=10;}
 out[n++]=0x4e;out[n++]=100;*size=n;
 return LT48_OK;
}
#endif
