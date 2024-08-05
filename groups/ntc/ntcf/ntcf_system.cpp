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

#include <ntcf_system.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_system_cpp, "$Id$ $CSID$")

#include <ntccfg_limits.h>
#include <ntccfg_tune.h>
#include <ntci_log.h>
#include <ntci_monitorable.h>
#include <ntcm_monitorableutil.h>
#include <ntcs_authorization.h>
#include <ntcs_compat.h>
#include <ntcs_datapool.h>
#include <ntcs_global.h>
#include <ntcs_metrics.h>
#include <ntcs_plugin.h>
#include <ntcs_proactormetrics.h>
#include <ntcs_processmetrics.h>
#include <ntcs_ratelimiter.h>
#include <ntcs_reactormetrics.h>
#include <ntcs_reservation.h>

#include <ntcr_datagramsocket.h>
#include <ntcr_interface.h>
#include <ntcr_listenersocket.h>
#include <ntcr_streamsocket.h>
#include <ntcr_thread.h>

#include <ntcp_datagramsocket.h>
#include <ntcp_interface.h>
#include <ntcp_listenersocket.h>
#include <ntcp_streamsocket.h>
#include <ntcp_thread.h>

#include <ntcdns_resolver.h>

#include <ntco_devpoll.h>
#include <ntco_epoll.h>
#include <ntco_eventport.h>
#include <ntco_iocp.h>
#include <ntco_ioring.h>
#include <ntco_kqueue.h>
#include <ntco_poll.h>
#include <ntco_pollset.h>
#include <ntco_select.h>

#include <bdlbb_pooledblobbufferfactory.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_once.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_fstream.h>
#include <bsl_sstream.h>

#define NTCF_SYSTEM_LOG_INVALID_CONFIGURATION_DRIVER_NAME(driverName)         \
    BSLS_LOG_FATAL("Invalid configuration: unsupported driver name '%s'",     \
                   driverName.c_str());

