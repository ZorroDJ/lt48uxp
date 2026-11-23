#include <labtool48/labtool48.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
static uint16_t r16(const unsigned char*p){return p[0]|(uint16_t)p[1]<<8;}
static uint32_t r32(const unsigned char*p){return r16(p)|(uint32_t)r16(p+2)<<16;}
static void w16(unsigned char*p,uint16_t v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);}
static void w32(unsigned char*p,uint32_t v){w16(p,(uint16_t)v);w16(p+2,(uint16_t)(v>>16));}
/* 00477620/0042cfb0: bounded record parser; 004609f0/00460c70:
 * tag17 packed fuse values are LSB first. Never publish a partial image. */
lt48_status lt48_pof_fuse_import(const lt48_pof_fuse_core*c,const unsigned char*d,
    size_t n,unsigned char*f,size_t count,int*security){
    if(!c||!d||!f||!security||count!=c->fuse_count)return LT48_INVALID_ARGUMENT;
    if(c->packed_size!=count/8+(count%8!=0))return LT48_FORMAT_ERROR;
    if(n<12||memcmp(d,"POF\0",4)||r32(d+4)!=0x10000)return LT48_FORMAT_ERROR;
    size_t at=12;uint32_t records=r32(d+8);const unsigned char*bits=NULL;int secure=0;unsigned seen=0;
    if(records>(n-12)/6)return LT48_FORMAT_ERROR;
    for(uint32_t i=0;i<records;i++){
        if(n-at<6)return LT48_FORMAT_ERROR;
        unsigned tag=r16(d+at);size_t size=r32(d+at+2);at+=6;
        if(size>n-at)return LT48_FORMAT_ERROR;
        if(tag==17){
            if(bits||size!=12+c->packed_size||memcmp(d+at,c->header,12))return LT48_DEVICE_MISMATCH;
            bits=d+at+12;
        }else if(tag==5){if(size!=2||(seen&1)||r16(d+at)>1)return LT48_FORMAT_ERROR;seen|=1;secure=r16(d+at);}
        else if(tag==2){if(!size||size>256||(seen&2)||d[at+size-1])return LT48_FORMAT_ERROR;seen|=2;}
        else if(tag==8){if(size!=2||(seen&4))return LT48_FORMAT_ERROR;seen|=4;}
        else if(tag!=1&&tag!=3&&tag!=4&&tag!=9&&tag!=14&&tag!=16)return LT48_UNSUPPORTED;
        at+=size;
    }
    if(at!=n||!bits||!(seen&2))return LT48_FORMAT_ERROR;
    for(size_t i=0;i<count;i++)f[i]=(bits[i/8]>>(i%8))&1u;
    *security=secure;return LT48_OK;
}
/* 0042d270 emits version00010000, selected core records and zero tag8;
 * optional tag5 carries security. No unimplemented vector records emitted. */
lt48_status lt48_pof_fuse_export(const lt48_pof_fuse_core*c,const unsigned char*f,
    size_t count,int security,unsigned char*d,size_t capacity,size_t*size){
    if(!c||!f||!size||count!=c->fuse_count||security<0||security>1)return LT48_INVALID_ARGUMENT;
    if(!c->device_name||!c->device_name_size||c->device_name_size>256||c->packed_size!=count/8+(count%8!=0))return LT48_FORMAT_ERROR;
    for(size_t i=0;i<count;i++)if(f[i]>1)return LT48_INVALID_ARGUMENT;
    *size=12+6+c->device_name_size+6+12+c->packed_size+8+(security?8:0);
    if(!d)return LT48_OK;
    if(capacity<*size)return LT48_BUFFER_TOO_SMALL;
    memset(d,0,*size);memcpy(d,"POF\0",4);w32(d+4,0x10000);w32(d+8,security?4:3);
    size_t at=12;w16(d+at,2);w32(d+at+2,(uint32_t)c->device_name_size);at+=6;
    memcpy(d+at,c->device_name,c->device_name_size);at+=c->device_name_size;
    w16(d+at,17);w32(d+at+2,12+c->packed_size);at+=6;memcpy(d+at,c->header,12);at+=12;
    for(size_t i=0;i<count;i++)d[at+i/8]|=f[i]<<(i%8);
    at+=c->packed_size;
    if(security){w16(d+at,5);w32(d+at+2,2);w16(d+at+6,1);at+=8;}
    w16(d+at,8);w32(d+at+2,2);return LT48_OK;
}

