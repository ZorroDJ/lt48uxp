/* Authored FFPI route: SAM7S64/128/256 expose DATA[15:0] (6175M,20.2).
 * Authenticate the selected byte definition, then require the exact physical
 * interface bijection before substituting the qualified word operation owner.
 * Keep the selected B core voltage; no byte bank or upper-mask is invented.
 */
#include "admission.h"
static int a91_four_group(const lt48_pin_map*a,unsigned ak,const lt48_pin_map*b,unsigned bk){
 lt48_pin_group x,y;return !lt48_pin_map_find_group(a,ak,&x)&&!lt48_pin_map_find_group(b,bk,&y)&&x.pin_count==y.pin_count&&!memcmp(x.pins,y.pins,x.pin_count);
}
static lt48_status a91_four_bridge(const lt48_installation*x,const lt48_s8253_resources*selected,lt48_s8253_resources**out){
 if(!A91FOUR_ADMITTED)return LT48_UNSUPPORTED;
 unsigned d=selected->capacity==65536?194:selected->capacity==131072?195:selected->capacity==262144?196:0;
 if(!d||selected->a91_width!=1||selected->pinmap!=115||selected->vars[1].value!=165)return LT48_SAFETY_REJECTED;
 size_t index=SIZE_MAX;for(size_t i=0;i<lt48_installation_device_count(x);i++){lt48_catalogue_entry e;lt48_status st=lt48_installation_get_device(x,i,&e);if(st)return st;if(e.device_class==2033&&e.device_type==d&&!strcmp(e.vendor,"Atmel")){index=i;break;}}
 if(index==SIZE_MAX)return LT48_UNSUPPORTED;
 lt48_s8253_resources*r=NULL;lt48_status st=lt48_at91nine_open(x,index,&r);if(st)return st;
 if(r->a91_width!=2||r->capacity!=selected->capacity||r->configuration_maximum[1]!=selected->configuration_maximum[1]||r->vars[2].value!=selected->vars[2].value)goto reject;
 for(unsigned k=0;k<4;k++)if(!a91_four_group(r->pins,k,selected->pins,k))goto reject;
 if(!a91_four_group(r->pins,7,selected->pins,6)||lt48_pin_map_group_count(r->pins)!=6||lt48_pin_map_group_count(selected->pins)!=8)goto reject;
 lt48_pin_group bus,low,high,core;lt48_logical_pin lp;
 if(lt48_pin_map_find_group(r->pins,6,&bus)||lt48_pin_map_find_group(selected->pins,7,&low)||lt48_pin_map_find_group(selected->pins,4,&high)||bus.pin_count!=16||low.pin_count!=8||high.pin_count!=8||memcmp(bus.pins,low.pins,8)||memcmp(bus.pins+8,high.pins,8))goto reject;
 if(lt48_pin_map_find_logical(r->pins,4,&lp)||lt48_pin_map_find_group(selected->pins,5,&core)||core.pin_count!=1||core.pins[0]!=lp.engine_pin)goto reject;
 static const unsigned rename[][2]={{24,49},{2,1},{16,16},{23,48},{10,40},{27,35},{26,34},{25,33},{28,46},{13,9}};
 if(lt48_pin_map_logical_count(r->pins)!=11||lt48_pin_map_logical_count(selected->pins)!=10)goto reject;
 for(unsigned k=0;k<sizeof rename/sizeof*rename;k++){lt48_logical_pin a,b;if(lt48_pin_map_find_logical(r->pins,rename[k][0],&a)||lt48_pin_map_find_logical(selected->pins,rename[k][1],&b)||a.engine_pin!=b.engine_pin)goto reject;}
 r->vars[1].value=selected->vars[1].value;r->a91_bridge=1;*out=r;return LT48_OK;
 reject:lt48_s8253_close(r);return LT48_SAFETY_REJECTED;
}
