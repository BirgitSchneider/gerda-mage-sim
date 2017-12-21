#!/bin/bash
#
# --ignore-existing         : skip updating files that exist on receiver
# --out-format="%o: %f%L"   : formatting the output
# --recursive               : recurse into directories
# --links                   : copy symlinks as symlinks
# --update                  : skip files that are newer on the receiver
# --times                   : preserve modification times
# --omit-dir-times          : omit directories from --times
# --compress                : compress while transferring
# --human-readable          : human-readable sizes
# --exclude=.git*           : exclude
# --exclude=UTILS           :
# --exclude=README.md       :
# --dry-run                 : do everything except actually transferring

if [ -f ./copy-decay0files-from-lngs.sh ]; then
  echo "set username for gerda-login.lngs.infn.it:"; read user
  sour=/nfs/gerda5/var/gerda-simulations/gerdasw.g4.10.3_v2.0.sqsh
  opts="--update \
       --times \
       --omit-dir-times \
       --compress \
       --human-readable"
  rsync $opts \
        --out-format="%o: %f%L" \
        --dry-run \
        $user@gerda-login.lngs.infn.it:$sour ../decay0files
  echo ""
  echo "This singularity container file will be transfered."
  echo ""
  echo "Proceed? [y/n]"; read ans
  if [ "$ans" == "y" ]; then
    mkdir -p ./container
    rsync $opts \
          --progress \
          $user@gerda-login.lngs.infn.it:$sour ./container/
  else
    echo "Aborting..."
  fi
else
  echo ERROR: must cd in UTILS before calling "./copy-singularity-container-from-lngs.sh"!
fi
