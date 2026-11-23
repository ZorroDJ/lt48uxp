/* Authored exact source bindings; original programming resources remain BYO. */
#ifndef LT48_COMPACT20_PROFILES_H
#define LT48_COMPACT20_PROFILES_H
#define COMPACT20_STAGE 4
static inline unsigned compact20_stage(unsigned d,unsigned m,unsigned p){
 static const unsigned rows[][4]={
 {0,0,0,2},
 {1,0,0,2},
 {2,0,0,2},
 {3,1,0,2},
 {7,4,10,1},
 {8,4,4,1},
 {31,25,14,3},
 {32,25,15,3},
 {33,26,16,3},
 {34,27,17,4},
 {35,28,18,4},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return rows[i][3];}
 return 0;
}
static inline int compact20_definition(unsigned d,unsigned m,unsigned p){unsigned t=compact20_stage(d,m,p);return t&&t<=COMPACT20_STAGE;}
static inline int compact20_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2022||strcmp(s->definition_stem,"GBBA___")||!compact20_definition(s->definition_resource,s->family_code,s->pinmap))return 0;
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"Altera:EP312",33,26,16,24},
 {"Altera:EP324",31,25,14,40},
 {"Altera:EP324 *44",32,25,15,44},
 {"Cypress:CY2291",34,27,17,20},
 {"Cypress:CY2292F *16SO",35,28,18,16},
 {"Cypress:CY7C330",0,0,0,28},
 {"Cypress:CY7C331",1,0,0,28},
 {"Cypress:CY7C332",2,0,0,28},
 {"Cypress:CY7C335",3,1,0,28},
 {"Intel:5AC312",33,26,16,24},
 {"Intel:5AC324",31,25,14,40},
 {"Intel:5AC324 *44",32,25,15,44},
 {"Philips:PLUS 16L8",7,4,10,20},
 {"Philips:PLUS 16R4",7,4,10,20},
 {"Philips:PLUS 16R6",7,4,10,20},
 {"Philips:PLUS 16R8",7,4,10,20},
 {"Philips:PLUS 20L8",8,4,4,24},
 {"Philips:PLUS 20R4",8,4,4,24},
 {"Philips:PLUS 20R6",8,4,4,24},
 {"Philips:PLUS 20R8",8,4,4,24},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;}
 return 0;
}
#endif
