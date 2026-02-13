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

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/../../cmake/templates/ntscfg_config.h
    ${CMAKE_BINARY_DIR}/ntscfg_config.h
    @ONLY)

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/../../cmake/templates/ntscfg_config.cpp
    ${CMAKE_BINARY_DIR}/ntscfg_config.cpp
    @ONLY)

add_library(nts STATIC)

if (${NTF_BUILD_WITH_OPENSSL})
    bbs_add_target_dynamic_dependency(nts openssl)
endif()

if (${NTF_BUILD_WITH_LZ4})
    bbs_add_target_dynamic_dependency(nts liblz4)
endif()

if (${NTF_BUILD_WITH_ZSTD})
    bbs_add_target_dynamic_dependency(nts libzstd)
endif()

if (${NTF_BUILD_WITH_ZLIB})
    bbs_add_target_dynamic_dependency(nts zlib)
endif()

bbs_setup_target_uor(
    nts
    NO_EMIT_PKG_CONFIG_FILE
    PRIVATE_PACKAGES ntsd ntsu ntsb ntso)

target_include_directories(
    nts
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

target_include_directories(
    ntscfg-iface
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/ntscfg_config.h
    DESTINATION
        "include"
    COMPONENT
        nts-headers
)

if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_link_libraries(ntscfg INTERFACE -lexecinfo -lutil -ldl)
    target_link_libraries(nts PUBLIC -lexecinfo -lutil -ldl)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(ntscfg INTERFACE -ldl)
    target_link_libraries(nts PUBLIC -ldl)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    target_link_libraries(ntscfg INTERFACE -lsendfile -lsocket -lnsl -ldl)
    target_link_libraries(nts PUBLIC -lsendfile -lsocket -lnsl -ldl)
endif()

bbs_emit_pkg_config(nts)
