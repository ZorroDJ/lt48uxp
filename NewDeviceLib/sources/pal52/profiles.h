/* Authored exact source bindings; original programming resources remain BYO. */
#ifndef LT48_PAL52_PROFILES_H
#define LT48_PAL52_PROFILES_H
#define PAL52_STAGE 2
static inline unsigned pal52_stage(unsigned d,unsigned m,unsigned p){
 static const unsigned rows[][4]={
 {19,4,0,2},
 {22,6,4,2},
 {23,7,5,2},
 {24,7,5,2},
 {25,7,5,2},
 {26,7,5,2},
 {27,8,6,2},
 {28,9,7,2},
 {29,9,7,2},
 {30,9,7,2},
 {38,16,12,2},
 {39,17,13,1},
 {40,17,13,1},
 {41,18,14,2},
 {42,19,14,2},
 {45,22,17,2},
 {46,23,17,2},
 {47,24,17,2},
 {60,36,25,2},
 {61,37,26,2},
 {62,38,27,2},
 {63,39,27,2},
 {64,40,27,2},
 {66,42,29,2},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return rows[i][3];}
 return 0;
}
static inline int pal52_definition(unsigned d,unsigned m,unsigned p){unsigned t=pal52_stage(d,m,p);return t&&t<=PAL52_STAGE;}
static inline int pal52_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2021||strcmp(s->definition_stem,"GBB____")||!pal52_definition(s->definition_resource,s->family_code,s->pinmap))return 0;
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"AMD:AmPAL18P8A/AL/L",66,42,29,20},
 {"AMD:AmPAL18P8B/Q",66,42,29,20},
 {"AMD:AmPAL22V10/A",38,16,12,24},
 {"AMD:PAL22V10-10",38,16,12,24},
 {"AMD:PAL22V10-15",38,16,12,24},
 {"Altera:EP220",46,23,17,20},
 {"Altera:EP310",45,22,17,20},
 {"Altera:EP320",46,23,17,20},
 {"Altera:EP330",47,24,17,20},
 {"Atmel:AT22LV10/L",62,38,27,24},
 {"Atmel:AT22V10/L",62,38,27,24},
 {"Atmel:ATV750/L",63,39,27,24},
 {"Atmel:ATV750B/BL",64,40,27,24},
 {"Atmel:ATV750BQ/BQL",64,40,27,24},
 {"Cypress:PAL22V10C",28,9,7,24},
 {"Cypress:PAL22V10G",28,9,7,24},
 {"Cypress:PAL22VP10C",29,9,7,24},
 {"Cypress:PAL22VP10G",29,9,7,24},
 {"Cypress:PALC16L8",22,6,4,20},
 {"Cypress:PALC16R4",22,6,4,20},
 {"Cypress:PALC16R6",22,6,4,20},
 {"Cypress:PALC16R8",22,6,4,20},
 {"Cypress:PALC22V10",25,7,5,24},
 {"Cypress:PALC22V10B",26,7,5,24},
 {"Cypress:PALC22V10D",27,8,6,24},
 {"Cypress:PALCE22V10",27,8,6,24},
 {"Cypress:PLD20G10C",30,9,7,24},
 {"Cypress:PLDC20G10",23,7,5,24},
 {"Cypress:PLDC20G10A",24,7,5,24},
 {"Intel:5C031",45,22,17,20},
 {"Intel:5C032",46,23,17,20},
 {"Intel:85C220",46,23,17,20},
 {"MMI:PAL16P8",19,4,0,20},
 {"MMI:PAL16RA8",19,4,0,20},
 {"MMI:PAL16RP4",19,4,0,20},
 {"MMI:PAL16RP6",19,4,0,20},
 {"MMI:PAL16RP8",19,4,0,20},
 {"Philips:ABT22V10",60,36,25,24},
 {"Philips:LVT22V10",61,37,26,24},
 {"Philips:PLQ22V10",60,36,25,24},
 {"Philips:PLV750",63,39,27,24},
 {"TI:CPAL22V10Z(T)",42,19,14,24},
 {"TI:CPAL22V10Z(ZP)",41,18,14,24},
 {"TI:EP330",47,24,17,20},
 {"TI:TIBPAL22V10",39,17,13,24},
 {"TI:TIBPAL22V10-10",39,17,13,24},
 {"TI:TIBPAL22V10-15B",39,17,13,24},
 {"TI:TIBPAL22V10-7",39,17,13,24},
 {"TI:TIBPAL22V10A",39,17,13,24},
 {"TI:TIBPAL22VP10",40,17,13,24},
 {"TI:TIBPAL22VP10-20",40,17,13,24},
 {"TI:TIBPAL22VP10-25",40,17,13,24},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;}
 return 0;
}
#endif
