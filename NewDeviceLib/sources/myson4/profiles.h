#include "configuration.h"
static const struct my4_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}my4_profiles[]={
{44,292,173,107,65536,0,320,1,25047,1,{65536},{0},{0},3,12,44},
{44,294,175,108,131072,0,330,1,25047,1,{131072},{0},{0},3,12,44},
{44,293,174,109,65536,0,330,0,0,1,{65536},{0},{0},3,10,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}my4_members[]={
{44,292,"MYSON","MTV312M64 *44"},
{44,294,"MYSON","MTV416M *44"},
{44,294,"MYSON","MTV416M *44Q"},
{44,293,"MYSON","MTV512M *44"},
};
