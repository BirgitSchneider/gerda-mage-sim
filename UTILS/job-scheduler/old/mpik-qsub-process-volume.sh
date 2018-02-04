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
### for p in bottoms glue_rings_bottom glue_rings_top tops tubs; do
###     for i in Ac228  Bi212  Bi214  K40  Pa234m  Pb214 Tl208; do
###         cat mpik-qsub.sh | sed "s/\$1/"$p"/g" | sed "s/\$2/"$i"/g" > mpik-qsub-minishroud-$p-$i.sh; qsub mpik-qsub-minishroud-$p-$i.sh;
###     done
### done
##################################################################

#!/bin/bash

#$ -P short	### queue
#$ -cwd		### start job in current working directory
#$ -j y		### have logoutput and erroroutput in the same file

### CHANGE ME ###
#$ -t 1-10 	### submit an array of jobs SGE_TASK_ID is the running variable (can only start at 1)

LOCATION="cables"
PART="$1"
ISOTOPE="$2"
MULTIPLICITY="coin"

#LOCATION="minishroud"
#PART="tubs"
#ISOTOPE="Bi214"
#MULTIPLICITY="edep"

### CHANGE ME ###

MACROPATH="./${LOCATION}/${PART}/${ISOTOPE}/${MULTIPLICITY}/log"
MACRONAME="raw-${LOCATION}-${PART}-${ISOTOPE}-${MULTIPLICITY}"

if [[ $LOCATION == "gedet" || $LOCATION == "intrinsic" ]]; then

	ID="$( printf %d $(($SGE_TASK_ID - 1)) )"

	MACFILENAME="${MACROPATH}/${MACRONAME}-ch${ID}.mac"
	LOGFILENAME="${MACROPATH}/${MACRONAME}-ch${ID}.out"

	echo $MACFILENAME
else
	ID="$( printf %03d $(($SGE_TASK_ID - 1)) )"

	MACFILENAME="${MACROPATH}/${MACRONAME}-${ID}.mac"
	LOGFILENAME="${MACROPATH}/${MACRONAME}-${ID}.out"

	echo $MACFILENAME
fi


cd "/lfs/l2/gerda/Hades/gerda-mage-sim"
singularity run --cleanenv --app MaGe ./UTILS/container/gerdasw.g4.10.3_v2.1.sqsh ${MACFILENAME} 1> ${LOGFILENAME} 2> ${LOGFILENAME}
