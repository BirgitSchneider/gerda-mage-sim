<img src="UTILS/gerda-logo.png" align="left"  height="80"/>

# gerda-mage-sim [![Build Status](https://travis-ci.com/mppmu/gerda-mage-sim.svg?token=wsiyLpecsT4BYkevdkq1&branch=master)](https://travis-ci.com/mppmu/gerda-mage-sim)

The aim of this repository is to provide a mirror (without actual data files)
of the `/lfs/l2/gerda/gerda-simulations/gerda-mage-sim` folder at the
`lfs2.mpi-hd.mpg.de` Heidelberg cluster (MPIK) containing official Monte Carlo
simulations of background contaminations for GERDA Phase II. It can be easily
browsed to check the current status of the simulations (what has been simulated
and what has not) and inspect the MaGe macro files. The repository is filled
with READMEs reporting all valuable informations for the sake of
reproducibility.

**The simulation you need isn't listed here?** Submit a Pull Request! See
[Contributing](https://github.com/mppmu/gerda-mage-sim/blob/master/CONTRIBUTING.md).

### GERDA PDFs
We distribute the official Probability Density Functions as a tarball that you
can download from the [releases
page](https://github.com/mppmu/gerda-mage-sim/releases). The archive is
organized like this repository: you'll find a ROOT file for each
`volume/part/isotope` simulation, each file contains a set of histograms. Refer
to [GSTR-18-009](https://www.mpi-hd.mpg.de/gerda/internal/GSTR/GSTR-18-009.pdf)
for a description of what each histogram represents. We also distribute, for
convenience, the GERDA data for which the PDFs are built, with the same
histogram format, as a separate tarball in the releases page. Find
[here](https://github.com/mppmu/gerda-bkg-model/tree/master/src/data-prod) the
software that we use to produce it.

In general you can consult
[GSTR-18-009](https://www.mpi-hd.mpg.de/gerda/internal/GSTR/GSTR-18-009.pdf)
for any further documentation (it could be outdated though).
[Here](https://github.com/mppmu/gerda-snippets/tree/master/MaGe-macros) you
find a reference macro file for MaGe.

Maintainers:
* Katharina von Sturm - [vonsturm@pd.infn.it](mailto:vonsturm@pd.infn.it)
* Luigi Pertoldi - [luigi.pertoldi@pd.infn.it](mailto:luigi.pertoldi@pd.infn.it)

**NOTE: clone with `git --recursive git@github.com:mppmu/gerda-mage-sim.git`**

### Useful links
* [Documentation](https://www.mpi-hd.mpg.de/gerda/internal/GSTR/GSTR-18-009.pdf)
* [Creating a new PDFs version](https://github.com/mppmu/gerda-mage-sim/blob/master/UTILS/README.md#pdfs-production)
* [Contributing](https://github.com/mppmu/gerda-mage-sim/blob/master/CONTRIBUTING.md)
* [`UTILS/`](https://github.com/mppmu/gerda-mage-sim/tree/master/UTILS): Command Line Tools used in the context of this repository
* [Germanium detectors sampled surfaces](https://github.com/mppmu/gerda-mage-sim/tree/master/gedet/surf_chanwise/ver)
* [MaGe source code repository](https://github.com/mppmu/MaGe)
