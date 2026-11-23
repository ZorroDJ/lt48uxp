#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <string.h>

lt48_status lt48_owner_rez_open_internal(const char *path, const char *sha, lt48_rez **out) {
    if (!path || !sha || strlen(sha) != 64 || !out) return LT48_INVALID_ARGUMENT;
    *out = NULL;
    lt48_rez *rez = NULL;
    lt48_status st = lt48_rez_open(path, &rez);
    unsigned char digest[32]; char hex[65];
    if (st == LT48_OK) st = lt48_rez_sha256(rez, digest);
    if (st == LT48_OK) {
        for (unsigned i = 0; i < 32; i++) snprintf(hex + i*2, 3, "%02x", digest[i]);
        if (strcmp(hex, sha)) st = LT48_SAFETY_REJECTED;
    }
    if (st != LT48_OK) lt48_rez_close(rez); else *out = rez;
    return st;
}

lt48_status lt48_owner_load_fpga_internal(lt48_runtime_transport_session *wire,
    const unsigned char *data, size_t size) {
    if (!wire || !data || !size) return LT48_INVALID_ARGUMENT;
    const unsigned char head[] = {0x38,0xaa,0x55}, select[] = {0x0f,0x0f};
    lt48_status st = lt48_runtime_transport_session_controller_write(wire, head, sizeof(head));
    if (st == LT48_OK) st = lt48_runtime_transport_session_controller_write(wire, data, size);
    if (st == LT48_OK) st = lt48_runtime_transport_session_controller_write(wire, select, sizeof(select));
    return st;
}

lt48_status lt48_owner_load_applet_internal(lt48_runtime_transport_session *wire,
    uint16_t address, const unsigned char *data, size_t size) {
    if (!wire || !data || !size || size > 65536u - address) return LT48_INVALID_ARGUMENT;
    size_t last = size - 1;
    const unsigned char head[] = {0x39,0xaa,0x55,(unsigned char)address,
        (unsigned char)(address>>8),(unsigned char)last,(unsigned char)(last>>8)};
    lt48_status st = lt48_runtime_transport_session_controller_write(wire, head, sizeof(head));
    return st == LT48_OK ? lt48_runtime_transport_session_controller_write(wire, data, size) : st;
}

static lt48_status owner_run(lt48_runtime_transport_session *wire,
    const lt48_compiled_wave *wave, const lt48_pin_map *pins,
    const lt48_compile_variable *vars, size_t var_count, const lt48_runtime_options *options,
    const lt48_applet_address *applets, size_t applet_count, unsigned char buffers[32][32],
    lt48_runtime_configuration_value *config, size_t config_capacity, size_t *config_count,
    uint32_t volts[4], lt48_compile_variable *updated, lt48_runtime_result **diagnostic) {
    if(diagnostic)*diagnostic=NULL;
    lt48_runtime_machine *machine = NULL; lt48_runtime_result *result = NULL;
    lt48_status st = lt48_runtime_machine_create(wave, pins, vars, var_count, options, &machine);
    if (st == LT48_OK) st = lt48_runtime_transport_session_run_with_applets(wire, machine, applets, applet_count, &result);
    if (st == LT48_OK) {
        const unsigned char *p; size_t n;
        const lt48_runtime_configuration_value *c;
        for (unsigned i = 0; i < 32; i++) {
            st = lt48_runtime_get_buffer(result, i, &p, &n);
            if (st != LT48_OK || n != 32) { st = LT48_RUNTIME_ERROR; break; }
            memcpy(buffers[i], p, 32);
        }
        if (st == LT48_OK) {
            st = lt48_runtime_get_configuration(result, &c, &n);
            if (st == LT48_OK && n > config_capacity) st = LT48_RUNTIME_ERROR;
            if (st == LT48_OK) { memcpy(config, c, n*sizeof(*c)); *config_count = n; }
        }
        for (unsigned i = 0; st == LT48_OK && i < 4; i++) st = lt48_runtime_get_voltage(result, i, &volts[i]);
        for (size_t i = 0; st == LT48_OK && i < lt48_runtime_event_count(result); i++) {
            lt48_runtime_event event;
            st = lt48_runtime_get_event(result, i, &event);
            if (st == LT48_OK && (event.kind == LT48_RUNTIME_RESULT_LATCH ||
                (event.kind == LT48_RUNTIME_RETURN_STATUS && !event.arguments[0]))) st = LT48_DEVICE_MISMATCH;
        }
    }
    for(size_t i=0;st==LT48_OK&&updated&&i<var_count;i++)st=lt48_runtime_get_variable(result,vars[i].variable_id,&updated[i]);
    if(diagnostic)*diagnostic=result;else lt48_runtime_result_close(result);
    lt48_runtime_machine_close(machine);
    return st;
}
lt48_status lt48_owner_run_internal(lt48_runtime_transport_session *wire,
    const lt48_compiled_wave *wave, const lt48_pin_map *pins,
    const lt48_compile_variable *vars, size_t var_count, const lt48_runtime_options *options,
    const lt48_applet_address *applets, size_t applet_count, unsigned char buffers[32][32],
    lt48_runtime_configuration_value *config, size_t config_capacity, size_t *config_count,
    uint32_t volts[4]) {return owner_run(wire,wave,pins,vars,var_count,options,applets,applet_count,buffers,config,config_capacity,config_count,volts,NULL,NULL);}

