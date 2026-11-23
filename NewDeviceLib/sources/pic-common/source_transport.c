#include "source_transport.h"
#include "source_verify_voltage.h"
#include "source_contact_capture.h"
#include "source_applet_finish_sequence.h"
#include "../../../c/src/internal.h"
#include <stdlib.h>
#include <string.h>

struct lt48_pic_source_transport {
    lt48_protocol_transport api;
    void *context;
    lt48_pic_source_session *session;
    lt48_runtime_transport_session *wire;
    lt48_pic_source_host host;
    uint16_t status;
    int prepared,forward_failed,deferred_fpga;
    lt48_status last_cleanup;
    unsigned char *fpga;size_t fpga_size;
    unsigned char *idle;size_t idle_size;
    lt48_pic_source_startup_plan startup;uint32_t startup_flags;int startup_bound;
};
static lt48_status source_wire_write(void *context,const unsigned char *bytes,size_t count) {
    lt48_pic_source_transport *owner=context;
    if(owner->forward_failed)return LT48_SAFETY_REJECTED;
    owner->status=0;
    lt48_status st=lt48_runtime_transport_session_controller_write(owner->wire,bytes,count);
    if(st!=LT48_OK)owner->status=1;
    return st;
}
static lt48_status source_wire_status(void *context,int force,uint16_t *status) {
    lt48_pic_source_transport *owner=context;
    if(owner->forward_failed){*status=1;return LT48_SAFETY_REJECTED;}
    if(!force && owner->status){*status=owner->status;return LT48_OK;}
    lt48_status st=lt48_runtime_transport_session_controller_status_internal(owner->wire,1,&owner->status);
    *status=owner->status;return st;
}
static lt48_status source_wire_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **result) {
    lt48_pic_source_transport *owner=context;
    if(owner->forward_failed)return LT48_SAFETY_REJECTED;
    const lt48_applet_address *applets;size_t count;
    lt48_status st=lt48_pic_source_session_applets_internal(owner->session,&applets,&count);
    if(st==LT48_OK)st=lt48_runtime_transport_session_run_with_applets(owner->wire,machine,applets,count,result);
    owner->status=st==LT48_OK?0:1;
    return st;
}
static int source_wire_cancelled(void *context) {
    lt48_pic_source_transport *owner=context;
    return owner->api.cancelled && owner->api.cancelled(owner->context);
}
static lt48_status source_wire_cleanup_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **result) {
    lt48_pic_source_transport *owner=context;
    /* A fresh uncancelled queue is required even if forward transport latched
     * a failure. Source POWEROFF runs before the independent global actions. */
    lt48_runtime_transport_session_close(owner->wire);owner->wire=NULL;
    lt48_protocol_transport clean=owner->api;clean.cancelled=NULL;
    lt48_status st=lt48_runtime_transport_session_create(&clean,owner->context,&owner->wire);
    const lt48_applet_address *applets;size_t count;
    if(st==LT48_OK)st=lt48_pic_source_session_applets_internal(owner->session,&applets,&count);
    if(st==LT48_OK)st=lt48_runtime_transport_session_run_with_applets(owner->wire,machine,applets,count,result);
    return st;
}
static lt48_status source_wire_finish(void *context,lt48_status operation,lt48_status source_cleanup) {
    lt48_pic_source_transport *owner=context;lt48_status cleanup=LT48_OK;int completed=0;
    lt48_status outcome=operation==LT48_OK?source_cleanup:operation;
    (void)lt48_owner_finish_fpga_internal(&owner->api,owner->context,&owner->wire,NULL,NULL,NULL,0,owner->idle,owner->idle_size,
        outcome,&cleanup,&completed);
    owner->prepared=0;owner->status=0;owner->forward_failed=0;owner->startup_bound=0;
    owner->last_cleanup=cleanup;
    return cleanup;
}
lt48_status lt48_pic_source_transport_cleanup_status_internal(const lt48_pic_source_transport *owner) {
    return owner?owner->last_cleanup:LT48_INVALID_ARGUMENT;
}
lt48_status lt48_pic_source_transport_create_internal(const lt48_protocol_transport *api,void *context,
    lt48_pic_source_session *session,const lt48_pic_source_host_binding *binding,
    uint32_t voltage,uint32_t minimum,uint32_t maximum,lt48_pic_source_transport **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!api || !api->write || !api->read || !session || !binding ||
       binding->poweron_role==LT48_PIC_ABSENT_ROLE || binding->poweroff_role==LT48_PIC_ABSENT_ROLE ||
       !minimum || minimum>maximum || voltage<minimum || voltage>maximum || voltage>UINT16_MAX)
        return LT48_INVALID_ARGUMENT;
    lt48_pic_source_transport *owner=calloc(1,sizeof(*owner));if(!owner)return LT48_NO_MEMORY;
    owner->api=*api;owner->context=context;owner->session=session;
    owner->host=(lt48_pic_source_host){binding->voltage_variable,binding->poweron_role,binding->poweroff_role,
        voltage,minimum,maximum,source_wire_write,source_wire_status,source_wire_runner,owner,source_wire_cleanup_runner};
    *out=owner;return LT48_OK;
}
void lt48_pic_source_transport_close_internal(lt48_pic_source_transport *owner) {
    if(!owner)return;
    if(owner->prepared)(void)source_wire_finish(owner,LT48_CANCELLED,LT48_OK);
    lt48_runtime_transport_session_close(owner->wire);free(owner->fpga);free(owner->idle);free(owner);
}
lt48_status lt48_pic_source_transport_idle_internal(lt48_pic_source_transport *owner,
    const unsigned char *bytes,size_t count) {
    if(!owner||!bytes||!count||count>65536)return LT48_INVALID_ARGUMENT;
    if(owner->prepared)return LT48_SAFETY_REJECTED;
    unsigned char *copy=malloc(count);if(!copy)return LT48_NO_MEMORY;
    memcpy(copy,bytes,count);free(owner->idle);owner->idle=copy;owner->idle_size=count;
    return LT48_OK;
}
static lt48_status source_prepare(lt48_pic_source_transport *owner,
    const unsigned char *fpga,size_t count,const lt48_pic_source_applet_payload *payloads,size_t payload_count,int deferred) {
    if(!owner || !fpga || !count)return LT48_INVALID_ARGUMENT;
    if(owner->prepared)return LT48_SAFETY_REJECTED;
    if(owner->startup_bound&&!deferred)return LT48_UNSUPPORTED;
    const lt48_applet_address *bindings;size_t binding_count;
    lt48_status st=lt48_pic_source_session_applets_internal(owner->session,&bindings,&binding_count);
    if(st!=LT48_OK)return st;
    if(payload_count!=binding_count || payload_count>256 || (payload_count&&!payloads))return LT48_SAFETY_REJECTED;
    for(size_t i=0;i<payload_count;i++) {
        const lt48_pic_source_applet_payload *p=payloads+i;
        size_t binding=0;
        while(binding<binding_count&&bindings[binding].wave!=p->role)binding++;
        if(binding==binding_count || p->address!=bindings[binding].address || !p->data ||
           !p->size || p->size>65536u-(size_t)p->address)return LT48_SAFETY_REJECTED;
        for(size_t j=0;j<i;j++) {
            const lt48_pic_source_applet_payload *q=payloads+j;
            if(q->role==p->role)return LT48_SAFETY_REJECTED;
            size_t first=p->address>q->address?p->address:q->address;
            size_t p_end=(size_t)p->address+p->size,q_end=(size_t)q->address+q->size;
            size_t last=p_end<q_end?p_end:q_end;
            if(first<last && memcmp(p->data+first-p->address,q->data+first-q->address,last-first))
                return LT48_SAFETY_REJECTED;
        }
    }
    unsigned char *copy=malloc(count);if(!copy)return LT48_NO_MEMORY;
    memcpy(copy,fpga,count);free(owner->fpga);owner->fpga=copy;owner->fpga_size=count;
    owner->last_cleanup=LT48_OK;
    lt48_runtime_transport_session_close(owner->wire);owner->wire=NULL;owner->status=0;owner->forward_failed=0;
    st=lt48_runtime_transport_session_create(&owner->api,owner->context,&owner->wire);
    owner->deferred_fpga=deferred;
    if(st==LT48_OK && !deferred)st=lt48_pic_source_transport_load_internal(owner->wire,fpga,count,payloads,payload_count,15);
    if(st==LT48_OK && deferred) {
        for(size_t i=0;st==LT48_OK&&i<payload_count;i++) {
            st=lt48_runtime_transport_session_wave_watchdog(owner->wire,10000,1);
            if(st==LT48_OK)st=lt48_owner_load_applet_internal(owner->wire,payloads[i].address,payloads[i].data,payloads[i].size);
        }
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(owner->wire);
    }
    if(st==LT48_OK)owner->prepared=1;
    else (void)source_wire_finish(owner,st,LT48_OK);
    return st;
}
lt48_status lt48_pic_source_transport_prepare_applets_internal(lt48_pic_source_transport *owner,
    const unsigned char *fpga,size_t count,const lt48_pic_source_applet_payload *payloads,size_t payload_count) {
    return source_prepare(owner,fpga,count,payloads,payload_count,0);
}
lt48_status lt48_pic_source_transport_prepare_startup_internal(lt48_pic_source_transport *owner,
    const unsigned char *fpga,size_t count,const lt48_pic_source_applet_payload *payloads,size_t payload_count) {
    return source_prepare(owner,fpga,count,payloads,payload_count,1);
}
lt48_status lt48_pic_source_transport_prepare_internal(lt48_pic_source_transport *owner,
    const unsigned char *fpga,size_t count) {
    return lt48_pic_source_transport_prepare_applets_internal(owner,fpga,count,NULL,0);
}
static lt48_status source_contact_read(void *context,unsigned char *raw,size_t count) {
    lt48_pic_source_transport *owner=context;
    return lt48_runtime_transport_session_controller_capture_internal(owner->wire,raw,(unsigned)count);
}
static lt48_status source_contact_timeout(void *context,unsigned milliseconds) {
    lt48_pic_source_transport *owner=context;
    return lt48_runtime_transport_session_wave_watchdog(owner->wire,milliseconds,0);
}
static lt48_status source_contact_restore(void *context) {
    const unsigned char value=0;return source_wire_write(context,&value,1);
}
static lt48_status source_contact_capture(lt48_pic_source_transport *owner,
    const lt48_pic_contact_masks *m,const unsigned char *payload,size_t size,lt48_pic_contact_result *result,int *matched) {
    if(!owner||!owner->prepared||!owner->fpga||!m||!result||!matched)return LT48_INVALID_ARGUMENT;
    if(m->alternate&&(!payload||size!=476))return LT48_INVALID_ARGUMENT;
    if(m->alternate) {
        const unsigned char expected[32]={0x6c,0x29,0x8f,0x40,0x20,0x55,0xea,0x4f,
            0xd0,0xc9,0x0a,0x72,0x89,0x92,0xa0,0x90,0x62,0xbb,0xc0,0x59,0x64,0xd4,0x7b,0x3e,
            0x9e,0x46,0x3d,0xa9,0x73,0x2a,0xf7,0x88};unsigned char actual[32];
        lt48_sha256(payload,size,actual);if(memcmp(actual,expected,32))return LT48_SAFETY_REJECTED;
    }
    unsigned char raw[12];lt48_pic_contact_io io={source_wire_write,source_contact_read,
        source_contact_timeout,source_contact_restore,owner};
    lt48_status status=lt48_pic_source_contact_capture(&io,m->alternate,0xb000,payload,size,raw);
    if(status==LT48_OK)*matched=lt48_pic_source_contact_match(m,raw,result);
    else owner->forward_failed=1;
    return status;
}
static lt48_status source_reload_fpga(lt48_pic_source_transport *owner) {
    const unsigned char head[]={0x38,0xaa,0x55};
    lt48_status st=source_wire_write(owner,head,sizeof(head));
    if(st==LT48_OK)st=source_wire_write(owner,owner->fpga,owner->fpga_size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(owner->wire);
    return st;
}
lt48_status lt48_pic_source_transport_contact_internal(lt48_pic_source_transport *owner,
    const lt48_pic_contact_masks *m,const unsigned char *payload,size_t size,lt48_pic_contact_result *result,int *matched) {
    if(!result||!matched)return LT48_INVALID_ARGUMENT;
    lt48_pic_contact_result staged;int accepted;
    lt48_status st=source_contact_capture(owner,m,payload,size,&staged,&accepted);
    if(st==LT48_OK)st=source_reload_fpga(owner);
    if(st==LT48_OK){*result=staged;*matched=accepted;}
    else if(owner)owner->forward_failed=1;
    return st;
}
lt48_status lt48_pic_source_transport_require_contact_internal(lt48_pic_source_transport *owner,
    const lt48_pic_contact_masks *m,lt48_pic_contact_result *result) {
    if(!owner||!owner->prepared||!owner->deferred_fpga||owner->startup_bound||!m||!result||m->alternate)
        return LT48_INVALID_ARGUMENT;
    lt48_pic_contact_result staged={0};int matched=0;
    lt48_status st=source_contact_capture(owner,m,NULL,0,&staged,&matched);
    if(st==LT48_OK){*result=staged;if(!matched)st=LT48_DEVICE_MISMATCH;}
    if(st==LT48_OK)st=source_reload_fpga(owner);
    if(st==LT48_OK)owner->deferred_fpga=0;
    else (void)source_wire_finish(owner,st,LT48_OK);
    return st;
}
static lt48_status source_stream_transfer(const lt48_protocol_transport *api,void *context,
    int input,uint8_t endpoint,unsigned char *bytes,size_t count) {
    if(api->cancelled&&api->cancelled(context))return LT48_CANCELLED;
    size_t transferred=0;
    lt48_status status=input?api->read(context,endpoint,bytes,count,&transferred):
        api->write(context,endpoint,bytes,count,&transferred);
    return status==LT48_OK&&transferred!=count?LT48_PROTOCOL_ERROR:status;
}
uint32_t lt48_pic_source_stream_chunk_internal(uint32_t bytes,uint16_t width) {
    if(!bytes||!width||width>INT16_MAX||bytes>INT32_MAX||bytes%width)return 0;
    uint32_t chunk=bytes/100;
    if(chunk>8192)return 8192;
    chunk-=chunk%width;return chunk?chunk:width;
}
static lt48_status source_stream_block(lt48_runtime_transport_session *wire,
    const lt48_protocol_transport *api,void *context,lt48_pic_source_stream_protocol protocol,
    int writing,unsigned char *data,size_t count,int final) {
    if(!wire||!api||!api->read||!api->write||!data||!count||(writing!=0&&writing!=1))
        return LT48_INVALID_ARGUMENT;
    if(protocol!=LT48_PIC_STREAM_BYTES_48)return LT48_UNSUPPORTED;
    /* Both quotient and remainder in the source command are 16-bit; the
     * quotient is counted in 512-byte USB packets. Never truncate a count. */
    if(count/512u>UINT16_MAX)return LT48_INVALID_ARGUMENT;
    if(api->cancelled&&api->cancelled(context))return LT48_CANCELLED;
    unsigned char command[512]={0},reply[512];
    unsigned char *staged=writing?NULL:malloc(count);
    if(!writing&&!staged)return LT48_NO_MEMORY;
    command[0]=writing?0x48:0x43;
    command[1]=(unsigned char)(count/512);command[2]=(unsigned char)(count/512>>8);
    command[3]=(unsigned char)(count%512);command[4]=(unsigned char)(count%512>>8);
    command[5]=writing?1:0;
    lt48_status status=lt48_runtime_transport_session_flush(wire);
    if(status==LT48_OK)status=source_stream_transfer(api,context,0,2,command,sizeof(command));
    if(status==LT48_OK)status=source_stream_transfer(api,context,!writing,writing?4:0x88,
        writing?data:staged,count);
    if(status==LT48_OK&&writing) {
        status=source_stream_transfer(api,context,1,0x88,reply,sizeof(reply));
        if(status==LT48_OK&&!reply[0])status=LT48_PROTOCOL_ERROR;
    }
    if(status==LT48_OK&&writing&&final) {
        memset(command,0,sizeof(command));command[0]=0x0c;command[1]=1;
        status=lt48_runtime_transport_session_flush(wire);
        if(status==LT48_OK)status=source_stream_transfer(api,context,0,2,command,sizeof(command));
        if(status==LT48_OK)status=source_stream_transfer(api,context,1,0x88,reply,sizeof(reply));
        if(status==LT48_OK&&(!(reply[0]|reply[1])||!(reply[2]&1)))status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK&&!writing) {
        if(api->cancelled&&api->cancelled(context))status=LT48_CANCELLED;
        else memcpy(data,staged,count);
    }
    free(staged);return status;
}
lt48_status lt48_pic_source_stream_internal(lt48_runtime_transport_session *wire,
    const lt48_protocol_transport *api,void *context,lt48_pic_source_stream_protocol protocol,
    int writing,unsigned char *data,size_t count) {
    return source_stream_block(wire,api,context,protocol,writing,data,count,1);
}
lt48_status lt48_pic_source_transport_stream_internal(lt48_pic_source_transport *owner,
    lt48_pic_source_stream_protocol protocol,int writing,unsigned char *data,size_t count) {
    if(!owner||!owner->prepared)return LT48_INVALID_ARGUMENT;
    if(owner->deferred_fpga||owner->status||owner->forward_failed)return LT48_SAFETY_REJECTED;
    lt48_status status=lt48_pic_source_stream_internal(owner->wire,&owner->api,owner->context,
        protocol,writing,data,count);
    if(status!=LT48_OK){owner->status=1;owner->forward_failed=1;}
    return status;
}
static lt48_status source_mapped_stream(lt48_runtime_transport_session *wire,
    const lt48_protocol_transport *api,void *context,lt48_pic_source_stream_protocol protocol,int writing,
    const lt48_pic_addtab_map *map,uint32_t first,uint32_t count,unsigned char *image,size_t image_size,
    lt48_pic_source_stream_begin begin,void *begin_context,lt48_pic_source_stream_result *result,uint32_t chunk) {
    if(result)*result=(lt48_pic_source_stream_result){0,UINT32_MAX};
    if(!wire||!api||!api->read||!api->write||!image||!image_size||!begin||!result||
       (writing!=0&&writing!=1)||!count||!lt48_pic_addtab_map_valid(map))return LT48_INVALID_ARGUMENT;
    if(protocol!=LT48_PIC_STREAM_BYTES_48)return LT48_UNSUPPORTED;
    uint32_t dense=first,remaining=count;lt48_pic_map_span span;
    /* Check all spans before beginning the first hardware callback. */
    while(remaining) {
        if(!lt48_pic_addtab_map_next(map,dense,remaining,&span)||
           span.length%map->element_bytes||span.image_offset>image_size||
           span.length>image_size-span.image_offset||span.length/512>UINT16_MAX)
            return LT48_SAFETY_REJECTED;
        dense+=span.length;remaining-=span.length;
    }
    dense=first;remaining=count;
    while(remaining) {
        if(!lt48_pic_addtab_map_next(map,dense,remaining,&span))return LT48_RUNTIME_ERROR;
        lt48_status status=api->cancelled&&api->cancelled(context)?LT48_CANCELLED:
            begin(begin_context,writing,&span);
        uint32_t offset=0;
        while(status==LT48_OK&&offset<span.length) {
            uint32_t block=span.length-offset;if(chunk&&block>chunk)block=chunk;
            status=source_stream_block(wire,api,context,protocol,writing,
                image+span.image_offset+offset,block,offset+block==span.length);
            if(status==LT48_OK){result->completed_bytes+=block;offset+=block;}
        }
        if(status!=LT48_OK){result->failed_dense_offset=dense+offset;return status;}
        dense+=span.length;remaining-=span.length;
    }
    return LT48_OK;
}
lt48_status lt48_pic_source_mapped_blank_internal(lt48_runtime_transport_session *wire,
    const lt48_protocol_transport *api,void *context,const lt48_pic_addtab_map *map,
    uint32_t first,uint32_t count,lt48_pic_source_stream_begin begin,void *begin_context,
    lt48_pic_source_stream_result *result) {
    if(result)*result=(lt48_pic_source_stream_result){0,UINT32_MAX};
    if(!wire||!api||!api->read||!api->write||!begin||!result||!lt48_pic_addtab_map_valid(map))return LT48_INVALID_ARGUMENT;
    uint32_t chunk=lt48_pic_source_stream_chunk_internal(count,map->element_bytes);
    if(!chunk)return LT48_INVALID_ARGUMENT;
    uint32_t dense=first,remaining=count;lt48_pic_map_span span;
    while(remaining) {
        if(!lt48_pic_addtab_map_next(map,dense,remaining,&span)||span.length%map->element_bytes)return LT48_SAFETY_REJECTED;
        dense+=span.length;remaining-=span.length;
    }
    dense=first;remaining=count;
    while(remaining) {
        if(!lt48_pic_addtab_map_next(map,dense,remaining,&span))return LT48_RUNTIME_ERROR;
        lt48_status status=api->cancelled&&api->cancelled(context)?LT48_CANCELLED:begin(begin_context,0,&span);
        uint32_t offset=0;
        while(status==LT48_OK&&offset<span.length) {
            uint32_t bytes=span.length-offset;if(bytes>chunk)bytes=chunk;
            uint32_t elements=bytes/map->element_bytes;
            unsigned char command[512]={0},reply[512],word[2]={(unsigned char)elements,(unsigned char)(elements>>8)};
            command[0]=0x44;command[3]=2;
            status=lt48_runtime_transport_session_flush(wire);
            if(status==LT48_OK)status=source_stream_transfer(api,context,0,2,command,sizeof(command));
            if(status==LT48_OK)status=source_stream_transfer(api,context,0,4,word,sizeof(word));
            memset(command,0,sizeof(command));command[0]=0x0b;
            if(status==LT48_OK)status=lt48_runtime_transport_session_flush(wire);
            if(status==LT48_OK)status=source_stream_transfer(api,context,0,2,command,sizeof(command));
            if(status==LT48_OK)status=source_stream_transfer(api,context,1,0x88,reply,sizeof(reply));
            if(status==LT48_OK&&!(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
            if(status==LT48_OK&&!(reply[2]&1)) {
                memset(command,0,sizeof(command));command[0]=0x43;command[3]=2;
                status=source_stream_transfer(api,context,0,2,command,sizeof(command));
                if(status==LT48_OK)status=source_stream_transfer(api,context,1,0x88,word,sizeof(word));
                uint32_t tail=(uint32_t)word[0]|(uint32_t)word[1]<<8;
                if(status==LT48_OK&&tail>=elements)status=LT48_PROTOCOL_ERROR;
                if(status==LT48_OK) {
                    uint32_t before=bytes-(tail+1)*map->element_bytes;
                    result->completed_bytes+=before;result->failed_dense_offset=dense+offset+before;
                    return LT48_DEVICE_MISMATCH;
                }
            }
            if(status==LT48_OK){result->completed_bytes+=bytes;offset+=bytes;}
        }
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(wire);
        if(status!=LT48_OK){result->failed_dense_offset=dense+offset;return status;}
        dense+=span.length;remaining-=span.length;
    }
    return LT48_OK;
}
lt48_status lt48_pic_source_mapped_stream_internal(lt48_runtime_transport_session *wire,
    const lt48_protocol_transport *api,void *context,lt48_pic_source_stream_protocol protocol,int writing,
    const lt48_pic_addtab_map *map,uint32_t first,uint32_t count,unsigned char *image,size_t image_size,
    lt48_pic_source_stream_begin begin,void *begin_context,lt48_pic_source_stream_result *result) {
    return source_mapped_stream(wire,api,context,protocol,writing,map,first,count,image,image_size,
        begin,begin_context,result,0);
}
lt48_status lt48_pic_source_mapped_worker_internal(lt48_runtime_transport_session *wire,
    const lt48_protocol_transport *api,void *context,lt48_pic_source_stream_protocol protocol,int writing,
    const lt48_pic_addtab_map *map,uint32_t first,uint32_t count,unsigned char *image,size_t image_size,
    lt48_pic_source_stream_begin begin,void *begin_context,lt48_pic_source_stream_result *result) {
    if(!lt48_pic_addtab_map_valid(map))return LT48_INVALID_ARGUMENT;
    uint32_t chunk=lt48_pic_source_stream_chunk_internal(count,map->element_bytes);
    if(!chunk)return LT48_INVALID_ARGUMENT;
    return source_mapped_stream(wire,api,context,protocol,writing,map,first,count,image,image_size,
        begin,begin_context,result,chunk);
}
lt48_status lt48_pic_source_transport_run_internal(lt48_pic_source_transport *owner,
    const lt48_pic_source_operation_contract *contract,uint16_t mode,uint32_t first,uint32_t last,
    const lt48_pic_source_range_options *options,lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!owner || !owner->prepared)return LT48_INVALID_ARGUMENT;
    if(owner->deferred_fpga || !contract || !options || !result || options->started || options->start) {
        lt48_status cleanup=source_wire_finish(owner,LT48_INVALID_ARGUMENT,LT48_OK);
        if(result){result->cleanup_attempted=1;result->cleanup_status=cleanup;}
        return LT48_INVALID_ARGUMENT;
    }
    lt48_pic_source_range_options bound=*options;
    bound.runner=source_wire_runner;bound.cancelled=source_wire_cancelled;bound.context=owner;
    lt48_status st=lt48_pic_source_operation_run_internal(owner->session,contract,&owner->host,mode,first,last,
        &bound,source_wire_finish,owner,result);
    /* Rejected preflight has not powered the selected device, but FPGA
     * preparation still touched hardware; release those settings as well. */
    if(!result->cleanup_attempted) {
        result->cleanup_attempted=1;result->cleanup_status=source_wire_finish(owner,st,LT48_OK);
    }
    return st==LT48_OK?result->cleanup_status:st;
}
static lt48_status source_applet_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **result) {
    lt48_pic_source_transport *owner=context;
    lt48_status status=source_wire_runner(context,machine,result);
    if(status==LT48_OK)status=lt48_runtime_transport_session_flush(owner->wire);
    if(status!=LT48_OK)owner->forward_failed=1;
    return status;
}
static lt48_status source_applet_cleanup_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **result) {
    lt48_pic_source_transport *owner=context;
    lt48_status status=source_wire_cleanup_runner(context,machine,result);
    if(status==LT48_OK)status=lt48_runtime_transport_session_flush(owner->wire);
    return status;
}
static lt48_status source_cleanup_queue(lt48_pic_source_transport *owner) {
    lt48_runtime_transport_session_close(owner->wire);owner->wire=NULL;
    lt48_protocol_transport clean=owner->api;clean.cancelled=NULL;
    return lt48_runtime_transport_session_create(&clean,owner->context,&owner->wire);
}
typedef struct {lt48_pic_source_transport *owner;lt48_status first;} source_finish_context;
static void source_finish_phase(void *context,enum lt48_pic_applet_finish_phase phase,
    lt48_pic_applet_finish_state *state) {
    source_finish_context *x=context;lt48_pic_source_transport *owner=x->owner;
    lt48_status st=LT48_OK;
    switch(phase) {
    case LT48_PIC_APP_FINISH_SETTLE:
        if(state->status)break; /* original 00442980 polls only status zero */
        st=source_cleanup_queue(owner);
        if(st==LT48_OK)st=lt48_runtime_transport_session_controller_status_internal(owner->wire,1,&state->status);
        if(st==LT48_OK&&state->status==2)st=LT48_DEVICE_MISMATCH;
        break;
    case LT48_PIC_APP_FINISH_OFF:
        st=lt48_pic_source_session_invoke_current_internal(owner->session,
            owner->host.poweroff_role,10,source_applet_cleanup_runner,owner);
        state->status=st==LT48_OK?0:st==LT48_TIMEOUT?1:4;
        break;
    case LT48_PIC_APP_FINISH_FPGA:
        /* Original 0043d3f0 resets status before loading, including after
         * POWEROFF failure. Retain that failure independently in x->first. */
        state->status=0;owner->status=0;owner->forward_failed=0;
        st=source_cleanup_queue(owner);
        if(st==LT48_OK)st=source_reload_fpga(owner);
        if(st!=LT48_OK)state->status=1;
        break;
    case LT48_PIC_APP_FINISH_RELEASE: {
        const unsigned char release[]={0x49,0xaa,0x55};state->status=0;
        st=source_cleanup_queue(owner);
        if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(owner->wire,release,sizeof(release));
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(owner->wire);
        if(st!=LT48_OK)state->status=1;
        break;
    }
    case LT48_PIC_APP_FINISH_PROGRESS:
    case LT48_PIC_APP_FINISH_REPORT:
        /* Public owner result/progress belongs to the caller. No Win32
         * progress object or timeout-triggered bridge reload is copied. */
        break;
    }
    if(x->first==LT48_OK)x->first=st;
}
static lt48_status source_powered_cleanup(lt48_pic_source_transport *owner,uint16_t reset_role) {
    if(owner->deferred_fpga) {
        source_finish_context x={owner,LT48_OK};
        lt48_pic_applet_finish_state state={owner->status,0,0,0,owner->fpga_size!=0,0,1};
        lt48_pic_applet_finish_sequence(&state,source_finish_phase,&x);
        owner->status=state.status;
        return x.first;
    }
    /* Retain the existing operation path until its admission is migrated. */
    lt48_status st=lt48_pic_source_session_invoke_current_internal(owner->session,
        reset_role,10,source_applet_cleanup_runner,owner);
    lt48_status off=lt48_pic_source_session_invoke_current_internal(owner->session,
        owner->host.poweroff_role,10,source_applet_cleanup_runner,owner);
    return st==LT48_OK?off:st;
}
typedef struct {
    lt48_pic_source_transport *owner;
    const lt48_pic_source_applet_transaction *plan;
    uint8_t data_in;
} source_action_context;
static lt48_status source_action_invoke(void *context,uint32_t address,uint32_t timeout) {
    source_action_context *a=context;
    lt48_status st=lt48_pic_source_session_applet_mode_internal(a->owner->session,
        a->plan->mode,0,address,a->plan->capacity,a->plan->size_buffer);
    if(st==LT48_OK)st=lt48_pic_source_session_invoke_current_internal(a->owner->session,
        a->plan->action_role,timeout,source_applet_runner,a->owner);
    const unsigned char *data;
    if(st==LT48_OK)st=lt48_pic_source_session_buffer_internal(a->owner->session,a->data_in,&data);
    return st==LT48_OK && !data[0]?LT48_DEVICE_MISMATCH:st;
}
typedef struct {
    lt48_pic_source_transport *owner;
    const lt48_pic_source_startup_plan *plan;
    uint16_t reset_role;uint32_t address;
    lt48_status status;int powered,compact;
    lt48_pic_contact_result contact;
    lt48_pic_source_identity_result identity;
} source_startup_execution;
static int source_startup_phase(void *context,enum lt48_pic_applet_startup_phase phase,
    lt48_pic_applet_startup_state *state) {
    source_startup_execution *x=context;lt48_pic_source_transport *owner=x->owner;
    const lt48_pic_source_startup_plan *p=x->plan;
    if(source_wire_cancelled(owner)){state->cancelled=1;x->status=LT48_CANCELLED;return -1;}
    lt48_status st=LT48_OK;int matched=1;
    switch(phase) {
    case LT48_PIC_APP_START_DOWNLOAD:
        /* Source resources were uploaded by deferred preparation, once. */
        return owner->deferred_fpga?1:-1;
    case LT48_PIC_APP_START_CONTACT:
        st=source_contact_capture(owner,&p->contact,p->alternate_payload,p->alternate_payload_size,&x->contact,&matched);break;
    case LT48_PIC_APP_START_CONTACT_CHOICE:
    case LT48_PIC_APP_START_ID_CHOICE:
        return p->decide?p->decide(p->context,phase==LT48_PIC_APP_START_ID_CHOICE,&x->contact,&x->identity):LT48_PIC_APP_START_ABORT;
    case LT48_PIC_APP_START_CONTACT_LOG:return 1;
    case LT48_PIC_APP_START_FPGA:st=source_reload_fpga(owner);break;
    case LT48_PIC_APP_START_SELECTOR:
        /* host_begin emits this source selector immediately before POWERON. */
        return 1;
    case LT48_PIC_APP_START_POWER: {
        x->powered=1;lt48_pic_source_host host=owner->host;host.runner=source_applet_runner;
        if(x->compact)host.voltage_centivolts=state->nominal;
        st=lt48_pic_source_host_begin_internal(owner->session,&host,(uint16_t)state->mode,x->address);break;
    }
    case LT48_PIC_APP_START_ID:
        st=x->compact?lt48_pic_source_compact_identity_session(owner->session,&p->identity,source_applet_runner,owner,&x->identity):
            lt48_pic_source_identity_session(owner->session,&p->identity,source_applet_runner,owner,&x->identity);
        matched=(int)x->identity.matched;break;
    case LT48_PIC_APP_START_CLEANUP: {
        st=source_powered_cleanup(owner,x->reset_role);
        if(st==LT48_OK) {
            x->powered=0;
            /* Cleanup runs uncancelled; re-entry must regain the normal
             * cancellation-aware transport instead of reusing that queue. */
            lt48_runtime_transport_session_close(owner->wire);owner->wire=NULL;
            st=lt48_runtime_transport_session_create(&owner->api,owner->context,&owner->wire);
            owner->status=0;owner->forward_failed=0;
        }
        break;
    }
    default:st=LT48_UNSUPPORTED;break;
    }
    if(st!=LT48_OK){x->status=st;return -1;}
    return matched;
}
lt48_status lt48_pic_source_transport_bind_startup_internal(lt48_pic_source_transport *owner,
    const lt48_pic_source_startup_plan *plan,uint32_t flags) {
    if(!owner||!plan)return LT48_INVALID_ARGUMENT;
    if(owner->prepared)return LT48_SAFETY_REJECTED;
    if(flags&~UINT32_C(0x80003))return LT48_UNSUPPORTED;
    if(plan->contact.alternate>1)return LT48_INVALID_ARGUMENT;
    const uint32_t *pairs[]={plan->contact.required,plan->contact.forbidden,plan->contact.auxiliary,plan->contact.adapter};
    for(unsigned i=0;i<4;i++)for(unsigned j=0;j<2;j++)if(pairs[i][j]>UINT32_C(0xffffff))return LT48_INVALID_ARGUMENT;
    if((flags&2u)&&plan->identity.table_index)return LT48_UNSUPPORTED;
    if((flags&2u)&&plan->identity.getid_role!=UINT16_MAX) {
        if(plan->identity.manufacturer_buffer>=32||plan->identity.device_buffer>=32||
           plan->identity.manufacturer_buffer==plan->identity.device_buffer)return LT48_INVALID_ARGUMENT;
        if(!lt48_pic_source_session_has_callback_internal(owner->session,plan->identity.getid_role))return LT48_UNSUPPORTED;
    }
    owner->startup=*plan;owner->startup_flags=flags;owner->startup_bound=1;return LT48_OK;
}
static lt48_status source_startup_run_profile(lt48_pic_source_transport *owner,
    const lt48_pic_source_startup_plan *plan,uint32_t flags,uint16_t mode,uint32_t address,
    uint16_t reset_role,int *powered,int compact,unsigned *checked) {
    source_startup_execution x={0};x.owner=owner;x.plan=plan;x.reset_role=reset_role;
    x.address=address;x.status=LT48_OK;x.compact=compact;
    uint32_t nominal=owner->host.voltage_centivolts;
    if(compact) {
        lt48_compile_variable voltage;
        lt48_status st=lt48_pic_source_session_variable_internal(owner->session,owner->host.voltage_variable,&voltage);
        if(st!=LT48_OK)return st;
        nominal=voltage.value;
    }
    lt48_pic_applet_startup_state state={flags,mode,nominal,1,*checked,0};
    int ok=compact?lt48_pic_compact_startup_sequence(&state,source_startup_phase,&x):
        lt48_pic_applet_startup_sequence(&state,source_startup_phase,&x);
    *checked=state.checked;*powered=x.powered;
    return ok?LT48_OK:x.status!=LT48_OK?x.status:LT48_CANCELLED;
}
static lt48_status source_startup_run(lt48_pic_source_transport *owner,
    const lt48_pic_source_startup_plan *plan,uint32_t flags,uint16_t mode,uint32_t address,uint16_t reset_role,int *powered) {
    unsigned checked=0;
    return source_startup_run_profile(owner,plan,flags,mode,address,reset_role,powered,0,&checked);
}
static lt48_status source_begin(lt48_pic_source_transport *owner,uint16_t mode,uint32_t address,
    uint16_t reset_role,int *powered) {
    if(owner->startup_bound) {
        if(!owner->deferred_fpga)return LT48_UNSUPPORTED;
        return source_startup_run(owner,&owner->startup,owner->startup_flags,mode,address,reset_role,powered);
    }
    if(owner->deferred_fpga)return LT48_UNSUPPORTED;
    *powered=1;lt48_pic_source_host host=owner->host;host.runner=source_applet_runner;
    return lt48_pic_source_host_begin_internal(owner->session,&host,mode,address);
}
static lt48_status source_applet_transaction_run(
    lt48_pic_source_transport *owner,const lt48_pic_source_applet_transaction *plan,
    const lt48_pic_source_applet_action_range *range,uint8_t data_in,uint16_t configuration_id,
    lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!owner||!owner->prepared||!result)return LT48_INVALID_ARGUMENT;
    lt48_status status=LT48_INVALID_ARGUMENT,source_cleanup=LT48_OK;
    int powered=0;
    if(owner->deferred_fpga&&!owner->startup_bound){status=LT48_UNSUPPORTED;goto done;}
    if(!plan||!plan->mode||!plan->action_timeout_seconds)goto done;
    if(configuration_id) {
        const unsigned char *data;
        if(!lt48_pic_source_applet_configuration_whole(configuration_id)) {status=LT48_UNSUPPORTED;goto done;}
        if(range||(plan->mode!=12&&plan->mode!=13)||plan->address||plan->action_timeout_seconds!=10||
           lt48_pic_source_session_buffer_internal(owner->session,data_in,&data)!=LT48_OK)goto done;
    }
    if(range) {
        const unsigned char *data;
        if(!lt48_pic_source_applet_action_valid(range)||range->last>=plan->capacity||
           range->first!=plan->address||
           lt48_pic_source_session_buffer_internal(owner->session,data_in,&data)!=LT48_OK)goto done;
    }
    const uint16_t roles[]={owner->host.poweron_role,owner->host.poweroff_role,
        plan->init_role,plan->action_role,plan->reset_role};
    for(size_t i=0;i<sizeof(roles)/sizeof(roles[0]);i++)
        if(!lt48_pic_source_session_has_callback_internal(owner->session,roles[i])) {
            status=LT48_UNSUPPORTED;goto done;
        }
    status=lt48_pic_source_session_applet_mode_internal(owner->session,plan->mode,1,
        plan->address,plan->capacity,plan->size_buffer);
    if(status!=LT48_OK)goto done;
    status=source_begin(owner,plan->mode,plan->address,plan->reset_role,&powered);
    if(status==LT48_OK)status=lt48_pic_source_session_applet_callback_internal(owner->session,
        plan->init_role,plan->mode,plan->address,plan->capacity,plan->size_buffer,source_applet_runner,owner);
    if(status==LT48_OK) {
        if(range) {
            source_action_context action={owner,plan,data_in};
            lt48_pic_source_applet_action_result traversed;
            status=lt48_pic_source_applet_action_run(range,source_action_invoke,&action,&traversed);
            result->traversal.completed_elements=traversed.completed_elements;
            result->traversal.first_mismatch=traversed.failed_address;
            lt48_status advance=lt48_pic_source_session_applet_mode_internal(owner->session,
                plan->mode,0,traversed.next_address,plan->capacity,plan->size_buffer);
            if(status==LT48_OK)status=advance;
        } else {
            status=lt48_pic_source_session_invoke_current_internal(owner->session,
                plan->action_role,plan->action_timeout_seconds,source_applet_runner,owner);
            if(configuration_id) {
                const unsigned char *data=NULL;
                if(status==LT48_OK)status=lt48_pic_source_session_buffer_internal(owner->session,data_in,&data);
                status=lt48_pic_source_applet_configuration_result(plan->mode,status,data&&data[0]);
            }
        }
    }
