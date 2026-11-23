/* Original class2044 stacked-die SPI, exact BYO bindings. */
#include "admission.h"
static int spiw6_binding(const lt48_catalogue_entry*e){
 if(!e->vendor)return 0;
 static const struct{const char*vendor,*name;unsigned cls,d,man,id;}rows[]={
{"Macronix","MX25L25835E *16SO",2044,307,194,8216},
{"Macronix","MX25L25855E @24TFBGA",2044,308,194,9752},
{"SPANSION","S70FL256P-BF-20",2044,232,1,24},
{"SPANSION","S70FL256P-BF-21",2044,234,1,24},
{"SPANSION","S70FL256P-MF-00",2044,231,1,24},
{"SPANSION","S70FL256P-MF-01",2044,233,1,24},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(e->device_class==rows[i].cls&&e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&!strcmp(e->vendor,rows[i].vendor)&&e->device_name_length==strlen(rows[i].name)&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;return 0;
}
