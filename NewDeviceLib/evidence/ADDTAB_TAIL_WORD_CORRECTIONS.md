# AddTab tail word setup corrections

Authored C transformations in `sources/addtab-tail-word/` retain source REZ
resources as user-provided inputs; no original applet or FPGA bytes are bundled.

`init_sequence.h` moves the original FF preload under asserted RESET for exact
matched INIT shapes. `region_transitions.h` moves address setup after the
engineer-region entry so that nonzero special-region offsets survive. Both
transformations fail closed on unexpected input shapes.

The public admission is for these transformations covers Dc2, De3, Dcf, Dd0, Dd1, D13b and D13c from the pinned
Dataman 13.10 GDCAA resource. Other transform bindings are private candidates.
Evidence is in `analysis/addtab-tail92-batch/init-correction.json`,
`region-correction.json`, and the native/FPGA boundary receipts described by
`docs/audit-qualification/ADDTAB_TAIL_WORD_SPECIAL_REGION_BOUNDARIES.md`.
Hardware qualification is pending. Permanent protection is not exposed.

The private exact23 batch also binds Dbd/Dbe/Dc6/Dc7 to the Dc2 INIT and
region corrections. All four source routines match byte for byte; see
`analysis/addtab-exact23-next/correction-bindings.json`. These bindings do not
constitute public admission or hardware qualification.
