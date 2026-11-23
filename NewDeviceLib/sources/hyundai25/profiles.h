/* Authored exact profiles; vendor procedures remain bring-your-own. */
#define LT48_HYUNDAI25_STAGE 3
struct hy25_profile {const char*key;unsigned d,m,p,pins,stage,start;};
static const struct hy25_profile hy25_profiles[]={
 {"Hyundai(Hynix):GMS81004T *20SO",0x154,161,104,20,1,61440},
 {"Hyundai(Hynix):GMS81004T *24SO",0x155,161,105,24,1,61440},
 {"Hyundai(Hynix):GMS81004T *28SO",0x156,161,106,28,1,61440},
 {"Hyundai(Hynix):GMS81004T *44",0x157,161,107,44,1,61440},
 {"Hyundai(Hynix):GMS81008T *20SO",0x158,162,104,20,1,57344},
 {"Hyundai(Hynix):GMS81008T *24SO",0x159,162,105,24,1,57344},
 {"Hyundai(Hynix):GMS81008T *28SO",0x15a,162,106,28,1,57344},
 {"Hyundai(Hynix):GMS81008T *44",0x15b,162,107,44,1,57344},
 {"Hyundai(Hynix):GMS81016T *20SO",0x15c,163,104,20,1,49152},
 {"Hyundai(Hynix):GMS81016T *24SO",0x15d,163,105,24,1,49152},
 {"Hyundai(Hynix):GMS81016T *28SO",0x15e,163,106,28,1,49152},
 {"Hyundai(Hynix):GMS81016T *44",0x15f,163,107,44,1,49152},
 {"Hyundai(Hynix):GMS81024T *20SO",0x160,164,104,20,1,40960},
 {"Hyundai(Hynix):GMS81024T *24SO",0x161,164,105,24,1,40960},
 {"Hyundai(Hynix):GMS81024T *28SO",0x162,164,106,28,1,40960},
 {"Hyundai(Hynix):GMS81024T *44",0x163,164,107,44,1,40960},
 {"Hyundai(Hynix):GMS81032T *20SO",0x164,165,104,20,1,32768},
 {"Hyundai(Hynix):GMS81032T *24SO",0x165,165,105,24,1,32768},
 {"Hyundai(Hynix):GMS81032T *28SO",0x166,165,106,28,1,32768},
 {"Hyundai(Hynix):GMS81032T *44",0x167,165,107,44,1,32768},
 {"Hyundai(Hynix):GMS81032TL *20SO",0x173,172,116,20,3,32768},
 {"Hyundai(Hynix):GMS81032TL *24SO",0x174,172,117,24,3,32768},
 {"Hyundai(Hynix):GMS81032TL *28SO",0x175,172,118,28,3,32768},
 {"Hyundai(Hynix):GMS81032TL *44",0x176,172,119,44,3,32768},
 {"Hyundai(Hynix):GMS87C5032",0x177,173,120,40,2,32768},
};
static const struct hy25_profile*hy25_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2000||strcmp(s->definition_stem,"GA_____")||s->capacity_bytes!=65536||s->element_bits!=8||strcmp(s->catalogue.vendor,"Hyundai(Hynix)"))return NULL;
 for(size_t i=0;i<sizeof hy25_profiles/sizeof *hy25_profiles;i++){const struct hy25_profile*p=&hy25_profiles[i];const char*name=strchr(p->key,':')+1;
 if(p->stage<=LT48_HYUNDAI25_STAGE&&strlen(name)==s->catalogue.device_name_length&&!memcmp(name,s->catalogue.device_name,s->catalogue.device_name_length)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins)return p;}
 return NULL;
}
