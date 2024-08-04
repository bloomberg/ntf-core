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

#include <ntcr_interface.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcr_interface_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>
#include <ntcdns_resolver.h>
#include <ntci_log.h>
#include <ntcm_monitorableregistry.h>
#include <ntcm_monitorableutil.h>
#include <ntcr_datagramsocket.h>
#include <ntcr_listenersocket.h>
#include <ntcr_streamsocket.h>
#include <ntcs_compat.h>
#include <ntcs_plugin.h>
#include <ntcs_ratelimiter.h>
#include <ntcs_strand.h>
#include <ntcs_threadutil.h>
#include <ntcs_user.h>

#include <bdlt_currenttime.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>

#define NTCR_INTERFACE_LOG_STARTING(config, numThreads)                       \
    NTCI_LOG_DEBUG(                                                           \
        "Interface '%s' is starting %d/%d thread(s) "                         \
        "with %s load balancing",                                             \
        config.metricName().c_str(),                                          \
        (int)(numThreads),                                                    \
        (int)(config.maxThreads()),                                           \
        (config.dynamicLoadBalancing().value() ? "dynamic" : "static"))

#define NTCR_INTERFACE_LOG_STARTED(config)                                    \
    NTCI_LOG_DEBUG("Interface '%s' has started", config.metricName().c_str())

#define NTCR_INTERFACE_LOG_STOPPING(config)                                   \
    NTCI_LOG_DEBUG("Interface '%s' is stopping", config.metricName().c_str())

#define NTCR_INTERFACE_LOG_STOPPED(config)                                    \
    NTCI_LOG_DEBUG("Interface '%s' has stopped", config.metricName().c_str())

#define NTCR_INTERFACE_LOG_LOAD_FACTOR_EXCEEDED(config, load)                 \
    NTCI_LOG_DEBUG("Interface '%s' least used thread has a load of %d, "      \
                   "greater than the maximum desired load of %d",             \
                   config.metricName().c_str(),                               \
                   (int)(load),                                               \
                   (int)(config.threadLoadFactor()))

#define NTCR_INTERFACE_LOG_EXPANDING(config, numThreads)                      \
    NTCI_LOG_DEBUG("Interface '%s' is expanding to %d/%d threads",            \
                   config.metricName().c_str(),                               \
                   (int)(numThreads),                                         \
                   (int)(config.maxThreads()))

