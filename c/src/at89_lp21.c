#include "at89_lp21.h"
#include <string.h>

lt48_status lt48_lp21_compile_row(const lt48_wave_code *source,
    enum lt48_lp21_row row, const lt48_pin_map *pins,
    const lt48_compile_variable *vars, size_t count, lt48_compiled_wave **out) {
    static const unsigned char expected[32]={
        0x75,0x45,0x4d,0x70,0xec,0x4a,0x09,0xbd,0x6d,0x73,0x1f,0x6c,0xf6,0xcb,0x9c,0x02,
        0xec,0x21,0x00,0xa3,0x6d,0x7d,0x18,0x80,0xc8,0xb1,0x0d,0x18,0x3e,0x8f,0x56,0x1f};
    static const size_t bounds[]={2,100,347,504};
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!source || !pins || !vars || row<LT48_LP21_SIGNATURE || row>LT48_LP21_LOCKS)
        return LT48_INVALID_ARGUMENT;
    if(source->wave_type!=0 || source->data_size!=504 || !source->data)
        return LT48_SAFETY_REJECTED;
    unsigned char hash[32];lt48_sha256(source->data,source->data_size,hash);
    if(memcmp(hash,expected,sizeof(hash)))return LT48_SAFETY_REJECTED;
    unsigned char raw[506];size_t body=bounds[row+1]-bounds[row];
    raw[0]=(unsigned char)body;raw[1]=(unsigned char)(body>>8);
    memcpy(raw+2,source->data+bounds[row],body);
    lt48_compact_wave *wave=NULL;
    lt48_status st=lt48_compact_decode(raw,body+2,&wave);
    if(st==LT48_OK)st=lt48_compact_compile(wave,pins,vars,count,1,0,out);
    lt48_compact_wave_close(wave);return st;
}

static int valid(const lt48_lp21_settings *s) {
    for(unsigned i=0;i<11;i++)if(s->value[i]>1)return 0;
    return s->value[11]<=2;
}
static int fuses_equal(const lt48_lp21_settings *a,const lt48_lp21_settings *b) {
    return !memcmp(a->value,b->value,11) && a->value[12]==b->value[12];
}
static void fuses_copy(lt48_lp21_settings *a,const lt48_lp21_settings *b) {
    memcpy(a->value,b->value,11);a->value[12]=b->value[12];
}
static void append(lt48_lp21_plan *p,enum lt48_lp21_row row,const lt48_lp21_settings *s) {
    p->steps[p->count++]=(lt48_lp21_step){row,*s};
}
lt48_status lt48_lp21_plan_settings(const lt48_lp21_settings *before,
    const lt48_lp21_settings *edits,unsigned mask,lt48_lp21_plan *out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    if(!before || !edits || (mask&~0x3fffu))return LT48_INVALID_ARGUMENT;
    if(!valid(before))return LT48_DEVICE_MISMATCH;
    lt48_lp21_settings wanted=*before,now=*before;
    for(unsigned i=0;i<13;i++)if(mask&(1u<<i))wanted.value[i]=edits->value[i];
    if(mask&0x2000)memcpy(wanted.signature,edits->signature,64);
    if(!valid(&wanted))return LT48_INVALID_ARGUMENT;
    /* Datasheets 3538E/3621E: CS1=00,CS0=FF is reserved; fuse 12h=00
     * is "Do not use". Preserve observed bytes on unrelated edits, but do
     * not introduce those settings from the original UI's ambiguous labels. */
    if((wanted.value[10] && !before->value[10]) ||
       (wanted.value[1] && !wanted.value[0] &&
        (wanted.value[0]!=before->value[0] || wanted.value[1]!=before->value[1])))
        return LT48_SAFETY_REJECTED;
    int signature=memcmp(wanted.signature,before->signature,64)!=0;
    int fuses=!fuses_equal(&wanted,before);
    /* Both protection levels inhibit signature/fuse writes. Unlocking needs
     * an explicit whole-chip erase, never a side effect of Configuration. */
    if(wanted.value[11]<before->value[11] || (before->value[11] && (signature || fuses)))
        return LT48_SAFETY_REJECTED;
    if(signature) {
        /* Keep the existing clock/ISP settings until the signature is safely
         * written. Source W152 alone writes signature before enabling it. */
        if(!now.value[8]) {
            now.value[8]=1;append(out,LT48_LP21_FUSES,&now);
        }
        memcpy(now.signature,wanted.signature,64);
        append(out,LT48_LP21_SIGNATURE,&now);
    }
    if(!fuses_equal(&now,&wanted)) {
        fuses_copy(&now,&wanted);append(out,LT48_LP21_FUSES,&now);
    }
    if(now.value[11]!=wanted.value[11]) {
        now.value[11]=wanted.value[11];append(out,LT48_LP21_LOCKS,&now);
    }
    return LT48_OK;
}

lt48_status lt48_lp21_update_settings(void *context,
    lt48_lp21_read_settings_fn read,lt48_lp21_write_row_fn write,
    const lt48_lp21_settings *edits,unsigned mask,lt48_lp21_update_result *out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    if(!read || !write || !edits || (mask&~0x3fffu))return LT48_INVALID_ARGUMENT;
    lt48_status st=read(context,&out->before);
    if(st!=LT48_OK)return st;
    if(!valid(&out->before))return LT48_DEVICE_MISMATCH;
    out->observed=out->before;out->observed_valid=1;
    lt48_lp21_plan plan;
    st=lt48_lp21_plan_settings(&out->before,edits,mask,&plan);
    for(size_t i=0;st==LT48_OK && i<plan.count;i++) {
        const lt48_lp21_step *step=&plan.steps[i];
        out->observed_valid=0;out->writes_attempted++;
        st=write(context,step->row,&step->expected);
        if(st==LT48_OK)st=read(context,&out->observed);
        if(st==LT48_OK) {
            out->observed_valid=valid(&out->observed);
            if(!out->observed_valid || memcmp(out->observed.value,step->expected.value,13) ||
               memcmp(out->observed.signature,step->expected.signature,64))st=LT48_DEVICE_MISMATCH;
        }
        if(st==LT48_OK)out->steps_verified++;
    }
    return st;
}
