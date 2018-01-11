FROM gipert/rjulia:latest

USER root
COPY . /scif/UTILS/
WORKDIR /scif/UTILS
RUN make

ENV PATH="$PATH:/scif/UTILS/:/scif/UTILS/post" \
    GERDA_DETDATA="/common/sw-other/gerda-metadata-t4z/detector-data"

# this is for HTCondor and  Docker
RUN chmod a+rwx -R /scif/julia/pkgs /scif/UTILS

WORKDIR /data
CMD /bin/zsh
