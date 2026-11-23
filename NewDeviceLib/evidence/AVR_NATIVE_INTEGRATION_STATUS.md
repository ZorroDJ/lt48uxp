# AVR native C and GUI support

Updated 2026-09-29. Fourteen original Dataman 13.10 entries and six explicit native DIP14 aliases are implemented
in the native C owner and GUI. AT90S1200, AT90S2313, AT90S8535, ATtiny12V, ATtiny24A and AT90S2343 are hardware-qualified. The other 14 entries are source-audited.
Admission is bound to the source digest, definition, family, pin map and capacity.

| Entry | Definition/family/pin map | Flash + EEPROM bytes |
| --- | --- | ---: |
| ATtiny12/V/L | D51/M3e/Pc | 1024 + 64 |
| AT90S1200 | D52/M3f/Pd | 1024 + 64 |
| AT90S2313 | D53/M40/Pd | 2048 + 128 |
| AT90S4414 | D54/M41/Pe | 4096 + 256 |
| AT90S4414 *44 | D55/M41/Pf | 4096 + 256 |
| AT90S8515 | D56/M42/Pe | 8192 + 512 |
| AT90S8515 *44 | D57/M42/Pf | 8192 + 512 |
| AT90S4434 | D58/M43/P10 | 4096 + 256 |
| AT90S8535 | D59/M44/P10 | 8192 + 512 |

Package variants require the original pin mapping/adapter. AT90S8535 *44 and
ATtiny2313 are not admitted by similarity.

Read, Blank Check, Program, Over-program, Verify, Compare, Erase and Secure
use the native C owner (`c/src/avr.c`) through the normal GTK workflow.
The production source before this addition is archived at
`analysis/avr-native-integration/source-before.tgz`.

## Operator workflow

Select the exact **Atmel** entry using Dataman 13.10. Full images start at zero.
For every entry, Flash comes first, followed immediately by EEPROM.
AT90S1200 uses **1,088 bytes**: Flash at 0000–03FF, EEPROM at 0400–043F.
AT90S2313 uses **2,176 bytes**:

| Byte offsets | Contents |
| --- | --- |
| 0000–07FF | 2,048 Flash bytes; little-endian instruction words |
| 0800–087F | 128 EEPROM bytes |

Partial operation ranges are rejected before USB. An FF-filled complete image
represents blank memory. Fuses and lock settings are separate from this image.

- **Program:** capture existing fuse settings, erase, confirm blank/unlocked,
  program both arrays, verify all bytes at source VCC codes 475/525, then read
  back fuse/lock state. No content backup is required by the owner.
- **Over-program:** same write and independent verification, without the
  initial erase. Uses the programming initialization, including fuse handling.
- **SPI programming / second fuse:** Configuration opens Programming Parameters
  for these devices. The second control is **RC oscillator (RCEN)** on AT90S1200
  and **Fast start (FSTRT)** on the other seven entries. Each choice is Preserve, Enabled or Disabled. Preserve is
  the default. Choices apply during Program/Over-program, are read back, and
  reset to Preserve when changing the selected device. Cancelling the dialog
  restores the prior choices. Memory operations otherwise preserve these fuses.
- **Secure:** selects source lock level 2 (both lock bits programmed), blocking
  further memory programming and verification; reads the lock state back.
- **Erase:** clears Flash, EEPROM and locks, verifies the complete blank image,
  and confirms the two fuse settings were preserved.
- A protected chip can return all FF. Blank Check, Verify and Compare reject
  lock level 2 instead of certifying those bytes as accessible blank contents.
  Read reports lock status alongside the bytes.

Program performs its verification before any separate Secure operation. The
result line reports SPIEN, the correctly named RCEN/FSTRT fuse, lock level and cleanup outcome. The GUI's
Configuration entry is an operative fuse selector for these devices, not a
metadata-only property editor.

## Evidence

- Source REZ SHA-256:
  `72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9`.
