# author : K.v.Sturm
# date   : 21.3.2018
# usage  : ./mpik-qsub-launch-sim.sh <mage.mac>

# description : launches jobs in the mpik job queue using Mage from container

#!/bin/bash

qsub -N $(basename $1) /lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/job-scheduler/mpik-run-mage.qsub $(pwd)/$1
