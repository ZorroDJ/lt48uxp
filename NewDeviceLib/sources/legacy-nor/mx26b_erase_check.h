/* Authored erased-byte checker; vendor resource remains BYO and unchanged.
 * Manufacturer erase algorithm verifies each byte as FF. Original CJNE uses
 * unmapped SFR FFh; this owned replacement uses immediate FFh instead.
 */
static lt48_status mx26b_erase_check(lt48_gaaf_serial_resources *r) {
 if(r->definition<0x6a||r->definition>0x6c)return LT48_OK;
 lt48_wave_code *a=&r->applets[15];unsigned char digest[32];char hex[65];lt48_sha256_context sha;
 if(a->load_address!=0x5265||a->data_size!=133)return LT48_SAFETY_REJECTED;
 lt48_sha256_init(&sha);lt48_sha256_update(&sha,a->data,a->data_size);lt48_sha256_final(&sha,digest);
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,"d7b4ee376a80c3fc398d4a5a94d56d4b77f08ec0c009afce9201cb901bd38e76")||
    memcmp(a->data+0x41,"\xb5\xff\x27",3))return LT48_SAFETY_REJECTED;
 unsigned char *owned=realloc(r->idle,r->idle_size+a->data_size);if(!owned)return LT48_NO_MEMORY;
 r->idle=owned;unsigned char *code=owned+r->idle_size;memcpy(code,a->data,a->data_size);
 code[0x41]=0xb4;a->data=code;return LT48_OK;
}
