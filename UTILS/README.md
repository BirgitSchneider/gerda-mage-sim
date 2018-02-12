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
    * `separate-contacts.jl`: Julia script to separate p+ and n+ contacts simulated surface vertices into different ROOT files (under `gedet/nplus/ver/` and `gedet/pplus/ver/`). It uses Cxx.jl, ROOT.jl and JSON.jl (so use `rjulia` inside the `gerda-mage-sim-utils` container!)
    * `surf-calc.jl`: Julia script that calculates the area for p+ and n+ contacts for each detector
*  `post/`: utilities for raw files post-processing. Learn on how we manage the post-processing flow at MPIK in [the Wiki page](https://github.com/mppmu/gerda-snippets/wiki/PDFs-for-GERDA's-PhaseII-background-modelling)
    * `livetime-calc-ph2.cxx`: C++ program to extract each run's livetime and RunConfig. Results are stored in a JSON file that will be used by `t4z-gen.cxx` (see examples below)
    * `t4z-gen.cxx`: C++ program to generate a `t4z-`file for each run starting from a collection of `raw-`files (see examples below)
    * `pdf-gen.cxx`: C++ program to generate a `pdf-`file from a collection of `t4z-`files (see examples below)
    * `pdf-gen-volume.cxx`: C++ program to join PDFs of each single part to produce a `pdf-`file for a volume (see examples below)
    * `check-simulation.cxx`: C++ program to produce nice plots of decay vertices and interaction vertices
    * `gerda-metadata/`: (git submodule) version of [`gerda-metadata`](https://github.com/mppmu/gerda-metadata) used for post-processing
    * `old/`: old post-processing scripts
    * `Makefile`: Makefile to launch the post-processing execution queue (See [the Wiki](https://github.com/mppmu/gerda-snippets/wiki/PDFs-for-GERDA's-PhaseII-background-modelling#post-processing-at-mpik-with-gnu-make) for details)
* `job-scheduler/`: batch job manager utilities. `mpik-run-mage.qsub`, `mpik-t4z-gen.qsub`, `mpik-pdf-gen.qsub` and `mpik-pdf-gen-volume.qsub` are used by the post-processing Makefile
* `ranger/`: [GNU ranger](https://ranger.github.io) file browser, very useful in the context of this repository. Define the following alias to use it: `alias='$GERDA_MAGE_SIM_LOCATION/UTILS/ranger/ranger.py'`
* `Dockerfile`: recipe file to produce a Docker image that includes all the software needed to run the provided scripts, build with `sudo docker build --rm . -t gerda-mage-sim-utils`. To produce a Singularity image which is usable also on clusters where you don't have root permissions you can use Oliver's [docker2singularity.py](https://github.com/oschulz/singularity-utils) Python script. An already processed Singularity image is available at LNGS under `/nfs/gerda5/var/gerda-simulations/gerda-mage-sim-utils.sqsh`
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
example post-processing at the MPIK cluster (first load the preinstalled software – see below – or use `gerda-mage-sim-utils.sqsh`):
```
$ cd UTILS
$ make bin
$ bin/livetime-calc-ph2 \
    -v \
    --metadata /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/gerda-metadata \
    --data /lfs/l3/gerda/Daq/data-phaseII/blind/v03.01/gen \
    --output /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/run-livetime.json
$ bin/t4z-gen \
    -v \
    --destdir /lfs/l2/gerda/gerda-simulations/gerda-mage-sim \
    --destdir /lfs/l2/gerda/gerda-simulations/gerda-pdfs/v0.2 \
    --metadata /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/gerda-metadata \
    --livetime-file /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/run-livetime.json \
    /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/ge_holders/plates/K40/edep
$ bin/pdf-gen \
    -v \
    --destdir /lfs/l2/gerda/gerda-simulations/gerda-pdfs/v0.2 \
    --ged-mapping /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/det-data/ged-mapping.json \
    ge_holders/plates/K40
$ bin/pdf-gen-volume \
    -v
    --destdir /lfs/l2/gerda/gerda-simulations/gerda-pdfs/v0.2 \
    --ged-mapping /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/det-data/ged-mapping.json \
    cables
```
But wait, [there's a Makefile that does all this for you](https://github.com/mppmu/gerda-snippets/wiki/PDFs-for-GERDA's-PhaseII-background-modelling#post-processing-at-mpik-with-gnu-make)!

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
```

### Useful [`sed`](https://www.gnu.org/software/sed/) commands to quickly modify macros
Change file directly with the `-i` flag
```shell
$ sed -i ... file
```
Delete all lines containing a 'word' in 'file'
```shell
$ sed -i '/word/d' file
```
Insert 'stuff' in line number 10 in 'file'
```shell
$ sed -i '10i stuff' file
```

### Set permission for gerda-simulations directory
To remove all extra permissions
```shell
$ setfacl -Rb gerda-simulations
```
The permissions set at the moment are the following:
```shell
$ setfacl -Rdm user:<user>:rwX gerda-simulations
```
