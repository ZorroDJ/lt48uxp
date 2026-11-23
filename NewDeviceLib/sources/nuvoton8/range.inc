/* NUC029 reads cache a complete aligned word. Prefix reads also work with
 * the byte-stream N79/NUC126 workers; keep this policy finite to this cohort. */
static lt48_status nv8_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;n79_address(s,a);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);size_t done=0;
 while(!st&&done<n){unsigned at=a+(unsigned)done,off=0,k=n79_region(s->r,at,&off);if(k>=s->r->regions){st=LT48_FORMAT_ERROR;break;}size_t take=n-done;if(take>off+s->r->region_length[k]-at)take=off+s->r->region_length[k]-at;unsigned skip=writing?0:(at-off)%4;size_t transfer=skip+take;unsigned char*scratch=writing?NULL:malloc(transfer);if(!writing&&!scratch){st=LT48_NO_MEMORY;break;}
 n79_address(s,at-skip);st=run_s8253(s,writing?8:5);for(size_t q=0;!st&&q<transfer;){size_t chunk=transfer-q;if(chunk>4096)chunk=4096;st=block_s8253(s,writing,writing?(unsigned char*)im+done+q:scratch+q,chunk);q+=chunk;}if(!st)st=stop_s8253(s);if(!st&&!writing)memcpy(out+done,scratch+skip,take);free(scratch);done+=take;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