namespace BloombergLP {
namespace ntcf {

namespace {

bsl::string defaultDriverName()
{
#if defined(BSLS_PLATFORM_OS_AIX)
    return "POLLSET";
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    return "KQUEUE";
#elif defined(BSLS_PLATFORM_OS_FREEBSD)
    return "KQUEUE";
#elif defined(BSLS_PLATFORM_OS_LINUX)
    return "EPOLL";
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
    return "EVENTPORT";
#elif defined(BSLS_PLATFORM_OS_UNIX)
    return "POLL";
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return "IOCP";
#else
#error Not implemented
#endif
}

bsl::string defaultReactorDriverName()
{
#if defined(BSLS_PLATFORM_OS_AIX)
    return "POLLSET";
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    return "KQUEUE";
#elif defined(BSLS_PLATFORM_OS_FREEBSD)
    return "KQUEUE";
#elif defined(BSLS_PLATFORM_OS_LINUX)
    return "EPOLL";
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
    return "EVENTPORT";
#elif defined(BSLS_PLATFORM_OS_UNIX)
    return "POLL";
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return "POLL";
#else
#error Not implemented
#endif
}

bsl::string defaultProactorDriverName()
{
#if defined(BSLS_PLATFORM_OS_AIX)
    return "UNSUPPORTED";
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    return "UNSUPPORTED";
#elif defined(BSLS_PLATFORM_OS_FREEBSD)
    return "UNSUPPORTED";
#elif defined(BSLS_PLATFORM_OS_LINUX)
    return "UNSUPPORTED";
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
    return "UNSUPPORTED";
#elif defined(BSLS_PLATFORM_OS_UNIX)
    return "UNSUPPORTED";
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return "IOCP";
#else
#error Not implemented
#endif
}

void createDefaultExecutor(bsl::shared_ptr<ntci::Executor>* result,
                           bslma::Allocator*                allocator)
{
    ntsa::Error error;

    allocator =
        allocator ? allocator : &bslma::NewDeleteAllocator::singleton();

    ntca::ThreadConfig threadConfig;
    threadConfig.setThreadName("default");

    bsl::shared_ptr<ntci::Thread> thread =
        ntcf::System::createThread(threadConfig, allocator);

    error = thread->start();
    BSLS_ASSERT_OPT(!error);

    *result = thread;
}

void createDefaultDriver(bsl::shared_ptr<ntci::Driver>* result,
                         bslma::Allocator*              allocator)
{
    allocator =
        allocator ? allocator : &bslma::NewDeleteAllocator::singleton();

    ntca::DriverConfig driverConfig;
    driverConfig.setMinThreads(1);
    driverConfig.setMaxThreads(1);

    bsl::shared_ptr<ntci::Driver> driver =
        ntcf::System::createDriver(driverConfig, allocator);

    *result = driver;
}

void createDefaultReactor(bsl::shared_ptr<ntci::Reactor>* result,
                          bslma::Allocator*               allocator)
{
    allocator =
        allocator ? allocator : &bslma::NewDeleteAllocator::singleton();

    ntca::ReactorConfig reactorConfig;
    reactorConfig.setMinThreads(1);
    reactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntci::Reactor> reactor =
        ntcf::System::createReactor(reactorConfig, allocator);

    *result = reactor;
}

void createDefaultProactor(bsl::shared_ptr<ntci::Proactor>* result,
                           bslma::Allocator*                allocator)
{
    allocator =
        allocator ? allocator : &bslma::NewDeleteAllocator::singleton();

    ntca::ProactorConfig proactorConfig;
    proactorConfig.setMinThreads(1);
    proactorConfig.setMaxThreads(1);

    bsl::shared_ptr<ntci::Proactor> proactor =
        ntcf::System::createProactor(proactorConfig, allocator);

    *result = proactor;
}

void createDefaultInterface(bsl::shared_ptr<ntci::Interface>* result,
                            bslma::Allocator*                 allocator)
{
    ntsa::Error error;

    allocator =
        allocator ? allocator : &bslma::NewDeleteAllocator::singleton();

    ntca::InterfaceConfig interfaceConfig;
    interfaceConfig.setThreadName("default");
    interfaceConfig.setMinThreads(1);
    interfaceConfig.setMaxThreads(64);

    bsl::shared_ptr<ntci::Interface> interface =
        ntcf::System::createInterface(interfaceConfig, allocator);

    error = interface->start();
    BSLS_ASSERT_OPT(!error);

    *result = interface;
}

void createDefaultResolver(bsl::shared_ptr<ntci::Resolver>* result,
                           bslma::Allocator*                allocator)
{
    ntsa::Error error;

    allocator =
        allocator ? allocator : &bslma::NewDeleteAllocator::singleton();

    ntca::ResolverConfig resolverConfig;

    bsl::shared_ptr<ntci::Resolver> resolver =
        ntcf::System::createResolver(resolverConfig, allocator);

    error = resolver->start();
    BSLS_ASSERT_OPT(!error);

    *result = resolver;
}

}  // close unnamed namespace

ntsa::Error System::initialize()
{
    BSLMT_ONCE_DO
    {
        ntsa::Error error = ntsf::System::initialize();
        if (error) {
            return error;
        }

        ntcm::MonitorableUtil::initialize();
        ntcs::Plugin::initialize();
        ntcs::Global::initialize();

        // We use a new delete allocator instead of the global allocator here
        // because we want prevent a visible "memory leak" if the global
        // allocator has been replaced in main.  This is because the memory
        // allocated by the plugins won't be freed until the application exits.

        bslma::Allocator* allocator = &bslma::NewDeleteAllocator::singleton();

#if NTC_BUILD_WITH_DEVPOLL
#if defined(BSLS_PLATFORM_OS_SOLARIS)
        {
            bsl::shared_ptr<ntco::DevpollFactory> devpollFactory;
            devpollFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerReactorFactory("DEVPOLL", devpollFactory);
        }
#endif
#endif

#if NTC_BUILD_WITH_EPOLL
#if defined(BSLS_PLATFORM_OS_LINUX)
        {
            bsl::shared_ptr<ntco::EpollFactory> epollFactory;
            epollFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerReactorFactory("EPOLL", epollFactory);
        }
#endif
#endif

#if NTC_BUILD_WITH_EVENTPORT
#if defined(BSLS_PLATFORM_OS_SOLARIS)
        {
            bsl::shared_ptr<ntco::EventPortFactory> eventPortFactory;
            eventPortFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerReactorFactory("EVENTPORT",
                                                 eventPortFactory);
        }
#endif
#endif

#if NTC_BUILD_WITH_KQUEUE
#if defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_FREEBSD)
        {
            bsl::shared_ptr<ntco::KqueueFactory> kqueueFactory;
            kqueueFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerReactorFactory("KQUEUE", kqueueFactory);
        }
#endif
#endif

#if NTC_BUILD_WITH_POLL
        {
            bsl::shared_ptr<ntco::PollFactory> pollFactory;
            pollFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerReactorFactory("POLL", pollFactory);
        }
#endif

#if NTC_BUILD_WITH_POLLSET
#if defined(BSLS_PLATFORM_OS_AIX)
        {
            bsl::shared_ptr<ntco::PollsetFactory> pollsetFactory;
            pollsetFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerReactorFactory("POLLSET", pollsetFactory);
        }
#endif
#endif

#if NTC_BUILD_WITH_SELECT
        {
            bsl::shared_ptr<ntco::SelectFactory> selectFactory;
            selectFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerReactorFactory("SELECT", selectFactory);
        }
#endif

#if NTC_BUILD_WITH_IOCP
#if defined(BSLS_PLATFORM_OS_WINDOWS)
        {
            bsl::shared_ptr<ntco::IocpFactory> iocpFactory;
            iocpFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerProactorFactory("IOCP", iocpFactory);
        }
#endif
#endif

#if NTC_BUILD_WITH_IORING
#if defined(BSLS_PLATFORM_OS_LINUX)
        if (ntco::IoRingFactory::isSupported()) {
            bsl::shared_ptr<ntco::IoRingFactory> iocpFactory;
            iocpFactory.createInplace(allocator, allocator);
            ntcs::Plugin::registerProactorFactory("IORING", iocpFactory);
        }
#endif
#endif

        ntcs::Global::setDefault(&createDefaultExecutor);
        ntcs::Global::setDefault(&createDefaultDriver);
        ntcs::Global::setDefault(&createDefaultReactor);
        ntcs::Global::setDefault(&createDefaultProactor);
        ntcs::Global::setDefault(&createDefaultInterface);
        ntcs::Global::setDefault(&createDefaultResolver);

        bsl::atexit(&System::exit);
    }

    return ntsa::Error();
}

ntsa::Error System::ignore(ntscfg::Signal::Value signal)
{
    return ntsf::System::ignore(signal);
}

bsl::shared_ptr<ntci::Scheduler> System::createScheduler(
    const ntca::SchedulerConfig& configuration,
    bslma::Allocator*            basicAllocator)
{
    return System::createInterface(configuration, basicAllocator);
}

bsl::shared_ptr<ntci::Scheduler> System::createScheduler(
    const ntca::SchedulerConfig&                     configuration,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    return System::createInterface(
        configuration, blobBufferFactory, basicAllocator);
}

bsl::shared_ptr<ntci::Scheduler> System::createScheduler(
    const ntca::SchedulerConfig&           configuration,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
{
    return System::createInterface(configuration, dataPool, basicAllocator);
}

bsl::shared_ptr<ntci::Interface> System::createInterface(
    const ntca::InterfaceConfig& configuration,
    bslma::Allocator*            basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::DataPool> dataPool;
    {
        bsl::shared_ptr<ntcs::DataPool> concreteDataPool;
        concreteDataPool.createInplace(allocator, allocator);
        dataPool = concreteDataPool;
    }

    return System::createInterface(configuration, dataPool, allocator);
}

bsl::shared_ptr<ntci::Interface> System::createInterface(
    const ntca::InterfaceConfig&                     configuration,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<bdlbb::BlobBufferFactory> incomingBlobBufferFactory =
        blobBufferFactory;

    bsl::shared_ptr<bdlbb::BlobBufferFactory> outgoingBlobBufferFactory;
    {
        bsl::shared_ptr<bdlbb::PooledBlobBufferFactory>
            concreteBlobBufferFactory;
        concreteBlobBufferFactory.createInplace(
            allocator,
            NTCCFG_DEFAULT_OUTGOING_BLOB_BUFFER_SIZE,
            allocator);
        outgoingBlobBufferFactory = concreteBlobBufferFactory;
    }

    bsl::shared_ptr<ntci::DataPool> dataPool;
    {
        bsl::shared_ptr<ntcs::DataPool> concreteDataPool;
        concreteDataPool.createInplace(allocator,
                                       incomingBlobBufferFactory,
                                       outgoingBlobBufferFactory,
                                       allocator);
        dataPool = concreteDataPool;
    }

    return System::createInterface(configuration, dataPool, allocator);
}

bsl::shared_ptr<ntci::Interface> System::createInterface(
    const ntca::InterfaceConfig&           configuration,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::InterfaceConfig effectiveConfig = configuration;
    ntcs::Compat::sanitize(&effectiveConfig);

    if (effectiveConfig.driverName().empty()) {
        effectiveConfig.setDriverName(defaultDriverName());
    }

    {
        bsl::shared_ptr<ntci::ReactorFactory> reactorFactory;
        error =
            ntcs::Plugin::lookupReactorFactory(&reactorFactory,
                                               effectiveConfig.driverName());
        if (!error) {
            bsl::shared_ptr<ntcr::Interface> interface;
            interface.createInplace(allocator,
                                    effectiveConfig,
                                    dataPool,
                                    reactorFactory,
                                    allocator);
            return interface;
        }
    }

    {
        bsl::shared_ptr<ntci::ProactorFactory> proactorFactory;
        error =
            ntcs::Plugin::lookupProactorFactory(&proactorFactory,
                                                effectiveConfig.driverName());
        if (!error) {
            bsl::shared_ptr<ntcp::Interface> interface;
            interface.createInplace(allocator,
                                    effectiveConfig,
                                    dataPool,
                                    proactorFactory,
                                    allocator);
            return interface;
        }
    }

    NTCF_SYSTEM_LOG_INVALID_CONFIGURATION_DRIVER_NAME(
        effectiveConfig.driverName());

    NTCCFG_ABORT();

#if defined(BSLS_PLATFORM_CMP_IBM)
    return bsl::shared_ptr<ntci::Interface>();
#endif
}

bsl::shared_ptr<ntci::Thread> System::createThread(
    const ntca::ThreadConfig& configuration,
    bslma::Allocator*         basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::ThreadConfig effectiveConfig = configuration;

    if (effectiveConfig.driverName().isNull() ||
        effectiveConfig.driverName().value().empty())
    {
        effectiveConfig.setDriverName(defaultDriverName());
    }

    {
        bsl::shared_ptr<ntci::ReactorFactory> reactorFactory;
        error = ntcs::Plugin::lookupReactorFactory(
            &reactorFactory,
            effectiveConfig.driverName().value());
        if (!error) {
            bsl::shared_ptr<ntcr::Thread> thread;
            thread.createInplace(allocator,
                                 effectiveConfig,
                                 reactorFactory,
                                 allocator);

            return thread;
        }
    }

    {
        bsl::shared_ptr<ntci::ProactorFactory> proactorFactory;
        error = ntcs::Plugin::lookupProactorFactory(
            &proactorFactory,
            effectiveConfig.driverName().value());
        if (!error) {
            bsl::shared_ptr<ntcp::Thread> thread;
            thread.createInplace(allocator,
                                 effectiveConfig,
                                 proactorFactory,
                                 allocator);

            return thread;
        }
    }

    NTCF_SYSTEM_LOG_INVALID_CONFIGURATION_DRIVER_NAME(
        effectiveConfig.driverName().value());

    NTCCFG_ABORT();

#if defined(BSLS_PLATFORM_CMP_IBM)
    return bsl::shared_ptr<ntci::Thread>();
#endif
}

bsl::shared_ptr<ntci::Thread> System::createThread(
    const ntca::ThreadConfig&             configuration,
    const bsl::shared_ptr<ntci::Reactor>& reactor,
    bslma::Allocator*                     basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcr::Thread> thread;
    thread.createInplace(allocator, configuration, reactor, allocator);

    return thread;
}

bsl::shared_ptr<ntci::Thread> System::createThread(
    const ntca::ThreadConfig&              configuration,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcp::Thread> thread;
    thread.createInplace(allocator, configuration, proactor, allocator);

    return thread;
}

bsl::shared_ptr<ntci::Driver> System::createDriver(
    const ntca::DriverConfig& configuration,
    bslma::Allocator*         basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::DriverConfig effectiveConfig = configuration;

    if (effectiveConfig.driverName().isNull() ||
        effectiveConfig.driverName().value().empty())
    {
        effectiveConfig.setDriverName(defaultDriverName());
    }

    {
        bsl::shared_ptr<ntci::ReactorFactory> reactorFactory;
        error = ntcs::Plugin::lookupReactorFactory(
            &reactorFactory,
            effectiveConfig.driverName().value());
        if (!error) {
            ntca::ReactorConfig reactorConfig;

            reactorConfig.setDriverName(effectiveConfig.driverName().value());

            if (!configuration.driverMechanism().isNull()) {
                reactorConfig.setDriverMechanism(
                    configuration.driverMechanism().value());
            }

            if (!configuration.metricName().isNull()) {
                reactorConfig.setMetricName(
                    configuration.metricName().value());
            }

            if (!configuration.minThreads().isNull()) {
                reactorConfig.setMinThreads(
                    configuration.minThreads().value());
            }

            if (!configuration.maxThreads().isNull()) {
                reactorConfig.setMaxThreads(
                    configuration.maxThreads().value());
            }

            if (!configuration.maxEventsPerWait().isNull()) {
                reactorConfig.setMaxEventsPerWait(
                    configuration.maxEventsPerWait().value());
            }

            if (!configuration.maxTimersPerWait().isNull()) {
                reactorConfig.setMaxTimersPerWait(
                    configuration.maxTimersPerWait().value());
            }

            if (!configuration.maxCyclesPerWait().isNull()) {
                reactorConfig.setMaxCyclesPerWait(
                    configuration.maxCyclesPerWait().value());
            }

            if (reactorConfig.maxThreads() > 1) {
                reactorConfig.setOneShot(true);
            }

            return reactorFactory->createReactor(reactorConfig,
                                                 bsl::shared_ptr<ntci::User>(),
                                                 allocator);
        }
    }

    {
        bsl::shared_ptr<ntci::ProactorFactory> proactorFactory;
        error = ntcs::Plugin::lookupProactorFactory(
            &proactorFactory,
            effectiveConfig.driverName().value());
        if (!error) {
            ntca::ProactorConfig proactorConfig;

            proactorConfig.setDriverName(effectiveConfig.driverName().value());

            if (!configuration.driverMechanism().isNull()) {
                proactorConfig.setDriverMechanism(
                    configuration.driverMechanism().value());
            }

            if (!configuration.metricName().isNull()) {
                proactorConfig.setMetricName(
                    configuration.metricName().value());
            }

            if (!configuration.minThreads().isNull()) {
                proactorConfig.setMinThreads(
                    configuration.minThreads().value());
            }

            if (!configuration.maxThreads().isNull()) {
                proactorConfig.setMaxThreads(
                    configuration.maxThreads().value());
            }

            if (!configuration.maxEventsPerWait().isNull()) {
                proactorConfig.setMaxEventsPerWait(
                    configuration.maxEventsPerWait().value());
            }

            if (!configuration.maxTimersPerWait().isNull()) {
                proactorConfig.setMaxTimersPerWait(
                    configuration.maxTimersPerWait().value());
            }

            if (!configuration.maxCyclesPerWait().isNull()) {
                proactorConfig.setMaxCyclesPerWait(
                    configuration.maxCyclesPerWait().value());
            }

            return proactorFactory->createProactor(
                proactorConfig,
                bsl::shared_ptr<ntci::User>(),
                allocator);
        }
    }

    NTCF_SYSTEM_LOG_INVALID_CONFIGURATION_DRIVER_NAME(
        effectiveConfig.driverName().value());

    NTCCFG_ABORT();

#if defined(BSLS_PLATFORM_CMP_IBM)
    return bsl::shared_ptr<ntci::Driver>();
#endif
}

bsl::shared_ptr<ntci::Reactor> System::createReactor(
    const ntca::ReactorConfig& configuration,
    bslma::Allocator*          basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::ReactorConfig effectiveConfig = configuration;

    if (effectiveConfig.driverName().isNull() ||
        effectiveConfig.driverName().value().empty())
    {
        effectiveConfig.setDriverName(defaultReactorDriverName());
    }

    {
        bsl::shared_ptr<ntci::ReactorFactory> reactorFactory;
        error = ntcs::Plugin::lookupReactorFactory(
            &reactorFactory,
            effectiveConfig.driverName().value());
        if (!error) {
            return reactorFactory->createReactor(effectiveConfig,
                                                 bsl::shared_ptr<ntci::User>(),
                                                 allocator);
        }
    }

    NTCF_SYSTEM_LOG_INVALID_CONFIGURATION_DRIVER_NAME(
        effectiveConfig.driverName().value());

    NTCCFG_ABORT();

#if defined(BSLS_PLATFORM_CMP_IBM)
    return bsl::shared_ptr<ntci::Reactor>();
#endif
}

bsl::shared_ptr<ntci::ReactorMetrics> System::createReactorMetrics(
    const bsl::string& prefix,
    const bsl::string& objectName,
    bslma::Allocator*  basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::ReactorMetrics> metrics;
    metrics.createInplace(allocator, prefix, objectName, allocator);

    return metrics;
}

bsl::shared_ptr<ntci::ReactorMetrics> System::createReactorMetrics(
    const bsl::string&                           prefix,
    const bsl::string&                           objectName,
    const bsl::shared_ptr<ntci::ReactorMetrics>& parent,
    bslma::Allocator*                            basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::ReactorMetrics> metrics;
    metrics.createInplace(allocator, prefix, objectName, parent, allocator);

    return metrics;
}

bsl::shared_ptr<ntci::Proactor> System::createProactor(
    const ntca::ProactorConfig& configuration,
    bslma::Allocator*           basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::ProactorConfig effectiveConfig = configuration;

    if (effectiveConfig.driverName().isNull() ||
        effectiveConfig.driverName().value().empty())
    {
        effectiveConfig.setDriverName(defaultProactorDriverName());
    }

    {
        bsl::shared_ptr<ntci::ProactorFactory> proactorFactory;
        error = ntcs::Plugin::lookupProactorFactory(
            &proactorFactory,
            effectiveConfig.driverName().value());
        if (!error) {
            return proactorFactory->createProactor(
                effectiveConfig,
                bsl::shared_ptr<ntci::User>(),
                allocator);
        }
    }

    NTCF_SYSTEM_LOG_INVALID_CONFIGURATION_DRIVER_NAME(
        effectiveConfig.driverName().value());

    NTCCFG_ABORT();

#if defined(BSLS_PLATFORM_CMP_IBM)
    return bsl::shared_ptr<ntci::Proactor>();
#endif
}

bsl::shared_ptr<ntci::ProactorMetrics> System::createProactorMetrics(
    const bsl::string& prefix,
    const bsl::string& objectName,
    bslma::Allocator*  basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::ProactorMetrics> metrics;
    metrics.createInplace(allocator, prefix, objectName, allocator);

    return metrics;
}

bsl::shared_ptr<ntci::ProactorMetrics> System::createProactorMetrics(
    const bsl::string&                            prefix,
    const bsl::string&                            objectName,
    const bsl::shared_ptr<ntci::ProactorMetrics>& parent,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::ProactorMetrics> metrics;
    metrics.createInplace(allocator, prefix, objectName, parent, allocator);

    return metrics;
}

bsl::shared_ptr<ntci::DatagramSocket> System::createDatagramSocket(
    const ntca::DatagramSocketOptions&    configuration,
    const bsl::shared_ptr<ntci::Reactor>& reactor,
    const bsl::shared_ptr<ntci::User>&    user,
    bslma::Allocator*                     basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::Resolver> resolver;
    if (user) {
        resolver = user->resolver();
    }

    // TODO: Get metrics from 'user'.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcr::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator,
                                 configuration,
                                 resolver,
                                 reactor,
                                 reactor,
                                 metrics,
                                 allocator);

    return datagramSocket;
}

bsl::shared_ptr<ntci::DatagramSocket> System::createDatagramSocket(
    const ntca::DatagramSocketOptions&     configuration,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const bsl::shared_ptr<ntci::User>&     user,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::Resolver> resolver;
    if (user) {
        resolver = user->resolver();
    }

    // TODO: Get metrics from 'user'.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator,
                                 configuration,
                                 resolver,
                                 proactor,
                                 proactor,
                                 metrics,
                                 allocator);

