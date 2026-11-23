# POF transformed Read reconstruction

Authored compiled C correction for Dataman 13.10 class 2028.
Not a device admission or a hardware qualification. No vendor payloads here.

The full-image path in `c/src/pof.c` reconstructs a source image from mapped
cell bits by intersecting the constraints on each source bit, then verifies
that the forward conversion reproduces every mapped cell. It publishes only
representable complete results. Original overwrite reconstruction remains
available separately for differential comparison.

This is necessary for the tested GCB A0/EPM5192(New) mapping: original x86
build/rebuild/build changes 67 mapped cell bytes for `(37*i+19)&255`; the new
inverse preserves them. The byte-core A1 mapping passes both reconstructions.
Exact source executable SHA256:
`f2d49c20757eb99748e91a497f5849e26bf14167726f77a9f313a79244701c12`.
GCB REZ SHA256:
`9bd0f6c9cd3e5e6888a23f8750c6acc6da7fafb64501f477e064db3c8bb74b12`.

Reproduction/evidence in the development repository:
- `tools/audit_pof_trs_image_x86.py --patterns 256 --roundtrip`
- `analysis/small-class-batch/pof-trs-roundtrip-x86.json`
- `analysis/small-class-batch/pof-full-image-regression.log`
- `docs/audit-qualification/SMALL_CLASS_COMPLETION_AUDIT.md`

The library index registers the canonical authored C implementation as
`source.pof-trs-read`, together with its build recipe and this evidence.
`c/src/pof.c` includes `sources/pof/trs_read_reconstruction.inc` directly, so
there is no separate implementation copy to drift. Changing the algorithm
requires a native-library rebuild. The resource index does not dynamically
replace executable code or create a supported-device entry. Variable correction
records remain reserved for parameter changes consumed by their engine.

The original-compatible overwrite reconstruction remains available for audit;
full-image Read uses the constraint inverse plus forward validation. The source
file contains both mapping traversal and corrected reconstruction, without
vendor coordinate tables. Those still come from the user's imported runtime.

Complete-operation admission is still pending. This report does not claim a
confirmed original UI bug without proving input reachability and the complete
application consequence.
