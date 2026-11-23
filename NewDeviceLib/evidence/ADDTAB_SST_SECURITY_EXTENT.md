# SST Security ID erase extent

The authored header binds only private tail definitions Df1/Df2 with their exact
main and imported erase extents. Native resource validation binds the original
REZ payloads and device IDs before using it.

The imported erase span includes 256 appended user Security ID bytes. This
metadata does not prove those bytes are electrically erasable. Microchip
DS20005020C, Security ID section and Table 7, states that Security ID is
non-erasable and that A5 programs it independently of ordinary A0 programming:
https://ww1.microchip.com/downloads/en/DeviceDoc/20005020C.pdf

The correction preserves the appended bytes, executes the original main chip
erase, verifies the main extent as FF and checks that Security ID stayed intact.
It does not change the source command bytes or enable permanent locking.
An all-FF blank check over a programmed Security ID still correctly fails.

This corrects the Linux interpretation of imported geometry. It does not by
itself establish a bug in Windows: its complete post-erase policy needs a
separate audit. Hardware qualification remains pending.

## Private Df0 / GLS36VF3203 extension

Greenliant S71270-05-000 (May 2010), page 5 and table 7 page 14,
likewise makes Security ID non-erasable. Table 7 specifies user x16 addresses
100008–100087, whereas the imported byte-address segment starts at 100010.
The uncorrected native/MCU/FPGA trial actually reads word 080008, outside that
window. A narrowly bound native correction changes only Df0's appended region
to byte address 200010 and preserves it across main-array erase. Wrong extents
and unexpected source addresses are rejected. Permanent locking stays disabled.

Sources:
- https://www.greenliant.com/doc/S71270.pdf
- https://www.alldatasheet.es/html-pdf/1282228/GREENLIANT/GLS36VF3203/9933/14/GLS36VF3203.html
- https://www.alldatasheet.es/html-pdf/1282228/GREENLIANT/GLS36VF3203/3543/5/GLS36VF3203.html

Diagnostic receipt:
`analysis/addtab-tail92-batch/word-pipeline-secsi-read-selected-exact23-datasheet/status.json`.
This is not a claim that the original Windows application has a confirmed bug;
its exact operation path still needs comparison. Df0 remains private pending
complete operation qualification.