/* TPofGroup 0045de40. Layout coordinates are words; image offsets are bytes.
 * Row exchange is supplied by the source-bound family owner. */
#include "internal.h"
#include <stdlib.h>
lt48_status lt48_pof_core_run_internal(const lt48_pof_core *core,unsigned mode,
    unsigned char blank,const unsigned char *input,size_t size,
    unsigned char *output,lt48_pof_core_exchange exchange,void *context,
    lt48_pof_core_result *out) {
    if(!core||!exchange||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=SIZE_MAX;
    if(mode!=1&&mode!=2&&mode!=4&&mode!=11)return LT48_UNSUPPORTED;
    if(size!=core->total_bytes || (mode==1&&!output) ||
       ((mode==2||mode==4)&&!input))return LT48_INVALID_ARGUMENT;
    unsigned char *pending=mode==1?malloc(size):NULL;
    if(mode==1&&!pending)return LT48_NO_MEMORY;
    size_t offset=0;lt48_status status=LT48_OK;
    for(size_t i=0;i<core->unit_count && status==LT48_OK;i++) {
        lt48_pof_core_unit u;status=lt48_pof_core_get_unit(core,i,&u);
        if(status!=LT48_OK)break;
        if(u.tag==2||u.tag==8)continue;
        unsigned width,rows,columns;uint16_t row=0,col=0,mask=0xffff;
        uint32_t address=0;
        if(u.payload_size!=12){status=LT48_FORMAT_ERROR;break;}
        if(u.tag==7) {
            row=r16(u.payload);col=r16(u.payload+2);rows=r16(u.payload+4);
            columns=r16(u.payload+6);mask=r16(u.payload+8);width=1+(r16(u.payload+10)&1);
        } else if(u.tag==17 && u.value>=12) {
            address=r32(u.payload);rows=1;columns=u.value-12;
            if(columns>r32(u.payload+6))columns=r32(u.payload+6);
            width=1;
        } else {status=LT48_UNSUPPORTED;break;}
        for(unsigned r=0;r<rows && status==LT48_OK;r++)for(unsigned c=0;c<columns;c++) {
            if(offset>size || width>size-offset){status=LT48_FORMAT_ERROR;break;}
            uint16_t expected=mode==11?blank:mode==1?0:input[offset];
            if((mode==2||mode==4)&&width==2)expected|=(uint16_t)input[offset+1]<<8;
            if(u.tag==7 && mode!=11)expected&=mask;
            uint16_t actual=expected;
            status=exchange(context,mode,u.tag,(uint16_t)(row+r),(uint16_t)(col+c),
                mask,address,width,&expected,&actual);
            if(status!=LT48_OK)break;
            if(mode==11)expected=blank;
            if(u.tag==7){actual&=mask;expected&=mask;}
            else if(mode==11)actual&=0xff;
            out->exchanges++;
            if(mode!=1 && actual!=expected) {
                out->first_mismatch=offset;out->expected=expected;out->actual=actual;
                status=LT48_DEVICE_MISMATCH;break;
            }
            if(mode==1){pending[offset]=(unsigned char)actual;if(width==2)pending[offset+1]=(unsigned char)(actual>>8);}
            offset+=width;out->completed_bytes=offset;if(u.tag==17)address++;
        }
    }
    if(status==LT48_OK && offset!=size)status=LT48_FORMAT_ERROR;
    if(status==LT48_OK && pending)memcpy(output,pending,size);
    free(pending);return status;
}

/* Class-local CFUN identities, in original registration order:
 * GC: 5016,5032,5128,464; GCB: read5128A,program5128A,
 * read5192A,program5192A. These handle exceptional fuse positions, not
 * authentication or device protection. Preserve the original 16-bit effects. */
lt48_status lt48_pof_exception_internal(unsigned callback,uint16_t row,
    uint16_t column,uint16_t *data_in,uint16_t *data_out,uint16_t *mask) {
    if(!data_in||!data_out||!mask)return LT48_INVALID_ARGUMENT;
    if(callback>7)return LT48_UNSUPPORTED;
    unsigned keep=0xffff,force=0;
    if(callback<=1) {
        if(row>=0x1c && row<=0x23) {
            unsigned base=row<0x20?2+4*(row-0x1c):3+4*(row-0x20);
            if(column>=base && (column-base)%16==0 && (column-base)/16<(callback?8u:4u))
                keep=0xffu^(1u<<((column-base)/16));
        }
        if((column==0xad||column==0xbd) && (row<0x10 || (row>=0x18&&row<0x28)))keep=callback?0:0xf0;
        if(row==0xff&&column==0xfe)keep=0;
    } else if(callback==2) {
        if(row<=0xf8 && ((row&15)==7 || (row&15)==8) &&
           column==0x5e - 2*(row>>4)+((row&15)==8))keep=0;
        else if(column<0x30 || (column>=0x40&&column<0x60) || (column>=0x80&&column<0xb0)) {
            if(row==9||row==0x19||row==0x49)keep=0x99;
            else if(row==0x141)keep=4;
        }
    } else if(callback==3) {
        if(row==0x3a && (column==0x122||column==0x123||column==0x126||column==0x127))keep=0xe1;
    } else {
        const unsigned starts5128[]={0x70,0x78,0x80,0x88};
        const unsigned starts5192[]={0xe0,0xf0,0x100,0x110};
        const unsigned columns[]={2,6,3,7};
        const unsigned *starts=callback>=6?starts5192:starts5128;
        unsigned span=callback>=6?12:8,bit=0;
        for(unsigned bank=0;bank<4;bank++)
            if(row>=starts[bank] && row<starts[bank]+span && column>=columns[bank] &&
               (column-columns[bank])%8==0 && (column-columns[bank])/8<8)
                bit=1u<<((column-columns[bank])/8);
        int extra_column=column<0x40 || (column>=0x50&&column<0x84) ||
            (column>=0x88&&column<0x8c) || (column>=0x90&&column<0x94) || (column>=0x98&&column<0x9c);
        int extra_row=callback>=6?((row>=0x121&&row<0x125)||(row>=0x127&&row<0x12b)):
            (row==0x91||row==0x92||row==0x95||row==0x96);
        if(callback&1) {
            force=bit;
            if(extra_row&&extra_column)force|=callback==7?15u:11u;
        } else if(bit)keep=0xffu^bit;
        else if(extra_row&&extra_column)keep=callback==6?0xf0:0xf4;
    }
    *data_in=(uint16_t)((*data_in&keep)|force);
    *data_out|=(uint16_t)force;*mask|=(uint16_t)force;
    return LT48_OK;
}

lt48_status lt48_pof_exception_compile_internal(unsigned callback,
    const uint8_t buffers[5],lt48_compiled_wave **out) {
    if(!buffers||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;if(callback>7)return LT48_UNSUPPORTED;
    uint16_t words[]={9,0xf002,(uint16_t)callback,0,0,0,0,0,0x52,1};
    for(unsigned i=0;i<5;i++) {
        for(unsigned j=0;j<i;j++)if(buffers[i]==buffers[j])return LT48_INVALID_ARGUMENT;
        words[3+i]=buffers[i];
    }
    return lt48_compiled_host_words_internal(words,10,out);
}

lt48_status lt48_pof_exception_bind_internal(const lt48_rez *rez,size_t wave_index,
    lt48_compiled_wave **out) {
    static const char *digests[]={
        "fc6529b7b6c6443dc9a962052b5f18e7f2928ae1bdfc3248d373a9a9fec32c20",
        "9bd0f6c9cd3e5e6888a23f8750c6acc6da7fafb64501f477e064db3c8bb74b12"};
    static const char *callbacks[]={"_CFUN_ExcCheck5016","_CFUN_ExcCheck5032",
        "_CFUN_ExcCheck5128","_CFUN_ExcCheck464","_CFUN_ExcCheck5128A",
        "_CFUN_ProgExcCheck5128A","_CFUN_ExcCheck5192A","_CFUN_ProgExcCheck5192A"};
    /* Original GC/GCB bind A0A380 to Col and A0A388 to Row.
     * Exception leaf argument order follows those globals. */
    static const char *buffers[]={"_BUF_Col","_BUF_Row","_BUF_DataIn","_BUF_DataOut","_BUF_Mask"};
    if(!rez||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;unsigned char digest[32];char hex[65];
    lt48_status status=lt48_rez_sha256(rez,digest);if(status!=LT48_OK)return status;
    static const char digits[]="0123456789abcdef";
    for(unsigned i=0;i<32;i++){hex[2*i]=digits[digest[i]>>4];hex[2*i+1]=digits[digest[i]&15];}hex[64]=0;
    unsigned group=!strcmp(hex,digests[0])?0:!strcmp(hex,digests[1])?1:2;
    if(group==2)return LT48_SAFETY_REJECTED;
    lt48_wave_code wave;status=lt48_rez_get_wave_code(rez,wave_index,&wave);if(status!=LT48_OK)return status;
    if(wave.wave_type!=3)return LT48_UNSUPPORTED;
    if(wave.data_size!=4||r16(wave.data)!=2)return LT48_FORMAT_ERROR;
    unsigned identifier=r16(wave.data+2),selected=8,found=0;
    uint8_t ids[5]={0};unsigned seen[5]={0};size_t index;
    status=lt48_rez_find_resource(rez,"DefineName",&index);if(status!=LT48_OK)return status;
    lt48_defined_name_collection_info info;
    status=lt48_rez_get_defined_name_collection_info(rez,index,&info);if(status!=LT48_OK)return status;
    for(size_t i=0;i<info.count;i++) {
        lt48_defined_name name;status=lt48_rez_get_defined_name(rez,index,i,&name);if(status!=LT48_OK)return status;
        for(unsigned k=4*group;k<4*group+4;k++)
            if(name.identifier==identifier && name.name_length==strlen(callbacks[k]) && !memcmp(name.name,callbacks[k],name.name_length)){selected=k;found++;}
        for(unsigned k=0;k<5;k++)
            if(name.name_length==strlen(buffers[k])&&!memcmp(name.name,buffers[k],name.name_length)) {
                if(name.identifier>255)return LT48_FORMAT_ERROR;
                ids[k]=(uint8_t)name.identifier;seen[k]++;
            }
    }
    if(found!=1)return LT48_UNSUPPORTED;
    for(unsigned k=0;k<5;k++)if(seen[k]!=1)return LT48_FORMAT_ERROR;
    return lt48_pof_exception_compile_internal(selected,ids,out);
}

/* 00461c10 / 00461e10: POF image bit -> transformed buffer coordinate.
 * Tables belong to the imported source, not to the operator binary. */
lt48_status lt48_pof_trs_coordinate_internal(unsigned kind,uint16_t address,
    uint16_t position,unsigned bit,const unsigned char *const tables[6],
    const size_t sizes[6],uint32_t out[3]) {
    if(!tables||!sizes||!out)return LT48_INVALID_ARGUMENT;
    if(kind>1||bit>=(kind?12u:8u))return LT48_UNSUPPORTED;
    uint32_t column,row,mapped,bank=kind?bit%6:bit&3,code;
#define TABLE(dst,table,offset) do { \
    size_t off_=(offset); \
    if(!tables[table]||off_>sizes[table]||sizes[table]-off_<4) return LT48_FORMAT_ERROR; \
    dst=r32(tables[table]+off_); \
} while(0)
    if(address<0x142) {
        TABLE(code,0,((address>>4)|(bank<<4))*4u);
        uint32_t base;
        TABLE(base,1,(address&15u)*4u);
        column=(base<<(kind?5:4))|(kind?(code*2u)&16u:code&8u)|bit;
        TABLE(row,2,(bank*0xb0u+position)*4u);
        TABLE(mapped,3,(size_t)((bank<<4)|code)*4u);
        /* Original 461d00 initializes all 288 correction cells to one. */
        if(kind&&position>0x7f)row^=1u;
    } else if(address==0x142) {
        column=kind?0x200:0x100;row=bit|0x60u;mapped=0;
    } else {
        size_t offset=((address/2u)&0x18u)|(address&4u);
        if(kind) {
            if(position<0x100)return LT48_FORMAT_ERROR;
            column=((((address/2u)&0x18u)|(address&7u))%6)*16u|(11u-bit)|0x140u;
            TABLE(row,4,(position-0x100u)*4u);
        } else {
            column=((address&3u)<<3)|(7u-bit)|0xa0u;
            TABLE(row,4,(position&0xffu)*4u);
        }
        /* This table is indexed by BYTE offset in the original x86. */
        TABLE(mapped,5,offset);
    }
#undef TABLE
    if(mapped>7)return LT48_FORMAT_ERROR;
    out[0]=column;out[1]=row;out[2]=mapped;return LT48_OK;
}

struct lt48_pof_trs_source {
    unsigned char *image;
    size_t image_size;
    const unsigned char *tables[2][6];
    size_t sizes[2][6];
    const unsigned char *fpga[2];
};

/* Return file-backed bytes only. Virtual padding is not source table data. */
static const unsigned char *pof_pe_bytes(const unsigned char *data,size_t size,
    uint32_t address,size_t length) {
    if(size<64||memcmp(data,"MZ",2))return NULL;
    size_t pe=r32(data+60);
    if(pe>size||size-pe<24||memcmp(data+pe,"PE\0\0",4))return NULL;
    unsigned sections=r16(data+pe+6),optional=r16(data+pe+20);
    size_t opt=pe+24;
    if(optional<32||optional>size-opt||r16(data+opt)!=0x10b)return NULL;
    uint32_t base=r32(data+opt+28);
    if(address<base)return NULL;
    uint32_t rva=address-base;
    size_t table=opt+optional;
    if(sections>(size-table)/40)return NULL;
    const unsigned char *result=NULL;
    for(unsigned i=0;i<sections;i++) {
        const unsigned char *section=data+table+i*40;
        uint32_t start=r32(section+12),raw_size=r32(section+16),raw=r32(section+20);
        if(rva<start)continue;
        size_t delta=(size_t)rva-start;
        if(delta>raw_size||length>raw_size-delta)continue;
        if(raw>size||delta>size-raw||length>size-raw-delta)return NULL;
        if(result)return NULL; /* Ambiguous overlapping sections. */
        result=data+raw+delta;
    }
    return result;
}

void lt48_pof_trs_source_close_internal(lt48_pof_trs_source *source) {
    if(source){free(source->image);free(source);}
}
lt48_status lt48_pof_source_fpga_internal(const lt48_pof_trs_source *source,
    unsigned selector,const unsigned char **data,size_t *size) {
    if(!source||!data||!size)return LT48_INVALID_ARGUMENT;
    *data=NULL;*size=0;
    if(selector>1)return LT48_UNSUPPORTED;
    if(!source->fpga[selector])return LT48_FORMAT_ERROR;
    *data=source->fpga[selector];*size=0xada;return LT48_OK;
}
lt48_status lt48_pof_trs_source_parse_internal(const unsigned char *data,
    size_t size,lt48_pof_trs_source **out) {
    static const unsigned char expected[32]={
        0xf2,0xd4,0x9c,0x20,0x75,0x7e,0xb9,0x97,0x48,0xe9,0x1a,0x49,0x7f,0x58,0x49,0xe2,
        0x6b,0xf1,0x41,0x67,0x72,0x6f,0x77,0xa9,0xf3,0x13,0xa7,0x92,0x44,0x70,0x1c,0x12};
    static const size_t lengths[2][6]={{64,16,704,64,240,24},{96,16,1056,96,368,24}};
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!data)return LT48_INVALID_ARGUMENT;
    if(size==16668u) {
        /* Exact derived bundle, independently extracted from the verified PE.
         * No arbitrary tables or executable callbacks may enter this path. */
        static const unsigned char bundle_digest[32]={
            0x01,0x61,0x82,0x96,0x22,0xb2,0x79,0xe9,0x82,0xe2,0x9f,0x8b,0xbe,0x29,0xcf,0x60,
            0x3c,0xcf,0x40,0x33,0x3d,0xb7,0x1b,0x68,0x18,0xa7,0xb0,0x9f,0x0c,0xc5,0xc2,0xb6};
        unsigned char digest[32];lt48_sha256(data,size,digest);
        if(memcmp(digest,bundle_digest,32)||memcmp(data,"LT48POF1",8)||memcmp(data+8,expected,32))return LT48_SAFETY_REJECTED;
        lt48_pof_trs_source *source=calloc(1,sizeof(*source));
        if(!source)return LT48_NO_MEMORY;
        source->image=malloc(size);if(!source->image){free(source);return LT48_NO_MEMORY;}
        memcpy(source->image,data,size);source->image_size=size;size_t at=40;
        for(unsigned k=0;k<2;k++)for(unsigned i=0;i<6;i++){
            source->sizes[k][i]=4*lengths[k][i];source->tables[k][i]=source->image+at;at+=source->sizes[k][i];
        }
        for(unsigned k=0;k<2;k++){source->fpga[k]=source->image+at;at+=0xada;}
        *out=source;return LT48_OK;
    }
    if(size!=6874112u)return LT48_SAFETY_REJECTED;
    unsigned char digest[32];lt48_sha256(data,size,digest);
    if(memcmp(digest,expected,32))return LT48_SAFETY_REJECTED;
    lt48_pof_trs_source *source=calloc(1,sizeof(*source));
    if(!source)return LT48_NO_MEMORY;
    source->image=malloc(size);
    if(!source->image){free(source);return LT48_NO_MEMORY;}
    memcpy(source->image,data,size);source->image_size=size;
    for(unsigned kind=0;kind<2;kind++) {
        const unsigned char *slot=pof_pe_bytes(source->image,size,0x769e98+4*kind,4);
        const unsigned char *object=slot?pof_pe_bytes(source->image,size,r32(slot),8):NULL;
        if(!object||r32(object+4)!=(kind?0x461e10u:0x461c10u))goto bad;
        const unsigned char *tables=pof_pe_bytes(source->image,size,r32(object),24);
        if(!tables)goto bad;
        for(unsigned i=0;i<6;i++) {
            source->sizes[kind][i]=4*lengths[kind][i];
            source->tables[kind][i]=pof_pe_bytes(source->image,size,r32(tables+4*i),source->sizes[kind][i]);
            if(!source->tables[kind][i])goto bad;
        }
    }
    for(unsigned i=0;i<2;i++) {
        const unsigned char *slot=pof_pe_bytes(source->image,size,0x76caf8u+4*i,4);
        source->fpga[i]=slot?pof_pe_bytes(source->image,size,r32(slot),0xada):NULL;
        if(!source->fpga[i])goto bad;
    }
    *out=source;return LT48_OK;
bad:
    lt48_pof_trs_source_close_internal(source);return LT48_FORMAT_ERROR;
}
/* Private cache representation: magic, source SHA256, twelve tables in
 * kind/index order, then both base FPGA images. No absolute PE addresses. */
lt48_status lt48_pof_source_bundle_internal(const lt48_pof_trs_source *source,
    unsigned char **out,size_t *size) {
    if(!source||!out||!size)return LT48_INVALID_ARGUMENT;
    *out=NULL;*size=0;unsigned char *data=malloc(16668);if(!data)return LT48_NO_MEMORY;
    memcpy(data,"LT48POF1",8);
    if(source->image_size==16668)memcpy(data+8,source->image+8,32);
    else lt48_sha256(source->image,source->image_size,data+8);
    size_t at=40;
    for(unsigned k=0;k<2;k++)for(unsigned i=0;i<6;i++){
        size_t n=source->sizes[k][i];
        if(n>16668-at){free(data);return LT48_FORMAT_ERROR;}
        memcpy(data+at,source->tables[k][i],n);at+=n;
    }
    for(unsigned k=0;k<2;k++){
        if(!source->fpga[k]||0xada>16668-at){free(data);return LT48_FORMAT_ERROR;}
        memcpy(data+at,source->fpga[k],0xada);at+=0xada;
    }
    if(at!=16668){free(data);return LT48_FORMAT_ERROR;}
    *out=data;*size=at;return LT48_OK;
}
lt48_status lt48_pof_trs_source_open_internal(const char *root,lt48_pof_trs_source **out) {
    static const char *names[]={"pof-runtime-13.10.bin","WDATA48UXP.exe","WDATA48UXP.EXE"};
    if(!root||!*root||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    for(unsigned i=0;i<3;i++) {
        size_t n=strlen(root)+strlen(names[i])+2;
        char *path=malloc(n);if(!path)return LT48_NO_MEMORY;
        snprintf(path,n,"%s/%s",root,names[i]);FILE *f=fopen(path,"rb");free(path);
        if(!f){if(errno==ENOENT)continue;return LT48_FILE_ERROR;}
        size_t size=i?6874112u:16668u;
        unsigned char *data=malloc(size);if(!data){fclose(f);return LT48_NO_MEMORY;}
        size_t got=fread(data,1,size,f);int trailing=fgetc(f),error=ferror(f);
        if(fclose(f)!=0)error=1;
        lt48_status status=error?LT48_FILE_ERROR:got!=size||trailing!=EOF?LT48_SAFETY_REJECTED:
            lt48_pof_trs_source_parse_internal(data,size,out);
        free(data);return status;
    }
    return LT48_NOT_FOUND;
}
lt48_status lt48_pof_trs_source_coordinate_internal(const lt48_pof_trs_source *source,
    unsigned kind,uint16_t address,uint16_t position,unsigned bit,uint32_t out[3]) {
    if(!source)return LT48_INVALID_ARGUMENT;
    if(kind>1)return LT48_UNSUPPORTED;
    return lt48_pof_trs_coordinate_internal(kind,address,position,bit,
        source->tables[kind],source->sizes[kind],out);
}

/* TTrsPOFBuffer uses column-major cells: control, valid, mask, data.
 * All mapped bits in the two audited sources fit one byte per cell. */
struct lt48_pof_trs_image {
    unsigned char *cells;
    size_t size;
    uint32_t columns,rows;
};
void lt48_pof_trs_image_close_internal(lt48_pof_trs_image *image) {
    if(image){free(image->cells);free(image);}
}
const unsigned char *lt48_pof_trs_image_cells_internal(const lt48_pof_trs_image *image,
    size_t *size,uint32_t *columns,uint32_t *rows) {
    if(!image)return NULL;
    if(size)*size=image->size;
    if(columns)*columns=image->columns;
    if(rows)*rows=image->rows;
    return image->cells;
}
#include "../../NewDeviceLib/sources/pof/trs_read_reconstruction.inc"
lt48_status lt48_pof_trs_image_create_internal(const lt48_pof_trs_source *source,
    unsigned kind,const lt48_pof_core *core,const unsigned char *input,size_t size,
    lt48_pof_trs_image **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!source||!core||!input||size!=core->total_bytes)return LT48_INVALID_ARGUMENT;
    lt48_pof_trs_image *image=calloc(1,sizeof(*image));if(!image)return LT48_NO_MEMORY;
    lt48_status status=pof_trs_walk(source,kind,core,image,input,NULL,0,NULL);
    if(status!=LT48_OK)goto fail;
    if(!image->rows||!image->columns||image->columns>SIZE_MAX/4/image->rows){status=LT48_FORMAT_ERROR;goto fail;}
    image->size=(size_t)image->columns*image->rows*4;
    image->cells=calloc(1,image->size);if(!image->cells){status=LT48_NO_MEMORY;goto fail;}
    status=pof_trs_walk(source,kind,core,image,input,NULL,1,NULL);
    if(status!=LT48_OK)goto fail;
    *out=image;return LT48_OK;
fail:lt48_pof_trs_image_close_internal(image);return status;
}
lt48_status lt48_pof_trs_image_rebuild_internal(const lt48_pof_trs_source *source,
    unsigned kind,const lt48_pof_core *core,lt48_pof_trs_image *image,
    unsigned char *output,size_t size) {
    if(!source||!core||!image||!output||size!=core->total_bytes)return LT48_INVALID_ARGUMENT;
    unsigned char *pending=malloc(size);if(!pending)return LT48_NO_MEMORY;
    /* The original converter leaves bytes beyond its write spans unchanged. */
    memcpy(pending,output,size);
    lt48_status status=pof_trs_walk(source,kind,core,image,NULL,pending,2,NULL);
    if(status==LT48_OK)memcpy(output,pending,size);
    free(pending);return status;
}

/* 004629a0: one-byte transformed-cell worker. Family PROGRAM already owns
 * its pulse retries; this loop must not add another retry around the wave. */
lt48_status lt48_pof_trs_image_run_internal(lt48_pof_trs_image *image,unsigned mode,
    unsigned char blank,lt48_pof_core_exchange exchange,void *context,
    lt48_pof_core_result *out) {
    if(!image||!exchange||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=SIZE_MAX;
    if(mode!=1&&mode!=2&&mode!=4&&mode!=11)return LT48_UNSUPPORTED;
    unsigned char *pending=NULL;
    if(mode==1){pending=malloc(image->size);if(!pending)return LT48_NO_MEMORY;memcpy(pending,image->cells,image->size);}
    lt48_status status=LT48_OK;
    for(uint32_t column=0;column<image->columns&&status==LT48_OK;column++) {
        for(uint32_t row=0;row<image->rows;row++) {
            size_t offset=((size_t)column*image->rows+row)*4;
            const unsigned char *cell=image->cells+offset;
            if(!cell[1])continue;
            uint16_t expected=cell[3],actual=mode==2?expected:0;
            status=exchange(context,mode,7,(uint16_t)column,(uint16_t)row,
                cell[2],(uint32_t)offset,1,&expected,&actual);
            if(status!=LT48_OK)break;
            out->exchanges++;
            actual&=cell[2];expected=(mode==11?blank:expected)&cell[2];
            if(mode!=1&&actual!=expected) {
                out->first_mismatch=offset;out->expected=expected;out->actual=actual;
                status=LT48_DEVICE_MISMATCH;break;
            }
            if(pending)pending[offset+3]=(unsigned char)actual;
            out->completed_bytes++;
        }
    }
    if(status==LT48_OK&&pending)memcpy(image->cells,pending,image->size);
    free(pending);return status;
}

/* Byte images are native POF core payloads, not one-byte-per-fuse JEDEC.
 * Class 2028 must transform the selected image before visiting socket cells
 * and reconstruct it after Read. Keep all read publication atomic. */
lt48_status lt48_pof_image_run_internal(const lt48_pal_resources *resources,
    const lt48_pof_trs_source *source,unsigned mode,const unsigned char *input,
    size_t size,unsigned char *output,lt48_pof_core_exchange exchange,void *context,
    lt48_pof_core_result *result) {
    if(!resources||!exchange||!result)return LT48_INVALID_ARGUMENT;
    memset(result,0,sizeof(*result));result->first_mismatch=SIZE_MAX;
    int kind=lt48_pal_resources_pof_internal(resources);
    const lt48_pof_core *core=lt48_pal_resources_pof_core_internal(resources);
    if(!kind||!core)return LT48_UNSUPPORTED;
    if(mode!=1&&mode!=2&&mode!=4&&mode!=11)return LT48_UNSUPPORTED;
    if(size!=core->total_bytes||(mode==1&&!output)||
       ((mode==2||mode==4)&&!input))return LT48_INVALID_ARGUMENT;
    unsigned char blank=lt48_pal_resources_blank_fuse(resources);
    if(kind==1)return lt48_pof_core_run_internal(core,mode,blank,input,size,
        output,exchange,context,result);
    if(!source)return LT48_INVALID_ARGUMENT;
    uint16_t id;uint32_t selector;
    lt48_status status=lt48_pal_resources_parameter_internal(resources,"_VAR_POFTrsIndex",&id,&selector);
    if(status!=LT48_OK)return status;
    if(selector>1)return LT48_UNSUPPORTED;
    unsigned char *pending=NULL;
    if(mode==1||mode==11) {
        pending=malloc(size);if(!pending)return LT48_NO_MEMORY;
        memset(pending,blank,size);
    }
    lt48_pof_trs_image *image=NULL;
    status=lt48_pof_trs_image_create_internal(source,selector,core,pending?pending:input,size,&image);
    if(status==LT48_OK)status=lt48_pof_trs_image_run_internal(image,mode,blank,exchange,context,result);
    if(status==LT48_OK&&mode==1) {
        status=pof_trs_read_reconstruct(source,selector,core,image,pending,size,result);
    }
    if(status==LT48_OK&&mode==1)memcpy(output,pending,size);
    lt48_pof_trs_image_close_internal(image);free(pending);return status;
}

#include "pof_file.inc"
