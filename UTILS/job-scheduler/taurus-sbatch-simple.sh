#!/bin/bash
##################################################################
### taurus-sbatch.sh: Thomas Wester 19.01.2018
##################################################################
### This script is meant to launch one job/macro
###    on the taurus cluster of the ZIH (Dresden)
### Launch using the command
###    'sbatch taurus-sbatch-simple.sh MACROFILE'
###
### A 'zzz_useless_logfile' will be produce in the directory from which the command is launched, which includes
### the output of slurm, this script and singularity.
### Check this if MaGe could not be launched or the job was killed, otherwise it can be savely deleted.
###
### Utilizes the wrapper script 'taurus-cdAndMaGe.sh' to cd to the correct directory within and container
### and runs MaGe.
##################################################################

#SBATCH --output=zzz_useless_logfile	###delete this logfile afterwards

### DEFAULT ###
#SBATCH --time=10:00:00		###max job duration in HH:MM:SS
#SBATCH --mem-per-cpu=1000	###max memory of the job in MB
### DEFAULT ###

if [ $# -lt 1 ]; then
  echo "Usage: sbatch taurus-sbatch-simple.sh MACRO"
  echo "To change job parameters, either change the default in the script,"
  echo "or add the corresponding option to sbatch:"
  echo " --time=HH:MM:SS"
  echo " --mem-per-cpu=MB"
  exit 0
fi

### these directories need to be changed in case the git repo on taurus is moved
GERDASIMDIR_OUTSIDE="/scratch/neutrino/GERDA/gerda-mage-sim.git"
GERDASIMDIR_INSIDE="/mnt/neutrino/GERDA/gerda-mage-sim.git"

### change this whenever a new container is available
CONTAINER="${GERDASIMDIR_OUTSIDE}/UTILS/container/gerdasw.g4.10.3_v3.0.sqsh"

RUNSCRIPT="${GERDASIMDIR_INSIDE}/UTILS/job-scheduler/taurus-cdAndMaGe.sh"

MACFILENAME=$1
LOGFILENAME=$(echo $MACFILENAME | cut -f 1 -d '.').out

srun singularity exec --cleanenv --bind /scratch:/mnt --app MaGe ${CONTAINER} ${RUNSCRIPT} ${GERDASIMDIR_INSIDE} ${MACFILENAME} ${LOGFILENAME}
