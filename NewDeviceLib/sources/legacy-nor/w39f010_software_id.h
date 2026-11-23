/* Authored W39F010 software identification. Winbond A4 table 7.2:
 * AA@5555,55@2AAA,90@5555; read 0/1; AA,55,F0 to return to array.
 * Avoids the original undocumented A9 high-voltage identification.
 * FPGA C2 decoder and pin mapping are verified through the digital pipeline.
 */
static lt48_status w39f010_software_id(lt48_gaaf_serial_resources *r,
 const lt48_catalogue_entry *e) {
 if(e->device_class!=2007 || e->device_type!=0xc1 || e->manufacturer_id!=0xda ||
    e->device_id!=0xa1 || !e->device_name || e->device_name_length!=7 ||
    memcmp(e->device_name,"W39F010",7))return LT48_OK;
 lt48_logical_pin ce,oe,we;
 if(r->capacity!=0x20000 ||
    lt48_pin_map_find_logical(r->pins,2,&ce)!=LT48_OK || ce.engine_pin!=0x74 ||
    lt48_pin_map_find_logical(r->pins,3,&oe)!=LT48_OK || oe.engine_pin!=0x7c ||
    lt48_pin_map_find_logical(r->pins,8,&we)!=LT48_OK || we.engine_pin!=0x98)
  return LT48_SAFETY_REJECTED;
 const uint16_t code[]={
  0, /* patched length below */
  0,0x99,0,0x75,0,0x7d, /* WE, CE, OE inactive */
  0,0xc2,0,0xaa,0,0x55,0,0x90,
  0,0xc4,0,0,0,0xc8,0,0,0,0xcc,0,0,
  0,0x74,0,0x7c,36,10,0,95,4,
  0,0xd0,36,10,0,0,0x74,95,5,
  0,0x75,0,0x7d,0,0xc2,0,0xaa,0,0x55,0,0xf0,
  0,0x75,0,0x7d
 };
 uint16_t words[sizeof(code)/sizeof(*code)];memcpy(words,code,sizeof(code));
 words[0]=(uint16_t)(sizeof(words)/sizeof(*words)-1);
 lt48_compiled_wave *wave=NULL;
 lt48_status status=lt48_compiled_host_words_internal(words,sizeof(words)/sizeof(*words),&wave);
 if(status!=LT48_OK)return status;
 lt48_compiled_wave_close(r->waves[2]);r->waves[2]=wave;
 for(size_t i=0;i<r->nested_count;i++)if(r->nested[i].operation==2)r->nested[i].compiled=wave;
 return LT48_OK;
}
