/* Authored exact source bindings; original programming resources remain BYO. */
#ifndef LT48_EP35_PROFILES_H
#define LT48_EP35_PROFILES_H
#include "../ep22four/profiles.h"
#define EP35_STAGE 3
static inline unsigned ep35_stage(unsigned d,unsigned m,unsigned p){
 if(ep22four_definition(d,m,p))return 3;
 static const unsigned rows[][4]={
 {48,25,18,3},
 {49,26,19,3},
 {50,27,18,3},
 {51,28,18,3},
 {52,29,18,3},
 {55,31,21,1},
 {56,32,22,1},
 {57,33,24,3},
 {58,34,21,3},
 {59,35,23,3},
 {73,43,30,3},
 {74,44,30,2},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return rows[i][3];}
 return 0;
}
static inline int ep35_definition(unsigned d,unsigned m,unsigned p){unsigned t=ep35_stage(d,m,p);return t&&t<=EP35_STAGE;}
static inline int ep35_binding(const lt48_device_spec*s){
 if(ep22four_binding(s))return 1;
 if(s->catalogue.device_class!=2021||strcmp(s->definition_stem,"GBB____")||!ep35_definition(s->definition_resource,s->family_code,s->pinmap))return 0;
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"Altera:EP1210",59,35,23,40},
 {"Altera:EP1800 @68",74,44,30,40},
 {"Altera:EP1810 @68",74,44,30,40},
 {"Altera:EP1830 @68",73,43,30,40},
 {"Altera:EP600",48,25,18,24},
 {"Altera:EP610",48,25,18,24},
 {"Altera:EP610 *28PLCC",49,26,19,28},
 {"Altera:EP630",52,29,18,24},
 {"Altera:EP900",55,31,21,40},
 {"Altera:EP900 *44",56,32,22,44},
 {"Altera:EP910",55,31,21,40},
 {"Altera:EP910 *44",56,32,22,44},
 {"Altera:EP910-T",55,31,21,40},
 {"Altera:EP910-T *44",56,32,22,44},
 {"Intel:5C060",50,27,18,24},
 {"Intel:5C090",55,31,21,40},
 {"Intel:5C090 *44",56,32,22,44},
 {"Intel:5C121",59,35,23,40},
 {"Intel:5C180 @68",74,44,30,40},
 {"Intel:85C060",51,28,18,24},
 {"Intel:85C090",58,34,21,40},
 {"Intel:85C090 *44",57,33,24,44},
 {"Intel:iPLD610",51,28,18,24},
 {"Intel:iPLD910",58,34,21,40},
 {"Intel:iPLD910 *44",57,33,24,44},
 {"TI:EP1800 @68",74,44,30,40},
 {"TI:EP1810 @68",74,44,30,40},
 {"TI:EP1830 @68",73,43,30,40},
 {"TI:EP610",50,27,18,24},
 {"TI:EP630",52,29,18,24},
 {"TI:EP900",55,31,21,40},
 {"TI:EP910",55,31,21,40},
 {"TI:EP910 *44",56,32,22,44},
 {"TI:EP910-T",55,31,21,40},
 {"TI:EP910-T *44",56,32,22,44},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;}
 return 0;
}
#endif
