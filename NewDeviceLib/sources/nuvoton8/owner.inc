/* Reuse the audited mapped owner; qualify mutations again after cleanup using
 * a separate powered raw-Read session. The older cohort remains unchanged. */
static lt48_status execute_nuvoton8(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 lt48_status st=execute_numicro81(r,api,ctx,op,a,n,im,size,out,outsize,margin,type,decide,dc,z);
 if(st||(op!=3&&op!=4&&op!=7))return st;
 unsigned char*actual=malloc(r->capacity);if(!actual){z->operation_status=LT48_NO_MEMORY;z->completed_bytes=0;return LT48_NO_MEMORY;}lt48_serial_page_result fresh;
 st=execute_numicro81(r,api,ctx,LT48_OPERATION_READ,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,decide,dc,&fresh);
 if(!st)st=w78bank_compare(z,0,actual,op==7?NULL:im,r->capacity);
 if(st){z->operation_status=st;z->completed_bytes=0;if(!fresh.cleanup_completed){z->cleanup_completed=0;z->cleanup_status=fresh.cleanup_status;}}
 if(st){lt48_runtime_transport_session*wire=NULL;lt48_status clean;int done;lt48_owner_finish_internal(api,ctx,&wire,NULL,NULL,NULL,0,st,&clean,&done);if(!done){z->cleanup_completed=0;z->cleanup_status=clean;}}
 free(actual);return st;
}
