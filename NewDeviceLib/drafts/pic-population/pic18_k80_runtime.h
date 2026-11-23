#ifndef LT48_PIC18_K80_RUNTIME_H
#define LT48_PIC18_K80_RUNTIME_H
#include "../../sources/pic-common/source_resources.h"
/* Authored RAM-only P10 correction. Original source files remain untouched.
 * Code's existing DJNZ hold changes10->200 iterations. Protect's two paths
 * receive208NOPs after lowering PGC, preserving every register/flag and all
 * existing addresses. Patches require complete exact original SHA256s. */
static inline lt48_status lt48_pic18_k80_recovery_patch(unsigned role,
 const unsigned char *original,size_t size,uint16_t base,unsigned char *out,
 size_t capacity,size_t *used) {
 static const unsigned char program_sha[32]={0x91,0x6a,0xce,0x54,0x65,0x38,0x2c,0x32,0x3c,0x4c,0xef,0xe7,0x03,0x1e,0x4d,0x1b,0xd7,0xc5,0x25,0xda,0xd6,0x30,0x86,0x5a,0x37,0xb4,0x73,0x25,0x82,0x92,0xa5,0xb0};
 static const unsigned char protect_sha[32]={0x34,0x57,0x9e,0xb2,0xb3,0x53,0x65,0xa1,0xe7,0x8d,0x4c,0xac,0x36,0xa8,0x07,0x37,0x75,0x89,0xd7,0x0a,0xbe,0x4b,0xdf,0x84,0x79,0xf2,0x8c,0xbd,0x4d,0x02,0xa0,0xf5};
 if(!original||!out||!used)return LT48_INVALID_ARGUMENT;
 *used=0;
 size_t required=role==20?1731:role==24?1262:0;
 if(!required||size!=(role==20?1731u:834u)||base!=(role==20?0x8000u:0x9800u))return LT48_SAFETY_REJECTED;
 if(capacity<required)return LT48_BUFFER_TOO_SMALL;
 unsigned char sha[32];lt48_sha256(original,size,sha);
 if(memcmp(sha,role==20?program_sha:protect_sha,32))return LT48_SAFETY_REJECTED;
 memcpy(out,original,size);
 if(role==20) {
  if(memcmp(original+0x2dd,"\xe5\x10\xf0\x7f\x0a\xdf\xfe\x74\xd4\xf0",10))return LT48_SAFETY_REJECTED;
  out[0x2e1]=200;*used=size;return LT48_OK;
 }
 const size_t sites[2]={0x246,0x327};size_t n=size;
 for(unsigned i=0;i<2;i++) {
  size_t site=sites[i];if(memcmp(original+site-3,"\xe5\x10\xf0\x74\xd4\xf0",6))return LT48_SAFETY_REJECTED;
  unsigned target=(unsigned)(base+n),resume=(unsigned)(base+site+3);
  out[site]=2;out[site+1]=(unsigned char)(target>>8);out[site+2]=(unsigned char)target;
  memset(out+n,0,208);n+=208;
  out[n++]=0x74;out[n++]=0xd4;out[n++]=0xf0;
  out[n++]=2;out[n++]=(unsigned char)(resume>>8);out[n++]=(unsigned char)resume;
 }
 if(n!=required)return LT48_SAFETY_REJECTED;
 *used=n;return LT48_OK;
}
static inline lt48_status lt48_pic18_k80_runtime_open(const char *path,const unsigned char digest[32],
 uint16_t family,uint16_t pinmap,lt48_pic_source_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_pic_source_resources *before=NULL;
 lt48_status st=lt48_pic_source_resources_library_internal(path,digest,family,pinmap,&before);
 if(st!=LT48_OK)return st;
 unsigned char buffers[2][1731];lt48_pic_source_ram_overlay overlays[2]={{0}};unsigned found=0;
 for(size_t i=0;st==LT48_OK&&i<lt48_pic_source_resources_count_internal(before);i++) {
  lt48_pic_source_resource_entry e;st=lt48_pic_source_resources_entry_internal(before,i,&e);if(st!=LT48_OK)break;
  if(e.role!=20&&e.role!=24)continue;
  if(found==2||e.source.wave_type!=6){st=LT48_SAFETY_REJECTED;break;}
  lt48_pic_source_ram_overlay *o=overlays+found;
  st=lt48_pic18_k80_recovery_patch(e.role,e.source.data,e.source.data_size,e.source.load_address,buffers[found],sizeof(buffers[found]),&o->size);
  if(st!=LT48_OK)break;
  o->role=e.role;o->load_address=e.source.load_address;o->slot_end=e.role==20?0x9000:0xa000;o->original_size=e.source.data_size;o->data=buffers[found];
  lt48_sha256(e.source.data,e.source.data_size,o->original_sha256);lt48_sha256(o->data,o->size,o->sha256);found++;
 }
 if(st==LT48_OK&&found!=2)st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK)st=lt48_pic_source_resources_overlay_library_internal(path,digest,family,pinmap,overlays,2,out);
 lt48_pic_source_resources_close_internal(before);return st;
}
#endif
