#ifndef LT48_AT89_S8253_H
#define LT48_AT89_S8253_H
#include <labtool48/labtool48.h>
/* Exact parallel-procedure resources and verified source aliases. */
struct lt48_s8253_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[17];
 lt48_compile_variable vars[8];lt48_wave_code applet[8];lt48_applet_address addresses[8];
 unsigned char *gc8_protect_copy; /* Authored opcode correction, owned by this resource. */
 const unsigned char *fpga;unsigned char *idle;size_t idle_size;
 unsigned definition,family,pinmap,flash,manufacturer,device;
 unsigned fields,capacity;
 unsigned regions,region_length[4],region_bank[4],variable_count,configuration_id;
 unsigned n79_xp,n79_direct,n79_locks,n79_secure,n79_physical[3];
 unsigned configuration_lock;
 unsigned configuration_category[32],configuration_field[32],configuration_maximum[32];
 unsigned msp430_overwrite_calibration;
 unsigned legacy_w79e201;
 unsigned n76one;
 unsigned sstmx_variant,sstmx_applets;
 unsigned psoc_security_bytes;
 unsigned nuvoton8;
 unsigned novatek25_main_only;
 unsigned stm8s4_page;
 unsigned at28c64b1_sdp;
 unsigned a91_width,a91_vcc,a91_bridge;
 unsigned kind,nominal; /* kind 1: P89V/LV51 mapped parallel Flash. */
 unsigned mf74_page;
 unsigned white3_page;
};
unsigned lt48_n76one_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_n76one_admitted_internal(unsigned,unsigned);
lt48_status lt48_n76one_open(const lt48_installation*,size_t,lt48_s8253_resources**);
unsigned lt48_mf74_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_mf74_admitted_internal(unsigned,unsigned);
lt48_status lt48_mf74_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_mf74_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_mw27_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_mw27_admitted_internal(unsigned,unsigned);
lt48_status lt48_mw27_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_mw27_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_sy80_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_sy76_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_mw36_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_sm30_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_sy80_admitted_internal(unsigned,unsigned);
int lt48_sy76_admitted_internal(unsigned,unsigned);
int lt48_mw36_admitted_internal(unsigned,unsigned);
int lt48_sm30_admitted_internal(unsigned,unsigned);
lt48_status lt48_sy80_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sy76_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_mw36_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sm30_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sy80_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_sy76_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_mw36_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_sm30_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_s8253_open(const lt48_installation *,size_t,lt48_s8253_resources **);
void lt48_s8253_close(lt48_s8253_resources *);
lt48_status lt48_s8253_address(const lt48_s8253_resources *,uint32_t,uint32_t *,uint32_t *);
lt48_status lt48_s8253_validate(const lt48_s8253_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_s8253_execute(const lt48_s8253_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
lt48_status lt48_s8253_compile_fragment(const lt48_wave_code *,unsigned,const lt48_pin_map *,const lt48_compile_variable *,lt48_compiled_wave **);
int lt48_p89v_admitted_internal(unsigned);
lt48_status lt48_p89v_open(const lt48_installation *,size_t,lt48_s8253_resources **);
lt48_status lt48_p89v_validate(const lt48_s8253_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
int lt48_sst_admitted_internal(unsigned);
lt48_status lt48_sst_open(const lt48_installation *,size_t,lt48_s8253_resources **);
lt48_status lt48_sst_validate(const lt48_s8253_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
int lt48_w78d24_admitted_internal(unsigned);
lt48_status lt48_w78d24_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_w78d24_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
int lt48_w78bank147_admitted_internal(unsigned);
lt48_status lt48_w78bank147_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_w78bank147_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_n79w79_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_n79w79_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_n79w79_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_n79w79_admitted_internal(unsigned,unsigned);
lt48_status lt48_numicro81_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_numicro81_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_numicro81_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_numicro81_admitted_internal(unsigned);
lt48_status lt48_msp43076_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_msp43076_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_msp43076_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_msp43076_admitted_internal(unsigned);
lt48_status lt48_msp43076_calibration_policy(lt48_s8253_resources*,unsigned);
lt48_status lt48_nuvoton30_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_nuvoton30_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_nuvoton30_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_nuvoton30_admitted_internal(unsigned);
lt48_status lt48_n79w79_21_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_n79w79_21_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_n79w79_21_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_n79w79_21_admitted_internal(unsigned);

unsigned lt48_sstmx16_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_sstmx16_admitted_internal(unsigned);
lt48_status lt48_sstmx16_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sstmx16_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

int lt48_psoc39_member_internal(unsigned);
unsigned lt48_psoc29_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_psoc29_admitted_internal(unsigned);
lt48_status lt48_psoc29_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_psoc29_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_xmega7_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_xmega7_admitted_internal(unsigned);
lt48_status lt48_xmega7_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_xmega7_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_nuvoton8_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_nuvoton8_admitted_internal(unsigned);
lt48_status lt48_nuvoton8_open(const lt48_installation*,size_t,lt48_s8253_resources**);
unsigned lt48_novatek25_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_novatek25_admitted_internal(unsigned);
int lt48_novatek25_member_internal(unsigned);
lt48_status lt48_novatek25_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_novatek25_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_sst58rc2_open(const lt48_installation*,size_t,lt48_s8253_resources**);
unsigned lt48_sst58rc2_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_sst58rc2_admitted_internal(unsigned);

unsigned lt48_cc254x5_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_cc254x5_admitted_internal(unsigned);
int lt48_cc254x5_member_internal(unsigned);
lt48_status lt48_cc254x5_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_cc254x5_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_stm8s4_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_stm8s4_admitted_internal(unsigned);
int lt48_stm8s4_member_internal(unsigned);
lt48_status lt48_stm8s4_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_stm8s4_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_r8c8_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_r8c8_admitted_internal(unsigned,unsigned);
lt48_status lt48_r8c8_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_r8c8_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
#endif

unsigned lt48_at28c64b1_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_at28c64b1_admitted_internal(void);
lt48_status lt48_at28c64b1_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_at28c64b1_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_at28c64b1_set_sdp(lt48_s8253_resources*,unsigned);

unsigned lt48_eep2041_3_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_eep2041_3_admitted_internal(void);
lt48_status lt48_eep2041_3_open(const lt48_installation*,size_t,lt48_s8253_resources**);

unsigned lt48_ramtron6_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_ramtron6_admitted_internal(void);
lt48_status lt48_ramtron6_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_ramtron6_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

int lt48_ramtron21_member_internal(const lt48_catalogue_entry*);

lt48_status lt48_ramtron6_verify_options(const lt48_s8253_resources*,lt48_operation_kind,lt48_verify_margin_selection,unsigned);

unsigned lt48_at91nine_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_at91nine_admitted_internal(void);
lt48_status lt48_at91nine_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_at91nine_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_t89six_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_t89six_admitted_internal(unsigned);
lt48_status lt48_t89six_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_t89six_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_sx30_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_sx30_admitted_internal(unsigned,unsigned);
lt48_status lt48_sx30_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sx30_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_mg15_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_mg15_admitted_internal(unsigned,unsigned);
lt48_status lt48_mg15_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_mg15_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_sn9_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_snas4_array_internal(unsigned definition);
int lt48_sn9_admitted_internal(unsigned,unsigned);
lt48_status lt48_sn9_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sn9_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_ob16_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_wt10_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_sm3_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_sm6_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_ob16_admitted_internal(unsigned,unsigned);
int lt48_wt10_admitted_internal(unsigned,unsigned);
int lt48_sm3_admitted_internal(unsigned,unsigned);
int lt48_sm6_admitted_internal(unsigned,unsigned);
lt48_status lt48_ob16_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_wt10_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sm3_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sm6_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_ob16_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_wt10_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_sm3_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_sm6_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_so30_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_so30_admitted_internal(unsigned,unsigned);
lt48_status lt48_so30_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_so30_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_wt8_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_wt8_admitted_internal(unsigned,unsigned);
lt48_status lt48_wt8_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_wt8_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_hy14_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_mw6_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_my7_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_my4_capacity_internal(unsigned,unsigned,unsigned,unsigned);
unsigned lt48_mw8_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_hy14_admitted_internal(unsigned,unsigned);
int lt48_mw6_admitted_internal(unsigned,unsigned);
int lt48_my7_admitted_internal(unsigned,unsigned);
int lt48_my4_admitted_internal(unsigned,unsigned);
int lt48_mw8_admitted_internal(unsigned,unsigned);
lt48_status lt48_hy14_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_mw6_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_my7_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_my4_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_mw8_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_hy14_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_mw6_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_my7_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_my4_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_mw8_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_gc8_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_gc8_admitted_internal(unsigned,unsigned);
lt48_status lt48_gc8_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_gc8_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_ab11_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_ab11_admitted_internal(unsigned,unsigned);
lt48_status lt48_ab11_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_ab11_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_su16_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_su16_admitted_internal(unsigned,unsigned);
lt48_status lt48_su16_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_su16_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_tp4_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_tp4_admitted_internal(unsigned,unsigned);
lt48_status lt48_tp4_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_tp4_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);


unsigned lt48_tp6_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_tp6_admitted_internal(unsigned,unsigned);
lt48_status lt48_tp6_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_tp6_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_sc2_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_sc2_admitted_internal(unsigned,unsigned);
lt48_status lt48_sc2_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sc2_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_sc4a_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_sc4a_admitted_internal(unsigned,unsigned);
lt48_status lt48_sc4a_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sc4a_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_sc4g_capacity_internal(unsigned,unsigned,unsigned,unsigned);
int lt48_sc4g_admitted_internal(unsigned,unsigned);
lt48_status lt48_sc4g_open(const lt48_installation*,size_t,lt48_s8253_resources**);
lt48_status lt48_sc4g_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);

unsigned lt48_white3_capacity_internal(unsigned,unsigned,unsigned,unsigned);
lt48_status lt48_white3_open(const lt48_installation*,size_t,lt48_s8253_resources**);

/* Offline finite ITE resource loader; public admission remains gated. */
lt48_status lt48_ite2_open(const lt48_installation*,size_t,lt48_s8253_resources**);

int lt48_ite2_admitted_internal(unsigned,unsigned);
lt48_status lt48_ite2_validate(const lt48_s8253_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
