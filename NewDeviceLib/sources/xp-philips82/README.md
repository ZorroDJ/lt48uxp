# Philips/NXP XP extensions

Authored C profiles for source-bound operation routines loaded from the user’s own Dataman 13.10 installation. No original applets, FPGA images or vendor files are distributed here.

Current staged cohort: 82 entries. Complete C and normal GTK workflows are qualified separately before publication; see docs/audit-qualification/XP_PHILIPS_82_AUDIT.md. Package-specific programming adapters remain required. Hardware testing is pending.

Extended Configuration API: nine input bytes, [edit mask, eight value slots]; output is the declared field count. Boolean flags precede numeric bytes; getters provide labels and counts. Unselected values are read and preserved. Older RB2/RA2 APIs retain their existing eight-byte edits. Erase-before-program and erase-result policies are explicit profile fields, separate from configuration layout.
