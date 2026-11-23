/* Authored N76E003 configuration schema; original payloads remain BYO. */
static const struct numicro_field n76one_fields[]={
{0,0,1,"Lock",{NULL}},{0,1,1,"RPD=0 (reset pin disabled)",{NULL}},{0,2,1,"OCD Enable",{NULL}},{0,3,1,"OCDPWM=0",{NULL}},{0,4,1,"CBS=0",{NULL}},{0,5,1,"CBORST=0",{NULL}},{0,6,1,"BOIAP=0",{NULL}},{0,7,1,"CBODEN=0",{NULL}},
{1,0,4,"LDROM size select",{"LDROM 4K,APROM 14K","LDROM 3K,APROM 15K","LDROM 2K,APROM 16K","LDROM 1K,APROM 17K","No LDROM,APROM 18K"}},
{1,1,3,"Brown-out Select bits",{"4.4V","3.7V","2.7V","2.2V"}},
{1,2,2,"WDT Window Select",{"Others","0101","1111"}},
};
