# Holtek OTP MCU profiles

Authored exact source bindings for 19 Dataman 13.10 entries (11 definitions).
Original GAD resources and four 80C32 RAM applets remain bring-your-own inputs.
Portable C owner: c/src/small_eprom.c; normal GTK preparation and worker execution.

Read, Blank Check, Program, Over-program, Verify and Compare operate on whole
images. Each 14/15-bit word occupies two little-endian bytes; unused bits must
be zero. Blank words are zero. No electrical Erase or separate Secure operation
exists in these source procedures. The separate configuration word is read and
preserved before writing and checked by source Verify. No independent option
editor is provided. Hardware qualification remains pending.

Evidence: docs/audit-qualification/HOLTEK_19_AUDIT.md and
 docs/data/holtek19_qualification.json.
