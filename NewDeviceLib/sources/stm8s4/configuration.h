static const struct numicro_field s8_fields_25329[]={
{1,0,1,"Memory readout protection (ROP)",{"Disable","Enable"}},
{1,1,1,"AFR1",{"AFR1 remapping inactive","A3:SPI_NSS;D2:TIM2_CH3"}},
{1,2,1,"AFR5",{"AFR5 remapping inactive","D0:CLK_CCO"}},
{1,3,1,"AFR6",{"AFR6 remapping inactive","D7:TIM1_CH4"}},
{1,4,1,"Window Watchdog reset on halt",{"no reset on halt if WWDG active","Reset on halt if WWDG active"}},
{1,5,1,"Window Watchdog activation",{"WWDG activated by software","WWDG activated by hardware"}},
{1,6,1,"Independent Watchdog activation",{"IWDG activated by software","IWDG activated by hardware"}},
{1,7,1,"LSI clock",{"Disable","Enable"}},
{1,8,1,"HSI clock trimming register size",{"3bit trim in CLK_HSITRIMR","4bit trim in CLK_HSITRIMR"}},
{1,9,2,"AWU clock prescaler",{"16MHz to 128kHz prescaler","8MHz to 128kHz prescaler","4MHz to 128kHz prescaler"}},
{1,10,1,"Auto wake-up unit/clock",{"LSI clock selected for AWU","HSE clock selected for AWU"}},
{1,11,1,"External clock selection",{"Ext. xtal on OSCIN/OSCOUT","Ext. clock signal on OSCIN"}},
{1,12,3,"HSE stabilization time",{"2048 HSE cycles","128 HSE cycles","8 HSE cycles","0.5 HSE cycles"}},
{2,0,255,"user boot code area",{NULL}},
};
static const struct numicro_field s8_fields_25368[]={
{1,0,1,"Memory readout protection (ROP)",{"Disable","Enable"}},
{1,1,1,"AFR0",{"AFR0 remapping inactive","D3:ADC_ETR"}},
{1,2,1,"AFR1",{"AFR1 remapping inactive","A3:TIM3_CH1;D2:TIM2_CH3"}},
{1,3,1,"AFR2",{"AFR2 remapping inactive","D0:CLK_CCO"}},
{1,4,1,"AFR3",{"AFR3 remapping inactive","D0:TIM1_BKIN"}},
{1,5,1,"AFR4",{"AFR4 remapping inactive","D7:TIM1_CH4"}},
{1,6,1,"AFR5",{"AFR5 remapping inactive","B3:TIM1_ETR;B2:TIM1_NCC3"}},
{1,7,1,"AFR6",{"AFR6 remapping inactive","B5:I2C_SDA;B4:I2C_SCL"}},
{1,8,1,"AFR7",{"AFR7 remapping inactive","D4:BEEP"}},
{1,9,1,"Window Watchdog reset on halt",{"no reset on halt if WWDG active","Reset on halt if WWDG active"}},
{1,10,1,"Window Watchdog activation",{"WWDG activated by software","WWDG activated by hardware"}},
{1,11,1,"Independent Watchdog activation",{"IWDG activated by software","IWDG activated by hardware"}},
{1,12,1,"LSI clock",{"Disable","Enable"}},
{1,13,1,"HSI clock trimming register size",{"3bit trim in CLK_HSITRIMR","4bit trim in CLK_HSITRIMR"}},
{1,14,2,"AWU clock prescaler",{"16MHz to 128kHz prescaler","8MHz to 128kHz prescaler","4MHz to 128kHz prescaler"}},
{1,15,1,"Auto wake-up unit/clock",{"LSI clock selected for AWU","HSE clock selected for AWU"}},
{1,16,1,"External clock selection",{"Ext. xtal on OSCIN/OSCOUT","Ext. clock signal on OSCIN"}},
{1,17,3,"HSE stabilization time",{"2048 HSE cycles","128 HSE cycles","8 HSE cycles","0.5 HSE cycles"}},
{1,18,1,"Bootloader",{"Disable","Enable"}},
{2,0,255,"user boot code area",{NULL}},
};
static const struct numicro_field*s8_schema(unsigned id,unsigned*count){switch(id){
case 25329:*count=sizeof s8_fields_25329/sizeof*s8_fields_25329;return s8_fields_25329;
case 25368:*count=sizeof s8_fields_25368/sizeof*s8_fields_25368;return s8_fields_25368;
default:*count=0;return NULL;}}