- Source executable SHA-256:
  `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
- `tools/audit_avr_resources.py`: all nine compact callbacks compile identically
  in C and the reference compiler; no defaulted logical pins. Exact callback
  comparison finds only AT90S2313 in this closure.
- `tools/audit_avr_host.py`: original x86 range dispatcher executed offline for
  24 cases covering six modes, two-byte elements, the Flash/EEPROM boundary,
  and the final element. External device/GUI boundaries are mocked.
- Original TXiPROMGroup constructor/binder and range/operation methods are in
  `analysis/avr-native-integration/host-decompile.log` and the existing
  `analysis/software-v13.10/ghidra-xiprom-selection-reset-stdout.log`.
  Source buffer IDs: Address 0, DataIn 1, DataOut 2, Mode 4. Original source
  Verify policy supplies 475/525; the native owner does not invent margins.
- `c/tests/test_avr.c`: source runtime paths, fuse decoding, signature rejection,
  both memory regions, range guards, cancellation/transport failure cleanup,
  and no partial read publication after failure.
- `c/tests/test_gtk_avr.c`: all 144 real GUI device/operation preflight routes execute rather
  than producing plan-only results.
- [AT90S2313 hardware report](live-qualification/AT90S2313_NATIVE_QUALIFICATION.md).
- [AT90S1200 hardware report](live-qualification/AT90S1200_NATIVE_QUALIFICATION.md).
- `tools/audit_avr1200_resources.py`: 17 C/reference compiler matches for both
  admitted closures; `tools/audit_avr1200_host.py`: 24 host traversal cases for
  the smaller memory geometry. Both sources are regression-tested in C.
- `analysis/avr-at90s1200-integration/source-before.tgz` preserves the qualified
  2313 implementation before the 1200 extension.

The [manufacturer datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/DOC0839.PDF)
was used to cross-check programming entry, fuse bit positions, lock programming,
and erase semantics. The [AT90S1200 datasheet](https://ww1.microchip.com/downloads/en/devicedoc/doc0838.pdf)
confirms the RCEN/SPIEN distinction. Pin operations come from the selected original resource.
Only volatile programmer setup is uploaded; no programmer identity/security
EEPROM or internal ATtiny challenge-response procedure is executed.

## Larger classic AVR group

Six entries share every original callback. Their capacities, signature variables
and pin maps differ and are compiled separately. DEVINIT seeds buffer 10 with the
Flash word boundary (source Size variable 32); that value must survive into LOAD
and PROGRAM. EEPROM high-address latching is retained for every byte, including
addresses 256–511 on the 8 KiB parts. Their erase pulse uses nominal VCC and the
source delay, distinct from AT90S2313's 4 V sequence.

- `tools/audit_avr8535_resources.py`: 71 C/reference compiled-wave comparisons
  across all eight entries; no defaulted logical pins.
- `tools/audit_avr8535_host.py`: 36 original x86 dispatcher cases covering both
  larger geometries, memory transitions and the upper EEPROM address boundary.
- Native runtime/failure tests cover all eight admitted pin/variable combinations.
- GUI preflight covers all 64 operations. Actual GTK controls were also exercised
  with fatal GTK diagnostics enabled: fuse labels, Preserve defaults, Cancel and
  device-change reset passed without accessing USB.
- `analysis/avr-at90s8535-integration/source-before.tgz` preserves the previously
  qualified AT90S1200/2313 implementation.
- [AT90S8535 datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/doc1041.pdf)
  cross-checks signature 1E9303, programming pins, both fuses and erase semantics.

- [AT90S8535 hardware report](live-qualification/AT90S8535_NATIVE_QUALIFICATION.md):
  full Flash/EEPROM, both fuses, locks and final all-FF read passed.

## ATtiny12 serial procedure

ATtiny12/V/L uses ten original compact waves, not the classic parallel procedure.
The normal GUI exposes all eight operations. Configuration provides a **fuse
byte** entry: exactly two hexadecimal digits, or blank to preserve. Bits7..0
are BODLEVEL, BODEN, SPIEN, RSTDISBL, CKSEL3..0, using the physical values
(0 = programmed). Malformed input is rejected before USB. Choices reset when
changing the device, and Cancel restores the previous choice.

Program/Over-program verifies both arrays before writing the complete fuse byte
through the source PROTECT callback with locks clear, then independently checks
all bits. Secure preserves all fuses and writes lock level 2. Erase preserves the
fuse byte and factory oscillator calibration. The result reports both bytes.
Calibration insertion into user Flash is disabled; images are written exactly.

[ATtiny12V hardware report](live-qualification/ATTINY12V_NATIVE_QUALIFICATION.md):
all operations passed, complete fuse byte changed/restored, final all-FF independent
read. `tools/audit_attiny12_resources.py` checks ten compiler matches and
`tools/audit_attiny12_host.py` checks 24 original host traversal cases.

## Paged ATtiny24/44/84 group

| Original source entry | Source tuple | Native DIP14 aliases | Flash + EEPROM bytes |
| --- | --- | --- | ---: |
| ATtiny24/V *14SO | D62/M4d/P17 | ATtiny24/V, ATtiny24A | 2048 +128 |
| ATtiny44/V *14SO | D63/M4e/P17 | ATtiny44/V, ATtiny44A | 4096 +256 |
| ATtiny84/V *14SO | D64/M4f/P17 | ATtiny84/V, ATtiny84A | 8192 +512 |

All nine entries expose the complete eight-operation C/GUI workflow. Native aliases
are explicitly labelled in the registry and do not modify the original databases.
The manufacturer AVR526 compatibility note and shared programming datasheet
establish A-revision compatibility; DIP14 and SO14 programming pin assignments
match. Package variants other than these 14-pin mappings are not inferred.

Configuration accepts **LOW HIGH EXT** as three hex bytes, e.g. `62 DF FF`;
blank preserves all three. Reserved extended bits must be 1. Readback checks all
17 implemented fuse bits. The result reports all three bytes and calibration.
The source direct fuse bank is kept separate from the global ID-check option.

Flash page sizes are 16 words for 24 and 32 words for 44/84; EEPROM pages are 4 bytes.
The owner preserves page counters and variables across original callbacks.
**Program and Over-program both perform a full rewrite using the source mode 2
initialization, which includes erase.** Neither offers partial or erase-free writes
for this group. Erase clears EEPROM even with EESAVE enabled, then restores the
original fuse bytes and verifies memory, fuses and calibration.

- [ATtiny24A hardware report](live-qualification/ATTINY24A_NATIVE_QUALIFICATION.md):
  all operations, three-fuse changes/restoration and EESAVE erase qualified;
  final 2176 bytes FF, original 62 DF FF/calibration 4E.
- `tools/audit_attiny24_resources.py`: 90 C/reference comparisons for all nine
  source/alias entries, including distinct geometry/signature variables.
- `tools/audit_attiny24_host.py`: 54 original x86 dispatcher cases including
  page boundaries and the ends of all three geometries, external boundaries mocked.
- Runtime tests cover 18 total AVR entries; all 144 GUI preflights pass. Existing
  PIC/PIC18/runtime and GUI regressions pass with the explicit configuration-bank binding.

## AVR sample batch

All six offered samples are complete: AT90S2313, AT90S1200, AT90S8535,
ATtiny12V, ATtiny24A and AT90S2343. Each was left erased and unlocked with its
original fuse settings restored and calibration preserved where applicable.

## FPGA-assisted AT90S2323/2343

Two additional complete source entries: AT90S/LS2323 D5f/M4a/P14 and
AT90S/LS2343 D60/M4b/P14, each 2048 Flash +128 EEPROM bytes. This brings native
AVR support to **20 entries, six hardware-qualified**. The other entries have
source audits; package variants and aliases are counted separately.

AT90S2343 passed all eight operations, RCEN change/restoration, negative memory
checks and protected-operation rejection. It finished erased/unlocked with RCEN
enabled. [Individual report](live-qualification/AT90S2343_NATIVE_QUALIFICATION.md).
AT90S2323 remains source-audited, with FSTRT instead of RCEN. Both reuse original
W81 FPGA configuration and ten compact callbacks. No writable SPIEN control is
exposed. All160 GUI preflights and actual per-device controls pass offline.

For these SPI parts, protected Read also rejects rather than publishing bytes.
Native Blank Check uses non-destructive mode1 because source mode11 can erase
locked parts. Locked parts hide their signature; explicit Erase recovers the
manually selected part and then checks identity and blank contents. See
`analysis/avr-at90s2343-integration/source-notes.md` for the audited source and
native handling. This is not listed as a proven Windows software bug.
