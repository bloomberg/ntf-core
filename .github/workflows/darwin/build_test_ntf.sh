#!/usr/bin/env bash

# Build, test, and install NTF on Darwin against the BDE libraries installed
# by build_deps.sh.
#
# Usage: build_test_ntf.sh <path/to/deps>
#
# This script must be run from the root of the ntf-core repository.

set -euxo pipefail

DEPS_DIR=$(cd "${1:?Usage: build_test_ntf.sh <path/to/deps>}" && pwd)
BDE_PREFIX="${DEPS_DIR}/bb"

build_ntf() {
    local jobs
    jobs=$(sysctl -n hw.ncpu)

    echo "Cores: ${jobs}"
    sw_vers
    clang --version

    unset DISTRIBUTION_REFROOT
    unset PREFIX

    ./configure --prefix "${BDE_PREFIX}" \
                --output "$(pwd)/build" \
                --jobs "${jobs}" \
                --generator Ninja \
                --standalone \
                --debug \
                --without-warnings \
                --without-warnings-as-errors \
                --from-continuous-integration

    make build
    make build_test

    if ! make test; then
        cat build/Testing/Temporary/LastTest.log
        echo "Failed to run tests successfully"
        exit 1
    fi

    make install
}

build_ntf
