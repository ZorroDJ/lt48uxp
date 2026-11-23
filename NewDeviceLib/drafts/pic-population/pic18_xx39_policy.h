#ifndef LT48_DRAFT_PIC18_XX39_POLICY_H
#define LT48_DRAFT_PIC18_XX39_POLICY_H
#include <labtool48/labtool48.h>
/* DS30485A4.1/20.4: reserved kernel shares panel1 in12KiB parts.
 * Do not create protection of that panel, or erase it blindly when already
 * protected. Reserved protection bits must remain set (table20-3).
 * This policy does not invent UID/configuration erase side effects. */
static inline lt48_status lt48_pic18_xx39_configuration_safe(uint32_t code_bytes,
 const unsigned char *configuration,size_t count) {
 if(!configuration||count!=14||(code_bytes!=12288&&code_bytes!=24576))return LT48_INVALID_ARGUMENT;
 /* ReadCfg masks unimplemented bits out. Only the implemented kernel-sharing
  * bit is a precondition; reserved write values are prepared separately. */
 unsigned required=code_bytes==12288?2u:0u;
 const unsigned bytes[]={8,10,12};
 for(unsigned i=0;i<3;i++)if((configuration[bytes[i]]&required)!=required)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
static inline void lt48_pic18_xx39_prepare_configuration(uint32_t code_bytes,unsigned char *configuration) {
 unsigned reserved=code_bytes==12288?12u:8u;
 configuration[8]|=(unsigned char)reserved;configuration[10]|=(unsigned char)reserved;configuration[12]|=(unsigned char)reserved;
}
static inline unsigned lt48_pic18_xx39_secure_code_mask(uint32_t code_bytes) {
 return code_bytes==12288?1u:code_bytes==24576?7u:0u;
}
#endif
