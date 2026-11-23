#include "pic18_j_program.h"
#include <string.h>
static int valid(const lt48_pic18_j_program_profile *p,const uint8_t *image,size_t size) {
 if(!p||!image||size!=p->code_bytes)return 0;
 switch(p->code_bytes) {
 case 8192:case 16384:case 32768:case 49152:case 65536:case 98304:case 131072:break;
 default:return 0;
 }
 for(unsigned i=0;i<8;i++) {
  uint8_t v=image[size-8+i];
  if((p->fixed_one[i]&p->fixed_zero[i])||
     (v&p->fixed_one[i])!=p->fixed_one[i]||(v&p->fixed_zero[i]))return 0;
 }
 return 1;
}
int lt48_pic18_j_program_whole(const lt48_pic18_j_program_profile *p,
 const lt48_pic18_j_program_io *io,void *context,const uint8_t *image,size_t size,
 lt48_pic18_j_program_result *r) {
 if(!r)return LT48_PIC18_J_PROGRAM_INVALID;
 *r=(lt48_pic18_j_program_result){LT48_PIC18_J_PROGRAM_INVALID,0,UINT32_MAX,0};
 if(!valid(p,image,size)||!io||!io->erase||!io->reenter||!io->program64||!io->read||!io->finish)
  return r->operation_error;
 int error=io->erase(context);
 if(!error)error=io->reenter(context); /* Reload erased, unprotected configuration. */
 uint8_t data[64];
 if(!error) {
  error=io->read(context,0x300000,data,8);
  for(unsigned i=0;!error&&i<8;i++)if(data[i]!=p->mirror_masks[i]) {
   error=LT48_PIC18_J_PROGRAM_MISMATCH;r->first_mismatch=0x300000+i;
  }
 }
 for(uint32_t a=0;!error&&a<p->code_bytes;a+=64) {
  error=io->program64(context,a,image+a);
  if(!error)r->programmed_blocks++;
 }
 /* Verify physical configuration and every code byte before reset activates
  * CP or write protection. Never perform a second configuration block write. */
 for(uint32_t a=0;!error&&a<p->code_bytes;a+=64) {
  error=io->read(context,a,data,64);
  for(unsigned i=0;!error&&i<64;i++)if(data[i]!=image[a+i]) {
   error=LT48_PIC18_J_PROGRAM_MISMATCH;r->first_mismatch=a+i;
  }
 }
 if(!error)error=io->reenter(context);
 if(!error) {
  error=io->read(context,0x300000,data,8);
  for(unsigned i=0;!error&&i<8;i++)if(data[i]!=(image[size-8+i]&p->mirror_masks[i])) {
   error=LT48_PIC18_J_PROGRAM_MISMATCH;r->first_mismatch=0x300000+i;
  }
 }
 r->operation_error=error;r->cleanup_error=io->finish(context);
 return error?error:r->cleanup_error;
}
