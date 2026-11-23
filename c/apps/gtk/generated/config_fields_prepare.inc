/* Generated; edit tools/configuration_profiles. */
    if(status==LT48_OK && !strcmp(operation_profile,"at89-c5114-nine-operation")){
        lt48_c5114_resources *r=NULL;status=lt48_c5114_open(installation,work->device_index,&r);
        unsigned fields=6,flags=5;
        unsigned char lock=(unsigned char)(work->mcu51_protection?work->mcu51_protection:1);
        if(work->operation==LT48_OPERATION_CONFIGURATION){expected=work->c5114_configuration[0]?work->c5114_configuration:NULL;expected_size=expected?fields+1:0;}
        if(work->operation==LT48_OPERATION_SECURE){expected=&lock;expected_size=1;if(work->mcu51_protection>2)status=LT48_INVALID_ARGUMENT;}
        if(status==LT48_OK)status=lt48_c5114_validate(r,work->operation,work->start,work->length,expected,expected_size);
        if(status==LT48_OK && (work->margin_choice>3 || work->margin_type>1))status=LT48_INVALID_ARGUMENT;
        if(status==LT48_OK){
            GString *m=g_string_new(NULL);g_string_append_printf(m,"READY TO %s — %s · %u bytes\nAT89C5114: 4096-byte code image. Program erases first; Overprogram uses the source page procedure without whole-chip erase. Erase unlocks and preserves the five configuration flags. Configuration is separate. Use the original 20-SO adapter. Hardware qualification pending.",lt48_operation_kind_name(work->operation),selector,work->length);
            if(work->operation==LT48_OPERATION_CONFIGURATION){unsigned mask=work->c5114_configuration[0];g_string_append(m,mask?"\nSelected changes:":"\nRead current settings.");for(unsigned i=0;i<fields;i++)if(mask&(1u<<i))g_string_append_printf(m,i>flags?"\n%s: %02X":"\n%s: %u",c5114_label(flags,i),work->c5114_configuration[i+1]);}
            if(work->operation==LT48_OPERATION_SECURE)g_string_append_printf(m,"\nSelected lock level: %u; stronger existing locks are preserved.",lock);
            result->message=g_string_free(m,FALSE);
        }
        lt48_c5114_close(r);goto operation_prepare_done;
    }

    if(status==LT48_OK && !strcmp(operation_profile,"at89-c5132-nine-operation")){
        lt48_c5132_resources *r=NULL;status=lt48_c5132_open(installation,work->device_index,&r);
        unsigned fields=5,flags=2;
        unsigned char lock=(unsigned char)(work->mcu51_protection?work->mcu51_protection:1);
        if(work->operation==LT48_OPERATION_CONFIGURATION){expected=work->c5132_configuration[0]?work->c5132_configuration:NULL;expected_size=expected?fields+1:0;}
        if(work->operation==LT48_OPERATION_SECURE){expected=&lock;expected_size=1;if(work->mcu51_protection>3)status=LT48_INVALID_ARGUMENT;}
        if(status==LT48_OK)status=lt48_c5132_validate(r,work->operation,work->start,work->length,expected,expected_size);
        if(status==LT48_OK && (work->margin_choice>3 || work->margin_type>1))status=LT48_INVALID_ARGUMENT;
        if(status==LT48_OK){
            GString *m=g_string_new(NULL);g_string_append_printf(m,"READY TO %s — %s · %u bytes\nAT89C5132: 65536-byte code plus 4096-byte boot image. Program erases both areas first; this removes any factory bootloader. Overprogram uses the source page procedure without whole-chip erase. Erase unlocks and preserves the extra row and non-security settings. Configuration is separate. Use the original 80-pin adapter. Hardware qualification pending.",lt48_operation_kind_name(work->operation),selector,work->length);
            if(work->operation==LT48_OPERATION_CONFIGURATION){unsigned mask=work->c5132_configuration[0];g_string_append(m,mask?"\nSelected changes:":"\nRead current settings.");for(unsigned i=0;i<fields;i++)if(mask&(1u<<i))g_string_append_printf(m,i>flags?"\n%s: %02X":"\n%s: %u",c5132_label(flags,i),work->c5132_configuration[i+1]);}
            if(work->operation==LT48_OPERATION_SECURE)g_string_append_printf(m,"\nSelected lock level: %u; stronger existing locks are preserved.",lock);
            result->message=g_string_free(m,FALSE);
        }
        lt48_c5132_close(r);goto operation_prepare_done;
    }

    if(status==LT48_OK && !strcmp(operation_profile,"philips-lpc-eight-operation")){
        lt48_lpc_resources *r=NULL;status=lt48_lpc_open(installation,work->device_index,&r);
        unsigned fields=7,flags=6;
        unsigned char lock=(unsigned char)(work->mcu51_protection?work->mcu51_protection:1);
        if(work->operation==LT48_OPERATION_CONFIGURATION){expected=work->lpc_configuration[0]?work->lpc_configuration:NULL;expected_size=expected?fields+1:0;}
        if(work->operation==LT48_OPERATION_SECURE){expected=&lock;expected_size=1;if(work->mcu51_protection>2)status=LT48_INVALID_ARGUMENT;}
        if(status==LT48_OK)status=lt48_lpc_validate(r,work->operation,work->start,work->length,expected,expected_size);
        if(status==LT48_OK && (work->margin_choice>3 || work->margin_type>1))status=LT48_INVALID_ARGUMENT;
        if(status==LT48_OK){
            GString *m=g_string_new(NULL);g_string_append_printf(m,"READY TO %s — %s · %u bytes\nPhilips/NXP LPC OTP: code plus 32 customer bytes at FCE0–FCFF; unused image addresses must be FF. Program does not erase. Settings are preserved; configuration and security are separate. Use the adapter required by the selected source definition. Hardware qualification pending.",lt48_operation_kind_name(work->operation),selector,work->length);
            if(work->operation==LT48_OPERATION_CONFIGURATION){unsigned mask=work->lpc_configuration[0];g_string_append(m,mask?"\nSelected changes:":"\nRead current settings.");for(unsigned i=0;i<fields;i++)if(mask&(1u<<i))g_string_append_printf(m,i>flags?"\n%s: %02X":"\n%s: %u",lpc_label(flags,i),work->lpc_configuration[i+1]);}
            if(work->operation==LT48_OPERATION_SECURE)g_string_append_printf(m,"\nSelected lock level: %u; stronger existing locks are preserved.",lock);
            result->message=g_string_free(m,FALSE);
        }
        lt48_lpc_close(r);goto operation_prepare_done;
    }

