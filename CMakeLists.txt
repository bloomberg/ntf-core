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

cmake_minimum_required(VERSION 3.22.0)

project(
    ntf-core
    VERSION
        2.5.13
    DESCRIPTION
        "NTF Core"
    HOMEPAGE_URL
        http://github.com/bloomberg/ntf-core
    LANGUAGES
        C CXX
)

include(CTest)

if (NTF_BUILD_SYSTEM)
    message(STATUS "Processing NTF workspace")
    include(${CMAKE_CURRENT_LIST_DIR}/repository.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/targets.cmake)
else()
    find_package(BdeBuildSystem)
    if (${BdeBuildSystem_FOUND})
        message(STATUS "Processing BBS workspace")
        include(${CMAKE_CURRENT_LIST_DIR}/variables.cmake)
        enable_testing()
        if (${NTF_BUILD_FROM_CONTINUOUS_INTEGRATION})
            bbs_build_tests_with_all()
        endif()
        add_subdirectory(groups)
    else()
        message(FATAL_ERROR "Build system not found")
    endif()
endif()

include(CPack)
