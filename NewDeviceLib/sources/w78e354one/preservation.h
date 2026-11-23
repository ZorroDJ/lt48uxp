/* Authored ROM-MAP query and raw option preservation, no lock/watchdog editor. */
static const unsigned char w78e354one_rommap[]={0x1e,0x00,0x3b,0xc4,0x3b,0xff,0x3b,0xc8,0x3b,0x7f,0x3b,0xd8,0x3b,0x02,0x11,0x00,0x00,0x11,0x01,0x00,0x7c,0x10,0x11,0x01,0x01,0x11,0x00,0x01,0x3b,0xd8,0x3b,0x00};
 
static lt48_status w78e354one_preserve_compile(const lt48_wave_code*code,const lt48_pin_map*p,const lt48_compile_variable*v,size_t n,lt48_compiled_wave**out){
 if(code->data_size!=142||memcmp(code->data+0x64,"\x3b\xd8\x3b\x00",4))return LT48_SAFETY_REJECTED;
 
 unsigned char b[44];
 b[0]=42;
 b[1]=0;
 memcpy(b+2,code->data+0x64,42);
 lt48_compact_wave*w=NULL;
 
 lt48_status st=lt48_compact_decode(b,sizeof b,&w);
 if(!st)st=lt48_compact_compile_with_pin_policy(w,p,v,n,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,out);
 lt48_compact_wave_close(w);
 return st;
 
}
