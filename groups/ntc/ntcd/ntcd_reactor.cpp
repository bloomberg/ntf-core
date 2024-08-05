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

#include <ntcd_reactor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_reactor_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>
#include <ntci_log.h>
#include <ntcm_monitorableutil.h>
#include <ntcs_datapool.h>
#include <ntcs_nomenclature.h>
#include <ntcs_reactormetrics.h>
#include <ntcs_strand.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bdlt_localtimeoffset.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_timeutil.h>

#define NTCD_REACTOR_LOG_WAIT_INDEFINITE()                                    \
    NTCI_LOG_TRACE("Polling for socket events indefinitely")

#define NTCD_REACTOR_LOG_WAIT_TIMED(timeout)                                  \
    NTCI_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        (int)(timeout))

#define NTCD_REACTOR_LOG_WAIT_TIMED_HIGH_PRECISION(timeInterval)              \
    do {                                                                      \
        bdlt::Datetime dateTimeDue =                                          \
            bdlt::EpochUtil::convertFromTimeInterval(timeInterval);           \
        dateTimeDue.addSeconds(                                               \
            bdlt::LocalTimeOffset::localTimeOffset(bdlt::CurrentTime::utc())  \
                .totalSeconds());                                             \
        char buffer[128];                                                     \
        dateTimeDue.printToBuffer(buffer, sizeof buffer);                     \
        NTCI_LOG_TRACE("Polling for sockets events or until %s", buffer);     \
    } while (false)

#define NTCD_REACTOR_LOG_WAIT_FAILURE(error)                                  \
    NTCI_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTCD_REACTOR_LOG_WAIT_TIMEOUT()                                       \
    NTCI_LOG_TRACE("Timed out polling for socket events")

#define NTCD_REACTOR_LOG_WAIT_RESULT(numEvents)                               \
    NTCI_LOG_TRACE("Polled %zu socket events", (bsl::size_t)(numEvents))

#define NTCD_REACTOR_LOG_EVENT(event)                                         \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Polled event " << event                     \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

namespace BloombergLP {
namespace ntcd {

namespace {

// The flag that defines whether all waiters are interrupted when the polling
// device gains or loses interest in socket events.
const bool k_INTERRUPT_ALL = false;

}  // close unnamed namespace

/// This struct describes the context of a waiter.
struct Reactor::Result {
  public:
    ntca::WaiterOptions                   d_options;
    bsl::shared_ptr<ntci::ReactorMetrics> d_metrics_sp;

