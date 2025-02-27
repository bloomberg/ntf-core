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

#ifndef INCLUDED_NTCF_TESTFIXTURE_CPP
#define INCLUDED_NTCF_TESTFIXTURE_CPP

#include <ntcf_testfixture.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_testfixture_cpp, "$Id$ $CSID$")

#include <bslmt_threadutil.h>

namespace BloombergLP {
namespace ntcf {

bsl::shared_ptr<ntcf::TestClient> TestFixture::createClient()
{
    bsl::shared_ptr<ntcf::TestClient> client;
    client.createInplace(d_allocator_p, 
                         d_config.client, 
                         d_clientScheduler_sp,
                         d_clientDataPool_sp,
                         d_encryption_sp,
                         d_server_sp->tcpEndpoint(), 
                         d_server_sp->udpEndpoint(), 
                         d_allocator_p);

    return client;
}

void TestFixture::dispatchConnect(
        const bsl::shared_ptr<ntcf::TestClient>& client,
        const bsl::shared_ptr<ntci::Connector>&  connector,
        const ntca::ConnectEvent&                event,
        const ntci::ConnectCallback&             callback)
{
    if (event.type() == ntca::ConnectEventType::e_COMPLETE) {
        LockGuard lock(&d_mutex);
        d_clientVector.push_back(client);
    }

    if (callback) {
        callback.execute(connector, event, ntci::Strand::unknown());
    }
}

void TestFixture::configure(ntca::SchedulerConfig*        result, 
                            const ntcf::TestClientConfig& configuration)
{
    result->setThreadName(configuration.name.value());

    if (configuration.driver.has_value()) {
        result->setDriverName(configuration.driver.value());
    }

    if (configuration.minThreads.has_value()) {
        result->setMinThreads(configuration.minThreads.value());
    }

    if (configuration.maxThreads.has_value()) {
        result->setMaxThreads(configuration.maxThreads.value());
    }

    if (configuration.dynamicLoadBalancing.has_value()) {
        result->setDynamicLoadBalancing(
            configuration.dynamicLoadBalancing.value());
    }

    if (configuration.keepAlive.has_value()) {
        result->setKeepAlive(configuration.keepAlive.value());
    }

    if (configuration.keepHalfOpen.has_value()) {
        result->setKeepHalfOpen(configuration.keepHalfOpen.value());
    }

    if (configuration.backlog.has_value()) {
        result->setBacklog(configuration.backlog.value());
    }

    if (configuration.sendBufferSize.has_value()) {
        result->setSendBufferSize(configuration.sendBufferSize.value());
    }

    if (configuration.receiveBufferSize.has_value()) {
        result->setBacklog(configuration.receiveBufferSize.value());
    }

    if (configuration.acceptGreedily.has_value()) {
        result->setAcceptGreedily(configuration.acceptGreedily.value());
    }

    if (configuration.acceptQueueLowWatermark.has_value()) {
        result->setAcceptQueueLowWatermark(
            configuration.acceptQueueLowWatermark.value());
    }

    if (configuration.acceptQueueHighWatermark.has_value()) {
        result->setAcceptQueueHighWatermark(
            configuration.acceptQueueHighWatermark.value());
    }

    if (configuration.readQueueLowWatermark.has_value()) {
        result->setReadQueueLowWatermark(
            configuration.readQueueLowWatermark.value());
    }

    if (configuration.readQueueHighWatermark.has_value()) {
        result->setReadQueueHighWatermark(
            configuration.readQueueHighWatermark.value());
    }

    if (configuration.writeQueueLowWatermark.has_value()) {
        result->setWriteQueueLowWatermark(
            configuration.writeQueueLowWatermark.value());
    }

    if (configuration.writeQueueHighWatermark.has_value()) {
        result->setWriteQueueHighWatermark(
            configuration.writeQueueHighWatermark.value());
    }
}

void TestFixture::configure(ntca::SchedulerConfig*        result, 
                            const ntcf::TestServerConfig& configuration)
{    
    result->setThreadName(configuration.name.value());

    if (configuration.driver.has_value()) {
        result->setDriverName(configuration.driver.value());
    }

    if (configuration.minThreads.has_value()) {
        result->setMinThreads(configuration.minThreads.value());
    }

    if (configuration.maxThreads.has_value()) {
        result->setMaxThreads(configuration.maxThreads.value());
    }

    if (configuration.dynamicLoadBalancing.has_value()) {
        result->setDynamicLoadBalancing(
            configuration.dynamicLoadBalancing.value());
    }

    if (configuration.keepAlive.has_value()) {
        result->setKeepAlive(configuration.keepAlive.value());
    }

    if (configuration.keepHalfOpen.has_value()) {
        result->setKeepHalfOpen(configuration.keepHalfOpen.value());
    }

    if (configuration.backlog.has_value()) {
        result->setBacklog(configuration.backlog.value());
    }

    if (configuration.sendBufferSize.has_value()) {
        result->setSendBufferSize(configuration.sendBufferSize.value());
    }

    if (configuration.receiveBufferSize.has_value()) {
        result->setBacklog(configuration.receiveBufferSize.value());
    }

    if (configuration.acceptGreedily.has_value()) {
        result->setAcceptGreedily(configuration.acceptGreedily.value());
    }

    if (configuration.acceptQueueLowWatermark.has_value()) {
        result->setAcceptQueueLowWatermark(
            configuration.acceptQueueLowWatermark.value());
    }

    if (configuration.acceptQueueHighWatermark.has_value()) {
        result->setAcceptQueueHighWatermark(
            configuration.acceptQueueHighWatermark.value());
    }

    if (configuration.readQueueLowWatermark.has_value()) {
        result->setReadQueueLowWatermark(
            configuration.readQueueLowWatermark.value());
    }

    if (configuration.readQueueHighWatermark.has_value()) {
        result->setReadQueueHighWatermark(
            configuration.readQueueHighWatermark.value());
    }

    if (configuration.writeQueueLowWatermark.has_value()) {
        result->setWriteQueueLowWatermark(
            configuration.writeQueueLowWatermark.value());
    }

    if (configuration.writeQueueHighWatermark.has_value()) {
        result->setWriteQueueHighWatermark(
            configuration.writeQueueHighWatermark.value());
    }
}

TestFixture::TestFixture(const ntcf::TestFixtureConfig& configuration,
                         bslma::Allocator*              basicAllocator)
: d_mutex()
, d_encryption_sp()
, d_serverDataPool_sp()
, d_serverScheduler_sp()
, d_server_sp()
, d_clientDataPool_sp()
, d_clientScheduler_sp()
, d_clientVector(basicAllocator)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error;

