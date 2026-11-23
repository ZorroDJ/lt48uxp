#ifndef LT48_PIC_COMMON_PROFILE_H
#define LT48_PIC_COMMON_PROFILE_H
#include <stddef.h>
#include <stdint.h>
#include "classic14_protocol.h"

/* Silicon addresses and host byte offsets are deliberately separate. */
typedef enum {
    LT48_PIC_COMPACT_LEGACY12 = 1,
    LT48_PIC_COMPACT_LEGACY16,
    LT48_PIC_COMPACT_ROWS,
    LT48_PIC_COMPACT_FPGA,
    LT48_PIC_APPLET_STREAM
} lt48_pic_strategy;
typedef enum {
    LT48_PIC_REGION_CODE = 1, LT48_PIC_REGION_IDS,
    LT48_PIC_REGION_CONFIGURATION, LT48_PIC_REGION_EEPROM,
    LT48_PIC_REGION_CALIBRATION
} lt48_pic_region_kind;
typedef struct {
    uint32_t address, count, image_offset;
    uint32_t compare_mask;
    uint16_t image_stride;
    uint8_t element_bytes, kind;
} lt48_pic_region;
typedef struct { uint32_t address, count; } lt48_pic_window;
typedef struct {
    uint32_t image_offset;
    uint16_t compare_mask, reserved_ones, unprotected_ones, secure_clear;
} lt48_pic_config_word;
/* Optional authored protocol action for a whole semantic group. The window
 * is in silicon word addresses; logical pins and timing retain source roles. */
typedef struct {
    uint32_t address, count;
    uint16_t data_pin, clock_pin, timing_variable, row_pending_buffer;
} lt48_pic_word_program_policy;
/* The low byte retains the source wait-variable ID. The upper byte selects
 * the authored wire protocol (0 = existing MSB, 1 = six-bit Load-PC).
 * Preserve the six-field stored schema and historical generated registries. */
enum { LT48_PIC_WORD_MSB = 0, LT48_PIC_WORD_ICSP6 = 0x100 };
static inline unsigned lt48_pic_word_protocol(const lt48_pic_word_program_policy *p) {
    return p->timing_variable>>8;
}
static inline unsigned lt48_pic_word_timing_variable(const lt48_pic_word_program_policy *p) {
    return p->timing_variable&255;
}
/* Shared opt-in transformation of source PROGRAM begin/wait/end triplets.
 * Policy selection belongs to a qualified source profile, never a part name. */
typedef struct {
    uint16_t data_pin, clock_pin, code_wait, data_wait;
} lt48_pic_internal_program_policy;
/* Some original callbacks retain an unreachable data-memory branch on parts
 * without EEPROM. Preserve the four-field descriptor and tag only that source
 * shape; the actual silicon extent remains zero. Complete traversal/effect
 * qualification must prove that no public operation reaches the branch. */
enum { LT48_PIC_INTERNAL_DORMANT_DATA = 0x100 };
static inline unsigned lt48_pic_internal_data_wait(const lt48_pic_internal_program_policy *p) {
    return p->data_wait&255;
}
/* Authored voltage choices retain the exact source values they replace.
 * All values and limits are centivolts, not DAC codes or percentages. */
typedef struct {
    uint16_t variable_id, source_value, selected_value, minimum, maximum;
} lt48_pic_voltage_binding;
typedef struct {
    const lt48_pic_voltage_binding *bindings;
    size_t count;
} lt48_pic_power_policy;
static inline int lt48_pic_power_policy_valid(const lt48_pic_power_policy *p) {
    if (!p) return 1; /* Existing qualified profiles retain source behavior. */
    if (!p->bindings || !p->count || p->count>256) return 0;
    for (size_t i=0;i<p->count;i++) {
        const lt48_pic_voltage_binding *b=p->bindings+i;
        if (b->variable_id>255 || !b->minimum || b->minimum>b->maximum ||
            b->selected_value<b->minimum || b->selected_value>b->maximum) return 0;
        for (size_t j=0;j<i;j++)
            if (p->bindings[j].variable_id==b->variable_id) return 0;
    }
    return 1;
}
/* Source-bound compact wait substitutions. Offsets refer to original compact
 * bytes, not compiled addresses. Both source and selected variables are checked. */
