#include "configuration.h"
static const struct wt8_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}wt8_profiles[]={
{36,313,191,123,16384,64,330,0,0,1,{16384},{0},{0},3,10,44},
{36,315,191,123,65536,64,330,0,0,1,{65536},{0},{0},3,10,44},
{36,316,192,124,49152,128,330,0,0,1,{49152},{0},{0},3,10,48},
{36,317,192,124,65536,128,330,0,0,1,{65536},{0},{0},3,10,48},
{36,314,191,123,32768,64,330,0,0,1,{32768},{0},{0},3,10,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}wt8_members[]={
{36,313,"Weltrend","WT61P7 *44TQ"},
{36,315,"Weltrend","WT61P8 *44TQ"},
{36,316,"Weltrend","WT61P802 @48LQ"},
{36,316,"Weltrend","WT61P803 @48LQ"},
{36,316,"Weltrend","WT61P805 @48LQ"},
{36,317,"Weltrend","WT61P806 @48LQ"},
{36,314,"Weltrend","WT61P8S *44TQ"},
{36,316,"Weltrend","WT69P803 @48LQ"},
};
