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

#include <ntcdns_system.h>

#include <ntcdns_compat.h>
#include <ntcs_strand.h>
#include <ntsu_resolverutil.h>

#include <ntci_log.h>
#include <ntsa_host.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcdns {

namespace {

const int k_DEFAULT_MIN_THREADS   = 0;
const int k_DEFAULT_MAX_THREADS   = 1;
const int k_DEFAULT_MAX_IDLE_TIME = 10;

}  // close unnamed namespace

ntsa::Error System::initialize()
{
    if (!d_threadPool_sp) {
        bslmt::ThreadAttributes threadAttributes;
        threadAttributes.setThreadName("dns-system");

        d_threadPool_sp.createInplace(d_allocator_p,
                                      threadAttributes,
                                      d_minThreads,
                                      d_maxThreads,
                                      d_maxIdleTime,
                                      d_allocator_p);

        int rc = d_threadPool_sp->start();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

void System::executeGetIpAddress(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     domainName,
    const bsls::TimeInterval&              startTime,
    const ntca::GetIpAddressOptions&       options,
    const ntci::GetIpAddressCallback&      callback)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_TRACE << "System DNS thread pool resolving domain name '"
                          << domainName << "' according to options " << options
                          << ": " << d_threadPool_sp->numActiveThreads()
                          << " active, " << d_threadPool_sp->numPendingJobs()
                          << " pending" << NTCI_LOG_STREAM_END;

    ntsa::Error error;

    ntca::GetIpAddressContext getIpAddressContext;
    ntca::GetIpAddressEvent   getIpAddressEvent;

    ntsa::IpAddressOptions ipAddressOptions;
    ntcdns::Compat::convert(&ipAddressOptions, options);

    bsl::vector<ntsa::IpAddress> ipAddressList;

    if (d_state != e_STATE_STARTED) {
        getIpAddressEvent.setType(ntca::GetIpAddressEventType::e_ERROR);
        getIpAddressContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
    }
    else {
        error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                                 domainName,
                                                 ipAddressOptions);
        if (error) {
            getIpAddressEvent.setType(ntca::GetIpAddressEventType::e_ERROR);
            getIpAddressContext.setError(error);
        }
        else {
            getIpAddressEvent.setType(ntca::GetIpAddressEventType::e_COMPLETE);
        }
    }

    bsls::TimeInterval endTime = bdlt::CurrentTime::now();

    getIpAddressContext.setDomainName(domainName);
    getIpAddressContext.setSource(ntca::ResolverSource::e_SYSTEM);

    if (endTime > startTime) {
        getIpAddressContext.setLatency(endTime - startTime);
    }

    getIpAddressEvent.setContext(getIpAddressContext);

    callback(resolver,
             ipAddressList,
             getIpAddressEvent,
             ntci::Strand::unknown());
}

void System::executeGetDomainName(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::IpAddress&                 ipAddress,
    const bsls::TimeInterval&              startTime,
    const ntca::GetDomainNameOptions&      options,
    const ntci::GetDomainNameCallback&     callback)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_TRACE << "System DNS thread pool resolving IP address "
                          << ipAddress << " according to options " << options
                          << ": " << d_threadPool_sp->numActiveThreads()
                          << " active, " << d_threadPool_sp->numPendingJobs()
                          << " pending" << NTCI_LOG_STREAM_END;

    ntsa::Error error;

    ntca::GetDomainNameContext getDomainNameContext;
    ntca::GetDomainNameEvent   getDomainNameEvent;

    bsl::string domainName;

    if (d_state != e_STATE_STARTED) {
        getDomainNameEvent.setType(ntca::GetDomainNameEventType::e_ERROR);
        getDomainNameContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
    }
    else {
        error = ntsu::ResolverUtil::getDomainName(&domainName, ipAddress);
        if (error) {
            getDomainNameEvent.setType(ntca::GetDomainNameEventType::e_ERROR);
            getDomainNameContext.setError(error);
        }
        else {
            getDomainNameEvent.setType(
                ntca::GetDomainNameEventType::e_COMPLETE);
        }
    }

    bsls::TimeInterval endTime = bdlt::CurrentTime::now();

    getDomainNameContext.setIpAddress(ipAddress);
    getDomainNameContext.setSource(ntca::ResolverSource::e_SYSTEM);

    if (endTime > startTime) {
        getDomainNameContext.setLatency(endTime - startTime);
    }

    getDomainNameEvent.setContext(getDomainNameContext);

    callback(resolver,
             domainName,
             getDomainNameEvent,
             ntci::Strand::unknown());
}