    return datagramSocket;
}

bsl::shared_ptr<ntci::ListenerSocket> System::createListenerSocket(
    const ntca::ListenerSocketOptions&    configuration,
    const bsl::shared_ptr<ntci::Reactor>& reactor,
    const bsl::shared_ptr<ntci::User>&    user,
    bslma::Allocator*                     basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::Resolver> resolver;
    if (user) {
        resolver = user->resolver();
    }

    // TODO: Get metrics from 'user'.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcr::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator,
                                 configuration,
                                 resolver,
                                 reactor,
                                 reactor,
                                 metrics,
                                 allocator);

    return listenerSocket;
}

bsl::shared_ptr<ntci::ListenerSocket> System::createListenerSocket(
    const ntca::ListenerSocketOptions&     configuration,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const bsl::shared_ptr<ntci::User>&     user,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::Resolver> resolver;
    if (user) {
        resolver = user->resolver();
    }

    // TODO: Get metrics from 'user'.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator,
                                 configuration,
                                 resolver,
                                 proactor,
                                 proactor,
                                 metrics,
                                 allocator);

    return listenerSocket;
}

bsl::shared_ptr<ntci::StreamSocket> System::createStreamSocket(
    const ntca::StreamSocketOptions&      configuration,
    const bsl::shared_ptr<ntci::Reactor>& reactor,
    const bsl::shared_ptr<ntci::User>&    user,
    bslma::Allocator*                     basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::Resolver> resolver;
    if (user) {
        resolver = user->resolver();
    }

    // TODO: Get metrics from 'user'.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcr::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator,
                               configuration,
                               resolver,
                               reactor,
                               reactor,
                               metrics,
                               allocator);

    return streamSocket;
}

