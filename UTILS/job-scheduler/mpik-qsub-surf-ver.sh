##################################################################
### mpik-qsub.sh: Katharina von Sturm 02.01.2018
##################################################################
### This script is meant to launch an array of jobs
###    on the mpik cluster
### Launch in a folder outside gerda-mage-sim using the command
###    'qsub mpik-qsub.sh'
##################################################################

#!/bin/bash

#$ -P short	### queue
#$ -cwd		### start job in current working directory
#$ -j y		### have logoutput and erroroutput in the same file
#$ -t 1-40 	### submit an array of jobs SGE_TASK_ID is the running variable (can only start at 1)

LOCATION="gedet"
PART="surf"
MULTIPLICITY="ver"

MACROPATH="./${LOCATION}/${PART}/${MULTIPLICITY}/log"
MACRONAME="ver-${LOCATION}-${PART}"

ID="$( printf %d $(($SGE_TASK_ID - 1)) )"

MACFILENAME="${MACROPATH}/${MACRONAME}-ch${ID}.mac"
LOGFILENAME="${MACROPATH}/${MACRONAME}-ch${ID}.out"

echo $MACFILENAME

cd "/lfs/l2/gerda/gerda-simulations/gerda-mage-sim"
singularity run --cleanenv --app MaGe ./UTILS/container/gerdasw.g4.10.3_v2.1.sqsh ${MACFILENAME} 1> ${LOGFILENAME} 2> ${LOGFILENAME}
