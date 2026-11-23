#ifndef LT48_MR7_MEMBERS_H
#define LT48_MR7_MEMBERS_H
#define LT48_MR7_ADMITTED 1
#define LT48_MR7_BYTE_ADMITTED 1
static int mr7_member(const lt48_catalogue_entry*e){
static const struct{const char*vendor,*name;unsigned cls,def,man,dev;}rows[]={
{"ACTRANS","AC39LV088 *48TS",2030,189,127,33},
{"ELAN","EM39LV088 *48TS",2030,189,127,33},
{"MIRA","P439LV088 *48TS",2030,189,127,33},
{"ESMT","FM91L03208BIA @56BGA",2033,61,137,35013},
{"ESMT","FM91L03208UIA @56BGA",2033,60,137,35012},
{"DouqiTech","DQ25Q64A @24TFBGA",2044,272,84,22},
{"ESMT","F25L64QA @24TFBGA",2044,258,140,16663},
};for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(e->device_class==rows[i].cls&&e->device_type==rows[i].def&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].dev&&!strcmp(e->vendor,rows[i].vendor)&&e->device_name_length==strlen(rows[i].name)&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;return 0;}
#endif
