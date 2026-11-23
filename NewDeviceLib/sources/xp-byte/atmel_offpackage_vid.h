/* Exact Dataman 13.10 GDAA Atmel off-package VID defaults. */
#ifndef LT48_XP_BYTE_ATMEL_VID_H
#define LT48_XP_BYTE_ATMEL_VID_H
static int xp_byte_atmel_vid_context(unsigned d,unsigned op){
 if(op!=3 && op!=4)return 0;
 switch(d){case 0x6a:case 0x6c:case 0x76:case 0x7b:case 0x7d:case 0x84:return 1;default:return 0;}}
static int xp_byte_atmel_vid_defaults(const lt48_compiled_wave*w,unsigned op){
 if(lt48_compiled_wave_defaulted_pin_count(w)!=2)return 0;
 for(size_t i=0;i<2;i++){lt48_defaulted_logical_pin p;
  if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK || p.logical_pin!=0 || p.classification!=LT48_DEFAULTED_PIN_UNRESOLVED ||
   p.compact_offset!=(op==3?(i?21u:9u):(i?76u:19u)) || p.compact_opcode!=(op==4 && !i?3u:4u))return 0;}
 return 1;}
#endif