typedef struct {
    uint16_t source_offset, source_variable, selected_variable;
    uint32_t minimum_us;
} lt48_pic_wait_binding;
typedef struct {
    uint16_t operation_id;
    const lt48_pic_wait_binding *bindings;
    size_t count;
} lt48_pic_wait_policy;
/* Optional authored quiet interval after source CLK/DAT-low initialization,
 * before its supply/entry sequence. Source instructions remain otherwise intact. */
typedef struct {
    uint16_t operation_id, clock_pin, data_pin;
    /* Low 20 bits: quiet interval. Optional high bits explicitly qualify a
     * literal ID comparison: bits20..23 revision width, bits24..31 source
     * ID variable. Existing four-field descriptors retain their ABI. */
    uint32_t quiet_us;
    /* Optional independent identity for a guarded source byte-selector check.
     * The source selector remains unchanged for configuration interpretation. */
    uint16_t id_source_value, id_expected, id_mask;
} lt48_pic_entry_policy;
static inline uint32_t lt48_pic_entry_quiet_us(const lt48_pic_entry_policy *p) {
    return p->quiet_us & 0xfffffu;
}
static inline unsigned lt48_pic_entry_revision_bits(const lt48_pic_entry_policy *p) {
    return (p->quiet_us >> 20) & 15u;
}
static inline unsigned lt48_pic_entry_id_variable(const lt48_pic_entry_policy *p) {
    return p->quiet_us >> 24;
}
/* Architecture contract for the source classic-14 command family. Numeric
 * bounds and entry selector are data; no part-name dispatch is permitted. */
typedef struct {
    const lt48_pic_entry_policy *entry;
    uint16_t data_pin,clock_pin,code_wait_variable;
    uint32_t source_code_wait_us,selected_code_wait_us;
    const lt48_pic_classic_protocol *protocol; /* NULL: existing source executor. */
} lt48_pic_classic14_policy;
typedef struct {
    uint16_t definition, family, pinmap;
    uint8_t strategy, variable_count, operation_count;
    uint32_t image_span, program_words, size_seed;
    const lt48_pic_region *regions;
    size_t region_count;
    const lt48_pic_window *read_windows, *program_windows;
    size_t window_count;
    const lt48_pic_config_word *configuration;
    size_t configuration_count;
    const lt48_pic_word_program_policy *word_program;
    const lt48_pic_internal_program_policy *internal_program;
    const lt48_pic_power_policy *power;
    const lt48_pic_wait_policy *wait;
    const lt48_pic_entry_policy *entry;
    /* Both zero retain the historical paired window_count. Explicit domains
     * require both counts and window_count=0, preventing ambiguous layouts. */
    size_t read_window_count, program_window_count;
    const lt48_pic_classic14_policy *classic14;
} lt48_pic_profile;

static inline size_t lt48_pic_read_window_count(const lt48_pic_profile *p) {
    return p->read_window_count?p->read_window_count:p->window_count;
}
static inline size_t lt48_pic_program_window_count(const lt48_pic_profile *p) {
    return p->program_window_count?p->program_window_count:p->window_count;
}

