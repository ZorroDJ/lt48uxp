# Importing your own device database

The Linux application reads the original binary `.rez` resources itself.
There is no JSON conversion step and no Windows software is executed by the
importer. No programmer connection is needed to import or verify a database.

## 1. Prepare your own files

Use an extracted or installed copy of the original software. This test release's
main supported database is **Dataman 13.10**. Obtain those files separately;
they are not part of this download. The GUI does not unpack Windows setup EXEs,
CAB archives or ZIP archives.

Keep the installation together in a local directory outside this distribution,
for example `~/LabTool-resources/Dataman-13.10/`. That directory must contain:

- `dev48uxp.rez`, the device catalogue.
- All matching `.rez` definition files from that same installation.
- The recognized `WDATA48UXP.exe` or `WDATA48UXP.EXE` from Dataman 13.10.
  Supported AEC imports use their recognized `WLT48UXP.exe` instead (AEC 13.10,
  14.30 and audited 14.40 variants). AEC 14.40 resources whose digests match an
  audited 13.10 engine resource are mapped to that exact engine.

To display the reader's live monitor version and U62 serial, the native GUI
optionally uses the original 600-byte `7e1df8` read-only applet. It is not
bundled with this source release. `build-lt48uxp identity` extracts the applet
from the recognized owned `WLT48UXP.exe`, verifies its SHA-256, and places it
under `~/.config/labtool48uxp/identity-applet.bin`. Alternatively set
`LABTOOL48_IDENTITY_APPLET` to the same verified applet. Without it the live
monitor version is still shown, while the serial is marked unavailable.

For reference, the supported Dataman executable's SHA-256 is:

`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`

The importer verifies actual contents. Renaming an unrelated executable will
not make it compatible. An already verified `idle-fpga.bin` can provide the
runtime object instead of the executable. Never mix individual `.rez` files
from different releases.

## 2. Select the installation in the GUI

1. Start `./start-lt48uxp`.
2. Open **Select**, then **Manage…** to open **Manage Device Databases**.
3. Choose **Add Directory…** and select the folder prepared above.
4. Select its row and click **Use Selected**. This makes it the active source.
5. Reopen **Manage…** and choose **Create private cache…**.
6. Choose a new destination name, for example
   `~/LabTool-resources/dataman-13.10-cache`. The destination must not exist.
7. Wait for the success message. The new cache is registered and selected.
   Import failures leave an existing destination untouched.
8. Close the manager, search for a part and choose the exact manufacturer,
   part and package. Only devices with implemented operations are operational;
   importing all catalogue names does not implement every device.

The importer audits the database, extracts the verified idle-FPGA object and
copies the required resources into a content-addressed private cache. It does
not copy the Windows executable. The cache contains original vendor data and
must remain outside the distribution repository too.

## 3. Command-line alternative

Run these commands from the distribution directory. Substitute your own paths:

```bash
./lt48-cli cache import "$HOME/LabTool-resources/Dataman-13.10" \
  "$HOME/LabTool-resources/dataman-13.10-cache"
./lt48-cli cache verify "$HOME/LabTool-resources/dataman-13.10-cache"
```

If the definitions and runtime executable are in different directories:

```bash
./lt48-cli cache import /path/to/definitions /path/to/new-cache \
  --runtime-source /path/to/runtime-installation
```

Then add the resulting cache directory in the GUI and choose **Use Selected**.
The GUI uses the active source directory for both definitions and runtime;
separate runtime-directory selection is available through the CLI.

To select a database at startup:

```bash
LABTOOL48_INSTALLATION="$HOME/LabTool-resources/dataman-13.10-cache" \
  ./start-lt48uxp
```

## 4. USB firmware is a separate input

The private database cache includes the idle-FPGA payload, but does not include
USB-bridge firmware. Supply your own supported FX2 firmware file separately.
Select that file in the programmer setup controls, or use:

```bash
LABTOOL48_FX2_FIRMWARE=/path/to/verified-fx2.hex ./start-lt48uxp
```

The application offers offline preparation followed by a separately confirmed
volatile firmware load. This distribution does not include a Windows-installer
or USB-firmware extraction utility. If you only have an installer and cannot
obtain the required files, report that setup limitation before hardware testing.

## Troubleshooting

- **Runtime not found:** put the matching executable beside the `.rez` files,
  or use the CLI's `--runtime-source` option.
- **Digest/validation failure:** confirm the exact source version and a complete,
  unmodified set of files. An arbitrary newer release is not interchangeable.
- **Destination exists:** select a new cache destination; imports do not replace
  an existing directory.
- **Device found but operations unavailable:** the database lists more devices
  than this Linux build implements. Consult `SUPPORTED_DEVICES.md`.
- **Database works, programmer does not:** database import does not establish
  USB permissions, bridge firmware readiness, socket power or adapter wiring.

Keep resources and caches local. Do not attach `.rez` files, firmware, Windows
executables or private-cache objects to GitHub issues or releases. For import
problems, send the error message, source version and file checksums instead.
