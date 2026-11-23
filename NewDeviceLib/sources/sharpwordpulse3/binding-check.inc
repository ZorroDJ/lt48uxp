/* Bind the source12V slope and per-sector worker without relaxed pin defaults. */
static lt48_status sharpwordpulse3_check(const lt48_gaaf_serial_resources*r){
 if(!r->sharpwordpulse3)return LT48_OK;
 if(r->nominal!=330||r->variable_count!=6||r->address_count!=4||r->legacy_config_mask||r->configuration_bits||r->legacy_erase_applet!=16)return LT48_SAFETY_REJECTED;
 if(r->variables[0].variable_id!=0||r->variables[0].channel!=0||r->variables[1].variable_id!=7||r->variables[1].value!=310||r->variables[1].channel!=2||r->variables[2].variable_id!=6||r->variables[2].value!=1200||r->variables[2].channel!=2||r->variables[3].variable_id!=14||r->variables[3].value!=0||r->variables[3].channel!=200||r->variables[4].value!=8||r->variables[5].value!=39)return LT48_SAFETY_REJECTED;
 for(unsigned op=0;op<LT48_GAAF_OPERATIONS;op++)if(r->waves[op]&&lt48_compiled_wave_defaulted_pin_count(r->waves[op]))return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
