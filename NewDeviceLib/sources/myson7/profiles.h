#include "configuration.h"
static const struct my7_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}my7_profiles[]={
{43,288,169,103,65536,0,500,0,0,1,{65536},{0},{0},3,10,42},
{43,289,170,104,65536,0,500,0,0,1,{65536},{0},{0},3,10,44},
{43,286,167,101,65536,0,500,0,0,1,{65536},{0},{0},3,10,40},
{43,287,168,102,65536,0,500,0,0,1,{65536},{0},{0},3,10,40},
{43,290,171,105,131072,0,310,0,0,1,{131072},{0},{0},3,10,42},
{43,291,172,106,131072,0,310,0,0,1,{131072},{0},{0},3,10,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}my7_members[]={
{43,288,"MYSON","MTV212M64 *42SDIP"},
{43,289,"MYSON","MTV212M64 *44"},
{43,286,"MYSON","MTV212M64(Non-USB)"},
{43,287,"MYSON","MTV212M64(USB)"},
{43,290,"MYSON","MTV412M *42SDIP"},
{43,291,"MYSON","MTV412M *44"},
{43,291,"MYSON","MTV412M *44Q"},
};
