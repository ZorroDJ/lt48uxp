/* Additional technology filters use admitted operation profiles, not name prefixes. */
static guint memory_filter_mask(const lt48_installation *x,size_t index,
    const lt48_catalogue_entry *entry,const char *profile) {
    if(entry->device_class==2034u&&entry->device_type==9u&&!strcmp(entry->vendor,"SIERRA"))return 1u<<6;

    if(entry->device_class!=2000 && entry->device_class!=2002 && entry->device_class!=2003 && entry->device_class!=2004 && entry->device_class!=2008 && entry->device_class!=2007 && entry->device_class!=2005 && entry->device_class!=2029 && entry->device_class!=2006 &&
       entry->device_class!=2010 && entry->device_class!=2011 && entry->device_class!=2012 &&
       entry->device_class!=2030 && entry->device_class!=2032 && entry->device_class!=2031 && entry->device_class!=2033 && entry->device_class!=2034 && entry->device_class!=2043 && entry->device_class!=2044 && entry->device_class!=2041 && entry->device_class!=2045 && entry->device_class!=2009)return FALSE;
    gboolean page_nor=!strcmp(profile,"at29word-eight-operation");
    if(entry->device_class==2043) {
        lt48_serial_page_resources *r=NULL;
        if(lt48_serial_page_open(x,index,&r)==LT48_OK)page_nor=lt48_serial_page_is_nor(r);
        lt48_serial_page_close(r);
    }
    gboolean gdac=!strcmp(profile,"gdac-serial-six-operation");
    gboolean flash=!strcmp(profile,"legacy28f-seven-operation") || (!strcmp(profile,"legacy-nor-seven-operation")||!strcmp(profile,"mx27page-otp-six-operation")) || !strcmp(profile,"compact-flash-nine-operation") || (gdac && entry->device_type!=0x1d) || !strcmp(profile,"legacy-dataflash-six-operation") || (!strcmp(profile,"gaaf-serial-six-operation") || (!strcmp(profile,"nexflash-eight-operation") || !strcmp(profile,"w45d-eight-operation"))) || (entry->device_class==2030 || entry->device_class==2032 || entry->device_class==2031 || entry->device_class==2033 || entry->device_class==2044 || entry->device_class==2041 || entry->device_class==2045) || !strcmp(profile,"w27-seven-operation") ||
        page_nor;
    gboolean resistive=entry->device_class==2044 && entry->device_type>=0x12 && entry->device_type<=0x15;
    gboolean at28_eeprom=!strcmp(profile,"at28c64b1-seven-operation")||(!strcmp(profile,"eep2041-three-seven-operation")||!strcmp(profile,"white3-seven-operation"));
    gboolean x88_eeprom=entry->device_class==2005&&entry->device_type==0x28&&entry->vendor&&!strcmp(entry->vendor,"Xicor");
    if(resistive||at28_eeprom||x88_eeprom)flash=FALSE;
    gboolean backed=resistive || strstr(profile,"nvram") || strstr(profile,"fram") ||
        strstr(profile,"sram") || (!strcmp(profile,"stk10c68-seven-operation")||!strcmp(profile,"stk11c68-seven-operation"));
    gboolean eeprom=!flash && !backed && (x88_eeprom || at28_eeprom || (gdac && entry->device_type==0x1d) || strstr(profile,"eeprom") ||
        !strcmp(profile,"at17n-eight-operation") || !strcmp(profile,"page-i2c-eight-operation") || !strcmp(profile,"page-i2c-nine-operation") || !strcmp(profile,"serial-page-seven-operation") || !strcmp(profile,"spi-page-eight-operation") ||
        (!strcmp(profile,"compact-spi-seven-operation") || !strcmp(profile,"compact-spi-eight-operation") || !strcmp(profile,"idlock-nine-operation") || !strcmp(profile,"incremental-eight-operation") || !strcmp(profile,"compact-flash-nine-operation")) || (!strcmp(profile,"compact-i2c-seven-operation") || !strcmp(profile,"compact-i2c-eight-operation") || !strcmp(profile,"endurance-nine-operation") || (!strcmp(profile,"spd-nine-operation") || !strcmp(profile,"at17-compact-eight-operation"))) ||
        !strcmp(profile,"microwire-seven-operation"));
    return (eeprom?(1u<<6):0u)|(flash?(1u<<7):0u)|(backed?(1u<<8):0u);
}
