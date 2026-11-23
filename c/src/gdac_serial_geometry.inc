/* Original XP primary geometry used by both Erase and UnLock workers. */
static const uint32_t gdac_sectors_a[] = {0x0};
static const uint32_t gdac_sectors_b[] = {0x0};
static const uint32_t gdac_sectors_c[] = {0x0};
static const uint32_t gdac_sectors_d[] = {0x0};
static const uint32_t gdac_sectors_e[] = {0x0};
static const uint32_t gdac_sectors_f[] = {0x0};
static const uint32_t gdac_sectors_10[] = {0x0};
static const uint32_t gdac_sectors_11[] = {0x0};
static const uint32_t gdac_sectors_12[] = {0x0};
static const uint32_t gdac_sectors_13[] = {0x0};
static const uint32_t gdac_sectors_14[] = {0x0};
static const uint32_t gdac_sectors_15[] = {0x0};
static const uint32_t gdac_sectors_16[] = {0x0};
static const uint32_t gdac_sectors_17[] = {0x0};
static const uint32_t gdac_sectors_18[] = {0x0};
static const uint32_t gdac_sectors_19[] = {0x0};
static const uint32_t gdac_sectors_1a[] = {0x0};
static const uint32_t gdac_sectors_1b[] = {0x0};
static const uint32_t gdac_sectors_1c[] = {0x0,0x10000,0x20000,0x30000,0x40000,0x50000,0x60000,0x70000,0x78000,0x7a000,0x7c000};
static const uint32_t gdac_sectors_1d[] = {0x0};
static const struct {unsigned definition;const uint32_t *starts;size_t count;} gdac_geometry[] = {
    {0xa,gdac_sectors_a,1},
    {0xb,gdac_sectors_b,1},
    {0xc,gdac_sectors_c,1},
    {0xd,gdac_sectors_d,1},
    {0xe,gdac_sectors_e,1},
    {0xf,gdac_sectors_f,1},
    {0x10,gdac_sectors_10,1},
    {0x11,gdac_sectors_11,1},
    {0x12,gdac_sectors_12,1},
    {0x13,gdac_sectors_13,1},
    {0x14,gdac_sectors_14,1},
    {0x15,gdac_sectors_15,1},
    {0x16,gdac_sectors_16,1},
    {0x17,gdac_sectors_17,1},
    {0x18,gdac_sectors_18,1},
    {0x19,gdac_sectors_19,1},
    {0x1a,gdac_sectors_1a,1},
    {0x1b,gdac_sectors_1b,1},
    {0x1c,gdac_sectors_1c,11},
    {0x1d,gdac_sectors_1d,1},
};
