/* Separate Flash and EEPROM arrays; preserve EEPROM NVR configuration. */
static const struct hc908_abasaz_ee *abasaz_ee(const lt48_hc908_resources*r,unsigned a){
 for(size_t i=0;i<sizeof hc908_abasaz_eeprom/sizeof *hc908_abasaz_eeprom;i++){const struct hc908_abasaz_ee*b=hc908_abasaz_eeprom+i;if(b->definition==r->definition&&a>=b->lo&&a<b->hi)return b;}return NULL;
}
static const struct hc908_gz_bank *abasaz_bank(const lt48_hc908_resources*r,unsigned a){
 for(size_t i=0;i<sizeof hc908_abasaz_banks/sizeof *hc908_abasaz_banks;i++){const struct hc908_gz_bank*b=hc908_abasaz_banks+i;if(b->definition==r->definition&&a>=b->lo&&a<b->hi)return b;}return NULL;
}
static lt48_status abasaz_byte(hc908_session*s,unsigned a,unsigned v){unsigned char b=(unsigned char)v;return monitor_write(s,a,&b,1);}
static lt48_status abasaz_eeprom_ready(hc908_session*s){
 for(size_t i=0;i<sizeof hc908_abasaz_eeprom/sizeof *hc908_abasaz_eeprom;i++){
  const struct hc908_abasaz_ee*b=hc908_abasaz_eeprom+i;if(b->definition!=s->r->definition)continue;
  unsigned char nvr,cfg,div[2],got[2];lt48_status st=monitor_read(s,b->nvr,&nvr,1);
  if(st!=LT48_OK)return st;
  if((nvr&31)!=16)return LT48_SAFETY_REJECTED;
  if(!b->divider)continue; /* Original AZ60 uses its EEPROM RC oscillator. */
  st=monitor_read(s,b->config,&cfg,1);if(st==LT48_OK)st=monitor_read(s,b->divider,div,2);
  unsigned value=cfg&b->clock_mask?84:168; /* selected4.8MHz OSC1 /2 */
  if(st==LT48_OK&&(((unsigned)div[0]&7)*256+div[1])!=value){
   if(!(div[0]&128))return LT48_SAFETY_REJECTED;
   unsigned char want[]={0x80,(unsigned char)value};st=monitor_write(s,b->divider,want,2);
   if(st==LT48_OK)st=monitor_read(s,b->divider,got,2);
   if(st==LT48_OK&&((got[0]&7)||got[1]!=value))st=LT48_SAFETY_REJECTED;
  }
  if(st!=LT48_OK)return st;
 }
 return LT48_OK;
}
static lt48_status abasaz_eeprom_cycle(hc908_session*s,const struct hc908_abasaz_ee*b,unsigned a,unsigned value,int erase){
 unsigned mode=erase?28:4;lt48_status st=abasaz_byte(s,b->control,mode);
 if(st==LT48_OK)st=abasaz_byte(s,a,value);
 if(st==LT48_OK){st=abasaz_byte(s,b->control,mode|1);lt48_status held=target_hold(11);if(st==LT48_OK)st=held;}
 lt48_status end=abasaz_byte(s,b->control,mode);if(st==LT48_OK)st=end;
 end=target_hold(1);if(st==LT48_OK)st=end;
 end=abasaz_byte(s,b->control,0);if(st==LT48_OK)st=end;
 unsigned char got=0;if(st==LT48_OK)st=monitor_read(s,b->control,&got,1);
 if(st==LT48_OK&&got)st=LT48_DEVICE_MISMATCH;
 return st;
}
/* AZ60 rev2.0 requires500 dummy Flash reads after leaving MARGIN mode.
 * This loop runs entirely in RAM, keeping UART overhead outside recovery. */
