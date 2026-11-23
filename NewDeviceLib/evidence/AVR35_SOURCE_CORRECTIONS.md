# AVR35 source corrections

These are authored, exact-source corrections. Original REZ files remain intact.
Qualification is recorded separately in the AVR35 final receipt.

- ATtiny22/L D50/M3d/P0c: signature variable40 is03 in Dataman13.10. The
  manufacturer's ATtiny22/L datasheet lists1E9106. Source CheckID W49 explicitly
  compares allthree bytes, unlike AVR routes that ignore the third byte.
  Correct variable40 from03 to06; preserve every other variable and callback.
- ATtiny26L Dcc/Ma3/P44: source VCC variable0 is330centivolts on channel2.
  ATtiny26(L) datasheet1477K, Enter Programming Mode steps2/4, requires4.5–5.5V;
  Table58's parallel timing specification uses5V±10%. Correct to500centivolts
  on the same channel. The host's final effective policy is audited separately;
  the demonstrated discrepancy concerns the default source procedure.

The corrected original-x86 callbacks are compared with C, and independent
models use manufacturer identities/geometry. Removing a required correction
must reject admission before hardware access. These corrections do not imply
hardware qualification or repair of original vendor executables.
