##################################################################
### mpik-qsub.sh: Katharina von Sturm 02.01.2018
##################################################################
### This script is meant to launch an array of jobs
###    on the mpik cluster
### Launch in a folder outside gerda-mage-sim using the command
###    'qsub mpik-qsub.sh'
###
### submit many different isotopes/parts using sed or change by hand:
###
### for i in $(find /lfs/l2/gerda/Hades/gerda-mage-sim/minishroud/ -type d -iname edep) $(find /lfs/l2/gerda/Hades/gerda-mage-sim/minishroud/ -type d -iname coin); do echo $i; done
###
##################################################################

#!/bin/bash

#$ -P short	### queue
#$ -cwd		### start job in current working directory
#$ -j y		### have logoutput and erroroutput in the same file

FOLDER=$1

cd "/lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/post"
singularity exec --cleanenv ../container/gerda-mage-sim-utils.img julia process-raw.jl ${FOLDER}
