# gedet

## volumes description

| volume name | mc name(s) | mc mass [kg] | mc volume [cm^3] | density [kg/cm^3] | volume description | notes |
| ----------- | ---------- | ------------ | ---------------- | ----------------- | ------------------ | ----- |
| intrinsic   | `Crystal_geometry$ID`, `CristalDeadLayer_geometry$ID` | | | | Volume of all detectors deployed in GERDA PhaseII, split in dead and active parts | Detectors and active/dead volumes are simulated separately, files are marked with the `AV\|DV` keyword and with the channel number for a total amount of 40 files. `$ID` refers to the MaGe volume naming convention, a dictionary can be found under `UTILS/det-data/ged-mapping.json` | 
| surf        | Custom primary vertices placed under `surf/ver/` | | | | Complete detector surface in contact with LAr | Detectors are simulated separately, further info in `surf/ver/README.md` |
| nplus       | Custom primary vertices placed under `nplus/ver/` | | | | n+ contact surface in contact with LAr | Detectors are simulated separately, further info in `nplus/ver/README.md` |
| pplus       | Custom primary vertices placed under `pplus/ver/` | | | | p+ contact surface in contact with LAr | Detectors are simulated separately, further info in `pplus/ver/README.md` |
