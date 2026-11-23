/* Exact authored profiles; original procedures and FPGA payloads remain BYO. */
struct legacy14_profile {const char *key;unsigned d,m,p,pins,capacity,bits,operations,kind,stage,secure,erase,nominal;};
#include "../otp13five/profiles.h"
static const struct legacy14_profile*legacy14_binding(const lt48_device_spec*s){
 const struct legacy14_profile*extra=otp13five_binding(s);if(extra)return extra;
 if(s->catalogue.device_class!=2013||strcmp(s->definition_stem,"GADA___"))return NULL;
 static const struct legacy14_profile rows[]={
 {"Analog Devices:ADuC824 @52Q",187,110,75,44,8832,8,9,5,3,1,1,500},
 {"Dallas:DS5000/T-32",138,73,41,40,32768,8,9,4,2,1,1,500},
 {"Dallas:DS5000/T-8",139,74,41,40,8192,8,9,4,2,1,1,500},
 {"Intel:8744/H",75,33,17,40,4096,8,8,2,2,1,0,500},
 {"Intel:87C196KB @68",94,42,23,40,16384,16,9,1,1,3,0,500},
 {"Intel:87C196KC @68",95,43,23,40,24576,16,9,1,1,3,0,500},
 {"Intel:87C196KD @68",96,44,23,40,40960,16,9,1,1,3,0,500},
 {"Intel:87C196KQ @68",97,45,23,40,24576,16,9,1,1,3,0,500},
 {"Intel:87C196KR @68",98,45,23,40,24576,16,9,1,1,3,0,500},
 {"Intel:87C196MC @84",93,40,22,40,24576,16,9,1,1,3,0,500},
 {"Intel:87C196MH @84",92,41,22,40,40960,16,9,1,1,3,0,500},
 {"NOVATEK:NT68F62",155,86,52,40,32768,8,7,7,3,0,1,500},
 {"ST:STV0680 @100Q",154,85,51,48,32768,8,7,6,3,0,1,330},
 {"ST:Z86E21",137,75,40,40,8192,8,8,3,2,2,0,500},
 };
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct legacy14_profile*p=rows+i;if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->capacity&&s->element_bits==p->bits&&!s->secondary_element_count)return p;}
 return NULL;
}
