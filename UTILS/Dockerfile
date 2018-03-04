FROM baltig.infn.it:4567/gerda/gerdasw-containers/gerdasw:g4.10.3_v2.3

USER root

RUN yum install -y -q libedit-devel ncurses-devel \
                      openssl openssl-devel symlinks parallel && \
    yum -q clean all

WORKDIR /scif
RUN mkdir -p /scif/julia/pkgs && \
    wget -q -O- https://baltig.infn.it/gerda/baseos-binaries/raw/master/julia-0.6.2-centos7-x86_64.tar.gz \
    | tar --strip-components 1 -xz -C "/scif/julia"

ENV PATH="$PATH:/scif/julia/usr/bin" \
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/lib64:/scif/julia/usr/lib" \
    MANPATH="$MANPATH:/scif/julia/usr/share/man" \
    JULIA_HOME="/scif/julia/usr/bin"

COPY . /scif/UTILS/
WORKDIR /scif/UTILS
RUN make bin && \
    find . ! -name 'bin' ! -name '.' -type d -exec rm -rf {} + && \
    chmod -R a+x /scif/UTILS/bin

ENV PATH="$PATH:/scif/UTILS/bin"

WORKDIR /data
CMD /bin/zsh
