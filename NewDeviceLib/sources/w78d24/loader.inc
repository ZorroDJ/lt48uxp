#include "profiles.h"
#include "admission.h"
int lt48_w78d24_admitted_internal(unsigned d){return lt48_w78d24_admitted(d);
 }
lt48_status lt48_w78d24_open(const lt48_installation *x,size_t device,lt48_s8253_resources **out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;

 *out=NULL;lt48_device_spec s;
 lt48_status st=lt48_installation_resolve_device(x,device,&s);

 if(st)return st;

 if(s.catalogue.device_class!=2048 || strcmp(s.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;

 const struct w78d24_profile*p=NULL;
 for(size_t i=0;i<sizeof w78d24_profiles/sizeof *w78d24_profiles;i++)if(w78d24_profiles[i].definition==s.definition_resource)p=w78d24_profiles+i;

 if(!p)return LT48_UNSUPPORTED;

 unsigned found=0;
 for(size_t i=0;i<sizeof w78d24_members/sizeof *w78d24_members;i++)if(w78d24_members[i].definition==s.definition_resource && w78d24_members[i].manufacturer==s.catalogue.manufacturer_id && w78d24_members[i].device==s.catalogue.device_id && !strcmp(w78d24_members[i].vendor,s.catalogue.vendor) && strlen(w78d24_members[i].name)==s.catalogue.device_name_length && !memcmp(w78d24_members[i].name,s.catalogue.device_name,s.catalogue.device_name_length))found=1;

 if(!found || s.family_code!=p->family || s.pinmap!=p->pinmap || s.capacity_bytes!=p->flash+2048)return LT48_SAFETY_REJECTED;

 lt48_s8253_resources*r=calloc(1,sizeof *r);

 if(!r)return LT48_NO_MEMORY;

 r->definition=p->definition;
 r->family=p->family;
 r->pinmap=p->pinmap;
 r->flash=p->flash;
 r->nominal=p->nominal;
 r->kind=3;
 r->fields=p->fields;
 r->capacity=p->flash+2048;
 r->manufacturer=s.catalogue.manufacturer_id;
 r->device=s.catalogue.device_id;

 char path[4096],name[24];
 size_t ri,fi;

 int n=snprintf(path,sizeof path,"%s/GDCC___.rez",lt48_installation_path(x));

 if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;
 goto fail;
 }
#define TRY(v) do { st=(v); if(st)goto fail; } while(0)
 TRY(lt48_owner_rez_open_internal(path,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",&r->rez));

 snprintf(name,sizeof name,"P%x",p->pinmap);
 lt48_pin_code pc;
 TRY(lt48_rez_find_resource(r->rez,name,&ri));
 TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));
 TRY(lt48_pin_map_parse(&pc,&r->pins));

 snprintf(name,sizeof name,"M%x",p->family);
 lt48_family_info f;
 TRY(lt48_rez_find_resource(r->rez,name,&fi));
 TRY(lt48_rez_get_family_info(r->rez,fi,&f));

 if(f.variable_count!=3||f.operation_count!=16){st=LT48_FORMAT_ERROR;
 goto fail;
 }
 for(unsigned i=0;i<4;i++){r->vars[i].kind=2;r->vars[i].variable_id=0xfff0+i;}
 for(unsigned i=0;i<f.variable_count;i++){lt48_family_variable v;
 TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
 r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
 }
 unsigned apps=0,mask=0;

 for(unsigned i=0;i<f.operation_count;i++){
  lt48_family_operation op;
 lt48_wave_code w;
 TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
 snprintf(name,sizeof name,"W%x",op.wave);
 TRY(lt48_rez_find_resource(r->rez,name,&ri));
 TRY(lt48_rez_get_wave_code(r->rez,ri,&w));

  if(!w.wave_type && op.operation_id<12 && !r->wave[op.operation_id]){lt48_compact_wave*c=NULL;
 TRY(lt48_compact_decode(w.data,w.data_size,&c));
 st=lt48_compact_compile(c,r->pins,r->vars,4,1,0,&r->wave[op.operation_id]);
 lt48_compact_wave_close(c);

 if(st){if(getenv("LT48_TRACE"))fprintf(stderr,"compact role %u compile %s\n",op.operation_id,lt48_status_string(st));goto fail;}

 if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;
 goto fail;
 }}
  else if(w.wave_type==6 && apps<4){unsigned a=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0;

 if(!a||w.load_address!=a){st=LT48_FORMAT_ERROR;
 goto fail;
 }unsigned bit=1u<<((a==0x6000)?0:(a==0x7800)?1:(a==0x8000)?2:3);

 if(mask&bit){st=LT48_FORMAT_ERROR;
 goto fail;
 }mask|=bit;
 r->applet[apps]=w;
 r->addresses[apps++]=(lt48_applet_address){op.operation_id,w.load_address};
 }
  else if(w.wave_type==4 && op.operation_id==30 && w.data_size==2780 && w.data[0]==0xda && w.data[1]==0x0a && !r->fpga)r->fpga=w.data+2;

  else {st=LT48_UNSUPPORTED;
 goto fail;
 }
 }
 for(unsigned i=0;i<12;i++)if(i!=4&&!r->wave[i]){st=LT48_FORMAT_ERROR;
 goto fail;
 }
 if(mask!=15 || !r->fpga){st=LT48_FORMAT_ERROR;
 goto fail;
 }
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));

 *out=r;
 return LT48_OK;

fail:if(getenv("LT48_TRACE"))fprintf(stderr,"SST D%x failed %s\n",p->definition,lt48_status_string(st));lt48_s8253_close(r);

 return st;

#undef TRY
}
lt48_status lt48_w78d24_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size){
 if(!r||r->kind!=3||!n||start>=r->capacity||n>r->capacity-start)return LT48_SAFETY_REJECTED;
 if((op<1||op>8)&&op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE||op==LT48_OPERATION_SECURE||op==LT48_OPERATION_CONFIGURATION)&&(start||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op==LT48_OPERATION_CONFIGURATION){if(!image&&!size)return LT48_OK;
 if(!image||size!=r->fields+1||(image[0]&~((1u<<r->fields)-1)))return LT48_INVALID_ARGUMENT;
 for(unsigned i=0;i<r->fields;i++)if((image[0]&(1u<<i))&&image[i+1]>1)return LT48_INVALID_ARGUMENT;
 }else if(op==LT48_OPERATION_SECURE){if(!image&&!size)return LT48_OK;
 if(!image||size!=1||image[0]!=1)return LT48_INVALID_ARGUMENT;
 }else if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE){if(!image||size!=n)return LT48_INVALID_ARGUMENT;}
 return LT48_OK;
}
