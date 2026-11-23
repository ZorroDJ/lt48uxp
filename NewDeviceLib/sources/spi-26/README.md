# SPI26 authored profiles and operation extensions

26 Dataman 13.10 class-2044 catalogue identities, 18 source definitions.
All programming/controller/FPGA resources remain bring-your-own from the exact
source installation. No vendor binary or full vendor callback is included here.
Normal execution, transport ownership and GUI dispatch are portable C.

- `closures.inc`, `members.inc`: exact source geometry, configuration policies,
  identity/ID/shape bindings and memory-kind descriptors.
- `micron_selector.inc`: authored, source-scoped OTP dispatcher/address edits
  for MT25QL256ABA/N25Q256A13. Main-array four-byte access is retained. Original
  callbacks are never modified in the user's installation.
- `sector_unlock.inc`, `sector_owner.inc`: older Macronix F3 unprotect and E2/FB
  per-64KiB-sector protection owner, with independent status readback. Existing
  secured-memory/factory locks are retained; no OTP lock is added.
- `readonly_id.inc`: preserve SST26 factory Security-ID bytes 0000..0007 exactly
  when programming a complete first security page. The user image must match
  them; final verification includes them. Standalone Blank Check retains raw
  whole-range semantics and can report these factory bytes nonblank.

SST26 Secure permanently locks user security-ID programming, as in the source;
Erase cannot unlock it and preserves the complete 2048-byte security region.
Micron Erase retains 64-byte OTP. cFeon (2R)/(2RC) source Erase includes all three
512-byte security rows; an existing per-row SPL lock can make verified Erase fail.
DataFlash requires the selected page-size variant and complete-array Program /
Overprogram streams. Its appended 64-byte security region is one-time
programmable, Erase preserves it, and Secure uses reversible sector protection.

Qualification is recorded separately in `docs/data/spi26_qualification.json`
and `docs/audit-qualification/SPI_26_AUDIT.md`. Source audit does not establish
silicon, analogue, socket or adapter qualification. Package adapters remain
source-specific. No extra permanent settings editors or unexposed chip features
are implied by a complete programmer operation profile.