namespace BloombergLP {
namespace ntcr {

void* Interface::run(void* context)
{
    ntcs::ThreadContext* runner = static_cast<ntcs::ThreadContext*>(context);
    BSLS_ASSERT_OPT(runner);

    ntcr::Interface* interface =
        static_cast<ntcr::Interface*>(runner->d_object_p);
    BSLS_ASSERT_OPT(interface);

    ntci::Reactor* reactor = static_cast<ntci::Reactor*>(runner->d_driver_p);
    BSLS_ASSERT_OPT(reactor);

    if (!runner->d_threadName.empty()) {
        bslmt::ThreadUtil::setThreadName(runner->d_threadName);
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(interface->d_config.metricName().c_str());
    NTCI_LOG_CONTEXT_GUARD_THREAD(runner->d_threadIndex);

    bsl::string metricName;
    {
        bsl::stringstream ss;
        ss << "thread-" << runner->d_threadIndex;

        metricName = ss.str();
    }

    ntca::WaiterOptions waiterOptions;
    waiterOptions.setMetricName(metricName);
    waiterOptions.setThreadHandle(bslmt::ThreadUtil::self());
    waiterOptions.setThreadIndex(runner->d_threadIndex);

    ntci::Waiter waiter = reactor->registerWaiter(waiterOptions);

    NTCI_LOG_TRACE("Thread has started");

    BSLS_ASSERT_OPT(runner->d_semaphore_p);
    runner->d_semaphore_p->post();

    reactor->run(waiter);
    reactor->drainFunctions();

    reactor->deregisterWaiter(waiter);

    return 0;
}

bsl::shared_ptr<ntci::Resolver> Interface::createResolver()
{
    BSLS_ASSERT_OPT(!d_config.resolverEnabled().isNull());
    BSLS_ASSERT_OPT(d_config.resolverEnabled().value());
    BSLS_ASSERT_OPT(!d_config.resolverConfig().isNull());

    bsl::shared_ptr<ntci::Interface> interface(this,
                                               bslstl::SharedPtrNilDeleter(),
                                               d_allocator_p);

    const bool interfaceOwned = false;

    bsl::shared_ptr<ntcdns::Resolver> resolver;
    resolver.createInplace(d_allocator_p,
                           d_config.resolverConfig().value(),
                           interface,
                           interfaceOwned,
                           d_allocator_p);

    d_user_sp->setResolver(resolver);

    return resolver;
}

bsl::shared_ptr<ntci::Reactor> Interface::addReactor()
{
    bsl::size_t minThreads = 1;
    bsl::size_t maxThreads = 1;

    BSLS_ASSERT_OPT(!d_config.dynamicLoadBalancing().isNull());
    if (d_config.dynamicLoadBalancing().value()) {
        BSLS_ASSERT_OPT(d_config.minThreads() >= 1);
        BSLS_ASSERT_OPT(d_config.maxThreads() >= d_config.minThreads());
        BSLS_ASSERT_OPT(d_config.maxThreads() <= NTCCFG_DEFAULT_MAX_THREADS);

        minThreads = d_config.minThreads();
        maxThreads = d_config.maxThreads();
    }

    bsl::string metricName;
    {
        BSLS_ASSERT_OPT(!d_config.metricName().empty());

        bsl::stringstream ss;
        ss << d_config.metricName() << "-driver-" << d_reactorVector.size();
        metricName = ss.str();
    }

    ntca::ReactorConfig reactorConfig;

    reactorConfig.setDriverName(d_config.driverName());
    reactorConfig.setMetricName(metricName);

    reactorConfig.setMinThreads(minThreads);
    reactorConfig.setMaxThreads(maxThreads);

    if (!d_config.maxEventsPerWait().isNull()) {
        reactorConfig.setMaxEventsPerWait(d_config.maxEventsPerWait().value());
    }

    if (!d_config.maxTimersPerWait().isNull()) {
        reactorConfig.setMaxTimersPerWait(d_config.maxTimersPerWait().value());
    }

    if (!d_config.maxCyclesPerWait().isNull()) {
        reactorConfig.setMaxCyclesPerWait(d_config.maxCyclesPerWait().value());
    }

    if (!d_config.driverMetrics().isNull()) {
        reactorConfig.setMetricCollection(d_config.driverMetrics().value());
    }

    if (!d_config.driverMetricsPerWaiter().isNull()) {
        reactorConfig.setMetricCollectionPerWaiter(
            d_config.driverMetricsPerWaiter().value());
    }

    if (!d_config.socketMetricsPerHandle().isNull()) {
        reactorConfig.setMetricCollectionPerSocket(
            d_config.socketMetricsPerHandle().value());
    }

    reactorConfig.setAutoAttach(false);
    reactorConfig.setAutoDetach(false);

    reactorConfig.setTrigger(ntca::ReactorEventTrigger::e_LEVEL);

    if (maxThreads > 1) {
        reactorConfig.setOneShot(true);
    }
    else {
        reactorConfig.setOneShot(false);
    }

    bsl::shared_ptr<ntci::Reactor> reactor =
        d_reactorFactory_sp->createReactor(reactorConfig,
                                           d_user_sp,
                                           d_allocator_p);

    d_reactorVector.push_back(reactor);

    return reactor;
}

ntsa::Error Interface::addThread()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    BSLS_ASSERT_OPT(!d_config.dynamicLoadBalancing().isNull());
    if (d_config.dynamicLoadBalancing().value()) {
        if (d_reactorVector.empty()) {
            this->addReactor();
            BSLS_ASSERT_OPT(!d_reactorVector.empty());
        }
    }
    else {
        if (d_reactorVector.size() < d_threadVector.size() + 1) {
            BSLS_ASSERT_OPT(d_reactorVector.size() == d_threadVector.size());
            this->addReactor();
            BSLS_ASSERT_OPT(d_reactorVector.size() ==
                            d_threadVector.size() + 1);
        }
    }

    bsl::size_t threadIndex = d_threadVector.size();
    NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex);

    bsl::string metricName;
    {
        BSLS_ASSERT_OPT(!d_config.metricName().empty());

        bsl::stringstream ss;
        ss << d_config.metricName() << "-thread-" << threadIndex;
        metricName = ss.str();
    }

    bsl::string threadName;
    {
        BSLS_ASSERT_OPT(!d_config.threadName().empty());

        bsl::stringstream ss;
        ss << d_config.threadName() << "-" << threadIndex;
        threadName = ss.str();
    }

    bsl::shared_ptr<ntci::Reactor> reactor;

    BSLS_ASSERT_OPT(!d_config.dynamicLoadBalancing().isNull());
    if (d_config.dynamicLoadBalancing().value()) {
        BSLS_ASSERT_OPT(d_reactorVector.size() == 1);
        reactor = d_reactorVector.front();
    }
    else {
        BSLS_ASSERT_OPT(d_reactorVector.size() > threadIndex);
        reactor = d_reactorVector[threadIndex];
    }

    BSLS_ASSERT_OPT(reactor);

    bslmt::ThreadAttributes threadAttributes;
    threadAttributes.setThreadName(threadName);
    threadAttributes.setDetachedState(
        bslmt::ThreadAttributes::e_CREATE_JOINABLE);
    threadAttributes.setStackSize(
        static_cast<int>(d_config.threadStackSize()));

    bslmt::ThreadUtil::Handle threadHandle =
        bslmt::ThreadUtil::invalidHandle();

    NTCI_LOG_TRACE("Thread is starting");

    ntcs::ThreadContext runner(d_allocator_p);

    runner.d_object_p    = this;
    runner.d_driver_p    = reactor.get();
    runner.d_semaphore_p = &d_threadSemaphore;
    runner.d_threadName  = threadName;
    runner.d_threadIndex = threadIndex;

    bslmt::ThreadUtil::ThreadFunction threadFunction =
        (bslmt::ThreadUtil::ThreadFunction)(&ntcr::Interface::run);
    void* threadUserData = &runner;

