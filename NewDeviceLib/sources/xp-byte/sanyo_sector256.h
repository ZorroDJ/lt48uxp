/* Dataman 13.10 GDAB D62-D68: byte program, 256-byte erase sectors.
 * Mandatory ENGINEER compact wave 15 performs seven-read unprotect.
 * Exact-source profiles; hardware qualification pending. */
static const struct intel_byte_closure xp_sanyo7_closures[]={
 {{0x66,54,37,32,262144,500,NULL,0},NULL,1024,2,13},
 {{0x67,54,35,32,524288,500,NULL,0},NULL,2048,2,13},
 {{0x68,55,36,40,524288,500,NULL,0},NULL,2048,2,13},
 {{0x65,53,33,40,2097152,330,NULL,0},NULL,8192,2,13},
 {{0x62,52,37,32,262144,330,NULL,0},NULL,1024,2,13},
 {{0x63,52,35,32,524288,330,NULL,0},NULL,2048,2,13},
 {{0x64,53,34,40,1048576,330,NULL,0},NULL,4096,2,13},
};
