# MX26L6420 erase completion

The authored erase_status.h binds only admitted/private tail definition De9.
The original compact erase procedure issues AA/55/80, AA/55/10, captures the
status byte and masks it with 0x80. All 256 input-byte values are audited by
`tools/audit_addtab_tail92_de9_erase_status.py`; successful completion is 0x80.

The Linux owner formerly expected 1 for this definition. The native fixture
reproduces failure after all main bytes become FF. Correcting this predicate
must retain failure handling, complete independent readback and cleanup.

This is a Linux host interpretation defect, not a Windows-software bug.
No physical-device or timing qualification is claimed. Public admission still
requires the complete operation and recovered-FPGA tests.
