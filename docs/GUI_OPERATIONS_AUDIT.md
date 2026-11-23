# GUI fixes and operation audit

2026-11-16. Scope: the three entries in [UI_BUGS.md](UI_BUGS.md), plus
related faults found while reviewing the actual operator code. The desktop
application was rebuilt. No device operations, firmware uploads or programmer
configuration changes were performed during this work.

## Changes

### Progress during slow operations

GTK progress polling now tries the owner mutex without waiting. Previously it
could wait behind a worker holding that mutex during USB close/release. The
native progress snapshot also has a bounded retry count, returning `LT48_BUSY`
without altering the caller's snapshot if the publisher is being updated.
Previously it could spin indefinitely if that publisher was descheduled.

The GUI retries on its next timer tick and keeps the indeterminate progress
indicator moving. Device communication timeouts, mismatch reporting,
cooperative cancellation and mandatory cleanup remain in effect. This fixes
identified blocking paths; the originally reported desktop “not responding”
dialog was not reproduced or retested with physical hardware in this audit.

### Full-image Buffer Edit

The editor now has an image-wide address scrollbar and mouse-wheel navigation.
It retains bounded 512-byte rendering rather than putting millions of editable
cells into a text widget. Go, Previous/Next, search and direct replacement remain
available. Scrolling can reach the last byte of a large or secondary image.

The audit also found and fixed related input faults:

- Applying displayed cells now checks their displayed address, resident buffer
  and revision. Editing the address entry or replacing the underlying buffer
  cannot silently redirect an old page into a new location.
- Row addresses must match the displayed range.
- Oversized rows/pages are rejected before copying into the 512-byte page array.
  Previously an oversized pasted row could overrun that array.
- Preflight failures reached through early exits now always receive a visible
  error message.

### Catalogue and support/filter cache

The parsed installation can be retained across searches with an explicit,
reference-counted lifetime. A persistent, bounded support/filter index lives in
`$XDG_CACHE_HOME/labtool48/catalogue` (normally
`~/.cache/labtool48/catalogue`). It records display support and technology flags;
it contains no applets or FPGA payloads.

Its fingerprint includes the source path and relevant source contents,
private-cache objects/manifest, native library image, GUI build/schema and
NewDeviceLib manifest, records and payload directories. Supported uppercase
Windows `.EXE` filenames are included. File symlinks used by the private cache
are hashed through their targets. Missing, malformed, obsolete or corrupt
indexes are rebuilt; cancellation does not commit a partial index. A source
change also replaces stale parsed metadata and clears a stale device selection.

This is a GUI display/search cache. Actual operation preparation and execution
still reopen and validate the selected source and native owner. A cached flag
cannot authorize hardware execution. Full installation audits remain explicit
source audits, not cached admission claims.

On Unix the native image is identified through `dladdr`. If image discovery or
fingerprinting is unavailable, persistent reuse is disabled and live indexing
still works. Non-Unix cache acceleration was not qualified by this Linux audit.

## Verification

Machine record: [gui_ui_fixes.json](data/gui_ui_fixes.json).

| Check | Scope |
| --- | --- |
| Actual GUI prepare/execute workers | 116 cases, two repetitions of applicable memory operations for eight representative devices; real source/resource admission, USB context creation deliberately blocked |
| Representative engines | ST M24512-W, Macronix MX25L6433F, Atmel AT29C010A, AMD PALCE16V8Q, Lattice GAL16V8D, PIC12F609, AT90S2313 and NS93C46 |
| Polling | Held owner mutex and interrupted progress publication return promptly; incomplete snapshots do not replace valid data |
| Editor | 8 MiB-plus image, distant/final addresses, secondary image, in-place replacement, page apply, undo, preserved neighbors, invalid range, stale address/revision and oversized row rejection |
| Completion | Actual failure/cancel completion handler restores controls and clears pending execution; no false success count |
| Catalogue | Cold/warm index equality, nine search filters, All versus Supported, parsed-object reuse, source/runtime/record invalidation, metadata replacement, corrupt-index rejection and cancellation |
| Existing regressions | GitHub Edit-workspace/LED GUI tests and native core tests |

All jobs ran serially through the 1 GiB resource guard. Earlier failed fixtures
and integration checks remain in their run logs; they were not counted as
passing evidence. The PALCE fixture was corrected to use the production fuse
map's one-byte-per-fuse representation. The cache integration check caught
the legitimate private-cache file symlinks before delivery.

The audit tests actual GUI dispatch up to a deliberately blocked USB boundary.
It does not replay every device, exercise every configuration editor or replace
the existing native/original-workflow qualification. Hardware success, native
cleanup electrical behavior and elimination of the reported physical-session
dialog are not established by a blocked-USB test.

## Performance and delivery

The machine record retains measured component timings for the 15,564-entry
Dataman installation. The final passing run measured about 0.50 seconds for
parsing, 17.72 seconds for first index construction, 0.095 milliseconds for a
cached index load and 0.146 seconds for result-row creation. These are component
measurements, not a complete application-launch benchmark. First use after
changing a source or native build still constructs the index in a worker.

The updated executable is `build-desktop/labtool48-gtk`, already used by the
desktop launcher. Restart an existing application instance to use it. No binary
release was uploaded. Device memberships, operation profiles and hardware-test
claims are unchanged; current support-list build provenance is refreshed
without rerunning or modifying sealed device qualifications.
