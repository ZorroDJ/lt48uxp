/* Authored exact PAL29 catalogue bindings; original assets remain BYO. */
#ifndef LT48_PAL29_PROFILES_H
#define LT48_PAL29_PROFILES_H
static inline int pal29_definition(unsigned d,unsigned m,unsigned p){
 static const unsigned rows[][3]={
 {9,2,0},
 {14,2,1},
 {32,11,9},
 {33,11,9},
 {34,13,9},
 {36,14,10},
 {44,21,16},
 {65,41,28},
 {67,41,28},
 {68,41,28},
 {69,41,28},
 {70,41,28},
 {71,41,28},
 {72,41,28},
 };for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return 1;return 0;
}
static inline int pal29_binding(const lt48_device_spec *s){
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"AMD:AmPAL20L10B/-20/A",70,41,28,24},
 {"AMD:AmPAL20R4A",67,41,28,24},
 {"AMD:AmPAL20R4B/6B/8B",67,41,28,24},
 {"AMD:AmPAL20RP10B/A/AL",72,41,28,24},
 {"AMD:AmPAL20RP4B/A/AL",68,41,28,24},
 {"AMD:AmPAL20RP6B/A/AL",69,41,28,24},
 {"AMD:AmPAL20RP8B/A/AL",71,41,28,24},
 {"AMD:AmPAL20XRP10",72,41,28,24},
 {"AMD:AmPAL20XRP4",68,41,28,24},
 {"AMD:AmPAL20XRP6",69,41,28,24},
 {"AMD:AmPAL20XRP8",71,41,28,24},
 {"AMD:AmPAL22P10/B/A/AL",65,41,28,24},
 {"AMD:AmPAL22XP10",65,41,28,24},
 {"MMI:PAL16C1/-2",9,2,0,20},
 {"MMI:PAL20C1",14,2,1,24},
 {"TI:PAL20L8A",36,14,10,24},
 {"TI:PAL20R4A",36,14,10,24},
 {"TI:PAL20R6A",36,14,10,24},
 {"TI:PAL20R8A",36,14,10,24},
 {"TI:TIBPAD16N8-7",33,11,9,20},
 {"TI:TIBPAD18N8-6",34,13,9,20},
 {"TI:TIBPAL16L8-10",32,11,9,20},
 {"TI:TIBPAL16R4-10",32,11,9,20},
 {"TI:TIBPAL16R6-10",32,11,9,20},
 {"TI:TIBPAL16R8-10",32,11,9,20},
 {"TI:TIBPAL20L10",44,21,16,24},
 {"TI:TIBPAL20X10",44,21,16,24},
 {"TI:TIBPAL20X4",44,21,16,24},
 {"TI:TIBPAL20X8",44,21,16,24},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 if(s->catalogue.device_class!=2021||strcmp(s->definition_stem,"GBB____"))return 0;
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;
 return 0;
}
#endif
