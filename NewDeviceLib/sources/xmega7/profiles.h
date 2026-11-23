/* Authored finite bindings; original payloads remain bring-your-own. */
struct x7_profile {unsigned definition,family,pinmap,capacity,configuration,fields;unsigned length[4],base[4];};
static const struct x7_profile x7_profiles[]={
{621,467,227,21760,25264,9,{16384,4096,1024,256},{8388608,8404992,9175040,9307136}},
{624,470,229,21120,25353,10,{16384,4096,512,128},{8388608,8404992,9175040,9307136}},
{622,468,227,38144,25264,9,{32768,4096,1024,256},{8388608,8421376,9175040,9307136}},
{625,471,229,38016,25353,10,{32768,4096,1024,128},{8388608,8421376,9175040,9307136}},
{623,469,229,10880,25353,10,{8192,2048,512,128},{8388608,8396800,9175040,9307136}},
};
static const struct{unsigned definition,manufacturer,device;const char*vendor,*name;}x7_members[]={
{621,30,16788,"Atmel","ATxmega16A4 *44TQ"},
{621,30,17044,"Atmel","ATxmega16D4 *44TQ"},
{624,30,17812,"Atmel","ATxmega16E5*32QFN 5x5"},
{622,30,16789,"Atmel","ATxmega32A4 *44TQ"},
{622,30,17045,"Atmel","ATxmega32D4 *44TQ"},
{625,30,19605,"Atmel","ATxmega32E5*32QFN 5x5"},
{623,30,16787,"Atmel","ATxmega8E5 *32QFN 5x5"},
};
