# C5132 timeout guard, experimental revision 1

Authored source: `sources/at89/c5132_timeout_guard.c`. This does not enable a
supported device. No original firmware is included.

The transformer accepts only the exact original W106 SHA-256
`efb53ad856520a7d3e33826b1438517ff6fbc04b54427da99f8058f2a6bdbfcf`.
Successful reads retain the original FPGA commands and returned byte. Polling
P3.2 is bounded to 65,536 samples. A timeout returns placeholder zero, releases
the read command and clears a sticky latch at SRAM 41FF. Later success cannot
clear it. R6 is preserved through scratch at 41FE. The helper at 4200 returns
the latch; resident command72 dispatches that helper. The owner must reserve
4100–4208 and check a status-bearing bridge response before accepting a capture.

Offline evidence:

- 32 native transformer input/hash/size checks.
- 1,282 instruction cases: successful reads match original output, MMIO,
  registers and accumulator; timeout boundary and sticky failure verified.
- 65,799 native query checks: complete prefix/latch spaces, transfer failures,
  short responses and cancellation.
- Nine native transport stage cases per installation: actual upload bytes,
  helper response gating and refusal of writes after guard failure.

The native powered stages load and check this candidate. The full operation
owner and its failure/cleanup testing are unfinished. Instruction timing,
electrical behavior and hardware qualification are not established. The source
lives in NewDeviceLib; derived bytes are built only from operator-supplied input.