  private:
    Result(const Result&) BSLS_KEYWORD_DELETED;
    Result& operator=(const Result&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new reactor result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit Result(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Result();
};

Reactor::Result::Result(bslma::Allocator* basicAllocator)
: d_options(basicAllocator)
, d_metrics_sp()
{
}

Reactor::Result::~Result()
{
}

void Reactor::initialize()
{
    if (d_config.metricName().isNull() ||
        d_config.metricName().value().empty())
    {
        // MRM: Request anonymous name from ntcf::System
        d_config.setMetricName(ntcs::Nomenclature::createReactorName());
    }

    BSLS_ASSERT(!d_config.metricName().isNull());
    BSLS_ASSERT(!d_config.metricName().value().empty());

    if (d_config.minThreads().isNull() || d_config.minThreads().value() == 0) {
        d_config.setMinThreads(1);
    }

    BSLS_ASSERT(!d_config.minThreads().isNull());
    BSLS_ASSERT(d_config.minThreads().value() > 0);

    if (d_config.maxThreads().isNull() || d_config.maxThreads().value() == 0) {
        d_config.setMaxThreads(NTCCFG_DEFAULT_MAX_THREADS);
    }

    BSLS_ASSERT(!d_config.maxThreads().isNull());
    BSLS_ASSERT(d_config.maxThreads().value() > 0);

    if (d_config.maxThreads().value() > NTCCFG_DEFAULT_MAX_THREADS) {
        d_config.setMaxThreads(NTCCFG_DEFAULT_MAX_THREADS);
    }

    if (d_config.minThreads().value() > d_config.maxThreads().value()) {
        d_config.setMinThreads(d_config.maxThreads().value());
    }

    if (d_config.maxThreads().value() > 1) {
        d_dynamic = true;
    }

    BSLS_ASSERT(d_config.minThreads().value() <=
                d_config.maxThreads().value());
    BSLS_ASSERT(d_config.maxThreads().value() <= NTCCFG_DEFAULT_MAX_THREADS);

    if (d_config.maxEventsPerWait().isNull()) {
        d_config.setMaxEventsPerWait(NTCCFG_DEFAULT_MAX_EVENTS_PER_WAIT);
    }

    if (d_config.maxTimersPerWait().isNull()) {
        d_config.setMaxTimersPerWait(NTCCFG_DEFAULT_MAX_TIMERS_PER_WAIT);
    }

    if (d_config.maxCyclesPerWait().isNull()) {
        d_config.setMaxCyclesPerWait(NTCCFG_DEFAULT_MAX_CYCLES_PER_WAIT);
    }

    if (d_config.metricCollection().isNull()) {
        d_config.setMetricCollection(NTCCFG_DEFAULT_DRIVER_METRICS);
    }

    if (d_config.metricCollectionPerWaiter().isNull()) {
        d_config.setMetricCollectionPerWaiter(
            NTCCFG_DEFAULT_DRIVER_METRICS_PER_WAITER);
    }

    if (d_config.metricCollectionPerSocket().isNull()) {
        d_config.setMetricCollectionPerSocket(false);
    }

    if (d_config.autoAttach().isNull()) {
        d_config.setAutoAttach(false);
    }

    if (d_config.autoDetach().isNull()) {
        d_config.setAutoDetach(false);
    }

    if (d_config.oneShot().isNull()) {
        if (d_config.maxThreads().value() == 1) {
            d_config.setOneShot(false);
        }
        else {
            d_config.setOneShot(true);
        }
    }

    if (d_config.trigger().isNull()) {
        d_config.setTrigger(ntca::ReactorEventTrigger::e_LEVEL);
    }

    if (d_user_sp) {
        d_dataPool_sp = d_user_sp->dataPool();
    }

    if (!d_dataPool_sp) {
        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(d_allocator_p, d_allocator_p);

        d_dataPool_sp = dataPool;
    }

    d_chronology_sp.createInplace(
        d_allocator_p,
        bsl::shared_ptr<ntcs::Driver>(this,
                                      bslstl::SharedPtrNilDeleter(),
                                      d_allocator_p),
        d_allocator_p);

    if (d_user_sp) {
        d_resolver_sp = d_user_sp->resolver();
    }

    if (!d_resolver_sp) {
        d_resolver_sp = d_machine_sp->resolver();
    }

    if (d_user_sp) {
        d_connectionLimiter_sp = d_user_sp->connectionLimiter();
    }

    if (d_user_sp) {
        d_metrics_sp = d_user_sp->reactorMetrics();
    }

    if (d_user_sp) {
        bsl::shared_ptr<ntci::Chronology> chronology = d_user_sp->chronology();
        if (chronology) {
            d_chronology_sp->setParent(chronology);
        }
    }

    d_monitor_sp = d_machine_sp->createMonitor(d_allocator_p);

    d_monitor_sp->setTrigger(d_config.trigger().value());
    d_monitor_sp->setOneShot(d_config.oneShot().value());

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
    d_detachFunctor_sp.createInplace(d_allocator_p, [this](const auto& entry) {
        return this->removeDetached(entry);
    });
#else
    d_detachFunctor_sp.createInplace(d_allocator_p,
                                     NTCCFG_BIND(&Reactor::removeDetached,
                                                 this,
                                                 NTCCFG_BIND_PLACEHOLDER_1));

#endif

    d_registry_sp.createInplace(d_allocator_p,
                                d_config.trigger().value(),
                                d_config.oneShot().value(),
                                d_allocator_p);
}

void Reactor::flush()
{
    while (d_chronology_sp->hasAnyScheduledOrDeferred()) {
        d_chronology_sp->announce(d_dynamic);
    }
}

ntsa::Error Reactor::add(ntsa::Handle handle, ntcs::Interest interest)
{
    ntsa::Error error;

    error = d_monitor_sp->add(handle);
    if (error) {
        return error;
    }

    error = d_monitor_sp->update(handle, interest);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Reactor::update(ntsa::Handle   handle,
                            ntcs::Interest interest,
                            UpdateType     type)
{
    NTCCFG_WARNING_UNUSED(type);

    return d_monitor_sp->update(handle, interest);
}

ntsa::Error Reactor::remove(ntsa::Handle handle)
{
    return d_monitor_sp->remove(handle);
}

ntsa::Error Reactor::removeDetached(
    const bsl::shared_ptr<ntcs::RegistryEntry>& entry)
{
    d_monitor_sp->remove(entry->handle());
    if (!entry->isProcessing() && entry->announceDetached(this->getSelf(this)))
    {
        entry->clear();
        Reactor::interruptOne();
    }
    return ntsa::Error();
}

bsl::shared_ptr<ntci::Reactor> Reactor::acquireReactor(
    const ntca::LoadBalancingOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);
    return this->getSelf(this);
}

void Reactor::releaseReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
                             const ntca::LoadBalancingOptions&     options)
{
    BSLS_ASSERT(reactor == this->getSelf(this));
    reactor->decrementLoad(options);
}

bool Reactor::acquireHandleReservation()
{
    if (d_connectionLimiter_sp) {
        return d_connectionLimiter_sp->acquire();
    }
    else {
        return true;
    }
}

void Reactor::releaseHandleReservation()
{
    if (d_connectionLimiter_sp) {
        d_connectionLimiter_sp->release();
    }
}

bsl::size_t Reactor::numReactors() const
{
    return 1;
}

bsl::size_t Reactor::numThreads() const
{
    return this->numWaiters();
}

bsl::size_t Reactor::minThreads() const
{
    return d_config.minThreads().value();
}

bsl::size_t Reactor::maxThreads() const
{
    return d_config.maxThreads().value();
}

Reactor::Reactor(const ntca::ReactorConfig&         configuration,
                 const bsl::shared_ptr<ntci::User>& user,
                 bslma::Allocator*                  basicAllocator)
: d_machine_sp(ntcd::Machine::getDefault())
, d_monitor_sp()
, d_user_sp(user)
, d_dataPool_sp()
, d_resolver_sp()
, d_chronology_sp()
, d_connectionLimiter_sp()
, d_metrics_sp()
, d_datagramSocketFactory_sp()
, d_listenerSocketFactory_sp()
, d_streamSocketFactory_sp()
, d_detachFunctor_sp()
, d_registry_sp()
, d_waiterSetMutex()
, d_waiterSet(basicAllocator)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_threadIndex(0)
, d_dynamic(false)
, d_load(0)
, d_run(true)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);

