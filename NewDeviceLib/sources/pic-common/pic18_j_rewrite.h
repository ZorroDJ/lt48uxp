#ifndef LT48_PIC18_J_REWRITE_H
#define LT48_PIC18_J_REWRITE_H
#include "pic18_j_program.h"
enum {LT48_PIC18_J_REWRITE_SECURE=1,LT48_PIC18_J_REWRITE_PATCH=2};
enum {LT48_PIC18_J_REWRITE_PROTECTED=-3};
typedef struct {
 lt48_pic18_j_program_result program;
 int backup_valid,already_secure;
} lt48_pic18_j_rewrite_result;
/* Preserving whole-chip rewrite after explicit erase authorization. Secure
 * changes CP0; patch replaces exactly the requested byte range. Two complete
 * reads must agree before erase. Backup and replacement are distinct caller
 * buffers, each exactly code_bytes; backup remains available after a failure.
 * reenter must establish the powered session even on the initial call.
 * Protected Secure is idempotent. A protected patch rejects before erase.
 * Reserved physical bits are normalized in preserved configuration; invalid
 * reserved bits supplied explicitly by a patch reject before erase.
 * This is an internal operation policy, not a GUI admission or default erase
 * authorization. Caller must explain the whole-chip erase/rewrite to operator. */
int lt48_pic18_j_rewrite(const lt48_pic18_j_program_profile *,
 const lt48_pic18_j_program_io *,void *,unsigned,uint32_t,const uint8_t *,size_t,
 uint8_t *,uint8_t *,size_t,lt48_pic18_j_rewrite_result *);
#endif