bsl::shared_ptr<ntci::StreamSocket> System::createStreamSocket(
    const ntca::StreamSocketOptions&       configuration,
    const bsl::shared_ptr<ntci::Proactor>& proactor,
    const bsl::shared_ptr<ntci::User>&     user,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntci::Resolver> resolver;
    if (user) {
        resolver = user->resolver();
    }

    // TODO: Get metrics from 'user'.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator,
                               configuration,
                               resolver,
                               proactor,
                               proactor,
                               metrics,
                               allocator);

    return streamSocket;
}

bsl::shared_ptr<ntci::RateLimiter> System::createRateLimiter(
    const ntca::RateLimiterConfig& configuration,
    bslma::Allocator*              basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

bsl::shared_ptr<ntci::DataPool> System::createDataPool(
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator, allocator);

    return dataPool;
}

bsl::shared_ptr<ntci::DataPool> System::createDataPool(
    bdlbb::BlobBufferFactory* blobBufferFactory,
    bslma::Allocator*         basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<bdlbb::BlobBufferFactory> blobBufferFactory_sp(
        blobBufferFactory,
        bslstl::SharedPtrNilDeleter(),
        allocator);

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator,
                           blobBufferFactory_sp,
                           blobBufferFactory_sp,
                           allocator);

    return dataPool;
}

