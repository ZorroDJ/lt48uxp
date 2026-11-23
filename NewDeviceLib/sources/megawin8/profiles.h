#include "configuration.h"
static const struct mw8_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}mw8_profiles[]={
{42,365,238,148,4096,0,500,13,25340,1,{4096},{0},{0},3,15,16},
{42,366,239,149,4096,0,500,13,25340,1,{4096},{0},{0},3,15,20},
{42,363,236,148,4096,0,320,13,25340,1,{4096},{0},{0},3,15,16},
{42,364,237,149,4096,0,320,13,25340,1,{4096},{0},{0},3,15,20},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}mw8_members[]={
{42,365,"MEGAWIN","MG86FE104 *16DIP"},
{42,365,"MEGAWIN","MG86FE104 *16SO"},
{42,366,"MEGAWIN","MG86FE104 *20DIP"},
{42,366,"MEGAWIN","MG86FE104 *20SO"},
{42,363,"MEGAWIN","MG86FL104 *16DIP"},
{42,363,"MEGAWIN","MG86FL104 *16SO"},
{42,364,"MEGAWIN","MG86FL104 *20DIP"},
{42,364,"MEGAWIN","MG86FL104 *20SO"},
};
