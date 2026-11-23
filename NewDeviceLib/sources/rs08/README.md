# RS08 KA2 C operation owner

Two exact Dataman 13.10 entries: Freescale MC9RS08KA2 (DIP-8) and
MC9RS08KA2 *8SO. Definition D6b, family M64, pin map P48 in GADB___.rez.
Original REZ, controller applets and FPGA data are supplied by the operator;
this directory contains authored C and documentation only.

Read, Blank Check, Program, Erase, Verify and Compare use a 16,384-byte
source-addressed image. Flash occupies 3800–3FFF (2 KiB); lower locations
must be FF. Select the actual package/adapter. Nominal VCC is 5 V; the
source enables external 12 V VPP only for mutation, through channel3.

## Original behavior and reused components

Reuse the class2014 host route, strict compact compiler, runtime, transport
and cleanup machinery. The FPGA payload is identical to S08 QD/QG8; pin-map
groups match and RS08 logical6 equals S08 logical62 (BKGD ZIF22); RESET
is logical4 (ZIF21). Existing corrected XC3030A simulation evidence applies.
This equality does not establish a new measured hardware net.

Four BYO controller workers are loaded: Program5100, Erase51D4, Read5336,
Protect/helper53C0. The last contains required BDM helpers at5461/548C/54E1;
the Protect entry itself is never called. Startup has two GPIO bytes, unlike
S08's four-byte startup. E0/C0 BDM commands read/write 16-bit addresses.
Programming uses FCR0211, PGM/HVEN and 64-byte rows, not S08 FCMD/FSTAT.
Erase saves 3FFA/3FFB, mass-erases and restores those trim bytes.

## Authored policy

- A read-only FOPT0210 check precedes operations. It uses an address-relocated
  BYO Read worker: only immediate bytes at5385 and5388 change (00/38 to10/02).
  This transformation is authored here in operations.c; no original worker
  bytes are bundled. The original reader is restored before ordinary access.
- Secured parts are rejected; blocked Flash zero reads are not valid contents.
  Secured recovery, Protect and calibration changes are not exposed.
- Source Program forces FF at 3FFA/3FFB and NVOPT3FFC. Preserve factory trim;
  accept FF placeholders or saved matching trim. NVOPT may be FF or saved
  unlocked01; reject attempts to set security. Reserved NVOPT bits read zero,
  so comparison uses SECD only. Verify accepts trim preservation placeholders.
- Program requires blank ordinary Flash. Every mutation is checked through
  fresh raw reads, including trim preservation; per-byte echoes are not proof.
- Terminal source LOAD/PROGRAM Stop is consumed by C before reset; unused
  completion capture is removed. DEVRESET retains source VPP ramp, supply
  shutdown and resident no-op Stop. Erase completion is followed by an explicit
  Stop before reset pin writes. Failed transport leaves recovery required.
- Margins use 5% or 10% of nominal supply. Output is published only after
  successful cleanup; all DAC channels, idle FPGA and LEDs are finalized.

Offline qualification does not establish analogue waveform timing or actual
silicon behavior. Hardware qualification is pending. See
`docs/audit-qualification/RS08_KA2_PROGRESS.md` and its sealed receipt.
