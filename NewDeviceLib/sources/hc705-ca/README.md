# HC705 C8A/C9A operation owner

Authored C owner. Exact Dataman 13.10 GADB resources remain bring-your-own;
no original REZ, controller applet or FPGA payload is included here.

The ten bindings are Motorola/Freescale C8A DIP40/*44 (first four), then
C9A DIP40/*44/*44QFP (six). Admission follows admission.h, independently of
catalogue visibility. Only sealed stages are complete support.

Read, Blank Check, Program, Verify and Compare use whole sparse images.
Program requires blank and fresh nominal/low/high raw readback. Source-selected
pin maps and adapters are required. Electrical Erase and Secure are unavailable.
Hardware and analogue qualification remain pending.

C consumes each worker terminal exactly once; source stop/capture/terminal
scratch checks are removed from LOAD/PROGRAM, retaining data and entry/reset
sequences. Raw Program acknowledgement is captured before source translation.
An authored RAM-only failure continuation waits for Stop instead of returning
early into the resident parser. No nonvolatile programmer changes are made.

See docs/audit-qualification/HC705_CA_PROGRESS.md for current qualification.

C9A images are16384 bytes; EPROM cells0020..004F,0100..3EFF,3FF0..3FFF.
The original preliminary transfers at image addresses001E/001F are consumed
but published as FF padding. The bootstrap's first mode argument is MOR bit0,
not the complete option byte. C checks its zero startup status and mode echo
before any ordinary programming data. An authored entry sets R1=0 explicitly.
C12MOR at3FF1 accepts00 or odd01..0F; security/reserved bits are rejected.
C12A mode requires FF in0100..0FFF, which its source Program sequence skips.
No Secure/Erase operation is introduced. Fresh raw Read verifies the actual
option byte, ordinary data, and nominal/low/high passes.
