#!/usr/bin/env bash

# configure() {
#     cd /workspace
#     PATH="$PATH:$(realpath srcs/bde-tools/bin)"
#     export PATH
#     eval "$(bbs_build_env -u opt_64_cpp17)"
# }

build_ntf() {    
    local rc=0
    
    cd /workspace/ntf-core
    rc=${?}
    if [ ${rc} -ne 0 ]; then
        echo "Failed to change directory: rc = ${rc}"
        exit ${rc}
    fi

    # sed -i s/CMakeLists.txt//g ./configure
    
    ./configure --prefix /opt/bb
    rc=${?}
    if [ ${rc} -ne 0 ]; then
        echo "Failed to configure: rc = ${rc}"
        exit ${rc}
    fi

    make build
    rc=${?}
    if [ ${rc} -ne 0 ]; then
        echo "Failed to build: rc = ${rc}"
        exit ${rc}
    fi

    make build_test
    rc=${?}
    if [ ${rc} -ne 0 ]; then
        echo "Failed to build tests: rc = ${rc}"
        exit ${rc}
    fi

    make test
    rc=${?}
    if [ ${rc} -ne 0 ]; then
        cat /workspace/ntf-core/build/Testing/Temporary/LastTest.log
        echo "Failed to build tests: rc = ${rc}"
        exit ${rc}
    fi

    make install
    rc=${?}
    if [ ${rc} -ne 0 ]; then
        echo "Failed to install: rc = ${rc}"
        exit ${rc}
    fi
}

# configure
build_ntf
