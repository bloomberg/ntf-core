#!/usr/bin/env bash

# Fetch, build, and install the BDE build system and the BDE libraries on
# Darwin.
#
# Usage: build_deps.sh <path/to/deps>
#
# The BDE build system is installed into <path/to/deps>/bde-tools and the BDE
# libraries are installed into the <path/to/deps>/bb prefix.

set -euxo pipefail

DEPS_DIR=$(mkdir -p "${1:?Usage: build_deps.sh <path/to/deps>}" && cd "${1}" && pwd)
SRCS_DIR="${DEPS_DIR}/srcs"
PREFIX="${DEPS_DIR}/bb"

fetch_git() {
    local org=$1
    local repo=$2
    local tag=$3
    local dest=$4

    mkdir -p "${SRCS_DIR}"
    rm -rf "${dest}"
    curl -SL "https://github.com/${org}/${repo}/archive/refs/tags/${tag}.tar.gz" | tar -xzC "${SRCS_DIR}"
    mv "${SRCS_DIR}/${repo}-${tag}" "${dest}"
}

fetch_deps() {
    fetch_git bloomberg bde-tools 4.31.0.0 "${DEPS_DIR}/bde-tools"
    fetch_git bloomberg bde 4.31.0.0 "${SRCS_DIR}/bde"
}

build_bde() {
    local jobs
    jobs=$(sysctl -n hw.ncpu)

    pushd "${SRCS_DIR}/bde"

    # Apple clang does not enable sized deallocation by default, which BDE
    # requires. Suppress warnings from the BDE build.
    export CXXFLAGS="-w -fsized-deallocation" CFLAGS="-w"

    PATH="${DEPS_DIR}/bde-tools/bin:${PATH}"
    export PATH
    eval "$(bbs_build_env -p clang -u opt_64_cpp20 -b "${SRCS_DIR}/bde/_build")"
    bbs_build configure --prefix="${PREFIX}"
    bbs_build build -j"${jobs}"
    bbs_build install --install_dir="/" --prefix="${PREFIX}"
    popd

    # cleanup
    rm -rf "${SRCS_DIR}"
}

fetch_deps
build_bde
