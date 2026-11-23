#include "../../NewDeviceLib/sources/w27e4096two/member.h"
#include "../../NewDeviceLib/sources/w27e4096two/erase-data.h"
#include "../../NewDeviceLib/sources/mxwordtwo/member.h"
#include "../../NewDeviceLib/sources/mxwordtwo/init.h"
#include "../../NewDeviceLib/sources/mxwordtwo/reset.h"
#include "../../NewDeviceLib/sources/mx1611one/setup.h"
#include "../../NewDeviceLib/sources/mx27pageone/member.h"
#include "../../NewDeviceLib/sources/mx27pageone/read-reset.h"
#include "../../NewDeviceLib/sources/mx27pageone/id-voltage.h"
#include "../../NewDeviceLib/sources/w29wordtwo/member.h"
#include "../../NewDeviceLib/sources/pss2005one/member.h"
#include "../../NewDeviceLib/sources/x88one/member.h"
#include "../../NewDeviceLib/sources/legacy-nor/ams_nor_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/fujitsu_002_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/sharp_s3_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/atmel_lv2048b_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/w49_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/m29f800_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/m28f800_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/mx26b_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/actrans4_admission.h"
#include "../../NewDeviceLib/sources/efst1/member.h"
#include "../../NewDeviceLib/sources/sm2964two/member.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_lv040_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/nexflash_010_admission.h"
#ifdef LT48_ADMISSION_TRIAL_HEADER
#include LT48_ADMISSION_TRIAL_HEADER
#else
#include "../../NewDeviceLib/sources/legacy-nor/batch_admissions.h"
#endif
#include "../../NewDeviceLib/sources/legacy-nor/macronix_psop3_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l8100_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l8000_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l3211_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_f1601_admission.h"
#include "../../NewDeviceLib/sources/mx28f1000/admission.h"
#include "../../NewDeviceLib/sources/legacy-st28/admission.h"
#include "../../NewDeviceLib/sources/legacy-sst1/admission.h"
#include "../../NewDeviceLib/sources/legacy-mx28p2/admission.h"
#include "../../NewDeviceLib/sources/legacy-alliance2/admission.h"
#include "../../NewDeviceLib/sources/legacy-macronix-bootblock4/admission.h"
#include "gaaf_serial.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../NewDeviceLib/sources/legacy-nor/macronix6_read_reset.h"
struct gaaf_profile { unsigned definition,family,pins,capacity,operations[LT48_GAAF_OPERATIONS+1]; };
#include "../../NewDeviceLib/sources/legacy-nor/actrans4.h"
#include "../../NewDeviceLib/sources/samsung-word2/profiles.h"
#include "../../NewDeviceLib/sources/word14/profiles.h"
#include "../../NewDeviceLib/sources/word14/admission.h"
#include "../../NewDeviceLib/sources/word14/maskrom.h"
#include "../../NewDeviceLib/sources/utron4/profiles.h"
#include "../../NewDeviceLib/sources/utron4/admission.h"
#include "../../NewDeviceLib/sources/sep7/profiles.h"
#include "../../NewDeviceLib/sources/sep7/admission.h"
#include "../../NewDeviceLib/sources/lnand10/profiles.h"
#include "../../NewDeviceLib/sources/lnand10/admission.h"
#include "../../NewDeviceLib/sources/nor3/profiles.h"
#include "../../NewDeviceLib/sources/sharp10/profiles.h"
#include "../../NewDeviceLib/sources/sharp10/admission.h"
#include "../../NewDeviceLib/sources/nor3/admission.h"
#include "../../NewDeviceLib/sources/sharp11/profiles.h"
#include "../../NewDeviceLib/sources/sharp11/admission.h"
#include "../../NewDeviceLib/sources/sst37four/profiles.h"
#include "../../NewDeviceLib/sources/sst37four/admission.h"
#include "../../NewDeviceLib/sources/sharpwordpulse3/profiles.h"
#include "../../NewDeviceLib/sources/micronc3two/profiles.h"
#include "../../NewDeviceLib/sources/micronc3two/admission.h"
#include "../../NewDeviceLib/sources/micronc3two/otp-exit.h"
#include "../../NewDeviceLib/sources/micronc3two/voltage.h"
#include "../../NewDeviceLib/sources/sharpwordpulse3/admission.h"
#include "../../NewDeviceLib/sources/sharpminato4/profiles.h"
#include "../../NewDeviceLib/sources/sharpminato4/admission.h"
#include "../../NewDeviceLib/sources/sharp1306/profiles.h"
#include "../../NewDeviceLib/sources/sharp1306/admission.h"
#include "../../NewDeviceLib/sources/sharpwordseven/profiles.h"
#include "../../NewDeviceLib/sources/sst37aliases/profiles.h"
#include "../../NewDeviceLib/sources/sst37aliases/admission.h"
#include "../../NewDeviceLib/sources/sst32four/profiles.h"
#include "../../NewDeviceLib/sources/sst32four/admission.h"
#include "../../NewDeviceLib/sources/pmc6/profiles.h"
#include "../../NewDeviceLib/sources/issi2/bindings.h"
#include "../../NewDeviceLib/sources/gt040one/profiles.h"
static const struct gaaf_profile w27e4096two_profiles[]={
#include "../../NewDeviceLib/sources/w27e4096two/profiles.inc"
};
static const struct gaaf_profile mxwordtwo_profiles[]={
#include "../../NewDeviceLib/sources/mxwordtwo/profile.inc"
};
static const struct gaaf_profile mx27pageone_profiles[]={
#include "../../NewDeviceLib/sources/mx27pageone/profile.inc"
};
static const struct gaaf_profile w29wordtwo_profiles[]={
#include "../../NewDeviceLib/sources/w29wordtwo/profile.inc"
};
#include "../../NewDeviceLib/sources/w29wordtwo/completion.h"
static const struct gaaf_profile pss2005one_profiles[]={
#include "../../NewDeviceLib/sources/pss2005one/profile.inc"
};
static const struct gaaf_profile x88one_profiles[]={
#include "../../NewDeviceLib/sources/x88one/profile.inc"
};
#include "../../NewDeviceLib/sources/x88one/program-settle.h"
static const struct gaaf_profile profiles[]={
#include "gaaf_serial_profiles.inc"
};
static const struct gaaf_profile legacy_nor53_profiles[]={
#include "legacy_nor53_profiles.inc"
#include "../../NewDeviceLib/sources/efst1/profile.inc"
#include "../../NewDeviceLib/sources/sm2964two/profile.inc"
};
static const struct gaaf_profile legacy_byte49_profiles[]={
#include "legacy_byte49_profiles.inc"
};
static const struct gaaf_profile legacy_word46_profiles[]={
#include "legacy_word46_profiles.inc"
};
#include "../../NewDeviceLib/sources/mx26aone/erase-wave.h"
static const struct gaaf_profile legacy_route45_profiles[]={
#include "legacy_route45_profiles.inc"
};
static unsigned byte49_mask(unsigned definition,unsigned *category) {
 switch(definition) {
#include "legacy_byte49_masks.inc"
 default:return ~0u;
 }
}
static unsigned route45_mask(unsigned definition) {
 switch(definition) {
#include "legacy_route45_masks.inc"
 default:return 0;
 }
}
/* Compact waves retain source IDs in nested calls, but the shared operation
 * owner uses its existing semantic slots. Applet IDs remain source IDs. */
