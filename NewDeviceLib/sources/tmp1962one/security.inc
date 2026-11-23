/* Toshiba manual 3.6.13/3.6.16: a secured array returns 0098, not data.
 * Check all eight source protection addresses before any nondestructive access.
 * Per-block protection (0/1) does not prohibit reading. Never unsecure on Read.
 * Erase uses the original explicit unsecure procedure and checks the effects.
 */
static lt48_status tmp1962one_readable(xp_session *s) {
    lt48_status status=LT48_OK;
    s->mode=0;memset(s->buffers[0],0,32);
    status=run(s,2);
    for(size_t i=0;status==LT48_OK && i<8;i++) {
        uint32_t address=tmp1962one_sectors[i]/2;
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(address>>(j*8));
        status=run(s,6);
        if(status==LT48_OK && s->buffers[2][0]>1)status=LT48_SAFETY_REJECTED;
    }
    lt48_status reset=run(s,3);
    return status==LT48_OK?reset:status;
}
