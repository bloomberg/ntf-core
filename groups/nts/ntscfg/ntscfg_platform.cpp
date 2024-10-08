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

#include <ntscfg_platform.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntscfg_platform_cpp, "$Id$ $CSID$")

#include <bdls_filesystemutil.h>
#include <bslmt_once.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/version.h>
#endif

#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <sysinfoapi.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

// Set to 1 to automatically initialize the library during C++ static
// initialization.
#define NTSCFG_PLATFORM_AUTO_INITIALIZE 0

// Set to 1 to automatically ignore SIGPIPE (causing functions to return EPIPE
// instead of having SIGPIPE raised) during C++ static initialization.
#define NTSCFG_PLATFORM_AUTO_IGNORE_SIGPIPE 0

namespace BloombergLP {
namespace ntscfg {

namespace {

#if NTSCFG_PLATFORM_AUTO_INITIALIZE

// This struct defines a guard class to automatically initialize and
// cleanup the necessary library state and platform conditions.
struct Initializer {
    // Initialize the library state.
    Initializer();

    // Clean up the library state.
    ~Initializer();
};

// The static object to automatically initialize and clean up the library
// state.
Initializer s_initializer;

Initializer::Initializer()
{
    int rc = ntscfg::Platform::initialize();
    BSLS_ASSERT_OPT(rc == 0);
}

Initializer::~Initializer()
{
    int rc = ntscfg::Platform::exit();
    BSLS_ASSERT_OPT(rc == 0);
}

#endif

}  // close unnamed namespace

int Platform::initialize()
{
#if defined(BSLS_PLATFORM_OS_UNIX)

#if NTSCFG_PLATFORM_AUTO_IGNORE_SIGPIPE
    BSLMT_ONCE_DO
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
#endif
    return 0;

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    BSLMT_ONCE_DO
    {
        WSADATA wsa;
        ZeroMemory(&wsa, sizeof wsa);

        WORD version = MAKEWORD(2, 0);
        int  rc      = WSAStartup(version, &wsa);
        if (rc != 0) {
            return WSAGetLastError();
        }

        if (!((LOBYTE(wsa.wVersion) >= 2) && (HIBYTE(wsa.wVersion) >= 0))) {
            return WSAGetLastError();
        }
    }

    return 0;

#else
#error Not implemented
#endif
}

int Platform::ignore(Signal::Value signal)
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    if (signal == Signal::e_PIPE) {
        ::signal(SIGPIPE, SIG_IGN);
    }

    return 0;

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    return 0;

#else
#error Not implemented
#endif
}

int Platform::exit()
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    return 0;

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    BSLMT_ONCE_DO
    {
        WSACleanup();
    }
    return 0;

#else
#error Not implemented
#endif
}

bsl::size_t Platform::numCores()
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    errno              = 0;
    const int cpuCount = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpuCount < 0) {
        return 1;
    }

    return static_cast<bsl::size_t>(cpuCount);

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    return static_cast<bsl::size_t>(systemInfo.dwNumberOfProcessors);

#else
#error Not implemented
#endif
}

bool Platform::supportsNotifications()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    // The full support for MSG_ERRQUEUE for both TCP and UDP has only been
    // verified on versions greater than or equal to Linux 4.18.0.

    int rc;

    bool result = false;

    int major = 0;
    int minor = 0;
    int patch = 0;
    int build = 0;

    rc = ntsscm::Version::systemVersion(&major, &minor, &patch, &build);
    if (rc == 0) {
        if (KERNEL_VERSION(major, minor, patch) >= KERNEL_VERSION(4, 18, 0)) {
            result = true;
        }
    }

    return result;

#else

    return false;

#endif
}

#if defined(BSLS_PLATFORM_OS_UNIX)

bool Platform::hasHostDatabase()
{
    return bdls::FilesystemUtil::exists("/etc/hosts");
}

bool Platform::hasPortDatabase()
{
    return bdls::FilesystemUtil::exists("/etc/services");
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

bool Platform::hasHostDatabase()
{
    return bdls::FilesystemUtil::exists(
        "C:\\Windows\\System32\\drivers\\etc\\hosts");
}

bool Platform::hasPortDatabase()
{
    return bdls::FilesystemUtil::exists(
        "C:\\Windows\\System32\\drivers\\etc\\services");
}

#else
#error Not implemented
#endif

bsl::string Platform::buildBranch()
{
    bsl::string result = NTS_BUILD_BRANCH;
    if (result.empty()) {
        bsl::stringstream ss;
        ss << NTS_VERSION_MAJOR << '.' << NTS_VERSION_MINOR << '.'
           << NTS_VERSION_PATCH;
        result = ss.str();
    }

    return result;
}

bsl::string Platform::buildCommitHash()
{
    bsl::string result = NTS_BUILD_COMMIT_HASH;
    return result;
}

bsl::string Platform::buildCommitHashAbbrev()
{
    bsl::string result = NTS_BUILD_COMMIT_HASH_ABBREV;
    return result;
}

}  // close package namespace
}  // close enterprise namespace
