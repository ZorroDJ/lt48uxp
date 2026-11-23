/* Authored private correction for exact Dataman 13.10 D90.
 * The imported D90 manual applet emits undocumented C0. The manufacturer
 * MX28F1000 table 1 and page 38-9 specify 40 + data automatic programming.
 * Reuse W119 from the same hash-verified, operator-supplied REZ. No vendor
 * firmware bytes are stored here. Public admission uses the exact identity gate.
 */
lt48_status lt48_legacy_mx28f1000_auto_open_internal(const lt48_installation *x,
 size_t device,lt48_gaaf_serial_resources **out) {
 lt48_catalogue_entry e;
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_status s=lt48_installation_get_device(x,device,&e);
 if(s!=LT48_OK)return s;
 if(e.device_class!=2007 || e.device_type!=0x90 || e.manufacturer_id!=0xc2 ||
    e.device_id!=0x11 || !e.vendor || strcmp(e.vendor,"Macronix") ||
    e.device_name_length!=9 || memcmp(e.device_name,"MX28F1000",9))return LT48_UNSUPPORTED;
 s=lt48_legacy_byte49_open_internal(x,device,out);
 if(s!=LT48_OK)return s;
 lt48_gaaf_serial_resources *r=*out;size_t index;lt48_wave_code code;
 s=lt48_rez_find_resource(r->rez,"W119",&index);
 if(s==LT48_OK)s=lt48_rez_get_wave_code(r->rez,index,&code);
 if(s==LT48_OK && (code.wave_type!=6 || code.load_address!=0x5184 ||
    code.data_size!=127 || code.length1!=131 || code.length2!=127))s=LT48_FORMAT_ERROR;
 if(s==LT48_OK) {
  unsigned found=0;
  for(size_t i=0;i<r->address_count;i++)if(r->addresses[i].wave==14) {
   r->addresses[i].address=code.load_address;found++;
  }
  if(found!=1)s=LT48_FORMAT_ERROR;
  else r->applets[14]=code;
 }
 if(s!=LT48_OK){lt48_gaaf_serial_close(r);*out=NULL;}
 return s;
}
