/* Exact 13.10 source profiles; source assets remain BYO. */
#include "admission.h"
struct page6_profile{const char*key;unsigned def,family,pinmap,pins,cap,man,id,vcc,page,vars,ops,applets,third,defaults;};
static const struct page6_profile page6_profiles[]={
{"Atmel:AT28LV64B",45,37,7,28,8192,0,0,330,64,5,10,3,20736,1},
{"Atmel:AT29C256",0,0,1,28,32768,31,220,500,64,5,11,4,20736,1},
{"Atmel:AT29LV256",1,1,1,28,32768,31,188,330,64,5,10,3,20736,1},
{"Hitachi:HN58C1001",39,31,0,32,131072,0,0,500,128,5,10,3,20736,0},
{"Xicor:X28C010",37,30,4,32,131072,0,0,500,256,4,10,3,21215,0},
{"Xicor:X28C512",38,29,3,32,65536,0,0,500,128,4,10,3,21215,0},
};
static const struct page6_profile*page6_find(const lt48_device_spec*d){
 if(d->catalogue.device_class!=2005||!d->catalogue.vendor)return NULL;
 char key[256];snprintf(key,sizeof key,"%s:%.*s",d->catalogue.vendor,(int)d->catalogue.device_name_length,d->catalogue.device_name);
 for(size_t i=0;i<sizeof page6_profiles/sizeof*page6_profiles;i++){
 const struct page6_profile*p=page6_profiles+i;
 if(!strcmp(key,p->key)&&d->definition_resource==p->def&&d->family_code==p->family&&d->pinmap==p->pinmap&&d->pin_count==p->pins&&d->capacity_bytes==p->cap&&d->element_bits==8&&d->catalogue.manufacturer_id==p->man&&d->catalogue.device_id==p->id)return p;
 }return NULL;
}
static int page6_member(unsigned cls,unsigned d){return cls==2005&&(d==0||d==1||d==0x25||d==0x26||d==0x27||d==0x2d);}
