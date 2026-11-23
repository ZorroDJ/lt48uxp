/* Authored exact IB2 extension of the audited IC2 engine. Vendor assets BYO. */
#ifndef MCU70_IC2_PROFILE_H
#define MCU70_IC2_PROFILE_H
static inline int mcu70_ic2_profile(unsigned d,unsigned m,unsigned p,unsigned n){
 return p==52 && ((d==0xee&&m==123&&n==32768)||(d==0xeb&&m==121&&n==16384));
}
static inline int mcu70_ib2_spec(const lt48_device_spec*s){
 return s->catalogue.device_class==2018 && s->definition_resource==0xeb &&
  s->family_code==121 && s->pinmap==52 && s->capacity_bytes==16384 &&
  s->pin_count==44 && !strcmp(s->catalogue.vendor,"Atmel") &&
  s->catalogue.device_name_length==sizeof("T89C51IB2 *44")-1 &&
  !memcmp(s->catalogue.device_name,"T89C51IB2 *44",sizeof("T89C51IB2 *44")-1);
}
#define MCU70_IB2_CLOSURE {0xeb,121,52,6,10,16384,16384,\
 {[0]=0,[1]=2,[2]=245,[3]=246,[4]=247,[6]=248,[8]=250,[10]=249,[12]=251,[21]=8},\
 (1u<<0)|(1u<<1)|(1u<<2)|(1u<<3)|(1u<<4)|(1u<<6)|(1u<<8)|(1u<<10)|(1u<<12)|(1u<<21)}
#endif
