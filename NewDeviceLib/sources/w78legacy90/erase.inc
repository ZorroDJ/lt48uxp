/* Exact-source correction: initialize the remaining-byte counter outside the
 * retry loop. The original resets it inside each iteration, repeating a
 * successful blank segment indefinitely. Original executable/compiler trials
 * and the authored counterpart are retained in the cohort evidence. */
static lt48_status legacy_erase_compile(const lt48_wave_code*w,const lt48_pin_map*p,const lt48_compile_variable*v,size_t nv,lt48_compiled_wave**out){
 static const unsigned char loop[]={0x62,0x0b,0x04,0,0,0,0},copy[]={0x49,0x09,0,0x01,0,0x04};
 unsigned char raw[203],digest[32];char hex[65];
 if(w->data_size!=sizeof raw)return LT48_SAFETY_REJECTED;
 lt48_sha256(w->data,w->data_size,digest);for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,"7f0ba8d2ee81cb07da1382283deff8856f3fceeeeb7d62e090afd82deb531dda")||memcmp(w->data+0x25,loop,sizeof loop)||memcmp(w->data+0x2c,copy,sizeof copy))return LT48_SAFETY_REJECTED;
 memcpy(raw,w->data,sizeof raw);memcpy(raw+0x25,copy,sizeof copy);memcpy(raw+0x2b,loop,sizeof loop);
 lt48_compact_wave*c=NULL;lt48_status st=lt48_compact_decode(raw,sizeof raw,&c);if(!st)st=lt48_compact_compile_with_pin_policy(c,p,v,nv,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,out);lt48_compact_wave_close(c);return st;
}