bsl::shared_ptr<ntci::DataPool> System::createDataPool(
    bsl::size_t       incomingBlobBufferSize,
    bsl::size_t       outgoingBlobBufferSize,
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator,
                           incomingBlobBufferSize,
                           outgoingBlobBufferSize,
                           allocator);

    return dataPool;
}

bsl::shared_ptr<ntci::DataPool> System::createDataPool(
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(allocator,
                           incomingBlobBufferFactory,
                           outgoingBlobBufferFactory,
                           allocator);

    return dataPool;
}

bsl::shared_ptr<ntci::Resolver> System::createResolver(
    const ntca::ResolverConfig& configuration,
    bslma::Allocator*           basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bool clientEnabled = true;
    if (!configuration.clientEnabled().isNull()) {
        clientEnabled = configuration.clientEnabled().value();
    }

    if (clientEnabled) {
        bsl::shared_ptr<ntci::DataPool> dataPool;
        {
            bsl::shared_ptr<ntcs::DataPool> concreteDataPool;
            concreteDataPool.createInplace(
                allocator,
                (bsl::size_t)(ntcdns::Resolver::k_UDP_MAX_PAYLOAD_SIZE),
                (bsl::size_t)(ntcdns::Resolver::k_UDP_MAX_PAYLOAD_SIZE),
                allocator);

            dataPool = concreteDataPool;
        }

        bsl::shared_ptr<ntci::Interface> interface;
        {
            ntca::InterfaceConfig interfaceConfig;
            interfaceConfig.setThreadName("dns");
            interfaceConfig.setMinThreads(1);
            interfaceConfig.setMaxThreads(1);
            interfaceConfig.setResolverEnabled(false);

            interface = ntcf::System::createInterface(interfaceConfig,
                                                      dataPool,
                                                      allocator);
        }

        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(allocator, configuration, interface, allocator);

        return resolver;
    }
    else {
        bsl::shared_ptr<ntcdns::Resolver> resolver;
        resolver.createInplace(allocator, configuration, allocator);

        return resolver;
    }
}

