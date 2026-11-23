# NewDeviceLib

The project's local library of **authored** device additions, corrections and
resources. Imported Windows libraries remain separate and are supplied by the
operator. This directory contains no vendor REZ, firmware or FPGA images.

- `devices/`: nine migrated native device additions.
- `corrections/`: exact-source variable corrections, initially IS25LP064A.
- `experimental/`: FM16W08 and FM18W08 work, retained without enabling operations.
- `sources/`: canonical authored source, initially the FM16W08 C diagnostic.
- `applets/`, `bitstreams/`: future authored outputs; no qualified outputs yet.
- `build/`: build recipes; loading a library never runs a build or diagnostic.
- `evidence/`: immutable snapshots of project reports used by these revisions.
- `bundles/`: existing Component Composer bundles, registered in the index.

`library.json` is the explicit index. Each definition/resource/bundle is bound
to its SHA-256. Definitions have stable IDs, versions, source dependencies,
explicit changes and qualification status. File names do not grant support.
The C loader rejects malformed, conflicting or hash-mismatched input. Executable
support additionally requires an existing compatible C engine. Editing a status
string cannot create a new engine or qualify a new hardware procedure.

## Use

The application discovers a complete library in this order:

1. `LT48_NEW_DEVICE_LIB` (explicit path, errors are reported).
2. `$XDG_DATA_HOME/labtool48/NewDeviceLib`, or
   `$HOME/.local/share/labtool48/NewDeviceLib`.
3. `../share/labtool48/NewDeviceLib` relative to the Linux executable.
4. The configured installation data directory.
5. This source-tree directory for development builds.

There is one selected library, not an implicit merge with competing copies.
Copy the complete directory to the user location before editing it there.

```sh
labtool48 new-device-lib list
labtool48 new-device-lib validate /path/to/NewDeviceLib --json
```

The GTK Components page includes a NewDeviceLib inspector. Its qualified
entries appear in the normal device selector when the exact base vendor library
is loaded. Experimental entries are visible in the inspector only. Reopen the
application or reload the imported device library after changing definitions;
an open installation holds a validated snapshot.

## Authoring and qualification

A `derived` definition names an exact imported base by file SHA-256, catalogue
key, class, definition, family, pin map, package pin count and capacity. Its
`overrides` contains only deliberate changes. This schema currently supports
name/manufacturer changes and optional device type, definition, pin map, pin
count, capacity and ID values. Omitted fields inherit from the exact base.
The reserved synthetic definition IDs on migrated records select existing
native procedures; do not repurpose one as an arbitrary new algorithm.

A `correction` has expected-old/new variable values, matched to that same source
identity. Operational variable corrections are DWORD variables consumed by
the class-2044 SPI C engine, plus exact-source class-2010 ATmega signature
variable40 corrections and the narrowly scoped M1/C1 voltage-variable0
corrections described below, consumed by the AVR C owner. Other engine-specific corrections need an
explicit consumer; they cannot be silently applied by this schema. A derived
alias also inherits its base's correction. Missing ISSI correction leaves its
inconsistent source definition unavailable, rather than programming it wrongly.

New operations needing a new algorithm require a C engine or a qualified
Composer execution path. Store their draft definitions and assets here as
`experimental` while developing that implementation. Experimental records do
not count as support and never enter normal operator selection.

For a change, create/revise the record and its evidence, update its version,
register its path when new, and refresh the reviewed file hashes:

```sh
python3 tools/refresh_new_device_lib.py NewDeviceLib --version 1.0.1
# New files: append --add-record devices/new-part.json
# Composer: append --add-bundle bundles/new-bundle.json
build-c/labtool48 new-device-lib validate NewDeviceLib --json
```

The refresh tool validates a staged copy through the C loader before atomically
replacing the index. It is an offline authoring utility. All GUI and runtime loading,
validation, source matching and correction application are C. Parameter-only
changes within an already supported engine do not require rebuilding the app.
Changes to engine behavior do. Keep revisions in source control; do not alter
historical evidence snapshots when revising a device.

## Applets and bitstreams

