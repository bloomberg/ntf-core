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

#ifndef INCLUDED_NTCF_SYSTEM
#define INCLUDED_NTCF_SYSTEM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntcf_api.h>
#include <ntcscm_version.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcf {

/// Provide a factory of objects implemented by the operating system.
///
/// @details
/// This struct provides utilities to create objects that implement abstract
/// interfaces using the operating system. These utilities serve as the entry
/// point for creating objects in a production execution environment.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @par Usage Example: Creating Scheduler for Asynchronous Sockets
/// This example illustrates how to create and run a pool of I/O threads,
/// create a listener socket driven by one of those I/O threads, create a
/// client socket driven by one of those I/O threads that connects to the
/// listener socket, accept the server socket from the listener socket,
/// exchange data between the client socket and the server socket, then close
/// all sockets, with all operations performed asynchronously where the user is
/// notified of the completion of each asynchronous operation through a
/// callback specified when each operation is initiated. For the purposes of
/// this example, we'll assume each operation will be successful, and for
/// brevity, some error handling is omitted. Note that this example is nearly
/// identical to the previous example, except for the choice of mechanism used
/// to manage the I/O threads.
///
/// To begin, let's implement that callbacks for each operation. First, let's
/// implement the connect callback. This callback will assert the specified
/// 'event' indicates an asynchronous 'connect' operation on the specified
/// 'streamSocket' completed successfully then post to the specified
/// 'semaphore'.
///
///     void processConnect(
///                 const bsl::shared_ptr<ntci::StreamSocket>&  streamSocket,
///                 const ntca::ConnectEvent&                   event,
///                 bslmt::Semaphore                           *semaphore)
///     {
///         BSLS_ASSERT(event.type() == ntca::ConnectEventType::e_COMPLETE);
///         semaphore->post();
///     }
///
/// Next, let's implement the accept callback. This callback will assert the
/// specified 'event' indicates an asynchronous 'accept' operation on the
/// specified 'listenerSocket' completed successfully then assign the specified
/// 'streamSocket' (which is the accepted socket) to the specified 'result'
/// then post to the specified 'semaphore'.
///
///     void processAccept(
///              const bsl::shared_ptr<ntci::ListenerSocket>&  listenerSocket,
///              const bsl::shared_ptr<ntci::StreamSocket>&    streamSocket,
///              const ntca::AcceptEvent&                      event,
///              bslmt::Semaphore                             *semaphore,
///              bsl::shared_ptr<ntci::StreamSocket>          *result)
///     {
///         BSLS_ASSERT(event.type() == ntca::AcceptEventType::e_COMPLETE);
///         *result = streamSocket;
///         semaphore->post();
///     }
///
/// Next, let's implement the send callback. This callback will assert the
/// specified 'event' indicates an asynchronous 'send' operation on the
/// specified 'streamSocket' completed successfully then post to the specified
/// 'semaphore'.
///
///     void processSend(
///                 const bsl::shared_ptr<ntci::StreamSocket>&  streamSocket,
///                 const ntca::SendEvent&                      event,
///                 bslmt::Semaphore                           *semaphore)
///     {
///         BSLS_ASSERT(event.type() == ntca::SendEventType::e_COMPLETE);
///         semaphore->post();
///     }
///
/// Next, let's implement the receive callback. This callback will assert the
/// specified 'event' indicates an asynchronous 'send' operation on the
/// specified 'streamSocket' completed successfully then assign the specified
/// 'data' (which is the data received) to the specified 'result' then post to
/// the specified 'semaphore'.
///
///     void processReceive(
///                 const bsl::shared_ptr<ntci::StreamSocket>&  streamSocket,
///                 const bsl::shared_ptr<bdlbb::Blob>&         data,
///                 const ntca::ReceiveEvent&                   event,
///                 bslmt::Semaphore                           *semaphore,
///                 bdlbb::Blob                                *result)
///     {
///         BSLS_ASSERT(event.type() == ntca::ReceiveEventType::e_COMPLETE);
///         *result = *data;
///         semaphore->post();
///     }
///
/// Next, let's implement the close callback. This callback will post to the
/// specified 'semaphore'.
///
///     void processClose(bslmt::Semaphore *semaphore)
///     {
///         semaphore->post();
///     }
///
/// Now, let's implement the main parts of the example. First, initialize the
/// library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, create and start a pool of I/O threads. For this example, we'll
/// use 3 threads, and observe that each socket will be assigned to a distinct
/// thread in the pool.
///
///     ntsa::Error      error;
///     bslmt::Semaphore semaphore;
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(3);
///     interfaceConfig.setMaxThreads(3);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Next, create a listener socket and begin listening.
///
///     ntca::ListenerSocketOptions listenerSocketOptions;
///     listenerSocketOptions.setSourceEndpoint(
///         ntsa::Endpoint(ntsa::Ipv4Address::loopback(), 0));
///
///     bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
///         interface->createListenerSocket(listenerSocketOptions);
///
///     error = listenerSocket->open();
///     BSLS_ASSERT(!error);
///
/// Next, connect a socket to the listener.
///
///    ntca::StreamSocketOptions streamSocketOptions;
///
///     bsl::shared_ptr<ntci::StreamSocket> clientSocket =
///         interface->createStreamSocket(streamSocketOptions);
///
///     ntci::ConnectCallback connectCallback =
///         clientSocket->createConnectCallback(
///             bdlf::BindUtil::bind(&processConnect,
///                                  bdlf::PlaceHolders::_1,
///                                  bdlf::PlaceHolders::_2,
///                                  &semaphore));
///
///     error = clientSocket->connect(listenerSocket->sourceEndpoint(),
//                                    ntca::ConnectOptions(),
//                                    connectCallback);
///     BSLS_ASSERT(!error);
///
///     semaphore.wait();
///
/// Next, accept a connection from the listener socket's backlog.
///
///     bsl::shared_ptr<ntci::StreamSocket> serverSocket;
///
///     ntci::AcceptCallback acceptCallback =
///         listenerSocket->createAcceptCallback(
///             bdlf::BindUtil::bind(&processAccept,
///                                  bdlf::PlaceHolders::_1,
///                                  bdlf::PlaceHolders::_2,
///                                  bdlf::PlaceHolders::_3,
///                                  &semaphore,
///                                  &serverSocket));
///
///     error = listenerSocket->accept(ntca::AcceptOptions(), acceptCallback);
///     BSLS_ASSERT(!error || error == ntsa::Error::e_WOULD_BLOCK);
///
///     semaphore.wait();
///
/// Next, send some data from the client to the server.
///
///     bdlbb::Blob clientData(
///                 clientSocket->outgoingBlobBufferFactory().get());
///
///     bdlbb::BlobUtil::append(&clientData, "Hello, world!", 13);
///
///     ntci::SendCallback sendCallback =
///         clientSocket->createSendCallback(
///             bdlf::BindUtil::bind(&processSend,
///                                  bdlf::PlaceHolders::_1,
///                                  bdlf::PlaceHolders::_2,
///                                  &semaphore));
///
///     error = clientSocket->send(clientData,
///                                ntca::SendOptions(),
///                                sendCallback);
///     BSLS_ASSERT(!error);
///
///     semaphore.wait();
///
/// Next, receive the expected amount of data from the client.
///
///     ntca::ReceiveOptions receiveOptions;
///     receiveOptions.setSize(13);
///
///     bdlbb::Blob serverData;
///
///     ntci::ReceiveCallback receiveCallback =
///         serverSocket->createReceiveCallback(
///             bdlf::BindUtil::bind(&processReceive,
///                                  bdlf::PlaceHolders::_1,
///                                  bdlf::PlaceHolders::_2,
///                                  bdlf::PlaceHolders::_3,
///                                  &semaphore,
///                                  &serverData));
///
///     error = serverSocket->receive(receiveOptions, receiveCallback);
///     BSLS_ASSERT(!error || error == ntsa::Error::e_WOULD_BLOCK);
///
///     semaphore.wait();
///
///     BSLS_ASSERT(bdlbb::BlobUtil::compare(clientData, serverData) == 0);
///
/// Next, close the client socket.
///
///     {
///         ntci::CloseCallback closeCallback =
///             clientSocket->createCloseCallback(
///                 bdlf::BindUtil::bind(&processClose, &semaphore));
///
///         clientSocket->close(closeCallback);
///         semaphore.wait();
///     }
///
/// Next, close the server socket.
///
///     {
///         ntci::CloseCallback closeCallback =
///             serverSocket->createCloseCallback(
///                 bdlf::BindUtil::bind(&processClose, &semaphore));
///
///         serverSocket->close(closeCallback);
///         semaphore.wait();
///     }
///
/// Next, close the listener socket.
///
///     {
///         ntci::CloseCallback closeCallback =
///             listenerSocket->createCloseCallback(
///                 bdlf::BindUtil::bind(&processClose, &semaphore));
///
///         listenerSocket->close(closeCallback);
///         semaphore.wait();
///     }
///
/// Finally, stop the thread pool.
///
///     interface->shutdown();
///     interface->linger();
///
/// @ingroup module_ntci_runtime
struct System {
  public:
    /// Initialize the current process to meet the requirements of this
    /// library and acquire the resources necessary for its implementation.
    /// Return the error. Note that this function will register the
    /// corresponding 'exit' function to be automatically invoked at program
    /// termination, but the 'exit' function may also be explicitly called
    /// to control when resources used by this library are released. Also
    /// note that this function is automatically called if any other
    /// function in this utility is called (other that 'exit'). Users are
    /// encouraged to call this function in 'main' before any other
    /// networking functions are called.
    static ntsa::Error initialize();

