# P89LPC flash source bindings

Author-created exact name/definition/family/pin-map/geometry bindings for the
Dataman13.10 GAB source. Original REZ, controller firmware and FPGA assets are
bring-your-own. Common operation owner: `c/src/lpc_flash.c`.

Code Read is unavailable in the original procedures. The owner explicitly
accepted publishing their original applicable operation set: Blank Check,
Program, Over-program, Verify, Compare, Erase, Secure, Configuration Read/Edit.
Code verification compares sector CRCs, with collision risk; it does not read
or recover code bytes and cannot locate a bad byte within a failing sector.

Only profiles covered by a sealed stage receipt are supported. Source compiler,
original interpreter traffic/configuration, byte-element host range evidence,
independent target effects/faults and normal GTK execution routes are audited.
Physical qualification remains pending. See `docs/audit-qualification/LPC_86_AUDIT.md`.
