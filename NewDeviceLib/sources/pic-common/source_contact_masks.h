#ifndef LT48_PIC_SOURCE_CONTACT_MASKS_H
#define LT48_PIC_SOURCE_CONTACT_MASKS_H
#include <labtool48/labtool48.h>
#include <string.h>
typedef struct {uint32_t required[2],forbidden[2],auxiliary[2],adapter[2];unsigned alternate;} lt48_pic_contact_masks;
typedef struct {uint32_t observed[2],missing[2],unexpected[2];} lt48_pic_contact_result;
static inline unsigned lt48_pic_contact_reverse(unsigned byte) {
    unsigned value=0;for(unsigned i=0;i<8;i++)value=(value<<1)|((byte>>i)&1u);return value;
}
static inline uint32_t lt48_pic_contact_sample(const unsigned char *raw,unsigned side) {
    if(side)return (uint32_t)raw[3]|(uint32_t)raw[4]<<8|(uint32_t)raw[5]<<16;
    return lt48_pic_contact_reverse(raw[2])|lt48_pic_contact_reverse(raw[1])<<8|lt48_pic_contact_reverse(raw[0])<<16;
}
/* Original 0043b450 through its contact acceptance branch. Detailed insertion
 * and adapter diagnostics are separate from the match decision. */
static inline int lt48_pic_source_contact_match(const lt48_pic_contact_masks *m,
    const unsigned char raw[12],lt48_pic_contact_result *out) {
    if(!m||!raw||!out)return 0;
    lt48_pic_contact_result r={0};
    for(unsigned side=0;side<2;side++)r.observed[side]=lt48_pic_contact_sample(raw,side);
    unsigned active=r.observed[0]||r.observed[1];
    for(unsigned side=0;side<2;side++) {
        if(!m->alternate)r.observed[side]|=lt48_pic_contact_sample(raw+6,side)&m->auxiliary[side];
        else if(active)r.observed[side]|=m->auxiliary[side];
        r.missing[side]=~r.observed[side]&m->required[side];
        r.unexpected[side]=r.observed[side]&m->forbidden[side];
    }
    *out=r;return !(r.missing[0]||r.missing[1]||r.unexpected[0]||r.unexpected[1]);
}
static inline void lt48_pic_contact_pack(const unsigned char pins[48],unsigned value,uint32_t pair[2]) {
    pair[0]=pair[1]=0;
    for(unsigned i=0;i<24;i++)pair[0]=(pair[0]<<1)|(pins[i]==value);
    for(unsigned i=48;i>24;i--)pair[1]=(pair[1]<<1)|(pins[i-1]==value);
}
/* Source 0041e1b0 mask construction. Raw TPinCode data starts at group count;
 * package pin count is carried separately by the REZ resource. */
static inline lt48_status lt48_pic_source_contact_masks(unsigned count,const unsigned char *data,size_t size,
    lt48_pic_contact_masks *out) {
    if(!data||!out||!count||count>48||size<3)return LT48_INVALID_ARGUMENT;
    const unsigned char *groups[3]={0};unsigned lengths[3]={0};size_t offset=3;
    unsigned center=(48-count)/2;
    for(unsigned i=0;i<data[0];i++) {
        if(size-offset<4)return LT48_FORMAT_ERROR;
        unsigned kind=data[offset]|(unsigned)data[offset+1]<<8;
        unsigned n=data[offset+2]|(unsigned)data[offset+3]<<8;offset+=4;
        if(n>size-offset)return LT48_FORMAT_ERROR;
        for(unsigned j=0;j<n;j++) {
            int pin=(int)center+(data[offset+j]<128?(int)data[offset+j]:(int)data[offset+j]-256);
            if(pin<0||pin>=48)return LT48_FORMAT_ERROR;
        }
        if(kind>=1&&kind<=3){if(groups[kind-1])return LT48_FORMAT_ERROR;groups[kind-1]=data+offset;lengths[kind-1]=n;}
        offset+=n;
    }
    if(size-offset!=(size_t)data[1]*4)return LT48_FORMAT_ERROR;
    lt48_pic_contact_masks result={0};unsigned char pins[48]={0};
    memset(pins+center,1,count);lt48_pic_contact_pack(pins,0,result.forbidden);
    if(groups[2])for(unsigned i=0;i<lengths[2];i++) {
        unsigned char v=groups[2][i];pins[(int)center+(v<128?(int)v:(int)v-256)]=0;
    }
    lt48_pic_contact_pack(pins,1,result.required);
    for(unsigned g=0;g<2;g++)if(groups[g]) {
        memset(pins,1,sizeof(pins));
        for(unsigned i=0;i<lengths[g];i++){unsigned char v=groups[g][i];pins[(int)center+(v<128?(int)v:(int)v-256)]=0;}
        lt48_pic_contact_pack(pins,0,g?result.adapter:result.auxiliary);
    }
    result.alternate=data[2]!=0;*out=result;return LT48_OK;
}
#endif
