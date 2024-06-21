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

# NTF Standard Variables

if (NOT DEFINED NTF_BUILD_FROM_SUPERPROJECT)
    if(NOT "${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
        set(NTF_BUILD_FROM_SUPERPROJECT TRUE CACHE INTERNAL "")
    else()
        set(NTF_BUILD_FROM_SUPERPROJECT FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_FROM_CONTINUOUS_INTEGRATION)
    if (DEFINED NTF_CONFIGURE_FROM_CONTINUOUS_INTEGRATION)
        set(NTF_BUILD_FROM_CONTINUOUS_INTEGRATION
            ${NTF_CONFIGURE_FROM_CONTINUOUS_INTEGRATION} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_FROM_CONTINUOUS_INTEGRATION})
        set(NTF_BUILD_FROM_CONTINUOUS_INTEGRATION
            $ENV{NTF_CONFIGURE_FROM_CONTINUOUS_INTEGRATION} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_FROM_CONTINUOUS_INTEGRATION FALSE)
    endif()
endif()

if (NOT DEFINED NTF_BUILD_FROM_PACKAGING)
    if (DEFINED NTF_CONFIGURE_FROM_PACKAGING)
        set(NTF_BUILD_FROM_PACKAGING
            ${NTF_CONFIGURE_FROM_PACKAGING} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_FROM_PACKAGING})
        set(NTF_BUILD_FROM_PACKAGING
            $ENV{NTF_CONFIGURE_FROM_PACKAGING} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_FROM_PACKAGING FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_REFROOT)
    if (DEFINED DISTRIBUTION_REFROOT)
        set(NTF_BUILD_REFROOT ${DISTRIBUTION_REFROOT} CACHE INTERNAL "")
    elseif (DEFINED ENV{DISTRIBUTION_REFROOT})
        set(NTF_BUILD_REFROOT $ENV{DISTRIBUTION_REFROOT} CACHE INTERNAL "")
    elseif (DEFINED NTF_CONFIGURE_REFROOT)
        set(NTF_BUILD_REFROOT ${NTF_CONFIGURE_REFROOT} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_REFROOT})
        set(NTF_BUILD_REFROOT $ENV{NTF_CONFIGURE_REFROOT} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_REFROOT "" CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_PREFIX)
    if (DEFINED PREFIX)
        set(NTF_BUILD_PREFIX ${PREFIX} CACHE INTERNAL "")
    elseif (DEFINED ENV{PREFIX})
        set(NTF_BUILD_PREFIX $ENV{PREFIX} CACHE INTERNAL "")
    elseif (DEFINED NTF_CONFIGURE_PREFIX)
        set(NTF_BUILD_PREFIX ${NTF_CONFIGURE_PREFIX} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_PREFIX})
        set(NTF_BUILD_PREFIX $ENV{NTF_CONFIGURE_PREFIX} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_PREFIX_PATH)
    if (DEFINED NTF_CONFIGURE_PREFIX_PATH)
        set(NTF_BUILD_PREFIX_PATH
            ${NTF_CONFIGURE_PREFIX_PATH} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_PREFIX_PATH})
        set(NTF_BUILD_PREFIX_PATH
            $ENV{NTF_CONFIGURE_PREFIX_PATH} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_PREFIX_PATH ${CMAKE_PREFIX_PATH} CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_UNAME)
    if (DEFINED NTF_CONFIGURE_UNAME)
        set(NTF_BUILD_UNAME ${NTF_CONFIGURE_UNAME} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_UNAME})
        set(NTF_BUILD_UNAME $ENV{NTF_CONFIGURE_UNAME} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_UNAME ${CMAKE_SYSTEM_NAME} CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_UFID)
    if (DEFINED UFID)
        set(NTF_BUILD_UFID ${UFID} CACHE INTERNAL "")
    elseif (DEFINED ENV{UFID})
        set(NTF_BUILD_UFID $ENV{UFID} CACHE INTERNAL "")
    elseif (DEFINED NTF_CONFIGURE_UFID)
        set(NTF_BUILD_UFID ${NTF_CONFIGURE_UFID} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_UFID})
        set(NTF_BUILD_UFID $ENV{NTF_CONFIGURE_UFID} CACHE INTERNAL "")
    else()
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(NTF_BUILD_UFID "dbg_exc_mt" CACHE INTERNAL "")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(NTF_BUILD_UFID "opt_exc_mt" CACHE INTERNAL "")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
            set(NTF_BUILD_UFID "opt_dbg_exc_mt" CACHE INTERNAL "")
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
            set(NTF_BUILD_UFID "opt_exc_mt" CACHE INTERNAL "")
        else()
            set(NTF_BUILD_UFID "opt_dbg_exc_mt" CACHE INTERNAL "")
        endif()

        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(NTF_BUILD_UFID "${NTF_BUILD_UFID}_64" CACHE INTERNAL "")
        endif()

        if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "AIX" AND
           NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
            set(NTF_BUILD_UFID "${NTF_BUILD_UFID}_cpp17" CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_DEPRECATED_FEATURES)
    if (DEFINED NTF_CONFIGURE_WITH_DEPRECATED_FEATURES)
        set(NTF_BUILD_WITH_DEPRECATED_FEATURES
            ${NTF_CONFIGURE_WITH_DEPRECATED_FEATURES} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_DEPRECATED_FEATURES})
        set(NTF_BUILD_WITH_DEPRECATED_FEATURES
            $ENV{NTF_CONFIGURE_WITH_DEPRECATED_FEATURES} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_DEPRECATED_FEATURES TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_WARNINGS)
    if (DEFINED NTF_CONFIGURE_WITH_WARNINGS)
        set(NTF_BUILD_WITH_WARNINGS
            ${NTF_CONFIGURE_WITH_WARNINGS} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_WARNINGS})
        set(NTF_BUILD_WITH_WARNINGS
            $ENV{NTF_CONFIGURE_WITH_WARNINGS} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_WARNINGS FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_WARNINGS_AS_ERRORS)
    if (DEFINED NTF_CONFIGURE_WITH_WARNINGS_AS_ERRORS)
        set(NTF_BUILD_WITH_WARNINGS_AS_ERRORS
            ${NTF_CONFIGURE_WITH_WARNINGS_AS_ERRORS} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_WARNINGS_AS_ERRORS})
        set(NTF_BUILD_WITH_WARNINGS_AS_ERRORS
            $ENV{NTF_CONFIGURE_WITH_WARNINGS_AS_ERRORS} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_WARNINGS_AS_ERRORS FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_DOCUMENTATION)
    if (DEFINED NTF_CONFIGURE_WITH_DOCUMENTATION)
        set(NTF_BUILD_WITH_DOCUMENTATION
            ${NTF_CONFIGURE_WITH_DOCUMENTATION} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_DOCUMENTATION})
        set(NTF_BUILD_WITH_DOCUMENTATION
            $ENV{NTF_CONFIGURE_WITH_DOCUMENTATION} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_DOCUMENTATION FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_DOCUMENTATION_INTERNAL)
    if (DEFINED NTF_CONFIGURE_WITH_DOCUMENTATION_INTERNAL)
        set(NTF_BUILD_WITH_DOCUMENTATION_INTERNAL
            ${NTF_CONFIGURE_WITH_DOCUMENTATION_INTERNAL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_DOCUMENTATION_INTERNAL})
        set(NTF_BUILD_WITH_DOCUMENTATION_INTERNAL
            $ENV{NTF_CONFIGURE_WITH_DOCUMENTATION_INTERNAL} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_DOCUMENTATION_INTERNAL FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_COVERAGE)
    if (DEFINED NTF_CONFIGURE_WITH_COVERAGE)
        set(NTF_BUILD_WITH_COVERAGE
            ${NTF_CONFIGURE_WITH_COVERAGE} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_COVERAGE})
        set(NTF_BUILD_WITH_COVERAGE
            $ENV{NTF_CONFIGURE_WITH_COVERAGE} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_COVERAGE FALSE CACHE INTERNAL "")
    endif()
