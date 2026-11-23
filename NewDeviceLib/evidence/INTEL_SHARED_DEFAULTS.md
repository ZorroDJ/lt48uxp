# Intel/Sharp exact pin-default guard

Authored C policy for D18/D19/D1a/D82/D83/D86/D87 of the pinned Dataman 13.10
GDABA__.rez. The owning C loader verifies the full original resource SHA256 and
exact definition/pin map/package/capacity closure before calling this guard.
The guard validates every defaulted pin's number, source byte offset, opcode and
classification. It does not enable arbitrary zero-filled lookups.

D18/D19/D1a source VCCQ through the odd bank to unused ZIF1 on 44-pin profiles.
D86/D87 source VPPL through UDN to unused ZIF1 on 42-pin profiles. D82/D83 issue
resident 1C01, which original-resident execution proves has no hardware writes.
All other defaults in these exact contexts are resident digital no-ops.

See docs/audit-qualification/INTEL_SHARED_WORD118_BATCH_AUDIT.md and the reproducible traces in
analysis/intel-shared-word118-batch/gaps.json. Exact original adapters remain
required. No hardware qualification is claimed.
