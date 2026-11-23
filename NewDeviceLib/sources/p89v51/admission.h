/* Publication gate; descriptors alone do not confer support. */
#define LT48_P89V_STAGE 2
static inline int lt48_p89v_admitted(unsigned d){return LT48_P89V_STAGE==2 || (LT48_P89V_STAGE==1 && d!=0xc5 && d!=0xc6 && d!=0xcb && d!=0xcc);}
