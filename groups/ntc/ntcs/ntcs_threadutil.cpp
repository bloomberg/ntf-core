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

#include <ntcs_threadutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_threadutil_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <pthread.h>
#include <signal.h>
#endif

namespace BloombergLP {
namespace ntcs {

ntsa::Error ThreadUtil::create(bslmt::ThreadUtil::Handle*     handle,
                               const bslmt::ThreadAttributes& attributes,
                               bslmt_ThreadFunction           function,
                               void*                          context)
{
    int rc;

    *handle = bslmt::ThreadUtil::invalidHandle();

#if defined(BSLS_PLATFORM_OS_UNIX)
    sigset_t newset, oldset;
    sigfillset(&newset);
    static const int synchronousSignals[] = {
        SIGBUS,
        SIGFPE,
        SIGILL,
        SIGSEGV,
        SIGSYS,
        SIGABRT,
        SIGTRAP,
#if !defined(BSLS_PLATFORM_OS_CYGWIN) || defined(SIGIOT)
        SIGIOT
#endif
    };
    static const int SIZE =
        sizeof synchronousSignals / sizeof *synchronousSignals;
    for (int i = 0; i < SIZE; ++i) {
        sigdelset(&newset, synchronousSignals[i]);
    }

    pthread_sigmask(SIG_BLOCK, &newset, &oldset);
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)

    rc = bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::create(
        handle,
        attributes,
        function,
        context);

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    rc = bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::create(
        handle,
        attributes,
        function,
        context);

#else
#error Not implemented
#endif

    ntsa::Error error = ntsa::Error::last();

#if defined(BSLS_PLATFORM_OS_UNIX)
    pthread_sigmask(SIG_SETMASK, &oldset, &newset);
#endif

    if (rc != 0) {
        return error;
    }

    BSLS_ASSERT_OPT(*handle != bslmt::ThreadUtil::Handle());
    BSLS_ASSERT_OPT(*handle != bslmt::ThreadUtil::invalidHandle());

    return ntsa::Error();
}

void ThreadUtil::join(bslmt::ThreadUtil::Handle handle)
{
    BSLS_ASSERT_OPT(handle != bslmt::ThreadUtil::Handle());
    BSLS_ASSERT_OPT(handle != bslmt::ThreadUtil::invalidHandle());

    void* threadStatus = 0;
    int   rc           = bslmt::ThreadUtil::join(handle, &threadStatus);
    BSLS_ASSERT_OPT(rc == 0);
    BSLS_ASSERT_OPT(threadStatus == 0);
}

ThreadContext::ThreadContext(bslma::Allocator* basicAllocator)
: d_object_p(0)
, d_driver_p(0)
, d_semaphore_p(0)
, d_threadName(basicAllocator)
, d_threadIndex(0)
{
}

ThreadContext::~ThreadContext()
{
}

}  // close package namespace
}  // close enterprise namespace
