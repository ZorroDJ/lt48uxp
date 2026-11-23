/* Authored source-bound OTP entry selector/address edits. Original callbacks,
 * MCU applets and FPGA images remain external BYO resources. */
static int sv_is_micron(unsigned d){return d==0x19d||d==0x19e||d==0x19f||d==0xf7||d==0xf8||d==0xf9;}
static lt48_status sv_replace(unsigned char*p,size_t*n,const unsigned char*old,size_t a,const unsigned char*new_bytes,size_t b,int first){
 size_t at=0,hits=0;for(size_t i=2;i+a<=*n;i++)if(!memcmp(p+i,old,a)){if(!hits)at=i;hits++;}
 if(!hits||(!first&&hits!=1))return LT48_SAFETY_REJECTED;
 memmove(p+at+b,p+at+a,*n-at-a);memcpy(p+at,new_bytes,b);*n=*n-a+b;return LT48_OK;
}
static lt48_status sv_correct_otp_selector(unsigned char*p,size_t*n,unsigned role){
 lt48_status st=LT48_OK;
 if(role==6){
  const unsigned char old[]={0x3d,17,0x3b,0,0x39,5},fix[]={0x3d,17,0x3b,1,0x39,5};
  size_t hits=0;for(size_t i=2;i+sizeof(old)<=*n;i++)if(!memcmp(p+i,old,sizeof(old)))hits++;
  if(hits)return sv_replace(p,n,old,sizeof(old),fix,sizeof(fix),0);
 }
 if(role==6||role==11){
  unsigned char old[]={0x3d,role==6?17:20,0x39,5};
  unsigned char fix[]={0x3d,role==6?17:20,0x3b,1,0x39,5};
  st=sv_replace(p,n,old,sizeof(old),fix,sizeof(fix),0);
 }else if(role==5||role==9){
  unsigned char old[]={0x3d,role==5?17:21,0x3b,0,0x39,5};
  unsigned char fix[]={0x3d,role==5?17:21,0x3b,1,0x39,5};
  st=sv_replace(p,n,old,sizeof(old),fix,sizeof(fix),1);
 }else if(role==7){
  for(unsigned i=0;st==LT48_OK&&i<2;i++){
   unsigned char old[]={0x40,0x17,4,i?0x13:0x4b,0,0,0};
   unsigned char fix[]={0x40,0x17,4,i?0x13:0x4b,0,0,0,0x40,0x12,4,i?0:1,0,0,0};
   st=sv_replace(p,n,old,sizeof(old),fix,sizeof(fix),0);
  }
  const unsigned char old[]={0x3d,19,0x3b,0,0x39,5},fix[]={0x3d,19,0x3e,18,0,0x39,5};
  if(st==LT48_OK)st=sv_replace(p,n,old,sizeof(old),fix,sizeof(fix),0);
  const unsigned char old_address[]={0x3e,0,3},address[]={0x5d,1,5,16,0x5f,0x3e,0,3,0x60};
  if(st==LT48_OK)st=sv_replace(p,n,old_address,sizeof(old_address),address,sizeof(address),0);
 }else return LT48_INVALID_ARGUMENT;
 p[0]=(unsigned char)(*n-2);p[1]=(unsigned char)((*n-2)>>8);return st;
}
