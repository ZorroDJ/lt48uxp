# Authored GL-N partial-range address ordering

The native AddTab owner applies `sources/addtab-gln/region_transitions.h` only
inside the exact GDCAA__.rez source/definition closure. Source SHA-256:
2439a3725457dfc077c67ea7420701829f4d91d62155ffd070921c767d322ac8.

Dba/Dbb/Dbc/Dc1/Dc3/Dc4/Dc5/Dc8/Dc9/Dca Read, Blank Check and Verify set
C4/C8/CC/C6 before entering the extra region. The original A000 engineer applet
bb745ca8... ends its entry by clearing C4/C8. Native nonzero-offset special-region
requests consequently need to restore the address after entry. The authored
reordering is byte-identical to the later Ddf/De0/De1 source operations; all
original pin maps and definition variables remain in force. Compare uses Verify.

Program is unchanged: its original applet restores the address from the four
command parameters before each word/page. Moving that host sequence is unnecessary.
The fixture now models those parameters instead of discarding them.

The correction is restricted to those ten definitions and three operations;
malformed/truncated sequences are rejected. The independent comparison tool is
`tools/audit_addtab_gln84_corrections.py`. This is a native partial-range correction,
not a claim that the original full-buffer Windows workflow is faulty. No hardware
qualification is implied.

## GL-P / Numonyx extension

Db7/Dcc/Dcd/Dce use the identical older Read/Blank/Verify address-before-entry
sequences and the same engineer applet. The correction is also applied to these
four exact definitions. Twelve additional corrected routines match Ddf source
byte-for-byte. Their 32-word program applet restores its own address and remains
unchanged. See `tools/audit_addtab_glp32_corrections.py`.

## GL-N boot-sector extension

Db8/Db9/Dbf/Dc0 have the same address-before-entry ordering. Their twelve
Read/Blank/Verify sequences receive the same correction and match the later
Ddf source exactly. Ddd/Dde already use the later ordering and remain unchanged.
`tools/audit_addtab_gln20_corrections.py` checks all four new corrected definitions
and rejects malformed sequences. Original program payloads remain unchanged.
