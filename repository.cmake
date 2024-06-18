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

include(CMakePackageConfigHelpers)

# Log a message.
function (ntf_log message)
    message(STATUS ${message})
endfunction()

# Log a message and abort with a stack trace.
function (ntf_die message)
    message(FATAL_ERROR ${message})
endfunction()

# Abort if a variable value is not defined.
function (ntf_assert_defined)
    list(POP_FRONT ARGN value)
    if ("${value}" STREQUAL "")
        ntf_die("The value is not defined")
    endif()
endfunction()

# Abort if a target is not defined.
function (ntf_assert_target_defined target)
    if(NOT TARGET ${target})
        ntf_die("There is no target named '${target}'")
    endif()
endfunction()

# Return the case-appropriate name of a target.
#
# TARGET - The target.
# OUTPUT - The variable name set in the parent scope.
function (ntf_nomenclature_target)
    cmake_parse_arguments(
        ARG "" "TARGET;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_OUTPUT})

    # string(TOLOWER ${ARG_TARGET} target_lowercase)
    # set(${ARG_OUTPUT} ${target_lowercase} PARENT_SCOPE)

    set(${ARG_OUTPUT} ${ARG_TARGET} PARENT_SCOPE)
endfunction()

# Return the case-appropriate name of a variable.
#
# VARIABLE - The variable.
# OUTPUT   - The variable name set in the parent scope.
function (ntf_nomenclature_variable)
    cmake_parse_arguments(
        ARG "" "VARIABLE;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_VARIABLE})
    ntf_assert_defined(${ARG_OUTPUT})

    string(TOUPPER ${ARG_VARIABLE} variable_uppercase)
    set(${ARG_OUTPUT} NTF_${variable_uppercase} PARENT_SCOPE)
endfunction()

# Set a variable scoped to a target.
#
# TARGET   - The target.
# VARIABLE - The variable name.
# VALUE    - The variable value.
function (ntf_target_variable_set)
    cmake_parse_arguments(
        ARG "" "TARGET;VARIABLE" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_VARIABLE})
    # ntf_assert_defined(${ARG_VALUE})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)
    ntf_nomenclature_variable(VARIABLE ${ARG_VARIABLE} OUTPUT variable)

    ntf_assert_target_defined(${target})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        set_property(TARGET ${target} PROPERTY ${variable} ${ARG_VALUE})
    else()
        set_property(TARGET ${target} PROPERTY ${variable} "")
    endif()
endfunction()


# Append to a variable scoped to a target.
#
# TARGET   - The target.
# VARIABLE - The variable name.
# VALUE    - The variable value.
function (ntf_target_variable_append)
    cmake_parse_arguments(
        ARG "" "TARGET;VARIABLE" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_VARIABLE})
    # ntf_assert_defined(${ARG_VALUE})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)
    ntf_nomenclature_variable(VARIABLE ${ARG_VARIABLE} OUTPUT variable)

    ntf_assert_target_defined(${target})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        get_property(result TARGET ${target} PROPERTY ${variable})
        list(APPEND result ${ARG_VALUE})
        set_property(TARGET ${target} PROPERTY ${variable} ${result})
    endif()
endfunction()

# Get a variable scoped to a target.
#
# TARGET   - The target.
# VARIABLE - The variable name.
# OUTPUT   - The variable name set in the parent scope.
function (ntf_target_variable_get)
    cmake_parse_arguments(
        ARG "" "TARGET;VARIABLE;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_VARIABLE})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)
    ntf_nomenclature_variable(VARIABLE ${ARG_VARIABLE} OUTPUT variable)

    ntf_assert_target_defined(${target})

    get_property(result TARGET ${target} PROPERTY ${variable})
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "description" variable scoped to a target.
#
# TARGET - The target.
# VALUE  - The variable value.
function (ntf_target_description_set)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "description" VALUE ${ARG_VALUE})
    else()
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "description" VALUE "")
    endif()
endfunction()

# Get the "description" variable scoped to a target.
#
# TARGET - The target.
# OUTPUT - The variable name set in the parent scope.
function (ntf_target_description_get)
    cmake_parse_arguments(
        ARG "" "TARGET;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_target_variable_get(
        TARGET ${ARG_TARGET} VARIABLE "description" OUTPUT result)

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "path" variable scoped to a target.
#
# TARGET - The target.
# VALUE  - The variable value.
function (ntf_target_path_set)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "path" VALUE ${ARG_VALUE})
    else()
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "path" VALUE "")
    endif()
endfunction()

# Get the "path" variable scoped to a target.
#
# TARGET - The target.
# OUTPUT - The variable name set in the parent scope.
function (ntf_target_path_get)
    cmake_parse_arguments(
        ARG "" "TARGET;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_target_variable_get(
        TARGET ${ARG_TARGET} VARIABLE "path" OUTPUT result)

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "private" variable scoped to a target.
#
# TARGET - The target.
# VALUE  - The variable value.
function (ntf_target_private_set)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "private" VALUE ${ARG_VALUE})
    else()
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "private" VALUE "")
    endif()
endfunction()

# Get the "private" variable scoped to a target.
#
# TARGET - The target.
# OUTPUT - The variable name set in the parent scope.
function (ntf_target_private_get)
    cmake_parse_arguments(
        ARG "" "TARGET;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_target_variable_get(
        TARGET ${ARG_TARGET} VARIABLE "private" OUTPUT result)

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "thirdparty" variable scoped to a target.
#
# TARGET - The target.
# VALUE  - The variable value.
function (ntf_target_thirdparty_set)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "thirdparty" VALUE ${ARG_VALUE})
    else()
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "thirdparty" VALUE "")
    endif()
endfunction()

# Get the "thirdparty" variable scoped to a target.
#
# TARGET - The target.
# OUTPUT - The variable name set in the parent scope.
function (ntf_target_thirdparty_get)
    cmake_parse_arguments(
        ARG "" "TARGET;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_target_variable_get(
        TARGET ${ARG_TARGET} VARIABLE "thirdparty" OUTPUT result)

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "requires" variable scoped to a target.
#
# TARGET - The target.
# VALUE  - The variable value.
function (ntf_target_requires_set)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "requires" VALUE ${ARG_VALUE})
    else()
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "requires" VALUE "")
    endif()
endfunction()

# Append to the "requires" variable scoped to a target.
#
# TARGET - The target.
# VALUE  - The variable value.
function (ntf_target_requires_append)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_target_variable_append(
            TARGET ${ARG_TARGET} VARIABLE "requires" VALUE ${ARG_VALUE})
    endif()
endfunction()

# Get the "requires" variable scoped to a target.
#
# TARGET - The target.
# OUTPUT - The variable name set in the parent scope.
function (ntf_target_requires_get)
    cmake_parse_arguments(
        ARG "" "TARGET;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_target_variable_get(
        TARGET ${ARG_TARGET} VARIABLE "requires" OUTPUT result)

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "libs" variable scoped to a target. The contents of this variable
# is assigned to the "Libs.private" variable in pkg-config metadata.
#
# TARGET - The target.
# VALUE  - The variable value.
function (ntf_target_libs_set)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "libs" VALUE ${ARG_VALUE})
    else()
        ntf_target_variable_set(
            TARGET ${ARG_TARGET} VARIABLE "libs" VALUE "")
    endif()
endfunction()

# Append to the "libs" variable scoped to a target. The contents of this
# variable is assigned to the "Libs.private" variable in pkg-config metadata.
#
# TARGET - The target.
# VALUE  - The variable value.
function (ntf_target_libs_append)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_target_variable_append(
            TARGET ${ARG_TARGET} VARIABLE "libs" VALUE ${ARG_VALUE})
    endif()
endfunction()

# Get the "libs" variable scoped to a target. The contents of this variable is
# assigned to the "Libs.private" variable in pkg-config metadata.
#
# TARGET - The target.
# OUTPUT - The variable name set in the parent scope.
function (ntf_target_libs_get)
    cmake_parse_arguments(
        ARG "" "TARGET;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_target_variable_get(
        TARGET ${ARG_TARGET} VARIABLE "libs" OUTPUT result)

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()


# Add preprocessor defininitions when compiling a target. The definitions
# are always added with private visibility.
#
# TARGET - The target.
# VALUE  - The list of preprocessor definitions
function (ntf_target_build_definition)
    cmake_parse_arguments(
        ARG "" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_VALUE})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)

    foreach(value ${ARG_VALUE})
        target_compile_definitions(${target} PRIVATE ${value})
    endforeach()

endfunction()

# Add options when compiling, and optionally, linking, a target. The options
# are always added with private visibility.
#
# TARGET       - The target.
# VALUE        - The list of compiler and/or linker options.
# COMPILE      - Include the option when compiling.
# COMPILE_ONLY - Include the option only when compiling
# LINK         - Include the option when linking.
# LINK_ONLY    - Include the option only when linking.
function (ntf_target_build_option)
    cmake_parse_arguments(
        ARG "COMPILE;COMPILE_ONLY;LINK;LINK_ONLY" "TARGET" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_VALUE})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)

    if (${ARG_COMPILE} AND NOT ${ARG_LINK_ONLY})
        set(include_when_compiling TRUE)
    else()
        set(include_when_compiling FALSE)
    endif()

    if (${ARG_LINK} AND NOT ${ARG_COMPILE_ONLY})
        set(include_when_linking TRUE)
    else()
        set(include_when_linking FALSE)
    endif()

    foreach(value ${ARG_VALUE})
        if (${include_when_compiling})
            target_compile_options(${target} PRIVATE ${value})
        endif()

        if (${include_when_linking})
            target_link_options(${target} PRIVATE ${value})
        endif()
    endforeach()

endfunction()


# Set the compiler and linker options for a target common to all UFIDs.
function (ntf_target_options_common_prolog target)
    ntf_repository_build_ufid_get(OUTPUT ufid)
    ntf_ufid_string_has(UFID ${ufid} FLAG "opt" OUTPUT is_opt)
    ntf_ufid_string_has(UFID ${ufid} FLAG "dbg" OUTPUT is_dbg)
    ntf_ufid_string_has(UFID ${ufid} FLAG "64"  OUTPUT is_64_bit)
    ntf_ufid_string_has(UFID ${ufid} FLAG "static" OUTPUT is_static)

    ntf_repository_toolchain_link_static_get(OUTPUT link_static)

    set_property(TARGET ${target} PROPERTY COMPILE_DEFINITIONS "")
    set_property(TARGET ${target} PROPERTY COMPILE_OPTIONS "")

    ntf_repository_standard_get(OUTPUT cxx_standard)
    if ("${cxx_standard}" STREQUAL "98")
        ntf_target_build_definition(
            TARGET ${target} VALUE BDE_BUILD_TARGET_CPP03)
    else()
        ntf_target_build_definition(
            TARGET ${target} VALUE BDE_BUILD_TARGET_CPP${cxx_standard})
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|SunPro|XL")
        if("${CMAKE_SYSTEM_NAME}" STREQUAL "AIX")
            # TODO
        elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
            ntf_target_build_definition(TARGET ${target} VALUE _DARWIN_C_SOURCE _BSD_SOURCE)
        elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
            ntf_target_build_definition(TARGET ${target} VALUE _BSD_SOURCE)
        elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
            ntf_target_build_definition(TARGET ${target} VALUE _GNU_SOURCE)
        elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD")
            ntf_target_build_definition(TARGET ${target} VALUE _BSD_SOURCE)
        elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
            ntf_target_build_definition(TARGET ${target} VALUE _GNU_SOURCE)
        elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
            # TODO
        else()
            # TODO
        endif()

        if (${is_dbg} AND NOT ${is_opt})
            ntf_target_build_definition(TARGET ${target} VALUE _DEBUG)
        endif()

        ntf_target_build_definition(
            TARGET
                ${target}
            VALUE
                _REENTRANT _THREAD_SAFE _POSIX_PTHREAD_SEMANTICS)

        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            ntf_target_build_definition(
                TARGET
                    ${target}
                VALUE
                    _FILE_OFFSET_BITS=64)
        endif()

        if (CMAKE_CXX_COMPILER_ID MATCHES "XL")
            ntf_target_build_definition(
                TARGET ${target} VALUE __NOLOCK_ON_INPUT __NOLOCK_ON_OUTPUT)
        endif()

        if (NOT ${is_dbg} OR ${is_opt})
            ntf_target_build_definition(TARGET ${target} VALUE NDEBUG)
        endif()

        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            if (${is_opt} AND ${is_dbg})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -O2 -g2)
            elseif (${is_opt})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -O2 -g0)
            elseif (${is_dbg})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -O0 -g2)
            endif()

            if (${is_64_bit})
                if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
                    ntf_target_build_option(
                        TARGET ${target} COMPILE LINK VALUE -m64)
                    ntf_target_build_option(
                        TARGET ${target} COMPILE LINK VALUE -march=westmere)
                endif()
            else()
                if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
                    ntf_target_build_option(
                        TARGET ${target} COMPILE LINK VALUE -m32)
                    ntf_target_build_option(
                        TARGET ${target} COMPILE LINK VALUE -march=westmere)
                    ntf_target_build_option(
                        TARGET ${target} COMPILE LINK VALUE -mstackrealign)
                    ntf_target_build_option(
                        TARGET ${target} COMPILE LINK VALUE -mfpmath=sse)
                endif()
            endif()

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -pipe)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -pthread)

            if (${is_static} OR ${link_static})
                # TODO: static PIE, where supported.
            else()
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -fPIC)
            endif()

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -fexceptions)

            if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                if(DEFINED CMAKE_CXX_COMPILER_VERSION
                    AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16.0)
                    ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE
                        -fcoroutines-ts)
                endif()
            else() #GCC
                ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE
                        -fcoroutines)
            endif()

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -fno-strict-aliasing)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -fno-omit-frame-pointer)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -fdiagnostics-show-option)

        elseif (CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
            if (${is_opt} AND ${is_dbg})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -xO2 -g -xs=no)
            elseif (${is_opt})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -xO2)
            elseif (${is_dbg})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -xO0 -g -xs=no)
            endif()

            if (${is_64_bit})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -m64)
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -xtarget=generic)
            else()
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -m32)
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -xtarget=generic)
            endif()

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -mt)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -temp=/bb/data/tmp)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -features=except)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -features=rtti)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -xcode=pic32)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -xbuiltin=%all)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -xannotate=no)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -xmemalign=8i)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -xthreadvar=dynamic)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -Qoption ccfe)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -xglobalstatic)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -errtags=yes)

        elseif (CMAKE_CXX_COMPILER_ID MATCHES "XL")
            if (${is_opt} AND ${is_dbg})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -O -g)
            elseif (${is_opt})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -O)
            elseif (${is_dbg})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -g)
            endif()

            if (${is_64_bit})
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -q64)
            else()
                ntf_target_build_option(
                    TARGET ${target} COMPILE LINK VALUE -q32)
            endif()

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qpic)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qeh)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qthreaded)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qtls)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qinline)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qfuncsect)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qrtti=all)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qalias=noansi)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qtbtable=small)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qnotempinc)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qlanglvl=staticstoreoverlinkage)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qlanglvl=newexcp)

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qarch=pwr6)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qtune=pwr7)

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -bmaxdata:0x50000000)

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qsuppress=1500-029)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qsuppress=1500-030)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qsuppress=1501-201)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qsuppress=1540-1281)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qsuppress=1540-2910)

            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qxflag=dircache:71,100)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qxflag=NoKeepDebugMetaTemplateType)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qxflag=tocrel)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qxflag=FunctionCVTmplArgDeduction2011)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qxflag=UnwindTypedefInClassDecl)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qxflag=inlinewithdebug:stepOverInline)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qxflag=noautoinline)
            ntf_target_build_option(
                TARGET ${target} COMPILE LINK VALUE -qxflag=TolerateIncorrectUncaughtEHState)
        else()
            message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
        endif()

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # TODO: We would like to apply _ITERATOR_DEBUG_LEVEL=0 but all code
        # compiled by MSVC must be compiled with the same iterator debug level
        # otherwise a linker error occurs. Since we cannot necessarily assume
        # all dependencies are compiled with a particular iterator debug level
        # omit this preprocessor definition for now.

        # TODO: Set the MSVC_RUNTIME_LIBRARY or the specific compiler flag
        # to select the appropriate C runtime library type (static or shared,
        # optimized or debug). For now, assume the default C runtime library
        # type automatically selected by CMake based upon CMAKE_BUILD_TYPE is
        # appropriate.

        ntf_target_build_definition(
            TARGET ${target} VALUE
            WIN32 WIN32_LEAN_AND_MEAN _WIN32 _MBCS _CONSOLE
            _CRT_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS
            _CRT_NONSTDC_NO_DEPRECATE _CRT_NONSTDC_NO_WARNINGS
            _SCL_SECURE_NO_WARNINGS
            NOGDI NOMINMAX)

        ntf_target_build_option(TARGET ${target} COMPILE VALUE
            /nologo /bigobj /ERRORREPORT:NONE /Z7 /GF /FD /FC /EHa)

        # TODO: Set the /MACHINE:<arch> flag when necessary. For now assume
        # the default is sufficient.

        ntf_target_build_option(TARGET ${target} LINK VALUE
            /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO)

        if (${is_opt} AND ${is_dbg})
            ntf_target_build_option(
                TARGET ${target} COMPILE VALUE /Ox /GL /GS- /Gs-)
            ntf_target_build_option(
                TARGET ${target} LINK VALUE /DEBUG:FULL /OPT:REF /LTCG)
        elseif(${is_dbg})
            ntf_target_build_option(
                TARGET ${target} COMPILE VALUE /Od /RTC1)
            ntf_target_build_option(
                TARGET ${target} LINK VALUE /DEBUG:FULL /OPT:NOREF)
        elseif(${is_opt})
            ntf_target_build_option(
                TARGET ${target} COMPILE VALUE /Ox /GL /GS- /Gs-)
            ntf_target_build_option(
                TARGET ${target} LINK VALUE /DEBUG:NONE /OPT:REF /LTCG)
        endif()

        # Force the redefinition of multiple symbols to work around the
        # problem of different translation units compiled with different
        # settings of /MD and /MDd.

        # ntf_target_build_option(
        #     TARGET ${target} LINK VALUE /FORCE)
    endif()

endfunction()

