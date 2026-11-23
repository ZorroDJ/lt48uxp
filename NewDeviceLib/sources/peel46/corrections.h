/* Authored repair of two GB ATV2500B host-image offsets. No wave payloads. */
#ifndef LT48_PEEL46_CORRECTIONS_H
#define LT48_PEEL46_CORRECTIONS_H
static inline int peel46_security_context(unsigned d,unsigned m,unsigned p){
 return (d==0x12u&&m==13u&&p==9u)||(d==0x13u&&m==14u&&p==10u);
}
static inline lt48_status peel46_security_wave(unsigned d,unsigned m,unsigned p,unsigned op,
 const unsigned char*raw,size_t n,unsigned char**out,size_t*size){
 *out=NULL;*size=0;
 if(!peel46_security_context(d,m,p)||op!=2u)return LT48_NOT_FOUND;
 const unsigned char first[]={0x49,1,0,0,4,2},second[]={0x49,0,4,1,0,2};
 if(n<0x39u || memcmp(raw+0x13,first,sizeof first)||memcmp(raw+0x33,second,sizeof second))return LT48_FORMAT_ERROR;
 unsigned char*copy=malloc(n);if(!copy)return LT48_NO_MEMORY;
 memcpy(copy,raw,n);copy[0x17]=0;copy[0x35]=0;*out=copy;*size=n;return LT48_OK;
}
#endif
