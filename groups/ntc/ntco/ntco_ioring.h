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

#ifndef INCLUDED_NTCO_IORING
#define INCLUDED_NTCO_IORING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_proactorconfig.h>
#include <ntccfg_platform.h>
#include <ntci_proactor.h>
#include <ntci_proactorfactory.h>
#include <ntci_user.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

#if NTC_BUILD_WITH_IORING
#if defined(BSLS_PLATFORM_OS_LINUX)

namespace BloombergLP {
namespace ntco {

/// @internal @brief
/// Provide a testing mechanism for the 'io_uring' API.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntco
class IoRingTest
{
  public:
    /// Destroy this object.
    virtual ~IoRingTest();

    /// Push a unit of work identified by the specified 'id' onto the 
    /// submission queue and immediately submit it. Return the error. 
    virtual ntsa::Error post(bsl::uint64_t id) = 0;

    /// Push a unit of work identified by the specified 'id' onto the 
    /// submission queue and but do not submit it until the next call to 
    /// 'wait'. Return the error. 
    virtual ntsa::Error defer(bsl::uint64_t id) = 0;

    /// Block until at least the specified 'minimumToComplete' number of units
    /// of work have completed. Load into the specified 'result' the vector of
    /// identifiers of units of work completed.
    virtual void wait(bsl::vector<bsl::uint64_t> *result, 
                      bsl::size_t                 minimumToComplete) = 0;

    // Return the index of the head entry in the submission queue.
    virtual bsl::uint32_t submissionQueueHead() const = 0;

    // Return the index of the tail entry in the submission queue.
    virtual bsl::uint32_t submissionQueueTail() const = 0;

    // Return the maximum number of entries in the submission queue.
    virtual bsl::uint32_t submissionQueueCapacity() const = 0;

    // Return the index of the head entry in the completion queue.
    virtual bsl::uint32_t completionQueueHead() const = 0;

    // Return the index of the tail entry in the completion queue.
    virtual bsl::uint32_t completionQueueTail() const = 0;

    // Return the maximum number of entries in the completion queue.
    virtual bsl::uint32_t completionQueueCapacity() const = 0;
};

/// @internal @brief
/// Provide a factory to produce proactors implemented using the 'io_uring'
/// API.
///
/// @details
/// This class implements the 'ntci::ProactorFactory' interface to produce
/// proactors implemented using the 'io_uring' API.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntco
class IoRingFactory : public ntci::ProactorFactory
{
    bslma::Allocator* d_allocator_p;

  private:
    IoRingFactory(const IoRingFactory&) BSLS_KEYWORD_DELETED;
    IoRingFactory& operator=(const IoRingFactory&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new proactor factory that produces proactors implemented using
    /// the 'io_uring' API. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit IoRingFactory(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~IoRingFactory() BSLS_KEYWORD_OVERRIDE;

    /// Create a new proactor with the specified 'configuration' operating in
    /// the environment of the specified 'user'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used. Return the error.
    bsl::shared_ptr<ntci::Proactor> createProactor(
        const ntca::ProactorConfig&        configuration,
        const bsl::shared_ptr<ntci::User>& user,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new test. Optionally specify a 'basicAllocator' used to 
    /// supply memory. If 'basicAllocator' is 0, the currently installed 
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntco::IoRingTest> createTest(
        bslma::Allocator* basicAllocator = 0);

    // Return true if the runtime properties of the current operating system
    // support proactors produced by this factory, otherwise return false.
    static bool isSupported();
};

}  // close package namespace
}  // close enterprise namespace

#endif
#endif

#endif
