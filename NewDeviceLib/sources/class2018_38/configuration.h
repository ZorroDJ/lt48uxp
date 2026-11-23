/* Authored representation of exact legacy object/category/field bindings.
 * Raw byte/word labels deliberately retain source indices where no label is proven. */
struct cl38_field {unsigned category,field,maximum,lock;const char*label;};
static const struct cl38_field cl38_atmel[]={
 {0,4,1,0,"Configuration flag 4"},{0,5,1,0,"Configuration flag 5"},
 {1,6,3,1,"Protection selection (0=none, 1–3=original lock pulse)"},
 {2,8,255,0,"Configuration byte 8"},{2,9,255,0,"Configuration byte 9"},
 {2,11,255,0,"Configuration byte 11"},{2,15,255,0,"Configuration byte 15"}};
static const struct cl38_field cl38_flags[]={
 {0,0,1,1,"Lock bit 1"},{0,1,1,1,"Lock bit 2"},{0,2,1,1,"Lock bit 3"}};
static const struct cl38_field cl38_infineon[]={{1,3,3,1,"Protection mask (two bits)"}};
static const struct cl38_field cl38_pxa[]={
 {0,0,1,1,"Protection flag 0"},{0,1,1,1,"Protection flag 1"},{0,2,1,1,"Protection flag 2"},
 {2,12,65535,0,"Configuration word 12"},{2,13,65535,0,"Configuration word 13"},
 {2,14,255,0,"Configuration byte 14"}};
static const struct cl38_field*cl38_schema(unsigned m,unsigned*n){
 const struct cl38_field*s=NULL;*n=0;
 if(m==120){s=cl38_atmel;*n=7;}
 else if((m>=87&&m<=90)||m==93||m==94){s=cl38_flags;*n=3;}
 else if(m==112){s=cl38_infineon;*n=1;}
 else if(m==136||m==137){s=cl38_pxa;*n=6;}
 return s;
}
