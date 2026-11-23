# ATmega signature metadata corrections

Thirteen exact Dataman13.10 entries use an expected third signature byte from
the P/PA variant for an A or older non-P part. The original source variables,
not programming routines or vendor assets, are corrected by exact-key,
SHA-bound NewDeviceLib records. P/PA aliases sharing a source definition retain
their existing IDs.

| Actual parts | Correct signature | Incorrect source third byte |
|---|---|---|
| ATmega48A | 1E 92 05 | 0A |
| ATmega88A, ATmega88/V | 1E 93 0A | 0F |
| ATmega168A, ATmega168/V | 1E 94 06 | 0B |

Primary evidence:
[Microchip AN2603, table5-1](https://onlinedocs.microchip.com/oxy/GUID-40C0ECBE-A0DB-4C41-A3E8-36526FC003B6-en-US-2/GUID-9962F7AD-84C2-4AB8-ACA4-8AAF009AD788.html)
and [older non-P migration table](https://onlinedocs.microchip.com/oxy/GUID-EDDC5F14-E7DB-4996-8AE5-E37AE12FEF7F-en-US-2/GUID-9962F7AD-84C2-4AB8-ACA4-8AAF009AD788.html).
[P/PA comparison](https://onlinedocs.microchip.com/oxy/GUID-C3FA204F-FF9A-47B3-BFC3-88C86A6E047C-en-US-2/GUID-9962F7AD-84C2-4AB8-ACA4-8AAF009AD788.html).

Original source CheckID execution with the documented identity, and the same
execution with the corrected variable, are recorded in the qualification
signature receipt. The bounded defect is rejection by the original CheckID
callback with the published device ID. No assertion is made that a Windows
operator could not override or disable that check.
