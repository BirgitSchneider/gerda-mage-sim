# minishroud/bottoms
**Remember to update also `UTILS/sim-parameters-all.json`**

## simulations info

| isotope \[edep\|coin\] | contact     | primaries   | software tag: \[branch/commit\]                                                              | notes   |
| ---------------------- | ----------- | ---------   | :------------------------------------------------------------------------------------------: | ------- |
| K40    \[edep\]        | T. Wester   | 6919166     | MGDO: `clhep_2.3.3.1/cece4fe` MaGe: `GERDAPhaseII-Dec2017/9fe03ca` container: `g4.10.3_v2.1` | The number of primaries is calculated such that if you put together *all* the five parts in the minishroud volume (i.e. top, bottom, tub, glue_ring_bottom and glue_ring_top) you get 1E08 primaries uniformly distributed all over the complete volume. You can find also simulations with Decay0 under `dk0/` |
| K40    \[coin\]        | T. Wester   | 6919166x10  | MGDO: `clhep_2.3.3.1/cece4fe` MaGe: `GERDAPhaseII-Dec2017/9fe03ca` container: `g4.10.3_v2.1` | The number of primaries is calculated such that if you put together *all* the five parts in the minishroud volume (i.e. top, bottom, tub, glue_ring_bottom and glue_ring_top) you get 1E09 primaries uniformly distributed all over the complete volume. You can find also simulations with Decay0 under `dk0/` |