static lt48_status abasaz_recover(hc908_session*s,const struct hc908_gz_bank*b){
 unsigned char p[]={0x9b,0x45,1,0xf4,0xc6,(unsigned char)(b->dummy>>8),(unsigned char)b->dummy,0xaf,0xff,0x65,0,0,0x26,0xf6,0x83};
 return execute_ram(s,p,sizeof p,NULL,0);
}
static lt48_status abasaz_smart_page(hc908_session*s,const struct hc908_gz_bank*b,unsigned a,const unsigned char*image){
 unsigned char bytes[8],got[8],p[32],dummy;int needed=0;
 for(unsigned i=0;i<8;i++){bytes[i]=flash(s->r,a+i)?image[a+i]:0;needed|=bytes[i];}
 if(!needed)return LT48_OK;
 for(unsigned attempt=0;attempt<84;attempt++){
  lt48_status st=abasaz_byte(s,b->control,1);
  if(st==LT48_OK)st=monitor_read(s,b->bpr,&dummy,1);
  if(st==LT48_OK)st=monitor_write(s,a,bytes,8);
  if(st==LT48_OK){size_t n=gp20_pulse(p,0);p[2]=9;p[4]=p[19]=(unsigned char)(b->control>>8);p[5]=p[20]=(unsigned char)b->control;p[7]=0;p[8]=250;p[17]=1;st=execute_ram(s,p,n,bytes,8);}
  if(st==LT48_OK)st=abasaz_byte(s,b->control,5);
  if(st==LT48_OK)st=target_hold(1);
  if(st==LT48_OK)st=abasaz_byte(s,b->control,4);
  if(st==LT48_OK)st=target_hold(1);
  if(st==LT48_OK)st=monitor_read(s,a,got,8);
  lt48_status end=abasaz_byte(s,b->control,0);if(st==LT48_OK)st=end;
  if(st==LT48_OK)st=abasaz_recover(s,b);
  if(st!=LT48_OK)return st;
  int match=1;for(unsigned i=0;i<8;i++)if(flash(s->r,a+i)&&got[i]!=bytes[i])match=0;
  if(match)return LT48_OK;
 }
 return LT48_DEVICE_MISMATCH;
}
static lt48_status abasaz_mutate(hc908_session*s,int erase,const unsigned char*data){
 lt48_status st=jk_enter(s,500,erase);if(st==LT48_OK)st=abasaz_eeprom_ready(s);
 int old=s->r->definition==0x37;
 if(st==LT48_OK&&erase){
  for(size_t i=0;st==LT48_OK&&i<sizeof hc908_abasaz_banks/sizeof *hc908_abasaz_banks;i++){
   const struct hc908_gz_bank*b=hc908_abasaz_banks+i;if(b->definition!=s->r->definition)continue;
   unsigned char p[64],dummy;size_t n;
   if(old){
    st=abasaz_byte(s,b->control,2);if(st==LT48_OK)st=monitor_read(s,b->bpr,&dummy,1);
    if(st==LT48_OK)st=abasaz_byte(s,b->dummy,0);
    n=gp20_pulse(p,1);p[2]=10;p[4]=p[19]=(unsigned char)(b->control>>8);p[5]=p[20]=(unsigned char)b->control;p[7]=0x66;p[8]=0x8a;p[17]=2;
   }else{n=hc908_erase_ram(p,b->dummy);p[30]=8;p[n-4]=20;gz_ram(p,n,b);}
   if(st==LT48_OK)st=execute_ram(s,p,n,NULL,0);
   if(old){lt48_status end=abasaz_byte(s,b->control,0);if(st==LT48_OK)st=end;if(st==LT48_OK)st=target_hold(1);}
  }
  for(size_t i=0;st==LT48_OK&&i<sizeof hc908_abasaz_eeprom/sizeof *hc908_abasaz_eeprom;i++){
   const struct hc908_abasaz_ee*b=hc908_abasaz_eeprom+i;if(b->definition==s->r->definition)st=abasaz_eeprom_cycle(s,b,b->lo,255,1);
  }
 }else if(st==LT48_OK){
  for(unsigned i=0;st==LT48_OK&&i<s->r->gx->range_count;i++){
   unsigned a=s->r->gx->ranges[i][0],end=s->r->gx->ranges[i][1];
   while(st==LT48_OK&&a<end){
    const struct hc908_abasaz_ee*e=abasaz_ee(s->r,a);
    if(e){if(data[a]!=255)st=abasaz_eeprom_cycle(s,e,a,data[a],0);a++;}
    else if(old){const struct hc908_gz_bank*b=abasaz_bank(s->r,a);if(!b){st=LT48_SAFETY_REJECTED;break;}st=abasaz_smart_page(s,b,a&~7u,data);a=(a&~7u)+8;}
    else{unsigned n=32-(a&31);if(n>end-a)n=end-a;st=jk_write_row(s,a,data+a,n);a+=n;}
   }
  }
 }
 lt48_status end=jk_exit(s);return st==LT48_OK?end:st;
}
