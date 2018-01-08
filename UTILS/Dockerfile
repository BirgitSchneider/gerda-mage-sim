FROM gipert/rjulia:latest

USER root
COPY . /scif/UTILS/
WORKDIR /scif/UTILS
RUN make

ENV PATH="$PATH:/scif/UTILS/:/scif/UTILS/post"

WORKDIR /data
CMD /bin/zsh
