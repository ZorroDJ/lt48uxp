/* Authored C5132 read timeout guard. Experimental, not admitted.
 * Transform only exact user-supplied W106. Keep the original FPGA 7C/7D
 * transaction and successful byte reply. Bound P3.2 polling to 65536 samples.
 * SRAM 41FE saves R6; 41FF is a sticky success latch, initialized to one.
 * Timeout emits a placeholder zero, clears the latch, releases with 7D and
 * returns to the resident dispatcher. A later successful read cannot hide it.
 * Resident command 72 invokes the query at 4200. Host must verify that latch
 * through a status-checked bridge transaction before publishing any capture.
 * Host must initialize the latch before each logical transaction, never while
 * an applet is running. No EEPROM or other persistent programmer state used.
 */
#include <labtool48/labtool48.h>
#include <stdio.h>
#include <string.h>
lt48_status lt48_c5132_guard_applet(const unsigned char *source,size_t size,
    unsigned char *out,size_t capacity,size_t *out_size) {
    if(!source || !out || !out_size)return LT48_INVALID_ARGUMENT;
    *out_size=0;if(size!=29)return LT48_SAFETY_REJECTED;
    unsigned char digest[32];char hash[65];lt48_sha256(source,size,digest);
    for(unsigned i=0;i<32;i++)snprintf(hash+i*2,3,"%02x",digest[i]);
    if(strcmp(hash,"efb53ad856520a7d3e33826b1438517ff6fbc04b54427da99f8058f2a6bdbfcf"))return LT48_SAFETY_REJECTED;
    if(capacity<265)return LT48_BUFFER_TOO_SMALL;
    /* Save R6, followed by the unchanged initialization and read-start prefix. */
    const unsigned char save[]={0x90,0x41,0xfe,0xee,0xf0};
    const unsigned char tail[]={
        0x7e,0,0x7f,0,        /* 256 x 256 polling iterations */
        0x30,0xb2,0x0b,      /* ready -> sample */
        0xdf,0xfb,0xde,0xf9, /* busy -> bounded retry */
        0x90,0x41,0xff,0xe4,0xf0, /* sticky timeout */
        0x80,1,              /* placeholder zero -> emit */
        0xe2,                /* original FPGA sample */
        0x90,0xc8,0,0xf0,    /* one host byte */
        0x90,0x41,0xfe,0xe0,0xfe, /* restore R6 */
        0x90,0xc8,0,0x7f,0, /* original final DPTR/R7 */
        0x74,0x7d,0xf2,0x22 /* original FPGA release and return */
    };
    unsigned char prefix[20];memcpy(prefix,source,sizeof prefix);
    memset(out,0,265);memcpy(out,save,sizeof save);memcpy(out+sizeof save,prefix,sizeof prefix);
    memcpy(out+sizeof save+sizeof prefix,tail,sizeof tail);out[255]=1;
    const unsigned char query[]={0x90,0x41,0xff,0xe0,0x90,0xc8,0,0xf0,0x22};
    memcpy(out+256,query,sizeof query);*out_size=265;return LT48_OK;
}
