/* Authored extension only. Original imported programming code stays private. */
static lt48_status atmel_word_timeout_apply(const lt48_installation *x,lt48_gaaf_serial_resources *r) {
 static const unsigned char expected_tail[]={0xc2,0x8c,0xc2,0x8d,0x74,0x3d,0xf2,0x90,0xc8,0x00,0x74,0x00,0x20,0xb2,0xfd,0xf0,0x74,0x3c,0xf2,0x00,0xe2,0x20,0xb2,0xfd,0xf0,0x74,0xd0,0xf2,0x00,0xe2,0x20,0xb2,0xfd,0xf0,0x74,0x3d,0xf2,0x22};
 static const unsigned char expected_original[]={0xeb,0x9e,0x94,0x65,0xf2,0x7f,0x3a,0xeb,0xaa,0x86,0xc2,0xb2,0xfa,0x71,0x8f,0xf1,0xb9,0xf9,0xe8,0xcc,0xc0,0x86,0x37,0x8d,0xe7,0x54,0x0c,0x8e,0x1d,0xfb,0x9e,0x30};
 lt48_wave_code *a=&r->applets[17];unsigned char hash[32];lt48_sha256_context sha;
 if(!r->legacy_word||a->load_address!=0x5100||a->data_size!=180)return LT48_SAFETY_REJECTED;
 lt48_sha256_init(&sha);lt48_sha256_update(&sha,a->data,a->data_size);lt48_sha256_final(&sha,hash);
 if(memcmp(hash,expected_original,32))return LT48_SAFETY_REJECTED;
 lt48_new_device_lib *lib=NULL;lt48_new_device_lib_report report;
 lt48_status s=lt48_new_device_lib_open(lt48_installation_new_device_lib_path(x),&lib,&report);
 if(s!=LT48_OK)return s;
 if(strcmp(report.sha256,lt48_installation_new_device_lib_sha256(x))){lt48_new_device_lib_close(lib);return LT48_SAFETY_REJECTED;}
 unsigned char *tail=NULL;size_t size=0;
 s=lt48_new_device_lib_read_resource(lib,"applet.atmel-word-timeout-tail","80c32-applet","lt48-word-timeout-51b4-v1",&tail,&size);
 lt48_new_device_lib_close(lib);
 if(s!=LT48_OK)return s;
 if(size!=sizeof expected_tail||memcmp(tail,expected_tail,size)){free(tail);return LT48_SAFETY_REJECTED;}
 /* Keep the appended code in the existing owned allocation. idle_size still
  * describes only the idle FPGA; normal cleanup frees the allocation once. */
 size_t original_size=a->data_size;
 unsigned char *owned=realloc(r->idle,r->idle_size+original_size+size);
 if(!owned){free(tail);return LT48_NO_MEMORY;}
 r->idle=owned;unsigned char *code=owned+r->idle_size;
 memcpy(code,a->data,original_size);memcpy(code+original_size,tail,size);free(tail);
 code[0x77]=0x51;code[0x78]=0xb4;code[0x9c]=0x51;code[0x9d]=0xb4;
 a->data=code;a->data_size=original_size+size;a->length2=a->data_size;a->length1=a->data_size+4;
 return LT48_OK;
}
