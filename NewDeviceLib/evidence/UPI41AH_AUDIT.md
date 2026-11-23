# Intel UPI-41AH / UPI-42AH support

2026-10-06. **Three entries added to the native C engine and normal GUI on an
audit basis. Hardware qualification is pending. No programmer was accessed.**

| Catalogue selection | Exact Dataman 13.10 binding | Code capacity | Package |
| --- | --- | ---: | --- |
| Intel:8741AH | GADA D91 / M4f / P2b | 1,024 bytes | DIP-40 |
| Intel:8742AH | GADA D92 / M4f / P2b | 2,048 bytes | DIP-40 |
| Intel:8742AH *44 | GADA D93 / M4f / P2c | 2,048 bytes | Original 44-pin package/adapter mapping |

These are eight-operation closures: Read, Blank Check, Program, Overprogram,
Verify, Compare, Secure and Configuration. There is no electrical erase.
The AH datasheet describes OTP packages; programming and protection are
irreversible on those parts. Package adapters must implement the selected
source pin map. The *44 selection is not permission to substitute a straight
DIP adapter without checking that mapping.

## Source boundary

The binding is restricted to the exact GADA file SHA-256
`aca05831121f0e30654601df133b298cec1d33e7ffb94626c5d62634e690c452`.
All three use FPGA Waf, 2,778 bytes, SHA-256
`5eb437ad954ee962e2594c6d55fdd88ed35867c68f54398b763a400bb4c1305a`.
They have no device-specific 80C32 applet. Existing audited controller pin,
voltage, pulse and capture commands operate the source FPGA.

M4f binds six compact routines and that FPGA. It contains no separate signature
or protection operation. The implementation therefore does **not** present
an unchanged M4f import as complete AH support. Authored C extensions live in
`NewDeviceLib/sources/upi41ah/`, registered with their build recipe and evidence.
Imported firmware and FPGA bytes remain operator-supplied.

The original array-address/data latch sequence and power routing are retained.
The authored sequence adds explicit timing guards, read/program supply choices,
Intel's intelligent programming algorithm, signature access and protection.
The specification is Intel **210393-008, November 1994**, particularly pages
3, 6–11 and 14 (programming timing):
[Intel UPI-41AH/42AH datasheet](https://ardent-tool.com/datasheets/Intel_UPI-41AH_42AH.pdf).
The unmodified source compiler comparisons are recorded separately from the
authored host-operation tests. Raw M4f differences alone are not treated as
proof of a Windows software bug: host-side parameter changes would also need
investigation for that claim.

## Array operations

- The zero-erased MCS-48 array convention is retained. AH definitions have the
  same memory-descriptor bytes as the corresponding earlier UPI definitions;
  the source family and AH-specific modes change. New GUI buffers use 00.
- Program checks the selected range for blankness before programming.
  Overprogram first checks every selected byte for valid 0-to-1 transitions.
  Neither operation erases or touches code outside that range.
- Initial pulses are 1 ms, with independent readback after each pulse, up to
  25 attempts per byte. A successful byte receives a 3X-ms overprogram pulse,
  where X is the number of initial pulses. Already-matching bytes need no pulse.
- Programming/byte verification uses VCC 6 V and VDD/EA 12.5 V. Read and final
  margin verification use nominal/margin VCC and VDD, with EA at 12.5 V.
  Power-up begins at 5 V; EA rises before the programming VCC/VDD transition.
- Explicit 20-us guards cover address setup/hold, RESET width, data setup/hold,
  T0-to-read and EA-to-VDD setup. Intel specifies up to 15 us for the 4*tCY
  constraints. Delays do not depend on USB scheduling.
- Programming is followed by a nominal 5-V comparison and the existing MCU
  voltage-margin comparisons. Shutdown holds RESET low before lowering EA and
  VDD, preventing normal code execution while the access mode is removed.
- The selected algorithm is never changed by a signature mismatch. The normal
  Abort/Retry/Ignore callback handles manufacturer/device mismatch.
- A nonzero security byte blocks ordinary array operations and additional
  signature programming. Configuration can still read the signature row.

## Signature and security

Port 1 selects the signature row with P10 low and P11–P17 high. Ordinary array
access uses P10/P11 both low. The added Port 1, /SS and /CS mapping comes from
the package pin table; source P2b/P2c still determines placement and all existing
controls. In particular PLCC /CS is pin 7 and EA is pin 8.

Configuration reads the complete 32-byte row and reports it in the GUI without
replacing the code image. Manufacturer 10h and device 11h are decoded; the
expected Intel signatures are 89/40 for 8741AH and 89/42 for 8742AH.

Only user bytes 12h and 13h can be edited. Both requested changes are validated
before programming either byte. The final complete row is compared, including
all preserved factory/test bytes. No general write access to the other 30
signature locations is exposed.

Secure is a separate explicit operation:

1. Confirm that signature security byte 1Fh is 00.
2. Read and retain two consecutive code bytes.
3. Program FF at the security location using the intelligent algorithm.
4. Independently read back security byte FF.
5. Read the same two code bytes and require at least one to differ.

Step 5 is Intel's recommended indirect protection check. It is not an exhaustive
proof against every possible data leak. The security-byte value alone is never
reported as proof of successful protection. Secure does not run automatically
after Program.

## Verification evidence

- **18** strict source compiler comparisons: all six compact operations for
  each definition, with no defaulted pins.
- **52** original Windows x86/native pulse-packet comparisons: both package
  PROG pins, 1-ms initial pulses and all 25 possible 3X-ms overprogram durations.
- **78** native transaction cases against an independent pin-level device model:
  all eight operations on all three entries, complete code arrays, address
  latching, Port 1 mode selection, voltage codes, timing guards, user-byte
  preservation, 25-attempt programming, ID Ignore/Retry, cancellation,
  failed capture, no-write faults, false security-byte success and invalid
  transitions. Erase and invalid configuration masks are rejected before I/O.
- **48** actual production GUI preflights, including unavailable-operation and
  invalid-edit rejection, plus complete signature-result formatting.
- Existing 8751/MCS-48 backend and production GUI regression suites are run
  alongside the new tests.

The model checks emitted transactions; it does not measure physical voltages,
clock output, edges, adapter wiring or device programming yield. The unchanged
vendor FPGA is reused as an exact source dependency, not claimed as a newly
reverse-engineered netlist. No AH entry is marked hardware tested.

Reproduce from the repository:

```bash
cmake --build build-c --target labtool48 labtool48-gtk labtool48-upi41ah-test labtool48-gtk-upi41ah-test
PYTHONPATH=src:tools .venv-productivity/bin/python tools/audit_upi41ah.py
ctest --test-dir build-c -R '^(upi41ah|gtk-upi41ah|mcu51|labtool48-gtk-mcu51-preflight)$' --output-on-failure
```

Resource assessment and compiler/pulse results are under
`analysis/upi41ah-integration/`. No VM, USB or real device is used by these audits.
