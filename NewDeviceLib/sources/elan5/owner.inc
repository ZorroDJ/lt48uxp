/* Original Read-mode INIT loads separate option words through role18. */
static lt48_status el5_read_options(small_session*s,uint16_t words[2]){
 s->mode=1;lt48_status st=init(s,NULL,NULL);if(!st)for(unsigned j=0;j<s->r->el5->cfg_count;j++)words[j]=(uint16_t)st62_field(s,2,s->r->el5->fields[j])&0x1fff;
 lt48_status reset=run(s,5);return st?st:reset;
}
static lt48_status el5_configuration(small_session*s,unsigned char*raw,lt48_small_eprom_result*out){
 uint16_t before[2]={0},want[2]={0},after[2]={0};lt48_status st=el5_read_options(s,before);unsigned changed=0;
 for(unsigned j=0;j<s->r->el5->cfg_count;j++){want[j]=before[j]&~s->r->el5_clear[j];changed|=want[j]!=before[j];st62_set_field(s,2,s->r->el5->fields[j],want[j]);}
 if(!st&&changed){s->mode=8;st=init(s,NULL,NULL);if(!st)st=run(s,9);lt48_status reset=run(s,5);if(!st)st=reset;if(!st)st=el5_read_options(s,after);if(!st&&memcmp(after,want,sizeof after)){out->mismatch_count++;st=LT48_DEVICE_MISMATCH;}}
 if(!st){for(unsigned j=0;j<s->r->el5->cfg_count;j++){raw[2*j]=(unsigned char)want[j];raw[2*j+1]=(unsigned char)(want[j]>>8);}out->completed_bytes=s->r->el5->cfg_count*2;}return st;
}