static unsigned route45_wave_slot(unsigned id) {
 switch(id) {
 case 14:return 0; case 13:return 1; case 10:return 2;
 case 1:return 3; case 2:return 4; case 9:return 5;
 case 15:return 6; case 16:return 7; case 11:return 8;
 case 12:return 9; case 20:return 10; case 17:return 11;
 case 18:return 12; case 19:return 13;
 default:return LT48_GAAF_OPERATIONS;
 }
}
static uint64_t word46_mask(unsigned definition) {
 switch(definition) {
#include "legacy_word46_masks.inc"
 default:return 0;
 }
}
#include "../../NewDeviceLib/sources/legacy-nor/w49_profiles.h"
#include "../../NewDeviceLib/sources/fujitsu41/profiles.h"
#include "../../NewDeviceLib/sources/st2009two/geometry.h"
#include "../../NewDeviceLib/sources/st2009two/profiles.h"
#include "../../NewDeviceLib/sources/st2009two/explicit-address.h"
#include "../../NewDeviceLib/sources/st2009two/admission.h"
#include "../../NewDeviceLib/sources/sst38one/profiles.h"
#include "../../NewDeviceLib/sources/sst38one/explicit-address.h"
#include "../../NewDeviceLib/sources/sst38one/admission.h"
#include "../../NewDeviceLib/sources/combined36/profiles.h"
#include "../../NewDeviceLib/sources/combined36/admission.h"
#include "../../NewDeviceLib/sources/hitachi15/profiles.h"
#include "../../NewDeviceLib/sources/hitachi15/read-applet.h"
#include "../../NewDeviceLib/sources/hitachi15/read-setup.h"
static const struct gaaf_profile hitachi_profiles[]={
{2,1,1,63360,{15,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{3,2,1,65536,{16,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{4,1,1,65536,{15,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{5,1,1,65536,{15,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{6,2,1,131072,{16,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{8,1,1,131072,{15,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{9,1,1,262144,{15,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{10,2,1,262144,{16,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{11,2,1,524288,{16,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
{12,1,1,524288,{15,4,65535,19,23,25,65535,65535,65535,65535,26,32,30,29,28,31,65535,65535,65535,65535}},
};
static const struct gaaf_profile legacy28f_profiles[]={
 {3,1,2,131072,{1,2,4,5,9,11,12,14,15,17,16,65535,65535,65535,65535,65535,65535,65535,65535,65535}},
 {6,3,2,131072,{0,2,6,7,10,11,12,14,15,18,16,65535,65535,65535,65535,65535,65535,65535,65535,65535}},
 {5,2,4,524288,{1,2,3,8,9,11,13,14,15,19,16,65535,65535,65535,65535,65535,65535,65535,65535,65535}},
};
/* Exact resources for DSP protection. Array operations use the audited
 * EPROM transport separately; this resource route authorizes Secure only. */
#include "../../NewDeviceLib/sources/novatek-umc6/secure-profiles.h"
#include "../../NewDeviceLib/sources/hitachipulse2/secure-profiles.h"
#include "../../NewDeviceLib/sources/hitachipulse2/retry-policy.h"
#include "../../NewDeviceLib/sources/novatek-umc6/admission.h"
static const struct gaaf_profile tms15_profiles[]={
 {75,60,1,8192,{3,4,65535,86,8,87,65535,9,20,22,91,65535,65535,65535,65535,65535,65535,65535,65535,65535}},
};
static const struct gaaf_profile tms25_profiles[]={
 {15,13,10,8192,{0,2,3,6,5,32,65535,13,14,15,20,33,65535,65535,65535,65535,65535,65535,65535,65535}},
};
void lt48_gaaf_serial_close(lt48_gaaf_serial_resources *r) {
 if(!r)return;
 for(unsigned i=0;i<LT48_GAAF_OPERATIONS;i++)lt48_compiled_wave_close(r->waves[i]);
 lt48_compiled_wave_close(r->page_read_reset);
 lt48_compiled_wave_close(r->id_setup);
 lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r->x88_program);free(r->mxwordtwo_erase);for(unsigned i=0;i<3;i++)free(r->st_workers[i]);free(r->sst38_workers[0]);free(r->sst38_workers[1]);free(r);
}
/* Exact authored copy-count replacements. Never relax runtime buffer bounds. */
static lt48_status macronix6_bounded_bytes(const lt48_wave_code *code,unsigned char **copy) {
 static const struct {const char *hash;unsigned offset,before,after;} patches[]={
#include "../../NewDeviceLib/sources/legacy-nor/macronix6_shift_patches.h"
 };
 unsigned char digest[32];char hex[65];lt48_sha256_context context;
 lt48_sha256_init(&context);lt48_sha256_update(&context,code->data,code->data_size);lt48_sha256_final(&context,digest);
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 *copy=NULL;
 for(size_t i=0;i<sizeof(patches)/sizeof(*patches);i++)if(!strcmp(hex,patches[i].hash)) {
  if(patches[i].offset>=code->data_size||code->data[patches[i].offset]!=patches[i].before){free(*copy);*copy=NULL;return LT48_SAFETY_REJECTED;}
  if(!*copy){*copy=malloc(code->data_size);if(!*copy)return LT48_NO_MEMORY;memcpy(*copy,code->data,code->data_size);}
  (*copy)[patches[i].offset]=(unsigned char)patches[i].after;
 }
 return LT48_OK;
}
#include "../../NewDeviceLib/sources/legacy-nor/m29f800_status_lane.h"
#include "../../NewDeviceLib/sources/legacy-nor/w49_id_without_byte.h"
#include "../../NewDeviceLib/sources/legacy-nor/atmel_lv2048b.h"
#include "../../NewDeviceLib/sources/legacy-nor/atmel_top_query.h"
#include "../../NewDeviceLib/sources/sharpminato4/binding-check.inc"
#include "../../NewDeviceLib/sources/sharpwordpulse3/binding-check.inc"
static lt48_status gaaf_open(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out,
 const struct gaaf_profile *list,size_t count,unsigned source_class) {
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec spec;lt48_status status=lt48_installation_resolve_device(x,device,&spec);
 if(status!=LT48_OK)return status;
 unsigned byte_page=source_class==2005&&(spec.definition_resource==14||spec.definition_resource==0x28||spec.definition_resource==0x3a||spec.definition_resource==0x33||spec.definition_resource==0x34||(spec.definition_resource>=0x50&&spec.definition_resource<=0x53));
 if(spec.catalogue.device_class!=source_class||spec.element_bits!=((source_class==2003||source_class==2004||source_class==2029||(source_class==2005&&!byte_page))?16u:8u))return LT48_UNSUPPORTED;
 const struct gaaf_profile *p=NULL;
 for(size_t i=0;i<count;i++)
  if(list[i].definition==spec.definition_resource&&list[i].family==spec.family_code&&list[i].pins==spec.pinmap&&list[i].capacity==spec.capacity_bytes)p=&list[i];
 if(!p)return LT48_UNSUPPORTED;
 lt48_gaaf_serial_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
 r->st2009two=list==st2009two_profiles;r->sst38one=list==sst38one_profiles;r->w27e4096two=list==w27e4096two_profiles;r->mxwordtwo=list==mxwordtwo_profiles;r->mx27pageone=list==mx27pageone_profiles;
 r->w29wordtwo=list==w29wordtwo_profiles;
 r->pss2005one=list==pss2005one_profiles;r->legacy_page_leave_protection=r->mxwordtwo||r->pss2005one||r->w29wordtwo||r->mx27pageone;
 r->x88one=list==x88one_profiles;
 r->first_byte_status=list==pmc6_profiles||list==gt040one_profiles;r->sm2964two=sm2964two_member(&spec.catalogue);r->sharpwordseven=list==sharpwordseven_profiles;r->sst32four=list==sst32four_profiles;r->legacy_erase_applet=(r->w27e4096two||r->sst32four||list==sharpwordpulse3_profiles)?16u:0u;r->sst37aliases=list==sst37aliases_profiles;r->micronc3two=list==micronc3two_profiles;r->sharpwordpulse3=list==sharpwordpulse3_profiles;r->sharpminato4=list==sharpminato4_profiles;r->sharp1306=list==sharp1306_profiles;r->sst37four=list==sst37four_profiles;r->sharp11=list==sharp11_profiles;r->sharp10=list==sharp10_profiles;r->nor3=list==nor3_2007_profiles||list==nor3_2008_profiles;r->lnand10=list==lnand10_profiles;r->sep7=list==sep7_profiles;r->utron4=list==utron4_profiles;r->word14=r->utron4||list==word14_profiles;r->hitachi15=list==h15_profiles;r->combined36=list==combined36_profiles;r->definition=p->definition;r->capacity=p->capacity;r->fujitsu41=list==fujitsu41_profiles;
 r->legacy_actrans=list==actrans4_profiles;
 r->legacy_dummy=source_class==2029;r->legacy_w49=r->legacy_dummy||list==w49_t16_profiles;
 r->tms320e=source_class==2001?15:source_class==2006?25:0;
 r->legacy_page=source_class==2005;r->legacy_page_byte=r->legacy_page&&(r->pss2005one||r->x88one||p->definition==0x3a||p->definition==0x33||p->definition==0x34||(p->definition>=0x50&&p->definition<=0x53));r->legacy_motorola=r->legacy_page && p->definition>=0x4a && p->definition<=0x4d;r->page_bytes=r->w29wordtwo?256:r->x88one?32:r->legacy_page?(r->legacy_motorola?256u:128u):0;r->legacy_word=source_class==2003;r->legacy_f3=source_class==2004||r->legacy_dummy;
 r->mx26aone=source_class==2008&&p->definition==0x3f;
 if(r->mx26aone&&(spec.catalogue.manufacturer_id!=0xc2||spec.catalogue.device_id!=0x99||strcmp(spec.catalogue.vendor,"Macronix")||spec.catalogue.device_name_length!=10||memcmp(spec.catalogue.device_name,"MX26C4000 ",10))){status=LT48_UNSUPPORTED;goto fail;}
 r->legacy_route=source_class==2008;r->vcc_variable=r->legacy_dummy?1u:r->legacy_f3?3u:r->legacy_route?15u:0u;
 r->legacy_nor=source_class==2007||r->legacy_word||r->legacy_route||r->legacy_f3||r->legacy_page;
 r->legacy28f=source_class==2002;r->reset_op=r->legacy28f?5:4;
 r->manufacturer=spec.catalogue.manufacturer_id;r->device=spec.catalogue.device_id;
 if(spec.catalogue.unknown6){status=LT48_UNSUPPORTED;goto fail;}
 r->configuration_bits=(r->tms320e||r->legacy28f||r->legacy_nor)?0:p->definition>=0x48&&p->definition<=0x53?8:(p->definition==0x46||p->definition==0x47)?9:0;
 char path[4096],name[32],hex[65];unsigned char digest[32];size_t index,family;
 int n=snprintf(path,sizeof(path),"%s/%s",lt48_installation_path(x),r->legacy_page?"GAAC___.rez":r->legacy_dummy?"GAABB__.rez":r->legacy_f3?"GAABA__.rez":r->legacy_route?"GAAE1__.rez":r->legacy_word?"GAAB___.rez":r->legacy_nor?"GAAE___.rez":r->tms320e==15?"GAA____.rez":r->tms320e==25?"GAAD___.rez":r->legacy28f?"GAAA___.rez":"GAAF___.rez");
 if(n<0||(size_t)n>=sizeof(path)){status=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(e) do{status=(e);if(status!=LT48_OK)goto fail;}while(0)
 TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,r->legacy_page?"a5b774bb0fada8081d9a158f710586df4dafb71c298d4c049c625627dde173c4":r->legacy_dummy?"175b86105f89b58c3e9e40d169e742f3bc193be9af9a3a3c5b7b998138ab182b":r->legacy_f3?"cb8e52932d7e04b06eb43530a95ea3533311cc49904a2e4924fd47837318b695":r->legacy_route?"4d97d960939a13b84a36d664def2823698b680ecbd97514be0b7b55ecc244e05":r->legacy_word?"884d761b7deca2ccbaa49e0b26c3b515e1381033c6b9af6fd496136135c47bdf":r->legacy_nor?"3a9cc5ad13d26c607a5d2bd5774c5cc54315b642aea14864a12da92f86765117":r->tms320e==15?"2e6261c867d6857786933c0d093e5bd669f70355cc218ff006c88c4fb919636b":r->tms320e==25?"e282aa2c1a8fbcc13070fee62dae551fe9db3809639a90529b5d423f99c90180":r->legacy28f?"3163ffcfd04a7cdc2c1853110175fae42fe9e399a8cce9fbc85e23d007b2e7d2":"9fdc0c7a88bb96aaead0314fb4f85c0800ab987abf370908000493cc272d13f6")){status=LT48_SAFETY_REJECTED;goto fail;}
 snprintf(name,sizeof(name),"P%x",p->pins);lt48_pin_code pc;
 TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof(name),"M%x",p->family);lt48_family_info info;
 TRY(lt48_rez_find_resource(r->rez,name,&family));TRY(lt48_rez_get_family_info(r->rez,family,&info));
 if(!info.variable_count||info.variable_count>LT48_GAAF_VARIABLES){status=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_rez_get_family_frame_profile(r->rez,family,&r->frame_profile));
 if(r->frame_profile.buffer_count!=(r->legacy_page?30u:r->legacy_dummy?24u:r->legacy_f3?27u:r->tms320e==15?11u:r->tms320e==25?9u:r->legacy28f?5u:(r->legacy_word||r->legacy_route)?26u:r->legacy_nor?25u:24u)||r->frame_profile.mode_buffer!=(r->legacy_page?16u:r->legacy_f3?3u:r->legacy_route?8u:2u)){status=LT48_FORMAT_ERROR;goto fail;}
 r->variable_count=info.variable_count;
 for(size_t i=0;i<r->variable_count;i++) {
  lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,family,i,&v));
  r->variables[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
  if(v.values[0]==0&&v.values[1]==r->vcc_variable)r->nominal=v.values[3];
 }
 unsigned seen=0,expected=0;
 if(r->st2009two)TRY(st2009two_variables(r->variables,r->variable_count,r->definition));
 if(r->mx27pageone)TRY(mx27pageone_id_voltage(r->variables,r->variable_count));
 for(unsigned i=0;i<LT48_GAAF_OPERATIONS+((r->st2009two||r->pss2005one||r->legacy_route||r->legacy_f3||r->legacy_motorola)?1u:0u);i++)if(p->operations[i]!=65535)expected|=1u<<i;
 for(size_t i=0;i<info.operation_count;i++) {
  lt48_family_operation op;lt48_wave_code code;
  TRY(lt48_rez_get_family_operation(r->rez,family,i,&op));
  /* D3F's IAD is protection UI metadata, not a memory operation. The
   * finite owner preserves existing protection and exposes no lock editor. */
  if(r->mxwordtwo&&r->definition==0x3f&&op.operation_id==20&&op.wave==0x7e)continue;
  /* Exact M31/D39 source override: the later W61 replaces W7D in slot 19.
   * The verified source file and final profile binding remain mandatory. */
  if(r->legacy_page && p->definition==0x39 && op.operation_id==19 && op.wave==0x7d)continue;
  if(op.operation_id>=LT48_GAAF_OPERATIONS+((r->st2009two||r->pss2005one||r->legacy_route||r->legacy_f3||r->legacy_motorola)?1u:0u)||p->operations[op.operation_id]!=op.wave||(seen&(1u<<op.operation_id))){status=LT48_FORMAT_ERROR;goto fail;}
  seen|=1u<<op.operation_id;snprintf(name,sizeof(name),"W%x",op.wave);
  TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&code));
  if(r->word14&&r->definition==0x39){
   if(op.operation_id==2)continue;
   if(op.operation_id==3){code.data=word14_maskrom_read;code.data_size=sizeof word14_maskrom_read;}
   if(op.operation_id==4){code.data=word14_maskrom_reset;code.data_size=sizeof word14_maskrom_reset;}
  }
  if(r->w27e4096two&&op.operation_id==5){if(code.wave_type!=0||code.data_size!=92||op.wave!=0x94){status=LT48_SAFETY_REJECTED;goto fail;}code.data=w27e4096two_erase;code.data_size=sizeof w27e4096two_erase;}
  if(r->mxwordtwo&&r->definition!=0x3a&&op.operation_id==3){if(code.data_size!=182||code.data[0x63]!=0x3d||code.data[0x64]!=15||code.data[0x8f]!=0x3d||code.data[0x90]!=16){status=LT48_SAFETY_REJECTED;goto fail;}code.data=mxwordtwo_init;code.data_size=sizeof mxwordtwo_init;}
  unsigned slot=r->st2009two&&op.operation_id==20?10u:r->pss2005one&&op.operation_id==20?19u:r->legacy_motorola&&op.operation_id==20?19u:r->legacy_f3&&op.operation_id==20?10u:r->legacy_route?route45_wave_slot(op.operation_id):op.operation_id;
  if((code.wave_type==0||code.wave_type==1)&&slot>=LT48_GAAF_OPERATIONS){status=LT48_FORMAT_ERROR;goto fail;}
  if(r->mx26aone&&op.operation_id==9){
   TRY(mx26aone_compile_erase(&code,&r->waves[slot]));
   r->nested[r->nested_count++]=(lt48_runtime_nested_wave){op.operation_id,r->waves[slot]};continue;
  }
  if(code.wave_type==0) {
   size_t compact_size=code.data_size;unsigned char *bounded=NULL;if(r->st2009two&&(op.operation_id==2||op.operation_id==3||op.operation_id==8||op.operation_id==9))TRY(st2009two_compact(&code,op.operation_id,&bounded,&compact_size));if(r->sst38one&&(op.operation_id==2||op.operation_id==5))TRY(sst38one_compact_delta(&code,op.operation_id,&bounded,&compact_size));if(r->micronc3two&&op.operation_id==9)TRY(micronc3two_otp_exit(&code,&bounded,&compact_size));if(r->micronc3two&&op.operation_id==4)TRY(micronc3two_array_reset(&code,&bounded,&compact_size));if(r->legacy_page)TRY(macronix6_bounded_bytes(&code,&bounded));if(r->mxwordtwo&&r->definition==0x3a&&op.operation_id==3){status=mx1611one_init(&code,&bounded,&compact_size);if(status!=LT48_OK){free(bounded);goto fail;}}
   if(r->legacy_nor&&!r->legacy_word&&!r->legacy_route&&!r->legacy_f3&&!r->legacy_page){status=atmel_top_query(&code,r->definition,op.operation_id,&bounded,&compact_size);if(status!=LT48_OK){free(bounded);goto fail;}}
   if(r->legacy_motorola){status=m29f800_status_lane(&code,r->definition,op.operation_id,&bounded);if(status!=LT48_OK){free(bounded);goto fail;}}
   if(r->legacy_w49&&!r->legacy_dummy){status=w49_id_without_byte(&code,r->definition,op.operation_id,&bounded,&compact_size);if(status!=LT48_OK){free(bounded);goto fail;}}
   if(r->legacy_word&&r->definition==0xfe){status=atmel_lv2048b_compact(&code,r->definition,op.operation_id,&bounded,&compact_size);if(status!=LT48_OK){free(bounded);goto fail;}}
   lt48_compact_wave *compact=NULL;status=lt48_compact_decode(bounded?bounded:code.data,compact_size,&compact);free(bounded);if(status!=LT48_OK)goto fail;
   int resident_noop=list==pmc6_profiles||list==gt040one_profiles||r->lnand10||(r->legacy28f && p->definition==5 && (op.operation_id==3||op.operation_id==4)) || r->legacy_nor||r->st2009two;
   status=lt48_compact_compile_with_pin_policy(compact,r->pins,r->variables,r->variable_count,1,
    resident_noop?LT48_PIN_FALLBACK_RESIDENT_NOOP:LT48_PIN_FALLBACK_STRICT,&r->waves[slot]);
   lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail;
   if(!r->legacy_nor&&!r->st2009two && lt48_compiled_wave_defaulted_pin_count(r->waves[slot])!=((list==pmc6_profiles||list==gt040one_profiles)?((op.operation_id==2||op.operation_id==3)?1u:0u):r->lnand10?(op.operation_id==3?1u:0u):(resident_noop?1u:0u))){status=LT48_SAFETY_REJECTED;goto fail;}
   r->nested[r->nested_count++]=(lt48_runtime_nested_wave){op.operation_id,r->waves[slot]};
  } else if((r->legacy_nor||r->st2009two)&&code.wave_type==1) {
   TRY(lt48_compiled_word_table_internal(code.data,code.data_size,&r->waves[slot]));
   r->nested[r->nested_count++]=(lt48_runtime_nested_wave){op.operation_id,r->waves[slot]};
  } else if(code.wave_type==4&&op.operation_id==(r->legacy_page?6u:r->legacy_dummy?10u:r->legacy_route?0u:r->legacy_word?13u:r->legacy_nor?11u:r->tms320e==15?7u:r->tms320e==25?11u:r->legacy28f?6u:10u)&&code.data_size==2780&&code.data[0]==0xda&&code.data[1]==0x0a)r->fpga=code.data+2;
  else if(code.wave_type==6&&code.data_size&&code.length1==code.data_size+4&&code.length2==code.data_size&&code.load_address>=0x5000&&(uint32_t)code.load_address+code.data_size<=0xb000) {
   for(size_t j=0;j<r->address_count;j++) {
    const lt48_wave_code *other=&r->applets[r->addresses[j].wave];
    if(!r->legacy_nor && code.load_address<other->load_address+other->data_size&&other->load_address<code.load_address+code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
   }
   if(r->st2009two&&(op.operation_id==13||op.operation_id==14||op.operation_id==15))TRY(st2009two_worker(r,&code,op.operation_id));
   if(r->sst38one&&(op.operation_id==14||op.operation_id==15))TRY(sst38one_worker(&code,op.operation_id,&r->sst38_workers[op.operation_id==15]));
   if(r->mxwordtwo&&r->definition==0x3f&&op.operation_id==18){
    /* Original shared erase helper requests the L part's 11V. F1615
     * BYTE/VPP absolute maximum is 10.5V (PM0615); use its own Program
     * worker's 10V/read DAC literals instead, in an owned RAM copy. */
    if(code.data_size!=122||code.data[0x22]!=0x7a||code.data[0x54]!=0x29){status=LT48_SAFETY_REJECTED;goto fail;}
    r->mxwordtwo_erase=malloc(code.data_size);if(!r->mxwordtwo_erase){status=LT48_NO_MEMORY;goto fail;}
    memcpy(r->mxwordtwo_erase,code.data,code.data_size);r->mxwordtwo_erase[0x22]=0x70;r->mxwordtwo_erase[0x54]=0x3c;code.data=r->mxwordtwo_erase;
   }
   r->applets[op.operation_id]=code;
   r->addresses[r->address_count++]=(lt48_applet_address){op.operation_id,code.load_address};
  } else {status=LT48_FORMAT_ERROR;goto fail;}
 }
 if(r->x88one){TRY(x88one_program_settle(r));r->legacy_config_mask=0;r->legacy_page_leave_protection=1;}
 else if(r->w29wordtwo) {
  if(r->waves[19]){status=LT48_FORMAT_ERROR;goto fail;}
  lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w29wordtwo_ack,sizeof w29wordtwo_ack,&c));
  status=lt48_compact_compile_with_pin_policy(c,r->pins,r->variables,r->variable_count,1,LT48_PIN_FALLBACK_STRICT,&r->waves[19]);
  lt48_compact_wave_close(c);if(status!=LT48_OK)goto fail;
  r->legacy_config_mask=0;
 }
 else if(r->legacy_page && !r->legacy_motorola) {
  lt48_compact_wave *compact=NULL;
  const unsigned char*reset=r->mxwordtwo?(r->definition==0x3a?mx1611one_reset:r->definition==0x3f?mxwordtwo_reset_f:mxwordtwo_reset_l):r->mx27pageone?mx27pageone_read_reset:macronix6_read_reset_bytes;
  size_t reset_size=r->mxwordtwo?(r->definition==0x3a?sizeof mx1611one_reset:sizeof mxwordtwo_reset_f):r->mx27pageone?sizeof mx27pageone_read_reset:sizeof macronix6_read_reset_bytes;
  TRY(lt48_compact_decode(reset,reset_size,&compact));
  status=lt48_compact_compile_with_pin_policy(compact,r->pins,r->variables,r->variable_count,1,
   LT48_PIN_FALLBACK_STRICT,&r->page_read_reset);
  lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail;
  r->legacy_config_mask=r->mxwordtwo?0u:r->mx27pageone?0u:r->pss2005one?0xfffu:(r->definition==0x2f||r->definition==0x30||r->definition==0x39)?0xffffu:3u;
 }
 else if(r->w27e4096two||r->sharpwordpulse3||r->sharpminato4)r->legacy_config_mask=0;
 else if(r->legacy_actrans) {
  unsigned table=~0u,sectors=0;
  for(size_t i=0;i<r->variable_count;i++) {
   if(r->variables[i].variable_id==19)table=r->variables[i].value;
   if(r->variables[i].variable_id==21)sectors=r->variables[i].value;
  }
  if(table!=0 || sectors!=11){status=LT48_SAFETY_REJECTED;goto fail;}
  r->legacy_config_mask=0x7ff;r->legacy_config_category=0;
 }
 else if(r->sharp11){r->legacy_config_mask=r->waves[9]?15u:0u;r->legacy_config_category=0;}
 else if(r->legacy_w49){r->legacy_config_mask=1u;r->legacy_config_category=0;}
 else if(r->legacy_motorola)r->legacy_config_mask=0;
 else if(r->sharp10){unsigned n=r->definition==0x17?9:r->definition==0x18?17:r->definition==0x71||r->definition==0x72?0:(r->definition&1)?4:8;r->legacy_config_mask=n?(1u<<n)-1:0;}
 else if(r->legacy_route)r->legacy_config_mask=route45_mask(r->definition);
 else if(r->sharpwordseven){r->legacy_config_mask=r->definition==0xd6?UINT32_MAX:15u;for(size_t v=0;v<r->variable_count;v++)if(r->variables[v].variable_id==26)r->sharpword_sector_count=r->variables[v].value;}
 else if(r->legacy_word)r->legacy_config_mask=(unsigned)word46_mask(r->definition);
 else if(r->legacy_nor && !r->legacy_word && !r->legacy_route && byte49_mask(r->definition,&r->legacy_config_category)!=~0u)
  r->legacy_config_mask=byte49_mask(r->definition,&r->legacy_config_category);
 else if(r->sst38one)r->legacy_config_mask=0;
 else if(r->combined36)r->legacy_config_mask=UINT32_MAX;
 else if(r->legacy_nor && r->waves[8]) {
  unsigned first=0,count=1;
  for(size_t i=0;i<r->variable_count;i++) {
   if(r->variables[i].variable_id==22)first=r->variables[i].value;
   if(r->variables[i].variable_id==24)count=r->variables[i].value;
  }
  if(!((first==0&&(count==1||count==7||count==8))||(first==8&&count==8))){status=LT48_SAFETY_REJECTED;goto fail;}
  /* Captured sector state is internal; it does not expose a configuration operation. */
  r->legacy_config_mask=((1u<<count)-1)<<first;
 }
 if(seen!=expected||!r->nominal||!r->fpga||!r->waves[0]||!r->waves[1]||!r->waves[3]||!r->waves[r->reset_op]){status=LT48_FORMAT_ERROR;goto fail;}
 if(r->hitachi15){
  if(!h15_binding(&spec)||r->variable_count!=4||r->nominal!=500||r->variables[1].value!=5||r->variables[2].value+1u!=r->capacity||r->variables[3].value!=2||r->variables[3].channel!=15){status=LT48_SAFETY_REJECTED;goto fail;}
  lt48_compact_wave*w=NULL;TRY(lt48_compact_decode(h15_read_setup,sizeof h15_read_setup,&w));
  status=lt48_compact_compile_with_pin_policy(w,r->pins,r->variables,r->variable_count,1,LT48_PIN_FALLBACK_STRICT,&r->waves[18]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
  r->applets[19]=(lt48_wave_code){.wave_type=6,.load_address=0x6000,.data=h15_read_applet,.data_size=sizeof h15_read_applet};
  r->addresses[r->address_count++]=(lt48_applet_address){19,0x6000};
 }
 TRY(sharpwordpulse3_check(r));
 TRY(sharpminato4_check(r));
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
 *out=r;return LT48_OK;
fail:lt48_gaaf_serial_close(r);return status;
#undef TRY
}

lt48_status lt48_legacy_nor53_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 return gaaf_open(x,device,out,legacy_nor53_profiles,sizeof(legacy_nor53_profiles)/sizeof(*legacy_nor53_profiles),2007);
}

/* Deliberately separate from public lt48_gaaf_serial_open until complete
 * operation and FPGA qualification of this batch. */
#include "../../NewDeviceLib/sources/legacy-nor/hynix_080_id_setup.h"
#include "../../NewDeviceLib/sources/legacy-nor/fujitsu_002_id_setup.h"
#include "../../NewDeviceLib/sources/legacy-nor/w39f010_software_id.h"

#include "../../NewDeviceLib/sources/legacy-nor/mx26b_erase_check.h"
lt48_status lt48_legacy_byte49_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 lt48_status status=gaaf_open(x,device,out,legacy_byte49_profiles,sizeof(legacy_byte49_profiles)/sizeof(*legacy_byte49_profiles),2007);
 if(status!=LT48_OK)return status;
 lt48_catalogue_entry e;
 status=lt48_installation_get_device(x,device,&e);
 if(status==LT48_OK)status=hynix_080_id_setup(*out,&e);
 if(status==LT48_OK)status=fujitsu_002_id_setup(*out,&e);
 if(status==LT48_OK)status=w39f010_software_id(*out,&e);
 if(status==LT48_OK)status=mx26b_erase_check(*out);
 if(status!=LT48_OK){lt48_gaaf_serial_close(*out);*out=NULL;}
 return status;
}

#include "../../NewDeviceLib/sources/mx28f1000/automatic_program.h"

lt48_status lt48_legacy_word46_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 return gaaf_open(x,device,out,legacy_word46_profiles,sizeof(legacy_word46_profiles)/sizeof(*legacy_word46_profiles),2003);
}

lt48_status lt48_legacy_route45_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 return gaaf_open(x,device,out,legacy_route45_profiles,sizeof(legacy_route45_profiles)/sizeof(*legacy_route45_profiles),2008);
}

lt48_status lt48_word14_open_internal(const lt48_installation*x,size_t i,lt48_gaaf_serial_resources**out){
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,i,&s);if(st)return st;
 if(s.catalogue.device_class!=2003||strcmp(lt48_installation_source_profile(x),"Dataman 13.10")||!word14_binding(&s))return LT48_UNSUPPORTED;
 st=gaaf_open(x,i,out,word14_profiles,sizeof word14_profiles/sizeof*word14_profiles,2003);
 if(!st){lt48_gaaf_serial_resources*r=*out;for(size_t v=0;v<r->variable_count;v++)if(r->variables[v].variable_id==26){unsigned n=r->variables[v].value;if(r->definition==0x166||r->definition==0x167)n++;r->legacy_config_mask=n>=32?UINT32_MAX:(1u<<n)-1u;}}return st;
}
lt48_status lt48_utron4_open_internal(const lt48_installation*x,size_t i,lt48_gaaf_serial_resources**out){
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,i,&s);if(st)return st;
 if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10")||!utron4_binding(&s))return LT48_UNSUPPORTED;
 st=gaaf_open(x,i,out,utron4_profiles,sizeof utron4_profiles/sizeof*utron4_profiles,2003);
 if(!st){(*out)->legacy_config_mask=3;(*out)->legacy_config_category=1;} /* Two default host-option selections only. */
 return st;
}
unsigned lt48_gaaf_serial_is_word14_internal(const lt48_gaaf_serial_resources*r){return r&&r->word14;}
static lt48_status samsung_word2_open(const lt48_installation*x,size_t device,lt48_gaaf_serial_resources**out){
 lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(x,device,&spec);if(st)return st;
 if(!samsung_word2_binding(&spec)||strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;
 st=gaaf_open(x,device,out,samsung_word2_profiles,2,2003);if(!st)(*out)->legacy_config_mask=0x7ffff;return st;
}
lt48_status lt48_x88one_open_internal(const lt48_installation*x,size_t device,lt48_gaaf_serial_resources**out){
 lt48_catalogue_entry e;lt48_status st=lt48_installation_get_device(x,device,&e);if(st)return st;
 if(!x88one_member(&e)||strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;
 return gaaf_open(x,device,out,x88one_profiles,1,2005);
}
lt48_status lt48_gaaf_serial_open(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 lt48_catalogue_entry m3;if(!lt48_installation_get_device(x,device,&m3)&&m3.device_class==2004&&!strcmp(m3.vendor,"Micron")&&(m3.device_type==0x87||m3.device_type==0x88))return gaaf_open(x,device,out,micronc3two_profiles,2,2004);

 lt48_catalogue_entry wb;if(!lt48_installation_get_device(x,device,&wb)&&w27e4096two_member(&wb))return gaaf_open(x,device,out,w27e4096two_profiles,1,2003);
 lt48_catalogue_entry mw;if(!lt48_installation_get_device(x,device,&mw)&&mxwordtwo_member(&mw))return gaaf_open(x,device,out,mxwordtwo_profiles,3,2005);
 lt48_catalogue_entry mp;if(!lt48_installation_get_device(x,device,&mp)&&mx27pageone_member(&mp))return gaaf_open(x,device,out,mx27pageone_profiles,1,2005);
 lt48_catalogue_entry ww;if(!lt48_installation_get_device(x,device,&ww)&&w29wordtwo_member(&ww))return gaaf_open(x,device,out,w29wordtwo_profiles,2,2005);
 lt48_catalogue_entry ps;if(!lt48_installation_get_device(x,device,&ps)&&pss2005one_member(&ps))return gaaf_open(x,device,out,pss2005one_profiles,1,2005);
 lt48_catalogue_entry x88;if(!lt48_installation_get_device(x,device,&x88)&&x88one_member(&x88))return lt48_x88one_open_internal(x,device,out);
 lt48_catalogue_entry gt;if(!lt48_installation_get_device(x,device,&gt)&&gt040one_member(&gt))return gaaf_open(x,device,out,gt040one_profiles,1,2009);
 lt48_catalogue_entry pm;if(!lt48_installation_get_device(x,device,&pm)&&(pmc6_member(&pm)||issi2_member(&pm)))return gaaf_open(x,device,out,pmc6_profiles,3,2009);
 lt48_catalogue_entry s32;if(!lt48_installation_get_device(x,device,&s32)&&s32.device_class==2003&&s32.device_type>=0x138&&s32.device_type<=0x13a&&!strcmp(s32.vendor,"SST"))return gaaf_open(x,device,out,sst32four_profiles,3,2003);
 lt48_catalogue_entry sa;if(!lt48_installation_get_device(x,device,&sa)&&sst37aliases_binding(&sa))return gaaf_open(x,device,out,sst37aliases_profiles,4,2007);
 lt48_catalogue_entry minato;if(!lt48_installation_get_device(x,device,&minato)&&minato.device_class==2004&&minato.device_type>=0x7e&&minato.device_type<=0x81&&!strcmp(minato.vendor,"SHARP"))return gaaf_open(x,device,out,sharpminato4_profiles,4,2004);
 lt48_catalogue_entry sh13;if(!lt48_installation_get_device(x,device,&sh13)&&sh13.device_class==2004&&sh13.device_type==0x50&&!strcmp(sh13.vendor,"SHARP"))return gaaf_open(x,device,out,sharp1306_profiles,1,2004);
 lt48_catalogue_entry s37;if(!lt48_installation_get_device(x,device,&s37)&&s37.device_class==2007&&s37.device_type>=0xb7&&s37.device_type<=0xba&&!strcmp(s37.vendor,"SST"))return gaaf_open(x,device,out,sst37four_profiles,4,2007);
 lt48_catalogue_entry sh10;if(!lt48_installation_get_device(x,device,&sh10)&&sh10.device_class==2008&&(sh10.device_type==0x17||sh10.device_type==0x18||(sh10.device_type>=0x71&&sh10.device_type<=0x78)))return gaaf_open(x,device,out,sharp10_profiles,10,2008);
 lt48_catalogue_entry se;if(!lt48_installation_get_device(x,device,&se)&&se.device_class==2029&&se.device_type>=0x10&&se.device_type<=0x19)return gaaf_open(x,device,out,sharp11_profiles,sizeof sharp11_profiles/sizeof*sharp11_profiles,2029);
 lt48_catalogue_entry n3;if(!lt48_installation_get_device(x,device,&n3)){if(n3.device_class==2007&&n3.device_type==0x9c)return gaaf_open(x,device,out,nor3_2007_profiles,1,2007);if(n3.device_class==2008&&(n3.device_type==0x39||n3.device_type==0x3a))return gaaf_open(x,device,out,nor3_2008_profiles,2,2008);}
 lt48_catalogue_entry ne;if(!lt48_installation_get_device(x,device,&ne)&&ne.device_class==2009&&ne.device_type>=0x5b&&ne.device_type<=0x62&&ne.device_type!=0x5f)return gaaf_open(x,device,out,lnand10_profiles,sizeof lnand10_profiles/sizeof*lnand10_profiles,2009);
 lt48_catalogue_entry sep_entry;if(lt48_installation_get_device(x,device,&sep_entry)==LT48_OK&&sep_entry.device_class==2009&&sep_entry.device_type>=0x64&&sep_entry.device_type<=0x68)return gaaf_open(x,device,out,sep7_profiles,sizeof sep7_profiles/sizeof*sep7_profiles,2009);
 lt48_catalogue_entry e;
 if(lt48_installation_get_device(x,device,&e)==LT48_OK) {
  if(w49_dummy(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length)||w49_t16(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_w49_open_private(x,device,out);
  if(actrans4(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_actrans4_open_private(x,device,out);
  if(e.device_class==2009&&(e.device_type==0x13||e.device_type==0x14)&&e.vendor&&!strcmp(e.vendor,"ST")){if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;return gaaf_open(x,device,out,st2009two_profiles,2,2009);}
  if(e.device_class==2004&&e.device_type==3&&e.vendor&&!strcmp(e.vendor,"SST")&&e.device_name&&e.device_name_length==13&&!memcmp(e.device_name,"38VF166 *48TS",13)){if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;return gaaf_open(x,device,out,sst38one_profiles,1,2004);}
 if(e.device_class==2004 && e.device_type>=0x65u && e.vendor && (!strcmp(e.vendor,"Fujitsu")||!strcmp(e.vendor,"Toshiba"))){lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(x,device,&spec);if(st)return st;if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10")||!combined36_binding(&spec))return LT48_UNSUPPORTED;return gaaf_open(x,device,out,combined36_profiles,sizeof combined36_profiles/sizeof*combined36_profiles,2004);}
  if(e.device_class==2004 && e.manufacturer_id==0x89u && e.vendor && !strcmp(e.vendor,"Intel")) {
   static const struct {unsigned definition,id;const char *name;} f3[]={
    {0x4du,0x88f2u,"DE28F800F3B @56SS"},{0x4cu,0x88f1u,"DE28F800F3T @56SS"},
    {0x4fu,0x88f4u,"DT28F160F3B @56SS"},{0x4eu,0x88f3u,"DT28F160F3T @56SS"},
    {0x4du,0x88f2u,"DT28F800F3B @56SS"},{0x4cu,0x88f1u,"DT28F800F3T @56SS"}};
   for(size_t i=0;i<sizeof(f3)/sizeof(*f3);i++)
    if(e.device_type==f3[i].definition && e.device_id==f3[i].id &&
       e.device_name_length==strlen(f3[i].name) && !memcmp(e.device_name,f3[i].name,e.device_name_length))
     return lt48_legacy_intel_f3_open_internal(x,device,out);
   return LT48_UNSUPPORTED;
  }
  if(mx26b(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_byte49_open_internal(x,device,out);
  if(m29f800(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length)||m28f800(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_motorola8_open_internal(x,device,out);
  if(macronix_l3211(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_macronix_l3211_open_internal(x,device,out);
  if(macronix_f1601(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_macronix_f1601_open_internal(x,device,out);
  if(macronix_l8000(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_macronix_l8000_open_internal(x,device,out);
  if(macronix_l8100(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_macronix_l8100_open_internal(x,device,out);
  if(macronix_psop3(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_macronix_psop3_open_internal(x,device,out);
  if(e.device_class==2005 && e.manufacturer_id==0xc2u && e.vendor && !strcmp(e.vendor,"Macronix")) {
   static const struct {unsigned definition,id;const char *name;} page[]={
    {0x2e,0xf1,"MX29F1610 *48TS"},{0x2f,0xfa,"MX29F1610A *48TS"},
    {0x30,0xfb,"MX29F1610B *48TS"},{0x32,0x88,"MX29F8100E *48TS"},
    {0x32,0x88,"MX29F8101E *48TS"},{0x31,0xf8,"MX29L1611 *48TS"}};
   for(size_t i=0;i<sizeof(page)/sizeof(*page);i++)
    if(e.device_type==page[i].definition&&e.device_id==page[i].id&&e.device_name_length==strlen(page[i].name)&&!memcmp(e.device_name,page[i].name,e.device_name_length))
     return lt48_legacy_macronix6_open_internal(x,device,out);
   return LT48_UNSUPPORTED;
  }
  if(e.device_class==2007) {
   if(ams_nor(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_byte49_open_internal(x,device,out);
   if(sharp_s3(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))return lt48_legacy_byte49_open_internal(x,device,out);
   if(fujitsu_002(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   /* Intel8: current native/source/FPGA composition, with full-array
    * erase verification. Exact source digest and profile tuple are checked
    * by the private loader; admission is restricted to these identities. */
   if(e.manufacturer_id==0x89u && e.vendor && !strcmp(e.vendor,"Intel")) {
    static const struct {unsigned definition,id;const char *name;} intel8[]={
    {0x4bu,0x75u,"E28F200BV-B @56"},
    {0x4au,0x74u,"E28F200BV-T @56"},
    {0x4bu,0x75u,"E28F200BX/BL-B @56"},
    {0x4au,0x74u,"E28F200BX/BL-T @56"},
    {0x4du,0x71u,"E28F400BX/BL-B @56"},
    {0x4cu,0x70u,"E28F400BX/BL-T @56"},
    {0x48u,0x71u,"TE28F400BV-B @56"},
    {0x49u,0x70u,"TE28F400BV-T @56"},
    };
    for(size_t n=0;n<sizeof(intel8)/sizeof(*intel8);n++)
     if(e.device_type==intel8[n].definition && e.device_id==intel8[n].id &&
        e.device_name_length==strlen(intel8[n].name) &&
        !memcmp(e.device_name,intel8[n].name,e.device_name_length))
      return lt48_legacy_byte49_open_internal(x,device,out);
   }
   if(st28(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if(sst1(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if(mx28f1000_auto(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_mx28f1000_auto_open_internal(x,device,out);
   if(mx28p2(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if(alliance2(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if(lt48_batch_find(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if(nexflash_010(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if(macronix_lv040(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if(macronix_bootblock4(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length))
    return lt48_legacy_byte49_open_internal(x,device,out);
   /* These exact byte49 identities have complete native/FPGA qualification.
    * The private loader additionally checks the exact resource digest,
    * definition/family/pin-map/capacity tuple and operation payloads. */
   if(e.vendor && e.manufacturer_id==0x31u && !strcmp(e.vendor,"Catalyst") &&
      e.device_name_length==12 &&
      ((e.device_type==0x85u && e.device_id==0x95u && !memcmp(e.device_name,"CAT28F001N-B",12)) ||
       (e.device_type==0x86u && e.device_id==0x94u && !memcmp(e.device_name,"CAT28F001N-T",12))))
    return lt48_legacy_byte49_open_internal(x,device,out);
   /* Same-boot exact Catalyst erase recipe; each Intel identity retains
    * its own read/program/check evidence. See the shared-erase audit. */
   if(e.vendor && e.manufacturer_id==0x89u && !strcmp(e.vendor,"Intel") &&
      e.device_name_length==11 &&
      ((e.device_type==0x85u && e.device_id==0x95u && !memcmp(e.device_name,"P28F001BX-B",11)) ||
       (e.device_type==0x86u && e.device_id==0x94u && !memcmp(e.device_name,"P28F001BX-T",11))))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if(e.device_type==0xc0u && e.manufacturer_id==0xdau && e.device_id==0x38u &&
      e.vendor && !strcmp(e.vendor,"Winbond") && e.device_name_length==7 &&
      !memcmp(e.device_name,"W39L512",7))
    return lt48_legacy_byte49_open_internal(x,device,out);
   /* Composed source/FPGA erase and native owner evidence, with each
    * capacity's own operation and address-boundary qualification. */
   if(e.manufacturer_id==0xdau && e.vendor && !strcmp(e.vendor,"Winbond") &&
      e.device_name_length==7 &&
      ((e.device_type==0xc2u && e.device_id==0x31u && !memcmp(e.device_name,"W39L010",7)) ||
       (e.device_type==0xc3u && e.device_id==0xb5u && !memcmp(e.device_name,"W39L020",7)) ||
       (e.device_type==0xc4u && e.device_id==0xb6u && !memcmp(e.device_name,"W39L040",7))))
    return lt48_legacy_byte49_open_internal(x,device,out);
   if((e.device_type==23||e.device_type==24)&&!sm2964two_member(&e))return LT48_UNSUPPORTED;
   if(e.device_type==197&&!efst1_member(&e))return LT48_UNSUPPORTED;
   return lt48_legacy_nor53_open_internal(x,device,out);
  }
  if(e.device_class==2003) {
   if(e.device_class==2003&&!strcmp(e.vendor,"SHARP")&&(e.device_type==0x168||e.device_type==0x169))return gaaf_open(x,device,out,sharpwordpulse3_profiles,2,2003);
   if(sharpwordseven_key(&e))return gaaf_open(x,device,out,sharpwordseven_profiles,sizeof sharpwordseven_profiles/sizeof*sharpwordseven_profiles,2003);
   lt48_status ut=lt48_utron4_open_internal(x,device,out);if(ut!=LT48_UNSUPPORTED)return ut;
   lt48_status w14=lt48_word14_open_internal(x,device,out);if(w14!=LT48_UNSUPPORTED)return w14;
   if(e.device_type==0x149u||e.device_type==0x14au)return samsung_word2_open(x,device,out);
   if(e.device_type==0xa3u||e.device_type==0xa4u||e.device_type==0x16u||e.device_type==0xa0u||e.device_type==0xa1u||e.device_type==0x165u)
    return lt48_legacy_word_remaining9_open_internal(x,device,out);
   if(e.device_type>=0x4du && e.device_type<=0x54u)
    return lt48_legacy_hynix8_open_internal(x,device,out);
   if(e.device_type>=0xdau && e.device_type<=0xdfu)
    return lt48_legacy_atmel_small7_open_internal(x,device,out);
   if(e.device_type>=0xe0u && e.device_type<=0xebu)
    return lt48_legacy_atmel_classic12_open_internal(x,device,out);
   if(atmel_lv2048b(e.device_class,e.device_type,e.manufacturer_id,e.device_id,e.vendor,e.device_name,e.device_name_length)||(e.device_type>=0xf5u && e.device_type<=0x107u && e.device_type!=0xfeu))
    return lt48_legacy_atmel20_open_internal(x,device,out);
   return lt48_legacy_word46_open_internal(x,device,out);
  }
  if(e.device_class==2008)return lt48_legacy_route45_open_internal(x,device,out);
  if(e.device_class==2001){
   if(e.device_type==0x77&&!strcmp(e.vendor,"Hitachi")){if(!LT48_HITACHIPULSE2_ADMITTED)return LT48_UNSUPPORTED;return gaaf_open(x,device,out,hitachipulse2_secure_profiles,1,2001); }
   if(e.device_type>=0x3c&&e.device_type<=0x40){
    if(NOVATEK_UMC6_STAGE<(e.device_type>=0x3f?2:1))return LT48_UNSUPPORTED;
    return gaaf_open(x,device,out,novatek_umc6_secure_profiles,5,2001);
   }
   return gaaf_open(x,device,out,tms15_profiles,1,2001);
  }
  if(e.device_class==2006)return gaaf_open(x,device,out,tms25_profiles,1,2006);
 }
 if(lt48_installation_get_device(x,device,&e)==LT48_OK && e.device_class==2002)
  return gaaf_open(x,device,out,legacy28f_profiles,sizeof(legacy28f_profiles)/sizeof(*legacy28f_profiles),2002);
 return gaaf_open(x,device,out,profiles,sizeof(profiles)/sizeof(profiles[0]),2009);
}
lt48_status lt48_fujitsu41_open_internal(const lt48_installation*x,size_t d,lt48_gaaf_serial_resources**out){
 lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(x,d,&spec);if(st)return st;
 if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10")||!fujitsu41_binding(&spec))return LT48_UNSUPPORTED;
 return gaaf_open(x,d,out,fujitsu41_profiles,sizeof fujitsu41_profiles/sizeof*fujitsu41_profiles,2009);
}
lt48_status lt48_hitachi15_open_internal(const lt48_installation*x,size_t d,lt48_gaaf_serial_resources**out){return gaaf_open(x,d,out,h15_profiles,sizeof h15_profiles/sizeof*h15_profiles,2009);}
lt48_status lt48_gaaf_hitachi_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 return gaaf_open(x,device,out,hitachi_profiles,sizeof(hitachi_profiles)/sizeof(hitachi_profiles[0]),2009);
}

void lt48_gaaf_serial_frame_init(const lt48_gaaf_serial_resources *r,lt48_gaaf_serial_frame *frame) {
 if(!r||!frame)return;
 memset(frame,0,sizeof(*frame));memcpy(frame->variables,r->variables,sizeof(frame->variables));frame->mode=1;
 if(r->st2009two){frame->configuration_count=1;frame->configuration[0]=(lt48_runtime_configuration_value){0,1,0,0};}
 if(r->micronc3two){frame->configuration_count=5;for(unsigned i=0;i<5;i++)frame->configuration[i]=(lt48_runtime_configuration_value){0,i?2u:0u,i?i-1:0u,0};}
 /* Original word-ROM setup reads two host option selections, not chip fuses. */
 if(r->utron4){frame->configuration_count=2;for(unsigned i=0;i<2;i++)frame->configuration[i]=(lt48_runtime_configuration_value){0,1,i,0};}
}
/* Commit only a complete, valid result. A failed wave cannot partially replace
 * the owning session's saved configuration or variable frame. */
lt48_status lt48_gaaf_serial_frame_commit(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,const lt48_runtime_result *result) {
 if(!r||!frame||!result)return LT48_INVALID_ARGUMENT;
 lt48_gaaf_serial_frame next=*frame;lt48_status status;
 for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
  lt48_runtime_event e;status=lt48_runtime_get_event(result,i,&e);if(status!=LT48_OK)return status;
  if(e.kind==LT48_RUNTIME_RESULT_LATCH||(e.kind==LT48_RUNTIME_RETURN_STATUS&&e.argument_count==1&&!e.arguments[0]))return LT48_RUNTIME_ERROR;
 }
 for(unsigned i=0;i<LT48_GAAF_BUFFERS;i++) {
  const unsigned char *bytes;size_t size;
  status=lt48_runtime_get_buffer(result,i,&bytes,&size);if(status!=LT48_OK)return status;
  if(size!=32)return LT48_RUNTIME_ERROR;
  memcpy(next.buffers[i],bytes,32);
 }
 for(size_t i=0;i<r->variable_count;i++) {
  status=lt48_runtime_get_variable(result,r->variables[i].variable_id,&next.variables[i]);if(status!=LT48_OK)return status;
 }
 for(unsigned i=0;i<4;i++){status=lt48_runtime_get_voltage(result,i,&next.volts[i]);if(status!=LT48_OK)return status;}
 const lt48_runtime_configuration_value *config;size_t count;
 status=lt48_runtime_get_configuration(result,&config,&count);if(status!=LT48_OK)return status;
 if(count>sizeof(next.configuration)/sizeof(*next.configuration))return LT48_RUNTIME_ERROR;
 for(size_t i=0;i<count;i++) {
  if(r->st2009two){unsigned n=r->definition==0x14?8:4;if(config[i].object_id||(config[i].category==0?(config[i].field>=n||config[i].value>1):(config[i].category!=1||config[i].field||config[i].value>1)))return LT48_SAFETY_REJECTED;continue;}
  if(r->micronc3two){if(config[i].object_id||(config[i].category==0?(config[i].field||config[i].value>1):(config[i].category!=2||config[i].field>=4||config[i].value>65535)))return LT48_SAFETY_REJECTED;continue;}
  unsigned category=r->legacy_nor?r->legacy_config_category:0;
  unsigned limit=category==1?65535u:1u;
  if(config[i].object_id!=0||config[i].category!=category||config[i].value>limit||
     (r->legacy_nor?(config[i].field>=32||!(r->legacy_config_mask&(1u<<config[i].field))):config[i].field>=r->configuration_bits))return LT48_SAFETY_REJECTED;
 }
 if(count)memcpy(next.configuration,config,count*sizeof(*config));
 next.configuration_count=count;
 *frame=next;return LT48_OK;
}
/* The two 5000 images are alternatives, not simultaneously resident. */
lt48_status lt48_legacy_nor53_applet_internal(const lt48_gaaf_serial_resources *r,
 unsigned mode,const lt48_wave_code **out) {
 if(!r||!r->legacy_nor||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 /* ACTRANS uses compact UNPROTECT/ERASE; no erase RAM image exists. */
 if((r->pss2005one||r->legacy_actrans||r->legacy_w49||r->sharpminato4) && mode==7)return LT48_OK;
 if(r->legacy_page) {
  unsigned page_op=mode==2?(r->pss2005one?11u:9u):mode==4||mode==5?8u:mode==11?7u:mode==7?18u:0u;
  if(page_op) {
   if(!r->applets[page_op].data_size)return LT48_UNSUPPORTED;
   *out=&r->applets[page_op];return LT48_OK;
  }
  return mode==0||mode==1||mode==8?LT48_OK:LT48_UNSUPPORTED;
 }
 unsigned op=r->legacy_dummy?(mode==1?15u:mode==2?13u:mode==4||mode==5?11u:mode==11?12u:0u):r->legacy_f3?(mode==1?16u:mode==2?14u:mode==4||mode==5?12u:mode==11?13u:mode==7?15u:0u):r->legacy_route?(mode==11?4:mode==4||mode==5?8:mode==2?6:0):r->legacy_word?(mode==11?14:mode==4||mode==5?15:mode==2?17:0):(mode==11?12:mode==4||mode==5?13:mode==2?14:0);
 /* Byte compact erase may jump to wave 15. Unlike inline Winbond erase,
  * Intel/SST procedures require this image resident before DEVINIT(7). */
 if(!r->legacy_dummy && !r->legacy_route && !r->legacy_word && mode==7 && r->applets[15].data_size)op=15;
 if(r->legacy_word && mode==7 && r->legacy_erase_applet) {
  op=r->legacy_erase_applet;
  if(op>=LT48_GAAF_OPERATIONS||!r->applets[op].data_size)return LT48_UNSUPPORTED;
 }
 if(op)*out=&r->applets[op];
 else if(mode!=0&&mode!=1&&mode!=7&&mode!=8)return LT48_UNSUPPORTED;
 return LT48_OK;
}

lt48_status lt48_gaaf_serial_wave(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,unsigned operation) {
 if(!r||!frame||!wire||operation>LT48_GAAF_OPERATIONS+1 ||
    (operation==LT48_GAAF_OPERATIONS+1&&!r->id_setup) ||
    (operation==LT48_GAAF_OPERATIONS&&!r->legacy_page))return LT48_INVALID_ARGUMENT;
 if(r->legacy_nor && operation==3) {
  const lt48_wave_code *a=NULL;
  lt48_status select=lt48_legacy_nor53_applet_internal(r,frame->mode,&a);
  if(select!=LT48_OK)return select;
  const lt48_wave_code *images[2]={a,r->legacy_motorola && frame->mode==2 && r->applets[10].data_size?&r->applets[10]:NULL};
  for(unsigned image=0;image<2;image++)if((a=images[image])) {
   size_t last=a->data_size-1;
   unsigned char header[]={0x39,0xaa,0x55,a->load_address&255,a->load_address>>8,last&255,last>>8};
   select=lt48_runtime_transport_session_controller_write(wire,header,sizeof(header));
   if(select==LT48_OK)select=lt48_runtime_transport_session_controller_write(wire,a->data,a->data_size);
   if(select!=LT48_OK)return select;
  }
 }

 const lt48_compiled_wave *wave=operation==LT48_GAAF_OPERATIONS+1?r->id_setup:operation==LT48_GAAF_OPERATIONS?r->page_read_reset:r->waves[operation];
 if(!wave)return LT48_UNSUPPORTED;
 lt48_runtime_options opts={0};lt48_runtime_buffer_seed seeds[LT48_GAAF_BUFFERS-1];size_t n=0;
 for(unsigned i=0;i<LT48_GAAF_BUFFERS;i++)if(i!=r->frame_profile.mode_buffer)seeds[n++]=(lt48_runtime_buffer_seed){i,frame->buffers[i],32};
 /* The exact ST D59 original CLRALL lowers programming rails to5V before off. */
 opts.host_staged_shutdown=(r->legacy_nor&&!r->legacy_word&&!r->legacy_route&&!r->legacy_f3&&!r->legacy_page&&r->definition==0x59)?1:r->sharpminato4||r->sharp10||r->sharp11||r->nor3||r->sep7||r->lnand10?4:r->micronc3two||r->sharpwordpulse3||r->word14||r->sharpwordseven||(r->legacy_word&&(r->definition==0x149u||r->definition==0x14au));
 unsigned char retry_parameter=0;
 if(r->tms320e==15&&r->definition==0x77&&(operation==3||operation==5)){
  retry_parameter=(unsigned char)hitachipulse2_retry(frame->variables,r->variable_count);
  if(!retry_parameter)return LT48_SAFETY_REJECTED;
  opts.parameter_buffer=&retry_parameter;opts.parameter_size=1;
 }
 opts.mode=frame->mode;opts.use_explicit_buffer_ids=1;opts.mode_buffer=r->frame_profile.mode_buffer;opts.address_buffer=r->legacy_page?2u:r->legacy_f3?1u:0u;opts.parameter_buffer_id=r->legacy_page?15u:r->legacy_f3?2u:r->legacy_route?5u:1u;
 opts.initial_buffers=seeds;opts.initial_buffer_count=n;opts.root_timeout_seconds=300;
 opts.nested_waves=r->nested;opts.nested_wave_count=r->nested_count;
 opts.configuration=frame->configuration;opts.configuration_count=frame->configuration_count;
 memcpy(opts.initial_voltage_centivolts,frame->volts,sizeof(frame->volts));
 lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
 lt48_status status=lt48_runtime_transport_session_wave_watchdog(wire,300000,1);
 if(status==LT48_OK)status=lt48_runtime_machine_create(wave,r->pins,frame->variables,r->variable_count,&opts,&machine);
 if(status==LT48_OK&&r->sep7&&operation==5)status=lt48_runtime_machine_stream_resume_internal(machine);
 if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(wire,machine,r->addresses,r->address_count,&result);
 if(status==LT48_OK)status=lt48_gaaf_serial_frame_commit(r,frame,result);
 lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}

lt48_status lt48_gaaf_serial_setup(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire) {
 if(!r||!frame||!wire)return LT48_INVALID_ARGUMENT;
 lt48_status status=LT48_OK;
 for(size_t i=0;status==LT48_OK&&(!r->legacy_nor||r->legacy_route)&&i<r->address_count;i++) {
  const lt48_wave_code *a=&r->applets[r->addresses[i].wave];size_t last=a->data_size-1;
  unsigned char header[]={0x39,0xaa,0x55,a->load_address&255,a->load_address>>8,last&255,last>>8};
  status=lt48_runtime_transport_session_controller_write(wire,header,sizeof(header));
  if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,a->data,a->data_size);
 }
 const unsigned char fpga[]={0x38,0xaa,0x55},selector[]={0x0f,r->nominal>400?15:7};
 if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,fpga,sizeof(fpga));
 if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,r->fpga,2778);
 if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,selector,sizeof(selector));
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,0);
 return status;
}
static lt48_status gaaf_io(const lt48_protocol_transport *api,void *ctx,int input,
 uint8_t endpoint,unsigned char *bytes,size_t size) {
 if(api->cancelled&&api->cancelled(ctx))return LT48_CANCELLED;
 size_t done=0;lt48_status status=input?api->read(ctx,endpoint,bytes,size,&done):api->write(ctx,endpoint,bytes,size,&done);
 return status==LT48_OK&&done!=size?LT48_PROTOCOL_ERROR:status;
}
static lt48_status gaaf_command(const lt48_protocol_transport *api,void *ctx,
 lt48_runtime_transport_session *wire,const unsigned char *command,size_t size,unsigned char *reply) {
 if(size>512)return LT48_INVALID_ARGUMENT;
 unsigned char packet[512]={0};memcpy(packet,command,size);
 lt48_status status=lt48_runtime_transport_session_flush(wire);
 if(status==LT48_OK)status=gaaf_io(api,ctx,0,2,packet,512);
 if(status==LT48_OK&&reply)status=gaaf_io(api,ctx,1,0x88,reply,512);
 return status;
}
/* The caller owns the applet-specific address and range contract. */
static lt48_status gaaf_stream_range(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 const lt48_protocol_transport *api,void *ctx,unsigned mode,unsigned vcc,
 unsigned char *bytes,size_t size,uint32_t start) {
 if(!r||!frame||!wire||!api||!api->read||!api->write||!bytes||!size||start>=r->capacity||size>r->capacity-start||
    (mode!=1&&mode!=2&&mode!=4&&mode!=5&&mode!=11&&!(r->legacy28f&&(mode==6||mode==7)))||vcc>65535)return LT48_INVALID_ARGUMENT;
 if(r->micronc3two)vcc=micronc3two_voltage(vcc);
 if(r->sst38one||r->st2009two){if(vcc<270)vcc=270;if(vcc>360)vcc=360;}
 unsigned width=(r->legacy_word||r->legacy_f3)?2u:1u;
 if((start%width)||(size%width))return LT48_SAFETY_REJECTED;
 int found=0;
 for(size_t i=0;i<r->variable_count;i++)if(frame->variables[i].kind==0&&frame->variables[i].variable_id==r->vcc_variable){frame->variables[i].value=(uint16_t)vcc;found=1;}
 if(!found)return LT48_FORMAT_ERROR;
 frame->mode=mode;
 lt48_status status=lt48_runtime_frame_set_mode(frame->buffers,r->frame_profile.buffer_count,
  r->frame_profile.reset_mask,r->frame_profile.mode_buffer,(uint16_t)mode,0,0);
 if(status==LT48_OK)for(unsigned i=0;i<4;i++)frame->buffers[r->legacy_f3?1u:0u][i]=(unsigned char)((start/width)>>(8*i));
 /* M4/M5/M6 DEVINIT does not set DAC0. Replay its original POWERON
  * with the requested VCC before selecting the worker, including margins. */
 if(status==LT48_OK&&r->hitachi15)status=lt48_gaaf_serial_wave(r,frame,wire,0);
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,r->hitachi15&&mode==1?18:3);
 unsigned char reply[512]={0};size_t pos=0;int transmit=mode!=1;
 if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(wire,300000,0);
 if(status==LT48_OK&&transmit) {
  for(unsigned i=0;status==LT48_OK&&i<width;i++) {
   unsigned char first[]={4,bytes[i]};status=gaaf_command(api,ctx,wire,first,2,reply);
   if(status==LT48_OK&&!reply[0])status=LT48_PROTOCOL_ERROR;
   /* FX2 04 returns transport completion in byte 0 and the sampled
    * compact-app status in byte 2. A failed first byte must not be followed
    * by a bulk transfer to an applet that has already returned. */
   if(status==LT48_OK&&(r->first_byte_status||r->lnand10||r->sep7||r->legacy_nor||r->fujitsu41||r->hitachi15)&&!(reply[2]&1))status=LT48_DEVICE_MISMATCH;
  }
  pos=width;
 }
 while(status==LT48_OK&&pos<size) {
  size_t length=size-pos;if(length>8192)length=8192;
  /* Class 2007 reads use CDrvUSB +0x40 (00496bb0): sample first,
   * then advance with D0. USB43 instead sends its selector before sampling
   * and remains required by the separate RAM-applet read routes. */
  int raw_read=r->legacy_nor&&!r->legacy_word&&!r->legacy_route&&!r->legacy_f3;
  unsigned char h[]={transmit?0x47:raw_read?0x46:0x43,length/512&255,length/512>>8,length%512&255,length%512>>8,transmit?(unsigned char)width:raw_read?0xd0:0};
  status=lt48_runtime_transport_session_wave_watchdog(wire,300000,0);
  if(status==LT48_OK)status=gaaf_command(api,ctx,wire,h,sizeof(h),NULL);
  if(status==LT48_OK)status=gaaf_io(api,ctx,!transmit,transmit?4:0x88,bytes+pos,length);
  if(status==LT48_OK&&transmit) {
   status=gaaf_io(api,ctx,1,0x88,reply,512);
   if(status==LT48_OK&&!(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;

  }
  if(status==LT48_OK)pos+=length;
 }
 if(status==LT48_OK) {
  const unsigned char end[]={0x40,0};status=gaaf_command(api,ctx,wire,end,2,reply);
  if(status==LT48_OK&&!reply[0])status=LT48_PROTOCOL_ERROR;
  if(status==LT48_OK&&transmit&&!reply[2])status=LT48_DEVICE_MISMATCH;
 }
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,r->reset_op);
 return status;
}

lt48_status lt48_legacy_nor53_stream_internal(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *f,lt48_runtime_transport_session *w,const lt48_protocol_transport *api,void *ctx,
 unsigned mode,unsigned vcc,unsigned char *bytes,size_t size,uint32_t start) {
 if(!r||!r->legacy_nor)return LT48_UNSUPPORTED;
 return gaaf_stream_range(r,f,w,api,ctx,mode,vcc,bytes,size,start);
}

lt48_status lt48_legacy28f_stream_internal(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *f,lt48_runtime_transport_session *w,const lt48_protocol_transport *api,void *ctx,
 unsigned mode,unsigned vcc,unsigned char *bytes,size_t size,uint32_t start) {
 if(!r||!r->legacy28f)return LT48_UNSUPPORTED;
 return gaaf_stream_range(r,f,w,api,ctx,mode,vcc,bytes,size,start);
}

/* Legacy serial applets start at zero and need a preserved full array. */
lt48_status lt48_gaaf_serial_stream(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 const lt48_protocol_transport *api,void *ctx,unsigned mode,unsigned vcc,
 unsigned char *bytes,size_t size) {
 if(!r||size!=r->capacity)return LT48_INVALID_ARGUMENT;
 return gaaf_stream_range(r,frame,wire,api,ctx,mode,vcc,bytes,size,0);
}
lt48_status lt48_gaaf_hitachi_stream_internal(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 const lt48_protocol_transport *api,void *ctx,unsigned mode,unsigned vcc,
 unsigned char *bytes,size_t size,uint32_t start) {
 if(mode!=1&&mode!=2&&mode!=4&&mode!=11)return LT48_UNSUPPORTED;
 if(mode==2&&((start&127)||(size&127)))return LT48_INVALID_ARGUMENT;
 return gaaf_stream_range(r,frame,wire,api,ctx,mode,vcc,bytes,size,start);
}

/* Recovery deliberately uses fresh queues for independent steps. A poisoned
 * transfer queue or cancellation must not prevent reset, power-off or rail-off. */
lt48_status lt48_gaaf_serial_cleanup(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,const lt48_protocol_transport *api,void *ctx,
 lt48_status operation_status) {
 if(!r||!frame||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 lt48_protocol_transport recovery=*api;recovery.cancelled=NULL;
 lt48_status status=LT48_OK;
 for(unsigned phase=0;phase<9;phase++) {
  lt48_runtime_transport_session *wire=NULL;
  lt48_status step=lt48_runtime_transport_session_create(&recovery,ctx,&wire);
  if(step==LT48_OK) {
   if(phase==0) {
    const unsigned char stop[]={0,0};unsigned char reply[512]={0};
    step=gaaf_command(&recovery,ctx,wire,stop,sizeof(stop),reply);
    if(step==LT48_OK&&!(reply[0]|reply[1]))step=LT48_PROTOCOL_ERROR;
   } else if(phase==1||phase==2) {
    step=lt48_gaaf_serial_wave(r,frame,wire,phase==1?r->reset_op:1);
    if(step==LT48_OK)step=lt48_runtime_transport_session_flush(wire);
   } else if(phase<7) {
    unsigned char off[]={0x10,(unsigned char)(6-phase),0};
    step=lt48_runtime_transport_session_controller_write(wire,off,sizeof(off));
    if(step==LT48_OK)step=lt48_runtime_transport_session_flush(wire);
   } else if(phase==7) {
    const unsigned char header[]={0x38,0xaa,0x55},finish[]={0x49,0xaa,0x55};
    step=lt48_runtime_transport_session_controller_write(wire,header,sizeof(header));
    if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(wire,r->idle,r->idle_size);
    if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(wire,finish,sizeof(finish));
    if(step==LT48_OK)step=lt48_runtime_transport_session_flush(wire);
   } else {
    const unsigned char led[]={0x40,status==LT48_OK&&operation_status==LT48_OK?4:2};
    step=lt48_runtime_transport_session_controller_write(wire,led,sizeof(led));
    if(step==LT48_OK)step=lt48_runtime_transport_session_flush(wire);
   }
  }
  lt48_runtime_transport_session_close(wire);
  if(status==LT48_OK&&step!=LT48_OK)status=step;
 }
 return status;
}

lt48_status lt48_gaaf_serial_erase(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire) {
 if(!r||!frame||!wire)return LT48_INVALID_ARGUMENT;
 if(!r->waves[5])return LT48_UNSUPPORTED;
 frame->mode=7;
 for(size_t i=0;i<r->variable_count;i++)if(frame->variables[i].kind==0&&frame->variables[i].variable_id==r->vcc_variable)frame->variables[i].value=(uint16_t)r->nominal;
 lt48_status status=lt48_runtime_frame_set_mode(frame->buffers,r->frame_profile.buffer_count,
  r->frame_profile.reset_mask,r->frame_profile.mode_buffer,7,0,0);
 /* Original44F750 loads7DFD10,7DFCE4,7DFD0C in that order. The SST
  * applet does not return a byte; no invented completion read belongs here. */
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,3);
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,5);
 if(status==LT48_OK&&r->st2009two&&frame->buffers[3][0]!=1)status=LT48_DEVICE_MISMATCH;
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,r->reset_op);
 return status;
}
lt48_status lt48_gaaf_serial_read_id(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 uint16_t *manufacturer,uint16_t *device) {
 if(!r||!frame||!wire||!manufacturer||!device)return LT48_INVALID_ARGUMENT;
 if(!r->waves[2])return LT48_UNSUPPORTED;
 unsigned man_buffer=r->legacy_page?19u:r->legacy_f3?5u:r->legacy_route?7u:(r->legacy28f||r->tms320e)?3u:4u,dev_buffer=r->legacy_route?6u:man_buffer+1;
 memset(frame->buffers[man_buffer],0,32);memset(frame->buffers[dev_buffer],0,32);
 for(size_t i=0;i<r->variable_count;i++)if(frame->variables[i].kind==0&&frame->variables[i].variable_id==r->vcc_variable)frame->variables[i].value=(uint16_t)r->nominal;
 lt48_status status=LT48_OK;
 if(r->id_setup)status=lt48_gaaf_serial_wave(r,frame,wire,LT48_GAAF_OPERATIONS+1);
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,2);
 if(status==LT48_OK) {
  *manufacturer=(uint16_t)(frame->buffers[man_buffer][0]|(unsigned)frame->buffers[man_buffer][1]<<8);
  *device=(uint16_t)(frame->buffers[dev_buffer][0]|(unsigned)frame->buffers[dev_buffer][1]<<8);
 }
 return status;
}

static lt48_status sep7_validate(const lt48_gaaf_serial_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
lt48_status lt48_gaaf_serial_validate(const lt48_gaaf_serial_resources *r,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t image_size,
 lt48_verify_margin_selection margin,unsigned margin_type) {
 if(r&&r->word14&&r->nominal==330&&margin_type==1&&
    (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE||
     (op==LT48_OPERATION_VERIFY&&(margin==LT48_VERIFY_MARGIN_ALL||margin==LT48_VERIFY_MARGIN_HIGH))))return LT48_SAFETY_REJECTED;
 if(r&&r->mx27pageone&&op>LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if(r&&r->lnand10){if(op==4||op>7)return LT48_UNSUPPORTED;if((op==3||op==7)&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;}
 if(r&&r->sep7)return sep7_validate(r,op,start,length,image,image_size,margin,margin_type);
 if(r&&r->word14&&(r->utron4||r->definition==0x39)&&op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if(r&&r->sst38one&&((start&1u)||(length&1u)||((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE)&&(start||length!=r->capacity))))return LT48_SAFETY_REJECTED;
 if(r&&r->combined36)return lt48_combined36_validate_internal(r,op,start,length,image,image_size,margin,margin_type);
 if(!r||!length||start>=r->capacity||length>r->capacity-start)return LT48_INVALID_ARGUMENT;
 if(r->st2009two&&(op>7||((op==3||op==4||op==7)&&(start||length!=r->capacity))))return LT48_SAFETY_REJECTED;
 if((r->legacy_word||r->legacy_f3||(r->legacy_page&&!r->legacy_page_byte))&&((start&1u)||(length&1u)))return LT48_SAFETY_REJECTED;
 if(r->sm2964two&&op==LT48_OPERATION_SECURE)return start==0&&length==r->capacity&&!image&&!image_size?LT48_OK:LT48_SAFETY_REJECTED;
 if(r->legacy_nor&&op>LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
 if(r->tms320e)return op==LT48_OPERATION_SECURE&&start==0&&length==r->capacity?LT48_OK:LT48_UNSUPPORTED;
 if((r->legacy28f||r->legacy_nor)&&op==LT48_OPERATION_ERASE&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
 if(!((r->st2009two||r->legacy28f||r->legacy_nor)&&op==LT48_OPERATION_OVER_PROGRAM)&&op!=LT48_OPERATION_SECURE&&op!=LT48_OPERATION_CONFIGURATION&&op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_BLANK_CHECK)return LT48_UNSUPPORTED;
 if((op==LT48_OPERATION_SECURE||op==LT48_OPERATION_CONFIGURATION)&&(r->configuration_bits!=8&&r->configuration_bits!=9))return LT48_UNSUPPORTED;
 if(op==LT48_OPERATION_CONFIGURATION&&(!image||image_size!=(r->configuration_bits==9?2u:1u)||(r->configuration_bits==9&&image[1]>1)))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||image_size!=length))return LT48_INVALID_ARGUMENT;
 if(margin_type>2||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
static lt48_status gaaf_compare(const unsigned char *actual,const unsigned char *expected,
 uint32_t base,size_t count,int blank,lt48_gaaf_serial_result *out) {
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(size_t i=0;i<count;i++)if(actual[i]!=(blank?255:expected[i])) {
  if(!out->mismatch_count)out->first_mismatch=base+(uint32_t)i;
  out->mismatch_count++;
 }
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}

#include "../../NewDeviceLib/sources/sep7/owner.inc"
#include "../../NewDeviceLib/sources/st2009two/owner.inc"
lt48_status lt48_gaaf_serial_execute(const lt48_gaaf_serial_resources *r,
 const lt48_protocol_transport *api,void *ctx,lt48_operation_kind op,uint32_t start,size_t length,
 const unsigned char *image,size_t image_size,unsigned char *read,size_t read_size,
 lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,
 void *decision_ctx,lt48_gaaf_serial_result *out) {
 if(r&&r->st2009two)return st2009two_execute(r,api,ctx,op,start,length,image,image_size,read,read_size,margin,margin_type,decide,decision_ctx,out);
 if(r&&r->sep7)return sep7_execute(r,api,ctx,op,start,length,image,image_size,read,read_size,margin,margin_type,decide,decision_ctx,out);
 if(r&&r->combined36)return lt48_combined36_execute_internal(r,api,ctx,op,start,length,image,image_size,read,read_size,margin,margin_type,decide,decision_ctx,out);
 if(r && r->tms320e) {
  lt48_status check=lt48_gaaf_serial_validate(r,op,start,length,image,image_size,margin,margin_type);
  if(check!=LT48_OK){if(out){memset(out,0,sizeof(*out));out->operation_status=check;}return check;}
  return lt48_tms320e_secure_internal(r,api,ctx,decide,decision_ctx,out);
 }
 if(r && r->legacy_nor)return lt48_legacy_nor53_execute_internal(r,api,ctx,op,start,length,image,image_size,read,read_size,margin,margin_type,decide,decision_ctx,out);
 if(r && r->legacy28f)return lt48_legacy28f_execute_internal(r,api,ctx,op,start,length,image,image_size,read,read_size,margin,margin_type,decide,decision_ctx,out);
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
 lt48_status status=lt48_gaaf_serial_validate(r,op,start,length,image,image_size,margin,margin_type);
 if(status==LT48_OK&&(!api||!api->read||!api->write))status=LT48_INVALID_ARGUMENT;
 if(status==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))status=LT48_BUFFER_TOO_SMALL;
 if(status!=LT48_OK){out->operation_status=status;return status;}
 int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE;
 unsigned char *pending=malloc(r->capacity),*expected=writing?malloc(r->capacity):NULL;
 if(!pending||(writing&&!expected)){free(pending);free(expected);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 lt48_gaaf_serial_frame frame;lt48_gaaf_serial_frame_init(r,&frame);
 lt48_gaaf_configuration_guard configuration_guard={0};
 lt48_runtime_transport_session *wire=NULL;
 status=lt48_runtime_transport_session_create(api,ctx,&wire);
 if(status==LT48_OK)status=lt48_gaaf_serial_setup(r,&frame,wire);
 if(status==LT48_OK&&r->waves[2])for(unsigned attempt=0;attempt<64;attempt++) {
  uint16_t man=0,dev=0;status=lt48_gaaf_serial_read_id(r,&frame,wire,&man,&dev);if(status!=LT48_OK)break;
  int match=(r->manufacturer||r->device)?man==r->manufacturer&&dev==r->device:man==dev&&(man==0||man==255);
  if(match)break;
  lt48_runtime_transport_session_close(wire);wire=NULL;
  status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,LT48_DEVICE_MISMATCH);if(status!=LT48_OK)break;
  lt48_m37_id_choice choice=decide?decide(decision_ctx,r->manufacturer,r->device,man,dev):LT48_M37_ID_ABORT;
  if(choice!=LT48_M37_ID_RETRY&&choice!=LT48_M37_ID_IGNORE){status=LT48_DEVICE_MISMATCH;break;}
  if(attempt==63&&choice==LT48_M37_ID_RETRY){status=LT48_SAFETY_REJECTED;break;}
  lt48_gaaf_serial_frame_init(r,&frame);
  status=lt48_runtime_transport_session_create(api,ctx,&wire);
  if(status==LT48_OK)status=lt48_gaaf_serial_setup(r,&frame,wire);
  if(status!=LT48_OK||choice==LT48_M37_ID_IGNORE)break;
 }
 if(status==LT48_OK&&(op==LT48_OPERATION_CONFIGURATION||op==LT48_OPERATION_SECURE)) {
  uint16_t value=op==LT48_OPERATION_CONFIGURATION?(uint16_t)(image[0]|(r->configuration_bits==9?(unsigned)image[1]<<8:0)):0;
  if(op==LT48_OPERATION_SECURE) {
   /* Read using the original mode1 INIT, which captures configuration before
    * launching the read applet. Complete its stream before applying changes. */
   status=lt48_gaaf_serial_stream(r,&frame,wire,api,ctx,1,r->nominal,pending,r->capacity);
   if(status==LT48_OK)for(size_t i=0;i<frame.configuration_count;i++)if(!frame.configuration[i].value)value|=1u<<frame.configuration[i].field;
   value|=0xf0;
  }
  if(status==LT48_OK)status=lt48_gaaf_serial_status_write(r,&frame,wire,value);
  goto operation_done;
 }
 if(status==LT48_OK)status=lt48_gaaf_serial_stream(r,&frame,wire,api,ctx,1,r->nominal,pending,r->capacity);
 if(writing&&status==LT48_OK) {
  memcpy(expected,pending,r->capacity);
  if(op==LT48_OPERATION_ERASE)memset(expected+start,255,length);else memcpy(expected+start,image,length);
  if(r->configuration_bits)status=lt48_gaaf_serial_configuration_begin(r,&frame,wire,&configuration_guard);
  if(status==LT48_OK)status=lt48_gaaf_serial_erase(r,&frame,wire);
  if(status==LT48_OK)status=lt48_gaaf_serial_stream(r,&frame,wire,api,ctx,1,r->nominal,pending,r->capacity);
  if(status==LT48_OK)status=gaaf_compare(pending,NULL,0,r->capacity,1,out);
  int nonblank=0;for(size_t i=0;i<r->capacity&&!nonblank;i++)nonblank=expected[i]!=255;
  if(status==LT48_OK&&nonblank)status=lt48_gaaf_serial_stream(r,&frame,wire,api,ctx,2,r->nominal,expected,r->capacity);
 }
 if(status==LT48_OK&&op!=LT48_OPERATION_READ) {
  if(!writing)status=gaaf_compare(pending+start,image,start,length,op==LT48_OPERATION_BLANK_CHECK,out);
  unsigned delta=margin_type==2?0:r->nominal/(margin_type?10:20);
  unsigned volts[2]={r->nominal-delta,r->nominal+delta},count=margin_type==2?1:2;
  /* Exact original44F0C0 x87 result for frozen NexFlash3V profiles.
   * Integer delta truncation misses the upper half-centivolt at310. */
  if(r->nominal==310&&margin_type==0)volts[1]=326;
  if(!writing) {
   if(op!=LT48_OPERATION_VERIFY||margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;count=1;}
   else if(margin==LT48_VERIFY_MARGIN_LOW)count=1;
   else if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];count=1;}
  }
  for(unsigned i=0;status==LT48_OK&&i<count;i++)status=lt48_gaaf_serial_stream(r,&frame,wire,api,ctx,4,volts[i],writing?expected:pending,r->capacity);
  if(writing&&status==LT48_OK) {
   status=lt48_gaaf_serial_stream(r,&frame,wire,api,ctx,1,r->nominal,pending,r->capacity);
   if(status==LT48_OK)status=gaaf_compare(pending,expected,0,r->capacity,0,out);
  }
 }
operation_done:
 out->operation_status=status;lt48_runtime_transport_session_close(wire);
 lt48_status restored=lt48_gaaf_serial_configuration_restore(r,api,ctx,&configuration_guard);
 out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,status==LT48_OK?restored:status);
 if(restored!=LT48_OK)out->cleanup_status=restored;
 out->cleanup_completed=out->cleanup_status==LT48_OK;
 if(status==LT48_OK)status=out->cleanup_status;
 if(status==LT48_OK){out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?(r->configuration_bits==9?2:1):op==LT48_OPERATION_SECURE?0:length;if(op==LT48_OPERATION_READ)memcpy(read,pending+start,length);}
 free(expected);free(pending);return status;
}

uint32_t lt48_gaaf_serial_capacity(const lt48_gaaf_serial_resources *r) { return r?r->capacity:0; }

/* Raw source configuration bits, not a guessed protection mask. Callers own
 * power/INIT and cleanup. Reads reset temporary buffers on every invocation. */
lt48_status lt48_gaaf_serial_status_read(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,uint16_t *raw) {
 if(!r||!frame||!wire||!raw)return LT48_INVALID_ARGUMENT;
 if(!r->configuration_bits||!r->waves[8])return LT48_UNSUPPORTED;
 lt48_status s=lt48_runtime_frame_set_mode(frame->buffers,r->frame_profile.buffer_count,
  r->frame_profile.reset_mask,r->frame_profile.mode_buffer,(uint16_t)frame->mode,0,0);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,8);
 if(s!=LT48_OK)return s;
 if(frame->configuration_count!=r->configuration_bits)return LT48_RUNTIME_ERROR;
 unsigned value=0,seen=0;
 for(size_t i=0;i<frame->configuration_count;i++) {
  const lt48_runtime_configuration_value *c=&frame->configuration[i];
  if(c->object_id||c->category||c->field>=r->configuration_bits||c->value>1||(seen&(1u<<c->field)))return LT48_RUNTIME_ERROR;
  seen|=1u<<c->field;if(!c->value)value|=1u<<c->field;
 }
 if(seen!=(1u<<r->configuration_bits)-1)return LT48_RUNTIME_ERROR;
 *raw=(uint16_t)value;return LT48_OK;
}
lt48_status lt48_gaaf_serial_status_write(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,uint16_t raw) {
 if(!r||!frame||!wire)return LT48_INVALID_ARGUMENT;
 if(!r->configuration_bits||!r->waves[6])return LT48_UNSUPPORTED;
 if(raw>=(1u<<r->configuration_bits))return LT48_INVALID_ARGUMENT;
 /* Original host4503A0 selects mode8, then INIT/PROTECT/RESET.
  * Restore nominal VCC after a preceding margin-verification pass. */
 frame->mode=8;
 for(size_t i=0;i<r->variable_count;i++)if(frame->variables[i].kind==0&&frame->variables[i].variable_id==r->vcc_variable)frame->variables[i].value=(uint16_t)r->nominal;
 lt48_status s=lt48_runtime_frame_set_mode(frame->buffers,r->frame_profile.buffer_count,
  r->frame_profile.reset_mask,r->frame_profile.mode_buffer,8,0,0);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,3);
 uint16_t actual=0;
 if(s==LT48_OK)s=lt48_gaaf_serial_status_read(r,frame,wire,&actual);
 if(s==LT48_OK&&actual!=raw) {
  frame->configuration_count=r->configuration_bits;
  for(unsigned i=0;i<r->configuration_bits;i++)frame->configuration[i]=(lt48_runtime_configuration_value){0,0,i,!(raw&(1u<<i))};
  s=lt48_runtime_frame_set_mode(frame->buffers,r->frame_profile.buffer_count,r->frame_profile.reset_mask,r->frame_profile.mode_buffer,8,0,0);
  if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,6);
 }
 /* Verify before RESET restores the prior rail state. Caller still owns
  * independent recovery if any step fails. */
 if(s==LT48_OK)s=lt48_gaaf_serial_status_read(r,frame,wire,&actual);
 if(s==LT48_OK&&actual!=raw)s=LT48_DEVICE_MISMATCH;
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,r->reset_op);
 return s;
}

/* NexFlash NXSF016F-1201 pp9-11: WR[3:0]=0 means no protected
 * sectors; preserve WD, RCE and HR. W45D source uses the same named
 * WR fields and applet, with bit8 AF passed through unchanged. */
lt48_status lt48_gaaf_serial_configuration_begin(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 lt48_gaaf_configuration_guard *guard) {
 if(!r||!frame||!wire||!guard)return LT48_INVALID_ARGUMENT;
 memset(guard,0,sizeof(*guard));
 if((r->configuration_bits!=8&&r->configuration_bits!=9))return LT48_UNSUPPORTED;
 frame->mode=8;
 for(size_t i=0;i<r->variable_count;i++)if(frame->variables[i].kind==0&&frame->variables[i].variable_id==r->vcc_variable)frame->variables[i].value=(uint16_t)r->nominal;
 lt48_status s=lt48_runtime_frame_set_mode(frame->buffers,r->frame_profile.buffer_count,r->frame_profile.reset_mask,r->frame_profile.mode_buffer,8,0,0);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,3);
 if(s==LT48_OK)s=lt48_gaaf_serial_status_read(r,frame,wire,&guard->original);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,r->reset_op);
 if(s==LT48_OK&&(guard->original&0xf0)) {
  /* Set before attempting the write: a failed transfer may still change it. */
  guard->restore_required=1;
  s=lt48_gaaf_serial_status_write(r,frame,wire,guard->original&~0xf0u);
 }
 return s;
}
/* Fresh transport/firmware frame, independent of a cancelled or poisoned
 * operation queue. Rail shutdown is attempted even if restoration fails. */
lt48_status lt48_gaaf_serial_configuration_restore(const lt48_gaaf_serial_resources *r,
 const lt48_protocol_transport *api,void *ctx,lt48_gaaf_configuration_guard *guard) {
 if(!r||!api||!api->read||!api->write||!guard)return LT48_INVALID_ARGUMENT;
 if(!guard->restore_required)return LT48_OK;
 lt48_protocol_transport recovery=*api;recovery.cancelled=NULL;
 lt48_gaaf_serial_frame frame;lt48_gaaf_serial_frame_init(r,&frame);
 lt48_runtime_transport_session *wire=NULL;
 lt48_status s=lt48_runtime_transport_session_create(&recovery,ctx,&wire);
 const unsigned char stop[]={0,0};unsigned char reply[512]={0};
 if(s==LT48_OK)s=gaaf_command(&recovery,ctx,wire,stop,sizeof(stop),reply);
 if(s==LT48_OK&&!(reply[0]|reply[1]))s=LT48_PROTOCOL_ERROR;
 if(s==LT48_OK)s=lt48_gaaf_serial_setup(r,&frame,wire);
 if(s==LT48_OK)s=lt48_gaaf_serial_status_write(r,&frame,wire,guard->original);
 lt48_runtime_transport_session_close(wire);
 if(s==LT48_OK)guard->restore_required=0;
 lt48_status cleanup=lt48_gaaf_serial_cleanup(r,&frame,&recovery,ctx,s);
 return s==LT48_OK?cleanup:s;
}

unsigned lt48_gaaf_serial_configuration_bits(const lt48_gaaf_serial_resources *r) { return r?r->configuration_bits:0; }

/* Original 004503a0: mode 8, initialize, protection wave, reset.
 * Real sampled protection status is evaluated by the runtime machine. */
lt48_status lt48_tms320e_secure_internal(const lt48_gaaf_serial_resources *r,
 const lt48_protocol_transport *api,void *ctx,lt48_m37_id_decision_fn decide,
 void *decision_ctx,lt48_gaaf_serial_result *out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
 if(!r||!r->tms320e||!api||!api->write||!api->read){out->operation_status=LT48_INVALID_ARGUMENT;return LT48_INVALID_ARGUMENT;}
 lt48_gaaf_serial_frame frame;lt48_gaaf_serial_frame_init(r,&frame);frame.mode=8;
 lt48_runtime_transport_session *wire=NULL;
 lt48_status status=lt48_runtime_transport_session_create(api,ctx,&wire);
 if(status==LT48_OK)status=lt48_gaaf_serial_setup(r,&frame,wire);
 if(status==LT48_OK&&r->waves[2])for(unsigned attempt=0;attempt<64;attempt++) {
  uint16_t man=0,dev=0;
  status=lt48_gaaf_serial_read_id(r,&frame,wire,&man,&dev);if(status!=LT48_OK)break;
  if((r->manufacturer||r->device)?man==r->manufacturer&&dev==r->device:man==dev&&(man==0||man==255))break;
  lt48_runtime_transport_session_close(wire);wire=NULL;
  status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,LT48_DEVICE_MISMATCH);if(status!=LT48_OK)break;
  lt48_m37_id_choice choice=decide?decide(decision_ctx,r->manufacturer,r->device,man,dev):LT48_M37_ID_ABORT;
  if((choice!=LT48_M37_ID_RETRY&&choice!=LT48_M37_ID_IGNORE)||attempt==63){status=LT48_DEVICE_MISMATCH;break;}
  lt48_gaaf_serial_frame_init(r,&frame);frame.mode=8;
  status=lt48_runtime_transport_session_create(api,ctx,&wire);
  if(status==LT48_OK)status=lt48_gaaf_serial_setup(r,&frame,wire);
  if(status!=LT48_OK||choice==LT48_M37_ID_IGNORE)break;
 }
 frame.mode=8;
 if(status==LT48_OK)status=lt48_runtime_frame_set_mode(frame.buffers,r->frame_profile.buffer_count,
  r->frame_profile.reset_mask,r->frame_profile.mode_buffer,8,0,0);
 const unsigned char busy[]={0x40,1};
 if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,busy,sizeof busy);
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,&frame,wire,3);
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,&frame,wire,5);
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,&frame,wire,4);
 if(status==LT48_OK)status=lt48_runtime_transport_session_status(wire,1);
 lt48_runtime_transport_session_close(wire);
 out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,status);
 out->cleanup_completed=out->cleanup_status==LT48_OK;
 if(status==LT48_OK)status=out->cleanup_status;
 out->operation_status=status;return status;
}

/* Exact authored Atmel timeout extension; original healthy path preserved. */
#include "../../NewDeviceLib/sources/legacy-nor/atmel-word-timeout/runtime.h"
lt48_status lt48_legacy_atmel20_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile candidates[]={
#include "legacy_atmel20_profiles.inc"
 };
 lt48_status s=gaaf_open(x,device,out,candidates,sizeof(candidates)/sizeof(*candidates),2003);
 if(s!=LT48_OK)return s;
 switch((*out)->definition) {
#define return_mask(value) ((*out)->legacy_config_mask=(unsigned)(value))
 case 0xf5: return_mask(0x1); break;
 case 0xf6: return_mask(0x1); break;
 case 0xf7: return_mask(0x1); break;
 case 0xf8: return_mask(0x1); break;
 case 0xf9: return_mask(0x1); break;
 case 0xfa: return_mask(0x1); break;
 case 0xfb: return_mask(0x1); break;
 case 0xfc: return_mask(0x1); break;
 case 0xfd: return_mask(0x1); break;
 case 0xfe: return_mask(0x1); break;
 case 0xff: return_mask(0x1); break;
 case 0x100: return_mask(0x1); break;
 case 0x101: return_mask(0x1); break;
 case 0x102: return_mask(0x1); break;
 case 0x103: return_mask(0x1); break;
 case 0x104: return_mask(0x1); break;
 case 0x105: return_mask(0x1); break;
 case 0x106: return_mask(0x1); break;
 case 0x107: return_mask(0x1); break;
#undef return_mask
 default:lt48_gaaf_serial_close(*out);*out=NULL;return LT48_SAFETY_REJECTED;
 }
 s=atmel_word_timeout_apply(x,*out);
 if(s!=LT48_OK){lt48_gaaf_serial_close(*out);*out=NULL;}
 return s;
}

/* Original classic Atmel CODE dependency, assembled from imported payloads. */
lt48_status lt48_legacy_atmel_classic12_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile candidates[]={
#include "legacy_atmel_classic12_profiles.inc"
 };
 lt48_status s=gaaf_open(x,device,out,candidates,sizeof(candidates)/sizeof(*candidates),2003);
 if(s!=LT48_OK)return s;
 lt48_gaaf_serial_resources *r=*out;
 const lt48_wave_code *supplier=&r->applets[16];
 static const unsigned operations[]={17,15,14};
 static const unsigned starts[]={0x5100,0x51b4,0x5211};
 static const unsigned sizes[]={180,93,89};
 if(supplier->load_address!=0x526a||supplier->data_size!=173) {s=LT48_SAFETY_REJECTED;goto fail;}
 size_t extra=0;
 for(unsigned i=0;i<3;i++) {
  const lt48_wave_code *a=&r->applets[operations[i]];
  if(a->load_address!=starts[i]||a->data_size!=sizes[i]){s=LT48_SAFETY_REJECTED;goto fail;}
  extra+=0x5317-starts[i];
 }
 /* Only the selected applet runs after each upload. Retain its original
  * bytes and original erase supplier at their exact CODE addresses. This
  * includes the shared 5300 failure handler without patching any opcode.
  * Gaps are padding, never executable according to the dependency audit. */
 unsigned char *owned=realloc(r->idle,r->idle_size+extra);
 if(!owned){s=LT48_NO_MEMORY;goto fail;}
 r->idle=owned;unsigned char *next=owned+r->idle_size;
 for(unsigned i=0;i<3;i++) {
  lt48_wave_code *a=&r->applets[operations[i]];size_t n=0x5317-starts[i];
  memset(next,0,n);memcpy(next,a->data,a->data_size);
  memcpy(next+0x526a-starts[i],supplier->data,supplier->data_size);
  a->data=next;a->data_size=n;a->length2=n;a->length1=n+4;next+=n;
 }
 r->legacy_config_mask=0u;
 return LT48_OK;
fail:
 lt48_gaaf_serial_close(r);*out=NULL;return s;
}

/* Original self-contained applets for the smaller Atmel word-NOR group. */
lt48_status lt48_legacy_atmel_small7_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile candidates[]={
#include "legacy_atmel_small7_profiles.inc"
 };
 return gaaf_open(x,device,out,candidates,sizeof(candidates)/sizeof(*candidates),2003);
}

/* Original Hynix word-NOR applets and source sector-state masks. */
static uint64_t hynix8_mask(unsigned definition) {
 switch(definition) {
#include "legacy_hynix8_masks.inc"
 default:return UINT64_MAX;
 }
}
lt48_status lt48_legacy_hynix8_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile candidates[]={
#include "legacy_hynix8_profiles.inc"
 };
 lt48_status s=gaaf_open(x,device,out,candidates,sizeof(candidates)/sizeof(*candidates),2003);
 if(s!=LT48_OK)return s;
 uint64_t mask=hynix8_mask((*out)->definition);
 if(mask>UINT32_MAX){lt48_gaaf_serial_close(*out);*out=NULL;return LT48_SAFETY_REJECTED;}
 (*out)->legacy_config_mask=(unsigned)mask;
 return LT48_OK;
}

/* Private remaining word-NOR profiles with explicit erase dependencies. */
static uint64_t remaining9_mask(unsigned definition) {
 switch(definition) {
#include "legacy_word_remaining9_masks.inc"
 default:return UINT64_MAX;
 }
}
lt48_status lt48_legacy_word_remaining9_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile candidates[]={
#include "legacy_word_remaining9_profiles.inc"
 };
 lt48_status status=gaaf_open(x,device,out,candidates,sizeof(candidates)/sizeof(*candidates),2003);
 if(status!=LT48_OK)return status;
 lt48_gaaf_serial_resources *r=*out;uint64_t mask=remaining9_mask(r->definition);
 if(mask>UINT32_MAX){lt48_gaaf_serial_close(r);*out=NULL;return LT48_UNSUPPORTED;}
 r->legacy_config_mask=(unsigned)mask;
 if(r->definition==0x16||r->definition==0x165||r->definition==0xa3||r->definition==0xa4)r->legacy_erase_applet=16;
 return LT48_OK;
}

/* Private audited-resource preparation. No public name admission yet. */
lt48_status lt48_legacy_intel_f3_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile f3[]={
#include "../../NewDeviceLib/sources/legacy-nor/intel_f3.h"
 };
 return gaaf_open(x,device,out,f3,sizeof(f3)/sizeof(*f3),2004);
}

lt48_status lt48_legacy_macronix6_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile profiles[]={
#include "../../NewDeviceLib/sources/legacy-nor/macronix6.h"
 };
 return gaaf_open(x,device,out,profiles,sizeof(profiles)/sizeof(*profiles),2005);
}

#include "../../NewDeviceLib/sources/legacy-nor/macronix6_page_transfer.h"

lt48_status lt48_legacy_motorola8_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile profiles[]={
#include "../../NewDeviceLib/sources/legacy-nor/motorola8.h"
 };
 return gaaf_open(x,device,out,profiles,sizeof(profiles)/sizeof(*profiles),2005);
}

/* Private source-bound PSOP44 candidates; normal catalogue admission is separate. */
lt48_status lt48_legacy_macronix_psop3_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile profiles[]={
#include "../../NewDeviceLib/sources/legacy-nor/macronix_psop3.h"
 };
 return gaaf_open(x,device,out,profiles,sizeof(profiles)/sizeof(*profiles),2005);
}

/* Source L8100 procedures never unprotect. Protected array failures are
 * detected by independent readback; do not apply the other page family's
 * absent save/unprotect/restore slots. Public admission remains separate. */
lt48_status lt48_legacy_macronix_l8100_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile profiles[]={
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l8100.h"
 };
 lt48_status s=gaaf_open(x,device,out,profiles,sizeof(profiles)/sizeof(*profiles),2005);
 if(s==LT48_OK) {
  (*out)->legacy_config_mask=(*out)->definition==0x37?0u:1u;
  (*out)->legacy_page_leave_protection=1;
 }
 return s;
}

/* Byte-page candidates: source routines leave boot protection unchanged. */
lt48_status lt48_legacy_macronix_l8000_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile profiles[]={
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l8000.h"
 };
 lt48_status s=gaaf_open(x,device,out,profiles,sizeof(profiles)/sizeof(*profiles),2005);
 if(s==LT48_OK){(*out)->legacy_config_mask=1;(*out)->legacy_page_leave_protection=1;}
 return s;
}

/* Larger byte-page candidates; source erase saves/restores boot protection. */
lt48_status lt48_legacy_macronix_l3211_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile profiles[]={
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l3211.h"
 };
 lt48_status s=gaaf_open(x,device,out,profiles,sizeof(profiles)/sizeof(*profiles),2005);
 if(s==LT48_OK){(*out)->legacy_page_erase_protection_only=1;(*out)->page_bytes=(*out)->definition==0x51?128u:256u;}
 return s;
}

/* Private F1601 candidate: exact source applet includes its final repeated byte. */
lt48_status lt48_legacy_macronix_f1601_open_internal(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 static const struct gaaf_profile profiles[]={
#include "../../NewDeviceLib/sources/legacy-nor/macronix_f1601.h"
 };
 lt48_status s=gaaf_open(x,device,out,profiles,sizeof(profiles)/sizeof(*profiles),2005);
 if(s==LT48_OK){(*out)->legacy_page_erase_protection_only=1;(*out)->page_bytes=128;}
 return s;
}

lt48_status lt48_legacy_actrans4_open_private(const lt48_installation *x,size_t device,lt48_gaaf_serial_resources **out) {
 lt48_catalogue_entry e;lt48_status s=lt48_installation_get_device(x,device,&e);
 if(s!=LT48_OK)return s;
 if(e.manufacturer_id!=0x7f)return LT48_UNSUPPORTED;
 return gaaf_open(x,device,out,actrans4_profiles,sizeof(actrans4_profiles)/sizeof(*actrans4_profiles),2004);
}

lt48_status lt48_legacy_w49_open_private(const lt48_installation*x,size_t device,lt48_gaaf_serial_resources**out) {
 lt48_catalogue_entry e;lt48_status s=lt48_installation_get_device(x,device,&e);if(s!=LT48_OK)return s;
 if(e.device_class==2029)return gaaf_open(x,device,out,w49_dummy_profiles,sizeof(w49_dummy_profiles)/sizeof(*w49_dummy_profiles),2029);
 return gaaf_open(x,device,out,w49_t16_profiles,sizeof(w49_t16_profiles)/sizeof(*w49_t16_profiles),2004);
}

unsigned lt48_gaaf_serial_is_combined36(const lt48_gaaf_serial_resources*r){return r&&r->combined36;}
unsigned lt48_gaaf_serial_qualified(const lt48_gaaf_serial_resources*r){return r&&(r->st2009two?LT48_ST2009TWO_ADMITTED:r->sst38one?LT48_SST38ONE_ADMITTED:r->w27e4096two?LT48_W27E4096TWO_ADMITTED:r->mxwordtwo?LT48_MXWORDTWO_ADMITTED:r->mx27pageone?LT48_MX27PAGEONE_ADMITTED:r->micronc3two?LT48_MICRONC3TWO_ADMITTED:r->sharpwordpulse3?LT48_SHARPWORDPULSE3_ADMITTED:r->sharpminato4?LT48_SHARPMINATO4_ADMITTED:r->w29wordtwo?LT48_W29WORDTWO_ADMITTED:r->pss2005one?LT48_PSS2005ONE_ADMITTED:r->x88one?LT48_X88ONE_ADMITTED:r->utron4?LT48_UTRON4_ADMITTED:r->sst32four?LT48_SST32FOUR_ADMITTED:r->sst37aliases?LT48_SST37ALIASES_ADMITTED:r->sharp1306?LT48_SHARP1306_ADMITTED:r->sst37four?LT48_SST37FOUR_ADMITTED:r->sharp10?(r->definition<0x70||LT48_SHARP10_STAGE>=2):r->sharp11?(LT48_SHARP11_STAGE>=(r->definition==0x10?1:2)):r->nor3?LT48_NOR3_ADMITTED:r->lnand10?LT48_LNAND10_ADMITTED:r->sep7?LT48_SEP7_ADMITTED:r->word14?word14_admitted(r->definition):(!r->combined36||(r->definition==0x67?LT48_COMBINED36_STAGE>=1:LT48_COMBINED36_STAGE>=2)));}
