/* Native host workflow from Intel 210393-008 pp.6-11, 16-18.
 * Imported M4f supplies the FPGA and array latch/read primitive.
 * All authored additions reside here and in resources.inc. No USB side path. */
static lt48_status upi_load(mcu_session *s,uint32_t address,unsigned char *value) {
    for(unsigned j=0;j<4;j++)s->buffers[4][j]=(unsigned char)(address>>(8*j));
    lt48_status status=run(s,6);if(status==LT48_OK)*value=s->buffers[5][0];return status;
}
static lt48_status upi_program_byte(mcu_session *s,uint32_t address,unsigned char wanted) {
    unsigned char actual=0;lt48_status status=upi_load(s,address,&actual);
    if(status!=LT48_OK)return status;
    if((actual|wanted)!=wanted)return LT48_DEVICE_MISMATCH;
    if(actual==wanted)return LT48_OK;
    lt48_compile_variable *pulse=var(s,20);
    if(!pulse || pulse->kind!=1)return LT48_FORMAT_ERROR;
    s->buffers[7][0]=wanted;
    /* 100-us timer units: 10 units = 1 ms; at most 25 initial pulses. */
    pulse->value=1;pulse->channel=10;
    unsigned tries;
    for(tries=1;tries<=25;tries++) {
        status=run(s,7);if(status!=LT48_OK)return status;
        status=upi_load(s,address,&actual);if(status!=LT48_OK)return status;
        if(actual==wanted)break;
    }
    if(tries>25)return LT48_DEVICE_MISMATCH;
    pulse->channel=(uint16_t)(30*tries); /* Mandatory 3X-ms overprogram pulse. */
    status=run(s,7);pulse->channel=10;
    if(status==LT48_OK)status=upi_load(s,address,&actual);
    return status==LT48_OK && actual!=wanted?LT48_DEVICE_MISMATCH:status;
}
static lt48_status upi_cycle(mcu_session *s,lt48_operation_kind op,uint32_t start,size_t n,
    const unsigned char *image,unsigned char *read,uint16_t vcc,int signature,lt48_mcu51_result *out) {
    int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if((signature && (start>=32 || n>32-start)) || (!signature && (start>=s->r->capacity || n>s->r->capacity-start)))return LT48_SAFETY_REJECTED;
    memset(s->buffers,0,sizeof(s->buffers));s->buffers[6][0]=signature?0xfe:0;
    var(s,0)->value=writing?600:vcc;var(s,11)->value=writing?1250:vcc;
    var(s,12)->value=writing?500:vcc;
    s->mode=writing?2:1;
    lt48_status status=run(s,4);
    for(size_t i=0;status==LT48_OK && i<n;i++) {
        unsigned char actual=0;
        uint32_t address=start+(uint32_t)i;
        status=writing?upi_program_byte(s,address,image[i]):upi_load(s,address,&actual);
        if(status!=LT48_OK) {
            if(status==LT48_DEVICE_MISMATCH){if(!out->mismatch_count)out->first_mismatch=address;out->mismatch_count++;}
            break;
        }
        if(read)read[i]=actual;
        out->completed_bytes=i+1;
        if(!writing && op!=LT48_OPERATION_READ && actual!=(op==LT48_OPERATION_BLANK_CHECK?0:image[i])) {
            if(!out->mismatch_count)out->first_mismatch=address;
            out->mismatch_count++;
            if(op!=LT48_OPERATION_COMPARE){status=LT48_DEVICE_MISMATCH;break;}
        }
        if(s->api->progress && ((i&63)==63 || i+1==n))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,i+1,n,0,0,0);
    }
    /* Return both HV pins to 5 V with PROG inactive before removing rails. */
    var(s,12)->value=500;
    lt48_status reset=run(s,5);
    if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    return status==LT48_OK?reset:status;
}
static lt48_status upi_identify(mcu_session *s,unsigned char row[32],lt48_m37_id_decision_fn decide,void *context,lt48_mcu51_result *out) {
    unsigned expected=s->r->capacity==1024?0x40:0x42;
    for(unsigned attempt=0;attempt<64;attempt++) {
        lt48_status status=upi_cycle(s,LT48_OPERATION_READ,0,32,NULL,row,500,1,out);
        if(status!=LT48_OK)return status;
        if(row[0x10]==0x89 && row[0x11]==expected)return LT48_OK;
        if(!decide)return LT48_DEVICE_MISMATCH;
        lt48_m37_id_choice choice=decide(context,0x89,(uint16_t)expected,row[0x10],row[0x11]);
        if(choice==LT48_M37_ID_IGNORE)return LT48_OK;
        if(choice==LT48_M37_ID_ABORT)return LT48_DEVICE_MISMATCH;
        if(choice!=LT48_M37_ID_RETRY)return LT48_INVALID_ARGUMENT;
    }
    return LT48_DEVICE_MISMATCH;
}
static lt48_status upi_configuration(mcu_session *s,const unsigned char *image,size_t n,unsigned char row[32],lt48_mcu51_result *out) {
    if(!n)return LT48_OK;
    if(row[31])return LT48_SAFETY_REJECTED;
    unsigned char before[32];memcpy(before,row,32);
    /* Validate BOTH requested bytes before any irreversible pulse. */
    for(unsigned i=0;i<2;i++)if((image[0]&(1u<<i)) && (row[0x12+i]|image[1+i])!=image[1+i])return LT48_DEVICE_MISMATCH;
    for(unsigned i=0;i<2;i++)if(image[0]&(1u<<i)) {
        lt48_status status=upi_cycle(s,LT48_OPERATION_PROGRAM,0x12+i,1,image+1+i,NULL,600,1,out);
        if(status!=LT48_OK)return status;
        before[0x12+i]=image[1+i];
    }
    lt48_status status=upi_cycle(s,LT48_OPERATION_READ,0,32,NULL,row,500,1,out);
    if(status==LT48_OK && memcmp(before,row,32))status=LT48_DEVICE_MISMATCH;
    return status;
}
static lt48_status upi_secure(mcu_session *s,lt48_mcu51_result *out) {
    unsigned char known[2],masked[2],security=0,ones=255;
    lt48_status status=upi_cycle(s,LT48_OPERATION_READ,0,2,NULL,known,500,0,out);
    if(status==LT48_OK)status=upi_cycle(s,LT48_OPERATION_PROGRAM,31,1,&ones,NULL,600,1,out);
    if(status==LT48_OK)status=upi_cycle(s,LT48_OPERATION_READ,31,1,NULL,&security,500,1,out);
    if(status==LT48_OK && security!=255)status=LT48_DEVICE_MISMATCH;
    if(status==LT48_OK)status=upi_cycle(s,LT48_OPERATION_READ,0,2,NULL,masked,500,0,out);
    /* Intel explicitly says the signature security byte alone is insufficient. */
    if(status==LT48_OK && !memcmp(known,masked,2))status=LT48_DEVICE_MISMATCH;
    return status;
}
