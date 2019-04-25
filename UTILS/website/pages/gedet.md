---
layout: page
---

# gedet

Simulations in the germanium detectors.

| part | description | mass | volume/surface | Density | MaGe volumes |
| -- | -- | -- | -- | -- | -- |
| nplus_coax | n+ contact surface in contact with LAr. Detectors are simulated all together. | --- | 21.34 cm^2 | --- | `gedet/nplus_coax/ver/ver-gedet-nplus_coax.root` |
| nplus_chanwise | n+ contact surface in contact with LAr. Detectors are simulated separately, further info in `nplus_chanwise/ver/README.md` | see `gedet/gedet-mc-info.json` | see `gedet/gedet-mc-info.json` | see `gedet/gedet-mc-info.json` | see description |
| intrinsic_coax | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts. **EnrCoax** detectors are simulated all together. | 15.56 kg | 2809.0 cm^3 | 5.54 g cm^-3 | `Crystal_geometry11_[4-10]`, `CristalDeadLayer_geometry11_[4-10]` |
| nplus_bege | n+ contact surface in contact with LAr. Detectors are simulated all together. | --- | 44.03 cm^2 | --- | `gedet/nplus_bege/ver/ver-gedet-nplus_bege.root` |
| pplus_coax | n+ contact surface in contact with LAr. Detectors are simulated all together. | --- | 2.914 cm^2 | --- | `gedet/pplus_coax/ver/ver-gedet-pplus_coax.root` |
| pplus_bege | n+ contact surface in contact with LAr. Detectors are simulated all together. | --- | 0.5301 cm^2 | --- | `gedet/pplus_bege/ver/ver-gedet-pplus_bege.root` |
| intrinsic_bege | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts. **BEGe** detectors are simulated all together. | 20.2 kg | 3647.0 cm^3 | 5.54 g cm^-3 | `Crystal_geometry5_[1-30]`, `CristalDeadLayer_geometry5_[1-30]` |
| lar_pplus_chanwise | LAr cylinders over all p+ contacts, as calculated by `UTILS/create-macros/create-lar-pplus-chanwise-macros.jl` | see `UTILS/create-macros/create-lar-pplus-chanwise-macros.jl` | see `UTILS/create-macros/create-lar-pplus-chanwise-macros.jl` | see `UTILS/create-macros/create-lar-pplus-chanwise-macros.jl` | see description |
| surf_chanwise | Complete detector surface in contact with LAr. Detectors are simulated separately, further info in `surf_chanwise/ver/README.md` | see `gedet/gedet-mc-info.json` | see `gedet/gedet-mc-info.json` | see `gedet/gedet-mc-info.json` | see description |
| intrinsic_nat | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts. **NatCoax** detectors are simulated all together. | 7.669 kg | 1441.0 cm^3 | 5.32 g cm^-3 | `Crystal_geometry11_[1-3]`, `CristalDeadLayer_geometry11_[1-3]` |
| pplus_chanwise | p+ contact surface in contact with LAr. Detectors are simulated separately, further info in `pplus_chanwise/ver/README.md` | see `gedet/gedet-mc-info.json` | see `gedet/gedet-mc-info.json` | see `gedet/gedet-mc-info.json` | see description |
| intrinsic_chanwise | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts. Detectors are simulated **separately**, files are marked with the channel number for a total amount of 40 files. `$ID` refers to the MaGe volume naming convention, a dictionary can be found under `UTILS/det-data/ged-mapping.json` | --- | --- | 5.54 g cm^-3 | see description |

<p align="center">
<p/>
