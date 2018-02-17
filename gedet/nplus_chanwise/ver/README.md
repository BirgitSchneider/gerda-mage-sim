# gedet/nplus_chanwise/ver
Vertices for `nplus_chanwise` simulations are placed here. As Geant4 does not efficiently sample complex surfaces (especially boolean solids) the Generic Surface Sampler (GSS) is used. The resulting vertices are stored as (x,y,z) coordinates in a ROOT file. For further informations see [the MaGe user-guide](https://github.com/mppmu/gerda-snippets/blob/master/MaGe-macros/MaGe-userguide.pdf) at §6.11.2.

These vertices for n+ contact surfaces are obtained by extraction from the total detector surfaces placed under `gedet/surf_chanwise/ver/`. the processing is performed by the `UTILS/surf-sampling/separate-contacts.jl` julia script.

## simulations info

| contact    | primaries                           | notes |
| ---------- | ----------------------------------- | ----- |
| L.Pertoldi | O(1E06), but vary for each detector | ROOT files are auto-genereted by the `UTILS/surf-sampling/separate-contacts.jl` script |

To get the actual number of events for each file you can run the following command into this folder:
```shell
$ root << EOF
          TFile * file;
          cout << "channel\tentries\n";
          for ( int i = 0; i < 39; ++i ) {
              file = TFile::Open(Form("ver-gedet-nplus_chanwise-ch%i.root", i));
              cout << i << '\t' << GSSTree->GetEntries() << '\n';
          }
EOF
```
The areas of the n+ and p+ contacts, for each detector, can be calculated with the `UTILS/surf-sampling/surf-calc.jl` script and also found in `gedet/gedet-mc-info.json`

| detector | ch   | p+ [cm^2] | n+ [cm^2] |
| -------- | ---- | --------- | --------- |
| GD02B    | `2`  | 0.017671  | 1.428436  |
| GD76B    | `17` | 0.017671  | 1.011580  |
| ANG1     | `36` | 0.269392  | 1.779576  |
| ANG5     | `8`  | 0.439823  | 3.551767  |
| GTF112   | `37` | 0.318400  | 3.798971  |
| GD02A    | `11` | 0.017671  | 1.231704  |
| GD35C    | `19` | 0.017671  | 1.495336  |
| GD61B    | `26` | 0.017671  | 1.623956  |
| GD61C    | `16` | 0.017671  | 1.489495  |
| GD89B    | `5`  | 0.017671  | 1.498582  |
| GD32C    | `14` | 0.017671  | 1.560177  |
| ANG4     | `29` | 0.462128  | 3.234113  |
| GD91B    | `25` | 0.017671  | 1.450165  |
| GD02D    | `6`  | 0.017671  | 1.433154  |
| GD35A    | `24` | 0.017671  | 1.486206  |
| ANG2     | `27` | 0.484119  | 3.688858  |
| GD61A    | `4`  | 0.017671  | 1.472003  |
| GD91A    | `0`  | 0.017671  | 1.309025  |
| ANG3     | `10` | 0.481920  | 3.228301  |
| GTF45_2  | `39` | 0.226038  | 3.232542  |
| GD89D    | `21` | 0.017671  | 1.372015  |
| GD79C    | `23` | 0.017671  | 1.725681  |
| GD02C    | `31` | 0.017671  | 1.644104  |
| GD79B    | `32` | 0.017671  | 1.625186  |
| GD00D    | `22` | 0.017671  | 1.688346  |
| GD32B    | `12` | 0.017671  | 1.535173  |
| GD89A    | `35` | 0.017671  | 1.169077  |
| GD89C    | `15` | 0.017671  | 1.454045  |
| GD35B    | `1`  | 0.017671  | 1.681300  |
| GD00C    | `18` | 0.017671  | 1.690688  |
| GD76C    | `20` | 0.017671  | 1.690718  |
| GD00A    | `30` | 0.017671  | 1.132187  |
| GD91D    | `33` | 0.017671  | 1.509343  |
| GD32A    | `13` | 0.017671  | 1.122269  |
| RG2      | `28` | 0.376677  | 2.931774  |
| GD00B    | `3`  | 0.017671  | 1.540449  |
| GD91C    | `7`  | 0.017671  | 1.418374  |
| GD32D    | `34` | 0.017671  | 1.547367  |
| GTF32    | `38` | 0.227137  | 3.223117  |
| RG1      | `9`  | 0.400396  | 2.931774  |
