#ifndef LT48_DRAFT_PIC18_XX39_ERASE_APPLET_H
#define LT48_DRAFT_PIC18_XX39_ERASE_APPLET_H
#include "pic18_xx39_erase_plan.h"
#include <labtool48/labtool48.h>
#include <string.h>
/* Authored compact plan interpreter, appended to the exact BYO dispatcher.
 * Dictionary tokens keep the 64-row partial-panel procedure in [9000,9800).
 * No public admission, filesystem mutation, or hardware access. */
typedef struct {
 unsigned char bytes[2048],tokens[1536],dictionary[255][3];
 size_t used,token_count,dictionary_count,labels[16],fix_count;
 struct {size_t at;unsigned label,relative;} fix[64];int failed;
} lt48_xx39_assembler;
static inline void lt48_xx39_byte(lt48_xx39_assembler *a,unsigned value){
 if(a->used>=sizeof(a->bytes))a->failed=1;
 else a->bytes[a->used++]=(unsigned char)value;
}
static inline void lt48_xx39_bytes(lt48_xx39_assembler *a,const unsigned char *p,size_t size){
 for(size_t i=0;i<size;i++)lt48_xx39_byte(a,p[i]);
}
#define LT48_XX39_BYTES(a,...) do{const unsigned char b[]={__VA_ARGS__};lt48_xx39_bytes((a),b,sizeof(b));}while(0)
static inline void lt48_xx39_fix(lt48_xx39_assembler *a,unsigned label,unsigned relative){
 if(a->fix_count>=64){a->failed=1;return;}
 a->fix[a->fix_count].at=a->used;a->fix[a->fix_count].label=label;
 a->fix[a->fix_count++].relative=relative;lt48_xx39_byte(a,0);
 if(!relative)lt48_xx39_byte(a,0);
}
static inline void lt48_xx39_call(lt48_xx39_assembler *a,unsigned label){
 lt48_xx39_byte(a,0x12);lt48_xx39_fix(a,label,0);
}
static inline int lt48_xx39_dictionary_emit(void *context,const lt48_xx39_erase_event *e){
 lt48_xx39_assembler *a=context;unsigned char row[3];
 if(e->kind==LT48_XX39_FRAME){if(e->command!=0&&e->command!=12)return -1;row[0]=(unsigned char)e->command;}
 else if(e->kind==LT48_XX39_COMMAND){if(e->command||e->value)return -1;row[0]=0x10;}
 else if(e->kind==LT48_XX39_OPERAND){if(e->command||e->value)return -1;row[0]=0x20;}
 else if(e->kind==LT48_XX39_WAIT_US){if(e->command||e->value!=11000)return -1;row[0]=0x30;}
 else return -1;
 row[1]=(unsigned char)e->value;row[2]=(unsigned char)(e->value>>8);
 size_t index=0;while(index<a->dictionary_count&&memcmp(row,a->dictionary[index],3))index++;
 if(index==255||a->token_count==sizeof(a->tokens))return -1;
 if(index==a->dictionary_count){memcpy(a->dictionary[index],row,3);a->dictionary_count++;}
 a->tokens[a->token_count++]=(unsigned char)index;return 0;
}
static inline lt48_status lt48_pic18_xx39_erase_applet(const unsigned char *original,size_t original_size,
 uint16_t base,uint32_t code_bytes,unsigned char *output,size_t capacity,size_t *written){
 static const unsigned char expected[32]={0xc0,0x31,0x4b,0xc4,0x90,0x62,0x37,0x30,0xb1,0x4d,0x28,0xf0,0x2a,0x10,0x0c,0x26,0x54,0xcf,0x0c,0x40,0x51,0x68,0x6e,0xdf,0x61,0x30,0x83,0x7b,0x3a,0x58,0xbd,0xc1};
 if(!written)return LT48_INVALID_ARGUMENT;
 *written=0;
 if(!original||!output||original==output)return LT48_INVALID_ARGUMENT;
 if(original_size!=527||base!=0x9000||(code_bytes!=12288&&code_bytes!=24576))return LT48_SAFETY_REJECTED;
 unsigned char sha[32];lt48_sha256(original,original_size,sha);
 if(memcmp(sha,expected,32))return LT48_SAFETY_REJECTED;
 lt48_xx39_assembler a={0};
 if(lt48_pic18_xx39_erase_plan(code_bytes,lt48_xx39_dictionary_emit,&a))return LT48_SAFETY_REJECTED;
 lt48_xx39_bytes(&a,original,151); /* Original command5 setup and dispatcher. */
 enum {LOOP,DONE,SPECIAL,OPERAND,WAIT,NEXT,SEND_COMMAND,SEND_OPERAND,TIMER,DICT,STREAM};
 /* R6:R7 holds stream pointer, R3 descriptor, R4:R5 operand, R2 timer count. */
 LT48_XX39_BYTES(&a,0x90);lt48_xx39_fix(&a,STREAM,0);
 LT48_XX39_BYTES(&a,0xae,0x82,0xaf,0x83);
 a.labels[LOOP]=a.used;
 LT48_XX39_BYTES(&a,0x8e,0x82,0x8f,0x83,0xe4,0x93,0xa3,0xae,0x82,0xaf,0x83);
 LT48_XX39_BYTES(&a,0xb4,0xff);lt48_xx39_fix(&a,NEXT,1);
 lt48_xx39_byte(&a,0x02);lt48_xx39_fix(&a,DONE,0);
 a.labels[NEXT]=a.used;
 LT48_XX39_BYTES(&a,0x75,0xf0,3,0xa4,0x24);size_t dict_low=a.used;lt48_xx39_byte(&a,0);
 LT48_XX39_BYTES(&a,0xf5,0x82,0xe5,0xf0,0x34);size_t dict_high=a.used;lt48_xx39_byte(&a,0);
 LT48_XX39_BYTES(&a,0xf5,0x83,0xe4,0x93,0xfb,0xa3,0xe4,0x93,0xfc,0xa3,0xe4,0x93,0xfd,0x90,0xf0,0);
 LT48_XX39_BYTES(&a,0xeb,0x54,0xf0,0x70);lt48_xx39_fix(&a,SPECIAL,1);
 lt48_xx39_call(&a,SEND_COMMAND);lt48_xx39_call(&a,SEND_OPERAND);
 lt48_xx39_byte(&a,0x02);lt48_xx39_fix(&a,LOOP,0);
 a.labels[SPECIAL]=a.used;
 LT48_XX39_BYTES(&a,0xeb,0xb4,0x10);lt48_xx39_fix(&a,OPERAND,1);
 LT48_XX39_BYTES(&a,0x7b,0);lt48_xx39_call(&a,SEND_COMMAND);
 lt48_xx39_byte(&a,0x02);lt48_xx39_fix(&a,LOOP,0);
 a.labels[OPERAND]=a.used;
 LT48_XX39_BYTES(&a,0xb4,0x20);lt48_xx39_fix(&a,WAIT,1);
 lt48_xx39_call(&a,SEND_OPERAND);lt48_xx39_byte(&a,0x02);lt48_xx39_fix(&a,LOOP,0);
 a.labels[WAIT]=a.used;
 /* Only builder-generated dictionary tokens can reach this branch. */
 LT48_XX39_BYTES(&a,0x7a,11);a.labels[TIMER]=a.used;
 LT48_XX39_BYTES(&a,0xc2,0x8c,0xc2,0x8d,0x75,0x8a,0x30,0x75,0x8c,0xf8,0xd2,0x8c,0x30,0x8d,0xfd,0xda);
 lt48_xx39_fix(&a,TIMER,1);LT48_XX39_BYTES(&a,0xc2,0x8c);
 lt48_xx39_byte(&a,0x02);lt48_xx39_fix(&a,LOOP,0);
 a.labels[DONE]=a.used;
 LT48_XX39_BYTES(&a,0x74,1,0xf2,0x02,0x90,6);
 a.labels[SEND_COMMAND]=a.used;
 LT48_XX39_BYTES(&a,0x74,0xd4,0xf0,0x74,4,0xf0,0x74,0xc0,0xf0,0xeb,0xf0,0x30,0x94,0xfd,0x22);
 a.labels[SEND_OPERAND]=a.used;
 LT48_XX39_BYTES(&a,0x74,0xd4,0xf0,0x74,8,0xf0,0x74,0xc0,0xf0,0xec,0xf0,0x30,0x94,0xfd,
  0x74,0xc0,0xf0,0xed,0xf0,0x30,0x94,0xfd,0x22);
 a.labels[DICT]=a.used;
 for(size_t i=0;i<a.dictionary_count;i++)lt48_xx39_bytes(&a,a.dictionary[i],3);
 a.labels[STREAM]=a.used;lt48_xx39_bytes(&a,a.tokens,a.token_count);lt48_xx39_byte(&a,255);
 if(a.failed)return LT48_SAFETY_REJECTED;
 a.bytes[dict_low]=(unsigned char)(base+a.labels[DICT]);a.bytes[dict_high]=(unsigned char)((base+a.labels[DICT])>>8);
 for(size_t i=0;i<a.fix_count;i++){
  size_t at=a.fix[i].at,target=a.labels[a.fix[i].label];if(!target)return LT48_SAFETY_REJECTED;
  if(a.fix[i].relative){int offset=(int)target-(int)(at+1);if(offset < -128||offset>127)return LT48_SAFETY_REJECTED;a.bytes[at]=(unsigned char)offset;}
  else{a.bytes[at]=(unsigned char)((base+target)>>8);a.bytes[at+1]=(unsigned char)(base+target);}
 }
 if(capacity<a.used)return LT48_BUFFER_TOO_SMALL;
 memcpy(output,a.bytes,a.used);*written=a.used;return LT48_OK;
}
#undef LT48_XX39_BYTES
#endif
