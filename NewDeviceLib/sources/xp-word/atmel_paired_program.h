/* Candidate source profiles. Public admission is deliberately separate.
 * Original DEVINIT selects applet 8000's second dispatcher and 05 E0;
 * that handler consumes two words per programming cycle. */
static const uint32_t xp_atmel7_sectors[]={0};
static const struct xp_closure xp_atmel7_closures[]={
 {0xfd,218,82,48,4194304,330,xp_atmel7_sectors,1},
 {0xff,220,85,48,4194304,300,xp_atmel7_sectors,1},
 {0x100,221,85,48,4194304,300,xp_atmel7_sectors,1}
};
static const struct xp_shared_word_shape xp_atmel7_shape={0,14,5,0};
