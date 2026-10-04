# Supported devices

Generated 2026-10-04 from the native C support registry.

**4263 unique supported device entries**, alphabetically grouped by manufacturer. Includes explicitly identified native extensions. Package variants and catalogue aliases are separate entries; these are not counts of physical samples tested.

Only entries with **all applicable operations executable** are included. Support is specific to the listed source database; a name in an imported catalogue alone does not imply support.

## Operations and evidence

- **Mask ROM**: Read, Verify and Compare; no writable or erasable array.
- **EPROM**: Read, Blank Check, Program, Over-program, Verify and Compare. UV erasure requires an external eraser; electrical Erase is not applicable.
- **W27C512/W27E512**: the six memory operations plus source electrical Erase and a full-array FF check after power removal.
- **SPI and XP parallel flash**: Read, Blank Check, Program, Over-program, Verify, Compare and whole-chip Erase. Secure is included where the original procedure supplies it; on Intel QB/QH and PCT/SST25VF064C it permanently locks OTP, rather than protecting the main array. Protection/status are read back; source package adapters remain mandatory.
- **Timekeeper RAM**: RAM writes/FF fill preserve RTC registers; Configuration explicitly writes clock/control fields. Full Dallas RTC reads acknowledge alarms/restart watchdog. Hardware qualification pending.
- **EEPROM and battery SRAM**: full memory operations and FF fill. SPI EEPROM additionally exposes block protection. Battery SRAM testing does not establish battery lifetime.
- **Parallel and serial NVRAM**: Read, Blank Check, Program, Over-program, Verify and Compare. Program includes STORE to EEPROM and fresh-power verification. Clear with an all-FF image.
- **Parallel FRAM**: Read, Blank Check, Program, Over-program, Verify and Compare. Arbitrary nonblank rewrites; clear by programming an all-FF buffer. No separate device erase command.
- **Microwire EEPROM**: Read, Blank Check, Program, Over-program, Verify, Compare and whole-chip Erase. Program can rewrite nonblank words in both bit directions; x16 files use little-endian words.
- **8751/87C51 MCU**: Read, Blank Check, Program, Over-program, Verify, Compare and Secure, plus the separate encryption array where present. No electrical erase; windowed parts need UV erasure and OTP parts cannot be erased.
- **Bipolar PROM**: Read, Blank Check, Program, Over-program, Verify and Compare. One-time programmable; no erase. x4 images use the low nibble of one byte per address.
- **PLD**: complete profiles also require Secure/Protect. A working Read/Blank/Program/Verify/Erase subset is listed under partial support until security programming is implemented.
- **HW**: direct physical evidence for the named sample and stated operations. “Not recorded” means no direct test is recorded for that entry; shared-engine evidence is separate.
- **Reference engine**: original-software host/applet reconstruction and representative hardware evidence underpin the admitted engine. This is not a complete independent Windows replay for each entry.
- **Exact group equivalence**: original resources and complete native transaction plans were compared against an admitted reference group; no new physical test is implied.
- **Windows replay**: matched original execution evidence, where explicitly recorded.
- Package suffixes retain the original catalogue spelling. Package variants may require adapters.

## Database coverage

| Database | Complete entries |
| --- | ---: |
| AEC 7.60 | 24 |
| Dataman 13.10 | 4263 |

Database counts overlap; the list below merges identical manufacturer/name pairs and retains version-specific evidence.


## ACTRANS

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| AC25LV010 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AC25LV512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Adesto

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| AT25F512B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT26DF081A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| RM24C128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM24C128A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM24C32B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM24C32B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM24C64B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM24C64B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM24EP128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM24EP32B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM24EP64B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| RM25C128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: RM25C SPI resistive-memory source audit |
| RM25C128A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: RM25C SPI resistive-memory source audit |
| RM25C32B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: RM25C SPI resistive-memory source audit |
| RM25C32B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: RM25C SPI resistive-memory source audit |
| RM25C64B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: RM25C SPI resistive-memory source audit |
| RM25C64B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: RM25C SPI resistive-memory source audit |

## Alliance

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| AS29LV400B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AS29LV400T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AS29LV800B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AS29LV800T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |

## Altera

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| EPC1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1064 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1064 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1064V | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1064V *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1213 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1213 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1441 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1441 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1441 as 1213 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC1441 as 1213 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC2 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPC2 *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| EPCQ256 @16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCS1 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCS128 @16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCS16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCS4 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCS64 @16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPM3032A *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM3032A *44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM3064A *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM3064A *44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7032 *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7032 *44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7032AE *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7032AE *44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7032AEas7032S *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7032AEas7032S*44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7032V *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7032V *44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064 *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064 *44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064 @100 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064 @100TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064 @68 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064 @84 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064AE *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064AE *44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064AEas7064 *44 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |
| EPM7064AEas7064*44TQ | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Altera EPM POF operations source audit |

## AMD

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| Am1736 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| Am1765 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| Am27128 | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| Am27128 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Reference engine |
| Am27128A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27128A *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am2732 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| Am2732A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| Am2732B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| Am27512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am2764 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am2764A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am2764A *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27C010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C020 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C040 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C100 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C1024 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C128 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27C128 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27C2048 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C2048 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27C256 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27C400 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C400 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C4096 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C4096 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27C64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27C64 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27H010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27H256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| Am27HB010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am2817A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| Am2864A/B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| Am29DL400BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29DL400BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29DL800BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29DL800BB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29DL800BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29DL800BT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F100B *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — AMD AM29F100B-90SI, SO44 in straight 1:1 adapter: All eight native operations qualified across 2026-09-28 and 2026-10-01: Read, Blank Check, Program, Over-program, Verify, Compare, Erase, Secure. All five sectors protected; attempted writes blocked; unprotect/erase and reprogram recovery passed. Final 128 KiB FF and unprotected. GTK uses the same C owner; GUI preflight tested. | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F100B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F100T *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F100T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F200AB *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F200AB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F200AT *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F200AT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F200B *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F200B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F200BB *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F200BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F200BT *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F200BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F200T *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F200T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F400AB *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F400AT *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F400B *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F400B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F400BB *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F400BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F400BT *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F400BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F400T *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F400T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F800AB *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F800AT *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F800B *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F800B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F800BB *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F800BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F800BT *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F800BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| AM29F800T *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29F800T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29LV400BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29LV400BB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29LV400BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29LV400BT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29LV800BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29LV800BB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29LV800BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am29LV800BT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| Am8751H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am8753H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am87C51 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am87C51 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am87C521 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am87C521 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am87C52T2 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am87C52T2 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am87C541 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am87C541 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| Am9716/2716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| PAL16L8-4 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16L8-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16L8-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16L8D/2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R4-4 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R4-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R4-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R4D/2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R6-4 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R6-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R6-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R6D/2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R8-4 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R8-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R8-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R8D/2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20L8-10/2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20L8-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20L8-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R4-10/2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R4-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R4-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R6-10/2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R6-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R6-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R8-10/2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R8-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL20R8-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PALCE 16V8H-10/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-5/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-7/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-10/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Yes — AMD PALCE16V8Q (selected D0 catalogue entry -25; speed marking not rechecked): Native C and GTK Read/Blank/Program/Verify/Erase/Secure; protected readback, erase/reprogram exact readback; final erased | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Z-15/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Z-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 20V8H-10 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8H-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8H-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8H-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8H-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8Q-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8Q-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8Q-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8Q-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 22V10H-10/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-5/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-7/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Q-10/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Q-15/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Yes — AMD PALCE22V10Q-15PC/5: Native C full Read, Blank Check, nontrivial full Program, Verify, Erase; independent 5,828-fuse readback. Secure not qualified. | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Q-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Q-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Z-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALLV16V8-10/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALLV16V8Z-25/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALLV16V8Z-30/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |

## AMIC

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| A25L010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010A @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010AM *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010AO *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L010O-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L016 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L016 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L016 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L016-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L016M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L016N-F *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020A-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020AM-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020AO-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020B-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020BM-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020BO-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020CM *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020CO *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L020O-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040A-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040AM-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040AO-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040AQ1-F @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040AQ4-F @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040M *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040O *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L040O-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L05PT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L05PT *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L05PT *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L05PU | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L05PU *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L05PU *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L080-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L080A-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L080AM-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L080AO-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L080M *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L080M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L080O *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L080O-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L10PT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L10PT *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L10PT *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L10PU | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L10PU *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L10PU *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L16PT *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L16PT *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L16PU *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L16PU *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L20PT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L20PT *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L20PT *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L20PU | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L20PU *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L20PU *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L40PT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L40PT *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L40PT *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L40PU | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L40PU *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L40PU *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L42PU *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L5120-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512A @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512AM *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512AO *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L512M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L80P *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L80P *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L82PT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L82PT *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L82PT *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L82PU | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L82PU *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25L82PU *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS040 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS040 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS040-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS040M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS040O-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS080 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS080 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS080-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS080M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS080O-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS512-F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS512M-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LS512O-F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LTQ016 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A25LTQ016 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| A27020L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| A275308 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| A276308/E | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| A276308A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| A277308 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| A278308 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| A278308A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |

