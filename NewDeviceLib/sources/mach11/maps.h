/* Exact authored MACH210/LV210 duplicate-coordinate fingerprint.
 * Conversion must retain the original OR packing and duplicated Read bit. */
static unsigned mach11_layout_aliases(const lt48_fuse_code*c){
 static const unsigned short dims[5]={63,69,4,64,289};
 static const unsigned char expected[32]={0xd1,0x8f,0x93,0xf2,0x63,0x57,0x28,0x26,0x47,0x32,0x99,0x30,0x14,0x6a,0xe7,0x6c,0x71,0xa1,0x67,0x8d,0x4b,0x21,0x97,0x1c,0x10,0xea,0x58,0xdd,0x25,0x9c,0xbd,0xb6};unsigned char digest[32];
 if(c->instruction_count!=73u||c->total_fuse_count!=12834u||memcmp(c->dimensions,dims,sizeof dims))return 0;
 lt48_sha256(c->bytecode,c->bytecode_size,digest);return !memcmp(digest,expected,32);
}
