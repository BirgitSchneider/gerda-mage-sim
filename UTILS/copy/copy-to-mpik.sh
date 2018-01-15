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

if [ -f ./copy-to-mpik.sh ]; then
  echo "set username for lfs2.mpi-hd.mpg.de:"; read user
  dest=/lfs/l2/gerda/Hades/gerda-mage-sim
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
        ../.. $user@lfs2.mpi-hd.mpg.de:$dest
  echo ""
  echo "This was the list of files that will be transferred."
  echo ""
  echo "NOTE: this script transfers ONLY files that do not exist on the receiver"
  echo "      yet, consider to delete the files you wish to update on the receiver."
  echo "      Modify the rsync options only if you really know what you're doing!"
  echo ""
  echo "Proceed? [y/n]"; read ans
  if [ "$ans" == "y" ]; then
    rsync $opts \
          --progress \
          ../.. $user@lfs2.mpi-hd.mpg.de:$dest
  else
    echo "Aborting..."
  fi
else
  echo ERROR: must cd in UTILS before calling "./copy-to-mpik.sh"!
fi
