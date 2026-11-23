/* Authored source-locked SpiStack framing correction. Source C2 frames end
 * with a repeated CS-low instead of CS-high; keep distinct die commands apart. */
static lt48_status gw_correct_die_frame(unsigned char*b,size_t n){
 unsigned found=0;for(size_t i=0;i+19<=n;i++){
  static const unsigned char prefix[]={0x3b,0xc2,0x32,30,0,0,0,0x3b,0xc0,0x3b};
  static const unsigned char suffix[]={0x32,30,0,0,0,0x11,0x11,0};
  if(!memcmp(b+i,prefix,sizeof prefix)&&b[i+10]<=1&&!memcmp(b+i+11,suffix,sizeof suffix)){b[i+18]=1;found++;}
 }
 return found==1?LT48_OK:LT48_SAFETY_REJECTED;
}
static lt48_status gw_build_die_select(lt48_addtab_spi_resources*r){
 for(unsigned die=0;die<2;die++){
  /* Explicit CS-high boundary, shift C2 + die, then latch with CS-high. */
  unsigned char code[]={0x3b,0xd4,0x3b,8,0x3b,0xd8,0x3b,0,0x11,17,1,
   0x11,17,0,0x3b,0xc0,0x3b,0xc2,0x32,30,0,0,0,0x3b,0xc0,0x3b,0,
   0x32,30,0,0,0,0x11,17,1};
  code[26]=(unsigned char)die;unsigned char raw[sizeof code+2];raw[0]=(unsigned char)sizeof code;raw[1]=0;memcpy(raw+2,code,sizeof code);lt48_compact_wave*w=NULL;
  lt48_status st=lt48_compact_decode(raw,sizeof raw,&w);
  if(st==LT48_OK)st=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[13+die]);
  lt48_compact_wave_close(w);if(st!=LT48_OK)return st;
 }
 return LT48_OK;
}
