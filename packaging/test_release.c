/* Project-authored package smoke checks. No USB context is created. */
#undef NDEBUG
#include <labtool48/labtool48.h>
#include <assert.h>
#include <string.h>
int main(void) {
 lt48_diagnostics_report report;
 assert(lt48_diagnostics_self_test(NULL, &report)==LT48_OK);
 assert(report.passed);
 lt48_buffer *buffer=NULL;
 unsigned char bytes[2]={0x12,0xed},readback[2]={0};
 const size_t capacity=8u*1024u*1024u+512u;
 assert(lt48_buffer_create(capacity,0xff,&buffer)==LT48_OK);
 assert(lt48_buffer_size(buffer)==capacity);
 assert(lt48_buffer_write(buffer,capacity-2,bytes,2)==LT48_OK);
 assert(lt48_buffer_read(buffer,capacity-2,readback,2)==LT48_OK);
 assert(memcmp(bytes,readback,2)==0);
 assert(lt48_buffer_write(buffer,capacity-1,bytes,2)!=LT48_OK);
 lt48_buffer_close(buffer);
 buffer=NULL;
 assert(lt48_buffer_create_sparse(512,&buffer)==LT48_OK);
 int known=1;
 assert(lt48_buffer_range_known(buffer,0,512,&known)==LT48_OK && !known);
 assert(lt48_buffer_write_acquired(buffer,0,bytes,2)==LT48_OK);
 assert(lt48_buffer_known_size(buffer)==2);
 lt48_buffer_close(buffer);
 return 0;
}