endif()


if (NOT DEFINED NTF_BUILD_WITH_TIME_TRACE)
    if (DEFINED NTF_CONFIGURE_WITH_TIME_TRACE)
        set(NTF_BUILD_WITH_TIME_TRACE
            ${NTF_CONFIGURE_WITH_TIME_TRACE} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_TIME_TRACE})
        set(NTF_BUILD_WITH_TIME_TRACE
            $ENV{NTF_CONFIGURE_WITH_TIME_TRACE} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_TIME_TRACE FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_TIME_REPORT)
    if (DEFINED NTF_CONFIGURE_WITH_TIME_REPORT)
        set(NTF_BUILD_WITH_TIME_REPORT
            ${NTF_CONFIGURE_WITH_TIME_REPORT} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_TIME_REPORT})
        set(NTF_BUILD_WITH_TIME_REPORT
            $ENV{NTF_CONFIGURE_WITH_TIME_REPORT} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_TIME_REPORT FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_STACK_TRACE_LEAK_REPORT)
    if (DEFINED NTF_CONFIGURE_WITH_STACK_TRACE_LEAK_REPORT)
        set(NTF_BUILD_WITH_STACK_TRACE_LEAK_REPORT
            ${NTF_CONFIGURE_WITH_STACK_TRACE_LEAK_REPORT} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_STACK_TRACE_LEAK_REPORT})
        set(NTF_BUILD_WITH_STACK_TRACE_LEAK_REPORT
            $ENV{NTF_CONFIGURE_WITH_STACK_TRACE_LEAK_REPORT} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_STACK_TRACE_LEAK_REPORT FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR)
    if (DEFINED NTF_CONFIGURE_WITH_STACK_TRACE_TEST_ALLOCATOR)
        set(NTF_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR
            ${NTF_CONFIGURE_WITH_STACK_TRACE_TEST_ALLOCATOR} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_STACK_TRACE_TEST_ALLOCATOR})
        set(NTF_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR
            $ENV{NTF_CONFIGURE_WITH_STACK_TRACE_TEST_ALLOCATOR} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_USAGE_EXAMPLES)
    if (DEFINED NTF_CONFIGURE_WITH_USAGE_EXAMPLES)
        set(NTF_BUILD_WITH_USAGE_EXAMPLES
            ${NTF_CONFIGURE_WITH_USAGE_EXAMPLES} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_USAGE_EXAMPLES})
        set(NTF_BUILD_WITH_USAGE_EXAMPLES
            $ENV{NTF_CONFIGURE_WITH_USAGE_EXAMPLES} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_USAGE_EXAMPLES TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_MOCKS)
    if (DEFINED NTF_CONFIGURE_WITH_MOCKS)
        set(NTF_BUILD_WITH_MOCKS
            ${NTF_CONFIGURE_WITH_MOCKS} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_MOCKS})
        set(NTF_BUILD_WITH_MOCKS
            $ENV{NTF_CONFIGURE_WITH_MOCKS} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_MOCKS FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_INTEGRATION_TESTS)
    if (DEFINED NTF_CONFIGURE_WITH_INTEGRATION_TESTS)
        set(NTF_BUILD_WITH_INTEGRATION_TESTS
            ${NTF_CONFIGURE_WITH_INTEGRATION_TESTS} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_INTEGRATION_TESTS})
        set(NTF_BUILD_WITH_INTEGRATION_TESTS
            $ENV{NTF_CONFIGURE_WITH_INTEGRATION_TESTS} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_INTEGRATION_TESTS FALSE CACHE INTERNAL "")
    endif()
