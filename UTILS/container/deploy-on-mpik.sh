#!/bin/bash
docker build . -t gerda-mage-sim-utils:latest && \
/common/sw-other/singularity-utils/bin/docker2singularity.py gerda-mage-sim-utils:latest container/gerda-mage-sim-utils.sqsh && \
scp container/gerda-mage-sim-utils.sqsh mpik:/lfs/l2/gerda/gerda-simulations/gerda-mage-sim/UTILS/container/ && \
docker tag gerda-mage-sim-utils:latest baltig.infn.it:4567/gerda/gerdasw-containers/gerda-mage-sim-utils:latest && \
docker push baltig.infn.it:4567/gerda/gerdasw-containers/gerda-mage-sim-utils:latest