void System::executeGetPort(const bsl::shared_ptr<ntci::Resolver>& resolver,
                            const bsl::string&                     serviceName,
                            const bsls::TimeInterval&              startTime,
                            const ntca::GetPortOptions&            options,
                            const ntci::GetPortCallback&           callback)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_TRACE << "System DNS thread pool resolving service name '"
                          << serviceName << "' according to options "
                          << options << ": "
                          << d_threadPool_sp->numActiveThreads() << " active, "
                          << d_threadPool_sp->numPendingJobs() << " pending"
                          << NTCI_LOG_STREAM_END;

    ntsa::Error error;

    ntca::GetPortContext getPortContext;
    ntca::GetPortEvent   getPortEvent;

    ntsa::PortOptions portOptions;
    ntcdns::Compat::convert(&portOptions, options);

    bsl::vector<ntsa::Port> portList;

    if (d_state != e_STATE_STARTED) {
        getPortEvent.setType(ntca::GetPortEventType::e_ERROR);
        getPortContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
    }
    else {
        error =
            ntsu::ResolverUtil::getPort(&portList, serviceName, portOptions);
        if (error) {
            getPortEvent.setType(ntca::GetPortEventType::e_ERROR);
            getPortContext.setError(error);
        }
        else {
            getPortEvent.setType(ntca::GetPortEventType::e_COMPLETE);
        }
    }

    bsls::TimeInterval endTime = bdlt::CurrentTime::now();

    getPortContext.setServiceName(serviceName);
    getPortContext.setSource(ntca::ResolverSource::e_SYSTEM);

    if (endTime > startTime) {
        getPortContext.setLatency(endTime - startTime);
    }

    getPortEvent.setContext(getPortContext);

    callback(resolver, portList, getPortEvent, ntci::Strand::unknown());
}

void System::executeGetServiceName(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    ntsa::Port                             port,
    const bsls::TimeInterval&              startTime,
    const ntca::GetServiceNameOptions&     options,
    const ntci::GetServiceNameCallback&    callback)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_TRACE << "System DNS thread pool resolving port " << port
                          << " according to options " << options << ": "
                          << d_threadPool_sp->numActiveThreads() << " active, "
                          << d_threadPool_sp->numPendingJobs() << " pending"
                          << NTCI_LOG_STREAM_END;

    ntsa::Error error;

    ntca::GetServiceNameContext getServiceNameContext;
    ntca::GetServiceNameEvent   getServiceNameEvent;

    bsl::string domainName;

    ntsa::Transport::Value transport;
    if (!options.transport().isNull()) {
        transport = options.transport().value();
    }
    else {
        transport = ntsa::Transport::e_TCP_IPV4_STREAM;
    }

    if (d_state != e_STATE_STARTED) {
        getServiceNameEvent.setType(ntca::GetServiceNameEventType::e_ERROR);
        getServiceNameContext.setError(ntsa::Error(ntsa::Error::e_CANCELLED));
    }
    else {
        error =
            ntsu::ResolverUtil::getServiceName(&domainName, port, transport);
        if (error) {
            getServiceNameEvent.setType(
                ntca::GetServiceNameEventType::e_ERROR);
            getServiceNameContext.setError(error);
        }
        else {
            getServiceNameEvent.setType(
                ntca::GetServiceNameEventType::e_COMPLETE);
        }
    }

    bsls::TimeInterval endTime = bdlt::CurrentTime::now();

    getServiceNameContext.setPort(port);
    getServiceNameContext.setSource(ntca::ResolverSource::e_SYSTEM);

    if (endTime > startTime) {
        getServiceNameContext.setLatency(endTime - startTime);
    }

    getServiceNameEvent.setContext(getServiceNameContext);

    callback(resolver,
             domainName,
             getServiceNameEvent,
             ntci::Strand::unknown());
}

