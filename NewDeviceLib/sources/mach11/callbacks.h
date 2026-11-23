/* Authored equivalents of Dataman 13.10 0045B9E0..0045BC49.
 * Session-private scratch buffers70..74 replace the original global allocation.
 * They are snapshotted on interpreter replay; original named buffers stay exact.
 * Native child calls use timeout10 seconds, not mode10. */
#ifndef LT48_MACH11_CALLBACKS_H
#define LT48_MACH11_CALLBACKS_H
#include <string.h>
static inline lt48_status mach11_step(unsigned action,unsigned arg,unsigned char b[][32]){
 unsigned char *meta=b[70];unsigned row=b[1][0]|(unsigned)b[1][1]<<8;
 if(action==0||action==8){for(unsigned i=0;i<100;i++)b[71+i/32][i%32]=action==8?255:0;meta[1]=1;return LT48_OK;}
 if(!meta[1])return LT48_SAFETY_REJECTED;
 switch(action){
 case 1:meta[0]=b[2][0];break;
 case 2:if(arg>=100)return LT48_FORMAT_ERROR;b[2][0]=b[71+arg/32][arg%32];break;
 case 3:case 10:for(unsigned i=0;i<(action==3?100:99);i++)b[71+i/32][i%32]=action==3?0:255;break;
 case 4:memcpy(b[7],b[1],2);break;
 case 5:case 9:{unsigned col=b[0][0]|(unsigned)b[0][1]<<8;
  if(action==9){col&=255;if(row>=66&&row<=79&&col>=2&&col<=79&&(col-2)%11==0)col+=4;}
  if(col>=(action==9?99u:100u))return LT48_SAFETY_REJECTED;
  b[71+col/32][col%32]=action==5?meta[0]:b[2][0];break;}
 case 6:b[2][0]=meta[0];break;
 case 7:meta[1]=0;break; /* No original named-buffer mutation on free. */
 default:return LT48_UNSUPPORTED;
 }
 return LT48_OK;
}
static inline lt48_status mach11_compile(unsigned cb,unsigned width,lt48_compiled_wave **out){
 uint16_t w[2048]={0};size_t n=1;
 if(!width||width>100||cb<1||cb>7)return LT48_FORMAT_ERROR;
#define MC_EMIT(v) do{if(n>=2048)return LT48_FORMAT_ERROR;w[n++]=(v);}while(0)
#define MC_STEP(a,b) do{MC_EMIT(0xf004);MC_EMIT(a);MC_EMIT(b);}while(0)
#define MC_CALL(v) do{MC_EMIT(0x25);MC_EMIT(v);}while(0)
 if(cb==1)MC_STEP(0,0);
 else if(cb==2||cb==3){
  size_t same=0,zero=0;
  if(cb==2){MC_STEP(1,0);MC_EMIT(0xf005);MC_EMIT(0);same=n;MC_EMIT(0);MC_EMIT(0xf005);MC_EMIT(1);zero=n;MC_EMIT(0);}
  MC_CALL(11);for(unsigned i=width;i-->0;){MC_STEP(2,i);MC_CALL(12);}MC_CALL(13);
  if(cb==2){MC_STEP(3,0);w[zero]=(uint16_t)n;MC_STEP(4,0);w[same]=(uint16_t)n;MC_STEP(5,0);MC_STEP(6,0);}
  else MC_STEP(7,0);
 }else if(cb==4)MC_STEP(9,0);
 else if(cb==5)MC_STEP(8,0);
 else if(cb==6)MC_STEP(7,0);
 else if(cb==7){MC_STEP(1,0);for(unsigned i=99;i-->0;){if(!i)MC_CALL(21);MC_STEP(2,i);MC_CALL(20);}MC_STEP(6,0);MC_STEP(10,0);}
 MC_EMIT(0x52);MC_EMIT(1);w[0]=(uint16_t)(n-1);
#undef MC_STEP
#undef MC_CALL
#undef MC_EMIT
 return lt48_compiled_host_words_internal(w,n,out);
}
#endif
