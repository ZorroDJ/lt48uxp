#ifndef LT48_GAAF_SERIAL_H
#define LT48_GAAF_SERIAL_H
/* Internal resources; catalogue admission additionally requires complete operations. */
#include <labtool48/labtool48.h>
#define LT48_GAAF_OPERATIONS 20u
#define LT48_GAAF_VARIABLES 16u
#define LT48_GAAF_BUFFERS 30u
struct lt48_gaaf_serial_resources {
 lt48_rez *rez;
 lt48_runtime_frame_profile frame_profile;
 lt48_pin_map *pins;
 lt48_compiled_wave *waves[LT48_GAAF_OPERATIONS];
 lt48_compiled_wave *page_read_reset;
 lt48_compiled_wave *id_setup;
 lt48_wave_code applets[LT48_GAAF_OPERATIONS];
 lt48_applet_address addresses[LT48_GAAF_OPERATIONS];
 size_t address_count;
 lt48_runtime_nested_wave nested[LT48_GAAF_OPERATIONS];
 size_t nested_count;
 lt48_compile_variable variables[LT48_GAAF_VARIABLES];
 size_t variable_count;
 const unsigned char *fpga;
 unsigned char *idle;
 unsigned st2009two;unsigned char*st_workers[3];
 size_t idle_size;
 unsigned char*sst38_workers[2];
 unsigned sst38one,micronc3two,sharpminato4,sharpwordpulse3,utron4,sst32four,sst37aliases,sharp1306,sst37four,sharp10,sharp11,nor3,lnand10,sep7,word14,hitachi15,combined36,fujitsu41,definition,capacity,nominal,configuration_bits,legacy28f,reset_op,tms320e,legacy_nor,legacy_word,legacy_route,legacy_f3,legacy_actrans,legacy_w49,legacy_dummy,legacy_page,legacy_page_byte,legacy_motorola,page_bytes,vcc_variable,legacy_config_mask,legacy_page_leave_protection,legacy_page_erase_protection_only,legacy_config_category,legacy_erase_applet;
 uint16_t manufacturer,device;
 unsigned w27e4096two,mxwordtwo,mx27pageone,w29wordtwo,pss2005one,x88one;unsigned char*x88_program,*mxwordtwo_erase;
 unsigned mx26aone;
 unsigned first_byte_status,sm2964two,sharpwordseven,sharpword_sector_count;
};
lt48_status lt48_legacy_mx28f1000_auto_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_macronix_psop3_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_gaaf_serial_open(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
void lt48_gaaf_serial_close(lt48_gaaf_serial_resources *);
typedef struct lt48_gaaf_serial_frame {
 unsigned char buffers[LT48_GAAF_BUFFERS][32];
 lt48_compile_variable variables[LT48_GAAF_VARIABLES];
 uint32_t volts[4];
 lt48_runtime_configuration_value configuration[32];
 size_t configuration_count;
 unsigned mode;
} lt48_gaaf_serial_frame;
void lt48_gaaf_serial_frame_init(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *);
lt48_status lt48_gaaf_serial_frame_commit(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,const lt48_runtime_result *);
lt48_status lt48_gaaf_serial_wave(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,unsigned);
lt48_status lt48_gaaf_serial_setup(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *);
lt48_status lt48_gaaf_serial_stream(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,const lt48_protocol_transport *,void *,unsigned,unsigned,unsigned char *,size_t);
lt48_status lt48_gaaf_serial_cleanup(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,const lt48_protocol_transport *,void *,lt48_status);
lt48_status lt48_gaaf_serial_erase(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *);
lt48_status lt48_gaaf_serial_read_id(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,uint16_t *,uint16_t *);
lt48_status lt48_gaaf_serial_validate(const lt48_gaaf_serial_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
lt48_status lt48_gaaf_serial_execute(const lt48_gaaf_serial_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_gaaf_serial_result *);
lt48_status lt48_gaaf_serial_status_read(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,uint16_t *);
lt48_status lt48_gaaf_serial_status_write(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,uint16_t);
typedef struct lt48_gaaf_configuration_guard {
 uint16_t original;
 int restore_required;
} lt48_gaaf_configuration_guard;
lt48_status lt48_gaaf_serial_configuration_begin(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,lt48_gaaf_configuration_guard *);
lt48_status lt48_gaaf_serial_configuration_restore(const lt48_gaaf_serial_resources *,const lt48_protocol_transport *,void *,lt48_gaaf_configuration_guard *);
lt48_status lt48_word14_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_hitachi15_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_gaaf_hitachi_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_gaaf_hitachi_stream_internal(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,const lt48_protocol_transport *,void *,unsigned,unsigned,unsigned char *,size_t,uint32_t);
lt48_status lt48_legacy28f_execute_internal(const lt48_gaaf_serial_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_gaaf_serial_result *);
lt48_status lt48_legacy28f_stream_internal(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,const lt48_protocol_transport *,void *,unsigned,unsigned,unsigned char *,size_t,uint32_t);
lt48_status lt48_tms320e_secure_internal(const lt48_gaaf_serial_resources *,const lt48_protocol_transport *,void *,lt48_m37_id_decision_fn,void *,lt48_gaaf_serial_result *);
lt48_status lt48_legacy_nor53_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_nor53_applet_internal(const lt48_gaaf_serial_resources *,unsigned,const lt48_wave_code **);
lt48_status lt48_legacy_nor53_execute_internal(const lt48_gaaf_serial_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_gaaf_serial_result *);
lt48_status lt48_legacy_nor53_stream_internal(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,const lt48_protocol_transport *,void *,unsigned,unsigned,unsigned char *,size_t,uint32_t);
lt48_status lt48_legacy_atmel20_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_atmel_classic12_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_atmel_small7_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_hynix8_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_word46_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
/* Private byte49 audit entry point; never selected by the public loader. */
lt48_status lt48_legacy_byte49_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_macronix6_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_macronix_f1601_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
#endif

/* Private class-2008 resources; not public catalogue admission. */
lt48_status lt48_legacy_route45_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);

/* Private remaining word profiles, pending complete qualification. */
lt48_status lt48_legacy_word_remaining9_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);

