#ifndef LT48_PIC_SOURCE_APPLET_GEOMETRY_H
#define LT48_PIC_SOURCE_APPLET_GEOMETRY_H
#include <stdint.h>
#include <limits.h>
/* Applet base 00465168..004653a5: range slots and sparse allocation are
 * distinct. Serialized auxiliary values are retained without inventing their
 * device meaning. Table-reference auxiliaries need their own contract. */
typedef struct {
    uint32_t element_count,element_bytes,traversal_bytes,backing_bytes;
    uint32_t first_element,last_element,first_image_element,last_image_element;
    uint32_t last_file_byte,checksum_last_byte,auxiliary_a,auxiliary_b;
} lt48_pic_source_applet_geometry;
static inline int lt48_pic_source_applet_geometry_prepare(uint32_t count,uint32_t bits,
    uint32_t backing,uint32_t auxiliary_a,uint32_t auxiliary_b,lt48_pic_source_applet_geometry *out) {
    if(!out||!count||!bits||bits>32||!backing||backing>INT32_MAX||
       auxiliary_a>=0x60000000u||auxiliary_b>=0x60000000u)return 0;
    uint32_t width=(bits+7)/8;uint64_t bytes=(uint64_t)count*width;
    if(bytes<8||bytes>INT32_MAX||backing<bytes)return 0;
    lt48_pic_source_applet_geometry g={count,width,(uint32_t)bytes,backing,
        0,count-1,0,count-1,(uint32_t)bytes-1,(uint32_t)bytes-8,auxiliary_a,auxiliary_b};
    *out=g;return 1;
}
#endif
