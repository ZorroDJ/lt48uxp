#include "configuration.h"
static const struct ab11_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}ab11_profiles[]={
{37,0,0,0,4096,0,500,4,25308,1,{4096},{0},{0},3,14,16},
{37,1,1,1,16384,0,500,4,25308,1,{16384},{0},{0},3,14,20},
{37,3,3,3,16384,0,500,4,25308,1,{16384},{0},{0},3,14,28},
{37,2,2,2,16384,0,500,4,25308,1,{16384},{0},{0},3,14,24},
{37,4,4,4,32768,0,500,4,25308,1,{32768},{0},{0},3,14,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}ab11_members[]={
{37,0,"ABOV","MC81F4204M *16SO"},
{37,0,"ABOV","MC81F4204V"},
{37,1,"ABOV","MC81F4215B"},
{37,1,"ABOV","MC81F4215D *20SO"},
{37,1,"ABOV","MC81F4216B"},
{37,1,"ABOV","MC81F4216D *20SO"},
{37,3,"ABOV","MC81F4315M *28SO"},
{37,2,"ABOV","MC81F4315S *24SS"},
{37,3,"ABOV","MC81F4316M *28SO"},
{37,2,"ABOV","MC81F4316S *24SS"},
{37,4,"ABOV","MC81F4432Q *44Q"},
};
