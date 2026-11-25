# Socket contact checking

**Release update:** Included in `0.1.0-test.20261124.1`. The dated
implementation and qualification record below describes preparation before
publication. Its references to the previous release are historical.

2026-11-23. Implemented in the local C/GTK application. The existing GitHub
release has not been replaced by this work. Restart the desktop application
for the rebuilt executable and library.

## Operator behavior

Socket contact checking is **enabled by default**. Normal GUI operations use a
shared initializer on the same claimed programmer handle, before the device
operation owner is called. This covers the 50 initialization sites in the normal
operation worker and its phase helpers, including Program, Over Program, Erase,
Secure, configuration operations and functional tests, as well as reads/checks.

The selected definition's pin code supplies required, prohibited and auxiliary
contact masks. Device identification is separate: a missing electronic ID does
not choose the device or substitute for the insertion check.

Before scanning, the check clears the socket controls through resident49 and
loads the verified idle FPGA. It does not assume that the previous device left
a suitable FPGA configuration. The normal route sends resident `47 AA 55` and
captures twelve bytes. Definitions selecting the alternate route use the exact
previously audited 476-byte B000 helper and resident05 instead.

Missing or unexpected contacts stop dispatch. The GUI reports **ZIF pin numbers**;
these are not package pin numbers when an adapter is involved. A timeout, short
transfer, cancellation, unknown runtime payload or failed cleanup also stops the
operation. Correct seating, orientation or wiring, then prepare the operation
again. No automatic Ignore or retry-to-program action is taken.

Contact scans and FPGA/helper transfers can change volatile programmer state.
A separate noncancellable finalizer runs after an attempted scan, even if the
primary transport session failed. The subsequent device owner performs its own
normal resource setup. This path does not read/write unit security identity,
licensing EEPROMs or nonvolatile programmer firmware.

## Fixture override

Open **Options → Socket contact check…**. Select
**Bypass socket contact check — intentional fixture override**, then **Apply**.
The dialog warns about bad-contact programming. Cancel/window close leaves the
previous setting unchanged. Changing the setting invalidates prepared operations.
It cannot be changed while an operation is running.

The override is captured with the prepared request. Preparation and operation
results explicitly say **SOCKET CONTACT CHECK BYPASSED**. It applies only to the
current application session; a new application starts with checking enabled.
Project/session loading does not silently restore a disabled check.

This bypass skips the new shared socket gate. Device-specific checks already
inside an engine, resource admission, voltage limits, ID decisions, verification
and cleanup remain active. It is not a universal disable-all-safety switch.
A socket scan cannot establish continuity beyond an intervening fixture or prove
that every contact will remain good throughout a later operation.

## Bring-your-own runtime and existing caches

Both payloads are checked by SHA-256 before any scan I/O:

- Idle FPGA, 2,778 bytes:
  `1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab`.
- Alternate dispatcher, 476 bytes:
  `6c298f402055ea4fd0c90a728992a09062bbc05964d47b3e9e463da9732af788`.

Neither is bundled with the application. The alternate helper can be extracted
from the owner's original Windows executable. The importer stores only the
verified helper as `contact-dispatcher.bin` in the private cache and preserves
it on reimport; it does not copy the executable. An unknown helper is rejected.

Older caches lack this newly imported helper. Ordinary normal-route definitions
still work with their existing verified idle object. Alternate-route definitions
stop during offline preparation until the helper is supplied. Use the database
manager to import the original resources/runtime into a **new** cache and select
it. Do not overwrite an existing cache or copy unrelated applets into it.
The Dataman13.10 corpus contains six alternate pin-map resources, all in
`GA_____.rez`; that is a resource count, not a six-device support count.

## Evidence and verification

The unchanged mask/capture implementation is reused from prior original-host
instruction comparisons:

- Mask builder/acceptance: 102 applet contexts and 180 compact contexts;
  10,812 and 19,080 acceptance cases respectively.
- Normal/alternate capture: 28 original-host/C comparisons.
- The current header hashes match those receipts; earlier qualification
  archives were not modified or rerun.

Current checks include:

- Every one of 48 missing-contact positions, an empty socket, unexpected
  contacts and auxiliary-sample acceptance.
- Exact idle/helper upload bytes and framing; normal25/alternate35 endpoint
  transfers, with timeout and short-transfer faults at every transfer.
- Cancellation and exclusive-use rejection; corrupted idle/helper rejection.
- Twenty actual GUI mutating dispatch cases: Program, Over Program, Erase and
  Secure, each with pass/mismatch/timeout/cancellation/fixture bypass.
  The device-owner function is not called after a failed check.
- Actual settings dialog default, window-close and Apply behavior, warning text,
  invalidated preparation and enabled-by-default fresh state. Cancel uses the
  same window-destroy path as closing the dialog.
- All six alternate source pin maps prepared from a valid isolated helper cache;
  the old cache fails closed. Cache/helper corruption is rejected.
- Actual cache import, verify and reimport of a one-device private fixture with
  the owner's original runtime; the helper digest is unchanged and no executable
  is retained. This is not a repeated audit of the entire vendor installation.
- The existing 116 prepare/dispatch regressions, core tests and UI regressions.

The tests use injected or blocked USB. **No physical programmer commands were
sent for this work.** This establishes the software gate and its failure paths;
it does not establish measured socket waveforms or physical-contact accuracy
on a particular unit/adapter. The reusable-mask evidence comes from Dataman13.10;
this is not a fresh audit of every historical Windows host version.

Machine record: [socket_contact_check.json](data/socket_contact_check.json).
No device memberships, operation profiles or hardware qualification marks changed.
The native public API also exposes this shared check for callers; direct users
of lower-level device engines remain responsible for their documented contact
policy. This change does not silently alter those engine contracts.