lt48_status lt48_owner_run_mutable_internal(lt48_runtime_transport_session *wire,
    const lt48_compiled_wave *wave, const lt48_pin_map *pins,
    lt48_compile_variable *vars, size_t var_count, const lt48_runtime_options *options,
    const lt48_applet_address *applets, size_t applet_count, unsigned char buffers[32][32],
    lt48_runtime_configuration_value *config, size_t config_capacity, size_t *config_count,
    uint32_t volts[4]) {return owner_run(wire,wave,pins,vars,var_count,options,applets,applet_count,buffers,config,config_capacity,config_count,volts,vars,NULL);}

lt48_status lt48_owner_run_result_internal(lt48_runtime_transport_session *wire,
    const lt48_compiled_wave *wave, const lt48_pin_map *pins,
    const lt48_compile_variable *vars, size_t var_count, const lt48_runtime_options *options,
    const lt48_applet_address *applets, size_t applet_count, unsigned char buffers[32][32],
    lt48_runtime_configuration_value *config, size_t config_capacity, size_t *config_count,
    uint32_t volts[4],lt48_runtime_result **diagnostic) {
    if(!diagnostic)return LT48_INVALID_ARGUMENT;
    return owner_run(wire,wave,pins,vars,var_count,options,applets,applet_count,buffers,
        config,config_capacity,config_count,volts,NULL,diagnostic);
}

lt48_status lt48_owner_finish_fpga_internal(const lt48_protocol_transport *api, void *context,
    lt48_runtime_transport_session **wire, lt48_owner_cleanup_wave_internal run, void *owner,
    const unsigned *prefix, size_t count,const unsigned char *idle,size_t idle_size,
    lt48_status operation, lt48_status *cleanup, int *completed) {
    lt48_runtime_transport_session_close(*wire); *wire = NULL;
    lt48_protocol_transport clean = *api; clean.cancelled = NULL;
    lt48_status first = LT48_OK;
    /* Each action gets a fresh queue: failed applet/transport bytes must not
     * leak into power-off, and one failed rail must not suppress the others. */
    size_t idle_phases=idle&&idle_size?1:0;
    for (size_t phase = 0; phase < count + 6 + idle_phases; phase++) {
        lt48_runtime_transport_session_close(*wire); *wire = NULL;
        lt48_status st = lt48_runtime_transport_session_create(&clean, context, wire);
        if (st == LT48_OK) {
            if (phase < count) st = run(owner, prefix[phase]);
            else if (phase < count + 4) {
                unsigned char off[] = {0x10,(unsigned char)(3-(phase-count)),0};
                st = lt48_runtime_transport_session_controller_write(*wire, off, sizeof(off));
            } else if (idle_phases && phase == count + 4) {
                const unsigned char load[] = {0x38,0xaa,0x55};
                st = lt48_runtime_transport_session_controller_write(*wire, load, sizeof(load));
                if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(*wire,idle,idle_size);
            } else if (phase == count + 4 + idle_phases) {
                const unsigned char done[] = {0x49,0xaa,0x55};
                st = lt48_runtime_transport_session_controller_write(*wire, done, sizeof(done));
            } else {
                unsigned char led[] = {0x40,operation == LT48_OK && first == LT48_OK ? 4 : 2};
                st = lt48_runtime_transport_session_controller_write(*wire, led, sizeof(led));
            }
            if (st == LT48_OK) st = lt48_runtime_transport_session_flush(*wire);
        }
        if (first == LT48_OK) first = st;
    }
    lt48_runtime_transport_session_close(*wire); *wire = NULL;
    *cleanup = first; *completed = first == LT48_OK;
    return operation == LT48_OK ? first : operation;
}
lt48_status lt48_owner_finish_internal(const lt48_protocol_transport *api,void *context,
    lt48_runtime_transport_session **wire,lt48_owner_cleanup_wave_internal run,void *owner,
    const unsigned *prefix,size_t count,lt48_status operation,lt48_status *cleanup,int *completed) {
    return lt48_owner_finish_fpga_internal(api,context,wire,run,owner,prefix,count,NULL,0,operation,cleanup,completed);
}
