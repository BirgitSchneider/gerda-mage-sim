# larveto

## volumes description

| volume name           | mc name(s)                                              | mc mass [kg] | mc volume [cm^3] | density [kg/cm^3] | volume description | notes |
| --------------------- | ------------------------------------------------------- | ------------ | ---------------- | ----------------- | ------------------ | ----- |
| fibers                | `LArInstr_FiberOuter_Physical`                          |              |                  |                   |                    |       |
| fibers (segmented)    | `LArInstr_FiberSegment_Physical`                        |              |                  |                   |                    | repeated volume select with confine/volume       |
| sipm_ring             | `LArInstr_FiberOuter_SensitiveEnd_Physical`             |              |                  |                   |                    |       |
| sipm_ring (segmented) | `LArInstr_FiberOuter_SensitiveEnd_Physical [0..nseg-1]` |              |                  |                   |                    |       |
| pmt_bottom            | `LArInstr_PMTGlassPhysical [9..15]`                     |              |                  |                   |                    |       |
| pmt_top               | `LArInstr_PMTGlassPhysical [0..8]`                      |              |                  |                   |                    |       |
| copper_shroud         | `LArInstr_VM2000CylinderPhysical_top                    |              |                  |                   |                    |       | 
|                       | `LArInstr_VM2000CylinderPhysical_bottom                 |              |                  |                   |                    |       |

A picture of the volumes put together: fibers (non-segmented), sipm_ring (non-segmented), pmt_bottom and pmt_top:
<p align="center">
  <img src="larveto.png" width="400"/>
</p>
