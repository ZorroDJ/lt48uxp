# RE2 timeout guard, revision 3

Authored source: `sources/at89/re2_timeout_guard.c`. No original firmware is
included. This resource does not itself enable a supported device.

The transformation accepts only exact original GETID/READ applet SHA-256 values
and load addresses. It appends 42 authored instruction bytes and reserves a
single latch byte at 5466 in the extended GETID image. A peripheral timeout sets
that byte to zero; original reply and recovery behavior is retained. Command 1
queries it; command 2 clears it. Setup and later successful reads do not clear it.
The native owner checks the bridge reply prefix before accepting the latch.

Offline evidence in the project:
- `analysis/at89-re2/timeout-guard-native.log`: 2411 input-boundary/hash checks.
- `analysis/at89-re2/timeout-guard-audit.json`: 2056 original/guarded execution cases.
- `analysis/at89-re2/fx2-query-audit.json`: 1538 bridge instruction cases.
- `analysis/at89-re2/resident-upload-audit.json`: 15 original upload/readback cases.
- `analysis/at89-re2/native-owner.log`: 996 endpoint-model workflow cases.

The endpoint tests cover cancellation during forward operations and cleanup,
protection transitions, settings restoration, dropped writes, timeouts and every
captured-response failure. They do not qualify hardware electrical timing.
See `docs/audit-qualification/AT89_RE2_AUDIT.md` for integration status and remaining gates.
