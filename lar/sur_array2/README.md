# lar/sur_array
**Remember to update also `UTILS/sim-parameters-all.json`**!

## simulations info

| isotope \[edep\|coin\] | contact    | primaries | software tag \[branch/commmit\]                                                              | notes                     |
| ---------------------- | ---------- | --------- | :------------------------------------------------------------------------------------------: | ------------------------- |
| Kr85 \[edep\]          | R.Hiller   | 1E10      | MGDO: `MGDO-v2-07/0aed68db` MaGe: `g4.10/dd443589` container: `g4.10.3_v3.0`                 | 514 keV gamma line is killed in the stacking action of MaGe by the default cut on the livetime of the origin state. Set it to 1.5 us to keep it. |
