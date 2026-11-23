#include "configuration.h"
static const struct sm3_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}sm3_profiles[]={
{32,180,81,61,4096,0,500,1,25047,1,{4096},{0},{0},5,11,40},
{32,181,82,62,4096,0,500,1,25047,1,{4096},{0},{0},5,11,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}sm3_members[]={
{32,180,"SyncMos","SM8951B"},
{32,181,"SyncMos","SM8951B *44"},
{32,181,"SyncMos","SM8951B *44Q"},
};
