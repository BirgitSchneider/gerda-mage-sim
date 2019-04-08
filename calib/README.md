# calibration
**Remember to update also `UTILS/sim-parameters-all.json`**!

## volumes description

| volume name                          | mc name(s)                   | mc mass [g] | mc volume [cm^3] | density [g/cm^3] | volume description                                          | notes                                      |
| ------------------------------------ | ---------------------------- | ----------- | ---------------- | ---------------- | ----------------------------------------------------------- | ------------------------------------------ |
| multi_s1_[8220/8405/8570]            | `calibration_source_inner_0` | 0.242531    | 0.0502655        | 4.825            | active part of calibration source no S1, material: CapsGold | all sources at same pos, calibration depth indicated in folder name |
| multi_s2_[8220/8405/8570]            | `calibration_source_inner_1` | 0.242531    | 0.0502655        | 4.825            | active part of calibration source no S2, material: CapsGold | all sources at same pos, calibration depth indicated in folder name |
| multi_s3_[8220/8405/8570]            | `calibration_source_inner_2` | 0.242531    | 0.0502655        | 4.825            | active part of calibration source no S3, material: CapsGold | all sources at same pos, calibration depth indicated in folder name |
| single_s2_[8151/8220/8405/8408/8570] | `calibration_source_inner_1` | 0.242531    | 0.0502655        | 4.825            | active part of calibration source no S2, material: CapsGold | other sources at 7250, calibration depth indicated in folder name |
| single_s3_[8139/8220/8305/8405/8570] | `calibration_source_inner_2` | 0.242531    | 0.0502655        | 4.825            | active part of calibration source no S3, material: CapsGold | other sources at 7250, calibration depth indicated in folder name |


## source activity (for multi_* statistics)

| calibration source | isotope | location in Gerda | ref. activity [kBq] | ref. date    | cal. actviity [kBq] | cal. date          | decays / 30min |
| ------------------ | ------- | ----------------- | ------------------- | ------------ | ------------------- | ------------------ | -------------- |
| AD9854             | Th228   | S3                | 24.2+1.1-0.9        | 15. May 2014 | 4.6+0.2-0.2         | 06. September 2017 | 8338780        |
| AD9855             | Th228   | S2                | 34.2+1.5-1.3        | 15. May 2014 | 6.6+0.3-0.3         | 06. September 2017 | 11784556       |
| AD9856             | Th228   | S1                | 30.8+1.4-1.2        | 15. May 2014 | 5.9+0.3-0.2         | 06. September 2017 | 10612992       |  
| AD9857             | Th228   | X                 | 41.3+1.8-1.6        | 15. May 2014 |                     |                    |                |

See also `https://www.mpi-hd.mpg.de/gerdawiki/index.php/TG-12_Calibration` and `https://www.mpi-hd.mpg.de/gerda/internal/EB/store/2015/JINST10-2015-P12005.pdf`.
