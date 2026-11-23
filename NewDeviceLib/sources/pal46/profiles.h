/* Authored exact bindings. Original resources remain bring-your-own.
 * Seven corrected profiles use corrections.h after source-digest validation. */
#ifndef LT48_PAL46_PROFILES_H
#define LT48_PAL46_PROFILES_H
static inline int pal46_definition(unsigned d,unsigned m,unsigned p){
 static const unsigned rows[][3]={
 {0,0,0},
 {1,0,0},
 {2,1,0},
 {3,0,0},
 {4,1,0},
 {5,0,0},
 {6,1,0},
 {7,0,0},
 {8,1,0},
 {10,3,0},
 {11,0,1},
 {12,0,1},
 {13,0,1},
 {15,0,1},
 {16,0,1},
 {18,0,1},
 };for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return 1;
 return 0;
}
static inline int pal46_binding(const lt48_device_spec*s){
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"AMD:PAL20L8-10",11,0,1,24},
 {"AMD:PAL20R4-10",11,0,1,24},
 {"AMD:PAL20R6-10",11,0,1,24},
 {"AMD:PAL20R8-10",11,0,1,24},
 {"MMI:PAL10H8/-2",2,1,0,20},
 {"MMI:PAL10L8/-2",1,0,0,20},
 {"MMI:PAL12H6/-2",4,1,0,20},
 {"MMI:PAL12L10",16,0,1,24},
 {"MMI:PAL12L6/-2",3,0,0,20},
 {"MMI:PAL14H4/-2",6,1,0,20},
 {"MMI:PAL14L4/-2",5,0,0,20},
 {"MMI:PAL16A4",0,0,0,20},
 {"MMI:PAL16H2/-2",8,1,0,20},
 {"MMI:PAL16L2/-2",7,0,0,20},
 {"MMI:PAL16L6",18,0,1,24},
 {"MMI:PAL16L8/A/A-2/A-4",0,0,0,20},
 {"MMI:PAL16L8B/B-2/B-4",10,3,0,20},
 {"MMI:PAL16L8D",10,3,0,20},
 {"MMI:PAL16L8H-15",10,3,0,20},
 {"MMI:PAL16R4/A/A-2/A-4",0,0,0,20},
 {"MMI:PAL16R4B/B-2/B-4",10,3,0,20},
 {"MMI:PAL16R4D",10,3,0,20},
 {"MMI:PAL16R4H-15",10,3,0,20},
 {"MMI:PAL16R6/A/A-2/A-4",0,0,0,20},
 {"MMI:PAL16R6B/B-2/B-4",10,3,0,20},
 {"MMI:PAL16R6D",10,3,0,20},
 {"MMI:PAL16R6H-15",10,3,0,20},
 {"MMI:PAL16R8/A/A-2/A-4",0,0,0,20},
 {"MMI:PAL16R8B/B-2/B-4",10,3,0,20},
 {"MMI:PAL16R8D",10,3,0,20},
 {"MMI:PAL16R8H-15",10,3,0,20},
 {"MMI:PAL16X4",0,0,0,20},
 {"MMI:PAL18L4",13,0,1,24},
 {"MMI:PAL20L10/A",12,0,1,24},
 {"MMI:PAL20L2",15,0,1,24},
 {"MMI:PAL20L8A/A-2",11,0,1,24},
 {"MMI:PAL20L8B/B-2",11,0,1,24},
 {"MMI:PAL20R4A/A-2",11,0,1,24},
 {"MMI:PAL20R4B/B-2",11,0,1,24},
 {"MMI:PAL20R6A/A-2",11,0,1,24},
 {"MMI:PAL20R6B/B-2",11,0,1,24},
 {"MMI:PAL20R8A/A-2",11,0,1,24},
 {"MMI:PAL20R8B/B-2",11,0,1,24},
 {"MMI:PAL20X10/A",12,0,1,24},
 {"MMI:PAL20X4/A",12,0,1,24},
 {"MMI:PAL20X8/A",12,0,1,24},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 if(s->catalogue.device_class!=2021||strcmp(s->definition_stem,"GBB____"))return 0;
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;
 return 0;
}
#endif
