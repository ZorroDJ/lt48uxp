/* Authored exact remaining PEEL/PZ/XCR compact source profiles. */
#ifndef LT48_PZ20_PROFILES_H
#define LT48_PZ20_PROFILES_H
static inline int pz20_definition(unsigned d,unsigned m,unsigned p){static const unsigned rows[][3]={
 {0,0,0},
 {1,1,1},
 {21,16,12},
 {22,17,13},
 {23,18,12},
 {24,19,13},
 {25,20,14},
 {26,21,14},
 {27,22,15},
 {28,23,16},
 {29,24,15},
 {30,25,15},
 {31,26,16},
 {32,27,17},
 {33,28,18},
 {34,29,18},
 {41,34,20},
 {42,35,20},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return 1;return 0;}
static inline int pz20_binding(const lt48_device_spec*s){if(s->catalogue.device_class!=2019||strcmp(s->definition_stem,"GB_____")||!pz20_definition(s->definition_resource,s->family_code,s->pinmap))return 0;
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"AMI (Gould):PEEL18CV8",0,0,0,20},
 {"Hyundai(Hynix):HY18CV8",0,0,0,20},
 {"ICT:PEEL18CV8",32,27,17,20},
 {"ICT:PEEL18CV8ZP",34,29,18,20},
 {"ICT:PEEL18LV8ZP",33,28,18,20},
 {"ICT:PEEL22CV8",1,1,1,24},
 {"Philips:P3Z22V10",26,21,14,24},
 {"Philips:P5Z22V10",25,20,14,24},
 {"Philips:PZ3032 *44",21,16,12,44},
 {"Philips:PZ3032C/N *44",22,17,13,44},
 {"Philips:PZ3064 *44",30,25,15,44},
 {"Philips:PZ3064 *84",31,26,16,48},
 {"Philips:PZ5032 *44",23,18,12,44},
 {"Philips:PZ5032C/N *44",24,19,13,44},
 {"Philips:PZ5064 *44",27,22,15,44},
 {"Philips:PZ5064 *84",28,23,16,48},
 {"Philips:PZ5064C/N *44",29,24,15,44},
 {"Xilinx:XCR3032A *44VQ",22,17,13,44},
 {"Xilinx:XCR3032XL *44",41,34,20,44},
 {"Xilinx:XCR3064XL *44",42,35,20,44},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;return 0;}
#endif
