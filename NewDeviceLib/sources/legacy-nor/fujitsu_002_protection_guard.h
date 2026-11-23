/* Sector protection changes are deliberately deferred. Before a mutation,
 * query all source sector-status addresses without invoking UNPROTECT.
 * Protected parts remain readable; program/erase reject without altering
 * their nonvolatile protection bits. DS05-20818-1E tables 2/4/6. */
static lt48_status fujitsu_002_protection_guard(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire) {
 if(!r->legacy_nor||r->legacy_word||r->legacy_route||r->legacy_f3||r->legacy_page||
    r->manufacturer!=4||(r->definition!=2&&r->definition!=3)||r->capacity!=0x40000)
  return LT48_OK;
 memset(frame->buffers[9],0,32);
 lt48_status status=lt48_gaaf_serial_wave(r,frame,wire,9);
 if(status!=LT48_OK)return status;
 for(unsigned i=0;i<4;i++)if(frame->buffers[9][i])return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
