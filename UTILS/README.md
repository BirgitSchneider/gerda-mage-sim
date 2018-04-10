# Command Line Tools
Collection of scripts used to produce macros or processing ROOT files. Run `make` to compile C++ programs.

* `det-data/`: contains (JSON) files with detectors' informations
* `readmes-tmp/`: template READMEs to be used in this repository
* `copy/`: `rsync`-based bash scripts to copy new files to MPIK and other locations
* `create-macros/` : macro generation utilities
    * `create-2nbb-macros.jl`: Julia script to produce, for each detector (dead layer and active volume), macros for 2nbb and 2nbbLV simulations (under `gedet/intrinsic/`)
    * `create-lar-pplus-macros.jl`: Julia script to produce macros for each detector for simulations inside the LAr close to the pplus contact (under `gedet/lar_pplus/`) using a template macro
    * `create-oncontacts-macros.jl`: Julia script to produce macros for each detector for simulations on the pplus and npluscontact (under `gedet/nplus/` and `gedet/pplus`) using a template macro
    * `create-macros-from-template.sh`: shell script to help you generate more statistics
    * `multiply-statistics.sh`: brute bash script to help you generate new sets of macro files with different number of primaries in them if you need more statistics
* `surf-sampling/`: gedet surface sampling tools
    * `create-surf-macros.jl`: Julia script to produce, for each detector, macro files for the surface sampling (under `gedet/surf/ver/`). It uses JSON.jl
    * `separate-contacts.jl`: Julia script to separate p+ and n+ contacts simulated surface vertices into different ROOT files (under `gedet/nplus/ver/` and `gedet/pplus/ver/`). It uses Cxx.jl, ROOT.jl and JSON.jl
    * `surf-calc.jl`: Julia script that calculates the area for p+ and n+ contacts for each detector
