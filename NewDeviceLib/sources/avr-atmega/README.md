# Classic ATmega parallel programming cohort

Author-created bindings and operation composition for 48 Dataman 13.10 entries,
18 definitions, one exact ten-routine compact resource set. The seed is
Atmel ATmega328P in DIP-28. Source assets stay in the user-imported library.
No vendor REZ, firmware, FPGA image or Windows executable is bundled here.

The existing portable C AVR owner, compiler/runtime, pin mapper, verified idle
FPGA loader, transport and GUI are reused. `profiles.h` admits exact names,
definition/family/pin-map/geometry tuples from the frozen cohort, only in the
Dataman 13.10 source profile. `owner.inc` composes complete operations.

## Operations and image layout

Read, Blank Check, Program, Over-program, Verify, Compare, Erase and Secure.
Images contain little-endian Flash words as bytes, then EEPROM bytes. Full-array
operations are required. Program explicitly erases, confirms blank/unlocked,
writes Flash and EEPROM pages, then independently reads both arrays at selected
margins. Over-program omits pre-erase and requires an unlocked part; both writes
use source programming initialization mode 2 to initialize page state. Failed
writes/reads never publish success; failed Reads leave the caller buffer intact.

All three fuse bytes are read and preserved by default. LOW HIGH EXT hex input
can request explicit Program changes. Optional fourth/fifth bytes select the
source BLB0/BLB1 modes (00 unlocked;01/02/03 protected); these apply after
Program verification or during Secure. ATmega48 entries have no boot section
and reject nonzero boot-lock choices. Erase clears EEPROM even when EESAVE was
enabled, restores the original fuses, and clears locks. Secure preserves both
boot-lock fields and fuses while applying the memory lock. Protected Read/Blank/
Verify/Compare reject rather than claiming an FF array is valid memory.
Optional calibration insertion into the image is explicitly disabled (device
configuration dropdown 3 = 1); the image is programmed exactly as supplied.
Factory calibration storage is not programmed; this source route does not expose
its value as the tinyAVR route does.

## Voltage and shutdown

Use the source VCC variable (PB profiles: 330 cV; others: 500 cV), VPP 1200 cV,
and the previously x86-qualified common verification selector (5%/10%, rounded
as the original). For this cohort, `host_avr_parallel` enables original opcode-3
float-to-centivolt quantization and original 443A50 shutdown: rails above 5 V
first go to 5 V in descending order, then active rails go to zero. The explicit
float conversion matters at 3.3 V. DAC packet equality is not a new physical
voltage measurement. Fallback cleanup also independently disables all four rails
and shows Good/Error appropriately.

Original compiler/interpreter comparisons and an independent parallel AVR
Flash/EEPROM/fuse/lock model qualify the software composition. Physical timing,
contacts, adapters and analogue margins remain untested for these entries.
Do not infer a straight adapter from a package label; use the selected source
pin mapping and correct package adapter.

See `docs/audit-qualification/ATMEGA_48_AUDIT.md` and the sealed qualification
receipt for exact gate counts, input hashes and reuse boundaries.

## AVR22 extension (staged)

Exact additional bindings are in `profiles22.h`. ATmega8A/8-L NEW and
ATmega16-L NEW have two physical fuse bytes: LOW HIGH, optionally followed by
the two boot-lock choices. EXT edits are rejected. Their optional unmapped
logical27 routing instruction is omitted only where absent, reusing the
resident zero-mask proof from the nine-entry NEW-route qualification.

AT90CAN uses the source 19-checkbox mapping: extended bits1–3, reserved bits
fixed1, high-fuse inversion mask F9. AT90USB1286/1287 use their **own original
20-checkbox callbacks** W12d/W12e: extended bits0–2 and inverted HWBE bit3.
The C owner translates these into three physical fuse bytes for the normal
GUI editor. All source callbacks, array operations, page geometry, configuration
semantics and dedicated source adapter footprints remain in use. P41 describes
a 32-pin programming-adapter footprint for these 64-pin targets; it is exactly
the already qualified P53 pin map. Package labels do not imply straight wiring.

Four source-bound USB correction records set the stored third signature byte
81 to the manufacturer's82. The original W128 ID callback actually reads only
the first two signature bytes; both source and native retain that checking
scope. The corrected third byte is metadata, not a claim that the original ID
callback rejects the device or that a third byte was physically checked.

Original x86/C configuration and traffic comparisons plus complete independent
native memory/fuse/lock effects qualify these entries on audit basis. No hardware
or analogue qualification is claimed. See
`docs/audit-qualification/AVR_22_AUDIT.md` and its per-stage receipts.

ATtiny25/45/85 use the existing C HVSP (`serial2`) owner with source Program
W90. Full images are Flash bytes followed by EEPROM bytes. Calibration remains
read-only and source calibration insertion is disabled. As with the qualified
24/44/84 route, both public writes use source initialization mode2, which erases;
Over-program is not an erase-free operation for this group. All17 physical fuse
bits and calibration are read and preserved; Secure uses the original lock mode.
The six new entries use original staged rail shutdown and the existing audited
5%/10% verification selector; earlier hardware-qualified serial profiles retain
their prior implementation. No MCU applet or new FPGA configuration is required.

## AVR25 source-bound extension

The staged `profiles25.h` bindings use per-family fuse schemas, preserving the
source checkbox order, inversion, reserved bits and physical two/three-byte
layout. The same portable C owner and normal GTK interface handle all operations.
The original pin maps remain unchanged. P38's unused logical19/20 aliases do
not determine XA wiring: the actual callbacks use group9. An early simulator
assumption and unnecessary workaround were withdrawn after full callback
compilation comparison showed no operation uses those logical entries.
Qualification is recorded in `docs/audit-qualification/AVR_25_AUDIT.md` and the
per-stage receipts; hardware/contact/adapter qualification remains pending.
No proprietary assets are stored in this authored source directory.
