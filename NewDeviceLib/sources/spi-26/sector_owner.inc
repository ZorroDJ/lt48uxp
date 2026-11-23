/* Source-bound MX25L1655D/3255D per-sector protection. No OTP lock writes.
 * The source F3 unprotect command is exposed as authored wave12; readback
 * uses the original FB sector query, preserving its address interpretation. */
static lt48_status sv_sector_protection(as_session *s,int secure) {
 const struct as_closure*p=s->r->closure;
 s->mode=secure?13u:0u;set32(s->buffers[0],0);
 lt48_status st=run(s,2);
 if(!secure && st==LT48_OK)st=run(s,12);
 for(size_t i=0;st==LT48_OK && i<p->secondary_count;i++) {
  if(p->secondary[i]>=p->segments[0].length)continue;
  set32(s->buffers[0],p->secondary[i]);set32(s->buffers[4],secure?1u:0u);
  if(secure) {
   st=run(s,11);
   if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
  }
  if(st==LT48_OK)st=run(s,6);
  if(st==LT48_OK && s->buffers[3][0]!=(secure?1u:0u))st=LT48_DEVICE_MISMATCH;
 }
 if(st==LT48_OK)st=run(s,3);
 return st;
}
