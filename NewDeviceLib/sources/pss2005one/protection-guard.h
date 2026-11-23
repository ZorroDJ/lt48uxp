/* Original GETCONFIG17 packs all twelve inverted status bits into BUF5.
 * Any set bit aborts original Program/Erase. Preserve that prerequisite;
 * never narrow to the generic two-bit mask, and never unlock implicitly. */
static lt48_status pss2005one_protection_guard(const lt48_gaaf_serial_resources*r,lt48_gaaf_serial_frame*f,lt48_runtime_transport_session*w){
 if(!r->pss2005one)return LT48_OK;
 lt48_status st=lt48_gaaf_serial_wave(r,f,w,17);if(st)return st;
 uint32_t mask=(uint32_t)f->buffers[5][0]|(uint32_t)f->buffers[5][1]<<8|(uint32_t)f->buffers[5][2]<<16|(uint32_t)f->buffers[5][3]<<24;
 if(mask&~0xfffu)return LT48_PROTOCOL_ERROR;
 return mask?LT48_SAFETY_REJECTED:LT48_OK;
}
