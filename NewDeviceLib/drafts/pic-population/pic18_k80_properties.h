#ifndef LT48_PIC18_K80_PROPERTIES_H
#define LT48_PIC18_K80_PROPERTIES_H
#include <labtool48/labtool48.h>
#include <stdio.h>
#include <stdlib.h>
/* Authored portable UI for the source-verified25288 field domains. Labels
 * describe datasheet fields; no Windows UI asset or executable is embedded. */
static inline void lt48_pic18_k80_properties_free(void *data) {
 lt48_property_model *m=data;if(!m)return;
 for(size_t p=0;p<m->page_count;p++) {
  for(size_t i=0;i<m->pages[p].control_count;i++)free((void *)m->pages[p].controls[i].key);
  free(m->pages[p].controls);
 }
 free(m->pages);free(m);
}
static inline lt48_property_model *lt48_pic18_k80_properties(void) {
 static const char *const boxes[]={"RETEN=1","INTOSCSEL=1","Extended instructions (XINST)","PLL enabled (PLLCFG)","Fail-safe clock monitor","Internal/external switchover","Power-up timer disabled","CANMX=1","MSSPMSK=1","MCLR enabled","Stack overflow reset","BBSIZ=1","Debugger disabled",
 "CP0 protection disabled","CP1 protection disabled","CP2 protection disabled","CP3 protection disabled","Boot read protection disabled","EEPROM read protection disabled",
 "WRT0 protection disabled","WRT1 protection disabled","WRT2 protection disabled","WRT3 protection disabled","Configuration write protection disabled","Boot write protection disabled","EEPROM write protection disabled",
 "EBTR0 protection disabled","EBTR1 protection disabled","EBTR2 protection disabled","EBTR3 protection disabled","Boot table-read protection disabled"};
 static const unsigned fields[]={0,1,2,3,4,5,6,7,8,9,10,11,12,256,257,258,259,260,261,512,513,514,515,516,517,518,768,769,770,771,772};
 static const char *const labels[]={"Secondary oscillator power","Oscillator","Brown-out reset","Brown-out voltage","Brown-out power","Watchdog enable","Watchdog prescaler"};
 static const char *const c0[]={"Reserved","Low-power SOSC","Digital SCLKI","High-power SOSC"};
 static const char *const c1[]={"LP","XT","HS2","HS1","EC3IO","EC3","RCIO","RC","INTIO2","INTIO1","EC2IO","EC2","EC1IO","EC1"};
 static const char *const c2[]={"Disabled","SBOREN control","Enabled except Sleep","Enabled"};
 static const char *const c3[]={"3.0 V","2.7 V","2.0 V","1.8 V"};
 static const char *const c4[]={"Low power","Medium power","High power","ZPBORMV"};
 static const char *const c5[]={"Disabled","Enabled except Sleep","SWDTEN control","Enabled"};
 static const char *const c6[]={"1:1","1:2","1:4","1:8","1:16","1:32","1:64","1:128","1:256","1:512","1:1024","1:2048","1:4096","1:8192","1:16384","1:32768","1:65536","1:131072","1:262144","1:524288","1:1048576","Reserved"};
 static const char *const *const choices[]={c0,c1,c2,c3,c4,c5,c6};
 static const size_t counts[]={4,14,4,4,4,4,22};
 static const char *const ids[]={"User ID 0","User ID 1","User ID 2","User ID 3","User ID 4","User ID 5","User ID 6","User ID 7"};
 lt48_property_model *m=calloc(1,sizeof(*m));if(!m)return NULL;
 m->title="PIC18 K80 configuration";m->pages=calloc(3,sizeof(*m->pages));if(!m->pages){free(m);return NULL;}
 m->page_count=3;static const char *const titles[]={"Clock and power","Protection","User IDs"};
 const size_t sizes[]={20,18,8};
 for(size_t p=0;p<3;p++) {
  m->pages[p].title=titles[p];m->pages[p].controls=calloc(sizes[p],sizeof(*m->pages[p].controls));
  if(!m->pages[p].controls){lt48_pic18_k80_properties_free(m);return NULL;}
  m->pages[p].control_count=sizes[p];
 }
 for(size_t i=0;i<46;i++) {
  unsigned category,field;lt48_property_control *c;
  if(i<31) {
   c=i<13?m->pages[0].controls+i:m->pages[1].controls+i-13;
   category=0;field=fields[i];c->kind=LT48_PROPERTY_CHECKBOX;c->label=boxes[i];
  } else if(i<38) {
   size_t n=i-31;c=m->pages[0].controls+13+n;category=1;field=(unsigned)n;
   c->kind=LT48_PROPERTY_CHOICE;c->label=labels[n];c->metadata.choice=(lt48_property_choice){choices[n],counts[n]};
  } else {
   size_t n=i-38;c=m->pages[2].controls+n;category=2;field=(unsigned)n;
   c->kind=LT48_PROPERTY_NUMBER;c->label=ids[n];c->metadata.number=(lt48_property_number){0,255,16};c->value=255;
  }
  char *key=malloc(64);if(!key){lt48_pic18_k80_properties_free(m);return NULL;}
  if(category==0)snprintf(key,64,"configuration.25288.checkbox.%u.%u",field>>8,field&255);
  else snprintf(key,64,"configuration.25288.%s.%u",category==1?"choice":"number",field);
  c->key=key;
 }
 if(lt48_property_model_validate(m,NULL)!=LT48_OK){lt48_pic18_k80_properties_free(m);return NULL;}
 return m;
}
#endif
