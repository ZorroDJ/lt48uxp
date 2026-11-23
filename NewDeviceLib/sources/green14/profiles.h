/* Authored exact catalogue admissions; source resources remain BYO. */
#ifndef LT48_GREEN14_BINDINGS_H
#define LT48_GREEN14_BINDINGS_H
static int green14_binding(const lt48_catalogue_entry*e){
 if(!e->vendor||strcmp(e->vendor,"Greenliant"))return 0;
 static const struct{unsigned cls,d,man,id;const char*name;}rows[]={
 {2001,0x43,191,165,"GLS27SF010"},
 {2001,0x43,191,165,"GLS27SF010 *32PLCC"},
 {2001,0x43,191,165,"GLS27SF010 *32TS"},
 {2001,0x44,191,166,"GLS27SF020"},
 {2001,0x44,191,166,"GLS27SF020 *32PLCC"},
 {2001,0x44,191,166,"GLS27SF020 *32TS"},
 {2001,0x42,191,164,"GLS27SF512"},
 {2001,0x42,191,164,"GLS27SF512 *32PLCC"},
 {2041,0xa,191,7,"GLS29EE010"},
 {2041,0xa,191,7,"GLS29EE010 *32PLCC"},
 {2041,0xa,191,7,"GLS29EE010 *32TS"},
 {2041,0x9,191,93,"GLS29EE512"},
 {2041,0x9,191,93,"GLS29EE512 *32PLCC"},
 {2041,0x9,191,93,"GLS29EE512 *32TS"},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(e->device_class==rows[i].cls&&e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&e->device_name_length==strlen(rows[i].name)&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;
 return 0;
}
#endif
