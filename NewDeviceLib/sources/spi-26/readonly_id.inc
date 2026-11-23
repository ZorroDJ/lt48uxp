/* SST26 factory Security-ID bytes 0000..0007 are read-only. Programming a
 * complete first security page must retain these bytes exactly. They are
 * excluded from the Program blank precheck; final readback still covers them. */
static size_t sv_readonly_prefix(const lt48_addtab_spi_resources*r,uint32_t start,size_t length){
 if(lt48_addtab_spi_is_spi26(r)&&r->closure->configuration==25124&&start==r->closure->segments[1].image_offset)
  return length<8?length:8;
 return 0;
}
