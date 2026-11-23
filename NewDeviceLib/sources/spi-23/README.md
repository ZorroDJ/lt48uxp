# XMC, Macronix and cFeon 256-Mbit SPI flash

23 exact Dataman 13.10 entries, 13 definitions and five configuration layouts.
Portable C AddTab SPI owner and normal GTK; raw Read and all eight operations.
Main array: 32 MiB. Appended source regions: Macronix/cFeon 512 bytes,
XMC-B four 256-byte rows, XMC-C three 256-byte rows. Complete 256-byte pages.
Macronix erase preserves its secured region; XMC/cFeon erase includes it.
Existing irreversible locks remain; no new permanent locks are exposed.
Macronix WPSEL advanced-sector mode rejects BP-based Secure.

All controller applets reuse sealed per-role evidence; three combinations.
The original XMC-B W23f information-row erase selects a one-opcode helper and
omits the required WREN/address on SPI. xmcb_erase.inc contains an authored
replacement fragment only. It is applied to the exact BYO callback in De4/De5,
retaining its normal-array branch. The correction erases four rows once at
the first security boundary and polls WIP using the original status reader.
No original applet, bitstream or complete vendor callback is bundled here.

Original vendor resources remain BYO. Source package adapters required.
Hardware/socket/adapter/analogue qualification pending.
Audit: docs/audit-qualification/SPI_23_AUDIT.md.