*  `post/`: utilities for raw files post-processing. Learn on how we manage the post-processing flow at MPIK in [the Wiki page](https://github.com/mppmu/gerda-snippets/wiki/PDFs-for-GERDA's-PhaseII-background-modelling)
    * `livetime-calc-ph2.cxx`: C++ program to extract each run's livetime and RunConfig. Results are stored in a JSON file that will be used by `t4z-gen.cxx` (see examples below)
    * `t4z-gen.cxx`: C++ program to generate a `t4z-`file for each run starting from a collection of `raw-`files (see examples below)
    * `pdf-gen.cxx`: C++ program to generate a `pdf-`file from a collection of `t4z-`files (see examples below)
    * `check-simulation.cxx`: C++ program to produce nice plots of decay vertices and interaction vertices
    * `res-curves-jsonizer.cxx`: C++ script to convert standard resolution curves into (Tier4izer-friendly) JSON format
    * `settings/`: JSON files for the post-processing
    * `gerda-metadata/`: (git submodule) version of [`gerda-metadata`](https://github.com/mppmu/gerda-metadata) used for post-processing
    * `old/`: old post-processing scripts
    * `Makefile`: Makefile to launch the post-processing execution queue (See [the Wiki](https://github.com/mppmu/gerda-snippets/wiki/PDFs-for-GERDA's-PhaseII-background-modelling#post-processing-at-mpik-with-gnu-make) for details)
    * `latex-report.jl`: Julia script to produce a nice report about the status of the simulations in LaTeX
* `job-scheduler/`: batch job manager utilities. `mpik-run-mage.qsub`, `mpik-t4z-gen.qsub`, `mpik-pdf-gen.qsub` and `mpik-pdf-gen-volume.qsub` are used by the post-processing Makefile
* `ranger/`: [GNU ranger](https://ranger.github.io) file browser, very useful in the context of this repository. Define the following alias to use it: `alias='$GERDA_MAGE_SIM_LOCATION/UTILS/ranger/ranger.py'`
* `health-dep/`: Health department!
    * `sim-doctor.cxx`: tool to check for problems in `.root` MaGe files
    * `pdf-doctor.cxx`: tool to check for problems in `t4z-*.root` and `pdf-*.root` files
* `Dockerfile`: recipe file to produce a Docker image that includes all the software needed to run the provided scripts, build with `sudo docker build --rm . -t gerda-mage-sim-utils`. To produce a Singularity image which is usable also on clusters where you don't have root permissions you can use Oliver's [docker2singularity.py](https://github.com/oschulz/singularity-utils) Python script. An already processed Singularity image is available at LNGS under `/nfs/gerda5/var/gerda-simulations/gerda-mage-sim-utils.sqsh`. First get all the Julia packages you need with `Pkg.add`, they will be installed under your home directory on the host (which is mounted by default by singularity), and so you won't need to do it again the next time you'll use Julia inside the container.
* `Makefile`: GNU Makefile to compile C++ programs in this directory

Usage examples:
```shell
$ cd UTILS/surf-sampling
$ sudo docker run -it --rm -v /common:/common -w $PWD gerda-mage-sim-utils julia surf-calc.jl
$ cd UTILS/create-macros
$ singularity exec gerda-mage-sim-utils.sqsh julia create-2nbb-macros.jl 2nbb
```
```shell
$ cd gerda-mage-sim
$ singularity run --cleanenv --app MaGe gerdasw.g4.10.3_v2.1 cables/hv_at_holders/K40/edep/log/raw-cables-hv_at_holders-K40-edep-000.mac
```

## PDFs production
example post-processing at the MPIK cluster (first load the preinstalled software – see below – or use `gerda-mage-sim-utils.sqsh`):
```
$ cd UTILS
$ make bin
$ bin/livetime-calc-ph2 \
    -v \
    --metadata /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/gerda-metadata \
    --data /lfs/l3/gerda/Daq/data-phaseII/blind/active/gen \
    --output /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/settings/run-livetime.json
$ bin/t4z-gen \
    -v \
    --srcdir /lfs/l2/gerda/gerda-simulations/gerda-mage-sim \
    --destdir /lfs/l2/gerda/gerda-simulations/gerda-pdfs/v0.2 \
    --metadata /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/gerda-metadata \
    --livetime-file /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/settings/run-livetime.json \
    --runlist-file /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/settings/run-list.json \
    /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/ge_holders/plates/K40/edep
$ bin/pdf-gen \
    -v \
    --destdir /lfs/l2/gerda/gerda-simulations/gerda-pdfs/v0.2 \
    --ged-mapping /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/det-data/ged-mapping.json \
    --custom-settings /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/settings/pdf-gen-settings-custom.json \
    ge_holders/plates/K40
```
But wait, [there's a Makefile that does all this for you](https://github.com/mppmu/gerda-snippets/wiki/PDFs-for-GERDA's-PhaseII-background-modelling#post-processing-at-mpik-with-gnu-make)! To create a new PDFs release at MPIK:
```shell
$ cd UTILS/post
$ make post CYCLE=<version>
$ ./create-release
```

### Analysis with the natural coax detectors
@mmilor made a dedicated keylist which is at the moment implemented in a separate branch called `t4z-plus-cycle_03.03-naturals` in the `post/gerda-metadata` submodule. To produce PDFs with the naturals detectors you have to:
1. Change to naturals branch and **manually update the `gerda-metadata` submodule**
```shell
$ git checkout naturals
$ git submodule update
```
2. `make post` and `create-release` as always.

### Create your own pdfs at MPIK
Recommended way:
* clone the repo somewhere
* change your configs in `UTILS/post/settings`
* start the production with the following commands:
```shell
$ cd UTILS/post
$ make post CYCLE=<your-name> GERDA_PDFS=<your-custom-location>
```

### Load preinstalled software (at MPIK) with [swmod](https://github.com/oschulz/swmod)
Add the following lines to your `~/.bashrc`
```shell
export PATH=$PATH:"/lfs/l3/gerda/sturm/sw/gerda-sw-all/util/swmod/bin/"
. swmod.sh init
export SWMOD_MODPATH="/lfs/l3/gerda/sturm/sw/.install"
export SWMOD_HOSTSPEC="linux-scientific-7.3-x86_64"
```
call the following command to see what is installed
```shell
$ swmod avail
```
and load what you need e.g.
```shell
$ swmod load gerda@master
$ swmod load julia
```

### `sed` Cheatsheet
```shell
$ sed -i ... file          # Change file directly with the `-i` flag
$ sed -i '/word/d' file    # Delete all lines containing a 'word' in 'file'
$ sed -i '10i stuff' file  # Insert 'stuff' in line number 10 in 'file'
$ rename a b *.root        # Find 'a' and replace it with 'b' in all `.root` filenames:
```
PS: with BSD sed (e.g. on OSX) you have to specify the extension of the backup file, e.g. `-i.bak`.

### Set permission for gerda-simulations directory
To remove all extra permissions
```shell
$ setfacl -Rb gerda-simulations
```
The permissions set at the moment are the following:
```shell
$ setfacl -Rdm user:<user>:rwX gerda-simulations
```
