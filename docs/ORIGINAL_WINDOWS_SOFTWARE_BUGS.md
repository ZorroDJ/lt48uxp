# Demonstrated bugs in the original Windows software

Updated: 2026-10-27

## Scope

This register contains demonstrated defects in the original Windows software. Linux implementation defects, incomplete analysis, unsupported native operations, and unexpected but potentially intentional Windows behavior do not belong here.

## Admission criteria

A finding is admitted only when the evidence establishes all of the following:

1. The affected executable/version and relevant resources are identified by hashes.
2. The original instructions exhibit the behavior, with a reproducible trace or original execution. A reconstructed implementation alone is insufficient.
3. The triggering state is reachable through the real application's inputs, resources, callers, or supported failure conditions. Arbitrary emulator states alone are insufficient.
4. The expected behavior has a concrete basis, and the observed behavior violates it with an identified consequence.
5. Calling conventions, operand meanings, resource binding, initialization, return-value interpretation, and relevant downstream checks have been checked. Mocked or bypassed code cannot be used to establish a consequence that depends on that code.

Each admitted entry must record its evidence, reproduction conditions, expected and actual behavior, impact, limitations, and any Linux compatibility decision. A deliberate Linux correction must be distinguished from exact Windows compatibility.

## Confirmed findings

Bounded findings are admitted below.

In particular, synthetic all-zero custom-voltage Verify pointers do not establish a real application bug: reachability remains unproved, and the audited GBA binding leaves those pointers unset. Short-transfer behavior in individual USB helpers also requires the complete caller/driver path and a demonstrated consequence before admission. An ignored return value or unusual status convention alone is not a defect.

## W1310-001 — Failed EPROM Read returns success and retains invalid data

**Status:** confirmed in original isolated execution, 2026-09-27. This is a
failure-propagation defect with visible error messages; it is not silent
corruption and does not establish the behavior of every device or version.

### Affected executable and resources

- Dataman 13.10 `WDATA48UXP.exe`, SHA256
  `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
- Demonstrated profile: AMD Am27C128, D1/M0, full 16384-byte normal UI Read.
- Definition SHA256
  `2e6261c867d6857786933c0d093e5bd669f70355cc218ff006c88c4fb919636b`.
- Owned resource manifest SHA256
  `354eabaacc9cbcd2d6afea674792b75610fc90c9b6534dbc5c9fe7133005f59d`.
  These hashes were re-extracted from the current owned resource audit.

### Reachable trigger and consequence

A normal UI Read passed startup, contacts and device ID (1/16). The first
163-byte raw data `DeviceIoControl` request received FALSE with zero bytes
transferred. No data was written into its destination. The original
application nevertheless published that block, advanced through the rest
of the image and returned **1** to its normal result handler. The handler
incremented the completed-operation count from 5 to 6.

The UI displayed both `Time-Out error !` and a power/disconnection error,
then **Read complete.** and checksum **000D1096h**. The retained image's first
163 bytes contain the untouched `BAADF00D` allocation pattern, rather than
bytes returned by the programmer. The complete 16384-byte original Windows
image was exported and independently checked, SHA256
`7bd80b6ab5fb8285915e3b32f3717136c4e075c658b60652562d7e4408179779`.

A full Read cannot be a successful complete result when its first required
block was not transferred and was never replaced with valid data. Showing
error messages does not repair the success return, counter increment or
retained invalid result. This establishes the consequence beyond an unusual
internal return-value convention.

### Checked original call chain

Addresses below use the PE's preferred base 00400000; the observed VM load
adds 005B0000.

- Normal Read: 0044F830; buffered reader: 004259D0.
- Raw read helper: 00496BB0. After its transport calls at 00496C22/00496C31,
  it ignores their returns, clears status at 00496C34..00496C36 and returns
  1 at 00496C3C.
- USB raw read: 0049D930; intercepted original driver call: 0049CA8E.
  0049D976..0049D984 ANDs the driver result with the sticky transport flag.
  The flag remained zero after failure; later raw transport results were
  therefore zero even when subsequent simulated driver replies succeeded.
- Live transport vtable read slot was bound to runtime A4D930, matching
  original 0049D930. Raw helper post-call stops showed result0 and requested
  count A3; the original higher helper then returned1.
- Original Read return at 004919A4 passed EAX=1 into 00425290. Its checked
  success branch at 004252B4..004252F3 increments the operation counter and
  invokes success completion. The final UI and exported buffer confirm that
  this branch was actually taken.

### Reproduction and evidence

The task directory is
`/home/ville/Documents/Codex/2026-09-24/there-is-a-chip-programmer-project`.

1. Run the unchanged original application in the isolated debugger VM with
   the simulated endpoint, select AMD Am27C128 and use normal UI Read.
2. At original Read entry arm task plugin command `lt48sim read,match,a8`.
   This serves the captured prefix and injects EAX0/count0 at transfer168.
3. Preserve original failure flags and caller instructions. Reply to the
   next 163-byte read with bytes01, then use `lt48sim faultdata,match` to
   serve the remaining99 observed raw reads with the recorded fixture.
   An early marker calls the manual reply a "retry"; publication at offset
   A3 proves it is the second chunk. It does not replace the failed first
   block. The actual interpretation supersedes that provisional label.
4. During reset, the original application requests a USB descriptor. Return
   the existing valid simulated descriptor. Do not patch the original Read
   result, error flags, completion logic or image.
5. Observe original return1, UI errors followed by completion, count6 and the
   invalid first block in the exported image.

Task verifier `work/verify-original-read-failure.py` checks the preserved
prefix, failure marker, remaining99 driver requests/replies, publication
bounds, original return and every byte of the exported image. Its report
`outputs/Dataman-VM-Am27C128-first-read-failure-capture.json` pins the hashes
of the direct traces, original executable assembly excerpts, VM plugin
binaries, resource provenance, immutable debugger logs and UI screenshot.

Important evidence includes debugger snapshots 20260927024458617324
(failure/first publication), 20260927024819531639 (live caller binding),
20260927024952158821 (sticky failure flag), 20260927025256253987 (final data
publication and descriptor request), 20260927025405242486 (return1), and
`Dataman-VM-Am27C128-first-read-failure-complete-ui.png`.

### Mocks and limits

All programmer traffic is simulated; physical programmer USB was absent
from Windows. Startup and descriptor replies identify a usable simulated
unit. The first data failure is the supported driver result FALSE/count0;
`GetLastError` is not overwritten or qualified. Later supplied data makes
this run deterministic. Original sticky failure logic, read results,
status updates and UI instructions were executed unchanged. The descriptor
reply models a device whose USB descriptor remains readable after a bulk
endpoint failure; it does not replace missing EPROM data.

The demonstrated scope is this executable/profile and failure contract.
Other error codes, short transfers, devices and versions require their own
qualification. No physical rail cleanup or security-app-let behavior is
qualified by this capture. The failure path differed from the success tail:
command writes stopped after the sticky failure, and reset attempted a
transport descriptor reopen. Complete native recovery/GUI integration
remains unfinished.

### Linux correction decision

Do not copy this error masking into Linux. A failed data transfer must stop
the Read and leave the result incomplete. The real C builder/executor/
collector probe `c/tests/test_dataman_oracle_read.c` now verifies the same
first-data failure: LT48_USB_ERROR, zero accepted data chunks and no complete
buffer exposed (collector_data returns LT48_BUSY). The ordinary C error
policy already does this; this adds a regression check tied to the original
failure. It does not qualify native cleanup or hardware/GUI operation.

## Entry format

- ID and concise defect description
- Product/version; executable and resource hashes
- Original function addresses and relevant callers
- Reachable trigger and reproduction procedure
- Expected behavior and its basis
- Actual behavior and demonstrated consequence
- Evidence files, probe hashes, mocks/bypasses, and independent checks
- Affected devices/operations and boundaries of the finding
- Linux compatibility/correction decision
- Status and date

## W1310-002 — M25P64 VDFPN Erase visits only the first sector

**Status:** confirmed bounded source-procedure defect, 2026-10-01. No claim
of silent Windows success: the downstream blank check can report failure.

- Executable: Dataman 13.10, SHA256
  `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
