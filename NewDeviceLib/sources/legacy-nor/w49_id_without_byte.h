/* Authored exact W49F201T/W49L201T ID correction. Both are x16-only:
 * manufacturer pinouts mark TSOP pin 47 NC. Remove only the dangling
 * logical BYTE=18 assignment; keep every real pin, delay and ID command. */
static lt48_status w49_id_without_byte(const lt48_wave_code *code,
 unsigned definition,unsigned operation,unsigned char **copy,size_t *size) {
 if((definition!=0xa7 && definition!=0xa8)||operation!=2)return LT48_OK;
 unsigned char digest[32];char hex[65];lt48_sha256_context context;
 lt48_sha256_init(&context);lt48_sha256_update(&context,code->data,code->data_size);lt48_sha256_final(&context,digest);
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,"45f389c8ad8a2805e00ceecd92e3be43cd7ac41aa28ddd491dc2cc2fd9ca056b") ||
    code->data_size!=57 || code->data[0]!=55 || code->data[1]!=0 ||
    memcmp(code->data+14,"\x11\x12\x01",3) || *copy)return LT48_SAFETY_REJECTED;
 *size=54;*copy=malloc(*size);if(!*copy)return LT48_NO_MEMORY;
 memcpy(*copy,code->data,14);memcpy(*copy+14,code->data+17,40);
 (*copy)[0]=52;return LT48_OK;
}