# Set the compiler and linker options for a target common to all UFIDs.
function (ntf_target_options_common_epilog target)

    ntf_repository_build_ufid_get(OUTPUT ufid)

    ntf_ufid_string_has(UFID ${ufid} FLAG "opt" OUTPUT is_opt)
    ntf_ufid_string_has(UFID ${ufid} FLAG "dbg" OUTPUT is_dbg)

    ntf_ufid_string_has(UFID ${ufid} FLAG "stlport" OUTPUT is_stlport)

    ntf_repository_build_warnings_get(
        OUTPUT enable_warnings)

    ntf_repository_build_warnings_as_errors_get(
        OUTPUT enable_warnings_as_errors)

    ntf_target_thirdparty_get(TARGET ${target} OUTPUT target_thirdparty)

    if (${target_thirdparty})
        set(enable_warnings FALSE)
        set(enable_warnings_as_errors FALSE)
    endif()

    if (${enable_warnings})
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            ntf_target_build_option(
                TARGET ${target} COMPILE VALUE -Wall -Wextra -Wpedantic)
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            ntf_target_build_option(
                TARGET ${target} COMPILE VALUE -Wall -Wextra -Wpedantic)
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
            ntf_target_build_option(
                TARGET ${target} COMPILE VALUE -errtags)
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "XL")
            # It is not currently possible to support warnings-as-errors using
            # xlc because the mechanism, if any, to hide warnings from external
            # headers is not known. For now, disable warnings altogether.
            #
            # ntf_target_build_option(
            #     TARGET ${target} COMPILE VALUE -qinfo=all)
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            ntf_target_build_definition(
                TARGET ${target} VALUE _CRT_SECURE_NO_WARNINGS)
            ntf_target_build_definition(
                TARGET ${target} VALUE _CRT_SECURE_NO_DEPRECATE)
            ntf_target_build_definition(
                TARGET ${target} VALUE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
            ntf_target_build_definition(
                TARGET ${target} VALUE _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS)

            ntf_target_build_option(TARGET ${target} COMPILE VALUE /W3)
        endif()

        if (${enable_warnings_as_errors})
            if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                ntf_target_build_option(
                    TARGET ${target} COMPILE VALUE -Werror)
            elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                # Treating warnings as errors is only supported for GCC
                # versions >= GCC 7.0. The codebase is not known to compile
                # without warnings on earlier versions of GCC.
                if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7.0)
                    ntf_target_build_option(
                        TARGET ${target} COMPILE VALUE -Werror)
                endif()
            elseif (CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
                ntf_target_build_option(
                    TARGET ${target} COMPILE VALUE -errwarn=%all)
            elseif (CMAKE_CXX_COMPILER_ID MATCHES "XL")
                # It is not currently possible to support warnings-as-errors
                # using xlc because the mechanism, if any, to hide warnings
                # from external headers is not known. For now, disable warnings
                # altogether.
                #
                # ntf_target_build_option(
                #     TARGET ${target} COMPILE VALUE -qhalt=w)
            elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
                ntf_target_build_option(TARGET ${target} COMPILE VALUE /WX)
                if (MSVC_VERSION GREATER 1900)
                    ntf_repository_install_refroot_get(OUTPUT install_refroot)
                    ntf_repository_install_prefix_get(OUTPUT install_prefix)

                    ntf_target_build_option(
                        TARGET ${target} COMPILE VALUE /experimental:external)

                    ntf_target_build_option(
                        TARGET ${target} COMPILE VALUE
                        /external:I${install_refroot}/${install_prefix}/include)

                    ntf_target_build_option(
                        TARGET ${target} COMPILE VALUE /external:W0)
                endif()
            endif()
        endif()
    else()
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|SunPro|XL")
            ntf_target_build_option(TARGET ${target} COMPILE VALUE -w)
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            ntf_target_build_option(TARGET ${target} COMPILE VALUE /W0)
        endif()
    endif()

    if (NOT WIN32)
        set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
        set(THREADS_PREFER_PTHREAD_FLAG TRUE)
        find_package(Threads QUIET REQUIRED)
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
            target_link_libraries(${target} PUBLIC Threads::Threads)
        elseif (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
            target_link_libraries(${target} PUBLIC execinfo)
            target_link_libraries(${target} PUBLIC util)
            target_link_libraries(${target} PUBLIC dl)
            target_link_libraries(${target} PUBLIC Threads::Threads)
        elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
            target_link_libraries(${target} PUBLIC dl)
            target_link_libraries(${target} PUBLIC rt)
            target_link_libraries(${target} PUBLIC Threads::Threads)
        endif()
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
            target_link_libraries(${target} PUBLIC Threads::Threads)
        elseif (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
            target_link_libraries(${target} PUBLIC execinfo)
            target_link_libraries(${target} PUBLIC util)
            target_link_libraries(${target} PUBLIC dl)
            target_link_libraries(${target} PUBLIC Threads::Threads)
        elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
            target_link_libraries(${target} PUBLIC dl)
            target_link_libraries(${target} PUBLIC rt)
            target_link_libraries(${target} PUBLIC Threads::Threads)
        endif()
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
        target_link_libraries(${target} PUBLIC sendfile)
        target_link_libraries(${target} PUBLIC socket)
        target_link_libraries(${target} PUBLIC resolv)
        target_link_libraries(${target} PUBLIC nsl)
        target_link_libraries(${target} PUBLIC dl)
        target_link_libraries(${target} PUBLIC Threads::Threads)
        if (${is_stlport})
            target_link_libraries(${target} PUBLIC -library=stlport4)
        endif()
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "XL")
        target_link_libraries(${target} PUBLIC Threads::Threads)
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_link_libraries(${target} PUBLIC kernel32.lib)
        target_link_libraries(${target} PUBLIC user32.lib)
        target_link_libraries(${target} PUBLIC gdi32.lib)
        target_link_libraries(${target} PUBLIC winspool.lib)
        target_link_libraries(${target} PUBLIC comdlg32.lib)
        target_link_libraries(${target} PUBLIC advapi32.lib)
        target_link_libraries(${target} PUBLIC shell32.lib)
        target_link_libraries(${target} PUBLIC ole32.lib)
        target_link_libraries(${target} PUBLIC oleaut32.lib)
        target_link_libraries(${target} PUBLIC uuid.lib)
        target_link_libraries(${target} PUBLIC odbc32.lib)
        target_link_libraries(${target} PUBLIC odbccp32.lib)
        target_link_libraries(${target} PUBLIC ws2_32.lib)
        target_link_libraries(${target} PUBLIC userenv.lib)
    endif()

endfunction()

# Set the compiler and linker options for a target based upon the "opt" UFID.
function (ntf_target_options_opt target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_OPT)

    # Note: Optimization flags are set in ntf_target_options_common_prolog.
endfunction()

# Set the compiler and linker options for a target based upon the "dbg" UFID.
function (ntf_target_options_dbg target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_DBG)

    # Note: Debug flags are set in ntf_target_options_common_prolog.
endfunction()

# Set the compiler and linker options for a target based upon the "exc" UFID.
function (ntf_target_options_exc target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_EXC)

    # Note: Exception flags are set in ntf_target_options_common_prolog.
endfunction()

# Set the compiler and linker options for a target based upon the "mt" UFID.
function (ntf_target_options_mt target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_MT)

    # Note: Multithreaded flags are set in ntf_target_options_common_prolog.
endfunction()

# Set the compiler and linker options for a target based upon the *lack* of
# the "64" UFID.
function (ntf_target_options_32 target)
    set(CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX "" CACHE INTERNAL "" FORCE)
endfunction()

# Set the compiler and linker options for a target based upon the "64" UFID.
function (ntf_target_options_64 target)
    set(CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX "64" CACHE INTERNAL "" FORCE)
endfunction()

# Set the compiler and linker options for a target based upon the "safe" UFID.
function (ntf_target_options_safe target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_SAFE)
endfunction()

# Set the compiler and linker options for a target based upon the "safe2" UFID.
function (ntf_target_options_safe2 target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_SAFE_2)
endfunction()

# Set the compiler and linker options for a target based upon the "aopt" UFID.
function (ntf_target_options_aopt target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BSLS_ASSERT_LEVEL_ASSERT_OPT)
endfunction()

# Set the compiler and linker options for a target based upon the "adbg" UFID.
function (ntf_target_options_adbg target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BSLS_ASSERT_LEVEL_ASSERT)
endfunction()

# Set the compiler and linker options for a target based upon the "asafe" UFID.
function (ntf_target_options_asafe target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BSLS_ASSERT_LEVEL_ASSERT_SAFE)
endfunction()

# Set the compiler and linker options for a target based upon the "anone" UFID.
function (ntf_target_options_anone target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BSLS_ASSERT_LEVEL_NONE)
endfunction()

# Set the compiler and linker options for a target based upon the "ropt" UFID.
function (ntf_target_options_ropt target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BSLS_REVIEW_LEVEL_REVIEW_OPT)
endfunction()

# Set the compiler and linker options for a target based upon the "rdbg" UFID.
function (ntf_target_options_rdbg target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BSLS_REVIEW_LEVEL_REVIEW)
endfunction()

# Set the compiler and linker options for a target based upon the "rsafe" UFID.
function (ntf_target_options_rsafe target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BSLS_REVIEW_LEVEL_REVIEW_SAFE)
endfunction()

# Set the compiler and linker options for a target based upon the "rnone" UFID.
function (ntf_target_options_rnone target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BSLS_REVIEW_LEVEL_NONE)
endfunction()

# Set the compiler and linker options for a target based upon the "asan" UFID.
function (ntf_target_options_asan target)
    ntf_target_build_definition(
        TARGET ${target} VALUE BDE_BUILD_TARGET_ASAN)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        ntf_target_build_option(
            TARGET ${target} COMPILE LINK VALUE -fsanitize=address)
        ntf_target_build_option(
            TARGET ${target} LINK VALUE -static-libsan)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        ntf_target_build_option(
            TARGET ${target} COMPILE LINK VALUE -fsanitize=address)
        ntf_target_build_option(
            TARGET ${target} LINK VALUE -static-libasan)
    else()
        ntf_die("The UFID flag 'asan' is not supported by compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

# Set the compiler and linker options for a target based upon the "msan" UFID.
function (ntf_target_options_msan target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_MSAN)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        ntf_target_build_option(
            TARGET ${target} COMPILE LINK VALUE -fsanitize=memory)
        ntf_target_build_option(
            TARGET ${target} LINK VALUE -static-libsan)
    else()
        ntf_die("The UFID flag 'msan' is not supported by compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

# Set the compiler and linker options for a target based upon the "tsan" UFID.
function (ntf_target_options_tsan target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_TSAN)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        ntf_target_build_option(
            TARGET ${target} COMPILE LINK VALUE -fsanitize=thread)
        ntf_target_build_option(
            TARGET ${target} LINK VALUE -static-libsan)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        ntf_target_build_option(
            TARGET ${target} COMPILE LINK VALUE -fsanitize=thread)
        ntf_target_build_option(
            TARGET ${target} LINK VALUE -static-libtsan)
    else()
        ntf_die("The UFID flag 'tsan' is not supported by compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

# Set the compiler and linker options for a target based upon the "ubsan" UFID.
function (ntf_target_options_ubsan target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_UBSAN)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        ntf_target_build_option(
            TARGET ${target} COMPILE LINK VALUE -fsanitize=undefined)
        ntf_target_build_option(
            TARGET ${target} LINK VALUE -static-libsan)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        ntf_target_build_option(
            TARGET ${target} COMPILE LINK VALUE -fsanitize=undefined)
        ntf_target_build_option(
            TARGET ${target} LINK VALUE -static-libubsan)
    else()
        ntf_die("The UFID flag 'ubsan' is not supported by compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

# Set the compiler and linker options for a target based upon the "cov" UFID.
function (ntf_target_options_cov target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_COV)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        ntf_target_build_option(TARGET ${target} COMPILE LINK --coverage)
    else()
        ntf_die("The UFID flag 'cov' is not supported by compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

# Set the compiler and linker options for a target based upon the "static" UFID.
function (ntf_target_options_static target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_STATIC)

    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE -static)
            ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE -lc++abi)
            ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE -pthread)
            ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE -fuse-ld=lld)
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE -static)
            ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE -static-libstdc++)
            ntf_target_build_option(TARGET ${target} COMPILE LINK VALUE -static-libgcc)
        else()
            ntf_die("The UFID flag 'static' is not supported by compiler: ${CMAKE_CXX_COMPILER_ID}")
        endif()
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        ntf_repository_build_ufid_get(OUTPUT ufid)
        ntf_ufid_string_has(UFID ${ufid} FLAG "opt" OUTPUT is_opt)
        if (${is_opt})
            set_property(
                TARGET   ${target}
                PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
        else()
            set_property(
                TARGET   ${target}
                PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
        endif()
    else()
        ntf_die("The UFID flag 'static' is not supported by the platform: ${CMAKE_SYSTEM_NAME}")
    endif()

endfunction()

# Set the compiler and linker options for a target based upon the "fuzz" UFID.
function (ntf_target_options_fuzz target)
    ntf_target_build_definition(TARGET ${target} VALUE BDE_BUILD_TARGET_FUZZ)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        ntf_target_build_definition(
            TARGET ${target} VALUE BDE_ACTIVATE_FUZZ_TESTING)

        ntf_target_build_option(
            TARGET ${target} COMPILE VALUE -fsanitize=fuzzer-no-link)

        ntf_target_build_option(
            TARGET ${target} LINK VALUE -fsanitize=fuzzer)

        ntf_target_build_option(
            TARGET ${target} LINK VALUE -nostdlib++)
    else()
        ntf_die("The UFID flag 'fuzz' is not supported by compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

# Set the compiler and linker options for a target based upon the "stlport"
# UFID.
function (ntf_target_options_stlport target)
    if (CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
        ntf_target_build_definition(
            TARGET ${target} VALUE BDE_BUILD_TARGET_STLPORT)
    else()
        ntf_die("The UFID flag 'stlport' is not supported by compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()

    # Note: STLport flags are set in ntf_target_options_common_epilog.
endfunction()

# Set the compiler and linker options for a target based upon the "pic" UFID.
function (ntf_target_options_pic target)
    # Note: Position-independant code flags are set in
    # ntf_target_options_common_prolog.
endfunction()

# Set the compiler and linker options for a target based upon the "shr" UFID.
function (ntf_target_options_shr target)
    ntf_die("The UFID flag 'shr' is not supported")
endfunction()

# Set the compiler and linker options for a target based upon the "ndebug" UFID.
function (ntf_target_options_ndebug target)
    ntf_target_build_definition(TARGET ${target} VALUE NDEBUG)
endfunction()

# Set the compiler and linker options for a target based upon the "cpp03" UFID.
function (ntf_target_options_cpp03 target)
    # Note: Specification of the usage of the C++03 standard is specified
    # in ntf_target_options.
endfunction()

# Set the compiler and linker options for a target based upon the "cpp11" UFID.
function (ntf_target_options_cpp11 target)
    # Note: Specification of the usage of the C++11 standard is specified
    # in ntf_target_options.
endfunction()

# Set the compiler and linker options for a target based upon the "cpp14" UFID.
function (ntf_target_options_cpp14 target)
    # Note: Specification of the usage of the C++14 standard is specified
    # in ntf_target_options.
endfunction()

# Set the compiler and linker options for a target based upon the "cpp17" UFID.
function (ntf_target_options_cpp17 target)
    # Note: Specification of the usage of the C++17 standard is specified
    # in ntf_target_options.
endfunction()

# Set the compiler and linker options for a target based upon the "cpp20" UFID.
function (ntf_target_options_cpp20 target)
    # Note: Specification of the usage of the C++20 standard is specified
    # in ntf_target_options.
endfunction()

# Set the compiler and linker options for a target based upon the UFID.
#
# TARGET - The target.
function (ntf_target_options)
    cmake_parse_arguments(
        ARG "" "TARGET;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)

    ntf_assert_target_defined(${target})

    ntf_repository_build_ufid_get(OUTPUT ufid)

    set(known_ufid_flags
        opt dbg exc mt 64 safe safe2
        aopt adbg asafe anone
        ropt rdbg rsafe rnone
        asan msan tsan ubsan
        fuzz
        cov
        static
        stlport pic shr ndebug
        cpp03 cpp11 cpp14 cpp17 cpp20)

    foreach(flag IN LISTS known_ufid_flags)
        ntf_ufid_string_has(UFID ${ufid} FLAG "${flag}" OUTPUT "is_${flag}")
    endforeach()

    ntf_repository_build_coverage_get(OUTPUT measure_coverage)
    ntf_repository_toolchain_link_static_get(OUTPUT link_static)

    ntf_target_options_common_prolog(${target})

    if (${is_opt})
        ntf_target_options_opt(${target})
    endif()

    if (${is_dbg})
        ntf_target_options_dbg(${target})
    endif()

    if (${is_exc})
        ntf_target_options_exc(${target})
    endif()

    if (${is_mt})
        ntf_target_options_mt(${target})
    endif()

    if (${is_64})
        ntf_target_options_64(${target})
    else()
        ntf_target_options_32(${target})
    endif()

    if (${is_safe})
        ntf_target_options_safe(${target})
    endif()

    if (${is_safe2})
        ntf_target_options_safe2(${target})
    endif()

    if (${is_aopt})
        ntf_target_options_aopt(${target})
    endif()

    if (${is_adbg})
        ntf_target_options_adbg(${target})
    endif()

    if (${is_asafe})
        ntf_target_options_asafe(${target})
    endif()

    if (${is_anone})
        ntf_target_options_anone(${target})
    endif()

    if (${is_ropt})
        ntf_target_options_ropt(${target})
    endif()

    if (${is_rdbg})
        ntf_target_options_rdbg(${target})
    endif()

    if (${is_rsafe})
        ntf_target_options_rsafe(${target})
    endif()

    if (${is_rnone})
        ntf_target_options_rnone(${target})
    endif()

    if (${is_asan})
        ntf_target_options_asan(${target})
    endif()

    if (${is_msan})
        ntf_target_options_msan(${target})
    endif()

    if (${is_tsan})
        ntf_target_options_tsan(${target})
    endif()

    if (${is_ubsan})
        ntf_target_options_ubsan(${target})
    endif()

    if (${is_cov} OR ${measure_coverage})
        ntf_target_options_cov(${target})
    endif()

    if (${is_static} OR ${link_static})
        ntf_target_options_static(${target})
    endif()

    if (${is_fuzz})
        ntf_target_options_fuzz(${target})
    endif()

    if (${is_stlport})
        ntf_target_options_stlport(${target})
    endif()

    if (${is_pic})
        ntf_target_options_pic(${target})
    endif()

    if (${is_shr})
        ntf_target_options_shr(${target})
    endif()

    if (${is_ndebug})
        ntf_target_options_ndebug(${target})
    endif()

    if (${is_cpp03})
        ntf_target_options_cpp03(${target})
    endif()

    if (${is_cpp11})
        ntf_target_options_cpp11(${target})
    endif()

    if (${is_cpp14})
        ntf_target_options_cpp14(${target})
    endif()

    if (${is_cpp17})
        ntf_target_options_cpp17(${target})
    endif()

    if (${is_cpp20})
        ntf_target_options_cpp20(${target})
    endif()
endfunction()


# Dump all properties of a target.
function (ntf_target_dump target)
    ntf_assert_target_defined(${target})

    if(NOT CMAKE_PROPERTY_LIST)
        execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
        string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
        string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    endif()

    list(APPEND CMAKE_PROPERTY_LIST NTF_NAME)
    list(APPEND CMAKE_PROPERTY_LIST NTF_PATH)
    list(APPEND CMAKE_PROPERTY_LIST NTF_DESCRIPTION)
    list(APPEND CMAKE_PROPERTY_LIST NTF_PRIVATE)
    list(APPEND CMAKE_PROPERTY_LIST NTF_REQUIRES)

    foreach(property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" property ${property})

        if (property STREQUAL "LOCATION" OR property MATCHES "^LOCATION_" OR property MATCHES "_LOCATION$")
            continue()
        endif()

        get_property(was_set TARGET ${target} PROPERTY ${property} SET)
        if(was_set)
            get_target_property(value ${target} ${property})
            message("${target} ${property} = ${value}")
        endif()
    endforeach()
endfunction()

# Dump a property of a target.
function (ntf_target_dump_property target property)

    if(NOT TARGET ${target})
        message(STATUS "There is no target named '${target}'")
        return()
    endif()

    get_property(was_set TARGET ${target} PROPERTY ${property} SET)
    if(was_set)
        get_target_property(value ${target} ${property})
        message("NTF Build: ${target} ${property} = ${value}")
    endif()

endfunction()

# Dump all properties of a source file.
function (ntf_source_dump file)

    if(NOT CMAKE_PROPERTY_LIST)
        execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
        # Convert command output into a CMake list
        string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
        string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    endif()

    list(APPEND CMAKE_PROPERTY_LIST NTF_NAME)
    list(APPEND CMAKE_PROPERTY_LIST NTF_PATH)
    list(APPEND CMAKE_PROPERTY_LIST NTF_DESCRIPTION)
    list(APPEND CMAKE_PROPERTY_LIST NTF_PRIVATE)
    list(APPEND CMAKE_PROPERTY_LIST NTF_REQUIRES)

    foreach(property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" property ${property})

        # Fix https://stackoverflow.com/questions/32197663/how-can-i-remove-the-the-location-property-may-not-be-read-from-target-error-i
        #if(property STREQUAL "LOCATION" OR property MATCHES "^LOCATION_" OR property MATCHES "_LOCATION$")
        #    continue()
        #endif()

        get_property(was_set SOURCE ${file} PROPERTY ${property} SET)
        if(was_set)
            get_source_file_property(value ${file} ${property})
            message("${file} ${property} = ${value}")
        endif()
    endforeach()
endfunction()

# Begin the definition of a executable.
#
# NAME        - The name of the executable.
# PATH        - The path to the executable. If the path is relative, the path
#               is interpreted relative to the root of the repository.
# DESCRIPTION - The single line description of the executable, as it should
#               appear in package meta-data.
# REQUIRES    - The intra-repository dependencies of the executable.
# PRIVATE     - The flag indicating that all headers and artifacts are private
#               and should not be installed.
# TEST        - The flag indicating this executable should be included in the
#               test suite.
# EXAMPLE     - The flag that indicates this executable is part of the example
#               suite for the repository.
function (ntf_executable)
    cmake_parse_arguments(
        NTF_EXECUTABLE "PRIVATE;TEST;EXAMPLE" "NAME;PATH;OUTPUT" "REQUIRES" ${ARGN})

    if ("${NTF_EXECUTABLE_NAME}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: NAME")
    endif()

    if ("${NTF_EXECUTABLE_PATH}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: PATH")
    endif()

    string(TOLOWER ${NTF_EXECUTABLE_NAME} target_lowercase)
    string(TOUPPER ${NTF_EXECUTABLE_NAME} target_uppercase)

    set(target ${target_lowercase})

    if (IS_ABSOLUTE ${NTF_EXECUTABLE_PATH})
        set(target_path ${NTF_EXECUTABLE_PATH})
    else()
        ntf_repository_path_get(OUTPUT repository_path)
        file(REAL_PATH ${NTF_EXECUTABLE_PATH} target_path
             BASE_DIRECTORY ${repository_path})
    endif()

    set(target_private ${NTF_EXECUTABLE_PRIVATE})
    if ("${target_private}" STREQUAL "")
        set(target_private FALSE)
    endif()

    ntf_repository_archive_output_path_get(OUTPUT archive_output_path)
    ntf_repository_library_output_path_get(OUTPUT library_output_path)
    ntf_repository_runtime_output_path_get(OUTPUT runtime_output_path)

    string(REPLACE "m_" "" target_undecorated "${target}")
    cmake_path(APPEND main_path ${target_path} "${target_undecorated}.m.cpp")

    if (VERBOSE)
        message(STATUS "NTF Build: adding executable '${target}'")
        message(STATUS "         * main_path = ${main_path}")
    endif()

    add_executable(${target} ${main_path})

    # Set the C standard.

    set_property(TARGET ${target} PROPERTY C_STANDARD 11)

    # Set the C++ standard version unless using xlc, which requires
    # special, non-portable flag for some C++03-like behavior.

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "XL")
        ntf_repository_standard_get(OUTPUT cxx_standard)
        set_property(TARGET ${target} PROPERTY CXX_STANDARD ${cxx_standard})
    endif()

    set_property(TARGET ${target} PROPERTY EXPORT_COMPILE_COMMANDS TRUE)

    set_property(TARGET ${target}
                 PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${archive_output_path})

    set_property(TARGET ${target}
                 PROPERTY LIBRARY_OUTPUT_DIRECTORY ${library_output_path})

    set_property(TARGET ${target}
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY ${runtime_output_path})

    if (NOT "${NTF_EXECUTABLE_OUTPUT}" STREQUAL "")
        set_target_properties(
            ${target} PROPERTIES OUTPUT_NAME ${NTF_EXECUTABLE_OUTPUT}
        )

        set_target_properties(
            ${target} PROPERTIES SUFFIX "${CMAKE_EXECUTABLE_SUFFIX}"
        )
    endif()

    ntf_target_options(TARGET ${target})

    ntf_target_path_set(
        TARGET ${target} VALUE ${target_path})

    ntf_target_private_set(
        TARGET ${target} VALUE ${target_private})

    ntf_target_thirdparty_set(
        TARGET ${target} VALUE FALSE)

    ntf_target_requires_set(
        TARGET ${target} VALUE ${NTF_EXECUTABLE_REQUIRES})

    set(NTF_TARGET_PATH_${target_uppercase} ${target_path}
        CACHE INTERNAL "")

    set(NTF_TARGET_PRIVATE_${target_uppercase} ${target_private}
        CACHE INTERNAL "")

    set(NTF_TARGET_REQUIRES_${target_uppercase} ${NTF_EXECUTABLE_REQUIRES}
        CACHE INTERNAL "")

    target_include_directories(
        ${target}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        $<BUILD_INTERFACE:${target_path}>
    )

    ntf_ide_vs_code_tasks_add_target(${target})
    ntf_ide_vs_code_launch_add_target(${target})

    foreach(entry ${NTF_EXECUTABLE_REQUIRES})
        set(dependency ${entry})

        if (VERBOSE)
            message(STATUS "NTF Build: linking executable '${target}' to target '${dependency}'")
            message(STATUS "         * target_link_libraries(${target} ${dependency})")
        endif()

        target_link_libraries(${target} PUBLIC ${dependency})
    endforeach()

    ntf_repository_runtime_output_path_get(OUTPUT runtime_output_path)

    if (${NTF_EXECUTABLE_TEST})
        set(executable_test_build_target "${target}.t")

        add_dependencies(
            build_test
            ${target}
        )

        cmake_path(
            APPEND
            test_usage_executable
            ${runtime_output_path} ${target})

        add_test(
            NAME ${executable_test_build_target}
            COMMAND ${test_usage_executable}
        )

        set_tests_properties(${executable_test_build_target} PROPERTIES TIMEOUT 600)

        ntf_ide_vs_code_tasks_add_target(${executable_test_build_target})
        ntf_ide_vs_code_launch_add_target(${executable_test_build_target})

    endif()

    if (${NTF_EXECUTABLE_EXAMPLE})
        if (NOT TARGET test_usage)
            add_custom_target(test_usage)
        endif()

        set(test_usage_target "test_usage_${target}")

        cmake_path(
            APPEND
            test_usage_executable
            ${runtime_output_path} ${target})

        add_custom_target(
            ${test_usage_target}
            COMMAND ${test_usage_executable}
            WORKING_DIRECTORY ${runtime_output_path})

        add_dependencies(test_usage ${test_usage_target})

    endif()

    if (NOT ${target_private})
        if (NOT "${NTF_EXECUTABLE_OUTPUT}" STREQUAL "")
            if(UNIX)
                set(executable_basename "${NTF_EXECUTABLE_OUTPUT}")
            elseif(WIN32)
                set(executable_basename "${NTF_EXECUTABLE_OUTPUT}.exe")
            else()
                message(FATAL_ERROR "Unsupported platform")
            endif()
        else()
            if(UNIX)
                set(executable_basename "${target}")
            elseif(WIN32)
                set(executable_basename "${target}.exe")
            else()
                message(FATAL_ERROR "Unsupported platform")
            endif()
        endif()

        cmake_path(
            APPEND
            executable_source_path
            ${runtime_output_path} ${executable_basename})

        set(executable_destination_relative_path "bin")

        install(
            PROGRAMS
                ${executable_source_path}
            DESTINATION
                ${executable_destination_relative_path}
            COMPONENT
                development
        )
    endif()

    if (NOT TARGET test_${target})
        add_custom_target(
            test_${target}
            COMMAND ${CMAKE_CTEST_COMMAND} -R ${target})
    endif()

    if (NOT TARGET format_${target})
        add_custom_target(format_${target})
        add_dependencies(format format_${target})
    endif()

    if (TARGET documentation)
        add_dependencies(documentation ${target})
    endif()

endfunction()

# Add a dependency to an executable, attempting to find the dependency through
# either CMake, pkg-config, or as a raw library.
#
# NAME       - The executable.
# DEPENDENCY - The dependency name list.
# OPTIONAL   - Flag indicating the target is linked to the dependency only if
#              it exists, otherwise the dependency must be must found
function (ntf_executable_requires)
    cmake_parse_arguments(
        ARG "OPTIONAL" "NAME" "DEPENDENCY" ${ARGN})

    ntf_assert_defined(${ARG_NAME})
    ntf_assert_defined(${ARG_DEPENDENCY})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    foreach (dependency ${ARG_DEPENDENCY})
        ntf_target_link_dependency(
            TARGET ${target} DEPENDENCY ${dependency} OUTPUT result OPTIONAL)

        if (NOT ${result} AND NOT ${ARG_OPTIONAL})
            ntf_die("The target '${target}' requires dependency '${dependency}' but it cannot be found")
        endif()
    endforeach()
endfunction()

# End the definition of an executable.
function (ntf_executable_end)
    cmake_parse_arguments(
        ARG "" "NAME" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    ntf_target_options_common_epilog(${target})

    # TODO: Implement any post-processing for an executable.

endfunction()

# Begin the definition of an interface.
#
# NAME        - The name of the interface.
# PATH        - The path to the interface. If the path is relative, the path
#               is interpreted relative to the root of the repository.
# DESCRIPTION - The single line description of the interface, as it should
#               appear in package meta-data.
# REQUIRES    - The intra-repository dependencies of the interface.
# PRIVATE     - The flag indicating that all headers and artifacts are private
#               and should not be installed.
function (ntf_interface)
    cmake_parse_arguments(
        NTF_INTERFACE "PRIVATE;THIRDPARTY" "NAME;DESCRIPTION" "REQUIRES" ${ARGN})

    if ("${NTF_INTERFACE_NAME}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: NAME")
    endif()

    # if ("${NTF_INTERFACE_PATH}" STREQUAL "")
    #     message(FATAL_ERROR "Invalid parameter: PATH")
    # endif()

    if ("${NTF_INTERFACE_DESCRIPTION}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: DESCRIPTION")
    endif()

    string(TOLOWER ${NTF_INTERFACE_NAME} target_lowercase)
    string(TOUPPER ${NTF_INTERFACE_NAME} target_uppercase)

    set(target ${target_lowercase})

    # if (IS_ABSOLUTE ${NTF_INTERFACE_PATH})
    #     set(target_path ${NTF_INTERFACE_PATH})
    # else()
    #     ntf_repository_path_get(OUTPUT repository_path)
    #     file(REAL_PATH ${NTF_INTERFACE_PATH} target_path
    #          BASE_DIRECTORY ${repository_path})
    # endif()

    set(target_private ${NTF_INTERFACE_PRIVATE})
    if ("${target_private}" STREQUAL "")
        set(target_private FALSE)
    endif()

    set(target_thirdparty ${NTF_INTERFACE_THIRDPARTY})
    if ("${target_thirdparty}" STREQUAL "")
        set(target_thirdparty FALSE)
    endif()

    ntf_repository_archive_output_path_get(OUTPUT archive_output_path)
    ntf_repository_library_output_path_get(OUTPUT library_output_path)
    ntf_repository_runtime_output_path_get(OUTPUT runtime_output_path)

    add_library(${target} INTERFACE)

    # Set the C standard.

    set_property(TARGET ${target} PROPERTY C_STANDARD 11)

    # Set the C++ standard version unless using xlc, which requires
    # special, non-portable flag for some C++03-like behavior.

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "XL")
        ntf_repository_standard_get(OUTPUT cxx_standard)
        set_property(TARGET ${target} PROPERTY CXX_STANDARD ${cxx_standard})
    endif()

    set_property(TARGET ${target} PROPERTY EXPORT_COMPILE_COMMANDS TRUE)

    set_property(TARGET ${target}
                 PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${archive_output_path})

    set_property(TARGET ${target}
                 PROPERTY LIBRARY_OUTPUT_DIRECTORY ${library_output_path})

    set_property(TARGET ${target}
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY ${runtime_output_path})

    # ntf_target_options(TARGET ${target})

    ntf_target_path_set(
        TARGET ${target} VALUE ${target_path})

    ntf_target_description_set(
        TARGET ${target} VALUE ${NTF_INTERFACE_DESCRIPTION})

    ntf_target_private_set(
        TARGET ${target} VALUE ${target_private})

    ntf_target_thirdparty_set(
        TARGET ${target} VALUE ${target_thirdparty})

    ntf_target_requires_set(
        TARGET ${target} VALUE ${NTF_INTERFACE_REQUIRES})

    # if (${target_thirdparty})
    #     file(REAL_PATH ${target_path}/.. target_parent_path)
    #
    #     target_include_directories(
    #         ${target}
    #         SYSTEM PUBLIC
    #         $<INSTALL_INTERFACE:include>
    #         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    #         $<BUILD_INTERFACE:${target_path}>
    #         $<BUILD_INTERFACE:${target_parent_path}>
    #     )
    # else()
    #     target_include_directories(
    #         ${target}
    #         PUBLIC
    #         $<INSTALL_INTERFACE:include>
    #         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    #         $<BUILD_INTERFACE:${target_path}>
    #     )
    # endif()

    foreach(entry ${NTF_INTERFACE_REQUIRES})
        set(dependency ${entry})

        if (VERBOSE)
            message(STATUS "NTF Build: linking interface '${target}' to target '${dependency}'")
            message(STATUS "         * target_link_libraries(${target} ${dependency})")
        endif()

        target_link_libraries(${target} PUBLIC ${dependency})
    endforeach()

    # if (NOT ${target_thirdparty})
    #     if (NOT TARGET test_${target})
    #         add_custom_target(
    #             test_${target}
    #             COMMAND ${CMAKE_CTEST_COMMAND} -R ${target})
    #     endif()
    #
    #     if (NOT TARGET format_${target})
    #         add_custom_target(format_${target})
    #         add_dependencies(format format_${target})
    #     endif()
    #
    #     if (TARGET documentation)
    #         add_dependencies(documentation ${target})
    #     endif()
    # endif()

endfunction()

# Add a dependency to an interface, attempting to find the dependency through
# either CMake, pkg-config, or as a raw library.
#
# NAME       - The interface.
# DEPENDENCY - The dependency name list.
# OPTIONAL   - Flag indicating the target is linked to the dependency only if
#              it exists, otherwise the dependency must be must found
function (ntf_interface_requires)
    cmake_parse_arguments(
        ARG "OPTIONAL" "NAME" "DEPENDENCY" ${ARGN})

    ntf_assert_defined(${ARG_NAME})
    ntf_assert_defined(${ARG_DEPENDENCY})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    foreach (dependency ${ARG_DEPENDENCY})
        ntf_target_link_dependency(
            TARGET ${target} DEPENDENCY ${dependency} OUTPUT result OPTIONAL)

        if (NOT ${result} AND NOT ${ARG_OPTIONAL})
            ntf_die("The target '${target}' requires dependency '${dependency}' but it cannot be found")
        endif()
    endforeach()
endfunction()

# End the definition of an interface.
function (ntf_interface_end)
    cmake_parse_arguments(
        ARG "" "NAME" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    ntf_target_private_get(TARGET ${target} OUTPUT target_private)

    # ntf_target_options_common_epilog(${target})

    ntf_repository_install_refroot_get(OUTPUT install_refroot)
    ntf_repository_install_prefix_get(OUTPUT install_prefix)

    ntf_repository_build_ufid_get(OUTPUT build_ufid)
    ntf_repository_install_ufid_get(OUTPUT install_ufid)

    ntf_ufid_string_has(UFID ${build_ufid} FLAG "64" OUTPUT is_64_bit)

    if (${is_64_bit} AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(lib_name "lib64" CACHE INTERNAL "")
    else()
        set(lib_name "lib" CACHE INTERNAL "")
    endif()

    # Set the relative path to the library directory under the prefix. For
    # example: lib64

    set(library_relative_path ${lib_name})

    # Set the relative path to the UFID-specific library directory under the
    # prefix. For example: lib64/dbg_exc_mt

    set(library_install_ufid_relative_path "${lib_name}/${install_ufid}")

    # Define the installation UFIDs that are aliases for this UFID. Building
    # the alias UFID is synonymous with the original UFID.

    set(alias_install_ufid_list)

    set(alias_install_ufid_pic ${install_ufid})
    ntf_ufid_add(UFID "${alias_install_ufid_pic}" FLAG "pic" OUTPUT alias_install_ufid_pic)
    list(APPEND alias_install_ufid_list ${alias_install_ufid_pic})

    if(${is_64_bit})
        set(alias_install_ufid_pic_64 ${install_ufid})
        ntf_ufid_add(UFID "${alias_install_ufid_pic_64}" FLAG "pic" OUTPUT alias_install_ufid_pic_64)
        ntf_ufid_add(UFID "${alias_install_ufid_pic_64}" FLAG "64" OUTPUT alias_install_ufid_pic_64)
        list(APPEND alias_install_ufid_list ${alias_install_ufid_pic_64})
    endif()

    # Define the alternate install UFIDs. Other build systems may expect
    # these paths to exist.

    set(alternate_install_ufid_list)

    list(APPEND alternate_install_ufid_list "!")
    list(APPEND alternate_install_ufid_list "${install_ufid}")

    if(${is_64_bit})
        list(APPEND alternate_install_ufid_list "${install_ufid}_64")
    endif()

    list(APPEND alternate_install_ufid_list ${alias_install_ufid_list})

    string (REPLACE "_exc_mt" "" modern_install_ufid_list "${alternate_install_ufid_list}")
    list(POP_FRONT modern_install_ufid_list)
    list(APPEND alternate_install_ufid_list ${modern_install_ufid_list})

    if (VERBOSE)
        message(STATUS "build_ufid:                  ${build_ufid}")
        message(STATUS "install_ufid:                ${install_ufid}")
        message(STATUS "alias_install_ufid_list:     ${alias_install_ufid_list}")
        message(STATUS "alternate_install_ufid_list: ${alternate_install_ufid_list}")
        message(STATUS "modern_install_ufid_list:    ${modern_install_ufid_list}")
    endif()

    # Install CMake target meta-data. TODO: Make this a configuration option.

    set(install_cmake_metadata TRUE)

    if (${install_cmake_metadata})
        if (NOT ${target_private})
            # CMake meta data names, version 1:

            # ${target}Config.cmake
            # ${target}ConfigVersion.cmake
            # ${target}Targets.cmake
            # ${target}Targets-debug.cmake

            # CMake meta data names, version 2:

            # ${target}-config.cmake
            # ${target}-config-version.cmake
            # ${target}-targets.cmake
            # ${target}-targets-debug.cmake

            set(cmake_metadata_relative_path
                ${library_install_ufid_relative_path}/cmake/${target})

            # Determine which CMake meta data case style to use.

            if (EXISTS "${install_refroot}/${install_prefix}/${library_relative_path}/cmake/${target}/${target}Config.cmake")
                set(install_cmake_metadata_style 1)
            else()
                set(install_cmake_metadata_style 2)
            endif()

            string(TOLOWER "${CMAKE_BUILD_TYPE}" buildTypeLowercase)

            if (${install_cmake_metadata_style} EQUAL 1)
                set(install_cmake_metadata_config "${target}Config")
                set(install_cmake_metadata_config_version "${target}ConfigVersion")
                set(install_cmake_metadata_targets "${target}Targets")
                set(install_cmake_metadata_targets_build "${target}Targets-${buildTypeLowercase}")
            else()
                set(install_cmake_metadata_config "${target}-config")
                set(install_cmake_metadata_config_version "${target}-config-version")
                set(install_cmake_metadata_targets "${target}-targets")
                set(install_cmake_metadata_targets_build "${target}-targets-${buildTypeLowercase}")
            endif()

            install(
                TARGETS
                    ${target}
                EXPORT
                    ${install_cmake_metadata_targets}
                    DESTINATION
                        ${library_install_ufid_relative_path}
                RUNTIME
                    DESTINATION
                        "bin"
                    COMPONENT
                        runtime
                LIBRARY
                    DESTINATION
                        ${library_install_ufid_relative_path}
                    COMPONENT
                        runtime
                ARCHIVE
                    DESTINATION
                        ${library_install_ufid_relative_path}
                    COMPONENT
                        development
            )

            # foreach (alternate_install_ufid ${alternate_install_ufid_list})
            #     set(canonical_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}${target}${CMAKE_STATIC_LIBRARY_SUFFIX})
            #
            #     if ("${alternate_install_ufid}" STREQUAL "!")
            #         set(alternate_library_name ${canonical_library_name})
            #     else()
            #         set(alternate_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}${target}.${alternate_install_ufid}${CMAKE_STATIC_LIBRARY_SUFFIX})
            #     endif()
            #
            #     install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_library_name}\")")
            #
            #     install(CODE "file(CREATE_LINK ${install_ufid}/${canonical_library_name} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_library_name} SYMBOLIC)")
            #
            # endforeach()

            export(
                EXPORT
                    ${install_cmake_metadata_targets}
                FILE
                    "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_targets}.cmake"

                # TODO: Export the target into a CMake namespace, if advisable.
                # NAMESPACE ${CMAKE_PROJECT_NAME}::
            )

            write_basic_package_version_file(
                "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config_version}.cmake"
                VERSION
                    ${CMAKE_PROJECT_VERSION}
                COMPATIBILITY
                    AnyNewerVersion
            )

            # Recursively find dependencies of each "requires" of this
            # target and manually generate it into the
            # "${target}-config.cmake" file. Ideally, this should be done
            # automatically by CMake itself but no version of CMake currently
            # supports such a feature.
            #
            # See the issue described at
            # https://gitlab.kitware.com/cmake/cmake/-/issues/20511

            ntf_target_requires_get(TARGET ${target} OUTPUT target_requires)

            set(target_config_cmake_content "")

            string(APPEND target_config_cmake_content "\
include(CMakeFindDependencyMacro)\n\
if (NOT TARGET ${target})\n\
    if (UNIX)\n\
        find_dependency(Threads)\n\
    endif()\n\
")

    set(target_requires_reversed ${target_requires})
    list(REVERSE target_requires_reversed)
    foreach (dependency ${target_requires_reversed})
            string(APPEND target_config_cmake_content "\
    find_dependency(${dependency} PATHS \"\${CMAKE_CURRENT_LIST_DIR}\" \"\${CMAKE_CURRENT_LIST_DIR}/..\")\n\
")
    endforeach()

            string(APPEND target_config_cmake_content "\
    include(\"\${CMAKE_CURRENT_LIST_DIR}/${install_cmake_metadata_targets}.cmake\")\n\
endif()\n\
")

            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config}.cmake"
                ${target_config_cmake_content})

            install(
                EXPORT
                    ${install_cmake_metadata_targets}
                FILE
                    ${install_cmake_metadata_targets}.cmake
                DESTINATION
                    ${cmake_metadata_relative_path}
                COMPONENT
                    development
                # TODO: Export the target into a CMake namespace, if advisable.
                # NAMESPACE ${CMAKE_PROJECT_NAME}::
            )

            install(
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config}.cmake
                    ${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config_version}.cmake
                DESTINATION
                    ${cmake_metadata_relative_path}
                COMPONENT
                    development
            )

            # Install symlinks.

            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_targets}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_targets_build}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_config}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_config_version}.cmake\")")


            install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake)")

            ntf_path_normalize(
                OUTPUT link_target
                INPUT "../${install_ufid}/cmake/${target}"
                NATIVE ESCAPE)

            install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target} SYMBOLIC)")

            foreach (alternate_install_ufid ${alternate_install_ufid_list})
                if (NOT "${alternate_install_ufid}" STREQUAL "!" AND
                    NOT "${alternate_install_ufid}" STREQUAL "${install_ufid}")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_targets}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_targets_build}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_config}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_config_version}.cmake\")")

                    install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake)")


                    ntf_path_normalize(
                        OUTPUT link_target
                        INPUT "../../${install_ufid}/cmake/${target}"
                        NATIVE ESCAPE)

                    install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target} SYMBOLIC)")
                endif()
            endforeach()

        endif()
    endif()

    # Install pkg-config meta-data. TODO: Make this a configuration option.

    set(install_pkgconfig_metadata TRUE)

    if (${install_pkgconfig_metadata})
        if (NOT ${target_private})
            ntf_target_description_get(TARGET ${target} OUTPUT target_description)
            ntf_target_requires_get(TARGET ${target} OUTPUT target_requires)
            ntf_target_libs_get(TARGET ${target} OUTPUT target_libs)

            string (REPLACE ";" " " target_requires_string "${target_requires}")

            set(target_libs_string "")
            foreach (target_libs_entry ${target_libs})
                set(target_libs_string "${target_libs_string} -l${target_libs_entry}")
            endforeach()

            # TODO: Set 'prefix' to "${pcfiledir}/../.." in the top-level
            # pkgconfig directory, set it to "${pcfiledir}/../../.." in the
            # UFID-specific pkgconfig directory, and do not install symlinks
            # in each UFID-specific pkgconfig directory, but separate copies
            # with prefix set to the directory hierarchy appropriately.

            set(target_pc_content "\
prefix=${CMAKE_INSTALL_PREFIX}\n\
libdir=\${prefix}/${library_install_ufid_relative_path}\n\
includedir=\${prefix}/include\n\
\n\
Name: ${target}\n\
Description: ${target_description}\n\
URL: ${CMAKE_PROJECT_HOMEPAGE_URL}\n\
Version: ${CMAKE_PROJECT_VERSION}\n\
Requires: \n\
Requires.private: ${target_requires_string}\n\
Cflags: -I\${includedir}\n\
Libs: \n\
Libs.private:${target_libs_string}\n\
")

            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${target}.pc"
                ${target_pc_content})

            install(
                FILES ${CMAKE_CURRENT_BINARY_DIR}/${target}.pc
                DESTINATION ${library_install_ufid_relative_path}/pkgconfig
            )

            # Install symlinks.

            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig/${target}.pc\")")

            install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig)")

            ntf_path_normalize(
                OUTPUT link_target
                INPUT "../${install_ufid}/pkgconfig/${target}.pc"
                NATIVE ESCAPE)

            install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig/${target}.pc SYMBOLIC)")

            foreach (alternate_install_ufid ${alternate_install_ufid_list})
                if (NOT "${alternate_install_ufid}" STREQUAL "!" AND
                    NOT "${alternate_install_ufid}" STREQUAL "${install_ufid}")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig/${target}.pc\")")

                    install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig)")

                    ntf_path_normalize(
                        OUTPUT link_target
                        INPUT "../../${install_ufid}/pkgconfig/${target}.pc"
                        NATIVE ESCAPE)

                    install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig/${target}.pc SYMBOLIC)")
                endif()
            endforeach()

        endif()

    endif()

