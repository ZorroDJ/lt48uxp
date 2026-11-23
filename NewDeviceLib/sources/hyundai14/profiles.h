#include "configuration.h"
static const struct hy14_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}hy14_profiles[]={
{39,250,133,85,20480,0,500,1,25047,1,{20480},{0},{0},7,11,20},
{39,251,133,86,20480,0,500,1,25047,1,{20480},{0},{0},7,11,24},
{39,252,134,87,20480,0,500,1,25047,1,{20480},{0},{0},7,11,28},
{39,253,135,85,32768,0,500,1,25047,1,{32768},{0},{0},7,11,20},
{39,254,135,86,32768,0,500,1,25047,1,{32768},{0},{0},7,11,24},
{39,255,136,87,32768,0,500,1,25047,1,{32768},{0},{0},7,11,28},
{39,256,137,88,4096,0,500,7,25048,1,{4096},{0},{0},5,11,28},
{39,257,138,88,8192,0,500,7,25048,1,{8192},{0},{0},5,11,28},
{39,258,139,88,16384,0,500,7,25048,1,{16384},{0},{0},5,11,28},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}hy14_members[]={
{39,250,"Hyundai(Hynix)","HMS81020ET *20DIP"},
{39,250,"Hyundai(Hynix)","HMS81020ET *20SO"},
{39,251,"Hyundai(Hynix)","HMS81020ET *24SO"},
{39,252,"Hyundai(Hynix)","HMS81020ET *28SO"},
{39,253,"Hyundai(Hynix)","HMS81032ET *20DIP"},
{39,253,"Hyundai(Hynix)","HMS81032ET *20SO"},
{39,254,"Hyundai(Hynix)","HMS81032ET *24SO"},
{39,255,"Hyundai(Hynix)","HMS81032ET *28SO"},
{39,256,"Hyundai(Hynix)","HMS87C1404B *28DIP"},
{39,256,"Hyundai(Hynix)","HMS87C1404B *28SO"},
{39,257,"Hyundai(Hynix)","HMS87C1408B *28DIP"},
{39,257,"Hyundai(Hynix)","HMS87C1408B *28SO"},
{39,258,"Hyundai(Hynix)","HMS87C1416B *28DIP"},
{39,258,"Hyundai(Hynix)","HMS87C1416B *28SO"},
};
