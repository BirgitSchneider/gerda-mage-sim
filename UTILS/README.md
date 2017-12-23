# Command Line Tools
Collection of scripts used to produce macros or processing ROOT files.

* `det-data/`: contains (JSON) files with detectors' informations
* `READMEsim_temp.md`, `READMEvol_temp.md`: template READMEs to be used in this repository
* `copy-to-lngs.sh`: `rsync`-based bash script to copy new files to LNGS, see [here](https://github.com/mppmu/gerda-snippets/wiki/gerda-mage-sim:-official-MaGe-simulations#copying-back-your-files-to-lngs) for further help
* `copy-decay0files-from-lngs.sh`: `rsync`-based script to copy the decay0 files on your cluster
* `copy-singularity-container-from-lngs.sh`: `rsync`-based script to copy the latest version of the [singularity image with the GERDA software](https://github.com/gipert/gerdasw-containers) on your cluster
* `create-2nbb-macros.jl`: julia script to produce, for each detector (dead layer and active volume), macros for 2nbb and 2nbbLV simulations (under `gedet/intrinsic/`)
* `surf-sampling/`:
    * `create-surf-macros.jl`: julia script to produce, for each detector, macro files for the surface sampling (under `gedet/surf/ver/`). It uses JSON.jl
    * `separate-contacts.jl`: julia script to separate p+ and n+ contacts simulated surface vertices into different ROOT files (under `gedet/nplus/ver/` and `gedet/pplus/ver/`). It uses Cxx.jl, ROOT.jl and JSON.jl
    * `surf-calc.jl`: julia script that calculates the area for p+ and n+ contacts for each detector
* `gen-spectra.cxx`: C++ program to produce standard energy histograms that can be found in this repository (`spc-` files) **work in progress...**
