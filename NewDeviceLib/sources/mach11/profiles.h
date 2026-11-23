/* Authored exact source bindings; original assets remain BYO. */
#ifndef LT48_MACH11_PROFILES_H
#define LT48_MACH11_PROFILES_H
#include <labtool48/labtool48.h>
#include <string.h>
#include <stdio.h>
#define MACH11_STAGE 3
static inline unsigned mach11_stage(unsigned d,unsigned m,unsigned p){
 static const unsigned rows[][4]={
 {9,6,7,1},
 {11,8,9,1},
 {6,4,6,2},
 {5,4,5,2},
 {14,10,12,1},
 {13,10,11,1},
 {10,7,8,1},
 {12,9,10,1},
 {17,12,15,3},
 {8,5,6,2},
 {7,5,5,2},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return rows[i][3];return 0;
}
static inline int mach11_definition(unsigned d,unsigned m,unsigned p){return mach11_stage(d,m,p)!=0;}
static inline int mach11_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2023||strcmp(s->definition_stem,"GBC____"))return 0;
 unsigned stage=mach11_stage(s->definition_resource,s->family_code,s->pinmap);if(!stage||stage>MACH11_STAGE)return 0;
 static const struct{const char*k;unsigned d,m,p,pins;}rows[]={
 {"AMD:MACH 120 @68",9,6,7,28},
 {"AMD:MACH 130 @84",11,8,9,24},
 {"AMD:MACH 210 *44",6,4,6,44},
 {"AMD:MACH 210 @44",5,4,5,28},
 {"AMD:MACH 215 *44",14,10,12,44},
 {"AMD:MACH 215 @44",13,10,11,28},
 {"AMD:MACH 220 @68",10,7,8,28},
 {"AMD:MACH 230 @84",12,9,10,24},
 {"AMD:MACH 435 @84",17,12,15,24},
 {"AMD:MACH LV210 *44",8,5,6,44},
 {"AMD:MACH LV210 @44",7,5,5,28},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(!strcmp(key,rows[i].k)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;
 return 0;
}
#endif
