/* Dataman 13.10 GDAAA exact source-audited profiles; hardware pending.
 * Closure sectors are protection regions. Erase uses the original chip-erase
 * wave once for the full array. DEVINIT selects normal versus bypass program. */
static const uint32_t xp_st102_bottom[]={0,0x4000,0x6000,0x8000,0x10000};
static const uint32_t xp_st102_top[]={0,0x10000,0x18000,0x1a000,0x1c000};
static const struct xp_closure xp_st102_closures[]={
 {0x136,272,98,44,131072,500,xp_st102_bottom,5},
 {0x10c,230,90,40,131072,500,xp_st102_bottom,5},
 {0x10d,231,90,40,131072,330,xp_st102_bottom,5},
 {0x10e,232,90,40,131072,330,xp_st102_top,5}
};
static const struct xp_shared_word_shape xp_st102_shape={0,18,7,1};
