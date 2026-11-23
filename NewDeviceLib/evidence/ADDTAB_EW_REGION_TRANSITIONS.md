# Authored M29EW / GL-S region transitions

The native AddTab owner applies `sources/addtab-ew/region_transitions.h` only
after its exact GDCAA__.rez SHA-256 and definition/family/pinmap/capacity checks.
REZ digest: 2439a3725457dfc077c67ea7420701829f4d91d62155ffd070921c767d322ac8.

Db4 READ/BLANK/PROGRAM/VERIFY: move the requested address setup after the
conditional engineer entry. Original A000 applet bb745ca8... clears its low
address bytes after entering the region. The reordered compact operations are
byte-identical to the corresponding Spansion source routines, interpreted with
the Micron definition's own variables. This makes native nonzero-offset extra
region operations possible without changing their chip algorithms.

De4/De6 DEVRESET: replace the empty routine with logical RESET# low and a 1 ms
delay. Both operations are copied structurally from the existing source reset
and GETID behavior (pin 5); the next DEVINIT raises RESET#. This supplies the
reset needed between native repeated main/OTP stages while retaining VCC.
The correction does not claim that the original full-buffer Windows UI is faulty.

Native fault fixtures cover all five definitions, full/partial/page-crossing
ranges, extra-region preservation, ID retries, false-success verification,
transport faults and UNLOCK failures. The bounded original-app execution audit
and x86 segment audits are separate. The full chip/FPGA simulation stops on an
unmodeled startup FFFF write; it is not claimed to pass. No hardware was used.
