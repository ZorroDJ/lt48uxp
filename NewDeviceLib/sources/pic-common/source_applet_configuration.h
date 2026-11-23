#ifndef LT48_PIC_SOURCE_APPLET_CONFIGURATION_H
#define LT48_PIC_SOURCE_APPLET_CONFIGURATION_H
#include <labtool48/labtool48.h>
#include <stdint.h>

/* Original family loader 0041fa72..0041fa99: nonzero configuration IDs
 * below 10000 use sector/bit configuration; >=10000 use whole callbacks.
 * Zero is deliberately not inferred from stale global UI state. */
static inline int lt48_pic_source_applet_configuration_whole(uint16_t id) {
    return id>=10000;
}
/* Original whole branches 004665aa and 004683b4. Transport/callback failure
 * remains distinct from a false device result. ReadCfg does not interpret
 * DataIn as success; writing configuration does. Retry/Ignore stays explicit. */
static inline lt48_status lt48_pic_source_applet_configuration_result(
    uint16_t mode,lt48_status status,int data_in) {
    if(mode!=12&&mode!=13)return LT48_INVALID_ARGUMENT;
    return status==LT48_OK&&mode==13&&!data_in?LT48_DEVICE_MISMATCH:status;
}
#endif
