# HC705 B/X bootstrap operations

Authored C owner, Dataman13.10 BYO GADB___.rez. Normal C/API/GTK Read,
Blank Check, Program, Verify and Compare. No vendor binary is bundled.

## Chain from software to pins

Class2014 constructor00455CD0/vtable00780040 supplies the shared host route.
Exact D14/D15/D16/D17/D13 definitions choose pin map, variables, compact waves,
two original8051 workers and one of three FPGA configurations. The C compiler
and runtime translate the compact setup/range/transfer sequences into controller
commands. Workers at5100 and523B/526E/5239 write F000 FPGA commands,
C002/C003 control values and use C800/D800 host FIFO ports. Source Read workers
upload the embedded target RAM routines (parallel B16/B5, serial B32/X32).
The target routine returns actual memory data; the C owner never treats Program
echoes as a raw Read. Target-ready and data signals are explicit offline fixtures;
this is not a full target bootstrap ROM or analogue simulation.

Selected XC3030A GPIO and parallel-bus paths are audited against the decoded
configuration and known FPGA-to-ZIF wiring. F000 C0/data selects the data latch;
C4/control selects bus mode, while source98/99 commands toggle their GPIO path.
The original pin maps select GND/VCC/VPP/IRQ and bootstrap controls through
existing source controller commands and the established pin-driver routes.
Use the exact selected *52/@64/@64Q adapter. Source pin-count40 describes
its programmer-side interface, not a physical DIP40 MCU package.
All rails/voltages, worker/FPGA payloads and successful source transfers remain
source based. Analogue levels and timing require later hardware qualification.

## Images and policies

| Profile | Image bytes | Read regions (hex, inclusive) |
|---|---:|---|
| B16 |4000h|0020–004F,0100–01FF,0300–3DFF,3FF0–3FFF|
| B32/X32 |8000h|0100–01FF,0400–7DFF,7FDE,7FF0–7FFF|
| B5 |2000h|0020–004F,0100–01FF,0800–1EFF,1FF0–1FFF|

EPROM erased00; B16/B32/X32 EEPROM0100–01FF erasedFF; holesFF.
Only complete images are accepted. B32/X32 Program transfers vectors shifted
by one host address, then the mask-option byte separately; its schedule must
never be inferred from the Read map. Every image address is checked against
original Windows interpretation and actual C emission before admission.

Program requires erased EPROM and replaces EEPROM with the image, including
its options byte. Existing EEPROM data are not automatically preserved: read/save
and merge them into the desired image first. EEPROM SEC=0 is refused because
it would prevent the required subsequent raw Read. B5 factory bit7/fixed bit5 and B16/B32
reserved MOR bits7..5 are refused. X32 defines all eight MOR bits and is handled
separately. B5 EPP is a program-protection option, not a Read-protection bit.
Separate Secure and whole-chip electrical Erase are unavailable. EPROM UV erase
is external, where the package allows it. No programmer identity/security storage
is accessed by these workflows.

Program always verifies fresh nominal/low/high Reads. Raw Read output is only
published after terminal01 and successful cleanup. C consumes source Stop and
completion once, restores source low rails/RESET, flushes the final GPIO write,
then zeros DACs, loads verified idle, finalizes and sets Good/Error. Unknown
transport state reports cleanup failure rather than sending blind recovery traffic.
Original compact69(10) sets a ten-second watchdog (five-second minimum).
USB blocking timeout remains at least30seconds.

See ../../../docs/audit-qualification/HC705_BX_PROGRESS.md (repository docs) and
immutable analysis/hc705-bx10 qualification evidence. Primary sources:
https://www.nxp.com/docs/en/data-sheet/MC68HC05B6.pdf and
https://www.nxp.com/docs/en/data-sheet/MC68HC05X16.pdf .
