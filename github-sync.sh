#!/bin/bash

while true; do
  cd /nfs/gerda5/var/gerda-simulations/gerda-mc2
  if git diff --quiet; then
    sleep 86400
  else
    echo "Changes found, committing and pushing to github.com..."
    git pull --quiet
    git add .
    git commit --quiet -m "update: $(date)"
    git push --quiet
  fi
done
