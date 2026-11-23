# M1/C1 parallel-programming supply correction

Dataman13.10 GAB____.rez defines VCC330cV/channel2 for ATmega16M1,32M1 and32C1.
[Microchip/Atmel datasheet7647O](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7647-Automotive-Microcontrollers-ATmega16M1-32M1-64M1-32C1-64C1_datasheet.pdf),
section25.8.1, specifies4.5–5.5V for parallel programming entry; tables25-15/26-8
specify5V±10%. Ordinary2.7–5.5V operation is a different specification.

Three exact-key, source-SHA-bound records change only VCC to500cV, retaining
channel2. JSON numeric values combine low16-bit value and high16-bit channel,
following the existing variable-delta ABI. No vendor assets are modified.

Original x86/native callback comparisons record the unmodified330cV source
and the deliberately corrected500cV parameters. This establishes a bounded
source-definition discrepancy. No whole-Windows-workflow defect is claimed:
a later host/UI voltage override has not been excluded. No hardware test or
analogue qualification is claimed.
