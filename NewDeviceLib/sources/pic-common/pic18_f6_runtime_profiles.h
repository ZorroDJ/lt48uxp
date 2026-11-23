#ifndef LT48_PIC18_F6_RUNTIME_PROFILES_H
#define LT48_PIC18_F6_RUNTIME_PROFILES_H
typedef struct{uint32_t definition;uint16_t family,pinmap;unsigned modern;const char *name;unsigned char erase_mask[14];} lt48_pic18_f6_runtime_policy;
static const lt48_pic18_f6_runtime_policy lt48_pic18_f6_runtime_profiles[]={
{39,28,15,0,"PIC18F6525 @64TQ",{0,0,0,0,0,0,0,0,7,192,7,224,7,64}},
{128,111,51,1,"PIC18F6527 @64TQ",{0,0,0,0,0,0,0,0,7,192,7,224,7,64}},
{41,30,16,0,"PIC18F6585 @64TQ",{0,0,0,0,0,0,0,0,7,192,7,224,7,64}},
{40,29,15,0,"PIC18F6621 @64TQ",{0,0,0,0,0,0,0,0,15,192,15,224,15,64}},
{129,112,51,1,"PIC18F6622 @64TQ",{0,0,0,0,0,0,0,0,15,192,15,224,15,64}},
{130,113,51,1,"PIC18F6627 @64TQ",{0,0,0,0,0,0,0,0,63,192,63,224,63,64}},
{42,31,16,0,"PIC18F6680 @64TQ",{0,0,0,0,0,0,0,0,15,192,15,224,15,64}},
{131,114,51,1,"PIC18F6722 @64TQ",{0,0,0,0,0,0,0,0,255,192,255,224,255,64}},
};
#endif