endfunction()

# Begin the definition of an adapter.
#
# NAME        - The name of the adapter.
# PATH        - The path to the adapter. If the path is relative, the path
#               is interpreted relative to the root of the repository.
# DESCRIPTION - The single line description of the adapter, as it should
#               appear in package meta-data.
# REQUIRES    - The intra-repository dependencies of the adapter.
# PRIVATE     - The flag indicating that all headers and artifacts are private
#               and should not be installed.
function (ntf_adapter)
    cmake_parse_arguments(
        NTF_ADAPTER "PRIVATE;THIRDPARTY" "NAME;PATH;DESCRIPTION;OUTPUT" "REQUIRES" ${ARGN})

    if ("${NTF_ADAPTER_NAME}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: NAME")
    endif()

    if ("${NTF_ADAPTER_PATH}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: PATH")
    endif()

    if ("${NTF_ADAPTER_DESCRIPTION}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: DESCRIPTION")
    endif()

    string(TOLOWER ${NTF_ADAPTER_NAME} target_lowercase)
    string(TOUPPER ${NTF_ADAPTER_NAME} target_uppercase)

    set(target ${target_lowercase})

    if (IS_ABSOLUTE ${NTF_ADAPTER_PATH})
        set(target_path ${NTF_ADAPTER_PATH})
    else()
        ntf_repository_path_get(OUTPUT repository_path)
        file(REAL_PATH ${NTF_ADAPTER_PATH} target_path
             BASE_DIRECTORY ${repository_path})
    endif()

    set(target_private ${NTF_ADAPTER_PRIVATE})
    if ("${target_private}" STREQUAL "")
        set(target_private FALSE)
    endif()

    set(target_thirdparty ${NTF_ADAPTER_THIRDPARTY})
    if ("${target_thirdparty}" STREQUAL "")
        set(target_thirdparty FALSE)
    endif()

    ntf_repository_archive_output_path_get(OUTPUT archive_output_path)
    ntf_repository_library_output_path_get(OUTPUT library_output_path)
    ntf_repository_runtime_output_path_get(OUTPUT runtime_output_path)

    add_library(${target} STATIC)

    # Set the C standard.

    set_property(TARGET ${target} PROPERTY C_STANDARD 11)

    # Set the C++ standard version unless using xlc, which requires
    # special, non-portable flag for some C++03-like behavior.

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "XL")
        ntf_repository_standard_get(OUTPUT cxx_standard)
        set_property(TARGET ${target} PROPERTY CXX_STANDARD ${cxx_standard})
    endif()

    set_property(TARGET ${target} PROPERTY EXPORT_COMPILE_COMMANDS TRUE)

    set_property(TARGET ${target}
                 PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${archive_output_path})

    set_property(TARGET ${target}
                 PROPERTY LIBRARY_OUTPUT_DIRECTORY ${library_output_path})

    set_property(TARGET ${target}
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY ${runtime_output_path})

    if (NOT "${NTF_ADAPTER_OUTPUT}" STREQUAL "")
        set_target_properties(
            ${target} PROPERTIES OUTPUT_NAME ${NTF_ADAPTER_OUTPUT}
        )

        set_target_properties(
            ${target} PROPERTIES EXPORT_NAME ${NTF_ADAPTER_OUTPUT}
        )
    endif()

    ntf_target_options(TARGET ${target})

    ntf_target_path_set(
        TARGET ${target} VALUE ${target_path})

    ntf_target_description_set(
        TARGET ${target} VALUE ${NTF_ADAPTER_DESCRIPTION})

    ntf_target_private_set(
        TARGET ${target} VALUE ${target_private})

    ntf_target_thirdparty_set(
        TARGET ${target} VALUE ${target_thirdparty})

    ntf_target_requires_set(
        TARGET ${target} VALUE ${NTF_ADAPTER_REQUIRES})

    if (${target_thirdparty})
        file(REAL_PATH ${target_path}/.. target_parent_path)

        target_include_directories(
            ${target}
            SYSTEM PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
            $<BUILD_INTERFACE:${target_path}>
            $<BUILD_INTERFACE:${target_parent_path}>
        )
    else()
        target_include_directories(
            ${target}
            PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
            $<BUILD_INTERFACE:${target_path}>
        )
    endif()

    foreach(entry ${NTF_ADAPTER_REQUIRES})
        set(dependency ${entry})

        if (VERBOSE)
            message(STATUS "NTF Build: linking adapter '${target}' to target '${dependency}'")
            message(STATUS "         * target_link_libraries(${target} ${dependency})")
        endif()

        target_link_libraries(${target} PUBLIC ${dependency})
    endforeach()

    if (NOT ${target_thirdparty})
        if (NOT TARGET test_${target})
            add_custom_target(
                test_${target}
                COMMAND ${CMAKE_CTEST_COMMAND} -R ${target})
        endif()

        if (NOT TARGET format_${target})
            add_custom_target(format_${target})
            add_dependencies(format format_${target})
        endif()

        if (TARGET documentation)
            add_dependencies(documentation ${target})
        endif()
    endif()

endfunction()

# Set the core base name of the artifact that is the result of building the
# adapter. The name should not include any platform-specific prefix or suffix
# or extension; the actual output name will be automatically decorated
# appropriately. For example, given "foo" on Unix the output name will be
# "libfoo.a", whereas on Windows the output name will tbe "foo.lib".
function (ntf_adapter_output)
    cmake_parse_arguments(
        ARG "" "NAME;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    set_target_properties(
        ${target} PROPERTIES OUTPUT_NAME ${ARG_OUTPUT}
    )

    set_target_properties(
        ${target} PROPERTIES EXPORT_NAME ${ARG_OUTPUT}
    )
endfunction()

# Add a dependency to an adapter, attempting to find the dependency through
# either CMake, pkg-config, or as a raw library.
#
# NAME       - The adapter.
# DEPENDENCY - The dependency name list.
# OPTIONAL   - Flag indicating the target is linked to the dependency only if
#              it exists, otherwise the dependency must be must found
function (ntf_adapter_requires)
    cmake_parse_arguments(
        ARG "OPTIONAL" "NAME" "DEPENDENCY" ${ARGN})

    ntf_assert_defined(${ARG_NAME})
    ntf_assert_defined(${ARG_DEPENDENCY})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    foreach (dependency ${ARG_DEPENDENCY})
        ntf_target_link_dependency(
            TARGET ${target} DEPENDENCY ${dependency} OUTPUT result OPTIONAL)

        if (NOT ${result} AND NOT ${ARG_OPTIONAL})
            ntf_die("The target '${target}' requires dependency '${dependency}' but it cannot be found")
        endif()
    endforeach()
endfunction()

