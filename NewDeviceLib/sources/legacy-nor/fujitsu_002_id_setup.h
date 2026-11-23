/* Authored ID setup, exact source-bound Fujitsu TSOP40 profiles only.
 * DS05-20818-1E table2 requires OE low for A9-VID identifier reads.
 * Keep the vendor wave and its branch offsets intact. */
static lt48_status fujitsu_002_id_setup(lt48_gaaf_serial_resources *r,
 const lt48_catalogue_entry *e) {
 if(!fujitsu_002(e->device_class,e->device_type,e->manufacturer_id,e->device_id,
    e->vendor,e->device_name,e->device_name_length))return LT48_OK;
 lt48_logical_pin oe;
 if(r->capacity!=0x40000||r->nominal!=500||
    lt48_pin_map_find_logical(r->pins,3,&oe)!=LT48_OK||oe.engine_pin!=0x44)
  return LT48_SAFETY_REJECTED;
 const uint16_t words[]={2,0,oe.engine_pin};
 return lt48_compiled_host_words_internal(words,3,&r->id_setup);
}
