# ST M28F201 zero preparation before erase

Authored C source: `sources/legacy-nor/st_m28f201_preerase.h`.
This is compiled into the private class-2007 owner. It does not add a supported
device or execute from the library index. Imported resources remain BYO.

ST's August 1998 M28F201 datasheet, page 7, requires programming the entire
array to zero before bulk erase. The authored prerequisite programs zero,
verifies at nominal voltage, and independently reads the array. Any failure
prevents the erase procedure. It applies only after exact imported resource
validation, to D89, 256 KiB, manufacturer 20h/device F4h; other legacy routes
are excluded. The expected erased image is then restored to FF.

Reference: https://docs.rs-online.com/f8f6/0900766b80029dbd.pdf

The prior private C owner failed the new ordering test: it launched erase with
A5 contents and no programming writes. The corrected native test passes both
successful preparation and a dropped-write case that must never launch erase.
The extracted library source also passes the byte49 stream and source erase-chain
regressions in `build-st28-library` (three CTest targets). No original Windows
software bug is claimed: its surrounding initialization still needs review.

The pre-extraction corrected build passed native read, normal/fault programming,
and twelve explicit operation cases through the original compact applets and
recovered FPGA with documented digital/analogue fixtures. Full erase is still
running against that unchanged build. Extraction changes code location, not the
sequence. Final qualification must retain the actual library hashes used by each
trial. Hardware testing and public/GUI admission remain pending.

Build and check from the repository root:

```sh
cmake -S . -B build-st28-library -DLT48_ENABLE_DESKTOP_TESTS=OFF
cmake --build build-st28-library --target labtool48-st28-preerase-test
ctest --test-dir build-st28-library -R legacy-st28-zero-preerase --output-on-failure
```