# End the definition of an adapter.
function (ntf_adapter_end)
    cmake_parse_arguments(
        ARG "" "NAME" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    ntf_target_private_get(TARGET ${target} OUTPUT target_private)

    ntf_target_options_common_epilog(${target})

    ntf_repository_install_refroot_get(OUTPUT install_refroot)
    ntf_repository_install_prefix_get(OUTPUT install_prefix)

    ntf_repository_build_ufid_get(OUTPUT build_ufid)
    ntf_repository_install_ufid_get(OUTPUT install_ufid)

    ntf_ufid_string_has(UFID ${build_ufid} FLAG "64" OUTPUT is_64_bit)

    get_target_property(target_output_name ${target} OUTPUT_NAME)
    if ("${target_output_name}" STREQUAL "" OR
        "${target_output_name}" STREQUAL "target_output_name-NOTFOUND")
        set(target_output_name "${target}")
    endif()

    if (${is_64_bit} AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(lib_name "lib64" CACHE INTERNAL "")
    else()
        set(lib_name "lib" CACHE INTERNAL "")
    endif()

    # Set the relative path to the library directory under the prefix. For
    # example: lib64

    set(library_relative_path ${lib_name})

    # Set the relative path to the UFID-specific library directory under the
    # prefix. For example: lib64/dbg_exc_mt

    set(library_install_ufid_relative_path "${lib_name}/${install_ufid}")

    # Define the installation UFIDs that are aliases for this UFID. Building
    # the alias UFID is synonymous with the original UFID.

    set(alias_install_ufid_list)

    set(alias_install_ufid_pic ${install_ufid})
    ntf_ufid_add(UFID "${alias_install_ufid_pic}" FLAG "pic" OUTPUT alias_install_ufid_pic)
    list(APPEND alias_install_ufid_list ${alias_install_ufid_pic})

    if(${is_64_bit})
        set(alias_install_ufid_pic_64 ${install_ufid})
        ntf_ufid_add(UFID "${alias_install_ufid_pic_64}" FLAG "pic" OUTPUT alias_install_ufid_pic_64)
        ntf_ufid_add(UFID "${alias_install_ufid_pic_64}" FLAG "64" OUTPUT alias_install_ufid_pic_64)
        list(APPEND alias_install_ufid_list ${alias_install_ufid_pic_64})
    endif()

    # Define the alternate install UFIDs. Other build systems may expect
    # these paths to exist.

    set(alternate_install_ufid_list)

    list(APPEND alternate_install_ufid_list "!")
    list(APPEND alternate_install_ufid_list "${install_ufid}")

    if(${is_64_bit})
        list(APPEND alternate_install_ufid_list "${install_ufid}_64")
    endif()

    list(APPEND alternate_install_ufid_list ${alias_install_ufid_list})

    string (REPLACE "_exc_mt" "" modern_install_ufid_list "${alternate_install_ufid_list}")
    list(POP_FRONT modern_install_ufid_list)
    list(APPEND alternate_install_ufid_list ${modern_install_ufid_list})

    if (VERBOSE)
        message(STATUS "build_ufid:                  ${build_ufid}")
        message(STATUS "install_ufid:                ${install_ufid}")
        message(STATUS "alias_install_ufid_list:     ${alias_install_ufid_list}")
        message(STATUS "alternate_install_ufid_list: ${alternate_install_ufid_list}")
        message(STATUS "modern_install_ufid_list:    ${modern_install_ufid_list}")
    endif()

    # Install CMake target meta-data. TODO: Make this a configuration option.

    set(install_cmake_metadata TRUE)

    if (${install_cmake_metadata})
        if (NOT ${target_private})
            # CMake meta data names, version 1:

            # ${target}Config.cmake
            # ${target}ConfigVersion.cmake
            # ${target}Targets.cmake
            # ${target}Targets-debug.cmake

            # CMake meta data names, version 2:

            # ${target}-config.cmake
            # ${target}-config-version.cmake
            # ${target}-targets.cmake
            # ${target}-targets-debug.cmake

            set(cmake_metadata_relative_path
                ${library_install_ufid_relative_path}/cmake/${target})

            # Determine which CMake meta data case style to use.

            if (EXISTS "${install_refroot}/${install_prefix}/${library_relative_path}/cmake/${target}/${target}Config.cmake")
                set(install_cmake_metadata_style 1)
            else()
                set(install_cmake_metadata_style 2)
            endif()

            string(TOLOWER "${CMAKE_BUILD_TYPE}" buildTypeLowercase)

            if (${install_cmake_metadata_style} EQUAL 1)
                set(install_cmake_metadata_config "${target}Config")
                set(install_cmake_metadata_config_version "${target}ConfigVersion")
                set(install_cmake_metadata_targets "${target}Targets")
                set(install_cmake_metadata_targets_build "${target}Targets-${buildTypeLowercase}")
            else()
                set(install_cmake_metadata_config "${target}-config")
                set(install_cmake_metadata_config_version "${target}-config-version")
                set(install_cmake_metadata_targets "${target}-targets")
                set(install_cmake_metadata_targets_build "${target}-targets-${buildTypeLowercase}")
            endif()

            install(
                TARGETS
                    ${target}
                EXPORT
                    ${install_cmake_metadata_targets}
                    DESTINATION
                        ${library_install_ufid_relative_path}
                RUNTIME
                    DESTINATION
                        "bin"
                    COMPONENT
                        runtime
                LIBRARY
                    DESTINATION
                        ${library_install_ufid_relative_path}
                    COMPONENT
                        runtime
                ARCHIVE
                    DESTINATION
                        ${library_install_ufid_relative_path}
                    COMPONENT
                        development
            )

            foreach (alternate_install_ufid ${alternate_install_ufid_list})
                set(canonical_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}${target_output_name}${CMAKE_STATIC_LIBRARY_SUFFIX})

                if ("${alternate_install_ufid}" STREQUAL "!")
                    set(alternate_library_name ${canonical_library_name})
                else()
                    set(alternate_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}${target_output_name}.${alternate_install_ufid}${CMAKE_STATIC_LIBRARY_SUFFIX})
                endif()

                install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_library_name}\")")

                ntf_path_normalize(
                    OUTPUT link_target
                    INPUT  "${install_ufid}/${canonical_library_name}"
                    NATIVE ESCAPE)

                install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_library_name} SYMBOLIC)")

            endforeach()

            export(
                EXPORT
                    ${install_cmake_metadata_targets}
                FILE
                    "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_targets}.cmake"

                # TODO: Export the target into a CMake namespace, if advisable.
                # NAMESPACE ${CMAKE_PROJECT_NAME}::
            )

            write_basic_package_version_file(
                "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config_version}.cmake"
                VERSION
                    ${CMAKE_PROJECT_VERSION}
                COMPATIBILITY
                    AnyNewerVersion
            )

            # Recursively find dependencies of each "requires" of this
            # target and manually generate it into the
            # "${target}-config.cmake" file. Ideally, this should be done
            # automatically by CMake itself but no version of CMake currently
            # supports such a feature.
            #
            # See the issue described at
            # https://gitlab.kitware.com/cmake/cmake/-/issues/20511

            ntf_target_requires_get(TARGET ${target} OUTPUT target_requires)

            set(target_config_cmake_content "")

            string(APPEND target_config_cmake_content "\
include(CMakeFindDependencyMacro)\n\
if (NOT TARGET ${target})\n\
    if (UNIX)\n\
        find_dependency(Threads)\n\
    endif()\n\
")

    set(target_requires_reversed ${target_requires})
    list(REVERSE target_requires_reversed)
    foreach (dependency ${target_requires_reversed})
            string(APPEND target_config_cmake_content "\
    find_dependency(${dependency} PATHS \"\${CMAKE_CURRENT_LIST_DIR}\" \"\${CMAKE_CURRENT_LIST_DIR}/..\")\n\
")
    endforeach()

            string(APPEND target_config_cmake_content "\
    include(\"\${CMAKE_CURRENT_LIST_DIR}/${install_cmake_metadata_targets}.cmake\")\n\
endif()\n\
")

            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config}.cmake"
                ${target_config_cmake_content})

            install(
                EXPORT
                    ${install_cmake_metadata_targets}
                FILE
                    ${install_cmake_metadata_targets}.cmake
                DESTINATION
                    ${cmake_metadata_relative_path}
                COMPONENT
                    development
                # TODO: Export the target into a CMake namespace, if advisable.
                # NAMESPACE ${CMAKE_PROJECT_NAME}::
            )

            install(
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config}.cmake
                    ${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config_version}.cmake
                DESTINATION
                    ${cmake_metadata_relative_path}
                COMPONENT
                    development
            )

            # Install symlinks.

            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_targets}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_targets_build}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_config}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_config_version}.cmake\")")


            install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake)")

            ntf_path_normalize(
                OUTPUT link_target
                INPUT "../${install_ufid}/cmake/${target}"
                NATIVE ESCAPE)

            install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target} SYMBOLIC)")

            foreach (alternate_install_ufid ${alternate_install_ufid_list})
                if (NOT "${alternate_install_ufid}" STREQUAL "!" AND
                    NOT "${alternate_install_ufid}" STREQUAL "${install_ufid}")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_targets}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_targets_build}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_config}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_config_version}.cmake\")")

                    install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake)")


                    ntf_path_normalize(
                        OUTPUT link_target
                        INPUT "../../${install_ufid}/cmake/${target}"
                        NATIVE ESCAPE)

                    install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target} SYMBOLIC)")
                endif()
            endforeach()

        endif()
    else()
        if (NOT ${target_private})
            set(archive_basename ${CMAKE_STATIC_LIBRARY_PREFIX}${target_output_name}${CMAKE_STATIC_LIBRARY_SUFFIX})

            ntf_repository_archive_output_path_get(OUTPUT archive_output_path)

            cmake_path(
                APPEND
                archive_source_path
                ${archive_output_path} ${archive_basename})

            set(archive_destination_relative_path "${lib_name}")

            install(
                FILES
                    ${archive_source_path}
                DESTINATION
                    ${archive_destination_relative_path}
                COMPONENT
                    development
            )
        endif()
    endif()

    # Install pkg-config meta-data. TODO: Make this a configuration option.

    set(install_pkgconfig_metadata TRUE)

    if (${install_pkgconfig_metadata})
        if (NOT ${target_private})
            ntf_target_description_get(TARGET ${target} OUTPUT target_description)
            ntf_target_requires_get(TARGET ${target} OUTPUT target_requires)
            ntf_target_libs_get(TARGET ${target} OUTPUT target_libs)

            string (REPLACE ";" " " target_requires_string "${target_requires}")

            set(target_libs_string "")
            foreach (target_libs_entry ${target_libs})
                if (TARGET ${target_libs_entry})
                    get_target_property(target_libs_entry_name ${target_libs_entry} OUTPUT_NAME)
                    if ("${target_libs_entry_name}" STREQUAL "" OR
                        "${target_libs_entry_name}" STREQUAL "target_libs_entry_name-NOTFOUND")
                        set(target_libs_entry_name ${target_libs_entry})
                    endif()
                else()
                    set(target_libs_entry_name ${target_libs_entry})
                endif()

                set(target_libs_string "${target_libs_string} -l${target_libs_entry_name}")
            endforeach()

            # TODO: Set 'prefix' to "${pcfiledir}/../.." in the top-level
            # pkgconfig directory, set it to "${pcfiledir}/../../.." in the
            # UFID-specific pkgconfig directory, and do not install symlinks
            # in each UFID-specific pkgconfig directory, but separate copies
            # with prefix set to the directory hierarchy appropriately.

            set(install_pkgconfig_metadata_with_dashes TRUE)

            set(target_pkgconfig_name ${target})
            if (${install_pkgconfig_metadata_with_dashes})
                string (REPLACE "_" "-" target_pkgconfig_name "${target}")
            endif()

            set(target_pc_content "\
prefix=${CMAKE_INSTALL_PREFIX}\n\
libdir=\${prefix}/${library_install_ufid_relative_path}\n\
includedir=\${prefix}/include\n\
\n\
Name: ${target_pkgconfig_name}\n\
Description: ${target_description}\n\
URL: ${CMAKE_PROJECT_HOMEPAGE_URL}\n\
Version: ${CMAKE_PROJECT_VERSION}\n\
Requires: \n\
Requires.private: ${target_requires_string}\n\
Cflags: -I\${includedir}\n\
Libs: -L\${libdir} -l${target_output_name}\n\
Libs.private:${target_libs_string}\n\
")

            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${target_pkgconfig_name}.pc"
                ${target_pc_content})

            install(
                FILES ${CMAKE_CURRENT_BINARY_DIR}/${target_pkgconfig_name}.pc
                DESTINATION ${library_install_ufid_relative_path}/pkgconfig
            )

            # Install symlinks.

            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig/${target_pkgconfig_name}.pc\")")

            install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig)")

            ntf_path_normalize(
                OUTPUT link_target
                INPUT  "../${install_ufid}/pkgconfig/${target_pkgconfig_name}.pc"
                NATIVE ESCAPE)

            install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig/${target_pkgconfig_name}.pc SYMBOLIC)")

            foreach (alternate_install_ufid ${alternate_install_ufid_list})
                if (NOT "${alternate_install_ufid}" STREQUAL "!" AND
                    NOT "${alternate_install_ufid}" STREQUAL "${install_ufid}")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig/${target_pkgconfig_name}.pc\")")

                    install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig)")

                    ntf_path_normalize(
                        OUTPUT link_target
                        INPUT "../../${install_ufid}/pkgconfig/${target_pkgconfig_name}.pc"
                        NATIVE ESCAPE)

                    install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig/${target_pkgconfig_name}.pc SYMBOLIC)")
                endif()
            endforeach()

        endif()

    endif()

endfunction()

# Classify a name as a group, package, adapter, or other.
#
# NAME   - The name to classify
# OUTPUT - The variable name set in the parent scope: either
#          "GROUP", "PACKAGE", "ADAPTER", or "OTHER".
function (ntf_taxonomy)
    cmake_parse_arguments(
        ARG "" "NAME;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})
    ntf_assert_defined(${ARG_OUTPUT})

    set(name ${ARG_NAME})
    set(result "OTHER")

    string(LENGTH ${name} name_length)

    if (${name_length} EQUAL 3)
        set(result "GROUP")
    else()
        string(SUBSTRING ${name} 0 2 name_prefix)
        if ("${name_prefix}" STREQUAL "a_")
            set(result "ADAPTER")
        elseif ("${name_prefix}" STREQUAL "s_")
            set(result "ADAPTER")
        elseif ("${name_prefix}" STREQUAL "m_")
            set(result "APPLICATION")
        else()
            if (${name_length} GREATER 3)
                string(SUBSTRING ${name} 0 3 group)
                if (TARGET ${group})
                    set(result "PACKAGE")
                endif()
            endif()
        endif()
    endif()

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)

endfunction()

# Scan meta-data and automatically add the corresponding group, package,
# adapter, or application.
#
# NAME - The target name
function (ntf_scan)
    cmake_parse_arguments(
        ARG "" "NAME" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})

    set(name ${ARG_NAME})
    set(taxonomy "")

    ntf_taxonomy(NAME ${name} OUTPUT taxonomy)

    if ("${taxonomy}" STREQUAL "GROUP")
        ntf_die("Unsupported taxonomy for '${name}': ${taxonomy}")
    elseif ("${taxonomy}" STREQUAL "PACKAGE")
        ntf_package(NAME ${name})
        ntf_package_dependencies(TARGET ${name})
        ntf_package_members(TARGET ${name})
        ntf_package_end(NAME ${name})
    elseif("${taxonomy}" STREQUAL "ADAPTER")

    elseif("${taxonomy}" STREQUAL "APPLICATION")
        ntf_die("Unsupported taxonomy for '${name}': ${taxonomy}")
    else()
        ntf_die("Unsupported taxonomy for '${name}': ${taxonomy}")
    endif()

endfunction()

function (ntf_target_build_info)
    cmake_parse_arguments(
        NTF_TARGET "" "NAME;PATH" "" ${ARGN})

    if ("${NTF_TARGET_NAME}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: NAME")
    endif()

    if ("${NTF_TARGET_PATH}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: PATH")
    endif()

    string(TOLOWER ${NTF_TARGET_NAME} target_lowercase)
    string(TOUPPER ${NTF_TARGET_NAME} target_uppercase)

    set(target ${target_lowercase})

    if (IS_ABSOLUTE ${NTF_TARGET_PATH})
        set(target_path ${NTF_TARGET_PATH})
    else()
        ntf_repository_path_get(OUTPUT repository_path)
        file(REAL_PATH ${NTF_TARGET_PATH} target_path
             BASE_DIRECTORY ${repository_path})
    endif()

    execute_process(
      COMMAND git rev-parse --abbrev-ref HEAD
      WORKING_DIRECTORY ${target_path}
      OUTPUT_VARIABLE GIT_BRANCH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
      COMMAND git log -1 --format=%H
      WORKING_DIRECTORY ${target_path}
      OUTPUT_VARIABLE GIT_COMMIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
      COMMAND git log -1 --format=%h
      WORKING_DIRECTORY ${target_path}
      OUTPUT_VARIABLE GIT_COMMIT_HASH_ABBREV
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(${target_uppercase}_BUILD_BRANCH 
        "${GIT_BRANCH}" 
        CACHE INTERNAL "" FORCE)

    set(${target_uppercase}_BUILD_COMMIT_HASH 
        "${GIT_COMMIT_HASH}" 
        CACHE INTERNAL "" FORCE)

    set(${target_uppercase}_BUILD_COMMIT_HASH_ABBREV 
        "${GIT_COMMIT_HASH_ABBREV}" 
        CACHE INTERNAL "" FORCE)

endfunction()

# Begin the definition of a group.
#
# NAME        - The name of the group
# PATH        - The path to the group. If the path is relative, the path is
#               interpreted relative to the root of the repository.
# DESCRIPTION - The single line description of the group, as it should appear
#               in package meta-data.
# REQUIRES    - The intra-repository dependencies of the group.
# PRIVATE     - The flag indicating that all headers and artifacts are private
#               and should not be installed.
function (ntf_group)
    cmake_parse_arguments(
        NTF_GROUP "PRIVATE;THIRDPARTY" "NAME;PATH;DESCRIPTION" "REQUIRES" ${ARGN})

    if ("${NTF_GROUP_NAME}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: NAME")
    endif()

    if ("${NTF_GROUP_PATH}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: PATH")
    endif()

    if ("${NTF_GROUP_DESCRIPTION}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: DESCRIPTION")
    endif()

    string(TOLOWER ${NTF_GROUP_NAME} target_lowercase)
    string(TOUPPER ${NTF_GROUP_NAME} target_uppercase)

    set(target ${target_lowercase})

    if (IS_ABSOLUTE ${NTF_GROUP_PATH})
        set(target_path ${NTF_GROUP_PATH})
    else()
        ntf_repository_path_get(OUTPUT repository_path)
        file(REAL_PATH ${NTF_GROUP_PATH} target_path
             BASE_DIRECTORY ${repository_path})
    endif()

    set(target_private ${NTF_GROUP_PRIVATE})
    if ("${target_private}" STREQUAL "")
        set(target_private FALSE)
    endif()

    set(target_thirdparty ${NTF_GROUP_THIRDPARTY})
    if ("${target_thirdparty}" STREQUAL "")
        set(target_thirdparty FALSE)
    endif()

    ntf_repository_archive_output_path_get(OUTPUT archive_output_path)
    ntf_repository_library_output_path_get(OUTPUT library_output_path)
    ntf_repository_runtime_output_path_get(OUTPUT runtime_output_path)

    add_library(${target} STATIC)

    # Set the C standard.

    set_property(TARGET ${target} PROPERTY C_STANDARD 11)

    # Set the C++ standard version unless using xlc, which requires
    # special, non-portable flag for some C++03-like behavior.

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "XL")
        ntf_repository_standard_get(OUTPUT cxx_standard)
        set_property(TARGET ${target} PROPERTY CXX_STANDARD ${cxx_standard})
    endif()

    set_property(TARGET ${target} PROPERTY EXPORT_COMPILE_COMMANDS TRUE)

    set_property(TARGET ${target}
                 PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${archive_output_path})

    set_property(TARGET ${target}
                 PROPERTY LIBRARY_OUTPUT_DIRECTORY ${library_output_path})

    set_property(TARGET ${target}
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY ${runtime_output_path})

    ntf_target_options(TARGET ${target})

    ntf_target_path_set(
        TARGET ${target} VALUE ${target_path})

    ntf_target_description_set(
        TARGET ${target} VALUE ${NTF_GROUP_DESCRIPTION})

    ntf_target_private_set(
        TARGET ${target} VALUE ${target_private})

    ntf_target_thirdparty_set(
        TARGET ${target} VALUE ${target_thirdparty})

    ntf_target_requires_set(
        TARGET ${target} VALUE ${NTF_GROUP_REQUIRES})

    foreach(entry ${NTF_GROUP_REQUIRES})
        set(dependency ${entry})

        if (VERBOSE)
            message(STATUS "NTF Build: linking group '${target}' to target '${dependency}'")
            message(STATUS "         * target_link_libraries(${target} ${dependency})")
        endif()

        target_link_libraries(${target} PUBLIC ${dependency})
    endforeach()

    if (NOT ${target_thirdparty})
        if (NOT TARGET test_${target})
            add_custom_target(
                test_${target}
                COMMAND ${CMAKE_CTEST_COMMAND} -R ${target})
        endif()

        if (NOT TARGET format_${target})
            add_custom_target(format_${target})
            add_dependencies(format format_${target})
        endif()

        if (TARGET documentation)
            add_dependencies(documentation ${target})
        endif()
    endif()

    ntf_target_build_info(NAME ${target} PATH ${target_path})

endfunction()

# Add a dependency to a group, attempting to find the dependency through
# either CMake, pkg-config, or as a raw library.
#
# NAME       - The group.
# DEPENDENCY - The dependency name list.
# OPTIONAL   - Flag indicating the target is linked to the dependency only if
#              it exists, otherwise the dependency must be must found
function (ntf_group_requires)
    cmake_parse_arguments(
        ARG "OPTIONAL" "NAME" "DEPENDENCY" ${ARGN})

    ntf_assert_defined(${ARG_NAME})
    ntf_assert_defined(${ARG_DEPENDENCY})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    foreach (dependency ${ARG_DEPENDENCY})
        ntf_target_link_dependency(
            TARGET ${target} DEPENDENCY ${dependency} OUTPUT result OPTIONAL)

        if (NOT ${result} AND NOT ${ARG_OPTIONAL})
            ntf_die("The target '${target}' requires dependency '${dependency}' but it cannot be found")
        endif()
    endforeach()
endfunction()

# End the definition of a group.
function (ntf_group_end)
    cmake_parse_arguments(
        ARG "" "NAME" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    ntf_target_private_get(TARGET ${target} OUTPUT target_private)

    ntf_target_options_common_epilog(${target})

    ntf_repository_install_refroot_get(OUTPUT install_refroot)
    ntf_repository_install_prefix_get(OUTPUT install_prefix)

    ntf_repository_build_ufid_get(OUTPUT build_ufid)
    ntf_repository_install_ufid_get(OUTPUT install_ufid)

    ntf_ufid_string_has(UFID ${build_ufid} FLAG "64" OUTPUT is_64_bit)

    get_target_property(target_output_name ${target} OUTPUT_NAME)
    if ("${target_output_name}" STREQUAL "" OR
        "${target_output_name}" STREQUAL "target_output_name-NOTFOUND")
        set(target_output_name "${target}")
    endif()

    if (${is_64_bit} AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(lib_name "lib64" CACHE INTERNAL "")
    else()
        set(lib_name "lib" CACHE INTERNAL "")
    endif()

    # Set the relative path to the library directory under the prefix. For
    # example: lib64

    set(library_relative_path ${lib_name})

    # Set the relative path to the UFID-specific library directory under the
    # prefix. For example: lib64/dbg_exc_mt

    set(library_install_ufid_relative_path "${lib_name}/${install_ufid}")

    # Define the installation UFIDs that are aliases for this UFID. Building
    # the alias UFID is synonymous with the original UFID.

    set(alias_install_ufid_list)

    set(alias_install_ufid_pic ${install_ufid})
    ntf_ufid_add(UFID "${alias_install_ufid_pic}" FLAG "pic" OUTPUT alias_install_ufid_pic)
    list(APPEND alias_install_ufid_list ${alias_install_ufid_pic})

    if(${is_64_bit})
        set(alias_install_ufid_pic_64 ${install_ufid})
        ntf_ufid_add(UFID "${alias_install_ufid_pic_64}" FLAG "pic" OUTPUT alias_install_ufid_pic_64)
        ntf_ufid_add(UFID "${alias_install_ufid_pic_64}" FLAG "64" OUTPUT alias_install_ufid_pic_64)
        list(APPEND alias_install_ufid_list ${alias_install_ufid_pic_64})
    endif()

    # Define the alternate install UFIDs. Other build systems may expect
    # these paths to exist.

    set(alternate_install_ufid_list)

    list(APPEND alternate_install_ufid_list "!")
    list(APPEND alternate_install_ufid_list "${install_ufid}")

    if(${is_64_bit})
        list(APPEND alternate_install_ufid_list "${install_ufid}_64")
    endif()

    list(APPEND alternate_install_ufid_list ${alias_install_ufid_list})

    string (REPLACE "_exc_mt" "" modern_install_ufid_list "${alternate_install_ufid_list}")
    list(POP_FRONT modern_install_ufid_list)
    list(APPEND alternate_install_ufid_list ${modern_install_ufid_list})

    if (VERBOSE)
        message(STATUS "build_ufid:                  ${build_ufid}")
        message(STATUS "install_ufid:                ${install_ufid}")
        message(STATUS "alias_install_ufid_list:     ${alias_install_ufid_list}")
        message(STATUS "alternate_install_ufid_list: ${alternate_install_ufid_list}")
        message(STATUS "modern_install_ufid_list:    ${modern_install_ufid_list}")
    endif()

    # Install CMake target meta-data. TODO: Make this a configuration option.

    set(install_cmake_metadata TRUE)

    if (${install_cmake_metadata})
        if (NOT ${target_private})
            # CMake meta data names, version 1:

            # ${target}Config.cmake
            # ${target}ConfigVersion.cmake
            # ${target}Targets.cmake
            # ${target}Targets-debug.cmake

            # CMake meta data names, version 2:

            # ${target}-config.cmake
            # ${target}-config-version.cmake
            # ${target}-targets.cmake
            # ${target}-targets-debug.cmake

            set(cmake_metadata_relative_path
                ${library_install_ufid_relative_path}/cmake/${target})

            # Determine which CMake meta data case style to use.

            if (EXISTS "${install_refroot}/${install_prefix}/${library_relative_path}/cmake/${target}/${target}Config.cmake")
                set(install_cmake_metadata_style 1)
            else()
                set(install_cmake_metadata_style 2)
            endif()

            string(TOLOWER "${CMAKE_BUILD_TYPE}" buildTypeLowercase)

            if (${install_cmake_metadata_style} EQUAL 1)
                set(install_cmake_metadata_config "${target}Config")
                set(install_cmake_metadata_config_version "${target}ConfigVersion")
                set(install_cmake_metadata_targets "${target}Targets")
                set(install_cmake_metadata_targets_build "${target}Targets-${buildTypeLowercase}")
            else()
                set(install_cmake_metadata_config "${target}-config")
                set(install_cmake_metadata_config_version "${target}-config-version")
                set(install_cmake_metadata_targets "${target}-targets")
                set(install_cmake_metadata_targets_build "${target}-targets-${buildTypeLowercase}")
            endif()

            install(
                TARGETS
                    ${target}
                EXPORT
                    ${install_cmake_metadata_targets}
                    DESTINATION
                        ${library_install_ufid_relative_path}
                RUNTIME
                    DESTINATION
                        "bin"
                    COMPONENT
                        runtime
                LIBRARY
                    DESTINATION
                        ${library_install_ufid_relative_path}
                    COMPONENT
                        runtime
                ARCHIVE
                    DESTINATION
                        ${library_install_ufid_relative_path}
                    COMPONENT
                        development
            )

            foreach (alternate_install_ufid ${alternate_install_ufid_list})
                set(canonical_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}${target_output_name}${CMAKE_STATIC_LIBRARY_SUFFIX})

                if ("${alternate_install_ufid}" STREQUAL "!")
                    set(alternate_library_name ${canonical_library_name})
                else()
                    set(alternate_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}${target_output_name}.${alternate_install_ufid}${CMAKE_STATIC_LIBRARY_SUFFIX})
                endif()

                install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_library_name}\")")

                ntf_path_normalize(
                    OUTPUT link_target
                    INPUT "${install_ufid}/${canonical_library_name}"
                    NATIVE ESCAPE)

                install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_library_name} SYMBOLIC)")

            endforeach()

            export(
                EXPORT
                    ${install_cmake_metadata_targets}
                FILE
                    "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_targets}.cmake"

                # TODO: Export the target into a CMake namespace, if advisable.
                # NAMESPACE ${CMAKE_PROJECT_NAME}::
            )

            write_basic_package_version_file(
                "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config_version}.cmake"
                VERSION
                    ${CMAKE_PROJECT_VERSION}
                COMPATIBILITY
                    AnyNewerVersion
            )

            # Recursively find dependencies of each "requires" of this
            # target and manually generate it into the
            # "${target}-config.cmake" file. Ideally, this should be done
            # automatically by CMake itself but no version of CMake currently
            # supports such a feature.
            #
            # See the issue described at
            # https://gitlab.kitware.com/cmake/cmake/-/issues/20511

            ntf_target_requires_get(TARGET ${target} OUTPUT target_requires)

            set(target_config_cmake_content "")

            string(APPEND target_config_cmake_content "\
include(CMakeFindDependencyMacro)\n\
if (NOT TARGET ${target})\n\
    if (UNIX)\n\
        find_dependency(Threads)\n\
    endif()\n\
")

    set(target_requires_reversed ${target_requires})
    list(REVERSE target_requires_reversed)
    foreach (dependency ${target_requires_reversed})
            string(APPEND target_config_cmake_content "\
    find_dependency(${dependency} PATHS \"\${CMAKE_CURRENT_LIST_DIR}\" \"\${CMAKE_CURRENT_LIST_DIR}/..\")\n\
")
    endforeach()

            string(APPEND target_config_cmake_content "\
    include(\"\${CMAKE_CURRENT_LIST_DIR}/${install_cmake_metadata_targets}.cmake\")\n\
endif()\n\
")

            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config}.cmake"
                ${target_config_cmake_content})

            install(
                EXPORT
                    ${install_cmake_metadata_targets}
                FILE
                    ${install_cmake_metadata_targets}.cmake
                DESTINATION
                    ${cmake_metadata_relative_path}
                COMPONENT
                    development
                # TODO: Export the target into a CMake namespace, if advisable.
                # NAMESPACE ${CMAKE_PROJECT_NAME}::
            )

            install(
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config}.cmake
                    ${CMAKE_CURRENT_BINARY_DIR}/${install_cmake_metadata_config_version}.cmake
                DESTINATION
                    ${cmake_metadata_relative_path}
                COMPONENT
                    development
            )

            # Install symlinks.

            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_targets}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_targets_build}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_config}.cmake\")")
            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target}/${install_cmake_metadata_config_version}.cmake\")")


            install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake)")

            ntf_path_normalize(
                OUTPUT link_target
                INPUT "../${install_ufid}/cmake/${target}"
                NATIVE ESCAPE)

            install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/cmake/${target} SYMBOLIC)")

            foreach (alternate_install_ufid ${alternate_install_ufid_list})
                if (NOT "${alternate_install_ufid}" STREQUAL "!" AND
                    NOT "${alternate_install_ufid}" STREQUAL "${install_ufid}")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_targets}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_targets_build}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_config}.cmake\")")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target}/${install_cmake_metadata_config_version}.cmake\")")

                    install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake)")


                    ntf_path_normalize(
                        OUTPUT link_target
                        INPUT "../../${install_ufid}/cmake/${target}"
                        NATIVE ESCAPE)

                    install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/cmake/${target} SYMBOLIC)")
                endif()
            endforeach()

        endif()
    else()
        if (NOT ${target_private})
            set(archive_basename ${CMAKE_STATIC_LIBRARY_PREFIX}${target_output_name}${CMAKE_STATIC_LIBRARY_SUFFIX})

            ntf_repository_archive_output_path_get(OUTPUT archive_output_path)

            cmake_path(
                APPEND
                archive_source_path
                ${archive_output_path} ${archive_basename})

            set(archive_destination_relative_path "${lib_name}")

            install(
                FILES
                    ${archive_source_path}
                DESTINATION
                    ${archive_destination_relative_path}
                COMPONENT
                    development
            )
        endif()
    endif()

    # Install pkg-config meta-data. TODO: Make this a configuration option.

    set(install_pkgconfig_metadata TRUE)

    if (${install_pkgconfig_metadata})
        if (NOT ${target_private})
            ntf_target_description_get(TARGET ${target} OUTPUT target_description)
            ntf_target_requires_get(TARGET ${target} OUTPUT target_requires)
            ntf_target_libs_get(TARGET ${target} OUTPUT target_libs)

            string (REPLACE ";" " " target_requires_string "${target_requires}")

            set(target_libs_string "")
            foreach (target_libs_entry ${target_libs})
                if (TARGET ${target_libs_entry})
                    get_target_property(target_libs_entry_name ${target_libs_entry} OUTPUT_NAME)
                    if ("${target_libs_entry_name}" STREQUAL "" OR
                        "${target_libs_entry_name}" STREQUAL "target_libs_entry_name-NOTFOUND")
                        set(target_libs_entry_name ${target_libs_entry})
                    endif()
                else()
                    set(target_libs_entry_name ${target_libs_entry})
                endif()

                set(target_libs_string "${target_libs_string} -l${target_libs_entry_name}")
            endforeach()

            # TODO: Set 'prefix' to "${pcfiledir}/../.." in the top-level
            # pkgconfig directory, set it to "${pcfiledir}/../../.." in the
            # UFID-specific pkgconfig directory, and do not install symlinks
            # in each UFID-specific pkgconfig directory, but separate copies
            # with prefix set to the directory hierarchy appropriately.

            set(target_pc_content "\
prefix=${CMAKE_INSTALL_PREFIX}\n\
libdir=\${prefix}/${library_install_ufid_relative_path}\n\
includedir=\${prefix}/include\n\
\n\
Name: ${target}\n\
Description: ${target_description}\n\
URL: ${CMAKE_PROJECT_HOMEPAGE_URL}\n\
Version: ${CMAKE_PROJECT_VERSION}\n\
Requires: \n\
Requires.private: ${target_requires_string}\n\
Cflags: -I\${includedir}\n\
Libs: -L\${libdir} -l${target_output_name}\n\
Libs.private:${target_libs_string}\n\
")

            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${target}.pc"
                ${target_pc_content})

            install(
                FILES ${CMAKE_CURRENT_BINARY_DIR}/${target}.pc
                DESTINATION ${library_install_ufid_relative_path}/pkgconfig
            )

            # Install symlinks.

            install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig/${target}.pc\")")

            install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig)")

            ntf_path_normalize(
                OUTPUT link_target
                INPUT "../${install_ufid}/pkgconfig/${target}.pc"
                NATIVE ESCAPE)

            install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/pkgconfig/${target}.pc SYMBOLIC)")

            foreach (alternate_install_ufid ${alternate_install_ufid_list})
                if (NOT "${alternate_install_ufid}" STREQUAL "!" AND
                    NOT "${alternate_install_ufid}" STREQUAL "${install_ufid}")
                    install(CODE "message(\"-- Installing: \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig/${target}.pc\")")

                    install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig)")

                    ntf_path_normalize(
                        OUTPUT link_target
                        INPUT "../../${install_ufid}/pkgconfig/${target}.pc"
                        NATIVE ESCAPE)

                    install(CODE "file(CREATE_LINK ${link_target} \${CMAKE_INSTALL_PREFIX}/${library_relative_path}/${alternate_install_ufid}/pkgconfig/${target}.pc SYMBOLIC)")
                endif()
            endforeach()

        endif()

    endif()

