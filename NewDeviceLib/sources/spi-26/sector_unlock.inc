/* Authored compact sequence: F3 unprotect-all through existing MCU role25,
 * followed by the same 50ms settling delay as the original erase entry.
 * No erase command and no security-region lock command is included. */
static const unsigned char sv_unlock[]={
 40,0, /* compact payload length */
 0x40,0x17,0x04,0xf3,0,0,0, 0x3d,0x19, 0x39,0x05,
 0x11,0x11,0, 0x11,0x09,0, 0x3e,0x17,0,
 0x3b,0xff, 0x7c,0x03, 0x39,0,
 0x32,0x50,0xc3,0,0
};
static lt48_status sv_compile_unlock(lt48_addtab_spi_resources*r){
 lt48_compact_wave*w=NULL;unsigned char code[sizeof(sv_unlock)];
 memcpy(code,sv_unlock,sizeof(code));code[0]=(unsigned char)(sizeof(code)-2);code[1]=0;
 lt48_status st=lt48_compact_decode(code,sizeof(code),&w);
 if(st==LT48_OK)st=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,
  LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[12]);
 lt48_compact_wave_close(w);return st;
}
