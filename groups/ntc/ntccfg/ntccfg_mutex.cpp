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

#include <ntccfg_mutex.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntccfg_mutex_cpp, "$Id$ $CSID$")

#if NTCCFG_FUTEX_ENABLED

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <errno.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
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
// clang-format on
#pragma comment(lib, "synchronization")
#endif

#endif // NTCCFG_FUTEX_ENABLED

namespace BloombergLP {
namespace ntccfg {

#if NTCCFG_FUTEX_ENABLED

#if defined(BSLS_PLATFORM_OS_LINUX)

// Some versions of GCC issue a spurious warning that the 'current' parameter
// is set but not used when 'Futex::compareAndSwap' is called.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#endif

NTCCFG_INLINE_NEVER
void Futex::wait()
{
    syscall(SYS_futex, (int*)(&d_value), FUTEX_WAIT, 2, 0, 0, 0);
}

NTCCFG_INLINE_NEVER
void Futex::wake()
{
    syscall(SYS_futex, (int*)(&d_value), FUTEX_WAKE, 1, 0, 0, 0);
}

NTCCFG_INLINE_NEVER
void Futex::lockContention(int c)
{
    do {
        if (c == 2 || compareAndSwap(&d_value, 1, 2) != 0) {
            this->wait();
        }
    } while ((c = compareAndSwap(&d_value, 0, 2)) != 0);
}

NTCCFG_INLINE_NEVER
void Futex::unlockContention()
{
    __atomic_store_n(&d_value, 0, __ATOMIC_SEQ_CST);
    this->wake();
}

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic pop
#endif

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

NTCCFG_INLINE_NEVER
void Futex::wait()
{
    Value compare = 2;
    WaitOnAddress(&d_value, &compare, sizeof compare, INFINITE);
}

NTCCFG_INLINE_NEVER
void Futex::wake()
{
    WakeByAddressSingle((Value*)(&d_value));
}

NTCCFG_INLINE_NEVER
void Futex::lockContention(Value c)
{
    while (true) {
        if (c == 2) {
            this->wait();
        }
        else {
            const Value previous = compareAndSwap(&d_value, 1, 2);
            if (previous != 0) {
                this->wait();
            }
        }

        c = compareAndSwap(&d_value, 0, 2);
        if (c == 0) {
            break;
        }
    }
}

NTCCFG_INLINE_NEVER
void Futex::unlockContention()
{
    _InterlockedExchange(&d_value, 0);
    this->wake();
}

#else
#error Not implemented
#endif

#endif  // NTCCFG_FUTEX_ENABLED

}  // close package namespace
}  // close enterprise namespace
