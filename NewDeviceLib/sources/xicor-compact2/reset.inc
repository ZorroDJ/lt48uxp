/* X84041: deselect, allow tP, then clear rails. Next INIT performs the
 * primary read/write-0/read reset. Source W91's unrelated two-wire tail
 * contains unbound serial pins and is not used by this authored owner. */
static const unsigned char x84041_reset[]={6,0,0x11,17,1,0x31,17,0x36};