    ntsa::Error error = ntcs::ThreadUtil::create(&threadHandle,
                                                 threadAttributes,
                                                 threadFunction,
                                                 threadUserData);

    if (error) {
        NTCI_LOG_ERROR("Failed to create thread: %s", error.text().c_str());
        return error;
    }

    d_threadSemaphore.wait();

    BSLS_ASSERT_OPT(threadHandle != bslmt::ThreadUtil::Handle());
    BSLS_ASSERT_OPT(threadHandle != bslmt::ThreadUtil::invalidHandle());

    bslmt::ThreadUtil::Id threadId =
        bslmt::ThreadUtil::handleToId(threadHandle);

    bsl::uint64_t threadIdValue = bslmt::ThreadUtil::idAsUint64(threadId);

    BSLS_ASSERT_OPT(d_threadVector.size() < d_config.maxThreads());
    d_threadVector.push_back(threadHandle);

    bsl::pair<ThreadMap::iterator, bool> insertResult =
        d_threadMap.insert(ThreadMap::value_type(threadIdValue, reactor));
    BSLS_ASSERT_OPT(insertResult.second);

    if (d_threadVector.size() > d_threadWatermark) {
        d_threadWatermark = d_threadVector.size();
    }

    return ntsa::Error();
}

bsl::shared_ptr<ntci::Reactor> Interface::acquireReactorUsedByThreadHandle(
    const ntca::LoadBalancingOptions& options)
{
    LockGuard lock(&d_mutex);

    bsl::shared_ptr<ntci::Reactor> result;

    BSLS_ASSERT_OPT(!options.threadHandle().isNull());
    bslmt::ThreadUtil::Handle threadHandle = options.threadHandle().value();

    if (threadHandle != bslmt::ThreadUtil::Handle() &&
        threadHandle != bslmt::ThreadUtil::invalidHandle())
    {
        bslmt::ThreadUtil::Id threadId =
            bslmt::ThreadUtil::handleToId(threadHandle);

        bsl::uint64_t threadIdValue = bslmt::ThreadUtil::idAsUint64(threadId);

        ThreadMap::iterator it = d_threadMap.find(threadIdValue);
        if (it != d_threadMap.end()) {
            result = it->second;
        }
    }

    if (result) {
        result->incrementLoad(options);
    }

    return result;
}

bsl::shared_ptr<ntci::Reactor> Interface::acquireReactorUsedByThreadIndex(
    const ntca::LoadBalancingOptions& options)
{
    LockGuard lock(&d_mutex);

    bsl::shared_ptr<ntci::Reactor> result;

    if (!d_threadVector.empty()) {
        bsl::size_t index =
            options.threadIndex().value() % d_threadVector.size();

        BSLS_ASSERT_OPT(index < d_threadVector.size());
        bslmt::ThreadUtil::Handle threadHandle = d_threadVector[index];

        BSLS_ASSERT_OPT(threadHandle != bslmt::ThreadUtil::Handle());
        BSLS_ASSERT_OPT(threadHandle != bslmt::ThreadUtil::invalidHandle());

        bslmt::ThreadUtil::Id threadId =
            bslmt::ThreadUtil::handleToId(threadHandle);

        bsl::uint64_t threadIdValue = bslmt::ThreadUtil::idAsUint64(threadId);

        ThreadMap::iterator it = d_threadMap.find(threadIdValue);
        if (it != d_threadMap.end()) {
            result = it->second;
        }
    }

    if (result) {
        result->incrementLoad(options);
    }

    return result;
}

bsl::shared_ptr<ntci::Reactor> Interface::acquireReactorWithLeastLoad(
    const ntca::LoadBalancingOptions& options)
{
    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    bsl::shared_ptr<ntci::Reactor> result;

    while (true) {
        BSLS_ASSERT_OPT(!d_reactorVector.empty());

        if (d_reactorVector.size() == 1) {
            result = d_reactorVector.front();
        }
        else {
            bsl::size_t minimumLoad      = static_cast<bsl::size_t>(-1);
            bsl::size_t minimumLoadIndex = static_cast<bsl::size_t>(-1);

            for (ReactorVector::iterator it = d_reactorVector.begin();
                 it != d_reactorVector.end();
                 ++it)
            {
                const bsl::shared_ptr<ntci::Reactor>& reactor = *it;
                bsl::size_t                           load = reactor->load();
                if (load < minimumLoad) {
                    minimumLoad      = load;
                    minimumLoadIndex = (it - d_reactorVector.begin());
                }
            }

            result = d_reactorVector[minimumLoadIndex];
        }

        BSLS_ASSERT_OPT(result);
        bsl::size_t load = result->load();
        if (load >= d_config.threadLoadFactor()) {
            NTCR_INTERFACE_LOG_LOAD_FACTOR_EXCEEDED(d_config, load);
            if (d_threadVector.size() < d_config.maxThreads()) {
                NTCR_INTERFACE_LOG_EXPANDING(d_config,
                                             d_threadVector.size() + 1);
                this->addThread();
                continue;
            }
        }

        result->incrementLoad(options);
        break;
    }

    BSLS_ASSERT(result);
    return result;
}

void Interface::interruptOne()
{
    LockGuard lock(&d_mutex);

    for (ReactorVector::iterator it  = d_reactorVector.begin();
                                 it != d_reactorVector.end();
                               ++it)
    {
        const bsl::shared_ptr<ntci::Reactor>& reactor = *it;
        reactor->interruptOne();
    }
}

void Interface::interruptAll()
{
    LockGuard lock(&d_mutex);

    for (ReactorVector::iterator it  = d_reactorVector.begin();
                                 it != d_reactorVector.end();
                               ++it)
    {
        const bsl::shared_ptr<ntci::Reactor>& reactor = *it;
        reactor->interruptAll();
    }
}

bslmt::ThreadUtil::Handle Interface::threadHandle() const
{
    return bslmt::ThreadUtil::invalidHandle();
}

bsl::size_t Interface::threadIndex() const
{
    return 0;
}

Interface::Interface(
    const ntca::InterfaceConfig&                 configuration,
    const bsl::shared_ptr<ntci::DataPool>&       dataPool,
    const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory,
    bslma::Allocator*                            basicAllocator)
: d_object("ntcr::Interface")
, d_mutex()
, d_user_sp()
, d_dataPool_sp(dataPool)
, d_resolver_sp()
, d_connectionLimiter_sp()
, d_socketMetrics_sp()
, d_chronology_sp()
, d_reactorFactory_sp(reactorFactory)
, d_reactorMetrics_sp()
, d_reactorVector(basicAllocator)
, d_threadVector(basicAllocator)
, d_threadMap(basicAllocator)
, d_threadSemaphore()
, d_threadWatermark(0)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntcs::Compat::sanitize(&d_config);

