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

### CHANGE ME ###
#$ -t 1-10 	### submit an array of jobs SGE_TASK_ID is the running variable (can only start at 1)

LOCATION="larveto"
PART="fibers"
ISOTOPE="Bi212"
MULTIPLICITY="edep"

### CHANGE ME ###

ID="$( printf %03d $(($SGE_TASK_ID - 1)) )"

MACROPATH="./${LOCATION}/${PART}/${ISOTOPE}/${MULTIPLICITY}/log"
MACRONAME="raw-${LOCATION}-${PART}-${ISOTOPE}-${MULTIPLICITY}"
MACFILENAME="${MACROPATH}/${MACRONAME}-${ID}.mac"
LOGFILENAME="${MACROPATH}/${MACRONAME}-${ID}.out"

cd "/lfs/l2/gerda/Hades/gerda-mage-sim"
singularity run --cleanenv --app MaGe ./UTILS/container/gerdasw.g4.10.3_v2.0.sqsh ${MACFILENAME} 1> ${LOGFILENAME} 2> ${LOGFILENAME}
