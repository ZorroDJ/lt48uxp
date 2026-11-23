#ifndef LT48_PIC_SOURCE_CONTACT_CAPTURE_H
#define LT48_PIC_SOURCE_CONTACT_CAPTURE_H
#include <labtool48/labtool48.h>
typedef lt48_status (*lt48_pic_contact_write)(void *,const unsigned char *,size_t);
typedef lt48_status (*lt48_pic_contact_read)(void *,unsigned char *,size_t);
typedef lt48_status (*lt48_pic_contact_timeout)(void *,unsigned);
typedef lt48_status (*lt48_pic_contact_restore)(void *);
typedef struct {
    lt48_pic_contact_write write;lt48_pic_contact_read read;
    lt48_pic_contact_timeout timeout;lt48_pic_contact_restore restore;void *context;
} lt48_pic_contact_io;
/* Original 0043d670/0043d6f0 capture, including actual 00478b80 upload.
 * The alternate payload and selection must be source-bound by the caller.
 * Restore denotes the source transport vtable+0 call with argument zero; its
 * physical effects and subsequent device FPGA reload belong to the owner.
 * Capture bytes are published only after the entire source route succeeds. */
static inline lt48_status lt48_pic_source_contact_capture(const lt48_pic_contact_io *io,
    unsigned alternate,uint16_t address,const unsigned char *payload,size_t size,unsigned char raw[12]) {
    if(!io||!io->write||!io->read||!io->timeout||!raw||alternate>1)return LT48_INVALID_ARGUMENT;
    if(alternate&&(!io->restore||!payload||!size||size>65536u-(size_t)address))return LT48_INVALID_ARGUMENT;
    lt48_status st=io->timeout(io->context,5000);
    if(alternate&&st==LT48_OK) {
        unsigned char header[]={0x39,0xaa,0x55,(unsigned char)address,(unsigned char)(address>>8),
            (unsigned char)(size-1),(unsigned char)((size-1)>>8)};
        st=io->timeout(io->context,5000);
        if(st==LT48_OK)st=io->write(io->context,header,sizeof(header));
        if(st==LT48_OK)st=io->write(io->context,payload,size);
        /* The original timer is reset here even if either upload write failed. */
        lt48_status timer=io->timeout(io->context,5000);
        if(st==LT48_OK)st=timer;
        if(st==LT48_OK){header[0]=0x3b;st=io->write(io->context,header,5);}
    }
    unsigned char request[]={alternate?0x05:0x47,0xaa,0x55},captured[12];
    if(st==LT48_OK)st=io->write(io->context,request,sizeof(request));
    if(st==LT48_OK)st=io->read(io->context,captured,sizeof(captured));
    if(alternate&&st==LT48_OK)st=io->restore(io->context);
    if(st==LT48_OK)for(unsigned i=0;i<12;i++)raw[i]=captured[i];
    return st;
}
#endif
