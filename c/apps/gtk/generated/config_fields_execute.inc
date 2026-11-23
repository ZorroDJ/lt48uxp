/* Generated; edit tools/configuration_profiles. */
    if (status == LT48_OK && !strcmp(operation_profile,"at89-c5114-nine-operation")) {
        lt48_c5114_resources *small = NULL;
        lt48_serial_page_result completed = {0};
        lt48_context *context = NULL;
        lt48_programmer *programmer = NULL;
        lt48_transport_info ready = {0};
        status = lt48_c5114_open(installation, request->device_index, &small);
        unsigned fields=6,flags=5;
        unsigned char config_result[6]={0},lock=(unsigned char)(request->mcu51_protection?request->mcu51_protection:1);
        const unsigned char *input=request->image;size_t input_size=request->image_size;
        if(request->operation==LT48_OPERATION_CONFIGURATION){input=request->c5114_configuration[0]?request->c5114_configuration:NULL;input_size=input?fields+1:0;}
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
            status = lt48_programmer_execute_at89_c5114(programmer, small,
                request->operation, request->start, request->length,
                input, input_size, request->operation==LT48_OPERATION_CONFIGURATION?config_result:result->read_bytes, request->operation==LT48_OPERATION_CONFIGURATION?fields:result->read_size,
                margin_values[request->margin_choice < 4 ? request->margin_choice : 0], request->margin_type, decide_selected_device_id, work->widgets, work->timeout, &completed);
        }
        g_mutex_lock(&work->widgets->progress_mutex);
        if (work->widgets->active_programmer == programmer) work->widgets->active_programmer = NULL;
        lt48_programmer_close(programmer);
        g_mutex_unlock(&work->widgets->progress_mutex);
        lt48_context_destroy(context);
        lt48_c5114_close(small);
        result->message = g_strdup_printf("%s: %s · %zu bytes · %zu mismatches · cleanup %s",
            lt48_operation_kind_name(request->operation), lt48_status_string(status),
            completed.completed_bytes, completed.mismatch_count, lt48_status_string(completed.cleanup_status));
        if(status==LT48_OK && request->operation==LT48_OPERATION_CONFIGURATION){g_free(result->message);result->message=c5114_configuration_message(flags,config_result,input_size!=0);}
        goto operation_execute_done;
    }

    if (status == LT48_OK && !strcmp(operation_profile,"at89-c5132-nine-operation")) {
        lt48_c5132_resources *small = NULL;
        lt48_serial_page_result completed = {0};
        lt48_context *context = NULL;
        lt48_programmer *programmer = NULL;
        lt48_transport_info ready = {0};
        status = lt48_c5132_open(installation, request->device_index, &small);
        unsigned fields=5,flags=2;
        unsigned char config_result[5]={0},lock=(unsigned char)(request->mcu51_protection?request->mcu51_protection:1);
        const unsigned char *input=request->image;size_t input_size=request->image_size;
        if(request->operation==LT48_OPERATION_CONFIGURATION){input=request->c5132_configuration[0]?request->c5132_configuration:NULL;input_size=input?fields+1:0;}
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
            status = lt48_programmer_execute_at89_c5132(programmer, small,
                request->operation, request->start, request->length,
                input, input_size, request->operation==LT48_OPERATION_CONFIGURATION?config_result:result->read_bytes, request->operation==LT48_OPERATION_CONFIGURATION?fields:result->read_size,
                margin_values[request->margin_choice < 4 ? request->margin_choice : 0], request->margin_type, decide_selected_device_id, work->widgets, work->timeout, &completed);
        }
        g_mutex_lock(&work->widgets->progress_mutex);
        if (work->widgets->active_programmer == programmer) work->widgets->active_programmer = NULL;
        lt48_programmer_close(programmer);
        g_mutex_unlock(&work->widgets->progress_mutex);
        lt48_context_destroy(context);
        lt48_c5132_close(small);
        result->message = g_strdup_printf("%s: %s · %zu bytes · %zu mismatches · cleanup %s",
            lt48_operation_kind_name(request->operation), lt48_status_string(status),
            completed.completed_bytes, completed.mismatch_count, lt48_status_string(completed.cleanup_status));
        if(status==LT48_OK && request->operation==LT48_OPERATION_CONFIGURATION){g_free(result->message);result->message=c5132_configuration_message(flags,config_result,input_size!=0);}
        goto operation_execute_done;
    }

    if (status == LT48_OK && !strcmp(operation_profile,"philips-lpc-eight-operation")) {
        lt48_lpc_resources *small = NULL;
        lt48_serial_page_result completed = {0};
        lt48_context *context = NULL;
        lt48_programmer *programmer = NULL;
        lt48_transport_info ready = {0};
        status = lt48_lpc_open(installation, request->device_index, &small);
        unsigned fields=7,flags=6;
        unsigned char config_result[7]={0},lock=(unsigned char)(request->mcu51_protection?request->mcu51_protection:1);
        const unsigned char *input=request->image;size_t input_size=request->image_size;
        if(request->operation==LT48_OPERATION_CONFIGURATION){input=request->lpc_configuration[0]?request->lpc_configuration:NULL;input_size=input?fields+1:0;}
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
            status = lt48_programmer_execute_at89_lpc(programmer, small,
                request->operation, request->start, request->length,
                input, input_size, request->operation==LT48_OPERATION_CONFIGURATION?config_result:result->read_bytes, request->operation==LT48_OPERATION_CONFIGURATION?fields:result->read_size,
                margin_values[request->margin_choice < 4 ? request->margin_choice : 0], request->margin_type, decide_selected_device_id, work->widgets, work->timeout, &completed);
        }
        g_mutex_lock(&work->widgets->progress_mutex);
        if (work->widgets->active_programmer == programmer) work->widgets->active_programmer = NULL;
        lt48_programmer_close(programmer);
        g_mutex_unlock(&work->widgets->progress_mutex);
        lt48_context_destroy(context);
        lt48_lpc_close(small);
        result->message = g_strdup_printf("%s: %s · %zu bytes · %zu mismatches · cleanup %s",
            lt48_operation_kind_name(request->operation), lt48_status_string(status),
            completed.completed_bytes, completed.mismatch_count, lt48_status_string(completed.cleanup_status));
        if(status==LT48_OK && request->operation==LT48_OPERATION_CONFIGURATION){g_free(result->message);result->message=lpc_configuration_message(flags,config_result,input_size!=0);}
        goto operation_execute_done;
    }

