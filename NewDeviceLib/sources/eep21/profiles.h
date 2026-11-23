/* Authored exact-source bindings; original REZ/FPGA/applets remain BYO. */
static int eep21_definition(unsigned cls,unsigned d){
 if(cls!=2001)return 0;
 switch(d){
case 0x41:return 1;
case 0x42:return 1;
case 0x43:return 1;
case 0x44:return 1;
case 0x45:return 1;
case 0x5:return 1;
case 0x7:return 1;
case 0x8:return 1;
case 0x9:return 1;
case 0xa:return 1;
case 0xb:return 1;
case 0xc:return 1;
case 0xd:return 1;
case 0xe:return 1;
default:return 0;}}
static int eep21_group(const lt48_m37_resource_audit*a){
 static const struct{unsigned d,m,p,pins,size;}rows[]={
 {0x43,53,11,32,131072},
 {0x44,53,12,32,262144},
 {0x41,51,6,28,32768},
 {0x42,52,8,28,65536},
 {0x45,54,13,32,524288},
 {0x7,6,11,32,131072},
 {0xd,6,12,32,262144},
 {0x5,12,6,28,32768},
 {0x9,8,11,32,131072},
 {0xb,8,12,32,262144},
 {0xe,10,13,32,524288},
 {0xa,9,11,32,131072},
 {0x8,7,11,32,131072},
 {0xc,9,12,32,262144},
 };
 static const unsigned char sha[32]={0x2e,0x62,0x61,0xc8,0x67,0xd6,0x85,0x77,0x86,0x93,0x3c,0x0d,0x09,0x3e,0x5b,0xd6,0x69,0xf7,0x03,0x55,0xcc,0x21,0x8f,0xf0,0x06,0xc8,0x8c,0x4f,0xb9,0x19,0x63,0x6b};
 if(a->device_class!=2001||memcmp(a->definition_sha256,sha,32))return 0;
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(a->device_type==rows[i].d&&a->family_code==rows[i].m&&a->pinmap==rows[i].p&&a->pin_count==rows[i].pins&&a->capacity_bytes==rows[i].size)return 1;
 return 0;
}

static unsigned eep21_read_nominal(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2001||strcmp(s->catalogue.vendor,"SST"))return 0;
 const char*n=(const char*)s->catalogue.device_name;size_t z=s->catalogue.device_name_length;
 return (z==7&&(!memcmp(n,"27VF010",7)||!memcmp(n,"27VF020",7)||!memcmp(n,"27VF512",7)))?330:0;
}
