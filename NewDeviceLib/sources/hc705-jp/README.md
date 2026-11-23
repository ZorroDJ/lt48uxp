# HC705 J/P native C owner

Exact Dataman13.10 Motorola/Freescale P6/A DIP28, P6 SO28, J2, J2(J1EMUL)
and P9 bindings. Original REZ, workers and FPGA bytes are supplied by the user.
Only authored C sources, membership/admission and documentation are distributed.

Read, Blank Check, Program, Verify and Compare use whole sparse binary images:
8192 bytes P6/P9,4096 J2,2048 J1EMUL. EPROM blank value00; holes must beFF.
No electrical Erase or Secure. P6 Program rejects SECURE set in1F00 bit7. P9 MOR0900 permits only bits0..2.
Program checks raw acknowledgements, stops on failure and requires blank, then uses fresh nominal/low/high raw readback.
Hardware and analogue qualification are pending. SO28 needs the source adapter.

Reuses the audited class2014 host, compiler, runtime, transport, cleanup and
GUI structure of the bootstrap18 owner. Separate workers, pin profiles and
entry sequences are qualified for J/P. P9 uses distinct Read and Program FPGA
images. P6A's1EFF MOR byte is retained explicitly, correcting the source mask.
P9's0900 MOR byte is retained too, with reserved bits rejected before Program.
A source-scoped timeout continuation returns63 rather than ambiguous00;
normal source Read instructions and results are unchanged. These bytes are
constructed from the user's original payload in RAM; no vendor payload is bundled.
DEVRESET retains the original low-rail ramp and RESET assertion; only the
already-consumed stop is omitted and queued RESET is flushed after every pass.
See docs/audit-qualification/HC705_JP_PROGRESS.md for current admission status.
