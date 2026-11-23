#include "../pic-common/source_verify_voltage.h"

/* ATmega parallel HV procedures. Only source compact routines touch pins.
 * Direct configuration is separate from legacy CheckID; all three lock banks
 * and fuse bytes survive between callbacks. No programmer identity access. */
static unsigned atmega_lock_field(const avr_session*s,unsigned i){if(s->r->old_avr)return s->r->old_avr->locks[i];if(atmega_legacy(s->r))return i?i+5:2;return (s->r->atmega==2||s->r->atmega==5||s->r->atmega==6||(s->r->schema&&s->r->schema->swap_locks))?(i==0?2:i-1):i;}
static unsigned atmega_lock(avr_session*s,unsigned i){return s->r->schema&&s->r->schema->no_boot&&i?0:cfg(s,atmega_legacy(s->r)?0:-2,1,atmega_lock_field(s,i));}
static void atmega_set_lock(avr_session*s,unsigned i,unsigned v){if(!(s->r->schema&&s->r->schema->no_boot&&i))setcfg(s,atmega_legacy(s->r)?0:-2,1,atmega_lock_field(s,i),v);}
static lt48_status atmega_get_config(avr_session*s,lt48_avr_result*out){
 /* Tiny15 mode1 can temporarily enable SPI fuses. Snapshot in HV mode8. */
 int hybrid=s->r->old_avr&&s->r->old_avr->family==162;
 lt48_status st=start(s,hybrid?8:1,s->r->nominal);
 if(!st&&hybrid)st=run(s,23);
 if(st==LT48_OK){
  unsigned v=get_fuse_byte(s);out->lock_mode=atmega_lock(s,0);
  out->boot_lock[0]=atmega_lock(s,1);out->boot_lock[1]=atmega_lock(s,2);
  if(v>0xffffff||out->lock_mode>2||out->boot_lock[0]>3||out->boot_lock[1]>3)st=LT48_RUNTIME_ERROR;
  else {out->configuration_valid=out->fuse_byte_valid=1;out->fuse_byte_count=lt48_avr_fuse_byte_count(s->r);
   out->fuse_byte=v&255;out->fuse_high=(v>>8)&255;out->fuse_extended=v>>16;
   out->spien=atmega_spien_mask(s->r)?!(v&atmega_spien_mask(s->r)):UINT32_MAX;out->aux_fuse=atmega_eesave_mask(s->r)?!(v&atmega_eesave_mask(s->r)):0;
   if(!st&&s->r->old_avr&&(s->r->schema->family==76||s->r->schema->family==162)){unsigned cal=cfg(s,s->r->schema->family==76?-2:0,2,0);if(cal>255)st=LT48_RUNTIME_ERROR;else out->calibration=cal;}
  }
 }
 return end(s,st);
}
static lt48_status atmega_scan(avr_session*s,lt48_operation_kind op,const unsigned char*expected,unsigned char*dest,unsigned vcc,lt48_avr_result*out){
 /* A read-protected FF array must not be published as a successful read. */
 if(atmega_lock(s,0)==2)return LT48_DEVICE_MISMATCH;
 lt48_status st=start(s,1,vcc);
 for(unsigned a=0;st==LT48_OK&&a<s->r->capacity/2;a++){
  dword(s->buffers[0],a);memset(s->buffers[1],0,32);
  for(unsigned b=0;b<2;b++)s->buffers[2][b]=op==LT48_OPERATION_BLANK_CHECK?255:expected?expected[2*a+b]:0;
  st=run(s,4);if(st)break;
  for(unsigned b=0;b<2;b++){unsigned off=2*a+b;unsigned char actual=s->buffers[1][b];if(dest)dest[off]=actual;
   if(op!=LT48_OPERATION_READ&&actual!=(op==LT48_OPERATION_BLANK_CHECK?255:expected[off])){if(!out->mismatch_count)out->first_mismatch=off;out->mismatch_count++;}}
  out->completed_bytes=2*a+2;
  if(s->api->progress&&!(a&63))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,2*a+2,s->r->capacity,0,0,0);
  if(out->mismatch_count&&op!=LT48_OPERATION_COMPARE){st=LT48_DEVICE_MISMATCH;break;}
 }
 st=end(s,st);return st==LT48_OK&&out->mismatch_count?LT48_DEVICE_MISMATCH:st;
}
static lt48_status atmega_protect(avr_session*s,unsigned fuses,const unsigned locks[3]){
 set_fuse_byte(s,fuses);for(unsigned i=0;i<3;i++)atmega_set_lock(s,i,locks[i]);
 lt48_status st=start(s,8,s->r->nominal);if(!st)st=run(s,7);return end(s,st);
}
static lt48_status atmega_execute(const lt48_avr_resources*r,const lt48_protocol_transport*api,void*context,
 lt48_operation_kind op,const unsigned char*image,unsigned char*read,lt48_verify_margin_selection margin,unsigned margin_type,lt48_avr_result*out){
 lt48_protocol_transport recovery=*api;
 avr_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;memcpy(s.vars,r->vars,sizeof(s.vars));
 unsigned char*pending=malloc(r->capacity);if(!pending)return LT48_NO_MEMORY;
 setcfg(&s,-1,0,1,1);setcfg(&s,-1,0,5,0);
 if(atmega_legacy(r)){
  for(unsigned i=0;i<3;i++)atmega_set_lock(&s,i,0);
  setcfg(&s,0,1,8,0); /* Legacy choice0 disables calibration insertion. */
  setcfg(&s,0,2,4,0);setcfg(&s,0,2,0,0);
 }else{
  for(unsigned i=0;i<4;i++)setcfg(&s,-2,1,i,0);
  setcfg(&s,-2,1,r->schema&&r->schema->no_boot&&r->schema->numeric_base==0?1:3,1);
  for(unsigned i=0;i<5;i++)setcfg(&s,-2,2,i,0);
 }
 lt48_status st=lt48_runtime_transport_session_create(api,context,&s.wire);
 if(st)goto done;
 const unsigned char busy[]={0x56,0x0a,1,0x40,1};
 st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
 if(!st)st=lt48_owner_load_fpga_internal(s.wire,r->idle,r->idle_size);
 if(!st)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof busy);
 if(!st)st=atmega_get_config(&s,out);
 int have_snapshot=!st;
 unsigned old=result_fuses(out),want=old;unsigned locks[3]={out->lock_mode,out->boot_lock[0],out->boot_lock[1]};
 uint32_t levels[4]={0};unsigned level_count;
 if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,margin_type,0,levels,&level_count);
 unsigned low=levels[0],high=levels[1];
 if(!st&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE)){
  st=start(&s,7,r->nominal);if(!st)st=run(&s,6);st=end(&s,st);
  /* NEW-route DEVINIT cannot clear EESAVE while locked. First erase unlocks;
   * repeat the same source entry/erase once unlocked to clear EEPROM too. */
  if(!st&&(!atmega_legacy(r)||r->old_avr)&&atmega_eesave_mask(r)&&(r->atmega==3||r->atmega==4||r->atmega==5||r->atmega==6||r->atmega==7||r->atmega==8||r->atmega==9)&&locks[0]&&!(old&atmega_eesave_mask(r))){
   st=start(&s,7,r->nominal);if(!st)st=run(&s,6);st=end(&s,st);
  }
  for(unsigned i=0;i<3;i++)locks[i]=0;
  if(!st)st=atmega_protect(&s,old,locks);
  if(!st)st=atmega_get_config(&s,out);
  if(!st&&(result_fuses(out)!=old||out->lock_mode||out->boot_lock[0]||out->boot_lock[1]))st=LT48_DEVICE_MISMATCH;
  if(!st)st=atmega_scan(&s,LT48_OPERATION_BLANK_CHECK,NULL,NULL,r->nominal,out);
 }
 if(!st&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)){
  if(op==LT48_OPERATION_OVER_PROGRAM&&locks[0])st=LT48_DEVICE_MISMATCH;
  if(r->fuse_byte>=0)want=(want&0xffff00)|(unsigned)r->fuse_byte;
  if(r->fuse_high>=0)want=(want&0xff00ff)|((unsigned)r->fuse_high<<8);
  if(r->fuse_extended>=0)want=(want&0xffff)|((unsigned)r->fuse_extended<<16);
  if(r->spien>=0)want=r->spien?want&~atmega_spien_mask(r):want|atmega_spien_mask(r);
  if(r->aux_fuse>=0)want=r->aux_fuse?want&~atmega_eesave_mask(r):want|atmega_eesave_mask(r);
  set_fuse_byte(&s,want);
  if(!st)st=start(&s,2,r->nominal);
  for(unsigned a=0;!st&&a<r->capacity/2;a++){
   dword(s.buffers[0],a);memcpy(s.buffers[2],image+2*a,2);memcpy(s.buffers[1],image+2*a,2);st=run(&s,5);
   if(api->progress&&!(a&63))api->progress(context,LT48_EXECUTION_READ_OPERATION,2*a+2,r->capacity,0,0,0);
  }
  st=end(&s,st);
  /* Normal Program has explicit pre-erase. Over-program omits it. */
  if(op==LT48_OPERATION_PROGRAM)for(unsigned i=0;i<3;i++){locks[i]=0;atmega_set_lock(&s,i,0);}
  if(!st)st=atmega_scan(&s,LT48_OPERATION_VERIFY,image,NULL,low,out);
  if(!st)st=atmega_scan(&s,LT48_OPERATION_VERIFY,image,NULL,high,out);
  for(unsigned i=0;i<2;i++)if(r->boot_choice[i]>=0)locks[i+1]=(unsigned)r->boot_choice[i];
  if(!st)st=atmega_protect(&s,want,locks);
  if(!st)st=atmega_get_config(&s,out);
  if(!st&&(result_fuses(out)!=want||out->lock_mode!=locks[0]||out->boot_lock[0]!=locks[1]||out->boot_lock[1]!=locks[2]))st=LT48_DEVICE_MISMATCH;
 }else if(!st&&op==LT48_OPERATION_SECURE){
  locks[0]=2;for(unsigned i=0;i<2;i++)if(r->boot_choice[i]>=0)locks[i+1]=(unsigned)r->boot_choice[i];if(!st)st=atmega_protect(&s,old,locks);if(!st)st=atmega_get_config(&s,out);
  if(!st&&(result_fuses(out)!=old||out->lock_mode!=2||out->boot_lock[0]!=locks[1]||out->boot_lock[1]!=locks[2]))st=LT48_DEVICE_MISMATCH;
 }else if(!st&&op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_NOMINAL){
  if(margin!=LT48_VERIFY_MARGIN_HIGH)st=atmega_scan(&s,op,image,NULL,low,out);
  if(!st&&margin!=LT48_VERIFY_MARGIN_LOW)st=atmega_scan(&s,op,image,NULL,high,out);
 }else if(!st&&op!=LT48_OPERATION_ERASE)st=atmega_scan(&s,op,image,op==LT48_OPERATION_READ?pending:NULL,r->nominal,out);
 /* Tiny15 memory access temporarily enables SPI. Restore the snapshot after
  * memory-only operations and failed writes, through its original HV callbacks.
  * A fresh uncancelled session makes restoration independent of a failed wire. */
 if(r->old_avr&&r->old_avr->family==162&&have_snapshot&&
    (st||op==LT48_OPERATION_READ||op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)){
  lt48_status operation=st,restore;recovery.cancelled=NULL;
  st=finish(&s,st,out);s.api=&recovery;s.wire=NULL;
  restore=lt48_runtime_transport_session_create(&recovery,context,&s.wire);
  if(!restore)restore=lt48_owner_load_fpga_internal(s.wire,r->idle,r->idle_size);
  if(!restore)restore=atmega_protect(&s,old,locks);
  if(!restore)restore=atmega_get_config(&s,out);
  if(!restore&&(result_fuses(out)!=old||out->lock_mode!=locks[0]))restore=LT48_DEVICE_MISMATCH;
  if(restore)out->configuration_valid=out->fuse_byte_valid=0;
  st=restore?restore:operation;
 }
 out->operation_status=st;st=finish(&s,st,out);
 if(!st&&op==LT48_OPERATION_READ)memcpy(read,pending,r->capacity);
 if(!st)out->completed_bytes=r->capacity;
 done:free(pending);return st;
}
