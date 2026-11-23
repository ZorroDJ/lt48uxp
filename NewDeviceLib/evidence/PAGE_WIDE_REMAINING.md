# Page-wide remaining definitions: release audit and host mask correction

2026-10-05. Authored compatibility rules in `sources/page-wide/remaining_guards.h`.
No vendor procedures, firmware or bitstreams are redistributed here.

The owner verifies GDBAA__.rez SHA-256
b889a62b78b8d63028e47b3c4c008b86f245fa96322c4f73336a5f882f19fe28
and the exact source definition/family/pin-map/capacity tuple before applying these rules.

## Sharp D57/D58/D5b/D5c/D5d/D5e

W97's five compact `04 08 01 02` instructions at offsets 0C,28,44,63,7F
refer to logical pin 8, absent from these pin maps. The original zero fallback
produces expanded words `0,2,1,22h,1,0`: resident command02 (the previously
audited no-op) then command22 with operand00. Variable1 is explicitly required
to select channel3. Resident handler0731..0741 writes00 to XDATA F800: **UDN
source ZIF1 off**. There is no DAC update, source enable or inferred pin mapping.

The native owner permits only this exact reset wave, six definitions, five
locations, logical pin8 and channel3; all other unresolved defaults still fail.
Native/reference comparisons retain the original compiled command sequence.

## Intel D3d: RD38F3340LLYD / RD38F3350LLZD

W6d's masks at2A and147 apply `AND 2` to only byte0 of DataIn, then compare its
DWORD against zero. Earlier two-byte OTP reads can leave byte1 nonzero. Original
USB reader004972C0 writes one byte; original interpreter44192B masks only the
specified width; comparator443970 compares the DWORD. The second-bank lock bit
can therefore be decoded as unlocked despite bit1 being clear.

The authored correction changes only the two AND lengths from1 to4. The mask
constant is unchanged (00000002). Electrical commands, addresses and OTP data
are unchanged. This is an intentional host calculation correction, not exact
Windows behavior.

`tools/audit_page_remaining.py` executes those original x86 instructions:
1024 corrected combinations pass; 384 original failures reproduce across four
upper-byte states and all256 captured bytes. USB transport calls are stubbed;
the original byte store, AND and comparator execute unmodified. Reachability
of nonzero byte1 follows W6d's first-bank OTP read loop: it combines two captured
bytes in DataIn before entering the second-bank scalar lock read. This evidence
establishes the calculation defect, not a complete Windows UI/hardware run.

Native/reference comparisons cover all eight catalogue entries. Full operation
and GUI reports are under `analysis/page-wide/remaining/`. Hardware testing is
pending. These resources do not add catalogue aliases or replace imported files.
