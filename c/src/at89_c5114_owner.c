#include "at89_c5114_owner.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
lt48_status lt48_c5114_open(const lt48_installation *x,size_t index,lt48_c5114_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status status=lt48_installation_resolve_device(x,index,&spec);
    if(status!=LT48_OK)return status;
    if(spec.catalogue.device_class!=2018 || strcmp(spec.definition_stem,"GAF____") || spec.definition_resource!=0x116)return LT48_UNSUPPORTED;
    return lt48_at89_tail_open(x,index,out);
}
void lt48_c5114_close(lt48_c5114_resources *r){lt48_at89_tail_close(r);}
lt48_status lt48_c5114_validate(const lt48_at89_tail_resources *r,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r)return LT48_INVALID_ARGUMENT;
    if(r->definition!=0x116 || r->family!=161 || r->pinmap!=71 || r->primary_count!=4096)return LT48_UNSUPPORTED;
    if((op<1 || op>8) && op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
    if(!length || start>=4096 || length>4096-start)return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE ||
       op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)
        if(start || length!=4096)return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_CONFIGURATION) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=7)return LT48_INVALID_ARGUMENT;
        if(image[0]&~63u)return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<6;i++)if((image[0]&(1u<<i)) && image[i+1]>(i<5?1:2))return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_SECURE) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=1 || !image[0] || image[0]>2)return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE) {
        if(!image || size!=length)return LT48_INVALID_ARGUMENT;
    }
    return LT48_OK;
}
typedef struct c5114_owner {
    lt48_c5114_transport transport;
    lt48_c5114_frame frame;
    lt48_c5114_erase_state erase;
    lt48_m37_id_decision_fn decide;void *decision_context;int ignore_id;
} c5114_owner;
static void voltage(c5114_owner *s,unsigned cv) {
    for(size_t i=0;i<1;i++)if(s->transport.variables[i].variable_id==0)s->transport.variables[i].value=cv;
}
static lt48_status step_owner(void *v,unsigned op,lt48_c5114_frame *frame) {
    c5114_owner *s=v;
    for(unsigned attempt=0;attempt<64;attempt++) {
        if(op==2 && s->ignore_id)for(size_t i=0;i<frame->config_count;i++)
            if(frame->config[i].object_id==-1 && frame->config[i].category==0 && frame->config[i].field==1)frame->config[i].value=0;
        lt48_status status=lt48_c5114_transport_step(&s->transport,op,frame);
        if(op!=2 || status!=LT48_DEVICE_MISMATCH || !s->decide)return status;
        /* The pinned W163 identifies four extra-row identity failures:
         * bytes at 30h/31h/60h/61h. The event diagnostic holds the driven
         * address; buffer10 holds the capture. Other failures cannot be ignored. */
        uint32_t address=frame->id_address;
        unsigned expected=address==0x30?0x58:address==0x31?0xd7:address==0x60?0xae:address==0x61?0xff:256;
        unsigned actual=frame->buffers[10][0];
        if(expected>255 || actual==expected || s->ignore_id || frame->id_failure!=(address==0x30?5u:6u))return status;
        lt48_m37_id_choice choice=s->decide(s->decision_context,0,(uint16_t)expected,0,(uint16_t)actual);
        if(choice==LT48_M37_ID_ABORT)return status;
        if(choice!=LT48_M37_ID_RETRY && choice!=LT48_M37_ID_IGNORE)return LT48_INVALID_ARGUMENT;
        status=lt48_c5114_transport_step(&s->transport,3,frame);
        if(status!=LT48_OK)return status;
        if(choice==LT48_M37_ID_IGNORE)s->ignore_id=1;
    }
    return LT48_DEVICE_MISMATCH;
}
static lt48_status settings_owner(c5114_owner *s,const unsigned char *desired,unsigned char *actual) {
    voltage(s,300);lt48_status reset;
    return lt48_c5114_settings_sequence(s->transport.resources,step_owner,s,&s->frame,desired,actual,&reset);
}
static lt48_status read_owner(c5114_owner *s,uint32_t start,size_t n,unsigned char *data,unsigned cv) {
    voltage(s,cv);lt48_status reset;size_t completed;
    return lt48_c5114_memory_sequence(s->transport.resources,step_owner,s,&s->frame,0,start,n,NULL,data,&completed,&reset);
}
static lt48_status compare_owner(lt48_serial_page_result *out,uint32_t start,
    const unsigned char *actual,const unsigned char *expected,size_t n) {
    for(size_t i=0;i<n;i++)if(actual[i]!=(expected?expected[i]:255)) {
        if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
        out->mismatch_count++;
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status cleanup_owner(void *v,unsigned phase) {
    c5114_owner *s=v;const lt48_at89_tail_resources *r=s->transport.resources;
    if(phase==0)return step_owner(s,3,&s->frame);
    if(phase==1) {
        if(!s->erase.restore_pending)return LT48_OK;
        unsigned char actual[6];lt48_status status=LT48_OK;
        if(s->erase.erase_incomplete) {
            /* Repeat an interrupted, already-authorized erase before
             * restoring the saved configuration. Recovery initialization
             * bypasses lock and identity gates; it cannot prompt the user. */
            voltage(s,300);memset(s->frame.buffers,0,sizeof s->frame.buffers);s->frame.mode=7;
            s->frame.config_count=1;s->frame.config[0]=(lt48_runtime_configuration_value){-1,0,1,0};
            status=step_owner(s,2,&s->frame);
            if(status==LT48_OK)status=step_owner(s,10,&s->frame);
            lt48_status reset=step_owner(s,3,&s->frame);
            if(status==LT48_OK)status=reset;
            if(status==LT48_OK)s->erase.erase_incomplete=0;
        }
        if(status==LT48_OK)status=settings_owner(s,s->erase.settings,actual);
        if(status==LT48_OK)s->erase.restore_pending=0;
        return status;
    }
    if(phase==2)return step_owner(s,1,&s->frame);
    return lt48_owner_load_fpga_internal(s->transport.wire,r->idle,r->idle_size);
}
lt48_status lt48_c5114_execute(const lt48_at89_tail_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t image_size,
    unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,
    unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,lt48_serial_page_result *out) {
    if(!out || !api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_c5114_validate(r,op,start,length,image,image_size);
    if(status==LT48_OK && (margin_type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL &&
       margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)))status=LT48_INVALID_ARGUMENT;
    if(status==LT48_OK && ((op==LT48_OPERATION_READ && (!read || read_size<length)) ||
       (op==LT48_OPERATION_CONFIGURATION && (!read || read_size<6))))status=LT48_BUFFER_TOO_SMALL;
    if(status!=LT48_OK){out->operation_status=status;return status;}
    if(api->cancelled && api->cancelled(context)){out->operation_status=LT48_CANCELLED;out->cleanup_completed=1;return LT48_CANCELLED;}
    unsigned char *actual=malloc(4096),saved[6],desired[6];
    if(!actual){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
    c5114_owner s={0};s.transport.resources=r;memcpy(s.transport.variables,r->vars,sizeof s.transport.variables);
    s.frame.mode=1;s.decide=decide;s.decision_context=decision_context;
    status=lt48_runtime_transport_session_create(api,context,&s.transport.wire);
    int started=status==LT48_OK;
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s.transport.wire,10000,1);
    if(status==LT48_OK)status=lt48_owner_load_fpga_internal(s.transport.wire,r->idle,r->idle_size);
    if(status==LT48_OK)status=step_owner(&s,0,&s.frame);
    for(unsigned i=0;status==LT48_OK && i<3;i++)status=lt48_runtime_transport_session_status(s.transport.wire,1);
    const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.transport.wire,busy,sizeof busy);
    if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s.transport.wire);
    int setup_uncertain=started && status!=LT48_OK;
    if(status==LT48_OK)status=settings_owner(&s,NULL,saved);
    int programming=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if(status==LT48_OK && (op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE)) {
        memcpy(desired,saved,6);
        if(op==LT48_OPERATION_SECURE){unsigned lock=image?image[0]:2;if(lock>desired[5])desired[5]=(unsigned char)lock;}
        else if(image)for(unsigned i=0;i<6;i++)if(image[0]&(1u<<i))desired[i]=image[i+1];
        int changed=memcmp(desired,saved,5);
        if(desired[5]<saved[5] || (changed && saved[5]))status=LT48_SAFETY_REJECTED;
        if(status==LT48_OK && memcmp(desired,saved,6))status=settings_owner(&s,desired,actual);
        if(status==LT48_OK)memcpy(saved,desired,6);
    } else if(status==LT48_OK) {
        if(op==LT48_OPERATION_ERASE || op==LT48_OPERATION_PROGRAM) {
            voltage(&s,300);lt48_status reset;
            status=lt48_c5114_erase_sequence(r,step_owner,&s,&s.frame,&s.erase,&reset);
            if(status==LT48_OK)memcpy(saved,s.erase.settings,6);
            else if(s.erase.mismatch_address!=UINT32_MAX){out->first_mismatch=s.erase.mismatch_address;out->mismatch_count=1;}
        } else if(saved[5]>=2 || (programming && saved[5]))status=LT48_SAFETY_REJECTED;
        if(status==LT48_OK && programming) {
            /* Execute the original page procedure, followed by independent
             * margin reads. PROGRAM first performs the original erase. */
            voltage(&s,300);size_t completed;lt48_status reset;
            if(status==LT48_OK)status=lt48_c5114_memory_sequence(r,step_owner,&s,&s.frame,1,0,4096,image,NULL,&completed,&reset);
            unsigned cv[]={margin_type?270u:285u,margin_type?330u:315u};
            for(unsigned i=0;status==LT48_OK && i<2;i++) {
                status=read_owner(&s,0,4096,actual,cv[i]);
                if(status==LT48_OK)status=compare_owner(out,0,actual,image,4096);
            }
        } else if(status==LT48_OK && op!=LT48_OPERATION_ERASE) {
            unsigned cv[]={margin_type?270u:285u,margin_type?330u:315u};
            unsigned passes=op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_ALL?2:1;
            if(op!=LT48_OPERATION_VERIFY || margin==LT48_VERIFY_MARGIN_NOMINAL)cv[0]=300;
            else if(margin==LT48_VERIFY_MARGIN_HIGH)cv[0]=cv[1];
            for(unsigned i=0;status==LT48_OK && i<passes;i++) {
                status=read_owner(&s,start,length,actual,cv[i]);
                if(status==LT48_OK && op!=LT48_OPERATION_READ)status=compare_owner(out,start,actual,op==LT48_OPERATION_BLANK_CHECK?NULL:image,length);
            }
        }
    }
    if(status==LT48_OK)status=settings_owner(&s,NULL,desired);
    if(status==LT48_OK && memcmp(saved,desired,6))status=LT48_DEVICE_MISMATCH;
    out->operation_status=status;
    if(started) {
        s.decide=NULL; /* Cleanup cannot wait for an interactive ID decision. */
        const unsigned phases[]={0,1,2,3};
        status=lt48_owner_finish_internal(api,context,&s.transport.wire,cleanup_owner,&s,phases,4,status,&out->cleanup_status,&out->cleanup_completed);
    } else {lt48_runtime_transport_session_close(s.transport.wire);out->cleanup_completed=1;}
    if(setup_uncertain){out->cleanup_status=LT48_CLEANUP_FAILED;out->cleanup_completed=0;}
    if(status==LT48_OK) {
        if(op==LT48_OPERATION_READ)memcpy(read,actual,length);
        if(op==LT48_OPERATION_CONFIGURATION)memcpy(read,desired,6);
        out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?6:length;
    }
    free(actual);return status;
}
