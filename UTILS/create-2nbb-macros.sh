#!/bin/bash
# 
# script to write MaGe macro files and corresponding
# bsub instructions for each detector (active volume
# or dead layer) starting for generic .mac and .bsub
# template files for
#                                        2nbbLV data
#                                        2nbb   data
#
# Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
# Created: 13/03/2017
#

### 2nbbLV ###
if [ -f ./create-2nbb-macros.sh ]; then

  ### 2nbb ###
  if [ "$1" == "2nbb" ]; then 
    export FOLDER="../gedet/intrinsic/2nbb/log"

    # Coaxial
    for i in `seq 1 10` ; do
      # active volume
      export SIM="raw-gedet-intrinsic-2nbb-AV_det11_${i}"
      export DETECTOR="Crystal_geometry11_${i}"
      envsubst < "${FOLDER}/raw-gedet-intrinsic-2nbb-%VOL-%DET.tmac" > "${FOLDER}/${SIM}.mac"

      # dead layer
      export SIM="raw-gedet-intrinsic-2nbb-DV_det11_${i}"
      export DETECTOR="CrystalDeadLayer_geometry11_${i}"
      envsubst < "${FOLDER}/raw-gedet-intrinsic-2nbb-%VOL-%DET.tmac" > "${FOLDER}/${SIM}.mac"
    done

      # BEGe
    for i in `seq 1 30` ; do

      # active volume
      export SIM="raw-gedet-intrinsic-2nbb-AV_det5_${i}"
      export DETECTOR="Crystal_geometry5_${i}"
      envsubst < "${FOLDER}/raw-gedet-intrinsic-2nbb-%VOL-%DET.tmac" > "${FOLDER}/${SIM}.mac"

      # dead layer
      export SIM="raw-gedet-intrinsic-2nbb-DV_det5_${i}"
      export DETECTOR="CrystalDeadLayer_geometry5_${i}"
      envsubst < "${FOLDER}/raw-gedet-intrinsic-2nbb-%VOL-%DET.tmac" > "${FOLDER}/${SIM}.mac"

    done
  elif [ "$1" == "2nbbLV" ]; then
    export FOLDER="../gedet/intrinsic/2nbbLV/log"

    # Coaxial
    for i in `seq 1 10` ; do
      # active volume
      export SIM="raw-gedet-intrinsic-2nbbLV-AV_det11_${i}"
      export DETECTOR="Crystal_geometry11_${i}"
      envsubst < "${FOLDER}/raw-gedet-intrinsic-2nbbLV-%VOL-%DET.tmac" > "${FOLDER}/${SIM}.mac"

      # dead layer
      export SIM="raw-gedet-intrinsic-2nbbLV-DV_det11_${i}"
      export DETECTOR="CrystalDeadLayer_geometry11_${i}"
      envsubst < "${FOLDER}/raw-gedet-intrinsic-2nbbLV-%VOL-%DET.tmac" > "${FOLDER}/${SIM}.mac"
    done

    # BEGe
    for i in `seq 1 30` ; do
      # active volume
      export SIM="raw-gedet-intrinsic-2nbbLV-AV_det5_${i}"
      export DETECTOR="Crystal_geometry5_${i}"
      envsubst < "${FOLDER}/raw-gedet-intrinsic-2nbbLV-%VOL-%DET.tmac" > "${FOLDER}/${SIM}.mac"

      # dead layer
      export SIM="raw-gedet-intrinsic-2nbbLV-DV_det5_${i}"
      export DETECTOR="CrystalDeadLayer_geometry5_${i}"
      envsubst < "${FOLDER}/raw-gedet-intrinsic-2nbbLV-%VOL-%DET.tmac" > "${FOLDER}/${SIM}.mac"
    done

  ### error ###
  else
    echo "error: specify 2nbb or 2nbbLV"
  fi

else
  echo ERROR: must cd in UTILS before calling "./create-2nbb-macros.sh"!
fi

unset FOLDER
unset SIM
unset DETECTOR
