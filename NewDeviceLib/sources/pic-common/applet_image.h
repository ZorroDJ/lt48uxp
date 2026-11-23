/* Byte-image policy for applet protocol owners. Generated profiles provide
 * independently qualified masks, region bounds and voltage policy. No device
 * name or catalogue membership selects behavior here. */
#ifndef LT48_PIC_APPLET_IMAGE_H
#define LT48_PIC_APPLET_IMAGE_H
#include "addtab_map.h"
typedef struct {
    uint32_t image_span,id_address,id_count,configuration_address,configuration_count;
    const unsigned char *configuration_mask,*deferred_mask,*secure_mask;
    uint32_t nominal_voltage,low_voltage,high_voltage;
    /* Explicitly selected configuration inside the ordinary storage map. */
    unsigned configuration_in_storage;
} lt48_pic_applet_image_policy;
static inline int lt48_pic_applet_image_overlap(uint32_t a,uint32_t n,uint32_t b,uint32_t m) {
    return n&&m&&(uint64_t)a+n>b&&(uint64_t)b+m>a;
}
static inline int lt48_pic_applet_image_policy_valid(const lt48_pic_applet_image_policy *p,const lt48_pic_addtab_map *map) {
    if(!p||!p->image_span||!lt48_pic_addtab_map_valid(map)||map->element_bytes!=1||
       !p->configuration_count||!p->configuration_mask||!p->deferred_mask||!p->secure_mask||
       (uint64_t)p->configuration_address+p->configuration_count>p->image_span||
       (uint64_t)p->id_address+p->id_count>p->image_span||
       !p->low_voltage||p->low_voltage>p->nominal_voltage||p->nominal_voltage>p->high_voltage||
       lt48_pic_applet_image_overlap(p->id_address,p->id_count,p->configuration_address,p->configuration_count))return 0;
    for(uint32_t i=0;i<p->configuration_count;i++)
        if((p->deferred_mask[i]|p->secure_mask[i])&~p->configuration_mask[i])return 0;
    if(p->configuration_in_storage>1)return 0;
    unsigned contained=0;
    uint64_t total=0;
    for(size_t i=0;i<map->count;i++) {
        const lt48_pic_map_segment *s=map->segments+i;total+=s->length;
        if((uint64_t)s->image_offset+s->length>p->image_span||
           lt48_pic_applet_image_overlap(s->image_offset,s->length,p->id_address,p->id_count))return 0;
        if(lt48_pic_applet_image_overlap(s->image_offset,s->length,p->configuration_address,p->configuration_count)) {
            if(!p->configuration_in_storage||p->configuration_address<s->image_offset||
               (uint64_t)p->configuration_address+p->configuration_count>(uint64_t)s->image_offset+s->length)return 0;
            contained++;
        }
        for(size_t j=0;j<i;j++)if(lt48_pic_applet_image_overlap(s->image_offset,s->length,map->segments[j].image_offset,map->segments[j].length))return 0;
    }
    return total==map->dense_capacity&&contained==p->configuration_in_storage;
}
/* Storage-only image ranges for comparisons and preservation. Configuration
 * is checked once using its own implemented-bit masks, even when embedded. */
static inline size_t lt48_pic_applet_image_storage_ranges(const lt48_pic_applet_image_policy *p,
    uint32_t address,uint32_t count,uint32_t starts[2],uint32_t lengths[2]) {
    if(!count)return 0;
    if(!p->configuration_in_storage||!lt48_pic_applet_image_overlap(address,count,p->configuration_address,p->configuration_count)) {
        starts[0]=address;lengths[0]=count;return 1;
    }
    size_t n=0;uint32_t end=address+count,cfg_end=p->configuration_address+p->configuration_count;
    if(address<p->configuration_address){starts[n]=address;lengths[n++]=p->configuration_address-address;}
    if(end>cfg_end){starts[n]=cfg_end;lengths[n++]=end-cfg_end;}
    return n;
}
/* Call after validating the policy/map together. Configuration reserved bits
 * may be present in an input image, but are not compared as physical storage. */
static inline unsigned lt48_pic_applet_image_mask(const lt48_pic_applet_image_policy *p,const lt48_pic_addtab_map *map,uint32_t address) {
    if(address>=p->image_span)return 0;
    if(address>=p->configuration_address&&address-p->configuration_address<p->configuration_count)
        return p->configuration_mask[address-p->configuration_address];
    if(address>=p->id_address&&address-p->id_address<p->id_count)return 255;
    for(size_t i=0;i<map->count;i++)if(address>=map->segments[i].image_offset&&address-map->segments[i].image_offset<map->segments[i].length)return 255;
    return 0;
}
#endif
