#ifndef LT48_HC908_FQTQY_PROFILES_H
#define LT48_HC908_FQTQY_PROFILES_H
/* F_ source route: MON08 entry followed by target RAM/GPIO fast transfer.
 * Linux retains entry/FPGA/serial helpers and uses audited MON08 RAM Flash
 * operations with independent readback, preserving both factory trim bytes. */
#define LT48_HC908_FQTQY_STAGE 15
struct hc908_f_profile {unsigned definition,family,pinmap,first,variables,read_size,patch,tx,rx,cleanup,stage;};
static const struct hc908_f_profile hc908_f_profiles[]={
 {0x5d,0x5b,0x3e,0xf800,6,487,0xc4,0x59df,0x5a1b,0x52d1,4},
 {0x5e,0x5c,0x3e,0xee00,6,487,0xc4,0x59df,0x5a1b,0x52d1,6},
 {0x59,0x57,0x3c,0xf800,5,473,0xbe,0x5950,0x598c,0x52c3,15},
 {0x5a,0x58,0x3d,0xf800,5,473,0xbe,0x5950,0x598c,0x52c3,15},
 {0x5b,0x59,0x3c,0xee00,5,473,0xbe,0x5950,0x598c,0x52c3,15},
 {0x5c,0x5a,0x3d,0xee00,5,473,0xbe,0x5950,0x598c,0x52c3,15},
};
static const struct {unsigned definition;const char *name;} hc908_f_members[]={
 {0x5d,"F_MC68HC908QT1 *8DIP"},{0x5d,"F_MC68HC908QT1 *8SOIC"},
 {0x5d,"F_MC68HC908QT2 *8DIP"},{0x5d,"F_MC68HC908QT2 *8SOIC"},
 {0x5e,"F_MC68HC908QT4 *8DIP"},{0x5e,"F_MC68HC908QT4 *8SOIC"},
 {0x59,"F_MC68HC908QY1 *16DIP"},{0x59,"F_MC68HC908QY1 *16SO"},
 {0x59,"F_MC68HC908QY2 *16DIP"},{0x59,"F_MC68HC908QY2 *16SO"},
 {0x5a,"F_MC68HC908QY1 *16TSS"},{0x5a,"F_MC68HC908QY2 *16TSS"},
 {0x5b,"F_MC68HC908QY4 *16DIP"},{0x5b,"F_MC68HC908QY4 *16SO"},
 {0x5c,"F_MC68HC908QY4 *16TSS"},
};
#endif