## Aplus Flash

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| AF24BC01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC01 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC128 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC16 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC32 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF24BC64 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AF25BC08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC08 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC16 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC32 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF25BC64 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AF93BC46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC46(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC66(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC86 *SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC86 *TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC86(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC86(x8)*SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AF93BC86(x8)*TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## ASAHI KASEI

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 93C45A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C45AF | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C45AV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C47 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C55A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C55AF | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C55AV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C57 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C65AF | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C65AV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C67 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AK28C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| AK28C64/F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| AK93C45B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AK93C55B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AK93C65B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AK93C75B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## AT&T

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 17128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| 17128F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| 1736 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| 1736F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| 1765 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| 1765F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |

## ATC

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C02W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC02W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LC04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## Atmel

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 17C002 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17C002 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17C002 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17C002A *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17C002A *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17C010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C010 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C010A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C010A *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C010A *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C020 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C020A *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C040A *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17C128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17C128 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17C256 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17C512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C512 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C512A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C512A *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17C65 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17C65 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV002 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17LV002 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17LV002 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17LV002 @8QFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17LV002A *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17LV002A *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17LV010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV010 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV010A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV010A *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV010A *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV020 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV020A *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV040 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17LV040A *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: AT17C/LV002/040 original RAM applet and configuration audit |
| 17LV128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV128 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV128A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV128A *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV256 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV256A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV256A *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV512 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV512A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV512A *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Larger compact AT17 memory and configuration audit |
| 17LV65 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 17LV65 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: Compact AT17C/LV memory and polarity audit |
| 24C01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C01A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — Atmel 24C04N-SI SO8 in straight 1:1 adapter: Seven native C/GTK memory operations; stable original backup, pattern/inverse, partial 16-byte-page and 256-byte-bank crossing, nonzero-offset read, negative Verify/Compare/Blank checks, fresh-power readback; final 512 FF verified at 4.75/5.25 V. Hardware WP enforcement not separately tested. | Dataman 13.10: Compact serial EEPROM source audit |
| 24C04B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04B @8MLP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04C *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04C @UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C08A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08B @8MLP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08C *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08C @UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C16B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C16B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C16B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C21 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C32C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C64C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 25010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25020 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 59C11 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 59C12/22 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 59C13 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| AT17F040 *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F040A *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F040A *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F080 *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F080 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F080 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F080A *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F080A *32TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F16 *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F16 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F16 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F16A *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F32 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17F32A *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: AT17F original word-stream, erase and independent readback audit |
| AT17N002 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N002 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N002 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N010 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N040 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N256 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT17N512 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, read | Not recorded | Dataman 13.10: AT17N original EEPROM and RESET/OE polarity audit |
| AT24C01C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C01C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C01C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C01C *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C01D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C01D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C01D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C01D *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C02C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C02C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C02C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C02C *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C02D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C02D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C02D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C02D *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C08D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C08D *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C08D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C08D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C08D *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C1024 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C128C *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16C @UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16D *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C16D @UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C256C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C256C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C256C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C256C *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32C *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32C *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32D *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32D *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32E *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32E *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C32E *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512C *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512C(150mil)8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C512C(208mil)8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64C *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64C *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64D *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24C64D *UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24CM01 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24CM01 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT24CM02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| AT25010A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25010A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25020A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25020A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25040A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25040A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AT25080A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25080A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25080A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25080B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25080B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25080B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AT25128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25128A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25128B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25128B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25160 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AT25160A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25160A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25160A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25160B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25160B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25160B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AT25256A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25256A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25256B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25256B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25320 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AT25320A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25320A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25320A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25320B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25320B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25320B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25512 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25640 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| AT25640A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25640A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25640A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25640B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25640B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25640B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25DF041A *SO 150mil | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25DF041A *SO 200mil | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25DF321 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25DF321 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25F1024 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25F1024A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25F2048 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25F4096 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25F512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25F512A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT25HP256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT25HP512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| AT26DF041 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT26DF081A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT26DF161 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT26DF161A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT26DF321 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT26DF321 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AT26F004 *SO(150mil) | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| AT26F004 *SO(200mil) | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| AT27BV400 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27BV400 *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C010L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C020 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C1024/L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C1024/L *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C128 | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| AT27C2048/L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C2048/L *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27C256R | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27C4096 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C4096 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27C512R | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27C516 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27HC1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27HC1024 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27HC256/L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27HC256R | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27HC256RL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27HC64/L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| AT27LV010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV020 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV1024/L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV1024/L *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV1026 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV256R | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT27LV4096 *40TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV4096 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| AT27LV512R | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| AT28BV256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| AT28C010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — Atmel AT28C010-15PC DIP32: Eight native C/GTK operations; full 128 KiB patterns/inverse, A16/page-boundary partial-write preservation, negative Verify/Compare/Blank checks, protected-program sequence and subsequent erase; final 131072 FF. Blocked-write enforcement not separately tested. | Dataman 13.10: Parallel page EEPROM source audits |
| AT28C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| AT28C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| AT28C16-T *28TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| AT28C17 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| AT28C256/F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — Atmel AT28C256-15PI DIP28: Native C/GTK eight operations; patterns/inverse, partial-page and A14-boundary preservation, negative Verify/Compare/Blank checks, protected-program sequence and subsequent erase; final 32768 FF. Blocked-write enforcement not separately tested. | Dataman 13.10: Parallel page EEPROM source audits |
| AT28HC256/L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| AT28HC64/L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| AT28HC64B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| AT28PC64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| AT29C257 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| AT30TSE004 @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| AT34C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| AT34C02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| AT34C02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| AT34C02B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, read | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| AT34C02B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, read | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| AT34C02B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, read | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| AT34C04 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| AT34C04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| AT34C04 @8UDFN  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| AT45BCM021B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D011 *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D011 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D021 *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D021 *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D021 *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D041 *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D041 *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D041 *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D081 *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D081 *32TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D161 *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D161 *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D161 *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45D321 *32TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB011B *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB011B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB021B *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB021B *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB021B *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB021B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB041B *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB041B *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB041B *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB041B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB081B *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB081B *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB081B *32TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB161B *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB161B *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB161B *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB321B *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB321B *32TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB321C *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT45DB321C *8SON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: Legacy DataFlash original applet/FPGA and margin audit |
| AT90S/LS2323 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| AT90S/LS2343 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — AT90S2343 DIP8 in direct socket: 2026-09-29 native C: all eight operations; full 2048-byte Flash +128-byte EEPROM pattern, independent readback, 4.75/5.25 V verifies, negative Verify/Compare/Blank, RCEN disabled and restored enabled, Secure, protected Read/Blank/Verify/Compare rejection without erase, final full FF read and Blank Check; unlocked, original RCEN enabled. GTK preflight and actual fuse controls tested offline. | Dataman 13.10: AVR native source audit |
| AT90S1200 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — AT90S1200 DIP20 in direct socket: 2026-09-29 native C: all eight operations; nonblank chip erased without backup; full 1024-byte Flash + 64-byte EEPROM pattern, 4.75/5.25 V verification, independent readback, negative Verify/Compare/Blank, SPIEN/RCEN changed and restored, Secure/protected-blank rejection, final erase with unlocked all-FF read. GTK production preflight tested offline. | Dataman 13.10: AVR native source audit |
| AT90S2313 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — AT90S2313 DIP20 in direct socket: 2026-09-29 native C: all eight operations; full 2048-byte Flash + 128-byte EEPROM pattern, 4.75/5.25 V verification, independent readback, negative Verify/Compare/Blank, SPIEN/FSTRT changed and restored, Secure/protected read and protected-blank rejection, final erase with unlocked all-FF read. GTK production preflight tested offline. | Dataman 13.10: AVR native source audit |
| AT90S4414 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| AT90S4414 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| AT90S4434 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| AT90S8515 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| AT90S8515 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| AT90S8535 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — AT90S8535 DIP40 in direct socket: 2026-09-29 native C: all eight operations; full 8192-byte Flash + 512-byte EEPROM pattern, 4.75/5.25 V verification, independent readback, negative Verify/Compare/Blank, SPIEN/FSTRT changed and restored, Secure/protected-blank rejection, final erase and unlocked all-FF read. GTK production preflight tested offline. | Dataman 13.10: AVR native source audit |
| ATtiny12/V/L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — ATtiny12V DIP8 in direct socket: 2026-09-29 native C: all eight operations; 1024-byte Flash + 64-byte EEPROM pattern and independent read, 4.75/5.25 V verifies, negative Verify/Compare/Blank, full fuse byte changed B3 then restored52, calibration29 preserved, Secure/protected-blank rejection, final erase and unlocked all-FF read. GTK preflight and fuse controls tested offline. | Dataman 13.10: AVR native source audit |
| ATtiny24/V | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Native DIP/A alias: original procedure + manufacturer compatibility audit |
| ATtiny24/V *14SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| ATtiny24A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — ATtiny24A DIP14 in direct socket; explicit native A-revision/DIP alias: 2026-09-29 native C: all eight operations; full 2048-byte Flash +128-byte EEPROM pattern, page commits, 4.75/5.25 V verification, independent readback, negative Verify/Compare/Blank, all three fuse bytes changed/restored, EESAVE-enabled erase with fuse restoration, Secure/protected-blank rejection, final all-FF read. Original62 DF FF/calibration4E preserved. GTK preflight and fuse controls tested offline. | Dataman 13.10: Native DIP/A alias: original procedure + manufacturer compatibility audit |
| ATtiny44/V | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Native DIP/A alias: original procedure + manufacturer compatibility audit |
| ATtiny44/V *14SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| ATtiny44A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Native DIP/A alias: original procedure + manufacturer compatibility audit |
| ATtiny84/V | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Native DIP/A alias: original procedure + manufacturer compatibility audit |
| ATtiny84/V *14SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: AVR native source audit |
| ATtiny84A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Native DIP/A alias: original procedure + manufacturer compatibility audit |

## Boya

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| BY25D05AS *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D05AS 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D05AS 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D10AS *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D10AS 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D10AS 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D16AS *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D16AS 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D16AS 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D20AS *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D20AS 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D20AS 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D40AS *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D40AS 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D40AS 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D80AS *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D80AS 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BY25D80AS 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Catalyst

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 24WC128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24WC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 34AC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34AC02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34AC02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34FC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34FC02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34FC02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34RC02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34WC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34WC02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 34WC02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 59C11/A/H/I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46J | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46J(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C57 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| CAT24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C02AI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C02I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C04I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C08I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C16I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C208 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C32 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: X24C44 / X2444 and CAT24C44 serial NVRAM audit |
| CAT24C44 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: X24C44 / X2444 and CAT24C44 serial NVRAM audit |
| CAT24C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C64 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24FC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24LC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC02AI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC02I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC04I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC08I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC16I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24WC32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24WC64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24WC64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT25C01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| CAT25C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| CAT25C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| CAT25C128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| CAT25C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| CAT25C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| CAT25C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| CAT27010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| CAT27128A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT27128A *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT27256 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT27512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT27512 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT2764A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT2764A *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT27C210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| CAT27C210 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| CAT27HC256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT27HC256 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CAT28C16A/AI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| CAT28C17A/AI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| CAT28C256/I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| CAT28C512P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — Catalyst CAT28C512P DIP32: Native extension, eight C/GTK operations; full patterns/inverse, partial-page/A15 boundary preservation, negative Verify/Compare/Blank checks, protected-program sequence and subsequent erase; final 65536 FF. Blocked-write enforcement not separately tested. | Dataman 13.10: Native extension: CAT28C512 datasheet + source composition |
| CAT28C64A/AI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| CAT28C64B/BI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| CAT28C65A/AI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| CAT28C65B/BI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| CAT35C102/I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| CAT64LC10 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC10J/W *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC10S/V *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC10U/Y *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC10UR/YR*8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC20J/W *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC20S/V *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC20U/Y *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC20UR/YR*8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC40 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC40J/W *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC40S/V *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC40U/Y *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT64LC40UR/YR*8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |

## cFeon

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| EN25B10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B10T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B16T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B20T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B40-G *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B40-H *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B40T-G *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B40T-H *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B80T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25E40A(2A) *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25E40A(2A) @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P05 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P40 @8VDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P40-G *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P40-H *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN29LV160AB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160AB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160AT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160AT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320(A)B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320(A)T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320AB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320AT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320BB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320BT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320CB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320CB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320CT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320CT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400AB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400AB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400AT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400AT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400B  *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400T  *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800AB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800AT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800B  *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800BB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800BT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800CB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800CB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800CT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800CT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800T  *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |

## Chrontel

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| CH9904 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |

## Cypress

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 27H010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| Am27C512 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CY27C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CY27H256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CY27H256 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CY27H512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |

## Dallas

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| DS1220AB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Dallas battery SRAM source audit |
| DS1220Y | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Dallas battery SRAM source audit |
| DS1225AB/AD | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Dallas battery SRAM source audit |
| DS1225Y | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Dallas battery SRAM source audit |
| DS1230Y/AB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Dallas battery SRAM source audit |
| DS1245Y/AB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Dallas battery SRAM source audit |
| DS1250Y/AB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Dallas battery SRAM source audit |
| DS1386-32K | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: RTC-aware native workflow audit; hardware testing pending |
| DS1386-8K | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: RTC-aware native workflow audit; hardware testing pending |
| DS1486 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: RTC-aware native workflow audit; hardware testing pending |

## EFST

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| F25L004A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |

## ELAN

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| EM25LV010 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EM25LV512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## EON

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| EN25B10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B10T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B16T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B20T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B40-G *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B40-H *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B40T-G *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B40T-H *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25B80T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P05 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P40 @8VDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P40-G *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P40-H *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN25P80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EN29LV160AB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160AB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160AT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160AT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV160T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320(A)B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320(A)T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320AB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320AT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320BB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV320BT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400AB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400AB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400AT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400AT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400B  *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV400JB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| EN29LV400JT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| EN29LV400T  *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800AB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800AT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800B  *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800BB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800BB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800BT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800CB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800CB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800CT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800CT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |
| EN29LV800JB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| EN29LV800JT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| EN29LV800T  *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: EN29LV array, two-stage ID and sector-protection audit; hardware testing pending |

## ESMT

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| F25L004A (150mil) | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| F25L004A (200mil) | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| F25L008A | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| F25L008A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| F25L016A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| F25L01PA *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L01PA @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L01PA(2D) *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L01PA(2D) *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L01PTA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L01PTA *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L01PTA *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L01PTA @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PA *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PA @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PA(2F) 150mil | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PA(2F) 200mil | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PA(2F) @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PTA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PTA *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PTA *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L02PTA @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PA *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PA @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PA(2D) 150mil | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PA(2D) 200mil | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PA(2D) @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PTA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PTA *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PTA *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04PTA @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L04UA *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| F25L05PA(2D) *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| F25L05PA(2D) *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Excelsemi

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| ES25M16A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ES25M80A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ES25M80A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ES25P16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ES25P80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ES29LV400DB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV400DB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV400DT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV400DT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV400EB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV400EB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV400ET *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV400ET @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV800DB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV800DB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV800DT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV800DT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV800EB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV800EB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV800ET *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| ES29LV800ET @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |

## Exel

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 93C46/P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| XL2864A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| XL2865A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| XL28C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| XL28C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| XL28C64B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| XLE28C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| XLS2864A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| XLS28C16A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| XLS28C17A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| XLS28C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| XLS28C64B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |

## Fairchild

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| NM27C020Q | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NM27C040Q | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NM27C128Q | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NM27C256Q | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NM27C512Q | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NM27LV010 *32TS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C16B/BQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| NMC27C32B/BQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| NMC27C64Q | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |

## FMD

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| FT24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02AW | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02AW *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02AW *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C02AW @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C04 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C04A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C04A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C04A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C08 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C08A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C08A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C08A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C1024A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C1024A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C128A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C128A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C16 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C16A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C16A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C16A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C256A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C256A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C256A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C32 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C32A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C32A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C512A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C512A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C512A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C64 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C64A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C64A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT24C64A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FT25C08A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C08A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C08A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C16A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C16A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C16A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C32A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C32A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C64A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C64A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25C64A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| FT25H02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25H02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25H02 *8TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25H04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25H04 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25H04 *8TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25L02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25L02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25L02 *8TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25L02 @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25L04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25L04 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25L04 *8TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT25L04 @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| FT93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FT93C46-R | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FT93C46A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FT93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FT93C56A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FT93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FT93C66A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## Freescale

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 2716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |

## FUDAN

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| FM24C02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C02A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C02A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C02B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C02B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C02B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C02C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C02C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C02C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C04A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C04A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C04A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C04B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C04B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C04B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C08A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C08A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C08A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C08B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C08B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C08B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C1024A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C1024A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C1024A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C128A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C128A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C16A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C16A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C16A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C16B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C16B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C16B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C256A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C256A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C256A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C32A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C32A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C512A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C512A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C512A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C64A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C64A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM24C64A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| FM93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FM93C46(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FM93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FM93C56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FM93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| FM93C66(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## Fujitsu

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 8742 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: MCS48 source audit; see individual hardware evidence |
| MB85R256 *28SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Parallel FRAM source audit |
| MB85RC04V *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC16 *8SON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC16V *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC1MT *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC256V *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC512T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC64A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RC64V *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| MB85RS128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MB85RS128B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MB85RS16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MB85RS16 *8SON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MB85RS1MT *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MB85RS256A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MB85RS256B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MB85RS2MT *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MB85RS512T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MB85RS64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MB85RS64V *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| MBM27128 | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| MBM27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MBM2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| MBM27C1000 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MBM27C1001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MBM27C1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MBM27C1024 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MBM27C1028 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MBM27C128/P | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| MBM27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MBM27C256A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MBM27C256AP | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MBM27C256H | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MBM27C4096 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MBM27C512/P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MBM27C64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| MBM29DL400B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29DL400T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29DL800B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29DL800T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F200B *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F200B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F200BA *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F200BA *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F200T *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F200T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F200TA *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F200TA *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F400B *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F400B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F400T *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F400T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F800B *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F800B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F800T *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29F800T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29LV400B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29LV400T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29LV800B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29LV800B @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29LV800T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM29LV800T @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MBM8532/2732 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |

## Generalplus

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| GPR25L005E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GPR25L011E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GPR25L021B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GPR25L041B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Giantec

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| GT24C01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| GT24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| GT24C02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C02A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C02A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C02A @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| GT24C04A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C04A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C04A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C04A @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| GT24C08A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C08A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C08A @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C1024 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C1024 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C128 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C128A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C128B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C128B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C128B @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| GT24C16A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C16A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C16A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C16A @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C256A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C256A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C256A @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C256C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C256C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C256C @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32A @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C32B @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C64 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C64A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C64A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT24C64A @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| GT25C01 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C01 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C01 *8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C04 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C08 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C128 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| GT25C128A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| GT25C128A *8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| GT25C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C16 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C32 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C32A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C32A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT25C64 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GT34C02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, read | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| GT34C02A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, read | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| GT34C02A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, read | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| GT34TS02 @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, read | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| GT93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C46A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C56A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C66A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C66A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C66A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C76 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| GT93C86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## GigaDevice

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| GD25B16B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B16B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B16B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B16B @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B32B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B32B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B32B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B32B @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B64B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B64B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B64B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25B64B @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D05B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D05B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D05B *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D05C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D05C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D05C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D05C *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D10B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D10B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D10B *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D10C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D10C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D10C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D10C *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D20C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D20C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D20C *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D20C SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D20C SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D40C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D40C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D40C *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D40C SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D40C SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D80C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D80C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D80C *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D80C SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25D80C SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25F40 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25F40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25F80 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25F80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q10 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q10 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q16 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q16 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q20 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q20 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q40 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q40 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q40 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q40 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q512 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q512 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q80 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q80 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25Q80 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD20C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD20C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD20C *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD20C SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD20C SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD40C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD40C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD40C *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD40C SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD40C SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD80C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD80C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD80C *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD80C SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| GD25WD80C SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D05 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D05 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D05 *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D10 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D10 *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D16 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D16 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D20 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D20 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D20 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D40 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D40 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D40 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D40 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D80 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D80 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MD25D80 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Hitachi

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 27C4001G | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 462716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| 462732 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| HD4074818FS @80 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HD4074818H @80 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HD407L4818FS @80 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HD407L4818H @80 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HN27128AG/AP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27256G/P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27512G/P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HN27C101 (uP only) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C101AG/AP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C101G/P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C1024HG | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HN27C1024HG *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HN27C128 (uP 10Bit) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C128 (uP only) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C256 (uP 10Bit) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C256 (uP only) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C256AG/HG | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C256G/FP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C301AG/AP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C301G/P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN27C4000G | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HN27C4096G/FP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HN27C4096G/FP *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HN27C64G | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN482764G/P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Hitachi XP EPROM source audit; HW testing pending |
| HN58C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| HN58C65 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |

## HK

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| HK24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C04 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C08 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C128 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C16 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C32 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C512 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| HK24C64 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |

## HOLTEK

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| HT24LC01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| HT24LC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| HT24LC04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| HT24LC08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| HT24LC16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| HT24LC32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| HT25LC512 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| HT27C010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27C020 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27C040 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27C4096 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27C4096 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HT27C512-B *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HT27LC010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27LC020 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27LC4096 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27LC4096 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| HT27LC512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HT27LC512-B *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HT93LC46-A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC46-A(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC46-B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC46-B(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC56-A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC56-A(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC56-B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC56-B(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC66-A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC66-A(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC66-B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HT93LC66-B(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## HUAHONG

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| BH25D10C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BH25D10C SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BH25D10C SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BH25D40C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BH25D40C SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| BH25D40C SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Hyundai(Hynix)

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| HY2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HY27C64A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| HY29LV400BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| HY29LV400TT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| HY29LV800BT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| HY29LV800TT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |

## ICE

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 25P05 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 27C010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| ICE27LC512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| ICE37LC512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |

## ICT

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 27CX010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27CX256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 93C46/AP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93CX46/AP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93CX56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93CX66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## Infineon.siemens

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| SDA2516-5 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| SDA2526-5 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| SDA2546-5 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| SDA2586-5 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| SDA3526-5 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| SDA3546-5 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| SDA3586-5 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |

## Intel

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 27010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27128 | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| 27128A/B | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Yes — Intel D27128A-3: Full read, full-image program and independent readback; does not assert every A/B variant was tested | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| 2716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| 27210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27210 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 2732 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| 27512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 2764A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| 27C010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C010A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C020 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C040 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C100 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C128 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C128 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C210 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C220 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C220 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C240 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C240 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C256 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C400 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C400 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C512 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C64 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 28C17A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| 8741/8741A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: MCS48 source audit; see individual hardware evidence |
| 8742 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: MCS48 source audit; see individual hardware evidence |
| 8748/8748H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: MCS48 source audit; see individual hardware evidence |
| 8749H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Yes — Intel D8749H, DIP40: Native C Read, zero-state Blank Check, Program across the 2 KiB array, Over-program, margin Verify and Compare; boundary updates and wrong-image/forbidden-transition rejection; independent final full read matched. Final programmed; UV erase required. GTK production preflight passed. | Dataman 13.10: MCS48 source audit; see individual hardware evidence |
| 8751/H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 8751BH | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 8751BH *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 8751H-8 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 8752/H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 8752BH | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 8752BH *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 87C257 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 87C257 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 87C42 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: MCS48 source audit; see individual hardware evidence |
| 87C51 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C51 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C51FA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C51FA *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C51FB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C51FB *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C51FC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Yes — Intel D87C51FC: Native C full Read/Over-program/margin Verify/Compare; normal Program on one blank byte; 64-byte encryption confirmed through full code readback; Secure level 3 blocks readout. Final programmed/protected, UV erase required. Full blank-chip Program and UV recovery not tested. | Dataman 13.10: 8751/87C51 source audit |
| 87C51FC *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C52 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C52 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C54 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C54 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C58 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87C58 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87L51FA *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87L51FB *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87L51FC *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87L52 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87L54 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| 87L58 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure | Not recorded | Dataman 13.10: 8751/87C51 source audit |
| EPCQ128A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCQ16A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCQ32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCQ4A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| EPCQ64A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| P27128A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27128A *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27128B | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| P27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27256 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P2764A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P2764A *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27C128 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27C128 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27C256 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27C64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| P27C64 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| QB25F016S33B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QB25F016S33T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QB25F160S33B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QB25F160S33T *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QB25F320S33B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QB25F320S33T *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QB25F640S33B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QB25F640S33T *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QH25F016S33B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QH25F016S33T *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QH25F160S33B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QH25F160S33T *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QH25F320S33B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QH25F320S33T *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QH25F640S33B *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| QH25F640S33T *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## ISSI

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 24C01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 27HC010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| IS25LD010 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD010 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD020 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD020 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD040 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD040 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD040 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD256C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD256C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD256C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS25LD512 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| IS27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| IS27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| IS27HC256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| IS27HC512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |

## Lattice

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| GAL16V8 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL16V8A/B/C/Z | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native GAL + source audit |
| GAL16V8D | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Yes — Lattice GAL16V8D-15LP: Native C and GTK full Read/Blank/Program/Verify/Erase/Secure; protected array readback and erase/reprogram exact recovery; final erased | Dataman 13.10: Native GAL + source audit |
| GAL18V10/B | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL22V10/B/C/D | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| PALCE 16V8H-10/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-5/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8H-7/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-10/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Q-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Z-15/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 16V8Z-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALCE 20V8H-10 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8H-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8H-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8H-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8H-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8Q-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8Q-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8Q-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 20V8Q-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: PALCE20V8 geometry, voltage branch and Secure source audit |
| PALCE 22V10H-10/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-15 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-15/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-5/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10H-7/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Q-10/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Q-15/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Q-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Q-25/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALCE 22V10Z-25 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE22 + source audit, PALCE22 Secure source audit; no Secure HW test |
| PALLV16V8-10/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALLV16V8Z-25/5 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |
| PALLV16V8Z-30/4 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: Native PALCE16 + Secure audit |

## Lucent(AT&T)

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 17128A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1736A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1765A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |

## Macronix

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| KH25L1005A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L1006E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L1021E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L1021E *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L1021E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L2006E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L4006E*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L4006E*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L4026E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L5121E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L5121E *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L5121E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L512A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L512E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L512E *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L512E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L6473F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L6473F @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L8036D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L8036D SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25L8036D SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25V16066*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25V16066*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25V40066*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25V40066*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25V80066*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| KH25V80066*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1005 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1005A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1005C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1006E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1006E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1006F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1006F @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1021E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1021E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1025 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1026E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L12801F *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1605 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1605A *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1605A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1608D *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1608D @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1608D*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1608D*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1608E SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L1608E SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2005 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2006E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2006E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2006E @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2016E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2025 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2025C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2026 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2026C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2026D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L2026E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L3205 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L3205A *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L3208D *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L3208D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L3208D @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L3208E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L3208E @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4004 *32TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| MX25L4005 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4005 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4005A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4005A*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4005A*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4005C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4005C*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4005C*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4006E | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4006E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4006E @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4006E*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4006E*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4025 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4025 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4025C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4026D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L4026E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L5121E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L5121E *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L5121E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L512A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L512C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L512E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L512E *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L512E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L6405 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L6408D *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L6408D @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L6408E *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L6408E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L6408E @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L6473F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L6473F @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L8005 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L8005 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L8005 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L8008E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L8008E *8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L8025 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25L8025 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1006E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1006E *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1006E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1006F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1006F *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1006F @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V16066 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V16066*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V16066*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1606F @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1606F*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V1606F*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V20066*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V20066*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V2006E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V2006E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V2006E @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V4005 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V4005 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V40066 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V40066*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V40066*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V4006E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V4006E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V4006E @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V512E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V512E *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V512E @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V8005 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V8005 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V80066 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V80066 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V80066*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V80066*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V8006E *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX25V8006E @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| MX27C1000 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C1000A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C1000A(NEW ID) | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C1001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C1024/A/B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C1024/A/B *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C2000 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C2000A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C2000A(NEW ID) | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C2048 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C2048 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MX27C4000 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C4000A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C4096 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C4096 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C4100 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C4100 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MX27C64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| MX27C8000 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C8000A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C8100 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27C8100 *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MX27L512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MX29F100B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F100T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F200B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F200CB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F200CT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F200T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F400B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F400CB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F400CT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F400T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F800B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F800CB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F800CB @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F800CT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F800CT @48FBGA | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MX29F800T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |

## Microchip

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 24AA01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA01 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA01 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA01 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA02 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — Microchip 24AA04I SO8 in straight 1:1 adapter: Seven native C/GTK memory operations; stable original backup, pattern/inverse, partial page and 256-byte-bank crossing, negative Verify/Compare/Blank checks, fresh-power readback; final 512 FF, verified at 3.04/3.36 V. Hardware WP enforcement not separately qualified. | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA04 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA04 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA08 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA08 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA1025 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA1025 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA16 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA16 *5SOT(L) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA16 *5SOT(R) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA16 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA164 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24AA174 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24AA256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA256 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA256 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA256 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA32A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA32A *8MSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA512 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA64 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA64F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA64F *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA64F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24AA65 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C01A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C02A *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C02A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C02A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C04A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C04A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C08B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08B *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C08B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C16B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C16B *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C16B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C16B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24C65 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24FC1025 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24FC1025 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24FC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24FC256 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24FC256 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24FC256 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24FC512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24FC512 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24FC512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC01B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC01B *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC01B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC01B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LC025 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LC02B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC02B *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC02B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC02B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC04B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC04B *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC04B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC04B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC04B @8MLP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC08B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC08B *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC08B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC08B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC1025 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC1025 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC128 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LC128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC164 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LC16B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC16B *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC16B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC16B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC21 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC256 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC256 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC256 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC32A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC32A *8MSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC41 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LC512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC512 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC515 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LC64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC64 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC64F | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC64F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| 24LC65 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24LCS52 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 25AA010A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA010A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA010A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA010A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA010A @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA020A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA020A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA020A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA020A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA020A @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25AA040A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA040A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA040A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA040A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA040A @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA080D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA1024 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA128 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA512 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25AA640 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25C040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25C080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25C160 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25C320 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25C320 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25LC010A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC010A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC010A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC010A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC010A @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC020A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC020A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC020A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC020A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC020A @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25LC040A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC040A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC040A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC040A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC040A @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC080D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC1024 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC128 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC512 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| 25LC640 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C128 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27HC256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27HC256L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27HC64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27HC64L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 28C04A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| 28C16A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| 28C17A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| 37LV128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 37LV36 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 37LV65 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 59C11 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 85C72 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 85C82 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 85C92 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93AA46A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA46B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA46C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA46C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA56A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA56B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA56C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA56C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA66A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA66B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA66C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA66C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA76A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA76B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA76C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA76C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA86A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA86B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA86C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93AA86C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C06 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46B *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56B *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66B *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C76 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C76(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C76A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C76A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C76B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C76B *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C76C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C76C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86A *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86B *SOT23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46/B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46BX | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46CX | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46CX *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56/B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56BX | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66/B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66BX | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC76 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC76(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC76A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC76B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC76C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC76C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC86(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC86A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC86B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC86C(x16) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC86C(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LCS56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93LCS66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| PIC12F609 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — PIC12F609 in direct socket: 2026-09-29 native C: all eight operations; full 1024-word pattern, four user IDs/configuration, low/high verify, independent readback, negative Verify/Compare/Blank, Secure/protected read and final erase. Calibration 0x2008 excluded. GTK production preflight tested offline. | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC12F609 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC12F615 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC12F615 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16F610 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16F610 *14SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16F610 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16F616 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16F616 *14SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16F616 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16F84 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — PIC16F84 (previous physical test): Prior reference executor only: read/erase, single-word program/verify and protection. Complete new C owner is source-audited, not hardware-qualified. | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16HV610 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16HV610 *14SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16HV610 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16HV616 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16HV616 *14SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC16HV616 *14TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC native source audit; see per-sample hardware scope |
| PIC18F13K50 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18F13K50 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18F13K50 *20SS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18F13K50 @20QFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18F14K50 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — PIC18F14K50 (previous physical test): Prior reference executor only: main-image read/blank/erase, 16-byte flash program/verify and protection. Complete new C owner, EEPROM/configuration paths are source-audited, not newly hardware-qualified. | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18F14K50 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18F14K50 *20SS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18F14K50 @20QFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18LF13K50 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18LF13K50 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18LF13K50 *20SS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18LF13K50 @20QFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18LF14K50 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18LF14K50 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18LF14K50 *20SS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |
| PIC18LF14K50 @20QFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: PIC18 native source audit; prior reference hardware tests |

## Micron

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| M25P10A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P10A @8MB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P10A @8MP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 @8MC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 @8ME | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 @8MP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P20 @8MP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P32 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P32 @8ME | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P32 @8MP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P40 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P40 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P40 @8MB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P40 @8MC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P40 @8MP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P80 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P80 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P80 @8MC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P80 @8MP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## MIRA

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| AC25LV010 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| AC25LV512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Mitsubishi

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| M5L27128K | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| M5L2716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| M5L27256K | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M5L2732 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| M5L27512K | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M5L2764K | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| M5M27128P | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| M5M27256P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M5M27512P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M5M2764P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| M5M27C100K/P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C101K/P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C102K/P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C102K/P *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C128K | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| M5M27C201K | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C202K | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C202K *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C256AK | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M5M27C256K/P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M5M27C401K | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C402K | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C402K *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M5M27C512AP | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |

## Mosel Vitelic

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| MF29F200BA *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29F200TA *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29F400BA *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29F400TA *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29F800B/E *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29F800T/E *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29LV400B/E *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29LV400T/E *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29LV800B/E *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| MF29LV800T/E *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |

## Mostek

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 2716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |

## Motorola

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 2716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |

## MStar

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| PS25LV040 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| PS25LV512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Nantronics

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| N25S10 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S10 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S10 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S10 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S16 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S16 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S16 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S20 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S20 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S20 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S32 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S32 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S40 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S40 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S40 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S40 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S80 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S80 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S80 SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S80 SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S80B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S80B @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S80B SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| N25S80B SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## NEC

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 8741A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: MCS48 source audit; see individual hardware evidence |
| 8748H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: MCS48 source audit; see individual hardware evidence |
| D2716D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| D2732D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| D28C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| D28C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| uPD27128 | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| uPD27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| uPD2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| uPD27C1000 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C1000A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C1001A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C1001D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C1024 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C1024A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C1024A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C2001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| uPD27C256A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| uPD27C4000 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C4001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C4096 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| uPD27C64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| uPD27C8000 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| uPD27C8001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |

## NexFlash

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| NX25B40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| NX25F011B-3V *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F011B-3V *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F011B-5V *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F011B-5V *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F021B-3V *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F021B-3V *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F021B-5V *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F021B-5V *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F041B-3V *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F041B-3V *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F041B-5V *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25F041B-5V *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| NX25P10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| NX25P16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| NX25P16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| NX25P20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| NX25P32 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| NX25P40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| NX25P80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## NS

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 24C02/02L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C04/04L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 25C040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 25C160 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 34C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 59C11 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93C06 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C26 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — National Semiconductor 93C46, DIP-8: Native C full 128-byte Program, margin Verify, Read/Compare; inverse nonblank rewrite in both bit directions; aligned word update with surrounding contents preserved; Erase and independent all-FF readback. Final erased. | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86A(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93CS06 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93CS26 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93CS46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93CS56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93CS66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| DM2732 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| DM54/74S188 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S287 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S288 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S387 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S471 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S472 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S473 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S474 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S475 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S570 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S571 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S572 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM54/74S573 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM77/87S180 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM77/87S181 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM77/87S184 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM77/87S185 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM77/87S195 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM77/87S280 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| DM77/87S281 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| GAL16V8 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL16V8-7/-10 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL16V8A/QS | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL20RA10 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL20V8 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL20V8-7/-10 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL20V8A/QS   | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL22CV10 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL22V10/B | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| NM25C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| NM27C010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NM27C020 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NM27C040 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NM27C128 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NM27C210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NM27C210 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NM27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NM27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NM27LC256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NM27LC512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NM27P040 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C020 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C1024 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C128B | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C128BN | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C128BQ | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C128C | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| NMC27C128CQ | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| NMC27C16B/BQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| NMC27C2048 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C2048 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C210 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C240 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C240 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| NMC27C256B | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C256BN | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C256C | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C256Q | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C32B/BQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| NMC27C512A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C512AN | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C64B | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C64BN | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| NMC27C64Q | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |

## Numonyx

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| M25P05A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P10A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P128 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P128 @8VDFPN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P32 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P64 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P64 @8VDFPN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P80 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE10 @8VDFPN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE20 @8VDFPN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE40 @8VDFPN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE80 @8VDFPN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## OKI

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 2716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| 28C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| MR27V1602D  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V1602D *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V1652D  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V1652D *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V1652D *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V3202D *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V3252D *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V401D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V402D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V402D *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V452CZ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V6452D *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V801D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V802D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MR27V802D *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM271024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM271024 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM271024AS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM271024AS *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM271024ZB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM271024ZB *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27128AS | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| MSM27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27256AS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27512AS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27512ZB-RS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM2764A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM2764AS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM2764AZB-RS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM2764RS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM2764Z | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C1000 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1000ZB | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1024 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1024AS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1024AS *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C128A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C128AS | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| MSM27C128AZB-RS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C1602CZ  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1602CZ *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1652CZ  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1652CZ *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C1655CZ @70 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C201CZ | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C2048 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C2048 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C256H | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C256HZB | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C256ZB | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C256ZB-RS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C3202CZ *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C3202CZ *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C3252CZ *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C401CZ | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C402CZ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C402CZ *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C452CZ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C64AS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| MSM27C802CZ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C802CZ *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Yes — OKI M27C802CZ, TSOP-44, straight DIP-44 adapter: Full 1 MiB read, blank precheck, program and independent readback; nominal Verify plus six mismatch positions; OTP left programmed. Over-program and margin Verify not physically tested. | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C822ZB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27C822ZB *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27V1655CZ @70 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| MSM27V3255CZ @70 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |

## ON

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46J | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46J(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C57 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C66(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| CAT24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C02 *TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C02AI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C02I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C03 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C03 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C03 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C03 *TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C04 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C04 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C04 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C04 @TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C04I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C05 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C05 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C05 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C05 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C05 @TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C08 *5SOT | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C08 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C08 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C08 *TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C08I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C16 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C16 *TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C16I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24C208 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, read | Not recorded | Dataman 13.10: Protected I2C Page and dual-port configuration audit; hardware testing pending |
| CAT24C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C32 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: X24C44 / X2444 and CAT24C44 serial NVRAM audit |
| CAT24C44 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: X24C44 / X2444 and CAT24C44 serial NVRAM audit |
| CAT24C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24C64 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24FC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24LC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC02AI | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC02I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC04I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC08I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24LC16I | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| CAT24WC32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24WC32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24WC64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT24WC64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| CAT25010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25010 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25010 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25010 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25020 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25020 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25020 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25020 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25040 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25040 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25040 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25080 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25080 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25080 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25128 *8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25128 *8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25128 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25128 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25160 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25160 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25160 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25160 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25256 *8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25256 *8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25256 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25320 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25320 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25320 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25320 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25512 *8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25512 *8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25512 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25512 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25640 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25640 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25640 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT25640 @8TDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| CAT34TS04 @8UDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |

## PCT

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 25VF010A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF010A @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF016B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF016B @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF020B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF020B @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF032B *16SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF032B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF040B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF040B @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF064C *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| 25VF064C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| 25VF064C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| 25VF080B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF080B @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF512A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF512A @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| PCT26VF016 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| PCT26VF016 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| PCT26VF032 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| PCT26VF032 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Philips

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 27C010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C210 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C240 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C240 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| 27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 27C64A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 82LS135 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82LS180 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82LS181 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S114 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S115 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S123 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S126 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S129 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S130 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S131 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S135 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S137 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S137A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S137B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S140 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S141 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S147 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S147A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S180 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S181 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S181A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S181B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S185 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S185A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S185B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S191 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S191A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S191B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| 82S23 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Bipolar PROM source audits; see individual hardware evidence |
| PCB2421 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| PCF85116 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| PCF8582 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| PCF8594 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| PCF8598 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |

## PMC (Chingis)

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| Pm25LD010 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD010 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD010C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD010C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD020 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD020 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD020C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD020C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD040 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD040 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD040 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD040C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD040C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD040C*SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD040C*SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD256C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD256C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD256C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD512 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD512C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD512C *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LD512C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LV010 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LV016 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LV020 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LV040 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LV080 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25LV512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25W020 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| Pm25W020 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## RAMTRON

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| FM1808 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Parallel FRAM source audit |
| FM18L08 *28SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Parallel FRAM source audit |

## Renesas

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| HN58X25128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| HN58X25256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX24002 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24002 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24004 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24004 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24008 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24008 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24016 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24016 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24032 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24032 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24064 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24064 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24128 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24128 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24256 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24256 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX24512 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| R1EX25002 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25002 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25004 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25004 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25008 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25008 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25016 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25016 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25032 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25032 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25064 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25064 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| R1EX25512 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |

## ROHM

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| BR24A01AF-WLB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24A02F-WLB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24A04F-WLB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24A08F-WLB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24A16F-WLB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24A32F-WLB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24A64F-WLB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G01-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G01-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G01F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G01F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G01FVJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G01FVJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G01FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G01FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G02-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G02-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G02F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G02F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G02FVJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G02FVJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G02FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G02FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G04-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G04-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G04F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G04F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G04FVJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G04FVJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G04FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G04FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G08-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G08-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G08F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G08F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G08FVJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G08FVJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G08FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G08FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G128-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G128-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G128F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G128F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G128FVJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G128FVJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G128FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G128FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G16-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G16-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G16F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G16F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G16FVJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G16FVJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G16FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G16FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G256-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G256-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G256F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G256F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G256FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G256FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G32-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G32-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G32F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G32F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G32FVJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G32FVJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G32FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G32FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G512-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G512-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G512F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G512F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G512FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G512FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G64-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G64-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G64F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G64F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G64FVJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G64FVJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G64FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24G64FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L01A-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L01AF-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L01AFVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L01AFVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L02-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L02F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L02FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L02FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L04-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L04F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L04FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L04FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L08-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L08F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L08FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L08FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L16-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L16F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L16FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L16FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L32-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L32F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L32FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L32FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L64-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L64F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L64FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24L64FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S08-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S08F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S08FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S08FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S128-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S128F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S128FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S128FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S16-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S16F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S16FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S16FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S256-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S256F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S256FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S32-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S32F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S32FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S32FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S64-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S64F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S64FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24S64FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T01-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T01F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T01FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T01FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T02-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T02F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T02FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T02FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T04-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T04F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T04FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T04FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T08-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T08F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T08FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T08FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T128-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T128F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T128FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T128FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T16-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T16F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T16FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T16FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T256-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T256F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T256FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T32-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T32F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T32FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T32FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T512F-3AM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T512FVT-3AM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T64-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T64F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T64FVJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR24T64FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| BR25A1MF-3M | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25A1MFJ-3M | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25A256F-3M | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25A256FJ-3M | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25A256FVT-3M | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25A512F-3M | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25A512FJ-3M | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25A512FVT-3M | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G128F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G128FJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G128FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G1MF-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G1MFJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G256F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G256FJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G256FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G320F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G320FJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G320FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G512F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G512FJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G512FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G640F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G640FJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25G640FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H010F-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H010FJ-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H010FVT-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H020F-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H020FJ-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H020FVT-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H040F-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H040FJ-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H040FVT-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H080F-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H080FJ-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H080FVT-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H128F-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H128FJ-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H128FVT-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H160F-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H160FJ-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H160FVT-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H256F-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H256FJ-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H256FVT-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H320F-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H320FJ-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H320FVT-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H640F-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H640F-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H640FJ-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H640FJ-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H640FVT-2AC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25H640FVT-2C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25L080 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25L080 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25L160 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25L160 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25L320 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25L640 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S128F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S128FJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S128FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S256F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S256FJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S256FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S320F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S320FJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S320FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S640F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S640FJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR25S640FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| BR29F200T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| BR9016 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| BR9016F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| BR9020 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| BR9020F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| BR9040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| BR9040F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| BR9080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| BR9080F *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| BR93A46F-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A46FJ-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A56F-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A56FJ-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A57 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A66F-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A66FJ-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A76F-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A76FJ-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A86F-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93A86FJ-WM | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46F-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G46FVT-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56F-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G56FVT-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66F-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66FVT-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66FVT-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G66FVT-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76F-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76FJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76FJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G76FJ-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86F-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86F-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86F-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86FJ-3 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86FJ-3A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93G86FJ-3B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H56RF-2LB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H56RF-WC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H56RFJ-WC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H66RF-2LB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H66RF-WC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H66RFJ-WC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H76RF-2LB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H76RF-WC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H76RFJ-WC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H86RF-2LB | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H86RF-WC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93H86RFJ-WC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L46-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L46F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L46FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L56-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L56F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L56FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L66-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L66F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L66FVT-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L76-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L76F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L76FJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L86-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L86F-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93L86FJ-W | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93LC46F/FV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| BR93LL46F/FV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |

## Samsung

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 24L16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93C46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| KM2817A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| KM28C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| KM28C16 *32PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| KM28C17 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| KM28C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| KM29C010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| S524C20D11 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S524C20D21 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S524C80D41 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S524C80D81 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S524L50X51 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S524LB0X91 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S524LB0XB1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |

## SANYO

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| LE25FV201T *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| LE25FV401T *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| LE25FV451T *32TS/W | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| LE25FW406M *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| LE25FW806M *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## SEEQ

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 27128 | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| 27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| 2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| 27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| DQ2804A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| DQ2816A/5516A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| DQ2817A/5517A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| DQ2864 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| DQ2864H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| DQ28C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| DQ28C256A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| DQ28C64/A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| DQ28C65 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| DQ28HC256/H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| DQ28HC64/H | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |

## Seiko

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| S-24CS01A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS01A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS02A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS02A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS04A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS04A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS08A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS08A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS16A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-24CS16A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| S-25C080A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| S-25C080A *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| S-29190A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S-29190A(SO) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S-29290A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S-29290A(SO) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S-29390A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S-29390A(SO) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| S-93C46B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S-93C46B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S-93C56B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S-93C56B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S-93C66B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S-93C66B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S-93C76A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S-93C76A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S-93C86B *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| S24CS64A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |

## Simtek

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| STK10C68-C45 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — Simtek STK10C68-C45 ceramic/gold DIP28: Hardware SRAM, STORE/RECALL and persistence qualified; native C/GTK integration and seven-operation qualification recorded in report | Dataman 13.10: Native extension: datasheet + hardware qualification |

## SONY

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| CXK27C1000 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| CXK27C1001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| CXK27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| CXK27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |

## SPANSION

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| S25FL001D-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL002D-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL002D-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL004A-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL004A-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL004D-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL004D-NF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL008A-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL008A-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL016A-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL016A-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL016A-NF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL032A-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL032P-BH-02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL032P-BH-03 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL032P-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL032P-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL032P-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-MF-02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-NF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-NF-02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-VF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-VF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL040A-VF-02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL064A-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL064P-BH-02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL064P-BH-03 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL064P-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL064P-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL127S-BH-C0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL127S-BH-D0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL127S-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL127S-MF-10 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL127S-NF-10 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128P-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128P-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128P-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128P-NF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-21 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-30 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-31 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-A0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-A1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-B0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-B1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-C0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-C1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-D0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-D1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-Z0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-BH-Z1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-MF-G0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-MF-G1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-MF-R0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-MF-R1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL128S-NF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL129P-BF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL129P-BF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL129P-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL129P-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL129P-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL129P-NF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL204K-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL204K-MF-04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL208K-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL208K-MF-04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL216K-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL216K-MF-04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-21 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-30 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-31 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-A0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-A1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-B0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-B1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-C0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-C1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-D0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-D1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-K0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-K1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-L0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-L1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-M0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-M1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-N0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-N1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-S0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-S1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-T0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-T1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-U0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-U1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-V0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-V1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-W0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-W1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-X0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-X1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-Y0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-Y1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-Z0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-BH-Z1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-MF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-MF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-MF-G0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-MF-G1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-MF-R0 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-MF-R1 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-NF-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FL256S-NF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S25FLK05-MA-00 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S29AL004D-BF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| S29AL004D-BF-02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| S29AL004D-TF-01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| S29AL004D-TF-02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| S99-50346 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S99-50350 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| S99-50367 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## SST

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 25LF020A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25LF040A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25LF080A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF010 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF010A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF010A @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF016B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF016B *8SO(NEW) | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF016B @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF020 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF020B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF020B @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF032B *16SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF032B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF040 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF040B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF040B @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF080 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF080B *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF080B @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF512 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF512A *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| 25VF512A @8WSON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| SST25PF040C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| SST25PF040C @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| SST25PF040C @8WDFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| SST25VF064C *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| SST25VF064C *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| SST25VF064C @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| SST26VF016 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| SST26VF032 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| SST45VF010 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| SST45VF020 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| SST45VF512 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |

## ST

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 24C02A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C04 @8MLP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 2716 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| 2732 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| 35080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, secure, configuration | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| 93C46A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C46A(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93C86(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93CS46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93CS47 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93CS56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93CS57 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93CS66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93CS67 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 95P08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| GAL16V8 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL16V8/AS/S | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL20V8 | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| GAL20V8/AS/S | Dataman 13.10 | read, blank-check, program, verify, erase, secure | Not recorded | Dataman 13.10: GAL complete operation source audits |
| M24128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| M24256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| M24256-BR *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24256-BW *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24512-R *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24512-R *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24512-R *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24512-W *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24512-W *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24512-W *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24C32 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| M24C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — STMicroelectronics 24C64 SO-8 on pin-for-pin adapter (suffix not supplied): Native C Read, Blank Check, Program, Over-program, Compare, 4.75/5.25 V Verify, FF Erase; five partial-range boundary checks, full-array preservation and fresh-power readback; final 8192 FF | Dataman 13.10: Compact serial EEPROM source audit |
| M24M01 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24M01 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M24M02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| M25P05 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P05A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P10A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P128 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P128 @8VDFPN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P16 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P32 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P64 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P64 @8VDFPN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P80 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25P80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M25PE80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M27128A | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Yes — ST M27128AFI: Full read/program/readback; blank check; nominal and margin verify, including mismatch cases | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| M27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M27512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M2764A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| M27C1000 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C1001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C1024 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C1024 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C160 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C160 *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C2001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C201 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C202 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C202 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C256B | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M27C320 *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C320 *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C322 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C4001 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C4002 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C4002 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C405 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M27C516 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M27C64A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| M27C800 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C800 *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27C801 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V101 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V160 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V160 *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V201 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V322 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V401 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V402 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V402 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V800 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27V800 *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27W101 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27W101 *32PLCC | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27W101 *32TS | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27W201 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27W402 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M27W402 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| M28C64C | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — ST M28C64C-15P6 DIP28: Seven native C/GTK memory operations; initially blank backup, full pattern/inverse, 32-byte page and 0x1000 boundary preservation, negative Verify/Compare/Blank checks, independent reads; final 8192 FF verified at 4.75/5.25 V. No software Secure operation applies. | Dataman 13.10: Parallel page EEPROM source audits |
| M34C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| M34E04 @8MLP  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| M45PE10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M45PE80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| M48T08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: RTC-aware native workflow audit; hardware testing pending |
| M48T08 *28SOH | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, configuration | Not recorded | Dataman 13.10: RTC-aware native workflow audit; hardware testing pending |
| M87C257 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M87C257 *32 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| M95010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95020 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95160 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95320 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95512-R *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| M95512-R *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| M95512-W *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| M95512-W *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| M95640 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| M95M01-R *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| M95M01-R *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| M95M02-D *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI EEPROM/FRAM Page source audit |
| ST24E64D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| ST24FC21 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| ST24FW21 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| ST24LC21B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| ST24LW21 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| ST25E64D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| ST27128A | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| ST27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| ST2764A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| ST27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| STTS2002 @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| TS27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TS27C64/A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |

## TI

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| PAL16L8A/A-2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R4A/A-2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R6A/A-2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| PAL16R8A/A-2 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| SMJ27C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| SMJ27C512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TIBPAL16L8-12 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16L8-15 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16L8-25 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16L8-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16L8-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R4-12 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R4-15 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R4-25 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R4-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R4-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R6-12 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R6-15 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R6-25 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R6-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R6-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R8-12 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R8-15 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R8-25 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R8-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL16R8-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20L8-10 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20L8-12 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20L8-15 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20L8-25 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20L8-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20L8-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R4-10 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R4-12 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R4-15 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R4-25 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R4-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R4-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R6-10 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R6-12 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R6-15 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R6-25 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R6-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R6-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R8-10 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R8-12 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R8-15 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R8-25 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R8-5 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TIBPAL20R8-7 | Dataman 13.10 | read, blank-check, program, verify, secure | Not recorded | Dataman 13.10: OTP PAL complete operation source audit |
| TMS27128 | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| TMS2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| TMS27C010A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C020 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMS27C210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C210 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C210A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C210A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C240 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C240 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMS27C512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMS27C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMS27P64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| TMS27PC010 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27PC010A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27PC040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27PC128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMS27PC210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27PC210 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TMS27PC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMS27PC512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMS27PC64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMS29F800B *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| TMS29F800B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| TMS29F800T *44PS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| TMS29F800T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| TMS29LF800B *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| TMS29LF800T *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |

## TMC

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 24A02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24A08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 24A16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| 93LC46 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC46(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC56(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC57 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC66(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC86 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| 93LC86(x8) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## Toshiba

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| TC24512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TC541000AF | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC541000J | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC541000P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC541001J | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC541001P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC54256AF | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TC54256AP | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TC544000P | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC54512AP | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TC54H1024P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC571000AD | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC571000D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC571001AD | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC571001D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC571024D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC571024D *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC5716200D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC57256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TC57256D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TC574000D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC574000DI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC574096D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC574200D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC57512AD | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TC578200D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC57H1024AD | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC57H1024AD *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC57H1024D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC57H1024D *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| TC57H256D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TC89121 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| TC89121 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| TC89122 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| TC89122 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| TMM24128A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM24256A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM24256B | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM24512A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM2464A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27128 | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| TMM27128A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27128AD | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27128ADI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27128D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27128DI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27256A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27256AD | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27256ADI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27256BDI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27256D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27256DI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM2732 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 2716/2732 source audit; HW testing pending |
| TMM27512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27512AD | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27512ADI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27512D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM27512DI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM2764 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM2764A | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| TMM2764AD | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM2764ADI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| TMM2764D | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| TMM2764DI | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |

## TSI

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| TS25L010A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L020A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L020A SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L020A SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L16AP SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L16AP SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L16BP *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L16P *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L16PE *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L512A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L80P *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25L80PE *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| TS25PE80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## UTRON

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| UT23C8001 | Dataman 13.10 | read, verify, compare | Not recorded | Dataman 13.10: Mask ROM read/verify/compare source audit |

## VLSI

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| VT27C256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| VT27C512 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| VT27C64 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |

## Waferscale

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| WS27C010L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| WS27C040L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| WS27C128F | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| WS27C128L | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| WS27C210L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| WS27C210L *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| WS27C256F | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| WS27C256L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| WS27C512F | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| WS27C512L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| WS27C64F | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| WS27C64L | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |
| WS57C010F | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| WS57C128F | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| WS57C128FB | AEC 7.60<br>Dataman 13.10 | EPROM (six operations) | Not recorded | AEC 7.60: Reference engine<br>Dataman 13.10: Reference engine |
| WS57C256F | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| WS57C256FB | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| WS57C512F | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| WS57C64F | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: Exact group equivalence |

## Winbond

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| W15B020 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W15B040 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W19B800AB *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| W19B800AT *48TS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: XP parallel flash array and sector protection audit |
| W25B20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25B40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25D10 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25D10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25D20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25D20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25D80 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25D80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25P10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25P16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25P16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25P20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25P32 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25P40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25P80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q16BV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q16BV *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q16BV @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q16BV SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q16BV SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q20BV @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q20BV SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q20BV SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q32 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q64 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q64 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q64BV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q64BV *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q64BV *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25Q64BV @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X05CL *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X05CL *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X05CL @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X05CL @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10BL *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10BL @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10BV *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10BV @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10CL *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10CL *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10CL @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X10CL @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X16 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X16 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X16A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X16A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X16BV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X16BV *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X16BV SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20BL *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20BL @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20BV *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20BV @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20CL *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20CL @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X20CL @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X21CL *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X21CL @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X21CL @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X32 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X32 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X32A *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X32A *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40A @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40A SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40A SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BL @8USON(2x3) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BL @8WSON(5x6) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BL SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BL SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BV | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BV @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BV SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40BV SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40CL *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40CL @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X40CL @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X41CL *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X64 *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X64 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X64BV *16SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X80 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X80 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X80A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X80A SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X80A SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X80BL @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X80BL SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W25X80BL SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| W27C512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: W27 electrical erase and Rapid source audit |
| W27E512/P | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: W27 electrical erase and Rapid source audit |
| W27F010 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 1–64 Mbit EPROM source audit; see per-device HW scope |
| W27F256 | Dataman 13.10 | EPROM (six operations) | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| W27F512 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: 27C64–27512 source audit; HW testing pending |
| W45B012 *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| W45B012 *8SON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| W45B512 *32PLCC | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| W45B512 *8SON | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| W45D041 *28SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |
| W45D041 *28TS | Dataman 13.10 | read, blank-check, program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: GAAF serial original applet and workflow audit |

## Xicor

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| X20C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Yes — Xicor X2004PI (compatibility selection; X20C04 itself not physically tested): Native C full Read, Blank Check, Program, Over-program, Compare, 4.75/5.25 V Verify, STORE/RECALL and fresh-power persistence; partial-range preservation; final EEPROM-backed FF | Dataman 13.10: X20C04 / X2004 and X221x NVRAM source audit |
| X2210 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: X20C04 / X2004 and X221x NVRAM source audit |
| X2212 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: X20C04 / X2004 and X221x NVRAM source audit |
| X22C10 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: X20C04 / X2004 and X221x NVRAM source audit |
| X22C12 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: X20C04 / X2004 and X221x NVRAM source audit |
| X24001 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24012 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24022 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24164 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24165 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24325 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24641 (5V) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — Xicor X24641 SO8, markings X24641 / E740; D1AC / 72796C: Native 5 V extension, seven C/GTK memory operations; full patterns/inverse, 32-byte page and 0x1000/0x1800 boundary preservation, negative Verify/Compare/Blank checks, fresh-power readback; final 8192 FF. WP held low; hardware write-inhibit behavior not separately qualified. | Dataman 13.10: Native X24641 5 V extension: datasheet + audited I2C procedure |
| X24645 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24C01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24C01A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24C04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — Xicor X24C04 SO8 in straight 1:1 adapter: Seven native C/GTK memory operations; stable original backup, pattern/inverse, partial 16-byte-page and 256-byte-bank crossing, nonzero-offset read, negative Verify/Compare/Blank checks, fresh-power readback; final 512 FF verified at 4.75/5.25 V. Hardware WP enforcement not separately tested. | Dataman 13.10: Compact serial EEPROM source audit |
| X24C08 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24C16 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24C44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Yes — Xicor X2444PI (compatibility selection; X24C44 itself not physically tested): Native C Read, Blank Check, Program, Over-program, Compare, 4.75/5.25 V Verify, explicit RECALL/STORE, fresh-power retention and partial-range preservation; final EEPROM-backed FFFF | Dataman 13.10: X24C44 / X2444 and CAT24C44 serial NVRAM audit |
| X24F008 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24F016 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24F032 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X24F064 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X25040 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25041 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25043 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25045 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25057 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25080 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25097 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25138 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25160 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25320 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25401 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact serial EEPROM source audit |
| X25640 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25642 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25650 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25C02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Yes — Xicor X25C02, SO-8 in a straight adapter: Native C/GTK Read, Blank, Program, Over-program, Verify, Compare and Erase; full pattern/inverse and preserved partial writes, 4.75/5.25 V and fresh-power verification, negative mismatch checks; final 256 bytes FF | Dataman 13.10: Compact SPI EEPROM source audit |
| X25F008 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25F016 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25F032 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25F064 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X25F128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| X2804A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| X2816A/B | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Compact parallel EEPROM source audit |
| X2864A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| X28C256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |
| X28C64 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — Xicor X28C64PI-15 DIP-28: Native C Read, Blank Check, Program, Over-program, Compare, 4.75/5.25 V Verify, page-crossing partial preservation and FF Erase; protected-program sequence and retention exercised, blocked-write enforcement not independently tested | Dataman 13.10: Parallel page EEPROM source audits |
| X28HC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Yes — Xicor X28HC256PI-15 DIP28: Eight native C/GTK operations; stable original backup, full 32 KiB pattern/inverse, A14/128-byte-page boundary preservation, negative Verify/Compare/Blank checks, protected programming and subsequent erase; final 32768 FF. Write-inhibit enforcement not separately tested. | Dataman 13.10: Parallel page EEPROM source audits |
| X28VC256 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: Parallel page EEPROM source audits |

## Xilinx

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| 1701 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1701 *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1701 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1701L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1701L *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1701L *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1702L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1702L *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1702LVC *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1704L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1704L *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1704LVC *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17128 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17128D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17128E | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17128E *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17128L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17128X/EL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17128X/EL *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1718D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1718L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17256D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17256E | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17256E *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17256L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17256X/EL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17256X/EL *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1736A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1736D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1736E | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17512L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17512L *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17512L *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1765 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1765D | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1765E | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1765E *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1765L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1765X/EL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 1765X/EL *20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S05 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S05L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S05XL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S10 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S100XL  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S100XL *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S10L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S10XL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S150XL  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S150XL *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S20 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S20L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S20XL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S30 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S30L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S30XL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S40 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S40 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S40L | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S40XL | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S40XL *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S50XL  | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| 17S50XL *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |
| XC17S100A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S100A *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S100A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S100A *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S150A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S150A *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S150A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S150A *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S15A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S15A *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S15A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S15A *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S200A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S200A *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S200A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S200A *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S300A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S300A *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S300A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S300A *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S30A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S30A *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S30A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S30A *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S50A | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S50A *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S50A *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17S50A *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V01 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V01 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V01 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V01 *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V02 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V02 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V02 *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V04 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V04 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V04 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V04 *VQ44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V08 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V08 *44VQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V16 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC17V16 *44VQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: XC17 original RAM applets, OTP array and configuration audit |
| XC18V01 *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V01 *20PLCC(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V01 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V01 *20SO(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V01 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V01 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V01 *44VQ(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V02 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V02 *44(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V02 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V02 *44VQ(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V04 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V04 *44(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V04 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V04 *44VQ(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V256 *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V256 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V256 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V256 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V512 *20PLCC | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V512 *20PLCC(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V512 *20SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V512 *20SO(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V512 *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V512 *44TQ | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XC18V512 *44VQ(NEW) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XCF01S *20TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XCF02S *20TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XCF04S *20TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Remaining configuration memory audit |
| XQ1701LCC *44 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, configuration | Not recorded | Dataman 13.10: Earlier serial configuration PROM source audit |

## XTX

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| XT25F01B 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F01B 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F01B @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F01D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F01D 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F01D 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F01D @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F02B 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F02B 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F02B @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F02D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F02D 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F02D 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F02D @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F04B 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F04B 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25F04B @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W01B 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W01B 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W01B @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W01D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W01D 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W01D 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W01D @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W02B 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W02B 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W02B @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W02D *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W02D 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W02D 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W02D @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W04B *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W04B 8SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W04B 8SO(208mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| XT25W04B @8DFN | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## YMC

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| Y24LC02 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| Y24LC02 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| Y24LC02 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: I2C Page source audit (small and larger groups) |
| Y25F05 | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure, configuration | Not recorded | Dataman 13.10: Compact SPI EEPROM source audit |
| Y54LC64 *8SO | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| Y54LC64 *8TSSOP | Dataman 13.10 | read, blank-check, program, verify, compare, erase | Not recorded | Dataman 13.10: GDAC serial original applet and workflow audit |
| Y93LC46 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| Y93LC46 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| Y93LC66 *8SO | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |
| Y93LC66 *8TSSOP | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase | Not recorded | Dataman 13.10: Microwire 93C source audit |

## Zetta

| Device | Database | Operations | Tested in HW | Original-software audit |
| --- | --- | --- | --- | --- |
| ZD25D16 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D16 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D16 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D20 *8TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D20 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D20 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D20 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D40 *8TSS | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D40 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D40 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D40 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D80 *SO(150mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D80 *SO(200mil) | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D80 @8USON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |
| ZD25D80 @8WSON | Dataman 13.10 | read, blank-check, program, over-program, verify, compare, erase, secure | Not recorded | Dataman 13.10: SPI NOR full operation and status/protection audit |

## Hardware-tested work awaiting native integration

**Not included in the supported-device count.** These tests remain useful evidence, but complete C/GUI workflows are not available.

Exact Dataman PALCE D0/D2 definitions now have all six applicable operations; physical evidence and source-only Secure evidence are distinguished above. See PAL/PALCE/GAL implementation status for the completed work and remaining gaps.

| Manufacturer | Device/sample | Hardware-test scope |
| --- | --- | --- |

## Partial operation support

Excluded from the supported-device count:


## Updating this list

After changing native device admission, rebuild and regenerate from the repository root:

```sh
cmake --build build-c -j4
python3 tools/generate_supported_devices.py
```

Maintain direct test evidence and its scope in `docs/data/device_evidence.json`; never infer a physical test from an engine-level support label. New audit levels require an explicit description in the generator. Both configured source installations must be present; a missing database stops generation rather than silently removing its devices.

The companion machine-readable list records source catalogue and native binary/library hashes, per-version operations and evidence. This generator uses offline CLI queries only and performs no programmer operations.
