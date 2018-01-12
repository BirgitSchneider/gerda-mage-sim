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
  sour=/nfs/gerda5/var/gerda-simulations/gerda-mc2/decay0files/
  opts="--ignore-existing \
       --recursive \
       --links \
       --update \
       --times \
       --omit-dir-times \
       --compress \
       --human-readable \
       --exclude-from=.rsyncignore"
  rsync $opts \
        --out-format="%o: %f%L" \
        --dry-run \
        $user@gerda-login.lngs.infn.it:$sour ../../decay0files
  echo ""
  echo "This was the list of decay0 files that will be transferred."
  echo ""
  echo "NOTE: this script transfers ONLY files that do not exist on the receiver"
  echo "      yet, consider to delete the files you wish to update on the receiver."
  echo "      Modify the rsync options only if you really know what you're doing!"
  echo ""
  echo "Proceed? [y/n]"; read ans
  if [ "$ans" == "y" ]; then
    rsync $opts \
          --progress \
          $user@gerda-login.lngs.infn.it:$sour ../../decay0files
  else
    echo "Aborting..."
  fi
else
  echo ERROR: must cd in UTILS/copy before calling "./copy-decay0files-from-lngs.sh"!
fi
