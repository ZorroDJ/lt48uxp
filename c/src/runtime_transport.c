#include <labtool48/labtool48.h>
#include "internal.h"
#include "../../NewDeviceLib/sources/pic-common/controller_status.h"
#include <stdlib.h>
#include <time.h>

struct lt48_runtime_transport_session {
    lt48_protocol_plan *plan;
    lt48_protocol_transport transport;
    void *context;
    size_t executed;
    int failed;
    int watchdog_valid;
    uint32_t watchdog_milliseconds;
};

lt48_status lt48_runtime_transport_session_create(
    const lt48_protocol_transport *transport, void *context,
    lt48_runtime_transport_session **out_session
) {
    lt48_runtime_transport_session *session;
    lt48_status status;
    if (!transport || !transport->write || !transport->read || !out_session)
        return LT48_INVALID_ARGUMENT;
    *out_session = NULL;
    session = calloc(1, sizeof(*session));
    if (!session) return LT48_NO_MEMORY;
    status = lt48_protocol_plan_create(&session->plan);
    if (status != LT48_OK) { free(session); return status; }
    session->transport = *transport;
    session->context = context;
    *out_session = session;
    return LT48_OK;
}

void lt48_runtime_transport_session_close(lt48_runtime_transport_session *session) {
    if (!session) return;
    lt48_protocol_plan_close(session->plan);
    free(session);
}

static lt48_status execute_pending(
    lt48_runtime_transport_session *session, lt48_runtime_capture *capture,
    size_t *out_captures
) {
    size_t total = lt48_protocol_plan_transaction_count(session->plan);
    size_t completed = 0;
    lt48_status status = lt48_protocol_plan_execute_range(
        session->plan, session->executed, total - session->executed,
        &session->transport, session->context, capture, 1,
        out_captures, &completed);
    session->executed += completed;
    if(status==LT48_OK && session->executed==total && total>=1024u) {
        status=lt48_protocol_plan_discard_completed_internal(session->plan);
        if(status==LT48_OK)session->executed=0;
    }
    if (status != LT48_OK) session->failed = 1;
    return status;
}

/* Original 443e80 uses 56-ms units and transport 4975c0 command 04.
 * Preserve the ready polling sequence, but propagate timeout/USB failure. */
static lt48_status poll_pin(lt48_runtime_transport_session *s,const lt48_runtime_event *e,lt48_runtime_capture *capture) {
    if(e->argument_count!=4 || e->arguments[0]>255 || e->arguments[1]>1 || e->arguments[3]>=70)return LT48_PROTOCOL_ERROR;
    struct timespec begin,now;if(timespec_get(&begin,TIME_UTC)!=TIME_UTC)return LT48_RUNTIME_ERROR;
    for(size_t attempts=0;attempts<100000u;attempts++) {
        size_t n=0;lt48_status status=lt48_protocol_plan_sample_pin_internal(s->plan,(uint8_t)(e->arguments[0]|2u),(uint8_t)e->arguments[3]);
        if(status==LT48_OK)status=execute_pending(s,capture,&n);
        if(status!=LT48_OK)return status;
        if(n!=1 || capture->length!=1)return LT48_PROTOCOL_ERROR;
        if((capture->values[0]&1u)==e->arguments[1]){capture->values[0]=1;return LT48_OK;}
        if(timespec_get(&now,TIME_UTC)!=TIME_UTC)return LT48_RUNTIME_ERROR;
        double elapsed=(double)(now.tv_sec-begin.tv_sec)*1000.0+(double)(now.tv_nsec-begin.tv_nsec)/1000000.0;
        if(elapsed<0 || elapsed>(double)e->arguments[2])return LT48_TIMEOUT;
    }
    return LT48_TIMEOUT;
}

