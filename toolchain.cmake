# Copyright 2020-2023 Bloomberg Finance L.P.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include_guard(GLOBAL)

set(NTF_TOOLCHAIN_VERSION_MAJOR 1)
set(NTF_TOOLCHAIN_VERSION_MINOR 0)
set(NTF_TOOLCHAIN_VERSION_PATCH 0)

message(STATUS "NTF Build Toolchain ${NTF_TOOLCHAIN_VERSION_MAJOR}.${NTF_TOOLCHAIN_VERSION_MINOR}.${NTF_TOOLCHAIN_VERSION_PATCH}")

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "")
    set(CMAKE_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME}
        CACHE STRING "Default" FORCE)
endif()

if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "")
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR}
        CACHE STRING "Default" FORCE)
endif()

if(DEFINED ENV{CC} AND DEFINED ENV{CXX})
    set(NTF_TOOLCHAIN_COMPILER_CC_PATH $ENV{CC} CACHE FILEPATH "" FORCE)
    set(NTF_TOOLCHAIN_COMPILER_CXX_PATH $ENV{CXX} CACHE FILEPATH "" FORCE)
elseif(NOT NTF_TOOLCHAIN_COMPILER_CC_PATH OR NOT NTF_TOOLCHAIN_COMPILER_CXX_PATH)
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
        find_program(
            NTF_TOOLCHAIN_COMPILER_CC_PATH gcc clang cc
            PATHS /opt/bb/bin
            REQUIRED)
        find_program(
            NTF_TOOLCHAIN_COMPILER_CXX_PATH g++ clang++ c++
            PATHS /opt/bb/bin
            REQUIRED)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD")
        find_program(
            NTF_TOOLCHAIN_COMPILER_CC_PATH gcc clang cc
            PATHS /opt/bb/bin
            REQUIRED)
        find_program(
            NTF_TOOLCHAIN_COMPILER_CXX_PATH g++ clang++ c++
            PATHS /opt/bb/bin
            REQUIRED)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        find_program(
            NTF_TOOLCHAIN_COMPILER_CC_PATH clang gcc cc
            PATHS /opt/bb/bin
            REQUIRED)
        find_program(
            NTF_TOOLCHAIN_COMPILER_CXX_PATH clang++ g++ c++
            PATHS /opt/bb/bin
            REQUIRED)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        find_program(
            NTF_TOOLCHAIN_COMPILER_CC_PATH gcc clang cc
            PATHS /opt/bb/bin
            REQUIRED)
        find_program(
            NTF_TOOLCHAIN_COMPILER_CXX_PATH g++ clang++ c++
            PATHS /opt/bb/bin
            REQUIRED)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
        find_program(
            NTF_TOOLCHAIN_COMPILER_CC_PATH cc gcc clang
            PATHS /opt/bb/bin
            REQUIRED)
        find_program(
            NTF_TOOLCHAIN_COMPILER_CXX_PATH CC g++ clang++
            PATHS /opt/bb/bin
            REQUIRED)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "AIX")
        find_program(
            NTF_TOOLCHAIN_COMPILER_CC_PATH xlc_r gcc clang
            PATHS /opt/bb/bin
            REQUIRED)
        find_program(
            NTF_TOOLCHAIN_COMPILER_CXX_PATH xlC_r g++ clang++
            PATHS /opt/bb/bin
            REQUIRED)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        find_program(NTF_TOOLCHAIN_COMPILER_CC_PATH cl clang gcc
            REQUIRED)
        find_program(NTF_TOOLCHAIN_COMPILER_CXX_PATH cl clang++ g++
            REQUIRED)
    else()
        message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    endif()
else()
    message(FATAL_ERROR "Failed to determine compiler")
endif()

get_filename_component(
    NTF_TOOLCHAIN_COMPILER_CC_NAME ${NTF_TOOLCHAIN_COMPILER_CC_PATH}
    NAME_WE CACHE)

get_filename_component(
    NTF_TOOLCHAIN_COMPILER_CXX_NAME ${NTF_TOOLCHAIN_COMPILER_CXX_PATH}
    NAME_WE CACHE)

get_filename_component(
    NTF_TOOLCHAIN_COMPILER_CC_BIN_DIR ${NTF_TOOLCHAIN_COMPILER_CC_PATH}
    DIRECTORY CACHE)

get_filename_component(
    NTF_TOOLCHAIN_COMPILER_CXX_BIN_DIR ${NTF_TOOLCHAIN_COMPILER_CXX_PATH}
    DIRECTORY CACHE)

if("${NTF_TOOLCHAIN_COMPILER_CC_NAME}" STREQUAL "gcc")
    set(NTF_TOOLCHAIN_DEBUGGER_NAME "gdb")
    find_program(NTF_TOOLCHAIN_DEBUGGER_PATH gdb REQUIRED)
