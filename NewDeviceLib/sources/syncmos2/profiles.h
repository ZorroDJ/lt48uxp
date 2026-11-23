#include "configuration.h"
static const struct sc2_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}sc2_profiles[]={
{47,306,186,119,4096,0,500,1,25047,1,{4096},{0},{0},5,12,20},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}sc2_members[]={
{47,306,"SyncMos","SM894051"},
{47,306,"SyncMos","SM894051 *20SO"},
};