lt48_status lt48_runtime_transport_session_run_with_applets(
    lt48_runtime_transport_session *session, lt48_runtime_machine *machine,
    const lt48_applet_address *applets, size_t applet_count,
    lt48_runtime_result **out_result
) {
    lt48_status status;
    lt48_runtime_machine_state state;
    if (!session || !machine || !out_result) return LT48_INVALID_ARGUMENT;
    *out_result = NULL;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (lt48_runtime_machine_get_state(machine) != LT48_RUNTIME_MACHINE_READY)
        return LT48_INVALID_ARGUMENT;
    for (;;) {
        size_t first, count, captures = 0;int polled=0;
        lt48_runtime_capture capture = {0};
        status = lt48_runtime_machine_advance(machine, &state);
        if (status != LT48_OK) break;
        lt48_runtime_machine_last_event_range(machine, &first, &count);
        for (size_t index = first; index < first + count; ++index) {
            lt48_runtime_event event;
            status = lt48_runtime_get_event(lt48_runtime_machine_result(machine), index, &event);
            if (status != LT48_OK) break;
            if (event.kind == LT48_RUNTIME_WAVE_WATCHDOG) {
                if (event.argument_count != 2 || event.arguments[0] > UINT32_MAX ||
                    (event.arguments[1] != 1 && event.arguments[1] != 2)) {
                    status = LT48_PROTOCOL_ERROR; break;
                }
                status = lt48_runtime_transport_session_wave_watchdog(session,
                    (uint32_t)event.arguments[0], event.arguments[1] == 1);
            } else if(event.kind==LT48_RUNTIME_CONTROLLER_PIN_POLL) {
                status=poll_pin(session,&event,&capture);captures=1;polled=1;
            } else {
                status = lt48_protocol_plan_apply_runtime_range(session->plan,
                    lt48_runtime_machine_result(machine), index, 1, applets, applet_count);
            }
            if (status != LT48_OK) break;
        }
        if (status != LT48_OK) break;
        if(!polled)status = execute_pending(session, &capture, &captures);
        if (status != LT48_OK) break;
        if (state == LT48_RUNTIME_MACHINE_FINISHED) {
            if (captures) { status = LT48_PROTOCOL_ERROR; break; }
            return lt48_runtime_machine_take_result(machine, out_result);
        }
        if(state==LT48_RUNTIME_MACHINE_READY&&!captures)continue;
        if (state != LT48_RUNTIME_MACHINE_AWAITING_CAPTURE || captures != 1) {
            status = LT48_PROTOCOL_ERROR; break;
        }
        {
            uint8_t buffer;
            size_t length;
            status = lt48_runtime_machine_capture_request(machine, &buffer, &length);
            if (status != LT48_OK) break;
            if (capture.buffer != buffer || capture.length != length) {
                status = LT48_PROTOCOL_ERROR; break;
            }
            status = lt48_runtime_machine_supply_capture_bytes(
                machine, capture.values, capture.length);
            if (status != LT48_OK) break;
        }
    }
    session->failed = 1;
    return status;
}

lt48_status lt48_runtime_transport_session_flush(lt48_runtime_transport_session *session) {
    lt48_runtime_capture capture = {0};
    size_t captures = 0;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    status = lt48_protocol_plan_flush(session->plan);
    if (status == LT48_OK) status = execute_pending(session, &capture, &captures);
    if (status == LT48_OK && captures) status = LT48_PROTOCOL_ERROR;
    if (status != LT48_OK) session->failed = 1;
    return status;
}

