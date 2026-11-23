# ST M28W word-NOR lock-write correction

2026-10-07. Authored C guard in `sources/intel-word/st_lock_write.h`.
No imported procedure, firmware or bitstream is redistributed.

Scope: GDABA__.rez SHA-256
`77e939e536a8af7d76fa8d661927e7f1e712eb7b1441fe6e7c86ddbeefdb51ec`,
class 2033 definitions D8–Dd, operation 11, W11, configuration 10059.
The C engine requires the original source digest and exact profile tuple before
applying the correction. The original W11 is 169 bytes; its final 12-byte tail
at offset 157 is checked before inserting authored dispatch instructions.

## Evidence and change

The original W11 writes back four user register words at word addresses 85h–88h,
then prepares the lock word in buffer Out and selects word address 80h.
There is no program-app dispatch between that final address selection and the
returned result. Compact opcode 82 earlier in the routine is a no-op in the
original compiler; `tools/audit_compact_noops.py` checks the original executable's
jump-table pointer and complete skip handler. This is a resource-level omission;
no claim about the full Windows application's visible result is made here.

The correction dispatches the profile's own program applet with C0h and the two
bytes already prepared in Out, captures its result, and stops the applet before
continuing the original tail. Both lock bits are read back by the native owner.
The selected configuration locks user registers and Parameter Block #0
permanently. Existing user register data is preserved. This is an intentional
Linux correction, not a byte-for-byte reproduction of the original procedure.

Manufacturer references: ST/Numonyx M28W160C command table 4, protection-register
program section, and lock-register table 7; M28W320C PRP description specifies
the same C0h command and bits 1/2 at word 80h.

- https://www.mouser.com/datasheet/2/671/M28W160C-1283370.pdf
- https://www.alldatasheet.com/html-pdf/158176/STMICROELECTRONICS/M28W320CT85N6T/25435/13/M28W320CT85N6T.html

Native tests exercise every affected definition, register preservation, successful
lock readback and a dropped OTP write. The untouched W11 failed lock readback;
the correction passes it. These are protocol-fixture tests, not hardware tests.
Do not infer analogue qualification or a Windows UI bug from this evidence.
