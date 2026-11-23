# Legacy flash8051 source profiles

41 exact Dataman13.10 entries:20 SST,17 RAMTRON and4 Mosel Vitelic.
31definitions/ten operation-resource sets. Original GADA resources remain BYO;
no vendor applet, REZ or FPGA bytes are distributed here.

Portable C: c/src/mcu51.c, normal GTK workers. Read, Blank, Program,
Over-program, Verify, Compare, Erase and Secure; later SST adds read-only
Configuration. Program requires blank code; Over-program checks one-way changes.
Erase clears the whole image. Raw code Read is implemented. Hardware pending.

Authored image_regions.h normalizes non-addressable file holes to FF and rejects
non-FF writes there. 89F54/F58 FFFF is a source security cell, excluded from the
writable code image. 89E/V564RD banked images require a whole-image traversal.
Source nominal voltage and original rounded margins are retained, including
3.3V SST variants. Secure preserves earlier readable SST settings and verifies
them; other source profiles provide no independent lock readback. Arbitrary
configuration editing is unavailable.

Qualification: docs/audit-qualification/FLASH51_41_AUDIT.md and
 docs/data/flash51_41_qualification.json.
