/* Authored exact-source bindings; original REZ/FPGA/applets remain BYO. */
static int macronixmtp2_definition(unsigned cls,unsigned d){
 if(cls!=2001)return 0;
 return d==0x26||d==0x27;}
static int macronixmtp2_group(const lt48_m37_resource_audit*a){
 static const struct{unsigned d,m,p,pins,size;}rows[]={
 {0x26,29,8,28,65536},
 {0x27,30,11,32,131072},
 };
 static const unsigned char sha[32]={0x2e,0x62,0x61,0xc8,0x67,0xd6,0x85,0x77,0x86,0x93,0x3c,0x0d,0x09,0x3e,0x5b,0xd6,0x69,0xf7,0x03,0x55,0xcc,0x21,0x8f,0xf0,0x06,0xc8,0x8c,0x4f,0xb9,0x19,0x63,0x6b};
 if(a->device_class!=2001||memcmp(a->definition_sha256,sha,32))return 0;
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(a->device_type==rows[i].d&&a->family_code==rows[i].m&&a->pinmap==rows[i].p&&a->pin_count==rows[i].pins&&a->capacity_bytes==rows[i].size)return 1;
 return 0;
}

