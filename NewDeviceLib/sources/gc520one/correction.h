/* Define the original FF acknowledgement comparison deterministically.
 * The source reads unassigned SFR FF; no hardware failure is alleged here.
 * Original bytes remain in the REZ. Only the temporary RAM copy changes. */
static lt48_status gc520_prepare_protect(lt48_s8253_resources*r,lt48_wave_code*w){
 if(w->data_size!=378||memcmp(w->data+0xf0,"\xb5\xff\x03",3)||memcmp(w->data+0x112,"\xb5\xff\x03",3))return LT48_SAFETY_REJECTED;
 r->gc8_protect_copy=malloc(w->data_size);if(!r->gc8_protect_copy)return LT48_NO_MEMORY;
 memcpy(r->gc8_protect_copy,w->data,w->data_size);r->gc8_protect_copy[0xf0]=0xb4;r->gc8_protect_copy[0x112]=0xb4;w->data=r->gc8_protect_copy;return LT48_OK;
}
