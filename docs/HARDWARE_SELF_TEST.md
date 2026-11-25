# Programmer communication and empty-socket diagnostics

**Release update:** Included in `0.1.0-test.20261124.1`. The dated
implementation and qualification record below describes preparation before
publication. Its references to the previous release are historical.

Implemented locally 2026-11-23 in portable C, CLI and GTK. These diagnostics do
not add devices or change their qualification. Restart the rebuilt application.
The existing GitHub release has not been replaced.

## Running the tests

Open **Diagnostics → Self Test**:

- **Test communication echo** checks the original 55, AA, 00, FF sequence and
  then all 256 byte values. No socket output commands or FPGA uploads occur in
  this diagnostic itself. Normal transport initialization still occurs first.
- Remove every chip, adapter and fixture. Select **ZIF socket is completely
  empty — no chip, adapter or fixture**, then **Test empty socket digital pins**.
  Confirmation is consumed; each subsequent run requires fresh confirmation.
- **Cancel diagnostic** stops these diagnostics. The socket finalizer runs
  without cancellation. This button does not cancel unrelated device operations.

The installation/cache field supplies the owner's verified idle FPGA for the
socket test. Echo does not need a device library. A missing or unknown idle
image stops socket preparation before opening USB. The contact-check fixture
bypass does not bypass this diagnostic's empty-socket requirements.

CLI equivalents:

```sh
build-desktop/labtool48 programmer echo --json
build-desktop/labtool48 programmer test-empty-socket \
  --installation /absolute/path/to/owner-cache --confirm-empty --json
```

Use `--bus N --address N` if multiple programmers are connected. The established
CLI initialization flow may load an already configured, verified volatile FX2
image; if re-enumeration is required it reports that fact and the diagnostic
has not run. Repeat only using the re-enumerated programmer address.

## Protocol and test sequence

Echo uses resident `4C value` and the existing bridge `0E` grant/read method.
The original Windows communication check at 498010 and its four mismatch paths
were previously executed in the x86 audit. The additional 256 values extend
coverage; this is a byte echo test, not a bulk throughput or LPT test.

The socket test:

1. Clears rails, sources, sinks and relay shadows with resident `49 AA 55`.
2. Loads the exact 2778-byte XC3030A idle FPGA, SHA-256
   `1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab`.
3. Captures both resident47 contact-scan polarities. All twelve bytes must be
   zero. Any contact indication rejects the drive test. This guard cannot prove
   that an electrically invisible fixture or device has been removed.
4. Clears outputs again, then applies fourteen digital vectors: all low, all
   high, six binary channel-address signatures and their complements.
5. Uses resident42 writes to F000 with `4*(ZIF-1)+state`. After establishing a
   complete vector, reselects each channel without changing its state and reads
   F000 through resident43. The FPGA returns pad input in bit0.
6. Checks resident54 hardware fault status after each vector. A failing vector
   or hardware fault stops further patterns. Reports failing ZIF pins and first
   expected/observed mismatch. Full success is 672 comparisons.
7. Independently attempts resident49 cleanup on every attempted socket-test
   exit, including timeout, short transfer and cancellation. Cleanup failure
   overrides success. This releases digital outputs and clears analogue drives.

No resident4A generated RAM routine is needed. No identity, license, EEPROM
write, ATtiny exchange or nonvolatile firmware update is part of these tests.

## What a pass means

Digital patterns distinguish all48 channels within the recovered fixed-wiring
model and can expose stuck levels and bridges between channels. Readback is at
the FPGA pad, behind the socket's passive protection network. An open resistor
or disconnected ZIF contact may still allow local FPGA drive/readback to pass:
**a pass is not end-to-end socket-contact continuity certification**. External
loopback or measurement is required for that stronger claim.

These tests do not measure voltage accuracy, driver current, leakage,
high-impedance isolation, analogue transient timing, all relay contacts or
continuity beyond a fixture. The reported source/FPGA pin mapping is subject to
the existing hardware manual's physical-wiring evidence limits. These are
purpose-built diagnostics, not a complete reproduction of Windows Self Test.
The Windows routine's board EEPROM writes/security stages are excluded.

## Offline verification

- Original resident4C,42,43 instruction execution: 768 byte cases.
- Exact XC3030A fitted structural model: 1392 external-pad drive/read/release
  checks for these fourteen patterns. No internal nets are forced.
- Native endpoint fixture: 260 echo comparisons/all260 mismatch positions;
  672 pin comparisons; every pin stuck high/low; 48 shorted pin pairs;
  occupied socket, hardware fault, cancellation, exclusive-use and bad payload.
- Timeout and short transfer injected at every endpoint position in the nominal
  echo and socket executions. Cleanup and failure propagation are checked.
- Actual GUI workers and CLI dispatch tested with USB replaced by controlled
  endpoints; missing confirmation/resources rejected before USB.

Receipts and hardware-run outcome are recorded in
[data/hardware_self_test.json](data/hardware_self_test.json). Analogue behavior
is outside the offline model.

## Live qualification — 2026-11-23

The owner confirmed powered USB connection and a completely empty socket.
The bridge initially needed its previously verified volatile RAM firmware.
After loading it and re-enumerating, the real CLI execution passed:

- Echo: **260/260** comparisons.
- Empty-socket digital test: **14/14** patterns, **672/672** comparisons,
  **zero failing pins**.
- Socket cleanup: **completed successfully**.

No chips or fixtures were fitted. These measurements do not change the
end-to-end continuity or analogue limitations above. Source diagnostics were
also exercised through production GUI workers with controlled USB endpoints;
the physical run used the same C operation owners through the CLI.
