/* Authored source-bound status-lane correction. Vendor resources stay unchanged.
 * The original erase compact wave selects byte FF (upper lane). FE selects
 * DQ7..0 at the identical word address, matching the manufacturer status ABI.
 * M29F800A3 D4A/D4B and M28F800A2 D4C/D4D are covered.
 * Their respective manufacturer status tables specify DQ7..DQ0.
 */
static lt48_status m29f800_status_lane(const lt48_wave_code *code,
 unsigned definition,unsigned operation,unsigned char **copy) {
 if((definition<0x4a || definition>0x4d)||operation!=13)return LT48_OK;
 static const unsigned char prefix[]={0x5e,0,0x3b,0xc4,0x3b,0xff};
 unsigned char digest[32];char hex[65];lt48_sha256_context context;
 lt48_sha256_init(&context);lt48_sha256_update(&context,code->data,code->data_size);lt48_sha256_final(&context,digest);
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,"7fb82297a6fdeb64a7081a76e9f105af3957b7c134e26f0c8063c24c7da6db32")||
    code->data_size!=96||memcmp(code->data,prefix,sizeof prefix)||*copy)return LT48_SAFETY_REJECTED;
 *copy=malloc(code->data_size);if(!*copy)return LT48_NO_MEMORY;
 memcpy(*copy,code->data,code->data_size);(*copy)[5]=0xfe;return LT48_OK;
}
