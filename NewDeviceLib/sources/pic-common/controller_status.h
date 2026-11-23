#ifndef LT48_PIC_CONTROLLER_STATUS_H
#define LT48_PIC_CONTROLLER_STATUS_H
#include <labtool48/labtool48.h>

/* Original CDrvUSB byte receiver 00496eb0 and status decoder 00497640:
 * ACK is the little-endian first word; flag bit 3 reports a rail fault.
 * Application status is 0 on success, 1 on failed exchange, 2 on rail fault.
 * A rail fault is a valid exchange, separated from transport failure so the
 * lifecycle can explicitly reject it and still perform power-off. */
static inline lt48_status lt48_pic_controller_status_decode(const unsigned char *data,
    size_t size,uint16_t *application_status) {
    if(!data || size<3 || !application_status)return LT48_INVALID_ARGUMENT;
    if(!(data[0]|data[1])){*application_status=1;return LT48_DEVICE_MISMATCH;}
    *application_status=(data[2]&8)?2:0;
    return LT48_OK;
}
#endif
