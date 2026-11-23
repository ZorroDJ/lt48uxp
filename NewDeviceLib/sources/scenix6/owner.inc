/* Authored ownership of code-independent FUSE and FUSEX configuration. */
static lt48_status sx6_read_options(small_session*s,uint16_t words[2]){
 s->mode=1;lt48_status st=init(s,NULL,NULL);
 if(!st){words[0]=(uint16_t)st62_field(s,2,4)&0xfff;words[1]=(uint16_t)st62_field(s,2,5)&0xfff;}
 lt48_status reset=run(s,s->r->reset_op);return st?st:reset;
}
static lt48_status sx6_configuration(small_session*s,lt48_operation_kind op,unsigned char*raw,lt48_small_eprom_result*out){
 uint16_t before[2]={0},after[2]={0},wanted[2]={0};lt48_status st=sx6_read_options(s,before);
 wanted[0]=before[0];wanted[1]=before[1];
 if(op==LT48_OPERATION_ERASE)wanted[0]=0xfff;
 else {wanted[0]&=~(op==LT48_OPERATION_SECURE?8:s->r->sx6_clear[0]);wanted[1]&=~(op==LT48_OPERATION_SECURE?0:s->r->sx6_clear[1]);}
 if(!st&&(op==LT48_OPERATION_ERASE||memcmp(wanted,before,sizeof wanted))){
  if(op!=LT48_OPERATION_ERASE&&!(before[0]&8))return LT48_SAFETY_REJECTED;
  st62_set_field(s,2,4,0xfff&~(op==LT48_OPERATION_SECURE?8:s->r->sx6_clear[0]));st62_set_field(s,2,5,wanted[1]);
  s->mode=op==LT48_OPERATION_ERASE?7:8;st=init(s,NULL,NULL);
  if(!st)st=run(s,op==LT48_OPERATION_ERASE?30:s->r->sx6->m<137?31:11);
  lt48_status reset=run(s,s->r->reset_op);if(!st)st=reset;
  if(!st)st=run(s,1);
  if(!st)st=lt48_runtime_transport_session_flush(s->wire);
  if(!st)st=run(s,0);
  if(!st)st=sx6_read_options(s,after);
  /* FUSE/code/ID are scrambled after CP is enabled; preserve the raw result.
   * FUSEX remains independently readable and must match exactly. */
  if(!st&&(after[1]!=wanted[1]||((wanted[0]&8)&&after[0]!=wanted[0])||(!(wanted[0]&8)&&after[0]==before[0]))){out->mismatch_count++;st=LT48_DEVICE_MISMATCH;}
 }else memcpy(after,before,sizeof after);
 if(!st&&op==LT48_OPERATION_ERASE)st=cycle(s,LT48_OPERATION_BLANK_CHECK,0,s->r->capacity,NULL,NULL,s->r->nominal,NULL,NULL,out);
 if(!st){for(unsigned j=0;j<2;j++){raw[j*2]=(unsigned char)after[j];raw[j*2+1]=(unsigned char)(after[j]>>8);}out->completed_bytes=op==LT48_OPERATION_ERASE?s->r->capacity:4;}
 return st;
}
