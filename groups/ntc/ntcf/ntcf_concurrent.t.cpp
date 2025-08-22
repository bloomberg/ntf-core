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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_concurrent_t_cpp, "$Id$ $CSID$")

#include <ntcf_concurrent.h>
#include <ntcf_test.h>

#include <bsl_queue.h>

using namespace BloombergLP;

#if NTC_BUILD_WITH_COROUTINES

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_CONNECT_COMPLETE(result)              \
    do {                                                                      \
        BALL_LOG_INFO << "Client socket connect complete: "                   \
                      << (result).event() << BALL_LOG_END;                    \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_SEND_COMPLETE(result, data)           \
    do {                                                                      \
        BALL_LOG_INFO << "Client socket send complete: " << (result).event()  \
                      << "\n"                                                 \
                      << bdlbb::BlobUtilHexDumper((data).get())               \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_RECEIVE_COMPLETE(result)              \
    do {                                                                      \
        BALL_LOG_INFO << "Client socket receive complete: "                   \
                      << (result).event() << "\n"                             \
                      << bdlbb::BlobUtilHexDumper((result).data().get())      \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_CLIENT_CLOSED()                              \
    do {                                                                      \
        BALL_LOG_INFO << "Client socket closed" << BALL_LOG_END;              \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_CONNECT_COMPLETE(result)              \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket connect complete: "                   \
                      << (result).event() << BALL_LOG_END;                    \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_ACCEPT_COMPLETE(result)               \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket accept complete: "                    \
                      << (result).event() << BALL_LOG_END;                    \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_SEND_COMPLETE(result, data)           \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket send complete: " << (result).event()  \
                      << "\n"                                                 \
                      << bdlbb::BlobUtilHexDumper((data).get())               \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(result)              \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket receive complete: "                   \
                      << (result).event() << "\n"                             \
                      << bdlbb::BlobUtilHexDumper((result).data().get())      \
                      << BALL_LOG_END;                                        \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_SERVER_CLOSED()                              \
    do {                                                                      \
        BALL_LOG_INFO << "Server socket closed" << BALL_LOG_END;              \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_LISTENER_READY(endpoint)                     \
    do {                                                                      \
        BALL_LOG_DEBUG << "Listening at " << (endpoint) << BALL_LOG_END;      \
    } while (false)

#define NTCF_CONCURRENT_TEST_LOG_LISTENER_CLOSED()                            \
    do {                                                                      \
        BALL_LOG_INFO << "Listener socket closed" << BALL_LOG_END;            \
    } while (false)

namespace BloombergLP {
namespace ntsa {

struct CoroutineHandleSchedulerTask {
    struct promise_type {
        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        CoroutineHandleSchedulerTask get_return_object()
        {
            std::coroutine_handle<promise_type> coroutine =
                std::coroutine_handle<promise_type>::from_promise(*this);

            return CoroutineHandleSchedulerTask(coroutine);
        }

        void return_void()
        {
        }

        void unhandled_exception()
        {
        }
    };

    explicit CoroutineHandleSchedulerTask(
        std::coroutine_handle<promise_type> handle)
    : handle(handle)
    {
    }

    auto get_handle()
    {
        return handle;
    }

    std::coroutine_handle<promise_type> handle;
};

class CoroutineHandleSchedulerSimple
{
    bsl::queue<bsl::coroutine_handle<void> > d_tasks;

  public:
    void emplace(bsl::coroutine_handle<void> task)
    {
        BSLS_ASSERT(task.address() != 0);
        d_tasks.push(task);
    }

    void schedule()
    {
        while (!d_tasks.empty()) {
            bsl::coroutine_handle<void> task = d_tasks.front();
            d_tasks.pop();

            task.resume();

            if (!task.done()) {
                d_tasks.push(task);
            }
            else {
                task.destroy();
            }
        }
    }

    auto suspend()
    {
        return std::suspend_always{};
    }
};

class CoroutineTaskSchedulerSimple
{
    bsl::queue<ntsa::CoroutineTask<void> > d_tasks;

  public:
    void emplace(ntsa::CoroutineTask<void>&& task)
    {
        d_tasks.push(bsl::move(task));
    }

    void schedule()
    {
        while (!d_tasks.empty()) {
            ntsa::CoroutineTask<void> task = bsl::move(d_tasks.front());
            d_tasks.pop();

            task.coroutine().resume();

            if (!task.coroutine().done()) {
                d_tasks.push(bsl::move(task));
            }
        }
    }
};

class CoroutineHandleSchedulerTest
{
  public:
    static std::suspend_always suspend()
    {
        return std::suspend_always();
    }

    static CoroutineHandleSchedulerTask createTaskA()
    {
        std::cout << "Task A starting\n";
        co_await CoroutineHandleSchedulerTest::suspend();
        std::cout << "Task A resuming - 1\n";
        co_await CoroutineHandleSchedulerTest::suspend();
        std::cout << "Task A resuming - 2\n";
        co_await CoroutineHandleSchedulerTest::suspend();
        std::cout << "Task A resuming - 3\n";
        co_await CoroutineHandleSchedulerTest::suspend();
        std::cout << "Task A complete\n";
    }

    static CoroutineHandleSchedulerTask createTaskB()
    {
        std::cout << "Task B starting\n";
        co_await CoroutineHandleSchedulerTest::suspend();
        std::cout << "Task B resuming - 1\n";
        co_await CoroutineHandleSchedulerTest::suspend();
        std::cout << "Task B resuming - 2\n";
        co_await CoroutineHandleSchedulerTest::suspend();
        std::cout << "Task B resuming - 3\n";
        co_await CoroutineHandleSchedulerTest::suspend();
        std::cout << "Task B complete\n";
    }

    static void test()
    {
        std::cout << '\n';

        CoroutineHandleSchedulerSimple sch;

        sch.emplace(createTaskA().get_handle());
        sch.emplace(createTaskB().get_handle());

        std::cout << "Start scheduling...\n";

        sch.schedule();

        std::cout << '\n';
    }
};

/// This is a scheduler class that schedules coroutines in a round-robin
/// fashion once N coroutines have been scheduled to it.
///
/// Only supports access from a single thread at a time so
///
/// This implementation was inspired by Gor Nishanov's CppCon 2018 talk
/// about nano-coroutines.
///
/// The implementation relies on symmetric transfer and noop_coroutine()
/// and so only works with a relatively recent version of Clang and does
/// not yet work with MSVC.
template <size_t N>
class round_robin_scheduler
{
    static_assert(N >= 2,
                  "Round robin scheduler must be configured to support at "
                  "least two coroutines");

    class schedule_operation
    {
      public:
        explicit schedule_operation(round_robin_scheduler& s) noexcept
        : m_scheduler(s)
        {
        }

        bool await_ready() noexcept
        {
            return false;
        }

        bsl::coroutine_handle<void> await_suspend(
            bsl::coroutine_handle<void> awaitingCoroutine) noexcept
        {
            return m_scheduler.exchange_next(awaitingCoroutine);
        }

        void await_resume() noexcept
        {
        }

      private:
        round_robin_scheduler& m_scheduler;
    };

    friend class schedule_operation;

  public:
    round_robin_scheduler() noexcept : m_index(0),
                                       m_noop(std::noop_coroutine())
    {
        for (size_t i = 0; i < N - 1; ++i) {
            m_coroutines[i] = m_noop;
        }
    }

    ~round_robin_scheduler()
    {
        // All tasks should have been joined before calling destructor.
        BSLS_ASSERT(
            std::all_of(m_coroutines.begin(), m_coroutines.end(), [&](auto h) {
                return h == m_noop;
            }));
    }

    schedule_operation schedule() noexcept
    {
        BSLS_LOG_INFO("Scheduling coroutine\n");

        return schedule_operation{*this};
    }

    /// Resume any queued coroutines until there are no more coroutines.
    void drain() noexcept
    {
        size_t countRemaining = N - 1;
        do {
            auto nextToResume = exchange_next(m_noop);
            if (nextToResume != m_noop) {
                nextToResume.resume();
                countRemaining = N - 1;
            }
            else {
                --countRemaining;
            }
        } while (countRemaining > 0);
    }

  private:
    bsl::coroutine_handle<void> exchange_next(
        bsl::coroutine_handle<void> coroutine) noexcept
    {
        auto coroutineToResume =
            std::exchange(m_coroutines[m_index], coroutine);
        m_index = m_index < (N - 2) ? m_index + 1 : 0;
        return coroutineToResume;
    }

    size_t                                         m_index;
    const bsl::coroutine_handle<void>              m_noop;
    std::array<bsl::coroutine_handle<void>, N - 1> m_coroutines;
};

class RoundRobinSchedulerUtil
{
  public:
    static ntsa::CoroutineTask<void> createTaskA(
        round_robin_scheduler<32>& scheduler)
    {
        std::cout << "Task A starting\n";
        co_await scheduler.schedule();
        std::cout << "Task A resuming - 1\n";
        co_await scheduler.schedule();
        std::cout << "Task A resuming - 2\n";
        co_await scheduler.schedule();
        std::cout << "Task A resuming - 3\n";
        co_await scheduler.schedule();
        std::cout << "Task A complete\n";

        co_return;
    }

    static ntsa::CoroutineTask<void> createTaskB(
        round_robin_scheduler<32>& scheduler)
    {
        std::cout << "Task B starting\n";
        co_await scheduler.schedule();
        std::cout << "Task B resuming - 1\n";
        co_await scheduler.schedule();
        std::cout << "Task B resuming - 2\n";
        co_await scheduler.schedule();
        std::cout << "Task B resuming - 3\n";
        co_await scheduler.schedule();
        std::cout << "Task B complete\n";

        co_return;
    }

    static ntsa::CoroutineTask<void> createTaskScheduler(
        round_robin_scheduler<32>& scheduler)
    {
        std::cout << "Drain starting" << std::endl;
        scheduler.drain();
        std::cout << "Drain complete" << std::endl;
        co_return;
    }

    static ntsa::CoroutineTask<void> coTest(
        round_robin_scheduler<32>& scheduler)
    {
        ntsa::CoroutineTask<void> taskA = createTaskA(scheduler);
        ntsa::CoroutineTask<void> taskB = createTaskB(scheduler);

        ntsa::CoroutineTask<void> taskScheduler =
            createTaskScheduler(scheduler);

        co_await ntsa::CoroutineBarrierUtil::when_all(
            bsl::move(taskA),
            bsl::move(taskB),
            bsl::move(taskScheduler));
    }

    static void test()
    {
        round_robin_scheduler<32> scheduler;

        ntsa::CoroutineTask<void> main = coTest(scheduler);

        ntsa::CoroutineTaskUtil::synchronize(bsl::move(main));
    }
};

typedef round_robin_scheduler<64> CoroutineTaskScheduler;

class CoroutineTaskSchedulerUtil
{
  public:
    static ntsa::CoroutineTask<void> createTaskScheduler(
        CoroutineTaskScheduler* scheduler)
    {
        std::cout << "Drain starting" << std::endl;
        scheduler->drain();
        std::cout << "Drain complete" << std::endl;
        co_return;
    }

    template <typename SCHEDULER, typename AWAITABLE>
    static auto schedule_on(SCHEDULER& scheduler, AWAITABLE awaitable)
        -> ntsa::CoroutineTask<ntsa::CoroutineMeta::remove_rvalue_reference_t<
            typename ntsa::CoroutineMeta::awaitable_traits<
                AWAITABLE>::await_result_t> >
    {
        co_await           scheduler.schedule();
        co_return co_await std::move(awaitable);
    }
};

}  // close namespace ntsa
}  // close namespace BloombergLP

namespace BloombergLP {
namespace ntcf {

// Provide tests for 'ntcf::Concurrent'.
class ConcurrentTest
{
    BALL_LOG_SET_CLASS_CATEGORY("NTCF.CONCURRENT");

    // Describe the configurable parameters of the application simulation.
    class Configuration
    {
      public:
        bsl::size_t    numConnections;
        ntsa::Endpoint endpoint;
    };

    // Create a new scheduler. Allocate memory using the specified 'allocator'.
    static bsl::shared_ptr<ntci::Scheduler> createScheduler(
        ntsa::Allocator allocator);

    // Create a new listener socket bound to any available ephemeral port at
    // the loopback address and begin listening. Allocate memory using the
    // specified 'allocator'.
    static bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         allocator);

    // Create a new stream socket. Allocate memory using the specified
    // 'allocator'.
    static bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const bsl::shared_ptr<ntci::Scheduler>& scheduler,
        ntsa::Allocator                         allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyExecute(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::Allocator                  allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyDatagramSocket(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::Allocator                  allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyStreamSocket(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::Allocator                  allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyApplication(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::CoroutineTaskScheduler*    taskSwitcher,
        ntsa::Allocator                  allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyApplicationListener(
        Configuration                         configuration,
        ntsa::CoroutineTaskScheduler*         taskSwitcher,
        bsl::shared_ptr<ntci::ListenerSocket> listenerSocket,
        ntsa::Allocator                       allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyApplicationClient(
        Configuration                       configuration,
        ntsa::CoroutineTaskScheduler*       taskSwitcher,
        bsl::shared_ptr<ntci::StreamSocket> streamSocket,
        ntsa::Allocator                     allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyApplicationServer(
        Configuration                       configuration,
        ntsa::CoroutineTaskScheduler*       taskSwitcher,
        bsl::shared_ptr<ntci::StreamSocket> streamSocket,
        ntsa::Allocator                     allocator);

    // TODO
    static ntsa::CoroutineTask<void> coVerifySandbox(
        bsl::shared_ptr<ntci::Scheduler> scheduler,
        ntsa::Allocator                  allocator);

  public:
    // TODO
    static void verifyExecute();

    // TODO
    static void verifyDatagramSocket();

    // TODO
    static void verifyStreamSocket();

    // TODO
    static void verifyApplication();

    // TODO
    static void verifySandbox();
};

bsl::shared_ptr<ntci::Scheduler> ConcurrentTest::createScheduler(
    ntsa::Allocator allocator)
{
    ntsa::Error error;

    ntca::SchedulerConfig schedulerConfig;

    schedulerConfig.setThreadName("test");
    schedulerConfig.setMinThreads(4);
    schedulerConfig.setMaxThreads(4);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::System::createScheduler(schedulerConfig, allocator.mechanism());

    error = scheduler->start();
    NTSCFG_TEST_OK(error);

    return scheduler;
}

bsl::shared_ptr<ntci::ListenerSocket> ConcurrentTest::createListenerSocket(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         allocator)
{
    ntsa::Error error;

    ntca::ListenerSocketOptions listenerSocketOptions;

    listenerSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    listenerSocketOptions.setKeepHalfOpen(true);

    listenerSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        scheduler->createListenerSocket(listenerSocketOptions,
                                        allocator.mechanism());

    error = listenerSocket->open();
    NTSCFG_TEST_OK(error);

    error = listenerSocket->listen();
    NTSCFG_TEST_OK(error);

    return listenerSocket;
}

bsl::shared_ptr<ntci::StreamSocket> ConcurrentTest::createStreamSocket(
    const bsl::shared_ptr<ntci::Scheduler>& scheduler,
    ntsa::Allocator                         allocator)
{
    ntsa::Error error;

    ntca::StreamSocketOptions streamSocketOptions;

    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
    streamSocketOptions.setKeepHalfOpen(true);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        scheduler->createStreamSocket(streamSocketOptions,
                                      allocator.mechanism());

    return streamSocket;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyExecute(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::Allocator                  allocator)
{
    ntccfg::Object scope("coVerifyExecute");

    BALL_LOG_DEBUG << "Starting on thread "
                   << bslmt::ThreadUtil::selfIdAsUint64() << BALL_LOG_END;

    for (bsl::size_t i = 0; i < 8; ++i) {
        co_await ntcf::Concurrent::resume(scheduler);

        BALL_LOG_DEBUG << "Resuming on thread "
                       << bslmt::ThreadUtil::selfIdAsUint64() << BALL_LOG_END;
    }

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyDatagramSocket(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::Allocator                  allocator)
{
    ntccfg::Object scope("coVerifyDatagramSocket");

    ntsa::Error error;

    // Create a client datagram socket.

    ntca::DatagramSocketOptions clientDatagramSocketOptions;

    clientDatagramSocketOptions.setTransport(
        ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    clientDatagramSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    bsl::shared_ptr<ntci::DatagramSocket> clientDatagramSocket =
        scheduler->createDatagramSocket(clientDatagramSocketOptions,
                                        allocator.mechanism());

    error = clientDatagramSocket->open();
    NTSCFG_TEST_OK(error);

    // Create a server datagram socket.

    ntca::DatagramSocketOptions serverDatagramSocketOptions;

    serverDatagramSocketOptions.setTransport(
        ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    serverDatagramSocketOptions.setSourceEndpoint(
        ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 0)));

    bsl::shared_ptr<ntci::DatagramSocket> serverDatagramSocket =
        scheduler->createDatagramSocket(serverDatagramSocketOptions,
                                        allocator.mechanism());

    error = serverDatagramSocket->open();
    NTSCFG_TEST_OK(error);

    // Connect the client datagram socket to the server datagram socket.

    ntca::ConnectOptions clientConnectOptions;
    ntci::ConnectResult  clientConnectResult;

    clientConnectResult = co_await ntcf::Concurrent::connect(
        clientDatagramSocket,
        serverDatagramSocket->sourceEndpoint(),
        clientConnectOptions);

    NTSCFG_TEST_OK(clientConnectResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_CONNECT_COMPLETE(clientConnectResult);

    // Connect the server datagram socket to the client datagram socket.

    ntca::ConnectOptions serverConnectOptions;
    ntci::ConnectResult  serverConnectResult;

    serverConnectResult = co_await ntcf::Concurrent::connect(
        serverDatagramSocket,
        clientDatagramSocket->sourceEndpoint(),
        ntca::ConnectOptions());
    NTSCFG_TEST_OK(serverConnectResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_SERVER_CONNECT_COMPLETE(serverConnectResult);

    // Send data from the client datagram socket to the server datagram socket.

    bsl::shared_ptr<bdlbb::Blob> clientSendData =
        clientDatagramSocket->createOutgoingBlob();

    bdlbb::BlobUtil::append(clientSendData.get(), "Hello, world!", 13);

    ntca::SendOptions clientSendOptions;
    ntci::SendResult  clientSendResult;

    clientSendResult = co_await ntcf::Concurrent::send(clientDatagramSocket,
                                                       clientSendData,
                                                       clientSendOptions);
    NTSCFG_TEST_OK(clientSendResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_SEND_COMPLETE(clientSendResult,
                                                  clientSendData);

    // Receive data at the server datagram socket from the client datagram
    // socket.

    ntca::ReceiveOptions serverReceiveOptions;
    ntci::ReceiveResult  serverReceiveResult;

    serverReceiveResult =
        co_await ntcf::Concurrent::receive(serverDatagramSocket,
                                           serverReceiveOptions);
    NTSCFG_TEST_OK(serverReceiveResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(serverReceiveResult);

    // Close the client datagram socket.

    co_await ntcf::Concurrent::close(clientDatagramSocket);
    NTCF_CONCURRENT_TEST_LOG_CLIENT_CLOSED();

    // Close the server datagram socket.

    co_await ntcf::Concurrent::close(serverDatagramSocket);
    NTCF_CONCURRENT_TEST_LOG_SERVER_CLOSED();

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyStreamSocket(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::Allocator                  allocator)
{
    ntccfg::Object scope("coVerifyStreamSocket");

    ntsa::Error error;

    // Create the listener socket and begin listening.

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        ntcf::ConcurrentTest::createListenerSocket(scheduler, allocator);

    // Create a client stream socket.

    bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket =
        ntcf::ConcurrentTest::createStreamSocket(scheduler, allocator);

    // Connect the client stream socket to the listener socket.

    ntca::ConnectOptions clientConnectOptions;
    ntci::ConnectResult  clientConnectResult;

    clientConnectResult =
        co_await ntcf::Concurrent::connect(clientStreamSocket,
                                           listenerSocket->sourceEndpoint(),
                                           clientConnectOptions);
    NTSCFG_TEST_OK(clientConnectResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_CONNECT_COMPLETE(clientConnectResult);

    // Accept the server stream socket from the listener socket.

    ntca::AcceptOptions serverAcceptOptions;
    ntci::AcceptResult  serverAcceptResult;

    serverAcceptResult =
        co_await ntcf::Concurrent::accept(listenerSocket, serverAcceptOptions);
    NTSCFG_TEST_OK(serverAcceptResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_SERVER_ACCEPT_COMPLETE(serverAcceptResult);

    bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket =
        serverAcceptResult.streamSocket();

    // Send data from the client stream socket to the server stream socket.

    bsl::shared_ptr<bdlbb::Blob> clientSendData =
        clientStreamSocket->createOutgoingBlob();

    bdlbb::BlobUtil::append(clientSendData.get(), "Hello, world!", 13);

    ntca::SendOptions clientSendOptions;
    ntci::SendResult  clientSendResult;

    clientSendResult = co_await ntcf::Concurrent::send(clientStreamSocket,
                                                       clientSendData,
                                                       clientSendOptions);
    NTSCFG_TEST_OK(clientSendResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_SEND_COMPLETE(clientSendResult,
                                                  clientSendData);

    // Receive data at the server datagram socket from the client datagram
    // socket.

    ntca::ReceiveOptions serverReceiveOptions;
    ntci::ReceiveResult  serverReceiveResult;

    serverReceiveOptions.setSize(13);

    serverReceiveResult =
        co_await ntcf::Concurrent::receive(serverStreamSocket,
                                           serverReceiveOptions);
    NTSCFG_TEST_OK(serverReceiveResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(serverReceiveResult);

    // Close the client stream socket.

    co_await ntcf::Concurrent::close(clientStreamSocket);
    NTCF_CONCURRENT_TEST_LOG_CLIENT_CLOSED();

    // Close the server stream socket.

    co_await ntcf::Concurrent::close(serverStreamSocket);
    NTCF_CONCURRENT_TEST_LOG_SERVER_CLOSED();

    // Close the listener socket.

    co_await ntcf::Concurrent::close(listenerSocket);
    NTCF_CONCURRENT_TEST_LOG_LISTENER_CLOSED();

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyApplication(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::CoroutineTaskScheduler*    taskSwitcher,
    ntsa::Allocator                  allocator)
{
    ntccfg::Object scope("coVerifyApplication");

    ntsa::Error error;

    std::vector<ntsa::CoroutineTask<void> > taskList;

    ntsa::CoroutineTask<void> schedulerTask =
        ntsa::CoroutineTaskSchedulerUtil::createTaskScheduler(taskSwitcher);

    // Create the listener socket and begin listening.

    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
        ntcf::ConcurrentTest::createListenerSocket(scheduler, allocator);

    Configuration configuration;

    configuration.numConnections = 3;
    configuration.endpoint       = listenerSocket->sourceEndpoint();

    NTCF_CONCURRENT_TEST_LOG_LISTENER_READY(configuration.endpoint);

    // Create a coroutine dedicated to the listener socket.

    ntsa::CoroutineTask<void> listenerTask =
        coVerifyApplicationListener(configuration,
                                    taskSwitcher,
                                    listenerSocket,
                                    allocator);

    taskList.emplace_back(bsl::move(listenerTask));

    for (bsl::size_t i = 0; i < configuration.numConnections; ++i) {
        // Create a client stream socket.

        bsl::shared_ptr<ntci::StreamSocket> streamSocket =
            ntcf::ConcurrentTest::createStreamSocket(scheduler, allocator);

        // Create a coroutine dedicated to the client stream socket.

        ntsa::CoroutineTask<void> clientTask =
            coVerifyApplicationClient(configuration,
                                      taskSwitcher,
                                      streamSocket,
                                      allocator);

        taskList.emplace_back(bsl::move(clientTask));
    }

    taskList.emplace_back(bsl::move(schedulerTask));

    // Run all coroutines until complete.

    co_await ntsa::CoroutineBarrierUtil::when_all(bsl::move(taskList));

    // taskSwitcher.schedule();

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyApplicationListener(
    Configuration                         configuration,
    ntsa::CoroutineTaskScheduler*         taskSwitcher,
    bsl::shared_ptr<ntci::ListenerSocket> listenerSocket,
    ntsa::Allocator                       allocator)
{
    ntccfg::Object scope("coVerifyApplicationListener");

    // co_await taskSwitcher->schedule();

#if 0
    BALL_LOG_DEBUG << "Starting listener coroutine on thread "
                   << bslmt::ThreadUtil::selfIdAsUint64() << BALL_LOG_END;

    co_await ntcf::Concurrent::resume(listenerSocket);

    BALL_LOG_DEBUG << "Resuming listener coroutine on thread "
                   << bslmt::ThreadUtil::selfIdAsUint64() << BALL_LOG_END;
#endif

    ntsa::Error error;

    for (bsl::size_t i = 0; i < configuration.numConnections; ++i) {
        ntccfg::Object scope(
            "coVerifyApplicationListener: for-loop iteration");

        // Accept a server stream socket from the listener socket.

        ntca::AcceptOptions acceptOptions;
        ntci::AcceptResult  acceptResult;

        acceptResult =
            co_await ntcf::Concurrent::accept(listenerSocket, acceptOptions);
        NTSCFG_TEST_OK(acceptResult.event().context().error());

        NTCF_CONCURRENT_TEST_LOG_SERVER_ACCEPT_COMPLETE(acceptResult);

        // Create a coroutine dedicated to the server stream socket.

        ntsa::CoroutineTask<void> serverTask =
            coVerifyApplicationServer(configuration,
                                      taskSwitcher,
                                      acceptResult.streamSocket(),
                                      allocator);

        // taskSwitcher->emplace(bsl::move(serverTask));
        //

        // ntsa::CoroutineTaskSchedulerUtil::schedule_on(*taskSwitcher,
        //                                               bsl::move(serverTask));

        // MRM: This serializes the processing of each connection.
        co_await serverTask;
    }

    // Close the listener socket.

    co_await ntcf::Concurrent::close(listenerSocket);
    NTCF_CONCURRENT_TEST_LOG_LISTENER_CLOSED();

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyApplicationClient(
    Configuration                       configuration,
    ntsa::CoroutineTaskScheduler*       taskSwitcher,
    bsl::shared_ptr<ntci::StreamSocket> streamSocket,
    ntsa::Allocator                     allocator)
{
    ntccfg::Object scope("coVerifyApplicationClient");

    ntsa::Error error;

    // co_await taskSwitcher->schedule();

    // Connect the client stream socket to the listener socket.

    ntca::ConnectOptions connectOptions;
    ntci::ConnectResult  connectResult;

    connectResult = co_await ntcf::Concurrent::connect(streamSocket,
                                                       configuration.endpoint,
                                                       connectOptions);
    NTSCFG_TEST_OK(connectResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_CONNECT_COMPLETE(connectResult);

    // Send data to the peer.

    bsl::shared_ptr<bdlbb::Blob> sendData = streamSocket->createOutgoingBlob();

    bdlbb::BlobUtil::append(sendData.get(), "Hello, world!", 13);

    ntca::SendOptions sendOptions;
    ntci::SendResult  sendResult;

    sendResult =
        co_await ntcf::Concurrent::send(streamSocket, sendData, sendOptions);

    NTSCFG_TEST_OK(sendResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_SEND_COMPLETE(sendResult, sendData);

    // Receive data from the peer.

    ntca::ReceiveOptions receiveOptions;
    ntci::ReceiveResult  receiveResult;

    receiveOptions.setSize(13);

    receiveResult =
        co_await ntcf::Concurrent::receive(streamSocket, receiveOptions);
    NTSCFG_TEST_OK(receiveResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_CLIENT_RECEIVE_COMPLETE(receiveResult);

    // Close the socket.

    co_await ntcf::Concurrent::close(streamSocket);
    NTCF_CONCURRENT_TEST_LOG_CLIENT_CLOSED();

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifyApplicationServer(
    Configuration                       configuration,
    ntsa::CoroutineTaskScheduler*       taskSwitcher,
    bsl::shared_ptr<ntci::StreamSocket> streamSocket,
    ntsa::Allocator                     allocator)
{
    BALL_LOG_INFO << "Processing accepted socket" << BALL_LOG_END;

    ntccfg::Object scope("coVerifyApplicationServer");

    ntsa::Error error;

    // co_await taskSwitcher->schedule();

    // Receive data from the peer.

    ntca::ReceiveOptions receiveOptions;
    ntci::ReceiveResult  receiveResult;

    receiveOptions.setSize(13);

    receiveResult =
        co_await ntcf::Concurrent::receive(streamSocket, receiveOptions);
    NTSCFG_TEST_OK(receiveResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_SERVER_RECEIVE_COMPLETE(receiveResult);

    // Send data to the peer.

    bsl::shared_ptr<bdlbb::Blob> sendData = receiveResult.data();

    ntca::SendOptions sendOptions;
    ntci::SendResult  sendResult;

    sendResult =
        co_await ntcf::Concurrent::send(streamSocket, sendData, sendOptions);

    NTSCFG_TEST_OK(sendResult.event().context().error());

    NTCF_CONCURRENT_TEST_LOG_SERVER_SEND_COMPLETE(sendResult, sendData);

    // Close the socket.

    co_await ntcf::Concurrent::close(streamSocket);
    NTCF_CONCURRENT_TEST_LOG_SERVER_CLOSED();

    co_return;
}

ntsa::CoroutineTask<void> ConcurrentTest::coVerifySandbox(
    bsl::shared_ptr<ntci::Scheduler> scheduler,
    ntsa::Allocator                  allocator)
{
    NTCCFG_WARNING_UNUSED(scheduler);
    NTCCFG_WARNING_UNUSED(allocator);

    co_return;
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyExecute)
{
    ntccfg::Object scope("verifyExecute");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler =
        ntcf::ConcurrentTest::createScheduler(allocator);

    ntci::SchedulerStopGuard schedulerGuard(scheduler);

    ntsa::CoroutineTask<void> task = coVerifyExecute(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyDatagramSocket)
{
    ntccfg::Object scope("verifyDatagramSocket");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task =
        coVerifyDatagramSocket(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyStreamSocket)
{
    ntccfg::Object scope("verifyStreamSocket");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task =
        coVerifyStreamSocket(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifyApplication)
{
    ntccfg::Object scope("verifyApplication");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTaskScheduler taskScheduler;

    ntsa::CoroutineTask<void> task =
        coVerifyApplication(scheduler, &taskScheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
}

NTSCFG_TEST_FUNCTION(ntcf::ConcurrentTest::verifySandbox)
{
#if 0
    ntccfg::Object scope("verifySandbox");

    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntci::Scheduler> scheduler;
    ntcf::System::getDefault(&scheduler);

    ntsa::CoroutineTask<void> task = coVerifySandbox(scheduler, allocator);
    ntsa::CoroutineTaskUtil::synchronize(bsl::move(task));
#endif

    BALL_LOG_INFO << "Testing simple scheduler" << BALL_LOG_END;

    ntsa::CoroutineHandleSchedulerTest::test();

    BALL_LOG_INFO << "Testing round-robin scheduler" << BALL_LOG_END;

    ntsa::RoundRobinSchedulerUtil::test();
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
