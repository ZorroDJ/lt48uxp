/* GC/GCB byte-image files. Source: 00477620, 0042cfb0, 0042d270.
 * File import matches original format/size compatibility, using file masks.
 * Export uses the selected layout. Vectors require a separate owner. */
static lt48_status pof_file_layout(const lt48_pof_core *c,size_t *file_size) {
    if(!c||!file_size)return LT48_INVALID_ARGUMENT;
    if(c->format!=1)return LT48_UNSUPPORTED;
    size_t total=0,n=12;unsigned meta=0;
    for(size_t i=0;i<c->unit_count;i++) {
        lt48_pof_core_unit u;lt48_status s=lt48_pof_core_get_unit(c,i,&u);
        if(s!=LT48_OK)return s;
        if(u.tag==2) {
            if((meta&1)||!u.payload_size||u.value!=u.payload_size||u.payload[u.payload_size-1])return LT48_FORMAT_ERROR;
            meta|=1;
        } else if(u.tag==8) {
            if((meta&2)||u.value!=2||u.payload_size!=2)return LT48_FORMAT_ERROR;
            meta|=2;
        } else if(u.tag==7||u.tag==17) {
            if(u.payload_size!=12||u.value<12)return LT48_FORMAT_ERROR;
            uint64_t span=u.tag==7?(uint64_t)r16(u.payload+4)*r16(u.payload+6)*(1+(r16(u.payload+10)&1)):r32(u.payload+6);
            if(span!=u.value-12)return LT48_UNSUPPORTED;
            if(span>SIZE_MAX-total)return LT48_FORMAT_ERROR;
            total+=(size_t)span;
        } else return LT48_UNSUPPORTED;
        if(n>SIZE_MAX-6||u.value>SIZE_MAX-n-6)return LT48_FORMAT_ERROR;
        n+=6+u.value;
    }
    if(meta!=3||!total||total!=c->total_bytes)return LT48_FORMAT_ERROR;
    *file_size=n;return LT48_OK;
}
lt48_status lt48_pof_core_export_internal(const lt48_pof_core *c,
    const unsigned char *input,size_t size,int security,
    unsigned char *file,size_t capacity,size_t *written) {
    if(!c||!input||!written||size!=c->total_bytes||security<0||security>1)return LT48_INVALID_ARGUMENT;
    size_t n;lt48_status s=pof_file_layout(c,&n);if(s!=LT48_OK)return s;
    if(security && (n>SIZE_MAX-8||c->unit_count==UINT32_MAX))return LT48_FORMAT_ERROR;
    n+=security?8:0;*written=n;
    if(!file)return LT48_OK;
    if(capacity<n)return LT48_BUFFER_TOO_SMALL;
    memcpy(file,"POF\0",4);w32(file+4,0x10000);w32(file+8,c->unit_count+(unsigned)security);
    size_t at=12,offset=0;
    for(size_t i=0;i<c->unit_count;i++) {
        lt48_pof_core_unit u;s=lt48_pof_core_get_unit(c,i,&u);if(s!=LT48_OK)return s;
        if(u.tag==8&&security){w16(file+at,5);w32(file+at+2,2);w16(file+at+6,1);at+=8;}
        w16(file+at,u.tag);w32(file+at+2,u.value);at+=6;
        if(u.tag==8)w16(file+at,0);
        else memcpy(file+at,u.payload,u.payload_size);
        if(u.tag==7||u.tag==17){size_t span=u.value-12;memcpy(file+at+12,input+offset,span);offset+=span;}
        at+=u.value;
    }
    return LT48_OK;
}
lt48_status lt48_pof_core_import_metadata_internal(const lt48_pof_core *c,
    const unsigned char *file,size_t size,unsigned char *output,size_t capacity,lt48_pof_file_metadata *metadata) {
    if(!c||!file||!output||!metadata||capacity!=c->total_bytes)return LT48_INVALID_ARGUMENT;
    size_t ignored;lt48_status s=pof_file_layout(c,&ignored);if(s!=LT48_OK)return s;
    if(size<12||memcmp(file,"POF\0",4)||r32(file+4)!=0x10000||r32(file+8)>(size-12)/6)return LT48_FORMAT_ERROR;
    unsigned char *pending=malloc(capacity);if(!pending)return LT48_NO_MEMORY;
    size_t at=12,offset=0;unsigned meta=0,format=0;int secure=0;
    lt48_pof_file_metadata pending_metadata={0};
    for(uint32_t i=0;i<r32(file+8);i++) {
        if(size-at<6){s=LT48_FORMAT_ERROR;break;}
        unsigned tag=r16(file+at);size_t n=r32(file+at+2);at+=6;
        if(n>size-at){s=LT48_FORMAT_ERROR;break;}
        if(tag==5){
            if((meta&4)||n!=2||r16(file+at)>1){s=LT48_FORMAT_ERROR;break;}
            meta|=4;secure=r16(file+at);
        } else if(tag==2){
            if((meta&1)||!n||n>256||file[at+n-1]){s=LT48_FORMAT_ERROR;break;}
            meta|=1; /* Names can be aliases; selected data layout is decisive. */
        } else if(tag==8){
            if((meta&2)||n!=2){s=LT48_FORMAT_ERROR;break;}meta|=2;
        } else if(tag==10){
            /* Original 00477620: DWORD row count, then equally sized rows,
             * each with one trailing NUL. Keep the file's own pin width. */
            if((meta&8)||n<6){s=LT48_FORMAT_ERROR;break;}
            uint32_t rows=r32(file+at);
            if(!rows||(n-4)%rows){s=LT48_FORMAT_ERROR;break;}
            size_t stride=(n-4)/rows;
            if(stride<2||stride>32767){s=LT48_FORMAT_ERROR;break;}
            for(uint32_t row=0;row<rows;row++) {
                const unsigned char *v=file+at+4+(size_t)row*stride;
                if(v[stride-1]||memchr(v,0,stride-1)){s=LT48_FORMAT_ERROR;break;}
            }
            if(s!=LT48_OK)break;
            pending_metadata.vectors=malloc(n-4);
            if(!pending_metadata.vectors){s=LT48_NO_MEMORY;break;}
            memcpy(pending_metadata.vectors,file+at+4,n-4);
            pending_metadata.row_count=rows;pending_metadata.pin_count=(uint32_t)stride-1;
            meta|=8;
        } else if(tag==7||tag==17){
            unsigned record_format=tag==7?1u:2u;
            if(n<12){s=LT48_FORMAT_ERROR;break;}
            if((format&&format!=record_format)||record_format!=c->format){s=LT48_DEVICE_MISMATCH;break;}
            format=record_format;
            size_t span=n-12;
            if(span>capacity-offset){s=LT48_DEVICE_MISMATCH;break;}
            /* 0042cfb0 matches total bytes and format, not coordinates or
             * partitioning. The selected device core controls later operations.
             * 00477620 supplies each file record's low mask byte. */
            unsigned char mask=tag==7?file[at+8]:0xff;
            for(size_t k=0;k<span;k++)pending[offset+k]=file[at+12+k]&mask;
            offset+=span;
        } else if(tag!=1&&tag!=3&&tag!=4&&tag!=9&&tag!=14&&tag!=16){s=LT48_UNSUPPORTED;break;}
        at+=n;
    }
    if(s==LT48_OK&&(at!=size||(meta&3)!=3))s=LT48_FORMAT_ERROR;
    if(s==LT48_OK&&(offset!=capacity||format!=c->format))s=LT48_DEVICE_MISMATCH;
    if(s==LT48_OK){
        memcpy(output,pending,capacity);pending_metadata.security=secure;
        lt48_pof_file_metadata_clear(metadata);*metadata=pending_metadata;
        pending_metadata.vectors=NULL;
    }
    lt48_pof_file_metadata_clear(&pending_metadata);
    free(pending);return s;
}

