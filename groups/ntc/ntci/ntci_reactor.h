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

#ifndef INCLUDED_NTCI_REACTOR
#define INCLUDED_NTCI_REACTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_loadbalancingoptions.h>
#include <ntca_reactorevent.h>
#include <ntca_reactoreventoptions.h>
#include <ntca_reactoreventtype.h>
#include <ntca_waiteroptions.h>
#include <ntccfg_platform.h>
#include <ntci_callback.h>
#include <ntci_datapool.h>
#include <ntci_driver.h>
#include <ntci_executor.h>
#include <ntci_reactorpool.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_timer.h>
#include <ntci_timerfactory.h>
#include <ntci_waiter.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_notificationqueue.h>
#include <ntsi_descriptor.h>
#include <bdlbb_blob.h>
#include <bdls_filesystemutil.h>
#include <bslmt_threadutil.h>
#include <bsl_functional.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntci {
class ReactorSocket;
}
namespace ntci {

/// Define a type alias for a callback invoked on an optional
/// strand with an optional cancelable authorization mechanism when a socket
/// becomes readable, writable, or encounters an error.
typedef ntci::Callback<void(const ntca::ReactorEvent& event)>
    ReactorEventCallback;

/// Define a type alias for a callback invoked on an optional
/// strand with an optional cancelable authorization mechanism when a socket
/// needs to process a notification.
typedef ntci::Callback<void(const ntsa::NotificationQueue& notifications)>
    ReactorNotificationCallback;

/// Define a type alias for function invoked when a reactor
/// event occurs.
typedef ReactorEventCallback::FunctionType ReactorEventFunction;

/// Provide an interface to the reactor asynchronous model.
///
/// @details
/// In the reactor pattern, a resource is polled until conditions are suitable
/// for an operation to be synchronously performed on that resource.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example
/// This example illustrates how to poll a reactor for events to learn when
/// sockets are readable and writable. First, for the purposes of explaining
/// this usage in a linear fashion, implement a reactor event callback function
/// that will simply post to a semaphore.
///
///     void processEvent(bslmt::Semaphore          *semaphore,
///                       const ntca::ReactorEvent&  event)
///     {
///         semaphore->post();
///     }
///
/// Next, create a reactor and configure it automatically attach sockets to
/// the underlying polling device when any interest for that socket is gained,
/// and automaticaly detach the socket from the underlying polling device when
/// all interest for that socket is lost.
///
///     ntsa::Error error;
///
///     ntca::ReactorConfig reactorConfig;
///     reactorConfig.setMinThreads(1);
///     reactorConfig.setMaxThreads(1);
///     reactorConfig.setAutoAttach(true);
///     reactorConfig.setAutoDetach(true);
///
///     bsl::shared_ptr<ntci::Reactor> reactor =
///         ntcf::System::createReactor(reactorConfig);
///
/// Next, create a reactor event callback that will post to a semaphore when
/// the event is announced.
///
///     bslmt::Semaphore semaphore;
///     ntci::ReactorEventCallback reactorEventCallback =
///         reactor->createReactorEventCallback(
///             bdlf::BindUtil::bind(&processEvent,
///                                  &semaphore,
///                                  bdlf::PlaceHolders::_1));
///
/// Next, register the thread that will poll the reactor.
///
///     ntca::WaiterOptions waiterOptions;
///     waiterOptions.setThreadHandle(bslmt::ThreadUtil::self());
///
///     ntci::Waiter waiter = reactor->registerWaiter(waiterOptions);
///
/// Now, create a listener socket.
///
///     ntsa::Handle listenerSocket;
///     error = ntsf::System::createListenerSocket(
///         &listenerSocket,
///         ntsa::Transport::e_TCP_IPV4_STREAM);
///     BSLS_ASSERT(!error);
///
/// Set the listener socket as non-blocking.
///
///     error = ntsf::System::setBlocking(listenerSocket, false);
///
/// Bind the listener socket to any port of the loopback address.
///
///     error = ntsf::System::bind(listenerSocket,
///                                ntsa::Endpoint("127.0.0.1:0"),
///                                false);
///     BSLS_ASSERT(!error);
///
/// Get the endpoint to which the listener socket was bound.
///
///     ntsa::Endpoint listenerSocketSourceEndpoint;
///     error = ntsf::System::getSourceEndpoint(&listenerSocketSourceEndpoint,
///                                             listenerSocket);
///     BSLS_ASSERT(!error);
///
/// Begin listening for connections.
///
///     error = ntsf::System::listen(listenerSocket, 1);
///     BSLS_ASSERT(!error);
///
/// Next, create a client stream socket to connect to the listener socket.
///
///     ntsa::Handle clientStreamSocket;
///     error = ntsf::System::createStreamSocket(
///         &clientStreamSocket,
///         ntsa::Transport::e_TCP_IPV4_STREAM);
///     BSLS_ASSERT(!error);
///
/// Set the client stream socket as non-blocking.
///
///     error = ntsf::System::setBlocking(listenerSocket, false);
///
/// Connect the client stream socket to the endpoint to which the listener
/// socket is bound.
///
///     error = ntsf::System::connect(clientStreamSocket,
///                                   listenerSocketSourceEndpoint);
///     BSLS_ASSERT(!error);
///
/// Gain interest in the writability of the client stream socket to learn
/// when the connection may be complete.
///
///     error = reactor->showWritable(clientStreamSocket,
///                                   ntca::ReactorEventOptions(),
///                                   reactorEventCallback);
///     BSLS_ASSERT(!error);
///
/// Poll until the client stream socket is writable.
///
///     while (semaphore.tryWait() != 0) {
///         reactor->poll(waiter);
///     }
///
/// Get the source endpoint to which the client stream socket is bound.
///
///     ntsa::Endpoint clientStreamSocketSourceEndpoint;
///     error = ntsf::System::getSourceEndpoint(
///                                    &clientStreamSocketSourceEndpoint,
///                                    clientStreamSocket);
///     BSLS_ASSERT(!error);
///
/// Get the remote endpoint to which the client stream socket is connected.
///
///     ntsa::Endpoint clientStreamSocketRemoteEndpoint;
///     error = ntsf::System::getRemoteEndpoint(
///                                    &clientStreamSocketRemoteEndpoint,
///                                    clientStreamSocket);
///     BSLS_ASSERT(!error);
///
/// Verify the client stream socket remote address matches the listener socket
/// source address.
///
///     BSLS_ASSERT(clientStreamSocketRemoteEndpoint ==
///                 listenerSocketSourceEndpoint);
///
/// Lose interest in the writability of the client stream socket.
///
///     error = reactor->hideWritable(clientStreamSocket);
///     BSLS_ASSERT(!error);
///
/// Gain interest in the readability of the listener socket to learn when a
/// connection may be accepted.
///
///     error = reactor->showReadable(listenerSocket,
///                                   ntca::ReactorEventOptions(),
///                                   reactorEventCallback);
///     BSLS_ASSERT(!error);
///
/// Poll until the listener socket is readable.
///
///     while (semaphore.tryWait() != 0) {
///         reactor->poll(waiter);
///     }
///
/// Accept a server stream socket.
///
///     ntsa::Handle serverStreamSocket;
///     error = ntsf::System::accept(&serverStreamSocket, listenerSocket);
///     BSLS_ASSERT(!error);
///
/// Set the server stream socket as non-blocking.
///
///     error = ntsf::System::setBlocking(serverStreamSocket, false);
///     BSLS_ASSERT(!error);
///
/// Get the source endpoint to which the server stream socket is bound.
///
///     ntsa::Endpoint serverStreamSocketSourceEndpoint;
///     error = ntsf::System::getSourceEndpoint(
///                                    &serverStreamSocketSourceEndpoint,
///                                    serverStreamSocket);
///     BSLS_ASSERT(!error);
///
/// Get the remote endpoint to which the server stream socket is connected.
///
///     ntsa::Endpoint serverStreamSocketRemoteEndpoint;
///     error = ntsf::System::getRemoteEndpoint(
///                                    &serverStreamSocketRemoteEndpoint,
///                                    serverStreamSocket);
///     BSLS_ASSERT(!error);
///
/// Verify the server stream socket remote address matches the client stream
/// socket source address.
///
///     BSLS_ASSERT(serverStreamSocketRemoteEndpoint ==
///                 clientStreamSocketSourceEndpoint);
///
/// Lose interest in the readability of the listener socket.
///
///     error = reactor->hideReadable(listenerSocket);
///     BSLS_ASSERT(!error);
///
/// Send one byte from the client stream socket to the server stream socket.
///
///     ntsa::SendContext sendContext;
///
///     const char sendStorage = 'X';
///     ntsa::Data sendData(
///         ntsa::ConstBuffer(&sendStorage, sizeof sendStorage));
///
///     error = ntsf::System::send(&sendContext,
///                                sendData,
///                                ntsa::SendOptions(),
///                                clientStreamSocket);
///     BSLS_ASSERT(!error);
///
///     BSLS_ASSERT(sendContext.bytesSendable() == 1);
///     BSLS_ASSERT(sendContext.bytesSent()     == 1);
///
/// Gain interest in the readability of the server stream socket to learn
/// when data is available to receive.
///
///     error = reactor->showReadable(serverStreamSocket,
///                                   ntca::ReactorEventOptions(),
///                                   reactorEventCallback);
///     BSLS_ASSERT(!error);
///
/// Poll until the server stream socket is readable.
///
///     while (semaphore.tryWait() != 0) {
///         reactor->poll(waiter);
///     }
///
/// Receive one byte at the server stream socket sent by the client stream
/// socket.
///
///     ntsa::ReceiveContext receiveContext;
///
///     char receiveStorage;
///     ntsa::Data receiveData(
///         ntsa::MutableBuffer(&receiveStorage, sizeof receiveStorage));
///
///     error = ntsf::System::receive(&receiveContext,
///                                   &receiveData,
///                                   ntsa::ReceiveOptions(),
///                                   serverStreamSocket);
///     BSLS_ASSERT(!error);
///
///     BSLS_ASSERT(receiveContext.bytesReceivable() == 1);
///     BSLS_ASSERT(receiveContext.bytesReceived()   == 1);
///
/// Verify the data received by the server stream socket matches the data
/// sent by the client stream socket.
///
///     BSLS_ASSERT(receiveStorage == sendStorage);
///
/// Lose interest in the readability of the server stream socket.
///
///     error = reactor->hideReadable(serverStreamSocket);
///     BSLS_ASSERT(!error);
///
/// Close the client stream socket.
///
///     error = ntsf::System::close(clientStreamSocket);
///     BSLS_ASSERT(!error);
///
/// Close the server stream socket.
///
///     error = ntsf::System::close(serverStreamSocket);
///     BSLS_ASSERT(!error);
///
/// Close the listener socket.
///
///     error = ntsf::System::close(listenerSocket);
///     BSLS_ASSERT(!error);
///
/// Deregister the thread that is polling the reactor.
///
///     reactor->deregisterWaiter(waiter);
///
/// @ingroup module_ntci_reactor
class Reactor : public ntci::Driver, public ntci::ReactorPool
{
  public:
    /// Destroy this object.
    virtual ~Reactor();

