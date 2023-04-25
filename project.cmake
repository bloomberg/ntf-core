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

include(legacy/wafstyleout)
include(layers/package_libs)
include(layers/ufid)
include(layers/install_pkg_config)
include(layers/install_cmake_config)

include(${CMAKE_CURRENT_LIST_DIR}/variables.cmake)

bde_prefixed_override(bdeproj project_process_uors)
function(bdeproj_project_process_uors proj listDir)
    bde_assert_no_extra_args()

    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        add_compile_options("-fno-omit-frame-pointer")
    endif()

    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        add_compile_options("-fno-omit-frame-pointer")
        add_compile_options("-Werror=return-type")
    endif()

    configure_file(
        ${listDir}/cmake/templates/ntscfg_config.h
        ${CMAKE_BINARY_DIR}/groups/nts/ntscfg/ntscfg_config.h
        @ONLY
    )

    configure_file(
        ${listDir}/cmake/templates/ntscfg_config.cpp
        ${CMAKE_BINARY_DIR}/groups/nts/ntscfg/ntscfg_config.cpp
        @ONLY
    )

    configure_file(
        ${listDir}/cmake/templates/ntccfg_config.h
        ${CMAKE_BINARY_DIR}/groups/ntc/ntccfg/ntccfg_config.h
        @ONLY
    )

    configure_file(
        ${listDir}/cmake/templates/ntccfg_config.cpp
        ${CMAKE_BINARY_DIR}/groups/ntc/ntccfg/ntccfg_config.cpp
        @ONLY
    )

    if (NOT ${NTF_BUILD_WITH_WARNINGS})
        if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            add_compile_options(/W0)
        else()
            add_compile_options(-w)
        endif()
    else()
        if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
            add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
            add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
            add_compile_options(/W3)
            if (${NTF_BUILD_WITH_WARNINGS_AS_ERRORS})
                add_compile_options(/WX)
            endif()
            if (MSVC_VERSION GREATER 1900)
                add_compile_options(/experimental:external /external:I${NTF_BUILD_REFROOT}/${NTF_BUILD_PREFIX}/include /external:W0)
            endif()
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            add_compile_options(-Wall -Wextra -Wpedantic)

            # Treating warnings as errors is only supported for GCC versions >=
            # GCC 7.0. The codebase is not known to compile without warnings on
            # earlier versions of GCC.
            if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7.0)
                if (${NTF_BUILD_WITH_WARNINGS_AS_ERRORS})
                    add_compile_options(-Werror)
                endif()
            endif()
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            add_compile_options(-Wall -Wextra -Wpedantic)

            if (${NTF_BUILD_WITH_WARNINGS_AS_ERRORS})
                add_compile_options(-Werror)
            endif()
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
            add_compile_options(-errtags)

            if (${NTF_BUILD_WITH_WARNINGS_AS_ERRORS})
                add_compile_options(-errwarn=%all)
            endif()
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "XL")
            # It is not currently possible to support warnings-as-errors using
            # xlc because the mechanism, if any, to hide warnings from external
            # headers is not known.

            # add_compile_options(-qinfo=all)

            # if (${NTF_BUILD_WITH_WARNINGS_AS_ERRORS})
            #     add_compile_options(-qhalt=w)
            # endif()
        endif()
    endif()

    if (${NTF_BUILD_WITH_COVERAGE})
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
           CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            add_compile_options(--coverage)
            add_link_options(--coverage)
        endif()
    endif()

    if (${NTF_BUILD_WITH_TIME_TRACE})
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            add_compile_options(-ftime-trace)
        endif()
    endif()

    if (${NTF_BUILD_WITH_TIME_REPORT})
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            add_compile_options(-ftime-report)
        endif()
    endif()

    if (${NTF_BUILD_WITH_NTS})
        bde_project_process_package_groups(
            ${proj}
            ${listDir}/groups/nts
        )

        if (${NTS_BUILD_WITH_USAGE_EXAMPLES})
            bde_project_process_applications(
                ${proj}
                ${listDir}/examples/m_ntsu01
                ${listDir}/examples/m_ntsu02
                ${listDir}/examples/m_ntsu03
                ${listDir}/examples/m_ntsu04
                ${listDir}/examples/m_ntsu05
                ${listDir}/examples/m_ntsu06
                ${listDir}/examples/m_ntsu07
                ${listDir}/examples/m_ntsu08
            )
        endif()
    endif()

    if (${NTF_BUILD_WITH_NTC})
        bde_project_process_package_groups(
            ${proj}
            ${listDir}/groups/ntc
        )

        if (${NTF_BUILD_WITH_USAGE_EXAMPLES})
            bde_project_process_applications(
                ${proj}
                ${listDir}/examples/m_ntcu01
                ${listDir}/examples/m_ntcu02
                ${listDir}/examples/m_ntcu03
                ${listDir}/examples/m_ntcu04
                ${listDir}/examples/m_ntcu05
                ${listDir}/examples/m_ntcu06
                ${listDir}/examples/m_ntcu07
                ${listDir}/examples/m_ntcu08
                ${listDir}/examples/m_ntcu09
                ${listDir}/examples/m_ntcu10
                ${listDir}/examples/m_ntcu11
                ${listDir}/examples/m_ntcu12
                ${listDir}/examples/m_ntcu13
                ${listDir}/examples/m_ntcu14
            )
        endif()
    endif()

endfunction()
