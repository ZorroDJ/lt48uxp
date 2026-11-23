/* Own the original seven-bit option byte and independent protection checkbox. */
static lt48_status sg4_read_options(small_session*s,unsigned*value,unsigned*locked){s->mode=1;lt48_status st=init(s,NULL,NULL);if(!st){*value=st62_field(s,2,0);*locked=!!st62_field(s,0,6);if(*value>127)st=LT48_SAFETY_REJECTED;}lt48_status reset=run(s,3);return st?st:reset;}
static lt48_status sg4_write_options(small_session*s,unsigned value,unsigned locked){st62_set_field(s,2,0,value);st62_set_field(s,0,6,locked);s->mode=8;lt48_status st=init(s,NULL,NULL);if(!st)st=run(s,6);lt48_status reset=run(s,3);return st?st:reset;}
static lt48_status sg4_special(small_session*s,lt48_operation_kind op,unsigned char*raw,lt48_small_eprom_result*out){
 unsigned before=0,locked=0,after=0,after_lock=0;lt48_status st=sg4_read_options(s,&before,&locked);unsigned wanted=op==LT48_OPERATION_CONFIGURATION?(before&~s->r->sg4_mask)|s->r->sg4_value:before;
 if(!st&&op==LT48_OPERATION_CONFIGURATION&&wanted!=before&&locked)return LT48_SAFETY_REJECTED;
 if(!st&&op==LT48_OPERATION_ERASE){s->mode=7;st=init(s,NULL,NULL);if(!st)st=run(s,7);lt48_status reset=run(s,3);if(!st)st=reset;if(!st)st=sg4_write_options(s,before,0);}
 else if(!st&&((op==LT48_OPERATION_SECURE&&!locked)||(op==LT48_OPERATION_CONFIGURATION&&wanted!=before)))st=sg4_write_options(s,wanted,op==LT48_OPERATION_SECURE?1:locked);
 if(!st)st=sg4_read_options(s,&after,&after_lock);
 unsigned expected_lock=op==LT48_OPERATION_ERASE?0:op==LT48_OPERATION_SECURE?1:locked;
 if(!st&&(after!=wanted||after_lock!=expected_lock)){out->mismatch_count++;st=LT48_DEVICE_MISMATCH;}
 if(!st&&op==LT48_OPERATION_ERASE)st=cycle(s,LT48_OPERATION_BLANK_CHECK,0,s->r->capacity,NULL,NULL,s->r->nominal,NULL,NULL,out);
 if(!st){raw[0]=after;raw[1]=after_lock;out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?2:s->r->capacity;}return st;
}
