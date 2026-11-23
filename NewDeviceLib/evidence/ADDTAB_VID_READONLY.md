# Private D57/Dac hardware-ID correction

This authored compact-wave transformation is restricted to D57/Dac operation
4 and the original 116-byte procedure. The C loader also binds the REZ digest
and complete profile. It retains rail setup, address selection and cleanup.

Original operation 16 sends C2 AA 55 90 and ends with C0 F0 while A9 is at
VID. Its recovered FPGA produces unexplained writes in that mode. This is not
claimed as a vendor bug. Hardware autoselect in Am29LV256M Table 3 requires
CE=OE=0, WE=1, A9=VID; it does not require those command writes.

The replacement directly captures both bytes through resident FPGA access,
advances D0 between bytes and after the word, then raises OE and CE. No applet
or software-ID/reset write is issued. The extra prefetch may address an
unspecified selector; such data must remain unknown and must not be consumed.

Audit tool: tools/audit_addtab_tail92_vid_readonly_trial.py --native.
The default mode records the preceding Python sequence prototype. Neither mode
alone qualifies complete device support. These definitions remain private.
Manufacturer datasheet: https://www1.futureelectronics.com/doc/SPANSION/AM29LV256MH123RPGI.pdf

## Additional private programming and region corrections

D57/Dac now use original W9d instead of W56 for operation 20. Both receive
four address bytes, so their compact programming frames remain unchanged.
`tools/audit_addtab_vid6_program_alternative.py` proves identical main-array
streams for 160 words at five addresses, and explicit unlock plus target-address
writes for five special-area positions. W9d remains a BYO vendor resource; no
vendor payload was copied into NewDeviceLib.

Dac READ/BLANK/VERIFY now set the requested address after the engineer-entry
applet, which otherwise clears it. This extends the authored region transition
correction to the exact Dac compact entry shape. Both definitions pass reads and
programming at the final 16 bytes of the special area, preserving neighbors and
the main array. Full admission remains pending erase and remaining fault checks.

## Erase boundary

The original optional operation 13 requests high-voltage sector unprotect.
That feature is deferred for D57/Dac: the authored guard omits this implicit
unprotect step, while Secure remains unavailable. Normal erase and its complete
readback checks still run; an erase of protected data must fail verification.

The common native owner now preserves only the bytes beyond erase_end before
Erase, instead of reading the whole array first. For these profiles that means
256 bytes instead of 32 MiB plus 256 bytes. The complete post-operation readback
is retained. Secure continues to preserve its full requested image.
