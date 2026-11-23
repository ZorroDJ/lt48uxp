# AEC 14.30 cFeon status-reader selector correction

Source audit, 2026-10-06. No hardware or Windows GUI qualification.

Affected GDCA definitions: D1e6/M1df, D1e7/M1e0, D1e8/M1e1,
D1e9/M1e2. Operation6 uses W480 (QY) or W481 (QX); operation17
binds the dual-entry W498 applet at 6000h.

Exact source GDCA SHA256:
`725a6bdefcae8aecc4f290e87f8c337eb405e677c2ed1df0ed8993c8091b12cf`.
Windows executable SHA256:
`7dc396cca11fa3d439e35332ce3903773febd730982b4cc9f2ae3e087d9f5bb3`.

The applet entry reads a data byte before dispatching commands. Zero selects
the four-address-byte path; nonzero selects its three-address-byte path.
The original status compact wave selects operation17, immediately sends
controller command5, then CS, WP and status opcode (05/35/15). It omits
the initial data selector. Consequently CS=58h is consumed as that selector,
and the command parameters start at 7Ch (WP) instead of 58h (CS).
The applet then drives the wrong logical pin values. This is directly visible
when executing its unchanged 8051 bytes, independently of emulated flash data.
The normal main-array waves already supply the required selector.

The correction inserts compact `3B 01` after each of the three operation17
selections, before command5. It selects the three-address-byte path while
leaving the original status opcode, dummy byte and configuration extraction
intact. Corrected execution emits status transactions on CS and the native
Protect/ReadCfg round trip passes for all four definitions. The correction
preserves unrelated status fields; it introduces no new protection policy.

This is an intentional Linux correction, not a claim of exact Windows
compatibility. The observed consequence is the malformed original applet
input/pin sequence. No claim is made about what the original Windows GUI
would display or whether a particular physical chip would be damaged.

Reproduction/evidence:

- `tools/assess_aec1430_spi.py`: exact D/M/W bindings and raw compact decoding.
- `analysis/aec1430-integration/spi-compiler-preparation.json`: original-resource
  native/reference compiler comparisons, including the affected procedures.
- `tools/audit_aec1430_status_selector.py`: unmodified applet entry/parameter
  trace before and after the selector is supplied.
- `analysis/aec1430-spi/protect-execution.json`: corrected register round trips.
- `analysis/aec1430-spi/workflows-execution.json`: full native workflows.
- `analysis/aec1430-spi/failures-execution.json`: dropped register writes
  rejected through actual readback, followed by cleanup.

Source implementation: `sources/cfeon/aec1430_status_selector.inc`, compiled
into the C owner. The caller requires the recognized AEC14.30 catalogue,
exact GDCA digest, admitted D/M/pin map/capacity tuple and exact W480/W481
binding. The transform additionally checks three exact command sites and the
compact length header. There is no vendor applet, FPGA or waveform payload
in this source file.
