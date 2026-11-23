#ifndef LT48_DRAFT_PIC18_XX39_EEPROM_READ_H
#define LT48_DRAFT_PIC18_XX39_EEPROM_READ_H
#include <labtool48/labtool48.h>
#include <string.h>
/* DS80148A issue4, DS30480C2.6.2: command0000 executes a CPU instruction.
 * Insert exactly one NOP between MOVWF EEADR and BSF EECON1,RD. Keep the
 * immediate MOVF EEDATA required by issue2. Exact original Read/Blank/Verify
 * payloads only. The original archive and all existing code addresses stay
 * unchanged: one LCALL replaces three bytes and calls an appended RAM helper. */
static inline lt48_status lt48_pic18_xx39_eeprom_read_applet(unsigned role,
 const unsigned char *source,size_t size,uint16_t base,unsigned char *output,
 size_t capacity,size_t *written) {
 if(!written)return LT48_INVALID_ARGUMENT;
 *written=0;
 if(!source||!output)return LT48_INVALID_ARGUMENT;
 typedef struct {unsigned role;uint16_t base,end;size_t size,offset;const char *sha;} binding;
 static const binding allowed[]={
  {17,0x6000,0x7000,937,0x302,"4c21ac7beda0ccb4f25e5635b7b72db49e01584ace38111dcec581409e3cdda2"},
  {19,0x7800,0x8000,1147,0x399,"ab8995a2b5843048c44bbe499d650b12ef958e9c1c5b7c1703538c7404ee7f96"},
  {21,0x7000,0x7800,1087,0x378,"0b837f67a85751563175d0961683a0de22a7b34bc34d9a7a4fedd21b5a97f5fa"}
 };
 const binding *b=NULL;
 for(size_t i=0;i<sizeof(allowed)/sizeof(*allowed);i++)if(allowed[i].role==role)b=allowed+i;
 if(!b||base!=b->base||size!=b->size)return LT48_SAFETY_REJECTED;
 unsigned char hash[32];lt48_sha256(source,size,hash);
 static const char hex[]="0123456789abcdef";
 for(unsigned i=0;i<32;i++)if(hex[hash[i]>>4]!=b->sha[2*i]||hex[hash[i]&15]!=b->sha[2*i+1])return LT48_SAFETY_REJECTED;
 const size_t extra=43;
 if(capacity<size+extra)return LT48_BUFFER_TOO_SMALL;
 if((uint32_t)base+size+extra>b->end||memcmp(source+b->offset,"\x74\xd4\xf0",3))return LT48_SAFETY_REJECTED;
 memcpy(output,source,size);
 uint16_t helper=(uint16_t)(base+size);
 output[b->offset]=0x12;output[b->offset+1]=(unsigned char)(helper>>8);output[b->offset+2]=(unsigned char)helper;
 /* F000 port grammar: four command bits then two operand octets. Explicit
  * FPGA-ready waits terminate each unit, exactly as in the source emitter. */
 const unsigned char bytes[]={0xd4,4,0xc0,0,0xd4,8,0xc0,0,0xc0,0};
 size_t n=size;
 for(unsigned i=0;i<sizeof(bytes);i++) {
  output[n++]=0x74;output[n++]=bytes[i];output[n++]=0xf0;
  if(i==3||i==7||i==9){output[n++]=0x30;output[n++]=0x94;output[n++]=0xfd;}
 }
 /* Execute the displaced MOV A,#D4;MOVX @DPTR,A before returning. */
 memcpy(output+n,source+b->offset,3);n+=3;output[n++]=0x22;
 if(n!=size+extra)return LT48_RUNTIME_ERROR;
 *written=n;return LT48_OK;
}
#endif