    this->initialize();
}

Reactor::Reactor(const ntca::ReactorConfig&            configuration,
                 const bsl::shared_ptr<ntci::User>&    user,
                 const bsl::shared_ptr<ntcd::Machine>& machine,
                 bslma::Allocator*                     basicAllocator)
: d_machine_sp(machine)
, d_monitor_sp()
, d_user_sp(user)
, d_dataPool_sp()
, d_resolver_sp()
, d_chronology_sp()
, d_connectionLimiter_sp()
, d_metrics_sp()
, d_datagramSocketFactory_sp()
, d_listenerSocketFactory_sp()
, d_streamSocketFactory_sp()
, d_detachFunctor_sp()
, d_registry_sp()
, d_waiterSetMutex()
, d_waiterSet(basicAllocator)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_threadIndex(0)
, d_load(0)
, d_run(true)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_machine_sp);

    this->initialize();
}

Reactor::~Reactor()
{
    // Assert all timers and functions are executed.

    BSLS_ASSERT_OPT(!d_chronology_sp->hasAnyDeferred());
    BSLS_ASSERT_OPT(!d_chronology_sp->hasAnyScheduled());
    BSLS_ASSERT_OPT(!d_chronology_sp->hasAnyRegistered());

    // Assert all waiters are deregistered.

    BSLS_ASSERT_OPT(d_waiterSet.empty());

    // Reset internal mechanisms.

    d_registry_sp.reset();
    d_streamSocketFactory_sp.reset();
    d_listenerSocketFactory_sp.reset();
    d_datagramSocketFactory_sp.reset();
    d_metrics_sp.reset();
    d_connectionLimiter_sp.reset();
    d_chronology_sp.reset();
    d_resolver_sp.reset();
    d_dataPool_sp.reset();
    d_user_sp.reset();
    d_monitor_sp.reset();
    d_machine_sp.reset();
}

void Reactor::setDatagramSocketFactory(
    const bsl::shared_ptr<ntci::DatagramSocketFactory>& datagramSocketFactory)
{
    d_datagramSocketFactory_sp = datagramSocketFactory;
}

void Reactor::setDatagramSocketFactory(
    const DatagramSocketFactoryFunction& datagramSocketFactory)
{
    bsl::shared_ptr<ntcd::DatagramSocketFactory> object;
    object.createInplace(d_allocator_p, datagramSocketFactory, d_allocator_p);

    d_datagramSocketFactory_sp = object;
}

void Reactor::setListenerSocketFactory(
    const bsl::shared_ptr<ntci::ListenerSocketFactory>& listenerSocketFactory)
{
    d_listenerSocketFactory_sp = listenerSocketFactory;
}

void Reactor::setListenerSocketFactory(
    const ListenerSocketFactoryFunction& listenerSocketFactory)
{
    bsl::shared_ptr<ntcd::ListenerSocketFactory> object;
    object.createInplace(d_allocator_p, listenerSocketFactory, d_allocator_p);

    d_listenerSocketFactory_sp = object;
}

