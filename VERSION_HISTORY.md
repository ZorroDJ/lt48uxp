# Version history

Release packages contain only project-created software and documentation.
Vendor databases, Windows executables, USB firmware and vendor FPGA/app-let
payloads remain bring-your-own. Device counts include aliases/package variants;
audit qualification does not mean every device was physically tested.

## 0.1.0-test.20261124.1 — 2026-11-24

### Added

- Default-on socket contact checking before normal GUI device operations.
  Failed contacts stop dispatch and report ZIF pin numbers.
- Explicit session-only fixture override under Options → Socket contact check.
  Restart restores checking. Direct low-level/debug paths retain their own policy.
- Portable C, CLI and GUI communication echo and empty-socket digital tests.
  Live programmer qualification passed 260 echo comparisons and 672 pin comparisons
  across 14 patterns, with successful cleanup. LEDs remain visually confirmed.
- Verified private-cache extraction of the alternate contact dispatcher from
  the user's original runtime. Old caches needing this helper require reimport.
- Packaging check for missing/untracked production source inputs.

### Fixed

- Public Git source omission of `c/src/core.c` caused by the `core.*` ignore
  rule. It existed in the working tree but was missing from Git downloads.
- Repeated catalogue hashing for source identification: compute once per open
  installation and reuse its immutable identity.
- Prepared-read wording now says “READY and plan prepared; no USB opened yet”.

### Unchanged and deferred

- NewDeviceLib 1.0.710 and 15,768 supported union entries; no device additions.
- Extra AEC 14.40 executable/FX2 firmware variants are not admitted without bytes
  and audit evidence. Existing source-specific support still applies.
- Digital readback is at the FPGA pad; it does not certify continuity through
  the protection network/ZIF contact or beyond an adapter.
- RAM, fixture, DAC measurement and fault-protection self-tests are deferred.
- Ubuntu 26.04 amd64 target; other releases/architectures are not binary-qualified.

Source-profile caching and prepared-read wording follow review of
[ZorroDJ's fork](https://github.com/ZorroDJ/lt48uxp). Additional folder/last-device
persistence features are not included in this release.

## 0.1.0-test.20261123.1 — 2026-11-23

- Published Ubuntu 26.04 amd64 Debian/portable packages and production C/GTK
  source, with project-authored NewDeviceLib 1.0.710.
- Added editor workspace fixes, asynchronous progress polling, full-image
  editor navigation and persistent catalogue/support-filter caching.
- Included LED-only diagnostic, FX2 HEX parser/loading feedback improvements,
  and the audited AEC 14.40 additions.
- Known publication defect: Git omitted `core.c`; corrected in 20261124.1.

## 0.1.0-test.20261004.1 — 2026-10-04

- Earlier limited-test Ubuntu 24.04 x86-64 binary release.
- Public repository was initially documentation-oriented; it did not provide
  the later complete native source distribution.
