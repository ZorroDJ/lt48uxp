#include "profiles.h"
#include "admission.h"
static const struct p29_profile*p29_find(unsigned d){for(size_t i=0;i<sizeof p29_profiles/sizeof*p29_profiles;i++)if(p29_profiles[i].definition==d)return p29_profiles+i;return NULL;}
unsigned lt48_psoc29_capacity_internal(unsigned cls,unsigned d,unsigned family,unsigned pinmap){const struct p29_profile*p=p29_find(d);return cls==2038&&p&&p->family==family&&p->pinmap==pinmap?p->capacity:0;}
int lt48_psoc29_admitted_internal(unsigned d){return p29_find(d)&&p29_admitted(d);}
lt48_status lt48_psoc29_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_catalogue_entry e;lt48_status early=lt48_installation_get_device(x,index,&e);if(early)return early;if(e.device_class!=2038||!p29_find(e.device_type))return LT48_UNSUPPORTED;
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,index,&s);if(st)return st;
 if(s.catalogue.device_class!=2038)return LT48_UNSUPPORTED;
 const struct p29_profile*p=p29_find(s.definition_resource);if(!p)return LT48_UNSUPPORTED;
 unsigned found=0;for(size_t i=0;i<sizeof p29_members/sizeof*p29_members;i++){const char*n=p29_members[i].name;if(p29_members[i].definition==p->definition&&p29_members[i].manufacturer==s.catalogue.manufacturer_id&&p29_members[i].device==s.catalogue.device_id&&!strcmp(p29_members[i].vendor,s.catalogue.vendor)&&strlen(n)==s.catalogue.device_name_length&&!memcmp(n,s.catalogue.device_name,s.catalogue.device_name_length))found=1;}
 if(!found||s.family_code!=p->family||s.pinmap!=p->pinmap||s.capacity_bytes!=p->capacity)return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;
 r->psoc_security_bytes=p->security;r->kind=11;r->sstmx_applets=6;r->n79_xp=1;r->manufacturer=s.catalogue.manufacturer_id;r->device=s.catalogue.device_id;r->definition=p->definition;r->family=p->family;r->pinmap=p->pinmap;r->capacity=r->flash=p->capacity;r->nominal=500;r->regions=1;r->region_length[0]=p->capacity;
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/%s",lt48_installation_path(x),"GDAE___.rez");
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=p->variables||f.configuration_set!=0||f.operation_count!=p->ops){st=LT48_FORMAT_ERROR;goto fail;}
 r->variable_count=f.variable_count;for(unsigned i=0;i<f.variable_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned apps=0,mask=0;
 for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 if(!w.wave_type&&op.operation_id<12&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st){fprintf(stderr,"PSoC D%x role%u compile status%d\n",p->definition,op.operation_id,st);goto fail;}}
 else if(w.wave_type==6&&apps<6){unsigned a=op.operation_id==16?0x5000:op.operation_id==17?0x6000:op.operation_id==20?0x8000:op.operation_id==21?0x7000:op.operation_id==23?0x9000:op.operation_id==24?0x9800:0;unsigned bit=op.operation_id==16?1:op.operation_id==17?2:op.operation_id==20?4:op.operation_id==21?8:op.operation_id==23?16:32;if(!a||a!=w.load_address||(mask&bit)){st=LT48_FORMAT_ERROR;goto fail;}mask|=bit;r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,a};}
 else if(w.wave_type==4&&op.operation_id==27u&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a&&!r->fpga)r->fpga=w.data+2;
 else{st=LT48_UNSUPPORTED;goto fail;}}
 /* Authored Dda package binding: source W166 leaves the selected clock pad
  * input-only. W167 is the same-source P73/CY8C29566 GPIO configuration;
  * all six worker payloads and the P73 pin map are identical. */
 if(p->definition==0xda){lt48_wave_code corrected;TRY(lt48_rez_find_resource(r->rez,"W167",&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&corrected));if(corrected.wave_type!=4||corrected.data_size!=2780||corrected.data[0]!=0xda||corrected.data[1]!=0x0a){st=LT48_FORMAT_ERROR;goto fail;}r->fpga=corrected.data+2;}
 if(mask!=63u||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
lt48_status lt48_psoc29_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=11||!n||start>=r->capacity||n>r->capacity-start)return LT48_INVALID_ARGUMENT;
 if(op<1||op>8)return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7||op==8)&&(start||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op==8)return im&&size==r->psoc_security_bytes?LT48_OK:LT48_INVALID_ARGUMENT;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}

int lt48_psoc39_member_internal(unsigned d){return p39_member(d);}
