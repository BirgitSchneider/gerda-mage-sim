# Command Line Tools
Collection of scripts used to produce macros or processing ROOT files. Run
`make` to compile C++ programs.

## Mage simulations production

`gerda-mage-sim/make.jl` implements a routine that populates the repository
with MaGe ROOT files by sending jobs to an SGE cluster. The behaviour is
similar to GNU Make, all macro files matching the pattern
`gerda-mage-sim/*/*/*/[edep|coin]/*.mac` are checked and if they have a
modification date that is newer with respect to the corresponding ROOT file (or
if the ROOT file does not exist) a `qsub` job is spawned.

**Prerequisites:**
- install Julia 1.0+ (a Julia container will not work). Downloading and
  installing the [official binaries](https://julialang.org/downloads/) (Generic
  Linux Binaries for x86 is OK). Install the JSON.jl and ProgressMeter.jl
  packages from within the REPL.
- Make sure you have access to a SGE cluster and the `qsub` command is
  available
- put a [gerda-sw-all](https://github.com/mppmu/gerda-sw-all) Singularity
  container at `UTILS/container/gerda-sw-all_active.simg` (we suggest to make
  use of symlinks)

then run:
```console
$ cd gerda-mage-sim
$ julia make.jl [--dry-run]
```
setting `JULIA_DEBUG=all` enables the debug logging mode.

## PDFs production

Analogously to the simulations production, `UTILS/post/make.jl` sends to the SGE cluster the PDF production chain.

**Additional prerequisites:**
- put a Singularity container produced with the Dockerfile in `UTILS` at
  `UTILS/container/gerda-mage-sim-utils_active.simg` (we suggest to make use of
  symlinks)
- put a ROOT file with the GERDA LAr probability map (see
  [gerda-larmap](https://github.com/gipert/gerda-larmap)) under
  `UTILS/post/gerda-larmap.root`

then run:
```console
$ cd UTILS/post
$ julia make.jl [--dry-run] --cycle <version> --destdir <gerda-pdfs-dir>
$ ./create-release
```
setting `JULIA_DEBUG=all` enables the debug logging mode.

The PDF production might be also run for individual simulations, for example,
at the MPIK cluster:
```console
$ cd UTILS
$ make
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

## `UTILS` directory content

* `det-data/`: contains (JSON) files with detectors' informations
* `container/`: deposit all `gerda-mage-sim-utils.sqsh` and `gerdasw.*.sqsh`
  containers here
    * `deploy-on-mpik.sh` script to build and deploy
      `gerda-mage-sim-utils.simg` from `Dockerfile`
* `copy/`: `rsync`-based bash scripts to copy new files to MPIK and other
  locations
* `create-macros/` : macro generation utilities
    * `create-2nbb-macros.jl`: Julia script to produce, for each detector (dead
      layer and active volume), macros for 2nbb and 2nbbLV simulations (under
      `gedet/intrinsic/`)
    * `create-lar-pplus-macros.jl`: Julia script to produce macros for each
      detector for simulations inside the LAr close to the pplus contact (under
      `gedet/lar_pplus/`) using a template macro
    * `create-oncontacts-macros.jl`: Julia script to produce macros for each
      detector for simulations on the pplus and npluscontact (under
      `gedet/nplus/` and `gedet/pplus`) using a template macro
    * `create-macros-from-template.sh`: shell script to help you generate more
      statistics
    * `multiply-statistics.sh`: brute bash script to help you generate new sets
      of macro files with different number of primaries in them if you need
      more statistics
* `readmes-tmp/`: template READMEs to be used in this repository
* `surf-sampling/`: gedet surface sampling tools
    * `create-surf-macros.jl`: Julia script to produce, for each detector,
      macro files for the surface sampling (under `gedet/surf/ver/`). It uses
      JSON.jl
    * `separate-contacts.jl`: Julia script to separate p+ and n+ contacts
      simulated surface vertices into different ROOT files (under
      `gedet/nplus/ver/` and `gedet/pplus/ver/`). It uses Cxx.jl, ROOT.jl and
      JSON.jl
    * `surf-calc.jl`: Julia script that calculates the area for p+ and n+
      contacts for each detector
*  `post/`: utilities for raw files post-processing.
    * `livetime-calc-ph2.cxx`: C++ program to extract each run's livetime and
      RunConfig. Results are stored in a JSON file that will be used by
      `t4z-gen.cxx` (see examples below)
    * `t4z-gen.cxx`: C++ program to generate a `t4z-`file for each run starting
      from a collection of `raw-`files (see examples below)
    * `t4z-gen-calib.cxx`: same as before but specific to calibration
      simulations
    * `pdf-gen.cxx`: C++ program to generate a `pdf-`file from a collection of
      `t4z-`files (see examples below)
    * `check-simulation.cxx`: C++ program to produce nice plots of decay
      vertices and interaction vertices
    * `res-curves-jsonizer.cxx`: C++ script to convert standard resolution
      curves into (Tier4izer-friendly) JSON format
    * `settings/`: JSON files for the post-processing
    * `gerda-metadata/`: (git submodule) version of
      [`gerda-metadata`](https://github.com/mppmu/gerda-metadata) used for
      post-processing
    * `old/`: old post-processing scripts
    * `make.jl`: Julia routine to launch the post-processing execution queue
    * `latex-report.jl`: Julia script to produce a nice
      report about the status of the simulations in LaTeX
* `job-scheduler/`: batch job manager utilities.
* `health-dep/`: Health department
    * `sim-doctor.cxx`: tool to check for problems in `.root` MaGe files
    * `pdf-doctor.cxx`: tool to check for problems in `t4z-*.root` and
      `pdf-*.root` files
* `Dockerfile`: recipe file to produce a Docker image that includes all the
  software needed to run the provided scripts, build with `sudo docker build
  --rm . -t gerda-mage-sim-utils`. To produce a Singularity image which is
  usable also on clusters where you don't have root permissions see
  `container/deploy-on-mpik.sh`.
* `Makefile`: GNU Makefile to compile C++ programs in this directory

### Analysis with the natural coax detectors
To produce multiplicity `M2` PDFs including the natural Coax detectors you have to:
1. Open the file
   `/lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post/settings/pdf-gen-settings-custom.json`
2. Set `"include-nat-coax-in-M2-spectra" : true`

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

### Cheatsheet
```shell
$ sed -i ... file                 # Change file directly with the `-i` flag
$ sed -i '/word/d' file           # Delete all lines containing a 'word' in 'file'
$ sed -i 's/word/repl/g' file     # Substitute all 'word' occurence with 'repl' in file
$ sed -i '10i stuff' file         # Insert 'stuff' in line number 10 in 'file'
$ rename a b *.root               # Find 'a' and replace it with 'b' in all `.root` filenames:
$ touch -d "2 hours ago" filename # change modification time to 2 hours ago
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
