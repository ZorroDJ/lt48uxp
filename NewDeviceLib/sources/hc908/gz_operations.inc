/* Typed per-array controller routing for GZ. Both GZ60 protection bytes are
 * physically in Flash1, so erase that array first. Original selected monitor
 * entry/serial helpers and source cleanup stay in use. */
static const struct hc908_gz_bank *gz_bank(const lt48_hc908_resources*r,unsigned a){
 for(size_t i=0;i<sizeof hc908_gz_banks/sizeof *hc908_gz_banks;i++){
  const struct hc908_gz_bank*b=hc908_gz_banks+i;
  if(b->definition==r->definition&&a>=b->lo&&a<b->hi)return b;
 }
 return NULL;
}
static void gz_ram(unsigned char*code,size_t n,const struct hc908_gz_bank*b){
 code[7]=(unsigned char)(b->bpr>>8);code[8]=(unsigned char)b->bpr;
 for(size_t i=0;i+2<n;i++)if(code[i]==0xc7&&code[i+1]==0xfe&&code[i+2]==8){
  code[i+1]=(unsigned char)(b->control>>8);code[i+2]=(unsigned char)b->control;i+=2;
 }
}
static lt48_status gz_erase(hc908_session*s){
 lt48_status st=jk_enter(s,500,1);
 for(size_t i=0;st==LT48_OK&&i<sizeof hc908_gz_banks/sizeof *hc908_gz_banks;i++){
  const struct hc908_gz_bank*b=hc908_gz_banks+i;if(b->definition!=s->r->definition)continue;
  unsigned char code[64];size_t n=hc908_erase_ram(code,b->dummy);
  if(n<32||code[29]!=0xa6||code[30]!=12){st=LT48_SAFETY_REJECTED;break;}
  code[20]=(unsigned char)(s->r->gx->erase_loops>>8);code[21]=(unsigned char)s->r->gx->erase_loops;
  code[30]=8; /* clear ERASE and MASS before the >=100us high-voltage hold */
  code[n-4]=(unsigned char)s->r->gx->erase_delay;gz_ram(code,n,b);
  st=execute_ram(s,code,n,NULL,0);
 }
 lt48_status end=jk_exit(s);return st==LT48_OK?end:st;
}
