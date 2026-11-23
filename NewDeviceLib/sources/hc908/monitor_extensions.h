#ifndef LT48_HC908_MONITOR_EXTENSIONS_H
#define LT48_HC908_MONITOR_EXTENSIONS_H
/* Authored MCS-51 extensions. Existing serial helpers are borrowed at their
 * exact source-guarded addresses (TX58C8/RX5904). They are not copied here.
 * Bytes below encode MOV/LCALL/RET; see monitor_extensions.md. */
static size_t hc908_send(unsigned char *p,unsigned value) {
 const unsigned char b[]={0x79,(unsigned char)value,0x12,0x58,0xc8,0x12,0x59,0x04};
 memcpy(p,b,sizeof b);return sizeof b;
}
static size_t hc908_read_preface(unsigned char *p,unsigned first) {
 size_t n=0;const unsigned addresses[]={0x80,0xffc0,0xffc1};
 for(unsigned i=0;i<3;i++) {
  n+=hc908_send(p+n,0x4a);n+=hc908_send(p+n,addresses[i]>>8);n+=hc908_send(p+n,addresses[i]&255);
  const unsigned char capture[]={0x12,0x59,0x04,0x20,0xb2,0xfd,0xea,0xf0};
  memcpy(p+n,capture,sizeof capture);n+=sizeof capture;
 }
 const unsigned char resume[]={0x7c,(unsigned char)(first>>8),0x7d,0,0x7e,0,0x22};
 memcpy(p+n,resume,sizeof resume);return n+sizeof resume;
}
/* Authenticated MON08 bridge. Host frame: TX-count, RX-extra-count, TX bytes.
 * Return every echo followed by RX bytes. Source serial helpers retain their
 * timer bounds. Q1 zero exits through the source Read cleanup; Q2 is data.
 * R4/R5 survive the borrowed helpers; R0 stays unchanged. */
static size_t hc908_monitor_bridge(unsigned char *p) {
 const unsigned char body[]={
  0x90,0xc8,0,0x75,0xa0,0xf0,
  0x20,0xb2,0xfd,0x30,0xb5,0,
  0xe0,0xfc,0x20,0xb2,0xfd,0xe0,0xfd,
  0x20,0xb2,0xfd,0xe0,0xf9,0x12,0x58,0xc8,0x12,0x59,4,
  0x20,0xb2,0xfd,0xea,0xf0,0xdc,0,
  0xed,0x60,0,0x12,0x59,4,0x20,0xb2,0xfd,0xea,0xf0,0xdd,0,
  0x80,0,0x02,0x52,0x62};
 memcpy(p,body,sizeof body);
 p[11]=(unsigned char)(52-12);p[36]=(unsigned char)(19-37);
 p[39]=(unsigned char)(6-40);p[49]=(unsigned char)(40-50);p[51]=(unsigned char)(6-52);
 return sizeof body;
}
#endif
