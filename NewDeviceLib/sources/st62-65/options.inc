/* Authored host ownership for the original ST62 option callbacks. */
static uint32_t st62_field(const small_session*s,unsigned category,unsigned field){
    for(size_t i=0;i<s->configuration_count;i++){const lt48_runtime_configuration_value*v=&s->configuration[i];if(v->object_id==0&&v->category==category&&v->field==field)return v->value;}return 0;
}
static void st62_set_field(small_session*s,unsigned category,unsigned field,uint32_t value){
    for(size_t i=0;i<s->configuration_count;i++){lt48_runtime_configuration_value*v=&s->configuration[i];if(v->object_id==0&&v->category==category&&v->field==field){v->value=value;return;}}
}
static unsigned st62_option_values(const small_session*s){
    if(s->r->st62_legacy)return st62_field(s,2,0)&255;
    unsigned value=0;for(unsigned bit=0;bit<16;bit++)if((s->r->st62_cfg_mask&(1u<<bit))&&st62_field(s,0,s->r->st62_cfg_fields[bit]))value|=1u<<bit;return value;
}
static int st62_protected(const small_session*s){return !s->r->st62_legacy&&st62_field(s,0,18)!=0;}
static lt48_status st62_read_options(small_session*s){
    s->mode=1;lt48_status st=init(s,NULL,NULL);if(st==LT48_OK)st=run(s,s->r->reset_op);return st;
}
static lt48_status st62_configuration(small_session*s,unsigned secure,unsigned char raw[2]){
    lt48_status st=st62_read_options(s);unsigned before=st62_option_values(s),set=s->r->st62_cfg_set;
    if(secure){set=0;for(unsigned i=0;i<16;i++)if(s->r->st62_cfg_fields[i]==18)set|=1u<<i;if(!set)return LT48_UNSUPPORTED;}
    unsigned wanted=before|set;
    if(st==LT48_OK&&wanted!=before){
        if(s->r->st62_legacy)st62_set_field(s,2,0,wanted);
        else for(unsigned i=0;i<16;i++)if(s->r->st62_cfg_mask&(1u<<i))/* Read and write maps are distinct: sourceM92 swaps two option fields. */
            st62_set_field(s,0,s->r->st62_cfg_write_fields[i],!!(wanted&(1u<<i)));
        s->mode=2;st=init(s,NULL,NULL);
        if(st==LT48_OK&&!s->r->st62_legacy)st=run(s,s->r->protect_op);
        if(st==LT48_OK)st=run(s,s->r->reset_op);
        if(st==LT48_OK&&s->r->st62_legacy){s->mode=4;st=init(s,NULL,NULL);if(st==LT48_OK)st=run(s,s->r->reset_op);}
        if(st==LT48_OK)st=run(s,1);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
        if(st==LT48_OK)st=run(s,0);
        if(st==LT48_OK)st=st62_read_options(s);
        if(st==LT48_OK&&st62_option_values(s)!=wanted)st=LT48_DEVICE_MISMATCH;
    }
    unsigned actual=st62_option_values(s);raw[0]=(unsigned char)actual;raw[1]=(unsigned char)(actual>>8);return st;
}
