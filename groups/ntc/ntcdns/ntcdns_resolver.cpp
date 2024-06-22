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

#include <ntcdns_resolver.h>

#include <ntcdns_compat.h>
#include <ntcdns_utility.h>
#include <ntci_log.h>

#include <ntsa_endpointoptions.h>
#include <ntsa_ipaddressoptions.h>
#include <ntsa_portoptions.h>
#include <ntsu_resolverutil.h>

#include <bdlb_chartype.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlt_currenttime.h>
#include <bslmf_assert.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_byteorderutil.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>

// #define NTCDNS_RESOLVER_LOG_VERBOSE 0

namespace BloombergLP {
namespace ntcdns {

namespace {

const bool k_DEFAULT_HOST_DATABASE_ENABLED = false;
const bool k_DEFAULT_PORT_DATABASE_ENABLED = false;

const bool k_DEFAULT_POSITIVE_CACHE_ENABLED = false;
const bool k_DEFAULT_NEGATIVE_CACHE_ENABLED = false;

const bool k_DEFAULT_CLIENT_ENABLED = false;

const int k_DEFAULT_SYSTEM_MIN_THREADS   = 0;
const int k_DEFAULT_SYSTEM_MAX_THREADS   = 1;
const int k_DEFAULT_SYSTEM_MAX_IDLE_TIME = 10;

void processGetIpAddressResult(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::vector<ntsa::IpAddress>&    ipAddressList,
    const bsls::TimeInterval&              startTime,
    const bsl::string&                     serviceName,
    ntsa::Port                             port,
    const ntca::GetIpAddressEvent&         event,
    const ntci::GetEndpointCallback&       callback)
{
#if NTCDNS_RESOLVER_LOG_VERBOSE
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing get IP address event " << event
                          << NTCI_LOG_STREAM_END;
#endif

    ntsa::Endpoint           endpoint;
    ntca::GetEndpointContext getEndpointContext;
    ntca::GetEndpointEvent   getEndpointEvent;

    bsl::string authority;
    authority.assign(event.context().domainName());
    authority.append(1, ':');
    if (!serviceName.empty()) {
        authority.append(serviceName);
    }
    else {
        authority.append(bsl::to_string(port));
    }

    getEndpointContext.setAuthority(authority);
    getEndpointContext.setSource(event.context().source());

    bsls::TimeInterval endTime = bdlt::CurrentTime::now();
    if (endTime > startTime) {
        getEndpointContext.setLatency(endTime - startTime);
    }

    if (!event.context().nameServer().isNull()) {
        getEndpointContext.setNameServer(event.context().nameServer().value());
    }

    if (!event.context().timeToLive().isNull()) {
        getEndpointContext.setTimeToLive(event.context().timeToLive().value());
    }

    if (event.type() == ntca::GetIpAddressEventType::e_COMPLETE) {
        if (ipAddressList.size() > 0) {
#if NTCDNS_RESOLVER_LOG_VERBOSE
            for (bsl::size_t i = 0; i < ipAddressList.size(); ++i) {
                const ntsa::IpAddress& ipAddress = ipAddressList[i];
                NTCI_LOG_STREAM_INFO << "The domain name '"
                                     << event.context().domainName()
                                     << "' has resolved to " << ipAddress
                                     << NTCI_LOG_STREAM_END;
            }
#endif

            getEndpointEvent.setType(ntca::GetEndpointEventType::e_COMPLETE);
            endpoint =
                ntsa::Endpoint(ntsa::IpEndpoint(ipAddressList.front(), port));
        }
        else {
#if NTCDNS_RESOLVER_LOG_VERBOSE
            NTCI_LOG_STREAM_INFO
                << "The domain name '" << event.context().domainName()
                << "' has no IP addresses assigned" << NTCI_LOG_STREAM_END;
#endif

            getEndpointEvent.setType(ntca::GetEndpointEventType::e_ERROR);
            getEndpointContext.setError(ntsa::Error(ntsa::Error::e_EOF));
        }
    }
    else {
        getEndpointEvent.setType(ntca::GetEndpointEventType::e_ERROR);
        getEndpointContext.setError(event.context().error());
    }

    getEndpointEvent.setContext(getEndpointContext);

    callback(resolver, endpoint, getEndpointEvent, ntci::Strand::unknown());
}

}  // close unnamed namespace

ntsa::Error Resolver::initialize()
{
    // Avoid redundant initialization.

    if (d_initialized) {
        return ntsa::Error();
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    // Start the interface, if owned.

    if (d_interface_sp && d_interfaceOwned) {
        d_interface_sp->start();
    }

    // Create the strand, if callbacks created by this object should, by
    // default, have strand semantics.

    // MRM: d_strand_sp = d_strandFactory_sp->createStrand(d_allocator_p);

    // Load the host database, if enabled.

    bool hostDatabaseEnabled = k_DEFAULT_HOST_DATABASE_ENABLED;
    if (!d_config.hostDatabaseEnabled().isNull()) {
        hostDatabaseEnabled = d_config.hostDatabaseEnabled().value();
    }

    if (hostDatabaseEnabled) {
        if (!d_hostDatabase_sp) {
            d_hostDatabase_sp.createInplace(d_allocator_p, d_allocator_p);

            if (d_config.hostDatabasePath().isNull()) {
                error = d_hostDatabase_sp->load();
                if (error) {
                    return error;
                }
            }
            else {
                error = d_hostDatabase_sp->loadPath(
                    d_config.hostDatabasePath().value());
                if (error) {
                    return error;
                }
            }
        }
    }

    // Load the port database, if enabled.

    bool portDatabaseEnabled = k_DEFAULT_PORT_DATABASE_ENABLED;
    if (!d_config.portDatabaseEnabled().isNull()) {
        portDatabaseEnabled = d_config.portDatabaseEnabled().value();
    }

    if (portDatabaseEnabled) {
        if (!d_portDatabase_sp) {
            d_portDatabase_sp.createInplace(d_allocator_p, d_allocator_p);

            if (d_config.portDatabasePath().isNull()) {
                error = d_portDatabase_sp->load();
                if (error) {
                    return error;
                }
            }
            else {
                error = d_portDatabase_sp->loadPath(
                    d_config.portDatabasePath().value());
                if (error) {
                    return error;
                }
            }
        }
    }

    // Create the cache, if enabled.

    bool positiveCacheEnabled = k_DEFAULT_POSITIVE_CACHE_ENABLED;
    if (!d_config.positiveCacheEnabled().isNull()) {
        positiveCacheEnabled = d_config.positiveCacheEnabled().value();
    }

    bool negativeCacheEnabled = k_DEFAULT_NEGATIVE_CACHE_ENABLED;
    if (!d_config.negativeCacheEnabled().isNull()) {
        negativeCacheEnabled = d_config.negativeCacheEnabled().value();
    }

    if (positiveCacheEnabled || negativeCacheEnabled) {
        if (!d_cache_sp) {
            d_cache_sp.createInplace(d_allocator_p, d_allocator_p);

            d_cache_sp->setPositiveCacheEnabled(positiveCacheEnabled);

            if (!d_config.positiveCacheMinTimeToLive().isNull()) {
                d_cache_sp->setPositiveCacheMinTimeToLive(
                    d_config.positiveCacheMinTimeToLive().value());
            }

            if (!d_config.positiveCacheMaxTimeToLive().isNull()) {
                d_cache_sp->setPositiveCacheMaxTimeToLive(
                    d_config.positiveCacheMaxTimeToLive().value());
            }

            d_cache_sp->setNegativeCacheEnabled(negativeCacheEnabled);

            if (!d_config.negativeCacheMinTimeToLive().isNull()) {
                d_cache_sp->setNegativeCacheMinTimeToLive(
                    d_config.negativeCacheMinTimeToLive().value());
            }

            if (!d_config.negativeCacheMaxTimeToLive().isNull()) {
                d_cache_sp->setNegativeCacheMaxTimeToLive(
                    d_config.negativeCacheMaxTimeToLive().value());
            }
        }
    }

    // Create and start the client, if enabled.

    bool clientEnabled = k_DEFAULT_CLIENT_ENABLED;
    if (!d_config.clientEnabled().isNull()) {
        clientEnabled = d_config.clientEnabled().value();
    }

    if (clientEnabled) {
        if (!d_client_sp) {
            ntcdns::ClientConfig clientConfig;
            if (d_config.clientSpecificationPath().isNull()) {
                error = ntcdns::Utility::loadClientConfig(&clientConfig);
            }
            else {
                error = ntcdns::Utility::loadClientConfigFromPath(
                    &clientConfig,
                    d_config.clientSpecificationPath().value());
            }

            if (error) {
                return error;
            }

            if (!d_config.clientRemoteEndpointList().empty()) {
                clientConfig.nameServer().clear();

                for (bsl::size_t i = 0;
                     i < d_config.clientRemoteEndpointList().size();
                     ++i)
                {
                    const ntsa::Endpoint& endpoint =
                        d_config.clientRemoteEndpointList()[i];

                    ntcdns::NameServerConfig nameServerConfig;

                    if (endpoint.isIp()) {
                        nameServerConfig.address().host() =
                            endpoint.ip().host().text();
                        nameServerConfig.address().port() =
                            endpoint.ip().port();
                    }
                    else if (endpoint.isLocal()) {
                        nameServerConfig.address().host() =
                            endpoint.local().value();
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    clientConfig.nameServer().push_back(nameServerConfig);
                }
            }

            if (!d_config.clientDomainSearchList().empty()) {
                clientConfig.search() = d_config.clientDomainSearchList();
            }

            if (!d_config.clientAttempts().isNull()) {
                clientConfig.attempts() =
                    NTCCFG_WARNING_NARROW(unsigned int,
                                          d_config.clientAttempts().value());
            }

            if (!d_config.clientTimeout().isNull()) {
                clientConfig.timeout() =
                    NTCCFG_WARNING_NARROW(unsigned int,
                                          d_config.clientTimeout().value());
            }

            if (!d_config.clientRotate().isNull()) {
                clientConfig.rotate() = d_config.clientRotate().value();
            }

            if (!d_config.clientDots().isNull()) {
                clientConfig.ndots() =
                    NTCCFG_WARNING_NARROW(unsigned int,
                                          d_config.clientDots().value());
            }

            if (!d_config.clientDebug().isNull()) {
                clientConfig.debug() = d_config.clientDebug().value();
            }

            if (!d_datagramSocketFactory_sp || !d_streamSocketFactory_sp) {
                // MRM: Log
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            d_client_sp.createInplace(d_allocator_p,
                                      clientConfig,
                                      d_cache_sp,
                                      d_datagramSocketFactory_sp,
                                      d_streamSocketFactory_sp,
                                      d_allocator_p);

            error = d_client_sp->start();
            if (error) {
                NTCI_LOG_STREAM_ERROR << "Failed to start client: " << error
                                      << NTCI_LOG_STREAM_END;
                return error;
            }
        }
    }

    bool systemEnabled = clientEnabled ? false : true;
    if (!d_config.systemEnabled().isNull()) {
        systemEnabled = d_config.systemEnabled().value();
    }

    if (systemEnabled) {
        if (!d_system_sp) {
            int minThreads = k_DEFAULT_SYSTEM_MIN_THREADS;
            if (!d_config.systemMinThreads().isNull()) {
                minThreads =
                    NTCCFG_WARNING_NARROW(int,
                                          d_config.systemMinThreads().value());
            }

            int maxThreads = k_DEFAULT_SYSTEM_MAX_THREADS;
            if (!d_config.systemMaxThreads().isNull()) {
                maxThreads =
                    NTCCFG_WARNING_NARROW(int,
                                          d_config.systemMaxThreads().value());
            }

            d_system_sp.createInplace(d_allocator_p,
                                      minThreads,
                                      maxThreads,
                                      k_DEFAULT_SYSTEM_MAX_IDLE_TIME,
                                      d_allocator_p);

            error = d_system_sp->start();
            if (error) {
                return error;
            }
        }
    }

    if (!clientEnabled && !systemEnabled) {
        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName("dns-resolver");

        d_threadPool_sp.createInplace(d_allocator_p,
                                      threadAttributes,
                                      1,
                                      1,
                                      10,
                                      d_allocator_p);

        int rc = d_threadPool_sp->start();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    d_initialized = true;

    return ntsa::Error();
}

Resolver::Resolver(const ntca::ResolverConfig& configuration,
                   bslma::Allocator*           basicAllocator)
: d_object("ntcdns::Resolver")
, d_mutex()
, d_interface_sp()
, d_interfaceOwned(false)
, d_datagramSocketFactory_sp()
, d_listenerSocketFactory_sp()
, d_streamSocketFactory_sp()
, d_timerFactory_sp()
, d_strandFactory_sp()
, d_executor_sp()
, d_strand_sp()
, d_overrides_sp()
, d_hostDatabase_sp()
, d_portDatabase_sp()
, d_cache_sp()
, d_client_sp()
, d_system_sp()
, d_threadPool_sp()
, d_state(e_STATE_STOPPED)
, d_initialized(false)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Resolver::Resolver(const ntca::ResolverConfig&             configuration,
                   const bsl::shared_ptr<ntci::Interface>& interface,
                   bool                                    owned,
                   bslma::Allocator*                       basicAllocator)
: d_object("ntcdns::Resolver")
, d_mutex()
, d_interface_sp(interface)
, d_interfaceOwned(owned)
, d_datagramSocketFactory_sp(interface)
, d_listenerSocketFactory_sp(interface)
, d_streamSocketFactory_sp(interface)
, d_timerFactory_sp(interface)
, d_strandFactory_sp(interface)
, d_executor_sp(interface)
, d_strand_sp()
, d_overrides_sp()
, d_hostDatabase_sp()
, d_portDatabase_sp()
, d_cache_sp()
, d_client_sp()
, d_system_sp()
, d_threadPool_sp()
, d_state(e_STATE_STOPPED)
, d_initialized(false)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Resolver::Resolver(
    const ntca::ResolverConfig&                         configuration,
    const bsl::shared_ptr<ntci::DatagramSocketFactory>& datagramSocketFactory,
    const bsl::shared_ptr<ntci::ListenerSocketFactory>& listenerSocketFactory,
    const bsl::shared_ptr<ntci::StreamSocketFactory>&   streamSocketFactory,
    const bsl::shared_ptr<ntci::TimerFactory>&          timerFactory,
    const bsl::shared_ptr<ntci::StrandFactory>&         strandFactory,
    const bsl::shared_ptr<ntci::Executor>&              executor,
    bslma::Allocator*                                   basicAllocator)
: d_object("ntcdns::Resolver")
, d_mutex()
, d_interface_sp()
, d_interfaceOwned(false)
, d_datagramSocketFactory_sp(datagramSocketFactory)
, d_listenerSocketFactory_sp(listenerSocketFactory)
, d_streamSocketFactory_sp(streamSocketFactory)
, d_timerFactory_sp(timerFactory)
, d_strandFactory_sp(strandFactory)
, d_executor_sp(executor)
, d_strand_sp()
, d_overrides_sp()
, d_hostDatabase_sp()
, d_portDatabase_sp()
, d_cache_sp()
, d_client_sp()
, d_system_sp()
, d_threadPool_sp()
, d_state(e_STATE_STOPPED)
, d_initialized(false)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Resolver::~Resolver()
{
    this->shutdown();
    this->linger();
}

ntsa::Error Resolver::start()
{
    LockGuard lock(&d_mutex);

    if (d_state == e_STATE_STARTED) {
        return ntsa::Error();
    }
    else if (d_state == e_STATE_STOPPING) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Error error = this->initialize();
    if (error) {
        return error;
    }

    d_state = e_STATE_STARTED;

    return ntsa::Error();
}

void Resolver::shutdown()
{
    bsl::shared_ptr<ntcdns::Client> client;
    bsl::shared_ptr<ntcdns::System> system;

    {
        LockGuard lock(&d_mutex);

        if (d_state != e_STATE_STARTED) {
            return;
        }

        client = d_client_sp;
        system = d_system_sp;

        d_state = e_STATE_STOPPING;
    }

    if (system) {
        system->shutdown();
    }

    if (client) {
        client->shutdown();
    }
}

void Resolver::linger()
{
    bsl::shared_ptr<bdlmt::ThreadPool> threadPool;
    bsl::shared_ptr<ntcdns::Client>    client;
    bsl::shared_ptr<ntcdns::System>    system;
    bsl::shared_ptr<ntci::Interface>   interface;

    {
        LockGuard lock(&d_mutex);

        if (d_state == e_STATE_STOPPED) {
            return;
        }

        threadPool = d_threadPool_sp;
        client     = d_client_sp;
        system     = d_system_sp;
        interface  = d_interface_sp;
    }

    if (threadPool) {
        threadPool->stop();
    }

    if (system) {
        system->linger();
    }

    if (client) {
        client->linger();
    }

    if (interface && d_interfaceOwned) {
        interface->shutdown();
        interface->linger();
    }

    d_state = e_STATE_STOPPED;
}

ntsa::Error Resolver::setIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->setIpAddress(domainName, ipAddressList);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::addIpAddress(
    const bslstl::StringRef&            domainName,
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->addIpAddress(domainName, ipAddressList);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::addIpAddress(const bslstl::StringRef& domainName,
                                   const ntsa::IpAddress&   ipAddress)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->addIpAddress(domainName, ipAddress);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::setPort(const bslstl::StringRef&       serviceName,
                              const bsl::vector<ntsa::Port>& portList,
                              ntsa::Transport::Value         transport)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->setPort(serviceName, portList, transport);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::addPort(const bslstl::StringRef&       serviceName,
                              const bsl::vector<ntsa::Port>& portList,
                              ntsa::Transport::Value         transport)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->addPort(serviceName, portList, transport);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::addPort(const bslstl::StringRef& serviceName,
                              ntsa::Port               port,
                              ntsa::Transport::Value   transport)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->addPort(serviceName, port, transport);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::setLocalIpAddress(
    const bsl::vector<ntsa::IpAddress>& ipAddressList)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->setLocalIpAddress(ipAddressList);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::setHostname(const bsl::string& name)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->setHostname(name);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::setHostnameFullyQualified(const bsl::string& name)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_overrides_sp) {
        d_overrides_sp.createInplace(d_allocator_p, d_allocator_p);
    }

    error = d_overrides_sp->setHostnameFullyQualified(name);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Resolver::getIpAddress(const bslstl::StringRef&         domainName,
                                   const ntca::GetIpAddressOptions& options,
                                   const ntci::GetIpAddressCallback& callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<Resolver> self = this->getSelf(this);

    bsls::TimeInterval startTime = bdlt::CurrentTime::now();

    // Lazily initialize each enabled mechanism used by this object, if
    // necessary.

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    // Get the IP addresses assigned to the domain name from the overrides, if
    // defined.

    if (d_overrides_sp) {
        ntsa::IpAddressOptions ipAddressOptions;
        ntcdns::Compat::convert(&ipAddressOptions, options);

        bsl::vector<ntsa::IpAddress> ipAddressList;
        error = d_overrides_sp->getIpAddress(&ipAddressList,
                                             domainName,
                                             ipAddressOptions);
        if (!error) {
            ntca::GetIpAddressContext getIpAddressContext;

            getIpAddressContext.setDomainName(domainName);
            getIpAddressContext.setSource(ntca::ResolverSource::e_OVERRIDE);

            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getIpAddressContext.setLatency(endTime - startTime);
            }

            ntca::GetIpAddressEvent getIpAddressEvent;
            getIpAddressEvent.setType(ntca::GetIpAddressEventType::e_COMPLETE);
            getIpAddressEvent.setContext(getIpAddressContext);

            callback.dispatch(self,
                              ipAddressList,
                              getIpAddressEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the IP addresses assigned to the domain name from the host database,
    // if enabled.

    if (d_hostDatabase_sp) {
        bsl::vector<ntsa::IpAddress> ipAddressList;
        ntca::GetIpAddressContext    getIpAddressContext;

        error = d_hostDatabase_sp->getIpAddress(&getIpAddressContext,
                                                &ipAddressList,
                                                domainName,
                                                options);
        if (!error) {
            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getIpAddressContext.setLatency(endTime - startTime);
            }

            ntca::GetIpAddressEvent getIpAddressEvent;
            getIpAddressEvent.setType(ntca::GetIpAddressEventType::e_COMPLETE);
            getIpAddressEvent.setContext(getIpAddressContext);

            callback.dispatch(self,
                              ipAddressList,
                              getIpAddressEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the IP addresses assigned to the domain name from the cache, if
    // enabled.

    if (d_cache_sp) {
        bsl::vector<ntsa::IpAddress> ipAddressList;
        ntca::GetIpAddressContext    getIpAddressContext;

        error = d_cache_sp->getIpAddress(&getIpAddressContext,
                                         &ipAddressList,
                                         domainName,
                                         options,
                                         startTime);
        if (!error) {
            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getIpAddressContext.setLatency(endTime - startTime);
            }

            ntca::GetIpAddressEvent getIpAddressEvent;
            getIpAddressEvent.setType(ntca::GetIpAddressEventType::e_COMPLETE);
            getIpAddressEvent.setContext(getIpAddressContext);

            callback.dispatch(self,
                              ipAddressList,
                              getIpAddressEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the IP addresses assigned to the domain name from the name servers,
    // if enabled.

    if (d_client_sp) {
        error = d_client_sp->getIpAddress(self, domainName, options, callback);
        if (!error) {
            return ntsa::Error();
        }
    }

    // Get the IP addresses assigned to domain name from the system, if
    // enabled.

    if (d_system_sp) {
        error = d_system_sp->getIpAddress(self,
                                          domainName,
                                          startTime,
                                          options,
                                          callback);
        if (!error) {
            return ntsa::Error();
        }
    }

    // The resolution has failed.

    {
        ntca::GetIpAddressContext getIpAddressContext;
        getIpAddressContext.setError(ntsa::Error(ntsa::Error::e_EOF));

        ntca::GetIpAddressEvent getIpAddressEvent;
        getIpAddressEvent.setType(ntca::GetIpAddressEventType::e_ERROR);
        getIpAddressEvent.setContext(getIpAddressContext);

        callback.dispatch(self,
                          bsl::vector<ntsa::IpAddress>(),
                          getIpAddressEvent,
                          d_strand_sp,
                          self,
                          true,
                          NTCCFG_MUTEX_NULL);
    }

    return ntsa::Error();
}

ntsa::Error Resolver::getDomainName(
    const ntsa::IpAddress&             ipAddress,
    const ntca::GetDomainNameOptions&  options,
    const ntci::GetDomainNameCallback& callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<Resolver> self = this->getSelf(this);

    bsls::TimeInterval startTime = bdlt::CurrentTime::now();

    // Lazily initialize each enabled mechanism used by this object, if
    // necessary.

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    // Get the domain name to which the IP address is assigned from the
    // overrides, if defined.

    if (d_overrides_sp) {
        bsl::string domainName;
        error = d_overrides_sp->getDomainName(&domainName, ipAddress);
        if (!error) {
            ntca::GetDomainNameContext getDomainNameContext;

            getDomainNameContext.setIpAddress(ipAddress);
            getDomainNameContext.setSource(ntca::ResolverSource::e_OVERRIDE);

            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getDomainNameContext.setLatency(endTime - startTime);
            }

            ntca::GetDomainNameEvent getDomainNameEvent;
            getDomainNameEvent.setType(
                ntca::GetDomainNameEventType::e_COMPLETE);
            getDomainNameEvent.setContext(getDomainNameContext);

            callback.dispatch(self,
                              domainName,
                              getDomainNameEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the domain name to which the IP address is assigned from the host
    // database, if enabled.

    if (d_hostDatabase_sp) {
        bsl::string                domainName;
        ntca::GetDomainNameContext getDomainNameContext;

        error = d_hostDatabase_sp->getDomainName(&getDomainNameContext,
                                                 &domainName,
                                                 ipAddress,
                                                 options);
        if (!error) {
            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getDomainNameContext.setLatency(endTime - startTime);
            }

            ntca::GetDomainNameEvent getDomainNameEvent;
            getDomainNameEvent.setType(
                ntca::GetDomainNameEventType::e_COMPLETE);
            getDomainNameEvent.setContext(getDomainNameContext);

            callback.dispatch(self,
                              domainName,
                              getDomainNameEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the domain name to which the IP address is assigned from the
    // cache, if enabled.

    if (d_cache_sp) {
        bsl::string                domainName;
        ntca::GetDomainNameContext getDomainNameContext;

        error = d_cache_sp->getDomainName(&getDomainNameContext,
                                          &domainName,
                                          ipAddress,
                                          options,
                                          startTime);
        if (!error) {
            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getDomainNameContext.setLatency(endTime - startTime);
            }

            ntca::GetDomainNameEvent getDomainNameEvent;
            getDomainNameEvent.setType(
                ntca::GetDomainNameEventType::e_COMPLETE);
            getDomainNameEvent.setContext(getDomainNameContext);

            callback.dispatch(self,
                              domainName,
                              getDomainNameEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the domain name to which the IP address is assigned from the
    // name servers, if enabled.

    if (d_client_sp) {
        error = d_client_sp->getDomainName(self, ipAddress, options, callback);
        if (!error) {
            return ntsa::Error();
        }
    }

    // Get the domain name to which the IP address is assigned from the system,
    // if enabled.

    if (d_system_sp) {
        error = d_system_sp->getDomainName(self,
                                           ipAddress,
                                           startTime,
                                           options,
                                           callback);
        if (!error) {
            return ntsa::Error();
        }
    }

    // The resolution has failed.

    {
        ntca::GetDomainNameContext getDomainNameContext;
        getDomainNameContext.setError(ntsa::Error(ntsa::Error::e_EOF));

        ntca::GetDomainNameEvent getDomainNameEvent;
        getDomainNameEvent.setType(ntca::GetDomainNameEventType::e_ERROR);
        getDomainNameEvent.setContext(getDomainNameContext);

        callback.dispatch(self,
                          bsl::string(),
                          getDomainNameEvent,
                          d_strand_sp,
                          self,
                          true,
                          NTCCFG_MUTEX_NULL);
    }

    return ntsa::Error();
}

ntsa::Error Resolver::getPort(const bslstl::StringRef&     serviceName,
                              const ntca::GetPortOptions&  options,
                              const ntci::GetPortCallback& callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<Resolver> self = this->getSelf(this);

    bsls::TimeInterval startTime = bdlt::CurrentTime::now();

    // Lazily initialize each enabled mechanism used by this object, if
    // necessary.

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    bsl::vector<ntsa::Port> portList;

    ntsa::PortOptions portOptions;
    ntcdns::Compat::convert(&portOptions, options);

    ntca::GetPortContext getPortContext;
    getPortContext.setServiceName(serviceName);

    // Get the ports assigned to the service name from the overrides, if
    // defined.

    if (d_overrides_sp) {
        error = d_overrides_sp->getPort(&portList, serviceName, portOptions);
        if (!error) {
            getPortContext.setSource(ntca::ResolverSource::e_OVERRIDE);

            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getPortContext.setLatency(endTime - startTime);
            }

            ntca::GetPortEvent getPortEvent;
            getPortEvent.setType(ntca::GetPortEventType::e_COMPLETE);
            getPortEvent.setContext(getPortContext);

            callback.dispatch(self,
                              portList,
                              getPortEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the ports assigned to the service name from the port database, if
    // enabled.

    if (d_portDatabase_sp) {
        error = d_portDatabase_sp->getPort(&getPortContext,
                                           &portList,
                                           serviceName,
                                           options);
        if (!error) {
            getPortContext.setSource(ntca::ResolverSource::e_DATABASE);

            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getPortContext.setLatency(endTime - startTime);
            }

            ntca::GetPortEvent getPortEvent;
            getPortEvent.setType(ntca::GetPortEventType::e_COMPLETE);
            getPortEvent.setContext(getPortContext);

            callback.dispatch(self,
                              portList,
                              getPortEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the ports assigned to the service name from the cache, if enabled.

    if (d_cache_sp) {
        error = d_cache_sp->getPort(&getPortContext,
                                    &portList,
                                    serviceName,
                                    options,
                                    startTime);
        if (!error) {
            getPortContext.setSource(ntca::ResolverSource::e_CACHE);

            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getPortContext.setLatency(endTime - startTime);
            }

            ntca::GetPortEvent getPortEvent;
            getPortEvent.setType(ntca::GetPortEventType::e_COMPLETE);
            getPortEvent.setContext(getPortContext);

            callback.dispatch(self,
                              portList,
                              getPortEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the ports assigned to the service name from the system.

    if (d_executor_sp) {
        error =
            ntsu::ResolverUtil::getPort(&portList, serviceName, portOptions);
        if (!error) {
            getPortContext.setSource(ntca::ResolverSource::e_SYSTEM);

            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getPortContext.setLatency(endTime - startTime);
            }

            ntca::GetPortEvent getPortEvent;
            getPortEvent.setType(ntca::GetPortEventType::e_COMPLETE);
            getPortEvent.setContext(getPortContext);

            callback.dispatch(self,
                              portList,
                              getPortEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);
            return ntsa::Error();
        }
    }
    else if (d_system_sp) {
        error = d_system_sp->getPort(self,
                                     serviceName,
                                     startTime,
                                     options,
                                     callback);
        if (!error) {
            return ntsa::Error();
        }
    }

    // The resolution has failed.

    {
        getPortContext.setError(error);

        ntca::GetPortEvent getPortEvent;
        getPortEvent.setType(ntca::GetPortEventType::e_ERROR);
        getPortEvent.setContext(getPortContext);

        portList.clear();

        callback.dispatch(self,
                          portList,
                          getPortEvent,
                          d_strand_sp,
                          self,
                          true,
                          NTCCFG_MUTEX_NULL);
    }

    return ntsa::Error();
}

ntsa::Error Resolver::getServiceName(
    ntsa::Port                          port,
    const ntca::GetServiceNameOptions&  options,
    const ntci::GetServiceNameCallback& callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<Resolver> self = this->getSelf(this);

    bsls::TimeInterval startTime = bdlt::CurrentTime::now();

    // Lazily initialize each enabled mechanism used by this object, if
    // necessary.

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    bsl::string serviceName;

    ntsa::Transport::Value transport;
    if (!options.transport().isNull()) {
        transport = options.transport().value();
    }
    else {
        transport = ntsa::Transport::e_TCP_IPV4_STREAM;
    }

    ntca::GetServiceNameContext getServiceNameContext;
    getServiceNameContext.setPort(port);

    // Get the service name to which the port is assigned from the overrides,
    // if defined.

    if (d_overrides_sp) {
        error = d_overrides_sp->getServiceName(&serviceName, port, transport);
        if (!error) {
            getServiceNameContext.setSource(ntca::ResolverSource::e_OVERRIDE);

            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getServiceNameContext.setLatency(endTime - startTime);
            }

            ntca::GetServiceNameEvent getServiceNameEvent;
            getServiceNameEvent.setType(
                ntca::GetServiceNameEventType::e_COMPLETE);
            getServiceNameEvent.setContext(getServiceNameContext);

            callback.dispatch(self,
                              serviceName,
                              getServiceNameEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the service name to which the port is assigned from the port
    // database, if enabled.

    if (d_portDatabase_sp) {
        bsl::string                 serviceName;
        ntca::GetServiceNameContext getServiceNameContext;

        error = d_portDatabase_sp->getServiceName(&getServiceNameContext,
                                                  &serviceName,
                                                  port,
                                                  options);
        if (!error) {
            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getServiceNameContext.setLatency(endTime - startTime);
            }

            ntca::GetServiceNameEvent getServiceNameEvent;
            getServiceNameEvent.setType(
                ntca::GetServiceNameEventType::e_COMPLETE);
            getServiceNameEvent.setContext(getServiceNameContext);

            callback.dispatch(self,
                              serviceName,
                              getServiceNameEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the service name to which the port is assigned from the cache, if
    // enabled.

    if (d_cache_sp) {
        bsl::string                 serviceName;
        ntca::GetServiceNameContext getServiceNameContext;

        error = d_cache_sp->getServiceName(&getServiceNameContext,
                                           &serviceName,
                                           port,
                                           options,
                                           startTime);
        if (!error) {
            bsls::TimeInterval endTime = bdlt::CurrentTime::now();
            if (endTime > startTime) {
                getServiceNameContext.setLatency(endTime - startTime);
            }

            ntca::GetServiceNameEvent getServiceNameEvent;
            getServiceNameEvent.setType(
                ntca::GetServiceNameEventType::e_COMPLETE);
            getServiceNameEvent.setContext(getServiceNameContext);

            callback.dispatch(self,
                              serviceName,
                              getServiceNameEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }

    // Get the service name to which the port is assigned from the system.

    if (d_executor_sp) {
        error =
            ntsu::ResolverUtil::getServiceName(&serviceName, port, transport);
        if (!error) {
            ntca::GetServiceNameEvent getServiceNameEvent;
            getServiceNameEvent.setType(
                ntca::GetServiceNameEventType::e_COMPLETE);
            getServiceNameEvent.setContext(getServiceNameContext);

            callback.dispatch(self,
                              serviceName,
                              getServiceNameEvent,
                              d_strand_sp,
                              self,
                              true,
                              NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }
    else if (d_system_sp) {
        error = d_system_sp->getServiceName(self,
                                            port,
                                            startTime,
                                            options,
                                            callback);
        if (!error) {
            return ntsa::Error();
        }
    }

    // The resolution has failed.

    {
        getServiceNameContext.setError(error);

        ntca::GetServiceNameEvent getServiceNameEvent;
        getServiceNameEvent.setType(ntca::GetServiceNameEventType::e_ERROR);
        getServiceNameEvent.setContext(getServiceNameContext);

        serviceName.clear();

        callback.dispatch(self,
                          serviceName,
                          getServiceNameEvent,
                          d_strand_sp,
                          self,
                          true,
                          NTCCFG_MUTEX_NULL);
    }

    return ntsa::Error();
}

ntsa::Error Resolver::getEndpoint(const bslstl::StringRef&         text,
                                  const ntca::GetEndpointOptions&  options,
                                  const ntci::GetEndpointCallback& callback)
{
    ntsa::Error error;

    bsl::shared_ptr<Resolver> self = this->getSelf(this);

    bsls::TimeInterval startTime = bdlt::CurrentTime::now();

    // Lazily initialize each enabled mechanism used by this object, if
    // necessary.

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    ntsa::IpAddress                 ipAddress;
    bdlb::NullableValue<ntsa::Port> port;

    bslstl::StringRef unresolvedDomainName;
    bslstl::StringRef unresolvedPort;

    if (!options.transport().isNull()) {
        if (options.transport().value() == ntsa::Transport::e_LOCAL_STREAM ||
            options.transport().value() == ntsa::Transport::e_LOCAL_DATAGRAM)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    const char* begin = text.begin();
    const char* end   = text.end();

    // MRM
#if 0
    if (begin == end) {
        if (!options.ipAddressFallback().isNull()) {
            ipAddress = options.ipAddressFallback().value();
        }

        if (!options.portFallback().isNull()) {
            port = options.portFallback().value();
        }

        if (ipAddress.isUndefined() && port.isNull()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
        else {
            ntsa::Endpoint endpoint(ntsa::IpEndpoint(ipAddress, port.value()));

            ntca::GetEndpointContext getEndpointContext;

            ntca::GetEndpointEvent getEndpointEvent;
            getEndpointEvent.setType(ntca::GetEndpointEventType::e_COMPLETE);
            getEndpointEvent.setContext(getEndpointContext);

            callback.dispatch(
                self, endpoint, getEndpointEvent, d_strand_sp, self, true, NTCCFG_MUTEX_NULL);

            return ntsa::Error();
        }
    }
#endif

    if (begin != end) {
        bool isNumber = true;
        {
            for (const char* current = begin; current < end; ++current) {
                if (!bdlb::CharType::isDigit(*current)) {
                    isNumber = false;
                    break;
                }
            }
        }

        if (isNumber) {
            if (!ntsa::PortUtil::parse(&port.makeValue(), begin, end - begin))
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            if (*begin == '[') {
                const char* mark = end - 1;
                while (mark > begin) {
                    if (*mark == ']') {
                        break;
                    }

                    --mark;
                }

                if (mark == begin) {
                    return ntsa::Error(ntsa::Error::e_INVALID);  // missing ']'
                }

                const char* ipv6AddressBegin = begin + 1;
                const char* ipv6AddressEnd   = mark;

                ntsa::Ipv6Address ipv6Address;
                if (ipv6Address.parse(
                        bslstl::StringRef(ipv6AddressBegin, ipv6AddressEnd)))
                {
                    ipAddress = ipv6Address;
                }
                else {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                const char* colon = mark + 1;

                if (colon >= end) {
                    return ntsa::Error(ntsa::Error::e_INVALID);  // missing ':'
                }

                if (*colon != ':') {
                    return ntsa::Error(ntsa::Error::e_INVALID);  // missing ':'
                }

                const char* portBegin = colon + 1;
                const char* portEnd   = end;

                if (portBegin >= portEnd) {
                    return ntsa::Error(
                        ntsa::Error::e_INVALID);  // missing port
                }

                if (!ntsa::PortUtil::parse(&port.makeValue(),
                                           portBegin,
                                           portEnd - portBegin))
                {
                    port.reset();
                    unresolvedPort.assign(portBegin, portEnd);
                }
            }
            else {
                const char* current   = end - 1;
                const char* mark      = 0;
                bsl::size_t numColons = 0;
                while (current > begin) {
                    if (*current == ':') {
                        if (mark == 0) {
                            mark = current;
                        }
                        ++numColons;
                        if (numColons > 1) {
                            break;
                        }
                    }

                    --current;
                }

                if (numColons == 0) {
                    // <ip-address-or-host> (missing ':' therefore no port)

                    const char* ipAddressBegin = begin;
                    const char* ipAddressEnd   = end;

                    if (!ipAddress.parse(
                            bslstl::StringRef(ipAddressBegin, ipAddressEnd)))
                    {
                        unresolvedDomainName.assign(ipAddressBegin,
                                                    ipAddressEnd);
                    }
                }
                else if (numColons == 1) {
                    // <ipv4-address-or-host>:<port>

                    const char* ipv4AddressBegin = begin;
                    const char* ipv4AddressEnd   = mark;

                    ntsa::Ipv4Address ipv4Address;
                    if (ipv4Address.parse(bslstl::StringRef(ipv4AddressBegin,
                                                            ipv4AddressEnd)))
                    {
                        ipAddress = ipv4Address;
                    }
                    else {
                        unresolvedDomainName.assign(ipv4AddressBegin,
                                                    ipv4AddressEnd);
                    }

                    const char* portBegin = mark + 1;
                    const char* portEnd   = end;

                    if (portBegin >= portEnd) {
                        return ntsa::Error(
                            ntsa::Error::e_INVALID);  // missing port
                    }

                    if (!ntsa::PortUtil::parse(&port.makeValue(),
                                               portBegin,
                                               portEnd - portBegin))
                    {
                        port.reset();
                        unresolvedPort.assign(portBegin, portEnd);
                    }
                }
                else {
                    // <ipv6-address> (more than one ':' found)

                    const char* ipv6AddressBegin = begin;
                    const char* ipv6AddressEnd   = end;

                    ntsa::Ipv6Address ipv6Address;
                    if (ipv6Address.parse(bslstl::StringRef(ipv6AddressBegin,
                                                            ipv6AddressEnd)))
                    {
                        ipAddress = ipv6Address;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
            }
        }
    }

    if (!unresolvedPort.empty()) {
        bsl::vector<ntsa::Port> portList;
        ntsa::PortOptions       portOptions;

        if (!options.portSelector().isNull()) {
            portOptions.setPortSelector(options.portSelector().value());
        }

        if (!options.transport().isNull()) {
            portOptions.setTransport(options.transport().value());
        }

        bool needPort = true;

        {
            LockGuard lock(&d_mutex);

            if (needPort && d_overrides_sp) {
                error = d_overrides_sp->getPort(&portList,
                                                unresolvedPort,
                                                portOptions);
                if (!error) {
                    needPort = false;
                }
            }

            if (needPort && d_portDatabase_sp) {
                ntca::GetPortContext getPortContext;

                ntca::GetPortOptions getPortOptions;
                ntcdns::Compat::convert(&getPortOptions, options);

                error = d_portDatabase_sp->getPort(&getPortContext,
                                                   &portList,
                                                   unresolvedPort,
                                                   getPortOptions);
                if (!error) {
                    needPort = false;
                }
            }

            if (needPort && d_cache_sp) {
                bsls::TimeInterval now = bdlt::CurrentTime::now();

                ntca::GetPortContext getPortContext;

                ntca::GetPortOptions getPortOptions;
                ntcdns::Compat::convert(&getPortOptions, options);

                error = d_cache_sp->getPort(&getPortContext,
                                            &portList,
                                            unresolvedPort,
                                            getPortOptions,
                                            now);
                if (!error) {
                    needPort = false;
                }
            }
        }

        if (needPort) {
            error = ntsu::ResolverUtil::getPort(&portList,
                                                unresolvedPort,
                                                portOptions);
            if (error) {
                return error;
            }
        }

        if (portList.empty()) {
            return ntsa::Error(ntsa::Error::e_EOF);  // unresolvable port
        }

        port = portList.front();
    }
    else if (port.isNull()) {
        if (!options.portFallback().isNull()) {
            port = options.portFallback().value();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (!unresolvedDomainName.empty()) {
        ntci::GetIpAddressCallback getIpAddressCallback =
            this->createGetIpAddressCallback(
                bdlf::BindUtil::bind(&processGetIpAddressResult,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     startTime,
                                     bsl::string(unresolvedPort),
                                     port.value(),
                                     bdlf::PlaceHolders::_3,
                                     callback),
                d_allocator_p);

        ntca::GetIpAddressOptions getIpAddressOptions;
        ntcdns::Compat::convert(&getIpAddressOptions, options);

        error = this->getIpAddress(unresolvedDomainName,
                                   getIpAddressOptions,
                                   getIpAddressCallback);
        if (error) {
            return error;
        }

        return ntsa::Error();
    }
    else if (ipAddress.isUndefined()) {
        if (!options.ipAddressFallback().isNull()) {
            ipAddress = options.ipAddressFallback().value();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (ipAddress.isUndefined()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (port.isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!options.ipAddressType().isNull()) {
        if (ipAddress.type() != options.ipAddressType().value()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (!options.transport().isNull()) {
        if (options.transport().value() ==
                ntsa::Transport::e_TCP_IPV4_STREAM ||
            options.transport().value() ==
                ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            if (!ipAddress.isV4()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else if (options.transport().value() ==
                     ntsa::Transport::e_TCP_IPV6_STREAM ||
                 options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            if (!ipAddress.isV6()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntsa::Endpoint endpoint(ntsa::IpEndpoint(ipAddress, port.value()));

    ntca::GetEndpointContext getEndpointContext;

    ntca::GetEndpointEvent getEndpointEvent;
    getEndpointEvent.setType(ntca::GetEndpointEventType::e_COMPLETE);
    getEndpointEvent.setContext(getEndpointContext);

    callback.dispatch(self,
                      endpoint,
                      getEndpointEvent,
                      d_strand_sp,
                      self,
                      true,
                      NTCCFG_MUTEX_NULL);

    return ntsa::Error();
}

ntsa::Error Resolver::getLocalIpAddress(bsl::vector<ntsa::IpAddress>* result,
                                        const ntsa::IpAddressOptions& options)
{
    ntsa::Error error;

    {
        LockGuard lock(&d_mutex);

        if (d_overrides_sp) {
            error = d_overrides_sp->getLocalIpAddress(result, options);
            if (!error) {
                return ntsa::Error();
            }
        }
    }

    return ntsu::ResolverUtil::getLocalIpAddress(result, options);
}

ntsa::Error Resolver::getHostname(bsl::string* result)
{
    ntsa::Error error;

    {
        LockGuard lock(&d_mutex);

        if (d_overrides_sp) {
            error = d_overrides_sp->getHostname(result);
            if (!error) {
                return ntsa::Error();
            }
        }
    }

    return ntsu::ResolverUtil::getHostname(result);
}

ntsa::Error Resolver::getHostnameFullyQualified(bsl::string* result)
{
    ntsa::Error error;

    {
        LockGuard lock(&d_mutex);

        if (d_overrides_sp) {
            error = d_overrides_sp->getHostnameFullyQualified(result);
            if (!error) {
                return ntsa::Error();
            }
        }
    }

    return ntsu::ResolverUtil::getHostnameFullyQualified(result);
}

void Resolver::execute(const Functor& functor)
{
    int rc;

    if (d_executor_sp) {
        d_executor_sp->execute(functor);
    }
    else if (d_system_sp) {
        d_system_sp->execute(functor);
    }
    else if (d_threadPool_sp) {
        rc = d_threadPool_sp->enqueueJob(functor);
        if (rc != 0) {
            return;
        }
    }
}

void Resolver::moveAndExecute(FunctorSequence* functorSequence,
                              const Functor&   functor)
{
    int rc;

    if (d_executor_sp) {
        d_executor_sp->moveAndExecute(functorSequence, functor);
    }
    else if (d_system_sp) {
        d_system_sp->moveAndExecute(functorSequence, functor);
    }
    else if (d_threadPool_sp) {
        {
            FunctorSequence::const_iterator it = functorSequence->begin();
            FunctorSequence::const_iterator et = functorSequence->end();

            for (; it != et; ++it) {
                rc = d_threadPool_sp->enqueueJob(*it);
                if (rc != 0) {
                    return;
                }
            }

            functorSequence->clear();
        }

        rc = d_threadPool_sp->enqueueJob(functor);
        if (rc != 0) {
            return;
        }
    }
}

bsl::shared_ptr<ntci::Strand> Resolver::createStrand(
    bslma::Allocator* basicAllocator)
{
    if (d_strandFactory_sp) {
        return d_strandFactory_sp->createStrand(basicAllocator);
    }
    else if (d_system_sp) {
        return d_system_sp->createStrand(basicAllocator);
    }
    else {
        return bsl::shared_ptr<ntci::Strand>();
    }
}

bsl::shared_ptr<ntci::Timer> Resolver::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    if (d_timerFactory_sp) {
        return d_timerFactory_sp->createTimer(options,
                                              session,
                                              basicAllocator);
    }
    else {
        return bsl::shared_ptr<ntci::Timer>();
    }
}

bsl::shared_ptr<ntci::Timer> Resolver::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    if (d_timerFactory_sp) {
        return d_timerFactory_sp->createTimer(options,
                                              callback,
                                              basicAllocator);
    }
    else {
        return bsl::shared_ptr<ntci::Timer>();
    }
}

ntsa::Error Resolver::loadHostDatabaseText(const char* data, bsl::size_t size)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    // Lazily initialize each enabled mechanism used by this object, if
    // necessary.

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    // Load the host database.

    if (!d_hostDatabase_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_hostDatabase_sp->loadText(data, size);
}

ntsa::Error Resolver::loadPortDatabaseText(const char* data, bsl::size_t size)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    // Lazily initialize each enabled mechanism used by this object, if
    // necessary.

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    // Load the port database.

    if (!d_portDatabase_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return d_portDatabase_sp->loadText(data, size);
}

ntsa::Error Resolver::cacheHost(const bsl::string&        domainName,
                                const ntsa::IpAddress&    ipAddress,
                                const ntsa::Endpoint&     nameServer,
                                bsl::size_t               timeToLive,
                                const bsls::TimeInterval& now)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    // Lazily initialize each enabled mechanism used by this object, if
    // necessary.

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    // Cache the host.

    if (!d_cache_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_cache_sp->updateHost(domainName, ipAddress, nameServer, timeToLive, now);
    return ntsa::Error();
}

const bsl::shared_ptr<ntci::Strand>& Resolver::strand() const
{
    return d_strand_sp;
}

bsls::TimeInterval Resolver::currentTime() const
{
    return bdlt::CurrentTime::now();
}

}  // close package namespace
}  // close enterprise namespace
