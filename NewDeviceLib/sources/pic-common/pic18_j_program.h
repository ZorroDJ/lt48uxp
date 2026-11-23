#ifndef LT48_PIC18_J_PROGRAM_H
#define LT48_PIC18_J_PROGRAM_H
#include <stddef.h>
#include <stdint.h>
/* Shared J programming after the operator has authorized whole-chip erase.
 * All callbacks retain ownership of the powered programming session. read and
 * program MUST NOT reset/reenter the target. Reset is explicit here, so newly
 * written CP remains dormant through independent full-array readback.
 * This service does not supply adapter wiring, option UI or source transport. */
typedef struct {
 uint32_t code_bytes;
 uint8_t mirror_masks[8],fixed_one[8],fixed_zero[8];
} lt48_pic18_j_program_profile;
typedef struct {
 int (*erase)(void *);
 int (*reenter)(void *);
 int (*program64)(void *,uint32_t,const uint8_t *);
 int (*read)(void *,uint32_t,uint8_t *,size_t);
 int (*finish)(void *);
} lt48_pic18_j_program_io;
typedef struct {
 int operation_error,cleanup_error;
 uint32_t first_mismatch,programmed_blocks;
} lt48_pic18_j_program_result;
enum {LT48_PIC18_J_PROGRAM_OK=0,LT48_PIC18_J_PROGRAM_INVALID=-1,
 LT48_PIC18_J_PROGRAM_MISMATCH=-2};
int lt48_pic18_j_program_whole(const lt48_pic18_j_program_profile *,
 const lt48_pic18_j_program_io *,void *,const uint8_t *,size_t,
 lt48_pic18_j_program_result *);
#endif
