#include "configuration.h"
static const struct ob16_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}ob16_profiles[]={
{33,245,128,80,4096,0,500,14,25359,1,{4096},{0},{0},3,11,10},
{33,246,129,81,4096,0,500,14,25359,1,{4096},{0},{0},3,11,14},
{33,247,130,82,4096,0,500,14,25359,1,{4096},{0},{0},3,11,16},
{33,242,126,77,4096,0,500,4,25339,1,{4096},{0},{0},3,11,10},
{33,243,126,78,4096,0,500,4,25339,1,{4096},{0},{0},3,11,14},
{33,244,127,79,4096,0,500,4,25339,1,{4096},{0},{0},3,11,16},
{33,248,131,83,4096,0,500,14,25359,1,{4096},{0},{0},3,11,14},
{33,249,132,84,4096,0,500,14,25359,1,{4096},{0},{0},3,11,16},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}ob16_members[]={
{33,245,"On-Bright","OB37R04A1 *10DIP"},
{33,245,"On-Bright","OB37R04A1 *10SO"},
{33,246,"On-Bright","OB37R04A1 *14DIP"},
{33,246,"On-Bright","OB37R04A1 *14SO"},
{33,247,"On-Bright","OB37R04A1 *16DIP"},
{33,247,"On-Bright","OB37R04A1 *16SO"},
{33,242,"On-Bright","OB37R04G1 *10DIP"},
{33,242,"On-Bright","OB37R04G1 *10SO"},
{33,243,"On-Bright","OB37R04G1 *14DIP"},
{33,243,"On-Bright","OB37R04G1 *14SO"},
{33,244,"On-Bright","OB37R04G1 *16DIP"},
{33,244,"On-Bright","OB37R04G1 *16SO"},
{33,248,"On-Bright","OB37R259A *14DIP"},
{33,248,"On-Bright","OB37R259A *14SO"},
{33,249,"On-Bright","OB37R259A *16DIP"},
{33,249,"On-Bright","OB37R259A *16SO"},
};
