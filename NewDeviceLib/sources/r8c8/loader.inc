#include "profiles.h"
unsigned lt48_r8c8_capacity_internal(unsigned cls,unsigned d,unsigned m,unsigned p){const struct r8c8_profile*q=r8c8_find(cls,d);
 return q&&q->m==m&&q->p==p?q->cap:0;
 }
int lt48_r8c8_admitted_internal(unsigned cls,unsigned d){const struct r8c8_profile*q=r8c8_find(cls,d);
 return q&&((d==0x265||d==0x266)?LT48_R8C8_STAGE2_ADMITTED:LT48_R8C8_STAGE1_ADMITTED);
 }
lt48_status lt48_r8c8_open(const lt48_installation*x,size_t i,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_device_spec s;
 lt48_status st=lt48_installation_resolve_device(x,i,&s);
 if(st)return st;
 const struct r8c8_profile*q=r8c8_find(s.catalogue.device_class,s.definition_resource);
 if(!q)return LT48_UNSUPPORTED;
 
 if(strcmp(s.catalogue.vendor,"Renesas")||s.catalogue.device_name_length!=strlen(q->name)||memcmp(s.catalogue.device_name,q->name,strlen(q->name))||s.family_code!=q->m||s.pinmap!=q->p||s.pin_count!=q->pins||s.capacity_bytes!=q->cap||s.element_bits!=8)return LT48_SAFETY_REJECTED;
 
 lt48_s8253_resources*r=calloc(1,sizeof*r);
 if(!r)return LT48_NO_MEMORY;
 r->kind=51;
 r->definition=q->d;
 r->family=q->m;
 r->pinmap=q->p;
 r->capacity=q->cap;
 r->flash=q->lengths[q->regions-1];
 r->nominal=500;
 r->n79_xp=q->xp;
 r->regions=q->regions;
 r->fields=1;
 r->configuration_id=25047;
 r->configuration_category[0]=0;
 r->configuration_field[0]=0;
 r->configuration_maximum[0]=1;
 
 for(unsigned k=0;
 k<q->regions;
 k++){r->region_length[k]=q->lengths[k];
 r->region_bank[k]=q->physical[k];
 r->n79_physical[k]=q->physical[k];
 }
 char path[4096],name[32];
 size_t ri,fi;
 snprintf(path,sizeof path,"%s/%s",lt48_installation_path(x),q->source);
 
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,q->sha,&r->rez));
 lt48_pin_code pc;
 snprintf(name,sizeof name,"P%x",q->p);
 TRY(lt48_rez_find_resource(r->rez,name,&ri));
 TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));
 TRY(lt48_pin_map_parse(&pc,&r->pins));
 TRY(lt48_pin_map_complete_groups_internal(r->pins,36));
 
 lt48_family_info f;
 snprintf(name,sizeof name,"M%x",q->m);
 TRY(lt48_rez_find_resource(r->rez,name,&fi));
 TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.variable_count!=q->variables||f.operation_count!=q->operations||f.configuration_set!=25047){st=LT48_FORMAT_ERROR;
 goto fail;
 }r->variable_count=f.variable_count;
 
 for(unsigned k=0;
 k<r->variable_count;
 k++){lt48_family_variable v;
 TRY(lt48_rez_get_family_variable(r->rez,fi,k,&v));
 r->vars[k]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
 }if(r->vars[0].variable_id||r->vars[0].value!=500||r->vars[0].channel!=2){st=LT48_FORMAT_ERROR;
 goto fail;
 }
 unsigned apps=0,mask=0;
 for(unsigned k=0;
 k<f.operation_count;
 k++){lt48_family_operation op;
 lt48_wave_code w;
 TRY(lt48_rez_get_family_operation(r->rez,fi,k,&op));
 snprintf(name,sizeof name,"W%x",op.wave);
 TRY(lt48_rez_find_resource(r->rez,name,&ri));
 TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 
 if(!w.wave_type&&op.operation_id<17&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;
 TRY(lt48_compact_decode(w.data,w.data_size,&c));
 st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);
 lt48_compact_wave_close(c);
 if(st)goto fail;
 }
 else if(w.wave_type==6&&apps<5){unsigned a=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:op.operation_id==23?0x9000:0;
 if(!a||w.load_address!=a){st=LT48_FORMAT_ERROR;
 goto fail;
 }mask|=1u<<(op.operation_id-17);
 r->applet[apps]=w;
 r->addresses[apps++]=(lt48_applet_address){op.operation_id,a};
 }
 else if(w.wave_type==4&&op.operation_id==(q->xp?27u:30u)&&w.data_size==2780&&!r->fpga)r->fpga=w.data+2;
 else{st=LT48_FORMAT_ERROR;
 goto fail;
 }}
 if(apps!=5||mask!=93||!r->fpga){st=LT48_FORMAT_ERROR;
 goto fail;
 }r->sstmx_applets=5;
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
 *out=r;
 return LT48_OK;
 
 fail:lt48_s8253_close(r);
 return st;
 
#undef TRY
}
