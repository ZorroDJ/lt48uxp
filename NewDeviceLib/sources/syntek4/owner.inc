/* Named choices3/4/5 are a separate six-bit OTP option word. */
static unsigned sy4_options(small_session*s){return (st62_field(s,1,3)&3)|((st62_field(s,1,4)&3)<<2)|((st62_field(s,1,5)&3)<<4);}
static lt48_status sy4_read_options(small_session*s){s->mode=1;lt48_status st=init(s,NULL,NULL);lt48_status reset=run(s,3);return st?st:reset;}
static lt48_status sy4_configuration(small_session*s,unsigned char*raw,lt48_small_eprom_result*out){
 lt48_status st=sy4_read_options(s);unsigned before=sy4_options(s),wanted=before&~s->r->sy4_clear;
 if(!st&&wanted!=before){for(unsigned f=0;f<3;f++)st62_set_field(s,1,3+f,(wanted>>(2*f))&3);s->mode=2;st=init(s,NULL,NULL);if(!st)st=run(s,3);if(!st)st=sy4_read_options(s);if(!st&&sy4_options(s)!=wanted){out->mismatch_count++;st=LT48_DEVICE_MISMATCH;}}
 if(!st){raw[0]=(unsigned char)wanted;raw[1]=0;out->completed_bytes=2;}return st;
}
