# S08 BDM operation owner

Portable C owner for22 exact Dataman13.10 catalogue bindings. Original REZ,
8051 applets and FPGA configurations are supplied by the operator; this
source directory contains none of their binary payloads. Admission is gated
by `admission.h` and the sealed qualification receipt.

Read, Blank Check, Program, Erase, Verify and Compare use a64KiB address
image. The selected definition sets the Flash start, GT60's1800..182B hole,
package routing and nominal supply500cV or320cV. Never infer an adapter from
its pin count. First10 are AW16/AW32/AW48, JM8 and GT8A/GT16(A)/GT32; remaining
12 are QD2/QD4, QG4/QG8 and GT60. Package aliases are separate bindings.

The class2014 host route and compiler/runtime are reused. The exact original
power/entry/reset, Read and Program compact waves are compiled strictly.
Original worker15 reads,16 programs,19 erases. Worker17 also contains common
BDM send/address/receive helpers: it must be downloaded even though Secure is
not exposed. Four distinct worker sets and five distinct FPGA configurations
are retained. No permanent programmer memory is modified by this owner.

Original worker startup is RESET-high GPIO byte, BKGD-high GPIO byte, then
start address little endian. Original helpers serialize commands MSB first:
C0 WRITE_BYTE, E0 READ_BYTE,16bit address and8bit data. They set volatile
FCDIV1820=93. Program launches FCMD1826=25 via FSTAT1825=C0; erase uses41,
then20 to restore trim/options. Each transmitted Program byte is echoed,
which is not proof of a successful physical Flash operation. Fresh raw reads
are authoritative. Every source Read/Program image address is audited
independently; Read regions never determine Program transfers.

## Deliberate Linux guards and normalization

- FOPT1821 is read before ordinary operations; only SEC1:0=10 is accepted.
  Source secured-Flash zero responses cannot be published as valid contents.
  Protected recovery/backdoor/Secure are outside this owner.
- Source option operation is **OR3E**. Default FF input is explicitly made
  unlocked FE; other requested options with SEC00=1 are rejected. No hidden
  secure action. Effective options are checked in fresh Read.
- Factory trim is read before mutation and checked afterwards. Program accepts FF trim placeholders or bytes matching the preflight Read;
  any requested calibration change is rejected before writing. Verify/Compare
  treat FF trim as preservation placeholders and default FF NVOPT as FE.
- Program requires blank ordinary Flash. Erase verifies fresh raw contents,
  preserving the source's calibration/options restoration.
- Voltage margins are5%/10% of the actual nominal supply.
- C consumes Stop exactly once. Original workers RET without a further
  completion write, so the source's unused final capture is omitted. Raw
  per-byte echoes are copied to scratch10 before the source masks them.
- Read output is published only after successful cleanup. Failed transport
  or uncertain worker state requires recovery; do not send a blind Stop.

Source changes do not imply hardware qualification. Digital target-ready/data
fixtures are not an analogue model. See
[qualification](../../../docs/audit-qualification/S08_22_PROGRESS.md).

Primary references: NXP
[AW](https://www.nxp.com/docs/en/data-sheet/MC9S08AW60.pdf),
[JM](https://www.nxp.com/docs/en/data-sheet/MC9S08JM16.pdf),
[QD](https://www.nxp.com/docs/en/data-sheet/MC9S08QD4.pdf),
[QG](https://www.nxp.com/docs/en/data-sheet/MC9S08QG8.pdf),
[GT/GB](https://www.nxp.com/docs/en/data-sheet/MC9S08GB60.pdf),
[GT A](https://www.nxp.com/docs/en/data-sheet/MC9S08GT16A.pdf).