System::System(bslma::Allocator* basicAllocator)
: d_mutex()
, d_threadPool_sp()
, d_minThreads(k_DEFAULT_MIN_THREADS)
, d_maxThreads(k_DEFAULT_MAX_THREADS)
, d_maxIdleTime(k_DEFAULT_MAX_IDLE_TIME)
, d_state(e_STATE_STOPPED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

System::System(int               minThreads,
               int               maxThreads,
               int               maxIdleTime,
               bslma::Allocator* basicAllocator)
: d_mutex()
, d_threadPool_sp()
, d_minThreads(minThreads)
, d_maxThreads(maxThreads)
, d_maxIdleTime(maxIdleTime)
, d_state(e_STATE_STOPPED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

System::~System()
{
}

ntsa::Error System::start()
{
    LockGuard lock(&d_mutex);

    if (d_state == e_STATE_STARTED) {
        return ntsa::Error();
    }
    else if (d_state != e_STATE_STOPPED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_state = e_STATE_STARTED;

    return ntsa::Error();
}

void System::shutdown()
{
    d_state.testAndSwap(e_STATE_STARTED, e_STATE_STOPPING);
}

void System::linger()
{
    bsl::shared_ptr<bdlmt::ThreadPool> threadPool;
    {
        LockGuard lock(&d_mutex);
        threadPool = d_threadPool_sp;
    }

    if (threadPool) {
        threadPool->stop();
        threadPool.reset();
    }

    {
        LockGuard lock(&d_mutex);
        d_state = e_STATE_STOPPED;
        d_threadPool_sp.reset();
    }
}

ntsa::Error System::getIpAddress(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     domainName,
    const bsls::TimeInterval&              startTime,
    const ntca::GetIpAddressOptions&       options,
    const ntci::GetIpAddressCallback&      callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;
    int         rc;

    if (d_state != e_STATE_STARTED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_threadPool_sp) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    rc = d_threadPool_sp->enqueueJob(
        bdlf::BindUtil::bind(&System::executeGetIpAddress,
                             this,
                             resolver,
                             domainName,
                             startTime,
                             options,
                             callback));
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error System::getDomainName(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::IpAddress&                 ipAddress,
    const bsls::TimeInterval&              startTime,
    const ntca::GetDomainNameOptions&      options,
    const ntci::GetDomainNameCallback&     callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;
    int         rc;

    if (d_state != e_STATE_STARTED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_threadPool_sp) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    rc = d_threadPool_sp->enqueueJob(
        bdlf::BindUtil::bind(&System::executeGetDomainName,
                             this,
                             resolver,
                             ipAddress,
                             startTime,
                             options,
                             callback));
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error System::getPort(const bsl::shared_ptr<ntci::Resolver>& resolver,
                            const bsl::string&                     serviceName,
                            const bsls::TimeInterval&              startTime,
                            const ntca::GetPortOptions&            options,
                            const ntci::GetPortCallback&           callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;
    int         rc;

    if (d_state != e_STATE_STARTED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_threadPool_sp) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    rc = d_threadPool_sp->enqueueJob(
        bdlf::BindUtil::bind(&System::executeGetPort,
                             this,
                             resolver,
                             serviceName,
                             startTime,
                             options,
                             callback));
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error System::getServiceName(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    ntsa::Port                             port,
    const bsls::TimeInterval&              startTime,
    const ntca::GetServiceNameOptions&     options,
    const ntci::GetServiceNameCallback&    callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;
    int         rc;

    if (d_state != e_STATE_STARTED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_threadPool_sp) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    rc = d_threadPool_sp->enqueueJob(
        bdlf::BindUtil::bind(&System::executeGetServiceName,
                             this,
                             resolver,
                             port,
                             startTime,
                             options,
                             callback));
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

void System::execute(const Functor& functor)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;
    int         rc;

    if (d_state != e_STATE_STARTED) {
        return;
    }

    if (!d_threadPool_sp) {
        error = this->initialize();
        if (error) {
            return;
        }
    }

    rc = d_threadPool_sp->enqueueJob(functor);
    if (rc != 0) {
        return;
    }
}

void System::moveAndExecute(FunctorSequence* functorSequence,
                            const Functor&   functor)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;
    int         rc;

    if (d_state != e_STATE_STARTED) {
        return;
    }

    if (!d_threadPool_sp) {
        error = this->initialize();
        if (error) {
            return;
        }
    }

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

bsl::shared_ptr<ntci::Strand> System::createStrand(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<System> self = this->getSelf(this);

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, self, allocator);

    return strand;
}

}  // close package namespace
}  // close enterprise namespace
