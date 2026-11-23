/* Audited main/special-array batch. Source digest is enforced by addtab_open. */
#ifndef LT48_ADDTAB_REMAINING43_ADMISSION_H
#define LT48_ADDTAB_REMAINING43_ADMISSION_H
static int addtab_remaining43_batch(unsigned d) {
 const unsigned defs[]={0x9,0xa,0x70,0x6e,0x6f,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x132,0x133,0x134,0x130,0x131,0x116};
 for(unsigned i=0;i<sizeof(defs)/sizeof(*defs);i++)if(d==defs[i])return 1;
 return 0;
}
static int addtab_samsung4_batch(unsigned d) {
 return d==0x12b || d==0x12c || d==0x12d || d==0x12e;
}
static int addtab_eon_esmt15_batch(unsigned d) {
 return d==0x74 || d==0x75 || d==0x3e || d==0x3f;
}
static int addtab_spansion2_batch(unsigned d) { return d==0x7f || d==0x80; }
static int addtab_d31six_batch(unsigned d) { return d==0x31; }
static int addtab_samsung128_batch(unsigned d) { return d==0x12f; }
static int addtab_st128_batch(unsigned d) { return d==0xef; }
static int addtab_mx26_sop_batch(unsigned d) { return d==0xea; }
static int addtab_remaining_admitted(unsigned d) {
 return addtab_remaining43_batch(d) || addtab_samsung4_batch(d) || addtab_eon_esmt15_batch(d) || addtab_spansion2_batch(d) || addtab_d31six_batch(d) || addtab_samsung128_batch(d) || addtab_st128_batch(d) || addtab_mx26_sop_batch(d);
}
#endif
