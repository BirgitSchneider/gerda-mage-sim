FROM gipert/gerda-sw-all:gerda-optical

USER root

ENV PATH="$PATH:/opt/src/gms-utils/bin"
COPY . /opt/src/gms-utils

WORKDIR /opt/src/gms-utils
RUN make clean && make -j"$(nproc)"

WORKDIR /data
CMD /bin/bash
