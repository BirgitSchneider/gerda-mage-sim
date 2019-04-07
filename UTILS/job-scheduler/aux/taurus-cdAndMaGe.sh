#!/bin/bash
#
# taurus-cdAndMaGe.sh
#
if [ $# -ne 3 ]; then
  echo "Wrapper for MaGe that cds in a given directory before executing."
  echo "USAGE: taurus-cdAndMaGe.sh [DIR] [MACROFILE] [LOGFILE]"
else
  cd $1
  MaGe $2 > ${3} 2>&1
fi

