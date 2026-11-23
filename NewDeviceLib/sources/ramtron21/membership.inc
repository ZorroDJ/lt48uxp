#include "admission.h"
static const struct{unsigned d;const char*vendor,*name;}r21_names[]={
#include "names.inc"
};
int lt48_ramtron21_member_internal(const lt48_catalogue_entry*e){
 if(!R21_ADMITTED||!e||e->device_class!=2038||!e->vendor)return 0;
 for(size_t i=0;i<sizeof r21_names/sizeof*r21_names;i++)if(e->device_type==r21_names[i].d&&!strcmp(e->vendor,r21_names[i].vendor)&&e->device_name_length==strlen(r21_names[i].name)&&!memcmp(e->device_name,r21_names[i].name,e->device_name_length))return 1;
 return 0;
}
