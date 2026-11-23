/* Exact BYTE# zero-table lookups are resident no-ops, never guessed pins. */
static lt48_status sharpminato4_check(const lt48_gaaf_serial_resources*r){
 if(!r->sharpminato4)return LT48_OK;
 if(r->nominal!=300||r->variable_count!=6||r->address_count!=4||r->legacy_config_mask||r->configuration_bits)return LT48_SAFETY_REJECTED;
 if(r->variables[0].variable_id!=3||r->variables[0].channel!=0||r->variables[1].variable_id!=1||r->variables[1].value!=280||r->variables[1].channel!=2)return LT48_SAFETY_REJECTED;
 for(unsigned op=0;op<LT48_GAAF_OPERATIONS;op++)if(r->waves[op]){
  size_t n=lt48_compiled_wave_defaulted_pin_count(r->waves[op]);unsigned expected=op==2||op==3;
  if(n!=expected)return LT48_SAFETY_REJECTED;
  if(n){lt48_defaulted_logical_pin p;lt48_status st=lt48_compiled_wave_get_defaulted_pin(r->waves[op],0,&p);if(st)return st;if(p.logical_pin!=18||p.compact_opcode!=0x11||p.compact_offset!=(op==2?5u:30u)||p.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP)return LT48_SAFETY_REJECTED;}
 }
 return LT48_OK;
}