endif()

# NTF Repository Variables

if (NOT DEFINED NTF_BUILD_WITH_BSL)
    if (DEFINED NTF_CONFIGURE_WITH_BSL)
        set(NTF_BUILD_WITH_BSL ${NTF_CONFIGURE_WITH_BSL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_BSL})
        set(NTF_BUILD_WITH_BSL $ENV{NTF_CONFIGURE_WITH_BSL} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_BSL TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_BDL)
    if (DEFINED NTF_CONFIGURE_WITH_BDL)
        set(NTF_BUILD_WITH_BDL ${NTF_CONFIGURE_WITH_BDL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_BDL})
        set(NTF_BUILD_WITH_BDL $ENV{NTF_CONFIGURE_WITH_BDL} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_BDL TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_BAL)
    if (DEFINED NTF_CONFIGURE_WITH_BAL)
        set(NTF_BUILD_WITH_BAL ${NTF_CONFIGURE_WITH_BAL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_BAL})
        set(NTF_BUILD_WITH_BAL $ENV{NTF_CONFIGURE_WITH_BAL} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_BAL TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_NTS)
    if (DEFINED NTF_CONFIGURE_WITH_NTS)
        set(NTF_BUILD_WITH_NTS ${NTF_CONFIGURE_WITH_NTS} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_NTS})
        set(NTF_BUILD_WITH_NTS $ENV{NTF_CONFIGURE_WITH_NTS} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_NTS TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_NTC)
    if (DEFINED NTF_CONFIGURE_WITH_NTC)
        set(NTF_BUILD_WITH_NTC ${NTF_CONFIGURE_WITH_NTC} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_NTC})
        set(NTF_BUILD_WITH_NTC $ENV{NTF_CONFIGURE_WITH_NTC} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_NTC TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_ADDRESS_FAMILY_IPV4)
    if (DEFINED NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4)
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_IPV4
            ${NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4})
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_IPV4
            $ENV{NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV4} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_IPV4 TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_ADDRESS_FAMILY_IPV6)
    if (DEFINED NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV6)
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_IPV6
            ${NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV6} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV6})
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_IPV6
            $ENV{NTF_CONFIGURE_WITH_ADDRESS_FAMILY_IPV6} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_IPV6 TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_ADDRESS_FAMILY_LOCAL)
    if (DEFINED NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL)
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_LOCAL
            ${NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL})
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_LOCAL
            $ENV{NTF_CONFIGURE_WITH_ADDRESS_FAMILY_LOCAL} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_ADDRESS_FAMILY_LOCAL TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_TRANSPORT_PROTOCOL_TCP)
    if (DEFINED NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_TCP)
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_TCP
            ${NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_TCP} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_TCP})
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_TCP
            $ENV{NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_TCP} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_TCP TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_TRANSPORT_PROTOCOL_UDP)
    if (DEFINED NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_UDP)
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_UDP
            ${NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_UDP} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_UDP})
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_UDP
            $ENV{NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_UDP} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_UDP TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL)
    if (DEFINED NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL)
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
            ${NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL})
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL
            $ENV{NTF_CONFIGURE_WITH_TRANSPORT_PROTOCOL_LOCAL} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_SELECT)
    if (DEFINED NTF_CONFIGURE_WITH_SELECT)
        set(NTF_BUILD_WITH_SELECT
            ${NTF_CONFIGURE_WITH_SELECT} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_SELECT})
        set(NTF_BUILD_WITH_SELECT
            $ENV{NTF_CONFIGURE_WITH_SELECT} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_SELECT TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_POLL)
    if (DEFINED NTF_CONFIGURE_WITH_POLL)
        set(NTF_BUILD_WITH_POLL
            ${NTF_CONFIGURE_WITH_POLL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_POLL})
        set(NTF_BUILD_WITH_POLL
            $ENV{NTF_CONFIGURE_WITH_POLL} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_POLL TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_EPOLL)
    if (DEFINED NTF_CONFIGURE_WITH_EPOLL)
        set(NTF_BUILD_WITH_EPOLL
            ${NTF_CONFIGURE_WITH_EPOLL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_EPOLL})
        set(NTF_BUILD_WITH_EPOLL
            $ENV{NTF_CONFIGURE_WITH_EPOLL} CACHE INTERNAL "")
    else()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
            set(NTF_BUILD_WITH_EPOLL TRUE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_EPOLL FALSE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_DEVPOLL)
    if (DEFINED NTF_CONFIGURE_WITH_DEVPOLL)
        set(NTF_BUILD_WITH_DEVPOLL
            ${NTF_CONFIGURE_WITH_DEVPOLL} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_DEVPOLL})
        set(NTF_BUILD_WITH_DEVPOLL
            $ENV{NTF_CONFIGURE_WITH_DEVPOLL} CACHE INTERNAL "")
    else()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
            set(NTF_BUILD_WITH_DEVPOLL TRUE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_DEVPOLL FALSE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_EVENTPORT)
    if (DEFINED NTF_CONFIGURE_WITH_EVENTPORT)
        set(NTF_BUILD_WITH_EVENTPORT
            ${NTF_CONFIGURE_WITH_EVENTPORT} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_EVENTPORT})
        set(NTF_BUILD_WITH_EVENTPORT
            $ENV{NTF_CONFIGURE_WITH_EVENTPORT} CACHE INTERNAL "")
    else()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
            set(NTF_BUILD_WITH_EVENTPORT TRUE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_EVENTPORT FALSE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_POLLSET)
    if (DEFINED NTF_CONFIGURE_WITH_POLLSET)
        set(NTF_BUILD_WITH_POLLSET
            ${NTF_CONFIGURE_WITH_POLLSET} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_POLLSET})
        set(NTF_BUILD_WITH_POLLSET
            $ENV{NTF_CONFIGURE_WITH_POLLSET} CACHE INTERNAL "")
    else()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "AIX")
            set(NTF_BUILD_WITH_POLLSET TRUE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_POLLSET FALSE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_KQUEUE)
    if (DEFINED NTF_CONFIGURE_WITH_KQUEUE)
        set(NTF_BUILD_WITH_KQUEUE
            ${NTF_CONFIGURE_WITH_KQUEUE} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_KQUEUE})
        set(NTF_BUILD_WITH_KQUEUE
            $ENV{NTF_CONFIGURE_WITH_KQUEUE} CACHE INTERNAL "")
    else()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR
           ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
            set(NTF_BUILD_WITH_KQUEUE TRUE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_KQUEUE FALSE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_IOCP)
    if (DEFINED NTF_CONFIGURE_WITH_IOCP)
        set(NTF_BUILD_WITH_IOCP
            ${NTF_CONFIGURE_WITH_IOCP} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_IOCP})
        set(NTF_BUILD_WITH_IOCP
            $ENV{NTF_CONFIGURE_WITH_IOCP} CACHE INTERNAL "")
    else()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
            set(NTF_BUILD_WITH_IOCP TRUE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_IOCP FALSE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_IORING)
    if (DEFINED NTF_CONFIGURE_WITH_IORING)
        set(NTF_BUILD_WITH_IORING
            ${NTF_CONFIGURE_WITH_IORING} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_IORING})
        set(NTF_BUILD_WITH_IORING
            $ENV{NTF_CONFIGURE_WITH_IORING} CACHE INTERNAL "")
    else()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
            set(NTF_BUILD_WITH_IORING FALSE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_IORING FALSE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_DYNAMIC_LOAD_BALANCING)
    if (DEFINED NTF_CONFIGURE_WITH_DYNAMIC_LOAD_BALANCING)
        set(NTF_BUILD_WITH_DYNAMIC_LOAD_BALANCING
            ${NTF_CONFIGURE_WITH_DYNAMIC_LOAD_BALANCING} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_DYNAMIC_LOAD_BALANCING})
        set(NTF_BUILD_WITH_DYNAMIC_LOAD_BALANCING
            $ENV{NTF_CONFIGURE_WITH_DYNAMIC_LOAD_BALANCING} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_DYNAMIC_LOAD_BALANCING TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_THREAD_SCALING)
    if (DEFINED NTF_CONFIGURE_WITH_THREAD_SCALING)
        set(NTF_BUILD_WITH_THREAD_SCALING
            ${NTF_CONFIGURE_WITH_THREAD_SCALING} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_THREAD_SCALING})
        set(NTF_BUILD_WITH_THREAD_SCALING
            $ENV{NTF_CONFIGURE_WITH_THREAD_SCALING} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_THREAD_SCALING TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_LOGGING)
    if (DEFINED NTF_CONFIGURE_WITH_LOGGING)
        set(NTF_BUILD_WITH_LOGGING
            ${NTF_CONFIGURE_WITH_LOGGING} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_LOGGING})
        set(NTF_BUILD_WITH_LOGGING
            $ENV{NTF_CONFIGURE_WITH_LOGGING} CACHE INTERNAL "")
    else()
        if (NOT "${UFID}" STREQUAL "")
            if (NOT ${UFID} MATCHES "opt")
                set(NTF_BUILD_WITH_LOGGING TRUE CACHE INTERNAL "")
            else()
                set(NTF_BUILD_WITH_LOGGING FALSE CACHE INTERNAL "")
            endif()
        else()
            if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
                set(NTF_BUILD_WITH_LOGGING TRUE CACHE INTERNAL "")
            else()
                set(NTF_BUILD_WITH_LOGGING FALSE CACHE INTERNAL "")
            endif()
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_METRICS)
    if (DEFINED NTF_CONFIGURE_WITH_METRICS)
        set(NTF_BUILD_WITH_METRICS
            ${NTF_CONFIGURE_WITH_METRICS} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_METRICS})
        set(NTF_BUILD_WITH_METRICS
            $ENV{NTF_CONFIGURE_WITH_METRICS} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_METRICS TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_INLINING_SUGGESTED)
    if (DEFINED NTF_CONFIGURE_WITH_INLINING_SUGGESTED)
        set(NTF_BUILD_WITH_INLINING_SUGGESTED
            ${NTF_CONFIGURE_WITH_INLINING_SUGGESTED} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_INLINING_SUGGESTED})
        set(NTF_BUILD_WITH_INLINING_SUGGESTED
            $ENV{NTF_CONFIGURE_WITH_INLINING_SUGGESTED} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_INLINING_SUGGESTED FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_INLINING_FORCED)
    if (DEFINED NTF_CONFIGURE_WITH_INLINING_FORCED)
        set(NTF_BUILD_WITH_INLINING_FORCED
            ${NTF_CONFIGURE_WITH_INLINING_FORCED} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_INLINING_FORCED})
        set(NTF_BUILD_WITH_INLINING_FORCED
            $ENV{NTF_CONFIGURE_WITH_INLINING_FORCED} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_INLINING_FORCED TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_INLINING_DISABLED)
    if (DEFINED NTF_CONFIGURE_WITH_INLINING_DISABLED)
        set(NTF_BUILD_WITH_INLINING_DISABLED
            ${NTF_CONFIGURE_WITH_INLINING_DISABLED} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_INLINING_DISABLED})
        set(NTF_BUILD_WITH_INLINING_DISABLED
            $ENV{NTF_CONFIGURE_WITH_INLINING_DISABLED} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_INLINING_DISABLED FALSE CACHE INTERNAL "")
    endif()
