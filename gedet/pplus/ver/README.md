# gedet/pplus/ver
Vertices for `nplus` simulations are placed here. As Geant4 does not efficiently sample complex surfaces (especially boolean solids) the Generic Surface Sampler (GSS) is used. The resulting vertices are stored as (x,y,z) coordinates in a ROOT file. For further informations see [the MaGe user-guide](https://github.com/mppmu/gerda-snippets/blob/master/MaGe-macros/MaGe-userguide.pdf) at §6.11.2.

These vertices for p+ contact surfaces are obtained by extraction from the total detector surfaces placed under `gedet/surf/ver/`. the processing is performed by the `UTILS/surf-sampling/separate-contacts.jl` julia script.

## simulations info

| contact     | primaries | MGDO branch/commit    | MaGe branch/commmit        | notes |
| ----------- | --------- | --------------------- | -------------------------- | ----- |
| L. Pertoldi | O(10E5) for coaxials and O(10E4) for BEGEs, but vary for each detector |  |  | ROOT files are auto-genereted by the `UTILS/surf-sampling/separate-contacts.jl` script |

To get the actual number of events for each file you can run the following command into this folder:
```shell
$ root << EOF
          TFile * file;
          cout << "channel\tentries\n";
          for ( int i = 0; i < 39; ++i ) {
              file = TFile::Open(Form("ver-gedet-pplus-ch%i.root", i));
              cout << i << '\t' << GSSTree->GetEntries() << '\n';
          }
EOF
```
