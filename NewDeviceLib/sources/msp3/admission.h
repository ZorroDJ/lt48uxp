/* Finite offline admission; sealing is a separate gate. */
static int msp3_admitted(unsigned d){return d>=0x16d&&d<=0x16f;}
