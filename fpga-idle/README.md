# Idle FPGA Verilog conversion

Corrected on 2026-10-14: this export uses the **XC3000A database**, configuration device **xc3030a**, fitted target **XC3030A-7PC84C**. It replaces the historical classic-XC3000 conversion previously copied here. A physical-part label alone does not select the architecture.

Source idle bitstream SHA-256: `1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab`. The source was decoded and byte-exact round-trip checked; export file hashes are recorded in `export-manifest.json`.

Database: `Components/XC3000/tools/open-source/prjcombine/databases/xc3000a.zstd`.
Exporter: `tools/xc3030decode/target/release/xc3030decode`.
Export directory: `analysis/xc3030a-idle-doc-export/output`.

`idle_fpga.v` requires `xc3000_primitives.v`. PCF and JSON preserve the recovered constraints. This is recovered structural HDL, with no claim of complete analogue or timing fidelity. A round trip alone does not validate every behavioral primitive.

| Copy | Source | SHA-256 |
| --- | --- | --- |
| `idle_fpga.v` | `analysis/xc3030a-idle-doc-export/output/verilog/1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab.v` | `a95549de0ac083765c08057bdeb74eb6f8b42f9516c922d9497484276852c193` |
| `idle_fpga.pcf` | `analysis/xc3030a-idle-doc-export/output/pcf/1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab.pcf` | `11e277704c99d5f3616c1aea5da544a2b09b48ef8a8f9e5c4d22128972e26fba` |
| `idle_fpga.constraints.json` | `analysis/xc3030a-idle-doc-export/output/constraints/1605d610bc48dc5a3878d0633eaaf3473bc01cedbfcf1cff0409d33ad08f0bab.constraints.json` | `e0a393765c12ff1749d25e671959ebca0a40e16728727c7ae26e35a411cf08fe` |
| `xc3000_primitives.v` | `tools/xc3030decode/models/xc3000_primitives.v` | `acc249f5acf397703e2ae6ae62753a5090996e467cf091a032eb90ac7b7d2893` |

The exporter header explicitly identifies `xc3030a`. `XC3000_*` identifiers are shared logical primitive names, not a selection of the classic architecture database. The stale unconditional EC `1101` warning has been removed from the exporter.
