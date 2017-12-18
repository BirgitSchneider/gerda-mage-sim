# Command Line Tools
Collection of scripts used to produce macros or processing ROOT files.

* `det-data/`: contains (JSON) files with detectors' informations
* `READMEsim_temp.md`, `READMEvol_temp.md`: template READMEs to be used in this repository
* `copy-to-lngs.sh`: `rsync`-based bash script to copy new files to LNGS, see [here](https://github.com/mppmu/gerda-snippets/wiki/MaGe-simulations-stored-at-LNGS#adding-your-contribution-to-the-official-repository) for further help
* `create-2nbb-macros.jl`: julia script to produce, for each detector (dead layer and active volume), macros for 2nbb and 2nbbLV simulations (under `gedet/intrinsic/`)
* `surf-sampling/`:
    * `create-surf-macros.jl`: julia script to produce, for each detector, macro files for the surface sampling (under `gedet/surf/ver/`). It uses JSON.jl
    * `separate-contacts.jl`: julia script to separate p+ and n+ contacts simulated surface vertices into different ROOT files (under `gedet/nplus/ver/` and `gedet/pplus/ver/`). It uses Cxx.jl, ROOT.jl and JSON.jl
    * `surf-calc.jl`: julia script that calculates the area for p+ and n+ contacts for each detector
* `gen-spectra.cxx`: C++ program to produce standard energy histograms that can be found in this repository (`spc-` files) **work in progress...**
