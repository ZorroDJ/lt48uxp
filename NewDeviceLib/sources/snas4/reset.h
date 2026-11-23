/* ASIX RESET first inspects the host image at3FFF and updates a Protect checkbox.
 * Array-only UI has no checkbox. Retain the exact reset/rail/delay hardware tail. */
static lt48_status snas4_reset_compile(const lt48_wave_code*w,const lt48_pin_map*p,const lt48_compile_variable*v,size_t n,lt48_compiled_wave**out){
 if(w->data_size!=66||memcmp(w->data+49,"\x11\x0e\x00",3))return LT48_SAFETY_REJECTED;
 unsigned char b[19]={17,0};memcpy(b+2,w->data+49,17);
 lt48_compact_wave*c=NULL;lt48_status st=lt48_compact_decode(b,sizeof b,&c);
 if(!st)st=lt48_compact_compile_with_pin_policy(c,p,v,n,1,LT48_PIN_FALLBACK_STRICT,out);
 lt48_compact_wave_close(c);return st;
}
