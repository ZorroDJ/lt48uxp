/* Authored exact source bindings; original programming resources remain BYO. */
#ifndef LT48_PEEL46_PROFILES_H
#define LT48_PEEL46_PROFILES_H
#define PEEL46_STAGE 3
static inline unsigned peel46_stage(unsigned d,unsigned m,unsigned p){
 static const unsigned rows[][4]={
 {2,2,2,1},
 {3,3,2,1},
 {4,4,2,2},
 {5,2,2,1},
 {6,5,2,2},
 {7,6,2,2},
 {8,7,3,2},
 {9,8,3,2},
 {10,7,4,2},
 {11,8,4,2},
 {12,9,5,2},
 {13,10,6,2},
 {14,11,7,2},
 {15,12,8,2},
 {16,13,9,3},
 {17,14,10,3},
 {18,13,9,3},
 {19,14,10,3},
 {20,15,11,3},
 {35,30,19,2},
 {36,31,19,2},
 {37,30,19,2},
 {38,31,19,2},
 {39,32,19,2},
 {40,33,19,2},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return rows[i][3];}
 return 0;
}
static inline int peel46_definition(unsigned d,unsigned m,unsigned p){unsigned t=peel46_stage(d,m,p);return t&&t<=PEEL46_STAGE;}
static inline int peel46_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2019||strcmp(s->definition_stem,"GB_____")||!peel46_definition(s->definition_resource,s->family_code,s->pinmap))return 0;
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"AMI (Gould):PEEL153",8,7,3,20},
 {"AMI (Gould):PEEL173",10,7,4,24},
 {"AMI (Gould):PEEL20CG10",2,2,2,24},
 {"AMI (Gould):PEEL20CG10A",3,3,2,24},
 {"AMI (Gould):PEEL22CV10",4,4,2,24},
 {"AMI (Gould):PEEL22CV10A",6,5,2,24},
 {"AMI (Gould):PEEL22CV10A+",7,6,2,24},
 {"AMI (Gould):PEEL22CV10Z",5,2,2,24},
 {"AMI (Gould):PEEL253",9,8,3,20},
 {"AMI (Gould):PEEL273",11,8,4,24},
 {"Atmel:ATV2500B",18,13,9,40},
 {"Atmel:ATV2500B *44",19,14,10,44},
 {"Atmel:ATV2500BQ/BQL",18,13,9,40},
 {"Atmel:ATV2500BQ/BQL *44",19,14,10,44},
 {"Atmel:ATV2500H/L",16,13,9,40},
 {"Atmel:ATV2500H/L *44",17,14,10,44},
 {"Atmel:ATV5000 @68",20,15,11,40},
 {"Atmel:ATV5100 @68",20,15,11,40},
 {"ICT:PA7024",12,9,5,24},
 {"ICT:PA7128",13,10,6,28},
 {"ICT:PA7140",14,11,7,40},
 {"ICT:PA7140 *44",15,12,8,44},
 {"ICT:PA7572",14,11,7,40},
 {"ICT:PA7572 *44",15,12,8,44},
 {"ICT:PEEL153",8,7,3,20},
 {"ICT:PEEL173",10,7,4,24},
 {"ICT:PEEL20CG10",2,2,2,24},
 {"ICT:PEEL20CG10A",3,3,2,24},
 {"ICT:PEEL22CV10",4,4,2,24},
 {"ICT:PEEL22CV10A",35,30,19,24},
 {"ICT:PEEL22CV10A+",37,30,19,24},
 {"ICT:PEEL22CV10A++",39,32,19,24},
 {"ICT:PEEL22CV10AZ",35,30,19,24},
 {"ICT:PEEL22CV10AZ+",37,30,19,24},
 {"ICT:PEEL22CV10AZ++",39,32,19,24},
 {"ICT:PEEL22CV10Z",5,2,2,24},
 {"ICT:PEEL22LV10AZ",36,31,19,24},
 {"ICT:PEEL22LV10AZ+",38,31,19,24},
 {"ICT:PEEL22LV10AZ++",40,33,19,24},
 {"ICT:PEEL253",9,8,3,20},
 {"ICT:PEEL273",11,8,4,24},
 {"Philips:PL 22V10",6,5,2,24},
 {"Philips:PLV 2500",16,13,9,40},
 {"Philips:PLV2500 *44",17,14,10,44},
 {"Philips:PLV5000 @68",20,15,11,40},
 {"Philips:PLV5100 @68",20,15,11,40},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;}
 return 0;
}
#endif
