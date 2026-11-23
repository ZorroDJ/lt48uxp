# Macronix 256-Mbit SPI flash

29 exact Dataman 13.10 identities, seven definitions, three configuration layouts
and three source package pin maps. Portable C AddTab SPI owner and normal GTK.
Image: 32 MiB main array followed by 512 bytes of secured memory. Program requires
256-byte pages; raw Read allows arbitrary ranges. Secure changes main-array BP
protection and preserves represented settings, factory/OTP locks and WPSEL.
Existing WPSEL advanced-sector mode rejects Secure: BP mode is disabled there.
No new OTP or WPSEL locks are offered. Ordinary Program/Erase retain the original
source unprotect behavior, including global unlock when WPSEL already is set.

The source erase geometry includes two trailing addresses beyond the array.
The original erase procedure stays in normal array mode; it does not enter
secured-region mode. Its geometry/commands are retained and secured data is
independently checked for preservation. A nonblank secured region remains
nonblank after main-array Erase. Locked secured memory cannot be programmed.

Vendor REZ, FPGA images and controller applets remain bring-your-own.
Physical hardware/adapter/analogue testing is pending.
Audit: docs/audit-qualification/SPI_MAC29_AUDIT.md.

An exact authored correction for W24f/config25435 changes the CR.TB selector
from checkbox6 (Factory Lock) to checkbox5 (TB). Original Windows execution
with opposing field values demonstrates the defect. The other two configuration
variants retain their original callbacks. Read/configuration preservation and
all four factory/user-lock combinations are checked independently.
