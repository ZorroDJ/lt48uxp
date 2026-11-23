/* Authored native POF mapping and corrected inverse.
 * Included by c/src/pof.c; no embedded vendor tables or payloads. */
/* Walk in original unit/row/column/bit order. Repeated coordinates OR
 * together during synthesis; reconstruction retains source write ordering. */
static lt48_status pof_trs_walk(const lt48_pof_trs_source *source,unsigned kind,
    const lt48_pof_core *core,lt48_pof_trs_image *image,
    const unsigned char *input,unsigned char *output,int pass,unsigned char *seen) {
    size_t offset=0;
    for(size_t i=0;i<core->unit_count;i++) {
        lt48_pof_core_unit unit;
        lt48_status status=lt48_pof_core_get_unit(core,i,&unit);
        if(status!=LT48_OK)return status;
        if(unit.tag==2||unit.tag==8)continue;
        if(unit.tag!=7||unit.payload_size!=12)return LT48_UNSUPPORTED;
        const unsigned char *p=unit.payload;
        unsigned width=1+(r16(p+10)&1),mask=r16(p+8);
        for(unsigned r=0;r<r16(p+4);r++) for(unsigned c=0;c<r16(p+6);c++) {
            if(offset>core->total_bytes||width>core->total_bytes-offset)return LT48_FORMAT_ERROR;
            uint16_t value=pass==1?(width==2?r16(input+offset):input[offset]):0;
            for(unsigned bit=0;bit<width*8;bit++) {
                if(!(mask&(1u<<bit)))continue;
                uint32_t coordinate[3];
                if((uint32_t)r16(p)+r>UINT16_MAX||(uint32_t)r16(p+2)+c>UINT16_MAX)
                    return LT48_FORMAT_ERROR;
                status=lt48_pof_trs_source_coordinate_internal(source,kind,
                    (uint16_t)(r16(p)+r),(uint16_t)(r16(p+2)+c),bit,coordinate);
                if(status!=LT48_OK)return status;
                if(coordinate[0]>UINT16_MAX||coordinate[1]>UINT16_MAX)return LT48_FORMAT_ERROR;
                if(pass==0) {
                    if(image->columns<=coordinate[0])image->columns=coordinate[0]+1;
                    if(image->rows<=coordinate[1])image->rows=coordinate[1]+1;
                } else {
                    if(coordinate[0]>=image->columns||coordinate[1]>=image->rows)return LT48_FORMAT_ERROR;
                    size_t cell=((size_t)coordinate[0]*image->rows+coordinate[1])*4;
                    unsigned char selected=(unsigned char)(1u<<coordinate[2]);
                    if(pass==1) {
                        image->cells[cell]=0;image->cells[cell+1]=1;
                        image->cells[cell+2]|=selected;
                        if(value&(1u<<bit))image->cells[cell+3]|=selected;
                    } else if(image->cells[cell+3]&selected)value|=(uint16_t)(1u<<bit);
                }
            }
            if(pass==2){output[offset]=(unsigned char)value;if(width==2)output[offset+1]=(unsigned char)(value>>8);}
            if(pass==3) {
                /* A source bit may contribute to several cell bits because
                 * two-byte records overlap at a one-byte stride. It may be
                 * one only when ALL those observed cell bits are one. */
                output[offset]&=(unsigned char)(~mask|value);
                seen[offset]|=(unsigned char)mask;
                if(width==2) {
                    output[offset+1]&=(unsigned char)((~mask|value)>>8);
                    seen[offset+1]|=(unsigned char)(mask>>8);
                }
            }
            /* 439690/439940 advance by object+4, the transformed byte width,
             * which is one for both source mappings, not tag-7 width. */
            offset++;
        }
    }
    return LT48_OK;
}

/* Authored inverse: intersect source-bit constraints, clear unmapped bits,
 * then forward-check every observed mapped bit. Publication is caller-owned. */
static lt48_status pof_trs_read_reconstruct(const lt48_pof_trs_source *source,
    unsigned selector,const lt48_pof_core *core,lt48_pof_trs_image *image,
    unsigned char *pending,size_t size,lt48_pof_core_result *result) {
    lt48_status status=LT48_OK;
        unsigned char *seen=calloc(size,1);
        if(!seen)status=LT48_NO_MEMORY;
        else {
            memset(pending,255,size);
            status=pof_trs_walk(source,selector,core,image,NULL,pending,3,seen);
            for(size_t i=0;i<size;i++)pending[i]&=seen[i];
            free(seen);
            /* Verify the reconstructed image represents every observed
             * mapped bit. Arbitrary physical states need not have a POF
             * preimage when the source mapping combines bits with OR. */
            lt48_pof_trs_image *check=NULL;
            if(status==LT48_OK)status=lt48_pof_trs_image_create_internal(source,selector,core,pending,size,&check);
            if(status==LT48_OK) {
                if(check->size!=image->size)status=LT48_FORMAT_ERROR;
                else for(size_t at=0;at<image->size;at+=4) {
                    unsigned char mask=image->cells[at+2];
                    if(((check->cells[at+3]^image->cells[at+3])&mask)!=0) {
                        result->first_mismatch=at;
                        result->expected=check->cells[at+3]&mask;
                        result->actual=image->cells[at+3]&mask;
                        status=LT48_DEVICE_MISMATCH;break;
                    }
                }
            }
            lt48_pof_trs_image_close_internal(check);
        }
    return status;
}