    /// Create a new reactor event callback to invoke the specified
    /// 'function' with no cancellable authorization mechanism on an
    /// unspecified strand.  Optionally specify a  'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    ntci::ReactorEventCallback createReactorEventCallback(
        const ntci::ReactorEventFunction& function,
        bslma::Allocator*                 basicAllocator = 0);

    /// Create a new reactor event callback to invoke the specified
    /// 'function' with the specified cancellable 'authorization' mechanism
    /// on an unspecified strand. Optionally specify a  'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    ntci::ReactorEventCallback createReactorEventCallback(
        const ntci::ReactorEventFunction&           function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        bslma::Allocator*                           basicAllocator = 0);

    /// Create a new reactor event callback to invoke the specified
    /// 'function' with no cancellable authorization mechanism on the
    /// specified 'strand'.  Optionally specify a  'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    ntci::ReactorEventCallback createReactorEventCallback(
        const ntci::ReactorEventFunction&    function,
        const bsl::shared_ptr<ntci::Strand>& strand,
        bslma::Allocator*                    basicAllocator = 0);

    /// Create a new reactor event callback to invoke the specified
    /// 'function' with the specified cancellable 'authorization' mechanism
    /// on the specified 'strand'. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    ntci::ReactorEventCallback createReactorEventCallback(
        const ntci::ReactorEventFunction&           function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        const bsl::shared_ptr<ntci::Strand>&        strand,
        bslma::Allocator*                           basicAllocator = 0);

    /// Start monitoring the specified 'socket'. Return the error.
    virtual ntsa::Error attachSocket(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket) = 0;

    /// Start monitoring the specified socket 'handle'. Return the error.
    virtual ntsa::Error attachSocket(ntsa::Handle handle) = 0;

    /// Start monitoring the specified 'socket' for the specified
    /// 'eventType' according to the specified 'options'. Return the error.
    ntsa::Error show(const bsl::shared_ptr<ntci::ReactorSocket>& socket,
                     ntca::ReactorEventType::Value               eventType,
                     const ntca::ReactorEventOptions&            options);

    /// Start monitoring the specified socket 'handle' for the specified
    /// 'eventType' according to the specified 'options'. Invoke the
    /// specified 'callback' when an event of that type occurs. Return the
    /// error.
    ntsa::Error show(ntsa::Handle                      handle,
                     ntca::ReactorEventType::Value     eventType,
                     const ntca::ReactorEventOptions&  options,
                     const ntci::ReactorEventCallback& callback);

    /// Start monitoring for readability of the specified 'socket' according
    /// to the specified 'options'. Return the error.
    virtual ntsa::Error showReadable(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntca::ReactorEventOptions&            options) = 0;

    /// Start monitoring for readability of the specified socket 'handle'
    /// according to the specified 'options'. Invoke the specified
    /// 'callback' when the socket becomes readable. Return the error.
    virtual ntsa::Error showReadable(
        ntsa::Handle                      handle,
        const ntca::ReactorEventOptions&  options,
        const ntci::ReactorEventCallback& callback) = 0;

    /// Start monitoring for writability of the specified 'socket' according
    /// to the specified 'options'. Return the error.
    virtual ntsa::Error showWritable(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntca::ReactorEventOptions&            options) = 0;

    /// Start monitoring for writability of the specified socket 'handle'
    /// according to the specified 'options'. Invoke the specified
    /// 'callback' when the socket becomes writable. Return the error.
    virtual ntsa::Error showWritable(
        ntsa::Handle                      handle,
        const ntca::ReactorEventOptions&  options,
        const ntci::ReactorEventCallback& callback) = 0;

    /// Start monitoring for errors of the specified 'socket'. Return
    /// the error.
    virtual ntsa::Error showError(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket,
        const ntca::ReactorEventOptions&            options) = 0;

    /// Start monitoring for errors of the specified socket 'handle'. Invoke
    /// the specified 'callback' when the socket has an error. Return the
    /// error.
    virtual ntsa::Error showError(
        ntsa::Handle                      handle,
        const ntca::ReactorEventOptions&  options,
        const ntci::ReactorEventCallback& callback) = 0;

    /// Start monitoring for notifications of the specified 'socket'. Return
    /// the error.
    virtual ntsa::Error showNotifications(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket);

    /// Start monitoring for notifications of the specified socket 'handle'.
    /// Invoke the specified 'callback' when the socket has a notification.
    /// Return the error.
    virtual ntsa::Error showNotifications(
        ntsa::Handle                             handle,
        const ntci::ReactorNotificationCallback& callback);

    /// Stop monitoring the specified 'socket' for the specified
    /// 'eventType'. Return the error.
    ntsa::Error hide(const bsl::shared_ptr<ntci::ReactorSocket>& socket,
                     ntca::ReactorEventType::Value               eventType);

    /// Stop monitoring the specified socket 'handle' for the specified
    /// 'eventType'. Return the error.
    ntsa::Error hide(ntsa::Handle                  handle,
                     ntca::ReactorEventType::Value eventType);

    /// Stop monitoring for readability of the specified 'socket'. Return
    /// the error.
    virtual ntsa::Error hideReadable(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket) = 0;

    /// Stop monitoring for readability of the specified socket
    /// 'handle'. Return the error.
    virtual ntsa::Error hideReadable(ntsa::Handle handle) = 0;

    /// Stop monitoring for writability of the specified 'socket'.
    virtual ntsa::Error hideWritable(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket) = 0;

    /// Stop monitoring for writability of the specified socket
    /// 'handle'. Return the error.
    virtual ntsa::Error hideWritable(ntsa::Handle handle) = 0;

    /// Stop monitoring for notifications of the specified 'socket'. Return the
    /// error.
    virtual ntsa::Error hideNotifications(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket);

    /// Stop monitoring for notifications of the specified socket 'handle'.
    /// Return the error.
    virtual ntsa::Error hideNotifications(ntsa::Handle handle);

    /// Stop monitoring for errors of the specified 'socket'. Return the
    /// error.
    virtual ntsa::Error hideError(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket) = 0;

    /// Stop monitoring for errors of the specified socket
    /// 'handle'. Return the error.
    virtual ntsa::Error hideError(ntsa::Handle handle) = 0;

    /// Stop monitoring the specified 'socket'. Return the error.
    virtual ntsa::Error detachSocket(
        const bsl::shared_ptr<ntci::ReactorSocket>& socket) = 0;

    /// Stop monitoring the specified socket 'handle'. Return the error.
    virtual ntsa::Error detachSocket(ntsa::Handle handle) = 0;

    /// Close all monitored sockets and timers.
    virtual ntsa::Error closeAll() = 0;

    /// Increment the estimation of the load on the reactor according to
    /// the specified load balancing 'options'.
    virtual void incrementLoad(const ntca::LoadBalancingOptions& options) = 0;

    /// Decrement the estimation of the load on the reactor according to
    /// the specified load balancing 'options'.
    virtual void decrementLoad(const ntca::LoadBalancingOptions& options) = 0;

    /// Execute all deferred functions managed by this object.
    virtual void drainFunctions() = 0;

    /// Clear all deferred functions managed by this object.
    virtual void clearFunctions() = 0;

    /// Clear all timers managed by this object.
    virtual void clearTimers() = 0;

    /// Clear all sockets managed by this object.
    virtual void clearSockets() = 0;

    /// Clear all resources managed by this object.
    virtual void clear() = 0;

    /// Return the number of sockets currently being monitored.
    virtual bsl::size_t numSockets() const = 0;

    /// Return the maximum number of sockets capable of being monitored
    /// at one time.
    virtual bsl::size_t maxSockets() const = 0;

    /// Return the number of timers currently being monitored.
    virtual bsl::size_t numTimers() const = 0;

    /// Return the maximum number of timers capable of being monitored
    /// at one time.
    virtual bsl::size_t maxTimers() const = 0;

    /// Return the flag that indicates a socket should be automatically
    /// attached to the reactor when interest in any event for a sodcket is
    /// gained.
    virtual bool autoAttach() const = 0;

    /// Return the flag that indicates a socket should be automatically
    /// detached from the reactor when interest in all events for the socket
    /// is lost.
    virtual bool autoDetach() const = 0;

    /// Return the default one-shot mode of the delivery of events. When
    /// one-shot mode is enabled, after a reactor detects the socket is
    /// readable or writable, interest in readability or writability must be
    /// explicitly re-registered before the reactor will again detect the
    /// socket is readable or writable. Note that the one-shot mode may
    /// be overridden on a per-socket, per-event basis at the time a socket
    /// registers interest in readability or writability.
    virtual bool oneShot() const = 0;

    /// Return the default trigger mode of the detection of events. When
    /// events are level-triggered, the event will occur as long as the
    /// conditions for the event continue to be satisfied. When events are
    /// edge-triggered, the event is raised when conditions for the event
    /// change are first satisfied, but the event is not subsequently raised
    /// until the conditions are "reset".
    virtual ntca::ReactorEventTrigger::Value trigger() const = 0;

    /// Return the estimation of the load on the reactor.
    virtual bsl::size_t load() const = 0;

    /// Return the handle of the thread that drives this reactor, or
    /// the default value if no such thread has been set.
    virtual bslmt::ThreadUtil::Handle threadHandle() const = 0;

    /// Return the index in the thread pool of the thread that drives this
    /// reactor, or 0 if no such thread has been set.
    virtual bsl::size_t threadIndex() const = 0;

    /// Return true if the reactor has no pending deferred functors no
    /// pending timers, and no registered sockets, otherwise return false.
    virtual bool empty() const = 0;

    /// Return the data pool.
    virtual const bsl::shared_ptr<ntci::DataPool>& dataPool() const = 0;

    /// Return true if the reactor supports registering events in the
    /// specified 'oneShot' mode, otherwise return false.
    virtual bool supportsOneShot(bool oneShot) const = 0;

    /// Return true if the reactor supports registering events having the
    /// specified 'trigger', otherwise return false.
    virtual bool supportsTrigger(
        ntca::ReactorEventTrigger::Value trigger) const = 0;

    /// Return true if the reactor supports notifications of the socket,
    /// otherwise return false
    virtual bool supportsNotifications() const;
};

