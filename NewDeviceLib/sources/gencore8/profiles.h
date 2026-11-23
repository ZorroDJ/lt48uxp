#include "configuration.h"
static const struct gc8_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}gc8_profiles[]={
#include "../gc520one/profiles.inc"
{40,265,146,93,4096,0,500,0,0,1,{4096},{0},{0},3,12,14},
{40,266,147,94,4096,0,500,0,0,1,{4096},{0},{0},3,12,16},
{40,267,148,95,4096,0,500,0,0,1,{4096},{0},{0},3,12,20},
{40,264,145,92,4096,0,500,0,0,1,{4096},{0},{0},3,12,8},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}gc8_members[]={
#include "../gc520one/members.inc"
{40,265,"GENCORE","GC87C510A0 *14DIP"},
{40,265,"GENCORE","GC87C510A0 *14SO"},
{40,266,"GENCORE","GC87C510A0 *16DIP"},
{40,266,"GENCORE","GC87C510A0 *16SO"},
{40,267,"GENCORE","GC87C510A0 *20DIP"},
{40,267,"GENCORE","GC87C510A0 *20SO"},
{40,264,"GENCORE","GC87C510A0 *8DIP"},
{40,264,"GENCORE","GC87C510A0 *8SO"},
};
