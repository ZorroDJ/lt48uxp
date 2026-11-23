# XM25QH64B source-consistency correction

Dataman 13.10 D16b (family M167, pin map P50, eight pins) uses the exact
qualified IS25LP064A compact procedures and MCU applets. Its own descriptors
specify an 8 MiB main array followed by four 256-byte information rows at
physical command addresses 0, 0x1000, 0x2000 and 0x3000. The shared family
encodes SecSiBuf as 2 MiB. The previously audited ISSI correction is scoped to
D163 and therefore does not apply to D16b automatically.

This separate exact-source/definition/family/pin-map/capacity-bound record
changes variable16 from 0x00200000 to 0x00800000, agreeing with the source's
own segment boundary. It applies to XM25QH64B *8SO and @8WSON. Vendor files
are unchanged; no new manufacturer ID, opcode or timing is invented.

Native qualification exercises the last main page, all four information rows,
configuration/protection and erase/preservation. Missing corrections must be
rejected before USB access. This is an explicit Linux source-consistency repair,
not a claim of independently confirmed physical XMC behavior or a demonstrated
whole-Windows failure. Physical testing remains pending.
