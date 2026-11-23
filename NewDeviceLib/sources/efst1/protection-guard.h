/* Authored refusal policy using the original GETCONFIG9 boot-status cycle.
 * Read-only state query precedes any original mutation entry sequence. */
static lt48_status efst1_protection_guard(const lt48_gaaf_serial_resources*r,
 lt48_gaaf_serial_frame*f,lt48_runtime_transport_session*w){
 if(!r->legacy_nor||r->legacy_word||r->legacy_f3||r->legacy_route||r->legacy_page||r->definition!=197||r->manufacturer!=140||r->capacity!=262144)return LT48_OK;
 memset(f->buffers[10],0,32);lt48_status st=lt48_gaaf_serial_wave(r,f,w,9);
 return st?st:(f->buffers[10][0]&1)?LT48_SAFETY_REJECTED:LT48_OK;
}
