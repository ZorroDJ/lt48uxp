#include "configuration.h"
static const struct sc4a_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}sc4a_profiles[]={
{48,211,104,69,32768,0,500,4,25187,1,{32768},{0},{0},3,16,40},
{48,210,103,69,32768,0,330,4,25187,1,{32768},{0},{0},3,16,40},
{48,213,104,69,65536,0,500,4,25187,1,{65536},{0},{0},3,16,40},
{48,212,103,69,65536,0,330,4,25187,1,{65536},{0},{0},3,16,40},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}sc4a_members[]={
{48,211,"SyncMos","SM59R08A2C @64TQ"},
{48,210,"SyncMos","SM59R08A2L @64TQ"},
{48,213,"SyncMos","SM59R16A2C @64TQ"},
{48,212,"SyncMos","SM59R16A2L @64TQ"},
};
