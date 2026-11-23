#include "configuration.h"
static const struct wt10_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}wt10_profiles[]={
{34,319,194,126,8192,64,330,0,0,1,{8192},{0},{0},3,10,16},
{34,320,195,127,8192,64,330,0,0,1,{8192},{0},{0},3,10,20},
{34,321,196,128,8192,64,330,0,0,1,{8192},{0},{0},3,10,24},
{34,322,197,129,8192,64,330,0,0,1,{8192},{0},{0},3,10,28},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}wt10_members[]={
{34,319,"Weltrend","WT6702F *16SO"},
{34,320,"Weltrend","WT6702F *20SS"},
{34,321,"Weltrend","WT6702F *24SS"},
{34,319,"Weltrend","WT6703F *16SO"},
{34,320,"Weltrend","WT6703F *20SS"},
{34,321,"Weltrend","WT6703F *24SS"},
{34,319,"Weltrend","WT69P3 *16SO"},
{34,320,"Weltrend","WT69P3 *20SS"},
{34,321,"Weltrend","WT69P3 *24SS"},
{34,322,"Weltrend","WT69P3 *28SS"},
};
