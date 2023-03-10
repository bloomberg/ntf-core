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

#include <ntcs_memorymap.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_memorymap_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <ntsa_error.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsl_cstdlib.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/mman.h>
#include <unistd.h>
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#else
#error Not implemented
#endif

namespace BloombergLP {
namespace ntcs {

#if defined(BSLS_PLATFORM_OS_UNIX)

#define NTCS_MEMORY_MAP_PROTECTION (PROT_READ | PROT_WRITE)

#if defined(BSLS_PLATFORM_OS_DARWIN)
#define NTCS_MEMORY_MAP_FLAGS (MAP_ANON | MAP_PRIVATE)
#else
#define NTCS_MEMORY_MAP_FLAGS (MAP_ANONYMOUS | MAP_PRIVATE)
#endif

void* MemoryMap::acquire(bsl::size_t numPages)
{
    const bsl::size_t size = numPages * MemoryMap::pageSize();

    void* result = ::mmap(0,
                          size,
                          NTCS_MEMORY_MAP_PROTECTION,
                          NTCS_MEMORY_MAP_FLAGS,
                          -1,
                          0);

    if (result == MAP_FAILED) {
        bsl::abort();
    }

    return result;
}

void MemoryMap::release(void* address, bsl::size_t numPages)
{
    const bsl::size_t size = numPages * MemoryMap::pageSize();

    int rc = ::munmap(address, size);
    if (rc != 0) {
        bsl::abort();
    }
}

bsl::size_t MemoryMap::pageSize()
{
    return static_cast<bsl::size_t>(::sysconf(_SC_PAGESIZE));
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

void* MemoryMap::acquire(bsl::size_t numPages)
{
    const bsl::size_t size = numPages * MemoryMap::pageSize();

    void* result =
        VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (result == 0) {
        bsl::abort();
    }

    return result;
}

void MemoryMap::release(void* address, bsl::size_t numPages)
{
    VirtualFree(address, 0, MEM_RELEASE);
}

bsl::size_t MemoryMap::pageSize()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    return static_cast<bsl::size_t>(si.dwAllocationGranularity);
}

#else
#error Not implemented
#endif

}  // close package namespace
}  // close enterprise namespace
