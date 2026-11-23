/* TC5332202P catalogue spelling; Toshiba TC5332202AP is a 2Mx16 mask ROM.
 * Retain source Read counter framing; omit unmapped VPP8 and all mutations.
 * No source GETID, programming or protection entry is exposed/executed. */
static const unsigned char word14_maskrom_read[]={
 0x21,0x00,0x4d,2,0,2,0,0,0,0x34,0x0f,
 0x3b,0xc4,0x3e,0,0,0x3b,0xc8,0x3e,0,1,0x3b,0xcc,0x3e,0,2,
 0x00,0,0x11,2,0,0x11,3,0,0x5c
};
static const unsigned char word14_maskrom_reset[]={0x06,0,0x39,0,0x36,0x12,0,2};
