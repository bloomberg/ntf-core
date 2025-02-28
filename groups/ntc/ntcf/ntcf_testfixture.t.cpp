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

#ifndef INCLUDED_NTCF_TESTFIXTURE_T_CPP
#define INCLUDED_NTCF_TESTFIXTURE_T_CPP

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_testfixture_t_cpp, "$Id$ $CSID$")

#include <ntcf_testfixture.h>

namespace BloombergLP {
namespace ntcf {

// Provide tests for 'ntcf::TestFixture'.
class TestFixtureTest
{
    // Provide state used to manage a test.
    class State;

    BALL_LOG_SET_CLASS_CATEGORY("NTCF.TEST.FIXTURE.TEST");

  public:
    // TODO
    static void verify();

    // TODO
    static void verifyBacklog();
};

// Provide state used to manage a test.
class TestFixtureTest::State
{

public:
    // Create new test state. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed default
    // allocator is used.
    explicit State(bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~State();
};

TestFixtureTest::State::State(bslma::Allocator* basicAllocator)
{

}

TestFixtureTest::State::~State()
{

}

NTSCFG_TEST_FUNCTION(ntcf::TestFixtureTest::verify)
{
    ntsa::Error error;

    // Configure the number of iterations.

    const bsl::size_t k_NUM_ITERATIONS = 10;
    const bsl::size_t k_NUM_STATE_TRANSITIONS = 10;
    const bsl::size_t k_NUM_PINGS = 10;

    // Create a server.

    ntcf::TestFixtureConfig fixtureConfig;
    fixtureConfig.server.backlog = 1;

    ntcf::TestFixture fixture(fixtureConfig, NTSCFG_TEST_ALLOCATOR);

    // Create a client.

    bsl::shared_ptr<ntcf::TestClient> client;
    error = fixture.clientConnect(&client);
    NTSCFG_TEST_OK(error);

    for (bsl::size_t i = 0; i < k_NUM_ITERATIONS; ++i) {
        // Ping the server with neither compression nor encryption.

        for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
            ntcf::TestFixtureUtil::ping(client);
        }

        // Test compression.

        for (bsl::size_t j = 0; j < k_NUM_STATE_TRANSITIONS; ++j) {
            // Instruct the server to enable compression when communicating
            // with this client.

            ntcf::TestFixtureUtil::enableRemoteCompression(client);

            // Enable compression at the client.

            ntcf::TestFixtureUtil::enableSourceCompression(client);

            // Ping the server with compression.

            for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
                ntcf::TestFixtureUtil::ping(client);
            }

            // Instruct the server to disable compression when communicating
            // with this client.

            ntcf::TestFixtureUtil::disableRemoteCompression(client);

            // Disable compression at the client.

            ntcf::TestFixtureUtil::disableSourceCompression(client);

            // Ping the server with neither compression nor encryption.

            for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
                ntcf::TestFixtureUtil::ping(client);
            }
        }

        // Test encryption.

        for (bsl::size_t j = 0; j < k_NUM_STATE_TRANSITIONS; ++j) {
            // Instruct the server to enable encryption when communicating with
            // this client.

            ntcf::TestFixtureUtil::enableRemoteEncryption(client);

            // Enable encryption at the client.

            ntcf::TestFixtureUtil::enableSourceEncryption(client);

            // Ping the server with encryption.

            for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
                ntcf::TestFixtureUtil::ping(client);
            }

            // Instruct the server to disable encryption when communicating
            // with this client.

            ntcf::TestFixtureUtil::disableRemoteEncryption(client);

            // Disable encryption at the client.

            ntcf::TestFixtureUtil::disableSourceEncryption(client);

            // Ping the server with neither compression nor encryption.

            for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
                ntcf::TestFixtureUtil::ping(client);
            }
        }

        // Test compression and encryption.

        for (bsl::size_t j = 0; j < k_NUM_STATE_TRANSITIONS; ++j) {
            // Instruct the server to enable compression when communicating
            // with this client.

            ntcf::TestFixtureUtil::enableRemoteCompression(client);

            // Enable compression at the client.

            ntcf::TestFixtureUtil::enableSourceCompression(client);

            // Ping the server with compression.

            for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
                ntcf::TestFixtureUtil::ping(client);
            }

            // Instruct the server to enable encryption when communicating with
            // this client.

            ntcf::TestFixtureUtil::enableRemoteEncryption(client);

            // Enable encryption at the client.

            ntcf::TestFixtureUtil::enableSourceEncryption(client);

            // Ping the server with compression and encryption.

            for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
                ntcf::TestFixtureUtil::ping(client);
            }

            // Instruct the server to disable encryption when communicating
            // with this client.

            ntcf::TestFixtureUtil::disableRemoteEncryption(client);

            // Disable encryption at the client.

            ntcf::TestFixtureUtil::disableSourceEncryption(client);

            // Ping the server with compression.

            for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
                ntcf::TestFixtureUtil::ping(client);
            }

            // Instruct the server to disable compression when communicating
            // with this client.

            ntcf::TestFixtureUtil::disableRemoteCompression(client);

            // Disable compression at the client.

            ntcf::TestFixtureUtil::disableSourceCompression(client);

            // Ping the server with neither compression nor encryption.

            for (bsl::size_t k = 0; k < k_NUM_PINGS; ++k) {
                ntcf::TestFixtureUtil::ping(client);
            }
        }
    }
}

NTSCFG_TEST_FUNCTION(ntcf::TestFixtureTest::verifyBacklog)
{
    ntsa::Error error;

    const bsl::size_t k_NUM_CONNECTIONS = 10;

    // Create a server.

    ntcf::TestFixtureConfig fixtureConfig;
    fixtureConfig.server.backlog = k_NUM_CONNECTIONS;

    ntcf::TestFixture fixture(fixtureConfig, NTSCFG_TEST_ALLOCATOR);

    fixture.serverApplyFlowControl();

    // Create clients up to the accept backlog limit.

    for (bsl::size_t i = 0; i < k_NUM_CONNECTIONS; ++i) {
        error = fixture.clientConnect();
        NTSCFG_TEST_OK(error);
    }

    fixture.serverRelaxFlowControl();

    // Ping the server.

    for (bsl::size_t i = 0; i < fixture.clientCount(); ++i)
    {
        const ntcf::TestClientPtr& client = fixture.client(i);

        ntcf::TestEchoResult result;
        ntcf::TestSignal     signal;
        ntcf::TestOptions    options;

        signal.id = i + 1;
        signal.reflect = 64;
        ntscfg::TestDataUtil::generateData(
            &signal.value, 
            32, 
            0, 
            ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE);

        error = client->signal(&result, signal, options);
        NTSCFG_TEST_OK(error);
    }
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