    d_encryption_sp.createInplace(d_allocator_p, d_allocator_p);

    // MRM: BALL_LOG_INFO << "Server scheduler construction starting" << BALL_LOG_END;

    if (d_config.server.name.isNull()) {
        d_config.server.name = "server";
    }

    if (d_config.server.minThreads.isNull() && 
        d_config.server.maxThreads.isNull()) 
    {
        d_config.server.minThreads = 1;
        d_config.server.maxThreads = 1;
    }

    if (d_config.server.keepHalfOpen.isNull()) {
        d_config.server.keepHalfOpen = false;
    }

    if (d_config.server.blobBufferSize.isNull()) {
        d_config.server.blobBufferSize.makeValue(4096);
    }

    d_serverDataPool_sp = ntcf::System::createDataPool(
        d_config.server.blobBufferSize.value(), 
        d_config.server.blobBufferSize.value(), 
        d_allocator_p);

    ntca::SchedulerConfig serverSchedulerConfig;
    ntcf::TestFixture::configure(&serverSchedulerConfig, d_config.server);

    d_serverScheduler_sp = ntcf::System::createScheduler(serverSchedulerConfig,
                                                         d_serverDataPool_sp,
                                                         d_allocator_p);

    error = d_serverScheduler_sp->start();
    BSLS_ASSERT_OPT(!error);

    // MRM: BALL_LOG_INFO << "Server scheduler construction complete" << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Client scheduler construction starting" << BALL_LOG_END;

    if (d_config.client.name.isNull()) {
        d_config.client.name = "client";
    }

    if (d_config.client.blobBufferSize.isNull()) {
        d_config.client.blobBufferSize.makeValue(4096);
    }

    if (d_config.server.minThreads.isNull() && 
        d_config.server.maxThreads.isNull()) 
    {
        d_config.server.minThreads = 1;
        d_config.server.maxThreads = 1;
    }

    if (d_config.server.keepHalfOpen.isNull()) {
        d_config.server.keepHalfOpen = false;
    }

    d_clientDataPool_sp = ntcf::System::createDataPool(
        d_config.client.blobBufferSize.value(), 
        d_config.client.blobBufferSize.value(), 
        d_allocator_p);

    ntca::SchedulerConfig clientSchedulerConfig;
    ntcf::TestFixture::configure(&clientSchedulerConfig, d_config.client);

    d_clientScheduler_sp = ntcf::System::createScheduler(clientSchedulerConfig,
                                                         d_clientDataPool_sp,
                                                         d_allocator_p);

    error = d_clientScheduler_sp->start();
    BSLS_ASSERT_OPT(!error);

    // MRM: BALL_LOG_INFO << "Client scheduler construction complete" << BALL_LOG_END;

