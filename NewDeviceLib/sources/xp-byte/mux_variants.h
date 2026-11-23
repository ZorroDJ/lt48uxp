/* Source-derived mux variants, private qualification only. */
static const uint32_t xp_mux15_protection[]={0};
static const uint32_t xp_mux15_erase_D22[]={0x0,0x10000,0x20000,0x30000,0x40000,0x50000,0x60000,0x70000};
static const uint32_t xp_mux15_erase_D20[]={0x0,0x10000,0x20000,0x30000,0x40000,0x50000,0x60000,0x70000,0x80000,0x90000,0xa0000,0xb0000,0xc0000,0xd0000,0xe0000,0xf0000,0xf2000,0xf4000,0xf6000,0xf8000,0xfa000,0xfc000,0xfe000};
static const uint32_t xp_mux15_erase_D1f[]={0x0,0x10000,0x20000,0x30000,0x40000,0x50000,0x60000,0x70000,0x80000,0x90000,0xa0000,0xb0000,0xc0000,0xd0000,0xe0000,0xf0000,0xf2000,0xf4000,0xf6000,0xf8000,0xfa000,0xfc000,0xfe000};
static const uint32_t xp_mux15_erase_D21[]={0x0};
static const uint32_t xp_mux15_erase_D15[]={0x0};
static const uint32_t xp_mux15_erase_D13[]={0x0};
static const uint32_t xp_mux15_erase_D16[]={0x0};
static const uint32_t xp_mux15_erase_D11[]={0x0,0x10000,0x20000,0x30000,0x40000,0x50000,0x60000,0x70000};
static const uint32_t xp_mux15_erase_D18[]={0x0};
static const struct intel_byte_closure xp_mux15_closures[]={
 {{0x22,13,12,32,524288,330,xp_mux15_protection,1},xp_mux15_erase_D22,8,2,12},
 {{0x20,6,10,32,1048576,330,xp_mux15_protection,1},xp_mux15_erase_D20,23,2,12},
 {{0x1f,5,7,40,1048576,330,xp_mux15_protection,1},xp_mux15_erase_D1f,23,2,12},
 {{0x21,7,11,32,1048576,330,xp_mux15_protection,1},xp_mux15_erase_D21,1,4,12},
 {{0x15,10,8,32,1048576,330,xp_mux15_protection,1},xp_mux15_erase_D15,1,4,12},
 {{0x13,10,13,32,1048576,330,xp_mux15_protection,1},xp_mux15_erase_D13,1,4,12},
 {{0x16,8,6,40,1048576,330,xp_mux15_protection,1},xp_mux15_erase_D16,1,4,12},
 {{0x11,9,13,32,524288,330,xp_mux15_protection,1},xp_mux15_erase_D11,8,4,12},
 {{0x18,8,6,40,2097152,330,xp_mux15_protection,1},xp_mux15_erase_D18,1,4,12},
};
