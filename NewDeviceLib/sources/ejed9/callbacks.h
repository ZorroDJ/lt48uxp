/* Authored equivalents of the GBD native callbacks; source buffers0..75
 * remain unchanged. Session-private76..80 replace the host heap allocation. */
#ifndef LT48_EJED9_CALLBACKS_H
#define LT48_EJED9_CALLBACKS_H
#include <string.h>
static inline unsigned ejed9_word(const unsigned char*p){return p[0]|(unsigned)p[1]<<8;}
static inline unsigned ejed9_dword(const unsigned char*p){return ejed9_word(p)|(unsigned)ejed9_word(p+2)<<16;}
static inline lt48_status ejed9_step(unsigned action,unsigned arg,unsigned char b[][32]){
 unsigned char *meta=b[76];unsigned col=ejed9_dword(b[0]),row=ejed9_word(b[1]);
#define EC(i) b[77+(i)/32][(i)%32]
 if(action==0||action==8){for(unsigned i=0;i<(action==8?99u:100u);i++)EC(i)=action==8?255:0;meta[1]=1;return LT48_OK;}
 if(action==7){meta[1]=0;return LT48_OK;} /* free(NULL) is harmless during recovery. */
 if(!meta[1])return LT48_SAFETY_REJECTED;
 switch(action){
 case 1:meta[0]=b[2][0];break;
 case 2:if(arg>=100)return LT48_FORMAT_ERROR;b[2][0]=EC(arg);break;
 case 3:case 10:for(unsigned i=0;i<(action==3?100:99);i++)EC(i)=action==3?0:255;break;
 case 4:memcpy(b[15],b[1],2);break;
 case 5:if(col>=100)return LT48_SAFETY_REJECTED;EC(col)=meta[0];b[3][0]=b[2][0];break;
 case 7:meta[1]=0;break;
 case 11:{if(col>=792)return LT48_SAFETY_REJECTED;unsigned bit=(EC(col/8)>>(col%8))&1u;b[3][0]=(254u+bit)^(ejed9_word(b[66])!=0);break;}
 case 12:{if(col>=99)return LT48_SAFETY_REJECTED;unsigned value=0;for(unsigned i=0;i<8;i++){unsigned bit=col+99*i;value|=((EC(bit/8)>>(bit%8))&1u)<<(7-i);}b[3][0]=(unsigned char)value;break;}
 case 13:if(col>=792)return LT48_SAFETY_REJECTED;if(!(b[2][0]&1))EC(col/8)&=(unsigned char)~(1u<<(col%8));break;
 case 14:{if(row>=66&&row<=79&&col>=19&&col<=96&&(col-19)%11==0)col-=4;
  if(col>=99)return LT48_SAFETY_REJECTED;
  for(unsigned i=0;i<8;i++){unsigned bit=col+99*i;if(!(b[2][0]&(0x80u>>i)))EC(bit/8)&=(unsigned char)~(1u<<(bit%8));}break;}
 case 15:b[65][0]=0;b[65][1]=0;break;
 case 16:b[65][0]=1;b[65][1]=0;break;
 case 17:if(arg>=99)return LT48_FORMAT_ERROR;EC(arg)=b[63][0];break;
 case 18:if(arg>=99)return LT48_FORMAT_ERROR;b[64][0]=EC(arg)^(ejed9_word(b[66])?255:0);break;
 default:return LT48_UNSUPPORTED;
 }
#undef EC
 return LT48_OK;
}
static inline lt48_status ejed9_compile(unsigned cb,unsigned width,lt48_compiled_wave**out){
 uint16_t w[2048]={0};size_t n=1;if(width<1||width>100||cb>11)return LT48_FORMAT_ERROR;
#define E(v) do{if(n>=2048)return LT48_FORMAT_ERROR;w[n++]=(v);}while(0)
#define S(a,b) do{E(0xf006);E(a);E(b);}while(0)
#define CALL(v) do{E(0x25);E(v);}while(0)
 if(cb==0)S(0,0);
 else if(cb==1||cb==2){size_t same=0,zero=0;
  if(cb==1){S(1,0);E(0xf007);E(0);same=n;E(0);E(0xf007);E(1);zero=n;E(0);}
  CALL(8);for(unsigned i=width;i-->0;){S(2,i);CALL(9);}CALL(10);
  if(cb==1){S(3,0);w[zero]=n;S(4,0);w[same]=n;S(5,0);}else S(7,0);
 }else if(cb==3)S(8,0);
 else if(cb==4)S(10,0);
 else if(cb==5)S(7,0);
 else if(cb==6){S(10,0);S(15,0);for(unsigned i=99;i-->0;){if(!i)S(16,0);CALL(33);S(17,i);}}
 else if(cb==7)S(12,0);
 else if(cb==8)S(11,0);
 else if(cb==9){S(15,0);for(unsigned i=0;i<99;i++){if(i==98)S(16,0);S(18,i);CALL(35);}S(10,0);}
 else if(cb==10)S(14,0);
 else if(cb==11)S(13,0);
 E(0x52);E(1);w[0]=n-1;
#undef E
#undef S
#undef CALL
 return lt48_compiled_host_words_internal(w,n,out);
}
#endif
