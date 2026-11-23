#include "configuration.h"
static const struct sn9_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}sn9_profiles[]={
#include "../snas4/profiles.inc"
#include "../cion2/profiles.inc"
{30,307,187,120,65536,0,330,1,25047,1,{65536},{0},{0},2,12,44},
{30,308,188,121,65536,0,330,1,25047,1,{65536},{0},{0},2,12,48},
{30,309,189,120,65536,0,500,1,25047,1,{65536},{0},{0},2,12,44},
{30,310,190,121,65536,0,500,1,25047,1,{65536},{0},{0},2,12,48},
{30,311,189,120,16384,0,500,1,25047,1,{16384},{0},{0},2,12,44},
{30,312,190,121,16384,0,500,1,25047,1,{16384},{0},{0},2,12,48},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}sn9_members[]={
#include "../snas4/members.inc"
#include "../cion2/members.inc"
{30,307,"Syntek","STK6031 *44"},
{30,307,"Syntek","STK6031 *44Q"},
{30,308,"Syntek","STK6031 *48LQ"},
{30,309,"Syntek","STK6032 *44"},
{30,309,"Syntek","STK6032 *44Q"},
{30,310,"Syntek","STK6032 *48LQ"},
{30,311,"Syntek","STK6033 *44"},
{30,311,"Syntek","STK6033 *44Q"},
{30,312,"Syntek","STK6033 *48LQ"},
};