endfunction()

# Begin the definition of a package that is a member of a group.
#
# NAME  - The name of the package.
#
# PATH  - The path to the package. If relative, the path is interpreted
#         relative to the root of the repository. If ommited and the group
#         is defined, the path is inferred from the name of the package and the
#         path to the group.
#
# GROUP - If defined, the group name of the group to which the package should
#         be added. Otherwise, the group is inferred from the first three
#         characters in the package name, and if the group is already defined,
#         the package is automatically added to the group.
#
# REQUIRES - The intra-group package dependencies of this package.
function (ntf_package)
    cmake_parse_arguments(
        NTF_PACKAGE "PRIVATE;THIRDPARTY" "NAME;PATH;GROUP" "REQUIRES" ${ARGN})

    if ("${NTF_PACKAGE_NAME}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: NAME")
    endif()

    string(TOLOWER ${NTF_PACKAGE_NAME} target_lowercase)
    string(TOUPPER ${NTF_PACKAGE_NAME} target_uppercase)

    set(target ${target_lowercase})

    if (NOT "${NTF_PACKAGE_GROUP}" STREQUAL "")
        string(TOLOWER ${NTF_PACKAGE_GROUP} group_lowercase)
        string(TOUPPER ${NTF_PACKAGE_GROUP} group_uppercase)

        set(group ${group_lowercase})

        ntf_target_path_get(TARGET ${group} OUTPUT group_path)

        if ("${group_path}" STREQUAL "")
            message(FATAL_ERROR "Invalid parameter: PATH")
        endif()

        ntf_target_private_get(TARGET ${group} OUTPUT group_private)
        ntf_target_thirdparty_get(TARGET ${group} OUTPUT group_thirdparty)
    else()
        string(SUBSTRING ${target} 0 3 group)

        string(TOLOWER ${group} group_lowercase)
        string(TOUPPER ${group} group_uppercase)

        ntf_target_path_get(TARGET ${group} OUTPUT group_path)

        if ("${group_path}" STREQUAL "")
            message(FATAL_ERROR "Invalid parameter: PATH")
        endif()

        ntf_target_private_get(TARGET ${group} OUTPUT group_private)
        ntf_target_thirdparty_get(TARGET ${group} OUTPUT group_thirdparty)
    endif()

    if (NOT "${NTF_PACKAGE_PATH}" STREQUAL "")
        if (IS_ABSOLUTE ${NTF_PACKAGE_PATH})
            set(target_path ${NTF_PACKAGE_PATH})
        else()
            ntf_repository_path_get(OUTPUT repository_path)
            file(REAL_PATH ${NTF_PACKAGE_PATH} target_path
                BASE_DIRECTORY ${repository_path})
        endif()
    else()
        cmake_path(APPEND target_path ${group_path} ${target})
    endif()

    set(target_private ${NTF_PACKAGE_PRIVATE})
    if (NOT ${target_private} AND ${group_private})
        set(target_private TRUE)
    endif()

    set(target_thirdparty ${NTF_PACKAGE_THIRDPARTY})
    if (NOT ${target_thirdparty} AND ${group_thirdparty})
        set(target_thirdparty TRUE)
    endif()

    ntf_repository_archive_output_path_get(OUTPUT archive_output_path)
    ntf_repository_library_output_path_get(OUTPUT library_output_path)
    ntf_repository_runtime_output_path_get(OUTPUT runtime_output_path)

    add_library(${target} OBJECT)

    # Set the C standard.

    set_property(TARGET ${target} PROPERTY C_STANDARD 11)

    # Set the C++ standard version unless using xlc, which requires
    # special, non-portable flag for some C++03-like behavior.

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "XL")
        ntf_repository_standard_get(OUTPUT cxx_standard)
        set_property(TARGET ${target} PROPERTY CXX_STANDARD ${cxx_standard})
    endif()

    set_property(TARGET ${target} PROPERTY EXPORT_COMPILE_COMMANDS TRUE)

    set_property(TARGET ${target}
                 PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${archive_output_path})

    set_property(TARGET ${target}
                 PROPERTY LIBRARY_OUTPUT_DIRECTORY ${library_output_path})

    set_property(TARGET ${target}
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY ${runtime_output_path})

    ntf_target_options(TARGET ${target})

    ntf_target_path_set(
        TARGET ${target} VALUE ${target_path})

    ntf_target_private_set(
        TARGET ${target} VALUE ${target_private})

    ntf_target_thirdparty_set(
        TARGET ${target} VALUE ${target_thirdparty})

    ntf_target_requires_set(
        TARGET ${target} VALUE ${NTF_PACKAGE_REQUIRES})

    if (${target_thirdparty})
        target_include_directories(
            ${target}
            SYSTEM PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
            $<BUILD_INTERFACE:${target_path}>
        )

        target_include_directories(
            ${group}
            SYSTEM PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
            $<BUILD_INTERFACE:${target_path}>
        )
    else()
        target_include_directories(
            ${target}
            PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
            $<BUILD_INTERFACE:${target_path}>
        )

        target_include_directories(
            ${group}
            PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
            $<BUILD_INTERFACE:${target_path}>
        )
    endif()

    if (VERBOSE)
        message(STATUS "NTF Build: linking group '${group}' to target '${target}'")
        message(STATUS "         * target_link_libraries(${group} ${target})")
    endif()

    target_sources(${group} PRIVATE $<TARGET_OBJECTS:${target}>)

    foreach(entry ${NTF_PACKAGE_REQUIRES})
        set(dependency ${entry})

        if (VERBOSE)
            message(STATUS "NTF Build: linking package '${target}' to target '${dependency}'")
            message(STATUS "         * target_link_libraries(${target} ${dependency})")
        endif()

        target_link_libraries(${target} PUBLIC ${dependency})
    endforeach()

    get_property(
        group_link_libraries TARGET ${group} PROPERTY LINK_LIBRARIES)

    foreach(group_link_library ${group_link_libraries})
        if (NOT "${group_link_library}" STREQUAL "${target}")
            set(group_link_library_type "UNKNOWN")
            if (TARGET ${group_link_library})
                get_property(group_link_library_type
                             TARGET ${group_link_library} PROPERTY TYPE)
            endif()

            string(SUBSTRING ${group_link_library} 0 1 group_link_library_head)

            if (NOT "${group_link_library_type}" STREQUAL "OBJECT_LIBARY" AND
                NOT "${group_link_library_head}" STREQUAL "$")
                if (VERBOSE)
                    message(STATUS "NTF Build: linking package '${target}' to target '${group_link_library}'")
                    message(STATUS "         * target_link_libraries(${target} ${group_link_library})")
                endif()

                target_link_libraries(${target} PUBLIC ${group_link_library})
            endif()
        endif()
    endforeach()

    if (NOT ${target_thirdparty})
        if (NOT TARGET test_${target})
            add_custom_target(
                test_${target}
                COMMAND ${CMAKE_CTEST_COMMAND} -R ${target})
        endif()

        if (NOT TARGET format_${target})
            add_custom_target(format_${target})
            add_dependencies(format_${group} format_${target})
        endif()
    endif()

endfunction()

# Add inter-group package dependendencies defined in a meta-data file.
function (ntf_package_dependencies)
    cmake_parse_arguments(
        NTF_PACKAGE_MEMBERS "" "TARGET;PATH;BASE" "" ${ARGN})

    if ("${NTF_PACKAGE_MEMBERS_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: TARGET")
    endif()

    set(base "${ARG_BASE}")
    if ("${base}" STREQUAL "")
        set(base ${target})
    endif()

    ntf_nomenclature_target(TARGET ${NTF_PACKAGE_MEMBERS_TARGET} OUTPUT target)
    ntf_assert_target_defined(${target})

    ntf_target_path_get(TARGET ${target} OUTPUT target_path)

    if ("${target_path}" STREQUAL "")
        message(FATAL_ERROR "The target is not defined")
    endif()

    set(target_members_path ${NTF_PACKAGE_MEMBERS_PATH})

    if ("${target_members_path}" STREQUAL "")
        set(target_members_path "${target_path}/package/${target}.dep")
    endif()

    ntf_file_metadata(PATH ${target_members_path} OUTPUT dependency_list)

    ntf_target_requires_set(
        TARGET ${target} VALUE ${dependency_list})

    foreach(dependency ${dependency_list})
        if (VERBOSE)
            message(STATUS "Adding package metadata dependency: ${target} -> ${dependency}")
        endif()

        if (VERBOSE)
            message(STATUS "NTF Build: linking package '${target}' to target '${dependency}'")
            message(STATUS "         * target_link_libraries(${target} ${dependency})")
        endif()

        target_link_libraries(${target} PUBLIC ${dependency})
    endforeach()
endfunction()

# Add components to a package defined in a meta-data file.
function (ntf_package_members)
    cmake_parse_arguments(
        NTF_PACKAGE_MEMBERS "" "TARGET;PATH" "" ${ARGN})

    if ("${NTF_PACKAGE_MEMBERS_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Invalid parameter: TARGET")
    endif()

    ntf_nomenclature_target(TARGET ${NTF_PACKAGE_MEMBERS_TARGET} OUTPUT target)
    ntf_assert_target_defined(${target})

    ntf_target_path_get(TARGET ${target} OUTPUT target_path)

    if ("${target_path}" STREQUAL "")
        message(FATAL_ERROR "The target is not defined")
    endif()

    set(target_members_path ${NTF_PACKAGE_MEMBERS_PATH})

    if ("${target_members_path}" STREQUAL "")
        set(target_members_path "${target_path}/package/${target}.mem")
    endif()

    ntf_file_metadata(PATH ${target_members_path} OUTPUT component_list)

    unset(target)

    foreach(component ${component_list})
        if (VERBOSE)
            message(STATUS "Adding package metadata component: ${component}")
        endif()
        ntf_component(NAME "${component}")
    endforeach()
endfunction()

# End the definition of a package that is a member of a group.
function (ntf_package_end)
    cmake_parse_arguments(
        ARG "" "NAME" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})

    ntf_nomenclature_target(TARGET ${ARG_NAME} OUTPUT target)

    ntf_assert_target_defined(${target})

    ntf_target_options_common_epilog(${target})

    # TODO: Implement any post-processing for a package.

endfunction()

# Add a header to a target. This function does not automatically add any
# include directories, but does automatically install the file.
#
# TARGET      - The target to which the header belongs
# PATH        - The path to the header, relative to the path to the target
# DESTINATION - The path to the installed header, relative to the installation
#               prefix
# PRIVATE     - The flag indicating the header should not be installed
# THIRDPARTY  - The flag indicating the header belongs to a thirdparty target.
function (ntf_header)
    cmake_parse_arguments(
        NTF_HEADER "PRIVATE;THIRDPARTY" "PATH;TARGET;DESTINATION" "" ${ARGN})

    if ("${NTF_HEADER_PATH}" STREQUAL "")
        ntf_die("Specify header path")
    endif()

    if ("${NTF_HEADER_TARGET}" STREQUAL "")
        ntf_die("Specify header target")
    endif()

    set(target ${NTF_HEADER_TARGET})

    ntf_target_path_get(TARGET ${target} OUTPUT target_path)

    if ("${target_path}" STREQUAL "")
        message(FATAL_ERROR "The target is not defined")
    endif()

    ntf_target_private_get(TARGET ${target} OUTPUT target_private)
    ntf_target_thirdparty_get(TARGET ${target} OUTPUT target_thirdparty)

    set(header_private ${NTF_HEADER_PRIVATE})
    if (NOT ${header_private} AND ${target_private})
        set(header_private TRUE)
    endif()

    set(header_thirdparty ${NTF_HEADER_THIRDPARTY})
    if (NOT ${header_thirdparty} AND ${target_thirdparty})
        set(header_thirdparty TRUE)
    endif()

    if (NOT IS_ABSOLUTE ${NTF_HEADER_PATH})
        file(REAL_PATH ${NTF_HEADER_PATH} header_path
             BASE_DIRECTORY ${target_path})
    else()
        set(header_path ${NTF_HEADER_PATH})
    endif()

    if (NOT EXISTS ${header_path})
        ntf_die("The header '${header_path}' does not exist")
    endif()

    get_filename_component(directory ${header_path} DIRECTORY)

    if (NOT ${header_private})
        get_target_property(target_type ${target} TYPE)
        if (NOT target_type STREQUAL "EXECUTABLE")

            if (NOT "${NTF_HEADER_DESTINATION}" STREQUAL "")
                set(header_destination_relative_path
                    "${NTF_HEADER_DESTINATION}")
            else()
                if (${header_thirdparty})
                    set(header_destination_relative_path "include/${target}")
                else()
                    set(header_destination_relative_path "include")
                endif()
            endif()

            install(
                FILES
                    ${header_path}
                DESTINATION
                    ${header_destination_relative_path}
                COMPONENT
                    development
            )
        endif()
    endif()
endfunction()

# Add a source to a target. This function does not automatically add any
# include directories, nor does it add any test driver.
function (ntf_source)
    cmake_parse_arguments(
        NTF_SOURCE "PRIVATE;THIRDPARTY" "PATH;TARGET" "" ${ARGN})

    if ("${NTF_SOURCE_PATH}" STREQUAL "")
        ntf_die("Specify source path")
    endif()

    if ("${NTF_SOURCE_TARGET}" STREQUAL "")
        ntf_die("Specify source target")
    endif()

    set(target ${NTF_SOURCE_TARGET})

    ntf_target_path_get(TARGET ${target} OUTPUT target_path)

    if ("${target_path}" STREQUAL "")
        message(FATAL_ERROR "The target is not defined")
    endif()

    ntf_target_private_get(TARGET ${target} OUTPUT target_private)
    ntf_target_thirdparty_get(TARGET ${target} OUTPUT target_thirdparty)

    set(source_private ${NTF_SOURCE_PRIVATE})
    if (NOT ${source_private} AND ${target_private})
        set(source_private TRUE)
    endif()

    set(source_thirdparty ${NTF_SOURCE_THIRDPARTY})
    if (NOT ${source_thirdparty} AND ${target_thirdparty})
        set(source_thirdparty TRUE)
    endif()

    if (NOT IS_ABSOLUTE ${NTF_SOURCE_PATH})
        file(REAL_PATH ${NTF_SOURCE_PATH} source_path
             BASE_DIRECTORY ${target_path})
    else()
        set(source_path ${NTF_SOURCE_PATH})
    endif()

    if (NOT EXISTS ${source_path})
        ntf_die("The source '${source_path}' does not exist")
    endif()

    get_filename_component(directory ${source_path} DIRECTORY)

    # Must be private to avoid the object file being compiled twice.
    target_sources(${target} PRIVATE ${source_path})

    # if (${source_thirdparty})
    #     file(REAL_PATH ${directory}/.. directory_parent)
    #
    #     target_include_directories(
    #         ${target}
    #         SYSTEM PUBLIC
    #         $<INSTALL_INTERFACE:include/${target}>
    #         $<BUILD_INTERFACE:${directory}>
    #         $<BUILD_INTERFACE:${directory_parent}>
    #     )
    # else()
    #     target_include_directories(
    #         ${target}
    #         PUBLIC
    #         $<INSTALL_INTERFACE:include>
    #         $<BUILD_INTERFACE:${directory}>
    #     )
    # endif()

endfunction()

# Add a component to a target.
function (ntf_component)
    cmake_parse_arguments(
        NTF_COMPONENT "PRIVATE;THIRDPARTY" "NAME;PATH;TARGET" "" ${ARGN})

    if (NOT "${NTF_COMPONENT_NAME}" STREQUAL "")
        string(TOLOWER ${NTF_COMPONENT_NAME} component_lowercase)
        string(TOUPPER ${NTF_COMPONENT_NAME} component_uppercase)

        set(component ${component_lowercase})
    endif()

    if (NOT "${NTF_COMPONENT_PATH}" STREQUAL "")
        set(component_source_path ${NTF_COMPONENT_PATH})

        get_filename_component(extension ${component_source_path} EXT)

        if ("${extension}" STREQUAL "")
            set(component_source_path "${component_source_path}.cpp")
            set(extension ".cpp")
        else()
            if (NOT "${extension}" STREQUAL ".c" AND
                NOT "${extension}" STREQUAL ".cpp")
                message(FATAL_ERROR "Invalid parameter: PATH")
            endif()
        endif()
    endif()

    if (NOT "${NTF_COMPONENT_TARGET}" STREQUAL "")
        string(TOLOWER ${NTF_COMPONENT_TARGET} target_lowercase)
        string(TOUPPER ${NTF_COMPONENT_TARGET} target_uppercase)

        set(target ${target_lowercase})

        ntf_target_path_get(TARGET ${target} OUTPUT target_path)

        if ("${target_path}" STREQUAL "")
            message(FATAL_ERROR "The target is not defined")
        endif()

        ntf_target_private_get(TARGET ${target} OUTPUT target_private)
        ntf_target_thirdparty_get(TARGET ${target} OUTPUT target_thirdparty)
    endif()

    if ("${component}" STREQUAL "")
        if ("${component_source_path}" STREQUAL "")
            message(FATAL_ERROR
                    "Failed to infer component name from source path")
        endif()

        get_filename_component(component ${component_source_path} NAME_WE)

        string(TOLOWER ${component} component_lowercase)
        string(TOUPPER ${component} component_uppercase)
    endif()

    if ("${target}" STREQUAL "")
        string(FIND ${component} "_" component_split)

        if (${component_split} EQUAL 1)
            string(SUBSTRING ${component} 0  2 target_prefix)
            string(SUBSTRING ${component} 2 -1 target_component)
            string(FIND ${target_component} "_" component_split)

            if (${component_split} EQUAL -1)
                ntf_die("Failed to infer target from component '${component}'")
            endif()

            string(SUBSTRING ${target_component} 0 ${component_split} target)
            set(target "${target_prefix}${target}")
        else()
            if (${component_split} EQUAL -1)
                ntf_die("Failed to infer target from component '${component}'")
            endif()

            string(SUBSTRING ${component} 0 ${component_split} target)
        endif()


        string(TOLOWER ${target} target_lowercase)
        string(TOUPPER ${target} target_uppercase)

        set(target ${target_lowercase})

        ntf_target_path_get(TARGET ${target} OUTPUT target_path)

        if ("${target_path}" STREQUAL "")
            message(FATAL_ERROR "The target is not defined")
        endif()

        ntf_target_private_get(TARGET ${target} OUTPUT target_private)
        ntf_target_thirdparty_get(TARGET ${target} OUTPUT target_thirdparty)
    endif()

    set(component_private ${NTF_COMPONENT_PRIVATE})
    if (NOT ${component_private} AND ${target_private})
        set(component_private TRUE)
    endif()

    set(component_thirdparty ${NTF_COMPONENT_THIRDPARTY})
    if (NOT ${component_thirdparty} AND ${target_thirdparty})
        set(component_thirdparty TRUE)
    endif()

    if ("${component_source_path}" STREQUAL "")
        if ("${target_path}" STREQUAL "")
            message(FATAL_ERROR
                    "Failed to infer component source path from target path")
        endif()

        cmake_path(
            APPEND component_source_path "${target_path}" "${component}.cpp")
        set(extension ".cpp")

    endif()

    if ("${extension}" STREQUAL "")
        message(FATAL_ERROR "The extension is ambiguous")
    endif()

    if ("${component}" STREQUAL "")
        message(FATAL_ERROR "The component name is ambiguous")
    endif()

    if ("${component_source_path}" STREQUAL "")
        message(FATAL_ERROR "The component source path is ambiguous")
    endif()

    if ("${target}" STREQUAL "")
        message(FATAL_ERROR "The component target is ambiguous")
    endif()

    if ("${target_path}" STREQUAL "")
        message(FATAL_ERROR "The component target path is ambiguous")
    endif()

    if (NOT IS_ABSOLUTE ${component_source_path})
        file(REAL_PATH ${component_source_path} component_source_path BASE_DIRECTORY ${target_path})
    endif()

    get_filename_component(directory ${component_source_path} DIRECTORY)

    cmake_path(
        APPEND
        component_header_path "${directory}" "${component}.h"
    )

    cmake_path(
        APPEND
        component_driver_path "${directory}" "${component}.t${extension}"
    )

    set(component_impl_build_target "${component}")
    set(component_test_build_target "${component}.t")

    if (VERBOSE)
        if (${component_private})
            set(component_private_string " [PRIVATE]")
        endif()

        message(STATUS "NTF Build: adding component '${component}' to target '${target}'${component_private_string}")
        message(STATUS "         * header = ${component_header_path}")
        message(STATUS "         * source = ${component_source_path}")
        message(STATUS "         * driver = ${component_driver_path}")
    endif()

    # Must be private to avoid the object file being compiled twice.
    target_sources(${target} PRIVATE ${component_source_path})

    if (${target_thirdparty})
        target_include_directories(
            ${target}
            SYSTEM PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${directory}>
        )
    else()
        target_include_directories(
            ${target}
            PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${directory}>
        )
    endif()

    if(NOT ${target_thirdparty} AND EXISTS ${component_driver_path})
        ntf_repository_archive_output_path_get(OUTPUT archive_output_path)
        ntf_repository_library_output_path_get(OUTPUT library_output_path)
        ntf_repository_runtime_output_path_get(OUTPUT runtime_output_path)

        add_executable(
            ${component_test_build_target}
            EXCLUDE_FROM_ALL
            ${component_driver_path}
        )

        # Set the C standard.

        set_property(TARGET ${component_test_build_target}
                     PROPERTY C_STANDARD 11)

        # Set the C++ standard version unless using xlc, which requires
        # special, non-portable flag for some C++03-like behavior.

        if (NOT CMAKE_CXX_COMPILER_ID MATCHES "XL")
            ntf_repository_standard_get(OUTPUT cxx_standard)
            set_property(TARGET ${component_test_build_target}
                         PROPERTY CXX_STANDARD ${cxx_standard})
        endif()

        set_property(TARGET ${component_test_build_target}
                     PROPERTY EXPORT_COMPILE_COMMANDS TRUE)

        set_property(TARGET ${component_test_build_target}
                     PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${archive_output_path})

        set_property(TARGET ${component_test_build_target}
                    PROPERTY LIBRARY_OUTPUT_DIRECTORY ${library_output_path})

        set_property(TARGET ${component_test_build_target}
                    PROPERTY RUNTIME_OUTPUT_DIRECTORY ${runtime_output_path})

        ntf_target_options(TARGET ${component_test_build_target})

        add_dependencies(
            ${component_test_build_target}
            ${target}
        )

        add_dependencies(
            build_test
            ${component_test_build_target}
        )

        target_link_libraries(
            ${component_test_build_target}
            PUBLIC
            ${target}
        )

        ntf_target_requires_get(TARGET ${target} OUTPUT target_requires)

        foreach(entry ${target_requires})
            set(dependency ${entry})

            if (VERBOSE)
                message(STATUS "NTF Build: linking component test driver '${component_test_build_target}' to target '${dependency}'")
                message(STATUS "         * target_link_libraries(${component_test_build_target} ${dependency})")
            endif()

            target_link_libraries(${component_test_build_target} PUBLIC ${dependency})
        endforeach()

        ntf_target_options_common_epilog(${target})

        add_test(
            NAME ${component_test_build_target}
            COMMAND ${component_test_build_target}
        )

        set_tests_properties(${component_test_build_target} PROPERTIES TIMEOUT 600)

        add_custom_target(
            test_${component}
            COMMAND ${CMAKE_CTEST_COMMAND} -R ${component})

        ntf_ide_vs_code_tasks_add_target(${component_test_build_target})
        ntf_ide_vs_code_launch_add_target(${component_test_build_target})
    endif()

    ntf_repository_path_get(OUTPUT repository_path)

    if (NOT ${target_thirdparty} AND EXISTS ${CLANG_FORMAT_PATH})
        set(format_path_list "")

        if(EXISTS ${component_header_path})
            list(APPEND format_path_list "${component_header_path}")
        endif()

        if(EXISTS ${component_source_path})
            list(APPEND format_path_list "${component_source_path}")
        endif()

        if(EXISTS ${component_driver_path})
            list(APPEND format_path_list "${component_driver_path}")
        endif()

        add_custom_target(
            format_${component}
            COMMAND ${CLANG_FORMAT_PATH} -i ${format_path_list}
            WORKING_DIRECTORY "${repository_path}")

        add_dependencies(format_${target} format_${component})
    endif()

    if (NOT ${component_private})
        if (EXISTS ${component_header_path})
            get_target_property(target_type ${target} TYPE)
            if (NOT target_type STREQUAL "EXECUTABLE")
                set(header_destination_relative_path "include")
                install(
                    FILES
                        ${component_header_path}
                    DESTINATION
                        ${header_destination_relative_path}
                    COMPONENT
                        development
                )
            endif()
        endif()
    endif()