done:
    result->operation_status=status;result->cleanup_attempted=1;
    if(powered) {
        source_cleanup=source_powered_cleanup(owner,plan->reset_role);
    }
    lt48_status global=source_wire_finish(owner,status,source_cleanup);
    result->cleanup_status=source_cleanup==LT48_OK?global:source_cleanup;
    return status==LT48_OK?result->cleanup_status:status;
}
lt48_status lt48_pic_source_transport_applet_transaction_internal(
    lt48_pic_source_transport *owner,const lt48_pic_source_applet_transaction *plan,
    lt48_pic_source_operation_result *result) {
    return source_applet_transaction_run(owner,plan,NULL,0,0,result);
}
lt48_status lt48_pic_source_transport_applet_action_internal(
    lt48_pic_source_transport *owner,const lt48_pic_source_applet_transaction *plan,
    const lt48_pic_source_applet_action_range *range,uint8_t data_in,
    lt48_pic_source_operation_result *result) {
    if(!range) {
        if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
        if(owner&&owner->prepared&&result)return source_applet_transaction_run(owner,NULL,NULL,0,0,result);
        return LT48_INVALID_ARGUMENT;
    }
    return source_applet_transaction_run(owner,plan,range,data_in,0,result);
}
lt48_status lt48_pic_source_transport_applet_configuration_internal(
    lt48_pic_source_transport *owner,const lt48_pic_source_applet_transaction *plan,
    uint16_t configuration_id,uint8_t data_in,lt48_pic_source_operation_result *result) {
    return source_applet_transaction_run(owner,configuration_id?plan:NULL,NULL,data_in,configuration_id,result);
}

