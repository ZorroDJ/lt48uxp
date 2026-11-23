# D120 program-address adaptation

The authored `sources/addtab-tail-word/program_address.h` converts both exact
three-byte source program setup frames to four-byte frames. The native owner
selects the existing W9d applet from the user's pinned Dataman 13.10 REZ in
place of D120's W11f. No original applet bytes are distributed here.

W9d emits explicit special-region unlock writes, avoiding D120's problematic
recovered-FPGA C2 sequence. This does not establish a defect in original silicon.
The source REZ digest, definition, operation and setup shape are checked; other
definitions retain their own applets. `analysis/addtab-tail92-batch/program-alternative.json`
records 160 main-word comparisons, five special unlock cases and two transformed
branches. Bounded native/MCU/FPGA trials cover main and special programming.
Hardware testing and permanent protection are not qualified.
