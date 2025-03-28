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

#include <ntcr_thread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcr_thread_cpp, "$Id$ $CSID$")

#include <ntci_log.h>

#include <ntccfg_limits.h>
#include <ntcdns_resolver.h>
#include <ntcs_monitorable.h>
#include <ntcr_datagramsocket.h>
#include <ntcr_listenersocket.h>
#include <ntcr_streamsocket.h>
#include <ntcs_datapool.h>
#include <ntcs_metrics.h>
#include <ntcs_nomenclature.h>
#include <ntcs_reactormetrics.h>
#include <ntcs_threadutil.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlt_currenttime.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_platform.h>

#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntcr {

void* Thread::run(ntcr::Thread* thread)
{
    if (!thread->d_threadAttributes.threadName().empty()) {
        bslmt::ThreadUtil::setThreadName(
            thread->d_threadAttributes.threadName());
    }

    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(
        thread->d_config.metricName().value().c_str());

    ntca::WaiterOptions waiterOptions;

    ntci::Waiter waiter = thread->d_reactor_sp->registerWaiter(waiterOptions);

    NTCI_LOG_TRACE("Thread '%s' has started",
                   thread->d_config.threadName().value().c_str());

    {
        ntccfg::ConditionMutexGuard guard(&thread->d_runMutex);
        thread->d_runState = k_RUN_STATE_STARTED;
        thread->d_runCondition.signal();
    }

    thread->d_reactor_sp->run(waiter);
    thread->d_reactor_sp->drainFunctions();
    thread->d_reactor_sp->deregisterWaiter(waiter);

    return 0;
}

void Thread::initialize()
{
    if (d_config.metricName().isNull() ||
        d_config.metricName().value().empty())
    {
        if (!d_config.threadName().isNull()) {
            d_config.setMetricName(d_config.threadName().value());
        }
    }

    if (d_config.threadName().isNull() ||
        d_config.threadName().value().empty())
    {
        if (!d_config.metricName().isNull()) {
            d_config.setThreadName(d_config.metricName().value());
        }
    }

    if (d_config.metricName().isNull() ||
        d_config.metricName().value().empty())
    {
        d_config.setMetricName(ntcs::Nomenclature::createThreadName());
    }

    BSLS_ASSERT_OPT(!d_config.metricName().isNull());
    BSLS_ASSERT_OPT(!d_config.metricName().value().empty());

    if (d_config.threadName().isNull() ||
        d_config.threadName().value().empty())
    {
        d_config.setThreadName(d_config.metricName().value());
    }

    BSLS_ASSERT_OPT(!d_config.threadName().isNull());
    BSLS_ASSERT_OPT(!d_config.threadName().value().empty());

    if (d_config.metricCollection().isNull()) {
        d_config.setMetricCollection(NTCCFG_DEFAULT_DRIVER_METRICS);
    }

    if (d_config.metricCollectionPerWaiter().isNull()) {
        d_config.setMetricCollectionPerWaiter(
            NTCCFG_DEFAULT_DRIVER_METRICS_PER_WAITER);
    }

    if (d_config.metricCollectionPerSocket().isNull()) {
        d_config.setMetricCollectionPerSocket(
            NTCCFG_DEFAULT_SOCKET_METRICS_PER_HANDLE);
    }

    if (!d_config.metricCollectionPerWaiter().isNull() &&
        d_config.metricCollectionPerWaiter().value())
    {
        d_config.setMetricCollection(true);
    }

    if (!d_config.metricCollectionPerSocket().isNull() &&
        d_config.metricCollectionPerSocket().value())
    {
        d_config.setMetricCollection(true);
    }

    if (d_config.resolverEnabled().isNull()) {
        d_config.setResolverEnabled(NTCCFG_DEFAULT_RESOLVER_ENABLED);
    }

    if (d_config.resolverEnabled().value()) {
        if (d_config.resolverConfig().isNull()) {
            ntca::ResolverConfig resolverConfig;
            d_config.setResolverConfig(resolverConfig);
        }
    }
}

Thread::Thread(const ntca::ThreadConfig&                    configuration,
               const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory,
               bslma::Allocator*                            basicAllocator)
: d_object("ntcr::Thread")
, d_reactor_sp()
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_threadAttributes(basicAllocator)
, d_runMutex()
, d_runCondition()
, d_runState(k_RUN_STATE_STOPPED)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->initialize();

    ntca::ReactorConfig reactorConfig;

    if (!d_config.driverName().isNull()) {
        reactorConfig.setDriverName(d_config.driverName().value());
    }

    reactorConfig.setMetricName(d_config.metricName().value());

    reactorConfig.setMinThreads(1);
    reactorConfig.setMaxThreads(1);

    if (!d_config.maxEventsPerWait().isNull()) {
        reactorConfig.setMaxEventsPerWait(d_config.maxEventsPerWait().value());
    }

    if (!d_config.maxTimersPerWait().isNull()) {
        reactorConfig.setMaxTimersPerWait(d_config.maxTimersPerWait().value());
    }

    if (!d_config.maxCyclesPerWait().isNull()) {
        reactorConfig.setMaxCyclesPerWait(d_config.maxCyclesPerWait().value());
    }

    if (!d_config.metricCollection().isNull()) {
        reactorConfig.setMetricCollection(d_config.metricCollection().value());
    }

    if (!d_config.metricCollectionPerWaiter().isNull()) {
        reactorConfig.setMetricCollectionPerWaiter(
            d_config.metricCollectionPerWaiter().value());
    }

    if (!d_config.metricCollectionPerSocket().isNull()) {
        reactorConfig.setMetricCollectionPerSocket(
            d_config.metricCollectionPerSocket().value());
    }

    reactorConfig.setAutoAttach(false);
    reactorConfig.setAutoDetach(false);

    reactorConfig.setTrigger(ntca::ReactorEventTrigger::e_LEVEL);
    reactorConfig.setOneShot(false);

    d_reactor_sp = reactorFactory->createReactor(reactorConfig,
                                                 bsl::shared_ptr<ntci::User>(),
                                                 d_allocator_p);
}