typedef struct {
    lt48_pic_source_transport *owner;
    const lt48_pic_source_applet_stream_plan *plan;
} source_mapped_execution;
static lt48_status source_mapped_begin(void *context,int writing,const lt48_pic_map_span *span) {
    (void)writing;source_mapped_execution *execution=context;
    lt48_pic_source_transport *owner=execution->owner;
    lt48_status status=lt48_pic_source_session_applet_span_internal(owner->session,span,
        execution->plan->image_buffer,execution->plan->increment_buffer);
    if(status==LT48_OK)status=lt48_pic_source_session_applet_invoke_selected_internal(
        owner->session,10,source_applet_runner,owner);
    return status;
}
static lt48_status source_applet_stream_run(
    lt48_pic_source_transport *owner,const lt48_pic_source_applet_stream_plan *plan,
    uint16_t tail,unsigned char *image,size_t image_size,lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!owner||!owner->prepared||!result)return LT48_INVALID_ARGUMENT;
    lt48_status status=LT48_INVALID_ARGUMENT,source_cleanup=LT48_OK;
    int powered=0;unsigned char *staged=NULL;lt48_pic_source_session *probe=NULL;
    lt48_pic_source_stream_result transfer={0,UINT32_MAX};
    if(owner->deferred_fpga&&!owner->startup_bound){status=LT48_UNSUPPORTED;goto done;}
    if(!plan||!image||!image_size||!plan->mode||(plan->writing!=0&&plan->writing!=1)||
       !plan->count||!lt48_pic_addtab_map_valid(plan->map)||
       plan->image_buffer==plan->size_buffer||plan->increment_buffer==plan->size_buffer)goto done;
    uint32_t chunk=lt48_pic_source_stream_chunk_internal(plan->count,plan->map->element_bytes);
    if(!chunk){status=LT48_INVALID_ARGUMENT;goto done;}
    if(plan->protocol!=LT48_PIC_STREAM_BYTES_48){status=LT48_UNSUPPORTED;goto done;}
    if(tail!=UINT16_MAX&&(plan->mode!=1||plan->writing||
        !lt48_pic_source_session_has_callback_internal(owner->session,tail))) {
        status=LT48_UNSUPPORTED;goto done;
    }
    const uint16_t roles[]={owner->host.poweron_role,owner->host.poweroff_role,
        plan->init_role,plan->reset_role};
    for(size_t i=0;i<sizeof(roles)/sizeof(roles[0]);i++)
        if(!lt48_pic_source_session_has_callback_internal(owner->session,roles[i])) {
            status=LT48_UNSUPPORTED;goto done;
        }
    /* Validate mode selection and every mapping output on an independent frame
     * before powering the device. Invalid later spans cannot execute an earlier
     * callback or partially replace the public read image. */
    status=lt48_pic_source_session_clone_internal(owner->session,&probe);
    if(status!=LT48_OK)goto done;
    status=lt48_pic_source_session_applet_mode_internal(probe,plan->mode,1,0,
        plan->map->dense_capacity,plan->size_buffer);
    if(status==LT48_OK&&plan->mode==3)status=lt48_pic_source_session_applet_mode_internal(probe,11,0,0,plan->map->dense_capacity,plan->size_buffer);
    uint16_t selected=0;
    if(status==LT48_OK)status=lt48_pic_source_session_applet_stream_role_internal(probe,&selected);
    if(status==LT48_OK&&!lt48_pic_source_session_has_callback_internal(probe,selected))status=LT48_UNSUPPORTED;
    uint32_t dense=plan->first,remaining=plan->count;lt48_pic_map_span span;
    while(status==LT48_OK&&remaining) {
        if(!lt48_pic_addtab_map_next(plan->map,dense,remaining,&span)||
           span.length%plan->map->element_bytes||span.image_offset>image_size||
           span.length>image_size-span.image_offset||span.length/512>UINT16_MAX) {
            status=LT48_SAFETY_REJECTED;break;
        }
        status=lt48_pic_source_session_applet_span_internal(probe,&span,plan->image_buffer,plan->increment_buffer);
        dense+=span.length;remaining-=span.length;
    }
    lt48_pic_source_session_close_internal(probe);probe=NULL;
    if(status!=LT48_OK)goto done;
    if(!plan->writing&&plan->mode!=3) {
        staged=malloc(image_size);if(!staged){status=LT48_NO_MEMORY;goto done;}
        memcpy(staged,image,image_size);
    }
    status=lt48_pic_source_session_applet_mode_internal(owner->session,plan->mode,1,0,
        plan->map->dense_capacity,plan->size_buffer);
    if(status!=LT48_OK)goto done;
    status=source_begin(owner,plan->mode,0,plan->reset_role,&powered);
    if(status==LT48_OK)status=lt48_pic_source_session_applet_callback_internal(owner->session,
        plan->init_role,plan->mode==3?11:plan->mode,0,plan->map->dense_capacity,plan->size_buffer,source_applet_runner,owner);
    source_mapped_execution execution={owner,plan};
    if(status==LT48_OK&&plan->mode==3)status=lt48_pic_source_mapped_blank_internal(owner->wire,&owner->api,
        owner->context,plan->map,plan->first,plan->count,source_mapped_begin,&execution,&transfer);
    else if(status==LT48_OK)status=lt48_pic_source_mapped_worker_internal(owner->wire,&owner->api,
        owner->context,plan->protocol,plan->writing,plan->map,plan->first,plan->count,
        staged?staged:image,image_size,source_mapped_begin,&execution,&transfer);
    if(status==LT48_OK&&tail!=UINT16_MAX) {
        lt48_pic_source_state state;
        status=lt48_pic_source_session_state_internal(owner->session,&state);
        if(status==LT48_OK) {
            state.image=staged;state.image_size=image_size;
            status=lt48_pic_source_session_restore_internal(owner->session,&state);
        }
        if(status==LT48_OK)status=lt48_pic_source_session_applet_callback_internal(
            owner->session,tail,plan->mode,0,plan->map->dense_capacity,plan->size_buffer,
            source_applet_runner,owner);
        const unsigned char *combined=NULL;size_t combined_size=0;
        if(status==LT48_OK)status=lt48_pic_source_session_image_internal(owner->session,&combined,&combined_size);
        if(status==LT48_OK&&combined_size!=image_size)status=LT48_RUNTIME_ERROR;
        if(status==LT48_OK)memcpy(staged,combined,image_size);
    }
    if(status!=LT48_OK)owner->forward_failed=1;
