#!/bin/bash
##################################################################
### taurus-sbatch.sh: Thomas Wester 19.01.2018
##################################################################
### This script is meant to launch an array of jobs
###    on the taurus cluster of the ZIH (Dresden)
### Launch using the command
###    'sbatch taurus-sbatch.sh LOCATION PART ISOTOPE MULTIPLCITY'
### e.g. sbatch taurus-sbatch.sh larveto coppershroud Bi214 coin
###
### A 'zzz_useless_logfile' will be produce in the directory from which the command is launched, which includes
### the output of slurm, this script and singularity.
### Check this if MaGe could not be launched or the job was killed, otherwise it can be savely deleted.
###
### Utilizes the wrapper script 'taurus-cdAndMaGe.sh' to cd to the correct directory within and container
### and runs MaGe.
##################################################################

#SBATCH --output=zzz_useless_logfile	###delete this logfile afterwards

### CHANGE ME ###
#SBATCH --time=10:00:00		###max job duration in HH:MM:SS
#SBATCH --mem-per-cpu=1000	###max memory of the job in MB
#SBATCH --array=0-1		###submit an array of jobs SLURM_ARRAY_TASK_ID is the running variable
### CHANGE ME ###

if [ $# -lt 4 ]; then
  echo "Usage: sbatch taurus-sbatch.sh LOCATION PART ISOTOPE MULTIPLCITY\n"
  echo "e.g. sbatch taurus-sbatch.sh larveto coppershroud Bi214 coin\n"
  exit 0
fi

LOCATION=$1
PART=$2
ISOTOPE=$3
MULTIPLICITY=$4

### or just hardcode this stuff
#LOCATION="larveto"
#PART="coppershroud"
#ISOTOPE="Bi214"
#MULTIPLICITY="coin"


### these directories need to be changed in case the git repo on taurus is moved
GERDASIMDIR_OUTSIDE="/scratch/neutrino/GERDA/gerda-mage-sim.git"
GERDASIMDIR_INSIDE="/mnt/neutrino/GERDA/gerda-mage-sim.git"

### change this whenever a new container is available
CONTAINER="${GERDASIMDIR_OUTSIDE}/UTILS/container/gerdasw.g4.10.3_v2.1.sqsh"

RUNSCRIPT="${GERDASIMDIR_INSIDE}/UTILS/job-scheduler/taurus-cdAndMaGe.sh"


MACROPATH="./${LOCATION}/${PART}/${ISOTOPE}/${MULTIPLICITY}/log"
MACRONAME="raw-${LOCATION}-${PART}-${ISOTOPE}-${MULTIPLICITY}"

if [[ $LOCATION == "gedet" || $LOCATION == "intrinsic" ]]; then

	ID="$( printf %d $SLURM_ARRAY_TASK_ID )"

	MACFILENAME="${MACROPATH}/${MACRONAME}-ch${ID}.mac"
	LOGFILENAME="${MACROPATH}/${MACRONAME}-ch${ID}.out"

	echo $MACFILENAME
else
	ID="$( printf %03d $SLURM_ARRAY_TASK_ID )"

	MACFILENAME="${MACROPATH}/${MACRONAME}-${ID}.mac"
	LOGFILENAME="${MACROPATH}/${MACRONAME}-${ID}.out"

	echo $MACFILENAME
fi

srun singularity exec --cleanenv --bind /scratch:/mnt --app MaGe ${CONTAINER} ${RUNSCRIPT} ${GERDASIMDIR_INSIDE} ${MACFILENAME} ${LOGFILENAME}