    d_server_sp.createInplace(d_allocator_p, 
                              d_config.server, 
                              d_serverScheduler_sp, 
                              d_serverDataPool_sp, 
                              d_encryption_sp,
                              d_allocator_p);
}

TestFixture::~TestFixture()
{
    // MRM: BALL_LOG_INFO << "Test fixture client closure starting" << BALL_LOG_END;

    for (bsl::size_t i = 0; i < d_clientVector.size(); ++i) {
        const ntcf::TestClientPtr& client = d_clientVector[i];
        client->close();    
    }

    d_clientVector.clear();

    // MRM: BALL_LOG_INFO << "Test fixture client closure complete" << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Client scheduler destruction starting" << BALL_LOG_END;

    if (d_clientScheduler_sp) {
        d_clientScheduler_sp->shutdown();
        d_clientScheduler_sp->linger();
        d_clientScheduler_sp.reset();
    }

    // MRM: BALL_LOG_INFO << "Client scheduler destruction complete" << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Test fixture server closure starting" << BALL_LOG_END;

    d_server_sp->close();
    d_server_sp.reset();

    // MRM: BALL_LOG_INFO << "Test fixture server closure complete" << BALL_LOG_END;

    // MRM: BALL_LOG_INFO << "Server scheduler destruction starting" << BALL_LOG_END;

    if (d_serverScheduler_sp) {
        d_serverScheduler_sp->shutdown();
        d_serverScheduler_sp->linger();
        d_serverScheduler_sp.reset();
    }

    // MRM: BALL_LOG_INFO << "Server scheduler destruction complete" << BALL_LOG_END;
}

void TestFixture::serverSetAcceptQueueLowWatermark(bsl::size_t value)
{
    LockGuard lock(&d_mutex);

    d_server_sp->setAcceptQueueLowWatermark(value);
}

void TestFixture::serverSetAcceptQueueHighWatermark(bsl::size_t value)
{
    LockGuard lock(&d_mutex);

    d_server_sp->setAcceptQueueHighWatermark(value);
}

void TestFixture::serverRelaxFlowControl()
{
    LockGuard lock(&d_mutex);

    d_server_sp->relaxFlowControl();
}

void TestFixture::serverApplyFlowControl()
{
    LockGuard lock(&d_mutex);

    d_server_sp->applyFlowControl();
}

ntsa::Error TestFixture::clientConnect()
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestClient> client = this->createClient();
    error = client->connect();
    if (error) {
        BALL_LOG_ERROR << "Failed to connect: " << error << BALL_LOG_END;
        return error;
    }

    d_clientVector.push_back(client);

    return ntsa::Error();
}

ntsa::Error TestFixture::clientConnect(
    bsl::shared_ptr<ntcf::TestClient>* result)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    result->reset();

    bsl::shared_ptr<ntcf::TestClient> client = this->createClient();
    error = client->connect();
    if (error) {
        BALL_LOG_ERROR << "Failed to connect: " << error << BALL_LOG_END;
        return error;
    }

    d_clientVector.push_back(client);
    *result = client;

    return ntsa::Error();
}


ntsa::Error TestFixture::clientConnect(const ntci::ConnectCallback& callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestClient> client = this->createClient();

    ntci::ConnectCallback callbackProxy = client->createConnectCallback(
        NTCCFG_BIND(&TestFixture::dispatchConnect,
                    this,
                    client,
                    NTCCFG_BIND_PLACEHOLDER_1,
                    NTCCFG_BIND_PLACEHOLDER_2,
                    callback),
        d_allocator_p);

    error = client->connect(callbackProxy);
    if (error) {
        BALL_LOG_ERROR << "Failed to connect: " << error << BALL_LOG_END;
        return error;
    }

    return ntsa::Error();
}


ntsa::Error TestFixture::clientConnect(
    bsl::shared_ptr<ntcf::TestClient>* result,
    const ntci::ConnectCallback&       callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    result->reset();

    bsl::shared_ptr<ntcf::TestClient> client = this->createClient();

    ntci::ConnectCallback callbackProxy = client->createConnectCallback(
    NTCCFG_BIND(&TestFixture::dispatchConnect,
                this,
                client,
                NTCCFG_BIND_PLACEHOLDER_1,
                NTCCFG_BIND_PLACEHOLDER_2,
                callback),
    d_allocator_p);

    error = client->connect(callbackProxy);
    if (error) {
        BALL_LOG_ERROR << "Failed to connect: " << error << BALL_LOG_END;
        return error;
    }

    *result = client;

    return ntsa::Error();
}

ntsa::Error TestFixture::clientConnect(ntcf::TestClientVector* result)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    bsl::shared_ptr<ntcf::TestClient> client = this->createClient();

    error = client->connect();
    if (error) {
        BALL_LOG_ERROR << "Failed to connect: " << error << BALL_LOG_END;
        return error;
    }

    d_clientVector.push_back(client);
    result->push_back(client);

    return ntsa::Error();
}

ntsa::Error TestFixture::clientConnect(ntcf::TestClientVector* result, 
                                       bsl::size_t             count)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    for (bsl::size_t i = 0; i < count; ++i) {
        bsl::shared_ptr<ntcf::TestClient> client = this->createClient();

        error = client->connect();
        if (error) {
            BALL_LOG_ERROR << "Failed to connect: " << error << BALL_LOG_END;
            return error;
        }

        d_clientVector.push_back(client);
        result->push_back(client);
    }

    return ntsa::Error();
}

const bsl::shared_ptr<ntcf::TestClient>& TestFixture::client(bsl::size_t index)
{
    LockGuard lock(&d_mutex);
    return d_clientVector[index];
}

bsl::size_t TestFixture::clientCount() const
{
    LockGuard lock(&d_mutex);
    return d_clientVector.size();
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
