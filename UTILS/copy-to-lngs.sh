#!/bin/bash
if [ -f ./copy-to-lngs.sh ]; then
  user=`whoami`
  rsync --out-format="%o: %f%L"\
        --recursive \
        --links \
        --update \
        --times \
        --compress \
        --human-readable \
        --checksum \
        --exclude=.git* \
        --exclude=UTILS \
        --exclude=README.md \
        --dry-run \
        .. $user@gerda-login.lngs.infn.it:/nfs/gerda5/var/gerda-simulations/gerda-mc

  echo "Proceed? [y/n]"
  read ans
  if [ "$ans" == "y" ]; then
  rsync --recursive \
        --links \
        --update \
        --times \
        --compress \
        --human-readable \
        --checksum \
        --exclude=.git* \
        --exclude=UTILS \
        --exclude=README.md \
        .. $user@gerda-login.lngs.infn.it:/nfs/gerda5/var/gerda-simulations/gerda-mc
  else
    echo "Aborting..."
  fi
else
  echo ERROR: must cd in UTILS before calling "./copy-to-lngs.sh"!
fi