Thread::Thread(const ntca::ThreadConfig&             configuration,
               const bsl::shared_ptr<ntci::Reactor>& reactor,
               bslma::Allocator*                     basicAllocator)
: d_object("ntcr::Thread")
, d_reactor_sp(reactor)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_runMutex()
, d_runCondition()
, d_runState(k_RUN_STATE_STOPPED)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    this->initialize();

    BSLS_ASSERT_OPT(d_reactor_sp->minThreads() == 1);
    BSLS_ASSERT_OPT(d_reactor_sp->maxThreads() == 1);
}

Thread::~Thread()
{
    this->shutdown();
    this->linger();

    d_reactor_sp->clear();
    d_reactor_sp.reset();
}

ntsa::Error Thread::start()
{
    bslmt::ThreadAttributes threadAttributes;
    threadAttributes.setThreadName(d_config.threadName().value());
    threadAttributes.setDetachedState(
        bslmt::ThreadAttributes::e_CREATE_JOINABLE);
    threadAttributes.setStackSize(NTCCFG_DEFAULT_STACK_SIZE);

    return this->start(threadAttributes);
}

ntsa::Error Thread::start(const bslmt::ThreadAttributes& threadAttributes)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().value().c_str());

    NTCI_LOG_TRACE("Thread '%s' is starting",
                   d_config.threadName().value().c_str());

    d_threadAttributes = threadAttributes;
    if (d_threadAttributes.threadName().empty()) {
        d_threadAttributes.setThreadName(d_config.threadName().value());
    }

    bslmt::ThreadUtil::ThreadFunction threadFunction =
        (bslmt::ThreadUtil::ThreadFunction)(&ntcr::Thread::run);
    void* threadUserData = this;

    ntsa::Error error = ntcs::ThreadUtil::create(&d_threadHandle,
                                                 d_threadAttributes,
                                                 threadFunction,
                                                 threadUserData);
    if (error) {
        NTCI_LOG_ERROR("Failed to create thread: %s", error.text().c_str());
        return error;
    }

    ntccfg::ConditionMutexGuard guard(&d_runMutex);
    while (d_runState != k_RUN_STATE_STARTED) {
        d_runCondition.wait(&d_runMutex);
    }

    return ntsa::Error();
}

void Thread::shutdown()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().value().c_str());

    ntccfg::ConditionMutexGuard guard(&d_runMutex);

    if (d_runState != k_RUN_STATE_STARTED) {
        return;
    }

    NTCI_LOG_TRACE("Thread '%s' is stopping",
                   d_config.threadName().value().c_str());

    d_runState = k_RUN_STATE_STOPPING;

    d_reactor_sp->stop();
}

void Thread::linger()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().value().c_str());

    int rc;

    if (d_runState == k_RUN_STATE_STOPPED) {
        return;
    }

    void* status = 0;
    rc           = bslmt::ThreadUtil::join(d_threadHandle, &status);
    if (rc != 0) {
        NTCI_LOG_FATAL("Failed to join thread: rc = %d", rc);
        NTCCFG_ABORT();
    }

    d_runState = k_RUN_STATE_STOPPED;

    d_reactor_sp->restart();

    NTCI_LOG_TRACE("Thread '%s' has stopped",
                   d_config.threadName().value().c_str());
}

ntsa::Error Thread::closeAll()
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_CONTEXT_GUARD_OWNER(d_config.metricName().value().c_str());

    return d_reactor_sp->closeAll();
}

void Thread::execute(const Functor& functor)
{
    d_reactor_sp->execute(functor);
}

void Thread::moveAndExecute(FunctorSequence* functorSequence,
                            const Functor&   functor)
{
    d_reactor_sp->moveAndExecute(functorSequence, functor);
}

bsl::shared_ptr<ntci::Timer> Thread::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return d_reactor_sp->createTimer(options, session, basicAllocator);
}

bsl::shared_ptr<ntci::Timer> Thread::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return d_reactor_sp->createTimer(options, callback, basicAllocator);
}

bsl::shared_ptr<ntci::Strand> Thread::createStrand(
    bslma::Allocator* basicAllocator)
{
    return d_reactor_sp->createStrand(basicAllocator);
}

bsl::shared_ptr<ntci::DatagramSocket> Thread::createDatagramSocket(
    const ntca::DatagramSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    return d_reactor_sp->createDatagramSocket(options, basicAllocator);
}

bsl::shared_ptr<ntci::ListenerSocket> Thread::createListenerSocket(
    const ntca::ListenerSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    return d_reactor_sp->createListenerSocket(options, basicAllocator);
}

bsl::shared_ptr<ntci::StreamSocket> Thread::createStreamSocket(
    const ntca::StreamSocketOptions& options,
    bslma::Allocator*                basicAllocator)
{
    return d_reactor_sp->createStreamSocket(options, basicAllocator);
}

bslmt::ThreadUtil::Handle Thread::threadHandle() const
{
    return d_threadHandle;
}

bsl::size_t Thread::threadIndex() const
{
    return 0;
}

const bsl::shared_ptr<ntci::Strand>& Thread::strand() const
{
    return ntci::Strand::unspecified();
}

bsls::TimeInterval Thread::currentTime() const
{
    return bdlt::CurrentTime::now();
}

}  // close package namespace
}  // close enterprise namespace
