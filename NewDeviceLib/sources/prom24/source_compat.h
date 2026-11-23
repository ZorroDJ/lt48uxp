/* Exact M82/P4d zero-filled lookup compatibility, not a guessed pin map. */
static int prom24_zero_strobe(const lt48_device_spec*s){
 return prom24_binding(s)&&s->family_code==0x82&&s->pinmap==0x4d;
}
static lt48_status prom24_check_defaults(const lt48_device_spec*s,unsigned op,const lt48_compiled_wave*w){
 size_t expected=prom24_zero_strobe(s)?(op==2||op==7?1:op==9?2:0):0;
 if(lt48_compiled_wave_defaulted_pin_count(w)!=expected)return LT48_SAFETY_REJECTED;
 for(size_t i=0;i<expected;i++){
  lt48_defaulted_logical_pin p;
  if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK||p.logical_pin!=21||
   p.compact_opcode!=(op==2?0x11:0x2b)||p.classification!=LT48_DEFAULTED_PIN_SOURCE_ZERO_STROBE||
   p.compact_offset!=(op==2?0x16:op==7?0x13:i?0x36:0x15))return LT48_SAFETY_REJECTED;
 }return LT48_OK;
}