bsl::shared_ptr<ntci::Authorization> System::createAuthorization(
    bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcs::Authorization> authorization;
    authorization.createInplace(allocator);

    return authorization;
}

ntsa::Error System::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionClient(result,
                                                    options,
                                                    basicAllocator);
}

ntsa::Error System::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>*         result,
    const ntca::EncryptionClientOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionServer(result,
                                                    options,
                                                    basicAllocator);
}

ntsa::Error System::createEncryptionResource(
    bsl::shared_ptr<ntci::EncryptionResource>* result,
    bslma::Allocator*                          basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->createEncryptionResource(result, basicAllocator);
}

ntsa::Error System::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>*         result,
    const ntca::EncryptionServerOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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





ntsa::Error System::generateCertificate(
        ntca::EncryptionCertificate*                  result,
        const ntsa::DistinguishedName&                subjectIdentity,
        const ntca::EncryptionKey&                    subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::generateCertificate(
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            subjectIdentity,
        const ntca::EncryptionKey&                subjectPrivateKey,
        const ntca::EncryptionCertificate&        issuerCertificate,
        const ntca::EncryptionKey&                issuerPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator*                         basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const ntsa::DistinguishedName&                subjectIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&   subjectPrivateKey,
    const ntca::EncryptionCertificateOptions&     options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
    const ntsa::DistinguishedName&                      subjectIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&         subjectPrivateKey,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& issuerCertificate,
    const bsl::shared_ptr<ntci::EncryptionKey>&         issuerPrivateKey,
    const ntca::EncryptionCertificateOptions&           options,
    bslma::Allocator*                                   basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::loadCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            path,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->loadCertificate(result, path, basicAllocator);
}

ntsa::Error System::loadCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            path,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::saveCertificate(
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const bsl::string&                                  path)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->saveCertificate(certificate, path);
}

ntsa::Error System::saveCertificate(
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const bsl::string&                                  path,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->saveCertificate(certificate, path, options);
}

ntsa::Error System::encodeCertificate(
    bsl::streambuf*                                     destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination, certificate);
}

ntsa::Error System::encodeCertificate(
    bsl::streambuf*                                     destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination,
                                               certificate,
                                               options);
}

