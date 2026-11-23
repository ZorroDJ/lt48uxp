# EFST F49L004BA/UA setup guard

The pinned Dataman 13.10 GDAA___.rez definitions D23/D24 use W19 with four
unmapped logical-pin-4 references. Offsets 17/39 are already verified resident
no-ops. Voltage routes at offsets 163/176 occur only in modes 13/14.
These definitions have neither Protect nor Unprotect, nor their prerequisites.

`tools/audit_xp_efst_dead_protection.py` poisons both voltage-route runtime
instructions. All seven admitted modes (0,1,2,4,5,7,11) produce exactly the
original interpreter result; modes 13 and 14 reach the poison as positive
controls. Both definitions are checked (18 trials). The separate original-x86
optional-protection dispatcher audit proves absent operations skip setup.

The authored C guard rejects every other mode before setup and validates the
exact four compiler-default metadata records. The owner pins the REZ hash,
definition, W19 size and variables and rejects protection bindings. No socket
pin assignment is invented. This is source/control-flow evidence, not hardware
qualification. Vendor resources remain external.
