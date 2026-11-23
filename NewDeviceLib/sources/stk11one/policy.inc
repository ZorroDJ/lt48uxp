/* Source address keys, Cypress001-50638 pp4/11 software command contract.
 * Hold OE and WE high throughout all six CE-controlled reads. This avoids
 * an additional OE clock edge; no bus capture is required to clock the key.
 * The source library remains intact. No NE pin is added to this part. */
static lt48_status stk11one_sequence(device*d,unsigned role,unsigned last){
 unsigned char body[128];size_t n=0;
 const unsigned char lead[]={0x11,4,1,0x11,17,1,0x11,3,1,0x12,5,2};
 memcpy(body,lead,sizeof lead);n=sizeof lead;
 const unsigned addresses[]={0,0x1555,0x0aaa,0x1fff,0x10f0,last};
 for(unsigned i=0;i<6;i++){
  body[n++]=0x17;body[n++]=4;
  for(unsigned b=0;b<4;b++)body[n++]=(unsigned char)(addresses[i]>>(8*b));
  body[n++]=0x11;body[n++]=4;body[n++]=0;
  body[n++]=0x32;body[n++]=100;body[n++]=0;body[n++]=0;body[n++]=0;
  body[n++]=0x11;body[n++]=4;body[n++]=1;
 }
 body[n++]=0x32;body[n++]=0x20;body[n++]=0x4e;body[n++]=0;body[n++]=0;
 return compile(d,role,body,n);
}
static lt48_status stk11one_policy(device*d,const lt48_pin_code*code,const char*root){
 if(code->pin_count!=28)return LT48_SAFETY_REJECTED;
 lt48_status s=lt48_pin_map_parse(code,&d->pins);d->v=(lt48_compile_variable){0,0,1,500,0};
 if(s==LT48_OK)s=resource_wave(d,"Wc8",0,0);
 if(s==LT48_OK)s=resource_wave(d,"W2",1,0);
 if(s==LT48_OK)s=resource_wave(d,"Wcb",2,1);
 if(s==LT48_OK)s=resource_wave(d,"Wcc",3,1);
 if(s==LT48_OK)s=stk11one_sequence(d,4,0x0f0f);
 if(s==LT48_OK)s=stk11one_sequence(d,5,0x0f0e);
 if(s==LT48_OK)s=lt48_load_verified_idle_fpga_internal(root,&d->idle,&d->idle_size);
 return s;
}
