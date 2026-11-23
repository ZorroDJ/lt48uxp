#include "configuration.h"
static const struct tp4_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}tp4_profiles[]={
{45,261,142,89,65536,0,380,1,25047,1,{65536},{0},{0},4,13,40},
{45,262,143,90,65536,0,380,1,25047,1,{65536},{0},{0},4,13,44},
{45,259,140,89,65536,0,330,1,25047,1,{65536},{0},{0},4,13,40},
{45,260,141,90,65536,0,330,1,25047,1,{65536},{0},{0},4,13,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}tp4_members[]={
{45,261,"TOPRO","TP2804H"},
{45,262,"TOPRO","TP2804H *44"},
{45,259,"TOPRO","TP2804L"},
{45,260,"TOPRO","TP2804L *44"},
};
