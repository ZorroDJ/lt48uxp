if(status==LT48_OK&&!strcmp(operation_profile,"tms87two-six-operation")){
 lt48_tms87_resources*r=NULL;lt48_small_eprom_result completed={0};lt48_context*context=NULL;lt48_programmer*programmer=NULL;lt48_transport_info ready={0};
 status=lt48_tms87_open(installation,request->device_index,&r);const unsigned char*in=request->image;size_t size=request->image_size;
 if(status==LT48_OK&&request->operation==LT48_OPERATION_READ){result->read_bytes=g_malloc(request->length);result->read_start=request->start;result->read_size=request->length;}
 if(status==LT48_OK)status=lt48_context_create(&context);
 if(status==LT48_OK)status=lt48_programmer_open(context,work->bus,work->address,&programmer);
 if(status==LT48_OK)status=lt48_programmer_transport_initialize(programmer,work->timeout,&ready);
 if(status==LT48_OK)status=lt48_programmer_set_cancel_check(programmer,gio_cancelled,work->widgets->read_cancellable);
 if(status==LT48_OK){g_mutex_lock(&work->widgets->progress_mutex);work->widgets->active_programmer=programmer;g_mutex_unlock(&work->widgets->progress_mutex);status=lt48_programmer_execute_tms87(programmer,r,request->operation,request->start,request->length,in,size,result->read_bytes,result->read_size,margin_values[request->margin_choice<4?request->margin_choice:0],request->margin_type,work->timeout,&completed);}
 g_mutex_lock(&work->widgets->progress_mutex);if(work->widgets->active_programmer==programmer)work->widgets->active_programmer=NULL;lt48_programmer_close(programmer);g_mutex_unlock(&work->widgets->progress_mutex);lt48_context_destroy(context);
 result->message=g_strdup_printf("TMS87 %s: %s · %zu bytes · %zu mismatches · cleanup %s",lt48_operation_kind_name(request->operation),lt48_status_string(status),completed.completed_bytes,completed.mismatch_count,lt48_status_string(completed.cleanup_status));
 lt48_tms87_close(r);goto operation_execute_done;
}
