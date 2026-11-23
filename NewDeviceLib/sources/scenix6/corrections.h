/* Authored, source-bound preservation fixes. Keep original waves for audit. */
static lt48_status sx6_corrected(const lt48_compiled_wave*source,int erase,int sx28,lt48_compiled_wave**out){
 size_t n=0;const uint16_t*w=lt48_compiled_wave_words(source,&n);if(!w||!n||n>8192)return LT48_FORMAT_ERROR;
 uint16_t*copy=malloc((n+1)*sizeof *copy);if(!copy)return LT48_NO_MEMORY;
 size_t found=SIZE_MAX,old=0;unsigned hits=0;
 for(size_t i=0;i+6<=n;i++){
  int match=erase?(sx28?(w[i]==0x36&&w[i+1]==2&&w[i+2]==8&&w[i+3]==0xfff&&w[i+4]==0&&w[i+5]==4):(w[i]==0x28&&w[i+1]==8&&w[i+2]==4&&w[i+3]==0xbff&&w[i+4]==0)):(w[i]==0x36&&w[i+1]==1&&w[i+2]==2&&w[i+3]==0xbff&&w[i+4]==0&&w[i+5]==4);
  if(match){found=i;old=erase&&!sx28?5:6;hits++;}
 }
 if(hits!=1){free(copy);return LT48_SAFETY_REJECTED;}
 memcpy(copy,w,n*sizeof *copy);
 if(erase){const uint16_t keep[]={0x34,8,0,8,0,4};memmove(copy+found+6,copy+found+old,(n-found-old)*sizeof *copy);memcpy(copy+found,keep,sizeof keep);n+=6-old;}
 else copy[found+3]=0xfff;
 copy[0]=(uint16_t)(n-1);
 lt48_status st=lt48_compiled_host_words_internal(copy,n,out);free(copy);return st;
}
