/* Exact authored layout fingerprints; vendor bytecode remains BYO.
 * Duplicate fuse coordinates require original conversion qualification. */
static unsigned palce54_layout_aliases(const lt48_fuse_code*c){
 static const struct {unsigned instructions,count;unsigned short dimensions[5];unsigned char sha[32];unsigned duplicates;} rows[]={
 {104,14504,{128,1,171,84,176},{0xef,0x57,0x09,0x60,0x0a,0x03,0x99,0xc8,0xc3,0x1b,0x7b,0x67,0x61,0xb8,0x5c,0x56,0x65,0xc4,0xb6,0xdf,0x42,0x0d,0x92,0xec,0x88,0x41,0x0b,0x19,0x80,0x5f,0x0d,0xeb},2},
 };unsigned char digest[32];for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(c->instruction_count==rows[i].instructions&&c->total_fuse_count==rows[i].count&&!memcmp(c->dimensions,rows[i].dimensions,sizeof c->dimensions)){lt48_sha256(c->bytecode,c->bytecode_size,digest);if(!memcmp(digest,rows[i].sha,32))return rows[i].duplicates;}return 0;}
