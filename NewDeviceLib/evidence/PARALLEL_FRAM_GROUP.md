# Parallel FRAM C/GTK support

2026-09-28. Three exact Dataman 13.10 definitions use the extended native C XP
owner. Read, Blank Check, Program, Over-program, Verify and Compare are
implemented. Program permits arbitrary nonblank rewrites in both bit directions.
FRAM has no separate erase command: clearing means programming an all-FF image,
then verifying it. In the GUI, fill the full buffer range with FF and Program
that buffer. Secure is not applicable to these procedures.

| Source selection | Definition / family / pin map | Capacity | Nominal VCC |
| --- | --- | ---: | ---: |
| RAMTRON:FM18L08 *28SO | D30 / M21 / P19 | 32768 bytes | 3.3 V |
| RAMTRON:FM1808 | D31 / M22 / P19 | 32768 bytes | 5 V |
| Fujitsu:MB85R256 *28SO | D32 / M23 / P19 | 32768 bytes | 3.3 V |

Resource numbers above are hexadecimal. Admission binds exact class, definition,
family, pin map, width, capacity, package pin count and the original GDAC___.rez
SHA-256 `4abfb9024179d1cf38f17bc43ebfdaabaca4694b95e76e73edfdbbe7a0f46710`.
The FRAM parameter frame has one VCC variable; the Hitachi EPROM frame has eight.
Read and write use the original four 80C32 applets and FPGA image unchanged.
Source reset, power-off, independent rail shutdown, idle FPGA restoration and
Good/Error finalization are retained. Read output is published only after
successful cleanup. FRAM Program, Verify and Compare additionally perform a
fresh Read and compare actual bytes after the original controller verification.
A disconnected-adapter test exposed a false pass from the controller flag; the
independent readback guard prevents that case from being reported as success.
Those operations also finish with a fresh power/setup/read lifecycle, preventing
a same-session one-byte bus echo from being accepted as retained data.
This is a native robustness correction, not a proven original Windows bug.
Verify margins are relative to each source's nominal VCC. Original x86 function
00467420 was executed for both nominal voltages and all three source margin
modes (six cases). Its 3.3 V five-percent results are 3.14/3.46 V, including
the original rounding. The source's 3.63 V ten-percent high margin is rejected
for the 3.3 V definitions; the native ceiling is 3.6 V. Five-percent verification
is supported. The 5 V definitions retain 4.75/5.25 V and 4.5/5.5 V choices.

## FM18W08-SG compatibility work

FM18W08 is absent from this source catalogue. The Cypress/Infineon datasheet
specifies the same 32 KiB bytewide pinout and a 2.7–5.5 V supply range, with a
chip-enable transition for each access. The 5 V RAMTRON:FM1808 procedure is the
compatibility route under test, using a pin-to-pin SO-28 adapter. This is not
an invented original FM18W08 entry, nor an extra catalogue support count.

Source: [FM18W08 datasheet, document 001-86207 revision E](https://www.infineon.com/assets/row/public/documents/10/49/infineon-fm18w08-256-kbit-32-k-8-wide-voltage-bytewide-f-ram-memory-datasheet-en.pdf).
See [physical qualification](live-qualification/FM18W08_NATIVE_QUALIFICATION.md)
for the original failures and the 2026-10-02 retrial. The replacement adapter
passes A7 independence, but a partial read reproducibly leaves the next address
zeroed; this is no longer adequately described as only an adapter problem.
The cause remains unresolved; none of these entries gains a hardware-tested flag.

The loader also accepts the exact owned Dataman bridge 1.14 image at
`analysis/software-v13.10/fx2-13.10.hex`, after a complete binary-delta audit.
It is a volatile RAM upload. The GUI firmware field can select this file;
the existing legacy default remains available. Loading the matching firmware
did not resolve the physical failure in this session.

## Evidence

- 12 native/reference compact-wave compiler comparisons across three definitions.
- 54 complete native/reference endpoint comparisons: six operations, three
  definitions, full capacity, last 166 bytes and final byte.
- 108 injected transport cases cover rewrites, mismatches, short reads, cancellation,
  operation/cleanup failure and atomic read publication.
- Production GUI preflights cover all 18 FRAM operations alongside 90 existing
  Hitachi EPROM preflights.
- All 180 existing Hitachi endpoint comparisons still pass.
- Original x86 block helpers reuse the earlier 101-case XP host audit; complete
  endpoint comparisons are independent-model agreement, not a Windows capture.

Artifacts: `analysis/fram-integration/`. Reproduction tools:
`tools/audit_fram_resources.py`, `tools/compare_fram_transfers.py`,
`c/tests/test_parallel_fram.c`, and `tools/native_xp_live.c`.
Production operation code is C. Python is used only for offline analysis.

## FM16W08 comparison, 2026-10-02

A different 8 KiB FM16W08-SG in another 1:1 adapter also lost the boundary
sentinel and did not retain an entirely FF image across subsequent operations.
This weakens a single-chip/adapter explanation. It is a bounded compatibility
trial, not an additional admitted catalogue device. See the
[FM16W08 report](live-qualification/FM16W08_RETRIAL_2026-10-02.md).
