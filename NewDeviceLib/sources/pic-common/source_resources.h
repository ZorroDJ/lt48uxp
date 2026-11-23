#ifndef LT48_PIC_SOURCE_RESOURCES_H
#define LT48_PIC_SOURCE_RESOURCES_H
#include "source_transport.h"
#include "source_contact_masks.h"

typedef struct lt48_pic_source_resources lt48_pic_source_resources;
/* Payload and compiled callback pointers are borrowed until owner close.
 * Types 0/4/6 retain compact/FPGA/applet resources respectively. This table is
 * independent of frame roles, host geometry and operation admission. */
typedef struct {
    uint16_t role,wave;
    lt48_wave_code source;
    const lt48_compiled_wave *compiled;
} lt48_pic_source_resource_entry;
lt48_status lt48_pic_source_resources_library_internal(const char *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,lt48_pic_source_resources **);
/* Exact GADC/M27..39 source context: original Kernel's empty address-group10
 * for P2/P3/P4, and M37..39/P5/W2c pin-zero convention. The only permitted
 * default emits the independently checked resident no-op selector; unrelated
 * bindings/substitutions reject. Normal construction remains strict. */
lt48_status lt48_pic_source_resources_older16_library_internal(const char *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,lt48_pic_source_resources **);
/* Exact older16 context plus the authored old-PIC17 PM2 address correction.
 * The unmodified older16 factory above remains available for source audit. */
lt48_status lt48_pic_source_resources_older16_authored_library_internal(const char *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,lt48_pic_source_resources **);
/* Exact PIC18C candidate reset continuation; no general pin/wave override. */
lt48_status lt48_pic_source_resources_pic18c_authored_library_internal(const char *,
 const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,lt48_pic_source_resources **);
/* Construction-only OTP14 program-word correction. Exact source payload and
 * timing/retry shape gates apply before publishing this immutable owner.
 * Other callbacks retain their source behavior. No device admission occurs. */
lt48_status lt48_pic_source_resources_otp14_library_internal(const char *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,lt48_pic_source_resources **);
/* Construction-only RAM overlays. Caller supplies an audited profile policy;
 * this factory verifies original and replacement hashes, load address, size
 * and slot/other-applet bounds. Copies are owned until close. The original
 * library binding remains intact. No exposed owner/session is mutated. */
typedef struct {
    uint16_t role,load_address;
    uint32_t slot_end;
    size_t original_size;
    unsigned char original_sha256[LT48_SHA256_SIZE];
    const unsigned char *data;size_t size;
    unsigned char sha256[LT48_SHA256_SIZE];
} lt48_pic_source_ram_overlay;
lt48_status lt48_pic_source_resources_overlay_library_internal(const char *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,
    const lt48_pic_source_ram_overlay *,size_t,lt48_pic_source_resources **);
/* Construction-only, exact original time-variable replacement. Voltage and
 * integer variables reject. Recompile callbacks before exposing the owner. */
typedef struct {lt48_compile_variable original,replacement;} lt48_pic_source_time_override;
lt48_status lt48_pic_source_resources_time_library_internal(const char *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,
    const lt48_pic_source_time_override *,size_t,lt48_pic_source_resources **);
/* Construct one immutable owner with both verified RAM and time corrections. */
lt48_status lt48_pic_source_resources_overlay_time_library_internal(const char *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,
    const lt48_pic_source_ram_overlay *,size_t,
    const lt48_pic_source_time_override *,size_t,lt48_pic_source_resources **);
int lt48_pic_source_resources_matches_binding_internal(const lt48_pic_source_resources *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t);
size_t lt48_pic_source_resources_count_internal(const lt48_pic_source_resources *);
lt48_status lt48_pic_source_resources_entry_internal(const lt48_pic_source_resources *,size_t,
    lt48_pic_source_resource_entry *);
/* Immutable compilation bindings; borrowed for the owner's lifetime. */
lt48_status lt48_pic_source_resources_bindings_internal(const lt48_pic_source_resources *,
    const lt48_pin_map **,const lt48_compile_variable **,size_t *);
/* Caller owns the returned mutable session; close it before the resource
 * owner. Source frame and complete compact/applet tables are bound together.
 * Image geometry and explicit buffer roles come from the host profile. */
lt48_status lt48_pic_source_resources_create_session_internal(const lt48_pic_source_resources *,
    const lt48_runtime_options *,lt48_pic_source_session **);
/* Offline factory for generated compact-source profiles. The exact library
 * digest, family, pin map, geometry and host roles come from source evidence.
 * It loads every family callback, including nested-only callbacks. No device
 * name, cohort policy, voltage override, USB access or admission occurs here.
 * The returned session borrows its compiled table and pins from this owner. */
lt48_status lt48_pic_source_resources_open_internal(const char *,
    const unsigned char [LT48_SHA256_SIZE],uint16_t,uint16_t,
    const lt48_pic_source_operation_contract *,const lt48_pic_source_host_binding *,
    const lt48_runtime_options *,lt48_pic_source_resources **);
void lt48_pic_source_resources_close_internal(lt48_pic_source_resources *);
lt48_pic_source_session *lt48_pic_source_resources_session_internal(lt48_pic_source_resources *);
lt48_status lt48_pic_source_resources_frame_internal(const lt48_pic_source_resources *,
    lt48_runtime_frame_profile *);
/* Derive contact policy from this immutable owner's selected raw pin resource.
 * No capture, upload, power or hardware access occurs here. */
lt48_status lt48_pic_source_resources_contact_masks_internal(const lt48_pic_source_resources *,
    lt48_pic_contact_masks *);
/* Complete resource/transport/host join. Uses the immutable source operation,
 * host and frame bindings stored at construction; explicit FPGA bytes and
 * audited voltage bounds are supplied by the selected profile. Each call
 * owns fresh preparation and unconditional source/global cleanup. */
lt48_status lt48_pic_source_resources_execute_internal(lt48_pic_source_resources *,
    const lt48_protocol_transport *,void *,const unsigned char *,size_t,
    uint16_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,
    lt48_pic_source_operation_result *);
/* Original ascending callback-role order, exact applet payloads and selected
 * type-4 FPGA payload. All pointers borrow the immutable resource owner.
 * Building this descriptor performs no I/O or source operation. */
typedef struct {
    lt48_pic_source_applet_payload applets[256];size_t applet_count;
    const unsigned char *fpga;size_t fpga_size;
} lt48_pic_source_preparation;
lt48_status lt48_pic_source_resources_preparation_internal(const lt48_pic_source_resources *,
    uint16_t,lt48_pic_source_preparation *);
/* Reject a transport session from a different resource owner before I/O.
 * This prepares resources only; power/operation/preservation policy is separate. */
lt48_status lt48_pic_source_resources_prepare_internal(const lt48_pic_source_resources *,
    uint16_t,lt48_pic_source_transport *);
/* Deferred preparation for the composite source startup policy. Same exact
 * selected owner/binding checks; FPGA and selector wait for contact acceptance. */
lt48_status lt48_pic_source_resources_prepare_startup_internal(const lt48_pic_source_resources *,
    uint16_t,lt48_pic_source_transport *);
/* Same source preparation for an existing independently managed queue.
 * Explicit FPGA selector and caller-owned cleanup preserve source route policy. */
lt48_status lt48_pic_source_resources_prepare_wire_internal(const lt48_pic_source_resources *,
    lt48_pic_source_session *,uint16_t,lt48_runtime_transport_session *,uint8_t);
#endif
