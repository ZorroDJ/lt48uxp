#ifndef LT48_PIC_SOURCE_APPLET_IDENTITY_H
#define LT48_PIC_SOURCE_APPLET_IDENTITY_H
#include <stddef.h>
#include <stdint.h>
/* Original 00465f30's comparison policy. READ invokes the selected GETID
 * procedure with the seeded words; its transport failure remains distinct in
 * the adapter. Table addresses are protocol addresses, not image offsets. */
typedef struct {uint32_t address;uint16_t expected;} lt48_pic_source_identity_word;
typedef struct {
    uint16_t manufacturer,device;
    unsigned has_callback,table_mode;
    const lt48_pic_source_identity_word *words;size_t count;
} lt48_pic_source_identity_plan;
enum lt48_pic_source_identity_diagnostic {LT48_PIC_ID_MANUFACTURER=1,
    LT48_PIC_ID_DEVICE=2,LT48_PIC_ID_TABLE=4};
typedef int (*lt48_pic_source_identity_read)(void *,uint32_t,uint16_t *,uint16_t *);
typedef void (*lt48_pic_source_identity_log)(void *,unsigned,uint16_t);
static inline int lt48_pic_source_identity_check_profile(const lt48_pic_source_identity_plan *p,
    lt48_pic_source_identity_read read,lt48_pic_source_identity_log log,void *context,int compact) {
    if(!p||!read||(p->count&&!p->words))return -1;
    if(!p->has_callback)return 1;
    uint16_t manufacturer=p->manufacturer,device=p->device;
    if(p->table_mode) {
        for(size_t i=0;i<p->count;i++) {
            device=p->words[i].expected;
            if(!read(context,p->words[i].address,&manufacturer,&device))return 0;
            if(device!=p->words[i].expected) {
                if(log)log(context,LT48_PIC_ID_TABLE,device);
                return 0;
            }
        }
        return 1;
    }
    if(!read(context,0,&manufacturer,&device))return 0;
    if(!p->manufacturer&&!p->device) {
        unsigned low=compact?manufacturer:manufacturer&255u;
        return low==(compact?device:device&255u)&&(low==0||low==255u);
    }
    if(manufacturer!=p->manufacturer&&log)log(context,LT48_PIC_ID_MANUFACTURER,manufacturer);
    if(device!=p->device&&log)log(context,LT48_PIC_ID_DEVICE,device);
    return manufacturer==p->manufacturer&&device==p->device;
}
static inline int lt48_pic_source_identity_check(const lt48_pic_source_identity_plan *p,
    lt48_pic_source_identity_read read,lt48_pic_source_identity_log log,void *context) {
    return lt48_pic_source_identity_check_profile(p,read,log,context,0);
}
static inline int lt48_pic_source_compact_identity_check(const lt48_pic_source_identity_plan *p,
    lt48_pic_source_identity_read read,lt48_pic_source_identity_log log,void *context) {
    return lt48_pic_source_identity_check_profile(p,read,log,context,1);
}
#endif
