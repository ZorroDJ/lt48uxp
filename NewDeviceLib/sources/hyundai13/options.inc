/* Authored configuration ownership. Original procedures remain user supplied. */
static lt48_status hy13_read_configuration(small_session*s,unsigned char raw[16]){
 unsigned m=s->r->hy13->m;size_t n=lt48_small_eprom_hyundai13_configuration_size(s->r);
 s->mode=1;lt48_status st=init(s,NULL,NULL);
 if(m==174||m==175){
  for(unsigned j=0;!st&&j<(m==175?16u:1u);j++){if(m==174)raw[j]=(unsigned char)st62_field(s,2,6);else for(size_t q=0;q<s->configuration_count;q++)if(s->configuration[q].object_id==-2&&s->configuration[q].category==2&&s->configuration[q].field==j)raw[j]=(unsigned char)s->configuration[q].value;}
  if(m==174)raw[1]=0;
 }else for(unsigned j=0;!st&&j<(m<=169?2u:1u);j++){
  uint32_t a=m<=169?0x707e + j:0xff0;
  for(unsigned k=0;k<4;k++)s->buffers[1][k]=(unsigned char)(a>>(8*k));
  st=run(s,8);if(!st)raw[j]=s->buffers[2][0];
 }
 if(m==170||m==171)raw[1]=0;
 (void)n;lt48_status reset=run(s,s->r->reset_op);return st==LT48_OK?reset:st;
}
static lt48_status hy13_configuration(small_session*s,unsigned secure,unsigned char*raw,lt48_small_eprom_result*out){
 unsigned m=s->r->hy13->m;size_t n=lt48_small_eprom_hyundai13_configuration_size(s->r);
 unsigned char before[16]={0},after[16]={0},wanted[16]={0};lt48_status st=hy13_read_configuration(s,before);
 memcpy(wanted,before,n);
 if(m==175){for(unsigned j=0;j<16;j++)wanted[j]|=s->r->hy13_extended[j];}
 else {uint16_t set=secure?(m<=169?1:m<=171?4:(uint16_t)s->r->source_options.number_values[6]):s->r->hy13_set;wanted[0]|=set&255;wanted[1]|=set>>8;}
 if(!st&&memcmp(wanted,before,n)){
  if(m<=169){static const unsigned fields[]={11,2,3,12,13,10};st62_set_field(s,1,8,wanted[0]&1);for(unsigned bit=0;bit<6;bit++)st62_set_field(s,0,fields[bit],!!(wanted[1]&(1u<<bit)));}
  else if(m<=171){st62_set_field(s,0,m==170?1:2,!!(wanted[0]&(m==170?1:2)));st62_set_field(s,0,3,!!(wanted[0]&4));}
  else if(m==174)st62_set_field(s,2,6,wanted[0]);
  else for(size_t q=0;q<s->configuration_count;q++)if(s->configuration[q].object_id==-2&&s->configuration[q].category==2&&s->configuration[q].field<16)s->configuration[q].value=wanted[s->configuration[q].field];
  s->mode=8;st=init(s,NULL,NULL);if(!st)st=run(s,m>=170&&m<=171&&!(wanted[0]&4)?13:11);
  lt48_status reset=run(s,s->r->reset_op);if(!st)st=reset;
  if(!st)st=run(s,1);
  if(!st)st=lt48_runtime_transport_session_flush(s->wire);
  if(!st)st=run(s,0);
  if(!st)st=hy13_read_configuration(s,after);
  if(!st&&memcmp(after,wanted,n)){out->mismatch_count++;for(size_t j=0;j<n;j++)if(after[j]!=wanted[j]){out->first_mismatch=j;break;}st=LT48_DEVICE_MISMATCH;}
 }else memcpy(after,before,n);
 if(!st){memcpy(raw,after,n);out->completed_bytes=n;}return st;
}
