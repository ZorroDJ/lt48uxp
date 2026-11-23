# SST/Winbond Page memories

Authored portable C extension of the existing XP transport/compiler/runtime and
normal GTK operation path. Dataman 13.10 only; 36 exact names, six definitions
D9–De in GDBA___.rez, M6/M7, P0/P1/P2. Vendor resources are imported by the user.

- 64/128/256 KiB x8; 128-byte selected range alignment. P0/P1/P2 select
  16/17/18 address signals. The original package adapter is required.
- VCC 5 V (M6) or 3.3 V (M7). The shared source variable supplies programming
  and erase delays. Unsupported 3.63-V high verification is rejected.
- Program and Over-program both support EEPROM-style replacement, including
  0-to-1 transitions. The source internally uses AA/55/A0 page programming.
- Normal writes select source Software Protection=Disable; Secure saves the
  array and rewrites it with Enable. This is software data protection, not
  readout protection. There is no independent protection-status register test.
- Erase runs the original AA/55/80/AA/55/10 chip-erase handler, checks its status
  and independently reads the complete array for FF.
- Blank/Verify/Compare use actual Read bytes. Mutation verification includes
  both source margins and another Read following power removal. The original
  downloaded compare/blank applets remain bound but are not sole proof.
- Every exit attempts reset, source power-off, four independent DAC-off
  commands, idle-FPGA restore and Good/Error finalization. A failed read is not
  published. Cancellation is disabled for electrical cleanup.

`open.inc` binds the complete source-file SHA, exact catalogue identities,
geometry, family/pins, variables and operation download addresses. `owner.inc`
uses the original Page bulk route (48), with bounded page-aligned batches.
The source Page host also has a pagewise route; Linux chooses the bulk route
and propagates transfer failures instead of trusting the original final report.

Qualification and reusable role fingerprints are recorded in
`docs/audit-qualification/SST_PAGE36_AUDIT.md` and
`NewDeviceLib/drafts/sst-page36/knowledge.json`. No hardware, adapter, analogue
ramp or physical timing qualification is claimed for this batch.
