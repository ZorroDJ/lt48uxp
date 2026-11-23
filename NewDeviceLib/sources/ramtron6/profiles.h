struct r6_profile{unsigned d,m,p,cap,pins,nominal;};
static const struct r6_profile r6_profiles[]={
{172,73,55,65536,40,500},
{173,74,56,65536,44,500},
{176,77,57,131072,40,500},
{177,78,58,131072,44,500},
#include "../ramtron21/low-voltage-profiles.inc"
};
static const struct{unsigned d;const char*vendor,*name;}r6_names[]={
{172,"RAMTRON","VRS51C1000"},
{173,"RAMTRON","VRS51C1000 *44"},
{173,"RAMTRON","VRS51C1000 *44Q"},
{176,"RAMTRON","VRS51C1100"},
{177,"RAMTRON","VRS51C1100 *44"},
{177,"RAMTRON","VRS51C1100 *44Q"},
#include "../ramtron21/names.inc"
};