    d_user_sp.createInplace(d_allocator_p, d_allocator_p);
    d_user_sp->setDataPool(d_dataPool_sp);

    if (d_config.socketMetrics().valueOr(NTCCFG_DEFAULT_SOCKET_METRICS)) {
        bsl::shared_ptr<ntcs::Metrics> metrics;
        metrics.createInplace(d_allocator_p,
                              "transport",
                              d_config.metricName(),
                              d_allocator_p);

        d_socketMetrics_sp = metrics;

        // TODO: Register socket metrics in d_user_sp.

        ntcm::MonitorableUtil::registerMonitorable(d_socketMetrics_sp);
    }

    if (d_config.driverMetrics().valueOr(NTCCFG_DEFAULT_DRIVER_METRICS)) {
        bsl::shared_ptr<ntcs::ReactorMetrics> reactorMetrics;
        reactorMetrics.createInplace(d_allocator_p,
                                     "transport",
                                     d_config.metricName(),
                                     d_allocator_p);

        d_reactorMetrics_sp = reactorMetrics;
        d_user_sp->setReactorMetrics(d_reactorMetrics_sp);

        ntcm::MonitorableUtil::registerMonitorable(d_reactorMetrics_sp);
    }

    if (!d_config.maxConnections().isNull() &&
        d_config.maxConnections().value() > 0)
    {
        bsl::shared_ptr<ntcs::Reservation> connectionLimiter;
        connectionLimiter.createInplace(d_allocator_p,
                                        d_config.maxConnections().value());

        d_connectionLimiter_sp = connectionLimiter;
        d_user_sp->setConnectionLimiter(d_connectionLimiter_sp);
    }

    BSLS_ASSERT_OPT(!d_config.dynamicLoadBalancing().isNull());
    
    if (d_config.maxThreads() > 1 && 
        !d_config.dynamicLoadBalancing().value()) 
    {
        d_chronology_sp.createInplace(d_allocator_p, this, d_allocator_p);
        d_user_sp->setChronology(d_chronology_sp);
    }
}

Interface::~Interface()
{
    this->shutdown();
    this->linger();

    d_resolver_sp.reset();
    d_user_sp.reset();

    d_threadMap.clear();
    d_threadVector.clear();

    for (ReactorVector::iterator it = d_reactorVector.begin();
         it != d_reactorVector.end();
         ++it)
    {
        const bsl::shared_ptr<ntci::Reactor>& reactor = *it;
        reactor->clear();
        BSLS_ASSERT_OPT(reactor->empty());
        BSLS_ASSERT_OPT(reactor.use_count() == 1);
    }

    d_reactorVector.clear();

    if (d_reactorMetrics_sp) {
        ntcm::MonitorableUtil::deregisterMonitorable(d_reactorMetrics_sp);
    }

    if (d_socketMetrics_sp) {
        ntcm::MonitorableUtil::deregisterMonitorable(d_socketMetrics_sp);
    }
}

