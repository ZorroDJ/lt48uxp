#include "configuration.h"
static const struct sc4g_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}sc4g_profiles[]={
{49,309,221,96,8192,0,500,17,25270,1,{8192},{0},{0},2,18,40},
{49,310,222,97,8192,0,500,17,25270,1,{8192},{0},{0},2,18,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}sc4g_members[]={
{49,309,"SyncMos","SM59R02G1"},
{49,310,"SyncMos","SM59R02G1 *44"},
{49,310,"SyncMos","SM59R02G1 *44Q"},
{49,310,"SyncMos","SM59R02G1 *44TQ"},
};
