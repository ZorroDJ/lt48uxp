/* Authored exact source bindings; original assets remain BYO. */
#ifndef LT48_EJED9_PROFILES_H
#define LT48_EJED9_PROFILES_H
#include <labtool48/labtool48.h>
#include <string.h>
#include <stdio.h>
#define EJED9_STAGE 3
static inline unsigned ejed9_stage(unsigned d,unsigned m,unsigned p){
 static const unsigned rows[][4]={
 {42,38,15,3},
 {43,39,15,3},
 {0,0,1,1},
 {0,0,1,1},
 {1,1,2,2},
 {3,3,4,1},
 {3,3,4,1},
 {4,4,5,2},
 {2,2,3,1},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return rows[i][3];return 0;
}
static inline int ejed9_definition(unsigned d,unsigned m,unsigned p){unsigned t=ejed9_stage(d,m,p);return t&&t<=EJED9_STAGE;}
static inline int ejed9_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2024||strcmp(s->definition_stem,"GBD____"))return 0;
 unsigned stage=ejed9_stage(s->definition_resource,s->family_code,s->pinmap);if(!stage||stage>EJED9_STAGE)return 0;
 static const struct{const char*k;unsigned d,m,p,pins;}rows[]={
 {"AMD:M4-128N/64 @84",42,38,15,24},
 {"AMD:M4LV-128N/64 @84",43,39,15,24},
 {"AMD:MACH 111 *44",0,0,1,44},
 {"AMD:MACH 111 *44TQ",0,0,1,44},
 {"AMD:MACH 131 @84",1,1,2,24},
 {"AMD:MACH 211 *44",3,3,4,44},
 {"AMD:MACH 211 *44TQ",3,3,4,44},
 {"AMD:MACH 221 @68",4,4,5,28},
 {"AMD:MACH 231 @84",2,2,3,24},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(!strcmp(key,rows[i].k)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;
 return 0;
}
#endif
