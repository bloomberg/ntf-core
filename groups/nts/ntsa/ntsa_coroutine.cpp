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

#include <ntsa_coroutine.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_coroutine_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_new.h>

#if NTS_BUILD_WITH_COROUTINES

namespace BloombergLP {
namespace ntsa {

void* CoroutineUtil::allocate(bsl::size_t size, const Allocator& allocator)
{
    NTSA_COROUTINE_LOG_CONTEXT();

    constexpr bsl::size_t maxAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

    const bsl::size_t frameRoundedSize =
        maxAlignment * ((size + maxAlignment - 1) / maxAlignment);

    size = frameRoundedSize + sizeof(Allocator);

    void* buf = allocator.mechanism()->allocate(size);

    NTSA_COROUTINE_LOG_ALLOCATE(buf, size);

    char* allocStart = static_cast<char*>(buf) + frameRoundedSize;
    ::new (static_cast<void*>(allocStart)) Allocator(allocator);
    return buf;
}

void CoroutineUtil::deallocate(void* ptr, bsl::size_t size)
{
    NTSA_COROUTINE_LOG_CONTEXT();

    constexpr bsl::size_t maxAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

    const bsl::size_t frameRoundedSize =
        maxAlignment * ((size + maxAlignment - 1) / maxAlignment);

    size = frameRoundedSize + sizeof(Allocator);

    NTSA_COROUTINE_LOG_FREE(ptr, size);

    char* allocStart = static_cast<char*>(ptr) + frameRoundedSize;

    const Allocator allocator = *reinterpret_cast<Allocator*>(allocStart);

    allocator.mechanism()->deallocate(ptr);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // NTS_BUILD_WITH_COROUTINES
