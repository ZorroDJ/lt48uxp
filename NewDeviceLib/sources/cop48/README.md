# National Semiconductor COP48 source profiles

Authored exact-name/definition/family/pin-map/geometry bindings for 48 Dataman
13.10 catalogue entries. Original GAD resources and the four RAM applets remain
bring-your-own inputs; no vendor bytes are distributed here.

Portable C owner: `c/src/small_eprom.c`. The regular GTK preparation/execution
workers use that owner. Seven operations: Read, Blank Check, Program,
Over-program, Verify, Compare and Secure. No electrical Erase for EPROM/OTP.
Whole images are required: arbitrary entry into the source streaming/counter
sequence is not qualified. COP8S/AJC images include nine extra bytes; the source
configuration byte is at capacity minus nine. Read masks its reserved bit 7;
Program/Verify apply the source 0x5F configuration rule. Secure is separate and
irreversible. No independent physical security-effect measurement is claimed.

Qualification and evidence: `docs/audit-qualification/COP_48_AUDIT.md` and
`docs/data/cop48_qualification.json`. Hardware qualification remains pending.
