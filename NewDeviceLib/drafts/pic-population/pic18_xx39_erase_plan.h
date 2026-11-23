#ifndef LT48_DRAFT_PIC18_XX39_ERASE_PLAN_H
#define LT48_DRAFT_PIC18_XX39_ERASE_PLAN_H
#include <stddef.h>
#include <stdint.h>

/* Authored DS30480C tables 3-1, 3-2, 3-3 and 3-5 erase sequence.
 * This portable plan is not an admitted runtime driver. The backend must
 * preserve command/operand boundaries: erase launches at the fourth falling
 * clock of the split NOP, with SDATA low throughout the following wait.
 * No undocumented 80h whole-chip option is emitted.
 * Table 3-2's upper-byte 20h conflicts with its own address description;
 * the partial panel is at 002000h, as shown in figure 3-2 and table 3-5.
 * Its '40 times' is hexadecimal: [2000h,3000h) contains 64 rows of 64 bytes.
 * UID/configuration and protection effects require separate qualification. */
enum lt48_xx39_erase_event_kind {
 LT48_XX39_FRAME, LT48_XX39_COMMAND, LT48_XX39_OPERAND, LT48_XX39_WAIT_US
};
typedef struct {unsigned kind; uint32_t command, value;} lt48_xx39_erase_event;
typedef int (*lt48_xx39_erase_emit)(void *,const lt48_xx39_erase_event *);
typedef struct {lt48_xx39_erase_emit emit;void *context;int status;} lt48_xx39_erase_writer;
static inline void lt48_xx39_erase_put(lt48_xx39_erase_writer *w,unsigned kind,
 unsigned command,uint32_t value) {
 if(!w->status){const lt48_xx39_erase_event e={kind,command,value};w->status=w->emit(w->context,&e);}
}
static inline void lt48_xx39_erase_instruction(lt48_xx39_erase_writer *w,uint16_t opcode) {
 lt48_xx39_erase_put(w,LT48_XX39_FRAME,0,opcode);
}
static inline void lt48_xx39_erase_pointer(lt48_xx39_erase_writer *w,uint32_t address) {
 const uint16_t registers[3]={0x6ef8,0x6ef7,0x6ef6};
 for(unsigned i=0;i<3;i++){
  lt48_xx39_erase_instruction(w,(uint16_t)(0x0e00|((address>>(16-8*i))&255)));
  lt48_xx39_erase_instruction(w,registers[i]);
 }
}
static inline void lt48_xx39_erase_wait(lt48_xx39_erase_writer *w) {
 lt48_xx39_erase_put(w,LT48_XX39_COMMAND,0,0);
 /* P11=10ms and P10=5us. Conservative 11ms, not a voltage sweep. */
 lt48_xx39_erase_put(w,LT48_XX39_WAIT_US,0,11000);
 lt48_xx39_erase_put(w,LT48_XX39_OPERAND,0,0);
}
static inline void lt48_xx39_erase_bulk(lt48_xx39_erase_writer *w,unsigned selector) {
 lt48_xx39_erase_pointer(w,0x3c0004);
 lt48_xx39_erase_put(w,LT48_XX39_FRAME,12,selector);
 lt48_xx39_erase_instruction(w,0);
 lt48_xx39_erase_wait(w);
}
static inline int lt48_pic18_xx39_erase_plan(uint32_t code_bytes,
 lt48_xx39_erase_emit emit,void *context) {
 if(!emit||(code_bytes!=12288&&code_bytes!=24576))return -1;
 lt48_xx39_erase_writer w={emit,context,0};
 lt48_xx39_erase_bulk(&w,0x83); /* Boot block. */
 lt48_xx39_erase_bulk(&w,0x88); /* First full 8KiB panel. */
 if(code_bytes==24576){
  lt48_xx39_erase_bulk(&w,0x89);
  lt48_xx39_erase_bulk(&w,0x8a);
 }else{
  lt48_xx39_erase_pointer(&w,0x3c0006);
  lt48_xx39_erase_put(&w,LT48_XX39_FRAME,12,0); /* Single panel mode. */
  lt48_xx39_erase_instruction(&w,0x8ea6); /* EEPGD=1. */
  lt48_xx39_erase_instruction(&w,0x9ca6); /* CFGS=0. */
  for(uint32_t address=0x2000;address<code_bytes&&!w.status;address+=64){
   lt48_xx39_erase_pointer(&w,address);
   static const uint16_t unlock[]={0x84a6,0x88a6,0x0e55,0x6ea7,0x0eaa,0x6ea7,0x82a6};
   for(unsigned i=0;i<sizeof(unlock)/sizeof(*unlock);i++)lt48_xx39_erase_instruction(&w,unlock[i]);
   lt48_xx39_erase_wait(&w);
   lt48_xx39_erase_instruction(&w,0x94a6); /* WREN=0. */
  }
 }
 lt48_xx39_erase_bulk(&w,0x81); /* Table3-1: data EEPROM, independently erased. */
 return w.status;
}
#endif