endif()

if (${NTF_BUILD_WITH_INLINING_DISABLED})
    set(NTF_BUILD_WITH_INLINING_SUGGESTED FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_INLINING_FORCED FALSE CACHE INTERNAL "")
endif()

if (${NTF_BUILD_WITH_INLINING_FORCED})
    set(NTF_BUILD_WITH_INLINING_DISABLED FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_INLINING_SUGGESTED FALSE CACHE INTERNAL "")
endif()

if (${NTF_BUILD_WITH_INLINING_SUGGESTED})
    set(NTF_BUILD_WITH_INLINING_DISABLED FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_INLINING_FORCED FALSE CACHE INTERNAL "")
endif()

if (NOT ${NTF_BUILD_WITH_INLINING_DISABLED} AND
    NOT ${NTF_BUILD_WITH_INLINING_FORCED} AND
    NOT ${NTF_BUILD_WITH_INLINING_SUGGESTED})
    set(NTF_BUILD_WITH_INLINING_FORCED TRUE CACHE INTERNAL "")
endif()

if (NOT DEFINED NTF_BUILD_WITH_BRANCH_PREDICTION)
    if (DEFINED NTF_CONFIGURE_WITH_BRANCH_PREDICTION)
        set(NTF_BUILD_WITH_BRANCH_PREDICTION
            ${NTF_CONFIGURE_WITH_BRANCH_PREDICTION} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_BRANCH_PREDICTION})
        set(NTF_BUILD_WITH_BRANCH_PREDICTION
            $ENV{NTF_CONFIGURE_WITH_BRANCH_PREDICTION} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_BRANCH_PREDICTION TRUE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_SPIN_LOCKS)
    if (DEFINED NTF_CONFIGURE_WITH_SPIN_LOCKS)
        set(NTF_BUILD_WITH_SPIN_LOCKS
            ${NTF_CONFIGURE_WITH_SPIN_LOCKS} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_SPIN_LOCKS})
        set(NTF_BUILD_WITH_SPIN_LOCKS
            $ENV{NTF_CONFIGURE_WITH_SPIN_LOCKS} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_SPIN_LOCKS FALSE CACHE INTERNAL "")
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_USERSPACE_MUTEXES)
    if (DEFINED NTF_CONFIGURE_WITH_USERSPACE_MUTEXES)
        set(NTF_BUILD_WITH_USERSPACE_MUTEXES
            ${NTF_CONFIGURE_WITH_USERSPACE_MUTEXES} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_USERSPACE_MUTEXES})
        set(NTF_BUILD_WITH_USERSPACE_MUTEXES
            $ENV{NTF_CONFIGURE_WITH_USERSPACE_MUTEXES} CACHE INTERNAL "")
    else()
        if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
            set(NTF_BUILD_WITH_USERSPACE_MUTEXES TRUE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_USERSPACE_MUTEXES FALSE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_SYSTEM_MUTEXES)
    if (DEFINED NTF_CONFIGURE_WITH_SYSTEM_MUTEXES)
        set(NTF_BUILD_WITH_SYSTEM_MUTEXES
            ${NTF_CONFIGURE_WITH_SYSTEM_MUTEXES} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_SYSTEM_MUTEXES})
        set(NTF_BUILD_WITH_SYSTEM_MUTEXES
            $ENV{NTF_CONFIGURE_WITH_SYSTEM_MUTEXES} CACHE INTERNAL "")
    else()
        if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
            set(NTF_BUILD_WITH_SYSTEM_MUTEXES FALSE CACHE INTERNAL "")
        else()
            set(NTF_BUILD_WITH_SYSTEM_MUTEXES TRUE CACHE INTERNAL "")
        endif()
    endif()
