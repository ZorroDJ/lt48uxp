# HC705 JJ7/JP7, SR3 and PL4/B

Portable C owner with exact Dataman13.10 bindings; original resources are BYO.
Admission is controlled by admission.h and the staged qualification receipt.
Read, Blank Check, Program, Verify and Compare in the normal operator GUI.
Hardware/analogue qualification is pending. No programmer EEPROM writes.

## Software to hardware chain

Device selection resolves an exact GADB___ definition/family/pin map from the
recognized source. The C owner downloads the original Read/Program8051 workers
and selected XC3030A image. Original compact PowerOn/Init/Reset/Off procedures
compile with strict pin resolution and run through the common C runtime and USB
queue transport. The MCU workers use C800/D800 FIFO classes and F000 FPGA writes.
No stock Secure worker is loaded or exposed by this memory-operation owner.

JJ7/JP7 source compact code, variables and all three workers are identical;
package pin maps and FPGA images differ. Their first FIFO parameter is mapped
RESET-low70hex. The worker applies it, then70+1 to release reset. The source
uploads a target RAM routine through serial signaling before transferring data.
Source1MHz clocks reach ZIF31 (JJ7 DIP20) and35 (JP7 *28SO). Logical19 is a
fixed-high strap in the selected configuration, not arbitrary GPIO.

SR3 first parameter RESET-low14hex; source2MHz clocks at ZIF9/10. Its original
Read consumes3856 replies, Program8192 including FF through image holes. C
preserves each raw Program echo in scratch10 before the original FF-hole mask,
so a failed first-hole transfer cannot be concealed. Bad echo requires recovery
without blindly sending Stop after a worker may already have returned.

PL4/B uses A000/A0BD workers. Initialization sends four mapped GPIO bytes,
39/81/7D/79hex, corresponding to logical4/18/19/20 high, before memory transfer.
It advances through initial unused addresses using target handshake edges;
C4 control words switch the FPGA bus direction. Source2MHz clock is at ZIF38.
The same owner preserves its mode-specific rail/reset sequence and engine start.

## Image and operation policy

Whole8192byte address image, holesFF, EPROM erased00. Physical images:

- JJ/JP:0700..1EFF and1FF0..1FFF. Read consumes all0021..1FFF replies and
  masks the other bytes. Program skips security/COP1FF0; requires image00 there.
  COP security is write-only and is not compared as physical EPROM. MOR1FF1
  remains in the ordinary programming/readback image; all eight bits are valid.
- SR3:1000..1EFF and1FF0..1FFF. Source image0FFF configuration callbacks are
  host metadata, not a physical MOR Read. Ordinary operations requireFF there;
  configuration programming and Secure are unavailable.
- PL4/B:0E00..1DFF and1FF0..1FFF.

Program requires blank EPROM and checks fresh nominal/low/high raw Reads.
JJ/JP Program Stop returns the last data byte, not a fixed success status;
Read Stop returns01 and timeout returns99 after draining dummy replies.
SR3/PL4 normal Stop returns01; SR3 failed Program returns failed-byte+1 and
exits immediately. Echoes alone never establish write success.

C consumes source Stop/completion once, preserves source mode-specific reset
and ramp behavior, flushes final reset, turns rails off and restores idle.
Known cleanup selects Good/Error. An unknown transport/worker state requires
recovery; output is not published as a successful Read. Separate Secure and
electrical Erase are unavailable. UV erase is external where the package allows it.
Selected SO28/*42 package adapters are mandatory; no straight mapping is inferred.

Evidence: [qualification](../../../docs/audit-qualification/HC705_JJ_SR_PL_PROGRESS.md).
Original8051 peripheral fixtures and XC3030A digital tests do not claim a full
chip boot-ROM, analogue rails, contact or physical timing model.
