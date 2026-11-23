# FM16W08 native pin-engine investigation — 2026-10-02

**Status: diagnostic implementation, not qualified device support.**

## Scope

FM16W08-SG remains in the user's 1:1 SO28-to-DIP28 adapter, bottom aligned.
The user authorized destructive tests and requested the part be left FF.
No claim of successful clearing is made unless the final readbacks support it.

Implementation: `tools/native_fm16w08_pin_probe.c`, entirely native C. It uses
existing C USB transport, original resident firmware and the hash-verified
baseline FPGA (SHA-256
`1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab`).
It does **not** load or execute the original FRAM operation applets or their
specialized FPGA image. It is a standalone diagnostic, not a GUI entry.

The existing `lt48_pin_control_plan` is an offline full-reset planner. This
experiment instead implements incremental pin transactions so successive bus
cycles do not reset power. Its physical routes agree with the hardware manual.

## Explicit bus operations

- Physical package addresses A0..A12: pins 10,9,8,7,6,5,4,3,25,24,21,23,2.
- D0..D7: pins 11,12,13,15,16,17,18,19.
- /CE=20, /OE=22, /WE=27, GND=14, VDD=28; add 10 for ZIF positions.
- Address and write data are established while /CE is high.
- Writes have one /CE cycle, with either /WE-controlled or /CE-controlled
  termination. Data is released after deselection.
- Each read-bit sample has its own complete /CE cycle, entirely within one
  FX2 command-2C packet, including deselection before the USB response.
  There are eight accesses to the **same requested address** per byte;
  there is no access to the next address and no speculative prefetch.
- The queue's single read result at FC02 is consumed once per bit; it is not
  incorrectly treated as an eight-byte capture buffer.
- No host/USB wait is deliberately inserted while /CE is active.
- Repeated reads and write verification can run with the supply continuously on.

## Power handling and limits

Default VCC comes from DAC0 via the ZIF38 relay; GND uses the ZIF24 relay.
DAC setpoint is 5.0 V, inside the FM16W08 range. Direct serial frames and
synchronized RAM shadows avoid the resident relay-use-counter handlers.

During startup/shutdown the top UDN sources hold /CE, /OE and /WE inactive.
Their DAC3 supply and DAC0 are stepped in 100 mV requests with 1 ms holds,
with address/data FPGA outputs released during shutdown. FPGA control drive
is handed to/from these sources while inactive. Ground remains until the
ramp and hold finish. Emergency/final reset uses a fresh transport session so
a previously failed session cannot suppress the all-off command. SIGINT and
SIGTERM are handled and lead to cleanup. This cannot guarantee cleanup after
host death, USB removal, or a controller that stops responding.

**These are commanded setpoints, not measured waveforms.** The DAC stepping
has not proved the instantaneous VDD slope or actual /CE tracking at the
chip. A one-second hold before shutdown does not establish a fall-rate limit.
No new electrical ratings or complete physical pin-map qualification are
inferred from this experiment.

Alternative comparisons used (a) top UDN source at ZIF38 instead of its VCC
relay, or (b) TPIC sink at ZIF24 instead of its ground relay. They were separate
comparisons; they did not energize conflicting high/low sources on one pin.

## Results

Artifacts: `analysis/fram-native-pins-2026-10-02/`.

### Small pattern / boundary tests

Seven addresses: 0000, 0001, 007F, 0080, 010F, 0110, 1FFF.
Nine values at each address: 00, FF, 55, AA, 0F, F0, 1F, 3F, 7F.
Each write was followed by two powered reads.

| Mode | Mismatching cases out of 63 | Post-power-cycle 7F checks |
|---|---:|---:|
| Relay VCC/GND, /WE-controlled writes | 3 | 7/7 passed |
| Relay VCC/GND, /CE-controlled writes | 3 | 7/7 passed |
| UDN VCC, relay GND | 5 | 7/7 passed |
| Relay VCC, TPIC GND | 3 | 7/7 passed |

The three recurring default-path failures were FF at 0001, 0080 and 0110;
both immediate reads returned 00. Thus these failures occurred without a
power cycle between write and verification. The UDN supply comparison did
not cure them. Neither did changing the write termination edge or ground path.

Initial bring-up attempts did not complete setup. The transport watchdog
initialization and independent cleanup were corrected; a subsequent run
completed setup and produced the results above. The initial failure was not
isolated sufficiently to call it an actual socket-rail fault.

### Full 8192-byte tests, default relay paths

The address-dependent pattern is `(address ^ (address >> 8)) & 0xff`.
It includes every byte value and exercises both address bytes.

