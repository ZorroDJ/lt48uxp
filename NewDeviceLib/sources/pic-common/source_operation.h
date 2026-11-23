#ifndef LT48_PIC_SOURCE_OPERATION_H
#define LT48_PIC_SOURCE_OPERATION_H
#include "source_session.h"
#include "source_geometry.h"
#include "source_host.h"
#include "source_compact_program_sequence.h"

/* One public phase dispatcher for all constructor-bound compact profiles.
 * Adapters supply already qualified stage bodies; this does not grant support
 * or infer preservation/verification/cleanup policy from a part name. */
typedef int (*lt48_pic_source_public_program)(lt48_pic_compact_program_state *,
    lt48_pic_compact_program_call,void *);
int lt48_pic_source_compact_program_dispatch_internal(lt48_pic_compact_program_state *,
    lt48_pic_compact_program_call,void *);

/* Source-bound compact host machinery. Silicon-specific behavior remains in
 * the compiled source callbacks; no device name selects an operation route. */
typedef struct {
    lt48_pic_source_geometry geometry;
    uint16_t element_bits,init_role,reset_role,load_role,program_role,erase_role,configuration_role;
} lt48_pic_source_operation_contract;
typedef lt48_status (*lt48_pic_source_finish)(void *);
typedef lt48_status (*lt48_pic_source_global_finish)(void *,lt48_status,lt48_status);
typedef struct {
    lt48_status operation_status,cleanup_status;
    lt48_pic_range_execution_result traversal;
    unsigned cleanup_attempted;
} lt48_pic_source_operation_result;

/* The owner supplies source-bound startup and uncancelled safe power-off /
 * transport cleanup. Startup must not duplicate the contract's DEVINIT.
 * This service owns mode, host traversal / transaction order and unconditional
 * cleanup. It does not infer voltage overrides, preservation or admission. */
lt48_status lt48_pic_source_operation_execute_internal(
    lt48_pic_source_session *,const lt48_pic_source_operation_contract *,uint16_t,
    uint32_t,uint32_t,const lt48_pic_source_range_options *,
    lt48_pic_source_finish,void *,lt48_pic_source_operation_result *);
/* Complete compact host owner: source startup, operation plan, source
 * POWEROFF and unconditional transport-level release supplied by the adapter.
 * options.start must be NULL; this service supplies the common host startup.
 * Transport/FPGA preparation and configuration/preservation remain explicit. */
lt48_status lt48_pic_source_operation_run_internal(
    lt48_pic_source_session *,const lt48_pic_source_operation_contract *,
    const lt48_pic_source_host *,uint16_t,uint32_t,uint32_t,
    const lt48_pic_source_range_options *,lt48_pic_source_global_finish,void *,
    lt48_pic_source_operation_result *);
/* One public compact operation over one private source frame. Verify slots
 * retain their original positions; zero slots are skipped. The source-bound
 * adapter owns startup and finish, including POWEROFF and safe release. */
typedef lt48_status (*lt48_pic_source_program_prepare_image)(void *,lt48_pic_source_session *);
typedef struct {
    uint32_t flags,first,last;
    unsigned stopped; /* Explicit original +98 state; never inferred from a name. */
    uint16_t voltage_variable;
    uint32_t nominal_voltage,verify_voltages[4];
    lt48_pic_source_range_options options;
    int (*ask_erase)(void *);
    /* Pure source configuration/preservation-image preparation on the private
     * clone, before startup. Canonical state commits only after full success.
     * Chip reads needed for preservation remain a separate powered stage. */
    lt48_pic_source_program_prepare_image prepare_image;
    void *prepare_context;
} lt48_pic_source_program_request;
typedef lt48_status (*lt48_pic_source_program_finish)(void *,lt48_pic_source_session *,uint16_t,lt48_status);
lt48_status lt48_pic_source_program_execute_internal(lt48_pic_source_session *,
    const lt48_pic_source_operation_contract *,const lt48_pic_source_program_request *,
    lt48_pic_source_program_finish,void *,lt48_pic_source_operation_result *);
typedef lt48_status (*lt48_pic_source_program_engine)(lt48_pic_source_session *,
    const lt48_pic_source_operation_contract *,const lt48_pic_source_program_request *,
    lt48_pic_source_program_finish,void *,lt48_pic_source_operation_result *);
#endif
