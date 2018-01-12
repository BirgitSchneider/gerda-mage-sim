# Command Line Tools
Collection of scripts used to produce macros or processing ROOT files. Run `make` to compile C++ programs.

* `det-data/`: contains (JSON) files with detectors' informations
* `readmes-tmp/`: template READMEs to be used in this repository
* `copy/`: `rsync`-based bash scripts to copy new files to LNGS and other locations, see [here](https://github.com/mppmu/gerda-snippets/wiki/gerda-mage-sim:-official-MaGe-simulations#copying-back-your-files-to-lngs) for further help
* `create-macros/` : macro generating utilities
    * `create-2nbb-macros.jl`: julia script to produce, for each detector (dead layer and active volume), macros for 2nbb and 2nbbLV simulations (under `gedet/intrinsic/`)
    * `create-lar-pplus-macros.jl`: julia script to produce macros for each detector for simulations inside the LAr close to the pplus contact (under `gedet/lar_pplus/`) using a template macro
    * `create-oncontacts-macros.jl`: julia script to produce macros for each detector for simulations on the pplus and npluscontact (under `gedet/nplus/` and `gedet/pplus`) using a template macro
    * `multiply-statistics.sh`: rude bash script to help you generate new macro files if you need more statistics
* `surf-sampling/`: gedet surface sampling tools
    * `create-surf-macros.jl`: julia script to produce, for each detector, macro files for the surface sampling (under `gedet/surf/ver/`). It uses JSON.jl
    * `separate-contacts.jl`: julia script to separate p+ and n+ contacts simulated surface vertices into different ROOT files (under `gedet/nplus/ver/` and `gedet/pplus/ver/`). It uses Cxx.jl, ROOT.jl and JSON.jl
    * `surf-calc.jl`: julia script that calculates the area for p+ and n+ contacts for each detector
* `post/`: raw files post-processing
    * `gen-spectra.cxx`: C++ program to produce standard energy histograms that can be found in this repository (`spc-` files). They are only meant for quick visualization purposes
    * `process-raw.jl`: julia script to help produce `t4z-` and `spc-` files for a part in the correct location
    * `process-volume.jl`: rjulia script to help produce `t4z-` and `spc-` files for a volume in the correct location
    * `check-simulation.cxx`: C++ program to produce nice plots of decay vertices and interaction vertices
* `job-scheduler/`: batch job manager utilities
* `ranger/`: GNU ranger file browser, very useful in the context of this repository. To use it define the following alias `alias='$GERDA_MAGE_SIM_LOCATION/UTILS/ranger/ranger.py'`
* `Dockerfile`: recipe file to produce a Docker image that includes all the software needed to run the provided scripts, build with `sudo docker build --rm . -t gerda-mage-sim-utils`. To produce a Singularity image which is usable also on clusters where you don't have root permissions run:
```
$ sudo docker run \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v $PWD:/output \
    --privileged -t --rm \
    singularityware/docker2singularity \
    gerda-mage-sim-utils
```
An already processed Singularity image is available at LNGS under `/nfs/gerda5/var/gerda-simulations/gerda-mage-sim-utils.img`

Usage examples:
```shell
$ cd UTILS/post
$ sudo docker run -it --rm -v /common:/common -w $PWD gerda-mage-sim-utils rjulia process-volume.jl cables Bi212 edep
$ singularity run -B /common:/common gerda-mage-sim-utils.img julia process-raw.jl ../../cables/hv_at_holders/K40/edep
```
```shell
$ cd gerda-mage-sim
$ singularity run --cleanenv --app MaGe gerdasw.g4.10.3_v2.1 cables/hv_at_holders/K40/edep/log/raw-cables-hv_at_holders-K40-edep-000.mac
```
