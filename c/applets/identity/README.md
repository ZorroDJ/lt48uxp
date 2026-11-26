# ZAPPLET identity banner

`identity48_applet.asm` is the project-authored 8051 implementation of the
private `LTID`/`LTRS` BANNER protocol described in `ZAPPLET-SPECS.md`.
The 414-byte `identity48_applet.bin` is built from that source and embedded
in the native library at CMake configure time. Neither the original vendor
executable nor the original security-info applet is required by the default
build.

Run `make verify` here to assemble, compare the binary byte-for-byte, and
verify its SHA-256. This requires `as31` and Python 3; the normal project
build requires neither assembler nor the original executable.

With `-DORIGINAL_IDENTITY_APPLET=ON`, the library retains the previous
verified read-only U62 serial applet path. The local build script still
extracts that original applet from the owned executable for this mode.
