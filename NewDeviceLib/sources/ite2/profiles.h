#include "admission.h"
static const struct ite2_profile{unsigned definition,family,pinmap,capacity,pins,variables,operations;}ite2_profiles[]={
{0,0,0,131072,48,3,14},
{1,1,0,262144,48,3,14},
};
static inline const struct ite2_profile*ite2_find(unsigned cls,unsigned d){if(cls!=2040)return NULL;for(size_t i=0;i<sizeof ite2_profiles/sizeof*ite2_profiles;i++)if(ite2_profiles[i].definition==d)return ite2_profiles+i;return NULL;}
static inline int ite2_member(const lt48_catalogue_entry*e){if(!e||!LT48_ITE2_ADMITTED||e->device_class!=2040||strcmp(e->vendor,"ITE"))return 0;const char*n=e->device_type==0?"IT6350-128 @48LQ":e->device_type==1?"IT6350-256 @48LQ":"";return strlen(n)==e->device_name_length&&!memcmp(n,e->device_name,e->device_name_length);}
