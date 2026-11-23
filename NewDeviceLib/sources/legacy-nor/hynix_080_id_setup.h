/* Authored, private qualification procedure. Original REZ is unchanged.
 * Evidence: docs/audit-qualification/BYTE080_ID_BOUNDARY.md.
 * Explicitly enable OE before the original high-voltage ID sequence.
 */
static lt48_status hynix_080_id_setup(lt48_gaaf_serial_resources *r,
 const lt48_catalogue_entry *e) {
 const char *name=NULL;
 if(e->device_class!=2007 || e->device_id!=0xd5)return LT48_OK;
 if(e->device_type==0x25 && e->manufacturer_id==0xad)name="HY29F080T *40TS";
 if(e->device_type==0x28 && e->manufacturer_id==0xc2)name="MX29F080 *40TS";
 if(e->device_type==0x29 && e->manufacturer_id==0xc2)name="MX29F080 *44PS";
 if(!name || !e->device_name || e->device_name_length!=strlen(name) ||
    memcmp(e->device_name,name,strlen(name)))return LT48_OK;
 lt48_logical_pin oe;
 if(r->capacity!=0x100000 ||
    lt48_pin_map_find_logical(r->pins,3,&oe)!=LT48_OK ||
    oe.engine_pin!=0x78)return LT48_SAFETY_REJECTED;
 /* Compiled host Q2 instruction; separate wave preserves source jump offsets. */
 const uint16_t words[]={2,0,oe.engine_pin};
 return lt48_compiled_host_words_internal(words,3,&r->id_setup);
}
