/* The boot lock is permanent. Query the original source status address
 * before DEVINIT can apply its temporary RESET/VID override. A locked chip
 * stays readable; memory mutations reject without changing its boot lock. */
static lt48_status atmel_008_protection_guard(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire) {
 if(!r->legacy_nor||r->legacy_word||r->legacy_route||r->legacy_f3||r->legacy_page||
    r->manufacturer!=0x1f||(r->definition!=0xb4&&r->definition!=0xb5)||r->capacity!=0x100000)return LT48_OK;
 memset(frame->buffers[9],0,32);
 lt48_status s=lt48_gaaf_serial_wave(r,frame,wire,9);
 if(s!=LT48_OK)return s;
 return (frame->buffers[9][0]&1)?LT48_SAFETY_REJECTED:LT48_OK;
}