done:
    lt48_pic_source_session_close_internal(probe);
    result->operation_status=status;result->cleanup_attempted=1;
    result->traversal.completed_elements=plan&&plan->map&&plan->map->element_bytes?transfer.completed_bytes/plan->map->element_bytes:0;
    result->traversal.first_mismatch=transfer.failed_dense_offset==UINT32_MAX?UINT32_MAX:
        transfer.failed_dense_offset/plan->map->element_bytes;
    if(powered) {
        source_cleanup=source_powered_cleanup(owner,plan->reset_role);
    }
    lt48_status global=source_wire_finish(owner,status,source_cleanup);
    result->cleanup_status=source_cleanup==LT48_OK?global:source_cleanup;
    lt48_status outcome=status==LT48_OK?result->cleanup_status:status;
    if(outcome==LT48_OK&&staged)memcpy(image,staged,image_size);
    free(staged);return outcome;
}
lt48_status lt48_pic_source_transport_applet_stream_run_internal(
    lt48_pic_source_transport *owner,const lt48_pic_source_applet_stream_plan *plan,
    unsigned char *image,size_t image_size,lt48_pic_source_operation_result *result) {
    return source_applet_stream_run(owner,plan,UINT16_MAX,image,image_size,result);
}
lt48_status lt48_pic_source_transport_applet_read_tail_internal(
    lt48_pic_source_transport *owner,const lt48_pic_source_applet_stream_plan *plan,uint16_t tail,
    unsigned char *image,size_t image_size,lt48_pic_source_operation_result *result) {
    if(tail==UINT16_MAX)return LT48_INVALID_ARGUMENT;
    return source_applet_stream_run(owner,plan,tail,image,image_size,result);
}