void Reactor::setStreamSocketFactory(
    const bsl::shared_ptr<ntci::StreamSocketFactory>& streamSocketFactory)
{
    d_streamSocketFactory_sp = streamSocketFactory;
}

void Reactor::setStreamSocketFactory(
    const StreamSocketFactoryFunction& streamSocketFactory)
{
    bsl::shared_ptr<ntcd::StreamSocketFactory> object;
    object.createInplace(d_allocator_p, streamSocketFactory, d_allocator_p);

    d_streamSocketFactory_sp = object;
}

ntci::Waiter Reactor::registerWaiter(const ntca::WaiterOptions& waiterOptions)
{
    Reactor::Result* result =
        new (*d_allocator_p) Reactor::Result(d_allocator_p);

    result->d_options = waiterOptions;

    {
        LockGuard lockGuard(&d_waiterSetMutex);

        if (result->d_options.threadHandle() == bslmt::ThreadUtil::Handle()) {
            result->d_options.setThreadHandle(bslmt::ThreadUtil::self());
        }

        if (d_waiterSet.empty()) {
            d_threadHandle = result->d_options.threadHandle();
            if (!result->d_options.threadIndex().isNull()) {
                d_threadIndex = result->d_options.threadIndex().value();
            }
        }

        if (!d_config.metricCollection().isNull() &&
            d_config.metricCollection().value())
        {
            if (!d_config.metricCollectionPerWaiter().isNull() &&
                d_config.metricCollectionPerWaiter().value())
            {
                if (result->d_options.metricName().empty()) {
                    bsl::stringstream ss;
                    ss << d_config.metricName().value() << "-"
                       << d_waiterSet.size();
                    result->d_options.setMetricName(ss.str());
                }

                bsl::shared_ptr<ntcs::ReactorMetrics> metrics;
                metrics.createInplace(d_allocator_p,
                                      "thread",
                                      result->d_options.metricName(),
                                      d_metrics_sp,
                                      d_allocator_p);

                ntcm::MonitorableUtil::registerMonitorable(
                    result->d_metrics_sp);
            }
            else {
                result->d_metrics_sp = d_metrics_sp;
            }
        }

        d_waiterSet.insert(result);
    }

    d_monitor_sp->registerWaiter();

    return result;
}

void Reactor::deregisterWaiter(ntci::Waiter waiter)
{
    Reactor::Result* result = static_cast<Reactor::Result*>(waiter);

    bool flush = false;

    {
        LockGuard lockGuard(&d_waiterSetMutex);

        bsl::size_t n = d_waiterSet.erase(result);
        BSLS_ASSERT_OPT(n == 1);

        if (d_waiterSet.empty()) {
            d_threadHandle = bslmt::ThreadUtil::Handle();
            flush          = true;
        }
    }

    if (flush) {
        this->flush();
    }

    if (!d_config.metricCollection().isNull() &&
        d_config.metricCollection().value())
    {
        if (!d_config.metricCollectionPerWaiter().isNull() &&
            d_config.metricCollectionPerWaiter().value())
        {
            ntcm::MonitorableUtil::deregisterMonitorable(result->d_metrics_sp);
        }
    }

    d_monitor_sp->deregisterWaiter();

    d_allocator_p->deleteObject(result);
}

bsl::shared_ptr<ntci::Strand> Reactor::createStrand(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<Reactor> self = this->getSelf(this);

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, self, allocator);

    return strand;
}

ntsa::Error Reactor::attachSocket(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry = d_registry_sp->add(socket);
    return this->add(entry->handle(), entry->interest());
}

ntsa::Error Reactor::attachSocket(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry = d_registry_sp->add(handle);
    return this->add(handle, entry->interest());
}

