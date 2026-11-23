# HC11 A/E/L EEPROM engine

Portable C operation owner for the exact Dataman13.10 fourteen-entry cohort.
Source applets, FPGA and compact procedures are read from the user's verified
GADA___.rez; this directory contains no redistributed vendor payloads.

Image offsets0000–01FF represent EEPROM B600–B7FF, not mask ROM.
Read, Blank Check, Program, Verify, Compare, Erase and separate CONFIG handling
are provided. Program autoerases EEPROM and restores CONFIG. Erase writesFF
through that same source erase/program route, followed by independent readback.
Selected CONFIG bits are merged with a live value; clearing NOSEC is deferred.
On optional-security masks already secured with NOSEC0, bootstrap entry itself
erases EEPROM/RAM/CONFIG. This silicon behavior also applies to Read.

The original compact stops omit some terminal captures. The owner adds explicit
captures into unused buffer75 after those stops and checks for01. Source target
kernel bytes and programmer pins are unchanged. All76 interpreter buffers are
carried between waves; a32-buffer snapshot would lose EEPROM/config addresses.
The normal configuration editor uses the existing immutable one-shot snapshot.

See docs/audit-qualification/HC11_A_E_L_WORKFLOW.md and qualification receipts.
Hardware, analogue timing and physical adapters remain pending.

## E2 / F1 / K EEPROM extensions

Exact additional tuples: MC68HC811E2 DIP48 and @52 (2 KiB F800–FFFF),
MC68HC11F1 @68 (512 bytes FE00–FFFF), MC68HC11K1/K4 @84
(640 bytes 0D80–0FFF), Motorola and Freescale names. Six memory operations
use the source bootstrap/read/EEPROM-program workers and selected FPGA/pins.
Whole EEPROM images start at file offset zero. Mask ROM is excluded.

The extension owner uses source mode8 rail setup and the original EEPROM
program13 flag1 handshake, avoiding source mode2's CONFIG rewrite. Source
BPROT=10 and EEPROM bulk erase/program are retained. CONFIG cells are never
written. E2/F1 source CONFIG14 uses scratch EEPROM and a reset vector;
bootstrap reads force mapping bits. CONFIG editing is separately deferred,
not exposed as supported by this six-operation profile. Original adapter
requirements and optional secured-mask bootstrap erasure remain applicable.
