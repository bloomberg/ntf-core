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

#include <ntsa_buffer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_buffer_cpp, "$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsl_cstdlib.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <winsock2.h>
#endif

// #define NTSA_BUFFER_OFFSETOF(Type, member)
//     ((bsl::size_t)((char *)(&((Type*)(0))->member) - (char*)(0)))

#if defined(BSLS_PLATFORM_CMP_IBM)
#define NTSA_BUFFER_OFFSETOF(Type, member) offsetof(Type, member)
#else
#define NTSA_BUFFER_OFFSETOF(Type, member) offsetof(Type, member)
#endif

namespace BloombergLP {
namespace ntsa {

bool MutableBuffer::validate() const
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    struct ::iovec systemObject;

    BSLMF_ASSERT(sizeof(d_data) == sizeof(systemObject.iov_base));
    BSLMF_ASSERT(sizeof(d_size) == sizeof(systemObject.iov_len));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(MutableBuffer, d_data) ==
                 NTSA_BUFFER_OFFSETOF(struct ::iovec, iov_base));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(MutableBuffer, d_size) ==
                 NTSA_BUFFER_OFFSETOF(struct ::iovec, iov_len));

    BSLMF_ASSERT(sizeof(MutableBuffer) == sizeof(struct ::iovec));

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    WSABUF systemObject;

    BSLMF_ASSERT(sizeof(d_data) == sizeof(systemObject.buf));
    BSLMF_ASSERT(sizeof(d_size) == sizeof(systemObject.len));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(MutableBuffer, d_data) ==
                 NTSA_BUFFER_OFFSETOF(WSABUF, buf));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(MutableBuffer, d_size) ==
                 NTSA_BUFFER_OFFSETOF(WSABUF, len));

    BSLMF_ASSERT(sizeof(MutableBuffer) == sizeof(WSABUF));

#else
#error Not implemented
#endif

    return true;
}

bool ConstBuffer::validate() const
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    struct ::iovec systemObject;

    BSLMF_ASSERT(sizeof(d_data) == sizeof(systemObject.iov_base));
    BSLMF_ASSERT(sizeof(d_size) == sizeof(systemObject.iov_len));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(ConstBuffer, d_data) ==
                 NTSA_BUFFER_OFFSETOF(struct ::iovec, iov_base));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(ConstBuffer, d_size) ==
                 NTSA_BUFFER_OFFSETOF(struct ::iovec, iov_len));

    BSLMF_ASSERT(sizeof(ConstBuffer) == sizeof(struct ::iovec));

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    WSABUF systemObject;

    BSLMF_ASSERT(sizeof(d_data) == sizeof(systemObject.buf));
    BSLMF_ASSERT(sizeof(d_size) == sizeof(systemObject.len));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(ConstBuffer, d_data) ==
                 NTSA_BUFFER_OFFSETOF(WSABUF, buf));

    BSLMF_ASSERT(NTSA_BUFFER_OFFSETOF(ConstBuffer, d_size) ==
                 NTSA_BUFFER_OFFSETOF(WSABUF, len));

    BSLMF_ASSERT(sizeof(ConstBuffer) == sizeof(WSABUF));

#else
#error Not implemented
#endif

    return true;
}

}  // close package namespace
}  // close enterprise namespace
