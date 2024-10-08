// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <ntcs_processutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_processutil_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <dirent.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#if defined(BSLS_PLATFORM_OS_AIX)
#include <sys/procfs.h>
#elif defined(BSLS_PLATFORM_OS_DARWIN)
#include <libproc.h>
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/task.h>
#include <sys/sysctl.h>
#elif defined(BSLS_PLATFORM_OS_LINUX)
#include <dirent.h>
#include <sys/procfs.h>
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
#include <procfs.h>
#endif

#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <psapi.h>
#include <sys/timeb.h>
#pragma comment(lib, "pdh")
#pragma comment(lib, "psapi")
// clang-format on
#else
#error Not implemented
#endif

namespace BloombergLP {
namespace ntcs {

#if defined(BSLS_PLATFORM_OS_UNIX)

void ProcessUtil::getResourceUsage(ntcs::ProcessStatistics* result)
{
    int rc;

    result->reset();

    struct ::rusage rusage;
    rc = ::getrusage(RUSAGE_SELF, &rusage);
    if (rc == 0) {
        bsls::TimeInterval cpuTimeUser;
        cpuTimeUser.setInterval(
            static_cast<bsls::Types::Int64>(rusage.ru_utime.tv_sec),
            static_cast<int>(rusage.ru_utime.tv_usec * 1000));

        result->setCpuTimeUser(cpuTimeUser);

        bsls::TimeInterval cpuTimeSystem;
        cpuTimeSystem.setInterval(
            static_cast<bsls::Types::Int64>(rusage.ru_stime.tv_sec),
            static_cast<int>(rusage.ru_stime.tv_usec * 1000));

        result->setCpuTimeSystem(cpuTimeSystem);

        result->setContextSwitchesUser(
            static_cast<bsl::size_t>(rusage.ru_nvcsw));

        result->setContextSwitchesSystem(
            static_cast<bsl::size_t>(rusage.ru_nivcsw));

        result->setPageFaultsMajor(static_cast<bsl::size_t>(rusage.ru_majflt));

        result->setPageFaultsMinor(static_cast<bsl::size_t>(rusage.ru_minflt));
    }

#if defined(BSLS_PLATFORM_OS_AIX)

    result->setMemoryResident(static_cast<bsl::size_t>(rusage.ru_maxrss));

    char path[PATH_MAX];
    bsl::sprintf(path, "/proc/%d/status", static_cast<int>(::getpid()));

    int fd = ::open(path, O_RDONLY);
    if (fd != -1) {
        ::pstatus_t status;
        ::ssize_t   count = ::read(fd, &status, sizeof status);
        if (count == sizeof status) {
            result->setMemoryAddressSpace(
                static_cast<bsl::size_t>(status.pr_brksize));
        }

        ::close(fd);
    }

#elif defined(BSLS_PLATFORM_OS_DARWIN)

    proc_taskinfo taskInfo;
    bsl::memset(&taskInfo, 0, sizeof taskInfo);

    rc = ::proc_pidinfo(::getpid(),
                        PROC_PIDTASKINFO,
                        0,
                        &taskInfo,
                        PROC_PIDTASKINFO_SIZE);
    if (rc == PROC_PIDTASKINFO_SIZE) {
        if (taskInfo.pti_resident_size > 0) {
            result->setMemoryResident(
                static_cast<bsl::size_t>(taskInfo.pti_resident_size));
        }

        if (taskInfo.pti_virtual_size > 0) {
            result->setMemoryAddressSpace(
                static_cast<bsl::size_t>(taskInfo.pti_virtual_size));
        }
    }

#elif defined(BSLS_PLATFORM_OS_FREEBSD)

    result->setMemoryResident(static_cast<bsl::size_t>(rusage.ru_maxrss));

    // TODO

#elif defined(BSLS_PLATFORM_OS_LINUX)

    char path[PATH_MAX];
    bsl::sprintf(path, "/proc/%d/statm", static_cast<int>(::getpid()));

    FILE* file = bsl::fopen(path, "r");
    if (file != 0) {
        bsl::size_t size     = 0;
        bsl::size_t resident = 0;
        bsl::size_t shared   = 0;
        bsl::size_t trs      = 0;
        bsl::size_t lrs      = 0;
        bsl::size_t drs      = 0;
        bsl::size_t dr       = 0;

        rc = bsl::fscanf(file,
                         "%zu %zu %zu %zu %zu %zu %zu",
                         &size,
                         &resident,
                         &shared,
                         &trs,
                         &lrs,
                         &drs,
                         &dr);
        if (rc != 0) {
            const bsl::size_t pageSize =
                static_cast<bsl::size_t>(::sysconf(_SC_PAGESIZE));

            result->setMemoryResident(resident * pageSize);
            result->setMemoryAddressSpace(size * pageSize);
        }

        bsl::fclose(file);
    }

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

    result->setMemoryResident(static_cast<bsl::size_t>(rusage.ru_maxrss));

    char path[PATH_MAX];
    bsl::sprintf(path, "/proc/%d/status", static_cast<int>(::getpid()));

    int fd = ::open(path, O_RDONLY);
    if (fd != -1) {
        ::pstatus_t status;
        ::ssize_t   count = ::read(fd, &status, sizeof status);
        if (count == sizeof status) {
            result->setMemoryAddressSpace(
                static_cast<bsl::size_t>(status.pr_brksize));
        }

        ::close(fd);
    }

#else
#error Not implemented
#endif
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

void ProcessUtil::getResourceUsage(ntcs::ProcessStatistics* result)
{
    result->reset();

// TODO: Implement this function.
#if 0
    BOOL rc;

    PROCESS_MEMORY_COUNTERS info;
    bsl::memset(&info, 0, sizeof info);

    rc = GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    if (rc) {
        result->setPageFaultsMajor(
            static_cast<bsl::size_t>(info.PageFaultCount));

        result->setMemoryResident(
            static_cast<bsl::size_t>(info.WorkingSetSize));
    }

    MEMORYSTATUSEX memoryStatus;
    bsl::memset(&memoryStatus, 0, sizeof memoryStatus);

    rc = GlobalMemoryStatusEx(&memoryStatus);
    if (rc) {
        if (memoryStatus.ullTotalVirtual > memoryStatus.ullAvailVirtual) {
            result->setMemoryAddressSpace(
                static_cast<bsl::size_t>(memoryStatus.ullTotalVirtual -
                                         memoryStatus.ullAvailVirtual));
        }
    }
#endif
}

#else
#error Not implemented
#endif

}  // close package namespace
}  // close enterprise namespace