endif()

if (NOT DEFINED NTF_BUILD_WITH_RECURSIVE_MUTEXES)
    if (DEFINED NTF_CONFIGURE_WITH_RECURSIVE_MUTEXES)
        set(NTF_BUILD_WITH_RECURSIVE_MUTEXES
            ${NTF_CONFIGURE_WITH_RECURSIVE_MUTEXES} CACHE INTERNAL "")
    elseif (DEFINED ENV{NTF_CONFIGURE_WITH_RECURSIVE_MUTEXES})
        set(NTF_BUILD_WITH_RECURSIVE_MUTEXES
            $ENV{NTF_CONFIGURE_WITH_RECURSIVE_MUTEXES} CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_RECURSIVE_MUTEXES FALSE CACHE INTERNAL "")
    endif()
endif()

if (${NTF_BUILD_WITH_SPIN_LOCKS})
    set(NTF_BUILD_WITH_USERSPACE_MUTEXES FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_SYSTEM_MUTEXES FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_RECURSIVE_MUTEXES FALSE CACHE INTERNAL "")
endif()

if (${NTF_BUILD_WITH_USERSPACE_MUTEXES})
    set(NTF_BUILD_WITH_SPIN_LOCKS FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_SYSTEM_MUTEXES FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_RECURSIVE_MUTEXES FALSE CACHE INTERNAL "")
