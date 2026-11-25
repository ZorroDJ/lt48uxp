# Next release: source publication and source identification

**Release update:** Included in `0.1.0-test.20261124.1`. The dated
implementation and qualification record below describes preparation before
publication. Its references to the previous release are historical.

2026-11-24. Implemented locally; no GitHub push, tag, release or issue message.

## Source publication

The previous public Git source tree omitted `c/src/core.c`: the release
checkout's `core.*` ignore rule treated it as a core dump. The file existed in
the local working tree, allowing a working-tree build to hide the omission.
The release checkout now explicitly unignores and tracks the original
project-created file. No vendor file was added.

`packaging/check-source.py` now checks the production CMake inputs and source
provenance manifest against disk and, when inside a Git checkout, the Git
index. The package builder invokes it before CMake. An ignored/untracked core
file now fails packaging even if it exists locally. Source archives without
`.git` still receive the required-file check. The canonical scripts are also
kept in this research tree's packaging directory for subsequent exports.

The regression recreates missing/ignored source cases and verifies the fixed
file is present in a Git-index archive. The actual next-release index is
archived and configured independently of working-tree files. This is not a
new public release, nor a full export of all recent research-tree changes.

## Cached source identification

The immutable catalogue's source profile is computed once during installation
opening, before NewDeviceLib binding, and retained for that installation's
lifetime. Subsequent lookups return the cached string without hashing the
catalogue. Separate installation objects retain separate identities. NULL and
unrecognized catalogues continue to return `unrecognized`.

All existing catalogue hashes and admitted device workflows remain unchanged.
An instrumented production-C test makes 100,000 lookups each for raw13.10,
cached13.10, cached14.30, cached14.40, legacy7.60 and an unrecognized test
catalogue:600,000 getters with zero additional catalogue hashes. It also
compares each cached value with a fresh computation. Existing116GUI operation
preparation/dispatch cases and core regression are retained as the gate.

## Prepared-read wording

The legacy prepared-read message now says:

> READY and plan prepared; no USB opened yet

This replaces `OFFLINE DRY RUN — no USB opened`. Preparation still does not
itself read the chip. Execution, confirmation and operation behavior are
unchanged. The fork's dialog-directory and last-device persistence features
are separate and have not been imported in this change.

## Contributor evidence and deferred changes

These improvements follow review of ZorroDJ's fork, notably commits
`e57a9df0a3b9c48d0c9ba847c332e10a6b75a4aa` (publication/source-profile caching)
and `b9c2d36` (prepared-read wording). The implementation initializes the
cached identity before NewDeviceLib binding so opening-time callers can use it.

The additional AEC14.40 executable hash and AEC1.14 FX2 hash are **not accepted**.
Their bytes are unavailable for audit; neither allowlist was changed. Device
support stays at15,768 union entries and NewDeviceLib1.0.710.

See [verification receipt](data/next_release_source_fixes.json).
