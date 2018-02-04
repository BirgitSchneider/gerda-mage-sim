# Command Line Tools
Collection of scripts used to produce macros or processing ROOT files. Run `make` to compile C++ programs.

* `det-data/`: contains (JSON) files with detectors' informations
* `readmes-tmp/`: template READMEs to be used in this repository
* `copy/`: `rsync`-based bash scripts to copy new files to MPIK and other locations
* `create-macros/` : macro generating utilities
    * `create-2nbb-macros.jl`: julia script to produce, for each detector (dead layer and active volume), macros for 2nbb and 2nbbLV simulations (under `gedet/intrinsic/`)
    * `create-lar-pplus-macros.jl`: julia script to produce macros for each detector for simulations inside the LAr close to the pplus contact (under `gedet/lar_pplus/`) using a template macro
    * `create-oncontacts-macros.jl`: julia script to produce macros for each detector for simulations on the pplus and npluscontact (under `gedet/nplus/` and `gedet/pplus`) using a template macro
    * `multiply-statistics.sh`: rude bash script to help you generate new macro files if you need more statistics
* `surf-sampling/`: gedet surface sampling tools
    * `create-surf-macros.jl`: julia script to produce, for each detector, macro files for the surface sampling (under `gedet/surf/ver/`). It uses JSON.jl
    * `separate-contacts.jl`: julia script to separate p+ and n+ contacts simulated surface vertices into different ROOT files (under `gedet/nplus/ver/` and `gedet/pplus/ver/`). It uses Cxx.jl, ROOT.jl and JSON.jl
    * `surf-calc.jl`: julia script that calculates the area for p+ and n+ contacts for each detector
*  `post/`: utilities for raw files post-processing. See the Wiki page for details on the simulations post-processing flow
    * `livetime-calc-ph2.cxx`: C++ program to extract each run's livetime and RunConfig. Results are stored in a JSON file that will be used by `t4z-gen.cxx`
    * `t4z-gen.cxx`: C++ program to generate a `t4z-`file for each run starting from a collection of `raw-`files
    * `pdf-gen.cxx`: C++ program to generate a `pdf-`file from a collection of `t4z-`files
    * `pdf-gen-volume.cxx`: C++ program to join PDFs of each single part to produce a `pdf-`file for a volume 
    * `check-simulation.cxx`: C++ program to produce nice plots of decay vertices and interaction vertices
    * `gerda-metadata/`: (git submodule) version of [`gerda-metadata`](https://github.com/mppmu/gerda-metadata) used for post-processing
    * `old/`: old post-processing scripts
* `job-scheduler/`: batch job manager utilities. `mpik-t4z-gen.qsub`, `mpik-pdf-gen.qsub` and `mpik-pdf-gen-volume.qsub` are used by the post-processing Makefile
* `ranger/`: GNU ranger file browser, very useful in the context of this repository. Define the following alias to use it: `alias='$GERDA_MAGE_SIM_LOCATION/UTILS/ranger/ranger.py'`
* `Dockerfile`: recipe file to produce a Docker image that includes all the software needed to run the provided scripts, build with `sudo docker build --rm . -t gerda-mage-sim-utils`. To produce a Singularity image which is usable also on clusters where you don't have root permissions you can use Oliver's [docker2singularity.py](https://github.com/oschulz/singularity-utils) Python script. An already processed Singularity image is available at LNGS under `/nfs/gerda5/var/gerda-simulations/gerda-mage-sim-utils.sqsh`
* `Makefile`: GNU Makefile to compile C++ programs in this directory

Usage examples:
```shell
$ cd UTILS/post
$ sudo docker run -it --rm -v /common:/common -w $PWD gerda-mage-sim-utils rjulia process-volume.jl cables Bi212 edep
$ singularity exec -B /common:/common gerda-mage-sim-utils.sqsh julia process-raw.jl ../../cables/hv_at_holders/K40/edep
```
```shell
$ cd gerda-mage-sim
$ singularity run --cleanenv --app MaGe gerdasw.g4.10.3_v2.1 cables/hv_at_holders/K40/edep/log/raw-cables-hv_at_holders-K40-edep-000.mac
```

### Useful `sed` commands to quickly change macros
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
$ setfacl -Rm user:pertoldi:rwX gerda-simulations/
$ setfacl -Rdm user:pertoldi:rwX gerda-simulations/
$ setfacl -Rm user:schuetz:rwX gerda-simulations/
$ setfacl -Rdm user:schuetz:rwX gerda-simulations/
$ setfacl -Rm user:sturm:rwX gerda-simulations/
$ setfacl -Rdm user:sturm:rwX gerda-simulations/
```

### Load preinstalled software (at MPIK)
Add the following lines to your ~/.bashrc
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
