# IS25LP064A region boundary correction

Updated 2026-10-04. Four Dataman 13.10 D163 package entries are admitted
in native C and the normal GUI on audit basis. Hardware qualification is pending.

## Geometry and source discrepancy

The [ISSI IS25LP064A/032A datasheet](https://www.issi.com/WW/pdf/25LP032-64A-B.pdf)
lists the IS25LP064A main array through sector 2047, address 7FF000h–7FFFFFh:
8 MiB (64 Mbit). Its security information area is four additional 256-byte
rows, at command addresses 000000h, 001000h, 002000h and 003000h.
Main Page Program is 02h; Information Row Program is 62h.

The imported D163 tables already describe that geometry correctly: the main
segment occupies image offsets 000000h–7FFFFFh and the four rows follow at
800000h, 800100h, 800200h and 800300h. Those are *image offsets*; the rows
use their separate command address space, not physical address 800000h.

However, family M167 variable 16, `SecSiBuf`, is encoded as kind 2,
value 0000h, high word/channel 0020h: **00200000h**. Init binds that variable
to buffer 16. Read/Blank/Program/Verify branch on buffer 1 (logical image
offset) >= buffer 16. Thus the source threshold disagrees with its own
segment tables and the datasheet. The original host mapping is documented
in `analysis/addtab-spi-integration/host.log` (0046a640/0046a720).

Exact inputs:

- Dataman 13.10 executable SHA256:
  `f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
- GDCA___.rez SHA256:
  `5b391bb2789baa1496eb5db708c3d867b8e25ec2b01dc7ea030eb9c9a84e5a39`.
- Program W37a SHA256:
  `faf9df6f395b69b57a8741d5ed0bfa743cb4a6e20c12da81c67f2a6414bf507f`.

## Native correction

After validating the exact resource hash and definition/family/pinmap/capacity
tuple, `c/src/addtab_spi.c` requires the original variable encoding and
changes its in-memory high word to 0080h before compilation. The effective
threshold is **00800000h**. Unexpected encodings fail closed. Vendor files
are unchanged. The descriptor generator retains `source_main_end` alongside
the corrected `main_end`. Other definitions, including XMC D16b, are unchanged.

This is an explicit Linux correction, not exact reproduction of the source
threshold. The before/after test executes native host logic and unchanged
original 8051 applets against an SPI model. It demonstrates the source-data
discrepancy and the native correction; it is not a complete Windows execution
or a hardware corruption demonstration. No new confirmed Windows executable
bug has been added to the strict original-bugs register on this evidence alone.

## Regression evidence

`tools/audit_is25lp064a_boundary.py` tests pages at 0, 1FFF00h, 200000h,
200100h, 7FFF00h and all four information-row image offsets. It checks:

- emitted command and physical address (02h main, 62h information row);
- readback and public Verify, including deliberate mismatches;
- isolation between main memory and information rows;
- complete public workflow including erase and protection;
- all four information-row read/write routes.

Before correction, the last main page emitted 62h and failed its main-memory
check. Afterwards it emits 02h and passes. The expanded test also corrected
the offline model: address/dummy clocks must not prematurely return byte zero.
The model uses FF during dummy clocks; this is an injected bus condition,
not a physical measurement or a datasheet guarantee of driven dummy data.

Reports: `analysis/is25lp064a-boundary/` (`before.log`, `after.log`,
`boundaries.json`, `workflows.json`, `regions.json`, `regression.log`,
`gui.log`, `native.log`). The normal GUI preflight is tested for D163;
the native admission/range test covers the full admitted common core.
