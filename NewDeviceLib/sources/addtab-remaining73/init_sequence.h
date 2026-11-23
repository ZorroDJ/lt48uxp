/* Authored correction: load idle data while target RESET is asserted.
 * Exact class-2045 profiles are additionally bound by the source digest gate.
 * Original operation 2 loads FF after releasing RESET; the recovered FPGA
 * produces a write cycle at that load. No relaxation of the NOR model is used.
 */
#ifndef LT48_R73_INIT_SEQUENCE_H
#define LT48_R73_INIT_SEQUENCE_H
#include <stdlib.h>
#include <string.h>
static unsigned char *remaining73_init_wave(unsigned d,unsigned operation,
 const unsigned char *code,size_t size,size_t *out_size) {
 unsigned byte=d==0x74 || d==0x75;
 if((!byte && d!=0x3e && d!=0x3f) || operation!=2 || size!=(byte?121u:142u))return NULL;
 static const unsigned char reset[]={0x11,5,0};
 static const unsigned char word_load[]={0x3b,0xc1,0x3b,0xff,0x3b,0xff};
 static const unsigned char byte_load[]={0x3b,0xc0,0x3b,0xff};
 const unsigned char *load=byte?byte_load:word_load;size_t n=byte?sizeof(byte_load):sizeof(word_load);
 size_t reset_at=0,load_at=0,rc=0,lc=0;
 for(size_t i=2;i+sizeof(reset)<=size;i++)if(!memcmp(code+i,reset,sizeof(reset))){reset_at=i;rc++;}
 for(size_t i=2;i+n<=size;i++)if(!memcmp(code+i,load,n)){load_at=i;lc++;}
 if(rc!=1 || lc!=1 || reset_at+sizeof(reset)>=load_at)return NULL;
 size_t at=reset_at+sizeof(reset);unsigned char *out=malloc(size);if(!out)return NULL;
 memcpy(out,code,at);memcpy(out+at,load,n);memcpy(out+at+n,code+at,load_at-at);
 memcpy(out+load_at+n,code+load_at+n,size-load_at-n);*out_size=size;return out;
}
#endif
