/* Reuse the typed SST operation owner, then independently check array mutations
 * across a new powered session, including any corruption during cleanup. */
static lt48_status execute_sst58rc2(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 lt48_status st=execute_sstmx16(r,api,ctx,op,a,n,im,size,out,outsize,margin,type,decide,dc,z);
 if(st||(op!=3&&op!=4&&op!=7))return st;
 unsigned char*actual=malloc(r->capacity);if(!actual)return z->operation_status=LT48_NO_MEMORY;
 lt48_serial_page_result fresh;st=execute_sstmx16(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,decide,dc,&fresh);
 if(!st)st=w78bank_compare(z,0,actual,op==7?NULL:im,r->capacity);
 free(actual);
 if(st){z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;lt48_runtime_transport_session*wire=NULL;lt48_status cleanup;int done;lt48_owner_finish_internal(&clean,ctx,&wire,NULL,NULL,NULL,0,st,&cleanup,&done);if(!done){z->cleanup_status=cleanup;z->cleanup_completed=0;}}
 return st;
}
