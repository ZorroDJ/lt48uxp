/* Authored exact catalogue identities; original assets remain BYO. */
#ifndef LT48_AT29_13_PROFILES_H
#define LT48_AT29_13_PROFILES_H
static int at29_binding(const lt48_catalogue_entry*e){
 if(e->device_class!=2041||!e->vendor||strcmp(e->vendor,"Atmel"))return 0;
 static const struct{unsigned d,man,id;const char*name;}rows[]={
 {6,31,53,"AT29BV010A"},
 {7,31,186,"AT29BV020"},
 {8,31,196,"AT29BV040A"},
 {1,31,213,"AT29C010"},
 {1,31,213,"AT29C010A"},
 {2,31,218,"AT29C020"},
 {3,31,91,"AT29C040"},
 {4,31,164,"AT29C040A"},
 {0,31,93,"AT29C512"},
 {6,31,53,"AT29LV010A"},
 {7,31,186,"AT29LV020"},
 {8,31,196,"AT29LV040A"},
 {5,31,61,"AT29LV512"},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&e->device_name_length==strlen(rows[i].name)&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;
 return 0;
}
#endif