typedef struct {
    lt48_pic_source_transport *owner;
    const lt48_pic_source_applet_program_plan *plan;
    const unsigned char *image;size_t image_size;
    lt48_pic_source_operation_result *result;
    lt48_status status;
    int powered;
} source_program_execution;
static lt48_status source_program_init(source_program_execution *x,uint16_t mode,uint32_t address) {
    return lt48_pic_source_session_applet_callback_internal(x->owner->session,x->plan->stream.init_role,
        mode,address,x->plan->capacity,x->plan->stream.size_buffer,source_applet_runner,x->owner);
}
static lt48_status source_program_transfer(source_program_execution *x,uint16_t mode,int blank) {
    lt48_pic_source_applet_stream_plan stream=x->plan->stream;stream.mode=mode;stream.writing=!blank;
    source_mapped_execution mapped={x->owner,&stream};lt48_pic_source_stream_result transferred={0,UINT32_MAX};
    lt48_status st=blank?lt48_pic_source_mapped_blank_internal(x->owner->wire,&x->owner->api,x->owner->context,
        stream.map,stream.first,stream.count,source_mapped_begin,&mapped,&transferred):
        lt48_pic_source_mapped_worker_internal(x->owner->wire,&x->owner->api,x->owner->context,
        stream.protocol,1,stream.map,stream.first,stream.count,(unsigned char *)x->image,x->image_size,source_mapped_begin,&mapped,&transferred);
    if(!blank) {
        if(mode==2)x->result->traversal.completed_elements=transferred.completed_bytes/stream.map->element_bytes;
        if(st!=LT48_OK)x->result->traversal.first_mismatch=transferred.failed_dense_offset==UINT32_MAX?UINT32_MAX:
            transferred.failed_dense_offset/stream.map->element_bytes;
    }
    return st;
}
static int source_program_phase(void *context,enum lt48_pic_applet_program_phase phase,
    lt48_pic_applet_program_state *state) {
    source_program_execution *x=context;lt48_pic_source_transport *owner=x->owner;
    const lt48_pic_source_applet_program_plan *p=x->plan;lt48_status st=LT48_OK;
    if(phase==LT48_PIC_APP_PROGRAM_RESULT)return x->status==LT48_OK&&!state->aborted&&!state->cancelled;
    if(phase==LT48_PIC_APP_PROGRAM_FINISH)return 1; /* unconditional cleanup is owned below */
    if(source_wire_cancelled(owner)){state->cancelled=1;x->status=LT48_CANCELLED;return 0;}
    switch(phase) {
    case LT48_PIC_APP_PROGRAM_MODE:
        st=lt48_pic_source_session_applet_mode_internal(owner->session,2,1,0,p->capacity,p->stream.size_buffer);break;
    case LT48_PIC_APP_PROGRAM_STARTUP: {
        if(p->startup) {
            st=source_startup_run(owner,p->startup,p->flags,2,0,p->stream.reset_role,&x->powered);
        } else {
            x->powered=1;lt48_pic_source_host host=owner->host;host.runner=source_applet_runner;
            st=lt48_pic_source_host_begin_internal(owner->session,&host,2,0);
        }
        if(st==LT48_OK)st=p->preserve(p->policy_context,owner->session);
        if(st==LT48_OK)st=lt48_pic_source_session_image_internal(owner->session,&x->image,&x->image_size);
        break;
    }
    case LT48_PIC_APP_PROGRAM_ERASE: {
        st=source_program_init(x,7,p->erase_range.first);
        lt48_pic_source_applet_transaction action={7,p->stream.init_role,p->erase_role,p->stream.reset_role,
            p->stream.size_buffer,p->capacity,p->erase_range.first,10};
        source_action_context ctx={owner,&action,p->data_in};lt48_pic_source_applet_action_result traversed;
        if(st==LT48_OK)st=lt48_pic_source_applet_action_run(&p->erase_range,source_action_invoke,&ctx,&traversed);
        if(st==LT48_OK)st=lt48_pic_source_session_applet_mode_internal(owner->session,7,0,traversed.next_address,p->capacity,p->stream.size_buffer);
        break;
    }
    case LT48_PIC_APP_PROGRAM_BLANK:
        st=source_program_init(x,11,p->stream.first);
        if(st==LT48_OK)st=source_program_transfer(x,11,1);
        if(st==LT48_DEVICE_MISMATCH){state->blank=0;st=LT48_OK;}
        break;
    case LT48_PIC_APP_PROGRAM_ASK_ERASE:
        return p->erase_choice?p->erase_choice(p->policy_context):0;
    case LT48_PIC_APP_PROGRAM_INIT:break; /* audited population has no UNLOCK */
    case LT48_PIC_APP_PROGRAM_PROGRESS:st=source_program_init(x,2,p->stream.first);break;
    case LT48_PIC_APP_PROGRAM_STREAM:st=source_program_transfer(x,2,0);break;
    case LT48_PIC_APP_PROGRAM_END_STREAM:st=lt48_runtime_transport_session_flush(owner->wire);break;
    case LT48_PIC_APP_PROGRAM_VERIFY: {
        lt48_compile_variable saved;st=lt48_pic_source_session_variable_internal(owner->session,owner->host.voltage_variable,&saved);
        if(st!=LT48_OK)break;
        for(size_t i=0;st==LT48_OK&&i<p->verify_voltage_count;i++) {
            st=lt48_pic_source_session_voltage_value_internal(owner->session,owner->host.voltage_variable,p->verify_voltages[i]);
            if(st==LT48_OK)st=source_program_init(x,4,p->stream.first);
            if(st==LT48_OK)st=source_program_transfer(x,4,0);
        }
        lt48_status restored=lt48_pic_source_session_voltage_value_internal(owner->session,owner->host.voltage_variable,saved.value);
        if(st==LT48_OK)st=restored;
        break;
    }
    case LT48_PIC_APP_PROGRAM_PROTECT: {
        st=source_program_init(x,13,0);
        if(st==LT48_OK)st=lt48_pic_source_session_invoke_current_internal(owner->session,p->protect_role,10,source_applet_runner,owner);
        const unsigned char *data=NULL;
        if(st==LT48_OK)st=lt48_pic_source_session_buffer_internal(owner->session,p->data_in,&data);
        st=lt48_pic_source_applet_configuration_result(13,st,data&&data[0]);break;
    }
    default:st=LT48_UNSUPPORTED;break;
    }
    if(st!=LT48_OK) {
        if(x->status==LT48_OK)x->status=st;
        if(st==LT48_CANCELLED)state->cancelled=1;
        return 0;
    }
    return 1;
}
lt48_status lt48_pic_source_transport_applet_program_internal(lt48_pic_source_transport *owner,
    const lt48_pic_source_applet_program_plan *p,unsigned char *image,size_t image_size,
    lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!owner||!owner->prepared||!result)return LT48_INVALID_ARGUMENT;
    source_program_execution x={owner,p,image,image_size,result,LT48_INVALID_ARGUMENT,0};
    lt48_pic_source_applet_program_plan bound;uint32_t automatic_levels[4];
    lt48_status cleanup=LT48_OK;lt48_pic_source_session *probe=NULL,*original=owner->session;
    int active=0;
    if(!p||!image||!image_size||!p->preserve||!p->capacity||p->stream.mode!=2||p->stream.writing!=1||
       !lt48_pic_addtab_map_valid(p->stream.map)||!p->stream.count||
       p->stream.size_buffer==p->stream.image_buffer||p->stream.size_buffer==p->stream.increment_buffer)goto done;
    if(!p->startup&&owner->startup_bound) {
        bound=*p;bound.startup=&owner->startup;
        bound.flags=(p->flags&~UINT32_C(0x80003))|owner->startup_flags;
        p=&bound;x.plan=p;
    }
    if((p->flags&~UINT32_C(0xc103f))||p->stream.protocol!=LT48_PIC_STREAM_BYTES_48) {x.status=LT48_UNSUPPORTED;goto done;}
    if((p->startup&&!owner->deferred_fpga)||(!p->startup&&(owner->deferred_fpga||(p->flags&UINT32_C(0x80003))))) {
        x.status=LT48_UNSUPPORTED;goto done;
    }
    if(p->startup&&(p->flags&2u)&&p->startup->identity.table_index) {x.status=LT48_UNSUPPORTED;goto done;}
    if(p->startup&&(p->flags&2u)&&p->startup->identity.getid_role!=UINT16_MAX&&
        !lt48_pic_source_session_has_callback_internal(owner->session,p->startup->identity.getid_role)) {x.status=LT48_UNSUPPORTED;goto done;}
    if((p->flags&16)&&p->auto_verify_voltages) {
        uint32_t explicit_values[4]={0};unsigned present=0,count=0;lt48_compile_variable variable;
        x.status=lt48_pic_source_session_variable_internal(owner->session,owner->host.voltage_variable,&variable);
        if(x.status!=LT48_OK)goto done;
        uint32_t nominal=variable.value;
        for(unsigned i=0;i<4;i++)if(p->verify_variables[i]!=UINT16_MAX) {
            x.status=lt48_pic_source_session_variable_internal(owner->session,p->verify_variables[i],&variable);
            if(x.status!=LT48_OK)goto done;
            if(variable.kind!=0){x.status=LT48_UNSUPPORTED;goto done;}
            explicit_values[i]=variable.value;present|=1u<<i;
        }
        x.status=lt48_pic_source_verify_voltages(nominal,explicit_values,present,p->verify_margin_type,
            p->verify_nominal_only,automatic_levels,&count);
        if(x.status!=LT48_OK)goto done;
        if(present&&!p->verify_nominal_only&&present!=((1u<<count)-1u)) {x.status=LT48_UNSUPPORTED;goto done;}
        bound=*p;bound.verify_voltages=automatic_levels;bound.verify_voltage_count=count;p=&bound;x.plan=p;
        x.status=LT48_INVALID_ARGUMENT; /* remaining preflight failures cannot report selector success */
    }
    if((p->flags&32)&&!lt48_pic_source_applet_configuration_whole(p->configuration_id)) {x.status=LT48_UNSUPPORTED;goto done;}
    if((p->flags&16)&&(!p->verify_voltages||!p->verify_voltage_count||p->verify_voltage_count>4))goto done;
    for(size_t i=0;(p->flags&16)&&i<p->verify_voltage_count;i++)
        if(p->verify_voltages[i]<owner->host.minimum_centivolts||p->verify_voltages[i]>owner->host.maximum_centivolts||p->verify_voltages[i]>UINT16_MAX)goto done;
    uint16_t roles[]={owner->host.poweron_role,owner->host.poweroff_role,p->stream.init_role,p->stream.reset_role,p->erase_role,p->protect_role};
    for(size_t i=0;i<6;i++) {
        if((i==4&&p->erase_role==UINT16_MAX)||(i==5&&!(p->flags&32)))continue;
        if(!lt48_pic_source_session_has_callback_internal(owner->session,roles[i])) {x.status=LT48_UNSUPPORTED;goto done;}
    }
    if(p->erase_role!=UINT16_MAX&&(!lt48_pic_source_applet_action_valid(&p->erase_range)||p->erase_range.last>=p->capacity))goto done;
    const unsigned char *data;if(lt48_pic_source_session_buffer_internal(owner->session,p->data_in,&data)!=LT48_OK)goto done;
    x.status=lt48_pic_source_session_clone_internal(owner->session,&probe);
    if(x.status!=LT48_OK)goto done;
    const uint16_t modes[]={2,11,4};
    for(size_t i=0;x.status==LT48_OK&&i<3;i++) {
        if((i==1&&!(p->flags&4))||(i==2&&!(p->flags&16)))continue;
        x.status=lt48_pic_source_session_applet_mode_internal(probe,modes[i],1,0,p->capacity,p->stream.size_buffer);
        uint16_t role;if(x.status==LT48_OK)x.status=lt48_pic_source_session_applet_stream_role_internal(probe,&role);
        if(x.status==LT48_OK&&!lt48_pic_source_session_has_callback_internal(probe,role))x.status=LT48_UNSUPPORTED;
    }
    uint32_t dense=p->stream.first,remaining=p->stream.count;lt48_pic_map_span span;
    while(x.status==LT48_OK&&remaining) {
        if(!lt48_pic_addtab_map_next(p->stream.map,dense,remaining,&span)||span.image_offset>image_size||
           span.length>image_size-span.image_offset||span.length%p->stream.map->element_bytes||span.length/512>UINT16_MAX) {x.status=LT48_SAFETY_REJECTED;break;}
        x.status=lt48_pic_source_session_applet_span_internal(probe,&span,p->stream.image_buffer,p->stream.increment_buffer);
        dense+=span.length;remaining-=span.length;
    }
    if(x.status==LT48_OK) {
        lt48_pic_source_state staged;
        x.status=lt48_pic_source_session_state_internal(probe,&staged);
        if(x.status==LT48_OK) {
            staged.image=image;staged.image_size=image_size;
            x.status=lt48_pic_source_session_restore_internal(probe,&staged);
        }
    }
    if(x.status==LT48_OK) {
        /* Preserve, program and verify one canonical image on a private session. */
        owner->session=probe;active=1;
        lt48_pic_applet_program_state state={p->flags,1,p->erase_role!=UINT16_MAX,0,0,0,0,0};
        int ok=lt48_pic_applet_program_sequence(&state,source_program_phase,&x);
        if(!ok&&x.status==LT48_OK)x.status=state.aborted||state.cancelled?LT48_CANCELLED:LT48_DEVICE_MISMATCH;
    }
