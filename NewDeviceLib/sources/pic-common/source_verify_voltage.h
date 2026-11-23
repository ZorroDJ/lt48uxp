#ifndef LT48_PIC_SOURCE_VERIFY_VOLTAGE_H
#define LT48_PIC_SOURCE_VERIFY_VOLTAGE_H
#include "../../../c/include/labtool48/labtool48.h"
#include <stdint.h>
#include <string.h>

/* Original 00467420: explicit source slots retain their positions. Presence,
 * rather than the variable's editor-enabled attribute, selects this branch.
 * The caller must reject a sparse explicit layout before transferring data. */
static inline lt48_status lt48_pic_source_verify_voltages(uint32_t nominal,
    const uint32_t explicit_values[4],unsigned present,unsigned margin_type,
    unsigned nominal_only,uint32_t out[4],unsigned *count) {
    if(!out||!count||!nominal||nominal>UINT16_MAX||present>15||
       (present&&!explicit_values))return LT48_INVALID_ARGUMENT;
    memset(out,0,4*sizeof(*out));*count=0;
    if(present&&!nominal_only) {
        for(unsigned i=0;i<4;i++)if(present&(1u<<i)) {
            if(!explicit_values[i]||explicit_values[i]>UINT16_MAX)return LT48_SAFETY_REJECTED;
            out[i]=explicit_values[i];++*count;
        }
    } else if(!present&&margin_type<2) {
        /* Float intermediate rounding is part of the original result. */
        float low=margin_type?0.949999988079071f:1.0f;
        float high=margin_type?1.0499999523162842f:1.0f;
        low=low-0.05000000074505806f;high=high+0.05000000074505806f;
        float a=low*(float)nominal,b=high*(float)nominal;
        out[0]=(uint32_t)((double)a+0.5);out[1]=(uint32_t)((double)b+0.5);*count=2;
    } else {out[0]=nominal;*count=1;}
    return LT48_OK;
}
#endif
