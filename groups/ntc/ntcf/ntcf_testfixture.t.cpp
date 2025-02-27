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

    // Create a server.

    ntcf::TestFixtureConfig fixtureConfig;
    fixtureConfig.server.backlog = 1;

    ntcf::TestFixture fixture(fixtureConfig, NTSCFG_TEST_ALLOCATOR);

    // Create a client.

    bsl::shared_ptr<ntcf::TestClient> client;
    error = fixture.clientConnect(&client);
    NTSCFG_TEST_OK(error);

    // MRM
#if 1

    // Ping the server with neither compression nor encryption.

    {
        ntcf::TestEchoResult result;
        ntcf::TestSignal     signal;
        ntcf::TestOptions    options;

        signal.id = 1;
        signal.reflect = 64;
        ntscfg::TestDataUtil::generateData(
            &signal.value, 
            32, 
            0, 
            ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE);

        error = client->signal(&result, signal, options);
        NTSCFG_TEST_OK(error);
    }

    // Instruct the server to enable compression when communicating with this
    // client.

    {
        ntcf::TestAcknowledgmentResult result;
        ntcf::TestControlCompression   compression;
        ntcf::TestOptions              options;

        compression.enabled = true;
        compression.acknowledge = true;
        compression.transition = 
            ntcf::TestControlTransition::e_ACKNOWLEDGE_BEFORE;

        error = client->compress(&result, compression, options);
        NTSCFG_TEST_OK(error);
    }

    // Enable compression at the client.

    client->enableCompression();

    // Ping the server with compression.

    {
        ntcf::TestEchoResult result;
        ntcf::TestSignal     signal;
        ntcf::TestOptions    options;

        signal.id = 2;
        signal.reflect = 64;
        ntscfg::TestDataUtil::generateData(
            &signal.value, 
            32, 
            0, 
            ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE);

        error = client->signal(&result, signal, options);
        NTSCFG_TEST_OK(error);
    }

    // Instruct the server to disable compression when communicating with this
    // client.

    {
        ntcf::TestAcknowledgmentResult result;
        ntcf::TestControlCompression   compression;
        ntcf::TestOptions              options;

        compression.enabled = false;
        compression.acknowledge = true;
        compression.transition = 
            ntcf::TestControlTransition::e_ACKNOWLEDGE_BEFORE;

        error = client->compress(&result, compression, options);
        NTSCFG_TEST_OK(error);
    }

    // Disable compression at the client.

    client->disableCompression();

    // Ping the server with neither compression nor encryption.

    {
        ntcf::TestEchoResult result;
        ntcf::TestSignal     signal;
        ntcf::TestOptions    options;

        signal.id = 2;
        signal.reflect = 64;
        ntscfg::TestDataUtil::generateData(
            &signal.value, 
            32, 
            0, 
            ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE);

        error = client->signal(&result, signal, options);
        NTSCFG_TEST_OK(error);
    }

#endif

    // Instruct the server to enable encryption when communicating with this
    // client.

    {
        ntcf::TestAcknowledgmentResult result;
        ntcf::TestControlEncryption    encryption;
        ntcf::TestOptions              options;

        encryption.enabled = true;
        encryption.acknowledge = true;
        encryption.transition = 
            ntcf::TestControlTransition::e_ACKNOWLEDGE_BEFORE;

        error = client->encrypt(&result, encryption, options);
        NTSCFG_TEST_OK(error);
    }

    // Enable encryption at the client.

    client->enableEncryption();

    // Ping the server with encryption.

    {
        ntcf::TestEchoResult result;
        ntcf::TestSignal     signal;
        ntcf::TestOptions    options;

        signal.id = 2;
        signal.reflect = 64;
        ntscfg::TestDataUtil::generateData(
            &signal.value, 
            32, 
            0, 
            ntscfg::TestDataUtil::k_DATASET_CLIENT_COMPRESSABLE);

        error = client->signal(&result, signal, options);
        NTSCFG_TEST_OK(error);
    }

    // Instruct the server to disable encryption when communicating with this
    // client.

    {
        ntcf::TestAcknowledgmentResult result;
        ntcf::TestControlEncryption    encryption;
        ntcf::TestOptions              options;

        encryption.enabled = false;
        encryption.acknowledge = true;
        encryption.transition = 
            ntcf::TestControlTransition::e_ACKNOWLEDGE_BEFORE;

        error = client->encrypt(&result, encryption, options);
        NTSCFG_TEST_OK(error);
    }

    // Disable encryption at the client.

    client->disableEncryption();

    // Ping the server neither compression nor encryption.

    {
        ntcf::TestEchoResult result;
        ntcf::TestSignal     signal;
        ntcf::TestOptions    options;

        signal.id = 2;
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
