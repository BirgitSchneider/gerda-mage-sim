FROM gipert/rjulia:latest

USER root
COPY . /scif/UTILS/
WORKDIR /scif/UTILS
RUN g++ $(root-config --cflags --libs) -o check-simulation check-simulation.cxx && \
    g++ $(root-config --cflags --libs) -lTreePlayer -o gen-spectra gen-spectra.cxx

ENV PATH="$PATH:/scif/UTILS/:/scif/UTILS/post"

WORKDIR /data
CMD /bin/zsh
