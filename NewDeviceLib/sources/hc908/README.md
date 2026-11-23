# Ordinary HC908 QT/QY owner

Authorized cohort: 42 original Dataman 13.10 aliases, definitions D48–D4d.
Runtime code is portable C; original REZ payloads remain bring-your-own inputs.
The full source address image is 65536 bytes, with sparse Flash regions.

The 42-entry release is gated by the publication receipt described in
`docs/audit-qualification/HC908_QT_QY_PROGRESS.md`.
Do not include F_ routes or transfer physical test flags to source aliases.

Release checklist:

- [x] Original host setup, operation order, status and cleanup.
- [x] Source compact compiler evidence reused and current C comparison.
- [x] Both complete downloaded worker sets, UART, security and trim effects.
- [x] Three selected XC3030A configurations and package pin routing.
- [x] C operation owner with independent target and transport fault checks.
- [x] All 42 production GUI preparations and configuration/image behavior.
- [x] Preserve previous inventory; publish actual complete supported entries.

Linux corrections must be explicit: reject failed monitor security instead of
returning invalid Flash data, and preserve both factory trim bytes FFC0/FFC1.
Manufacturer references: NXP MC68HC908QY4 rev.6 and MC68HC908QY4A rev.3.

Flash-critical timing uses the authored target RAM helpers described in
`monitor_extensions.md`; the unsafe original mutation entries are not called.
Read, Blank Check, Program, Verify, Compare and Erase are available. Program
requires blank Flash. No standalone Secure/Configuration operation is claimed;
security and FLBPR reside in the complete sparse address image.

## Shared family extensions

The same portable C operation owner also has explicit source-bound JK/JL38
profiles and GP32/GT/JB/BD48 profiles (30 aliases,16 definitions). Their
publication seals and hardware status are recorded separately:

- `docs/audit-qualification/HC908_JK_JL_PROGRESS.md`
- `docs/audit-qualification/HC908_GX_JB_PROGRESS.md`
- `docs/audit-qualification/HC908_GX_JB_WORKFLOW.md`

Do not apply the QT/QY trim, Flash map or clock contract to those profiles.
GX profiles select their Flash/vector/protection regions, physical rows,
monitor authentication address, original UART helpers/FPGA/setup, byte timing
and mass-erase timing. JB12/JB16 requires200ms mass erase and uses an authored
long RAM loop; BD48 usesFE07 Flash control andFE08 volatile protection.
Original Windows payloads remain bring-your-own. All admitted profiles expose
Read, Blank Check, Program, Verify, Compare and Erase through C and normal GUI.
No hardware qualification is inherited from a similar part or package.
