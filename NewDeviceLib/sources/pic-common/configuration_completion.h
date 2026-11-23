#ifndef LT48_PIC_CONFIGURATION_COMPLETION_H
#define LT48_PIC_CONFIGURATION_COMPLETION_H
#include "source_session.h"
#include "applet_image.h"

/* Infer legal fields from qualified, independently owned PostConfig bits.
 * PreConfig hints are retained only if they reproduce the requested image.
 * Coupled bits, ambiguous recovery and excessive domains fail closed. */
static inline int lt48_pic_configuration_merge_owner(int a,int b) {
    if(a==-1)return b;
    if(b==-1||a==b)return a;
    return -2;
}
static inline lt48_status lt48_pic_complete_configuration_fields(
    lt48_pic_source_session *session,const lt48_pic_configuration_composition *p,
    const lt48_pic_applet_image_policy *policy,const lt48_pic_addtab_map *mapping) {
    if(!session||!lt48_pic_configuration_composition_valid(p)||!policy||!mapping)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_state state;lt48_status st=lt48_pic_source_session_state_internal(session,&state);
    if(st!=LT48_OK)return st;
    if(state.image_size!=policy->image_span||p->image_span<state.image_size||p->field_count>1024||p->word_count>1024)return LT48_SAFETY_REJECTED;
    int *owner=malloc(p->node_count*sizeof(*owner));
    uint32_t *masks=calloc((p->field_count?p->field_count:1)*p->word_count,sizeof(*masks));
    unsigned char *input=malloc(p->image_span),*output=malloc(p->image_span);
    if(!owner||!masks||!input||!output){free(owner);free(masks);free(input);free(output);return LT48_NO_MEMORY;}
    lt48_pic_composition_value values[1024];size_t existing[1024];unsigned changed=0;
    st=LT48_SAFETY_REJECTED;
    for(size_t i=0;i<p->node_count;i++) {
        const lt48_pic_composition_node *n=p->nodes+i;int field=-1;
        if(n->kind==LT48_PIC_CFG_FIELD) {
            for(size_t j=0;j<p->field_count;j++)if(lt48_pic_composition_field_same(n->object_id,n->category,n->field,p->fields+j))field=(int)j;
            if(field<0)goto done;
        } else if(n->kind==LT48_PIC_CFG_IMAGE) {
            if(n->value/8>=state.image_size)goto done;
        } else if(n->kind==LT48_PIC_CFG_NOT)field=owner[n->first];
        else if(n->kind==LT48_PIC_CFG_SELECT) {
            field=owner[n->second];
            if(n->second!=n->third)field=lt48_pic_configuration_merge_owner(owner[n->first],lt48_pic_configuration_merge_owner(field,owner[n->third]));
        } else if(n->kind==LT48_PIC_CFG_COMPARE) {
            for(unsigned k=0;k<32;k++)field=lt48_pic_configuration_merge_owner(field,lt48_pic_configuration_merge_owner(owner[n->left[k]],owner[n->right[k]]));
        }
        owner[i]=field;
    }
    for(size_t i=0;i<p->field_count;i++) {
        const lt48_pic_composition_field *f=p->fields+i;existing[i]=SIZE_MAX;
        values[i]=(lt48_pic_composition_value){f->object_id,f->category,f->field,f->minimum};
        for(size_t j=0;j<state.configuration_count;j++)if(lt48_pic_composition_field_same(state.configuration[j].object_id,state.configuration[j].category,state.configuration[j].field,f)) {
            if(existing[i]!=SIZE_MAX)goto done;
            existing[i]=j;uint32_t v=state.configuration[j].value;
            if(v>=f->minimum&&v<=f->maximum)values[i].value=v;
        }
    }
    memset(input,255,p->image_span);memcpy(input,state.image,state.image_size);
    if(!lt48_pic_configuration_compose(p,input,p->image_span,values,p->field_count,output))goto done;
    for(size_t i=0;i<p->word_count;i++) {
        const lt48_pic_composition_word *w=p->words+i;
        if((uint64_t)w->image_offset+w->width>state.image_size)goto done;
        for(unsigned k=0;k<8u*w->width;k++) {
            uint32_t address=w->image_offset+k/8;unsigned mask=lt48_pic_applet_image_mask(policy,mapping,address)&(1u<<(k%8));
            if(!mask)continue;
            int field=owner[w->bits[k]];
            if(field==-2)goto done;
            if(field>=0)masks[(size_t)field*p->word_count+i]|=UINT32_C(1)<<k;
            else if((output[address]^state.image[address])&mask)goto done;
        }
    }
    for(size_t f=0;f<p->field_count;f++) {
        const lt48_pic_composition_field *field=p->fields+f;
        uint64_t width=(uint64_t)field->maximum-field->minimum+1;
        unsigned hint_valid=existing[f]!=SIZE_MAX&&state.configuration[existing[f]].value>=field->minimum&&state.configuration[existing[f]].value<=field->maximum;
        unsigned solutions=0,selected=0;uint32_t winner=0;int has_bits=0;
        for(size_t w=0;w<p->word_count;w++)if(masks[f*p->word_count+w])has_bits=1;
        if(!has_bits){if(!hint_valid)goto done;continue;}
        uint64_t hints=hint_valid?1:0;
        for(uint64_t trial=0;;trial++) {
            if(trial<hints)values[f].value=state.configuration[existing[f]].value;
            else {
                if(width>4096)goto done;
                uint64_t at=trial-hints;if(at>=width)break;
                values[f].value=field->minimum+(uint32_t)at;
            }
            if(!lt48_pic_configuration_compose(p,input,p->image_span,values,p->field_count,output))goto done;
            int same=1;
            for(size_t i=0;i<p->word_count&&same;i++) {
                const lt48_pic_composition_word *w=p->words+i;uint32_t mask=masks[f*p->word_count+i];
                for(unsigned j=0;j<w->width;j++)if((output[w->image_offset+j]^state.image[w->image_offset+j])&((mask>>(8*j))&255u)){same=0;break;}
            }
            if(!same)continue;
            if(trial<hints){winner=values[f].value;selected=1;break;}
            if(++solutions>1)goto done;
            winner=values[f].value;selected=1;
        }
        if(!selected)goto done;
        values[f].value=winner;
        if(existing[f]==SIZE_MAX||state.configuration[existing[f]].value!=winner)changed++;
    }
    if(!changed){st=LT48_OK;goto done;}
    size_t missing=0;for(size_t i=0;i<p->field_count;i++)if(existing[i]==SIZE_MAX)missing++;
    if(state.configuration_count+missing>1024)goto done;
    size_t count=state.configuration_count;
    lt48_runtime_configuration_value *complete=malloc((count+missing)*sizeof(*complete));
    if(!complete){st=LT48_NO_MEMORY;goto done;}
    memcpy(complete,state.configuration,count*sizeof(*complete));
    for(size_t i=0;i<p->field_count;i++) {
        size_t at=existing[i]==SIZE_MAX?count++:existing[i];
        complete[at]=(lt48_runtime_configuration_value){values[i].object_id,values[i].category,values[i].field,values[i].value};
    }
    state.configuration=complete;state.configuration_count=count;
    st=lt48_pic_source_session_restore_internal(session,&state);free(complete);
done:free(owner);free(masks);free(input);free(output);return st;
}
#endif
