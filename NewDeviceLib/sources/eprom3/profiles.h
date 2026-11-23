/* Authored exact legacy byte-memory extensions; vendor assets remain BYO. */
static int eprom3_definition(unsigned cls,unsigned d){return cls==2001&&(d==0x25||d==0x3a||d==0x3b);}
static int eprom3_group(const lt48_m37_resource_audit*a){
 static const unsigned char sha[32]={0x2e,0x62,0x61,0xc8,0x67,0xd6,0x85,0x77,0x86,0x93,0x3c,0x0d,0x09,0x3e,0x5b,0xd6,0x69,0xf7,0x03,0x55,0xcc,0x21,0x8f,0xf0,0x06,0xc8,0x8c,0x4f,0xb9,0x19,0x63,0x6b};
 static const struct{unsigned d,m,p,pins,size;}rows[]={
 {37,28,11,32,32768},
 {59,45,12,32,262144},
 {58,44,12,32,262144},
 };if(a->device_class!=2001||memcmp(a->definition_sha256,sha,32))return 0;for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(a->device_type==rows[i].d&&a->family_code==rows[i].m&&a->pinmap==rows[i].p&&a->pin_count==rows[i].pins&&a->capacity_bytes==rows[i].size)return 1;return 0;
}
static int eprom3_electrical(const lt48_m37_resource_audit*a){return eprom3_group(a)&&a->device_type!=0x25;}
