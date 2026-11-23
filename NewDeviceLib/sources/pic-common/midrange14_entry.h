#ifndef LT48_PIC_MIDRANGE14_ENTRY_H
#define LT48_PIC_MIDRANGE14_ENTRY_H
#include <labtool48/labtool48.h>
#include <string.h>
/* DS41572C section4.1, DS41561C/DS41686A section4: VPP-first works
 * regardless of the previous oscillator/MCLR configuration. Disconnect VDD
 * before VPP on exit. These are controller requests, not measured waveforms.
 * The caller must first verify the source digest and exact D/M/P membership. */
static inline lt48_status lt48_pic_midrange_entry_compile(
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,
 unsigned role,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!pins||!vars||n!=10||(role!=3&&role!=4))return LT48_SAFETY_REJECTED;
 unsigned vcc=0,vpp=0;for(size_t i=0;i<n;i++){
  if(vars[i].variable_id==0){if(vars[i].kind||(vars[i].channel!=1&&vars[i].channel!=2))return LT48_SAFETY_REJECTED;vcc++;}
  if(vars[i].variable_id==3){if(vars[i].kind||vars[i].channel!=3)return LT48_SAFETY_REJECTED;vpp++;}
 }
 if(vcc!=1||vpp!=1)return LT48_SAFETY_REJECTED;
 const unsigned char entry[]={
  0x11,53,0,0x11,52,0,0x11,14,0,
  0x32,100,0,0,0,
  0x03,14,3,0x00,3,0x32,0xe8,3,0,0,
  0x02,1,0,0x02,2,0,0x0d,2,1,0x0d,2,2,
  0x32,0xe8,3,0,0
 };
 const unsigned char exit[]={
  0x11,53,0,0x11,52,0,
  0x0e,2,1,0,0x0e,2,2,0,
  0x32,0xe8,3,0,0,
  0x01,3,0,0x11,14,0,
  0x32,0xe8,3,0,0,0x36
 };
 const unsigned char *payload=role==3?entry:exit;
 size_t bytes=role==3?sizeof(entry):sizeof(exit);unsigned char raw[128];
 raw[0]=(unsigned char)bytes;raw[1]=(unsigned char)(bytes>>8);memcpy(raw+2,payload,bytes);
 lt48_compact_wave *decoded=NULL;lt48_status st=lt48_compact_decode(raw,bytes+2,&decoded);
 if(st==LT48_OK)st=lt48_compact_compile(decoded,pins,vars,n,1,0,out);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK){lt48_compiled_wave_close(*out);
 *out=NULL;}
 lt48_compact_wave_close(decoded);
 return st;
}
#endif
