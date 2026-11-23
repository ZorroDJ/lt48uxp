#ifndef LT48_DRAFT_BASELINE12_PRIMARY_POWER_H
#define LT48_DRAFT_BASELINE12_PRIMARY_POWER_H
#include <labtool48/labtool48.h>
/* Primary memory-programming AC/DC tables explicitly permit12.5..13.5V.
 * Keep exact original13.10 requests. Official PICkit2 corroborates12.5V for
 * all16base parts it contains;570 uses its own DS41670A table. Generic DFP
 *10..12V metadata disagreement is retained in the evidence,not silently erased.
 * Public device admission and physical analogue qualification are separate. */
static inline unsigned lt48_pic_baseline12_primary_vpp(unsigned d,unsigned f,unsigned p) {
 static const unsigned bindings[][4]={
 {57,51,15,1300},
 {58,51,16,1300},
 {59,52,17,1300},
 {60,52,18,1300},
 {61,53,19,1300},
 {62,53,20,1300},
 {354,245,84,1275},
 {355,245,85,1275},
 {356,246,85,1275},
 {357,246,84,1275},
 {358,247,84,1275},
 {359,247,85,1275},
 {360,248,84,1275},
 {361,248,85,1275},
 {362,249,87,1275},
 {363,250,87,1275},
 {364,251,87,1275},
 {365,252,88,1275},
 {366,253,88,1275},
 {367,254,88,1275},
 {383,266,95,1250},
 {384,267,96,1250},
 };
 for(size_t i=0;i<sizeof(bindings)/sizeof(*bindings);i++)
  if(bindings[i][0]==d&&bindings[i][1]==f&&bindings[i][2]==p)return bindings[i][3];
 return 0;
}
static inline lt48_status lt48_pic_baseline12_primary_power_validate(unsigned d,unsigned f,unsigned p,
 const lt48_compile_variable *variables,size_t count) {
 unsigned expected=lt48_pic_baseline12_primary_vpp(d,f,p),found=0;
 if(!expected||!variables||!count)return LT48_SAFETY_REJECTED;
 for(size_t i=0;i<count;i++)if(variables[i].variable_id==3) {
  const lt48_compile_variable *v=variables+i;
  if(found++||v->kind!=0||v->enabled!=1||v->channel!=3||v->value!=expected||v->value<1250||v->value>1350)return LT48_SAFETY_REJECTED;
 }
 return found==1?LT48_OK:LT48_SAFETY_REJECTED;
}
#endif
