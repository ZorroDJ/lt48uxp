#ifndef LT48_HC908_INTERNAL_H
#define LT48_HC908_INTERNAL_H
#include <labtool48/labtool48.h>
/* Profile query shared with installation; candidate construction admits nothing. */
int lt48_hc908_is_fqtqy(const lt48_hc908_resources *resources);
int lt48_hc908_is_abasaz(const lt48_hc908_resources *r);
int lt48_hc908_is_gpgz(const lt48_hc908_resources *r);
unsigned lt48_hc908_blank_value(const lt48_hc908_resources *r);
int lt48_hc908_is_apsr(const lt48_hc908_resources *r);
int lt48_hc908_is_grkx(const lt48_hc908_resources *r);
int lt48_hc908_is_gxjb(const lt48_hc908_resources *r);
int lt48_hc908_is_jkjl(const lt48_hc908_resources *);
typedef struct lt48_hc908_resources lt48_hc908_resources;
lt48_status lt48_hc908_open_candidate(const lt48_installation *,size_t,lt48_hc908_resources **);
void lt48_hc908_close(lt48_hc908_resources *);
unsigned lt48_hc908_flash_start(const lt48_hc908_resources *);
lt48_status lt48_hc908_set_security_key(lt48_hc908_resources *,const unsigned char *,size_t);
lt48_status lt48_hc908_validate(const lt48_hc908_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
lt48_status lt48_hc908_execute(const lt48_hc908_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
#endif
