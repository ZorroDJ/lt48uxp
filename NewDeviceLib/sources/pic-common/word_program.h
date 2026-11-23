#ifndef LT48_PIC_COMMON_WORD_PROGRAM_H
#define LT48_PIC_COMMON_WORD_PROGRAM_H
#include <labtool48/labtool48.h>
#include <string.h>

/* Authored MSB ICSP single-word strategy, DS40001838C figures 3-19/3-21.
 * Addresses come from shared host buffer 2, data from buffer 6. Explicit
 * Load-PC and a commit for every word avoid inheriting a row accumulator.
 * This does not admit a source profile or implement electrical entry/exit.
 * Timing is an audited family variable, not an arbitrary fixed delay. */
static inline lt48_status lt48_pic_word_program_compile(
    const lt48_pin_map *pins,const lt48_compile_variable *variables,size_t count,
    unsigned data_pin,unsigned clock_pin,unsigned timing_variable,unsigned protocol,
    lt48_compiled_wave **output) {
    if(!pins || !output || data_pin>255 || clock_pin>255 || timing_variable>255 || protocol>1)
        return LT48_INVALID_ARGUMENT;
    *output=NULL;unsigned char raw[128]={0};size_t used=2;
    int found=0;
    if(!variables || data_pin==clock_pin)return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<count;i++)if(variables[i].variable_id==timing_variable)found++;
    if(found!=1)return LT48_SAFETY_REJECTED;
#define BYTE(v) raw[used++]=(unsigned char)(v)
#define COMMAND(v) do {BYTE(0x1d);BYTE(data_pin);BYTE(clock_pin);BYTE(v);BYTE(0);BYTE(0);BYTE(0);BYTE(8);BYTE(1);}while(0)
#define CLEAR() do {BYTE(0x40);BYTE(12);BYTE(4);BYTE(0);BYTE(0);BYTE(0);BYTE(0);}while(0)
#define PACK(buffer,bits) do {BYTE(0x45);BYTE(12);BYTE(1);BYTE(buffer);BYTE(0);BYTE(bits);}while(0)
#define PAYLOAD() do {BYTE(0x1c);BYTE(data_pin);BYTE(clock_pin);BYTE(12);BYTE(24);BYTE(1);}while(0)
    if(protocol==1) {
        /* DS40001738D: six-bit LSB commands; Load-PC has a 22-bit
         * payload, NVM data has 14 bits, each bounded by zero start/stop.
         * Buffer 2 contains the absolute PC; buffer 6 contains one word. */
#define LSB_COMMAND(v,bits) do {BYTE(0x1a);BYTE(data_pin);BYTE(clock_pin);BYTE(v);BYTE(0);BYTE(0);BYTE(0);BYTE(bits);BYTE(1);}while(0)
#define LSB_PACK(buffer,bits) do {BYTE(0x45);BYTE(12);BYTE(0);BYTE(buffer);BYTE(0);BYTE(bits);}while(0)
#define LSB_PAYLOAD(bits) do {BYTE(0x19);BYTE(data_pin);BYTE(clock_pin);BYTE(12);BYTE(bits);BYTE(1);}while(0)
        LSB_COMMAND(29,6);LSB_COMMAND(0,1);CLEAR();LSB_PACK(2,16);LSB_PAYLOAD(22);LSB_COMMAND(0,1);
        LSB_COMMAND(2,6);LSB_COMMAND(0,1);CLEAR();LSB_PACK(6,14);LSB_PAYLOAD(14);LSB_COMMAND(0,1);
        LSB_COMMAND(8,6);
#undef LSB_PAYLOAD
#undef LSB_PACK
#undef LSB_COMMAND
    } else {
    COMMAND(0x80);CLEAR();PACK(2,16);PAYLOAD();
    COMMAND(0);CLEAR();PACK(6,14);PAYLOAD();COMMAND(0xe0);
    }
    BYTE(0x31);BYTE(timing_variable);
    raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
    lt48_compact_wave *compact=NULL;
    lt48_status st=lt48_compact_decode(raw,used,&compact);
    if(st==LT48_OK)st=lt48_compact_compile(compact,pins,variables,count,1,0,output);
    lt48_compact_wave_close(compact);
    if(st==LT48_OK && lt48_compiled_wave_defaulted_pin_count(*output)) {
        lt48_compiled_wave_close(*output);*output=NULL;st=LT48_SAFETY_REJECTED;
    }
    return st;
#undef PAYLOAD
#undef PACK
#undef CLEAR
#undef COMMAND
#undef BYTE
}
/* Compatibility wrapper keeps the existing MSB callers byte-identical. */
static inline lt48_status lt48_pic_msb_word_program_compile(
    const lt48_pin_map *pins,const lt48_compile_variable *variables,size_t count,
    unsigned data_pin,unsigned clock_pin,unsigned timing_variable,
    lt48_compiled_wave **output) {
    return lt48_pic_word_program_compile(pins,variables,count,data_pin,clock_pin,timing_variable,0,output);
}
#endif