ntsa::Error Interface::start()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    bsl::shared_ptr<Interface> self = this->getSelf(this);

    ntsa::Error error;

    bsl::shared_ptr<ntci::Resolver> resolver;

    {
        LockGuard lock(&d_mutex);

        if (!d_resolver_sp) {
            BSLS_ASSERT_OPT(!d_config.resolverEnabled().isNull());
            if (d_config.resolverEnabled().value()) {
                d_resolver_sp = resolver = this->createResolver();
            }
        }
        else {
            resolver = d_resolver_sp;
        }

        BSLS_ASSERT_OPT(d_threadVector.empty());
        BSLS_ASSERT_OPT(d_threadMap.empty());

        bsl::size_t numThreadsToAdd = d_config.minThreads();
        if (numThreadsToAdd < d_threadWatermark) {
            numThreadsToAdd = d_threadWatermark;
        }

        NTCR_INTERFACE_LOG_STARTING(d_config, numThreadsToAdd);

        for (bsl::size_t threadIndex = 0; threadIndex < numThreadsToAdd;
             ++threadIndex)
        {
            error = this->addThread();
            if (error) {
                return error;
            }
        }
    }

    if (resolver) {
        error = resolver->start();
        if (error) {
            return error;
        }
    }

    NTCR_INTERFACE_LOG_STARTED(d_config);

    return ntsa::Error();
}

void Interface::shutdown()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    NTCR_INTERFACE_LOG_STOPPING(d_config);

    bsl::shared_ptr<ntci::Resolver> resolver;
    ReactorVector                   reactorVector(d_allocator_p);
    {
        LockGuard lock(&d_mutex);

        resolver      = d_resolver_sp;
        reactorVector = d_reactorVector;
    }

    if (resolver) {
        resolver->shutdown();
    }

    for (bsl::size_t i = 0; i < reactorVector.size(); ++i) {
        const bsl::shared_ptr<ntci::Reactor>& reactor = reactorVector[i];
        reactor->stop();
    }
}

void Interface::linger()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    bsl::shared_ptr<ntci::Resolver> resolver;

    ThreadVector  threadVector(d_allocator_p);
    ReactorVector reactorVector(d_allocator_p);
    {
        LockGuard lock(&d_mutex);

        resolver      = d_resolver_sp;
        threadVector  = d_threadVector;
        reactorVector = d_reactorVector;
    }

    if (resolver) {
        resolver->linger();
    }

    for (bsl::size_t i = 0; i < threadVector.size(); ++i) {
        bslmt::ThreadUtil::Handle threadHandle = threadVector[i];

        BSLS_ASSERT_OPT(threadHandle != bslmt::ThreadUtil::Handle());
        BSLS_ASSERT_OPT(threadHandle != bslmt::ThreadUtil::invalidHandle());

        void* threadStatus = 0;
        int   rc = bslmt::ThreadUtil::join(threadHandle, &threadStatus);
        BSLS_ASSERT_OPT(rc == 0);
        BSLS_ASSERT_OPT(threadStatus == 0);

        NTCI_LOG_CONTEXT_GUARD_THREAD(i);
        NTCI_LOG_TRACE("Thread has stopped");
    }

    threadVector.clear();

    for (bsl::size_t i = 0; i < reactorVector.size(); ++i) {
        const bsl::shared_ptr<ntci::Reactor>& reactor = reactorVector[i];
        reactor->restart();
    }

    reactorVector.clear();

    {
        LockGuard lock(&d_mutex);

        d_threadVector.clear();
        d_threadMap.clear();
    }

    NTCR_INTERFACE_LOG_STOPPED(d_config);
}

ntsa::Error Interface::closeAll()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ReactorVector reactorVector(d_allocator_p);
    {
        LockGuard lock(&d_mutex);
        reactorVector = d_reactorVector;
    }

    for (bsl::size_t i = 0; i < reactorVector.size(); ++i) {
        const bsl::shared_ptr<ntci::Reactor>& reactor = reactorVector[i];
        reactor->closeAll();
    }

    reactorVector.clear();

    return ntsa::Error();
}

bsl::shared_ptr<ntci::DatagramSocket> Interface::createDatagramSocket(
    const ntca::DatagramSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::DatagramSocketOptions effectiveOptions;
    ntcs::Compat::convert(&effectiveOptions, options, d_config);

    bsl::shared_ptr<ntci::Reactor> reactor =
        this->acquireReactor(effectiveOptions.loadBalancingOptions());
    BSLS_ASSERT_OPT(reactor);

    bsl::shared_ptr<ntci::ReactorPool> reactorPool = this->getSelf(this);
    BSLS_ASSERT_OPT(reactorPool);

    bsl::shared_ptr<ntcr::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator,
                                 effectiveOptions,
                                 d_resolver_sp,
                                 reactor,
                                 reactorPool,
                                 d_socketMetrics_sp,
                                 allocator);

    return datagramSocket;
}

bsl::shared_ptr<ntci::ListenerSocket> Interface::createListenerSocket(
    const ntca::ListenerSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::ListenerSocketOptions effectiveOptions;
    ntcs::Compat::convert(&effectiveOptions, options, d_config);

    bsl::shared_ptr<ntci::Reactor> reactor =
        this->acquireReactor(effectiveOptions.loadBalancingOptions());
    BSLS_ASSERT_OPT(reactor);

    bsl::shared_ptr<ntci::ReactorPool> reactorPool = this->getSelf(this);
    BSLS_ASSERT_OPT(reactorPool);

    bsl::shared_ptr<ntcr::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator,
                                 effectiveOptions,
                                 d_resolver_sp,
                                 reactor,
                                 reactorPool,
                                 d_socketMetrics_sp,
                                 allocator);

    return listenerSocket;
}