NTCCFG_INLINE
ntci::ReactorEventCallback Reactor::createReactorEventCallback(
    const ntci::ReactorEventFunction& function,
    bslma::Allocator*                 basicAllocator)
{
    return ntci::ReactorEventCallback(function, basicAllocator);
}

NTCCFG_INLINE
ntci::ReactorEventCallback Reactor::createReactorEventCallback(
    const ntci::ReactorEventFunction&           function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    bslma::Allocator*                           basicAllocator)
{
    return ntci::ReactorEventCallback(function, authorization, basicAllocator);
}

NTCCFG_INLINE
ntci::ReactorEventCallback Reactor::createReactorEventCallback(
    const ntci::ReactorEventFunction&    function,
    const bsl::shared_ptr<ntci::Strand>& strand,
    bslma::Allocator*                    basicAllocator)
{
    return ntci::ReactorEventCallback(function, strand, basicAllocator);
}

NTCCFG_INLINE
ntci::ReactorEventCallback Reactor::createReactorEventCallback(
    const ntci::ReactorEventFunction&           function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    const bsl::shared_ptr<ntci::Strand>&        strand,
    bslma::Allocator*                           basicAllocator)
{
    return ntci::ReactorEventCallback(function,
                                      authorization,
                                      strand,
                                      basicAllocator);
}

