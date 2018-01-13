# gedet/intrinsic

## simulations info

| isotope \[edep\|coin\] | contact     | primaries   | software tag \[branch/commit\]           | notes   |
| ---------------------- | ----------- | ----------- | :--------------------------------------: | ------- |
| 2nbb \[edep\]   | L. Pertoldi | `1E08 * 80` | MGDO: `clhep_2.3.3.1/cece4fe` MaGe: `GERDAPhaseII-Dec2017/9fe03ca` container: `g4.10.3_v2.0` | Each Detector and active/dead volume is simulated separately, files are marked with the `AV\|DV` keyword and the channel number. Macros are auto-generated with the `UTILS/create-2nbb-macros.jl` script from the `log/raw-gedet-intrinsic-2nbb-%VOL-%DET.tmac` template macro |
| 2nbbLV \[coin\] | L. Pertoldi | `1E08 * 80` | MGDO: `clhep_2.3.3.1/cece4fe` MaGe: `GERDAPhaseII-Dec2017/9fe03ca` container: `g4.10.3_v2.0` | Each Detector and active/dead volume is simulated separately, files are marked with the `AV\|DV` keyword and the channel number. Macros are auto-generated with the `UTILS/create-2nbb-macros.jl` script from the `log/raw-gedet-intrinsic-2nbbLV-%VOL-%DET.tmac` template macro |
