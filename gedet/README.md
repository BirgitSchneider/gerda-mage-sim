# gedet

## volumes description

| volume name        | mc name(s)                                                    | mc mass [kg] | mc volume [cm^3] | density [g/cm^3]    | volume description                                                            | notes |
| ------------------ | ------------------------------------------------------------- | ------------ | ---------------- | ------------------- | ----------------------------------------------------------------------------- | ----- |
| intrinsic_chanwise | `Crystal_geometry$ID` and `CristalDeadLayer_geometry$ID`      | see below    | see below        | 5.54(enr) 5.32(nat) | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts | Detectors are simulated **separately**, files are marked with the channel number for a total amount of 40 files. `$ID` refers to the MaGe volume naming convention, a dictionary can be found under `UTILS/det-data/ged-mapping.json` | 
| intrinsic_all      | `Crystal_geometry$ID` and `CristalDeadLayer_geometry$ID`      | see below    | see below        | 5.54(enr) 5.32(nat) | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts | Detectors are simulated **all together**. |
| intrinsic_bege     | `Crystal_geometry$ID` and `CristalDeadLayer_geometry$ID` bege | see below    | see below        | 5.54(enr)           | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts | **BEGe** detectors are simulated all together. |
| intrinsic_coax     | `Crystal_geometry$ID` and `CristalDeadLayer_geometry$ID` coax | see below    | see below        | 5.54(enr)           | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts | **EnrCoax** detectors are simulated all together. |
| intrinsic_nat      | `Crystal_geometry$ID` and `CristalDeadLayer_geometry$ID` nat  | see below    | see below        | 5.32(nat)           | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts | **Natcoax** detectors are simulated all together. |
| surf_chanwise      | Custom primary vertices placed under `surf_chanwise/ver/`     | N.A.         | N.A.             | N.A.                | Complete detector surface in contact with LAr                                 | Detectors are simulated separately, further info in `surf_chanwise/ver/README.md` |
| nplus_chanwise     | Custom primary vertices placed under `nplus_chanwise/ver/`    | N.A.         | see below        | N.A.                | n+ contact surface in contact with LAr                                        | Detectors are simulated separately, further info in `nplus_chanwise/ver/README.md` |
| nplus_bege         | Custom primary vertices placed under `nplus_bege/ver/`        | N.A.         | see below        | N.A.                | n+ contact surface in contact with LAr                                        | BEGe detectors are simulated all together |
| nplus_coax         | Custom primary vertices placed under `nplus_coax/ver/`        | N.A.         | see below        | N.A.                | n+ contact surface in contact with LAr                                        | Coax detectors are simulated all together |
| pplus_chanwise     | Custom primary vertices placed under `pplus_chanwise/ver/`    | N.A.         | see below        | N.A.                | p+ contact surface in contact with LAr                                        | Detectors are simulated separately, further info in `pplus_chanwise/ver/README.md` |
| pplus_bege         | Custom primary vertices placed under `pplus_bege/ver/`        | N.A.         | see below        | N.A.                | p+ contact surface in contact with LAr                                        | BEGe detectors are simulated all together |
| pplus_coax         | Custom primary vertices placed under `pplus_coax/ver/`        | N.A.         | see below        | N.A.                | p+ contact surface in contact with LAr                                        | Coax detectors are simulated all together |
| lar_pplus_chanwise | FILL ME                                                       | FILL ME      | FILL ME          | FILL ME             | FILL ME                                                                       | FILL ME |

## Detector MaGe properties

Useful detector properties implemented in MaGe can be found in the `gedet-mc-info.json` JSON file in this directory.