lt48_status lt48_runtime_transport_session_status(
    lt48_runtime_transport_session *session, int settle
) {
    lt48_runtime_capture capture = {0};
    size_t captures = 0;
    lt48_status status;
    if (!session || (settle != 0 && settle != 1)) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    status = lt48_protocol_plan_settle_status(session->plan, settle);
    if (status == LT48_OK) status = execute_pending(session, &capture, &captures);
    if (status == LT48_OK && captures) status = LT48_PROTOCOL_ERROR;
    if (status != LT48_OK) session->failed = 1;
    return status;
}
lt48_status lt48_runtime_transport_session_controller_status_internal(
    lt48_runtime_transport_session *session,int settle,uint16_t *application_status
) {
    if(!session || !application_status || (settle!=0 && settle!=1))return LT48_INVALID_ARGUMENT;
    if(session->failed){*application_status=1;return LT48_SAFETY_REJECTED;}
    lt48_runtime_capture capture={0};size_t captures=0;
    lt48_status st=lt48_protocol_plan_status_capture_internal(session->plan,settle);
    if(st==LT48_OK)st=execute_pending(session,&capture,&captures);
    if(st==LT48_OK && (captures!=1 || capture.length!=3))st=LT48_PROTOCOL_ERROR;
    if(st==LT48_OK)st=lt48_pic_controller_status_decode(capture.values,capture.length,application_status);
    if(st!=LT48_OK){*application_status=1;session->failed=1;}
    return st;
}

lt48_status lt48_runtime_transport_session_controller_write(
    lt48_runtime_transport_session *session, const unsigned char *bytes, size_t size
) {
    lt48_runtime_capture capture = {0};
    size_t captures = 0;
    lt48_status status;
    if (!session || !bytes || !size || size > 65536) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    status = lt48_protocol_plan_controller_write(session->plan, bytes, size);
    if (status == LT48_OK) status = execute_pending(session, &capture, &captures);
    if (status == LT48_OK && captures) status = LT48_PROTOCOL_ERROR;
    if (status != LT48_OK) session->failed = 1;
    return status;
}
lt48_status lt48_runtime_transport_session_controller_capture_internal(
    lt48_runtime_transport_session *s,unsigned char *data,unsigned count) {
    if(!s||!data||!count||count>32)return LT48_INVALID_ARGUMENT;
    if(s->failed)return LT48_SAFETY_REJECTED;
    lt48_runtime_capture capture={0};size_t captures=0;
    lt48_status status=lt48_protocol_plan_controller_capture_internal(s->plan,count);
    if(status==LT48_OK)status=execute_pending(s,&capture,&captures);
    if(status==LT48_OK&&(captures!=1||capture.length!=count))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK)for(unsigned i=0;i<count;i++)data[i]=capture.values[i];
    else s->failed=1;
    return status;
}


lt48_status lt48_runtime_transport_session_wave_watchdog(
    lt48_runtime_transport_session *session, uint32_t milliseconds,
    int interpreter_active
) {
    lt48_runtime_capture capture = {0};
    size_t captures = 0;
    lt48_status status;
    if (!session || (interpreter_active != 0 && interpreter_active != 1))
        return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    status = lt48_protocol_plan_wave_watchdog(session->plan, milliseconds,
        session->watchdog_valid, session->watchdog_milliseconds, interpreter_active);
    if (status == LT48_OK) status = execute_pending(session, &capture, &captures);
    if (status == LT48_OK && captures) status = LT48_PROTOCOL_ERROR;
    if (status != LT48_OK) {
        session->watchdog_valid = 0;
        session->failed = 1;
        return status;
    }
    /* Cache only after successful writes. The original caches failed sends. */
    session->watchdog_milliseconds = milliseconds;
    session->watchdog_valid = 1;
    return LT48_OK;
}

lt48_status lt48_runtime_transport_session_run(lt48_runtime_transport_session *s,
    lt48_runtime_machine *m, lt48_runtime_result **r) {
    return lt48_runtime_transport_session_run_with_applets(s,m,NULL,0,r);
}

lt48_status lt48_runtime_transport_error_indicator_internal(
    const lt48_protocol_transport *transport, void *context) {
    lt48_runtime_transport_session *fresh = NULL;
    const unsigned char error[] = {0x40,2};
    lt48_status status=lt48_runtime_transport_session_create(transport,context,&fresh);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(fresh,error,sizeof(error));
    if(status==LT48_OK)status=lt48_runtime_transport_session_flush(fresh);
    lt48_runtime_transport_session_close(fresh);
    return status;
}
