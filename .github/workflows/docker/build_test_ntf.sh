#!/bin/bash

set -e

configure() {
    cd /workspace
    PATH="$PATH:$(realpath srcs/bde-tools/bin)"
    export PATH
    eval "$(bbs_build_env -u opt_64_cpp17)"
}

build_ntf() {    
    cd /workspace/ntf-core
    sed -i s/CMakeLists.txt//g ./configure
    ./configure --prefix /opt/bb
    make -j8
    make test | tee test.log

    if cat test.log | grep -q "Errors while running CTest"; then
        echo "There are errors while running UTs, exiting..."
        exit 1
    fi

    make install
}

configure
build_ntf
