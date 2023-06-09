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

#include <bslmt_once.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <linux/version.h>
#endif

#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
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

bool s_supportsTimestamps = false;

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

bool Platform::supportsTimestamps()
{
#if defined(BSLS_PLATFORM_OS_LINUX)
    BSLMT_ONCE_DO
    {
        int major = 0;
        int minor = 0;
        int patch = 0;
        int build = 0;
        int rc =
            ntsscm::Version::systemVersion(&major, &minor, &patch, &build);
        if (rc == 0) {
            if (KERNEL_VERSION(major, minor, patch) >=
                KERNEL_VERSION(4, 18, 0))
            {
                s_supportsTimestamps = true;
            }
        }
    }
#endif
    return s_supportsTimestamps;
}

}  // close package namespace
}  // close enterprise namespace
