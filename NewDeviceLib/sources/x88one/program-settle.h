/* Authored temporary-RAM timing guard over the exact BYO113-byte worker.
 * Eight NOPs after final SDP A0, before C0/data: 4us at24MHz/12cycles.
 * Preserve the early non-streaming branch target. No original bytes shipped.
 * Does not claim the observed model discrepancy is a physical vendor bug. */
static lt48_status x88one_program_settle(lt48_gaaf_serial_resources*r){
 lt48_wave_code*w=&r->applets[9];
 if(w->data_size!=113||w->load_address!=0x5100||memcmp(w->data+0x4b,"\xf2\x74\xc0\xf2",4)||memcmp(w->data+0x2a,"\x30\xb5\x2c",3))return LT48_SAFETY_REJECTED;
 r->x88_program=malloc(121);if(!r->x88_program)return LT48_NO_MEMORY;
 memcpy(r->x88_program,w->data,0x4c);memset(r->x88_program+0x4c,0,8);memcpy(r->x88_program+0x54,w->data+0x4c,113-0x4c);r->x88_program[0x2c]+=8;
 w->data=r->x88_program;w->data_size=121;return LT48_OK;
}
