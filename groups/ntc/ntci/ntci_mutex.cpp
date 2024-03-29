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

#include <ntci_mutex.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_mutex_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <errno.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

namespace BloombergLP {
namespace ntci {

#if NTCI_MUTEX_SYSTEM

// Some versions of GCC issue a spurious warning that the 'current' paramter
// is set but not used when 'Mutex::compareAndSwap' is called.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#endif

__attribute__((noinline)) void Mutex::wait()
{
    syscall(SYS_futex, (int*)(&d_value), FUTEX_WAIT, 2, 0, 0, 0);
}

__attribute__((noinline)) void Mutex::wake()
{
    syscall(SYS_futex, (int*)(&d_value), FUTEX_WAKE, 1, 0, 0, 0);
}

__attribute__((noinline)) void Mutex::lockContention(int c)
{
    do {
        if (c == 2 || compareAndSwap(&d_value, 1, 2) != 0) {
            this->wait();
        }
    } while ((c = compareAndSwap(&d_value, 0, 2)) != 0);
}

__attribute__((noinline)) void Mutex::unlockContention()
{
    __atomic_store_n(&d_value, 0, __ATOMIC_SEQ_CST);
    this->wake();
}

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic pop
#endif

#endif

}  // close package namespace
}  // close enterprise namespace
