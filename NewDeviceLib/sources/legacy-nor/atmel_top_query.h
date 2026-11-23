/* Authored, exact-source correction for the byte-mode AT49BV008AT lock query.
 * Atmel 1049I p4 specifies byte address FC002 (8192AT uses word 7E002).
 * Source W12D/W130 instead double the shared geometry-derived value.
 * Retain the command/response sequence; replace only address calculation.
 * The containing REZ hash and D/M/P tuple are checked by gaaf_open. */
static lt48_status atmel_top_query(const lt48_wave_code *code,
 unsigned definition,unsigned operation,unsigned char **owned,size_t *length) {
 if(definition!=0xb5||(operation!=8&&operation!=9))return LT48_OK;
 static const unsigned char expected[]={0x41,0x0d,0x04,0x14,0x4d,0x02,0x0d,0x02,0,0,0};
 static const unsigned char replacement[]={0x40,0x0d,0x04,0x02,0xc0,0x0f,0};
 size_t n=operation==8?75u:70u;
 if(code->wave_type||code->data_size!=n||*owned||memcmp(code->data+6,expected,sizeof expected))return LT48_SAFETY_REJECTED;
 unsigned char *p=malloc(n-4);if(!p)return LT48_NO_MEMORY;
 memcpy(p,code->data,6);memcpy(p+6,replacement,sizeof replacement);
 memcpy(p+13,code->data+17,n-17);p[0]=(unsigned char)(n-6);p[1]=0;
 *owned=p;*length=n-4;return LT48_OK;
}