ntsa::Error System::encodeCertificate(
    bdlbb::Blob*                                        destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination, certificate);
}

ntsa::Error System::encodeCertificate(
    bdlbb::Blob*                                        destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination,
                                               certificate,
                                               options);
}

ntsa::Error System::encodeCertificate(
    bsl::string*                                        destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination, certificate);
}

ntsa::Error System::encodeCertificate(
    bsl::string*                                        destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination,
                                               certificate,
                                               options);
}

ntsa::Error System::encodeCertificate(
    bsl::vector<char>*                                  destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination, certificate);
}

ntsa::Error System::encodeCertificate(
    bsl::vector<char>*                                  destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeCertificate(destination,
                                               certificate,
                                               options);
}

ntsa::Error System::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    bsl::streambuf*                               source,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeCertificate(result, source, basicAllocator);
}

ntsa::Error System::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    bsl::streambuf*                               source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bdlbb::Blob&                            source,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeCertificate(result, source, basicAllocator);
}

ntsa::Error System::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bdlbb::Blob&                            source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            source,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeCertificate(result, source, basicAllocator);
}

ntsa::Error System::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::vector<char>&                      source,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeCertificate(result, source, basicAllocator);
}

ntsa::Error System::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::vector<char>&                      source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::generateKey(
        ntca::EncryptionKey*                  result,
        const ntca::EncryptionKeyOptions&     options,
        bslma::Allocator*                     basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->generateKey(result, options, basicAllocator);
}

ntsa::Error System::generateKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                const ntca::EncryptionKeyOptions&     options,
                                bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->generateKey(result, options, basicAllocator);
}

ntsa::Error System::loadKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                            const bsl::string&                    path,
                            bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->loadKey(result, path, basicAllocator);
}

ntsa::Error System::loadKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                            const bsl::string&                     path,
                            const ntca::EncryptionResourceOptions& options,
                            bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->loadKey(result, path, options, basicAllocator);
}

ntsa::Error System::saveKey(
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const bsl::string&                          path)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->saveKey(privateKey, path);
}

ntsa::Error System::saveKey(
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const bsl::string&                          path,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->saveKey(privateKey, path, options);
}

ntsa::Error System::encodeKey(
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey);
}

ntsa::Error System::encodeKey(
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey, options);
}

ntsa::Error System::encodeKey(
    bdlbb::Blob*                                destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey);
}

ntsa::Error System::encodeKey(
    bdlbb::Blob*                                destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey, options);
}

ntsa::Error System::encodeKey(
    bsl::string*                                destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey);
}

ntsa::Error System::encodeKey(
    bsl::string*                                destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey, options);
}

ntsa::Error System::encodeKey(
    bsl::vector<char>*                          destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey);
}

ntsa::Error System::encodeKey(
    bsl::vector<char>*                          destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->encodeKey(destination, privateKey, options);
}

ntsa::Error System::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                              bsl::streambuf*                       source,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeKey(result, source, basicAllocator);
}

ntsa::Error System::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                              bsl::streambuf*                        source,
                              const ntca::EncryptionResourceOptions& options,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                              const bdlbb::Blob&                    source,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeKey(result, source, basicAllocator);
}

ntsa::Error System::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                              const bdlbb::Blob&                     source,
                              const ntca::EncryptionResourceOptions& options,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                              const bsl::string&                    source,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeKey(result, source, basicAllocator);
}

ntsa::Error System::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                              const bsl::string&                     source,
                              const ntca::EncryptionResourceOptions& options,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

ntsa::Error System::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                              const bsl::vector<char>&              source,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver;
    error = ntcs::Plugin::lookupEncryptionDriver(&encryptionDriver);
    if (error) {
        return error;
    }

    return encryptionDriver->decodeKey(result, source, basicAllocator);
}

ntsa::Error System::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                              const bsl::vector<char>&               source,
                              const ntca::EncryptionResourceOptions& options,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

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

void System::enableMonitorableRegistry(
    const ntca::MonitorableRegistryConfig& configuration,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::enableMonitorableRegistry(configuration,
                                                     basicAllocator);
}

void System::enableMonitorableRegistry(
    const bsl::shared_ptr<ntci::MonitorableRegistry>& monitorableRegistry)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::enableMonitorableRegistry(monitorableRegistry);
}

void System::disableMonitorableRegistry()
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::disableMonitorableRegistry();
}

void System::enableMonitorableCollector(
    const ntca::MonitorableCollectorConfig& configuration,
    bslma::Allocator*                       basicAllocator)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::enableMonitorableCollector(configuration,
                                                      basicAllocator);
}

void System::enableMonitorableCollector(
    const bsl::shared_ptr<ntci::MonitorableCollector>& monitorableCollector)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::enableMonitorableCollector(monitorableCollector);
}

void System::disableMonitorableCollector()
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::disableMonitorableCollector();
}

void System::enableProcessMetrics()
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    bslma::Allocator* allocator = bslma::Default::globalAllocator();

    bsl::shared_ptr<ntcs::ProcessMetrics> processMetrics;
    processMetrics.createInplace(allocator, "process", "global");

    ntcm::MonitorableUtil::registerMonitorableProcess(processMetrics);
}

void System::disableProcessMetrics()
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::deregisterMonitorableProcess();
}

void System::registerMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::registerMonitorable(monitorable);
}