elseif("${NTF_TOOLCHAIN_COMPILER_CC_NAME}" STREQUAL "clang")
    set(NTF_TOOLCHAIN_DEBUGGER_NAME "lldb")
    find_program(NTF_TOOLCHAIN_DEBUGGER_PATH lldb REQUIRED)
endif()

if (DEFINED UFID)
    set(NTF_TOOLCHAIN_UFID ${UFID} CACHE INTERNAL "")
elseif (DEFINED ENV{UFID})
    set(NTF_TOOLCHAIN_UFID $ENV{UFID} CACHE INTERNAL "")
elseif (DEFINED NTF_CONFIGURE_UFID)
    set(NTF_TOOLCHAIN_UFID ${NTF_CONFIGURE_UFID} CACHE INTERNAL "")
elseif (DEFINED ENV{NTF_CONFIGURE_UFID})
    set(NTF_TOOLCHAIN_UFID $ENV{NTF_CONFIGURE_UFID} CACHE INTERNAL "")
else()
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(NTF_TOOLCHAIN_UFID "dbg_exc_mt" CACHE INTERNAL "")
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(NTF_TOOLCHAIN_UFID "opt_exc_mt" CACHE INTERNAL "")
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        set(NTF_TOOLCHAIN_UFID "opt_dbg_exc_mt" CACHE INTERNAL "")
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
        set(NTF_TOOLCHAIN_UFID "opt_exc_mt" CACHE INTERNAL "")
    else()
        set(NTF_TOOLCHAIN_UFID "opt_dbg_exc_mt" CACHE INTERNAL "")
    endif()

    if(NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i386")
        set(NTF_TOOLCHAIN_UFID "${NTF_TOOLCHAIN_UFID}_64" CACHE INTERNAL "")
    endif()

    if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "AIX" AND
       NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
        set(NTF_TOOLCHAIN_UFID "${NTF_TOOLCHAIN_UFID}_cpp17" CACHE INTERNAL "")
    endif()
endif()

if ("${NTF_TOOLCHAIN_UFID}" MATCHES "64")
    set(NTF_TOOLCHAIN_BITNESS 64 CACHE INTERNAL "")
else()
    set(NTF_TOOLCHAIN_BITNESS 32 CACHE INTERNAL "")
endif()

if (VERBOSE)
message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_COMPILER_CC_NAME  =    ${NTF_TOOLCHAIN_COMPILER_CC_NAME}")
message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_COMPILER_CC_PATH  =    ${NTF_TOOLCHAIN_COMPILER_CC_PATH}")

message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_COMPILER_CXX_NAME =    ${NTF_TOOLCHAIN_COMPILER_CXX_NAME}")
message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_COMPILER_CXX_PATH =    ${NTF_TOOLCHAIN_COMPILER_CXX_PATH}")

message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_COMPILER_CC_BIN_DIR  = ${NTF_TOOLCHAIN_COMPILER_CC_BIN_DIR}")
message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_COMPILER_CXX_BIN_DIR = ${NTF_TOOLCHAIN_COMPILER_CXX_BIN_DIR}")

message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_DEBUGGER_NAME =        ${NTF_TOOLCHAIN_DEBUGGER_NAME}")
message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_DEBUGGER_PATH =        ${NTF_TOOLCHAIN_DEBUGGER_PATH}")

message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_UFID =                 ${NTF_TOOLCHAIN_UFID}")
message(STATUS "NTF Build Toolchain: NTF_TOOLCHAIN_BITNESS =              ${NTF_TOOLCHAIN_BITNESS}")

endif()

include(CMakeInitializeConfigs)

set(CMAKE_C_COMPILER ${NTF_TOOLCHAIN_COMPILER_CC_PATH} CACHE STRING "Default" FORCE)
set(CMAKE_CXX_COMPILER ${NTF_TOOLCHAIN_COMPILER_CXX_PATH} CACHE STRING "Default" FORCE)

if (VERBOSE)
message(STATUS "NTF Build Toolchain: CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "NTF Build Toolchain: CMAKE_HOST_SYSTEM_PROCESSOR = ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "NTF Build Toolchain: CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "NTF Build Toolchain: CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "NTF Build Toolchain: CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

message(STATUS "NTF Build Toolchain: CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")

message(STATUS "NTF Build Toolchain: CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message(STATUS "NTF Build Toolchain: CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "NTF Build Toolchain: CMAKE_CXX_FLAGS_DEBUG_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}")

message(STATUS "NTF Build Toolchain: CMAKE_CXX_FLAGS_INIT = ${CMAKE_CXX_FLAGS_INIT}")
message(STATUS "NTF Build Toolchain: CMAKE_CXX_FLAGS_DEBUG_INIT = ${CMAKE_CXX_FLAGS_DEBUG_INIT}")
message(STATUS "NTF Build Toolchain: CMAKE_CXX_FLAGS_DEBUG_RELEASE_INIT = ${CMAKE_CXX_FLAGS_RELEASE_INIT}")

message(STATUS "NTF Build Toolchain: DEFAULT_CXX_FLAGS = ${DEFAULT_CXX_FLAGS}")
message(STATUS "NTF Build Toolchain: DEFAULT_CXX_FLAGS_DEBUG = ${DEFAULT_CXX_FLAGS_DEBUG}")
message(STATUS "NTF Build Toolchain: DEFAULT_CXX_FLAGS_DEBUG_RELEASE = ${DEFAULT_CXX_FLAGS_RELEASE}")

message(STATUS "NTF Build Toolchain: DEFAULT_CXX_FLAGS_INIT = ${DEFAULT_CXX_FLAGS_INIT}")
message(STATUS "NTF Build Toolchain: DEFAULT_CXX_FLAGS_DEBUG_INIT = ${DEFAULT_CXX_FLAGS_DEBUG_INIT}")
message(STATUS "NTF Build Toolchain: DEFAULT_CXX_FLAGS_DEBUG_RELEASE_INIT = ${DEFAULT_CXX_FLAGS_RELEASE_INIT}")
endif()

set(CMAKE_C_FLAGS_INIT                  "" CACHE STRING "Default"        FORCE)
set(CMAKE_C_FLAGS_DEBUG_INIT            "" CACHE STRING "Debug"          FORCE)
set(CMAKE_C_FLAGS_RELEASE_INIT          "" CACHE STRING "Release"        FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL_INIT       "" CACHE STRING "MinSizeRel"     FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT   "" CACHE STRING "RelWithDebInfo" FORCE)

set(CMAKE_CXX_FLAGS_INIT                "" CACHE STRING "Default"        FORCE)
set(CMAKE_CXX_FLAGS_DEBUG_INIT          "" CACHE STRING "Debug"          FORCE)
set(CMAKE_CXX_FLAGS_RELEASE_INIT        "" CACHE STRING "Release"        FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "" CACHE STRING "MinSizeRel"     FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "" CACHE STRING "RelWithDebInfo" FORCE)

set(CMAKE_C_FLAGS           "$ENV{CFLAGS}" CACHE STRING "Default"        FORCE)
set(CMAKE_C_FLAGS_DEBUG                 "" CACHE STRING "Debug"          FORCE)
set(CMAKE_C_FLAGS_RELEASE               "" CACHE STRING "Release"        FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL            "" CACHE STRING "MinSizeRel"     FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO        "" CACHE STRING "RelWithDebInfo" FORCE)

set(CMAKE_CXX_FLAGS       "$ENV{CXXFLAGS}" CACHE STRING "Default"        FORCE)
set(CMAKE_CXX_FLAGS_DEBUG               "" CACHE STRING "Debug"          FORCE)
set(CMAKE_CXX_FLAGS_RELEASE             "" CACHE STRING "Release"        FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL          "" CACHE STRING "MinSizeRel"     FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO      "" CACHE STRING "RelWithDebInfo" FORCE)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)


if ("${NTF_TOOLCHAIN_COMPILER_CXX_NAME}" STREQUAL "xlC_r")

if (VERBOSE)
message(STATUS "NTF Toolchain: Overriding standard compile/archive/link commands")
endif()

set(CMAKE_CXX_STANDARD_DEFAULT "" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD "17" CACHE STRING "" FORCE)

set(CMAKE_CXX_ARCHIVE_CREATE
    "<CMAKE_AR> -X${NTF_TOOLCHAIN_BITNESS} cr <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_APPEND
    "<CMAKE_AR> -X${NTF_TOOLCHAIN_BITNESS} r <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_FINISH
    "<CMAKE_RANLIB> -X${NTF_TOOLCHAIN_BITNESS} <TARGET> <LINK_FLAGS>")

set(CMAKE_C_ARCHIVE_CREATE
    "<CMAKE_AR> -X${NTF_TOOLCHAIN_BITNESS} cr <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND
    "<CMAKE_AR> -X${NTF_TOOLCHAIN_BITNESS} r <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH
    "<CMAKE_RANLIB> -X${NTF_TOOLCHAIN_BITNESS} <TARGET> <LINK_FLAGS>")

set(CMAKE_CXX_CREATE_SHARED_LIBRARY
    "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <LINK_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")

set(CMAKE_C_CREATE_SHARED_LIBRARY
    "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_C_FLAGS> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS> <LINK_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
endif()

