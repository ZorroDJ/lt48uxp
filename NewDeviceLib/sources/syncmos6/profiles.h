#include "configuration.h"
static const struct sm6_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}sm6_profiles[]={
{31,178,79,59,65536,0,500,1,25047,1,{65536},{0},{0},6,11,40},
{31,179,80,60,65536,0,500,1,25047,1,{65536},{0},{0},6,11,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}sm6_members[]={
{31,178,"SyncMos","SM89S16R1"},
{31,179,"SyncMos","SM89S16R1 *44"},
{31,179,"SyncMos","SM89S16R1 *44Q"},
{31,178,"SyncMos","SM89T16R1"},
{31,179,"SyncMos","SM89T16R1 *44"},
{31,179,"SyncMos","SM89T16R1 *44Q"},
};