done:
    result->operation_status=x.status;result->cleanup_attempted=1;
    if(x.powered) {
        cleanup=source_powered_cleanup(owner,p->stream.reset_role);
    }
    lt48_status global=source_wire_finish(owner,x.status,cleanup);result->cleanup_status=cleanup==LT48_OK?global:cleanup;
    owner->session=original;
    if(active&&x.status==LT48_OK&&result->cleanup_status==LT48_OK) {
        lt48_pic_source_state committed;
        x.status=lt48_pic_source_session_state_internal(probe,&committed);
        if(x.status==LT48_OK)x.status=lt48_pic_source_session_restore_internal(original,&committed);
        if(x.status==LT48_OK)memmove(image,committed.image,image_size);
        else {
            /* A failed host commit must not leave the success indication lit. */
            source_wire_finish(owner,x.status,LT48_OK);
            result->operation_status=x.status;
        }
    }
    lt48_pic_source_session_close_internal(probe);
    return x.status==LT48_OK?result->cleanup_status:x.status;
}

int lt48_pic_source_transport_matches_origin_internal(const lt48_pic_source_transport *owner,const void *origin) {
    return owner&&lt48_pic_source_session_matches_origin_internal(owner->session,origin);
}

lt48_status lt48_pic_source_transport_load_internal(lt48_runtime_transport_session *wire,
    const unsigned char *fpga,size_t fpga_size,const lt48_pic_source_applet_payload *payloads,size_t count,uint8_t selector) {
    if(!wire||!fpga||!fpga_size||count>256||(count&&!payloads))return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<count;i++) {
        const lt48_pic_source_applet_payload *p=payloads+i;
        if(!p->data||!p->size||p->size>65536u-(size_t)p->address)return LT48_SAFETY_REJECTED;
        for(size_t j=0;j<i;j++) {
            const lt48_pic_source_applet_payload *q=payloads+j;
            size_t first=p->address>q->address?p->address:q->address;
            size_t pe=(size_t)p->address+p->size,qe=(size_t)q->address+q->size,last=pe<qe?pe:qe;
            if(first<last&&memcmp(p->data+first-p->address,q->data+first-q->address,last-first))return LT48_SAFETY_REJECTED;
        }
    }
    lt48_status status=LT48_OK;
    for(size_t i=0;status==LT48_OK&&i<count;i++) {
        status=lt48_runtime_transport_session_wave_watchdog(wire,10000,1);
        if(status==LT48_OK)status=lt48_owner_load_applet_internal(wire,payloads[i].address,payloads[i].data,payloads[i].size);
    }
    const unsigned char head[]={0x38,0xaa,0x55},select[]={0x0f,selector};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,head,sizeof(head));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,fpga,fpga_size);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,select,sizeof(select));
    if(status==LT48_OK)status=lt48_runtime_transport_session_flush(wire);
    return status;
}

