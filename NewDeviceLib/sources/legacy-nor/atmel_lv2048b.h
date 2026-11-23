/* Authored B-revision binding. Manufacturer 3279A marks chip pins 12/47 NC.
 * Omit only exact RESET/BYTE instructions, including VID-to-RESET routing.
 * Original command sequences, branch structure and delays remain imported. */
static lt48_status atmel_lv2048b_compact(const lt48_wave_code *code,
 unsigned definition,unsigned operation,unsigned char **copy,size_t *size) {
 if(definition!=0xfe||(operation!=2&&operation!=3))return LT48_OK;
 const char *expected=operation==2?
  "8d4f54b03d592aa8d5928951d0be394cba44578a37a0e261e4efba175fe31a20":
  "394af8dd00c25804ca71a598bd8880785a61b0be00e9cf3d614dfbb250b8c216";
 unsigned char digest[32];char hex[65];lt48_sha256_context context;
 lt48_sha256_init(&context);lt48_sha256_update(&context,code->data,code->data_size);lt48_sha256_final(&context,digest);
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 const unsigned id_cuts[]={2,8},init_cuts[]={27,30,43,79,156};
 const unsigned *cuts=operation==2?id_cuts:init_cuts;unsigned count=operation==2?2:5;
 size_t original=operation==2?59:254;
 if(*copy||strcmp(hex,expected)||code->data_size!=original||
  (code->data[0]+256u*code->data[1])!=original-2)return LT48_SAFETY_REJECTED;
 *size=original-3*count;*copy=malloc(*size);if(!*copy)return LT48_NO_MEMORY;
 size_t src=0,dst=0;
 for(unsigned i=0;i<count;i++) {
  memcpy(*copy+dst,code->data+src,cuts[i]-src);dst+=cuts[i]-src;src=cuts[i]+3;
 }
 memcpy(*copy+dst,code->data+src,original-src);
 (*copy)[0]=(unsigned char)(*size-2);(*copy)[1]=(unsigned char)((*size-2)>>8);
 return LT48_OK;
}
