# LT48UXP — limited Linux test distribution

> **Download the binary package from [Releases](https://github.com/ashurbanipal-labs/lt48uxp/releases).**
> The automatically generated “Source code” archives contain documentation only.
> Extract the `ubuntu24.04-x86_64.tar.gz` asset, enter its `lt48uxp` directory,
> and follow the setup instructions below. Original databases and firmware are
> bring-your-own and are not included.

Build: **0.1.0-test.20261004.1**. Target: **Ubuntu 24.04, x86-64**.
Independent native C/GTK application for the LabTool-48UXP programmer.
This directory contains application binaries, launchers and tester documentation.
Source code, reverse-engineering material, original Windows software, device
resource files and USB firmware are not included.

## Install dependencies and start

```bash
sudo apt update
sudo apt install libgtk-4-1 libusb-1.0-0
./start-lt48uxp
```

Run the launcher from the extracted directory, or use its absolute path. Keep
`bin` and `lib` together. No Python, compiler, Ghidra or Windows VM is needed.
The launchers locate the bundled library relative to themselves. Do not run the
GUI with sudo. This build is not claimed compatible with Ubuntu 22.04 or other
architectures.

CLI version check:

```bash
./lt48-cli version --json
```

The executable reports `0.1.0`; use the full distribution build identifier above
when reporting results. BUILD-INFO.json identifies the input binaries. Release
copies have debug sections and development-directory runtime paths removed.

## USB permissions

From this directory:

```bash
sudo install -m 0644 system/70-labtool48.rules /etc/udev/rules.d/70-labtool48.rules
sudo udevadm control --reload-rules
```

Reconnect the programmer's USB cable afterwards. The rule grants access to the
active local desktop user; it does not make the USB device world-writable.

## Device database and firmware setup

The application reads original `.rez` files directly. JSON conversion is not
needed. Use **Manage Device Databases** to select a complete matching database
installation or a previously verified native private-cache directory. Dataman
13.10 provides the main supported set in this build. A catalogue entry alone
does not mean its operations are implemented.

A private cache contains the original resource data under SHA-256 object names,
logical resource links, a manifest and the verified idle-FPGA payload. When
transferring one, copy the whole directory and preserve its internal links.
Do not mix individual resource files from different software versions.

To import an extracted installation using the bundled C CLI:

```bash
./lt48-cli cache import /path/to/definitions /path/to/new-private-cache \
  --runtime-source /path/to/original-installation
./lt48-cli cache verify /path/to/new-private-cache
```

The runtime source must contain an exactly recognized `WLT48UXP.exe`,
`WDATA48UXP.exe` (or uppercase `.EXE`), or verified `idle-fpga.bin`.
The destination must not already exist. An arbitrary software version is not
accepted merely because the filename matches. This utility imports an extracted
installation; it does not unpack a Windows installer.

The GUI recognizes the supported AEC and Dataman runtime names through the
same C importer and remembers a newly created cache as the active database.
See [Importing your own devices](docs/IMPORTING-DEVICES.md) for the complete
GUI procedure, expected files and troubleshooting.

You can set the initial database explicitly:

```bash
LABTOOL48_INSTALLATION=/path/to/private-cache ./start-lt48uxp
```

For the separate volatile USB-bridge firmware load, select the verified firmware
file in the programmer setup controls, or start with:

```bash
LABTOOL48_FX2_FIRMWARE=/path/to/verified-fx2.hex ./start-lt48uxp
```

The private database cache does not by itself supply this USB firmware file.
The firmware-loading UI provides offline preparation and a separate execution
step. This package does not extract USB firmware from Windows installers.
Some existing default-path text still names the development tree; select actual
resource paths explicitly. Registered databases are stored in the user's
`~/.config/labtool48uxp/databases.ini`.

## Imported catalogue versus supported devices

**The binary decides what it can operate. Importing a Windows database does
not enable every device listed in that database.** The original resources and
USB firmware are bring-your-own inputs; keep them and generated private caches
outside this distribution repository and do not upload them with test reports.

The native C backend checks three things for a selected device:

1. **Catalogue entry:** the selected database contains the device.
2. **Audited resources:** its definition, family, pin map, capacity and resource
   contents match an admitted native workflow. Depending on the engine, SHA-256
   checks bind whole resource files or specific audited resources.
3. **Implemented operation:** this binary implements the requested operation
   for that accepted definition. The GUI uses these C checks to determine
   operation availability.

Finding a Read, Program or Erase callback in an imported resource file is not
sufficient to enable that operation.

| Imported content | Result |
| --- | --- |
| Audited resources and implemented workflow | Applicable operations are enabled |
| Listed device without an implemented workflow | Listing alone does not enable operations |
| Changed or unrecognized operation resources | The native engine rejects them |
| Corrupted or incomplete installation | Import or validation fails |

For example, the EN29LV flash batch requires the audited `GDAAA__.rez` digest
and one of ten admitted definitions with matching geometry and pin maps. A
matching device name in another software release does not automatically qualify.

**Cache verification confirms integrity and structural completeness; it does
not certify Linux operation support.** The supported-device list is generated
from the binary's operation checks, independently of the imported catalogue's
size. A newer or arbitrary Windows library can therefore provide less usable
coverage with this binary than the audited Dataman 13.10 library. Keep each
source version complete and separate; do not combine individual `.rez` files.

## Device coverage and testing

See [Supported devices](docs/SUPPORTED_DEVICES.md): **4,263 catalogue entries**,
including aliases and package variants. Entries include all applicable native
operations, but physical qualification is sample-specific. The list distinguishes
recorded hardware evidence from source audits. Selecting an operation may erase,
program or protect the inserted part; confirm the selected device and adapter.
Original package adapter requirements still apply, particularly TSOP/FBGA parts.

This is an external-test preparation snapshot. CLI startup, library resolution
and bundle integrity were checked; no new hardware tests were performed while
assembling it. Fresh-machine GUI startup and the first-run setup still require
external qualification. The
production GUI import worker passed offline tests for both supported Dataman
executable names, cache verification and rejection of unrecognized runtime data.

## Reporting a result

See [Testing and reports](docs/TESTING.md). Use the GitHub issue tracker when
invited to https://github.com/ashurbanipal-labs/lt48uxp.

## Integrity

From this directory run:

```bash
sha256sum -c SHA256SUMS
```
