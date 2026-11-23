/* Generated staged OTP14 membership; four unresolved packages excluded. */
#ifndef LT48_PIC_OTP14_PROFILES_H
#define LT48_PIC_OTP14_PROFILES_H
#ifdef LT48_ADMISSION_TRIAL_HEADER
#include LT48_ADMISSION_TRIAL_HEADER
#endif
#ifndef LT48_PIC_OTP14_ENABLED
#define LT48_PIC_OTP14_ENABLED 1
#endif
typedef struct {
 unsigned definition,family,pinmap,words,configuration_mask,protection_mask;
 const char *manufacturer,*device,*adapter_note;
} lt48_pic_otp14_profile;
#if LT48_PIC_OTP14_ENABLED
static const lt48_pic_otp14_profile pic_otp14_profiles[]={
 {400,276,102,1024,31,16,"Microchip","PIC16C61",NULL},
 {385,268,99,2048,63,48,"Microchip","PIC16C62",NULL},
 {395,272,101,512,16255,16176,"Microchip","PIC16C620",NULL},
 {395,272,101,512,16255,16176,"Microchip","PIC16C620A",NULL},
 {396,273,101,1024,16255,16176,"Microchip","PIC16C621",NULL},
 {396,273,101,1024,16255,16176,"Microchip","PIC16C621A",NULL},
 {397,274,101,2048,16255,16176,"Microchip","PIC16C622",NULL},
 {397,274,101,2048,16255,16176,"Microchip","PIC16C622A",NULL},
 {403,277,99,2048,16255,16176,"Microchip","PIC16C62A",NULL},
 {403,277,99,2048,16255,16176,"Microchip","PIC16C62B",NULL},
 {404,278,99,4096,16255,16176,"Microchip","PIC16C63",NULL},
 {404,278,99,4096,16255,16176,"Microchip","PIC16C63A",NULL},
 {386,268,97,2048,63,48,"Microchip","PIC16C64",NULL},
 {387,270,98,2048,63,48,"Microchip","PIC16C64 *44",NULL},
 {388,270,100,2048,63,48,"Microchip","PIC16C64 *44Q","Requires a rotated 44-pin adapter: chip pin 1 to DIP pin 7. VDD 7/28 to DIP 13/34; GND 6/29 to DIP 12/35; MCLR 18 to DIP 24; CLK 16 to DIP 22; data 17 to DIP 23. A straight 1:1 adapter is incompatible."},
 {401,277,97,2048,16255,16176,"Microchip","PIC16C64A",NULL},
 {402,277,98,2048,16255,16176,"Microchip","PIC16C64A *44",NULL},
 {389,269,97,4096,63,48,"Microchip","PIC16C65",NULL},
 {390,269,98,4096,63,48,"Microchip","PIC16C65 *44",NULL},
 {405,278,97,4096,16255,16176,"Microchip","PIC16C65A",NULL},
 {406,278,98,4096,16255,16176,"Microchip","PIC16C65A *44",NULL},
 {407,278,100,4096,16255,16176,"Microchip","PIC16C65A *44Q","Requires a rotated 44-pin adapter: chip pin 1 to DIP pin 7. VDD 7/28 to DIP 13/34; GND 6/29 to DIP 12/35; MCLR 18 to DIP 24; CLK 16 to DIP 22; data 17 to DIP 23. A straight 1:1 adapter is incompatible."},
 {405,278,97,4096,16255,16176,"Microchip","PIC16C65B",NULL},
 {406,278,98,4096,16255,16176,"Microchip","PIC16C65B *44",NULL},
 {408,279,99,8192,16255,16176,"Microchip","PIC16C66",NULL},
 {409,279,97,8192,16255,16176,"Microchip","PIC16C67",NULL},
 {410,279,98,8192,16255,16176,"Microchip","PIC16C67 *44",NULL},
 {411,279,100,8192,16255,16176,"Microchip","PIC16C67 *44Q","Requires a rotated 44-pin adapter: chip pin 1 to DIP pin 7. VDD 7/28 to DIP 13/34; GND 6/29 to DIP 12/35; MCLR 18 to DIP 24; CLK 16 to DIP 22; data 17 to DIP 23. A straight 1:1 adapter is incompatible."},
 {400,276,102,1024,31,16,"Microchip","PIC16C71",NULL},
 {413,280,101,512,16383,16304,"Microchip","PIC16C710",NULL},
 {414,280,103,512,16383,16304,"Microchip","PIC16C710 *20SSOP",NULL},
 {415,281,101,1024,16383,16304,"Microchip","PIC16C711",NULL},
 {416,281,103,1024,16383,16304,"Microchip","PIC16C711 *20SSOP",NULL},
 {398,275,101,1024,16255,16176,"Microchip","PIC16C712",NULL},
 {399,274,101,2048,16255,16176,"Microchip","PIC16C716",NULL},
 {403,277,99,2048,16255,16176,"Microchip","PIC16C72",NULL},
 {403,277,99,2048,16255,16176,"Microchip","PIC16C72A",NULL},
 {391,271,99,4096,63,48,"Microchip","PIC16C73",NULL},
 {404,278,99,4096,16255,16176,"Microchip","PIC16C73A",NULL},
 {404,278,99,4096,16255,16176,"Microchip","PIC16C73B",NULL},
 {393,271,97,4096,63,48,"Microchip","PIC16C74",NULL},
 {392,271,98,4096,63,48,"Microchip","PIC16C74 *44",NULL},
 {394,271,100,4096,63,48,"Microchip","PIC16C74 *44Q","Requires a rotated 44-pin adapter: chip pin 1 to DIP pin 7. VDD 7/28 to DIP 13/34; GND 6/29 to DIP 12/35; MCLR 18 to DIP 24; CLK 16 to DIP 22; data 17 to DIP 23. A straight 1:1 adapter is incompatible."},
 {408,279,99,8192,16255,16176,"Microchip","PIC16C76",NULL},
 {409,279,97,8192,16255,16176,"Microchip","PIC16C77",NULL},
 {410,279,98,8192,16255,16176,"Microchip","PIC16C77 *44",NULL},
 {412,279,100,8192,16255,16176,"Microchip","PIC16C77 *44Q","Requires a rotated 44-pin adapter: chip pin 1 to DIP pin 7. VDD 7/28 to DIP 13/34; GND 6/29 to DIP 12/35; MCLR 18 to DIP 24; CLK 16 to DIP 22; data 17 to DIP 23. A straight 1:1 adapter is incompatible."},
 {420,284,106,4096,16255,13104,"Microchip","PIC16C774",NULL},
 {421,284,107,4096,16255,13104,"Microchip","PIC16C774 *44",NULL},
 {422,284,108,4096,16255,13104,"Microchip","PIC16C774 *44TQ","Requires a rotated 44-pin adapter: chip pin 1 to DIP pin 7. VDD 7/28 to DIP 13/34; GND 6/29 to DIP 12/35; MCLR 18 to DIP 24; CLK 16 to DIP 22; data 17 to DIP 23. A straight 1:1 adapter is incompatible."},
 {415,281,101,1024,16383,16304,"Microchip","PIC16LC711",NULL},
 {416,281,103,1024,16383,16304,"Microchip","PIC16LC711 *20SSOP",NULL},
 {419,283,99,4096,16255,13104,"Microchip","PIC16C773",NULL},
 {419,283,99,4096,16255,13104,"Microchip","PIC16LC773",NULL},
 {418,282,105,4096,16191,16176,"Microchip","PIC16C923 @64TQ",NULL},
 {417,282,104,4096,16191,16176,"Microchip","PIC16C923 @68",NULL},
 {417,282,104,4096,16191,16176,"Microchip","PIC16C924 @68",NULL},
};
#endif
#endif
