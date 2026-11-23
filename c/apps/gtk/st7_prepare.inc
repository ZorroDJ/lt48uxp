if(status==LT48_OK&&!strcmp(operation_profile,"st7twelve-nine-operation")){
 lt48_st7_resources*r=NULL;status=lt48_st7_open(installation,work->device_index,&r);
 const unsigned char*in=expected;size_t size=expected_size;
 if(work->operation==LT48_OPERATION_CONFIGURATION){in=work->rb2_configuration[0]?work->rb2_configuration:NULL;size=in?lt48_st7_option_bytes(r)+1:0;}
 if(status==LT48_OK)status=lt48_st7_validate(r,work->operation,work->start,work->length,in,size,margin_values[work->margin_choice<4?work->margin_choice:0],work->margin_type);
 if(status==LT48_OK)result->message=g_strdup_printf("READY TO %s — %s\nST7 source-audited 64-KiB sparse image: code and fitted EEPROM regions; holes must be FF. Options are separate and preserved. Read protection blocks code access; use Erase to clear it. Erase also destroys calibration that the chip clears on protection removal. Secure enables read protection after reading code; irreversible write protection is unavailable. Original package adapter required. Hardware testing pending.",lt48_operation_kind_name(work->operation),selector);
 lt48_st7_close(r);goto operation_prepare_done;
}
