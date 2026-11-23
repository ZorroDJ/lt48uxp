# Ordinary HC908 JK/JL owner

38 exact Motorola/Freescale aliases; definitions D29/D2a–D2f/D5f–D61.
Portable C and normal GTK use the six-operation profile
`hc908-jkjl-six-operation`. Vendor REZ/applets/FPGA remain bring-your-own.

`jk_jl_profiles.h` carries exact geometry, volatile/nonvolatile protection,
monitor auth address, original serial helpers and delay count per source
profile; `jk_jl_members.h` fixes permitted names and definition bindings.
`jk_jl_operations.inc` composes the existing C transport/monitor/RAM owner.

The complete workflow and deliberate Linux replacements are documented in
`docs/audit-qualification/HC908_JK_JL_WORKFLOW.md`; staged qualification is
in `docs/audit-qualification/HC908_JK_JL_PROGRESS.md`. Hardware pending.

Unlike QT/QY, FFC0/C1 are monitor ROM; they are FF image padding. JK1/JK3/JL3
unlock volatile FE09. JL8/JL16 include FFCF protection, FFD0 mask options
and FFDC–FFFF vectors; nonvolatile protection is written last. Program
requires blank Flash. Wrong keys reject reads/program; documented mass
erase works without knowing the current key.

Source PTB3=0 selects OSC1/2:4MHz JK1/JK3/JL3,2.4MHz JL8/JL16 bus. Do not
borrow QT/QY divide or trim assumptions. Program delay counts38/20 and
erase delay counts38/21 are instruction-audited at those source clocks.
