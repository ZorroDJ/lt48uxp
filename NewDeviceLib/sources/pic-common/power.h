#ifndef LT48_PIC_COMMON_POWER_H
#define LT48_PIC_COMMON_POWER_H
#include <labtool48/labtool48.h>
#include "profile.h"

/* Check the whole source binding before changing any value. Called before
 * compilation, so both compiled constants and runtime variables use the
 * selected policy. It never changes an imported vendor file. */
static inline lt48_status lt48_pic_power_apply(const lt48_pic_power_policy *p,
    lt48_compile_variable *variables, size_t count) {
    if (!p) return LT48_OK;
    if (!variables || !lt48_pic_power_policy_valid(p)) return LT48_SAFETY_REJECTED;
    for (size_t i=0;i<p->count;i++) {
        const lt48_pic_voltage_binding *b=p->bindings+i;
        size_t found=0;
        for (size_t j=0;j<count;j++) if (variables[j].variable_id==b->variable_id) {
            found++;
            if (variables[j].kind!=0 || !variables[j].enabled ||
                variables[j].value!=b->source_value) return LT48_SAFETY_REJECTED;
        }
        if (found!=1) return LT48_SAFETY_REJECTED;
    }
    for (size_t i=0;i<p->count;i++)
        for (size_t j=0;j<count;j++)
            if (variables[j].variable_id==p->bindings[i].variable_id)
                variables[j].value=p->bindings[i].selected_value;
    return LT48_OK;
}
#endif
