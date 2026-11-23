#ifndef LT48_PIC_CONFIGURATION_COMPOSITION_H
#define LT48_PIC_CONFIGURATION_COMPOSITION_H
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Generated source composition data. No silicon masks, GUI option list,
 * preservation policy or operation admission is implied by a valid graph. */
enum { LT48_PIC_CFG_CONSTANT,LT48_PIC_CFG_FIELD,LT48_PIC_CFG_IMAGE,
       LT48_PIC_CFG_NOT,LT48_PIC_CFG_COMPARE,LT48_PIC_CFG_SELECT };
typedef struct {
    uint8_t kind;
    uint32_t value;
    int object_id;
    uint16_t category,field;
    uint32_t first,second,third;
    const uint32_t *left,*right;
} lt48_pic_composition_node;
typedef struct {
    int object_id;
    uint16_t category,field;
    uint32_t minimum,maximum;
    uint8_t raw_storage_domain;
} lt48_pic_composition_field;
typedef struct { int object_id;uint16_t category,field;uint32_t value; } lt48_pic_composition_value;
typedef struct { uint32_t image_offset;uint8_t width;const uint32_t *bits; } lt48_pic_composition_word;
typedef struct {
    const char *context;
    uint32_t image_span;
    const lt48_pic_composition_node *nodes;size_t node_count;
    const lt48_pic_composition_field *fields;size_t field_count;
    const lt48_pic_composition_word *words;size_t word_count;
} lt48_pic_configuration_composition;
typedef struct {
    const char *key,*profile,*definition_file;
    uint32_t definition,family,pinmap;
    const lt48_pic_configuration_composition *composition;
} lt48_pic_composition_member;

static inline int lt48_pic_composition_field_same(int object,uint16_t category,uint16_t field,const lt48_pic_composition_field *p) {
    return object==p->object_id && category==p->category && field==p->field;
}
static inline int lt48_pic_configuration_composition_valid(const lt48_pic_configuration_composition *p) {
    if(!p || !p->image_span || !p->nodes || !p->node_count || p->node_count>65536 ||
       (p->field_count&&!p->fields) || p->field_count>1024 || !p->words || !p->word_count)return 0;
    for(size_t i=0;i<p->field_count;i++) {
        const lt48_pic_composition_field *f=p->fields+i;
        if(f->object_id < -1 || f->object_id>255 || f->category>2 || f->minimum>f->maximum ||
           f->raw_storage_domain>1 || (f->category==0 && f->maximum>1) || (f->category==1 && f->maximum>65535))return 0;
        for(size_t j=0;j<i;j++)if(lt48_pic_composition_field_same(f->object_id,f->category,f->field,p->fields+j))return 0;
    }
    for(size_t i=0;i<p->node_count;i++) {
        const lt48_pic_composition_node *n=p->nodes+i;
        if(n->kind==LT48_PIC_CFG_CONSTANT) {if(n->value>1)return 0;}
        else if(n->kind==LT48_PIC_CFG_FIELD) {
            size_t count=0;
            if(n->value>31)return 0;
            for(size_t j=0;j<p->field_count;j++)if(lt48_pic_composition_field_same(n->object_id,n->category,n->field,p->fields+j))count++;
            if(count!=1)return 0;
        } else if(n->kind==LT48_PIC_CFG_IMAGE) {if((uint64_t)n->value>=(uint64_t)p->image_span*8)return 0;}
        else if(n->kind==LT48_PIC_CFG_NOT) {if(n->first>=i)return 0;}
        else if(n->kind==LT48_PIC_CFG_SELECT) {if(n->first>=i || n->second>=i || n->third>=i)return 0;}
        else if(n->kind==LT48_PIC_CFG_COMPARE) {
            if(!n->left || !n->right || n->value>8)return 0;
            for(size_t j=0;j<32;j++)if(n->left[j]>=i || n->right[j]>=i)return 0;
        } else return 0;
    }
    uint64_t end=0;
    for(size_t i=0;i<p->word_count;i++) {
        const lt48_pic_composition_word *w=p->words+i;
        if(!w->width || w->width>4 || !w->bits || w->image_offset<end || (uint64_t)w->image_offset+w->width>p->image_span)return 0;
        for(size_t j=0;j<8u*w->width;j++)if(w->bits[j]>=p->node_count)return 0;
        end=(uint64_t)w->image_offset+w->width;
    }
    return 1;
}

