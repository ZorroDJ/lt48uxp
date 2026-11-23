    if (status == LT48_OK && !strcmp(operation_profile,"lpc-flash-crc-eight-operation")) {
        lt48_lpcflash_resources *small = NULL;
        lt48_serial_page_result completed = {0};
        lt48_context *context = NULL;
        lt48_programmer *programmer = NULL;
        lt48_transport_info ready = {0};
        status = lt48_lpcflash_open(installation, request->device_index, &small);
        unsigned fields=lt48_lpcflash_field_count(small);
        if(status==LT48_OK)status=lt48_lpcflash_set_erase(small,request->lpcflash_sector_mask?request->lpcflash_sector_mask:(1u<<(fields-4))-1,request->lpcflash_preserve_isp);
        unsigned char config_result[20]={0},lock=(unsigned char)(request->mcu51_protection?request->mcu51_protection:7);
        const unsigned char *input=request->image;size_t input_size=request->image_size;
        if(request->operation==LT48_OPERATION_CONFIGURATION){input=(request->lpcflash_configuration[0]||request->lpcflash_configuration[1]||request->lpcflash_configuration[2])?request->lpcflash_configuration:NULL;input_size=input?24:0;}
        if(request->operation==LT48_OPERATION_SECURE){input=&lock;input_size=1;}
        if (status == LT48_OK && request->operation == LT48_OPERATION_READ) {
            result->read_bytes = g_malloc(request->length);
            result->read_start = request->start;
            result->read_size = request->length;
        }
        if (status == LT48_OK) status = lt48_context_create(&context);
        if (status == LT48_OK) status = lt48_programmer_open(context, work->bus, work->address, &programmer);
        if (status == LT48_OK) status = lt48_programmer_transport_initialize(programmer, work->timeout, &ready);
        if (status == LT48_OK) status = lt48_programmer_set_cancel_check(programmer, gio_cancelled, work->widgets->read_cancellable);
        if (status == LT48_OK) {
            g_mutex_lock(&work->widgets->progress_mutex);
            work->widgets->active_programmer = programmer;
            g_mutex_unlock(&work->widgets->progress_mutex);
            status = lt48_programmer_execute_lpcflash(programmer, small,
                request->operation, request->start, request->length,
                input, input_size, request->operation==LT48_OPERATION_CONFIGURATION?config_result:result->read_bytes, request->operation==LT48_OPERATION_CONFIGURATION?fields:result->read_size,
                margin_values[request->margin_choice < 4 ? request->margin_choice : 0], request->margin_type, decide_selected_device_id, work->widgets, work->timeout, &completed);
        }
        g_mutex_lock(&work->widgets->progress_mutex);
        if (work->widgets->active_programmer == programmer) work->widgets->active_programmer = NULL;
        lt48_programmer_close(programmer);
        g_mutex_unlock(&work->widgets->progress_mutex);
        lt48_context_destroy(context);
        lt48_lpcflash_close(small);
        result->message = g_strdup_printf("%s: %s · %zu bytes · %zu mismatches · cleanup %s",
            lt48_operation_kind_name(request->operation), lt48_status_string(status),
            completed.completed_bytes, completed.mismatch_count, lt48_status_string(completed.cleanup_status));
        if(status==LT48_OK && request->operation==LT48_OPERATION_CONFIGURATION){g_free(result->message);result->message=lpcflash_configuration_message(fields,config_result,input_size!=0);}
        goto operation_execute_done;
    }