An authored resource declares ID, version, kind, relative path, hash, ABI,
license, source references and build-recipe reference. Supported kinds include
C/assembly/Verilog source, 80C32 applet, FPGA bitstream, build recipe and evidence.
Applet/bitstream records require source references, a build recipe and a real
ABI name. No asset is executed merely because it is in the index. Engines use
the native typed/hash-checked resource API; newly introduced payload ABIs need
engine support and auditing before hardware use.

The existing Component Bundle schema remains the detailed format for authored
algorithms, pin maps, adapters, state-machine operations and reproducible payload
build manifests. Such bundles can be stored in `bundles/` and indexed under
`bundles` with path and hash. This library does not replace that schema or its
execution/evidence gates. Registering a bundle preserves it but does not turn
its objects into operational catalogue entries automatically.

Imported resources are referenced, never copied here. Only index-declared
files enter CMake packages. The current `LicenseRef-Project-Unspecified` label
records the existing lack of a project license; it grants no new license.

### Compiled POF Read correction

`source.pof-trs-read` is the canonical C include used by `c/src/pof.c` for
transformed POF mapping and corrected Read reconstruction. Its build recipe
and audit evidence are indexed alongside it. It contains no vendor tables;
the exact original tables are obtained from the user's imported runtime.
This is a compiled algorithm correction, not a runtime variable override.
Changing it requires rebuilding the native library and refreshing its resource
hash. Its registration does not admit a device to the supported list.

## Compiled AT45DB642D parallel configuration correction

`source.at45db642d-config-read` corrects the exact D26/M26/W8b protection
reader in the imported Dataman GDCA source. It fixes the impossible masked
comparison and register-byte traversal. Its build recipe and source-audit
report are indexed with hashes. This compiled C correction embeds no vendor
waveform, applet or FPGA image and requires a rebuild when changed.

## AEC 14.30 cFeon status-selector correction

`source.aec1430-cfeon-status` inserts the missing dual-entry selector in the
four exact EN25QX/QY256A definitions' status readers. It is compiled into the
native C SPI owner, with exact source and operation binding checks. The indexed
[evidence](evidence/AEC1430_CFEON_STATUS_SELECTOR.md) records the original
applet parameter trace and the correction. Vendor payloads remain BYO.

## Private ST M28F201 pre-erase correction

`source.st-m28f201-preerase` contains the authored zero-fill, verify and readback prerequisite for the exact private M28F201 profile. [Evidence](evidence/ST_M28F201_PREERASE.md) records its native tests and remaining full-erase qualification. It requires rebuilding C; registration does not admit a device or distribute vendor payloads.

## Atmel / ST boot-block NOR, version 1.0.62

Exact source-bound C admission for AT49BV008A and M28F410/420, plus an authored permanent-boot-lock guard for the Atmel bottom-boot part. AT49BV008AT remains deferred because its top-boot status query does not match the current manufacturer/FPGA binding. Source resources and original adapters remain bring-your-own. Full memory-operation and GUI audits are in [the batch report](../docs/audit-qualification/AMS_NOR_BATCH_AUDIT.md); hardware qualification is pending. The guard does not set, clear or override the permanent lock.

## Classic ATmega signatures and parallel programming

The initial48-entry cohort uses the shared native AVR engine and normal GTK operator.
Thirteen exact-key corrections in `corrections/avr-atmega-signature-*.json` fix
manufacturer-documented A/non-P identities while preserving P/PA aliases sharing
a definition. [Evidence](evidence/AVR_SIGNATURE_CORRECTIONS.md) and the
[operation audit](../docs/audit-qualification/ATMEGA_48_AUDIT.md) distinguish
these intentional corrections from unchanged original programming routines.
No vendor payloads are bundled. Hardware qualification is pending.

### M1/C1 parallel-programming voltage

The three exact Dataman13.10 D113–D115 M1/C1 entries have authored VCC deltas
from330cV to500cV, following manufacturer parallel-programming specifications.
The correction parser admits kind0 only for this source-SHA-bound variable0
change, retaining channel2. Numeric expected/value fields encode low16-bit
value plus high16-bit channel. Other existing correction scopes remain kind2.
See [the evidence](evidence/AVR_M1_C1_PARALLEL_VOLTAGE.md).

