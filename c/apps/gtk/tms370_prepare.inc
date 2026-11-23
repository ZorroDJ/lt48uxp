if(status==LT48_OK&&!strcmp(operation_profile,"tms370seven-six-operation")){
 lt48_tms370_resources*r=NULL;status=lt48_tms370_open(installation,work->device_index,&r);
 if(status==LT48_OK)status=lt48_tms370_validate(r,work->operation,work->start,work->length,expected,expected_size,margin_values[work->margin_choice<4?work->margin_choice:0],work->margin_type);
 if(status==LT48_OK)result->message=g_strdup_printf("READY TO %s — %s\nTMS370 source-audited 32-KiB sparse image. EPROM 6000–7FFF; fitted EEPROM 1F00–1FFF. Holes must be FF. EEPROM blank is 00; EPROM blank is FF. Program requires blank EPROM; Over Program permits only 1-to-0 EPROM changes. EEPROM can be rewritten. Electrical erase and protection operations unavailable. Original package adapter required. Hardware testing pending.",lt48_operation_kind_name(work->operation),selector);
 lt48_tms370_close(r);goto operation_prepare_done;
}