    /// Ignore the specified 'signal'. Return the error.
    static ntsa::Error ignore(ntscfg::Signal::Value signal);

    /// Create a new scheduler having the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Scheduler> createScheduler(
        const ntca::SchedulerConfig& configuration,
        bslma::Allocator*            basicAllocator = 0);

    /// Create a new scheduler having the specified 'configuration'.
    /// Allocate blob buffers for incoming data using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntci::Scheduler> createScheduler(
        const ntca::SchedulerConfig&                     configuration,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Create a new scheduler having the specified 'configuration'.
    /// Allocate data containers using the specified 'dataPool'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Scheduler> createScheduler(
        const ntca::SchedulerConfig&           configuration,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator*                      basicAllocator = 0);

    /// Create a new interface having the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Interface> createInterface(
        const ntca::InterfaceConfig& configuration,
        bslma::Allocator*            basicAllocator = 0);

    /// Create a new interface having the specified 'configuration'.
    /// Allocate blob buffers for incoming data using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntci::Interface> createInterface(
        const ntca::InterfaceConfig&                     configuration,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Create a new interface having the specified 'configuration'.
    /// Allocate data containers using the specified 'dataPool'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<ntci::Interface> createInterface(
        const ntca::InterfaceConfig&           configuration,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator*                      basicAllocator = 0);

    /// Create a new thread with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    static bsl::shared_ptr<ntci::Thread> createThread(
        const ntca::ThreadConfig& configuration,
        bslma::Allocator*         basicAllocator = 0);

    /// Create a new thread with the specified 'configuration' that will run
    /// the specified 'reactor'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntci::Thread> createThread(
        const ntca::ThreadConfig&             configuration,
        const bsl::shared_ptr<ntci::Reactor>& reactor,
        bslma::Allocator*                     basicAllocator = 0);

    /// Create a new thread with the specified 'configuration' that will run
    /// the specified 'proactor'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntci::Thread> createThread(
        const ntca::ThreadConfig&              configuration,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        bslma::Allocator*                      basicAllocator = 0);

    /// Create a new driver with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    static bsl::shared_ptr<ntci::Driver> createDriver(
        const ntca::DriverConfig& configuration,
        bslma::Allocator*         basicAllocator = 0);

    /// Create a new reactor with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    static bsl::shared_ptr<ntci::Reactor> createReactor(
        const ntca::ReactorConfig& configuration,
        bslma::Allocator*          basicAllocator = 0);

    /// Create new reactor metrics for the specified 'objectName' whose
    /// field names have the specified 'prefix'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    static bsl::shared_ptr<ntci::ReactorMetrics> createReactorMetrics(
        const bsl::string& prefix,
        const bsl::string& objectName,
        bslma::Allocator*  basicAllocator = 0);

    /// Create new reactor metrics for the specified 'objectName' whose
    /// field names have the specified 'prefix'. Aggregate updates into the
    /// specified 'parent'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntci::ReactorMetrics> createReactorMetrics(
        const bsl::string&                           prefix,
        const bsl::string&                           objectName,
        const bsl::shared_ptr<ntci::ReactorMetrics>& parent,
        bslma::Allocator*                            basicAllocator = 0);

    /// Create a new proactor with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    static bsl::shared_ptr<ntci::Proactor> createProactor(
        const ntca::ProactorConfig& configuration,
        bslma::Allocator*           basicAllocator = 0);

    /// Create nea proactor metrics for the specified 'objectName' whose
    /// field names have the specified 'prefix'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    static bsl::shared_ptr<ntci::ProactorMetrics> createProactorMetrics(
        const bsl::string& prefix,
        const bsl::string& objectName,
        bslma::Allocator*  basicAllocator = 0);

    /// Create new proactor metrics for the specified 'objectName' whose
    /// field names have the specified 'prefix'. Aggregate updates into the
    /// specified 'parent'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntci::ProactorMetrics> createProactorMetrics(
        const bsl::string&                            prefix,
        const bsl::string&                            objectName,
        const bsl::shared_ptr<ntci::ProactorMetrics>& parent,
        bslma::Allocator*                             basicAllocator = 0);

    /// Create a new datagram socket with the specified 'configuration'
    /// driven by the specified 'reactor' operating in the environment of
    /// the specified 'user'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntci::DatagramSocket> createDatagramSocket(
        const ntca::DatagramSocketOptions&    configuration,
        const bsl::shared_ptr<ntci::Reactor>& reactor,
        const bsl::shared_ptr<ntci::User>&    user,
        bslma::Allocator*                     basicAllocator = 0);

    /// Create a new datagram socket with the specified 'configuration'
    /// driven by the specified 'proactor' operating in the environment of
    /// the specified 'user'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntci::DatagramSocket> createDatagramSocket(
        const ntca::DatagramSocketOptions&     configuration,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const bsl::shared_ptr<ntci::User>&     user,
        bslma::Allocator*                      basicAllocator = 0);

    /// Create a new listener socket with the specified 'configuration'
    /// driven by the specified 'reactor' operating in the environment of
    /// the specified 'user'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions&    configuration,
        const bsl::shared_ptr<ntci::Reactor>& reactor,
        const bsl::shared_ptr<ntci::User>&    user,
        bslma::Allocator*                     basicAllocator = 0);

    /// Create a new listener socket with the specified 'configuration'
    /// driven by the specified 'proactor' operating in the environment of
    /// the specified 'user'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions&     configuration,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const bsl::shared_ptr<ntci::User>&     user,
        bslma::Allocator*                      basicAllocator = 0);

    /// Create a new stream socket with the specified 'configuration'
    /// driven by the specified 'reactor' operating in the environment of
    /// the specified 'user'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions&      configuration,
        const bsl::shared_ptr<ntci::Reactor>& reactor,
        const bsl::shared_ptr<ntci::User>&    user,
        bslma::Allocator*                     basicAllocator = 0);

    /// Create a new stream socket with the specified 'configuration'
    /// driven by the specified 'proactor' operating in the environment of
    /// the specified 'user'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions&       configuration,
        const bsl::shared_ptr<ntci::Proactor>& proactor,
        const bsl::shared_ptr<ntci::User>&     user,
        bslma::Allocator*                      basicAllocator = 0);

    /// Create a new rate limiter with the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    static bsl::shared_ptr<ntci::RateLimiter> createRateLimiter(
        const ntca::RateLimiterConfig& configuration,
        bslma::Allocator*              basicAllocator = 0);

    /// Create a new data pool using the default sizes for incoming and
    /// outgoing blob buffers. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static bsl::shared_ptr<ntci::DataPool> createDataPool(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new data pool using the specified 'blobBufferFactory' to
    /// supply blob buffers for incoming and outgoing data. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    static bsl::shared_ptr<ntci::DataPool> createDataPool(
        bdlbb::BlobBufferFactory* blobBufferFactory,
        bslma::Allocator*         basicAllocator = 0);

    /// Create a new data pool using the specified 'incomingBlobBufferSize'
    /// and 'outgoingBlobBufferSize'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    static bsl::shared_ptr<ntci::DataPool> createDataPool(
        bsl::size_t       incomingBlobBufferSize,
        bsl::size_t       outgoingBlobBufferSize,
        bslma::Allocator* basicAllocator = 0);

    /// Create a new data pool using the specified
    /// 'incomingBlobBufferFactory' and 'outgoingBlobBufferFactory'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    ///  used.
    static bsl::shared_ptr<ntci::DataPool> createDataPool(
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>&
            incomingBlobBufferFactory,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>&
                          outgoingBlobBufferFactory,
        bslma::Allocator* basicAllocator = 0);

    /// Create a new resolver with the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    static bsl::shared_ptr<ntci::Resolver> createResolver(
        const ntca::ResolverConfig& configuration,
        bslma::Allocator*           basicAllocator = 0);

    /// Create a new cancelable callback authorization mechanism. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    ///  used.
    static bsl::shared_ptr<ntci::Authorization> createAuthorization(
        bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        bslma::Allocator*                        basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>*         result,
        const ntca::EncryptionClientOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator*                        basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        bslma::Allocator*                        basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>*         result,
        const ntca::EncryptionServerOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator*                        basicAllocator = 0);

    /// Load into the specified 'result' a new encryption resource. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used. Return the
    /// error.
    static ntsa::Error createEncryptionResource(
        bsl::shared_ptr<ntci::EncryptionResource>* result,
        bslma::Allocator*                          basicAllocator = 0);

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by itself. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*                  result,
        const ntsa::DistinguishedName&                subjectIdentity,
        const ntca::EncryptionKey&                    subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified
    /// by the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            subjectIdentity,
        const ntca::EncryptionKey&                subjectPrivateKey,
        const ntca::EncryptionCertificate&        issuerCertificate,
        const ntca::EncryptionKey&                issuerPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator*                         basicAllocator = 0);

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by itself. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntsa::DistinguishedName&                subjectIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&   subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified
    /// by the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
        const ntsa::DistinguishedName&                      subjectIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&         subjectPrivateKey,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& issuerCertificate,
        const bsl::shared_ptr<ntci::EncryptionKey>&         issuerPrivateKey,
        const ntca::EncryptionCertificateOptions&           options,
        bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a certificate stored at the specified
    /// 'path' in the Privacy Enhanced Mail (PEM) format. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.  Return the error.
    static ntsa::Error loadCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bsl::string&                            path,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate stored at the specified
    /// 'path' according to the specified 'options'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.  Return the error.
    static ntsa::Error loadCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bsl::string&                            path,
        const ntca::EncryptionResourceOptions&        options,
        bslma::Allocator*                             basicAllocator = 0);

    /// Save the specified 'certificate' to the specified 'path' in the Privacy
    /// Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error saveCertificate(
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const bsl::string&                                  path);

    /// Save the specified 'certificate' to the specified 'path' according to
    /// the specified 'options'. Return the error.
    static ntsa::Error saveCertificate(
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const bsl::string&                                  path,
        const ntca::EncryptionResourceOptions&              options);

    /// Encode the specified 'certificate' to the specified 'destination'
    /// in the Privacy Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error encodeCertificate(
        bsl::streambuf*                                     destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    /// Encode the specified 'certificate' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    static ntsa::Error encodeCertificate(
        bsl::streambuf*                                     destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const ntca::EncryptionResourceOptions&              options);

    /// Encode the specified 'certificate' to the specified 'destination'
    /// in the Privacy Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error encodeCertificate(
        bdlbb::Blob*                                        destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    /// Encode the specified 'certificate' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    static ntsa::Error encodeCertificate(
        bdlbb::Blob*                                        destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const ntca::EncryptionResourceOptions&              options);

    /// Encode the specified 'certificate' to the specified 'destination'
    /// in the Privacy Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error encodeCertificate(
        bsl::string*                                        destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    /// Encode the specified 'certificate' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    static ntsa::Error encodeCertificate(
        bsl::string*                                        destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const ntca::EncryptionResourceOptions&              options);

    /// Encode the specified 'certificate' to the specified 'destination'
    /// in the Privacy Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error encodeCertificate(
        bsl::vector<char>*                                  destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    /// Encode the specified 'certificate' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    static ntsa::Error encodeCertificate(
        bsl::vector<char>*                                  destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const ntca::EncryptionResourceOptions&              options);

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        bsl::streambuf*                               source,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' according to the specified 'options'. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        bsl::streambuf*                               source,
        const ntca::EncryptionResourceOptions&        options,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bdlbb::Blob&                            source,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' according to the specified 'options'. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bdlbb::Blob&                            source,
        const ntca::EncryptionResourceOptions&        options,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bsl::string&                            source,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' according to the specified 'options'. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bsl::string&                            source,
        const ntca::EncryptionResourceOptions&        options,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bsl::vector<char>&                      source,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' according to the specified 'options'. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bsl::vector<char>&                      source,
        const ntca::EncryptionResourceOptions&        options,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' an RSA key generated according to
    /// the specified 'options'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error generateKey(
        ntca::EncryptionKey*                  result,
        const ntca::EncryptionKeyOptions&     options,
        bslma::Allocator*                     basicAllocator = 0);

    /// Load into the specified 'result' an RSA key generated according to
    /// the specified 'options'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error generateKey(
        bsl::shared_ptr<ntci::EncryptionKey>* result,
        const ntca::EncryptionKeyOptions&     options,
        bslma::Allocator*                     basicAllocator = 0);

    /// Load into the specified 'result' a private key stored at the specified
    /// 'path' in the Privacy Enhanced Mail (PEM) format. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.  Return the error.
    static ntsa::Error loadKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                               const bsl::string&                    path,
                               bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key stored at the specified
    /// 'path' according to the specified 'options'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.  Return the error.
    static ntsa::Error loadKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                               const bsl::string&                     path,
                               const ntca::EncryptionResourceOptions& options,
                               bslma::Allocator* basicAllocator = 0);

    /// Save the specified 'privateKey' to the specified 'path' in the Privacy
    /// Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error saveKey(
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const bsl::string&                          path);

    /// Save the specified 'privateKey' to the specified 'path' according to
    /// the specified 'options'. Return the error.
    static ntsa::Error saveKey(
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const bsl::string&                          path,
        const ntca::EncryptionResourceOptions&      options);

    /// Encode the specified 'privateKey' to the specified 'destination' in the
    /// Privacy Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error encodeKey(
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey);

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    static ntsa::Error encodeKey(
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions&      options);

    /// Encode the specified 'privateKey' to the specified 'destination' in the
    /// Privacy Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error encodeKey(
        bdlbb::Blob*                                destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey);

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    static ntsa::Error encodeKey(
        bdlbb::Blob*                                destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions&      options);

    /// Encode the specified 'privateKey' to the specified 'destination' in the
    /// Privacy Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error encodeKey(
        bsl::string*                                destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey);

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    static ntsa::Error encodeKey(
        bsl::string*                                destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions&      options);

    /// Encode the specified 'privateKey' to the specified 'destination' in the
    /// Privacy Enhanced Mail (PEM) format. Return the error.
    static ntsa::Error encodeKey(
        bsl::vector<char>*                          destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey);

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    static ntsa::Error encodeKey(
        bsl::vector<char>*                          destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions&      options);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                 bsl::streambuf*                       source,
                                 bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return the
    /// error. Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error decodeKey(
        bsl::shared_ptr<ntci::EncryptionKey>*  result,
        bsl::streambuf*                        source,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                 const bdlbb::Blob&                    source,
                                 bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return the
    /// error. Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error decodeKey(
        bsl::shared_ptr<ntci::EncryptionKey>*  result,
        const bdlbb::Blob&                     source,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                 const bsl::string&                    source,
                                 bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return the
    /// error. Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error decodeKey(
        bsl::shared_ptr<ntci::EncryptionKey>*  result,
        const bsl::string&                     source,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                 const bsl::vector<char>&              source,
                                 bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return the
    /// error. Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error decodeKey(
        bsl::shared_ptr<ntci::EncryptionKey>*  result,
        const bsl::vector<char>&               source,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);

    /// Set the default monitorable object registry to an object with the
    /// specified 'configuration' and enable the registration of monitorable
    /// objects with that default registry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableRegistry(
        const ntca::MonitorableRegistryConfig& configuration,
        bslma::Allocator*                      basicAllocator = 0);

    /// Set a default monitorable object registry to the specified
    /// 'monitorableRegistry' and enable the registration of monitorable
    /// object with that default registry. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableRegistry(
        const bsl::shared_ptr<ntci::MonitorableRegistry>& monitorableRegistry);

    /// Disable the registration of monitorable objects with the default
    /// registry and unset the default monitorable object registry, if any.
    static void disableMonitorableRegistry();

    /// Set the default monitorable object collector to an object with the
    /// specified 'configuration' and enable the periodic collection of
    /// monitorable objects by that default collector. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableCollector(
        const ntca::MonitorableCollectorConfig& configuration,
        bslma::Allocator*                       basicAllocator = 0);

    /// Set the default monitorable object collector to the specified
    /// 'monitorableCollector' and enable the periodic collection of
    /// monitorable objects by that default collector. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator is 0, the
    /// global allocator is used.
    static void enableMonitorableCollector(
        const bsl::shared_ptr<ntci::MonitorableCollector>&
            monitorableCollector);

    /// Disable the periodic collection of monitorable objects by the
    /// default monitorable object collector and unset the default
    /// monitorable object collector, if any.
    static void disableMonitorableCollector();

    /// Enable the periodic collection of process-wide metrics.
    static void enableProcessMetrics();

    /// Disable the periodic collection of process-wide metrics.
    static void disableProcessMetrics();

    /// Add the specified 'monitorable' to the default monitorable object
    /// registry, if a default monitorable object registry has been enabled.
    static void registerMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& monitorable);

    /// Remove the specified 'monitorable' from the default monitorable
    /// object registry, if a default monitorable object registry has been
    /// enabled.
    static void deregisterMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& monitorable);

    /// Add the specified 'monitorablePublisher' to the default monitorable
    /// object collector, if a default monitorable object collector has been
    /// set.
    static void registerMonitorablePublisher(
        const bsl::shared_ptr<ntci::MonitorablePublisher>&
            monitorablePublisher);

    /// Remove the specified 'monitorablePublisher' from the default
    /// monitorable object collector, if a default monitorable object
    /// collector has been set.
    static void deregisterMonitorablePublisher(
        const bsl::shared_ptr<ntci::MonitorablePublisher>&
            monitorablePublisher);

    /// Add a monitorable publisher to the BSLS log at the specified
    /// 'severityLevel'.
    static void registerMonitorablePublisher(
        bsls::LogSeverity::Enum severityLevel);

    /// Remove a monitorable publisher to the BSLS log at the specified
    /// 'severityLevel'.
    static void deregisterMonitorablePublisher(
        bsls::LogSeverity::Enum severityLevel);

    /// Force the collection of statistics from each monitorable object
    /// registered with the default monitorable object registry and publish
    /// their statistics through each registered publisher.
    static void collectMetrics();

    /// Return true if the specified 'driverName' is supported on the
    /// current platform with the specified 'dynamicLoadBalancing' behavior,
    /// otherwise return false.
    static bool testDriverSupport(const bsl::string& driverName,
                                  bool               dynamicLoadBalancing);

    /// Load into the specified 'driverNames' each supported driver on the
    /// current platform with the specified 'dynamicLoadBalancing' behavior.
    static void loadDriverSupport(bsl::vector<bsl::string>* driverNames,
                                  bool dynamicLoadBalancing);

    /// Register the specified 'encryptionDriver'. Return the error.
    static ntsa::Error registerEncryptionDriver(
        const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver);

    /// Deregister the specified 'encryptionDriver'. Return the error.
    static ntsa::Error deregisterEncryptionDriver(
        const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver);

    /// Return true if an encryption driver has been registered, and false
    /// otherwise.
    static bool supportsEncryptionDriver();

    /// Register the specified 'reactorFactory' to be able to produce
    /// reactors implemented by the specified 'driverName'. Return the
    /// error.
    static ntsa::Error registerReactorFactory(
        const bsl::string&                           driverName,
        const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory);

    /// Deregister the specified 'reactorFactory' from being able to produce
    /// reactors implemented by the specified 'driverName'. Return the
    /// error.
    static ntsa::Error deregisterReactorFactory(
        const bsl::string&                           driverName,
        const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory);

    /// Load into the specified 'result' the registered reactor factory that
    /// is able to produce reactors implemented by the specified
    /// 'driverName'. Return the error.
    static ntsa::Error lookupReactorFactory(
        bsl::shared_ptr<ntci::ReactorFactory>* result,
        const bsl::string&                     driverName);

    /// Return true if a reactor factory has been registered to be able to
    /// produce reactors implemented by the specified 'driverName', and
    /// false otherwise.
    static bool supportsReactorFactory(const bsl::string& driverName);

    /// Register the specified 'proactorFactory' to be able to produce
    /// proactors implemented by the specified 'driverName'. Return the
    /// error.
    static ntsa::Error registerProactorFactory(
        const bsl::string&                            driverName,
        const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory);

    /// Deregister the specified 'proactorFactory' from being able to produce
    /// proactors implemented by the specified 'driverName'. Return the
    /// error.
    static ntsa::Error deregisterProactorFactory(
        const bsl::string&                            driverName,
        const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory);

    /// Load into the specified 'result' the registered proactor factory that
    /// is able to produce proactors implemented by the specified
    /// 'driverName'. Return the error.
    static ntsa::Error lookupProactorFactory(
        bsl::shared_ptr<ntci::ProactorFactory>* result,
        const bsl::string&                      driverName);

    /// Return true if a proactor factory has been registered to be able to
    /// produce proactors implemented by the specified 'driverName', and
    /// false otherwise.
    static bool supportsProactorFactory(const bsl::string& driverName);

    // Install the specified 'executor' as the default executor.
    static void setDefault(const bsl::shared_ptr<ntci::Executor>& executor);

    // Install the specified 'strand' as the default strand.
    static void setDefault(const bsl::shared_ptr<ntci::Strand>& strand);

    // Install the specified 'driver' as the default driver.
    static void setDefault(const bsl::shared_ptr<ntci::Driver>& driver);

    // Install the specified 'reactor' as the default reactor.
    static void setDefault(const bsl::shared_ptr<ntci::Reactor>& reactor);

    // Install the specified 'proactor' as the default proactor.
    static void setDefault(const bsl::shared_ptr<ntci::Proactor>& proactor);

    // Install the specified 'interface' as the default interface.
    static void setDefault(const bsl::shared_ptr<ntci::Interface>& interface);

    // Install the specified 'resolver' as the default resolver.
    static void setDefault(const bsl::shared_ptr<ntci::Resolver>& resolver);

    // Load into the specified 'result' the default executor. If no default
    // executor is explicitly installed, automatically create install a default
    // executor with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Executor>* result);

    // Load into the specified 'result' the default strand. If no default
    // strand is explicitly installed, automatically create install a default
    // strand with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Strand>* result);

    // Load into the specified 'result' the default driver. If no default
    // driver is explicitly installed, automatically create install a default
    // driver with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Driver>* result);

    // Load into the specified 'result' the default reactor. If no default
    // reactor is explicitly installed, automatically create install a default
    // reactor with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Reactor>* result);

    // Load into the specified 'result' the default proactor. If no default
    // proactor is explicitly installed, automatically create install a default
    // proactor with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Proactor>* result);

    // Load into the specified 'result' the default interface. If no default
    // interface is explicitly installed, automatically create install a default
    // interface with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Interface>* result);

    // Load into the specified 'result' the default resolver. If no default
    // resolver is explicitly installed, automatically create install a default
    // resolver with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Resolver>* result);

    /// Release the resources necessary for this library's implementation.
    static void exit();
};

/// Provide a guard to automatically initialize the resources required by this
/// library upon construction and release all resources used by this library
/// upon destruction.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class SystemGuard
{
  private:
    SystemGuard(const SystemGuard&) BSLS_KEYWORD_DELETED;
    SystemGuard& operator=(const SystemGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Initialize the current process to meet the requirements of this
    /// library and acquire the resources necessary for its implementation.
    /// The behavior is undefined if any resource required by this library
    /// cannot be acquired.  Users are encouraged to create an instance of
    /// this system guard in 'main' before any other networking functions
    /// are called.
    SystemGuard();

    /// Initialize the current process to meet the requirements of this
    /// library and acquire the resources necessary for its implementation.
    /// Ignore the specified 'signal' in this process.  The behavior is
    /// undefined if any resource required by this library cannot be
    /// acquired.  Users are encouraged to create an instance of this system
    /// guard in 'main' before any other networking functions are called.
    explicit SystemGuard(ntscfg::Signal::Value signal);

    /// Release the resources necessary for this library's implementation.
    ~SystemGuard();
};

}  // close package namespace
}  // close enterprise namespace
#endif
