#include "configuration.h"
static const struct mw6_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}mw6_profiles[]={
{41,140,60,36,65536,0,330,6,25062,1,{65536},{0},{0},3,12,40},
{41,141,61,37,65536,0,330,6,25062,1,{65536},{0},{0},3,12,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}mw6_members[]={
{41,140,"MEGAWIN","MPC89L516X2 "},
{41,141,"MEGAWIN","MPC89L516X2F *44Q"},
{41,141,"MEGAWIN","MPC89L516X2P *44"},
{41,140,"MEGAWIN","MPC89L556X2 "},
{41,141,"MEGAWIN","MPC89L556X2F *44Q"},
{41,141,"MEGAWIN","MPC89L556X2P *44"},
};