endif()

if (${NTF_BUILD_WITH_SYSTEM_MUTEXES})
    set(NTF_BUILD_WITH_SPIN_LOCKS FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_USERSPACE_MUTEXES FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_RECURSIVE_MUTEXES FALSE CACHE INTERNAL "")
endif()

if (${NTF_BUILD_WITH_RECURSIVE_MUTEXES})
    set(NTF_BUILD_WITH_SPIN_LOCKS FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_USERSPACE_MUTEXES FALSE CACHE INTERNAL "")
    set(NTF_BUILD_WITH_SYSTEM_MUTEXES FALSE CACHE INTERNAL "")
endif()

if (NOT ${NTF_BUILD_WITH_SPIN_LOCKS} AND
    NOT ${NTF_BUILD_WITH_USERSPACE_MUTEXES} AND
    NOT ${NTF_BUILD_WITH_SYSTEM_MUTEXES} AND
    NOT ${NTF_BUILD_WITH_RECURSIVE_MUTEXES})
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(NTF_BUILD_WITH_USERSPACE_MUTEXES TRUE CACHE INTERNAL "")
    else()
        set(NTF_BUILD_WITH_SYSTEM_MUTEXES TRUE CACHE INTERNAL "")
    endif()
endif()

# Convert NTF_BUILD_* "boolean" variables into 0 or 1 for suitability for
# use in generating the configuration headers.

unset(NTF_BUILD_VARIABLE_LIST)
get_cmake_property(NTF_BUILD_VARIABLE_LIST VARIABLES)
list(SORT NTF_BUILD_VARIABLE_LIST)
foreach (NTF_BUILD_VARIABLE_NAME ${NTF_BUILD_VARIABLE_LIST})
    if (NTF_BUILD_VARIABLE_NAME MATCHES "NTF_BUILD_")
        string(TOUPPER "${${NTF_BUILD_VARIABLE_NAME}}"
               NTF_BUILD_VARIABLE_VALUE)

        if ("${NTF_BUILD_VARIABLE_VALUE}" STREQUAL "Y" OR
            "${NTF_BUILD_VARIABLE_VALUE}" STREQUAL "YES" OR
            "${NTF_BUILD_VARIABLE_VALUE}" STREQUAL "ON" OR
            "${NTF_BUILD_VARIABLE_VALUE}" STREQUAL "TRUE")
            set(${NTF_BUILD_VARIABLE_NAME} 1 CACHE INTERNAL "")
        endif()

        if ("${NTF_BUILD_VARIABLE_VALUE}" STREQUAL "N" OR
            "${NTF_BUILD_VARIABLE_VALUE}" STREQUAL "NO" OR
            "${NTF_BUILD_VARIABLE_VALUE}" STREQUAL "OFF" OR
            "${NTF_BUILD_VARIABLE_VALUE}" STREQUAL "FALSE")
            set(${NTF_BUILD_VARIABLE_NAME} 0 CACHE INTERNAL "")
        endif()
    endif()
endforeach()
unset(NTF_BUILD_VARIABLE_NAME)
unset(NTF_BUILD_VARIABLE_VALUE)
unset(NTF_BUILD_VARIABLE_LIST)

