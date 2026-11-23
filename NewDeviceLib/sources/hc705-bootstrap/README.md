# HC705 C4A/C8/C9/D9 bootstrap owner

Eighteen exact Dataman13.10 catalogue aliases; seven source definitions and
four XC3030A configurations. Original controller/FPGA payloads remain in the
user-provided library. No original binary bytes are embedded here.

Read, Blank Check, Program, Verify and Compare use sparse source-addressed
images. Blank cells are00 for C4A/C8/C8A-*44Q, FF for C9/D9. Non-device image
bytes must beFF. Read data is actual serial capture, never expected-image data.
Program requires blank cells and verifies with fresh nominal/low/high reads.
Security programming and electrical erase are excluded. OTP cannot be erased;
windowed parts require UV erase. Original package wiring is mandatory.

C8 source checks terminal01 itself. C4A/C9 termination is explicitly consumed;
read timeout63 rejects the operation. Program acknowledgements are insufficient:
the source failure code can wrap to00 for expectedFF. Fresh Read resolves this.

Entry framing keeps one reset selector after each Read/Program worker launch.
Repeated identical selector writes in the source are omitted so they cannot be
consumed as initial stream data. All source rail/entry delays are retained.
This is an explicit Linux normalization, not a claim of a confirmed Windows bug.

Admission is staged8 then18 only after offline qualification. Hardware and
analogue qualification remain pending; this owner is audited against original
workflows, not claimed to have been physically tested on eighteen parts.

Each pass restores VPP and IRQ to their source low levels and asserts RESET.
The already-consumed stop is omitted from DEVRESET; its mode remains1 so the
conditional hardware restoration is executed. Whole-owner tests count these
actual ramp/pin transactions after every stopped worker. This corrects the
initial1.0.330 owner, which skipped that conditional between passes; final
power-off was still performed.1.0.331 supersedes that implementation.
