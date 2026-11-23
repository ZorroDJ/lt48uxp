#ifndef LT48_PIC_REMAINING14_PUBLIC_H
#define LT48_PIC_REMAINING14_PUBLIC_H
#include "remaining14_ordinary_transport.h"
#include "remaining14_pic14000_transport.h"
#include "remaining14_legacy_profiles.h"
/* Exact audited41-entry cohort. Candidate generation alone never admits
 * another profile; the source table and manifest are integrity bound. */
#ifndef LT48_PIC_REMAINING14_PUBLIC_ENABLED
#define LT48_PIC_REMAINING14_PUBLIC_ENABLED 1
#endif
static inline const rm14_profile *rm14_public_match(const lt48_device_spec *spec){
#if LT48_PIC_REMAINING14_PUBLIC_ENABLED
 if(!spec)return NULL;
 for(unsigned i=0;i<sizeof remaining14_profiles/sizeof *remaining14_profiles;i++){
  const rm14_profile *p=remaining14_profiles+i;lt48_device_spec original=*spec;
  if(spec->capacity_bytes!=p->source_bytes&&spec->capacity_bytes!=p->backing_bytes)continue;
  original.capacity_bytes=p->source_bytes;if(rm14_matches(&original,p))return p;
 }
#else
 (void)spec;
#endif
 return NULL;
}
static inline int rm14_public_parity(const rm14_profile *p){return p&&p->family>=342&&p->family<=346;}
static inline lt48_status rm14_public_policy(const rm14_profile *p,rm14_otp_policy *out){
 if(!p||!out||p->has_erase)return LT48_UNSUPPORTED;
 if(rm14_public_parity(p)){
  *out=(rm14_otp_policy){.common={p->code_words,0x3fff,{0x3fff,0x3f30,500,400,550}},
   .blank_id_mask=15,.protection_values={0,0x1510,0x2a20,0x3f30},.protection_count=4,.factory_word=UINT32_MAX};
  return LT48_OK;
 }
 if(p->family==44)return rm14_pic14000_policy(p,out);
 lt48_status st=rm14_ordinary_policy(p,out);return st==LT48_OK?st:rm14_modern_policy(p,out);
}
#endif
