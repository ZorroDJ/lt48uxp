/* Original C-series streams start at zero in each region. Execute complete
 * source regions and preserve unselected bytes instead of pretending the source
 * consumes a random address. All buffers remain bounded by the exact profile. */
static lt48_status cycle(as_session*s,unsigned mode,uint32_t start,size_t length,
 const unsigned char*image,unsigned char*read,uint16_t vcc,lt48_addtab_spi_result*out){
 if(s->r->closure->kind!=4)return cycle_source(s,mode,start,length,image,read,vcc,out);
 lt48_status st=LT48_OK;size_t done=0;
 for(size_t k=0;st==LT48_OK&&k<s->r->closure->segment_count;k++){
  const struct as_segment*p=s->r->closure->segments+k;
  uint32_t lo=start>p->image_offset?start:p->image_offset;
  uint32_t hi=start+(uint32_t)length<p->image_offset+p->length?start+(uint32_t)length:p->image_offset+p->length;
  if(lo>=hi)continue;
  size_t n=hi-lo,offset=lo-p->image_offset;
  unsigned char*whole=malloc(p->length);if(!whole)return LT48_NO_MEMORY;
  st=cycle_source(s,1,p->image_offset,p->length,NULL,whole,vcc,out);
  if(st==LT48_OK&&mode==1)memcpy(read+done,whole+offset,n);
  else if(st==LT48_OK&&mode==11){
   for(size_t i=0;i<n;i++)if(whole[offset+i]!=255){if(!out->mismatch_count)out->first_mismatch=lo+(uint32_t)i;out->mismatch_count++;}
   if(out->mismatch_count)st=LT48_DEVICE_MISMATCH;
   else if(offset==0&&n==p->length)st=cycle_source(s,11,p->image_offset,p->length,NULL,NULL,vcc,out);
  }else if(st==LT48_OK){
   memcpy(whole+offset,image+done,n);
   st=cycle_source(s,mode,p->image_offset,p->length,whole,NULL,vcc,out);
   if(st==LT48_OK&&mode==2){
    unsigned char*actual=malloc(p->length);
    if(!actual)st=LT48_NO_MEMORY;
    else{
     st=cycle_source(s,1,p->image_offset,p->length,NULL,actual,vcc,out);
     if(st==LT48_OK)for(size_t i=0;i<p->length;i++)if(actual[i]!=whole[i]){if(!out->mismatch_count)out->first_mismatch=p->image_offset+(uint32_t)i;out->mismatch_count++;}
     if(st==LT48_OK&&out->mismatch_count)st=LT48_DEVICE_MISMATCH;
     free(actual);
    }
   }
  }
  free(whole);if(st==LT48_OK)done+=n;
 }
 out->completed_bytes=done;return st;
}
