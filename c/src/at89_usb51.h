#ifndef LT48_AT89_USB51_INTERNAL_H
#define LT48_AT89_USB51_INTERNAL_H
#include <labtool48/labtool48.h>
/* Exact C5130/31 resource foundation. Not complete-operation admission. */
struct lt48_usb51_resources {
    lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[14];
    lt48_compile_variable vars[5];lt48_wave_code applet[7];lt48_applet_address addresses[7];
    const unsigned char *fpga;unsigned char *idle;size_t idle_size;
    unsigned definition,family,pinmap,flash;
};
lt48_status lt48_usb51_open(const lt48_installation *,size_t,lt48_usb51_resources **);
void lt48_usb51_close(lt48_usb51_resources *);
/* region 0=code, 1=padding, 2/3=source command spaces at F000h/F400h.
 * Physical space names remain to be verified; these are image offsets. */
lt48_status lt48_usb51_address(const lt48_usb51_resources *,uint32_t,uint32_t *,uint32_t *,unsigned *);
lt48_status lt48_usb51_compile_fragment(const lt48_wave_code *,unsigned,const lt48_pin_map *,const lt48_compile_variable *,lt48_compiled_wave **);
lt48_status lt48_usb51_validate(const lt48_usb51_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_usb51_execute(const lt48_usb51_resources *,const lt48_protocol_transport *,void *,
    lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,
    lt48_verify_margin_selection,unsigned,lt48_serial_page_result *);
#endif
