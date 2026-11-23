/* RTC-aware host contract over the digest-bound, audited byte bus.
 * Included only by small_eprom.c so pin/voltage execution has one owner. */
static const unsigned char dallas_rtc_masks[14]={
    0xff,0x7f,0x7f,0xff,0x7f,0xff,0x07,0x87,0x3f,0xdf,0xff,0xfc,0xff,0xff};
static const unsigned char st_rtc_masks[8]={0xff,0xff,0x7f,0x3f,0x47,0x3f,0x1f,0xff};
static int rtc_bcd(unsigned b,unsigned min,unsigned max){return (b&15)<10 && (b>>4)<10 && ((b>>4)*10+(b&15))>=min && ((b>>4)*10+(b&15))<=max;}
static int rtc_top_kind(int k){return k==3||k==4||k==6;}
static const unsigned char ds164_masks[8]={0xc0,0xff,0x7f,0x3f,0x47,0x3f,0x1f,0xff};
static const unsigned char mux_masks[14]={0x7f,0xff,0x7f,0xff,0xff,0xff,7,0x3f,0x1f,0xff,0x7f,0xff,0,0};
static const unsigned char st129_masks[16]={0,0xff,0xff,0xff,0xbf,0xff,0xbf,0xff,0xff,0xff,0x7f,0x3f,0x47,0x3f,0x1f,0xff};
static const unsigned char *rtc_masks(int k){return k==6?st129_masks:k==5?mux_masks:k==4?ds164_masks:k==3?st_rtc_masks:dallas_rtc_masks;}
static unsigned rtc_bytes(int k){return !k?0:k==6?16:rtc_top_kind(k)?8:14;}
static unsigned rtc_ctl(int k){return k==6?8:rtc_top_kind(k)?0:11;}
static int mux_number(unsigned v,unsigned lo,unsigned hi,int binary){return binary?(v>=lo&&v<=hi):rtc_bcd(v,lo,hi);}
lt48_status lt48_timekeeper_validate_registers(int kind,const unsigned char *bytes,size_t size) {
    size_t count=rtc_bytes(kind);
    if(kind<1||kind>6||!bytes||size!=count)return LT48_INVALID_ARGUMENT;
    const unsigned char *m=rtc_masks(kind);
    for(size_t i=0;i<count;i++)if((bytes[i]&~m[i]) && !((kind==1||kind==2)&&i==11) && !(kind==5&&i>=12) && !(kind==6&&i==0))return LT48_INVALID_ARGUMENT;
    if(kind==5){
        int binary=!!(bytes[11]&4),h24=!!(bytes[11]&2);
        if(bytes[11]&0x80)return LT48_INVALID_ARGUMENT;
        if(!mux_number(bytes[0],0,59,binary)||!mux_number(bytes[2],0,59,binary)||
           !mux_number(bytes[4]&(h24?0xff:0x7f),h24?0:1,h24?23:12,binary)||
           !mux_number(bytes[6],1,7,binary)||!mux_number(bytes[7],1,31,binary)||!mux_number(bytes[8],1,12,binary)||!mux_number(bytes[9],0,99,binary))return LT48_INVALID_ARGUMENT;
        for(unsigned i=1;i<=5;i+=2)if((bytes[i]&0xc0)!=0xc0&&!mux_number(bytes[i]&(i==5&&!h24?0x7f:0xff),i==5&&!h24?1:0,i==5?(h24?23:12):59,binary))return LT48_INVALID_ARGUMENT;
        /* Only the normal 32 kHz timebase; do not reset divider or select
         * DS1687 bank 1 through a clock image. */
        if((bytes[10]&0x70)!=0x20)return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(kind==6){
        if(!rtc_bcd(bytes[1],0,99))return LT48_INVALID_ARGUMENT;
        for(unsigned i=2;i<=4;i++)if(!rtc_bcd(bytes[i]&(i==4?0x3f:0x7f),0,i==4?23:59))return LT48_INVALID_ARGUMENT;
        if(!rtc_bcd(bytes[5]&0x3f,0,31)||!rtc_bcd(bytes[6]&0x1f,1,12))return LT48_INVALID_ARGUMENT;
        unsigned rpt=((bytes[2]>>7)&1)|((bytes[3]>>6)&2)|((bytes[4]>>5)&4)|((bytes[5]>>4)&8)|((bytes[5]>>2)&16);
        if(rpt!=0&&rpt!=16&&rpt!=24&&rpt!=28&&rpt!=30&&rpt!=31)return LT48_INVALID_ARGUMENT;
        bytes+=8;
    }
    if(rtc_top_kind(kind)) {
        if(!rtc_bcd(bytes[1]&127,0,59)||!rtc_bcd(bytes[2],0,59)||!rtc_bcd(bytes[3],0,23)||
           !rtc_bcd(bytes[4]&7,1,7)||!rtc_bcd(bytes[5],1,31)||!rtc_bcd(bytes[6],1,12)||!rtc_bcd(bytes[7],0,99)||
           (bytes[4]&0x40))return LT48_INVALID_ARGUMENT;
    }else {
        unsigned hour=bytes[4],alarm=bytes[5]&127;
        if(!rtc_bcd(bytes[0],0,99)||!rtc_bcd(bytes[1],0,59)||!rtc_bcd(bytes[2],0,59)||
           !rtc_bcd(hour&((hour&64)?31:63),(hour&64)?1:0,(hour&64)?12:23)||
           !rtc_bcd(bytes[6],1,7)||!rtc_bcd(bytes[8],1,31)||!rtc_bcd(bytes[9]&31,1,12)||
           !rtc_bcd(bytes[10],0,99)||!rtc_bcd(bytes[12],0,99)||!rtc_bcd(bytes[13],0,99)||
           (!(bytes[3]&128)&&!rtc_bcd(bytes[3],0,59))||
           (!(bytes[5]&128)&&!rtc_bcd(alarm&((alarm&64)?31:63),(alarm&64)?1:0,(alarm&64)?12:23))||
           (!(bytes[7]&128)&&!rtc_bcd(bytes[7],1,7)))return LT48_INVALID_ARGUMENT;
        unsigned masks=((bytes[3]>>7)&1)|((bytes[5]>>6)&2)|((bytes[7]>>5)&4);
        if(masks!=0&&masks!=4&&masks!=6&&masks!=7)return LT48_INVALID_ARGUMENT;
    }
    /* ST W is a transaction latch, never a final saved configuration. */
    if(rtc_top_kind(kind) && (bytes[0]&0x80))return LT48_INVALID_ARGUMENT;
    /* DS1486 DIP shares INTA/SQW: do not enable both functions. */
    if(kind==2 && !(bytes[9]&0x40) && !(bytes[11]&((bytes[11]&0x40)?4:8)))return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
static uint32_t rtc_base(const lt48_small_eprom_resources *r){return rtc_top_kind(r->timekeeper)?r->capacity-rtc_bytes(r->timekeeper):0;}
static size_t rtc_count(const lt48_small_eprom_resources *r){return rtc_bytes(r->timekeeper);}
static uint32_t rtc_command(const lt48_small_eprom_resources *r){return rtc_base(r)+rtc_ctl(r->timekeeper);}
static lt48_status rtc_byte(small_session *s,uint32_t addr,int write,unsigned char *byte) {
    for(unsigned j=0;j<4;j++)s->buffers[s->r->address_buffer][j]=(unsigned char)(addr>>(8*j));
    s->buffers[s->r->output_buffer][0]=*byte;
    lt48_status st=run(s,10);
    if(st==LT48_OK)st=run(s,6); /* All address pins, including nonsequential register accesses. */
    if(st==LT48_OK)st=run(s,write?s->r->program_op:s->r->load_op);
    if(st==LT48_OK&&!write)*byte=s->buffers[s->r->input_buffer][0];
    return st;
}
static lt48_status rtc_command_write(small_session *s,unsigned char value) {
    unsigned char actual=0;lt48_status st=rtc_byte(s,rtc_command(s->r),1,&value);
    if(st==LT48_OK)st=rtc_byte(s,rtc_command(s->r),0,&actual);
    unsigned char mask=rtc_masks(s->r->timekeeper)[rtc_ctl(s->r->timekeeper)];
    return st!=LT48_OK?st:((actual^value)&mask)?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status rtc_start(small_session *s,uint16_t volts) {
    memset(s->buffers,0,sizeof(s->buffers));s->mode=1;
    lt48_compile_variable *v=var(s,0);if(v)v->value=volts;
    lt48_status st=run(s,0);
    if(st==LT48_OK && s->r->second_enable)st=run(s,9);
    if(st==LT48_OK)st=run(s,2);
    if(st==LT48_OK)st=lt48_runtime_transport_session_status(s->wire,1);
    if(st==LT48_OK)st=lt48_timekeeper_power_hold_internal();
    if(st==LT48_OK && s->r->second_enable)st=run(s,8);
    /* Any read breaks the phantom-clock key. Drain an already active
     * 64-bit transfer as well before interpreting a byte as RAM. */
    for(unsigned i=0;st==LT48_OK && s->r->phantom && i<65;i++){
        unsigned char discarded=0;st=rtc_byte(s,0,0,&discarded);
    }
    return st;
}
/* Independent uncancellable restoration, even if a cancelled/failed transfer
 * may have reached the chip. Do not reuse a possibly failed pending queue. */
static lt48_status rtc_restore(small_session *s,const lt48_protocol_transport *api,void *ctx,unsigned char value) {
    lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
    lt48_protocol_transport restore=*api;restore.cancelled=NULL;
    lt48_status st=lt48_runtime_transport_session_create(&restore,ctx,&s->wire);
    if(st==LT48_OK)st=rtc_command_write(s,value);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
    return st;
}
static lt48_status rtc_registers(small_session *s,const lt48_protocol_transport *api,void *ctx,
    const unsigned char *image,unsigned char *snapshot,lt48_small_eprom_result *out) {
    int stchip=rtc_top_kind(s->r->timekeeper);unsigned ctl=rtc_ctl(s->r->timekeeper);unsigned char original=0;
    lt48_status st=rtc_byte(s,rtc_command(s->r),0,&original);
    if(st!=LT48_OK)return st;
    /* Never clear a pre-existing ST write latch: that would commit somebody
     * else's staged clock changes. RAM operations do not access this latch. */
    if(stchip && (original&0x80))return LT48_SAFETY_REJECTED;
    unsigned char frozen=stchip?(original|0x40):s->r->multiplexed?(original|0x80):(original&0x7c);
    if(image&&stchip)frozen=0xc0|(image[ctl]&0x3f); /* W+R while loading. */
    st=rtc_command_write(s,frozen);
    uint32_t base=rtc_base(s->r);size_t count=rtc_count(s->r);
    const unsigned char *mask=rtc_masks(s->r->timekeeper);
    if(st==LT48_OK&&image&&!stchip)st=rtc_command_write(s,s->r->multiplexed?(image[11]|0x80):(image[11]&0x7c));
    for(size_t i=0;st==LT48_OK&&i<count;i++) {
        if(i==ctl) {if(snapshot)snapshot[i]=original;continue;}
        unsigned char value=image?image[i]:0;
        if(image && mask[i])st=rtc_byte(s,base+(uint32_t)i,1,&value);
        unsigned char actual=0;
        if(st==LT48_OK)st=rtc_byte(s,base+(uint32_t)i,0,&actual);
        if(st==LT48_OK&&image&&((actual^image[i])&mask[i])) {
            out->first_mismatch=base+(uint32_t)i;out->mismatch_count++;st=LT48_DEVICE_MISMATCH;
        }
        if(st==LT48_OK&&snapshot)snapshot[i]=actual;
    }
    /* On failure restore the prior transfer/control state. Data already
     * explicitly written may be partial; never report atomic rollback. */
    unsigned char final=image&&st==LT48_OK?image[ctl]:original;
    if(!stchip && !s->r->multiplexed)final&=0xfc;
    lt48_status restore=rtc_restore(s,api,ctx,final);
    if(restore!=LT48_OK){out->cleanup_status=restore;return st==LT48_OK?restore:st;}
    /* Resume cancellation for RAM work after mandatory control restoration. */
    lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
    restore=lt48_runtime_transport_session_create(api,ctx,&s->wire);
    return st==LT48_OK?restore:st;
}
static lt48_status rtc_ram(small_session *s,lt48_operation_kind op,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,lt48_small_eprom_result *out) {
    if(!length)return LT48_OK;
    for(unsigned j=0;j<4;j++)s->buffers[s->r->address_buffer][j]=(unsigned char)(start>>(8*j));
    lt48_status st=run(s,10);
    if(st==LT48_OK)st=run(s,6);
    return st==LT48_OK?range(s,op,start,length,image,read,out):st;
}
static lt48_status rtc_finish(small_session *s,const lt48_protocol_transport *api,void *ctx,lt48_status operation) {
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
    lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
    lt48_status first=lt48_runtime_transport_session_create(&cleanup,ctx,&s->wire),st=first;
    if(st==LT48_OK && s->r->second_enable)st=run(s,9);
    /* ST source RESET is empty. The borrowed byte-bus RESET cuts rails,
     * so it must not precede ST's shutdown ramp. */
    if(st==LT48_OK && !s->r->st_power)st=run(s,s->r->reset_op);
    if(st==LT48_OK)st=run(s,1);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
    if(first==LT48_OK)first=st;
    for(unsigned i=0;i<4;i++) {
        lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
        st=lt48_runtime_transport_session_create(&cleanup,ctx,&s->wire);
        unsigned char off[]={0x10,(unsigned char)(3-i),0};
        if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,off,sizeof off);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
        if(first==LT48_OK)first=st;
    }
    if(s->wire){
        const unsigned char finish[]={0x49,0xaa,0x55};
        st=lt48_runtime_transport_session_controller_write(s->wire,finish,sizeof finish);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
        if(first==LT48_OK)first=st;
        unsigned char led[]={0x40,operation==LT48_OK&&first==LT48_OK?4:2};
        st=lt48_runtime_transport_session_controller_write(s->wire,led,sizeof led);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
        if(first==LT48_OK)first=st;
    }
    lt48_runtime_transport_session_close(s->wire);s->wire=NULL;return first;
}
static lt48_status timekeeper_execute(const lt48_small_eprom_resources *r,
    const lt48_protocol_transport *api,void *ctx,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,
    lt48_small_eprom_result *out) {
    if(rtc_top_kind(r->timekeeper) && op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_LOW)return LT48_UNSUPPORTED;
    int config=op==LT48_OPERATION_CONFIGURATION;
    int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE;
    uint32_t low=!r->timekeeper||rtc_top_kind(r->timekeeper)?0:14,high=rtc_top_kind(r->timekeeper)?rtc_base(r):r->capacity;
    uint32_t begin=start<low?low:start,end=start+(uint32_t)length;if(end>high)end=high;
    size_t n=end>begin?end-begin:0,skip=begin>start?begin-start:0;
    unsigned char *pending=calloc(r->capacity,1),*expected=malloc(r->capacity);
    if(!pending||!expected){free(pending);free(expected);return LT48_NO_MEMORY;}
    small_session s={0};s.r=r;memcpy(s.vars,r->vars,sizeof(s.vars));
    lt48_status st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
    if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    static const unsigned char header[]={0x38,0xaa,0x55};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,header,sizeof header);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
    if(st==LT48_OK)st=rtc_start(&s,(uint16_t)r->nominal);
    unsigned char bank=0;int bank_saved=0;
    if(st==LT48_OK && r->multiplexed){
        st=rtc_byte(&s,10,0,&bank);
        if(st==LT48_OK){bank_saved=1;unsigned char zero_bank=bank&0x6f;st=rtc_byte(&s,10,1,&zero_bank);}
    }
    static const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof busy);
    if(st==LT48_OK&&rtc_top_kind(r->timekeeper)&&!config){
        unsigned char day=0;st=rtc_byte(&s,r->capacity-4,0,&day);
        if(st==LT48_OK&&(day&0x40))st=LT48_SAFETY_REJECTED;
    }
    if(st==LT48_OK&&(writing||config))st=rtc_ram(&s,LT48_OPERATION_READ,low,high-low,NULL,expected+low,out);
    if(st==LT48_OK&&writing){
        if(op==LT48_OPERATION_ERASE)memset(expected+begin,255,n);else memcpy(expected+begin,image+skip,n);
        st=rtc_ram(&s,LT48_OPERATION_PROGRAM,begin,n,expected+begin,NULL,out);
    }else if(st==LT48_OK&&config)st=rtc_registers(&s,api,ctx,image,NULL,out);
    else if(st==LT48_OK&&op==LT48_OPERATION_READ){
        uint32_t rb=rtc_base(r),re=rb+(uint32_t)rtc_count(r);
        if(r->timekeeper && start<re && start+length>rb){
            unsigned char regs[16];st=rtc_registers(&s,api,ctx,NULL,regs,out);
            uint32_t b=start>rb?start:rb,e=start+length<re?start+(uint32_t)length:re;
            if(st==LT48_OK)memcpy(pending+b,regs+b-rb,e-b);
        }
        if(st==LT48_OK)st=rtc_ram(&s,LT48_OPERATION_READ,begin,n,NULL,pending+begin,out);
    }else if(st==LT48_OK){
        /* M48T08 minimum operating voltage is 4.75 V, exactly the low
         * margin. Use nominal for ST to avoid power-fail comparator threshold. */
        unsigned v[2]={r->nominal*95/100,r->nominal*105/100},count=op==LT48_OPERATION_VERIFY?2:1;
        if(op!=LT48_OPERATION_VERIFY||margin==LT48_VERIFY_MARGIN_NOMINAL){v[0]=r->nominal;count=1;}
        else if(rtc_top_kind(r->timekeeper) && margin==LT48_VERIFY_MARGIN_ALL)v[0]=r->nominal;
        else if(margin==LT48_VERIFY_MARGIN_LOW)count=1;
        else if(margin==LT48_VERIFY_MARGIN_HIGH){v[0]=r->nominal*105/100;count=1;}
        for(unsigned i=0;st==LT48_OK&&i<count;i++){
            st=rtc_start(&s,(uint16_t)v[i]);
            if(st==LT48_OK)st=rtc_ram(&s,op,begin,n,image?image+skip:NULL,NULL,out);
        }
    }
    if(st==LT48_OK&&(writing||config))st=rtc_ram(&s,LT48_OPERATION_VERIFY,low,high-low,expected+low,NULL,out);
    if(bank_saved){
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
        lt48_protocol_transport restore=*api;restore.cancelled=NULL;
        lt48_status bs=lt48_runtime_transport_session_create(&restore,ctx,&s.wire);
        /* Explicit Configuration may change rate; retain the original bank. */
        unsigned char v=config&&st==LT48_OK?(image[10]&0x6f)|(bank&0x10):bank&0x7f;
        if(bs==LT48_OK)bs=rtc_byte(&s,10,1,&v);
        if(bs==LT48_OK)bs=lt48_runtime_transport_session_flush(s.wire);
        if(bs!=LT48_OK){out->cleanup_status=bs;if(st==LT48_OK)st=bs;}
    }
    out->operation_status=st;
    lt48_status cleaned=rtc_finish(&s,api,ctx,st);
    if(out->cleanup_status==LT48_OK)out->cleanup_status=cleaned;
    out->cleanup_completed=out->cleanup_status==LT48_OK;
    if(st==LT48_OK)st=out->cleanup_status;
    /* Verify battery-backed RAM again after actual socket power removal. This
     * does not establish remaining battery life, nor compare a ticking clock. */
    if(st==LT48_OK&&(writing||config)){
        lt48_small_eprom_result fresh={0};
        st=lt48_small_eprom_execute(r,api,ctx,LT48_OPERATION_READ,low,high-low,NULL,0,
            pending,high-low,LT48_VERIFY_MARGIN_NOMINAL,NULL,NULL,&fresh);
        if(st==LT48_OK)for(uint32_t i=low;i<high;i++)if(pending[i-low]!=expected[i]){
            if(!out->mismatch_count)out->first_mismatch=i;
            out->mismatch_count++;st=LT48_DEVICE_MISMATCH;
        }
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(st!=LT48_OK){
            lt48_protocol_transport finish=*api;finish.cancelled=NULL;
            lt48_status a=lt48_runtime_transport_session_create(&finish,ctx,&s.wire);
            if(a==LT48_OK){
                unsigned char error[]={0x40,2};a=lt48_runtime_transport_session_controller_write(s.wire,error,sizeof error);
                if(a==LT48_OK)a=lt48_runtime_transport_session_flush(s.wire);
                lt48_runtime_transport_session_close(s.wire);
            }
            if(out->cleanup_status==LT48_OK)out->cleanup_status=a;
            out->cleanup_completed=out->cleanup_status==LT48_OK;
        }
    }
    if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending+start,length);
    out->operation_status=st;if(st==LT48_OK)out->completed_bytes=config?rtc_count(r):op==LT48_OPERATION_READ?length:n;
    free(pending);free(expected);return st;
}
