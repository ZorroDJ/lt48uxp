if(status==LT48_OK&&!strcmp(operation_profile,"tms87two-six-operation")){
 lt48_tms87_resources*r=NULL;status=lt48_tms87_open(installation,work->device_index,&r);
 if(status==LT48_OK)status=lt48_tms87_validate(r,work->operation,work->start,work->length,expected,expected_size,margin_values[work->margin_choice<4?work->margin_choice:0],work->margin_type);
 if(status==LT48_OK)result->message=g_strdup_printf("READY TO %s — %s\nTMS87 multiplexed EPROM: whole-device byte images. Program requires blank EPROM; Over Program permits only 1-to-0 changes. Byte pairs use authored baseline-FPGA controls and verified reads, with independent original raw Read and margin checks. Windowed parts require UV erasure; electrical Erase and protection operations are unavailable. Original DIP-28 placement required. Hardware testing pending.",lt48_operation_kind_name(work->operation),selector);
 lt48_tms87_close(r);goto operation_prepare_done;
}