void lt48_pof_file_metadata_clear(lt48_pof_file_metadata *m) {
    if(m){free(m->vectors);memset(m,0,sizeof(*m));}
}
lt48_status lt48_pof_core_import_internal(const lt48_pof_core *c,
    const unsigned char *file,size_t size,unsigned char *output,size_t capacity,int *security) {
    if(!c||!output||!security||capacity!=c->total_bytes)return LT48_INVALID_ARGUMENT;
    unsigned char *pending=malloc(capacity);if(!pending)return LT48_NO_MEMORY;
    lt48_pof_file_metadata metadata={0};
    lt48_status s=lt48_pof_core_import_metadata_internal(c,file,size,pending,capacity,&metadata);
    /* The old byte-only API must not silently discard functional vectors. */
    if(s==LT48_OK&&metadata.row_count)s=LT48_UNSUPPORTED;
    if(s==LT48_OK){memcpy(output,pending,capacity);*security=metadata.security;}
    lt48_pof_file_metadata_clear(&metadata);free(pending);return s;
}
lt48_status lt48_pof_core_export_metadata_internal(const lt48_pof_core *c,
    const unsigned char *image,size_t size,const lt48_pof_file_metadata *m,
    unsigned char *file,size_t capacity,size_t *written) {
    if(!m||!written)return LT48_INVALID_ARGUMENT;
    size_t extra=0;
    if(m->row_count) {
        if(!m->vectors||!m->pin_count||m->pin_count>=32767)return LT48_INVALID_ARGUMENT;
        uint64_t bytes=(uint64_t)m->row_count*(m->pin_count+1u);
        if(bytes>UINT32_MAX-4u||bytes>SIZE_MAX-10u)return LT48_INVALID_ARGUMENT;
        size_t stride=(size_t)m->pin_count+1;
        for(uint32_t row=0;row<m->row_count;row++) {
            const char *v=m->vectors+(size_t)row*stride;
            if(v[stride-1]||memchr(v,0,stride-1))return LT48_FORMAT_ERROR;
        }
        extra=10+(size_t)bytes;
    } else if(m->vectors||m->pin_count)return LT48_INVALID_ARGUMENT;
    size_t base=0;lt48_status s=lt48_pof_core_export_internal(c,image,size,m->security,NULL,0,&base);
    if(s!=LT48_OK)return s;
    if(base>SIZE_MAX-extra|| (extra&&c->unit_count>=UINT32_MAX-(unsigned)m->security))return LT48_FORMAT_ERROR;
    *written=base+extra;if(!file)return LT48_OK;
    if(capacity<*written)return LT48_BUFFER_TOO_SMALL;
    s=lt48_pof_core_export_internal(c,image,size,m->security,file,capacity,&base);
    if(s!=LT48_OK)return s;
    if(extra){
        w32(file+8,r32(file+8)+1);w16(file+base,10);w32(file+base+2,(uint32_t)extra-6);
        w32(file+base+6,m->row_count);memcpy(file+base+10,m->vectors,extra-10);
    }
    return LT48_OK;
}
