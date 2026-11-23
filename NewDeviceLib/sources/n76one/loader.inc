
#include "admission.h"
unsigned lt48_n76one_capacity_internal(unsigned cls,unsigned d,unsigned family,unsigned pinmap){return LT48_N76ONE_ADMITTED&&cls==2039&&d==5&&family==5&&pinmap==5?18432:0;}
int lt48_n76one_admitted_internal(unsigned cls,unsigned d){return LT48_N76ONE_ADMITTED&&cls==2039&&d==5;}
lt48_status lt48_n76one_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_catalogue_entry e;lt48_status early=lt48_installation_get_device(x,index,&e);if(early)return early;if(e.device_class!=2039||e.device_type!=5)return LT48_UNSUPPORTED;
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,index,&s);if(st)return st;
 static const struct nw21_profile profile={5,5,5,18432,11,25399,1,{18432},{0},{0},4,n76one_fields};const struct nw21_profile*p=&profile;
 if(!lt48_n76one_capacity_internal(s.catalogue.device_class,s.definition_resource,s.family_code,s.pinmap)||s.capacity_bytes!=18432||s.pin_count!=20||s.element_bits!=8||strcmp(s.catalogue.vendor,"Nuvoton")||s.catalogue.device_name_length!=strlen("N76E003 *20TSSOP")||memcmp(s.catalogue.device_name,"N76E003 *20TSSOP",strlen("N76E003 *20TSSOP")))return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;
 r->kind=9;r->n76one=1;r->n79_xp=1;r->legacy_w79e201=p->definition==0x99;r->manufacturer=s.catalogue.manufacturer_id;r->device=s.catalogue.device_id;r->definition=p->definition;r->family=p->family;r->pinmap=p->pinmap;r->capacity=p->capacity;r->flash=p->length[0];r->nominal=500;r->fields=p->fields;r->configuration_id=p->configuration;r->regions=p->regions;
 memcpy(r->region_length,p->length,sizeof p->length);memcpy(r->region_bank,p->logical,sizeof p->logical);memcpy(r->n79_physical,p->physical,sizeof p->physical);
 r->configuration_lock=16;for(unsigned i=0;i<p->fields;i++)if(strstr(p->schema[i].label,"EEPROM Protect"))r->n79_locks|=1u<<i;for(unsigned i=0;i<p->fields;i++){if((strstr(p->schema[i].label,"Lock")!=NULL))r->configuration_lock=i;r->configuration_category[i]=p->schema[i].category;r->configuration_field[i]=p->schema[i].field;r->configuration_maximum[i]=p->schema[i].maximum;}
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/%s",lt48_installation_path(x),"GDAEA__.rez");
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"2a9dadc602599e4eacbf75cc59d3020838c2f39c17842ff778a084a377f89ba9",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=p->variables||f.configuration_set!=p->configuration||f.operation_count!=12u){st=LT48_FORMAT_ERROR;goto fail;}
 r->variable_count=f.variable_count;for(unsigned i=0;i<f.variable_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned apps=0,mask=0;
 for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 if(!w.wave_type&&op.operation_id<17&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st){fprintf(stderr,"NuMicro D%x role%u compile status%d\n",p->definition,op.operation_id,st);goto fail;}}
 else if(w.wave_type==6&&apps<4){unsigned a=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0;unsigned bit=a==0x6000?1:a==0x7800?2:a==0x8000?4:8;if(!a||a!=w.load_address||(mask&bit)){st=LT48_FORMAT_ERROR;goto fail;}mask|=bit;r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,a};}
 else if(w.wave_type==4&&op.operation_id==27u&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a&&!r->fpga)r->fpga=w.data+2;
 else{st=LT48_UNSUPPORTED;goto fail;}}
 /* Engineer15 is the source configuration reader. Compile a second owned
  * instance at helper6 for the existing typed-settings machinery. */
 snprintf(name,sizeof name,"W17");lt48_wave_code engineer;lt48_compact_wave*copy=NULL;TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&engineer));TRY(lt48_compact_decode(engineer.data,engineer.data_size,&copy));st=lt48_compact_compile(copy,r->pins,r->vars,r->variable_count,1,0,&r->wave[6]);lt48_compact_wave_close(copy);if(st)goto fail;
 if(mask!=15||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