| Written pattern | First powered read | Second powered read | Read after power cycle |
|---|---:|---:|---:|
| Address-dependent pattern | 0 mismatches | 0 | 0 |
| All FF | 431 mismatches | 434 | 435 |

All six binary readbacks are retained as `memory-phase*-read*.bin`. The FF
failures were spread through the array, not limited to address zero. In the
post-cycle FF read, failing addresses extended from 0001 through 13C0; all
addresses with eight or more one bits passed in that particular read. This
correlation is an observation, not proof of a power or signal-integrity cause.

The address-dependent pattern is a substantial improvement over the previous
operation path. However multiple things changed together: exact-address reads,
FPGA image, timing, voltage and rail/control sequencing. The result therefore
**does not prove prefetch was the cause** of the original failures.

## Offline checks

`tools/test_native_fm16w08_pin_packets.c` checks 6,240 generated packets against
an independent pin-state contract: address changes only while deselected,
one /CE cycle, correct address/data, one read sample or write termination,
all data pins released at completion, and no changes to VCC/GND/NC pins.
Both write-edge styles and all 256 values are covered at boundary addresses.
These checks do not simulate analog behaviour or verify physical timing.

## Datasheet comparison

User-supplied `Components/FM1608.PDF` is Ramtron, July 28, 2000; pages 7–8
were checked. Common to FM1608 and FM16W08: address is latched on /CE falling,
and every new address requires a new /CE cycle. Neither is ordinary SRAM
with /CE permanently low.

| Requirement | FM1608 (2000) | FM16W08 Ramtron rev 1.2 / Cypress rev ** |
|---|---|---|
| VDD | 4.5–5.5 V | 2.7–5.5 V |
| Input high minimum | 2.0 V | 0.7 × VDD |
| Input high maximum (operating) | VDD+1.0 V | VDD+0.3 V |
| /CE active minimum | 120 ns | 70 ns at >=3 V; 80 ns below |
| /CE active maximum in AC table | 10,000 ns | No maximum in these AC tables |
| Read/write cycle minimum | 180 ns | 130 ns at >=3 V; 145 ns below |
| Precharge minimum | 60 ns | 60 ns at >=3 V; 65 ns below |
| VDD minimum to first access | 1 microsecond | 10 milliseconds |
| Minimum VDD fall-time slope | Not listed in that table | 100 / 30 microseconds per volt |

The FM16W08 prose mentions a maximum /CE-low time, despite no maximum being
listed in those AC tables. Do not silently import the FM1608 limit, or claim
that the prototype's pulse durations have been measured. The FM1608 is not
qualified by this work; it needs a separate profile and timing validation.

## Reproduction

```bash
cc -std=c11 -Wall -Wextra -Wno-misleading-indentation \
  -I c/include -I c/src tools/native_fm16w08_pin_probe.c \
  -L build-c -Wl,-rpath,"$PWD/build-c" -llabtool48 \
  -o build-c/native-fm16w08-pin-probe
cc -std=c11 -Wall -Wextra -Wno-misleading-indentation \
  -I c/include -I c/src tools/test_native_fm16w08_pin_packets.c \
  -L build-c -Wl,-rpath,"$PWD/build-c" -llabtool48 \
  -o build-c/test-native-fm16w08-pin-packets
build-c/test-native-fm16w08-pin-packets
```

Live modes are destructive and specific to this device and adapter:
`--probe`, `--probe-ce`, `--probe-top`, `--probe-sink`, `--memory OUTPUT_PREFIX`,
or `--clear OUTPUT_PREFIX`, following the private-cache path argument.
Use a fresh output prefix; files are created exclusively to preserve evidence.

## Remaining work

Resolve the powered FF failures before admitting this engine to normal device
support. Distinguish an actual write failure from a read/restore failure and
confirm loaded voltage/control levels when accessible. Retain the complete
power-cycle and boundary tests when changing timing or driver configuration.
The diagnostic engine is deliberately not counted as newly supported devices.

## Final hardware state

The final standalone `--clear` run (`final.jsonl`, `final.log`) wrote FF to
all 8192 addresses. Its first powered read had 441 mismatches, second 434,
and post-power-cycle read 430. All three full readbacks are preserved as
`final-phase1-read*.bin`. Therefore the part is **not confirmed erased**.
Final output shutdown completed successfully; the error LED was selected
to reflect failed verification. The last read is not a measurement after
the final shutdown itself.

Production `c/src/xp_eprom.c` still matches the saved pre-experiment baseline.
No diagnostic changes were promoted into the GUI or device support count.

## 30-second retention test with pins grounded

The user requested an SRAM-versus-nonvolatile check and elimination of I/O
back-power. The first retention run was cancelled cleanly to incorporate
that correction; its `retention30.*` files are not a completed retention test.

