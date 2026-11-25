# Linux application UI bug list

User-reported issues in the Linux application. These are separate from
[original Windows software bugs](ORIGINAL_WINDOWS_SOFTWARE_BUGS.md).
Root causes remain unconfirmed unless an entry states otherwise.

## UI-001 — False “programmer not responding” during slow operations

- **Reported:** 2026-11-16.
- **Status:** Blocking progress paths fixed and offline audited; the originally reported dialog has not been live-retested. See [GUI audit](GUI_OPERATIONS_AUDIT.md).
- **Symptom:** A slow device operation, such as serial flash, brings up a
  “programmer not responding” dialog while the operation is taking time.
- **Expected:** The GUI remains responsive and distinguishes a legitimately
  long operation from an actual communication failure. An expected wait must
  not be presented as a disconnected or unresponsive programmer.
- **Verification scope:** Exercise a long serial-flash operation through the
  normal GUI, check the dialog against worker/transport activity, and retain
  genuine timeout, cancellation and disconnect reporting. Do not simply
  suppress all communication errors or extend waits without evidence.

## UI-002 — Edit exposes only approximately0x200 bytes of a larger device

- **Reported:** 2026-11-16.
- **Status:** Fixed and offline audited: full-image scrollbar/wheel navigation, bounded page rendering, editing/undo and stale-page/bounds guards. See [GUI audit](GUI_OPERATIONS_AUDIT.md).
- **Symptom:** Edit shows only about0x200 bytes (512 bytes) even when the
  selected device has a much larger memory image.
- **Expected:** The editor provides navigation and editing across the full
  selected image, with the correct size and addresses. Separate security or
  configuration regions must retain their defined mapping and edit policy.
- **Verification scope:** Compare selected profile size, allocated buffer,
  editor address range and navigation. Establish whether this is a fixed
  display/page limit, incorrect region selection, or actual buffer truncation;
  the bounded display page was not image truncation. Distant offsets, the final
  byte, secondary buffers, edits and undo passed offline tests.

## UI-003 — Slow catalogue startup and search; cache catalogue/index

- **Reported:** 2026-11-16.
- **Status:** Implemented and offline audited: persistent support/filter index and retained parsed catalogue; source/build invalidation and corrupt-cache rejection tested. See [GUI audit](GUI_OPERATIONS_AUDIT.md).
- **Symptom:** Loading the device catalogue at startup is slow; device search
  has also previously been reported as slow.
- **Proposed fix:** Persist the parsed catalogue and supported-device search
  index. Reuse valid cached data at startup and during search, rebuilding when
  source database content, catalogue/cache schema, or native support-library
  version/content changes. Keep source-version selection and supported-only
  filtering correct.
- **Verification scope:** Measure cold and warm startup/search times. Check
  invalidation after database or support-library changes, switching database
  versions, and recovery from a missing, partial or corrupt cache. Cached
  catalogue presence must never imply executable device support.

## Related completed maintenance

See [GitHub contributor findings](GITHUB_CONTRIBUTOR_FINDINGS.md) for the earlier
Edit workspace-switch fix and other maintenance. That fix does not establish
resolution of UI-002.

## UI-004 — Inconsistent socket contact-check gate

- **Source:** GitHub issue4 and owner request, 2026-11-23.
- **Status:** Shared C/GUI gate implemented locally; offline qualification is
  documented in [Socket contact checking](SOCKET_CONTACT_CHECK.md). Hardware
  testing and publication of a new release remain separate.
- **Behavior:** Enabled by default; failed/missing checks stop normal operation
  dispatch. Options provides an explicit session-only fixture bypass with visible
  warnings. Existing engine safety checks remain active.