bsl::shared_ptr<ntci::StreamSocket> Interface::createStreamSocket(
    const ntca::StreamSocketOptions& options,
    bslma::Allocator*                basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::StreamSocketOptions effectiveOptions;
    ntcs::Compat::convert(&effectiveOptions, options, d_config);

    bsl::shared_ptr<ntci::Reactor> reactor =
        this->acquireReactor(effectiveOptions.loadBalancingOptions());
    BSLS_ASSERT_OPT(reactor);

    bsl::shared_ptr<ntci::ReactorPool> reactorPool = this->getSelf(this);
    BSLS_ASSERT_OPT(reactorPool);

    bsl::shared_ptr<ntcr::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator,
                               effectiveOptions,
                               d_resolver_sp,
                               reactor,
                               reactorPool,
                               d_socketMetrics_sp,
                               allocator);

    return streamSocket;
}

bsl::shared_ptr<ntci::Strand> Interface::createStrand(
    bslma::Allocator* basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    if (d_chronology_sp) {
        bsl::shared_ptr<ntcs::Strand> strand;
        strand.createInplace(allocator, d_chronology_sp, allocator);
        return strand;
    }

    ntca::LoadBalancingOptions loadBalancingOptions;
    loadBalancingOptions.setWeight(0);

    bsl::shared_ptr<ntci::Reactor> reactor =
        this->acquireReactor(loadBalancingOptions);
    BSLS_ASSERT_OPT(reactor);

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, reactor, allocator);

    return strand;
}

bsl::shared_ptr<ntci::RateLimiter> Interface::createRateLimiter(
    const ntca::RateLimiterConfig& configuration,
    bslma::Allocator*              basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::uint64_t sustainedRateLimit;
    if (!configuration.sustainedRateLimit().isNull()) {
        sustainedRateLimit = configuration.sustainedRateLimit().value();
    }
    else {
        sustainedRateLimit = bsl::numeric_limits<bsl::uint64_t>::max();
    }

    bsls::TimeInterval sustainedRateWindow;
    if (!configuration.sustainedRateWindow().isNull()) {
        sustainedRateWindow = configuration.sustainedRateWindow().value();
    }
    else {
        sustainedRateWindow = bsls::TimeInterval(1, 0);
    }

    bsl::uint64_t peakRateLimit;
    if (!configuration.peakRateLimit().isNull()) {
        peakRateLimit = configuration.peakRateLimit().value();
    }
    else {
        peakRateLimit = sustainedRateLimit;
    }

    bsls::TimeInterval peakRateWindow;
    if (!configuration.peakRateWindow().isNull()) {
        peakRateWindow = configuration.peakRateWindow().value();
    }
    else {
        peakRateWindow = sustainedRateWindow;
    }

    bsls::TimeInterval currentTime;
    if (!configuration.currentTime().isNull()) {
        currentTime = configuration.currentTime().value();
    }
    else {
        currentTime = bdlt::CurrentTime::now();
    }

    bsl::shared_ptr<ntcs::RateLimiter> rateLimiter;
    rateLimiter.createInplace(allocator,
                              sustainedRateLimit,
                              sustainedRateWindow,
                              peakRateLimit,
                              peakRateWindow,
                              currentTime);

    return rateLimiter;
}

ntsa::Error Interface::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionClient(result,
                                                    options,
                                                    basicAllocator);
}

ntsa::Error Interface::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>*         result,
    const ntca::EncryptionClientOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionClient(result,
                                                    options,
                                                    blobBufferFactory,
                                                    basicAllocator);
}

ntsa::Error Interface::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionClient(result,
                                                    options,
                                                    dataPool,
                                                    basicAllocator);
}

ntsa::Error Interface::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionServer(result,
                                                    options,
                                                    basicAllocator);
}

ntsa::Error Interface::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>*         result,
    const ntca::EncryptionServerOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionServer(result,
                                                    options,
                                                    blobBufferFactory,
                                                    basicAllocator);
}

ntsa::Error Interface::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionServer(result,
                                                    options,
                                                    dataPool,
                                                    basicAllocator);
}

ntsa::Error Interface::createEncryptionResource(
    bsl::shared_ptr<ntci::EncryptionResource>* result,
    bslma::Allocator*                          basicAllocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionResource(result, basicAllocator);
}


ntsa::Error Interface::generateCertificate(
    ntca::EncryptionCertificate*                  result,
    const ntsa::DistinguishedName&                subjectIdentity,
    const ntca::EncryptionKey&                    subjectPrivateKey,
    const ntca::EncryptionCertificateOptions&     options,
    bslma::Allocator*                             basicAllocator) 
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->generateCertificate(result,
                                                 subjectIdentity,
                                                 subjectPrivateKey,
                                                 options,
                                                 basicAllocator);
}

