# gedet

## volumes description

| volume name        | mc name(s) | mc mass [kg] | mc volume [cm^3] | density [kg/cm^3] | volume description | notes |
| ------------------ | ---------- | ------------ | ---------------- | ----------------- | ------------------ | ----- |
| intrinsic_chanwise | `Crystal_geometry$ID` and `CristalDeadLayer_geometry$ID`   | see below | N.A. | | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts | Detectors and active/dead volumes are simulated **separately**, files are marked with the channel number for a total amount of 40 files. `$ID` refers to the MaGe volume naming convention, a dictionary can be found under `UTILS/det-data/ged-mapping.json` | 
| intrinsic          | `Crystal_geometry$ID` and `CristalDeadLayer_geometry$ID`   | see below | N.A. | | Volume of detectors deployed in GERDA PhaseII, split in dead and active parts | Detectors and active/dead volumes are simulated **all together**. |
| surf_chanwise      | Custom primary vertices placed under `surf_chanwise/ver/`  | N.A. | N.A. | N.A. | Complete detector surface in contact with LAr | Detectors are simulated separately, further info in `surf_chanwise/ver/README.md` |
| nplus_chanwise     | Custom primary vertices placed under `nplus_chanwise/ver/` | N.A. | N.A. | N.A. | n+ contact surface in contact with LAr | Detectors are simulated separately, further info in `nplus_chanwise/ver/README.md` |
| nplus_bege         | Custom primary vertices placed under `nplus_bege/ver/`     | N.A. | N.A. | N.A. | n+ contact surface in contact with LAr | BEGe detectors are simulated all together |
| nplus_coax         | Custom primary vertices placed under `nplus_coax/ver/`     | N.A. | N.A. | N.A. | n+ contact surface in contact with LAr | Coax detectors are simulated all together |
| pplus_chanwise     | Custom primary vertices placed under `pplus_chanwise/ver/` | N.A. | N.A. | N.A. | p+ contact surface in contact with LAr | Detectors are simulated separately, further info in `pplus_chanwise/ver/README.md` |
| pplus_bege         | Custom primary vertices placed under `pplus_bege/ver/`     | N.A. | N.A. | N.A. | p+ contact surface in contact with LAr | BEGe detectors are simulated all together |
| pplus_coax         | Custom primary vertices placed under `pplus_coax/ver/`     | N.A. | N.A. | N.A. | p+ contact surface in contact with LAr | Coax detectors are simulated all together |
| lar_pplus_chanwise |                                                            |      |      |      | FILL ME                                |                                           |

## DL and AV masses in MaGe

|channel | detectors | DL mass [kg] | AV mass [kg]|
|--------|-----------|--------------|-------------|
|  0     | 91A       | 0.0695507    | 0.558278    |
|  1     | 35B       | 0.0699668    | 0.744159    |
|  2     | 02B       | 0.0722685    | 0.556694    |
|  3     | 00B       | 0.0831613    | 0.617949    |
|  4     | 61A       | 0.0796998    | 0.660286    |
|  5     | 89B       | 0.0883903    | 0.537077    |
|  6     | 02D       | 0.109938     | 0.553812    |
|  7     | 91C       | 0.0713986    | 0.562222    |
|  8     | ANG5      | 0.469738     | 2.28692     |
|  9     | RG1       | 0.227132     | 1.85816     |
| 10     | ANG3      | 0.312876     | 2.07287     |
| 11     | 02A       | 0.0565841    | 0.491953    |
| 12     | 32B       | 0.0839694    | 0.639849    |
| 13     | 32A       | 0.0542651    | 0.411089    |
| 14     | 32C       | 0.0779667    | 0.669628    |
| 15     | 89C       | 0.0744343    | 0.526743    |
| 16     | 61C       | 0.0726806    | 0.567876    |
| 17     | 76B       | 0.0590725    | 0.329243    |
| 18     | 00C       | 0.090774     | 0.745593    |
| 19     | 35C       | 0.0634542    | 0.578948    |
| 20     | 76C       | 0.100938     | 0.730695    |
| 21     | 89D       | 0.073113     | 0.463946    |
| 22     | 00D       | 0.0904854    | 0.730853    |
| 23     | 79C       | 0.0997517    | 0.721253    |
| 24     | 35A       | 0.0761838    | 0.697998    |
| 25     | 91B       | 0.0728945    | 0.582856    |
| 26     | 61B       | 0.0855659    | 0.673905    |
| 27     | ANG2      | 0.435991     | 2.4716      |
| 28     | RG2       | 0.338836     | 1.75211     |
| 29     | ANG4      | 0.237236     | 2.13991     |
| 30     | 00A       | 0.055093     | 0.420159    |
| 31     | 02C       | 0.0891885    | 0.706655    |
| 32     | 79B       | 0.0891606    | 0.657925    |
| 33     | 91D       | 0.0788082    | 0.626683    |
| 34     | 32D       | 0.0622322    | 0.668159    |
| 35     | 89A       | 0.0617257    | 0.459485    |
| 36     | ANG1      | 0.156392     | 0.802456    |
| 37     | GTF112    | 0.077482     | 2.91187     |
| 38     | GTF32     | 0.0660271    | 2.26281     |
| 39     | GTF45     | 0.0664049    | 2.28482     |