static inline int lt48_pic_profile_valid(const lt48_pic_profile *p) {
    if (!p || !p->image_span || !p->regions || !p->region_count ||
            !lt48_pic_power_policy_valid(p->power)) return 0;
    if(p->classic14) {
        const lt48_pic_classic14_policy *c=p->classic14;
        const lt48_pic_entry_policy *e=c->entry;
        if(c->protocol) {
            if(p->strategy!=LT48_PIC_COMPACT_ROWS||p->entry||p->wait||p->internal_program||p->word_program||
               !e||e->operation_id!=3||e->data_pin!=c->data_pin||e->clock_pin!=c->clock_pin||
               c->data_pin>255||c->clock_pin>255||c->data_pin==c->clock_pin||
               c->code_wait_variable||c->source_code_wait_us||c->selected_code_wait_us||
               !lt48_pic_classic_protocol_valid(c->protocol)||p->program_words!=c->protocol->program_words||
               p->configuration_count!=c->protocol->configuration_words||
               e->id_mask!=0x3fe0||(e->id_expected&~e->id_mask))return 0;
            unsigned code=0,data=0,ids=0,config=0;
            for(size_t i=0;i<p->region_count;i++) {
                const lt48_pic_region *r=p->regions+i;
                if(r->element_bytes!=2||r->image_stride!=2||r->image_offset!=r->address*2)return 0;
                if(r->kind==LT48_PIC_REGION_CODE&&r->address==0&&r->count==c->protocol->program_words&&r->compare_mask==0x3fff)code++;
                else if(r->kind==LT48_PIC_REGION_IDS&&r->address==0x2000&&r->count==4&&r->compare_mask==0x3fff)ids++;
                else if(r->kind==LT48_PIC_REGION_EEPROM&&r->address==0x2100&&r->count==c->protocol->eeprom_bytes&&r->compare_mask==0xff)data++;
                else if(r->kind==LT48_PIC_REGION_CONFIGURATION&&r->count==1&&r->address>=0x2007&&
                    r->address<0x2007+c->protocol->configuration_words&&r->compare_mask==c->protocol->configuration_mask[r->address-0x2007])config++;
                else return 0;
            }
            if(code!=1||data!=(c->protocol->eeprom_bytes?1u:0u)||ids!=1||config!=c->protocol->configuration_words)return 0;
        } else {
        if(p->strategy!=LT48_PIC_COMPACT_ROWS||p->entry||p->wait||p->internal_program||p->word_program||
           !e||e->operation_id!=3||e->data_pin!=c->data_pin||e->clock_pin!=c->clock_pin||
           c->data_pin!=48||c->clock_pin!=49||c->code_wait_variable!=8||
           c->source_code_wait_us!=2500||c->selected_code_wait_us!=3000||
           lt48_pic_entry_quiet_us(e)||lt48_pic_entry_revision_bits(e)!=5||
           lt48_pic_entry_id_variable(e)!=16||e->id_mask!=0x3fe0||
           (e->id_expected&~e->id_mask)||e->id_source_value>0x3fff)return 0;
        }
    }
    if(p->entry && (p->strategy!=LT48_PIC_COMPACT_ROWS || p->entry->operation_id>=32 ||
        p->entry->clock_pin>255 || p->entry->data_pin>255 ||
        p->entry->clock_pin==p->entry->data_pin || !lt48_pic_entry_quiet_us(p->entry) ||
        lt48_pic_entry_quiet_us(p->entry)>1000000 ||
        lt48_pic_entry_revision_bits(p->entry)>=14 ||
        (!lt48_pic_entry_revision_bits(p->entry) && lt48_pic_entry_id_variable(p->entry)) ||
        (p->entry->id_mask && (!lt48_pic_entry_revision_bits(p->entry) ||
          p->entry->id_mask!=(0x3fffu&~((1u<<lt48_pic_entry_revision_bits(p->entry))-1u)) ||
          (p->entry->id_expected&~p->entry->id_mask) || p->entry->id_source_value>0x3fff)) ||
        (!p->entry->id_mask && (p->entry->id_source_value||p->entry->id_expected))))return 0;
    for (size_t i=0;i<p->region_count;i++) {
        const lt48_pic_region *a=&p->regions[i];
        uint64_t last=(uint64_t)a->image_offset+(uint64_t)(a->count?a->count-1:0)*a->image_stride+a->element_bytes;
        if (!a->count || !a->element_bytes || a->element_bytes>4 ||
            a->image_stride<a->element_bytes || last>p->image_span ||
            (uint64_t)a->address+a->count>UINT64_C(0x100000000) ||
            !a->compare_mask || (a->element_bytes<4 && (a->compare_mask>>(8*a->element_bytes)))) return 0;
        for (size_t j=0;j<i;j++) {
            const lt48_pic_region *b=&p->regions[j];
            /* Current descriptor is one address space with contiguous elements.
             * Separate spaces must receive a typed extension, not alias here. */
            if ((uint64_t)a->address<(uint64_t)b->address+b->count &&
                (uint64_t)b->address<(uint64_t)a->address+a->count) return 0;
            uint64_t bend=(uint64_t)b->image_offset+(uint64_t)(b->count-1)*b->image_stride+b->element_bytes;
            if (a->image_offset<bend && b->image_offset<last) return 0;
        }
    }
    if(p->strategy==LT48_PIC_COMPACT_ROWS) {
        if((!p->read_window_count!=!p->program_window_count) ||
            (p->read_window_count && p->window_count) ||
            !lt48_pic_read_window_count(p) || !lt48_pic_program_window_count(p) ||
            !p->read_windows || !p->program_windows ||
            !p->configuration_count || !p->configuration) return 0;
        for(unsigned domain=0;domain<2;domain++) {
            const lt48_pic_window *windows=domain?p->program_windows:p->read_windows;
            size_t count=domain?lt48_pic_program_window_count(p):lt48_pic_read_window_count(p);
            for(size_t i=0;i<count;i++) {
                const lt48_pic_window *a=&windows[i];
                if(!a->count || ((uint64_t)a->address+a->count)*2>p->image_span) return 0;
            }
        }
        for(size_t i=0;i<p->configuration_count;i++) {
            const lt48_pic_config_word *c=&p->configuration[i];
            /* A fully reserved 14-bit word has no comparable device bits.
             * It still occupies its source frame and must normalize to ones. */
            if((uint64_t)c->image_offset+2>p->image_span ||
                (!c->compare_mask && c->reserved_ones!=0x3fff) ||
                (c->reserved_ones & c->compare_mask) ||
                ((c->unprotected_ones|c->secure_clear)&~c->compare_mask)) return 0;
        }
    }
    if(p->word_program) {
        const lt48_pic_word_program_policy *w=p->word_program;
        if(p->strategy!=LT48_PIC_COMPACT_ROWS || !w->count ||
            (uint64_t)w->address+w->count>0x10000 || w->data_pin>255 ||
            w->clock_pin>255 || w->data_pin==w->clock_pin ||
            w->timing_variable>511 || w->row_pending_buffer>=76) return 0;
        int region=0,window=0;
        for(size_t i=0;i<p->region_count;i++) {
            const lt48_pic_region *r=&p->regions[i];
            if(r->kind==LT48_PIC_REGION_IDS && r->element_bytes==2 &&
                w->address>=r->address &&
                (uint64_t)w->address+w->count<=(uint64_t)r->address+r->count)region=1;
        }
        for(size_t i=0;i<lt48_pic_program_window_count(p);i++) {
            const lt48_pic_window *r=&p->program_windows[i];
            if(w->address>=r->address &&
                (uint64_t)w->address+w->count<=(uint64_t)r->address+r->count)window=1;
        }
        if(!region || !window)return 0;
    }
    if(p->internal_program) {
        const lt48_pic_internal_program_policy *w=p->internal_program;
        if(p->strategy!=LT48_PIC_COMPACT_ROWS || p->word_program ||
            w->data_pin>255 || w->clock_pin>255 || w->data_pin==w->clock_pin ||
            w->code_wait>255 || w->data_wait>511 || w->code_wait==lt48_pic_internal_data_wait(w))
            return 0;
        if(w->data_wait&LT48_PIC_INTERNAL_DORMANT_DATA)
            for(size_t i=0;i<p->region_count;i++)
                if(p->regions[i].kind==LT48_PIC_REGION_EEPROM && p->regions[i].count)return 0;
    }
    if(p->wait) {
        const lt48_pic_wait_policy *w=p->wait;
        if(p->strategy!=LT48_PIC_COMPACT_ROWS || p->internal_program ||
           w->operation_id>=32 || !w->bindings || !w->count || w->count>256)return 0;
        for(size_t i=0;i<w->count;i++) {
            const lt48_pic_wait_binding *b=w->bindings+i;
            if(b->source_offset<2 || b->source_variable>255 ||
               b->selected_variable>255 || b->source_variable==b->selected_variable ||
               !b->minimum_us)return 0;
            for(size_t j=0;j<i;j++)if(w->bindings[j].source_offset==b->source_offset)return 0;
        }
    }
    return 1;
}
static inline unsigned lt48_pic_profile_mask(const lt48_pic_profile *p,uint32_t address) {
    for(size_t i=0;i<p->region_count;i++) {
        const lt48_pic_region *r=&p->regions[i];
        if(address>=r->address && (uint64_t)address-r->address<r->count) return r->compare_mask;
    }
    return 0;
}
#endif
