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

#ifndef INCLUDED_NTCDNS_RESOLVER
#define INCLUDED_NTCDNS_RESOLVER

#include <ntca_resolverconfig.h>
#include <ntcdns_cache.h>
#include <ntcdns_client.h>
#include <ntcdns_database.h>
#include <ntcdns_system.h>
#include <ntcdns_vocabulary.h>
#include <ntci_resolver.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntsb_resolver.h>
#include <bdlmt_threadpool.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bsls_keyword.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcdns {

/// @internal @brief
/// Provide a resolver using DNS.
///
/// @details
/// Provide a mechanism that implements the 'ntci::Resolver'
/// interface to asynchronously resolve domain names to address and service
/// names to ports (and the reverse) by directly sending messages to a
/// configured set of DNS servers.
//
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class Resolver : public ntci::Resolver, public ntccfg::Shared<Resolver>
{
    enum State { e_STATE_STARTED, e_STATE_STOPPING, e_STATE_STOPPED };

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                               d_object;
    mutable Mutex                                d_mutex;
    bsl::shared_ptr<ntci::Interface>             d_interface_sp;
    bool                                         d_interfaceOwned;
    bsl::shared_ptr<ntci::DatagramSocketFactory> d_datagramSocketFactory_sp;
    bsl::shared_ptr<ntci::ListenerSocketFactory> d_listenerSocketFactory_sp;
    bsl::shared_ptr<ntci::StreamSocketFactory>   d_streamSocketFactory_sp;
    bsl::shared_ptr<ntci::TimerFactory>          d_timerFactory_sp;
    bsl::shared_ptr<ntci::StrandFactory>         d_strandFactory_sp;
    bsl::shared_ptr<ntci::Executor>              d_executor_sp;
    bsl::shared_ptr<ntci::Strand>                d_strand_sp;
    bsl::shared_ptr<ntsb::ResolverOverrides>     d_overrides_sp;
    bsl::shared_ptr<ntcdns::HostDatabase>        d_hostDatabase_sp;
    bsl::shared_ptr<ntcdns::PortDatabase>        d_portDatabase_sp;
    bsl::shared_ptr<ntcdns::Cache>               d_cache_sp;
    bsl::shared_ptr<ntcdns::Client>              d_client_sp;
    bsl::shared_ptr<ntcdns::System>              d_system_sp;
    bsl::shared_ptr<bdlmt::ThreadPool>           d_threadPool_sp;
    bsls::AtomicInt                              d_state;
    bool                                         d_initialized;
    ntca::ResolverConfig                         d_config;
    bslma::Allocator*                            d_allocator_p;

  private:
    Resolver(const Resolver&) BSLS_KEYWORD_DELETED;
    Resolver& operator=(const Resolver&) BSLS_KEYWORD_DELETED;

  private:
    /// Initialize the host database, port database, cache, client, and
    /// system, according to whether each are enabled. Return the error.
    ntsa::Error initialize();

  public:
    enum {
        k_UDP_MAX_PAYLOAD_SIZE = 65527,
        // The maximum UDP payload size.

        k_DNS_MAX_PAYLOAD_SIZE = 512,
        // The maximum DNS payload size.

        k_DNS_PORT = 53
        // The default DNS port.
    };

    /// Create a new resolver having the specified 'configuration' that
    /// is incapable of running DNS clients or servers but still capable
    /// of performing asynchronous resolutions using separate thread pool
    /// managed by this object to call the blocking operating system
    /// interfaces directly. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Resolver(const ntca::ResolverConfig& configuration,
                      bslma::Allocator*           basicAllocator = 0);

    /// Create a new resolver having the specified 'configuration' using
    /// the specified 'interface' that becomes 'owned' by this object.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Resolver(const ntca::ResolverConfig&             configuration,
                      const bsl::shared_ptr<ntci::Interface>& interface,
                      bool                                    owned,
                      bslma::Allocator* basicAllocator = 0);

    /// Create a new resolver having the specified 'configuration' whose
    /// client uses sockets created by the specified
    /// 'datagramSocketFactory' and 'streamSocketFactory' and whose server
    /// uses the specified 'listenerSocketFactory'. Create timers using the
    /// specified 'timerFactory'. Create strands using the specified
    /// 'strandFactory'.  Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Resolver(
        const ntca::ResolverConfig& configuration,
        const bsl::shared_ptr<ntci::DatagramSocketFactory>&
            datagramSocketFactory,
        const bsl::shared_ptr<ntci::ListenerSocketFactory>&
            listenerSocketFactory,
        const bsl::shared_ptr<ntci::StreamSocketFactory>& streamSocketFactory,
        const bsl::shared_ptr<ntci::TimerFactory>&        timerFactory,
        const bsl::shared_ptr<ntci::StrandFactory>&       strandFactory,
        const bsl::shared_ptr<ntci::Executor>&            executor,
        bslma::Allocator*                                 basicAllocator = 0);

    /// Destroy this object.
    ~Resolver() BSLS_KEYWORD_OVERRIDE;

    /// Start the resolver. Return the error.
    ntsa::Error start() BSLS_KEYWORD_OVERRIDE;

    /// Begin stopping the resolver.
    void shutdown() BSLS_KEYWORD_OVERRIDE;

    /// Wait for the resolver to stop.
    void linger() BSLS_KEYWORD_OVERRIDE;

    /// Set the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error setIpAddress(const bslstl::StringRef&            domainName,
                             const bsl::vector<ntsa::IpAddress>& ipAddressList)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddressList', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error addIpAddress(const bslstl::StringRef&            domainName,
                             const bsl::vector<ntsa::IpAddress>& ipAddressList)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'domainName' to resolve to the specified
    /// 'ipAddress', and vice-versa, in addition to any previously,
    /// explicitly defined associations. Return the error. Note that calling
    /// this function affects the future behavior of this object only:
    /// 'ipAddress' will be subsequently returned from calling
    /// 'getIpAddress()' of 'domainName', and 'domainName' will be returned
    /// from calling 'getDomainName()' of any of the addresses in
    /// 'ipAddressList', but this function does not affect any name
    /// resolution caches maintained elsewhere by the system.
    ntsa::Error addIpAddress(const bslstl::StringRef& domainName,
                             const ntsa::IpAddress&   ipAddress)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error setPort(const bslstl::StringRef&       serviceName,
                        const bsl::vector<ntsa::Port>& portList,
                        ntsa::Transport::Value         transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'portList' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error addPort(const bslstl::StringRef&       serviceName,
                        const bsl::vector<ntsa::Port>& portList,
                        ntsa::Transport::Value         transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'serviceName' to resolve to the specified
    /// 'port' for use by the specified 'transport', and vice-versa,
    /// in addition to any previously, explicitly defined associations.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'port' will be subsequently returned
    /// from calling 'getPort()' of 'serviceName', and 'serviceName' will be
    /// returned from calling 'getServiceName()' of any of the ports in
    /// 'portList', but this function does not affect any name resolution
    /// caches maintained elsewhere by the system.
    ntsa::Error addPort(const bslstl::StringRef& serviceName,
                        ntsa::Port               port,
                        ntsa::Transport::Value   transport)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the local IP addresses assigned to the local machine to the
    /// specified 'ipAddressList'. Return the error. Note that calling this
    /// function affects the future behavior of this object only:
    /// 'ipAddressList' will be subsequently returned from calling
    /// 'getLocalIpAddress()' but this function does not set the local IP
    /// addresses of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setLocalIpAddress(const bsl::vector<ntsa::IpAddress>&
                                      ipAddressList) BSLS_KEYWORD_OVERRIDE;

    /// Set the hostname of the local machine to the specified 'name'.
    /// Return the error. Note that calling this function affects the future
    /// behavior of this object only: 'name' will be subsequently returned
    /// from calling 'getHostname()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setHostname(const bsl::string& name) BSLS_KEYWORD_OVERRIDE;

    /// Set the canonical, fully-qualified hostname of the local machine to
    /// the specified 'name'. Return the error. Note that calling this
    /// function affects the future behavior of this object only: 'name'
    /// will be subsequently returned from calling
    /// 'getHostnameFullyQualified()' but this function does not set the
    /// hostname of the system or have any wider effect on other objects or
    /// name resolution functionality in this process.
    ntsa::Error setHostnameFullyQualified(const bsl::string& name)
        BSLS_KEYWORD_OVERRIDE;

    /// Resolve the specified 'domainName' to the IP addresses assigned to
    /// the 'domainName', according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the IP addresses assigned to
    /// the 'domainName'. Return the error.
    ntsa::Error getIpAddress(const bslstl::StringRef&          domainName,
                             const ntca::GetIpAddressOptions&  options,
                             const ntci::GetIpAddressCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Resolve the specified 'ipAddress' to the domain name to which the
    /// 'ipAddress' has been assigned, according to the specified 'options'.
    /// When resolution completes or fails, invoke the specified 'callback'
    /// on the callback's strand, if any, with the domain name to which the
    /// 'ipAddress' has been assigned. Return the error.
    ntsa::Error getDomainName(const ntsa::IpAddress&             ipAddress,
                              const ntca::GetDomainNameOptions&  options,
                              const ntci::GetDomainNameCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Resolve the specified 'serviceName' to the ports assigned to the
    /// 'serviceName', according to the specified 'options'. When resolution
    /// completes or fails, invoke the specified 'callback' on the
    /// callback's strand, if any, with the ports assigned to the
    /// 'serviceName'. Return the error.
    ntsa::Error getPort(const bslstl::StringRef&     serviceName,
                        const ntca::GetPortOptions&  options,
                        const ntci::GetPortCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Resolve the specified 'port' to the service name to which the 'port'
    /// has been assigned, according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the service name to which the
    /// 'port' has been assigned. Return the error.
    ntsa::Error getServiceName(ntsa::Port                          port,
                               const ntca::GetServiceNameOptions&  options,
                               const ntci::GetServiceNameCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Parse and potentially resolve the components of the specified
    /// 'text', in the format of '<port>' or '[<host>][:<port>]'. If the
    /// optionally specified '<host>' component is not an IP address,
    /// interpret the '<host>' as a domain name and resolve it into an IP
    /// address. If the optionally specified '<port>' is a name and not a
    /// number, interpret the '<port>' as a service name and resolve it into
    /// a port. Perform all resolution and validation of the characteristics
    /// of the desired 'result' according to the specified 'options'. When
    /// resolution completes or fails, invoke the specified 'callback' on
    /// the callback's strand, if any, with the endpoint to which the
    /// components of the 'text' resolve. Return the error.
    ntsa::Error getEndpoint(const bslstl::StringRef&         text,
                            const ntca::GetEndpointOptions&  options,
                            const ntci::GetEndpointCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the IP addresses assigned to the
    /// local machine. Perform all resolution and validation of the
    /// characteristics of the desired 'result' according to the specified
    /// 'options'. Return the error.
    ntsa::Error getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                  const ntsa::IpAddressOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Return the hostname of the local machine.
    ntsa::Error getHostname(bsl::string* result) BSLS_KEYWORD_OVERRIDE;

    /// Return the canonical, fully-qualified hostname of the local machine.
    ntsa::Error getHostnameFullyQualified(bsl::string* result)
        BSLS_KEYWORD_OVERRIDE;

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

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load the DNS host database as defined by the specified 'data' having
    /// the specified 'size'. Return the error.
    ntsa::Error loadHostDatabaseText(const char* data, bsl::size_t size);

    /// Load the DNS port database as defined by the specified 'data' having
    /// the specified 'size'. Return the error.
    ntsa::Error loadPortDatabaseText(const char* data, bsl::size_t size);

    /// Insert or update the host entry for the specified 'domainName' to be
    /// associated with the specified 'ipAddress' starting from the
    /// specified 'now' for the specified 'timeToLive'. Return the error.
    ntsa::Error cacheHost(const bsl::string&        domainName,
                          const ntsa::IpAddress&    ipAddress,
                          const ntsa::Endpoint&     nameServer,
                          bsl::size_t               timeToLive,
                          const bsls::TimeInterval& now);

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