ntsa::Error Reactor::showReadable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntca::ReactorEventOptions&            options)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntcs::Interest interest = entry->showReadable(options);

        error = this->update(entry->handle(), interest, e_INCLUDE);
        if (error) {
            return error;
        }
        if (k_INTERRUPT_ALL) {
            this->interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry_sp->add(socket);

            ntcs::Interest interest = entry->showReadable(options);

            error = this->add(entry->handle(), interest);
            if (error) {
                return error;
            }
            if (k_INTERRUPT_ALL) {
                this->interruptAll();
            }
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Reactor::showReadable(ntsa::Handle                      handle,
                                  const ntca::ReactorEventOptions&  options,
                                  const ntci::ReactorEventCallback& callback)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry_sp->lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntcs::Interest interest =
            entry->showReadableCallback(options, callback);

        error = this->update(handle, interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (k_INTERRUPT_ALL) {
            this->interruptAll();
        }

        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry_sp->add(handle);

            ntcs::Interest interest =
                entry->showReadableCallback(options, callback);

            error = this->add(handle, interest);
            if (error) {
                return error;
            }

            if (k_INTERRUPT_ALL) {
                this->interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Reactor::showWritable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntca::ReactorEventOptions&            options)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntcs::Interest interest = entry->showWritable(options);

        error = this->update(entry->handle(), interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (k_INTERRUPT_ALL) {
            this->interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry_sp->add(socket);

            ntcs::Interest interest = entry->showWritable(options);

            error = this->add(entry->handle(), interest);
            if (error) {
                return error;
            }

            if (k_INTERRUPT_ALL) {
                this->interruptAll();
            }
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Reactor::showWritable(ntsa::Handle                      handle,
                                  const ntca::ReactorEventOptions&  options,
                                  const ntci::ReactorEventCallback& callback)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry_sp->lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntcs::Interest interest =
            entry->showWritableCallback(options, callback);

        error = this->update(handle, interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (k_INTERRUPT_ALL) {
            this->interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry_sp->add(handle);

            ntcs::Interest interest =
                entry->showWritableCallback(options, callback);

            error = this->add(handle, interest);
            if (error) {
                return error;
            }

            if (k_INTERRUPT_ALL) {
                this->interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Reactor::showError(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntca::ReactorEventOptions&            options)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntcs::Interest interest = entry->showError(options);

        error = this->update(entry->handle(), interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (k_INTERRUPT_ALL) {
            this->interruptAll();
        }
        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry_sp->add(socket);

            ntcs::Interest interest = entry->showError(options);

            error = this->add(entry->handle(), interest);
            if (error) {
                return error;
            }

            if (k_INTERRUPT_ALL) {
                this->interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Reactor::showError(ntsa::Handle                      handle,
                               const ntca::ReactorEventOptions&  options,
                               const ntci::ReactorEventCallback& callback)
{
    ntsa::Error error;

    if (!options.trigger().isNull()) {
        if (options.trigger().value() == ntca::ReactorEventTrigger::e_EDGE) {
            if (!this->supportsTrigger(options.trigger().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    if (!options.oneShot().isNull()) {
        if (options.oneShot().value()) {
            if (!this->supportsOneShot(options.oneShot().value())) {
                return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
            }
        }
    }

    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry_sp->lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntcs::Interest interest = entry->showErrorCallback(options, callback);

        error = this->update(handle, interest, e_INCLUDE);
        if (error) {
            return error;
        }

        if (k_INTERRUPT_ALL) {
            this->interruptAll();
        }

        return ntsa::Error();
    }
    else {
        if (d_config.autoAttach().value()) {
            entry = d_registry_sp->add(handle);
            ntcs::Interest interest =
                entry->showErrorCallback(options, callback);

            error = this->add(handle, interest);
            if (error) {
                return error;
            }

            if (k_INTERRUPT_ALL) {
                this->interruptAll();
            }

            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

ntsa::Error Reactor::hideReadable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest            interest = entry->hideReadable(options);
        if (!d_config.autoDetach().value()) {
            return this->update(entry->handle(), interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(entry->handle(), interest, e_EXCLUDE);
            }
            else {
                d_registry_sp->remove(socket);
                return this->remove(entry->handle());
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Reactor::hideReadable(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry_sp->lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest interest = entry->hideReadableCallback(options);
        if (!d_config.autoDetach().value()) {
            return this->update(handle, interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(handle, interest, e_EXCLUDE);
            }
            else {
                d_registry_sp->remove(handle);
                return this->remove(handle);
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Reactor::hideWritable(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest            interest = entry->hideWritable(options);
        if (!d_config.autoDetach().value()) {
            return this->update(entry->handle(), interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(entry->handle(), interest, e_EXCLUDE);
            }
            else {
                d_registry_sp->remove(socket);
                return this->remove(entry->handle());
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Reactor::hideWritable(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry_sp->lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest interest = entry->hideWritableCallback(options);
        if (!d_config.autoDetach().value()) {
            return this->update(handle, interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(handle, interest, e_EXCLUDE);
            }
            else {
                d_registry_sp->remove(handle);
                return this->remove(handle);
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Reactor::hideError(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry =
        bslstl::SharedPtrUtil::staticCast<ntcs::RegistryEntry>(
            socket->getReactorContext());

    if (NTCCFG_LIKELY(entry)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest            interest = entry->hideError(options);
        if (!d_config.autoDetach().value()) {
            return this->update(entry->handle(), interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(entry->handle(), interest, e_EXCLUDE);
            }
            else {
                d_registry_sp->remove(socket);
                return this->remove(entry->handle());
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Reactor::hideError(ntsa::Handle handle)
{
    bsl::shared_ptr<ntcs::RegistryEntry> entry;
    bool found = d_registry_sp->lookup(&entry, handle);
    if (NTCCFG_LIKELY(found)) {
        ntca::ReactorEventOptions options;
        ntcs::Interest            interest = entry->hideErrorCallback(options);
        if (!d_config.autoDetach().value()) {
            return this->update(handle, interest, e_EXCLUDE);
        }
        else {
            if (interest.wantReadableOrWritable()) {
                return this->update(handle, interest, e_EXCLUDE);
            }
            else {
                d_registry_sp->remove(handle);
                return this->remove(handle);
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error Reactor::detachSocket(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcs::RegistryEntry> entry = d_registry_sp->remove(socket);

    if (NTCCFG_LIKELY(entry)) {
        error = this->remove(entry->handle());
        if (error) {
            return error;
        }
        if (k_INTERRUPT_ALL) {
            this->interruptAll();
        }
        return ntsa::Error();
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error Reactor::detachSocket(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket,
    const ntci::SocketDetachedCallback&         callback)
{
    BSLS_ASSERT((d_config.maxThreads().value() > 1) ==
                static_cast<bool>(callback.strand()));
    const ntsa::Error error =
        d_registry_sp->removeAndGetReadyToDetach(socket,
                                                 callback,
                                                 *d_detachFunctor_sp);
    return error;
}

ntsa::Error Reactor::detachSocket(ntsa::Handle handle)
{
    ntsa::Error error;

    bsl::shared_ptr<ntcs::RegistryEntry> entry = d_registry_sp->remove(handle);

    if (NTCCFG_LIKELY(entry)) {
        error = this->remove(handle);
        if (error) {
            return error;
        }

        if (k_INTERRUPT_ALL) {
            this->interruptAll();
        }

        return ntsa::Error();
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error Reactor::detachSocket(ntsa::Handle                        handle,
                                  const ntci::SocketDetachedCallback& callback)
{
    BSLS_ASSERT((d_config.maxThreads().value() > 1) ==
                static_cast<bool>(callback.strand()));
    const ntsa::Error error =
        d_registry_sp->removeAndGetReadyToDetach(handle,
                                                 callback,
                                                 *d_detachFunctor_sp);
    return error;
}

ntsa::Error Reactor::closeAll()
{
    d_chronology_sp->closeAll();
    d_registry_sp->closeAll(ntsa::k_INVALID_HANDLE);
    return ntsa::Error();
}

void Reactor::incrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load += weight;
}

void Reactor::decrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load -= weight;
}

void Reactor::run(ntci::Waiter waiter)
{
    while (d_run) {
        this->poll(waiter);
    }
}

void Reactor::poll(ntci::Waiter waiter)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    Reactor::Result* result = static_cast<Reactor::Result*>(waiter);
    NTCCFG_WARNING_UNUSED(result);

    BSLS_ASSERT(result->d_options.threadHandle() == bslmt::ThreadUtil::self());

    NTCS_METRICS_GET();

    // Block until a socket is readable, writable, has an error, or any timer
    // expires.

    bsl::vector<ntca::ReactorEvent> events;

    bdlb::NullableValue<bsls::TimeInterval> earliestTimerDue =
        d_chronology_sp->earliest();

    if (!earliestTimerDue.isNull()) {
        NTCD_REACTOR_LOG_WAIT_TIMED_HIGH_PRECISION(earliestTimerDue.value());

        error = d_monitor_sp->dequeue(&events, earliestTimerDue.value());
    }
    else {
        NTCD_REACTOR_LOG_WAIT_INDEFINITE();

        error = d_monitor_sp->dequeue(&events);
    }

    bsl::size_t numResults = 0;

    if (error) {
        if (error == ntsa::Error::e_WOULD_BLOCK) {
            NTCD_REACTOR_LOG_WAIT_TIMEOUT();
            NTCS_METRICS_UPDATE_POLL(0, 0, 0);
        }
        else {
            NTCD_REACTOR_LOG_WAIT_FAILURE(error);
        }
    }
    else {
        numResults = events.size();
        NTCD_REACTOR_LOG_WAIT_RESULT(numResults);
    }

    if (numResults > 0) {
        bsl::size_t numReadable    = 0;
        bsl::size_t numWritable    = 0;
        bsl::size_t numErrors      = 0;
        bsl::size_t numTimers      = 0;
        bsl::size_t numDetachments = 0;

        for (bsl::size_t i = 0; i < numResults; ++i) {
            const ntca::ReactorEvent& event = events[i];

            ntsa::Handle descriptorHandle = event.handle();

            bsl::shared_ptr<ntcs::RegistryEntry> entry;
            if (!d_registry_sp->lookupAndMarkProcessingOngoing(
                    &entry,
                    descriptorHandle))
            {
                continue;
            }

            NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(descriptorHandle);

            NTCD_REACTOR_LOG_EVENT(event);
            bool fatalError = false;
            if (event.isError()) {
                if (event.error()) {
                    fatalError = true;
                    NTCS_METRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN();
                    if (entry->announceError(event)) {
                        ++numErrors;
                    }
                    NTCS_METRICS_UPDATE_ERROR_CALLBACK_TIME_END();
                }
                else {
                    bsl::weak_ptr<ntcd::Session> session_wp;
                    ntsa::Error                  error =
                        d_machine_sp->lookupSession(&session_wp,
                                                    descriptorHandle);
                    if (!error) {
                        bsl::shared_ptr<ntcd::Session> session_sp =
                            session_wp.lock();
                        if (session_sp) {
                            bdlma::LocalSequentialAllocator<
                                ntsa::NotificationQueue::
                                    k_NUM_BYTES_TO_ALLOCATE>
                                                    lsa(d_allocator_p);
                            ntsa::NotificationQueue notifications(
                                event.handle(),
                                &lsa);

                            error = session_sp->receiveNotifications(
                                &notifications);
                            if (!error) {
                                NTCS_METRICS_UPDATE_ERROR_CALLBACK_TIME_BEGIN();
                                if (entry->announceNotifications(
                                        notifications))
                                {
                                    ++numErrors;
                                }
                                NTCS_METRICS_UPDATE_ERROR_CALLBACK_TIME_END();
                            }
                        }
                    }
                }
            }
            if (!fatalError) {
                if (event.isWritable()) {
                    NTCS_METRICS_UPDATE_WRITE_CALLBACK_TIME_BEGIN();
                    if (entry->announceWritable(event)) {
                        ++numWritable;
                    }
                    NTCS_METRICS_UPDATE_WRITE_CALLBACK_TIME_END();
                }
                else if (event.isReadable()) {
                    NTCS_METRICS_UPDATE_READ_CALLBACK_TIME_BEGIN();
                    if (entry->announceReadable(event)) {
                        ++numReadable;
                    }
                    NTCS_METRICS_UPDATE_READ_CALLBACK_TIME_END();
                }
            }

            if (entry->decrementProcessCounter() == 0 &&
                entry->announceDetached(this->getSelf(this)))
            {
                entry->clear();
                ++numDetachments;
            }
        }

        const bsl::size_t numTotal =
            numReadable + numWritable + numErrors + numTimers + numDetachments;

        if (NTCCFG_UNLIKELY(numTotal == 0)) {
            NTCS_METRICS_UPDATE_SPURIOUS_WAKEUP();
            bslmt::ThreadUtil::yield();
        }
        else {
            NTCS_METRICS_UPDATE_POLL(numReadable, numWritable, numErrors);
        }
    }

    // Invoke functions deferred while processing each polled event and process
    // all expired timers.

    bsl::size_t numCycles = d_config.maxCyclesPerWait().value();
    while (numCycles != 0) {
        if (d_chronology_sp->hasAnyScheduledOrDeferred()) {
            d_chronology_sp->announce(d_dynamic);
            --numCycles;
        }
        else {
            break;
        }
    }
}

void Reactor::interruptOne()
{
    d_monitor_sp->interruptOne();
}

void Reactor::interruptAll()
{
    d_monitor_sp->interruptAll();
}

void Reactor::stop()
{
    d_run = false;
    d_monitor_sp->stop();
}

void Reactor::restart()
{
    d_run = true;
    d_monitor_sp->restart();
}

void Reactor::drainFunctions()
{
    d_chronology_sp->drain();
}

void Reactor::clearFunctions()
{
    d_chronology_sp->clearFunctions();
}

void Reactor::clearTimers()
{
    d_chronology_sp->clearTimers();
}

void Reactor::clearSockets()
{
    bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> > entryList;
    d_registry_sp->clear(&entryList, ntsa::k_INVALID_HANDLE);

    for (bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> >::iterator it =
             entryList.begin();
         it != entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry = *it;
        this->remove(entry->handle());
    }

    entryList.clear();
}

void Reactor::clear()
{
    d_chronology_sp->clear();

    bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> > entryList;
    d_registry_sp->clear(&entryList, ntsa::k_INVALID_HANDLE);

    for (bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> >::iterator it =
             entryList.begin();
         it != entryList.end();
         ++it)
    {
        const bsl::shared_ptr<ntcs::RegistryEntry>& entry = *it;
        this->remove(entry->handle());
    }

    entryList.clear();
}

void Reactor::execute(const Functor& functor)
{
    d_chronology_sp->execute(functor);
}

void Reactor::moveAndExecute(FunctorSequence* functorSequence,
                             const Functor&   functor)
{
    d_chronology_sp->moveAndExecute(functorSequence, functor);
}

bsl::shared_ptr<ntci::Timer> Reactor::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return d_chronology_sp->createTimer(options, session, basicAllocator);
}

bsl::shared_ptr<ntci::Timer> Reactor::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return d_chronology_sp->createTimer(options, callback, basicAllocator);
}

bsl::shared_ptr<ntci::DatagramSocket> Reactor::createDatagramSocket(
    const ntca::DatagramSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    return d_datagramSocketFactory_sp->createDatagramSocket(options,
                                                            basicAllocator);
}

bsl::shared_ptr<ntci::ListenerSocket> Reactor::createListenerSocket(
    const ntca::ListenerSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    return d_listenerSocketFactory_sp->createListenerSocket(options,
                                                            basicAllocator);
}

bsl::shared_ptr<ntci::StreamSocket> Reactor::createStreamSocket(
    const ntca::StreamSocketOptions& options,
    bslma::Allocator*                basicAllocator)
{
    return d_streamSocketFactory_sp->createStreamSocket(options,
                                                        basicAllocator);
}

bsl::shared_ptr<ntsa::Data> Reactor::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> Reactor::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> Reactor::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> Reactor::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void Reactor::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createIncomingBlobBuffer(blobBuffer);
}

void Reactor::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createOutgoingBlobBuffer(blobBuffer);
}

bsl::size_t Reactor::numSockets() const
{
    return d_registry_sp->size();
}

bsl::size_t Reactor::maxSockets() const
{
    return static_cast<bsl::size_t>(-1);
}

bsl::size_t Reactor::numTimers() const
{
    return d_chronology_sp->numScheduled();
}

bsl::size_t Reactor::maxTimers() const
{
    return static_cast<bsl::size_t>(-1);
}

bool Reactor::autoAttach() const
{
    return d_config.autoAttach().value();
}

bool Reactor::autoDetach() const
{
    return d_config.autoDetach().value();
}

bool Reactor::oneShot() const
{
    return d_config.oneShot().value();
}

ntca::ReactorEventTrigger::Value Reactor::trigger() const
{
    return d_config.trigger().value();
}

bsl::size_t Reactor::load() const
{
    return static_cast<bsl::size_t>(d_load);
}

bslmt::ThreadUtil::Handle Reactor::threadHandle() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadHandle;
}

bsl::size_t Reactor::threadIndex() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadIndex;
}

bsl::size_t Reactor::numWaiters() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_waiterSet.size();
}

bool Reactor::empty() const
{
    if (d_chronology_sp->hasAnyScheduledOrDeferred()) {
        return false;
    }

    if (d_chronology_sp->hasAnyRegistered()) {
        return false;
    }

    if (this->numSockets() != 0) {
        return false;
    }

    return true;
}

const bsl::shared_ptr<ntci::DataPool>& Reactor::dataPool() const
{
    return d_dataPool_sp;
}

bool Reactor::supportsOneShot(bool oneShot) const
{
    return d_monitor_sp->supportsOneShot(oneShot);
}

bool Reactor::supportsTrigger(ntca::ReactorEventTrigger::Value trigger) const
{
    return d_monitor_sp->supportsTrigger(trigger);
}

bool Reactor::supportsNotifications() const
{
    return true;
}

const bsl::shared_ptr<ntci::Strand>& Reactor::strand() const
{
    return ntci::Strand::unspecified();
}

bsls::TimeInterval Reactor::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Reactor::
    incomingBlobBufferFactory() const
{
    return d_dataPool_sp->incomingBlobBufferFactory();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& Reactor::
    outgoingBlobBufferFactory() const
{
    return d_dataPool_sp->outgoingBlobBufferFactory();
}

const char* Reactor::name() const
{
    return "SIMULATION";
}

ReactorFactory::ReactorFactory(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ReactorFactory::~ReactorFactory()
{
}

bsl::shared_ptr<ntci::Reactor> ReactorFactory::createReactor(
    const ntca::ReactorConfig&         configuration,
    const bsl::shared_ptr<ntci::User>& user,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::Reactor> reactor;
    reactor.createInplace(allocator, configuration, user, allocator);

    return reactor;
}

}  // close package namespace
}  // close enterprise namespace
