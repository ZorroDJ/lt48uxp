# MCM68766 compact EPROM bindings

Motorola:MCM68766 and Freescale:MCM68766, Dataman13.10 only. Both bind
GA_____.rez D123/M8c/P55, 8192x8, 24-pin DIP. The source file hash is pinned.
Original REZ resources remain operator-supplied; this directory contains
only authored profiles, admission state and documentation. Execution is
implemented by the existing portable C owner in c/src/small_eprom.c.

Read, Blank Check, Program, Over-program, Verify and Compare are provided.
Electrical Erase and Secure are unavailable. UV erase is external. Source
blank value is FF; programming clears1bits. Program first checks blankness;
Over-program first rejects any requested0-to-1 transition. Both verify through
fresh reads at475/525cV. Partial ranges preserve data outside the range.

Reuse the class2000 host, GA buffer frame (mode7,address1,input2,output3),
operation slots0/1/2/6/7/9 and compact EPROM/idle-FPGA machinery. Both names
are exact aliases; the definition's FF blank is checked explicitly. The
bipolar PROM zero-blank policy must not apply. Shared old profiles retain
all prior behavior. The owner explicitly loads the verified BYO idle FPGA
before this new EPROM profile, as the original common pin engine requires.
No device-specific downloaded80C32 applet or FPGA configuration is present.

The source Read path uses13-bit address carry, G/VPP low,8-bit capture and
G/VPP high. Program uses five retries of the source1ms timed pulse at24V.
Program mode2 selects6V VCC; Over-program mode3 retains5V. These settings are
preserved from the original software, not inferred from the part name.
The1984 Motorola databook instead specifies5V programming VCC, nominal25V
pulse and2ms pulses (including cumulative-pulse requirements). These are
recorded as different procedures; neither equivalence nor an original-software
bug is claimed. Physical programming/endurance qualification remains pending.

Normal GTK prepares the six operations and explains FF blank, UV erase and
hardware qualification. Source audit includes original/reference/C compilation,
original x86 host and every Read/Program address/scratch/command sequence,
plus an independent pin/address/data/EPROM-pulse model executing actual C.
See docs/audit-qualification/MCM68766_PROGRESS.md and the sealed receipt.
