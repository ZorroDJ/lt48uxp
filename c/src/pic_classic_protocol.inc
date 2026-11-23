/* Production transport adapter for the authored classic protocol. The source
 * pin map resolves both selectors; the shared session owns USB and cleanup. */
typedef struct {pic_session *s;unsigned data,clock,vcc;} classic_wire_context;
static lt48_status classic_wire_transfer(void *v,int read,unsigned bits,uint32_t value,uint32_t *out) {
    classic_wire_context *c=v;pic_session *s=c->s;
    if(!out||(read&&(bits!=14))||(!read&&(bits!=1&&bits!=6&&bits!=14))||
       value>=(1u<<bits))return LT48_INVALID_ARGUMENT;
    if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
    unsigned char packet[6]={(unsigned char)(read?0x60:0x58),
        (unsigned char)(c->data+(read?2:(value&1))), (unsigned char)c->clock,(unsigned char)(bits-1),
        (unsigned char)value,(unsigned char)(value>>8)};
    lt48_status st=LT48_OK;
    /* Command60 has14samples but13clocks; original ICSP callbacks prime
     * the first data bit with18 afterSTART and before the capture handler. */
    if(read){const unsigned char prime[]={0x18,(unsigned char)(c->clock+1)};
        st=lt48_runtime_transport_session_controller_write(s->wire,prime,sizeof prime);}
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,packet,read?4:4+(bits+7)/8);
    if(st==LT48_OK&&read) {
        unsigned char bytes[2];st=lt48_runtime_transport_session_controller_capture_internal(s->wire,bytes,2);
        if(st==LT48_OK)*out=(bytes[0]|(uint32_t)bytes[1]<<8)&0x3fff;
    }else *out=0;
    return st;
}
static lt48_status classic_wire_delay(void *v,unsigned us) {
    classic_wire_context *c=v;unsigned char packet[3];
    if(!us||us>6553500)return LT48_INVALID_ARGUMENT;
    if(us<=255){packet[0]=0x4e;packet[1]=(unsigned char)us;return lt48_runtime_transport_session_controller_write(c->s->wire,packet,2);}
    unsigned units=us/100,remainder=us%100;packet[0]=0x4f;packet[1]=(unsigned char)units;packet[2]=(unsigned char)(units>>8);
    lt48_status st=lt48_runtime_transport_session_controller_write(c->s->wire,packet,3);
    if(st==LT48_OK&&remainder){packet[0]=0x4e;packet[1]=(unsigned char)remainder;st=lt48_runtime_transport_session_controller_write(c->s->wire,packet,2);}
    return st;
}
static lt48_status classic_wire_reenter(void *v) {
    classic_wire_context *c=v;pic_session *s=c->s;
    lt48_status st=reset(s);if(st==LT48_OK)st=p17_start(s,s->mode,c->vcc);
    /* The original optional short-ID comparison is disabled for this owner.
     * Compare the full 14-bit read with the independent silicon ID mask. */
    lt48_pic_classic_wire w={c,classic_wire_transfer,classic_wire_delay,NULL};
    if(st==LT48_OK)st=lt48_pic_classic_load(&w,0,0x3fff);
    for(unsigned i=0;st==LT48_OK&&i<6;i++)st=lt48_pic_classic_command(&w,6);
    uint16_t id=0;
    if(st==LT48_OK)st=lt48_pic_classic_command(&w,4);
    if(st==LT48_OK)st=lt48_pic_classic_frame(&w,1,&id);
    const lt48_pic_entry_policy *entry=s->r->profile->classic14->entry;
    if(st==LT48_OK&&(id&entry->id_mask)!=entry->id_expected)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=reset(s);
    if(st==LT48_OK)st=p17_start(s,s->mode,c->vcc);
    return st;
}
static lt48_status classic_wire_create(pic_session *s,classic_wire_context *c,lt48_pic_classic_wire *w) {
    const lt48_pic_classic14_policy *policy=s->r->profile->classic14;
    if(!policy||!policy->protocol)return LT48_INVALID_ARGUMENT;
    lt48_logical_pin data,clock;
    lt48_status st=lt48_pin_map_find_logical(s->r->pins,policy->data_pin,&data);
    if(st==LT48_OK)st=lt48_pin_map_find_logical(s->r->pins,policy->clock_pin,&clock);
    if(st!=LT48_OK)return st;
    unsigned vcc=0;for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)vcc=s->vars[i].value;
    if(data.engine_pin==clock.engine_pin||!vcc)return LT48_SAFETY_REJECTED;
    *c=(classic_wire_context){s,data.engine_pin,clock.engine_pin,vcc};
    *w=(lt48_pic_classic_wire){c,classic_wire_transfer,classic_wire_delay,classic_wire_reenter};return LT48_OK;
}
static lt48_status classic_protocol_range(pic_session *s,const lt48_pic_window *window,unsigned mode,
    const unsigned char *source,unsigned char *destination) {
    classic_wire_context context;lt48_pic_classic_wire wire;
    lt48_status st=classic_wire_create(s,&context,&wire);if(st!=LT48_OK)return st;
    const lt48_pic_classic_protocol *p=s->r->profile->classic14->protocol;
    lt48_pic_87xa_profile p87={p->program_words,p->eeprom_bytes};
    size_t n=window->count;if(n>p->program_words||!n)return LT48_SAFETY_REJECTED;
    uint16_t *values=malloc(n*sizeof(*values));if(!values)return LT48_NO_MEMORY;
    if(mode==2){for(size_t i=0;i<n;i++)values[i]=word(source+2*(window->address+i))&word_mask(s->r,window->address+(unsigned)i);
        st=lt48_pic_classic_isrow4(p)?lt48_pic_row4_program(p,&wire,window->address,values,n):
            lt48_pic_classic_is87x(p)?lt48_pic_87x_program(p,&wire,window->address,values,n):
            lt48_pic_classic_is7x(p)?lt48_pic_7x_program(p,&wire,window->address,values,n):
            lt48_pic_classic_is87xa(p)?lt48_pic_87xa_program(&p87,&wire,window->address,values,n):
            lt48_pic_classic_program_range(p,&wire,window->address,values,n);
    } else {
        st=lt48_pic_classic_is87xa(p)?lt48_pic_87xa_read(&p87,&wire,window->address,values,n):
            lt48_pic_classic_read_range(p,&wire,window->address,values,n);
        if(st==LT48_OK)for(size_t i=0;i<n;i++)if(word_mask(s->r,window->address+(unsigned)i))
            put_word(destination+2*(window->address+i),values[i]);
    }
    free(values);return st;
}
static lt48_status classic_protocol_configuration(pic_session *s) {
    classic_wire_context context;lt48_pic_classic_wire wire;
    lt48_status st=classic_wire_create(s,&context,&wire);if(st!=LT48_OK)return st;
    uint16_t values[2];const lt48_pic_classic_protocol *p=s->r->profile->classic14->protocol;
    for(unsigned i=0;i<p->configuration_words;i++)values[i]=word(s->image+2*(0x2007+i));
    s->mode=8;
    if(lt48_pic_classic_is87xa(p)){
        lt48_pic_87xa_profile p87={p->program_words,p->eeprom_bytes};
        return lt48_pic_87xa_configuration(&p87,&wire,values[0]);
    }
    if(lt48_pic_classic_isrow4(p))return lt48_pic_row4_configuration(p,&wire,values,p->configuration_words);
    if(lt48_pic_classic_is87x(p))return lt48_pic_87x_configuration(p,&wire,values[0]);
    return lt48_pic_classic_is7x(p)?lt48_pic_7x_configuration(p,&wire,values,p->configuration_words):lt48_pic_classic_configuration(p,&wire,values,p->configuration_words);
}
static lt48_status classic_protocol_erase(pic_session *s,unsigned vcc) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    s->mode=7;classic_wire_context context;lt48_pic_classic_wire wire;
    lt48_status st=classic_wire_create(s,&context,&wire);
    const lt48_pic_classic_protocol *p=s->r->profile->classic14->protocol;
    if(st==LT48_OK&&lt48_pic_classic_is87xa(p)){
        lt48_pic_87xa_profile p87={p->program_words,p->eeprom_bytes};
        return lt48_pic_87xa_erase(&p87,&wire);
    }
    if(st==LT48_OK&&lt48_pic_classic_isrow4(p))return lt48_pic_row4_erase(p,&wire);
    if(st==LT48_OK&&lt48_pic_classic_is87x(p))return lt48_pic_87x_erase(p,&wire);
    return st==LT48_OK?(lt48_pic_classic_is7x(p)?lt48_pic_7x_erase(p,&wire):lt48_pic_classic_erase(p,&wire)):st;
}
static lt48_status classic_protocol_factory(pic_session *s,unsigned vcc,uint16_t *value) {
    if(!s->r->profile->classic14->protocol->calibration_words){
        value[0]=value[1]=0;return LT48_OK; /* No factory calibration in this family. */
    }
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    unsigned mode=s->mode;s->mode=1;
    classic_wire_context context;lt48_pic_classic_wire wire;
    lt48_status st=classic_wire_create(s,&context,&wire);
    const lt48_pic_classic_protocol *p=s->r->profile->classic14->protocol;
    if(st==LT48_OK)st=lt48_pic_classic_read_range(p,&wire,0x2007+p->configuration_words,value,p->calibration_words);
    lt48_status end=reset(s);s->mode=mode;return st==LT48_OK?end:st;
}