endfunction()

# Add a dependency to a target, attempting to find the dependency through
# CMake.
#
# TARGET     - The target.
# DEPENDENCY - The dependency name.
# OUTPUT     - The variable name set in the parent scope: true if the
#              dependency is found, and false otherwise.
function (ntf_target_link_dependency_by_cmake)
    cmake_parse_arguments(
        ARG "" "TARGET;DEPENDENCY;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_DEPENDENCY})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)

    ntf_assert_target_defined(${target})

    get_property(target_type TARGET ${target} PROPERTY "TYPE")

    set(dependency ${ARG_DEPENDENCY})

    string(TOLOWER ${dependency} dependency_lowercase)
    string(TOUPPER ${dependency} dependency_uppercase)

    ntf_repository_install_refroot_get(OUTPUT install_refroot)
    ntf_repository_install_prefix_get(OUTPUT install_prefix)

    ntf_repository_build_ufid_get(OUTPUT build_ufid)
    ntf_repository_install_ufid_get(OUTPUT install_ufid)

    ntf_ufid_string_has(UFID ${build_ufid} FLAG "64" OUTPUT is_64_bit)

    if (${is_64_bit} AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(lib_name "lib64" CACHE INTERNAL "")
    else()
        set(lib_name "lib" CACHE INTERNAL "")
    endif()

    # Set the library directory under the prefix. For example,
    # "/opt/bb/lib64".

    if ("${install_refroot}" STREQUAL "")
        cmake_path(
            SET prefixLibraryDir
            "${install_prefix}/${lib_name}")
    else()
        cmake_path(
            SET prefixLibraryDir
            "${install_refroot}/${install_prefix}/${lib_name}")
    endif()

    # Set the UFID-specific library directory under the prefix. For example,
    # "/opt/bb/lib64/dbg_exc_mt". Note that certain UFID flags are not present
    # in the path, notably those indicating the architecture or the C++
    # standard version.

    set(prefixLibraryUfidDir "${prefixLibraryDir}/${install_ufid}")

    # Set the expected library suffix for the current platform.

    if (UNIX)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    elseif(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    endif()

    set(dependency_prefix ${dependency_uppercase})

    set(DEPENDENCY_FOUND "${dependency_prefix}_FOUND")
    set(DEPENDENCY_CONFIG "${dependency_prefix}_CONFIG")

    set(originalCmakePrefixPath "")
    if (NOT "${CMAKE_PREFIX_PATH}" STREQUAL "")
        set(originalCmakePrefixPath ${CMAKE_PREFIX_PATH})
    endif()

    set(search_directory_list)

    list(APPEND search_directory_list "${prefixLibraryUfidDir}/cmake")
    list(APPEND search_directory_list "${prefixLibraryDir}/cmake")

    if (NOT "${CMAKE_PREFIX_PATH}" STREQUAL "")
        list(APPEND search_directory_list ${CMAKE_PREFIX_PATH})
    endif()

    if (VERBOSE)
        message(STATUS "Searching for target '${target}' dependency '${dependency}' using CMake in ${search_directory_list}")
    endif()

    # Attempt to find the dependency using CMake.

    # Note: HINTS and PATHS are not propogated to any calls to find_dependency
    # that may be performed inside a dependencies configuration (e.g.
    # <uor>Config.cmake) to automatically transitively find dependencies of the
    # dependency. It has been observed that the following call is not
    # sufficient to correctly find meta-data in some directory layouts on
    # some systems in some build configurations, e.g. Solaris 64-bit using
    # a layout strategy like:
    #
    #     <refroot>/<prefix>/lib64/cmake/<uor>/<uor>Config.cmake.
    #
    # find_package(
    #     ${dependency_uppercase} QUIET CONFIG
    #     NAMES ${dependency}
    #     HINTS ${search_directory_list}
    #     PATHS ${search_directory_list}
    #     NO_DEFAULT_PATH
    # )

    foreach (search_directory ${search_directory_list})
        if (VERBOSE)
            message(STATUS "Searching for target '${target}' dependency '${dependency}' using CMake in ${search_directory}")
        endif()

        set(CMAKE_PREFIX_PATH ${search_directory} CACHE INTERNAL "")

        if (VERBOSE)
            find_package(
                ${dependency_uppercase} CONFIG
                NAMES ${dependency}
            )
        else()
            find_package(
                ${dependency_uppercase} QUIET CONFIG
                NAMES ${dependency}
            )
        endif()

        if (NOT ${${DEPENDENCY_FOUND}})
            if (VERBOSE)
                message(STATUS "Target '${target}' dependency '${dependency}' not found by CMake in ${search_directory}")
            endif()
            continue()
        endif()

        if (NOT TARGET ${dependency})
            if (VERBOSE)
                message(STATUS "Target '${target}' dependency '${dependency}' found using cmake but no target defined in ${search_directory}")
            endif()
            continue()
        endif()

        if (VERBOSE)
            message(STATUS "Dependency '${dependency}' found using cmake at '${${DEPENDENCY_CONFIG}}'")
        endif()

        if ("${target_type}" STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(
                ${target} INTERFACE ${dependency})
        else()
            target_link_libraries(
                ${target} PUBLIC ${dependency})
        endif()

        if (NOT "${originalCmakePrefixPath}" STREQUAL "")
            set(CMAKE_PREFIX_PATH ${originalCmakePrefixPath} CACHE INTERNAL "")
        endif()

        set(${ARG_OUTPUT} TRUE PARENT_SCOPE)
        return()
    endforeach()

    if (VERBOSE)
        message(STATUS "Target '${target}' dependency '${dependency}' not found by CMake in ${search_directory_list}")
    endif()

    if (NOT "${originalCmakePrefixPath}" STREQUAL "")
        set(CMAKE_PREFIX_PATH ${originalCmakePrefixPath} CACHE INTERNAL "")
    endif()

    set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
    return()

endfunction()


# Add a dependency to a target, attempting to find the dependency through
# pkg-config.
#
# TARGET     - The target.
# DEPENDENCY - The dependency name.
# OUTPUT     - The variable name set in the parent scope: true if the
#              dependency is found, and false otherwise.
function (ntf_target_link_dependency_by_pkgconfig)
    cmake_parse_arguments(
        ARG "" "TARGET;DEPENDENCY;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_DEPENDENCY})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)

    ntf_assert_target_defined(${target})

    get_property(target_type TARGET ${target} PROPERTY "TYPE")

    set(dependency ${ARG_DEPENDENCY})

    string(TOLOWER ${dependency} dependency_lowercase)
    string(TOUPPER ${dependency} dependency_uppercase)

    ntf_repository_install_refroot_get(OUTPUT install_refroot)
    ntf_repository_install_prefix_get(OUTPUT install_prefix)

    ntf_repository_build_ufid_get(OUTPUT build_ufid)
    ntf_repository_install_ufid_get(OUTPUT install_ufid)

    ntf_ufid_string_has(UFID ${build_ufid} FLAG "64" OUTPUT is_64_bit)

    if (${is_64_bit} AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(lib_name "lib64" CACHE INTERNAL "")
    else()
        set(lib_name "lib" CACHE INTERNAL "")
    endif()

    # If the target already exists, link the target to the dependency.

    if (TARGET ${dependency})
        if ("${target_type}" STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(
                ${target} INTERFACE ${dependency})
        else()
            target_link_libraries(
                ${target} PUBLIC ${dependency})
        endif()
        set(${ARG_OUTPUT} TRUE PARENT_SCOPE)
        return()
    endif()

    # If the target has already been found using pkg-config and imported as an
    # IMPORTED_TARGET, link the target to the dependency.

    if (TARGET "PkgConfig::${dependency}")
        if ("${target_type}" STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(
                ${target} INTERFACE "PkgConfig::${dependency}")
        else()
            target_link_libraries(
                ${target} PUBLIC "PkgConfig::${dependency}")
        endif()
        set(${ARG_OUTPUT} TRUE PARENT_SCOPE)
        return()
    endif()

    # Set the library directory under the prefix. For example,
    # "/opt/bb/lib64".

    if ("${install_refroot}" STREQUAL "")
        cmake_path(
            SET prefixLibraryDir
            "${install_prefix}/${lib_name}")
    else()
        cmake_path(
            SET prefixLibraryDir
            "${install_refroot}/${install_prefix}/${lib_name}")
    endif()

    # Set the UFID-specific library directory under the prefix. For example,
    # "/opt/bb/lib64/dbg_exc_mt". Note that certain UFID flags are not present
    # in the path, notably those indicating the architecture or the C++
    # standard version.

    set(prefixLibraryUfidDir "${prefixLibraryDir}/${install_ufid}")

    # Set the expected library suffix for the current platform.

    if (UNIX)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    elseif(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    endif()

    # Find the pkgconfig package.

    find_program(
        PKG_CONFIG_EXECUTABLE
        NAMES
            pkg-config
        PATHS
            ${prefixLibraryUfidDir}/bin
            ${prefixLibraryDir}/bin
            /opt/bb/${lib_name}/bin
        NO_DEFAULT_PATH)

    find_package(PkgConfig QUIET)

    if (NOT PKG_CONFIG_FOUND)
        if (VERBOSE)
            message(STATUS "Failed to find dependency ${dependency} using pkg-config: pkg-config is not found")
        endif()
        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
        return()
    endif()

    set(dependency_prefix ${dependency_uppercase})
    string(REPLACE "-" "_" dependency_prefix "${dependency_prefix}")

    set(DEPENDENCY_FOUND "${dependency_prefix}_FOUND")

    set(DEPENDENCY_STATIC_INCLUDE_DIRS "${dependency_prefix}_STATIC_INCLUDE_DIRS")
    set(DEPENDENCY_STATIC_LIBRARY_DIRS "${dependency_prefix}_STATIC_LIBRARY_DIRS")
    set(DEPENDENCY_STATIC_LIBRARIES "${dependency_prefix}_STATIC_LIBRARIES")
    set(DEPENDENCY_STATIC_LINK_LIBRARIES "${dependency_prefix}_STATIC_LINK_LIBRARIES")
    set(DEPENDENCY_LIBRARIES "${dependency_prefix}_LIBRARIES")
    set(DEPENDENCY_LINK_LIBRARIES "${dependency_prefix}_LINK_LIBRARIES")

    # Save the original value of PKG_CONFIG_PATH environment variable, if any,
    # and set or override the PKG_CONFIG_PATH environment so that pkg-config
    # searches the library directory and the ufid-specific library directory
    # under the prefix.

    if (NOT "${install_refroot}" STREQUAL "" AND
        NOT "${install_refroot}" STREQUAL "/")
        if(DEFINED ENV{PKG_CONFIG_SYSROOT_DIR})
            set(originalPkgConfigSysRootDir "$ENV{PKG_CONFIG_SYSROOT_DIR}")
            set(ENV{PKG_CONFIG_SYSROOT_DIR} ${install_refroot})
        else()
            set(originalPkgConfigSysRootDir "")
            set(ENV{PKG_CONFIG_SYSROOT_DIR} ${install_refroot})
        endif()
    endif()

    if(DEFINED ENV{PKG_CONFIG_PATH})
        set(originalPkgConfigPath "$ENV{PKG_CONFIG_PATH}")
        set(ENV{PKG_CONFIG_PATH}
            "${prefixLibraryUfidDir}/pkgconfig:${prefixLibraryDir}/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    else()
        set(originalPkgConfigPath "")
        set(ENV{PKG_CONFIG_PATH}
            "${prefixLibraryUfidDir}/pkgconfig:${prefixLibraryDir}/pkgconfig")
    endif()

    # Try to find the dependency using pkg-config.

    if (VERBOSE)
        message(STATUS "Searching for target '${target}' dependency '${dependency}' using pkg-config in: $ENV{PKG_CONFIG_PATH}")
    endif()

    unset(${DEPENDENCY_FOUND})

    string(REPLACE "_" "-" dependency_alternate ${dependency})

    if (NOT ${dependency_alternate} STREQUAL ${dependency})
        if (VERBOSE)
            pkg_search_module(${dependency_prefix}
                              ${dependency}
                              ${dependency_alternate})
        else()
            pkg_search_module(${dependency_prefix}
                              QUIET
                              ${dependency}
                              ${dependency_alternate})
        endif()
    else()
        if (VERBOSE)
            pkg_search_module(${dependency_prefix} ${dependency})
        else()
            pkg_search_module(${dependency_prefix} QUIET ${dependency})
        endif()
    endif()

    # Restore the original value of the PKG_CONFIG_PATH environment variable,
    # if any.

    if(NOT "${originalPkgConfigPath}" STREQUAL "")
        set(ENV{PKG_CONFIG_PATH} "${originalPkgConfigPath}")
    else()
        unset(ENV{PKG_CONFIG_PATH})
    endif()

    if(NOT "${originalPkgConfigSysRootDir}" STREQUAL "")
        set(ENV{PKG_CONFIG_SYSROOT_DIR} "${originalPkgConfigSysRootDir}")
    else()
        unset(ENV{PKG_CONFIG_SYSROOT_DIR})
    endif()

    # Check if the dependency was found.

    if ("${${DEPENDENCY_FOUND}}" STREQUAL "" OR NOT ${${DEPENDENCY_FOUND}})
        if (VERBOSE)
            message(STATUS "Target '${target}' dependency '${dependency}' not found by pkg-config")
        endif()

        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
        return()
    endif()

    if (VERBOSE)
        message(STATUS "Dependency '${dependency}' found using pkg-config")
        message(STATUS "NTF: ${DEPENDENCY_STATIC_INCLUDE_DIRS}:   ${${DEPENDENCY_STATIC_INCLUDE_DIRS}}")
        message(STATUS "NTF: ${DEPENDENCY_STATIC_LIBRARY_DIRS}:   ${${DEPENDENCY_STATIC_LIBRARY_DIRS}}")
        message(STATUS "NTF: ${DEPENDENCY_STATIC_LIBRARIES}:      ${${DEPENDENCY_STATIC_LIBRARIES}}")
        message(STATUS "NTF: ${DEPENDENCY_STATIC_LINK_LIBRARIES}: ${${DEPENDENCY_STATIC_LINK_LIBRARIES}}")
        message(STATUS "NTF: ${DEPENDENCY_LIBRARIES}:             ${${DEPENDENCY_LIBRARIES}}")
        message(STATUS "NTF: ${DEPENDENCY_LINK_LIBRARIES}:        ${${DEPENDENCY_LINK_LIBRARIES}}")
    endif()

    # Create an import target from pkg-config. See _pkg_create_imp_target in
    # https://github.com/Kitware/CMake/blob/master/Modules/FindPkgConfig.cmake
    #
    # The import target is created manually (instead of requesting
    # pkg_search_module create it automatically) for two reasons:
    #
    # 1. We want the target to be named "${dependency}" and not
    # "PkgConfig::${dependency}", which is pkg_search_module's behavior.
    #
    # 2. We want to ensure the header directory is included as a "system"
    # directory (e.g. using -isystem) so that warnings are not produced while
    # including those headers, to facilitate building with warnings treated as
    # errors (not all dependencies can be assumed to be warning-free.)

    add_library(${dependency} INTERFACE IMPORTED GLOBAL)

    # Set the C standard.

    set_property(TARGET ${dependency} PROPERTY C_STANDARD 11)

    # Set the C++ standard version unless using xlc, which requires
    # special, non-portable flag for some C++03-like behavior.

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "XL")
        ntf_repository_standard_get(OUTPUT cxx_standard)
        set_property(TARGET ${dependency}
                     PROPERTY CXX_STANDARD ${cxx_standard})
    endif()

    target_include_directories(
        ${dependency}
        SYSTEM INTERFACE ${${DEPENDENCY_STATIC_INCLUDE_DIRS}})

    foreach(static_library_directory ${${DEPENDENCY_STATIC_LIBRARY_DIRS}})
        if (EXISTS "${static_library_directory}/${install_ufid}")
            target_link_directories(
                ${dependency}
                INTERFACE "${static_library_directory}/${install_ufid}")
        endif()

        if (EXISTS "${static_library_directory}")
            target_link_directories(
                ${dependency}
                INTERFACE "${static_library_directory}")
        endif()
    endforeach()

    set(interface_link_libraries)

    # If true add -L and -l rules to target link libraries, otherwise, add
    # full paths to libraries.

    # TODO: !!! In the ntf-core release, this is set to TRUE. Maybe set
    #           conditionally whether we are linking an executable !!!
    set(NTF_BUILD_LINK_USING_L_RULES FALSE)


    # If true use -l rules for all libraries not found in the refroot.
    set(NTF_BUILD_LINK_USING_L_RULES_FOR_SYSTEM_LIBRARIES TRUE)

    if (${NTF_BUILD_LINK_USING_L_RULES})
        set(static_libraries ${${DEPENDENCY_STATIC_LIBRARIES}})
        foreach (static_library ${static_libraries})
            if (VERBOSE)
                message(STATUS "Dependency '${dependency}' needs link library '${static_library}'")
            endif()
            if (UNIX)
                list(APPEND interface_link_libraries "-l${static_library}")
            elseif (WIN32)
                list(APPEND interface_link_libraries "${static_library}.lib")
            else()
                ntf_die("Unknown platform")
            endif()
        endforeach()
    else()
        set(static_libraries)
        set(system_libraries)

        foreach (static_library ${${DEPENDENCY_STATIC_LIBRARIES}})
            if (VERBOSE)
                message(STATUS "Dependency '${dependency}' needs link library '${static_library}'")
            endif()

            set(lib_prefix ${CMAKE_STATIC_LIBRARY_PREFIX})
            set(lib_suffix ${CMAKE_STATIC_LIBRARY_SUFFIX})

            set(static_library_name_list)
            list(APPEND static_library_name_list
                 ${lib_prefix}${static_library}${lib_suffix})
            list(APPEND static_library_name_list
                 ${lib_prefix}lib${static_library}${lib_suffix})
            list(APPEND static_library_name_list
                 ${lib_prefix}${static_library}lib${lib_suffix})

            set(static_library_path_list)
            list(APPEND static_library_path_list "${prefixLibraryUfidDir}")
            list(APPEND static_library_path_list "${prefixLibraryDir}")
            list(APPEND static_library_path_list
                        ${${DEPENDENCY_STATIC_LIBRARY_DIRS}})

            unset(static_library_path)
            find_library(
                static_library_path
                NAMES ${static_library_name_list}
                PATHS ${static_library_path_list}
                NAMES_PER_DIR
                NO_DEFAULT_PATH
                NO_CACHE)

            if ("${static_library_path}" MATCHES "NOTFOUND")
                if (VERBOSE)
                    message(STATUS "Target '${target}' dependency '${static_library}' not found as '${static_library_name_list}' in '${static_library_path_list}'")
                endif()

                if (${NTF_BUILD_LINK_USING_L_RULES_FOR_SYSTEM_LIBRARIES})
                    if (VERBOSE)
                        message(STATUS "Target '${target} dependency '${static_library}' assumed to be system library linked using -l${static_library}")
                    endif()

                    if (UNIX)
                        list(APPEND system_libraries "-l${static_library}")
                    elseif (WIN32)
                        list(APPEND system_libraries "${static_library}.lib")
                    else()
                        ntf_die("Unknown platform")
                    endif()
                    continue()
                else()
                    unset(static_library_path)
                    find_library(
                        static_library_path
                        NAMES ${static_library_name_list}
                        NAMES_PER_DIR
                        NO_CACHE)

                    if ("${static_library_path}" MATCHES "NOTFOUND")
                        if (VERBOSE)
                            message(STATUS "Target '${target} dependency '${static_library}' not found as '${static_library_name_list}' in system paths")
                        endif()
                        if (UNIX)
                            list(APPEND system_libraries "-l${static_library}")
                        elseif (WIN32)
                            list(APPEND system_libraries "${static_library}.lib")
                        else()
                            ntf_die("Unknown platform")
                        endif()
                        continue()
                    endif()
                endif()
            endif()

            if (VERBOSE)
                message(STATUS "Target '${target}' dependency '${static_library}' found: '${static_library_path}'")
            endif()

            list(APPEND static_libraries "${static_library_path}")
        endforeach()

        foreach (static_library ${static_libraries})
            list(APPEND interface_link_libraries ${static_library})
        endforeach()

        foreach (system_library ${system_libraries})
            list(APPEND interface_link_libraries ${system_library})
        endforeach()
    endif()

    set_property(
        TARGET ${dependency} PROPERTY
        INTERFACE_LINK_LIBRARIES "${interface_link_libraries}")

    if (VERBOSE)
        ntf_target_dump("${dependency}")
    endif()

    if ("${target_type}" STREQUAL "INTERFACE_LIBRARY")
        target_link_libraries(${target} INTERFACE ${dependency})
    else()
        target_link_libraries(${target} PUBLIC ${dependency})
    endif()

    set(${ARG_OUTPUT} TRUE PARENT_SCOPE)

endfunction()

# Add a dependency to a target, attempting to find the dependency through
# either CMake, pkg-config, or as a raw library.
#
# TARGET     - The target.
# DEPENDENCY - The dependency name.
# OUTPUT     - The variable name set in the parent scope: true if the
#              dependency is found, and false otherwise.
# OPTIONAL   - Flag indicating the target is linked to the dependency only if
#              it exists, otherwise the dependency must be must found
function (ntf_target_link_dependency)
    cmake_parse_arguments(
        ARG "OPTIONAL" "TARGET;DEPENDENCY;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_TARGET})
    ntf_assert_defined(${ARG_DEPENDENCY})

    ntf_nomenclature_target(TARGET ${ARG_TARGET} OUTPUT target)

    ntf_assert_target_defined(${target})

    get_property(target_type TARGET ${target} PROPERTY "TYPE")

    set(dependency ${ARG_DEPENDENCY})

    string(TOLOWER ${dependency} dependency_lowercase)
    string(TOUPPER ${dependency} dependency_uppercase)

    set(dependency_optional ${ARG_OPTIONAL})
    if ("${dependency_optional}" STREQUAL "")
        set(dependency_optional FALSE)
    endif()

    # If the target already exists, link the target to the dependency.

    if (TARGET ${dependency})
        if ("${target_type}" STREQUAL "INTERFACE_LIBRARY")
            target_link_libraries(${target} INTERFACE ${dependency})
        else()
            target_link_libraries(${target} PUBLIC ${dependency})
        endif()
        ntf_target_requires_append(TARGET ${target} VALUE ${dependency})

        if (NOT "${ARG_OUTPUT}" STREQUAL "")
            set(${ARG_OUTPUT} TRUE PARENT_SCOPE)
        endif()
        return()
    endif()

    # Attempt to find the dependency using CMake.

    ntf_target_link_dependency_by_cmake(
        TARGET
            ${target}
        DEPENDENCY
            ${dependency}
        OUTPUT
            dependency_found_by_cmake
    )

    if (${dependency_found_by_cmake})
        ntf_target_requires_append(TARGET ${target} VALUE ${dependency})
        if (NOT "${ARG_OUTPUT}" STREQUAL "")
            set(${ARG_OUTPUT} TRUE PARENT_SCOPE)
        endif()
        return()
    endif()

    # Attempt to find the dependency using pkg-config.

    ntf_target_link_dependency_by_pkgconfig(
        TARGET
            ${target}
        DEPENDENCY
            ${dependency}
        OUTPUT
            dependency_found_by_pkgconfig
    )

    if (${dependency_found_by_pkgconfig})
        ntf_target_requires_append(TARGET ${target} VALUE ${dependency})
        if (NOT "${ARG_OUTPUT}" STREQUAL "")
            set(${ARG_OUTPUT} TRUE PARENT_SCOPE)
        endif()
        return()
    endif()

    # TODO: ntf_target_link_dependency_on_system

    if (NOT ${dependency_optional})
        ntf_die("The target '${target}' requires ${dependency}")
    endif()

    if (NOT "${ARG_OUTPUT}" STREQUAL "")
        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
    endif()

endfunction()

# Set a variable scoped to a repository.
#
# VARIABLE - The variable name.
# VALUE    - The variable value.
function (ntf_repository_variable_set)
    cmake_parse_arguments(
        ARG "" "VARIABLE" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_VARIABLE})
    # ntf_assert_defined(${ARG_VALUE})

    ntf_nomenclature_variable(VARIABLE ${ARG_VARIABLE} OUTPUT variable)

    if (NOT "${ARG_VALUE}" STREQUAL "")
        set_property(
            DIRECTORY
                ${CMAKE_CURRENT_LIST_DIR}
            PROPERTY
                ${variable}
            ${ARG_VALUE})
    else()
        set_property(
            DIRECTORY
                ${CMAKE_CURRENT_LIST_DIR}
            PROPERTY
                ${variable}
            "")
    endif()
endfunction()

# Append to a variable scoped to a repository.
#
# VARIABLE - The variable name.
# VALUE    - The variable value.
function (ntf_repository_variable_append)
    cmake_parse_arguments(
        ARG "" "VARIABLE" "VALUE" ${ARGN})

    ntf_assert_defined(${ARG_VARIABLE})
    # ntf_assert_defined(${ARG_VALUE})

    ntf_nomenclature_variable(VARIABLE ${ARG_VARIABLE} OUTPUT variable)

    if (NOT "${ARG_VALUE}" STREQUAL "")
        get_property(
            result DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY ${variable})
        list(APPEND result ${ARG_VALUE})
        set_property(
            DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY ${variable} ${result})
    endif()
endfunction()

# Get a variable scoped to a repository.
#
# VARIABLE - The variable name.
# OUTPUT   - The variable name set in the parent scope.
function (ntf_repository_variable_get)
    cmake_parse_arguments(
        ARG "" "VARIABLE;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_VARIABLE})
    ntf_assert_defined(${ARG_OUTPUT})

    ntf_nomenclature_variable(VARIABLE ${ARG_VARIABLE} OUTPUT variable)

    get_property(
        result DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY ${variable})

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "name" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_name_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "name" VALUE ${ARG_VALUE})
endfunction()

# Get the "name" variable scoped to a repository.
#
# OUTPUT - The variable name set in the parent scope.
function (ntf_repository_name_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "name" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "path" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_path_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "path" VALUE ${ARG_VALUE})
endfunction()

# Get the "path" variable scoped to a repository.
#
# OUTPUT - The variable path set in the parent scope.
function (ntf_repository_path_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "path" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "url" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_url_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "url" VALUE ${ARG_VALUE})
endfunction()

# Get the "url" variable scoped to a repository.
#
# OUTPUT - The variable url set in the parent scope.
function (ntf_repository_url_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "url" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "version" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_version_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    set(version ${ARG_VALUE})

    string(REPLACE "." ";" version_list ${version})
    list(GET version_list 0 version_major)
    list(GET version_list 1 version_minor)
    list(GET version_list 2 version_patch)

    ntf_repository_variable_set(
        VARIABLE "version" VALUE ${version})

    ntf_repository_variable_set(
        VARIABLE "version_major" VALUE ${version_major})

    ntf_repository_variable_set(
        VARIABLE "version_minor" VALUE ${version_minor})

    ntf_repository_variable_set(
        VARIABLE "version_patch" VALUE ${version_patch})

endfunction()

# Get the "version" variable scoped to a repository.
#
# OUTPUT - The variable name set in the parent scope.
function (ntf_repository_version_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "version" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "standard" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_standard_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    set(standard ${ARG_VALUE})

    if ("${standard}" STREQUAL "03")
        set(standard "98")
    endif()

    ntf_repository_variable_set(
        VARIABLE "standard" VALUE ${standard})
endfunction()

# Get the "standard" variable scoped to a repository.
#
# OUTPUT - The variable name set in the parent scope.
function (ntf_repository_standard_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "standard" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "install_refroot" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_install_refroot_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_repository_variable_set(
            VARIABLE "install_refroot" VALUE ${ARG_VALUE})
    else()
        ntf_repository_variable_set(
            VARIABLE "install_refroot" VALUE "")
    endif()
endfunction()

# Get the "install_refroot" variable scoped to a repository.
#
# OUTPUT - The variable install_refroot set in the parent scope.
function (ntf_repository_install_refroot_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "install_refroot" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "install_prefix" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_install_prefix_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "install_prefix" VALUE ${ARG_VALUE})
endfunction()

# Get the "install_prefix" variable scoped to a repository.
#
# OUTPUT - The variable install_prefix set in the parent scope.
function (ntf_repository_install_prefix_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "install_prefix" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "install_ufid" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_install_ufid_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "install_ufid" VALUE ${ARG_VALUE})
endfunction()

# Get the "install_ufid" variable scoped to a repository.
#
# OUTPUT - The variable install_ufid set in the parent scope.
function (ntf_repository_install_ufid_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "install_ufid" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "build_type" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_build_type_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "build_type" VALUE ${ARG_VALUE})
endfunction()

# Get the "build_type" variable scoped to a repository.
#
# OUTPUT - The variable build_type set in the parent scope.
function (ntf_repository_build_type_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "build_type" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "build_ufid" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_build_ufid_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "build_ufid" VALUE ${ARG_VALUE})
endfunction()

# Get the "build_ufid" variable scoped to a repository.
#
# OUTPUT - The variable build_ufid set in the parent scope.
function (ntf_repository_build_ufid_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "build_ufid" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "build_warnings" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_build_warnings_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "build_warnings" VALUE ${ARG_VALUE})
endfunction()

# Get the "build_warnings" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_build_warnings_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "build_warnings" OUTPUT result)

    if ("${result}" STREQUAL "")
        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
    else()
        set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
    endif()
endfunction()

# Set the "build_warnings_as_errors" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_build_warnings_as_errors_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "build_warnings_as_errors" VALUE ${ARG_VALUE})
endfunction()

# Get the "build_warnings_as_errors" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_build_warnings_as_errors_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "build_warnings_as_errors" OUTPUT result)

    if ("${result}" STREQUAL "")
        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
    else()
        set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
    endif()
endfunction()

# Set the "build_documentation" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_build_documentation_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "build_documentation" VALUE ${ARG_VALUE})
endfunction()

# Get the "build_documentation" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_build_documentation_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "build_documentation" OUTPUT result)

    if ("${result}" STREQUAL "")
        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
    else()
        set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
    endif()
endfunction()

# Set the "build_documentation_internal" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_build_documentation_internal_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "build_documentation_internal" VALUE ${ARG_VALUE})
endfunction()

# Get the "build_documentation_internal" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_build_documentation_internal_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "build_documentation_internal" OUTPUT result)

    if ("${result}" STREQUAL "")
        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
    else()
        set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
    endif()
endfunction()

# Set the "build_coverage" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_build_coverage_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "build_coverage" VALUE ${ARG_VALUE})
endfunction()

# Get the "build_coverage" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_build_coverage_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "build_coverage" OUTPUT result)

    if ("${result}" STREQUAL "")
        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
    else()
        set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
    endif()
endfunction()

# Set the "toolchain_link_static" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_toolchain_link_static_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "toolchain_link_static" VALUE ${ARG_VALUE})
endfunction()

# Get the "toolchain_link_static" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_toolchain_link_static_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(VARIABLE "toolchain_link_static" OUTPUT result)

    if ("${result}" STREQUAL "")
        set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
    else()
        set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
    endif()
endfunction()

# Set the "toolchain_compiler_name" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_toolchain_compiler_name_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "toolchain_compiler_name" VALUE ${ARG_VALUE})
endfunction()

# Get the "toolchain_compiler_name" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_toolchain_compiler_name_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "toolchain_compiler_name" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "toolchain_compiler_path" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_toolchain_compiler_path_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "toolchain_compiler_path" VALUE ${ARG_VALUE})
endfunction()

# Get the "toolchain_compiler_path" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_toolchain_compiler_path_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "toolchain_compiler_path" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "toolchain_debugger_name" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_toolchain_debugger_name_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    # ntf_assert_defined(${ARG_VALUE})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_repository_variable_set(
            VARIABLE "toolchain_debugger_name" VALUE ${ARG_VALUE})
    endif()
endfunction()

# Get the "toolchain_debugger_name" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_toolchain_debugger_name_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "toolchain_debugger_name" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "toolchain_debugger_path" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_toolchain_debugger_path_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    # ntf_assert_defined(${ARG_VALUE})

    if (NOT "${ARG_VALUE}" STREQUAL "")
        ntf_repository_variable_set(
            VARIABLE "toolchain_debugger_path" VALUE ${ARG_VALUE})
    endif()
endfunction()

# Get the "toolchain_debugger_path" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_toolchain_debugger_path_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "toolchain_debugger_path" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "archive_output_path" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_archive_output_path_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "archive_output_path" VALUE ${ARG_VALUE})
endfunction()

# Get the "archive_output_path" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_archive_output_path_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "archive_output_path" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "library_output_path" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_library_output_path_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "library_output_path" VALUE ${ARG_VALUE})
endfunction()

# Get the "library_output_path" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_library_output_path_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "library_output_path" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "runtime_output_path" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_runtime_output_path_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "runtime_output_path" VALUE ${ARG_VALUE})
endfunction()

# Get the "runtime_output_path" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_runtime_output_path_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "runtime_output_path" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "package_output_path" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_package_output_path_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "package_output_path" VALUE ${ARG_VALUE})
endfunction()

# Get the "package_output_path" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_package_output_path_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "package_output_path" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Set the "doxygen_output_path" variable scoped to a repository.
#
# VALUE  - The variable value.
function (ntf_repository_doxygen_output_path_set)
    cmake_parse_arguments(
        ARG "" "VALUE" "" ${ARGN})

    ntf_assert_defined(${ARG_VALUE})

    ntf_repository_variable_set(
        VARIABLE "doxygen_output_path" VALUE ${ARG_VALUE})
endfunction()

# Get the "doxygen_output_path" variable scoped to a repository.
#
# OUTPUT - The variable set in the parent scope.
function (ntf_repository_doxygen_output_path_get)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_repository_variable_get(
        VARIABLE "doxygen_output_path" OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Register a BDE-style repository. This function should be called after a
# CMake project() is declared.
#
# NAME    - The name of the repository
# VERSION - The version
# PATH    - The absolute path to the repository
# URL     - The URL from where the repository may be cloned
# UFID    - The UFID
function (ntf_repository)
    cmake_parse_arguments(
        ARG "" "NAME;VERSION;PATH;URL;UFID" "" ${ARGN})

    ntf_assert_defined(${ARG_NAME})
    ntf_assert_defined(${ARG_VERSION})
    ntf_assert_defined(${ARG_PATH})
    ntf_assert_defined(${ARG_URL})

    ntf_repository_name_set(VALUE ${ARG_NAME})
    ntf_repository_version_set(VALUE ${ARG_VERSION})
    ntf_repository_path_set(VALUE ${ARG_PATH})
    ntf_repository_url_set(VALUE ${ARG_URL})

    if (DEFINED DISTRIBUTION_REFROOT)
        ntf_repository_install_refroot_set(VALUE "${DISTRIBUTION_REFROOT}")
    elseif(DEFINED ENV{DISTRIBUTION_REFROOT})
        ntf_repository_install_refroot_set(VALUE "$ENV{DISTRIBUTION_REFROOT}")
    else()
        ntf_repository_install_refroot_set(VALUE "")
    endif()

    ntf_repository_install_prefix_set(VALUE ${CMAKE_INSTALL_PREFIX})

    ntf_repository_build_type_set(VALUE ${CMAKE_BUILD_TYPE})

    ntf_ufid_parse_for_build(UFID "${ARG_UFID}" OUTPUT build_ufid)
    ntf_ufid_parse_for_install(UFID "${build_ufid}" OUTPUT install_ufid)

    ntf_repository_build_ufid_set(VALUE ${build_ufid})
    ntf_repository_install_ufid_set(VALUE ${install_ufid})

    set(cpp_standard_list
        "03" "11" "14" "17" "20")

    set(cpp_standard "")
    foreach(cpp_standard_candidate IN LISTS cpp_standard_list)
        ntf_ufid_string_has(UFID ${build_ufid}
                            FLAG "cpp${cpp_standard_candidate}"
                            OUTPUT "is_cpp${cpp_standard_candidate}")

        if (${is_cpp${cpp_standard_candidate}})
            if (${cpp_standard_candidate} STREQUAL "03")
                set(cpp_standard "98")
            else()
                set(cpp_standard ${cpp_standard_candidate})
            endif()
            break()
        endif()
    endforeach()

    if ("${cpp_standard}" STREQUAL "")
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|MSVC")
            set(cpp_standard "20")
        else()
            set(cpp_standard "98")
        endif()
    endif()

    message(STATUS "Using C++${cpp_standard}")

    ntf_repository_standard_set(VALUE "${cpp_standard}")

    if (NOT "${NTF_TOOLCHAIN_COMPILER_CXX_NAME}" STREQUAL "")
        ntf_repository_toolchain_compiler_name_set(
            VALUE ${NTF_TOOLCHAIN_COMPILER_CXX_NAME})
    else()
        get_filename_component(compiler_name ${CMAKE_CXX_COMPILER} NAME_WE)
        ntf_repository_toolchain_compiler_name_set(
            VALUE ${compiler_name})
    endif()

    if (NOT "${NTF_TOOLCHAIN_COMPILER_CXX_PATH}" STREQUAL "")
        ntf_repository_toolchain_compiler_path_set(
            VALUE ${NTF_TOOLCHAIN_COMPILER_CXX_PATH})
    else()
        if (IS_ABSOLUTE ${CMAKE_CXX_COMPILER})
            ntf_repository_toolchain_compiler_path_set(
                VALUE ${CMAKE_CXX_COMPILER})
        else()
            find_program(compiler_path ${CMAKE_CXX_COMPILER} REQUIRED NO_CACHE)
            ntf_repository_toolchain_compiler_path_set(
                VALUE ${compiler_path})
        endif()
    endif()

    if (NOT "${NTF_TOOLCHAIN_DEBUGGER_NAME}" STREQUAL "")
        ntf_repository_toolchain_debugger_name_set(
            VALUE ${NTF_TOOLCHAIN_DEBUGGER_NAME})
    else()
        ntf_repository_toolchain_compiler_name_get(OUTPUT compiler_name)
        if (NOT "${compiler_name}" STREQUAL "cl")
            if("${compiler_name}" STREQUAL "cc" OR
               "${compiler_name}" STREQUAL "c++")
                if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                    ntf_repository_toolchain_debugger_name_set(VALUE "lldb")
                elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                    ntf_repository_toolchain_debugger_name_set(VALUE "gdb")
                else()
                    ntf_repository_toolchain_debugger_name_set(VALUE "")
                endif()
            elseif("${compiler_name}" STREQUAL "clang" OR
                "${compiler_name}" STREQUAL "clang++")
                ntf_repository_toolchain_debugger_name_set(VALUE "lldb")
            elseif("${compiler_name}" STREQUAL "gcc" OR
                   "${compiler_name}" STREQUAL "g++")
                ntf_repository_toolchain_debugger_name_set(VALUE "gdb")
            else()
                ntf_repository_toolchain_debugger_name_set(VALUE "")
            endif()
        else()
            ntf_repository_toolchain_debugger_name_set(VALUE "")
        endif()
    endif()

    if (NOT "${NTF_TOOLCHAIN_DEBUGGER_PATH}" STREQUAL "")
        ntf_repository_toolchain_debugger_path_set(
            VALUE ${NTF_TOOLCHAIN_DEBUGGER_PATH})
    else()
        ntf_repository_toolchain_debugger_name_get(OUTPUT debugger_name)
        if (NOT "${debugger_name}" STREQUAL "")
            find_program(debugger_path ${debugger_name} NO_CACHE)
            if ("${debugger_path}" STREQUAL "debugger_path-NOTFOUND")
                set(debugger_path "${debugger_name}")
            endif()
            ntf_repository_toolchain_debugger_path_set(
                VALUE ${debugger_path})
        else()
            ntf_repository_toolchain_debugger_path_set(VALUE "")
        endif()
    endif()

    if (NOT TARGET build_test)
        add_custom_target(build_test)
    endif()

    if (NOT TARGET all.t)
        add_custom_target(all.t)
    endif()

    add_dependencies(all.t build_test)

    if (NOT TARGET format)
        add_custom_target(format)
    endif()

    ntf_repository_install_refroot_get(OUTPUT install_refroot)
    ntf_repository_install_prefix_get(OUTPUT install_prefix)

    find_program(
        CLANG_FORMAT_PATH
        NAMES
            clang-format
        PATHS
            ${install_refroot}/${install_prefix}/bin
            /opt/bb/bin
    )

    find_program(
        NTF_SDLC_FORMAT
        NAMES
            ntf-sdlc-format
            ntf-sdlc-format.cmd
        PATHS
            ${install_refroot}/${install_prefix}/libexec/ntf
            /opt/bb/libexec/ntf
    )

    find_program(
        NTF_SDLC_VERIFY
        NAMES
            ntf-sdlc-verify
            ntf-sdlc-verify.cmd
        PATHS
            ${install_refroot}/${install_prefix}/libexec/ntf
            /opt/bb/libexec/ntf
    )

    if (DEFINED NTF_SDLC_VERIFY)
        if (NOT TARGET verify)
            add_custom_target(
                verify
                COMMAND ${NTF_SDLC_VERIFY} --repository ${ARG_PATH} --output ${CMAKE_BINARY_DIR}
                WORKING_DIRECTORY ${ARG_PATH})
        endif()
    else()
        if (NOT TARGET verify)
            add_custom_target(
                verify
                COMMAND ntf-sdlc-verify --repository ${ARG_PATH} --output ${CMAKE_BINARY_DIR}
                WORKING_DIRECTORY ${ARG_PATH})
        endif()
    endif()

    find_program(
        NTF_SDLC_RELEASE
        NAMES
            ntf-sdlc-release
            ntf-sdlc-release.cmd
        PATHS
            ${install_refroot}/${install_prefix}/libexec/ntf
            /opt/bb/libexec/ntf
    )

    if (DEFINED NTF_SDLC_RELEASE)
        if (NOT TARGET release_major)
            add_custom_target(
                release_major
                COMMAND ${NTF_SDLC_RELEASE} --repository ${ARG_PATH} --package ${ARG_NAME} --major
                WORKING_DIRECTORY ${ARG_PATH})
        endif()

        if (NOT TARGET release_minor)
            add_custom_target(
                release_minor
                COMMAND ${NTF_SDLC_RELEASE} --repository ${ARG_PATH} --package ${ARG_NAME} --minor
                WORKING_DIRECTORY ${ARG_PATH})
        endif()

        if (NOT TARGET release_patch)
            add_custom_target(
                release_patch
                COMMAND ${NTF_SDLC_RELEASE} --repository ${ARG_PATH} --package ${ARG_NAME} --patch
                WORKING_DIRECTORY ${ARG_PATH})
        endif()
    else()
        if (NOT TARGET release_major)
            add_custom_target(
                release_major
                COMMAND ntf-sdlc-release --repository ${ARG_PATH} --package ${ARG_NAME} --major
                WORKING_DIRECTORY ${ARG_PATH})
        endif()

        if (NOT TARGET release_minor)
            add_custom_target(
                release_minor
                COMMAND ntf-sdlc-release --repository ${ARG_PATH} --package ${ARG_NAME} --minor
                WORKING_DIRECTORY ${ARG_PATH})
        endif()

        if (NOT TARGET release_patch)
            add_custom_target(
                release_patch
                COMMAND ntf-sdlc-release --repository ${ARG_PATH} --package ${ARG_NAME} --patch
                WORKING_DIRECTORY ${ARG_PATH})
        endif()
    endif()

    # Maintain a file structure like:
    #
    # bin - Application and test driver executable binaries
    # lib - Static and shared libraries
    # obj - CMake-generated and other compiler-generated artifacts
    # pkg - CPack-generated binary and source packages
    # doc - Generated documentation

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/pkg)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc)

    file(REAL_PATH ${CMAKE_BINARY_DIR}/lib archiveOutputDirectory)
    file(REAL_PATH ${CMAKE_BINARY_DIR}/lib libraryOutputDirectory)
    file(REAL_PATH ${CMAKE_BINARY_DIR}/bin runtimeOutputDirectory)
    file(REAL_PATH ${CMAKE_BINARY_DIR}/pkg packageOutputDirectory)
    file(REAL_PATH ${CMAKE_BINARY_DIR}/doc doxygenOutputDirectory)

    ntf_repository_archive_output_path_set(VALUE ${archiveOutputDirectory})
    ntf_repository_library_output_path_set(VALUE ${libraryOutputDirectory})
    ntf_repository_runtime_output_path_set(VALUE ${runtimeOutputDirectory})
    ntf_repository_package_output_path_set(VALUE ${packageOutputDirectory})
    ntf_repository_doxygen_output_path_set(VALUE ${doxygenOutputDirectory})

    set(cmake_ctest_arguments ${CMAKE_CTEST_ARGUMENTS})
    list(APPEND cmake_ctest_arguments --stop-on-failure --output-on-failure --output-junit Testing/Temporary/Test.xml)
    set(CMAKE_CTEST_ARGUMENTS ${cmake_ctest_arguments} PARENT_SCOPE)

    set(CPACK_OUTPUT_FILE_PREFIX ${packageOutputDirectory} PARENT_SCOPE)

    # Do not include a top-level <project>-<version> directory in generated
    # source archive packages (e.g., .tar.gz).

    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE PARENT_SCOPE)

    # Include the root of the repository when packaging.

    ntf_repository_path_get(OUTPUT repository_path)
    set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${repository_path};/" PARENT_SCOPE)

    # Suppress warnings about a potentially unused variable.

    set(ufid ${UFID})
    set(distribution_refroot ${DISTRIBUTION_REFROOT})
    set(verbose ${CMAKE_VERBOSE_MAKEFILE})

    ntf_ide_vs_code_tasks_create()
    ntf_ide_vs_code_launch_create()
    ntf_ide_vs_code_c_cpp_properties_create()
endfunction()

# Dump the configuration of the repository to the log.
function (ntf_repository_dump)
    ntf_repository_name_get(OUTPUT repository_name)
    ntf_repository_version_get(OUTPUT repository_version)
    ntf_repository_path_get(OUTPUT repository_path)
    ntf_repository_url_get(OUTPUT repository_url)

    ntf_repository_install_refroot_get(OUTPUT repository_install_refroot)
    ntf_repository_install_prefix_get(OUTPUT repository_install_prefix)
    ntf_repository_install_ufid_get(OUTPUT repository_install_ufid)

    ntf_repository_build_ufid_get(OUTPUT repository_build_ufid)
    ntf_repository_build_type_get(OUTPUT repository_build_type)

    message(STATUS "NTF Build: Repository name =                    ${repository_name}")
    message(STATUS "NTF Build: Repository version =                 ${repository_version}")
    message(STATUS "NTF Build: Repository path =                    ${repository_path}")
    message(STATUS "NTF Build: Repository URL =                     ${repository_url}")
    message(STATUS "NTF Build: Repository install refroot =         ${repository_install_refroot}")
    message(STATUS "NTF Build: Repository install prefix =          ${repository_install_prefix}")
    message(STATUS "NTF Build: Repository install UFID =            ${repository_install_ufid}")
    message(STATUS "NTF Build: Repository build UFID =              ${repository_build_ufid}")
    message(STATUS "NTF Build: Repository build type =              ${repository_build_type}")

    # Dump the definitions of all the NTF_BUILD_* variables.

    message(STATUS "NTF: Variables")
    get_cmake_property(variable_list VARIABLES)
    list(SORT variable_list)
    foreach (variable_name ${variable_list})
        if (variable_name MATCHES "NTF_BUILD_")
            message(STATUS "NTF Build: ${variable_name}=${${variable_name}}")
        endif()
    endforeach()

endfunction()

# Wrap up the declaration of the NTF project by instructing CMake how to
# generate and install the packaging meta-data.
function (ntf_repository_end)
    ntf_repository_name_get(OUTPUT repository_name)
    ntf_repository_path_get(OUTPUT repository_path)

    ntf_repository_install_refroot_get(OUTPUT install_refroot)
    ntf_repository_install_prefix_get(OUTPUT install_prefix)

    ntf_repository_build_ufid_get(OUTPUT ufid)

    ntf_ufid_string_has(UFID ${ufid} FLAG "cov" OUTPUT is_cov)
    ntf_repository_build_coverage_get(OUTPUT measure_coverage)

    set(CPACK_PACKAGE_VENDOR "Bloomberg" PARENT_SCOPE)
    set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_NAME} PARENT_SCOPE)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION} PARENT_SCOPE)
    set(CPACK_PACKAGE_CONTACT
        "Matthew Millett <mmillett2@bloomberg.net>"
        PARENT_SCOPE)
    set(CPACK_PACKAGE_FILE_NAME
        ${PROJECT_NAME}-${PROJECT_VERSION}
        PARENT_SCOPE)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME
        ${PROJECT_NAME}-${PROJECT_VERSION}-src
        PARENT_SCOPE)

    if("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
        set(CPACK_GENERATOR TGZ ZIP PARENT_SCOPE)
        set(CPACK_SOURCE_GENERATOR TGZ ZIP PARENT_SCOPE)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD")
        set(CPACK_GENERATOR TGZ ZIP PARENT_SCOPE)
        set(CPACK_SOURCE_GENERATOR TGZ ZIP PARENT_SCOPE)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        set(CPACK_GENERATOR TGZ ZIP DEB PARENT_SCOPE) # RPM
        set(CPACK_SOURCE_GENERATOR TGZ ZIP PARENT_SCOPE)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(CPACK_GENERATOR TGZ ZIP PARENT_SCOPE)
        set(CPACK_SOURCE_GENERATOR TGZ ZIP PARENT_SCOPE)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
        set(CPACK_GENERATOR TGZ ZIP PARENT_SCOPE)
        set(CPACK_SOURCE_GENERATOR TGZ ZIP PARENT_SCOPE)
    endif()

    set(CPACK_SOURCE_IGNORE_FILES
        ".git" ".gitignore .vscode .config build"
        PARENT_SCOPE)

    set(CPACK_ARCHIVE_COMPONENT_INSTALL ON PARENT_SCOPE)

    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64" PARENT_SCOPE)
    #set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>=2.7-18)")
    #set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    #set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    #    "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst")

    ntf_repository_build_documentation_get(OUTPUT build_documentation)

    ntf_repository_build_documentation_internal_get(
        OUTPUT build_documentation_internal)

    if (${build_documentation} OR ${build_documentation_internal})
        ntf_repository_doxygen_output_path_get(OUTPUT doxygen_output_directory)

        set(doxygen_src_dir ${repository_path})
        set(doxygen_out_dir ${doxygen_output_directory})

        if (${build_documentation_internal})
            set(doxygen_internal "YES")
        else()
            set(doxygen_internal "NO")
        endif()

        configure_file(
            ${repository_path}/docs/doxygen/configuration
            ${CMAKE_BINARY_DIR}/Doxyfile
            @ONLY)

        find_program(
            DOXYGEN_PATH
            NAMES doxygen doxygen.exe
            PATHS ${install_refroot}/${install_prefix}/bin /opt/bb/bin)

        if (DEFINED DOXYGEN_PATH)
            add_custom_command(
                TARGET documentation
                COMMAND ${DOXYGEN_PATH} ${CMAKE_BINARY_DIR}/Doxyfile
                WORKING_DIRECTORY ${doxygen_output_directory}
                COMMENT "Generating documentation"
            )

            install(
                DIRECTORY
                    ${doxygen_output_directory}/html/
                DESTINATION
                    ${install_refroot}/${install_prefix}/share/doc/${repository_name}
                MESSAGE_NEVER)
        endif()
    endif()

    ntf_ide_vs_code_tasks_close()
    ntf_ide_vs_code_launch_close()

    if (${is_cov} OR ${measure_coverage})
        find_program(
            GCOV_PATH
            NAMES gcov
            PATHS ${install_refroot}/${install_prefix}/bin /opt/bb/bin)

        find_program(
            LCOV_PATH
            NAMES lcov lcov.bat lcov.exe lcov.perl
            PATHS ${install_refroot}/${install_prefix}/bin /opt/bb/bin)

        find_program(
            GENHTML_PATH
            NAMES genhtml genhtml.perl genhtml.bat
            PATHS ${install_refroot}/${install_prefix}/bin /opt/bb/bin)

        find_program(
            GCOVR_PATH
            NAMES gcovr
            PATHS ${install_refroot}/${install_prefix}/bin /opt/bb/bin)

        find_program(
            CPPFILT_PATH
            NAMES c++filt
            PATHS ${install_refroot}/${install_prefix}/bin /opt/bb/bin)

        set(GCOVR_HTML_FOLDER_CMD
            ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/coverage
        )

        ntf_repository_name_get(OUTPUT repository_name)

        set(html_title "Code Coverage: ${repository_name}")

        set(coverage_command
            ${GCOVR_PATH} --root ${PROJECT_SOURCE_DIR} --object-directory=${PROJECT_BINARY_DIR} --exclude=${PROJECT_SOURCE_DIR}/\(.+/\)?\(.+\)\\.t\\.cpp\$ --exclude=${PROJECT_SOURCE_DIR}/\(.+/\)?\(.+\)\\.m\\.cpp\$ --html coverage/index.html --html-details --html-title ${html_title} --xml coverage/index.xml --xml-pretty)

        if (VERBOSE)
            message(STATUS "Enabled code coverage")
            message(STATUS "    GCOV_PATH:    ${GCOV_PATH}")
            message(STATUS "    LCOV_PATH:    ${LCOV_PATH}")
            message(STATUS "    GENHTML_PATH: ${GENHTML_PATH}")
            message(STATUS "    GCOVR_PATH:   ${GCOVR_PATH}")
            message(STATUS "    CPPFILT_PATH: ${CPPFILT_PATH}")

            string(REPLACE ";" " "
                   coverage_command_string
                   "${coverage_command}")

            message(STATUS "    Command:      ${coverage_command_string}")
        endif()

        add_custom_target(
            coverage
            COMMAND
                ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/coverage
            COMMAND
                ${coverage_command}
            BYPRODUCTS
                ${PROJECT_BINARY_DIR}/coverage/index.html
                ${PROJECT_BINARY_DIR}/coverage/index.xml
            WORKING_DIRECTORY
                ${PROJECT_BINARY_DIR}
            VERBATIM
            COMMENT "Generating code coverage"
        )

        add_custom_command(
            TARGET coverage
            POST_BUILD
            COMMAND ;
            COMMENT "Generated '${PROJECT_BINARY_DIR}/coverage/index.html'"
        )
    endif()

endfunction()

# Enable or disable compiler warnings.
function (ntf_repository_enable_warnings flag)
    if (${flag})
        ntf_repository_build_warnings_set(VALUE TRUE)
    else()
        ntf_repository_build_warnings_set(VALUE FALSE)
    endif()
endfunction()

# Enable or disable the treatment of compiler warnings as errors.
function (ntf_repository_enable_warnings_as_errors flag)
    if (${flag})
        ntf_repository_build_warnings_as_errors_set(VALUE TRUE)
    else()
        ntf_repository_build_warnings_as_errors_set(VALUE FALSE)
    endif()
endfunction()

# Enable or disable the generation of documentation.
function (ntf_repository_enable_documentation flag)
    if (${flag})
        ntf_repository_build_documentation_set(VALUE TRUE)

        if (NOT TARGET documentation)
            add_custom_target(documentation ALL)
        endif()
    else()
        ntf_repository_build_documentation_set(VALUE FALSE)
    endif()
endfunction()

# Enable or disable the generation of documentation of internal implementation
# details.
function (ntf_repository_enable_documentation_internal flag)
    if (${flag})
        ntf_repository_build_documentation_internal_set(VALUE TRUE)

        if (NOT TARGET documentation)
            add_custom_target(documentation ALL)
        endif()
    else()
        ntf_repository_build_documentation_internal_set(VALUE FALSE)
    endif()
endfunction()

# Enable or disable code coverage, unless the UFID indicates code coverage
# should be applied.
function (ntf_repository_enable_coverage flag)
    if (${flag})
        ntf_repository_build_coverage_set(VALUE TRUE)
    else()
        ntf_repository_build_coverage_set(VALUE FALSE)
    endif()
endfunction()

# Enable or disable static linking, if supported by the platform.
function (ntf_repository_enable_link_static flag)
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" OR
       "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        if (${flag})
            ntf_repository_toolchain_link_static_set(VALUE TRUE)
        else()
            ntf_repository_toolchain_link_static_set(VALUE FALSE)
        endif()
    else()
        if (${flag})
            ntf_die("Static linking is not supported on this platform")
        endif()
    endif()
endfunction()

# Create the '.vscode/tasks.json' file and write the standard prolog.
function (ntf_ide_vs_code_tasks_create)
    ntf_repository_path_get(OUTPUT repository_path)
    set(path "${repository_path}/.vscode/tasks.json")

    if(UNIX)
        set(problem_matcher "\$gcc")
    else()
        set(problem_matcher "\$msCompile")
    endif()

    set(content "")
    string(APPEND content "\
{\n\
  \"version\": \"2.0.0\",\n\
  \"tasks\": [\n\
    {\n\
      \"type\": \"process\",\n\
      \"label\": \"all\",\n\
      \"command\": \"${CMAKE_COMMAND}\",\n\
      \"args\": [\n\
        \"--build\",\n\
        \"${CMAKE_BINARY_DIR}\",\n\
        \"--parallel\",\n\
        \"8\"\n\
      ],\n\
      \"problemMatcher\": [ \"${problem_matcher}\" ],\n\
      \"group\": {\n\
        \"kind\": \"build\",\n\
        \"isDefault\": true\n\
      }\n\
    }")

    file(WRITE ${path} ${content})
endfunction()

# Add a target to 'vs.code/tasks.json'.
function (ntf_ide_vs_code_tasks_add_target target)
    ntf_repository_path_get(OUTPUT repository_path)
    set(path "${repository_path}/.vscode/tasks.json")

    if(UNIX)
        set(problem_matcher "\$gcc")
    else()
        set(problem_matcher "\$msCompile")
    endif()

    set(content "")
    string(APPEND content ",\n\
    {\n\
      \"type\": \"process\",\n\
      \"label\": \"${target}\",\n\
      \"command\": \"${CMAKE_COMMAND}\",\n\
      \"args\": [\n\
        \"--build\",\n\
        \"${CMAKE_BINARY_DIR}\",\n\
        \"--parallel\",\n\
        \"8\",\n\
        \"--target\",\n\
        \"${target}\"\n\
      ],\n\
      \"problemMatcher\": [ \"${problem_matcher}\" ],\n\
      \"group\": {\n\
        \"kind\": \"build\",\n\
        \"isDefault\": true\n\
      }\n\
    }")

    file(APPEND ${path} ${content})
endfunction()

# Write the standard epilog to '.vscode/tasks.json'.
function (ntf_ide_vs_code_tasks_close)
    ntf_repository_path_get(OUTPUT repository_path)
    set(path "${repository_path}/.vscode/tasks.json")

    set(content "")
    string(APPEND content "\n\
  ]\n\
}")

    file(APPEND ${path} ${content})
endfunction()

# Create the '.vscode/launch.json' file and write the standard prolog.
function (ntf_ide_vs_code_launch_create)
    ntf_repository_path_get(OUTPUT repository_path)
    set(path "${repository_path}/.vscode/launch.json")

    if(UNIX)
        set(launch_type "cppdbg")
    else()
        set(launch_type "cppvsdbg")
    endif()

    set(content "")
    string(APPEND content "\
{\n\
  \"version\": \"0.2.0\",\n\
  \"configurations\": [\n\
    {\n\
      \"name\": \"attach\",\n\
      \"type\": \"${launch_type}\",\n\
      \"request\": \"attach\",\n\
      \"processId\": \"\${command:pickRemoteProcess}\",\n\
      \"program\": \"\"\n\
    }")

    file(WRITE ${path} ${content})
endfunction()



function (ntf_ide_vs_code_launch_add_target target)
    ntf_repository_path_get(OUTPUT repository_path)
    set(path "${repository_path}/.vscode/launch.json")

    if(UNIX)
        set(launch_type "cppdbg")
    else()
        set(launch_type "cppvsdbg")
    endif()

    ntf_repository_toolchain_debugger_name_get(OUTPUT debugger_name)
    ntf_repository_toolchain_debugger_path_get(OUTPUT debugger_path)

    ntf_repository_runtime_output_path_get(OUTPUT runtime_output_path)

    get_filename_component(bin_dir ${runtime_output_path} ABSOLUTE)

    set(content "")
    string(APPEND content ",\n\
    {\n\
      \"name\": \"${target}\",\n\
      \"type\": \"${launch_type}\",\n\
      \"request\": \"launch\",\n\
      \"program\": \"${bin_dir}/${target}${CMAKE_EXECUTABLE_SUFFIX}\",\n\
      \"args\": [],\n\
      \"stopAtEntry\": false,\n\
      \"cwd\": \"${bin_dir}\",\n\
      \"environment\": [],\n\
      \"externalConsole\": false")

    if("${debugger_name}" STREQUAL "gdb" OR "${debugger_name}" STREQUAL "lldb")
        string(APPEND content ",\n\
      \"MIMode\": \"${debugger_name}\"")
        if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
            string(APPEND content ",\n\
            \"miDebuggerPath\": \"${debugger_path}\"")
        endif()
    endif()

        string(APPEND content "\n\
    }")

    file(APPEND ${path} ${content})
endfunction()

function (ntf_ide_vs_code_launch_close)
    ntf_repository_path_get(OUTPUT repository_path)
    set(path "${repository_path}/.vscode/launch.json")

    set(content "")
    string(APPEND content "\n\
  ]\n\
}")
    file(APPEND ${path} ${content})
endfunction()

# Write '.vscode/c_cpp_properties.json'.
function (ntf_ide_vs_code_c_cpp_properties_create)
    ntf_repository_path_get(OUTPUT repository_path)

    ntf_repository_toolchain_compiler_name_get(OUTPUT compiler_name)
    ntf_repository_toolchain_compiler_path_get(OUTPUT compiler_path)

    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64" OR
       "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "ARM64")
        set(arch "arm64")
    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
        set(arch "arm64")
    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR
           "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "amd64"  OR
           "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64")
        set(arch "x64")
    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "sparc")
        set(arch "sparc")
    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "powerpc")
        set(arch "powerpc")
    else()
        message(FATAL_ERROR
                "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    set(c_cpp_properties_text "")
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        string(APPEND c_cpp_properties_text "\
{\n\
  \"version\": 4,\n\
  \"configurations\": [\n\
    {\n\
    \"name\": \"Linux\",\n\
      \"compilerPath\": \"${compiler_path}\",\n\
      \"compileCommands\": \"${CMAKE_BINARY_DIR}/compile_commands.json\",\n\
      \"cStandard\": \"c11\",\n\
      \"cppStandard\": \"c++20\",\n\
      \"intelliSenseMode\": \"linux-gcc-${arch}\"\n\
    }\n\
  ]\n\
}")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        string(APPEND c_cpp_properties_text "\
{\n\
  \"version\": 4,\n\
  \"configurations\": [\n\
    {\n\
      \"name\": \"macOS\",\n\
      \"compilerPath\": \"${compiler_path}\",\n\
      \"compileCommands\": \"${CMAKE_BINARY_DIR}/compile_commands.json\",\n\
      \"cStandard\": \"c11\",\n\
      \"cppStandard\": \"c++20\",\n\
      \"intelliSenseMode\": \"macos-clang-${arch}\",\n\
      \"macFrameworkPath\": [ \"/System/Library/Frameworks\", \"/Library/Frameworks\" ]\n\
    }\n\
  ]\n\
}")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        string(APPEND c_cpp_properties_text "\
{\n\
  \"version\": 4,\n\
  \"configurations\": [\n\
    {\n\
      \"name\": \"Win32\",\n\
      \"windowsSdkVersion\": \"$ENV{UCRTVersion}\",\n\
      \"compilerPath\": \"${compiler_path}\",\n\
      \"compileCommands\": \"${CMAKE_BINARY_DIR}/compile_commands.json\",\n\
      \"cStandard\": \"c11\",\n\
      \"cppStandard\": \"c++20\",\n\
      \"intelliSenseMode\": \"windows-msvc-${arch}\"\n\
    }\n\
  ]\n\
}")
    endif()

    file(WRITE "${repository_path}/.vscode/c_cpp_properties.json"
         ${c_cpp_properties_text})
endfunction()



# Test if a list of UFID flags contains a UFID flag.
#
# UFID   - The list of UFID flags.
# VALUE  - The UFID flag to test.
# OUTPUT - The variable name set in the parent scope to TRUE if the flag is
#          contained in the UFID, and FALSE otherwise.
function (ntf_ufid_list_has)
    cmake_parse_arguments(
        ARG "" "FLAG;OUTPUT" "UFID" ${ARGN})

    ntf_assert_defined(${ARG_UFID})
    ntf_assert_defined(${ARG_FLAG})
    ntf_assert_defined(${ARG_OUTPUT})

    foreach (flag ${ARG_UFID})
        if ("${flag}" STREQUAL "${ARG_FLAG}")
            set(${ARG_OUTPUT} TRUE PARENT_SCOPE)
            return()
        endif()
    endforeach()

    set(${ARG_OUTPUT} FALSE PARENT_SCOPE)
endfunction()

# Test if a UFID string consisting of underscore-separated UFID flags contains
# a UFID flag.
#
# UFID   - The UFID string.
# VALUE  - The UFID flag to test.
# OUTPUT - The variable name set in the parent scope to TRUE if the flag is
#          contained in the UFID, and FALSE otherwise.
function (ntf_ufid_string_has)
    cmake_parse_arguments(
        ARG "" "UFID;FLAG;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_UFID})
    ntf_assert_defined(${ARG_FLAG})
    ntf_assert_defined(${ARG_OUTPUT})

    string(REGEX MATCHALL "[^-_]+" ufid_list "${ARG_UFID}")

    ntf_ufid_list_has(UFID ${ufid_list} FLAG ${ARG_FLAG} OUTPUT result)
    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

# Parse a UFID list and return its canonical form.
#
# UFID   - The UFID string
# OUTPUT - The variable name set in the parent scope to the canonical form
#          of the UFID.
function (ntf_ufid_list_canonicalize)
    cmake_parse_arguments(
        ARG "" "OUTPUT" "UFID" ${ARGN})

    ntf_assert_defined(${ARG_UFID})
    ntf_assert_defined(${ARG_OUTPUT})

    set(ufid_list ${ARG_UFID})

    # Remove duplicate flags from the list.

    list(REMOVE_DUPLICATES ufid_list)

    # These flags can appear in a valid ufid. The order of those flags is
    # important.

    set(known_ufid_flags
        opt dbg exc mt 64 safe safe2
        aopt adbg asafe anone
        ropt rdbg rsafe rnone
        asan msan tsan ubsan
        fuzz
        cov
        static
        stlport pic shr ndebug
        cpp03 cpp11 cpp14 cpp17 cpp20)

    set(result)
    foreach(known_ufid_flag IN LISTS known_ufid_flags)
        if (${known_ufid_flag} IN_LIST ufid_list)
            list(APPEND result ${known_ufid_flag})
        endif()
    endforeach()

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)

endfunction()


# Parse a UFID string and return its canonical form.
#
# UFID   - The UFID string
# OUTPUT - The variable name set in the parent scope to the canonical form
#          of the UFID.
function (ntf_ufid_string_canonicalize)
    cmake_parse_arguments(
        ARG "" "UFID;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_UFID})
    ntf_assert_defined(${ARG_OUTPUT})

    string(REGEX MATCHALL "[^-_]+" ufid_list "${ARG_UFID}")

    ntf_ufid_list_canonicalize(UFID ${ufid_list} OUTPUT result)

    string(REPLACE ";" "_" result_string "${result}")

    set(${ARG_OUTPUT} ${result_string} PARENT_SCOPE)
endfunction()




# Parse a UFID and return its canonical form used for building.
#
# UFID   - The requested UFID or the empty string to use the UFID implied by
#          CMAKE_BUILD_TYPE.
# OUTPUT - The variable name set in the parent scope.
function (ntf_ufid_parse_for_build)
    cmake_parse_arguments(
        ARG "" "UFID;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    # Set the requested UFID from the function argument or from the
    # CMAKE_BUILD_TYPE if the function argument is not defined.

    set(build_ufid "${ARG_UFID}")

    if ("${build_ufid}" STREQUAL "")
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(build_ufid "dbg_exc_mt")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(build_ufid "opt_exc_mt")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
            set(build_ufid "opt_dbg_exc_mt")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
            set(build_ufid "opt_exc_mt")
        else()
            set(build_ufid "opt_dbg_exc_mt")
        endif()

        if(CMAKE_SIZEOF_VOID_P EQUAL 8 OR
           "${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS" OR
           "${CMAKE_SYSTEM_NAME}" STREQUAL "AIX")
            set(build_ufid "${build_ufid}_64")
        endif()

        if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "AIX" AND
           NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
            set(build_ufid "${build_ufid}_cpp17")
        endif()
    endif()

    # If "exc" or "mt" is not present, include them as it is now not supported
    # in the UFID system to not build with support for exceptions or threading.

    set(build_ufid "${build_ufid}_exc_mt")

    # Canonicalize the requested UFID.

    ntf_ufid_string_canonicalize(
        UFID ${build_ufid} OUTPUT canonical_build_ufid)

    # UFIDs specifically requesting position-independant code are treated
    # as if the flag is not present. All builds use position-independant code
    # and the required "pic"-flavored symlinks are installed.

    string(REPLACE "_pic"    "" canonical_build_ufid "${canonical_build_ufid}")

    if ("${canonical_build_ufid}" STREQUAL "")
        ntf_die("The UFID ${build_ufid} is not valid")
    endif()

    set(${ARG_OUTPUT} ${canonical_build_ufid} PARENT_SCOPE)
endfunction()

# Parse a UFID and return its canonical form used for installing.
#
# UFID   - The requested UFID or the empty string to use the UFID implied by
#          CMAKE_BUILD_TYPE.
# OUTPUT - The variable name set in the parent scope.
function (ntf_ufid_parse_for_install)
    cmake_parse_arguments(
        ARG "" "UFID;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_OUTPUT})

    ntf_ufid_parse_for_build(UFID "${ARG_UFID}" OUTPUT build_ufid)

    set(install_ufid ${build_ufid})

    string(REPLACE "_64"    "" install_ufid "${install_ufid}")
    string(REPLACE "_cpp03" "" install_ufid "${install_ufid}")
    string(REPLACE "_cpp14" "" install_ufid "${install_ufid}")
    string(REPLACE "_cpp17" "" install_ufid "${install_ufid}")
    string(REPLACE "_cpp20" "" install_ufid "${install_ufid}")

    if ("${install_ufid}" STREQUAL "")
        ntf_die("The install UFID ${install_ufid} is not valid")
    endif()

    set(${ARG_OUTPUT} ${install_ufid} PARENT_SCOPE)
endfunction()

# Add a flag to a UFID and return its canonical form.
#
# UFID   - The UFID
# FLAG   - The flag to add
# OUTPUT - The variable name set in the parent scope
function (ntf_ufid_add)
    cmake_parse_arguments(
        ARG "" "UFID;FLAG;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_UFID})
    ntf_assert_defined(${ARG_FLAG})
    ntf_assert_defined(${ARG_OUTPUT})

    set(ufid "${ARG_UFID}_${ARG_FLAG}")

    ntf_ufid_string_canonicalize(
        UFID ${ufid} OUTPUT canonical_ufid)

    if ("${canonical_ufid}" STREQUAL "")
        ntf_die("The UFID ${ufid} is not valid")
    endif()

    set(${ARG_OUTPUT} ${canonical_ufid} PARENT_SCOPE)
endfunction()

# Read meta-data from a file and return it as a list.
#
# PATH   - The path to the meta-data file
# OUTPUT - The variable name set in the parent scope
function (ntf_file_metadata)
    cmake_parse_arguments(
        ARG "" "PATH;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_PATH})
    ntf_assert_defined(${ARG_OUTPUT})

    set(items "")

    file(STRINGS ${ARG_PATH} lines)

    foreach (line IN LISTS lines)
        if (line)
            string(REGEX REPLACE " *#.*$" "" line "${line}")
            string(STRIP "${line}" line)
        endif()
        if (line)
            string(REGEX REPLACE " +" ";" line_list "${line}")
            list(APPEND items ${line_list})
        endif()
    endforeach()

    set(${ARG_OUTPUT} ${items} PARENT_SCOPE)

endfunction()

# Convert all redundant slashes to the minimal normalized form as forward
# slashes, or, if the NATIVE flag is specified, to forward slashes on Unixes
# and backward slashes on Windows.
#
# INPUT    - The path.
# OUTPUT   - The variable name set in the parent scope.
function (ntf_path_normalize)
    cmake_parse_arguments(
        ARG "NATIVE;ESCAPE" "INPUT;OUTPUT" "" ${ARGN})

    ntf_assert_defined(${ARG_INPUT})
    ntf_assert_defined(${ARG_OUTPUT})

    set(input "${ARG_INPUT}")

    if (${ARG_NATIVE})
        cmake_path(NATIVE_PATH input NORMALIZE result)
        if (${ARG_ESCAPE})
            string(REPLACE "\\" "\\\\" result "${result}")
        endif()
    else()
        cmake_path(NORMAL_PATH input OUTPUT_VARIABLE result)
    endif()

    set(${ARG_OUTPUT} ${result} PARENT_SCOPE)
endfunction()


# Test this module.
function (ntf_test)
    set(requested_ufid_list)
    list(APPEND requested_ufid_list "dbg")
    list(APPEND requested_ufid_list "dbg_opt")
    list(APPEND requested_ufid_list "opt")
    list(APPEND requested_ufid_list "dbg_opt_cpp17_dbg_exc_mt")

    ntf_ufid_parse_for_build(UFID "" OUTPUT build_ufid)
    ntf_ufid_parse_for_install(UFID "${build_ufid}" OUTPUT install_ufid)

    message(STATUS "**")
    message(STATUS "O: CMAKE_BUILD_TYPE")
    message(STATUS "B: ${build_ufid}")
    message(STATUS "B: ${install_ufid}")

    foreach (requested_ufid ${requested_ufid_list})
        ntf_ufid_parse_for_build(UFID "${requested_ufid}" OUTPUT build_ufid)
        ntf_ufid_parse_for_install(UFID "${build_ufid}" OUTPUT install_ufid)

        set(alternate_install_ufid "${install_ufid}")
        ntf_ufid_add(UFID "${alternate_install_ufid}" FLAG "pic" OUTPUT alternate_install_ufid)
        ntf_ufid_add(UFID "${alternate_install_ufid}" FLAG "tsan" OUTPUT alternate_install_ufid)

        message(STATUS "**")
        message(STATUS "O: ${requested_ufid}")
        message(STATUS "B: ${build_ufid}")
        message(STATUS "B: ${install_ufid}")
        message(STATUS "A: ${alternate_install_ufid}")
    endforeach()

    message(FATAL_ERROR "Test complete")
endfunction()
