if(status==LT48_OK&&!strcmp(operation_profile,"lpc-flash-crc-eight-operation")){
 lt48_lpcflash_resources*r=NULL;status=lt48_lpcflash_open(installation,work->device_index,&r);
 unsigned fields=lt48_lpcflash_field_count(r);unsigned sectors=fields>=4?fields-4:0;
 if(!status)status=lt48_lpcflash_set_erase(r,work->lpcflash_sector_mask?work->lpcflash_sector_mask:(1u<<sectors)-1,work->lpcflash_preserve_isp);
 const unsigned char*input=work->image;size_t size=work->image_size;unsigned char lock=(unsigned char)(work->mcu51_protection?work->mcu51_protection:7);
 if(work->operation==LT48_OPERATION_CONFIGURATION){input=(work->lpcflash_configuration[0]||work->lpcflash_configuration[1]||work->lpcflash_configuration[2])?work->lpcflash_configuration:NULL;size=input?24:0;}
 if(work->operation==LT48_OPERATION_SECURE){input=&lock;size=1;}
 if(!status)status=lt48_lpcflash_validate(r,work->operation,work->start,work->length,input,size);
 if(!status&&(work->margin_choice>3||work->margin_type>1))status=LT48_INVALID_ARGUMENT;
 if(!status)result->message=g_strdup_printf("READY TO %s — %s · %u bytes\nP89LPC Flash: code Read unavailable; Read Configuration is available. Code Verify/Compare/Blank use sector CRCs, not byte readback. Full code images only. Program erases first. Unchecked settings preserved.\nErase: sector mask %04X, %s ISP loader. Use the source adapter mapping. Hardware qualification pending.",lt48_operation_kind_name(work->operation),selector,work->length,work->lpcflash_sector_mask?work->lpcflash_sector_mask:(1u<<sectors)-1,work->lpcflash_preserve_isp?"preserve":"erase including");
 if(!status&&work->operation==LT48_OPERATION_CONFIGURATION){GString*m=g_string_new(result->message);uint32_t mask=work->lpcflash_configuration[0]|(uint32_t)work->lpcflash_configuration[1]<<8|(uint32_t)work->lpcflash_configuration[2]<<16;g_string_append(m,mask?"\nSelected setting changes:":"\nRead current configuration.");for(unsigned i=0;i<fields;i++)if(mask&(1u<<i))g_string_append_printf(m,"\n%s: %02X",lpcflash_label(i),work->lpcflash_configuration[4+i]);g_free(result->message);result->message=g_string_free(m,FALSE);}
 if(!status&&work->operation==LT48_OPERATION_SECURE){char*m=g_strdup_printf("%s\nAdd security bit mask %u to every sector; existing bits preserved.",result->message,lock);g_free(result->message);result->message=m;}
 lt48_lpcflash_close(r);goto operation_prepare_done;
}
