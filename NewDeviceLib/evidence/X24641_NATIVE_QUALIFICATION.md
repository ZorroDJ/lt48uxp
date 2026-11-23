# Xicor X24641 SO8 — native qualification

Date: 2026-09-29. New user-supplied SO8 chip, authorized for destructive testing
and final erased state. No original-content backup requested or saved.
Markings: top `X24641`, `E740`; bottom `D1AC`, `72796C`.
Selection: **Dataman 13.10 → Xicor → X24641 (5V)**.

## Result

**Passed; left all 8192 bytes FF.** Final Erase was followed by Blank Check,
a separate full Read and Verify at 4.75/5.25 V. All cleanups completed successfully;
the final operation selected Good and cleared Busy. No adapter reseating was
needed during this qualification.

Final FF SHA-256:
`7d2c7ac4888bfd75cd5f56e8d61f69595121183afc81556c876732fd3782c62f`.

| Test | Result |
| --- | --- |
| Initial Blank Check | All 8192 bytes FF |
| Full address-dependent pattern | Program passed; independent full Read exact |
| Verify / Compare | Both passed |
| Wrong expected byte at 8191 | Both rejected; exactly one mismatch at 8191 |
| Blank Check on programmed array | Correctly rejected |
| Full inverse pattern | Over-program and separate full Read passed |
| Seven-byte write at 4093 | Crossed 32-byte page and 0x1000 boundary; all neighbors preserved |
| Seven-byte write at 6141 | Crossed 0x1800 into WP-controlled upper quarter; full read exact |
| Erase | Full array rewritten FF |
| Final Blank / Read / Verify | Zero mismatches, all 8192 bytes FF |

Every writing operation additionally verifies the entire array at low/high
margins and reads it again after socket power removal. The full patterns and
independent reads establish working data/address transfers beyond an apparent
FF response from an absent or poorly seated chip.

WP was held low for writing. The separate behavior of inhibited writes with WP
high was not physically qualified. Secure is not applicable as a persistent
software operation. No programmer nonvolatile firmware, identity or security
settings were changed.

## Implementation and coverage

All seven memory operations are implemented in native C and routed through the
normal GTK interface. Reopen the application and choose **Xicor → X24641 (5V)**
under Dataman 13.10. The explicit voltage label excludes AG/AH-marked 1.8–3.6 V
parts. Erase requires no input image.

This is one native extension, not an original Windows catalogue entry. Its
[source/datasheet audit](../X24641_NATIVE_PROCEDURE.md) describes the M24C64
procedure reuse and two explicit PROGRAM operand changes for 32-byte pages.
The earlier X24645-family estimate does not apply to this corrected identity.
X24165, X24325 and X24645 remain unadmitted.

The generated list now has **945 complete entries**: 942 original catalogue
entries and three explicit native extensions. Only this sample receives new
hardware qualification evidence.

## Evidence

`analysis/x24641-integration/` holds the prior source archive, actual native
compiled waves, seven reference-compiler comparisons, individual operation
JSON/logs, test images and readbacks, SHA-256 hashes and regression logs.
The manufacturer datasheet was read from its archived HTML pages; the attempted
PDF download failed, and no local PDF is claimed.

Three relevant test suites passed: existing compact-I2C owner, X24641 owner,
and production GUI preparation. X24641 has 69 execution/fault cases, including
all 32 partial-page start offsets and deliberate bad readbacks. Existing compact
I2C has 37 cases. GUI preparation covers 42 operations across six entries,
including Erase without an input file. These are headless GUI preparation checks
and hardware tests of the same C backend, not mouse-driven GUI testing.
