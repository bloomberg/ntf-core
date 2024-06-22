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

#ifndef INCLUDED_NTCDNS_SYSTEM
#define INCLUDED_NTCDNS_SYSTEM

#include <ntccfg_platform.h>
#include <ntcdns_vocabulary.h>
#include <ntcscm_version.h>

#include <ntca_getdomainnamecontext.h>
#include <ntca_getdomainnameoptions.h>
#include <ntca_getipaddresscontext.h>
#include <ntca_getipaddressoptions.h>
#include <ntca_getportcontext.h>
#include <ntca_getportoptions.h>
#include <ntca_getservicenamecontext.h>
#include <ntca_getservicenameoptions.h>

#include <ntci_executor.h>
#include <ntci_getdomainnamecallback.h>
#include <ntci_getipaddresscallback.h>
#include <ntci_getportcallback.h>
#include <ntci_getservicenamecallback.h>
#include <ntci_resolver.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>

#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_port.h>

#include <bdlmt_threadpool.h>

#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bsls_atomic.h>
#include <bsls_timeinterval.h>

#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcdns {

/// @internal @brief
/// Provide a asynchronous resolver using blocking operating system APIs.
///
/// @details
/// Provide a mechanism that performs name resolution
/// asynchronously on a "hidden", internally-managed thread pool using blocking
/// operating system APIs (e.g. the POSIX 'gethostbyname' function).
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class System : public ntci::Executor,
               public ntci::StrandFactory,
               public ntccfg::Shared<System>
{
    enum State { e_STATE_STARTED, e_STATE_STOPPING, e_STATE_STOPPED };

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                              d_mutex;
    bsl::shared_ptr<bdlmt::ThreadPool> d_threadPool_sp;
    int                                d_minThreads;
    int                                d_maxThreads;
    int                                d_maxIdleTime;
    bsls::AtomicInt                    d_state;
    bslma::Allocator*                  d_allocator_p;

  private:
    System(const System&) BSLS_KEYWORD_DELETED;
    System& operator=(const System&) BSLS_KEYWORD_DELETED;

  private:
    /// Create the thread pool. Return the error.
    ntsa::Error initialize();

    /// Resolve the specified 'domainName' to the IP addresses assigned to
    /// the 'domainName', according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the IP addresses assigned to
    /// the 'domainName'.
    void executeGetIpAddress(const bsl::shared_ptr<ntci::Resolver>& resolver,
                             const bsl::string&                     domainName,
                             const bsls::TimeInterval&              startTime,
                             const ntca::GetIpAddressOptions&       options,
                             const ntci::GetIpAddressCallback&      callback);

    /// Resolve the specified 'ipAddress' to the domain name to which the
    /// 'ipAddress' has been assigned, according to the specified 'options'.
    /// When resolution completes or fails, invoke the specified 'callback'
    /// on the callback's strand, if any, with the domain name to which the
    /// 'ipAddress' has been assigned.
    void executeGetDomainName(const bsl::shared_ptr<ntci::Resolver>& resolver,
                              const ntsa::IpAddress&                 ipAddress,
                              const bsls::TimeInterval&              startTime,
                              const ntca::GetDomainNameOptions&      options,
                              const ntci::GetDomainNameCallback&     callback);

    /// Resolve the specified 'serviceName' to the ports assigned to the
    /// 'serviceName', according to the specified 'options'. When resolution
    /// completes or fails, invoke the specified 'callback' on the
    /// callback's strand, if any, with the ports assigned to the
    /// 'serviceName'.
    void executeGetPort(const bsl::shared_ptr<ntci::Resolver>& resolver,
                        const bsl::string&                     serviceName,
                        const bsls::TimeInterval&              startTime,
                        const ntca::GetPortOptions&            options,
                        const ntci::GetPortCallback&           callback);

    /// Resolve the specified 'port' to the service name to which the 'port'
    /// has been assigned, according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the service name to which the
    /// 'port' has been assigned.
    void executeGetServiceName(const bsl::shared_ptr<ntci::Resolver>& resolver,
                               ntsa::Port                             port,
                               const bsls::TimeInterval&           startTime,
                               const ntca::GetServiceNameOptions&  options,
                               const ntci::GetServiceNameCallback& callback);

  public:
    /// Create a new asynchronous system resolver backed by a thread pool
    /// having between zero and one threads, joining the thread after ten
    /// seconds if no operations are pending. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit System(bslma::Allocator* basicAllocator = 0);

    /// Create a new asynchronous system resolver backed by a thread pool
    /// having between the specified 'minThreads' and 'maxThreads', joining
    /// each thread after the specified 'maxIdleTime', in seconds, if no
    /// operations are pending. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit System(int               minThreads,
                    int               maxThreads,
                    int               maxIdleTime,
                    bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~System() BSLS_KEYWORD_OVERRIDE;

    /// Start the object.
    ntsa::Error start();

    /// Begin stopping the object.
    void shutdown();

    /// Wait for the object to stop.
    void linger();

    /// Resolve the specified 'domainName' to the IP addresses assigned to
    /// the 'domainName', according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the IP addresses assigned to
    /// the 'domainName'. Return the error.
    ntsa::Error getIpAddress(const bsl::shared_ptr<ntci::Resolver>& resolver,
                             const bsl::string&                     domainName,
                             const bsls::TimeInterval&              startTime,
                             const ntca::GetIpAddressOptions&       options,
                             const ntci::GetIpAddressCallback&      callback);

    /// Resolve the specified 'ipAddress' to the domain name to which the
    /// 'ipAddress' has been assigned, according to the specified 'options'.
    /// When resolution completes or fails, invoke the specified 'callback'
    /// on the callback's strand, if any, with the domain name to which the
    /// 'ipAddress' has been assigned. Return the error.
    ntsa::Error getDomainName(const bsl::shared_ptr<ntci::Resolver>& resolver,
                              const ntsa::IpAddress&                 ipAddress,
                              const bsls::TimeInterval&              startTime,
                              const ntca::GetDomainNameOptions&      options,
                              const ntci::GetDomainNameCallback&     callback);

    /// Resolve the specified 'serviceName' to the ports assigned to the
    /// 'serviceName', according to the specified 'options'. When resolution
    /// completes or fails, invoke the specified 'callback' on the
    /// callback's strand, if any, with the ports assigned to the
    /// 'serviceName'. Return the error.
    ntsa::Error getPort(const bsl::shared_ptr<ntci::Resolver>& resolver,
                        const bsl::string&                     serviceName,
                        const bsls::TimeInterval&              startTime,
                        const ntca::GetPortOptions&            options,
                        const ntci::GetPortCallback&           callback);

    /// Resolve the specified 'port' to the service name to which the 'port'
    /// has been assigned, according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the service name to which the
    /// 'port' has been assigned. Return the error.
    ntsa::Error getServiceName(const bsl::shared_ptr<ntci::Resolver>& resolver,
                               ntsa::Port                             port,
                               const bsls::TimeInterval&           startTime,
                               const ntca::GetServiceNameOptions&  options,
                               const ntci::GetServiceNameCallback& callback);

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Create a new strand to serialize execution of functors. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
