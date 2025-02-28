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

#ifndef INCLUDED_NTCF_TESTFIXTURE
#define INCLUDED_NTCF_TESTFIXTURE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcf_testvocabulary.h>
#include <ntcf_testmessage.h>
#include <ntcf_testclient.h>
#include <ntcf_testserver.h>

namespace BloombergLP {
namespace ntcf {

/// Provide a test fixture.
///
/// @par Thread Safety
/// This class is thread safe.
class TestFixture
{
    /// This type defines a type alias for this type.
    typedef TestFixture Self;

    /// Defines a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    /// Defines a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    /// Defines a type alias for a vector of clients.
    typedef bsl::vector< bsl::shared_ptr<ntcf::TestClient> > ClientVector;

    mutable Mutex                                d_mutex;
    bsl::shared_ptr<ntcf::TestMessageEncryption> d_encryption_sp;
    bsl::shared_ptr<ntci::DataPool>              d_serverDataPool_sp;
    bsl::shared_ptr<ntci::Scheduler>             d_serverScheduler_sp;
    bsl::shared_ptr<ntcf::TestServer>            d_server_sp;
    bsl::shared_ptr<ntci::DataPool>              d_clientDataPool_sp;
    bsl::shared_ptr<ntci::Scheduler>             d_clientScheduler_sp;
    ClientVector                                 d_clientVector;
    ntcf::TestFixtureConfig                      d_config;
    bslma::Allocator*                            d_allocator_p;

    BALL_LOG_SET_CLASS_CATEGORY("NTCF.TEST.FIXTURE");

private:
    TestFixture(const TestFixture&) BSLS_KEYWORD_DELETED;
    TestFixture& operator=(const TestFixture&) BSLS_KEYWORD_DELETED;

private:
    /// Create and return a new client.
    bsl::shared_ptr<ntcf::TestClient> createClient();

    /// Invoke the specified 'callback' with the specified 'connector' and
    /// 'event'.
    void dispatchConnect(
        const bsl::shared_ptr<ntcf::TestClient>& client,
        const bsl::shared_ptr<ntci::Connector>&  connector,
        const ntca::ConnectEvent&                event,
        const ntci::ConnectCallback&             callback);

    /// Load the appropriate scheduler configuration into the specified
    /// 'result' for the specified client 'configuration'.
    static void configure(ntca::SchedulerConfig*        result, 
                          const ntcf::TestClientConfig& configuration);

    /// Load the appropriate scheduler configuration into the specified
    /// 'result' for the specified server 'configuration'.
    static void configure(ntca::SchedulerConfig*        result, 
                          const ntcf::TestServerConfig& configuration);

public:
    /// Create a new test fixture having the specified 'configuration'. 
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is 
    /// used.
    explicit TestFixture(const ntcf::TestFixtureConfig& configuration,
                         bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~TestFixture();

    /// Set the server accept queue low watermark to the specified 'value'.
    void serverSetAcceptQueueLowWatermark(bsl::size_t value);

    /// Set the server accept queue high watermark to the specified 'value'.
    void serverSetAcceptQueueHighWatermark(bsl::size_t value);

    /// Start monitoring the listener socket backlog.
    void serverRelaxFlowControl();

    /// Stop monitoring the listener socket backlog.
    void serverApplyFlowControl();

    /// Connect a new client to the server.
    ntsa::Error clientConnect();

    /// Connect a new client to the server, block until the connection is
    /// established, and load the new client into the specified 'result'.
    /// Return the error.
    ntsa::Error clientConnect(bsl::shared_ptr<ntcf::TestClient>* result);

    /// Connect a new client to the server and invoke the specified 'callback'
    /// when the connection is established or an error occurs. Return the
    /// error.
    ntsa::Error clientConnect(const ntci::ConnectCallback& callback);

    /// Connect a new client to the server and invoke the specified 'callback'
    /// when the connection is established or an error occurs. Return the
    /// error.
    ntsa::Error clientConnect(bsl::shared_ptr<ntcf::TestClient>* result,
                              const ntci::ConnectCallback&       callback);

    /// Connect a another client to the server, block until the connection is
    /// established, and append the new client to the specified 'result'.
    /// Return the error.
    ntsa::Error clientConnect(ntcf::TestClientVector* result);

    /// Connect another 'count' specified number of clients to the server,
    /// block until the connection for each is established, then append each
    /// client to the specified 'result'. Return the error.
    ntsa::Error clientConnect(ntcf::TestClientVector* result, 
                              bsl::size_t             count);

    /// Return the client at the specified 'index'.
    const bsl::shared_ptr<ntcf::TestClient>& client(bsl::size_t index);

    /// Return the number of clients.
    bsl::size_t clientCount() const;
};

/// Provide test fixture utilities.
///
/// @par Thread Safety
/// This class is thread safe.
class TestFixtureUtil
{
    static bsls::AtomicUint64 s_pingId;
    
public:
    /// Signal the peer of the specified 'client' and block until a response is
    /// received.
    static void ping(const ntcf::TestClientPtr& client);

    /// Instruct the server to enable compression when communicating with the
    /// specified 'client'.
    static void enableRemoteCompression(const ntcf::TestClientPtr& client);

    /// Enable compression at the specified 'client'.
    static void enableSourceCompression(const ntcf::TestClientPtr& client);

    /// Instruct the server to disable compression when communicating with the
    /// specified 'client'.
    static void disableRemoteCompression(const ntcf::TestClientPtr& client);

    /// Disable compression at the specified 'client'.
    static void disableSourceCompression(const ntcf::TestClientPtr& client);

    /// Instruct the server to enable encryption when communicating with the
    /// specified 'client'.
    static void enableRemoteEncryption(const ntcf::TestClientPtr& client);

    /// Enable encryption at the specified 'client'.
    static void enableSourceEncryption(const ntcf::TestClientPtr& client);

    /// Instruct the server to disable encryption when communicating with the
    /// specified 'client'.
    static void disableRemoteEncryption(const ntcf::TestClientPtr& client);

    /// Disable encryption at the specified 'client'.
    static void disableSourceEncryption(const ntcf::TestClientPtr& client);
};

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
