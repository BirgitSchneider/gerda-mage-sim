#!/bin/bash
docker build . -t gerda-mage-sim-utils:latest && \
sudo docker run \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v $PWD:/output \
    --privileged -t --rm \
    singularityware/docker2singularity gerda-mage-sim-utils:latest && \
mv gerda-mage-sim-utils*.simg gerda-mage-sim-utils.simg && \
chown `whoami`:`id -gn` gerda-mage-sim-utils.simg && \
rsync --progress gerda-mage-sim-utils.simg \
    `whoami`@lfs2.mpi-hd.mpg.de:/lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/container/
