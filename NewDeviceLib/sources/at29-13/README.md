# Atmel AT29 parallel page flash

Authored portable C owner and exact Dataman 13.10 catalogue bindings for thirteen
entries. Original REZ, controller applets and FPGA bytes remain bring-your-own.

The selected D0–D8/M0–M5/P0–P3 contexts retain 64–512 KiB arrays, 128/256/512-byte
pages and 3.3/5 V nominal supplies. Program/Over-program use complete source pages;
Read, Blank, Verify and Compare currently use page-aligned ranges too. Erase and
Secure require the whole array. Independent readback follows writes and erase.
The proven SST Page-memory cleanup and transport helpers are reused.

D0/D1/D5: Secure rewrites the same contents with software data protection enabled;
ordinary Program disables that protection. This is the original procedure set,
including AT29C010A's source selection; no additional silicon features are implied.

D2/D3/D4/D6/D7/D8: Secure runs the original permanent boot-lock callback for the
explicitly selected lower or upper block. Existing opposite-block locks are
preserved. Both can be locked by two explicit operations. No unlock exists;
chip erase cannot remove those locks. Read Configuration is internal to the
operation owner, not a separate arbitrary configuration editor. Ordinary Program
selects the source software-protection-disable option without clearing boot locks.

Evidence: docs/audit-qualification/AT29_13_AUDIT.md and its qualification receipt.
Qualification is offline; physical timing and hardware testing remain pending.
