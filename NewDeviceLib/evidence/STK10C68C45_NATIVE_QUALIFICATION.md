# Simtek STK10C68-C45 — hardware qualification

Date: 2026-09-28. User-supplied ceramic/gold DIP28 device. User authorized
backup, SRAM writes, nonvolatile STORE/RECALL testing, and final FF contents.

## Result

**Passed; left all 8192 bytes FF**, independently read after STORE and a socket
power cycle. Cleanup succeeded, with all rails commanded off and the Good LED
selected. No programmer nonvolatile firmware or identity/security changes were
made. The device was tested only at nominal 5.0 V.

| Test | Result |
| --- | --- |
| Original backup and repeated full reads | Match; original array was all C0 |
| Four-byte SRAM write | Exact; other 8188 bytes preserved |
| Hardware RECALL after probe | Original contents restored exactly |
| Full address-dependent pattern | All 8192 bytes match independent read |
| STORE pattern; overwrite SRAM with inverse | Volatile inverse reads exactly |
| Hardware RECALL of stored pattern | All 8192 original pattern bytes restored |
| STORE inverse; socket power cycle | All 8192 inverse bytes retained |
| Write FF, STORE, socket power cycle | All 8192 bytes FF |
| Cleanup | Success; rails off, Good selected |

Three intentional STORE cycles were performed: pattern, inverse, and final FF.
The full-array pattern and its inverse exercise both bit states and addresses;
this is qualification of this sample and procedure, not an endurance test.

## Software scope

Initially qualified through a dedicated C utility, then integrated into the
production C owner and GTK during the same session. Select **Dataman 13.10 →
Simtek → STK10C68-C45**. Reopen the application from the existing desktop launcher
to load the new build. This is an explicitly named native extension, not an
original catalogue entry or an alias for STK11C68.

Read, Blank Check, Program, Over-program, Verify, Compare and Erase are now
available. Program/Over-program preserve unselected bytes, verify SRAM before
STORE, and verify all 8192 bytes after a socket power cycle. Erase uses that
same process with all FF. All operations use nominal 5.0 V; explicit low/high
Verify margin requests are rejected during preparation.

See [procedure and provenance](../STK10C68_HARDWARE_PROCEDURE.md) for the
datasheet-based /NE procedure and bounded reuse of STK11C68 SRAM resource waves.
The first read-only qualification attempt stopped during its second read because
the new runner omitted watchdog procedure boundaries. This was fixed before chip
writes. Its original failure and cleanup error remain in the historical log.

### Production owner and GUI integration qualification

A second hardware sequence exercised `lt48_programmer_execute_stk10c68`, the
same native owner called by GTK, with ordinary user images and operation ranges.
All operations and expected negative outcomes passed:

| Operation/check | Result |
| --- | --- |
| Initial Read | 8192 FF, as left by the first qualification |
| Program full new pattern | SRAM match, STORE, power cycle, full persistence match |
| Independent Read | Exact match to the new image |
| Verify and Compare | Both passed with correct image |
| Verify and Compare with wrong final byte | Both rejected; one mismatch at 8191 |
| Blank Check on pattern | Correctly rejected |
| Over-program four bytes at address 127 | Passed; independent full read confirmed all other bytes preserved |
| Erase | SRAM FF verified, STORE, power cycle, complete persistent FF verified |
| Final Blank Check and separate Read | Both passed; all 8192 bytes FF |
| Every operation cleanup | Passed |

This adds three intentional STORE cycles to the initial three. The sample was
left cleared after the production-owner tests too.

Five focused CTest suites passed: core, STK10C68 injected execution/failure tests,
seven STK10C68 production GUI preparations, and existing compact SPI owner/GUI
regressions. Eight STK10C68 fault/execution cases cover partial preservation,
SRAM mismatch before STORE, persistence mismatch, read failure, cancellation,
and failed-read buffer isolation. GUI preparation is tested headlessly; this is
not a claim of mouse-driven hardware operation testing.

Production integration artifacts: `analysis/stk10c68-gui/`, including the source
backup, build logs, regression log, per-operation JSON, images, and `live.log`.
All seven operations are implemented in C; Python only orchestrated the tests.

## Evidence

Directory: `analysis/stk10c68-integration/`.

- `initial/live.jsonl`: failed first read-only attempt; no writes or STORE.
- `read-confirmed/live.jsonl`: repeated matching reads and successful cleanup.
- `qualification/live.jsonl`: all 44 reported steps passed.
- `qualification/original.bin`: preserved original 8192-byte contents.
- `qualification/pattern-read.bin`, `inverse-read.bin`, `final-ff.bin`:
  independent full-array readbacks.
- `qualification/runner-used.c`: exact runner source used for this test.
- `image-hashes.json`: SHA-256 checksums; files independently checked against
  the expected pattern, inverse, and FF in an offline check after execution.
- `STK10C68.pdf`, `plan.txt`, `source-before.tgz`: datasheet, compiled wave
  listing, and pre-work source backup.

Original SHA-256: `51a6bbea2eadd7a12fe53afa5f9087955fd50f581e6c16cc260d4250b12d84ac`.

Final FF SHA-256: `7d2c7ac4888bfd75cd5f56e8d61f69595121183afc81556c876732fd3782c62f`.

The runner builds cleanly with `-std=c11 -Wall -Wextra -Werror`. The later integration checks are described above; no other Simtek device is
admitted by this qualification.