typedef struct {
    lt48_pic_source_transport *owner;
    lt48_pic_source_startup_plan startup;
    uint32_t startup_flags;
    const lt48_pic_source_program_request *request;
    unsigned checked;
    int reentry;
} source_compact_program_transport;
static lt48_status source_compact_program_start(void *context,lt48_pic_source_session *stage,
    uint16_t mode,uint32_t address) {
    source_compact_program_transport *x=context;lt48_pic_source_transport *owner=x->owner;
    owner->session=stage;
    if(!owner->prepared) {
        if(!x->reentry)return LT48_SAFETY_REJECTED;
        /* A successful intermediate finish retained the RAM resources. Use
         * a fresh forward queue; startup reloads the selected FPGA again. */
        lt48_runtime_transport_session_close(owner->wire);owner->wire=NULL;
        lt48_status st=lt48_runtime_transport_session_create(&owner->api,owner->context,&owner->wire);
        if(st!=LT48_OK)return st;
        owner->prepared=1;owner->deferred_fpga=1;owner->forward_failed=0;owner->status=0;
    }
    int powered=0;x->reentry=0;
    return source_startup_run_profile(owner,&x->startup,x->startup_flags,mode,address,
        LT48_PIC_ABSENT_ROLE,&powered,1,&x->checked);
}
static lt48_status source_compact_program_runner(void *context,lt48_runtime_machine *machine,
    lt48_runtime_result **result) {
    source_compact_program_transport *x=context;
    return source_applet_runner(x->owner,machine,result);
}
static int source_compact_program_cancelled(void *context) {
    source_compact_program_transport *x=context;return source_wire_cancelled(x->owner);
}
static int source_compact_program_ask(void *context) {
    source_compact_program_transport *x=context;
    return x->request->ask_erase && x->request->ask_erase(x->request->options.context);
}
static lt48_status source_compact_program_finish(void *context,lt48_pic_source_session *stage,
    uint16_t mode,lt48_status operation) {
    (void)mode;source_compact_program_transport *x=context;lt48_pic_source_transport *owner=x->owner;
    owner->session=stage;
    lt48_status source=source_powered_cleanup(owner,LT48_PIC_ABSENT_ROLE);
    lt48_status global=source_wire_finish(owner,operation,source);
    lt48_status st=source==LT48_OK?global:source;
    x->reentry=st==LT48_OK;
    return st;
}
lt48_status lt48_pic_source_transport_compact_program_internal(lt48_pic_source_transport *owner,
    const lt48_pic_source_operation_contract *contract,const lt48_pic_source_program_request *request,
    lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!owner || !owner->prepared || !result)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_session *original=owner->session;
    lt48_status st=LT48_INVALID_ARGUMENT;
    if(!contract || !request || !owner->deferred_fpga || !owner->startup_bound ||
       request->options.started || request->options.start || request->options.runner || request->options.cancelled)
        goto done;
    if((request->flags&16) && request->voltage_variable!=owner->host.voltage_variable)goto done;
    for(unsigned i=0;i<4;i++)if(request->flags&16) {
        uint32_t voltage=request->verify_voltages[i];
        if(voltage && (voltage<owner->host.minimum_centivolts || voltage>owner->host.maximum_centivolts))
            goto done;
    }
    source_compact_program_transport x={owner,owner->startup,owner->startup_flags,request,0,0};
    lt48_pic_source_program_request bound=*request;
    bound.options.start=source_compact_program_start;bound.options.runner=source_compact_program_runner;
    bound.options.cancelled=source_compact_program_cancelled;bound.options.context=&x;
    bound.ask_erase=source_compact_program_ask;
    st=lt48_pic_source_program_execute_internal(original,contract,&bound,source_compact_program_finish,&x,result);
done:
    owner->session=original;
    if(!result->cleanup_attempted) {
        result->operation_status=st;result->cleanup_attempted=1;
        result->cleanup_status=source_wire_finish(owner,st,LT48_OK);
    }
    return st==LT48_OK?result->cleanup_status:st;
}

/* J configuration is embedded in Flash. Unlike the legacy composite program
 * route, this adapter never calls DEVINIT between Program and verification. */