# Print variables.

message(STATUS "NTF: Repository:                                ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "NTF: Distribution refroot:                      ${NTF_BUILD_REFROOT}")
message(STATUS "NTF: Installation prefix:                       ${NTF_BUILD_PREFIX}")
message(STATUS "NTF: Installation prefix path:                  ${NTF_BUILD_PREFIX_PATH}")
message(STATUS "NTF: Platform:                                  ${NTF_BUILD_UNAME}")
message(STATUS "NTF: UFID:                                      ${NTF_BUILD_UFID}")

message(STATUS "NTF: Bitness:                                   ${CMAKE_SIZEOF_VOID_P}")

if (${NTF_BUILD_FROM_SUPERPROJECT})
    message(STATUS "NTF: Building from superproject:                yes")
else()
    message(STATUS "NTF: Building from superproject:                no")
endif()

if (${NTF_BUILD_FROM_PACKAGING})
    message(STATUS "NTF: Building from packaging:                   yes")
else()
    message(STATUS "NTF: Building from packaging:                   no")
endif()

if (${NTF_BUILD_FROM_CONTINUOUS_INTEGRATION})
    message(STATUS "NTF: Building from continuous integration:      yes")
else()
    message(STATUS "NTF: Building from continuous integration:      no")
endif()

if (${NTF_BUILD_WITH_USAGE_EXAMPLES})
    message(STATUS "NTF: Building with usage examples:              yes")
else()
    message(STATUS "NTF: Building with usage examples:              no")
endif()

if (${NTF_BUILD_WITH_MOCKS})
    message(STATUS "NTF: Building with mocks:                       yes")
else()
    message(STATUS "NTF: Building with mocks:                       no")
endif()

if (${NTF_BUILD_WITH_INTEGRATION_TESTS})
    message(STATUS "NTF: Building with integration tests:           yes")
else()
    message(STATUS "NTF: Building with integration tests:           no")
endif()

if (${NTF_BUILD_WITH_BSL})
    message(STATUS "NTF: Building with BSL:                         yes")
else()
    message(STATUS "NTF: Building with BSL:                         no")
endif()

if (${NTF_BUILD_WITH_BDL})
    message(STATUS "NTF: Building with BDL:                         yes")
else()
    message(STATUS "NTF: Building with BDL:                         no")
endif()

if (${NTF_BUILD_WITH_BAL})
    message(STATUS "NTF: Building with BAL:                         yes")
else()
    message(STATUS "NTF: Building with BAL:                         no")
endif()

if (${NTF_BUILD_WITH_NTS})
    message(STATUS "NTF: Building with NTS:                         yes")
else()
    message(STATUS "NTF: Building with NTS:                         no")
endif()

if (${NTF_BUILD_WITH_NTC})
    message(STATUS "NTF: Building with NTC:                         yes")
else()
    message(STATUS "NTF: Building with NTC:                         no")
endif()

if (${NTF_BUILD_WITH_ADDRESS_FAMILY_IPV4})
    message(STATUS "NTF: Building with IPv4:                        yes")
else()
    message(STATUS "NTF: Building with IPv4:                        no")
endif()

if (${NTF_BUILD_WITH_ADDRESS_FAMILY_IPV6})
    message(STATUS "NTF: Building with IPv6:                        yes")
else()
    message(STATUS "NTF: Building with IPv6:                        no")
endif()

if (${NTF_BUILD_WITH_TRANSPORT_PROTOCOL_TCP})
    message(STATUS "NTF: Building with TCP:                         yes")
else()
    message(STATUS "NTF: Building with TCP:                         no")
endif()

if (${NTF_BUILD_WITH_TRANSPORT_PROTOCOL_UDP})
    message(STATUS "NTF: Building with UDP:                         yes")
else()
    message(STATUS "NTF: Building with UDP:                         no")
endif()

if (${NTF_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL})
    message(STATUS "NTF: Building with Unix domain sockets:         yes")
else()
    message(STATUS "NTF: Building with Unix domain sockets:         no")
endif()

if (${NTF_BUILD_WITH_SELECT})
    message(STATUS "NTF: Building with select:                      yes")
else()
    message(STATUS "NTF: Building with select:                      no")
endif()

if (${NTF_BUILD_WITH_POLL})
    message(STATUS "NTF: Building with poll:                        yes")
else()
    message(STATUS "NTF: Building with poll:                        no")
endif()

if (${NTF_BUILD_WITH_EPOLL})
    message(STATUS "NTF: Building with epoll:                       yes")
else()
    message(STATUS "NTF: Building with epoll:                       no")
endif()

if (${NTF_BUILD_WITH_DEVPOLL})
    message(STATUS "NTF: Building with /dev/poll:                   yes")
else()
    message(STATUS "NTF: Building with /dev/poll:                   no")
endif()

if (${NTF_BUILD_WITH_EVENTPORT})
    message(STATUS "NTF: Building with event port:                  yes")
else()
    message(STATUS "NTF: Building with event port:                  no")
endif()

if (${NTF_BUILD_WITH_POLLSET})
    message(STATUS "NTF: Building with pollset:                     yes")
else()
    message(STATUS "NTF: Building with pollset:                     no")
endif()

if (${NTF_BUILD_WITH_KQUEUE})
    message(STATUS "NTF: Building with kqueue:                      yes")
else()
    message(STATUS "NTF: Building with kqueue:                      no")
endif()

if (${NTF_BUILD_WITH_IORING})
    message(STATUS "NTF: Building with io_uring:                    yes")
else()
    message(STATUS "NTF: Building with io_uring:                    no")
endif()

if (${NTF_BUILD_WITH_IOCP})
    message(STATUS "NTF: Building with I/O completion ports:        yes")
else()
    message(STATUS "NTF: Building with I/O completion ports:        no")
endif()

if (${NTF_BUILD_WITH_DYNAMIC_LOAD_BALANCING})
    message(STATUS "NTF: Building with dynamic load balancing:      yes")
else()
    message(STATUS "NTF: Building with dynamic load balancing:      no")
endif()

if (${NTF_BUILD_WITH_THREAD_SCALING})
    message(STATUS "NTF: Building with thread scaling:              yes")
else()
    message(STATUS "NTF: Building with thread scaling:              no")
endif()

if (${NTF_BUILD_WITH_LOGGING})
    message(STATUS "NTF: Building with logging:                     yes")
else()
    message(STATUS "NTF: Building with logging:                     no")
endif()

if (${NTF_BUILD_WITH_METRICS})
    message(STATUS "NTF: Building with metrics:                     yes")
else()
    message(STATUS "NTF: Building with metrics:                     no")
endif()

if (${NTF_BUILD_WITH_INLINING_SUGGESTED})
    message(STATUS "NTF: Building with inlining suggested:          yes")
else()
    message(STATUS "NTF: Building with inlining suggested:          no")
endif()

if (${NTF_BUILD_WITH_INLINING_FORCED})
    message(STATUS "NTF: Building with inlining forced:             yes")
else()
    message(STATUS "NTF: Building with inlining forced:             no")
endif()

if (${NTF_BUILD_WITH_INLINING_DISABLED})
    message(STATUS "NTF: Building with inlining disabled:           yes")
else()
    message(STATUS "NTF: Building with inlining disabled:           no")
endif()

if (${NTF_BUILD_WITH_BRANCH_PREDICTION})
    message(STATUS "NTF: Building with branch prediction:           yes")
else()
    message(STATUS "NTF: Building with branch prediction:           no")
endif()

if (${NTF_BUILD_WITH_SPIN_LOCKS})
    message(STATUS "NTF: Building with spin locks:                  yes")
else()
    message(STATUS "NTF: Building with spin locks:                  no")
endif()

if (${NTF_BUILD_WITH_USERSPACE_MUTEXES})
    message(STATUS "NTF: Building with userspace mutexes:           yes")
else()
    message(STATUS "NTF: Building with userspace mutexes:           no")
endif()

if (${NTF_BUILD_WITH_SYSTEM_MUTEXES})
    message(STATUS "NTF: Building with system mutexes:              yes")
else()
    message(STATUS "NTF: Building with system mutexes:              no")
endif()

if (${NTF_BUILD_WITH_RECURSIVE_MUTEXES})
    message(STATUS "NTF: Building with recursive mutexes:           yes")
else()
    message(STATUS "NTF: Building with recursive mutexes:           no")
endif()

if (${NTF_BUILD_WITH_STACK_TRACE_LEAK_REPORT})
    message(STATUS "NTF: Building with stack trace leak report:     yes")
else()
    message(STATUS "NTF: Building with stack trace leak report:     no")
endif()

if (${NTF_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR})
    message(STATUS "NTF: Building with stack trace test allocator:  yes")
else()
    message(STATUS "NTF: Building with stack trace test allocator:  no")
endif()

if (${NTF_BUILD_WITH_WARNINGS})
    message(STATUS "NTF: Building with warnings:                    yes")
else()
    message(STATUS "NTF: Building with warnings:                    no")
endif()

if (${NTF_BUILD_WITH_WARNINGS_AS_ERRORS})
    message(STATUS "NTF: Building with warnings as errors:          yes")
else()
    message(STATUS "NTF: Building with warnings as errors:          no")
endif()

if (${NTF_BUILD_WITH_DOCUMENTATION})
    message(STATUS "NTF: Building with documentation:               yes")
else()
    message(STATUS "NTF: Building with documentation:               no")
endif()

if (${NTF_BUILD_WITH_DOCUMENTATION_INTERNAL})
    message(STATUS "NTF: Building with documentation of internals:  yes")
else()
    message(STATUS "NTF: Building with documentation of internals:  no")
endif()

if (${NTF_BUILD_WITH_TIME_TRACE})
    message(STATUS "NTF: Building with compilation time trace:      yes")
else()
    message(STATUS "NTF: Building with compilation time trace:      no")
endif()

if (${NTF_BUILD_WITH_TIME_REPORT})
    message(STATUS "NTF: Building with compilation time report:     yes")
else()
    message(STATUS "NTF: Building with compilation time report:     no")
endif()

if (${NTF_BUILD_WITH_COVERAGE})
    message(STATUS "NTF: Building with coverage:                    yes")
else()
    message(STATUS "NTF: Building with coverage:                    no")
endif()