ntsa::Error Interface::generateCertificate(
    ntca::EncryptionCertificate*              result,
    const ntsa::DistinguishedName&            subjectIdentity,
    const ntca::EncryptionKey&                subjectPrivateKey,
    const ntca::EncryptionCertificate&        issuerCertificate,
    const ntca::EncryptionKey&                issuerPrivateKey,
    const ntca::EncryptionCertificateOptions& options,
    bslma::Allocator*                         basicAllocator) 
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->generateCertificate(result,
                                                 subjectIdentity,
                                                 subjectPrivateKey,
                                                 issuerCertificate,
                                                 issuerPrivateKey,
                                                 options,
                                                 basicAllocator);
}

ntsa::Error Interface::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const ntsa::DistinguishedName&                subjectIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&   subjectPrivateKey,
    const ntca::EncryptionCertificateOptions&     options,
    bslma::Allocator*                             basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->generateCertificate(result,
                                                 subjectIdentity,
                                                 subjectPrivateKey,
                                                 options,
                                                 basicAllocator);
}

ntsa::Error Interface::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
    const ntsa::DistinguishedName&                      subjectIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&         subjectPrivateKey,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& issuerCertificate,
    const bsl::shared_ptr<ntci::EncryptionKey>&         issuerPrivateKey,
    const ntca::EncryptionCertificateOptions&           options,
    bslma::Allocator*                                   basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->generateCertificate(result,
                                                 subjectIdentity,
                                                 subjectPrivateKey,
                                                 issuerCertificate,
                                                 issuerPrivateKey,
                                                 options,
                                                 basicAllocator);
}

ntsa::Error Interface::loadCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            path,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->loadCertificate(result,
                                             path,
                                             options,
                                             basicAllocator);
}

ntsa::Error Interface::saveCertificate(
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const bsl::string&                                  path,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->saveCertificate(certificate, path, options);
}

ntsa::Error Interface::encodeCertificate(
    bsl::streambuf*                                     destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination,
                                               certificate,
                                               options);
}

ntsa::Error Interface::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    bsl::streambuf*                               source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeCertificate(result,
                                               source,
                                               options,
                                               basicAllocator);
}

ntsa::Error Interface::generateKey(
    ntca::EncryptionKey*                  result,
    const ntca::EncryptionKeyOptions&     options,
    bslma::Allocator*                     basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->generateKey(result, options, basicAllocator);
} 


ntsa::Error Interface::generateKey(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    const ntca::EncryptionKeyOptions&     options,
    bslma::Allocator*                     basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->generateKey(result, options, basicAllocator);
}

ntsa::Error Interface::loadKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                               const bsl::string&                     path,
                               const ntca::EncryptionResourceOptions& options,
                               bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->loadKey(result, path, options, basicAllocator);
}

ntsa::Error Interface::saveKey(
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const bsl::string&                          path,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->saveKey(privateKey, path, options);
}

ntsa::Error Interface::encodeKey(
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey, options);
}

ntsa::Error Interface::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>*  result,
    bsl::streambuf*                        source,
    const ntca::EncryptionResourceOptions& options,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeKey(result,
                                       source,
                                       options,
                                       basicAllocator);
}

void Interface::execute(const Functor& functor)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    if (d_chronology_sp) {
        d_chronology_sp->execute(functor);
        return;
    }

    ntca::LoadBalancingOptions loadBalancingOptions;
    loadBalancingOptions.setWeight(0);

    bsl::shared_ptr<ntci::Reactor> reactor =
        this->acquireReactor(loadBalancingOptions);
    BSLS_ASSERT_OPT(reactor);

    reactor->execute(functor);
}

void Interface::moveAndExecute(FunctorSequence* functorSequence,
                               const Functor&   functor)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    if (d_chronology_sp) {
        d_chronology_sp->moveAndExecute(functorSequence, functor);
        return;
    }

    ntca::LoadBalancingOptions loadBalancingOptions;
    loadBalancingOptions.setWeight(0);

    bsl::shared_ptr<ntci::Reactor> reactor =
        this->acquireReactor(loadBalancingOptions);
    BSLS_ASSERT_OPT(reactor);

    reactor->moveAndExecute(functorSequence, functor);
}

bsl::shared_ptr<ntci::Timer> Interface::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    if (d_chronology_sp) {
        return d_chronology_sp->createTimer(options, session, basicAllocator);
    }

    ntca::LoadBalancingOptions loadBalancingOptions;
    loadBalancingOptions.setWeight(0);

    bsl::shared_ptr<ntci::Reactor> reactor =
        this->acquireReactor(loadBalancingOptions);
    BSLS_ASSERT_OPT(reactor);

    return reactor->createTimer(options, session, basicAllocator);
}

bsl::shared_ptr<ntci::Timer> Interface::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    if (d_chronology_sp) {
        return d_chronology_sp->createTimer(options, callback, basicAllocator);
    }

    ntca::LoadBalancingOptions loadBalancingOptions;
    loadBalancingOptions.setWeight(0);

    bsl::shared_ptr<ntci::Reactor> reactor =
        this->acquireReactor(loadBalancingOptions);
    BSLS_ASSERT_OPT(reactor);

    return reactor->createTimer(options, callback, basicAllocator);
}

