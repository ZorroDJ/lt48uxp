/* Authored Intel UPI-41AH/42AH extension. See UPI41AH_AUDIT.md.
 * Source P2b/P2c still determines socket placement and all existing controls.
 * Add only datasheet Port 1, /SS and /CS pins, using package pin numbers. */
static lt48_status upi_pins(const lt48_pin_code *original,lt48_pin_map **out) {
    if(original->data_size>200 || (original->pin_count!=40 && original->pin_count!=44))return LT48_SAFETY_REJECTED;
    unsigned char data[256];size_t split=3;
    for(unsigned i=0;i<original->data[0];i++) {
        if(split+4>original->data_size)return LT48_FORMAT_ERROR;
        size_t n=original->data[split+2]+((size_t)original->data[split+3]<<8);
        split+=4+n;if(split>original->data_size)return LT48_FORMAT_ERROR;
    }
    memcpy(data,original->data,split);data[0]++;data[1]+=2;
    size_t p=split;data[p++]=6;data[p++]=0;data[p++]=8;data[p++]=0;
    static const unsigned char dip[]={27,28,29,30,31,32,33,34};
    static const unsigned char plcc[]={30,31,32,33,35,36,37,38};
    for(unsigned i=0;i<8;i++)data[p++]=(original->pin_count==40?dip[i]:plcc[i])-1;
    memcpy(data+p,original->data+split,original->data_size-split);p+=original->data_size-split;
    data[p++]=68;data[p++]=0;data[p++]=original->pin_count==40?4:5;data[p++]=0; /* /SS */
    data[p++]=69;data[p++]=0;data[p++]=original->pin_count==40?5:6;data[p++]=0; /* /CS */
    lt48_pin_code code={original->pin_count,data,p};return lt48_pin_map_parse(&code,out);
}
static lt48_status upi_wave(lt48_mcu51_resources *r,unsigned op,const unsigned char *body,size_t n) {
    unsigned char bytes[256];if(n>sizeof(bytes)-2)return LT48_INVALID_ARGUMENT;
    bytes[0]=(unsigned char)n;bytes[1]=(unsigned char)(n>>8);memcpy(bytes+2,body,n);
    lt48_compact_wave *raw=NULL;lt48_compiled_wave *compiled=NULL;
    lt48_status status=lt48_compact_decode(bytes,n+2,&raw);
    if(status==LT48_OK)status=lt48_compact_compile(raw,r->pins,r->vars,r->var_count,1,0,&compiled);
    lt48_compact_wave_close(raw);
    if(status==LT48_OK && lt48_compiled_wave_defaulted_pin_count(compiled))status=LT48_SAFETY_REJECTED;
    if(status!=LT48_OK){lt48_compiled_wave_close(compiled);return status;}
    lt48_compiled_wave_close(r->wave[op]);r->wave[op]=compiled;return LT48_OK;
}
static lt48_status upi_resources(lt48_mcu51_resources *r) {
    /* Source M4f setup, with explicit /SS=/CS=1 and the selected Port 1
     * access code BEFORE EA high voltage. VDD and VCC are chosen per cycle. */
    static const unsigned char init[]={
        0x34,15,0x11,9,1,0x11,51,1,0x11,52,1,0x31,27,
        0x12,0,1,0x03,0,0,0x00,30,0x03,20,12,0x00,12,
        0x03,25,14,0x00,14,0x12,5,0,
        0x11,68,1,0x11,69,1,0x16,6,6,
        0x11,21,1,0x11,19,0,0x11,9,0,0x31,29,
        0x00,13,0x31,29,0x00,0,0x31,29,0x00,11,0x31,27};
    /* Source's active-low PROG hardware pulse, one pulse per invocation.
     * Buffer 7 retains the intended byte across independently captured reads. */
    static const unsigned char pulse[]={0x11,19,0,0x16,9,7,0x31,29,0x2c,21,20,0,0x31,29};
    /* M4f latch/read ordering, with explicit 20-us setup/hold guards.
     * 4*tCY <= 15 us (Intel p.14); no dependence on USB scheduling. */
    static const unsigned char load[]={0x11,9,0,0x31,29,0x16,13,4,0x31,29,
        0x11,9,1,0x31,29,0x11,19,1,0x31,29,0x18,9,5,0x11,19,0,0x31,29};
    static const unsigned char reset[]={0x11,21,1,0x11,19,1,0x11,9,0,0x31,29,0x00,12,0x00,14,0x31,27,0x36};
    if(r->var_count>=16)return LT48_FORMAT_ERROR;
    r->vars[r->var_count++]=(lt48_compile_variable){0,30,0,500,0};
    for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==29){r->vars[i].value=0;r->vars[i].channel=20;}
    lt48_status status=upi_wave(r,4,init,sizeof(init));
    if(status==LT48_OK)status=upi_wave(r,7,pulse,sizeof(pulse));
    if(status==LT48_OK)status=upi_wave(r,6,load,sizeof(load));
    if(status==LT48_OK)status=upi_wave(r,5,reset,sizeof(reset));
    return status;
}
