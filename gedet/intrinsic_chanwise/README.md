# gedet/intrinsic_chanwise
**Remember to update also `UTILS/sim-parameters-all.json`**!

## simulations info

| isotope \[edep\|coin\] | contact     | primaries   | software tag \[branch/commit\]           | notes   |
| ---------------------- | ----------- | ----------- | :--------------------------------------: | ------- |
| 2nbb \[edep\]   | L.Pertoldi | `1E08` | MGDO: `clhep_2.3.3.1/cece4fe` MaGe: `GERDAPhaseII-Dec2017/9fe03ca` container: `g4.10.3_v2.1` | Each detector is simulated separately, files are marked with the channel number. The number of simulated events in each volume is calculated such to have a uniform distribution in the Ge76 mass over the complete array. Macros are auto-generated with the `UTILS/create-macros/create-2nbb-macros.jl` script from the `log/raw-gedet-intrinsic_chanwise-2nbb-%DET.tmac` template macro |
| 2nbbLV \[edep\] | L.Pertoldi | `1E08` | MGDO: `clhep_2.3.3.1/cece4fe` MaGe: `GERDAPhaseII-Dec2017/9fe03ca` container: `g4.10.3_v2.1` | Each detector is simulated separately, files are marked with the channel number. The number of simulated events in each volume is calculated such to have a uniform distribution in the Ge76 mass over the complete array. Macros are auto-generated with the `UTILS/create-macros/create-2nbb-macros.jl` script from the `log/raw-gedet-intrinsic_chanwise-2nbbLV-%DET.tmac` template macro |
