/* Authored manufacturer QFN44 maps; exact source bindings only. */
#ifndef LT48_PIC_ENHANCED_QFN44_H
#define LT48_PIC_ENHANCED_QFN44_H
#include "profile.h"
static inline lt48_status lt48_pic_enhanced_qfn44_select(const lt48_pic_profile *p,lt48_pin_map **pins){
 if(!p||!pins||!*pins)return LT48_INVALID_ARGUMENT;
 unsigned selected=0;
 if(p->definition==489&&p->family==321&&p->pinmap==137)selected=1;
 if(p->definition==492&&p->family==322&&p->pinmap==137)selected=1;
 if(p->definition==509&&p->family==332&&p->pinmap==137)selected=1;
 if(p->definition==512&&p->family==333&&p->pinmap==137)selected=1;
 if(p->definition==174&&p->family==135&&p->pinmap==52)selected=2;
 if(p->definition==177&&p->family==136&&p->pinmap==52)selected=2;
 if(p->definition==180&&p->family==137&&p->pinmap==52)selected=2;
 if(p->definition==187&&p->family==142&&p->pinmap==52)selected=2;
 if(p->definition==190&&p->family==143&&p->pinmap==52)selected=2;
 if(p->definition==193&&p->family==144&&p->pinmap==52)selected=2;
 if(p->definition==298&&p->family==212&&p->pinmap==71)selected=1;
 if(p->definition==301&&p->family==213&&p->pinmap==71)selected=1;
 if(p->definition==304&&p->family==214&&p->pinmap==71)selected=1;
 if(p->definition==311&&p->family==216&&p->pinmap==71)selected=1;
 if(p->definition==314&&p->family==217&&p->pinmap==71)selected=1;
 if(p->definition==317&&p->family==218&&p->pinmap==71)selected=1;
 if(!selected)return LT48_OK;
 if(lt48_pin_map_package_pin_count(*pins)!=44)return LT48_SAFETY_REJECTED;
 static const unsigned char map1[]={3,4,0,2,0,2,0,6,27,1,0,2,0,5,29,3,0,40,0,0,1,2,3,4,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,28,30,31,32,33,34,35,36,37,38,39,40,41,42,43,0,0,6,0,23,0,15,0,24,0,16,0,14,0,17,0};
 static const unsigned char map2[]={3,4,0,2,0,3,0,6,7,27,1,0,2,0,5,29,3,0,39,0,0,1,2,3,4,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,28,30,31,32,33,34,35,36,37,38,39,40,41,42,43,0,0,6,0,23,0,15,0,24,0,16,0,14,0,17,0};
 const lt48_pin_code code={44,selected==1?map1:map2,selected==1?sizeof map1:sizeof map2};
 lt48_pin_map *next=NULL;lt48_status st=lt48_pin_map_parse(&code,&next);
 if(st==LT48_OK){lt48_pin_map_close(*pins);*pins=next;}return st;
}
#endif
