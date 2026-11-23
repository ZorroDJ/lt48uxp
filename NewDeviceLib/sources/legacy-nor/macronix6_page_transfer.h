/* Authored private x16 page host transport.
 * Evidence: original page-dispatch/page-ranges, program-applet and program-rtl
 * audits in analysis/legacy-macronix6-batch. Public execution stays gated.
 * Byte ranges at this boundary; only complete source-sized program pages accepted.
 */
lt48_status lt48_legacy_macronix6_stream_internal(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 const lt48_protocol_transport *api,void *ctx,unsigned mode,unsigned vcc,
 unsigned char *bytes,size_t size,uint32_t start) {
 if(!r||!r->legacy_page||!frame||!wire||!api||!api->read||!api->write||
    !bytes||!size||start>=r->capacity||size>r->capacity-start||vcc>65535||
    (mode!=1&&mode!=2))return LT48_INVALID_ARGUMENT;
 if((!r->legacy_page_byte&&((start&1)||(size&1)))||(mode==2&&((start&(r->page_bytes-1))||(size&(r->page_bytes-1)))))return LT48_SAFETY_REJECTED;
 int found=0;
 for(size_t i=0;i<r->variable_count;i++)if(frame->variables[i].kind==0&&frame->variables[i].variable_id==r->vcc_variable) {
  frame->variables[i].value=(uint16_t)vcc;found=1;
 }
 if(!found)return LT48_FORMAT_ERROR;
 frame->mode=mode;
 lt48_status status=lt48_runtime_frame_set_mode(frame->buffers,r->frame_profile.buffer_count,
  r->frame_profile.reset_mask,r->frame_profile.mode_buffer,(uint16_t)mode,0,0);
 /* Word profiles double addresses in DEVINIT; byte profiles retain them. */
 if(status==LT48_OK)for(unsigned i=0;i<4;i++)frame->buffers[2][i]=(unsigned char)((start/(r->legacy_page_byte?1u:2u))>>(8*i));
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,3);
 for(size_t offset=0;status==LT48_OK&&offset<size;) {
  if(api->cancelled&&api->cancelled(ctx))return LT48_CANCELLED;
  size_t n=size-offset,limit=mode==2?r->page_bytes:8192u;if(n>limit)n=limit;
  if(mode==2) {
   /* The original page dispatcher supplies a word address for every page;
    * PROGRAM copies it and doubles the copy for its applet prefix. */
   uint32_t address=(start+(uint32_t)offset)/(r->legacy_page_byte?1u:2u);
   for(unsigned i=0;i<4;i++)frame->buffers[2][i]=(unsigned char)(address>>(8*i));
   status=lt48_gaaf_serial_wave(r,frame,wire,5);
  }
  unsigned char h[]={mode==2?0x44:0x46,n/512&255,n/512>>8,n%512&255,n%512>>8,mode==1?0xd0:0};
  if(status==LT48_OK)status=gaaf_command(api,ctx,wire,h,sizeof h,NULL);
  if(status==LT48_OK)status=gaaf_io(api,ctx,mode==1,mode==1?0x88:4,bytes+offset,n);
  if(status==LT48_OK&&mode==2&&r->w29wordtwo) {
   status=lt48_gaaf_serial_wave(r,frame,wire,19);
   if(status==LT48_OK&&frame->buffers[7][0]!=1)status=LT48_DEVICE_MISMATCH;
  }
  if(status==LT48_OK&&mode==2)status=lt48_runtime_transport_session_status(wire,1);
  /* The source applet leaves OE active during its reset attempt. Explicitly
   * deselect/disable outputs before the documented reset command sequence. */
  if(status==LT48_OK&&mode==2&&r->page_read_reset)status=lt48_gaaf_serial_wave(r,frame,wire,LT48_GAAF_OPERATIONS);
  if(status==LT48_OK)offset+=n;
 }
 if(status==LT48_OK) {
  const unsigned char stop[]={0,0};unsigned char reply[512]={0};
  status=gaaf_command(api,ctx,wire,stop,sizeof stop,reply);
  if(status==LT48_OK&&!(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
 }
 if(status==LT48_OK)status=lt48_gaaf_serial_wave(r,frame,wire,r->reset_op);
 /* The operation owner must always run electrical cleanup on error and must
  * verify programmed data by an independent read before reporting success. */
 return status;
}

/* Private chip erase with mandatory independent full-array blank evidence.
 * The caller owns power setup and unconditional electrical cleanup. */
lt48_status lt48_legacy_macronix6_erase_internal(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 const lt48_protocol_transport *api,void *ctx) {
 if(!r||!r->legacy_page||!frame||!wire||!api||!api->read||!api->write)
  return LT48_INVALID_ARGUMENT;
 if(api->cancelled&&api->cancelled(ctx))return LT48_CANCELLED;
 unsigned char *readback=malloc(r->capacity);
 if(!readback)return LT48_NO_MEMORY;
 int found=0;
 for(size_t i=0;i<r->variable_count;i++)if(frame->variables[i].kind==0&&frame->variables[i].variable_id==r->vcc_variable){frame->variables[i].value=(uint16_t)r->nominal;found=1;}
 if(!found){free(readback);return LT48_FORMAT_ERROR;}
 frame->mode=7;
 lt48_status s=lt48_runtime_frame_set_mode(frame->buffers,r->frame_profile.buffer_count,
  r->frame_profile.reset_mask,r->frame_profile.mode_buffer,7,0,0);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,3);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,13);
 if(s==LT48_OK&&r->w29wordtwo)s=lt48_gaaf_serial_wave(r,frame,wire,19);
 if(s==LT48_OK&&r->w29wordtwo&&frame->buffers[7][0]!=1)s=LT48_DEVICE_MISMATCH;
 if(s==LT48_OK&&r->page_read_reset)s=lt48_gaaf_serial_wave(r,frame,wire,LT48_GAAF_OPERATIONS);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,r->reset_op);
 if(s==LT48_OK)s=lt48_legacy_macronix6_stream_internal(r,frame,wire,api,ctx,
  1,r->nominal,readback,r->capacity,0);
 if(s==LT48_OK)for(size_t i=0;i<r->capacity;i++)if(readback[i]!=0xff){s=LT48_DEVICE_MISMATCH;break;}
 free(readback);return s;
}