The completed `grounded30.*` run used the native engine to:

1. Write all 8192 bytes with `(address ^ (address >> 8)) & 0xff`.
2. Read and compare while VDD remained on: **zero mismatches**.
3. Deselect the chip, drive address/data pins low **before** ramp-down,
   keep /CE, /OE and /WE inactive via the tracking source during the ramp,
   and command VDD to zero. /CE was not deliberately driven low while the
   supply was valid, which would have started another memory access.
4. Reset sources and supply relays off, retain a ground reference, then
   enable TPIC sinks on **all 28 package contacts, including VDD and the
   control pins**. Wait 10 ms for discharge. All 28 digital pin samples
   read low. These samples are logic-level checks, not analog measurements.
5. Hold that configuration with no USB/pin transactions for **30.000319
   seconds**, measured with CLOCK_MONOTONIC. USB supplies the programmer,
   but no socket contact is commanded high during this interval.
6. Release the sinks before restoring device power, wait for startup, and
   read the complete array twice: **zero mismatches on both reads**.

All three 8192-byte readbacks are identical, SHA-256
`5d2b4b8245a5191b93aa7660bc149070d22bea7a2904be7c769f461d758d06d5`.
The test completed with successful cleanup. Source snapshot:
`analysis/fram-native-pins-2026-10-02/engine-grounded30.c`.

This provides strong evidence of nonvolatile retention under the tested
conditions, contrary to ordinary unpowered SRAM relying on residual charge
or I/O back-power. It is not a silicon-authenticity determination or a
long-term retention/endurance qualification, and does not resolve the
data-dependent FF failures.

The current diagnostic exposes `--retention OUTPUT_PREFIX`. Its `--clear`
mode now performs one powered verification after writing FF; the earlier
three-read clear results above came from the earlier diagnostic version.

After the successful retention test, FF clearing was attempted again. The
complete powered read reported **436 mismatches** (`after-retention.*`).
Cleanup succeeded and outputs were released; the part remains **not confirmed
erased**. This is the latest hardware state in this report.

## Write-timing margin comparison

User requested longer data setup, write pulse and data hold intervals.
`--timing` now compares six profiles with added nominal microseconds:

| Setup | Pulse | Hold | Failing comparisons / total |
|---:|---:|---:|---:|
| 0 | 0 | 0 | 18 / 168 |
| 20 | 0 | 0 | 18 / 168 |
| 0 | 20 | 0 | 18 / 168 |
| 0 | 0 | 20 | 18 / 168 |
| 20 | 20 | 20 | 18 / 168 |
| 100 | 100 | 100 | 18 / 168 |

Both /WE- and /CE-controlled writes were used, three repetitions each,
at 0000, 0001, 007F, 0080, 010F, 0110 and 1FFF. Values were FF, 7F, AA
and 55. Before each comparison, zero was written and read back correctly,
controlling the prior data state. /OE stayed high throughout writes.
Power remained on, using the original native diagnostic's 5 V relay path.
Two unchanged native reads followed every tested write.

Result: **108 mismatches among 1008 comparisons**, with no transport/status
errors. Every failure was FF at 0001, 0080 or 0110, returning 00 on both
reads. Each of those failures repeated in all 36 timing/style/round
combinations. No tested added delay improved the result. This weakens
insufficient write timing margin as an explanation; read timing, electrical
levels, and whether the read itself changes data remain unresolved.

Delays use the original resident 4E loop, encoded inside the same FX2 queue,
so no USB scheduling wait is introduced while /CE is low. The numbers are
nominal added delays, not oscilloscope-measured pin timing. The zero profile
still includes command execution overhead. This is an FM16W08 experiment;
it is not admitted for FM1608, which specifies a 10 microsecond maximum
/CE-active interval.

Offline packet-contract tests now cover **18,528 packets**, including
both write styles with zero, 20 and 100 microsecond additions. They pass.
All evidence and the tested source snapshot are under
`analysis/fram-timing-2026-10-02/`. No production device definition or GUI
workflow was changed.

Final clearing after the timing comparison: all 8192 bytes were written FF;
the powered verification found **410 mismatches**. Cleanup succeeded,
outputs are off, and the Error LED reflects failed verification. The part
is still not confirmed erased. See `analysis/fram-timing-2026-10-02/final.*`.

## One /CE cycle per byte comparison

`LT48_FRAM_READ=byte` selects a second native read method. With /CE high it
sets the address and releases all data pads, then asserts /CE and /OE.
Original FX2 command 09 selects and samples the eight data pads in order,
returning eight separate bits. It does not change the address. A final queue
deasserts /OE and /CE even if capture fails. Thus the chip sees one access
per requested byte, without prefetch. The eight electrical bit samples are
sequential within that access, not a simultaneous eight-bit latch.

