/* Authored field meanings; no original applets/FPGA payloads. */
static const struct mw36_field mwF6one_fields_25457[]={
{0,0,1,"HWBS",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"HWBS2",{NULL}},
{0,3,1,"BO0REO",{NULL}},
{0,4,1,"BO1REO",{NULL}},
{0,5,1,"HWWIDL",{NULL}},
{0,6,1,"WRENO",{NULL}},
{0,7,1,"HWENW",{NULL}},
{0,8,1,"NSWDT",{NULL}},
{0,9,1,"WDSFWP",{NULL}},
{1,0,3,"Gain Define",{"BOD1 2.0V","BOD1 2.4V","BOD1 3.7V","BOD1 4.2V"}},
{1,1,15,"ISP Memory size",{"7.5K bytes","7K bytes","6.5K bytes","6K bytes","5.5K bytes","5K bytes","4.5K bytes","4K bytes","3.5K bytes","3K bytes","2.5K bytes","2K bytes","1.5K bytes","1K bytes","0.5K bytes","No ISP code"}},
{1,2,7,"WDT Pre-Scalar",{"FOSC/2","FOSC/4","FOSC/8","FOSC/16","FOSC/32","FOSC/64","FOSC/128","FOSC/256"}},
{2,0,255,"IAP boundary",{NULL}},
};