bsl::shared_ptr<ntsa::Data> Interface::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> Interface::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> Interface::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> Interface::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void Interface::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createIncomingBlobBuffer(blobBuffer);
}

void Interface::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createOutgoingBlobBuffer(blobBuffer);
}

bsl::shared_ptr<ntci::Reactor> Interface::acquireReactor(
    const ntca::LoadBalancingOptions& options)
{
    bsl::shared_ptr<ntci::Reactor> result;

    if (!options.threadHandle().isNull()) {
        result = this->acquireReactorUsedByThreadHandle(options);
        if (result) {
            return result;
        }
    }

    if (!options.threadIndex().isNull()) {
        result = this->acquireReactorUsedByThreadIndex(options);
        if (result) {
            return result;
        }
    }

    return this->acquireReactorWithLeastLoad(options);
}

void Interface::releaseReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
                               const ntca::LoadBalancingOptions&     options)
{
    reactor->decrementLoad(options);
}

bool Interface::acquireHandleReservation()
{
    if (d_connectionLimiter_sp) {
        return d_connectionLimiter_sp->acquire();
    }
    else {
        return true;
    }
}

void Interface::releaseHandleReservation()
{
    if (d_connectionLimiter_sp) {
        d_connectionLimiter_sp->release();
    }
}

bool Interface::expand()
{
    LockGuard lock(&d_mutex);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().c_str());

    if (d_threadVector.size() < d_config.maxThreads()) {
        NTCR_INTERFACE_LOG_EXPANDING(d_config, d_threadVector.size() + 1);
        ntsa::Error error = this->addThread();
        if (error) {
            return false;
        }

        return true;
    }

    return false;
}

bsl::size_t Interface::numReactors() const
{
    LockGuard lock(&d_mutex);
    return d_reactorVector.size();
}

bsl::size_t Interface::numThreads() const
{
    LockGuard lock(&d_mutex);
    return d_threadVector.size();
}

bsl::size_t Interface::minThreads() const
{
    return d_config.minThreads();
}

bsl::size_t Interface::maxThreads() const
{
    return d_config.maxThreads();
}

bsls::TimeInterval Interface::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<ntci::Strand>& Interface::strand() const
{
    return ntci::Strand::unspecified();
}

bool Interface::lookupByThreadHandle(
    bsl::shared_ptr<ntci::Executor>* result,
    bslmt::ThreadUtil::Handle        threadHandle) const
{
    LockGuard lock(&d_mutex);

    result->reset();

    if (threadHandle != bslmt::ThreadUtil::Handle() &&
        threadHandle != bslmt::ThreadUtil::invalidHandle())
    {
        bslmt::ThreadUtil::Id threadId =
            bslmt::ThreadUtil::handleToId(threadHandle);

        bsl::uint64_t threadIdValue = bslmt::ThreadUtil::idAsUint64(threadId);

        ThreadMap::const_iterator it = d_threadMap.find(threadIdValue);
        if (it != d_threadMap.end()) {
            *result = it->second;
            return true;
        }
    }
    else {
        if (!d_reactorVector.empty()) {
            *result = d_reactorVector.front();
            return true;
        }
    }

    return false;
}

bool Interface::lookupByThreadIndex(bsl::shared_ptr<ntci::Executor>* result,
                                    bsl::size_t threadIndex) const
{
    LockGuard lock(&d_mutex);

    result->reset();

    if (threadIndex < d_threadVector.size()) {
        bslmt::ThreadUtil::Handle threadHandle = d_threadVector[threadIndex];

        BSLS_ASSERT_OPT(threadHandle != bslmt::ThreadUtil::Handle());
        BSLS_ASSERT_OPT(threadHandle != bslmt::ThreadUtil::invalidHandle());

        bslmt::ThreadUtil::Id threadId =
            bslmt::ThreadUtil::handleToId(threadHandle);

        bsl::uint64_t threadIdValue = bslmt::ThreadUtil::idAsUint64(threadId);

        ThreadMap::const_iterator it = d_threadMap.find(threadIdValue);
        if (it != d_threadMap.end()) {
            *result = it->second;
            return true;
        }
    }

    return false;
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Interface::
    incomingBlobBufferFactory() const
{
    return d_dataPool_sp->incomingBlobBufferFactory();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Interface::
    outgoingBlobBufferFactory() const
{
    return d_dataPool_sp->outgoingBlobBufferFactory();
}

const bsl::shared_ptr<ntci::Resolver>& Interface::resolver() const
{
    return d_resolver_sp;
}

const ntca::InterfaceConfig& Interface::configuration() const
{
    return d_config;
}

bool Interface::isSupported(const bsl::string& driverName,
                            bool               dynamicLoadBalancing)
{
    NTCCFG_WARNING_UNUSED(dynamicLoadBalancing);

    return ntcs::Plugin::supportsReactorFactory(driverName);
}

void Interface::loadSupportedDriverNames(bsl::vector<bsl::string>* driverNames,
                                         bool dynamicLoadBalancing)
{
    NTCCFG_WARNING_UNUSED(dynamicLoadBalancing);

    ntcs::Plugin::loadSupportedReactorFactoryDriverNames(driverNames);
}

}  // close package namespace
}  // close enterprise namespace
