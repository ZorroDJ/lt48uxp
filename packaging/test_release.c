/* Project-authored package smoke checks. No USB context is created. */
#undef NDEBUG
#include <labtool48/labtool48.h>
#include <assert.h>
#include <string.h>
static unsigned calls;
static lt48_status no_write(void *c,uint8_t e,const unsigned char *d,size_t n,size_t *done){(void)c;(void)e;(void)d;(void)n;*done=0;++calls;return LT48_USB_ERROR;}
static lt48_status no_read(void *c,uint8_t e,unsigned char *d,size_t n,size_t *done){(void)c;(void)e;(void)d;(void)n;*done=0;++calls;return LT48_USB_ERROR;}
static int cancelled(void *c){(void)c;return 1;}
int main(void) {
 lt48_protocol_transport api={no_write,no_read,cancelled,NULL,0};
 lt48_hardware_test_result hw;
 assert(lt48_hardware_test_execute(LT48_HARDWARE_TEST_ECHO,NULL,0,&api,NULL,&hw)==LT48_CANCELLED);
 assert(lt48_hardware_test_execute(LT48_HARDWARE_TEST_EMPTY_SOCKET,NULL,0,&api,NULL,&hw)==LT48_SAFETY_REJECTED);
 lt48_contact_check_profile contact={0};lt48_contact_check_result contacts;
 assert(lt48_contact_check_execute(&contact,&api,NULL,&contacts)==LT48_SAFETY_REJECTED);
 assert(!calls);
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
