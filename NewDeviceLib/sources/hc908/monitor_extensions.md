# Authored MON08 and target RAM extensions

Scope: ordinary D48–D4d QT/QY source routes only. Original payloads are supplied
by the operator; this directory contains authored code, not vendor binaries.

## Authenticated reads and monitor proxy

The original Read worker is patched at 51A6. Its six R4/R5/R6 initialization
bytes become `LCALL 5A00; LJMP 51AC`. The authored preface reads MON08 RAM0080
and factory trim FFC0/FFC1, returns those three bytes to the host, reinstates
R4/R5/R6 and returns. The original F7FF/EDFF priming read and IREAD loop remain.
The host requires RAM0080 bit6 before accepting Flash data or starting mutations.
The entry key is separate from the eight new image security bytes FFF6–FFFD.

For RAM-helper operations the same entry instead jumps to the preface, whose
tail jumps to a 55-byte proxy at 5C00. The proxy uses the source TX58C8/RX5904
routines, checks command echo in the C owner and returns to source cleanup5262
on host termination. These extensions occupy only volatile programmer RAM.
They do not access programmer EEPROM or ATtiny security.

## Target Flash timing

The original Program and Erase mutation entries are not executed by the Linux
owner. Their serial monitor WRITE/delay path leaves Flash control asserted far
longer than the documented 30–40us byte interval. Their downloaded payloads are
retained only because they contain the shared monitor TX/RX helper routines.

The authored HC08 Program (57bytes) or Erase (53bytes) helper is downloaded at
00B0. Program data occupies 008C..00AB. Every downloaded byte is read back.
MON08 READSP returns SP+1; only F0..FA is accepted. The stacked PC at that value
plus4/5 is replaced with 00B0 and read back before RUN. The helper ends with SWI,
returning to the monitor; a 20ms host wait precedes FLCR and result readback.
No UART transaction occurs during the Flash-critical pulse interval.

The Program helper uses 21-iteration instruction delays, maximum32bytes per
row and no row crossing. At bus2.4/2.4576/2.5MHz its checked byte intervals are
35.2–37.5us and maximum row HV is1.268ms, below the4ms limit. Erase uses at least
4ms mass-erase time and at least100us hold after clearing ERASE. These bounds
are instruction-derived, not a physical waveform measurement.

Erase first saves both trim bytes, mass erases, then restores FFC0 and FFC1 in
one row transaction. Program skips both. Ordinary and A parts reverse their
voltage meanings; raw byte preservation avoids reinterpreting either value.
An unknown entry key prevents erase because trim cannot be backed up safely.

## Verification and limits

Independent target execution checks the exact C-built helper bytes, Flash
state/timing and adjacent cells. Original 8051 execution checks proxy serial
framing and a distinct-byte entry key. Production C tests check full operations,
readback, key changes, cleanup and injected faults. Selected XC3030A netlists
check digital COM routing for all three package mappings.

Physical timing, oscillator tolerance on actual parts, analogue rails and
adapter contacts remain hardware qualification work. Use the selected original
package wiring; package names alone do not authorize a straight adapter.
