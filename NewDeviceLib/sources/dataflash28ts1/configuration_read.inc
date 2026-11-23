/* Authored finite GDCA D86/M84/W177 parallel protection reader repair.
 * Same established traversal correction as D26; W177 already uses F0 mask.
 * Vendor bytes remain unchanged on disk and unbundled. */
static lt48_status df28_readcfg(unsigned char*b,size_t*n){
 static const unsigned char extra[]={0x3b,0xff,0x7c,0x19};
 static const unsigned char stale[]={0x5e,0x19,0,0xf0,0,0,0};
 size_t a=SIZE_MAX,z=SIZE_MAX;unsigned na=0,nz=0;if(*n!=536)return LT48_SAFETY_REJECTED;
 for(size_t i=0;i+sizeof stale<=*n;i++)if(!memcmp(b+i,stale,sizeof stale)){z=i;nz++;}
 for(size_t i=0;i+sizeof extra<=*n;i++)if(!memcmp(b+i,extra,sizeof extra)){a=i;na++;}
 if(na!=1||nz!=1||a>=z)return LT48_SAFETY_REJECTED;
 b[z+1]=3;memmove(b+a,b+a+sizeof extra,*n-a-sizeof extra);*n-=sizeof extra;
 unsigned size=b[0]|(unsigned)b[1]<<8;if(size<sizeof extra)return LT48_FORMAT_ERROR;
 size-=sizeof extra;b[0]=size;b[1]=size>>8;return LT48_OK;
}