/* Private Intel F3 candidates; public catalogue admission remains disabled. */
lt48_status lt48_legacy_intel_f3_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
lt48_status lt48_legacy_macronix6_stream_internal(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,const lt48_protocol_transport *,void *,unsigned,unsigned,unsigned char *,size_t,uint32_t);

lt48_status lt48_legacy_macronix6_erase_internal(const lt48_gaaf_serial_resources *,lt48_gaaf_serial_frame *,lt48_runtime_transport_session *,const lt48_protocol_transport *,void *);

/* Private Motorola/Freescale candidates, not public catalogue admission. */
lt48_status lt48_legacy_motorola8_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);

lt48_status lt48_legacy_macronix_l8100_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);

lt48_status lt48_legacy_macronix_l8000_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);

lt48_status lt48_legacy_macronix_l3211_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);


lt48_status lt48_legacy_actrans4_open_private(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);

lt48_status lt48_legacy_w49_open_private(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);

lt48_status lt48_fujitsu41_open_internal(const lt48_installation*,size_t,lt48_gaaf_serial_resources**);

lt48_status lt48_combined36_validate_internal(const lt48_gaaf_serial_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
lt48_status lt48_combined36_execute_internal(const lt48_gaaf_serial_resources*,const lt48_protocol_transport*,void*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void*,lt48_gaaf_serial_result*);

lt48_status lt48_utron4_open_internal(const lt48_installation*,size_t,lt48_gaaf_serial_resources**);

lt48_status lt48_x88one_open_internal(const lt48_installation*,size_t,lt48_gaaf_serial_resources**);
