# AT89C5114 configuration decoding correction

2026-10-05. Authored C decoder; no vendor payload is redistributed.

The exact Dataman 13.10 GAF D116/Ma1/P47 source W169 leaves a
lock-decoding intermediate in its scratch buffer while extracting flags.
Original x86 interpreter fragment execution and native execution reproduce
that result (tools/audit_at89_tail_configuration.py). This establishes the
fragment defect, not a claim about every Windows UI path.

The native correction decodes the captured raw byte: inverse bits 3..7 are
five flags; low bits 11/10/01 mean lock 0/1/2. Low 00 is rejected. Bit 2
is not exposed. The original W168 writer remains the configuration writer.

The production owner test exercises all 256 raw bytes, all 96 writable
flag/lock combinations, erase restoration, rejected/dropped writes and
cancellation. Original libraries and imported cache are checked separately.
Hardware qualification remains pending. Register-bit labels are used because
the exact C5114 field names have not been independently established.
