# Catalyst CAT28C512P — native qualification

Date: 2026-09-28. User-supplied new DIP32 device. Destructive testing authorized;
no original-content backup requested or saved. Native selection:
**Dataman 13.10 → Catalyst → CAT28C512P**.

## Result

**Passed; left all 65536 bytes FF.** Final Erase was followed by Blank Check,
a separate full Read and Verify at 4.75/5.25 V. All cleanup operations succeeded;
the final successful operation selected Good and cleared Busy.

Final FF SHA-256:
`71189f7fb6aed638640078fba3a35fda6c39c8962e74dcc75935aac948da9063`.

| Test | Result |
| --- | --- |
| Initial Blank Check | Entire 64 KiB FF |
| Full address-dependent pattern | Program passed; independent full Read exact |
| Verify / Compare | Both passed |
| Wrong expected byte at address 65535 | Both rejected, exactly one mismatch at 65535 |
| Blank Check on programmed array | Correctly rejected |
| Full inverse pattern via Over-program | Passed; independent Read exact |
| Seven-byte write at address 32765 | Crossed 128-byte page and A15 boundary; entire readback exact, all neighbors preserved |
| Secure | Protected-program sequence completed and full image retained |
| Read after Secure | Exact match to expected image |
| Erase after Secure | Disable-protection programming path completed; full FF verified |
| Final Blank / Read / Verify | All 65536 bytes FF; zero mismatches |

Program, Over-program, Secure and Erase include full-array voltage-margin
verification and a fresh-power readback. The independent read files above add
another complete comparison against host-generated expectations.

Secure is **software write protection**, not readout protection. A separate
raw write attempt while protection remains enabled was not performed.
Consequently blocked-write enforcement is not independently hardware-proven;
the audited protected-program sequence, image preservation and subsequent
normal clearing were tested. No programmer persistent firmware, security or
identity settings were changed.

## Implementation and GUI

All eight operations are available through the normal GTK interface and native
C owner. Reopen the desktop application and choose **Catalyst → CAT28C512P**
under Dataman 13.10. Erase and Secure need no input file; Secure requires the
whole memory range.

This is an explicit **native extension**, absent from the examined original
Windows catalogues. Its [procedure audit](../CAT28C512_NATIVE_PROCEDURE.md)
documents the source-component composition and manufacturer pinout check.
It is not labelled as an original Windows CAT28C512 workflow. The separate
identification area/high-voltage ID behavior is outside the tested profile.

## Evidence

`analysis/cat28c512-integration/` contains:

- `source-before.tgz`: C source backup before integration.
- `audit.json`, `native-compile.json`: complete DIP32 pin mapping, six
  reference/native compiler matches and fourteen original applet executions
  using 128-byte pages, including upper-address boundaries.
- `pattern-read.bin`, `inverse-read.bin`, `partial-read.bin`,
  `secured-read.bin`, `final-ff.bin`: independent full-array readbacks.
- Individual operation JSON/logs and `live.log`: all positive and deliberately
  negative results, mismatch addresses and cleanup status.
- `image-hashes.json`: SHA-256 hashes of test images/readbacks.
- `regression.log`: all four relevant suites passed. CAT owner: 142 cases,
  including 128 partial-page starts; original Xicor/Atmel owners: 78 cases each;
  production GTK preparation: 24 cases, eight per admitted page EEPROM.
- `atmel-regression.log`: existing cross-release audit still passes after
  extracting the shared 8051 interpreter.
- `support.json`: eight executable operations in the normal support registry.

The CAT test fixture initially rejected a 31-byte DIP32 runtime upload because
its DIP28 predecessor expected 27 bytes. This was corrected in the fake USB
backend before the first hardware write; it was not a hardware failure.

GUI coverage consists of headless execution of the actual production
preparation code and physical tests of its C backend; no mouse-driven GUI test
is claimed. This sample qualifies CAT28C512P only, not other packages or EEPROM
families. The generated list totals **941 complete supported entries**: 939
original catalogue entries plus two explicitly identified native extensions.
