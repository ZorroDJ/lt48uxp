#ifndef LT48_PIC_COMMON_CLASSIC14_H
#define LT48_PIC_COMMON_CLASSIC14_H
#include "entry.h"
/* Source-bound classic architecture compilers. Exact shape gates and source
 * digests are required. Availability is controlled by qualified membership. */
static inline lt48_status lt48_pic_classic14_identity_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t var_count,
 const lt48_pic_entry_policy *p,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||!pins||!vars||!p||p->operation_id!=3||p->id_mask!=0x3fe0||
    (p->id_expected&~p->id_mask)||lt48_pic_entry_revision_bits(p)!=5||
    lt48_pic_entry_id_variable(p)!=16||p->data_pin>255||p->clock_pin>255||
    p->data_pin==p->clock_pin)return LT48_INVALID_ARGUMENT;
 unsigned found=0;
 for(size_t i=0;i<var_count;i++)if(vars[i].variable_id==16){
  if(vars[i].kind!=2||vars[i].enabled||vars[i].channel||vars[i].value!=p->id_source_value)return LT48_SAFETY_REJECTED;
  found++;
 }
 if(found!=1)return LT48_SAFETY_REJECTED;
 const unsigned char op[]={0x2b,0x1b,0x1a,0x40,0x45,0x41,0x5d,0x6c,0x6b,0x60};
 const unsigned char n[]={2,5,8,6,5,3,3,2,1,0};
 const unsigned char args[][8]={
  {(unsigned char)p->clock_pin,1},
  {(unsigned char)p->data_pin,(unsigned char)p->clock_pin,12,14,1},
  {(unsigned char)p->data_pin,(unsigned char)p->clock_pin,0,0,0,0,1,1},
  {11,4,0,0,0,0},{11,0,12,5,8},{23,4,16},{23,1,11},{6,0},{0},{0}
 };
 size_t count=lt48_compact_instruction_count(source),size=5,match=0;unsigned matches=0,uses=0;
 for(size_t i=0;i<count;i++){
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)return st;
  if(size>65534||in.operand_size>65535-size-1)return LT48_SAFETY_REJECTED;
  size+=1+in.operand_size;
  if(in.opcode==0x41&&in.operand_size==3&&in.operands[2]==16)uses++;
 }
 for(size_t start=0;start+10<=count;start++){
  unsigned same=1;
  for(unsigned j=0;j<10;j++){
   lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,start+j,&in);if(st!=LT48_OK)return st;
   if(in.opcode!=op[j]||in.operand_size!=n[j]||memcmp(in.operands,args[j],n[j])){same=0;break;}
  }
  if(same){match=start;matches++;}
 }
 if(matches!=1||uses!=1)return LT48_SAFETY_REJECTED;
 unsigned char *raw=malloc(size);if(!raw)return LT48_NO_MEMORY;
 size_t used=2;lt48_status st=LT48_OK;
 for(size_t i=0;i<count;i++){
  lt48_compact_instruction in;st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)break;
  if(i==match+5){
   unsigned id=p->id_expected>>5;
   const unsigned char literal[]={0x40,23,4,(unsigned char)id,(unsigned char)(id>>8),0,0};
   memcpy(raw+used,literal,sizeof(literal));used+=sizeof(literal);
  }else{
   raw[used++]=in.opcode;memcpy(raw+used,in.operands,in.operand_size);
   if(i==match+4)raw[used+4]=9;
   used+=in.operand_size;
  }
 }
 lt48_compact_wave *changed=NULL;
 if(st==LT48_OK){raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);st=lt48_compact_decode(raw,used,&changed);}
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,var_count,1,0,out);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK&&*out){lt48_compiled_wave_close(*out);*out=NULL;}
 lt48_compact_wave_close(changed);free(raw);return st;
}

static inline lt48_status lt48_pic_classic14_wait_compile(
 const lt48_compact_wave *source,const lt48_pin_map *pins,
 const lt48_compile_variable *vars,size_t count,const lt48_pic_classic14_policy *policy,
 unsigned role,lt48_compiled_wave **output) {
 if(!output)return LT48_INVALID_ARGUMENT;
 *output=NULL;
 if(!source||!pins||!vars||!policy||(role!=8&&role!=10))return LT48_INVALID_ARGUMENT;
 if(policy->data_pin!=48||policy->clock_pin!=49||policy->code_wait_variable!=8||
    policy->source_code_wait_us!=2500||policy->selected_code_wait_us!=3000)return LT48_SAFETY_REJECTED;
 unsigned found=0;
 for(size_t i=0;i<count;i++)if(vars[i].variable_id==policy->code_wait_variable) {
  if(vars[i].kind!=1||vars[i].enabled!=1||vars[i].value!=1||vars[i].channel!=25)return LT48_SAFETY_REJECTED;
  found++;
 }
 if(found!=1)return LT48_SAFETY_REJECTED;
 size_t n=lt48_compact_instruction_count(source),size=2;unsigned expected=role==8?2:1;
 for(size_t i=0;i<n;i++) {
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)return st;
  if(size>65530||in.operand_size>65530-size-1)return LT48_SAFETY_REJECTED;
  size+=1+in.operand_size;
 }
 if(size>65535-3*expected)return LT48_SAFETY_REJECTED;
 unsigned char *raw=malloc(size+3*expected);if(!raw)return LT48_NO_MEMORY;
 size_t used=2;unsigned matched=0;lt48_status st=LT48_OK;
 for(size_t i=0;i<n;i++) {
  lt48_compact_instruction in,begin;st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)break;
  if(in.opcode==0x31&&in.operand_size==1&&in.operands[0]==policy->code_wait_variable) {
   if(!i||matched==expected){st=LT48_SAFETY_REJECTED;break;}
   st=lt48_compact_get_instruction(source,i-1,&begin);if(st!=LT48_OK)break;
   const unsigned char command[]={48,49,8,0,0,0,6,1};
   if(begin.opcode!=0x1a||begin.operand_size!=8||memcmp(begin.operands,command,8)) {st=LT48_SAFETY_REJECTED;break;}
   raw[used++]=0x32;for(unsigned b=0;b<4;b++)raw[used++]=(unsigned char)(policy->selected_code_wait_us>>(8*b));
   matched++;
  }else{raw[used++]=in.opcode;memcpy(raw+used,in.operands,in.operand_size);used+=in.operand_size;}
 }
 if(st==LT48_OK&&matched!=expected)st=LT48_SAFETY_REJECTED;
 lt48_compact_wave *changed=NULL;
 if(st==LT48_OK){raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);st=lt48_compact_decode(raw,used,&changed);}
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,count,1,0,output);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*output))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK&&*output){lt48_compiled_wave_close(*output);*output=NULL;}
 lt48_compact_wave_close(changed);free(raw);return st;
}
#endif
