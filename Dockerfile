FROM debian:trixie-slim

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get upgrade -y

RUN apt-get install -y --no-install-recommends \
    awk sed wget \
    ca-certificates \
    rsync \
    devscripts \
    jq \
    vim \
    zsh \
    sudo \
    gdb \
    build-essential \
    make \
    astyle  \
    indent \
    doxygen \
    valgrind \
    gcc \
    clang \
    clang-tidy \
    clangd \
    python3 \
    python3-typer \
    python3-matplotlib \
    git

## Get UVolante stuff (pyc-objdump)
# RUN wget https://www.uvolante.org/apt/uvolante.sources -O /etc/apt/sources.list.d/uvolante.sources
# RUN apt-get update && apt-get install -y --no-install-recommends pyc-objdump

CMD ["/bin/bash"]
