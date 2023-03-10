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

#ifndef INCLUDED_NTCS_THREADUTIL
#define INCLUDED_NTCS_THREADUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide utilities for creating threads.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
struct ThreadUtil {
    /// Create a new thread having the specified 'attributes' that executes
    /// the specified 'function' with the specified 'context' argument.
    /// Load into the specified 'handle' the handle that identifies the
    /// thread. Return the error. Note that the thread name is *not*
    /// assigned to 'attributes.threadName()' by this function; this
    /// responsibility must be assumed by the 'function'.
    static ntsa::Error create(bslmt::ThreadUtil::Handle*     handle,
                              const bslmt::ThreadAttributes& attributes,
                              bslmt_ThreadFunction           function,
                              void*                          context);

    /// Block until the specified 'handle' has completed.
    static void join(bslmt::ThreadUtil::Handle handle);
};

/// @internal @brief
/// Describe the context in which a thread is run.
///
/// @par Thread Safety
/// This struct is not thread safe.
///
/// @ingroup module_ntcs
struct ThreadContext {
  private:
    ThreadContext(const ThreadContext&) BSLS_KEYWORD_DELETED;
    ThreadContext& operator=(const ThreadContext&) BSLS_KEYWORD_DELETED;

  public:
    void*             d_object_p;
    void*             d_driver_p;
    bslmt::Semaphore* d_semaphore_p;
    bsl::string       d_threadName;
    bsl::size_t       d_threadIndex;

    /// Create a new thread context. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit ThreadContext(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ThreadContext();
};

}  // close package namespace
}  // close enterprise namespace
#endif
