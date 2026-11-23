# LT48UXP — independent Linux programmer software

Build **0.1.0-test.20261123.1**, Ubuntu **26.04 amd64**. Limited-test prerelease.
This repository now includes the project's native C library, CLI, GTK 4 GUI,
authored NewDeviceLib and a standalone CMake build. The existing `fpga-idle/`
project-generated Verilog study is retained.

**No original vendor software, databases, firmware or imported private caches
are supplied.** Obtain your own matching resources separately. Source archives
now contain buildable application source, unlike the October documentation-only
release. Packages are available under [Releases](https://github.com/ashurbanipal-labs/lt48uxp/releases).

## Install the Ubuntu package

Download `lt48uxp_0.1.0~test.20261123.1_amd64.deb`, then:

```bash
sudo apt install ./lt48uxp_0.1.0~test.20261123.1_amd64.deb
```

Launch **LabTool-48UXP** from the application menu, or run `labtool48-gtk`.
Run it as your normal desktop user. The package installs a udev rule; reconnect
USB after installation. On a remotely logged-in/non-desktop session, check device
permissions rather than running the GUI as root. Uninstall with:

```bash
sudo apt remove lt48uxp
```

This binary is built on Ubuntu 26.04 and is not claimed compatible with 24.04,
22.04 or other architectures. Use the source build for other environments.
Dependencies are declared by the `.deb` and resolved by apt.

## Extract-and-run alternative

Extract `lt48uxp-0.1.0-test.20261123.1-ubuntu26.04-amd64.tar.gz`, enter its
`lt48uxp` directory, install runtime dependencies and run:

```bash
sudo apt install libgtk-4-1 libusb-1.0-0
./start-lt48uxp
./lt48-cli version --json
```

Keep `bin`, `lib` and `share` together. For USB access with the tar package:

```bash
sudo install -m 0644 lib/udev/rules.d/70-labtool48.rules /etc/udev/rules.d/70-labtool48.rules
sudo udevadm control --reload-rules
```

Reconnect USB afterwards. The tar package needs no system-wide application install.

## Import your own device resources

Use **Manage Device Databases** to import a complete matching extracted vendor
installation, or select an existing verified private cache. `.rez` is read
directly; no JSON conversion or Windows VM is required. Dataman 13.10 is the
main supported baseline, with additional source-specific AEC support. Files
from different versions must not be mixed. The application does not unpack
Windows installers. See [Importing devices](docs/IMPORTING-DEVICES.md).

CLI example (use `./lt48-cli` instead of `labtool48` with the tar package):

```bash
labtool48 cache import /path/to/definitions /path/to/new-private-cache \
  --runtime-source /path/to/original-installation
labtool48 cache verify /path/to/new-private-cache
```

The runtime source must contain a recognized original executable or verified
idle-FPGA object. USB-bridge firmware is also bring-your-own: use **Import FX2
HEX…** in programmer controls. Firmware must pass native validation; a matching
filename is insufficient. Importing a database does not itself load USB firmware.

Optional startup overrides:

```bash
LABTOOL48_INSTALLATION=/path/to/private-cache labtool48-gtk
LABTOOL48_FX2_FIRMWARE=/path/to/verified-fx2.hex labtool48-gtk
```

Only the selected source's admitted native operations are executable. Seeing a
device in **All** does not establish support. **Supported** filters by native
admission; changing resource versions can change availability. NewDeviceLib
1.0.710 accompanies this build. The published support union is 15,768 entries
across sources, not 15,768 hardware-tested physical parts. Consult
[Supported devices](docs/SUPPORTED_DEVICES.md) and per-entry audit/HW attributes.

## Build from source

```bash
sudo apt install build-essential cmake pkg-config libgtk-4-dev libusb-1.0-0-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_INSTALL_LIBDIR=lib
cmake --build build -j2
sudo cmake --install build
sudo ldconfig
labtool48-gtk
```

GTK 4.10 or newer is required by the GUI APIs. The CLI and underlying operation
owners are native C. Python, Ghidra and the Windows VM are not runtime/build
requirements. The standalone CMake build excludes the private research harnesses.
Run `ctest --test-dir build --output-on-failure` for the included offline
package diagnostics. To rebuild the .deb and tar assets, install `python3` and
`dpkg-dev`, then run `python3 packaging/build-packages.py`.

Manifest-declared project-created library resources are checked by SHA-256
while configuring. SOURCE-PROVENANCE.json records the production export inputs.

To inspect that library without a programmer:

```bash
labtool48 new-device-lib validate /usr/local/share/labtool48/NewDeviceLib --json
```

## Changes and qualification limits

This release includes the recent contributor maintenance fixes and GUI work:
nonblocking progress polling, full-image editor navigation and edit guards,
retained catalogue parsing and a persistent support/filter index. See the
[GUI audit](docs/GUI_OPERATIONS_AUDIT.md). The three GUI fixes passed offline
regressions including 116 real prepare/dispatch cases stopped before USB.
The originally reported physical-session “not responding” warning still needs
live confirmation. First catalogue indexing after a source/build change can
still take time; later searches reuse the cached index.

Most supported entries are qualified by original-workflow audit and simulation,
not individual hardware testing. Inspect adapter requirements, voltage and
operation availability before using a valuable part. Operation-specific
limitations remain visible; publication does not waive them. Test only devices
whose contents you may alter. [Testing/reporting guide](docs/TESTING.md).

The packages contain no vendor `.rez`, Windows executable, USB firmware, vendor
FPGA bitstream or private imported object. The sole shipped applet `.bin` is a
38-byte routine authored in this project; its assembly and encoder are included.
No license for vendor material is conveyed. No additional open-source license
is selected by this publication; dependency licenses continue to apply.

## Why “ashurbanipal-labs”?

The name refers to the [Library of Ashurbanipal](https://en.wikipedia.org/wiki/Library_of_Ashurbanipal),
the ancient collection of cuneiform clay tablets at Nineveh. Think of those
tablets as an early kind of **ROM-code**: information encoded into a physical
medium, retained without power, and readable long after its creators were gone.
Their contents were texts rather than executable instructions, but the
preservation principle is familiar to anyone reading an old ROM.

This project carries that idea into electronics: recover the knowledge,
document how it works, and keep older equipment readable and useful. The
repository name `lt48uxp` identifies the programmer it supports.