NTCCFG_INLINE
ntsa::Error Reactor::show(const bsl::shared_ptr<ntci::ReactorSocket>& socket,
                          ntca::ReactorEventType::Value    eventType,
                          const ntca::ReactorEventOptions& options)
{
    if (eventType == ntca::ReactorEventType::e_READABLE) {
        return this->showReadable(socket, options);
    }
    else if (eventType == ntca::ReactorEventType::e_WRITABLE) {
        return this->showWritable(socket, options);
    }
    else if (eventType == ntca::ReactorEventType::e_ERROR) {
        return this->showError(socket, options);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTCCFG_INLINE
ntsa::Error Reactor::show(ntsa::Handle                      handle,
                          ntca::ReactorEventType::Value     eventType,
                          const ntca::ReactorEventOptions&  options,
                          const ntci::ReactorEventCallback& callback)
{
    if (eventType == ntca::ReactorEventType::e_READABLE) {
        return this->showReadable(handle, options, callback);
    }
    else if (eventType == ntca::ReactorEventType::e_WRITABLE) {
        return this->showWritable(handle, options, callback);
    }
    else if (eventType == ntca::ReactorEventType::e_ERROR) {
        return this->showError(handle, options, callback);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTCCFG_INLINE
ntsa::Error Reactor::hide(const bsl::shared_ptr<ntci::ReactorSocket>& socket,
                          ntca::ReactorEventType::Value eventType)
{
    if (eventType == ntca::ReactorEventType::e_READABLE) {
        return this->hideReadable(socket);
    }
    else if (eventType == ntca::ReactorEventType::e_WRITABLE) {
        return this->hideWritable(socket);
    }
    else if (eventType == ntca::ReactorEventType::e_ERROR) {
        return this->hideError(socket);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTCCFG_INLINE
ntsa::Error Reactor::hide(ntsa::Handle                  handle,
                          ntca::ReactorEventType::Value eventType)
{
    if (eventType == ntca::ReactorEventType::e_READABLE) {
        return this->hideReadable(handle);
    }
    else if (eventType == ntca::ReactorEventType::e_WRITABLE) {
        return this->hideWritable(handle);
    }
    else if (eventType == ntca::ReactorEventType::e_ERROR) {
        return this->hideError(handle);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTCCFG_INLINE
bool Reactor::supportsNotifications() const
{
    return false;
}

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
