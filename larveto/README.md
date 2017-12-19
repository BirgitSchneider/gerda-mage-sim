# larveto

## volumes description

| volume name | mc name(s) | mc mass [kg] | mc volume [cm^3] | density [kg/cm^3] | volume description | notes |
| ----------- | ---------- | ------------ | ---------------- | ----------------- | ------------------ | ----- |
| fibers      | LArInstr_FiberOuter_Physical |              |                  |                   |                    |	  |
| fibers (segmented) | LArInstr_FiberSegment_Physical |     ||                   |                    | repeated volume select with confine/volume       | 
| sipm_ring   | LArInstr_FiberOuter_SensitiveEnd_Physical |              |                  |                   |                    |       |
| sipm_ring (segmented) | LArInstr_FiberOuter_SensitiveEnd_Physical [0..nseg-1] |              |                  |                   |                    |       |
| pmt_bottom  | LArInstr_PMTGlassPhysical [9..15]           |              |                  |                   |                    |       |
| pmt_top     | LArInstr_PMTGlassPhysical [0..8]           |              |                  |                   |                    |       |



