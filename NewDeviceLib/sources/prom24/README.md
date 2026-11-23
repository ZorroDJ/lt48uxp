# AMD/TI PROM24 source profiles

Authored exact bindings extend the existing portable C PROM owner and normal
GTK route. Original resource files are bring-your-own. Read, Blank Check,
Program, Over-program, Verify and Compare are applicable; OTP has no Erase.
AMD blank is 00 (0-to-1 programming); TI M84 blank is FF (1-to-0). The per-stage source audits and hardware limitations are
in docs/audit-qualification/PROM_24_AUDIT.md and docs/data/prom24_stage*_qualification.json.
Reusable profiles, discoveries and metrics are in NewDeviceLib/drafts/prom24.
