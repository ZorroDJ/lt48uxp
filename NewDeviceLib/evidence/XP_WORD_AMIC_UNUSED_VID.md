# Exact-source AMIC SO44 VID fallback

Dataman 13.10 GDAAA__.rez D93/D94 use P1e (44 contacts, centered at ZIF3–46).
DEVINIT W27 and DEVRESET W2c reference absent logical pin 4. The original
zero-filled pin lookup is reproduced by the compiler's explicit analysis
policy; the general production policy deliberately rejects analog defaults.

These particular expansions have an identified effect. Init in modes 13/14
queues controller `22 01`, enabling the top-board UDN source at ZIF1, and sets
DAC3 to 10 V. Reset in the same modes queues `22 00`, disabling it. Digital
commands 0/1/2 from the same missing entries are the separately established
resident defaults. The analog route is **not** a no-op. All selected P1e groups
and logical pins lie in ZIF3–46; no replacement mapping to a device pin is made.

The authored guard permits only these exact default offsets/opcodes/logical
numbers. The C owner must also bind the original REZ digest, D93/D94, P1e,
W27/W2c, seven variables and the VID=1000/channel3 tuple. This preserves the
source sequence; it does not establish electrical suitability of another
adapter or a board revision. Original package wiring is required.

`tools/audit_xp_shared_word131_amic.py` checks four independent/native compiler
comparisons and 36 mode traces. Hardware qualification is pending. No claim is
made that the original software's unused-socket route is a bug.
