#ifndef LT48_SOURCE_ERASE_TIMING_H
#define LT48_SOURCE_ERASE_TIMING_H
#include "source_session.h"
#include <stdlib.h>
#include <string.h>

/* Candidate common-engine policy: adjust only the private session's declared
 * erase-time magnitude. Original callbacks, applets, units and voltages remain
 * bound to their source. Integration/admission requires the cohort's full gates. */
static inline lt48_status lt48_pic_source_raise_erase_wait_shape(
    lt48_pic_source_session *session,uint16_t variable_id,uint32_t minimum_us,
    uint16_t expected_magnitude) {
    if(!session||!minimum_us||minimum_us>255000)return LT48_INVALID_ARGUMENT;
    if(expected_magnitude!=100&&expected_magnitude!=160)return LT48_SAFETY_REJECTED;
    lt48_pic_source_state state;
    lt48_status status=lt48_pic_source_session_state_internal(session,&state);
    if(status!=LT48_OK)return status;
    size_t index=state.variable_count;
    for(size_t i=0;i<state.variable_count;i++)if(state.variables[i].variable_id==variable_id)index=i;
    if(index==state.variable_count)return LT48_NOT_FOUND;
    const lt48_compile_variable *original=state.variables+index;
    /* Unit 1 is 100 us; exact cohort policies bind the original magnitude.
     * K20/K22 uses 100, K50 uses 160. Keep the whole-ms serialization and
     * reject any unbound source shape rather than guessing its units. */
    if(original->kind!=1||original->value!=1||original->channel!=expected_magnitude||!original->enabled)
        return LT48_SAFETY_REJECTED;
    uint32_t magnitude=((minimum_us+999)/1000)*10;
    if(magnitude<original->channel)magnitude=original->channel;
    lt48_compile_variable *variables=malloc(state.variable_count*sizeof(*variables));
    if(!variables)return LT48_NO_MEMORY;
    memcpy(variables,state.variables,state.variable_count*sizeof(*variables));
    variables[index].channel=(uint16_t)magnitude;
    state.variables=variables;
    status=lt48_pic_source_session_restore_internal(session,&state);
    free(variables);return status;
}
static inline lt48_status lt48_pic_source_raise_erase_wait(
    lt48_pic_source_session *session,uint16_t variable_id,uint32_t minimum_us) {
    return lt48_pic_source_raise_erase_wait_shape(session,variable_id,minimum_us,100);
}
#endif
