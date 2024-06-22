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

#ifndef INCLUDED_NTCDNS_CLIENT
#define INCLUDED_NTCDNS_CLIENT

#include <ntcscm_version.h>

#include <ntcdns_cache.h>
#include <ntcdns_protocol.h>
#include <ntcdns_utility.h>
#include <ntcdns_vocabulary.h>

#include <ntca_getdomainnamecontext.h>
#include <ntca_getdomainnameoptions.h>
#include <ntca_getipaddresscontext.h>
#include <ntca_getipaddressoptions.h>
#include <ntci_callback.h>
#include <ntci_datagramsocket.h>
#include <ntci_datagramsocketfactory.h>
#include <ntci_getdomainnamecallback.h>
#include <ntci_getipaddresscallback.h>
#include <ntci_interface.h>
#include <ntci_streamsocket.h>
#include <ntci_streamsocketfactory.h>
#include <ntsa_error.h>
#include <ntsf_system.h>
#include <ntsi_resolver.h>

#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>

#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>

#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {
class Resolver;
}
namespace ntcdns {
class Client;
}
namespace ntcdns {
class ClientNameServer;
}
namespace ntcdns {

/// @internal @brief
/// Provide an interface for any operation performed by the client.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class ClientOperation
{
  public:
    /// Destroy this object.
    virtual ~ClientOperation();

    /// Send a request to perform this operation through the specified
    /// 'datagramSocket' to the name server at the specified 'endpoint'.
    /// Identify the request using the specified 'transactionId'. Return the
    /// error.
    virtual ntsa::Error sendRequest(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntsa::Endpoint&                        endpoint,
        bsl::uint16_t                                transactionId) = 0;

    /// Send a request to perform this operation through the specified
    /// 'streamSocket' to the name server at the specified 'endpoint'.
    /// Identify the request using the specified 'transactionId'. Return the
    /// error.
    virtual ntsa::Error sendRequest(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocketSocket,
        const ntsa::Endpoint&                      endpoint,
        bsl::uint16_t                              transactionId) = 0;

    /// Invoke the response callback with the contents of the specified
    /// 'response' received from the specified 'endpoint' at the specified
    /// 'serverIndex'.
    virtual void processResponse(const ntcdns::Message&    response,
                                 const ntsa::Endpoint&     endpoint,
                                 bsl::size_t               serverIndex,
                                 const bsls::TimeInterval& now) = 0;

    /// Invoke the response callback with the specified 'error'.
    virtual void processError(const ntsa::Error& error) = 0;

    /// Prepare the operation to target the next name server and return that
    /// name server, or null if all name servers have been tried.
    virtual bsl::shared_ptr<ntcdns::ClientNameServer> tryNextServer() = 0;

    /// Prepare the operation to target the next name in the search list.
    /// Return true if such a name exists, and false otherwise.
    virtual bool tryNextSearch() = 0;
};

/// @internal @brief
/// Provide a mechanism to perform an operation to get the IP addresses
/// assigned to a domain name.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class ClientGetIpAddressOperation
: public ClientOperation,
  public ntccfg::Shared<ClientGetIpAddressOperation>
{
  public:
    /// This type defines a list of question names to try when performing
    /// the operation.
    typedef bsl::vector<bsl::string> SearchList;

    /// Define a type alias for a list of name servers to
    /// try when performing the operation.
    typedef bsl::vector<bsl::shared_ptr<ntcdns::ClientNameServer> > ServerList;

  private:
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                  d_object;
    mutable Mutex                   d_mutex;
    bsl::shared_ptr<ntci::Resolver> d_resolver_sp;
    const bsl::string               d_name;
    ServerList                      d_serverList;
    bsl::size_t                     d_serverIndex;
    const SearchList                d_searchList;
    bsl::size_t                     d_searchIndex;
    const ntca::GetIpAddressOptions d_options;
    ntci::GetIpAddressCallback      d_callback;
    bsl::shared_ptr<ntci::Timer>    d_timer_sp;
    bsl::shared_ptr<ntcdns::Cache>  d_cache_sp;
    bsls::AtomicBool                d_pending;
    bslma::Allocator*               d_allocator_p;

  private:
    ClientGetIpAddressOperation(ClientGetIpAddressOperation&)
        BSLS_KEYWORD_DELETED;
    ClientGetIpAddressOperation& operator=(const ClientGetIpAddressOperation&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Defines a type alias for a vector of endpoints.
    typedef bsl::vector<ntsa::Endpoint> EndpointList;

    /// Create a new get IP address operation to get the IP addresses
    /// assigned to the specified 'name' according to the specified
    /// 'options' and invoke the specified 'callback' when the operation
    /// completes or fails. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    ClientGetIpAddressOperation(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const bsl::string&                     name,
        const ServerList&                      serverList,
        const SearchList&                      searchList,
        const ntca::GetIpAddressOptions&       options,
        const ntci::GetIpAddressCallback&      callback,
        const bsl::shared_ptr<ntcdns::Cache>&  cache,
        bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~ClientGetIpAddressOperation() BSLS_KEYWORD_OVERRIDE;

    /// Send a request to perform this operation through the specified
    /// 'datagramSocket' to the name server at the specified 'endpoint'.
    /// Identify the request using the specified 'transactionId'. Return the
    /// error.
    ntsa::Error sendRequest(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntsa::Endpoint&                        endpoint,
        bsl::uint16_t transactionId) BSLS_KEYWORD_OVERRIDE;

    /// Send a request to perform this operation through the specified
    /// 'streamSocket' to the name server at the specified 'endpoint'.
    /// Identify the request using the specified 'transactionId'. Return the
    /// error.
    ntsa::Error sendRequest(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocketSocket,
        const ntsa::Endpoint&                      endpoint,
        bsl::uint16_t transactionId) BSLS_KEYWORD_OVERRIDE;

    /// Invoke the response callback with the contents of the specified
    /// 'response' received from the specified 'endpoint' at the specified
    /// 'serverIndex'.
    void processResponse(const ntcdns::Message&    response,
                         const ntsa::Endpoint&     endpoint,
                         bsl::size_t               serverIndex,
                         const bsls::TimeInterval& now) BSLS_KEYWORD_OVERRIDE;

    /// Invoke the response callback with the specified 'error'.
    void processError(const ntsa::Error& error) BSLS_KEYWORD_OVERRIDE;

    /// Prepare the operation to target the next name server and return that
    /// name server, or null if all name servers have been tried.
    bsl::shared_ptr<ntcdns::ClientNameServer> tryNextServer()
        BSLS_KEYWORD_OVERRIDE;

    /// Prepare the operation to target the next name in the search list.
    /// Return true if such a name exists, and false otherwise.
    bool tryNextSearch() BSLS_KEYWORD_OVERRIDE;

    /// Return the name to resolve.
    const bsl::string& name() const;

    /// The options that control the behavior of the operation.
    const ntca::GetIpAddressOptions& options() const;

    /// Return the index of the current name server being tried.
    bsl::size_t serverIndex() const;

    /// Return the index of the current search domain being tried.
    bsl::size_t searchIndex() const;
};

/// @internal @brief
/// Provide a mechanism to perform an operation to get the domain name to which
/// an IP address is assigned.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class ClientGetDomainNameOperation
: public ClientOperation,
  public ntccfg::Shared<ClientGetDomainNameOperation>
{
  public:
    /// Define a type alias for a list of name servers to
    /// try when performing the operation.
    typedef bsl::vector<bsl::shared_ptr<ntcdns::ClientNameServer> > ServerList;

  private:
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                   d_object;
    mutable Mutex                    d_mutex;
    bsl::shared_ptr<ntci::Resolver>  d_resolver_sp;
    const ntsa::IpAddress            d_ipAddress;
    ServerList                       d_serverList;
    bsl::size_t                      d_serverIndex;
    const ntca::GetDomainNameOptions d_options;
    ntci::GetDomainNameCallback      d_callback;
    bsl::shared_ptr<ntci::Timer>     d_timer_sp;
    bsl::shared_ptr<ntcdns::Cache>   d_cache_sp;
    bsls::AtomicBool                 d_pending;
    bslma::Allocator*                d_allocator_p;

  private:
    ClientGetDomainNameOperation(ClientGetDomainNameOperation&)
        BSLS_KEYWORD_DELETED;
    ClientGetDomainNameOperation& operator=(
        const ClientGetDomainNameOperation&) BSLS_KEYWORD_DELETED;

  public:
    /// Defines a type alias for a vector of endpoints.
    typedef bsl::vector<ntsa::Endpoint> EndpointList;

    /// Create a new get domain name operation to get the domain name to
    /// which the specified 'ipAddress' is assigned according to the
    /// specified 'options' and invoke the specified 'callback' when the
    /// operation completes or fails. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    ClientGetDomainNameOperation(
        const bsl::shared_ptr<ntci::Resolver>& resolver,
        const ntsa::IpAddress&                 ipAddress,
        const ServerList&                      serverList,
        const ntca::GetDomainNameOptions&      options,
        const ntci::GetDomainNameCallback&     callback,
        const bsl::shared_ptr<ntcdns::Cache>&  cache,
        bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~ClientGetDomainNameOperation() BSLS_KEYWORD_OVERRIDE;

    /// Send a request to perform this operation through the specified
    /// 'datagramSocket' to the name server at the specified 'endpoint'.
    /// Identify the request using the specified 'transactionId'. Return the
    /// error.
    ntsa::Error sendRequest(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntsa::Endpoint&                        endpoint,
        bsl::uint16_t transactionId) BSLS_KEYWORD_OVERRIDE;

    /// Send a request to perform this operation through the specified
    /// 'streamSocket' to the name server at the specified 'endpoint'.
    /// Identify the request using the specified 'transactionId'. Return the
    /// error.
    ntsa::Error sendRequest(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntsa::Endpoint&                      endpoint,
        bsl::uint16_t transactionId) BSLS_KEYWORD_OVERRIDE;

    /// Invoke the response callback with the contents of the specified
    /// 'response' received from the specified 'endpoint' at the specified
    /// 'serverIndex'.
    void processResponse(const ntcdns::Message&    response,
                         const ntsa::Endpoint&     endpoint,
                         bsl::size_t               serverIndex,
                         const bsls::TimeInterval& now) BSLS_KEYWORD_OVERRIDE;

    /// Invoke the response callback with the specified 'error'.
    void processError(const ntsa::Error& error) BSLS_KEYWORD_OVERRIDE;

    /// Prepare the operation to target the next name server and return that
    /// name server, or null if all name servers have been tried.
    bsl::shared_ptr<ntcdns::ClientNameServer> tryNextServer()
        BSLS_KEYWORD_OVERRIDE;

    /// Prepare the operation to target the next name in the search list.
    /// Return true if such a name exists, and false otherwise.
    bool tryNextSearch() BSLS_KEYWORD_OVERRIDE;

    /// Return the IP address to resolve.
    const ntsa::IpAddress& ipAddress() const;

    /// The options that control the behavior of the operation.
    const ntca::GetDomainNameOptions& options() const;

    /// Return the index of the current name server being tried.
    bsl::size_t serverIndex() const;

    /// Return the index of the current search domain being tried.
    bsl::size_t searchIndex() const;
};

/// @internal @brief
/// Provide a name server to which to a client sends requests.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class ClientNameServer : public ntci::DatagramSocketSession,
                         public ntci::StreamSocketSession,
                         public ntccfg::Shared<ClientNameServer>
{
    /// This typedef defines a vector of operations.
    typedef bsl::vector<bsl::shared_ptr<ntcdns::ClientOperation> >
        OperationVector;

    /// This typedef defines a queue of operations.
    typedef ntcdns::Queue<bsl::shared_ptr<ntcdns::ClientOperation> >
        OperationQueue;

    /// This typedef defines a map of transaction IDs to operations.
    typedef ntcdns::Map<bsl::uint16_t,
                        bsl::shared_ptr<ntcdns::ClientOperation> >
        OperationMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    enum State {
        // This enumeration enumerates the states of operation.

        e_STATE_STARTED,
        e_STATE_STOPPING,
        e_STATE_STOPPED
    };

    ntccfg::Object                               d_object;
    OperationQueue                               d_operationQueue;
    OperationMap                                 d_operationMap;
    Mutex                                        d_datagramSocketMutex;
    bsl::shared_ptr<ntci::DatagramSocket>        d_datagramSocket_sp;
    bsl::shared_ptr<ntci::DatagramSocketFactory> d_datagramSocketFactory_sp;
    Mutex                                        d_streamSocketMutex;
    bsl::shared_ptr<ntci::StreamSocket>          d_streamSocket_sp;
    bsl::shared_ptr<ntci::StreamSocketFactory>   d_streamSocketFactory_sp;
    ntccfg::ConditionMutex                       d_stateMutex;
    ntccfg::Condition                            d_stateCondition;
    State                                        d_state;
    const ntsa::Endpoint                         d_endpoint;
    const bsl::size_t                            d_index;
    const ntcdns::ClientConfig                   d_config;
    bslma::Allocator*                            d_allocator_p;

  private:
    ClientNameServer(ClientNameServer&) BSLS_KEYWORD_DELETED;
    ClientNameServer& operator=(const ClientNameServer&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than the read queue high watermark.
    void processReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue has been
    /// drained down to less than or equal to the write queue low watermark.
    /// This condition will not occur until the write queue high watermark
    /// condition occurs. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is greater
    /// than the write queue high watermark. This condition will occur the
    /// first time the write queue high watermark has been reached but
    /// then will not subsequently ooccur until the write queue low
    /// watermark. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than or equal to the read queue low watermark.
    void processReadQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the read queue is greater
    /// than the read queue high watermark.
    void processReadQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ReadQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue has been
    /// drained down to less than or equal to the write queue low watermark.
    /// This condition will not occur until the write queue high watermark
    /// condition occurs. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueLowWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the condition that the size of the write queue is greater
    /// than the write queue high watermark. This condition will occur the
    /// first time the write queue high watermark has been reached but
    /// then will not subsequently ooccur until the write queue low
    /// watermark. The write queue low watermark conditions and the
    /// high watermark conditions are guaranteed to occur serially.
    void processWriteQueueHighWatermark(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::WriteQueueEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the initiation of the shutdown sequence from the specified
    /// 'origin'.
    void processShutdownInitiated(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for reading.
    void processShutdownReceive(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the socket being shut down for writing.
    void processShutdownSend(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process the completion of the shutdown sequence.
    void processShutdownComplete(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const ntca::ShutdownEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Process an error.
    void processError(const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
                      const ntca::ErrorEvent& event) BSLS_KEYWORD_OVERRIDE;

    /// Create the internal datagram socket.
    ntsa::Error createDatagramSocket();

    /// Create the internal stream socket.
    ntsa::Error createStreamSocket();

    /// Process the connection of the specified 'datagramSocket' according
    /// to the specified 'event'.
    void processDatagramSocketConnected(
        const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
        const bsl::shared_ptr<ntci::Connector>&      connector,
        const ntca::ConnectEvent&                    event);

    /// Process the connection of the specified 'streamSocket' according to
    /// the specified 'event'.
    void processStreamSocketConnected(
        const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
        const bsl::shared_ptr<ntci::Connector>&    connector,
        const ntca::ConnectEvent&                  event);

    /// Flush queued operations.
    void flush();

  public:
    /// Create a new client name server for a client having the specified
    /// 'configuration' representing a name server at the specified 'index'
    /// in that configuration that sends requests to the specified
    /// 'endpoint' using sockets created by the specified
    /// 'datagramSocketFactory' and 'streamSocketFactory'.
    explicit ClientNameServer(
        const bsl::shared_ptr<ntci::DatagramSocketFactory>&
            datagramSocketFactory,
        const bsl::shared_ptr<ntci::StreamSocketFactory>& streamSocketFactory,
        const ntsa::Endpoint&                             endpoint,
        bsl::size_t                                       index,
        const ntcdns::ClientConfig&                       configuration,
        bslma::Allocator*                                 basicAllocator = 0);

    /// Destroy this object.
    ~ClientNameServer() BSLS_KEYWORD_OVERRIDE;

    /// Start the name server.
    ntsa::Error start();

    /// Initiate the specified 'operation'. Return the error.
    ntsa::Error initiate(
        const bsl::shared_ptr<ntcdns::ClientOperation>& operation);

    /// Cancel the specified 'operation' and invoke it's callback notifying
    /// the initiatior that the operation has been cancelled.
    void cancel(const bsl::shared_ptr<ntcdns::ClientOperation>& operation);

    /// Cancel all operations: invoke each operation's callback notifying
    /// the initiator that the operation has been cancelled.
    void cancelAll();

    /// Abandon the specified 'operation': do not invoke the operation's
    /// callback.
    void abandon(const bsl::shared_ptr<ntcdns::ClientOperation>& operation);

    /// Abandon all operations: do not invoke any operation's callback.
    void abandonAll();

    /// Begin stopping the name server.
    void shutdown();

    /// Wait until the name server is stopped.
    void linger();

    /// The endpoint of the name server.
    const ntsa::Endpoint& endpoint() const;
};

/// @internal @brief
/// Provide a DNS client.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class Client : public ntccfg::Shared<Client>
{
    /// This type defines a list of question names to try when performing
    /// the operation.
    typedef bsl::vector<bsl::string> SearchList;

    /// Define a type alias for a list of name servers to
    /// try when performing the operation.
    typedef bsl::vector<bsl::shared_ptr<ntcdns::ClientNameServer> > ServerList;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    enum State {
        // This enumeraiton enumerates the states of operation.

        e_STATE_STARTED,
        e_STATE_STOPPING,
        e_STATE_STOPPED
    };

    ntccfg::Object                               d_object;
    Mutex                                        d_mutex;
    bsl::shared_ptr<ntci::DatagramSocketFactory> d_datagramSocketFactory_sp;
    bsl::shared_ptr<ntci::StreamSocketFactory>   d_streamSocketFactory_sp;
    bsl::shared_ptr<ntcdns::Cache>               d_cache_sp;
    ServerList                                   d_serverList;
    State                                        d_state;
    bool                                         d_initialized;
    ntcdns::ClientConfig                         d_config;
    bslma::Allocator*                            d_allocator_p;

  private:
    Client(const Client&) BSLS_KEYWORD_DELETED;
    Client& operator=(const Client&) BSLS_KEYWORD_DELETED;

  private:
    /// Initialize the mechanisms used by this object, if necessary.
    ntsa::Error initialize();

  public:
    /// Create a new client having the specified 'configuration'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Client(
        const ntcdns::ClientConfig&           configuration,
        const bsl::shared_ptr<ntcdns::Cache>& cache,
        const bsl::shared_ptr<ntci::DatagramSocketFactory>&
            datagramSocketFactory,
        const bsl::shared_ptr<ntci::StreamSocketFactory>& streamSocketFactory,
        bslma::Allocator*                                 basicAllocator = 0);

    /// Destroy this object.
    ~Client();

    /// Start the client. Return the error.
    ntsa::Error start();

    /// Begin stopping the client.
    void shutdown();

    /// Wait until the client has stopped.
    void linger();

    /// Get the IP addresses assigned to the specified 'name' and invoke
    /// the specfied 'callback' when resolution completes or an error
    /// occurs. Return the error.
    ntsa::Error getIpAddress(const bsl::shared_ptr<ntci::Resolver>& resolver,
                             const bsl::string&                     domainName,
                             const ntca::GetIpAddressOptions&       options,
                             const ntci::GetIpAddressCallback&      callback);

    /// Get the domain name to which the specified 'ipAddress' has been
    /// assigned and invoke the specfied 'callback' when resolution
    /// completes or an error occurs. Return the error.
    ntsa::Error getDomainName(const bsl::shared_ptr<ntci::Resolver>& resolver,
                              const ntsa::IpAddress&                 ipAddress,
                              const ntca::GetDomainNameOptions&      options,
                              const ntci::GetDomainNameCallback&     callback);
};

}  // close package namespace
}  // close enterprise namespace
#endif
