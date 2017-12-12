# gedet/surf/ver
Vertices for `surf` simulations are placed here, corresponding macro files can be found under `log/`. As Geant4 does not efficiently sample complex surfaces (especially boolean solids) the Generic Surface Sampler (GSS) is used. The resulting vertices are stored as (x,y,z) coordinates in a ROOT file. For further informations see [the MaGe user-guide](https://github.com/mppmu/gerda-snippets/blob/master/MaGe-macros/MaGe-userguide.pdf) at §6.11.2.

The simulations are further processed to extract p+ and n+ surfaces under `gedet/pplus/ver` and `gedet/nplus/ver/`.

## simulations info

| contact     | primaries | MGDO branch/commit    | MaGe branch/commmit        | notes |
| ----------- | --------- | --------------------- | -------------------------- | ----- |
| L. Pertoldi | O(10E6), but vary for each detector |  |  | Macros are auto-genereted by the `UTILS/create-surf-macros.jl` script |

To get the actual number of events for each file you can run the following command into this folder:
```shell
$ root << EOF
          TFile * file;
          cout << "channel\tentries\n";
          for ( int i = 0; i < 39; ++i ) {
              file = TFile::Open(Form("ver-gedet-surf-ch%i.root", i));
              cout << i << '\t' << GSSTree->GetEntries() << '\n';
          }
EOF
```