The subsequent [21-entry extension](../docs/audit-qualification/ATMEGA_21_AUDIT.md)
and [nine-entry extension](../docs/audit-qualification/ATMEGA_9_AUDIT.md) reuse
the same C owner. The latter stores exact bindings in`profiles9.h` and its
P53-only optional-pin normalization in`normalize9.h`, alongside three voltage
correction records. Missing M1/C1 voltage corrections reject admission.
Qualification and physical adapter limits are recorded in the batch report.

## SST/Winbond Page memories — 1.0.349

The [36-entry batch](../docs/audit-qualification/SST_PAGE36_AUDIT.md) stores
its authored admission bindings and C owner under `sources/sst-page/`.
It reuses the existing XP transport/runtime and GTK route with exact
Dataman13.10 resources. All eight normal memory operations are audited,
including EEPROM-style rewriting, source chip erase and content-preserving
software protection. Vendor assets are not bundled. Hardware testing is pending.

## SPI package extensions — 1.0.350 (first stage)

The [SPI package audit](../docs/audit-qualification/SPI_PACKAGES_222_AUDIT.md) adds the first 17 Macronix SO-16 entries through the existing portable C SPI owner and normal GUI. Authored closures, exact name/identity bindings and a separate admission list live in `sources/spi-packages`. Original resources remain bring-your-own. At that first-stage publication, the remaining 205 candidates were unadmitted; the completion below covers all 222.

## SPI package completion — 1.0.351

All [222 SPI package entries](../docs/audit-qualification/SPI_PACKAGES_222_AUDIT.md)
are now qualified in portable C and the normal GUI. The first 17 were published
in 1.0.350. Exact bindings and separate admission remain in `sources/spi-packages`.
D164/M168 (ISSI SO-16) and D16b/M167 (XMC) have separately scoped boundary
corrections; the vendor files are unchanged. Hardware qualification is pending.

`drafts/spi-reuse222/semantic-profiles.json` preserves complete source/effective
contracts, correction requirements and evidence links for future retrieval.
`compiler-contexts.json` preserves actual original compiler comparisons and
member bindings. These are reusable evidence, not automatic admission rules.
Original MCU applets and FPGA configurations remain bring-your-own.


## Classic AVR35 extension — 1.0.398

The remaining classic ATmega, AT90PWM, AT90S and ATtiny profiles use exact
source-bound contracts in `sources/avr-atmega/profiles35.h`. Contracts specify
sparse named or numeric fuse fields, reserved bits, one/two/three physical fuse
bytes, lock fields, EESAVE and SPIEN. All production operations and normal GTK
integration are C. Original resources remain bring-your-own.

ATtiny15L combines high-voltage configuration with HV or SPI memory access.
Configuration is captured through HV mode8 before memory access; the owner
restores and checks original fuses after memory-only operations and failed
writes. Recovery uses a fresh uncancelled transport. Calibration insertion is
explicitly disabled, preserving the supplied image and factory calibration.

Two required exact-source deltas are stored in `corrections/avr35-*`: Tiny22's
manufacturer identity06 and Tiny26L's5V parallel programming supply. Original
callback witnesses and independent target checks distinguish intentional
corrections from unchanged source workflows. See
[the audit](../docs/audit-qualification/AVR_35_AUDIT.md) and final receipt.
Hardware, physical adapters and analogue timing require later qualification.

## NuMicro 81-entry batch — 1.0.507 / 1.0.508

`sources/numicro81/` owns the finite profiles, typed configuration schemas,
loader and C operation owner for 81 original Nuvoton aliases (43 definitions).
The first 16 and remaining 65 have separate immutable qualification receipts.
Normal GUI support includes full raw Read and all nine original operation
categories. Vendor REZ, controller applets and FPGA images remain bring-your-own.
Hardware qualification is pending.

[Operation contracts, configuration packet and audit scope](../docs/audit-qualification/NUMICRO81_COMPLETION.md).
