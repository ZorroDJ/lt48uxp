#include "pic18_j_rewrite.h"
#include <string.h>
static int geometry(uint32_t n) {
 switch(n){case 8192:case 16384:case 32768:case 49152:case 65536:case 98304:case 131072:return 1;default:return 0;}
}
static int overlaps(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return x<=y?y-x<n:x-y<m;
}
static int finish(const lt48_pic18_j_program_io *io,void *context,lt48_pic18_j_rewrite_result *r,int error) {
 r->program.operation_error=error;r->program.cleanup_error=io->finish(context);
 return error?error:r->program.cleanup_error;
}
int lt48_pic18_j_rewrite(const lt48_pic18_j_program_profile *p,
 const lt48_pic18_j_program_io *io,void *context,unsigned operation,uint32_t first,
 const uint8_t *patch,size_t patch_size,uint8_t *backup,uint8_t *replacement,
 size_t size,lt48_pic18_j_rewrite_result *r) {
 if(!r)return LT48_PIC18_J_PROGRAM_INVALID;
 memset(r,0,sizeof(*r));r->program.operation_error=LT48_PIC18_J_PROGRAM_INVALID;r->program.first_mismatch=UINT32_MAX;
 if(!p||!geometry(p->code_bytes)||size!=p->code_bytes||!backup||!replacement||
    overlaps(backup,size,replacement,size)||!io||!io->erase||!io->reenter||!io->program64||!io->read||!io->finish||
    !(p->mirror_masks[1]&4)||
    (operation!=LT48_PIC18_J_REWRITE_SECURE&&operation!=LT48_PIC18_J_REWRITE_PATCH))return r->program.operation_error;
 if(operation==LT48_PIC18_J_REWRITE_SECURE) {
  if(first||patch||patch_size)return r->program.operation_error;
 }else if(!patch||!patch_size||first>=size||patch_size>size-first||
           overlaps(patch,patch_size,backup,size)||overlaps(patch,patch_size,replacement,size))return r->program.operation_error;
 for(unsigned i=0;i<8;i++)if(p->fixed_one[i]&p->fixed_zero[i])return r->program.operation_error;
 int error=io->reenter(context);uint8_t mirrors[8],data[64];
 if(!error)error=io->read(context,0x300000,mirrors,8);
 if(error)return finish(io,context,r,error);
 if(!(mirrors[1]&4)) {
  if(operation==LT48_PIC18_J_REWRITE_SECURE){r->already_secure=1;return finish(io,context,r,0);}
  return finish(io,context,r,LT48_PIC18_J_REWRITE_PROTECTED);
 }
 for(uint32_t a=0;!error&&a<p->code_bytes;a+=64)error=io->read(context,a,backup+a,64);
 /* Do not erase on an unstable preservation read. The first pass remains in
  * caller storage even if the independent second pass fails. */
 for(uint32_t a=0;!error&&a<p->code_bytes;a+=64) {
  error=io->read(context,a,data,64);
  for(unsigned i=0;!error&&i<64;i++)if(data[i]!=backup[a+i]) {
   error=LT48_PIC18_J_PROGRAM_MISMATCH;r->program.first_mismatch=a+i;
  }
 }
 for(unsigned i=0;!error&&i<8;i++)if((backup[size-8+i]&p->mirror_masks[i])!=mirrors[i]) {
  error=LT48_PIC18_J_PROGRAM_MISMATCH;r->program.first_mismatch=0x300000+i;
 }
 if(error)return finish(io,context,r,error);
 r->backup_valid=1;memcpy(replacement,backup,size);
 for(unsigned i=0;i<8;i++)replacement[size-8+i]=(uint8_t)((replacement[size-8+i]|p->fixed_one[i])&~p->fixed_zero[i]);
 if(operation==LT48_PIC18_J_REWRITE_SECURE)replacement[size-7]&=(uint8_t)~4u;
 else memcpy(replacement+first,patch,patch_size);
 /* Validate before erase. Do not silently repair explicitly requested bits. */
 for(unsigned i=0;i<8;i++)if((replacement[size-8+i]&p->fixed_one[i])!=p->fixed_one[i]||
    (replacement[size-8+i]&p->fixed_zero[i]))return finish(io,context,r,LT48_PIC18_J_PROGRAM_INVALID);
 return lt48_pic18_j_program_whole(p,io,context,replacement,size,&r->program);
}
