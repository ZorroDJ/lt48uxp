#ifndef LT48_PIN_CONTROL_H
#define LT48_PIN_CONTROL_H
#include <labtool48/labtool48.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Board-specific physical routes. Zero means all output drivers released;
 * the protected FPGA input remains connected. Array indices are ZIF-1. */
enum lt48_pin_route {
 LT48_PIN_RELEASE=0, LT48_PIN_FPGA_LOW=1, LT48_PIN_FPGA_HIGH=2,
 LT48_PIN_BOTTOM_SOURCE=4, LT48_PIN_TOP_SOURCE=8,
 LT48_PIN_TPIC_SINK=16, LT48_PIN_VCC_RELAY=32, LT48_PIN_GND_RELAY=64
};
typedef struct {
 unsigned available_routes;
 /* Serial bank 1 = even, 2 = odd; OUT numbering is 1-based. */
 unsigned bottom_bank, bottom_output, bottom_dac_channel;
 /* Zero means that no relay of this type exists at this contact. */
 unsigned vcc_relay_output, ground_relay_output;
 unsigned short sink_xdata_address;
 unsigned char sink_mask, top_source_enable_operand;
} lt48_pin_capability;
/* Firmware-derived topology for this board. Does not imply that every contact
 * has been measured, or establish load/current/voltage ratings. */
LT48_API lt48_status lt48_pin_control_capability(unsigned zif_pin,
 lt48_pin_capability *);
typedef struct {
 unsigned routes[48];
 /* Shared DAC setpoints, not measured socket voltages. */
 unsigned millivolts[4];
 /* Device/adapter-specific ceilings must be supplied for active rails. */
 unsigned maximum_millivolts[4];
 /* Caller-established discharge and relay-settle intervals; no board-wide
  * timing or current limit is inferred from the unloaded qualification. */
 unsigned discharge_us, settle_us;
 int verified_baseline_fpga;
} lt48_pin_configuration;
enum lt48_pin_step_kind {
 LT48_PIN_CONTROLLER_BYTES=1, LT48_PIN_FPGA_BYTE=2,
 LT48_PIN_WAIT_US=3, LT48_PIN_STATUS_CHECK=4
};
typedef struct {
 enum lt48_pin_step_kind kind;
 unsigned char bytes[16];
 size_t size;
 unsigned delay_us;
} lt48_pin_step;
typedef struct {
 lt48_pin_step steps[192]; size_t count;
 unsigned char serial[2][4];
 unsigned char sink_registers[8];
 unsigned char dac_codes[4];
} lt48_pin_plan;
/* Offline only. No USB; never a transport-ready admission by itself.
 * Each plan transitions through full reset (break before make), not a delta.
 * Caller must stop on failed steps, apply reset cleanup and mark state unknown
 * until confirmed. No nonvolatile relay-counter handlers are emitted.
 * Input configuration and output plan may not alias. Output unchanged on error. */
LT48_API lt48_status lt48_pin_control_plan(const lt48_pin_configuration *,
 lt48_pin_plan *, char *reason, size_t reason_size);
#ifdef __cplusplus
}
#endif
#endif
