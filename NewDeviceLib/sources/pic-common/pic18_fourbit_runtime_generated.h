/* Generated protocol policy; no device-support admission. */
#ifndef LT48_PIC18_FOURBIT_RUNTIME_GENERATED_H
#define LT48_PIC18_FOURBIT_RUNTIME_GENERATED_H
typedef struct { uint32_t definition; uint16_t family,pinmap; uint32_t erase_us; uint16_t source_erase_magnitude; } lt48_pic18_fourbit_runtime_policy;
static const lt48_pic18_fourbit_runtime_policy lt48_pic18_fourbit_runtime_policies[] = {
    {525,376,197,10200,100},
    {526,377,198,10200,100},
    {527,378,199,10200,100},
    {528,379,200,10200,100},
    {529,380,197,10200,100},
    {530,381,198,10200,100},
    {531,382,199,10200,100},
    {532,383,200,10200,100},
    {533,384,197,10200,100},
    {534,385,198,10200,100},
    {535,386,199,10200,100},
    {536,387,200,10200,100},
    {537,388,197,10200,100},
    {538,389,198,10200,100},
    {539,390,199,10200,100},
    {540,391,200,10200,100},
    {559,406,205,12200,100},
    {560,407,206,12200,100},
    {561,408,207,12200,100},
    {562,409,205,12200,100},
    {563,410,206,12200,100},
    {564,411,207,12200,100},
    {565,412,205,15200,100},
    {566,413,206,15200,100},
    {567,414,207,15200,100},
    {568,415,205,15200,100},
    {569,416,206,15200,100},
    {570,417,207,15200,100},
    {587,434,212,12200,160},
    {588,435,212,15200,160},
    {589,436,213,15200,160},
    {590,437,214,15200,160},
};
static inline const lt48_pic18_fourbit_runtime_policy *lt48_pic18_fourbit_runtime_find(uint32_t d,uint16_t f,uint16_t p) {
    for(size_t i=0;i<sizeof(lt48_pic18_fourbit_runtime_policies)/sizeof(*lt48_pic18_fourbit_runtime_policies);i++) {
        const lt48_pic18_fourbit_runtime_policy *r=lt48_pic18_fourbit_runtime_policies+i;
        if(r->definition==d&&r->family==f&&r->pinmap==p)return r;
    } return NULL;
}
static const unsigned char lt48_pic18_fourbit_program_original_sha[32]={0x41,0x5e,0xba,0xd8,0x53,0xba,0x49,0xce,0xcf,0xd3,0x09,0xab,0x83,0x99,0xf4,0x99,0x1d,0x22,0xf8,0xc8,0xdb,0x1c,0x09,0xd2,0xb5,0xcc,0x64,0x9b,0x6f,0x29,0x91,0x78};
static const unsigned char lt48_pic18_fourbit_program_patched_sha[32]={0x16,0x47,0xc4,0xb2,0x6c,0xa7,0x98,0xaa,0x74,0x9e,0xbe,0x6f,0xa4,0x69,0x7a,0x25,0x9d,0x1c,0xa0,0xb3,0x3c,0xa2,0x0c,0x8b,0xc9,0xa4,0xa9,0x0a,0x26,0xb6,0x9a,0x1c};
static const unsigned char lt48_pic18_fourbit_configuration_original_sha[32]={0x7f,0x9a,0xeb,0x21,0x0e,0x26,0xf2,0x8a,0xa1,0xad,0xc0,0x99,0x69,0x5b,0x58,0x1a,0xa7,0xae,0xa2,0xe0,0x9e,0x6b,0xfa,0x64,0xbc,0x81,0xda,0x29,0x5a,0xab,0x3e,0x52};
static const unsigned char lt48_pic18_fourbit_configuration_patched_sha[32]={0x59,0x42,0x2a,0x59,0x4b,0xf8,0x98,0xd6,0xd4,0x78,0x28,0xfc,0x31,0x33,0xfb,0xe4,0x9b,0x95,0x85,0xe1,0x30,0x28,0xba,0xeb,0x21,0x03,0xdb,0x50,0x71,0x64,0x96,0x32};
#endif