- Database: `GDBB___.rez`, SHA256
  `d2c4b6b43cff23dcfca67c92e4341a9e630322bb41850236ce83937228f5fab0`.
- Reachable catalogue entries: `Numonyx:M25P64 @8VDFPN` and
  `ST:M25P64 @8VDFPN`, D52/M34. Capacity and primary erase interval are both
  `0x800000`; Erase is W46, SHA256
  `7a4df3f230ebc4f0ef0b3381baa5f45b19ca6c7512261f1979f641ebf5f21348`.
- W46 selects D8 sector erase, passes the current address to the original
  W56 applet, and executes one command. It does not internally traverse sectors.
- The original worker at `0x467e80`, including its original range helper at
  `0x465e80`, executes that wave once at zero for the normal full-array range.
  Reproduction: `python3 tools/audit_serial_flash_erase_geometry.py`;
  evidence: `analysis/serial-flash-completion/d52-original-erase-traversal.json`
  and `erase-worker-x86.txt`. UI, transport and postcheck endpoints are mocked;
  the evidence establishes address traversal, not UI result propagation.
- Expected: all 128 sectors must be erased. ST's
  [M25P64 datasheet, description and Sector Erase instruction](https://media.digikey.com/pdf/Data%20Sheets/ST%20Microelectronics%20PDFS/M25P64.pdf)
  specifies 128 sectors of 64 KiB and sector-at-a-time D8 erase.
- Consequence: with nonblank data above the first 64 KiB, this source erase
  sequence leaves that data intact. The separate SO16 D53 entry uses bulk C7
  and is not affected by this finding.
- **Linux correction:** D52 traverses 128 intervals of 65536 bytes while retaining
  its exact W46/W56 procedure. Independent full-array readback remains mandatory.
  This is a documented correction, not byte-for-byte host compatibility.

## W1310-003 — Stacked Intel OTP lock calculation retains stale upper bytes

**Status:** confirmed bounded source-calculation defect, 2026-10-05. No claim
about full Windows UI result propagation or physical programming is made.

- Dataman13.10 executable SHA256:
  `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
- GDBAA__.rez SHA256:
  `b889a62b78b8d63028e47b3c4c008b86f245fa96322c4f73336a5f882f19fe28`.
- D3d/M26: Intel RD38F3340LLYD @88CSP and RD38F3350LLZD @88CSP.
  READCFG W6d SHA256:
  `191ce881fa5c085a9b878cfda51fda324f28dfc048bdb68ee302663e602879dc`.
- W6d reads two-byte first-bank OTP data into DataIn, combining low/high captures
  at00E0 and010F. It subsequently reads the second-bank scalar lock byte at0142.
  Original USB reader004972C0 overwrites only byte0. At0147 W6d ANDs only that
  byte with02, then tests the complete DWORD for zero at014F. With nonzero byte1,
  clearing bit1 cannot make the DWORD zero, so the source lock checkbox is
  calculated as unlocked. A normal erased OTP data word (FFFF) supplies that
  nonzero byte1; no uninitialised-memory assumption is required.
- Expected: this checkbox is the inverse of raw bit1, as shown by the mask,
  branch and checkbox assignment in the source procedure. Upper bits must not
  affect the result.
- Reproducer: `PYTHONPATH=src:tools .venv-productivity/bin/python
  tools/audit_page_remaining.py`. Original byte capture, mask interpreter and
  comparator run unmodified. Transport calls are stubbed. Of1024 combinations
  across four upper-byte states and all256 low bytes,384 produce the wrong
  original scalar result. The widened mask produces the expected result in all.
  The preceding source loop establishes reachability; this is not a claim that
  all synthetic upper-byte patterns occur on one physical part.
- **Linux correction:** NewDeviceLib's exact-source rule widens the masks at002A
  and0147 from one byte to four. It preserves the mask constant00000002 and all
  electrical commands. Independent lock and OTP-data readback remains required.
- Evidence: `analysis/page-wide/remaining/audit.json` and
  `NewDeviceLib/evidence/PAGE_WIDE_REMAINING.md`.


## AT45DB642D parallel protection reader: impossible masked-byte comparison

**Source-procedure defect; no claim of physical failure from this audit.** Dataman 13.10, `GDCA___.rez` SHA-256 `5b391bb2789baa1496eb5db708c3d867b8e25ec2b01dc7ea030eb9c9a84e5a39`, D26/M26, operation 6 / W8b. Executable SHA-256 `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.

The original compact instructions apply `buffer3 &= 0xF0` (`4b 01 03 f0 00 00 00 01`), then in the first-sector branch test `buffer3 == 0xFF` (`5e 03 00 ff 00 00 00`). No input byte can satisfy that equality. A valid protected-sector register value FF reaches this case but becomes F0. The C0, 30 and 00 alternatives also do not match F0, leaving the first two sector configuration fields unchanged instead of reporting both as protected. This is a reachable documented device value, not an arbitrary emulator state. The serial sibling W91 correctly tests F0 after the same mask.

The selected original compact compilation is independently compared between native and reference compilers in `analysis/adesto-batch/compiler-with-declared-defaults.json`. The unchanged source instructions provide the bounded arithmetic proof; the DataFlash model is additional integration evidence, not the basis for declaring this comparison impossible. Actual hardware waveform behavior and any other downstream consequence are not claimed here.

Linux deliberately corrects the comparison, and separately aligns the parallel register traversal with the documented byte order, using the authored exact-source correction in `NewDeviceLib/sources/adesto/at45db642d_config_read.inc`. Corrected native Secure/readback and ignored-write rejection are exercised in `analysis/adesto-batch/dataflash-execution.json`. See `ADESTO_MEMORY_BATCH_AUDIT.md` for scope and limitations.

### D86/M84/W177 TSSOP28 traversal variant, 2026-10-25

The 1,024-byte-page TSSOP28 definition has the same extra protection-register capture and stale sector31 comparison. W177 already fixes the first F0 comparison; it still masks the current byte in buffer3 and then compares the saved, unmasked byte in buffer25 against F0 for the last sector. Captured FF therefore reports that sector unprotected. The original x86 interpreter and an uncorrected portable C probe agree on the original result; this is a bounded source-reader defect, with no physical-failure claim.

The authored D86-only repair removes the extra capture and uses the current masked byte, leaving the original files unchanged. `analysis/dataflash28ts1/waves-original.json` records357 actual original/C callback cases; `waves-corrected.json` separately checks the declared repair through the original compiler/interpreter. An independent32-byte protection plus32-byte lockdown specification checks1,408 register patterns;132 differ from the unmodified reader. The complete native C Secure/readback and dropped-protection-write checks pass after the repair. See `NewDeviceLib/sources/dataflash28ts1/configuration_read.inc` and `docs/audit-qualification/DATAFLASH28TS1_AUDIT.md`. The full register traversal diagnosis uses modeled bus inputs; it does not establish physical bus timing.

## W1430-001 — cFeon status reader omits the applet selector

**Status:** confirmed original-resource/app-let command-sequence defect,
2026-10-06; no Windows GUI or physical-device consequence claimed.

AEC14.30 GDCA D1e6–D1e9 (EN25QX/QY256A), ReadCfg W480/W481 selects
W498 at 6000h. The applet consumes an initial data byte to choose its
three-/four-address-byte entry. These status waves omit it. CS is consumed
as that selector, WP is consumed as CS, and the status opcode is consumed
as WP. Executing the unchanged applet produces the wrong logical-pin sequence.
The normal main-array procedures supply the selector.

The actual operation bindings, source compiler correspondence, applet entry,
parameter consumption and malformed output are recorded in
[the correction evidence](../NewDeviceLib/evidence/AEC1430_CFEON_STATUS_SELECTOR.md),
including exact executable and GDCA hashes. Reproduce with
`tools/audit_aec1430_status_selector.py`; the proof uses original 8051 bytes,
not a rewritten status-handler algorithm. The trigger is the shipped ReadCfg
wave's normal command sequence. Correct status reads must address the source
pin map's CS/WP signals; the uncorrected sequence does not.

Linux deliberately inserts the missing nonzero selector at each of the three
status reads, under exact source/D/M/W admission. All four corrected register
round trips and full native workflows pass. Dropped protection writes remain
failures with cleanup. This finding does not establish what error text Windows
would display, nor any damage to physical hardware.


## W1310-004 — QT/QY source Flash worker exceeds the specified byte interval

**Status:** confirmed source-payload timing violation in original instruction
execution,2026-10-16. No physical device damage or measured waveform is claimed.

Affected: Dataman13.10 PE SHA256
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`,
GADB___.rez SHA256
`a9a45cce0fb16ce810a5f884b82a8e46afc7b0ccbd80ab6ab25b5a77613c3a3b`,
ordinary QT/QY D48–D4d source routes. Program host455E10 calls the shared
range procedure in program/program-verify mode; selected source bindings enter
Program worker539E. This is reachable through ordinary programming, not a
synthetic entry into unrelated data.

Original bytes at5512..5519 are `7B05 7FFF DFFE DBFA`. Actual8051 execution
reaches them with target FLCR PGM/HVEN asserted after a Flash data write. On a
classic24MHz80C32 the loop takes2566machine cycles, **1283us**, before additional
serial processing. The ordinary and A part data sheets require30–40us between
byte writes / the final write and PGM removal. This isolated delay already
exceeds that limit. No later host result handling can shorten that active interval.

Reproduce offline through `tools/run_limited_simulation.py` and
`tools/audit_hc908_workers.py`; its receipt records active-state visits, guarded
bytes and the cycle calculation for both geometry bundles. The original host
and compact bindings are checked by `audit_hc908_host.py`,
`audit_hc908_program_host.py` and `audit_hc908_compiler.py`. The FPGA audit also
checks the source target clock. Primary expected behavior: QT/QY Flash program
flow/timing in [ordinary](https://www.nxp.com/docs/en/data-sheet/MC68HC908QY4.pdf)
and [A](https://www.nxp.com/docs/en/data-sheet/MC68HC908QY4A.pdf) data sheets.

Impact: an out-of-spec programming interval; this evidence does not establish
an observed silicon failure rate or extend the finding to other MCU routes.
Linux correction: avoid original mutation entries and use authored target-RAM
code with instruction-timed pulses and independent readback. See the
[42-entry qualification](audit-qualification/HC908_QT_QY_PROGRESS.md).

Two additional worker observations (unchecked authentication and FFC0-only
restore) are recorded there with their limits. They are not separately admitted
as end-to-end Windows bugs without completing the downstream/caller proof.

## W1310-007 — ATmega A/non-P entries use P/PA signatures

**Status:** confirmed original CheckID callback/metadata defect, 2026-10-18.
Bounded to 13 Dataman13.10 package entries; Windows UI override behavior is not
established or ruled out by this finding.

### Affected inputs and trigger

- Original executable SHA256 `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
- `GAB____.rez` SHA256 `72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9`.
- Source routine `_FUN_CheckID`, role22/W1A1, SHA256
  `a6826226b34f147472ec5c30ab80aa85ead6b3f7c90957f8017485b8d88011b9`.
- Nine ATmega48A/88A/168A package entries, plus four older ATmega88/V and
  ATmega168/V TQ/MLF entries. Exact names, definitions, families, pins and
  request traces: `analysis/avr-atmega48/signatures.json`.

With the selected profile's CheckID routine and a documented physical identity,
the actual original interpreter returns zero for all 13 entries. Only changing
variable40 to the manufacturer's expected third byte changes each return to
one. All three capture bytes are supplied and consumed through the declared
pin-group transport boundary; no signature-comparison branch or return value
is mocked. The bound DEVINIT calls this routine when CheckID is enabled.

### Expected and observed values

| Device family | Manufacturer signature | Incorrect source third byte |
|---|---|---|
| ATmega48A | 1E 92 05 | 0A |
| ATmega88A / older ATmega88/V | 1E 93 0A | 0F |
| ATmega168A / older ATmega168/V | 1E 94 06 | 0B |

Microchip [AN2603 table5-1](https://onlinedocs.microchip.com/oxy/GUID-40C0ECBE-A0DB-4C41-A3E8-36526FC003B6-en-US-2/GUID-9962F7AD-84C2-4AB8-ACA4-8AAF009AD788.html)
and the [older-device migration table](https://onlinedocs.microchip.com/oxy/GUID-EDDC5F14-E7DB-4996-8AE5-E37AE12FEF7F-en-US-2/GUID-9962F7AD-84C2-4AB8-ACA4-8AAF009AD788.html)
establish the expected identities. The source values instead match P/PA
variants sharing the source definitions.

### Consequence, limitation and Linux decision

A correct published identity is rejected by the original profile's CheckID
callback. This does not demonstrate failed programming, damaged silicon,
licensing behavior, or inability to continue through a Windows Ignore/override
path. Physical replies are datasheet-grounded fixtures; no hardware was used.

The native implementation applies 13 authored, exact-name, SHA-bound signature
corrections through NewDeviceLib. P/PA aliases retain their own identities; the
original routines and vendor files are retained. This is an explicit Linux
metadata correction, not a claim of exact reproduction of the erroneous IDs.
Reproduction: guarded `tools/audit_atmega_signatures.py`; sealed receipt
`docs/data/atmega48_qualification.json`.

## W1310-008 — GBB positive-output PAL Blank compares an impossible byte

**Status:** confirmed in composed original execution, 2026-10-19. Limited to
MMI PAL10H8/-2, PAL12H6/-2, PAL14H4/-2 and PAL16H2/-2, definitions D2/D4/D6/D8,
M1/P0. This does not establish an analogue device model or a corrected algorithm.

Executable SHA256: `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
GBB____.rez SHA256: `85ca2609c5d2691cd435b8edd8550523d7bac5c0e8107b599f189692baeeafa7`.

The actual original parser supplies reachable cell masks; original M1 Init
loads VerifyFuse=FF. In mode11, original LOAD changes DataOut to FF. Original
004599F0 masks DataIn with the cell mask, then compares it with that unmasked
FF. Actual original USB group capture 00496C50 returns only four bits and
zeroes unused upper bits. Consequently no possible successful four-pin capture
can pass this comparison. The consequence is the original `Device is not blank !`
branch, rather than a status-code interpretation inferred from a decompiler.

Reproduction: guarded `tools/audit_pal46_capture_x86.py`,
`tools/audit_pal46_original_parser.py`, then
`tools/audit_pal46_composed_rows.py`. The composed tool executes source Init,
LOAD and row comparison using real first-cell masks, with every four-pin value
checked for PAL10H8 and the same branch checked for all four definitions.
Only allocation, storage, UI/progress and lower transport are hooks; they do
not decide the comparison. Receipts and limits:
[PAL hold closure](audit-qualification/PAL_46_HOLD_CLOSURE.md).

Linux decision: keep these four unqualified. Its previous isolated H-model
Blank pass did not prove original host/callback equivalence: the C owner did
not import callback-mutated expected data in mode11. No physical programming
or corrected-polarity qualification is claimed.

## W1310-009 — GBB PAL18L4 Blank tests a bit that capture cannot supply

**Status:** confirmed in composed original execution, 2026-10-19. Dataman13.10
GBB Dd/M0/P1/F8, with the executable/resource hashes above.

The actual original parser assigns 144 logical fuses to capture bit5. Both P1
output groups contain five pins. Actual CDrvUSB 00496C50 zeroes bit5 for every
possible capture. Original LOAD XOR FF therefore supplies bit5=1; original
Blank expects zero and rejects that real masked cell for all 32 possible
five-pin captures. No hypothetical contact failure, retained-byte assumption,
or device polarity model is needed for this contradiction.

The original parser and composed row tools named above reproduce this result;
`analysis/pal46/holds/original-composed-rows.json` records the 32 original Blank
comparisons. The earlier iterator fixture's uninitialized-group results and
failed double-hook composition attempts are excluded from the evidence.
Linux decision: retain PAL18L4 as unqualified; do not silently drop bit5 or
invent a replacement fuse map. See the [closure report](audit-qualification/PAL_46_HOLD_CLOSURE.md).

## W1310-010 — GBB PAL14L4's second-bank fuse map selects input pins

**Status:** confirmed source pin-map/physical-geometry defect, 2026-10-19.
Dataman13.10 MMI PAL14L4/-2 D5/M0/P0/F3 and PAL14H4/-2 D6/M1/P0/F3, with
executable/resource hashes above. Analogue hardware effects were not tested.

Original 0043AB00 plus 0043A470/0043A4A0/0043A180 produce F3's complete map.
For the second half of its 448 fuses, PLine8..11 selects P0 O2 and bit2/3,
which map to DIP pins13/12 after normal bottom-aligned placement. The MMI
PAL14L4 drawing explicitly identifies these as inputs. The required second-bank
outputs are pins15/14, with four terms per output. The independent JEDEC output
ranges in MAME's authored table agree. These are active fuses, not phantom
locations. A programming/read mask cannot turn an input pin into the required
output. F3 payloads agree across the previously compared AEC7.60/Dataman13.10/
AEC14.30 resources; only 13.10's instructions were executed in this new probe.

Reproduction: `tools/audit_pal46_original_parser.py` and
`tools/audit_pal46_pin_geometry.py`. Primary drawing: MMI1981 Bipolar LSI
Databook, PDF111/printed6-24; source URLs, hashes and all mismatching coordinates
are retained in the [closure report](audit-qualification/PAL_46_HOLD_CLOSURE.md)
and `analysis/pal46/holds/pin-geometry.json`. This finding does not depend on
reconstructing an alternative full input/product-term map.

Linux decision: withdraw previously admitted MMI PAL14L4/-2 from C executable
support and therefore the normal GUI's supported selection. PAL14H4 remains
unqualified. The matching-original digital target had consumed the same bad
coordinates and therefore could not detect this physical-pin error. PAL16L2's
complete independent output-pin comparison passed; it is not withdrawn.

### Linux closure for PAL W1310-008 through W1310-010

The confirmed original faults are retained as historical findings. NewDeviceLib
1.0.371 provides explicit authored corrections for the affected MMI PAL profiles,
including the separately resolved F2/F8 geometry holds. See
[PAL authored corrections](audit-qualification/PAL_46_AUTHORED_CORRECTIONS.md).
The original files are unchanged. The earlier startup-voltage suspicion was
disproved by tracing compact opcode00 as a voltage-variable selection and is
not recorded as an original-software bug.

## W1310-011 — PAL12H6/L6 F2 aliases physically distinct fuses

**Confirmed for the pinned Dataman13.10 GBB F2 and original host machinery.**
The actual original parser produces 96 duplicate coordinates in the 384-fuse
map. Actual original image conversion maps conflicting logical fuse48/fuse144
values01 and10 to the same round-trip11 result. These are distinct physical
product/input crosspoints in MMI's drawings and the independent JEDEC ordering;
they are not alternative names for one fuse. The source map also selects the
wrong physical output for288 logical positions, including48 input-pin positions.

Evidence: original parser, alias conversion and geometry receipts linked by
[hold closure](audit-qualification/PAL_46_HOLD_CLOSURE.md), and the independently
checked replacement in [authored qualification](audit-qualification/PAL_46_AUTHORED_CORRECTIONS.md).
Linux uses an authored384-coordinate bijection. This does not assert that every
other vendor release was dynamically executed.

## W1310-012 — PAL18L4 F8 aliases distinct fuses and references an absent capture bit

**Confirmed for the pinned Dataman13.10 GBB F8 and original host machinery.**
The actual original parser produces216 duplicate coordinates among720 logical
fuses. Original conversion collapses conflicting fuse0/fuse108 values01 and10
into11. The source additionally references bit5 for144 fuses although the
actual group capture supplies only bits0..4. MMI's physical array and independent
JEDEC ordering require720 distinct crosspoints; the authored replacement uses
those distinct coordinates within the five captured bits. The separate
W1310-009 finding documents the resulting impossible Blank comparison.

Evidence and Linux closure: [hold closure](audit-qualification/PAL_46_HOLD_CLOSURE.md)
and [authored qualification](audit-qualification/PAL_46_AUTHORED_CORRECTIONS.md).

## W1310-013 — enhanced AMD RP fuse maps use the wrong output stride

**Confirmed for Dataman 13.10 GBB F37/F38/F3a/F3b.** Executable SHA256
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`;
GBB SHA256
`85ca2609c5d2691cd435b8edd8550523d7bac5c0e8107b599f189692baeeafa7`. This is a mapping fault,
not a rejected parser construct. The actual original 0043AB00 parser and
0043A470/0043A4A0/0043A180 iterators reproduce every coordinate. Original
438AC0 encoding and438CC0 decoding merge conflicting logical values in the
three overlapping maps: 01 and10 both round-trip as11. The independent
12-case receipt is `analysis/pal29/map-audit/original-alias-conversion.json`;
the source-parser receipt is in the same directory.

F37, F38 and F3a repeat160,240 and320 coordinates respectively. For example,
F37 logical fuses1080 and1400 both select row0/input0/output3. Its product
stream distinguishes320-fuse registered blocks from360-fuse combinatorial
blocks, but its output stream advances every360 fuses throughout.

F3b has no aliases, but its nine-term stride is wrong for RP10: logical fuse320
selects product8/output0; the documented eight-term array requires product0/
output1. Its last AND output consequently receives no ordinary AND fuses.
The [AMD 1988 databook](https://ftpmirror.your.org/pub/misc/bitsavers/components/amd/_dataBooks/1988_AMD_PAL_Device_Data_Book.pdf)
pp5-280..283 and5-300..303 show the actual per-output product counts. RP4/6/8/10
have86/84/82/80 terms, forty inputs each, plus ten polarity fuses.

Linux uses authored, exact GBB-hash/D/M/P-scoped maps in
`NewDeviceLib/sources/pal29/maps.h`. Original assets and the generic decoder
remain unchanged. C operations and GUI JEDEC geometry share these replacements;
qualification is recorded in [PAL29 audit](audit-qualification/PAL_29_AUDIT.md).
No physical original-programmer failure or damaged device is claimed.

## W1310-014 — TI PAL20 A-series Secure requests14V instead of21V

**Confirmed parameter discrepancy for D24/M0e/Pa** in the same pinned
Dataman13.10 executable/GBB hashes listed in W1310-013, covering TI PAL20L8A,
PAL20R4A, PAL20R6A and PAL20R8A. Original source VSP is1400cV/channel2;
W42 lowers VCC and issues two35us pulses on package pins1 and13. Executing
the actual original interpreter confirms the14V DAC request. This original
trace remains in the immutable PAL29 stage1 evidence.

TI's [1986 TTL Data Book Volume4](https://ftpmirror.your.org/pub/misc/bitsavers/components/ti/_dataBooks/1986_TI_The_TTL_Data_Book_Vol_4.pdf),
printed3-25, specifies20..22V with nominal21V for security programming;
printed3-29 also labels both security pulses21V. Thus14V does not meet either
published voltage description. This is not a claim of observed silicon behavior.

The same TI publication is inconsistent about VCC: the explicit parameter limit
is0..0.4V, while the illustrative waveform labels6V. Linux retains the original
VCC-off sequence, consistent with the explicit maximum, and corrects only the
unambiguous pulse-voltage discrepancy to21V through the authored `pal29/voltage.h`
profile. No6V mode is inferred. The discrepancy and lack of physical security
qualification remain explicit in the PAL29 report. Other TI definitions have
not been changed by this narrowly scoped correction.

## W1310-015 — ATmega1281/2561 BOD configuration is shifted by one bit

**Confirmed boundary:** original Dataman13.10 configuration callbacks, isolated
x86 execution with a valid physical capture fixture; no hardware test or claim
about the complete Windows GUI result message.

Affected executable SHA256:
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`;
GAB____.rez SHA256:
`72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9`.
Bindings: Atmel ATMega1281/L @64TQ (Dc9/Ma0) and ATMega2561/V @64TQ (Dca/Ma1).
Both select W135 LoadConfig and W136 Protect. The selected configuration labels
identify fields16–18 as BODLEVEL0–2. W135 extracts physical bits1–3, and W136
writes fields16–18 back to physical bits1–3. The manufacturer's table30-3 places
BODLEVEL0–2 at bits0–2, with bit3 reserved.

Reproduction: execute the unmodified original LoadConfig with captured low/high/
extended/lock values62/DF/FC/FF, retaining its configuration object; then execute
the unmodified original Protect without an edit. Original flags16–18 become
0/1/1 instead of0/0/1. An independent decoder of the emitted raw pin transitions
records fuse writes62/DF/**FD**. Thus physical BOD0 is changed from0 to1; table31-6
maps these valid settings from nominal4.3V to2.7V. These are ordinary editable,
programmable fuse states, not an unreachable scratch value.

Evidence and reproduction: `tools/audit_avr31_bod_offsets.py`,
`analysis/avr31/bod-offsets/valid-levels/original.json`, its two complete traffic
files and immutable `release/` input copies. The original code, resource binding,
bit extraction, configuration object and output pin traffic are retained.
External capture and transport boundaries are fixtures; the emitted byte defect
is directly observed, while the physical threshold consequence follows the
[manufacturer datasheet](https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf).

Linux decision: implemented and qualified in NewDeviceLib1.0.391. A narrowly
scoped authored correction changes the three read and three write bit offsets.
Original assets are retained. Corrected callbacks passed original-x86/reference/C
comparisons and independent target checks; both entries are admitted in C/GTK. This deliberately differs from the defective
configuration mapping; it is not claimed as exact Windows compatibility.


## W1310-016 — ATtiny22/L default signature expects the wrong third byte

**Confirmed boundary:** Dataman13.10 GAB____.rez D50/M3d/P0c default identity
variable40 and original W49 CheckID callback. No claim about a full Windows GUI
session or other software versions; no physical device test.

The source expects1E9103; the ATtiny22/L manufacturer specification lists
1E9106. Unmodified original executable CheckID, with declared pin samples for
1E9106, returns failure using the original03 value and success changing only
variable40 to06. All24 input bits are consumed in both trials; the final captured
byte is06. This is a source definition error, not inference from similar names.
Executable and REZ SHA256 are the same pinned13.10 files listed in W1310-015.

Reproduction: `tools/audit_avr35_corrections.py`; exact inputs, captures and
requests are preserved in `analysis/avr35/corrections/original.json` and the AVR35
final qualification archive. The manufacturer publication is preserved as
`analysis/avr35/datasheets/tiny22.pdf` with its source/hash index.

Linux applies only the exact-source/key correction in
`NewDeviceLib/corrections/avr35-tiny22-signature.json`. Original assets remain
intact. The AVR owner rejects this entry if the correction is absent. Corrected
original callbacks are compared against C; an independent target model supplies
the documented identity. Hardware qualification remains pending.

## W1310-017 — ST62x30B/x32B option Read and Write interchange two fields

**Status:** confirmed in original isolated x86 execution, 2026-10-20. This is
an emitted-option-byte defect; no physical chip or full interactive Windows GUI
run is claimed.

**Affected bindings:** Dataman13.10, class2012, family M5C (decimal92):
ST62E30B, ST62E32B, ST62T30B and ST62T32B. The source constructor/vtable are
004553A0/0077FE78. Executable SHA256:
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
Exact executable/resource hashes and original controller-write digests are in
`analysis/st62-65/stage3/m92-roundtrip.json`.

**Reproduction:** execute the original power/entry and Read-reset callback with
a valid chip option byte, retain that same configuration object, enable only
its PROTECT checkbox (field18), then execute the original power/entry and
PROTECT callback. DEVINIT is executed between the Read and Write and is checked
not to change the configuration choices. Chip captures and widget property
storage are fixtures; decoding, option assembly, nested procedures and emitted
controller writes execute the original instructions.

| Chip option read | Only requested addition | Expected master byte | Original emitted master byte |
| --- | --- | --- | --- |
| 04 | PROTECT, bit4 | 14 | 18 |
| 08 | PROTECT, bit4 | 18 | 14 |

The original reset decoder stores physical bit2 in field15 and bit3 in field16.
The writer places field15 at bit3 and field16 at bit2. The original callbacks
return success with a matching response for their incorrectly assembled byte;
the source's write verification compares against that byte, rather than the
options retained from Read. The master location is segment5C, window40
(normalized option address1700); shadow writes are recorded separately.

**Expected behavior and consequence:** changing read protection alone must not
change another decoded option. ST's [ST62T30B/E30B datasheet, page15](https://www.st.com/resource/en/datasheet/cd00018962.pdf)
defines independent TIMER pull-up (bit2), NMI pull-up (bit3), and PROTECT (bit4)
settings. The original payload asks for the wrong pull-up setting. The actual
result on already programmed OTP cells depends on the physical programming
state; this finding establishes the wrong request, not a measured chip result.

**Linux decision:** authored NewDeviceLib option metadata retains separate
reader and writer maps. The C owner translates the desired physical choices
through the writer map and confirms them through a fresh source Read after
power removal. The original REZ procedures are preserved. All four aliases
share this correction. Native wire tests use different pull-up patterns and
require every unselected decoded option to remain unchanged.

**Reproduce offline:** run `tools/audit_st62_65_m92_roundtrip.py 3` through the
project's `run_limited_simulation.py` resource guard. The receipt is preserved
with the ST62 qualification evidence when that batch is sealed.

## Dataman 13.10 EPM pin-group writer sends an uninitialised tail

**Confirmed scope:** original transport helper `004976A0`, called from compact
runtime `00440799` through vtable slot `6C`. The EPM7064S @68 setup uses P0D
with an unmapped `FC` entry. Instructions `004976D2..004976DA` skip entries
`FC..FF` while building the output array, but `004976E7` passes the original
entry count to the raw writer. Thus the transfer includes bytes that this
function did not initialise. This is an instruction-level count discrepancy,
not an inference from a failed chip test.

**Observed offline:** the S-family replay produced the valid prefix
`0c10141c24303438404460686c7074787c90989c`, followed by stack byte `52`,
where the uncorrected C implementation had emitted sentinel `FC`. The final
byte is not a repeatable socket command and is not qualified as such.

**Linux correction:** runtime group writes filter `FC..FF` and use the number
of emitted bytes. The original procedure resource remains unchanged. The EPM
replay executes the original helper and explicitly excludes its uninitialised
tail from the equivalence comparison. Electrical effects of that original
extra byte have not been measured. See `tools/epm44_replay.py` and the EPM44
audit for admission status; this finding alone does not qualify a device.

## Dataman 13.10 ATV2500B ignores the JEDEC security request during INIT

**Confirmed bounded software defect, 2026-10-21.** This concerns importing the
JEDEC request into the Program options. It does not establish a measured
silicon protection result or every possible Windows option combination.

**Affected inputs:** executable SHA256
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`;
`GB_____.rez` SHA256
`913000514454bd9ee191bc4167efa52aff3969ff46e84081d9ddfbe2635c6f7f`.
D12/Md/P9 and D13/Me/Pa cover the four ATV2500B and BQ/BQL package aliases.
Their Fd map places the JEDEC security request at index 71648, type 10,
row 39, column 191, bit 10.

**Reachable trigger and checked semantics:** use a normal 71,745-bit image
with that request bit set and Secure initially unchecked. Original map
construction gives two data bytes per cell, with six bytes in its complete
record. Expanded 6C copies the two **data** bytes into BUF0 at offset zero.
With normal clearing enabled, the real marked-buffer reset clears BUF0;
only buffer 7 is exempt. Compact
W2e/W36 then read BUF0 bytes **4..5** into BUF1 at byte offset 0013h. Those
bytes are padding, so the conditional never sees the requested 0400h bit.
The later copy at offset 0033h also writes bytes 4..5 rather than the data
bytes consumed by expanded 6E.

**Original observation:** 104 actual original/C callback comparisons include
valid full host images with the request clear and set. The unmodified set
case leaves BUF7 at AA, does not set current-object field 5, and leaves the
host image unchanged. The native INIT diagnostic records the same result.
The current-object helper writes field 5 at Kernel+2Ch; original constructor
00424D50 binds Program options to Kernel+1Ch. Thus this is exactly the
0020h flag checked by original Program 004447E0 before its delayed Secure
stage. With that option initially clear, INIT does not enable the stage
from the file request. We do not infer a whole-application outcome when the
operator separately selected Secure or other UI code changed the option.

**Preserved evidence:** `analysis/peel46/original-security/original.json`
hashes an immutable private copy of the unmodified executable/resources,
compiler/callback receipts, C sources and native diagnostic. Map construction
and reset behavior have separate original-execution evidence. The final
PEEL46 qualification adds corrected callback comparisons and eight original
Program-controller cases for a request introduced during Program, including
Program, Verify and Secure failures. The stage bodies are tested separately;
controller mocks alone are not used to establish the buffer-offset defect.

**Linux correction:** authored NewDeviceLib
`sources/peel46/corrections.h` changes only W2e/W36 operand bytes 0017h and
0035h from 04h to 00h, for the two exact definitions. The original library
is unchanged. The normalized image and flag survive subsequent callbacks;
Secure runs only after successful array programming and verification. The
GUI explicitly announces a file-requested Secure operation. Its audit label
is `peel46_security_corrected_audited`, distinct from unmodified source
compatibility. Hardware qualification remains pending.

## W1310-018 — Macronix W24f takes CR.TB from Factory Lock

**Status:** confirmed in original compiler/interpreter execution, 2026-10-22.
Dataman 13.10 executable SHA-256:
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
GDCA SHA-256:
`5b391bb2789baa1496eb5db708c3d867b8e25ec2b01dc7ea030eb9c9a84e5a39`.
W24f payload SHA-256:
`83d3b820f20e761f18027472c91236bb7bb3ea49c2c0523a7664a9b480f20f95`.

Configuration25435 names checkbox5 TB and checkbox6 Factory Lock; ReadCfg W244
also populates them separately from CR.bit3 and SecurityRegister.bit0. Protect
W24f instead reads checkbox6 into the byte supplied as CR. These independently
reachable register values therefore create a counterexample: TB=1/factory=0
produces CR.bit3=0; TB=0/factory=1 produces CR.bit3=1. The original x86 compiler
and runtime execute the unmodified callback and reproduce both results. The
unchanged W250 sibling reads checkbox5 and passes all four value combinations.

**Affected entries:** MX25L25672F and MX25L25773G, each SO-16/WSON (De0/De1).
**Impact:** the protection callback composes the wrong top/bottom selector and
passes that byte to the original register-writing applet. This is a field-selector
error, not an inferred failure based on similar call graphs. Physical effects
and execution of the complete Windows GUI were not tested.

**Expected basis:** actual source schema/ReadCfg, sibling callback and Macronix
CR.TB bit definition. Evidence: analysis/spi-mac29/tb-original.json and
analysis/spi-mac29/tb-correction.json. The latter uses the actual original
compiler and interpreter plus independent corrected-native register effects.

**Linux decision:** exact source-locked authored correction in
NewDeviceLib/sources/spi-mac29/tb_selector.inc changes selector6 to5 at57h.
The source file remains unchanged. The other25 batch entries are not patched.
See [the batch audit](audit-qualification/SPI_MAC29_AUDIT.md).

## W1310-019 — W25M C2 die-select frames do not deassert chip select

**Confirmed source behavior; hardware effect not yet tested.** Dataman13.10
GDCA W489,W48c,W48d,W48e,W48f,W490 end C2+die with the same CS-low
command that began the frame. Actual original x86 compiler/interpreter output
confirms the missing high boundary; fitted FPGA GPIO binds that command to CS.
Two C2 selections merge into one frame in the independent SPI endpoint. A
native upper-die write then verifies against the wrong lower die; independent
bank sentinels detect it. This distinguishes source behavior from a presumed
success based on status alone. No physical chip failure is claimed.

**Linux:** exact-source authored framing correction; explicit per-segment die
selection and independently preserved/verified protection on both dies. See
[the54-entry audit](audit-qualification/SPI_54_AUDIT.md). Vendor assets unchanged.

The independent [fitted die-framing witness](data/spi54_die_fitted_qualification.json)
now follows complete original/corrected MCU streams through actual XC3030A
CS/MOSI pads, including D4/D8 effects: original C200C201 is one frame; corrected
C200 and C201 are separate. No physical-silicon outcome is claimed.

## W1310-020 — XMC-B information-row erase emits an incomplete instruction

**Confirmed source behavior; physical silicon outcome untested.** Dataman 13.10
GDCA definitions De4/De5 (XM25QH256B WSON/SO-16) bind erase callback W23f.
Its security branch selects applet role26 (W25a at A000), a one-opcode helper,
then supplies an erase timeout and address bytes that this helper does not
transmit as an address. Actual original x86 compiler/interpreter output fed
through the unchanged original 8051 handler produces five separate `64h`
frames, without address bytes or a fresh WREN in those frames' sequence.
This is an observed command/framing defect, not a similarity-based inference.

XMC's [Rev.G datasheet](https://www.xmcwh.com/uploads/424/XM25QH%28QU%29256B_Ver.G.pdf),
section8.39, requires Write Enable followed by 64h and three address bytes,
with chip select held low for the whole erase instruction. Its information
rows are at000000,001000,002000,003000 (section8.38).

**Linux:** a source-locked authored compact fragment replaces only the security
branch in W23f for De4/De5. At the first security boundary it sends four complete
WREN/IRER sequences and polls WIP using the original status reader and bounded
interpreter machinery. Later security boundaries do not repeat those erases.
The normal-array branch and all vendor files stay unchanged. Only the authored
replacement fragment is bundled in NewDeviceLib; original callback bytes are
required from the user's installation. Locked nonblank rows fail independent
readback. See `analysis/spi-23/xmcb-original.json` and
[the batch audit](audit-qualification/SPI_23_AUDIT.md).

## W1310-021 — 256-Mbit Micron OTP callbacks misselect dual-entry applets

**Confirmed source/ABI behavior; physical silicon outcome untested.** Dataman
13.10 GDCA definitions D19d/D19e/D19f (MT25QL256ABA) and Df7/Df8/Df9
(N25Q256A13) bind dual-entry read/blank/program/verify applets for four-byte
main-array access and three-byte OTP access. Their first host byte selects the
entry. Read and Verify/Compare select the four-byte entry while providing
three OTP address bytes. Blank Check similarly uses a common four-byte call
for OTP addresses. ReadCfg omits the selector (MT25) or selects the wrong entry
(N25Q); the OTP-lock subcall in Protect omits it altogether.

Actual original x86 compiler/interpreter witnesses cover both callback families,
five roles and original/corrected forms (20 witnesses). The actual original
`00440450 -> 00440540` start path was also executed: it sends only the five-byte
`3B AA 55 <entry-low> <entry-high>` controller packet, with no implicit selector.
Both branches of the unchanged 8051 bootstrap were executed. Thus the missing
selector is consumed from the following chip-select parameter; the inconsistent
address calls misframe or shift OTP traffic. The failed native invocation and
read shifted by one byte are retained separately from passing corrected tests.
A mocked start boundary alone was not used to establish the defect.

**Linux:** authored source-locked compact edits choose the existing three-byte
OTP entry, and Blank Check supplies three address bytes for OTP while retaining
four for the main array. Four-byte main-array operations remain unchanged.
Read, Blank Check, Verify/Compare, ReadCfg and Protect are corrected together.
Vendor REZ files, applets and FPGA images remain unchanged and bring-your-own.

Evidence: `analysis/spi-26/original-otp.json`, `startup.json`,
`compiler-original-unmodified.json`, failed attempts, and
[the batch audit](audit-qualification/SPI_26_AUDIT.md). Executable SHA-256:
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`;
GDCA SHA-256: `5b391bb2789baa1496eb5db708c3d867b8e25ec2b01dc7ea030eb9c9a84e5a39`.

## W1310-022 — Legacy W78/W77 erase restarts the successful blank-check segment

**Status:** demonstrated in isolated execution of the original 13.10 compiler and interpreter, 2026-10-23. Physical programmer behavior and a complete interactive Windows run remain untested.

**Affected source:** Dataman 13.10 `GDAE___.rez`, SHA-256 `9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36`; the shared `_FUN_ERASE` resource is 203 bytes, SHA-256 `7f0ba8d2ee81cb07da1382283deff8856f3fceeeeb7d62e090afd82deb531dda`. It is bound by the sixteen definitions behind the 90 legacy W78/W77 entries. Executable SHA-256: `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.

**Reproduction and reachability:** select W78E51B (4 KiB, D69) or W77E58 (32 KiB, D6f), invoke the source Erase callback in mode 7 with the selected device's byte capacity in `_BUF_ChipSize`, and supply successful blank status `01`. The original unmodified 8051 blank worker independently produces `01` for matching FF samples. The resource's erase binding and XP host route are checked; capture arguments use the original buffer-pointer ABI. Only the external transport/status is supplied by the fixture.

**Observed control flow:** the original compiler and interpreter execute `Counter = ChipSize` inside the retry loop. After the blank worker reports success, the counter reaches zero for 4 KiB or 16,384 for 32 KiB, then is initialized again on the next iteration. Three bounded original observations give `[0, 0, 0]` and `[16384, 16384, 16384]`; the harness stops at the third repeated status. This establishes a repeated successful segment in this callback, not the behavior of an entire Windows session after an external timeout.

**Expected behavior:** a successful segment reduces the remaining byte count, and a fully blank device finishes. For these capacities the intended 16 KiB segmentation needs one and two successful statuses. The source's decrement and zero-counter exit implement that intent, but repeated initialization prevents it.

**Linux correction:** a hash-scoped authored transformation in `NewDeviceLib/sources/w78legacy90/erase.inc` moves the counter initialization immediately before the retry loop. Source pulse commands, retry limit, blank worker, address handling and delays are retained. The original compiler/interpreter running the authored resource finishes with counters `[0]` and `[16384, 0]`. Full-array independent raw readback still checks erase success; the callback's constant-success output is insufficient.

**Evidence:** `tools/audit_w78legacy90_erase_counter.py`, `tools/audit_w78legacy90_applets.py`, and the immutable [first-stage qualification](data/w78legacy90_first12_qualification.json), whose archive retains the original/corrected compiler/interpreter trials. No conclusion about other software releases is made.

## Dataman13.10 SyncMos A2 watchdog forces LVD on

**Confirmed offline:** GDAE SM59R08/16A2C/L W14a stores `watchdog_index - 1` when the watchdog is enabled, and clears bit6 when LVD is selected. It never sets bit6 for LVD disabled in that branch. W149 decodes bit6 clear as LVD enabled. Thus selecting watchdog1 and LVD0 writes byte00, which reads back as watchdog1/LVD1. Original x86/compiler/runtime equivalence is included in the SyncMos A2 callback audit.

**Linux correction:** `sc4a_source_options` supplies bit6 in the internal watchdog arithmetic input when LVD is disabled. The original source pulse sequence is retained. All612 Protect/LVD/WDT/Oscillator combinations are checked through the source encoder and decoder in `tools/audit_syncmos4a_options.py`; no claim about other releases.

## Dataman 13.10 GMS97C8032 second protection loop emits no pulses

**Confirmed offline in the original callback, 2026-10-27.** GAF definition De8 / family M76 binds PROTECT We6. With the original protection selector set to 2, the original x86 compiler/interpreter emits 25 LB1 pulses and no LB2 pulses. Selector 1 also emits 25 LB1 pulses; selector 0 emits none. This is an isolated original callback result, not a physical-chip observation or a complete interactive Windows test.

The affected executable SHA-256 is `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`; GAF REZ SHA-256 is `126dcd7dd7252b5af78e151d34106f0c17b5e52575b9606a80b9f615f912c123`. The We6 payload SHA-256 is `110ce6743135bf4103049690d438786678fc05c5141a4c774555fc6f8b21970d`.

**Cause and consequence:** the selector-2 branch contains a 25-iteration LB1 loop followed by an LB2 loop with the same counter and limit. The first loop leaves counter10 at25; the second loop does not initialize it. The original loop helper therefore skips the LB2 body. This is not an inferred difference between two decompilations: original execution, request bytes and all70 scratch buffers are compared with the C interpreter. The bound original PROTECT resource explicitly contains the selector-2/LB2 branch, and the original configuration helper supplies the selected value. The intended second lock train does not reach the transport.

**Linux correction:** retain the original PROTECT callback, then for selector2 execute an authored LB2 train with an initialized counter. The independent native target checks25 LB1 plus25 LB2 pulses and both lock bits. The original resource is unchanged. See `NewDeviceLib/sources/hyundai24/second_lock.h`, `tools/audit_hyundai24_waves.py` and [Hyundai24 qualification](audit-qualification/HYUNDAI24_AUDIT.md). Other releases and silicon behavior are unqualified.

## Dataman 13.10 FM8P56 blank worker compares incompatible high and low bytes

**Confirmed offline at the original worker boundary, 2026-10-28.** GDACA D9/M7 uses RAM W14 at7800. Its command6 handler stores one expected byte. The Blank loop compares both captured low and high bytes with that same byte; the high-byte read is XOR-inverted and masked to five bits. An erased 13-bit word is FF/1F, so no one-byte expected value can match both bytes. Executing the unchanged worker with expected FF and erased samples returns a mismatch (00/02/00), rather than success. This establishes the worker comparison defect; a complete interactive Windows Blank Check and physical hardware were not tested.

The original Read and Program workers also explicitly mask the high byte to1F. Feeling Technology's FM8P54/56 Rev1.21 (May2005), page4, independently specifies1024x13 program memory. D9's 12-bit metadata is inconsistent with both. This metadata inconsistency is not proof that the Windows host discards bit12.

**Linux correction:** the exact finite NewDeviceLib FM8P56 geometry binding reports13bits in two-byte storage. Blank Check uses the qualified original raw Read path and an authored FF/1F comparison. Source worker bytes are retained unchanged. See `tools/audit_fm8one_workers.py` and `docs/audit-qualification/FM8ONE_AUDIT.md`.

## Dataman13.10 W29C101/102 page-worker timeout returns success

**Confirmed isolated worker defect; full Windows final-operation outcome not established.** GAAC M46/M47 Program9 is the exact277byte Wb4 payload atA000 (SHA256 e43d6aa819c167b5c16617d88d6e2490b7c373ace347a3ab568a829b9e15d27e). AtA0F5 a matching data-poll bit branches toA0FE. With32mismatches and32timer expiries, DJNZ R7 atA0FC also falls through toA0FE. Both paths emitC800=1. The zero-result block atA109 has no incoming branch from the worker entry.

`tools/audit_w29wordtwo_workers.py` executes both cases across five page boundaries and three patterns for both pin maps. Peripheral-ready, timer-expiry and target-poll values are explicit inputs. This proves the worker status defect without claiming that a physical programming failure or false final Windows success has been observed.

**Linux policy:** retain the exact worker and source FPGA; consume its completion byte, reject an explicit zero/short capture, and always require independent margin and fresh-power raw Read verification. A dropped write is tested separately from the result-byte fixture. See `docs/audit-qualification/W29WORDTWO_AUDIT.md`.

## W1310-023 — ST72F34x downloaded option reader loses its row index

**Status:** confirmed in isolated original monitor execution and the original
Windows configuration callback, 2026-10-28. No complete Windows GUI run or
physical-chip failure is claimed.

### Affected assets and reachable input

Dataman13.10 executable SHA256
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`;
`GADB___.rez` SHA256
`a9a45cce0fb16ce810a5f884b82a8e46afc7b0ccbd80ab6ab25b5a77613c3a3b`.
Definitions D7c/D7d/D7e bind M117/M118/M119 and W209/role18 at56E9,
1036bytes, SHA256
`978c50c89692d387bc35ffec719b6750e904ea2c8dd5f67a2b1dbe124c7cace2`.
The original 8051 instructions download the same ST7 option-reading monitor
at0085 for all three package variants.

F7/FC are the first two option values installed by the original Program INIT
recovery branch. They are independently encoded in the physical option row;
this is an application-created state, not an arbitrary CPU-register fixture.

### Original behavior and consequence

The option loop uses X as a descending row index and Y as its decoded result.
At the first-byte transmit, X is0F. The original CALL00D0 transmits through a
subroutine that loads X=8 and decrements it to0. It returns without preserving
X. The caller's following BRA resumes the option loop with X=0 rather than0F.
The second byte consequently uses one pair rather than eight and visits an
address outside the32-byte row.

For the original F7/FC defaults, the second result is EE or EF, not FC. Both
possible outcomes of the out-of-row comparison were exercised, so this
conclusion does not require choosing a particular physical alias behavior for
that address. The corrected reader returns FC. Source-monitor trials with
other independent option patterns reproduce the index loss.

A separate joined test feeds the original monitor's returned82/A5 values for
an82/A4 row through the **actual original x86 CFG callback**. It stores A5 in
numeric option field1. Thus an incorrect decoded option can reach the
application's configuration state. This does not establish what every later
GUI action, programming operation or downstream check does with that value.

### Linux correction and reproduction

The C loader changes only the two reversed embedded bytes corresponding to
`BRA -35` after the first transmit, replacing that branch with `LD X,#0F`.
The existing TNZ/JRPL resumes the intended loop. The source archive is retained
unchanged. All actual C-uploaded workers are compared against their source
bytes, allowing only this finite repair.

Reproduce under the project resource guard:

- `tools/audit_st7twelve_options.py`: original/corrected monitor trials.
- `tools/audit_st7twelve_option_index.py`: F7/FC and both outside-row outcomes.
- `tools/audit_st7twelve_option_consequence.py`: original x86 field update.
- `tools/audit_st7twelve_bindings.py`: actual C-uploaded worker bytes.

Results are in `analysis/st7twelve/stage1/`; the sealed ST7 qualification
preserves exact inputs and receipts. GPIO readiness, programming completion
and physical-cell behavior remain declared model boundaries.