void System::deregisterMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& monitorable)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::deregisterMonitorable(monitorable);
}

void System::registerMonitorablePublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& monitorablePublisher)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::registerMonitorablePublisher(monitorablePublisher);
}

void System::deregisterMonitorablePublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& monitorablePublisher)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::deregisterMonitorablePublisher(
        monitorablePublisher);
}

void System::collectMetrics()
{
    ntcm::MonitorableUtil::collectMetrics();
}

void System::registerMonitorablePublisher(
    bsls::LogSeverity::Enum severityLevel)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::registerMonitorablePublisher(severityLevel);
}

void System::deregisterMonitorablePublisher(
    bsls::LogSeverity::Enum severityLevel)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcm::MonitorableUtil::deregisterMonitorablePublisher(severityLevel);
}

bool System::testDriverSupport(const bsl::string& driverName,
                               bool               dynamicLoadBalancing)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    if (ntcr::Interface::isSupported(driverName, dynamicLoadBalancing)) {
        return true;
    }

    if (ntcp::Interface::isSupported(driverName, dynamicLoadBalancing)) {
        return true;
    }

    return false;
}

void System::loadDriverSupport(bsl::vector<bsl::string>* driverNames,
                               bool                      dynamicLoadBalancing)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    driverNames->clear();

    ntcr::Interface::loadSupportedDriverNames(driverNames,
                                              dynamicLoadBalancing);

    ntcp::Interface::loadSupportedDriverNames(driverNames,
                                              dynamicLoadBalancing);
}

ntsa::Error System::registerEncryptionDriver(
    const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::registerEncryptionDriver(encryptionDriver);
}

ntsa::Error System::deregisterEncryptionDriver(
    const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::deregisterEncryptionDriver(encryptionDriver);
}

bool System::supportsEncryptionDriver()
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::supportsEncryptionDriver();
}

ntsa::Error System::registerReactorFactory(
    const bsl::string&                           driverName,
    const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::registerReactorFactory(driverName, reactorFactory);
}

ntsa::Error System::deregisterReactorFactory(
    const bsl::string&                           driverName,
    const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::deregisterReactorFactory(driverName, reactorFactory);
}

ntsa::Error System::lookupReactorFactory(
    bsl::shared_ptr<ntci::ReactorFactory>* result,
    const bsl::string&                     driverName)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::lookupReactorFactory(result, driverName);
}

bool System::supportsReactorFactory(const bsl::string& driverName)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::supportsReactorFactory(driverName);
}

ntsa::Error System::registerProactorFactory(
    const bsl::string&                            driverName,
    const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::registerProactorFactory(driverName, proactorFactory);
}

ntsa::Error System::deregisterProactorFactory(
    const bsl::string&                            driverName,
    const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::deregisterProactorFactory(driverName,
                                                   proactorFactory);
}

ntsa::Error System::lookupProactorFactory(
    bsl::shared_ptr<ntci::ProactorFactory>* result,
    const bsl::string&                      driverName)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::lookupProactorFactory(result, driverName);
}

bool System::supportsProactorFactory(const bsl::string& driverName)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    return ntcs::Plugin::supportsProactorFactory(driverName);
}

void System::setDefault(const bsl::shared_ptr<ntci::Executor>& executor)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::setDefault(executor);
}

void System::setDefault(const bsl::shared_ptr<ntci::Strand>& strand)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::setDefault(strand);
}

void System::setDefault(const bsl::shared_ptr<ntci::Driver>& driver)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::setDefault(driver);
}

void System::setDefault(const bsl::shared_ptr<ntci::Reactor>& reactor)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::setDefault(reactor);
}

void System::setDefault(const bsl::shared_ptr<ntci::Proactor>& proactor)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::setDefault(proactor);
}

void System::setDefault(const bsl::shared_ptr<ntci::Interface>& interface)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::setDefault(interface);
}

void System::setDefault(const bsl::shared_ptr<ntci::Resolver>& resolver)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::setDefault(resolver);
}

void System::getDefault(bsl::shared_ptr<ntci::Executor>* result)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::getDefault(result);
}

void System::getDefault(bsl::shared_ptr<ntci::Strand>* result)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::getDefault(result);
}

void System::getDefault(bsl::shared_ptr<ntci::Driver>* result)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::getDefault(result);
}

void System::getDefault(bsl::shared_ptr<ntci::Reactor>* result)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::getDefault(result);
}

void System::getDefault(bsl::shared_ptr<ntci::Proactor>* result)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::getDefault(result);
}

void System::getDefault(bsl::shared_ptr<ntci::Interface>* result)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::getDefault(result);
}

void System::getDefault(bsl::shared_ptr<ntci::Resolver>* result)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    ntcs::Global::getDefault(result);
}

void System::exit()
{
    BSLMT_ONCE_DO
    {
        ntcs::Global::exit();
        ntcs::Plugin::exit();
        ntcm::MonitorableUtil::exit();

        ntsf::System::exit();
    }
}

SystemGuard::SystemGuard()
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);
}

SystemGuard::SystemGuard(ntscfg::Signal::Value signal)
{
    ntsa::Error error;

    error = ntcf::System::initialize();
    BSLS_ASSERT_OPT(!error);

    error = ntcf::System::ignore(signal);
    BSLS_ASSERT_OPT(!error);
}

SystemGuard::~SystemGuard()
{
    ntcf::System::exit();
}

}  // close package namespace
}  // close enterprise namespace
