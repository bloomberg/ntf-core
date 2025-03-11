FROM docker.io/ubuntu:22.04 AS builder

# Set up CA certificates first before installing other dependencies
RUN apt-get update && \
    apt-get install -y ca-certificates && \
    apt-get install -y --no-install-recommends \
    build-essential \
    gdb \
    curl \
    python3.10 \
    ninja-build \
    pkg-config \
    libssl-dev \
    liblz4-dev \
    libzstd-dev \
    libz-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# BDE build requires CMake > 3.24, which is not available in Ubuntu 22.04 yet
# Consider installation of cmake from apt once it's met BDE requirements
RUN mkdir -p /deps_build/CMake \
    && cd /deps_build/CMake \
    && curl -s -L -O https://github.com/Kitware/CMake/releases/download/v3.27.1/cmake-3.27.1-linux-x86_64.sh \
    && /bin/bash cmake-3.27.1-linux-x86_64.sh -- --skip-license --prefix=/usr/local \
    && rm -rf /deps_build

WORKDIR /workspace

ADD build_deps.sh /workspace/build_deps.sh

RUN /workspace/build_deps.sh

CMD [ "/bin/bash" ]