typedef struct {
    lt48_pic_source_transport *owner;
    const lt48_pic_source_applet_program_plan *plan;
    lt48_pic18_j_program_result *result;
    int powered,finished;
} source_j_execution;
static lt48_status source_j_status(int status) {
    return status==LT48_PIC18_J_PROGRAM_INVALID?LT48_INVALID_ARGUMENT:
        status==LT48_PIC18_J_PROGRAM_MISMATCH?LT48_DEVICE_MISMATCH:
        status==LT48_PIC18_J_REWRITE_PROTECTED?LT48_SAFETY_REJECTED:(lt48_status)status;
}
static int source_j_erase(void *context) {
    source_j_execution *x=context;const lt48_pic_source_applet_program_plan *p=x->plan;
    lt48_pic_source_transport *owner=x->owner;
    lt48_status st=lt48_pic_source_session_applet_mode_internal(owner->session,7,1,0,p->capacity,p->stream.size_buffer);
    if(st==LT48_OK&&!x->powered)st=source_begin(owner,7,0,p->stream.reset_role,&x->powered);
    if(st==LT48_OK)st=lt48_pic_source_session_applet_callback_internal(owner->session,
        p->stream.init_role,7,0,p->capacity,p->stream.size_buffer,source_applet_runner,owner);
    if(st==LT48_OK)st=lt48_pic_source_session_invoke_current_internal(owner->session,
        p->erase_role,10,source_applet_runner,owner);
    const unsigned char *data=NULL;
    if(st==LT48_OK)st=lt48_pic_source_session_buffer_internal(owner->session,p->data_in,&data);
    return st==LT48_OK&&!data[0]?LT48_DEVICE_MISMATCH:st;
}
static int source_j_reenter(void *context) {
    source_j_execution *x=context;const lt48_pic_source_applet_program_plan *p=x->plan;
    /* Mode4 repeats physical entry without Program's host CP-buffer mutation.
     * Preserving operations begin with Read, before the erase callback. */
    lt48_status st=LT48_OK;
    if(!x->powered) {
        st=lt48_pic_source_session_applet_mode_internal(x->owner->session,4,1,0,p->capacity,p->stream.size_buffer);
        if(st==LT48_OK)st=source_begin(x->owner,4,0,p->stream.reset_role,&x->powered);
    }
    if(st!=LT48_OK)return st;
    return lt48_pic_source_session_applet_callback_internal(x->owner->session,
        p->stream.init_role,4,0,p->capacity,p->stream.size_buffer,source_applet_runner,x->owner);
}
static int source_j_transfer(source_j_execution *x,uint32_t address,unsigned char *data,size_t length,int writing) {
    const lt48_pic_source_applet_program_plan *p=x->plan;lt48_pic_source_transport *owner=x->owner;
    if(!length||length>64||(!writing&&address>=p->capacity&&
        (address<UINT32_C(0x300000)||address+length>UINT32_C(0x300008))&&
        (address<UINT32_C(0x3ffffe)||address+length>UINT32_C(0x400000))))return LT48_INVALID_ARGUMENT;
    lt48_status st=lt48_pic_source_session_applet_mode_internal(owner->session,writing?2:1,0,
        address,p->capacity,p->stream.size_buffer);
    /* The callback's physical pointer is independent of its image cursor.
     * Read only streams bytes into the caller's small readback buffer. */
    lt48_pic_map_span span={0,writing?address:0,address,(uint32_t)length,1,0};
    if(st==LT48_OK)st=lt48_pic_source_session_applet_span_internal(owner->session,&span,
        p->stream.image_buffer,p->stream.increment_buffer);
    if(st==LT48_OK)st=lt48_pic_source_session_applet_invoke_selected_internal(owner->session,10,source_applet_runner,owner);
    if(st==LT48_OK)st=lt48_pic_source_transport_stream_internal(owner,LT48_PIC_STREAM_BYTES_48,writing,data,length);
    return st;
}
static int source_j_program64(void *context,uint32_t address,const uint8_t *data) {
    return source_j_transfer(context,address,(unsigned char *)data,64,1);
}
static int source_j_read(void *context,uint32_t address,uint8_t *data,size_t length) {
    return source_j_transfer(context,address,data,length,0);
}
static int source_j_finish(void *context) {
    source_j_execution *x=context;lt48_status cleanup=LT48_OK;x->finished=1;
    if(x->powered)cleanup=source_powered_cleanup(x->owner,x->plan->stream.reset_role);
    lt48_status global=source_wire_finish(x->owner,source_j_status(x->result->operation_error),cleanup);
    return cleanup==LT48_OK?global:cleanup;
}
static int source_j_memory(source_j_execution *x,const lt48_pic18_j_program_profile *profile,
    int erase,unsigned char *output,size_t size,lt48_pic18_j_memory_result *r) {
    unsigned char *staged=malloc(size);int error=staged?LT48_OK:LT48_NO_MEMORY;
    if(!error&&erase)error=source_j_erase(x);
    if(!error)error=source_j_reenter(x);
    if(!error)error=source_j_read(x,0x300000,r->configuration,8);
    if(!error)r->protected_read=!(r->configuration[1]&4);
    unsigned char id[2]={0};
    if(!error)error=source_j_read(x,0x3ffffe,id,2);
    if(!error){r->device_id=(uint16_t)(id[0]|((unsigned)id[1]<<8));r->id_observed=1;}
    for(unsigned b=0;!error&&erase&&b<8;b++)if(r->configuration[b]!=profile->mirror_masks[b]) {
        error=LT48_DEVICE_MISMATCH;r->transfer.first_mismatch=0x300000+b;
    }
    for(uint32_t a=0;!error&&a<size;a+=64) {
        error=source_j_read(x,a,staged+a,64);
        if(!error)r->received_bytes+=64;
    }
    if(!error&&erase)for(uint32_t a=0;a<size;a++)if(staged[a]!=255) {
        error=LT48_DEVICE_MISMATCH;r->transfer.first_mismatch=a;break;
    }
    if(!error&&!erase) {
        if(r->protected_read) {
            r->configuration_from_mirrors=1;
            for(unsigned b=0;b<8;b++)staged[size-8+b]=(uint8_t)(((r->configuration[b]&profile->mirror_masks[b])|
                (255u&~profile->mirror_masks[b])|profile->fixed_one[b])&~profile->fixed_zero[b]);
        }else for(unsigned b=0;!error&&b<8;b++)if((staged[size-8+b]&profile->mirror_masks[b])!=r->configuration[b]) {
            error=LT48_DEVICE_MISMATCH;r->transfer.first_mismatch=(uint32_t)size-8+b;
        }
    }
    r->transfer.operation_error=error;r->transfer.cleanup_error=source_j_finish(x);
    if(!error&&!r->transfer.cleanup_error)memcpy(output,staged,size);
    free(staged);return error?error:r->transfer.cleanup_error;
}
static lt48_status source_j_execute(lt48_pic_source_transport *owner,
    const lt48_pic_source_applet_program_plan *p,const lt48_pic18_j_program_profile *profile,
    const unsigned char *image,size_t size,lt48_pic18_j_program_result *result,
    unsigned rewrite,uint32_t first,const unsigned char *patch,size_t patch_size,
    unsigned char *backup,unsigned char *replacement,lt48_pic18_j_rewrite_result *rewrite_result,
    lt48_pic18_j_memory_result *memory_result) {
    if(result)*result=(lt48_pic18_j_program_result){LT48_INVALID_ARGUMENT,LT48_OK,UINT32_MAX,0};
    if(!owner||!owner->prepared||!result)return LT48_INVALID_ARGUMENT;
    lt48_status st=LT48_INVALID_ARGUMENT;lt48_pic_source_session *probe=NULL,*original=owner->session;
    if(!p||!profile||!image||size!=profile->code_bytes||p->capacity!=profile->code_bytes||
       p->stream.size_buffer==p->stream.image_buffer||p->stream.size_buffer==p->stream.increment_buffer||
       p->stream.image_buffer==p->stream.increment_buffer||p->stream.protocol!=LT48_PIC_STREAM_BYTES_48)goto done;
    switch(size){case 8192:case 16384:case 32768:case 49152:case 65536:case 98304:case 131072:break;default:goto done;}
    if(owner->deferred_fpga&&!owner->startup_bound){st=LT48_UNSUPPORTED;goto done;}
    uint16_t roles[]={owner->host.poweron_role,owner->host.poweroff_role,
        p->stream.init_role,p->stream.reset_role,p->erase_role};
    for(unsigned i=0;i<5;i++)if(!lt48_pic_source_session_has_callback_internal(original,roles[i])){st=LT48_UNSUPPORTED;goto done;}
    st=lt48_pic_source_session_clone_internal(original,&probe);
    if(st!=LT48_OK)goto done;
    lt48_pic_source_state staged;st=lt48_pic_source_session_state_internal(probe,&staged);
    if(st==LT48_OK&&!rewrite){staged.image=image;staged.image_size=size;st=lt48_pic_source_session_restore_internal(probe,&staged);}
    if(st==LT48_OK&&staged.image_size!=size)st=LT48_INVALID_ARGUMENT;
    for(unsigned mode=1;st==LT48_OK&&mode<=2;mode++) {
        st=lt48_pic_source_session_applet_mode_internal(probe,(uint16_t)mode,0,0,p->capacity,p->stream.size_buffer);
        uint16_t role;if(st==LT48_OK)st=lt48_pic_source_session_applet_stream_role_internal(probe,&role);
        if(st==LT48_OK&&!lt48_pic_source_session_has_callback_internal(probe,role))st=LT48_UNSUPPORTED;
    }
    if(st==LT48_OK) {
        const unsigned char *data=NULL;
        st=lt48_pic_source_session_buffer_internal(probe,p->data_in,&data);
        lt48_pic_map_span first={0,0,0,64,1,0};
        /* Generic scratch access permits buffers outside this bound host
         * frame. Validate DataIn against the frame before destructive erase. */
        if(st==LT48_OK)st=lt48_pic_source_session_applet_span_internal(probe,&first,
            p->data_in,p->stream.increment_buffer);
        if(st==LT48_OK)st=lt48_pic_source_session_applet_span_internal(probe,&first,
            p->stream.image_buffer,p->stream.increment_buffer);
    }
    if(st!=LT48_OK)goto done;
    owner->session=probe;
    source_j_execution x={owner,p,result,0,0};
    const lt48_pic18_j_program_io io={source_j_erase,source_j_reenter,source_j_program64,source_j_read,source_j_finish};
    if(memory_result)st=(lt48_status)source_j_memory(&x,profile,rewrite==4,(unsigned char *)image,size,memory_result);
    else st=source_j_status(rewrite?lt48_pic18_j_rewrite(profile,&io,&x,rewrite,first,patch,patch_size,
        backup,replacement,size,rewrite_result):lt48_pic18_j_program_whole(profile,&io,&x,image,size,result));
    result->operation_error=source_j_status(result->operation_error);
    if(!x.finished)result->cleanup_error=source_wire_finish(owner,st,LT48_OK);
    owner->session=original;
    lt48_pic_source_session_close_internal(probe);
    return st==LT48_OK?(lt48_status)result->cleanup_error:st;
done:
    result->operation_error=st;result->cleanup_error=source_wire_finish(owner,st,LT48_OK);
    lt48_pic_source_session_close_internal(probe);return st;
}

lt48_status lt48_pic_source_transport_j_program_internal(lt48_pic_source_transport *owner,
    const lt48_pic_source_applet_program_plan *p,const lt48_pic18_j_program_profile *profile,
    const unsigned char *image,size_t size,lt48_pic18_j_program_result *result) {
    return source_j_execute(owner,p,profile,image,size,result,0,0,NULL,0,NULL,NULL,NULL,NULL);
}
lt48_status lt48_pic_source_transport_j_rewrite_internal(lt48_pic_source_transport *owner,
    const lt48_pic_source_applet_program_plan *p,const lt48_pic18_j_program_profile *profile,
    unsigned operation,uint32_t first,const unsigned char *patch,size_t patch_size,
    unsigned char *backup,unsigned char *replacement,size_t size,lt48_pic18_j_rewrite_result *result) {
    if(!result)return LT48_INVALID_ARGUMENT;
    memset(result,0,sizeof(*result));
    if(operation!=LT48_PIC18_J_REWRITE_SECURE&&operation!=LT48_PIC18_J_REWRITE_PATCH) {
        result->program.operation_error=LT48_INVALID_ARGUMENT;result->program.first_mismatch=UINT32_MAX;
        if(owner&&owner->prepared)result->program.cleanup_error=source_wire_finish(owner,LT48_INVALID_ARGUMENT,LT48_OK);
        return LT48_INVALID_ARGUMENT;
    }
    return source_j_execute(owner,p,profile,replacement,size,&result->program,
        operation,first,patch,patch_size,backup,replacement,result,NULL);
}

lt48_status lt48_pic_source_transport_j_memory_internal(lt48_pic_source_transport *owner,
    const lt48_pic_source_applet_program_plan *p,const lt48_pic18_j_program_profile *profile,
    int erase,unsigned char *image,size_t size,lt48_pic18_j_memory_result *result) {
    if(!result)return LT48_INVALID_ARGUMENT;
    memset(result,0,sizeof(*result));
    if(erase!=0&&erase!=1)return LT48_INVALID_ARGUMENT;
    return source_j_execute(owner,p,profile,image,size,&result->transfer,erase?4:3,0,NULL,0,NULL,NULL,NULL,result);
}

lt48_status lt48_pic_source_transport_compact_owner_internal(lt48_pic_source_transport *owner,
 lt48_pic_source_transport_compact_owner execute,void *context,lt48_pic_source_operation_result *result) {
 if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
 if(!owner||!owner->prepared||!result)return LT48_INVALID_ARGUMENT;
 lt48_status st=LT48_INVALID_ARGUMENT;
 if(execute&&!owner->deferred_fpga&&!owner->startup_bound)
  st=execute(context,owner->session,&owner->host,source_wire_finish,owner,source_wire_cancelled,result);
 else if(execute)st=LT48_UNSUPPORTED;
 if(owner->prepared) {
  /* The owner can reject before startup. Preparation still belongs to us. */
  result->operation_status=st;result->cleanup_attempted=1;
  result->cleanup_status=source_wire_finish(owner,st,LT48_OK);
  if(st==LT48_OK)st=result->cleanup_status;
 }
 return st;
}