/* All output changes are committed together after graph evaluation succeeds.
 * Input/output may alias. Image nodes refer to the original input; source reads
 * after writes have already been composed by the descriptor extractor. */
static inline int lt48_pic_configuration_compose(const lt48_pic_configuration_composition *p,
    const unsigned char *input,size_t span,const lt48_pic_composition_value *values,size_t count,unsigned char *output) {
    if(!p || !input || !output || span!=p->image_span || (count&&!values) || count>1024 ||
       !lt48_pic_configuration_composition_valid(p))return 0;
    for(size_t i=0;i<count;i++) {
        size_t found=0;
        for(size_t j=0;j<i;j++)if(values[i].object_id==values[j].object_id && values[i].category==values[j].category && values[i].field==values[j].field)return 0;
        for(size_t j=0;j<p->field_count;j++)if(lt48_pic_composition_field_same(values[i].object_id,values[i].category,values[i].field,p->fields+j)) {
            found++;
            if(values[i].value<p->fields[j].minimum || values[i].value>p->fields[j].maximum)return 0;
        }
        if(found!=1)return 0;
    }
    unsigned char *computed=(unsigned char *)malloc(p->node_count),*image=(unsigned char *)malloc(span);
    if(!computed || !image) {free(computed);free(image);return 0;}
    int ok=0;
    for(size_t i=0;i<p->node_count;i++) {
        const lt48_pic_composition_node *n=p->nodes+i;unsigned bit=0;
        if(n->kind==LT48_PIC_CFG_CONSTANT)bit=n->value;
        else if(n->kind==LT48_PIC_CFG_FIELD) {
            size_t found=0;
            for(size_t j=0;j<count;j++)if(values[j].object_id==n->object_id && values[j].category==n->category && values[j].field==n->field) {
                found++;bit=(values[j].value>>n->value)&1u;
            }
            if(found!=1)goto done;
        } else if(n->kind==LT48_PIC_CFG_IMAGE)bit=(input[n->value/8]>>(n->value%8))&1u;
        else if(n->kind==LT48_PIC_CFG_NOT)bit=1u-computed[n->first];
        else if(n->kind==LT48_PIC_CFG_SELECT)bit=computed[computed[n->first]?n->second:n->third];
        else if(n->kind==LT48_PIC_CFG_COMPARE) {
            uint32_t left=0,right=0;
            for(unsigned k=0;k<32;k++) {left|=(uint32_t)computed[n->left[k]]<<k;right|=(uint32_t)computed[n->right[k]]<<k;}
            switch(n->value) {
            case 0:bit=left==right;break;case 1:bit=left!=right;break;
            case 2:bit=left<right;break;case 3:bit=left<=right;break;
            case 4:bit=left>right;break;case 5:bit=left>=right;break;
            case 6:bit=!!(left&right);break;case 7:bit=!!(left|right);break;
            case 8:if(!right)goto done;bit=!!(left%right);break;
            default:goto done;
            }
        }
        computed[i]=(unsigned char)bit;
    }
    memcpy(image,input,span);
    for(size_t i=0;i<p->word_count;i++) {
        const lt48_pic_composition_word *w=p->words+i;uint32_t value=0;
        for(unsigned k=0;k<8u*w->width;k++)value|=(uint32_t)computed[w->bits[k]]<<k;
        for(unsigned k=0;k<w->width;k++)image[w->image_offset+k]=(unsigned char)(value>>(8*k));
    }
    memcpy(output,image,span);ok=1;
done:free(computed);free(image);return ok;
}
#endif
