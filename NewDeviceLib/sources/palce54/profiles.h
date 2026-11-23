/* Authored bindings. Original programming resources remain BYO. */
#ifndef LT48_PALCE54_PROFILES_H
#define LT48_PALCE54_PROFILES_H
#include <labtool48/labtool48.h>
#include <stdio.h>
#include <string.h>
#define PALCE54_STAGE 4
static inline unsigned palce54_stage(unsigned d,unsigned m,unsigned p){
 static const unsigned rows[][4]={
 {3,3,2,4},
 {4,3,3,4},
 {15,11,13,3},
 {16,11,14,3},
 {18,13,16,3},
 {20,15,18,3},
 {21,16,19,3},
 {22,17,19,3},
 {23,18,21,1},
 {24,19,21,1},
 {25,20,21,1},
 {26,21,20,1},
 {27,22,22,1},
 {28,23,23,2},
 {29,23,23,2},
 {30,24,22,1},
 {31,25,22,1},
 {32,26,22,1},
 {33,27,24,2},
 {34,28,24,2},
 {35,29,24,2},
 {36,30,24,2},
 {37,31,24,2},
 {38,32,24,2},
 {39,33,24,2},
 {40,34,4,3},
 {41,35,25,3},
 };for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return rows[i][3];return 0;
}
static inline int palce54_definition(unsigned d,unsigned m,unsigned p){unsigned t=palce54_stage(d,m,p);return t&&t<=PALCE54_STAGE;}
static inline int palce54_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2023||strcmp(s->definition_stem,"GBC____")||!palce54_definition(s->definition_resource,s->family_code,s->pinmap))return 0;
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"AMD:MACH 110 *44",16,11,14,44},
 {"AMD:MACH 110 @44",15,11,13,28},
 {"AMD:PALCE 20RA10/25/4",41,35,25,24},
 {"AMD:PALCE 20RA10H",41,35,25,24},
 {"AMD:PALCE 20RA10Q-15",41,35,25,24},
 {"AMD:PALCE 24V10",20,15,18,28},
 {"AMD:PALCE 26V12H",21,16,19,28},
 {"AMD:PALCE 26V12H/4",22,17,19,28},
 {"AMD:PALCE 29MA16/4",18,13,16,24},
 {"AMD:PALLV 22V10-10",40,34,4,24},
 {"AMD:PALLV 22V10-15",40,34,4,24},
 {"AMD:PALLV 22V10-7",40,34,4,24},
 {"AMD:PALLV 22V10Z-25",40,34,4,24},
 {"Atmel:ATF1500/L *44",28,23,23,44},
 {"Atmel:ATF1500A *44",29,23,23,44},
 {"Atmel:ATF16LV8C",24,19,21,20},
 {"Atmel:ATF16V8B/L",23,18,21,20},
 {"Atmel:ATF16V8BQ/BQL",23,18,21,20},
 {"Atmel:ATF16V8C",24,19,21,20},
 {"Atmel:ATF16V8CZ",25,20,21,20},
 {"Atmel:ATF20V8B/L",26,21,20,24},
 {"Atmel:ATF20V8BQ/BQL",26,21,20,24},
 {"Atmel:ATF22LV10C",30,24,22,24},
 {"Atmel:ATF22LV10C(UES)",31,25,22,24},
 {"Atmel:ATF22LV10CEXT",32,26,22,24},
 {"Atmel:ATF22LV10CZ/CQZ",30,24,22,24},
 {"Atmel:ATF22LV10CZ/CQZ(UES)",31,25,22,24},
 {"Atmel:ATF22V10B/L",27,22,22,24},
 {"Atmel:ATF22V10BQ/BQL",27,22,22,24},
 {"Atmel:ATF22V10C/CQ",30,24,22,24},
 {"Atmel:ATF22V10C/CQ(UES)",31,25,22,24},
 {"Atmel:ATF22V10CEXT",32,26,22,24},
 {"Atmel:ATF22V10CZ/CQZ",30,24,22,24},
 {"Atmel:ATF22V10CZ/CQZ(UES)",31,25,22,24},
 {"Atmel:ATF750C",35,29,24,24},
 {"Atmel:ATF750C as V750",33,27,24,24},
 {"Atmel:ATF750C as V750B",34,28,24,24},
 {"Atmel:ATF750CEXT",36,30,24,24},
 {"Atmel:ATF750CL",39,33,24,24},
 {"Atmel:ATF750CL as V750",37,31,24,24},
 {"Atmel:ATF750CL as V750B",38,32,24,24},
 {"Cypress:PALCE 16V8",3,3,2,20},
 {"Cypress:PALCE 20V8",4,3,3,24},
 {"Lattice:PALCE 20RA10/25/4",41,35,25,24},
 {"Lattice:PALCE 20RA10H",41,35,25,24},
 {"Lattice:PALCE 20RA10Q-15",41,35,25,24},
 {"Lattice:PALCE 24V10",20,15,18,28},
 {"Lattice:PALCE 26V12H",21,16,19,28},
 {"Lattice:PALCE 26V12H/4",22,17,19,28},
 {"Lattice:PALCE 29MA16/4",18,13,16,24},
 {"Lattice:PALLV 22V10-10",40,34,4,24},
 {"Lattice:PALLV 22V10-15",40,34,4,24},
 {"Lattice:PALLV 22V10-7",40,34,4,24},
 {"Lattice:PALLV 22V10Z-25",40,34,4,24},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;}
 return 0;
}
#endif
