---
layout: default
---

# gerda-mage-sim

The aim of this repository is to provide a mirror (without actual data files)
of the `/lfs/l2/gerda/gerda-simulations/gerda-mage-sim` folder at the
`lfs2.mpi-hd.mpg.de` Heidelberg cluster (MPIK) containing official Monte Carlo
simulations of background contaminations for GERDA Phase II. It can be easily
browsed to check the current status of the simulations (what has been simulated
and what has not) and inspect the MaGe macro files. The repository is filled
with READMEs reporting all valuable informations for the sake of
reproducibility.

## GERDA PDFs
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

Contacts:
* Luigi Pertoldi <[pertoldi@pd.infn.it](mailto:pertoldi@pd.infn.it)>
* Katharina von Sturm <[vonsturm@pd.infn.it](mailto:vonsturm@pd.infn.it)>
