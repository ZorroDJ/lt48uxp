#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Digest-bound AddTab byte SPI flash. Exact source segment tables and compact procedures. */
struct as_segment { uint32_t address,length,image_offset,step,page; };
struct as_field { unsigned category,index,policy,protected_value; };
struct as_closure {
 unsigned definition,family,pinmap,pins,capacity,configuration,erase_end,default_end,erase_verified_end,secure_kind;
 const struct as_segment *segments;size_t segment_count;
 const uint32_t *primary;size_t primary_count;
 const uint32_t *secondary;size_t secondary_count;
 const struct as_field *fields;size_t field_count;
 unsigned kind; /* 0 SPI NOR, 1 DataFlash, 2 I2C RRAM, 3 legacy DataFlash, 4 SST LPC. */
};
#include "addtab_spi_closures.inc"
#include "../../NewDeviceLib/sources/memreuse7/members.h"
#include "../../NewDeviceLib/sources/memreuse7/spi-profiles.h"
#include "../../NewDeviceLib/sources/sstfwh1/closures.inc"
#include "../../NewDeviceLib/sources/sstfwh1/host_policy.inc"
#include "../../NewDeviceLib/sources/sstfwh1/members.inc"
#include "../../NewDeviceLib/sources/dataflash28ts1/closures.inc"
#include "../../NewDeviceLib/sources/dataflash28ts1/members.inc"
#include "../../NewDeviceLib/sources/sst49c7/closures.inc"
#include "../../NewDeviceLib/sources/sst49c7/members.inc"
#include "../../NewDeviceLib/sources/spiw6/closures.inc"
#include "../../NewDeviceLib/sources/spiw6/members.inc"
#include "addtab_spi_admission.inc"
#include "adesto_closures.inc"
#include "adesto_admission.inc"
#include "aec1430_spi_closures.inc"
#include "aec1430_spi_admission.inc"
#include "../../NewDeviceLib/sources/aec1440-spi/closures.h"
#include "../../NewDeviceLib/sources/aec1440-spi/members.h"
#include "../../NewDeviceLib/sources/aec1440-spi/admission.h"
#include "../../NewDeviceLib/sources/spi-packages/closures.inc"
#include "../../NewDeviceLib/sources/spi-packages/admission.inc"
#include "../../NewDeviceLib/sources/spi-packages/members.inc"
#include "../../NewDeviceLib/sources/spi-micron40/closures.inc"
#include "../../NewDeviceLib/sources/spi-micron40/members.inc"
#include "../../NewDeviceLib/sources/spi-26/closures.inc"
#include "../../NewDeviceLib/sources/spi-26/members.inc"
#include "../../NewDeviceLib/sources/spi-26/micron_selector.inc"
#include "../../NewDeviceLib/sources/spi-23/closures.inc"
#include "../../NewDeviceLib/sources/spi-23/members.inc"
#include "../../NewDeviceLib/sources/spi-23/guards.inc"
#include "../../NewDeviceLib/sources/spi-23/xmcb_erase.inc"
#include "../../NewDeviceLib/sources/spi-54/closures.inc"
#include "../../NewDeviceLib/sources/spi-54/members.inc"
#include "../../NewDeviceLib/sources/spi-54/guards.inc"
#include "../../NewDeviceLib/sources/spi-mac29/closures.inc"
#include "../../NewDeviceLib/sources/spi-mac29/members.inc"
#include "../../NewDeviceLib/sources/spi-mac29/guards.inc"
#include "../../NewDeviceLib/sources/spi-mac29/tb_selector.inc"
#include "../../NewDeviceLib/sources/adesto/at45db642d_config_read.inc"
#include "../../NewDeviceLib/sources/dataflash28ts1/configuration_read.inc"
#include "../../NewDeviceLib/sources/cfeon/aec1430_status_selector.inc"
struct lt48_addtab_spi_resources {
    unsigned applet_count;
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[16];
    lt48_wave_code applet[10];
    lt48_applet_address addresses[10];
    const unsigned char *fpga;
    lt48_compile_variable vars[32];
    size_t var_count;
    uint32_t capacity;
    uint16_t man,dev,nominal;
    const struct as_closure *closure;
};
#include "../../NewDeviceLib/sources/spi-54/die_select.inc"
#include "../../NewDeviceLib/sources/spi-26/sector_unlock.inc"
void lt48_addtab_spi_close(lt48_addtab_spi_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
int lt48_addtab_spi_secure_is_permanent(const lt48_addtab_spi_resources *r){return r && (r->closure->secure_kind==2 || r->closure->secure_kind==3);}
unsigned lt48_addtab_spi_is_micron40(const lt48_addtab_spi_resources*r){if(r)for(size_t i=0;i<sizeof(mi_closures)/sizeof(*mi_closures);i++)if(r->closure==&mi_closures[i])return 1;return 0;}
unsigned lt48_addtab_spi_is_spi26(const lt48_addtab_spi_resources*r){if(r)for(size_t i=0;i<sizeof(sv_closures)/sizeof(*sv_closures);i++)if(r->closure==&sv_closures[i])return 1;return 0;}
unsigned lt48_addtab_spi_is_spi23(const lt48_addtab_spi_resources*r){if(r)for(size_t i=0;i<sizeof(nx_closures)/sizeof(*nx_closures);i++)if(r->closure==&nx_closures[i])return 1;return 0;}
unsigned lt48_addtab_spi_is_spi54(const lt48_addtab_spi_resources*r){if(r)for(size_t i=0;i<sizeof(gw_closures)/sizeof(*gw_closures);i++)if(r->closure==&gw_closures[i])return 1;return 0;}
unsigned lt48_addtab_spi_is_mac29(const lt48_addtab_spi_resources*r){if(r)for(size_t i=0;i<sizeof(mx_closures)/sizeof(*mx_closures);i++)if(r->closure==&mx_closures[i])return 1;return 0;}
unsigned lt48_addtab_spi_memory_kind(const lt48_addtab_spi_resources *r){return r?r->closure->kind:0;}
uint32_t lt48_addtab_spi_capacity(const lt48_addtab_spi_resources *r){return r?r->capacity:0;}
lt48_status lt48_addtab_spi_open(const lt48_installation *x,size_t device,lt48_addtab_spi_resources **out) {
    lt48_device_spec spec;lt48_family_info family;lt48_pin_code pc;
    lt48_addtab_spi_resources *r=NULL;lt48_status status;char path[4096],name[24],hex[65];
    size_t index,fi;unsigned char digest[32];
    if(!out||!x)return LT48_INVALID_ARGUMENT;
    *out=NULL;status=lt48_installation_resolve_device(x,device,&spec);if(status!=LT48_OK)return status;
    if(spec.catalogue.device_class!=2044 && spec.catalogue.device_class!=2041)return LT48_UNSUPPORTED;
    const struct as_closure *closure=NULL;
    int sr_binding=0;
    /* The opened catalogue is immutable. Classify it once per constructor,
     * rather than hashing it again for every candidate descriptor. */
    const char *source_profile=lt48_installation_source_profile(x);
    const int aec14=!strcmp(source_profile,"AEC 14.30");
    const int aec1440=!strcmp(source_profile,"AEC 14.40");
    const int dataman13=!strcmp(source_profile,"Dataman 13.10");
    if(aec1440) {
        if(!a1440_binding(&spec.catalogue))return LT48_UNSUPPORTED;
        for(size_t i=0;i<sizeof(a1440_closures)/sizeof(*a1440_closures);i++){
            const struct as_closure*c=a1440_closures+i;
            if(spec.definition_resource==c->definition&&spec.family_code==c->family&&spec.pinmap==c->pinmap&&spec.pin_count==c->pins&&spec.capacity_bytes==c->capacity&&spec.element_bits==8)closure=c;
        }
    } else if(aec14) {
        for(size_t i=0;i<sizeof(a14_closures)/sizeof(*a14_closures);i++) {
            const struct as_closure *c=&a14_closures[i];
            if(spec.catalogue.device_class==2044 && spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8)closure=c;
        }
    } else {
    for(size_t i=0;i<sizeof(as_closures)/sizeof(*as_closures);i++) {
        const struct as_closure *c=&as_closures[i];
        if(spec.definition_resource==c->definition && spec.family_code==c->family &&
           spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8)closure=c;
    }
    for(size_t i=0;i<sizeof(ad_closures)/sizeof(*ad_closures);i++) {
        const struct as_closure *c=&ad_closures[i];
        if(spec.definition_resource==c->definition && spec.family_code==c->family &&
           spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8)closure=c;
    }
    for(size_t i=0;i<sizeof(sr_closures)/sizeof(*sr_closures);i++) {
        const struct as_closure *c=&sr_closures[i];
        if(spec.catalogue.device_class==2044 && spec.definition_resource==c->definition && spec.family_code==c->family &&
           spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8) {
            for(size_t n=0;n<sizeof(sr_names)/sizeof(*sr_names);n++)
                if(!strcmp(spec.catalogue.vendor,sr_names[n].vendor) &&
                   strlen(sr_names[n].name)==spec.catalogue.device_name_length &&
                   !memcmp(spec.catalogue.device_name,sr_names[n].name,spec.catalogue.device_name_length) &&
                   spec.catalogue.manufacturer_id==sr_names[n].manufacturer && spec.catalogue.device_id==sr_names[n].device){closure=c;sr_binding=1;}
        }
    }
    for(size_t i=0;i<sizeof(mi_closures)/sizeof(*mi_closures);i++) {
        const struct as_closure*c=&mi_closures[i];
        if(dataman13 && spec.catalogue.device_class==2044 &&
           spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap &&
           spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8)
            for(size_t n=0;n<sizeof(mi_names)/sizeof(*mi_names);n++)
                if(!strcmp(spec.catalogue.vendor,mi_names[n].vendor) && strlen(mi_names[n].name)==spec.catalogue.device_name_length &&
                   !memcmp(spec.catalogue.device_name,mi_names[n].name,spec.catalogue.device_name_length) &&
                   spec.catalogue.manufacturer_id==mi_names[n].manufacturer && spec.catalogue.device_id==mi_names[n].device){closure=c;sr_binding=2;}
    }
    for(size_t i=0;i<sizeof(mx_closures)/sizeof(*mx_closures);i++) {
        const struct as_closure*c=&mx_closures[i];
        if(dataman13 && spec.catalogue.device_class==2044 &&
           spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap &&
           spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8)
            for(size_t n=0;n<sizeof(mx_names)/sizeof(*mx_names);n++)
                if(!strcmp(spec.catalogue.vendor,mx_names[n].vendor) && strlen(mx_names[n].name)==spec.catalogue.device_name_length &&
                   !memcmp(spec.catalogue.device_name,mx_names[n].name,spec.catalogue.device_name_length) &&
                   spec.catalogue.manufacturer_id==mx_names[n].manufacturer && spec.catalogue.device_id==mx_names[n].device){closure=c;sr_binding=3;}
    }
    for(size_t i=0;i<sizeof(gw_closures)/sizeof(*gw_closures);i++) {
        const struct as_closure*c=&gw_closures[i];
        if(dataman13 && spec.catalogue.device_class==2044 &&
           spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap &&
           spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8)
            for(size_t n=0;n<sizeof(gw_names)/sizeof(*gw_names);n++)
                if(!strcmp(spec.catalogue.vendor,gw_names[n].vendor) && strlen(gw_names[n].name)==spec.catalogue.device_name_length &&
                   !memcmp(spec.catalogue.device_name,gw_names[n].name,spec.catalogue.device_name_length) &&
                   spec.catalogue.manufacturer_id==gw_names[n].manufacturer && spec.catalogue.device_id==gw_names[n].device){closure=c;sr_binding=4;}
    }
    for(size_t i=0;i<sizeof(nx_closures)/sizeof(*nx_closures);i++) {
        const struct as_closure*c=&nx_closures[i];
        if(dataman13 && spec.catalogue.device_class==2044 &&
           spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap &&
           spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8)
            for(size_t n=0;n<sizeof(nx_names)/sizeof(*nx_names);n++)
                if(!strcmp(spec.catalogue.vendor,nx_names[n].vendor) && strlen(nx_names[n].name)==spec.catalogue.device_name_length &&
                   !memcmp(spec.catalogue.device_name,nx_names[n].name,spec.catalogue.device_name_length) &&
                   spec.catalogue.manufacturer_id==nx_names[n].manufacturer && spec.catalogue.device_id==nx_names[n].device){closure=c;sr_binding=5;}
    }
    for(size_t i=0;i<sizeof(sv_closures)/sizeof(*sv_closures);i++) {
        const struct as_closure*c=&sv_closures[i];
        if(dataman13 && spec.catalogue.device_class==2044 &&
           spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap &&
           spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8)
            for(size_t n=0;n<sizeof(sv_names)/sizeof(*sv_names);n++)
                if(!strcmp(spec.catalogue.vendor,sv_names[n].vendor) && strlen(sv_names[n].name)==spec.catalogue.device_name_length &&
                   !memcmp(spec.catalogue.device_name,sv_names[n].name,spec.catalogue.device_name_length) &&
                   spec.catalogue.manufacturer_id==sv_names[n].manufacturer && spec.catalogue.device_id==sv_names[n].device){closure=c;sr_binding=6;}
    }
    if(!aec14&&spiw6_binding(&spec.catalogue))for(size_t i=0;i<sizeof sw_closures/sizeof*sw_closures;i++){const struct as_closure*c=sw_closures+i;if(spec.definition_resource==c->definition&&spec.family_code==c->family&&spec.pinmap==c->pinmap&&spec.pin_count==c->pins&&spec.capacity_bytes==c->capacity&&spec.element_bits==8){closure=c;sr_binding=7;}}
    if(dataman13&&sst49c7_binding(&spec.catalogue))for(size_t i=0;i<sizeof sc_closures/sizeof*sc_closures;i++){const struct as_closure*c=sc_closures+i;if(spec.definition_resource==c->definition&&spec.family_code==c->family&&spec.pinmap==c->pinmap&&spec.pin_count==c->pins&&spec.capacity_bytes==c->capacity&&spec.element_bits==8){closure=c;sr_binding=8;}}
    if(dataman13&&df28_binding(&spec.catalogue)){const struct as_closure*c=df28_closures;if(spec.definition_resource==c->definition&&spec.family_code==c->family&&spec.pinmap==c->pinmap&&spec.pin_count==c->pins&&spec.capacity_bytes==c->capacity&&spec.element_bits==8){closure=c;sr_binding=9;}}
    if(dataman13&&sstfwh1_binding(&spec.catalogue)){const struct as_closure*c=fw_closures;if(spec.definition_resource==c->definition&&spec.family_code==c->family&&spec.pinmap==c->pinmap&&spec.pin_count==c->pins&&spec.capacity_bytes==c->capacity&&spec.element_bits==8){closure=c;sr_binding=10;}}
    if(dataman13&&mr7_member(&spec.catalogue))for(size_t i=0;i<sizeof mr7_spis/sizeof*mr7_spis;i++){const struct as_closure*c=mr7_spis+i;if(spec.definition_resource==c->definition&&spec.family_code==c->family&&spec.pinmap==c->pinmap&&spec.pin_count==c->pins&&spec.capacity_bytes==c->capacity&&spec.element_bits==8){closure=c;sr_binding=11;}}
    if(spec.catalogue.device_class==2041) {
        static const struct as_segment segment={0,8650752,0,1,1056};
        static const uint32_t bound=0;
        static const struct as_closure old={0x20,0x17,0x0e,40,8650752,0,8650752,8650752,8650752,0,&segment,1,&bound,1,&bound,1,NULL,0,3};
        closure=spec.definition_resource==old.definition && spec.family_code==old.family && spec.pinmap==old.pinmap && spec.pin_count==old.pins && spec.capacity_bytes==old.capacity && spec.element_bits==8?&old:NULL;
    }
    }
    if(!closure)return LT48_UNSUPPORTED;
#ifndef LT48_ADDTAB_SPI_AUDIT_ALL
    int admitted=0;
    if(aec1440)admitted=LT48_AEC1440_ADMITTED;
    else if(aec14) {
        for(size_t i=0;i<sizeof(a14_admitted)/sizeof(*a14_admitted);i++)
            if(closure->definition==a14_admitted[i])admitted=1;
    } else {
    for(size_t i=0;i<sizeof(as_admitted)/sizeof(*as_admitted);i++)
        if(closure->definition==as_admitted[i])admitted=1;
    for(size_t i=0;i<sizeof(ad_admitted)/sizeof(*ad_admitted);i++)
        if(closure->definition==ad_admitted[i])admitted=1;
    for(size_t i=0;i<sizeof(sr_admitted)/sizeof(*sr_admitted);i++)
        if(sr_binding==1 && closure->definition==sr_admitted[i])admitted=1;
    }
    if(sr_binding==2||sr_binding==3||sr_binding==4||sr_binding==5||sr_binding==6)admitted=1;
    if(sr_binding==7&&LT48_SPIW6_ADMITTED)admitted=1;
    if(sr_binding==8||sr_binding==9||sr_binding==10)admitted=1;
    if(sr_binding==11&&LT48_MR7_ADMITTED)admitted=1;
    if(!admitted)return LT48_UNSUPPORTED;
#endif
    int n=snprintf(path,sizeof(path),"%s/%s",lt48_installation_path(x),closure->kind==3?"GDBA___.rez":"GDCA___.rez");
    if(n<0||(size_t)n>=sizeof(path))return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
#define TRY(a) do{status=(a);if(status!=LT48_OK){if(getenv("LABTOOL48_ADDTAB_SPI_TRACE"))fprintf(stderr,"addtab open line %d status %d\n",__LINE__,status);goto fail;}}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,aec1440?"702936863173924e260be39d597cd7ebbc35a8d5c811fbb7561234191a5f9835":aec14?"725a6bdefcae8aecc4f290e87f8c337eb405e677c2ed1df0ed8993c8091b12cf":closure->kind==3?"4129e4261b9eeb7a4b7f4272d99b3e8d4ec45e41f9150a86baa57f909e8fc7ff":"5b391bb2789baa1496eb5db708c3d867b8e25ec2b01dc7ea030eb9c9a84e5a39")){status=LT48_SAFETY_REJECTED;goto fail;}
    if(closure->field_count>256){status=LT48_FORMAT_ERROR;goto fail;}
    r->applet_count=0;r->nominal=closure->kind==4?330:closure->kind==2?320:310;r->closure=closure;
    r->capacity=spec.capacity_bytes;r->man=spec.catalogue.manufacturer_id;r->dev=spec.catalogue.device_id;
    snprintf(name,sizeof(name),"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    /* GDCA Kernel declares seven zero-initialized control groups. */
    TRY(lt48_pin_map_complete_groups_internal(r->pins,7));
    snprintf(name,sizeof(name),"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(family.variable_count>32 || family.variable_count<3 || family.operation_count<(closure->kind==3?11u:closure->kind==2?13u:18u) || family.operation_count>24){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;
    for(size_t i=0;i<r->var_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
    }
    if(r->vars[0].variable_id!=0 || r->vars[0].value!=r->nominal){status=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_installation_apply_new_device_variables(x,device,r->vars,r->var_count));
    /* An admitted source with inconsistent region geometry needs its explicit
       NewDeviceLib correction. Never silently use the vendor's bad boundary. */
    for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==16) {
        uint32_t boundary=(uint32_t)r->vars[i].value|((uint32_t)r->vars[i].channel<<16);
        int aligned=boundary==r->capacity;
        for(size_t j=0;j<closure->segment_count;j++)if(boundary==closure->segments[j].image_offset)aligned=1;
#ifndef LT48_ADDTAB_SPI_AUDIT_ALL
        if(!aligned){status=LT48_UNSUPPORTED;goto fail;}
#else
        (void)aligned;
#endif
    }
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
#include "../../NewDeviceLib/sources/spi-54/fpga_select.inc"
        if(code.wave_type==0 && op.operation_id<16) {
            lt48_compact_wave *w=NULL;unsigned char *corrected=NULL;
            if(sr_binding==9&&op.operation_id==6){
                if(op.wave!=0x177){status=LT48_SAFETY_REJECTED;goto fail;}
                corrected=malloc(code.data_size);if(!corrected){status=LT48_NO_MEMORY;goto fail;}
                memcpy(corrected,code.data,code.data_size);status=df28_readcfg(corrected,&code.data_size);
                if(status!=LT48_OK){free(corrected);goto fail;}
            }
            if(closure->kind==1 && closure->definition==0x26 && op.operation_id==6) {
                if(op.wave!=0x8b){status=LT48_SAFETY_REJECTED;goto fail;}
                corrected=malloc(code.data_size);if(!corrected){status=LT48_NO_MEMORY;goto fail;}
                memcpy(corrected,code.data,code.data_size);
                status=adesto_correct_parallel_config(corrected,&code.data_size);
                if(status!=LT48_OK){free(corrected);goto fail;}
            }
            if(aec14 && closure->definition>=0x1e6 && closure->definition<=0x1e9 && op.operation_id==6) {
                if(op.wave!=(closure->definition>=0x1e8?0x481u:0x480u)){status=LT48_SAFETY_REJECTED;goto fail;}
                corrected=malloc(code.data_size+6);if(!corrected){status=LT48_NO_MEMORY;goto fail;}
                memcpy(corrected,code.data,code.data_size);
                status=aec1430_status_selector(corrected,&code.data_size);
                if(status!=LT48_OK){free(corrected);goto fail;}
            }
            if(sr_binding==3 && closure->configuration==25435 && op.operation_id==11) {
                if(op.wave!=0x24f){status=LT48_SAFETY_REJECTED;goto fail;}
                corrected=malloc(code.data_size);if(!corrected){status=LT48_NO_MEMORY;goto fail;}
                memcpy(corrected,code.data,code.data_size);status=mx_correct_tb(corrected,code.data_size);
                if(status!=LT48_OK){free(corrected);goto fail;}
            }
            if(sr_binding==4 && closure->configuration==25409 &&
               (op.operation_id==2 || op.operation_id==5 || op.operation_id==7 || op.operation_id==8 || op.operation_id==9 || op.operation_id==10)) {
                corrected=malloc(code.data_size);if(!corrected){status=LT48_NO_MEMORY;goto fail;}
                memcpy(corrected,code.data,code.data_size);status=gw_correct_die_frame(corrected,code.data_size);
                if(status!=LT48_OK){free(corrected);goto fail;}
            }
            if(sr_binding==5 && (closure->definition==0xe4 || closure->definition==0xe5) && op.operation_id==10){
                if(op.wave!=0x23f){status=LT48_SAFETY_REJECTED;goto fail;}
                corrected=malloc(code.data_size+sizeof(nx_xmcb_body));if(!corrected){status=LT48_NO_MEMORY;goto fail;}
                memcpy(corrected,code.data,code.data_size);status=nx_correct_xmcb(corrected,&code.data_size);
                if(status!=LT48_OK){free(corrected);goto fail;}
            }
            if(sr_binding==6 && sv_is_micron(closure->definition) && (op.operation_id==5 || op.operation_id==6 || op.operation_id==7 || op.operation_id==9 || op.operation_id==11)) {
                corrected=malloc(code.data_size+64);if(!corrected){status=LT48_NO_MEMORY;goto fail;}
                memcpy(corrected,code.data,code.data_size);
                status=sv_correct_otp_selector(corrected,&code.data_size,op.operation_id);
                if(status!=LT48_OK){free(corrected);goto fail;}
            }
            status=lt48_compact_decode(corrected?corrected:code.data,code.data_size,&w);
            free(corrected);if(status!=LT48_OK)goto fail;
            status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,
                LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);
            lt48_compact_wave_close(w);if(status!=LT48_OK){if(getenv("LABTOOL48_ADDTAB_SPI_TRACE"))fprintf(stderr,"compile op %u status %d\n",op.operation_id,status);goto fail;}
        } else if(code.wave_type==6) {
            unsigned slot=op.operation_id==17?0:op.operation_id==19?1:op.operation_id==20?2:op.operation_id==21?3:op.operation_id==16?4:op.operation_id==18?5:op.operation_id==23?6:(op.operation_id==26||(sr_binding==7&&op.operation_id==15))?7:op.operation_id==24?8:op.operation_id==25?9:10;
            static const uint16_t addr[]={0x6000,0x7800,0x8000,0x7000,0x5000,0x5800,0x9000,0xa000,0x9800,0xa800};
            if(r->applet_count>=10 || slot==10 || (code.load_address!=addr[slot] && !(op.operation_id==24 && code.load_address==0xa000) && !(op.operation_id==25 && (code.load_address==0x9800 || code.load_address==0x9a00))) || code.data_size==0 ||
               code.data_size>0x800 || code.length1!=code.data_size+4 || code.length2!=code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            unsigned target=0;while(target<r->applet_count && r->addresses[target].wave!=op.operation_id)target++;
            r->applet[target]=code;r->addresses[target]=(lt48_applet_address){op.operation_id,code.load_address};
            if(target==r->applet_count)r->applet_count++;
        } else if(code.wave_type==4 && (op.operation_id==30 || (closure->kind==3 && op.operation_id==27)) && code.data_size==2780 &&
                  code.data[0]==0xda && code.data[1]==0x0a)r->fpga=code.data+2;
        else {status=LT48_UNSUPPORTED;goto fail;}
    }
    if(sr_binding==6 && closure->secure_kind==5)TRY(sv_compile_unlock(r));
    if(sr_binding==4 && closure->configuration==25409)TRY(gw_build_die_select(r));
    if(r->applet_count<(closure->kind==3?5u:closure->kind==2?4u:7u)){status=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<r->applet_count;i++)if(!r->applet[i].data){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->wave[0]||!r->wave[1]||!r->wave[2]||!r->wave[3]||(!closure->kind&&!r->wave[4])|| (closure->kind!=2&&!r->wave[10])){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->fpga){status=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:if(getenv("LABTOOL48_ADDTAB_SPI_TRACE"))fprintf(stderr,"open final status %d\n",status);lt48_addtab_spi_close(r);return status;
#undef TRY
}
lt48_status lt48_addtab_spi_validate(const lt48_addtab_spi_resources *r,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r||!length||start>=r->capacity||length>r->capacity-start)return LT48_SAFETY_REJECTED;
    if(op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE&&(!image||size!=length))return LT48_INVALID_ARGUMENT;

    if((op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE) && (start!=0||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM) {
        /* The DataFlash program applet starts its page counter at zero.
         * Only complete segments can be passed to that stream ABI. */
        if(r->closure->kind==1 && (start!=0 || (length!=r->closure->segments[0].length && length!=r->capacity)))return LT48_SAFETY_REJECTED;
        size_t remaining=length;uint32_t logical=start;
        for(size_t i=0;i<r->closure->segment_count && remaining;i++) {
            const struct as_segment *p=&r->closure->segments[i];
            if(logical<p->image_offset || logical-p->image_offset>=p->length)continue;
            uint32_t delta=logical-p->image_offset;size_t n=p->length-delta;if(n>remaining)n=remaining;
            if(delta%p->page || (n%p->page && !(delta==0 && n==p->length && n<p->page)))return LT48_SAFETY_REJECTED;
            logical+=(uint32_t)n;remaining-=n;
        }
        if(remaining)return LT48_SAFETY_REJECTED;
    }
    if(op==LT48_OPERATION_SECURE && (!r->wave[11] || !r->closure->secure_kind))return LT48_UNSUPPORTED;
    return LT48_OK;
}
lt48_status lt48_addtab_spi_validate_verify_options(const lt48_addtab_spi_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned margin_type) {
    (void)op;
    if(!r||margin_type>1)return LT48_INVALID_ARGUMENT;
    if(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&
       margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
static uint16_t margin_voltage(const lt48_addtab_spi_resources *r,unsigned type,int high) {
    /* Original x87 rounding: nearest integer, ties to even (310 cV). */
    unsigned scaled=r->nominal*(high?(type?110:105):(type?90:95));
    unsigned q=scaled/100,rem=scaled%100;
    return (uint16_t)(q+(rem>50 || (rem==50 && (q&1))));
}

typedef struct as_session {
    const lt48_addtab_spi_resources *r;
    const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[32][32];uint32_t volts[4];
    lt48_compile_variable vars[32];uint32_t mode;unsigned die_override;
    lt48_runtime_configuration_value config[256];size_t config_count;
} as_session;
static lt48_status run(as_session *s,unsigned op) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0 && (s->vars[i].value<270 || s->vars[i].value>360))return LT48_SAFETY_REJECTED;
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[31];size_t count=0;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    for(uint8_t i=0;i<32;i++)if(i!=(s->r->closure->kind==3?7:8))seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    lt48_runtime_nested_wave nested[4];size_t nested_count=0;
    if(s->r->wave[11])nested[nested_count++]=(lt48_runtime_nested_wave){11,s->r->wave[11]};
    if(s->r->closure->kind==1 && s->r->wave[3])nested[nested_count++]=(lt48_runtime_nested_wave){3,s->r->wave[3]};
    if(s->r->closure->kind==2)for(unsigned id=0;id<2;id++)
        if(s->r->wave[id])nested[nested_count++]=(lt48_runtime_nested_wave){id,s->r->wave[id]};
    options.nested_waves=nested;options.nested_wave_count=nested_count;
    options.configuration=s->config;options.configuration_count=s->config_count;
    options.mode=s->mode;options.use_explicit_buffer_ids=1;options.mode_buffer=s->r->closure->kind==3?7:8;
    options.address_buffer=0;options.parameter_buffer_id=s->r->closure->kind==3?2:3;options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));options.root_timeout_seconds=30;options.host_staged_shutdown=fw_shutdown_policy(s->r->closure);
    /* Erase's source tES DWORD is divided by 1,000,000 by the compact wave.
     * Include each possible array/security-region wait and host overhead. */
    if(op==10)for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==9) {
        uint32_t us=(uint32_t)s->vars[i].value|((uint32_t)s->vars[i].channel<<16);
        options.root_timeout_seconds+=(us/1000000u+1u)*(uint32_t)s->r->closure->segment_count;
    }
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(s->wire,options.root_timeout_seconds*1000u,1);
    if(status==LT48_OK)status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->applet_count,&result);
    if(status==LT48_OK) {
        for(uint8_t i=0;i<32;i++) {
            const unsigned char *data;size_t size;status=lt48_runtime_get_buffer(result,i,&data,&size);
            if(status!=LT48_OK||size!=32){status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],data,32);
        }
        if(status==LT48_OK) {
            const lt48_runtime_configuration_value *values;size_t n;
            status=lt48_runtime_get_configuration(result,&values,&n);
            if(status==LT48_OK && n>256)status=LT48_RUNTIME_ERROR;
            if(status==LT48_OK){memcpy(s->config,values,n*sizeof(*values));s->config_count=n;}
        }
        for(uint8_t i=0;status==LT48_OK&&i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event event;lt48_runtime_get_event(result,i,&event);
            if(event.kind==LT48_RUNTIME_RESULT_LATCH ||
               (event.kind==LT48_RUNTIME_RETURN_STATUS && event.argument_count==1 && event.arguments[0]==0))status=LT48_RUNTIME_ERROR;
        }
    }
    if(getenv("LABTOOL48_ADDTAB_SPI_TRACE"))fprintf(stderr,
        "addtab wave=%u mode=%u status=%d address=%02x%02x%02x%02x data=%02x retry=%u volts=%u,%u,%u,%u\n",
        op,s->mode,status,s->buffers[0][3],s->buffers[0][2],s->buffers[0][1],s->buffers[0][0],
        s->buffers[3][0],s->buffers[15][0],s->volts[0],s->volts[1],s->volts[2],s->volts[3]);
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);
    if(status==LT48_OK && op==2 && s->die_override)status=run(s,12+s->die_override);
    return status;
}
static lt48_status transfer(as_session *s,int input,uint8_t endpoint,unsigned char *bytes,size_t length) {
    size_t done=0;
    if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
    lt48_status status=input?s->api->read(s->context,endpoint,bytes,length,&done):s->api->write(s->context,endpoint,bytes,length,&done);
    if(status==LT48_OK&&done!=length)status=LT48_PROTOCOL_ERROR;
    return status;
}
static lt48_status cmd(as_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    unsigned char packet[512]={0};if(n>512)return LT48_INVALID_ARGUMENT;
    memcpy(packet,p,n);lt48_status status=lt48_runtime_transport_session_flush(s->wire);
    if(status==LT48_OK)status=transfer(s,0,2,packet,512);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static lt48_status flag(as_session *s,unsigned opcode,unsigned *value) {
    unsigned char p[2]={(unsigned char)opcode,1},reply[512]={0};
    lt48_status status=cmd(s,p,opcode==0x0b?1:2,reply);
    if(status==LT48_OK && !(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK)*value=reply[2]&1;
    return status;
}
static lt48_status block(as_session *s,unsigned opcode,unsigned char *data,size_t n,unsigned char *reply) {
    unsigned char p[6]={(unsigned char)opcode,(unsigned char)(n/512),(unsigned char)(n/512>>8),
        (unsigned char)(n%512),(unsigned char)(n%512>>8),opcode==0x48?1:0};
    lt48_status status=cmd(s,p,6,NULL);
    if(status==LT48_OK)status=transfer(s,opcode==0x43,opcode==0x43?0x88:4,data,n);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static void set32(unsigned char *,uint32_t);
static lt48_status setup(as_session *s) {
    /* Native bridge initialization already establishes the transport. FX2
     * opcode 0x18 is reserved (RET at CODE:0a1f), not a reset with a reply. */
    lt48_status status=LT48_OK;
    /* Original 43d580 visits operation table slots, not source wave numbers. */
    for(unsigned i=0;status==LT48_OK&&i<s->r->applet_count;i++) {
        const lt48_wave_code *a=&s->r->applet[i];size_t last=a->data_size-1;
        unsigned char header[]={0x39,0xaa,0x55,(unsigned char)a->load_address,(unsigned char)(a->load_address>>8),(unsigned char)last,(unsigned char)(last>>8)};
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,header,sizeof(header));
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,a->data,a->data_size);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
    }
    const unsigned char fpga[]={0x38,0xaa,0x55},selector[]={0x0f,15},busy[]={0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,fpga,sizeof(fpga));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,s->r->fpga,2778);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,selector,sizeof(selector));
    if(status==LT48_OK)status=run(s,0);
    s->mode=0;set32(s->buffers[2],s->r->capacity);
    if(status==LT48_OK)status=run(s,2);
    if(status==LT48_OK)status=lt48_runtime_transport_session_status(s->wire,1);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof(busy));
    return status;
}
static lt48_status stream(as_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_addtab_spi_result *out) {
    (void)vcc;
    lt48_status status=LT48_OK;size_t chunk=8192;
    out->completed_bytes=0;
    for(size_t offset=0;status==LT48_OK&&offset<length;) {
        size_t n=length-offset;if(n>chunk)n=chunk;
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,mode==1?5000:mode==11?10000:60000,0);
        if(status!=LT48_OK)break;
        if(mode==1)status=block(s,0x43,read+offset,n,NULL);
        else if(mode==11) {
            unsigned char count[2]={(unsigned char)(n),(unsigned char)(n>>8)};unsigned passed=0;
            status=block(s,0x44,count,2,NULL);
            if(status==LT48_OK)status=flag(s,0x0b,&passed);
            if(status==LT48_OK&&!passed) {
                unsigned char remaining[2]={0};status=block(s,0x43,remaining,2,NULL);
                size_t rem=remaining[0]|(size_t)remaining[1]<<8;
                if(status==LT48_OK) {
                    if(rem>=n)status=LT48_PROTOCOL_ERROR;
                    else {out->first_mismatch=start+(uint32_t)(offset+n-rem-1);out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}
                }
            }
        } else {
            unsigned char reply[512]={0};status=block(s,0x48,(unsigned char*)image+offset,n,reply);
            if(status==LT48_OK&&!(reply[0]|reply[1])) {
                /* The original derives an unchecked remaining count here.
                 * Never resume programming from untrusted failure counters. */
                /* Original error callback 004689e0 drains one actual byte
                 * through transport +24 (00496850) before stopping the applet. */
                unsigned char actual[1];
                status=block(s,0x43,actual,sizeof(actual),NULL);
                out->mismatch_count++;out->first_mismatch=UINT32_MAX;
                if(status==LT48_OK)status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK&&offset+n==length) {
                unsigned passed=0;status=flag(s,0x0c,&passed);
                if(status==LT48_OK&&!passed) {
                    unsigned char actual[1];
                    status=block(s,0x43,actual,sizeof(actual),NULL);
                    out->mismatch_count++;
                    out->first_mismatch=start+(uint32_t)(length-1);
                    if(status==LT48_OK)status=LT48_DEVICE_MISMATCH;
                }
            }
        }
        if(status==LT48_OK){offset+=n;out->completed_bytes=offset;}
    }
    /* DEVRESET must be sent even when the bulk worker failed. The final
     * uncancellable owner below also provides an independent reset path. */
    if(status==LT48_OK) {
        const unsigned char finish[]={0,0};unsigned char reply[512]={0};
        status=cmd(s,finish,sizeof(finish),reply);
        if(status==LT48_OK&&!(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    }
    return status;
}
static void set32(unsigned char *b,uint32_t value) {
    for(unsigned j=0;j<4;j++)b[j]=(unsigned char)(value>>(8*j));
}
static lt48_status cycle_source(as_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_addtab_spi_result *out) {
    s->mode=mode;
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    set32(s->buffers[2],s->r->capacity);
    set32(s->buffers[0],s->r->closure->kind==3?(start/1056u)*2048u+start%1056u:start);
    lt48_status status=run(s,2);size_t done=0;
    while(status==LT48_OK && done<length) {
        uint32_t logical=start+(uint32_t)done;const struct as_segment *segment=NULL;
        for(size_t i=0;i<s->r->closure->segment_count;i++) {
            const struct as_segment *p=&s->r->closure->segments[i];
            if(logical>=p->image_offset && logical-p->image_offset<p->length){segment=p;break;}
        }
        if(!segment)return LT48_SAFETY_REJECTED;
        uint32_t delta=logical-segment->image_offset;
        size_t n=length-done;if(n>segment->length-delta)n=segment->length-delta;
        if(mode==2 && s->r->closure->kind!=1 && s->r->closure->kind!=3 && s->r->closure->kind!=4 && n>segment->page)n=segment->page;
        uint32_t physical=segment->address+delta;
        if(s->r->closure->kind==1 && segment->image_offset==0) {
            unsigned stride=1;while(stride<segment->page)stride*=2;
            physical=(delta/segment->page)*stride+delta%segment->page;
        }
        set32(s->buffers[0],physical);
        /* These source callbacks change die/security mode only at an exact
         * segment boundary. Re-establish that known entry state for partial
         * operations, while buffer0 retains the requested physical address. */
        set32(s->buffers[1],(lt48_addtab_spi_is_spiw6_internal(s->r)||s->r->closure->kind==4)?segment->image_offset:logical);set32(s->buffers[9],segment->step);
        if(s->r->wave[13])status=run(s,13+(logical>=16777216u));
        if(status==LT48_OK && s->r->closure->kind!=3)status=run(s,mode==1?5:mode==11?7:mode==2?8:9);
        if(status==LT48_OK)status=stream(s,mode,logical,n,image?image+done:NULL,read?read+done:NULL,vcc,out);
        if(status==LT48_OK)done+=n;
    }
    if(status==LT48_OK)status=run(s,3);
    out->completed_bytes=done;return status;
}
#include "../../NewDeviceLib/sources/sst49c7/range.inc"
/* Flash validation must observe actual bytes, including the final byte. A
 * controller success bit alone was insufficient on a disconnected adapter. */
static lt48_status checked_cycle(as_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *pending,uint16_t vcc,lt48_addtab_spi_result *out) {
    lt48_status status=cycle(s,mode,start,length,image,NULL,vcc,out);
    if(status==LT48_OK) {
        status=cycle(s,1,start,length,NULL,pending,vcc,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=image[i]) {
            if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    }
    return status;
}
static lt48_status identify(as_session *s,lt48_m37_id_decision_fn decide,void *context) {
    if(!s->r->wave[4])return LT48_OK;
    for(unsigned attempt=0;attempt<64;attempt++) {
        memset(s->buffers[5],0,32);memset(s->buffers[7],0,32);
        lt48_status status=run(s,4);if(status!=LT48_OK)return status;
        unsigned man=s->buffers[7][0]|(unsigned)s->buffers[7][1]<<8;
        unsigned dev=s->buffers[5][0]|(unsigned)s->buffers[5][1]<<8;
        int match=(s->r->man||s->r->dev)?man==s->r->man&&dev==s->r->dev:
            (man==dev && (man==0 || man==255));
        if(match)return LT48_OK;
        lt48_m37_id_choice choice=decide?decide(context,s->r->man,s->r->dev,man,dev):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)return LT48_OK;
        if(choice!=LT48_M37_ID_RETRY)return LT48_DEVICE_MISMATCH;
    }
    return LT48_SAFETY_REJECTED;
}
#include "../../NewDeviceLib/sources/spi-26/sector_owner.inc"
/* Read actual option values first. Only explicit protection fields change;
 * unrelated settings (including factory/OTP locks) retain their read values. */
static lt48_status protection_single(as_session *s,int secure) {
    const struct as_closure *p=s->r->closure;
    if(!p->secure_kind || (!secure && p->secure_kind==2))return LT48_OK;
    if(p->secure_kind==5)return sv_sector_protection(s,secure);
    if(p->secure_kind==3) {
        if(p->definition<0x7d || p->definition>0x81)return LT48_UNSUPPORTED;
        /* AT25DF source W152 performs irreversible 64-KiB sector lockdown.
         * Buffer4 is the original host's selected-sector boolean. ReadCfg
         * W14e returns the actual lockdown byte in buffer3, not a dialog field.
         * Programming/erase use their own source unprotect sequence; they
         * must never issue the irreversible Secure procedure implicitly. */
        if(!secure)return LT48_OK;
        s->mode=13;
        lt48_status st=run(s,2);
        for(size_t i=0;st==LT48_OK && i<p->secondary_count;i++) {
            set32(s->buffers[0],p->secondary[i]);set32(s->buffers[4],1);
            st=run(s,11);
            if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
            if(st==LT48_OK)st=run(s,6);
            if(st==LT48_OK && s->buffers[3][0]!=255)st=LT48_DEVICE_MISMATCH;
        }
        if(st==LT48_OK)st=run(s,3);
        return st;
    }
    s->mode=0;set32(s->buffers[0],0);set32(s->buffers[2],s->r->capacity);
    lt48_status status=run(s,2);if(status==LT48_OK)status=run(s,6);
    if(status==LT48_OK)status=mx_secure_guard(p,secure,s->config,s->config_count);
    if(status==LT48_OK&&lt48_addtab_spi_is_spiw6_internal(s->r)&&p->definition>=0x133)for(size_t i=0;i<s->config_count;i++)if(s->config[i].category==0&&(s->config[i].field==8||s->config[i].field==264)&&s->config[i].value)status=LT48_SAFETY_REJECTED;
    if(status==LT48_OK)status=nx_secure_guard(p,secure,s->config,s->config_count);
    if(status==LT48_OK)status=gw_secure_guard(p,secure,s->config,s->config_count);
    if(status!=LT48_OK)return status;
    for(size_t i=0;i<p->field_count;i++) {
        const struct as_field *f=&p->fields[i];int found=0;
        for(size_t j=0;j<s->config_count;j++)if(s->config[j].category==f->category && s->config[j].field==f->index) {
            found=1;
            if(f->policy==1)s->config[j].value=secure?f->protected_value:0;
            if(f->policy==2)s->config[j].value=0;
        }
        if(!found)return LT48_RUNTIME_ERROR;
    }
    lt48_runtime_configuration_value expected[256];size_t count=s->config_count;
    memcpy(expected,s->config,count*sizeof(*expected));
    /* ReadCfg/Protect can leave OTP/status-bank mode selected. The Windows
     * operation boundary resets that mode before the next procedure. */
    status=run(s,3);
    s->mode=13;
    if(status==LT48_OK)status=run(s,2);
    if(status==LT48_OK)status=run(s,11);
    if(status==LT48_OK)status=run(s,3);
    s->mode=0;
    if(status==LT48_OK)status=run(s,2);
    if(status==LT48_OK)status=run(s,6);
    for(size_t i=0;status==LT48_OK && i<count;i++) {
        int found=0;
        for(size_t j=0;j<s->config_count;j++)if(expected[i].object_id==s->config[j].object_id && expected[i].category==s->config[j].category && expected[i].field==s->config[j].field && expected[i].value==s->config[j].value)found=1;
        if(!found)status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)status=run(s,3);
    return status;
}
static lt48_status sc_unlock(as_session*s){
 if(s->r->closure->kind!=4)return LT48_OK;
 s->mode=15;set32(s->buffers[0],0);lt48_status st=run(s,2);
 for(size_t i=0;st==LT48_OK&&i+1<s->r->closure->primary_count;i++){
  set32(s->buffers[0],s->r->closure->primary[i]);st=run(s,14);
  if(st==LT48_OK&&s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
 }
 if(st==LT48_OK)st=run(s,3);
 return st;
}
static lt48_status protection(as_session*s,int secure){
 if(!s->r->wave[13])return protection_single(s,secure);
 lt48_status st=LT48_OK;
 for(unsigned die=0;st==LT48_OK&&die<2;die++){s->die_override=die+1;st=protection_single(s,secure);}
 s->die_override=0;return st;
}
#include "../../NewDeviceLib/sources/spi-26/readonly_id.inc"
#include "../../NewDeviceLib/sources/spiw6/mutation-guard.inc"
lt48_status lt48_addtab_spi_execute(const lt48_addtab_spi_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_addtab_spi_result *out) {
    if(!out||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_addtab_spi_validate(r,op,start,length,image,size);
    if(status!=LT48_OK)return status;
    if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
    status=lt48_addtab_spi_validate_verify_options(r,op,margin,margin_type);
    if(status!=LT48_OK)return status;
    unsigned char *pending=malloc(length),*preserved=NULL;if(!pending)return LT48_NO_MEMORY;
    if(op==LT48_OPERATION_SECURE || (op==LT48_OPERATION_ERASE && r->closure->erase_verified_end<r->capacity)) {
        preserved=malloc(length);if(!preserved){free(pending);return LT48_NO_MEMORY;}
        lt48_addtab_spi_result before;
        status=lt48_addtab_spi_execute(r,api,context,LT48_OPERATION_READ,start,length,NULL,0,preserved,length,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,&before);
        if(status!=LT48_OK){*out=before;free(preserved);free(pending);return status;}
    }
    as_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;memcpy(s.vars,r->vars,sizeof(s.vars));
    for(size_t i=0;i<r->closure->field_count;i++)
        s.config[s.config_count++]=(lt48_runtime_configuration_value){-1,r->closure->fields[i].category,r->closure->fields[i].index,0};
    if(r->closure->kind==1)for(size_t i=0;i<s.config_count;i++)
        if(s.config[i].category==1 && s.config[i].field==0)s.config[i].value=1;
    set32(s.buffers[2],r->capacity);
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(preserved);free(pending);return status;}
    status=setup(&s);
    if(status==LT48_OK)status=identify(&s,decide,decision_context);
    if(status==LT48_OK)status=sw_mutation_guard(&s,op);
    size_t readonly_prefix=sv_readonly_prefix(r,start,length);
    if(status==LT48_OK&&r->closure->kind!=2&&r->closure->kind!=3&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)) {
        status=cycle(&s,1,start,length,NULL,pending,r->nominal,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++)
            if(i<readonly_prefix?pending[i]!=image[i]:(op==LT48_OPERATION_PROGRAM?pending[i]!=255:(pending[i]&image[i])!=image[i])) {
                out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;status=LT48_DEVICE_MISMATCH;break;
            }
        if(status==LT48_OK && op==LT48_OPERATION_PROGRAM && length>readonly_prefix)status=cycle(&s,11,start+(uint32_t)readonly_prefix,length-readonly_prefix,NULL,NULL,r->nominal,out);
        if(status==LT48_OK)status=sc_unlock(&s);
        if(status==LT48_OK)status=protection(&s,0);
    }
    if(status==LT48_OK&&op==LT48_OPERATION_ERASE)status=sc_unlock(&s);
    if(status==LT48_OK && op==LT48_OPERATION_ERASE && (r->closure->kind==1 || r->wave[13]))
        status=protection(&s,0);
    if(status==LT48_OK && op==LT48_OPERATION_ERASE && r->closure->kind==2) {
        memset(pending,255,length);
        status=cycle(&s,2,0,length,pending,NULL,r->nominal,out);
    } else if(status==LT48_OK && op==LT48_OPERATION_ERASE) {
        s.mode=7;memset(s.buffers[0],0,32);set32(s.buffers[2],r->capacity);
        if(status==LT48_OK)status=run(&s,2);
        for(size_t i=0;status==LT48_OK && i<r->closure->primary_count;i++) {
            uint32_t address=r->closure->primary[i];if(address>=r->closure->erase_end)break;
            set32(s.buffers[0],address);status=run(&s,10);
            if(status==LT48_OK && s.buffers[r->closure->kind==3?2:3][0]!=1)status=LT48_DEVICE_MISMATCH;
        }
        if(status==LT48_OK)status=run(&s,3);
        if(status==LT48_OK)status=cycle(&s,1,0,r->closure->erase_verified_end,NULL,pending,r->nominal,out);
        if(status==LT48_OK)for(size_t i=0;i<r->closure->erase_verified_end;i++)if(pending[i]!=255) {
            if(!out->mismatch_count)out->first_mismatch=(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    } else if(status==LT48_OK && op==LT48_OPERATION_SECURE) {
        status=protection(&s,1);
    } else if(status==LT48_OK) {
        if(op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2]={margin_voltage(r,margin_type,0),margin_voltage(r,margin_type,1)};size_t n=2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
            for(size_t i=0;status==LT48_OK&&i<n;i++)status=checked_cycle(&s,4,start,length,image,pending,volts[i],out);
        } else {
            unsigned mode=op==LT48_OPERATION_READ?1:op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_COMPARE?5:2;
            if(op==LT48_OPERATION_COMPARE)status=checked_cycle(&s,mode,start,length,image,pending,r->nominal,out);
            else status=cycle(&s,mode,start,length,image,op==LT48_OPERATION_READ?pending:NULL,r->nominal,out);
        }
    }
    if(status==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM))
        for(unsigned i=0;status==LT48_OK&&i<2;i++)status=checked_cycle(&s,4,start,length,image,pending,margin_voltage(r,margin_type,i),out);
    out->operation_status=status;
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;s.api=&cleanup;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        const unsigned char stop[]={0,0};unsigned char reply[512]={0};
        lt48_status step=cmd(&s,stop,sizeof(stop),reply);
        if(step==LT48_OK&&!(reply[0]|reply[1]))step=LT48_PROTOCOL_ERROR;
        if(step==LT48_OK)step=run(&s,3);
        if(step==LT48_OK)step=run(&s,1);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(step!=LT48_OK)out->cleanup_status=step;
        for(unsigned channel=0;channel<4;channel++) {
            unsigned char off[]={0x10,(unsigned char)(3-channel),0};
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,off,sizeof(off));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        }
        /* Original generic tail restores the Windows idle FPGA after power-off. */
        static const unsigned char idle_header[]={0x38,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s.wire,idle_header,sizeof(idle_header));
        if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(out->cleanup_status==LT48_OK && step!=LT48_OK)out->cleanup_status=step;
        static const unsigned char finish[]={0x49,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s.wire,finish,sizeof(finish));
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        out->cleanup_completed=out->cleanup_status==LT48_OK;
        if(out->cleanup_completed) {
            unsigned char led[]={0x40,status==LT48_OK?4:2};
            step=lt48_runtime_transport_session_controller_write(s.wire,led,sizeof(led));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(step!=LT48_OK){out->cleanup_status=step;out->cleanup_completed=0;}
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,length);
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    if(status==LT48_OK &&
       (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||
        op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_ERASE||op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_SECURE)) {
        /* Fresh power/setup/read after cleanup also rejects a floating bus
         * echoing the last written byte during same-session verification. */
        lt48_addtab_spi_result fresh;
        status=lt48_addtab_spi_execute(r,api,context,LT48_OPERATION_READ,start,length,
            NULL,0,pending,length,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=(op==LT48_OPERATION_ERASE?(i<r->closure->erase_verified_end?255:preserved[i]):op==LT48_OPERATION_BLANK_CHECK?255:op==LT48_OPERATION_SECURE?preserved[i]:image[i])) {
            if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
        out->operation_status=status;
        if(status!=LT48_OK && out->cleanup_completed) {
            const unsigned char error_led[]={0x40,2};
            lt48_status led=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(led==LT48_OK)led=lt48_runtime_transport_session_controller_write(s.wire,error_led,sizeof(error_led));
            if(led==LT48_OK)led=lt48_runtime_transport_session_flush(s.wire);
            if(led!=LT48_OK){out->cleanup_status=led;out->cleanup_completed=0;}
            lt48_runtime_transport_session_close(s.wire);
        }
    }
    if(status==LT48_OK)out->completed_bytes=length;
    free(preserved);free(pending);return status;
}

unsigned lt48_addtab_spi_is_spiw6_internal(const lt48_addtab_spi_resources*r){if(r)for(size_t i=0;i<sizeof sw_closures/sizeof*sw_closures;i++)if(r->closure==sw_closures+i)return 1;return 0;}

int lt48_spiw6_binding_internal(const lt48_catalogue_entry*e){return spiw6_binding(e);}

unsigned lt48_addtab_spi_is_sst49c7_internal(const lt48_addtab_spi_resources*r){return r&&r->closure->kind==4;}

unsigned lt48_addtab_spi_is_df28_internal(const lt48_addtab_spi_resources*r){return r&&r->closure==df28_closures;}

unsigned lt48_addtab_spi_is_sstfwh_internal(const lt48_addtab_spi_resources*r){return r&&r->closure==fw_closures;}
