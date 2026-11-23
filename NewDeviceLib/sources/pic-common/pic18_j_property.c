#include "pic18_j_property.h"
#include <stdlib.h>
#include <string.h>
struct lt48_pic18_j_property {
 const lt48_pic18_j_options_profile *profile;
 lt48_property_model model;lt48_property_page page;
 lt48_property_control *controls;const char ***labels;
};
void lt48_pic18_j_property_close(lt48_pic18_j_property *p) {
 if(!p)return;
 if(p->labels)for(size_t i=0;i<p->profile->field_count;i++)free(p->labels[i]);
 free(p->labels);free(p->controls);free(p);
}
lt48_property_model *lt48_pic18_j_property_model(lt48_pic18_j_property *p){return p?&p->model:NULL;}
lt48_status lt48_pic18_j_property_create(const lt48_pic18_j_options_profile *profile,
 const uint8_t *image,size_t size,lt48_pic18_j_property **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_pic18_j_option_value values[64];size_t n=0;
 lt48_status st=lt48_pic18_j_options_decode(profile,image,size,values,64,&n);
 if(st!=LT48_OK)return st;
 if(n>LT48_PROPERTY_MAX_CHOICE_CONTROLS)return LT48_UNSUPPORTED;
 lt48_pic18_j_property *p=calloc(1,sizeof(*p));if(!p)return LT48_NO_MEMORY;
 p->profile=profile;p->controls=calloc(n,sizeof(*p->controls));p->labels=calloc(n,sizeof(*p->labels));
 if(!p->controls||!p->labels){lt48_pic18_j_property_close(p);return LT48_NO_MEMORY;}
 for(size_t i=0;i<n;i++) {
  const lt48_pic18_j_option *f=profile->fields+i;size_t selected=SIZE_MAX;
  if(f->choice_count>LT48_PROPERTY_MAX_CHOICES){lt48_pic18_j_property_close(p);return LT48_UNSUPPORTED;}
  p->labels[i]=calloc(f->choice_count,sizeof(*p->labels[i]));
  if(!p->labels[i]){lt48_pic18_j_property_close(p);return LT48_NO_MEMORY;}
  for(size_t j=0;j<f->choice_count;j++) {
   p->labels[i][j]=f->choices[j].name;
   if(f->choices[j].value==values[i].value)selected=j;
  }
  if(selected==SIZE_MAX){lt48_pic18_j_property_close(p);return LT48_UNSUPPORTED;}
  p->controls[i]=(lt48_property_control){.key=f->name,.label=f->name,.kind=LT48_PROPERTY_CHOICE,.value=(uint32_t)selected};
  p->controls[i].metadata.choice=(lt48_property_choice){p->labels[i],f->choice_count};
 }
 p->page=(lt48_property_page){"Configuration",p->controls,n};p->model=(lt48_property_model){"Device configuration",&p->page,1};
 st=lt48_property_model_validate(&p->model,NULL);
 if(st!=LT48_OK){lt48_pic18_j_property_close(p);return st;}
 *out=p;return LT48_OK;
}
lt48_status lt48_pic18_j_property_commit(lt48_pic18_j_property *p,uint8_t *image,size_t size) {
 if(!p)return LT48_INVALID_ARGUMENT;
 const lt48_pic18_j_options_profile *profile=p->profile;
 if(p->model.pages!=&p->page||p->model.page_count!=1||p->page.controls!=p->controls||
    p->page.control_count!=profile->field_count)return LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_property_model_validate(&p->model,NULL);if(st!=LT48_OK)return st;
 lt48_pic18_j_option_value edits[64];
 for(size_t i=0;i<profile->field_count;i++) {
  const lt48_pic18_j_option *f=profile->fields+i;const lt48_property_control *c=p->controls+i;
  if(!c->key||strcmp(c->key,f->name)||c->kind!=LT48_PROPERTY_CHOICE||
     c->metadata.choice.labels!=p->labels[i]||c->metadata.choice.count!=f->choice_count||c->value>=f->choice_count)return LT48_INVALID_ARGUMENT;
  edits[i]=(lt48_pic18_j_option_value){f->id,f->choices[c->value].value};
 }
 return lt48_pic18_j_options_edit(profile,image,size,edits,profile->field_count);
}