The command-09 listing at 071C–0755 shows that every selector is ORed with
02 (released input state). It therefore cannot also drive /CE high inside
that same command. The diagnostic uses three USB exchanges (select, sample,
deselect); the /CE-active interval is consequently host-scheduling-dependent.
This is a known limitation and is not admitted for the FM1608 10 us maximum.
The original eight-cycle diagnostic remains selectable with
`LT48_FRAM_READ=bit`, and remains the default.

Three repetitions of the 63-case native probe gave identical results:
FF at 0001, 0080 and 0110 returned 00 on both reads; the other 60 cases
passed each time. All 21 post-power-cycle 7F checks passed. This means the
extra restore cycles of the earlier read method are **not necessary** for
those three failures. The changed pulse duration also means this comparison
does not isolate access count as the only electrical variable.

Final all-FF write and single-cycle full read: **363 mismatches**, cleanup
successful, error LED set. The part is not confirmed erased. The count
should not be treated as a proven improvement over earlier FF runs, whose
counts varied; the three controlled small-test failures remained unchanged.
Artifacts: `analysis/fram-single-cycle-2026-10-02/`, including source snapshot,
three probe logs and the full final readback. Production GUI support was not
changed.

### Related ST reset-glitch report

The [ST community report](https://community.st.com/stm32-mcus-products-25/data-corruption-at-address-0x00-in-the-fram-memory-after-a-stm32u595zjt6q-reset-158046)
describes address-zero corruption on another parallel FRAM during STM32
warm reset, with reported CS/WR glitches. The author corrected the initial
claim about blank firmware: FMC initialization was required. ST suggested
gating CS inactive through reset; a participant reported that similar logic
worked. This is a useful analogue for control glitches during mode changes,
not proof of the cause here. Our remaining FF failures occur during powered
accesses without a controller reset or FPGA reload between write and read.

### Related TL866 FRAM reports

The [EEVblog discussion](https://www.eevblog.com/forum/repair/tl866-wont-program-fram/)
contains both serial FM24Cxx and parallel FM1608/FM16W08 reports; the Arduino
workaround concerns the serial parts and should not be carried over here.
Parallel-part users reported repeatable errors at selected addresses despite
the chips working in their target equipment. In April 2021, one participant
reported working FM16W08 read/write support with software V10.55.

[Post 21](https://www.eevblog.com/forum/repair/tl866-wont-program-fram/#msg3546675)
reports that the older firmware actually wrote correctly but read back
incorrectly, duplicating the preceding byte at a recalled interval of 256
or 512 bytes; the author explicitly qualified their recollection of the
interval. They reported successful programming after a firmware update.
These are user reports, not an inspected TL866 implementation or a confirmed
LabTool defect.

This reinforces the need to distinguish write failure, readback failure and
read-induced modification. It does not identify our cause: the native probe
accesses individual bytes, and repeated FF-to-00 results at 0001, 0080 and
0110 do not establish the reported TL866 block-boundary duplication pattern.
No hardware operation or production-support change was made from this report.

## Powered read-order comparison

The native diagnostic now has `--read-order`. For each of seven addresses,
four values (00, FF, 7F, AA), both write styles, and both reader orders, it
writes 00 and then the test value without an intervening read. It alternates
the eight-cycle reader and the one-cycle command-09 reader for four reads.
Every order starts with fresh writes. Power remains on throughout the test.

Result: **112 trials, 448 completed reads, 48 mismatching reads in 12 trials**.
Only FF at 0001, 0080 and 0110 failed, returning 00 on all four reads,
regardless of write style or which reader ran first. There were no transport
errors or changes between successive reads in a trial. This excludes a
reader-specific discrepancy in this comparison and shows no progressive
change across those reads. It does not distinguish an incorrect write from
a shared read fault or corruption on the very first access. Both readers
still use the same baseline FPGA, pin mapping and physical data path.

Evidence and tested source: `analysis/fram-read-order-2026-10-02/`.
The subsequent full FF clearing attempt returned **359 mismatches** with
the one-cycle reader. Cleanup succeeded; outputs are off, Error LED set.
The device is not confirmed erased. No GUI qualification was added.


## /OE-active write comparison — 2026-10-03

[Separate live report](FM16W08_OE_WRITE_TRIAL_2026-10-03.md): /OE low during
write entry/body, with /OE raised before /WE rises to avoid the 10 ns data
turnaround, produced the same failures as both high-/OE controls. Each mode
failed 6/70 cases with each of two read methods. Final FF readback had 476
non-FF bytes; power-off cleanup succeeded. This does not qualify the device
or reproduce the page-9 final edge exactly.
