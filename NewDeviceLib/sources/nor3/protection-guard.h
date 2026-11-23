/* Query permanent boot protection before original INIT applies its VID override. */
static lt48_status nor3_protection_guard(const lt48_gaaf_serial_resources*r,
 lt48_gaaf_serial_frame*f,lt48_runtime_transport_session*w){
 if(!r->nor3||r->legacy_route)return LT48_OK;
 memset(f->buffers[10],0,32);
 lt48_status s=lt48_gaaf_serial_wave(r,f,w,9);
 if(s==LT48_OK&&(f->buffers[10][0]&1))s=LT48_SAFETY_REJECTED;
 return s;
}
