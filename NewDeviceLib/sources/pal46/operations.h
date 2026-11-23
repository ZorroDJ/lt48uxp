/* Authored MMI 1983 PAL20/PAL24 corrections, scoped by GBB digest + D/M/P.
 * Amend decoded instruction boundaries; never patch offsets inside operands.
 * Source pins, branch structure, addressing and capture routing are retained. */
#ifndef LT48_PAL46_OPERATIONS_H
#define LT48_PAL46_OPERATIONS_H
static lt48_status pal46_amend_wave(unsigned op,const unsigned char *raw,size_t size,
                                  unsigned char **out,size_t *out_size) {
 *out=NULL;*out_size=0;
 if(op!=5 && op!=7)return LT48_NOT_FOUND;
 lt48_compact_wave *wave=NULL;lt48_status st=lt48_compact_decode(raw,size,&wave);
 if(st!=LT48_OK)return st;
 unsigned char bytes[1024];size_t n=2;unsigned pulses=0;
#define ADD(...) do{const unsigned char a[]={__VA_ARGS__};if(n+sizeof a>sizeof bytes){st=LT48_FORMAT_ERROR;goto done;}memcpy(bytes+n,a,sizeof a);n+=sizeof a;}while(0)
 for(size_t i=0;i<lt48_compact_instruction_count(wave);i++){
  lt48_compact_instruction q;st=lt48_compact_get_instruction(wave,i,&q);if(st!=LT48_OK)goto done;
  if(op==5 && q.opcode==0 && q.operands[0]==15){ADD(2,1,13);continue;}
  if(op==7 && q.opcode==0 && (q.operands[0]==13||q.operands[0]==15)){ADD(2,1,42);continue;}
  if(op==5 && q.opcode==9){
   /* Each output pulse gets its own <=60 us VCC high phase. No long
    * settling delay at VIHH: source/controller packet edges add real delay. */
   ADD(2,1,15);pulses++;
  }
  if(op==7 && q.opcode==3 && q.operands[0]==0){ADD(2,1,42,3,0,42,0x31,44);continue;}
  if(op==7 && q.opcode==0x2d){ADD(0x2d,q.operands[0],43,q.operands[2],q.operands[3],0x31,44);continue;}
  if(op==5 && q.opcode==0x2b){ADD(0x31,44);} /* LR/address setup before clock. */
  if(n+1+q.operand_size>sizeof bytes){st=LT48_FORMAT_ERROR;goto done;}
  bytes[n++]=q.opcode;memcpy(bytes+n,q.operands,q.operand_size);n+=q.operand_size;
  if(op==5 && q.opcode==0x0a){ADD(2,1,13,0x31,44);} /* 5V,100us off; <20% duty. */
 }
 if(op==7){ADD(2,1,13);} /* Return to nominal after security pulses. */
 if(op==5 && pulses!=2){st=LT48_FORMAT_ERROR;goto done;}
 bytes[0]=(unsigned char)(n-2);bytes[1]=(unsigned char)((n-2)>>8);
 *out=malloc(n);if(!*out){st=LT48_NO_MEMORY;goto done;}memcpy(*out,bytes,n);*out_size=n;
 done:lt48_compact_wave_close(wave);
#undef ADD
 return st;
}
#endif
