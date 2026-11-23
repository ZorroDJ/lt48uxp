/* GP20 first-generation Flash: erased00, eight-byte latches before HV,
 * FDIV=/2 at source4MHz, 1.10ms pulses and internal margin reads.
 * MC68HC908GP20 rev2.1 section11; explicit Linux target-RAM replacement
 * for the original smart-program worker. No UART while HVEN is asserted. */
static lt48_status gp20_control(hc908_session*s,unsigned char v){
 return monitor_write(s,0xfe08,&v,1);
}
static size_t gp20_pulse(unsigned char*p,int erase){
 const unsigned char code[]={0x9b,0xa6,0x49,0xc7,0xfe,8,0x45,2,0x26,
  0xaf,0xff,0x65,0,0,0x26,0xf9,0xa6,0x41,0xc7,0xfe,8,0x83};
 memcpy(p,code,sizeof code);
 if(erase){p[2]=0x4a;p[7]=0xff;p[8]=0xff;p[17]=0x42;}
 return sizeof code;
}
static lt48_status gp20_page(hc908_session*s,unsigned a,const unsigned char*image){
 unsigned char bytes[8],got[8],code[32],dummy;
 for(unsigned i=0;i<8;i++)bytes[i]=image[a+i];
 for(unsigned attempt=0;attempt<32;attempt++){
  lt48_status st=gp20_control(s,0x41);
  if(st==LT48_OK)st=monitor_read(s,0xff80,&dummy,1);
  if(st==LT48_OK)st=monitor_write(s,a,bytes,8);
  if(st==LT48_OK){size_t n=gp20_pulse(code,0);st=execute_ram(s,code,n,bytes,8);}
  if(st==LT48_OK)st=gp20_control(s,0x45); /* >=50us since HVEN fell */
  if(st==LT48_OK)st=target_hold(1); /* tVTP >=150us */
  if(st==LT48_OK)st=gp20_control(s,0x44);
  if(st==LT48_OK)st=target_hold(1); /* tHVD >=50us */
  if(st==LT48_OK)st=monitor_read(s,a,got,8);
  lt48_status end=gp20_control(s,0);if(st==LT48_OK)st=end;
  if(st!=LT48_OK)return st;
  int match=1;for(unsigned i=0;i<8;i++)if(flash(s->r,a+i)&&got[i]!=bytes[i])match=0;
  if(match)return LT48_OK;
 }
 return LT48_DEVICE_MISMATCH;
}
static lt48_status gp20_mutate(hc908_session*s,int erase,const unsigned char*data){
 lt48_status st=jk_enter(s,500,erase);unsigned char dummy,code[32];
 if(st==LT48_OK&&erase){
  st=gp20_control(s,0x42);
  if(st==LT48_OK)st=monitor_read(s,0xff80,&dummy,1);
  dummy=0;if(st==LT48_OK)st=monitor_write(s,0xb000,&dummy,1);
  if(st==LT48_OK){size_t n=gp20_pulse(code,1);st=execute_ram(s,code,n,NULL,0);}
  if(st==LT48_OK)st=gp20_control(s,0); /* tKill >=200us already elapsed */
  if(st==LT48_OK)st=target_hold(1);
 }else if(st==LT48_OK){
  for(unsigned a=0xb000;st==LT48_OK&&a<0xfe00;a+=8)st=gp20_page(s,a,data);
  for(unsigned a=0xffd8;st==LT48_OK&&a<0x10000;a+=8)st=gp20_page(s,a,data);
  if(st==LT48_OK)st=gp20_page(s,0xff80,data); /* protection last */
 }
 /* Clear internal margin/program mode also on failures before source reset. */
 lt48_status clear=gp20_control(s,0);if(st==LT48_OK)st=clear;
 lt48_status end=jk_exit(s);return st==LT48_OK?end:st;
}
