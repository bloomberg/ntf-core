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

#include <ntcdns_client.h>

#include <ntcdns_compat.h>

#include <ntci_log.h>
#include <ntcs_blobutil.h>
#include <ntsa_domainname.h>
#include <ntsa_endpoint.h>
#include <ntsu_resolverutil.h>

#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>

#include <bslmt_lockguard.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>

#include <bsl_ostream.h>

#define NTCDNS_CLIENT_LOG_STARTING(configuration)                             \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE                                                 \
            << "DNS client is starting with configuration = "                 \
            << (configuration) << NTCI_LOG_STREAM_END;                        \
    } while (false)

#define NTCDNS_CLIENT_LOG_STOPPING()                                          \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << "DNS client is stopping"                     \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_LOG_STOPPED()                                           \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << "DNS client has stopped"                     \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_ENCODE_FAILURE(request, error)            \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Failed to encode request " << (request)     \
                              << ": " << (error) << NTCI_LOG_STREAM_END;      \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_DECODE_FAILURE(error)                     \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Failed to decode response: " << (error)     \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_SEND_REFUSAL()                            \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Failed to send: the operation has already " \
                                 "timed out or been cancelled"                \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_SEND_OBJECT(request, endpoint)            \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Sending request " << (request) << " to "    \
                              << (endpoint) << NTCI_LOG_STREAM_END;           \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_SEND_BYTES(requestBlob, endpoint)         \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE                                                 \
            << "Sending " << (requestBlob)->length() << " bytes to "          \
            << (endpoint) << ":\n"                                            \
            << bdlbb::BlobUtilHexDumper((requestBlob).get())                  \
            << NTCI_LOG_STREAM_END;                                           \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_SEND_FAILURE(request, error)              \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Failed to send " << (request)               \
                              << " to datagram socket: " << (error)           \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_STALE_RESPONSE(response,                  \
                                                   expectedServerIndex,       \
                                                   foundServerIndex)          \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG                                                 \
            << "Ignoring response " << (response)                             \
            << " from name server index " << (foundServerIndex)               \
            << ": now expecting response from name server index "             \
            << (expectedServerIndex) << NTCI_LOG_STREAM_END;                  \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_REDUNDANT_RESPONSE(response)              \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Ignoring response " << (response)           \
                              << ": another response or error has already "   \
                                 "completed the operation"                    \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_UNEXPECTED_RESPONSE(response, endpoint)   \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Ignoring response " << (response)           \
                              << " from " << (endpoint)                       \
                              << ": the transaction ID " << (response).id()   \
                              << " is not expected" << NTCI_LOG_STREAM_END;   \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_TTL_MISMATCH(newTtl, oldTtl)              \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Resource record time to live " << (newTtl)  \
                              << " does not match previous resource "         \
                                 "record time to live "                       \
                              << (oldTtl) << NTCI_LOG_STREAM_END;             \
    } while (false)

#define NTCDNS_CLIENT_SERVER_LOG_STARTING(serverIndex, endpoint)              \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << "DNS client name server " << (serverIndex)   \
                              << " to " << (endpoint) << " is starting"       \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_SERVER_LOG_STOPPING(serverIndex, endpoint)              \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << "DNS client name server " << (serverIndex)   \
                              << " to " << (endpoint) << " is stopping"       \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_SERVER_LOG_STOPPED(serverIndex, endpoint)               \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << "DNS client name server " << (serverIndex)   \
                              << " to " << (endpoint) << " has stopped"       \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_SERVER_LOG_RECEIVE_FAILURE(error)                       \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Failed to receive: " << (error)             \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_OPERATION_LOG_RECEIVE_OBJECT(response, endpoint)        \
    do {                                                                      \
        NTCI_LOG_STREAM_DEBUG << "Received response " << (response)           \
                              << " from " << (endpoint)                       \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCDNS_CLIENT_SERVER_LOG_RECEIVE_BYTES(responseBlob, endpoint)        \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE                                                 \
            << "Received " << (responseBlob)->length() << " bytes from "      \
            << (endpoint) << ":\n"                                            \
            << bdlbb::BlobUtilHexDumper((responseBlob).get())                 \
            << NTCI_LOG_STREAM_END;                                           \
    } while (false)

// Some versions of GCC erroneously warn when 'timeToLive.value()' is
// called even when protected by a check of '!timeToLive.isNull()'.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace BloombergLP {
namespace ntcdns {

namespace {

// The maximum UDP payload size.
const bsl::size_t k_UDP_MAX_PAYLOAD_SIZE = 65527;

// The maximum DNS payload size.
const bsl::size_t k_DNS_MAX_PAYLOAD_SIZE = 512;

// The default DNS port.
const ntsa::Port k_DNS_PORT = 53;

bsls::AtomicUint s_generation;

bsl::uint16_t generateTransactionId()
{
    bsl::uint16_t result = 0;

    while (true) {
        unsigned int old = s_generation.load();
        if (old >= 65535) {
            result                = 1;
            unsigned int previous = s_generation.testAndSwap(old, result);
            if (previous == old) {
                break;
            }
        }
        else {
            result = NTCCFG_WARNING_NARROW(bsl::uint16_t, old + 1);
            unsigned int previous = s_generation.testAndSwap(old, result);
            if (previous == old) {
                break;
            }
        }
    }

    return result;
}

}  // close unnamed namespace

ClientOperation::~ClientOperation()
{
}

ClientGetIpAddressOperation::ClientGetIpAddressOperation(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     name,
    const ServerList&                      serverList,
    const SearchList&                      searchList,
    const ntca::GetIpAddressOptions&       options,
    const ntci::GetIpAddressCallback&      callback,
    const bsl::shared_ptr<ntcdns::Cache>&  cache,
    bslma::Allocator*                      basicAllocator)
: d_object("ntcdns::ClientGetIpAddressOperation")
, d_mutex()
, d_resolver_sp(resolver)
, d_name(name, basicAllocator)
, d_serverList(serverList, basicAllocator)
, d_serverIndex(0)
, d_searchList(searchList, basicAllocator)
, d_searchIndex(0)
, d_options(options)
, d_callback(callback, basicAllocator)
, d_timer_sp()
, d_cache_sp(cache)
, d_pending(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ClientGetIpAddressOperation::~ClientGetIpAddressOperation()
{
}

ntsa::Error ClientGetIpAddressOperation::sendRequest(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntsa::Endpoint&                        endpoint,
    bsl::uint16_t                                transactionId)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (d_searchIndex >= d_searchList.size()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_pending) {
        NTCDNS_CLIENT_OPERATION_LOG_SEND_REFUSAL();
        return ntsa::Error(ntsa::Error::e_CANCELLED);
    }

    ntcdns::Message request;

    request.setId(transactionId);
    request.setDirection(ntcdns::Direction::e_REQUEST);
    request.setOperation(ntcdns::Operation::e_STANDARD);

    request.setAa(false);
    request.setAd(false);
    request.setCd(false);
    request.setRa(false);
    request.setRd(true);
    request.setTc(false);

    ntcdns::Question& question = request.addQd();

    question.setName(d_searchList[d_searchIndex]);

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType;
    error = ntcdns::Compat::convert(&ipAddressType, d_options);
    if (error) {
        return error;
    }

    if (ipAddressType.isNull()) {
        question.setType(ntcdns::Type::e_A);
    }
    else {
        if (ipAddressType.value() == ntsa::IpAddressType::e_V4) {
            question.setType(ntcdns::Type::e_A);
        }
        else if (ipAddressType.value() == ntsa::IpAddressType::e_V6) {
            question.setType(ntcdns::Type::e_AAAA);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    question.setClassification(ntcdns::Classification::e_INTERNET);

    bsl::shared_ptr<bdlbb::Blob> requestBlob =
        datagramSocket->createOutgoingBlob();

    requestBlob->setLength(k_DNS_MAX_PAYLOAD_SIZE);

    BSLS_ASSERT_OPT(requestBlob->numDataBuffers() == 1);
    BSLS_ASSERT_OPT(requestBlob->numBuffers() == 1);

    ntcdns::MemoryEncoder encoder(
        reinterpret_cast<bsl::uint8_t*>(requestBlob->buffer(0).data()),
        requestBlob->buffer(0).size());

    bsl::size_t p0 = encoder.position();

    error = request.encode(&encoder);
    if (error) {
        NTCDNS_CLIENT_OPERATION_LOG_ENCODE_FAILURE(request, error);
        return error;
    }

    bsl::size_t p1          = encoder.position();
    bsl::size_t requestSize = p1 - p0;

    ntcs::BlobUtil::resize(requestBlob, requestSize);

    BSLS_ASSERT_OPT(requestBlob->numDataBuffers() == 1);
    BSLS_ASSERT_OPT(requestBlob->numBuffers() == 1);

    NTCDNS_CLIENT_OPERATION_LOG_SEND_OBJECT(request, endpoint);
    NTCDNS_CLIENT_OPERATION_LOG_SEND_BYTES(requestBlob, endpoint);

    ntca::SendOptions sendOptions;
    sendOptions.setEndpoint(endpoint);

    error = datagramSocket->send(*requestBlob, sendOptions);
    if (error) {
        NTCDNS_CLIENT_OPERATION_LOG_SEND_FAILURE(request, error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ClientGetIpAddressOperation::sendRequest(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntsa::Endpoint&                      endpoint,
    bsl::uint16_t                              transactionId)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(endpoint);
    NTCCFG_WARNING_UNUSED(transactionId);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void ClientGetIpAddressOperation::processResponse(
    const ntcdns::Message&    response,
    const ntsa::Endpoint&     endpoint,
    bsl::size_t               serverIndex,
    const bsls::TimeInterval& now)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (serverIndex != d_serverIndex) {
        NTCDNS_CLIENT_OPERATION_LOG_STALE_RESPONSE(response,
                                                   d_serverIndex,
                                                   serverIndex);
        return;
    }

    if (d_pending.swap(false) == false) {
        NTCDNS_CLIENT_OPERATION_LOG_REDUNDANT_RESPONSE(response);
        return;
    }

    if (d_timer_sp) {
        d_timer_sp->close();
    }

    bsl::vector<ntsa::IpAddress> ipAddressList;
    ntca::GetIpAddressContext    context;

    context.setSource(ntca::ResolverSource::e_SERVER);
    context.setNameServer(endpoint);

    if (response.qdcount() > 0) {
        const ntcdns::Question& question = response.qd(0);
        context.setDomainName(question.name());
    }

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType;
    ntcdns::Compat::convert(&ipAddressType, d_options);

    bdlb::NullableValue<bsl::size_t> timeToLive;

    if (response.ancount() > 0) {
        for (bsl::size_t i = 0; i < response.ancount(); ++i) {
            const ntcdns::ResourceRecord& answer = response.an(i);

            if (answer.rdata().isIpv4Value()) {
                BSLMF_ASSERT(sizeof answer.rdata().ipv4() == 4);
                ntsa::Ipv4Address ipv4Address;
                ipv4Address.copyFrom(&answer.rdata().ipv4(),
                                     sizeof answer.rdata().ipv4());

                ntsa::IpAddress ipAddress(ipv4Address);

                if (ipAddressType.isNull() ||
                    ipAddressType.value() == ntsa::IpAddressType::e_V4)
                {
                    ipAddressList.push_back(ipAddress);

                    if (timeToLive.isNull()) {
                        timeToLive.makeValue(answer.ttl());
                    }
                    else {
                        bsl::size_t timeToLiveValue = timeToLive.value();
                        if (timeToLiveValue != answer.ttl()) {
                            NTCDNS_CLIENT_OPERATION_LOG_TTL_MISMATCH(
                                answer.ttl(),
                                timeToLiveValue);
                            if (answer.ttl() < timeToLiveValue) {
                                timeToLive = answer.ttl();
                            }
                        }
                    }
                }

                if (d_cache_sp) {
                    d_cache_sp->updateHost(context.domainName(),
                                           ipAddress,
                                           endpoint,
                                           answer.ttl(),
                                           now);

                    if (d_name != context.domainName()) {
                        d_cache_sp->updateHost(d_name,
                                               ipAddress,
                                               endpoint,
                                               answer.ttl(),
                                               now);
                    }
                }
            }
            else if (answer.rdata().isIpv6Value()) {
                BSLMF_ASSERT(sizeof answer.rdata().ipv6() == 16);
                ntsa::Ipv6Address ipv6Address;
                ipv6Address.copyFrom(&answer.rdata().ipv6(),
                                     sizeof answer.rdata().ipv6());

                ntsa::IpAddress ipAddress(ipv6Address);

                if (ipAddressType.isNull() ||
                    ipAddressType.value() == ntsa::IpAddressType::e_V6)
                {
                    ipAddressList.push_back(ipAddress);

                    if (timeToLive.isNull()) {
                        timeToLive.makeValue(answer.ttl());
                    }
                    else {
                        bsl::size_t timeToLiveValue = timeToLive.value();
                        if (timeToLiveValue != answer.ttl()) {
                            NTCDNS_CLIENT_OPERATION_LOG_TTL_MISMATCH(
                                answer.ttl(),
                                timeToLiveValue);
                            if (answer.ttl() < timeToLiveValue) {
                                timeToLive = answer.ttl();
                            }
                        }
                    }
                }

                if (d_cache_sp) {
                    d_cache_sp->updateHost(context.domainName(),
                                           ipAddress,
                                           endpoint,
                                           answer.ttl(),
                                           now);

                    if (d_name != context.domainName()) {
                        d_cache_sp->updateHost(d_name,
                                               ipAddress,
                                               endpoint,
                                               answer.ttl(),
                                               now);
                    }
                }
            }
        }
    }

    if (ipAddressType.isNull()) {
        ntsu::ResolverUtil::sortIpAddressList(&ipAddressList);
    }

    ntca::GetIpAddressEvent event;

    if (ipAddressList.empty()) {
        event.setType(ntca::GetIpAddressEventType::e_ERROR);
        context.setError(ntsa::Error(ntsa::Error::e_EOF));
    }
    else {
        event.setType(ntca::GetIpAddressEventType::e_COMPLETE);

        if (!timeToLive.isNull()) {
            context.setTimeToLive(timeToLive.value());
        }

        if (!d_options.ipAddressSelector().isNull()) {
            ntsa::IpAddress ipAddress =
                ipAddressList[d_options.ipAddressSelector().value() %
                              ipAddressList.size()];
            ipAddressList.clear();
            ipAddressList.push_back(ipAddress);
        }
    }

    event.setContext(context);

    d_callback(d_resolver_sp, ipAddressList, event, ntci::Strand::unknown());
    d_callback.reset();

    d_resolver_sp.reset();
    d_serverList.clear();
}

void ClientGetIpAddressOperation::processError(const ntsa::Error& error)
{
    if (d_pending.swap(false) == false) {
        return;
    }

    if (d_timer_sp) {
        d_timer_sp->close();
    }

    bsl::vector<ntsa::IpAddress> ipAddressList;

    ntca::GetIpAddressContext context;
    context.setDomainName(d_name);
    context.setError(error);

    ntca::GetIpAddressEvent event;
    event.setType(ntca::GetIpAddressEventType::e_ERROR);
    event.setContext(context);

    d_callback(d_resolver_sp, ipAddressList, event, ntci::Strand::unknown());
    d_callback.reset();

    d_resolver_sp.reset();
    d_serverList.clear();
}

bsl::shared_ptr<ntcdns::ClientNameServer> ClientGetIpAddressOperation::
    tryNextServer()
{
    LockGuard lock(&d_mutex);

    bsl::shared_ptr<ntcdns::ClientNameServer> result;

    if (d_serverIndex < d_serverList.size() - 1) {
        ++d_serverIndex;
        d_searchIndex = 0;
        result        = d_serverList[d_serverIndex];
    }

    return result;
}

bool ClientGetIpAddressOperation::tryNextSearch()
{
    LockGuard lock(&d_mutex);

    if (d_searchIndex < d_searchList.size() - 1) {
        ++d_searchIndex;
        return true;
    }

    return false;
}

const bsl::string& ClientGetIpAddressOperation::name() const
{
    return d_name;
}

const ntca::GetIpAddressOptions& ClientGetIpAddressOperation::options() const
{
    return d_options;
}

bsl::size_t ClientGetIpAddressOperation::serverIndex() const
{
    LockGuard lock(&d_mutex);
    return d_serverIndex;
}

bsl::size_t ClientGetIpAddressOperation::searchIndex() const
{
    LockGuard lock(&d_mutex);
    return d_searchIndex;
}

ClientGetDomainNameOperation::ClientGetDomainNameOperation(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::IpAddress&                 ipAddress,
    const ServerList&                      serverList,
    const ntca::GetDomainNameOptions&      options,
    const ntci::GetDomainNameCallback&     callback,
    const bsl::shared_ptr<ntcdns::Cache>&  cache,
    bslma::Allocator*                      basicAllocator)
: d_object("ntcdns::ClientGetDomainNameOperation")
, d_mutex()
, d_resolver_sp(resolver)
, d_ipAddress(ipAddress)
, d_serverList(serverList, basicAllocator)
, d_serverIndex(0)
, d_options(options)
, d_callback(callback, basicAllocator)
, d_timer_sp()
, d_cache_sp(cache)
, d_pending(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ClientGetDomainNameOperation::~ClientGetDomainNameOperation()
{
}

ntsa::Error ClientGetDomainNameOperation::sendRequest(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntsa::Endpoint&                        endpoint,
    bsl::uint16_t                                transactionId)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (!d_pending) {
        NTCDNS_CLIENT_OPERATION_LOG_SEND_REFUSAL();
        return ntsa::Error(ntsa::Error::e_CANCELLED);
    }

    ntcdns::Message request;

    request.setId(transactionId);
    request.setDirection(ntcdns::Direction::e_REQUEST);
    request.setOperation(ntcdns::Operation::e_STANDARD);

    request.setAa(false);
    request.setAd(false);
    request.setCd(false);
    request.setRa(false);
    request.setRd(true);
    request.setTc(false);

    ntcdns::Question& question = request.addQd();

    if (d_ipAddress.isV4()) {
        bsl::string arpaName;
        {
            bsl::uint32_t originalValue = d_ipAddress.v4().value();
            bsl::uint32_t reversedValue =
                bsls::ByteOrderUtil::swapBytes32(originalValue);

            arpaName.assign(ntsa::Ipv4Address(reversedValue).text());
            arpaName.append(".in-addr.arpa");
        }

        question.setName(arpaName);
        question.setType(ntcdns::Type::e_PTR);
        question.setClassification(ntcdns::Classification::e_INTERNET);
    }
    else if (d_ipAddress.isV6()) {
        bsl::string arpaName;
        {
            // MRM: Reverse bytes in the IPv6 address?
            return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
        }

        question.setName(arpaName);
        question.setType(ntcdns::Type::e_PTR);
        question.setClassification(ntcdns::Classification::e_INTERNET);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<bdlbb::Blob> requestBlob =
        datagramSocket->createOutgoingBlob();

    requestBlob->setLength(k_DNS_MAX_PAYLOAD_SIZE);

    BSLS_ASSERT_OPT(requestBlob->numDataBuffers() == 1);
    BSLS_ASSERT_OPT(requestBlob->numBuffers() == 1);

    ntcdns::MemoryEncoder encoder(
        reinterpret_cast<bsl::uint8_t*>(requestBlob->buffer(0).data()),
        requestBlob->buffer(0).size());

    bsl::size_t p0 = encoder.position();

    error = request.encode(&encoder);
    if (error) {
        NTCDNS_CLIENT_OPERATION_LOG_ENCODE_FAILURE(request, error);
        return error;
    }

    bsl::size_t p1          = encoder.position();
    bsl::size_t requestSize = p1 - p0;

    ntcs::BlobUtil::resize(requestBlob, requestSize);

    BSLS_ASSERT_OPT(requestBlob->numDataBuffers() == 1);
    BSLS_ASSERT_OPT(requestBlob->numBuffers() == 1);

    NTCDNS_CLIENT_OPERATION_LOG_SEND_OBJECT(request, endpoint);
    NTCDNS_CLIENT_OPERATION_LOG_SEND_BYTES(requestBlob, endpoint);

    ntca::SendOptions sendOptions;
    sendOptions.setEndpoint(endpoint);

    error = datagramSocket->send(*requestBlob, sendOptions);
    if (error) {
        NTCDNS_CLIENT_OPERATION_LOG_SEND_FAILURE(request, error);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ClientGetDomainNameOperation::sendRequest(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntsa::Endpoint&                      endpoint,
    bsl::uint16_t                              transactionId)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(endpoint);
    NTCCFG_WARNING_UNUSED(transactionId);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void ClientGetDomainNameOperation::processResponse(
    const ntcdns::Message&    response,
    const ntsa::Endpoint&     endpoint,
    bsl::size_t               serverIndex,
    const bsls::TimeInterval& now)
{
    NTCCFG_WARNING_UNUSED(now);

    NTCI_LOG_CONTEXT();

    if (serverIndex != d_serverIndex) {
        NTCDNS_CLIENT_OPERATION_LOG_STALE_RESPONSE(response,
                                                   d_serverIndex,
                                                   serverIndex);
        return;
    }

    if (d_pending.swap(false) == false) {
        NTCDNS_CLIENT_OPERATION_LOG_REDUNDANT_RESPONSE(response);
        return;
    }

    if (d_timer_sp) {
        d_timer_sp->close();
    }

    bsl::string                domainName;
    ntca::GetDomainNameContext context;

    context.setIpAddress(d_ipAddress);
    context.setSource(ntca::ResolverSource::e_SERVER);
    context.setNameServer(endpoint);

    bdlb::NullableValue<bsl::size_t> timeToLive;

    if (response.ancount() > 0) {
        for (bsl::size_t i = 0; i < response.ancount(); ++i) {
            const ntcdns::ResourceRecord& answer = response.an(i);

            if (timeToLive.isNull()) {
                timeToLive.makeValue(answer.ttl());
            }
            else {
                bsl::size_t timeToLiveValue = timeToLive.value();
                if (timeToLiveValue != answer.ttl()) {
                    NTCDNS_CLIENT_OPERATION_LOG_TTL_MISMATCH(answer.ttl(),
                                                             timeToLiveValue);
                    if (answer.ttl() < timeToLiveValue) {
                        timeToLive = answer.ttl();
                    }
                }
            }

            if (answer.rdata().isPointerValue()) {
                domainName = answer.rdata().pointer().ptrdname();
            }
        }
    }

    if (!timeToLive.isNull()) {
        context.setTimeToLive(timeToLive.value());
    }

    ntca::GetDomainNameEvent event;
    event.setType(ntca::GetDomainNameEventType::e_COMPLETE);
    event.setContext(context);

    d_callback(d_resolver_sp, domainName, event, ntci::Strand::unknown());
    d_callback.reset();

    d_resolver_sp.reset();
    d_serverList.clear();
}

void ClientGetDomainNameOperation::processError(const ntsa::Error& error)
{
    if (d_pending.swap(false) == false) {
        return;
    }

    if (d_timer_sp) {
        d_timer_sp->close();
    }

    bsl::string domainName;

    ntca::GetDomainNameContext context;
    context.setIpAddress(d_ipAddress);
    context.setError(error);

    ntca::GetDomainNameEvent event;
    event.setType(ntca::GetDomainNameEventType::e_ERROR);
    event.setContext(context);

    d_callback(d_resolver_sp, domainName, event, ntci::Strand::unknown());
    d_callback.reset();

    d_resolver_sp.reset();
    d_serverList.clear();
}

bsl::shared_ptr<ntcdns::ClientNameServer> ClientGetDomainNameOperation::
    tryNextServer()
{
    LockGuard lock(&d_mutex);

    bsl::shared_ptr<ntcdns::ClientNameServer> result;

    if (d_serverIndex < d_serverList.size() - 1) {
        ++d_serverIndex;
        result = d_serverList[d_serverIndex];
    }

    return result;
}

bool ClientGetDomainNameOperation::tryNextSearch()
{
    return false;
}

const ntsa::IpAddress& ClientGetDomainNameOperation::ipAddress() const
{
    return d_ipAddress;
}

const ntca::GetDomainNameOptions& ClientGetDomainNameOperation::options() const
{
    return d_options;
}

bsl::size_t ClientGetDomainNameOperation::serverIndex() const
{
    LockGuard lock(&d_mutex);
    return d_serverIndex;
}

bsl::size_t ClientGetDomainNameOperation::searchIndex() const
{
    return 0;
}

void ClientNameServer::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(event);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntsa::Endpoint               endpoint;
    bsl::shared_ptr<bdlbb::Blob> responseBlob =
        datagramSocket->createIncomingBlob();

    {
        ntca::ReceiveContext receiveContext;
        ntca::ReceiveOptions receiveOptions;

        error = datagramSocket->receive(&receiveContext,
                                        responseBlob.get(),
                                        receiveOptions);
        if (error) {
            if (error != ntsa::Error(ntsa::Error::e_EOF)) {
                NTCDNS_CLIENT_SERVER_LOG_RECEIVE_FAILURE(error);
            }
            return;
        }

        if (!receiveContext.endpoint().isNull()) {
            endpoint = receiveContext.endpoint().value();
        }
    }

    BSLS_ASSERT_OPT(responseBlob->numDataBuffers() == 1);
    BSLS_ASSERT_OPT(responseBlob->numBuffers() == 1);

    NTCDNS_CLIENT_SERVER_LOG_RECEIVE_BYTES(responseBlob, endpoint);

    ntcdns::Message response(d_allocator_p);

    ntcdns::MemoryDecoder decoder(
        reinterpret_cast<bsl::uint8_t*>(responseBlob->buffer(0).data()),
        responseBlob->length());

    error = response.decode(&decoder);
    if (error) {
        NTCDNS_CLIENT_OPERATION_LOG_DECODE_FAILURE(error);
        return;
    }

    NTCDNS_CLIENT_OPERATION_LOG_RECEIVE_OBJECT(response, endpoint);

    bsl::shared_ptr<ntcdns::ClientOperation> operation;
    if (!d_operationMap.remove(&operation, response.id())) {
        NTCDNS_CLIENT_OPERATION_LOG_UNEXPECTED_RESPONSE(response, endpoint);
        return;
    }

    bool tryNextServer = false;

    if (response.error() == ntcdns::Error::e_OK) {
        operation->processResponse(response,
                                   d_endpoint,
                                   d_index,
                                   datagramSocket->currentTime());
    }
    else if (response.tc()) {
        // MRM: If truncated, try on TCP socket.
    }
    else {
        if (response.error() == ntcdns::Error::e_NAME_ERROR) {
            // MRM: Name was not found on this name server. Try again with a
            // different name prefixed with the next scope.

            if (operation->tryNextSearch()) {
                bsl::uint16_t transactionId = generateTransactionId();

                if (!d_operationMap.add(transactionId, operation)) {
                    operation->processError(
                        ntsa::Error(ntsa::Error::e_INVALID));
                    return;
                }

                error = operation->sendRequest(datagramSocket,
                                               d_endpoint,
                                               transactionId);
                if (error) {
                    d_operationMap.remove(transactionId);
                    tryNextServer = true;
                }
            }
            else {
                tryNextServer = true;
            }
        }
        else if (response.error() == ntcdns::Error::e_REFUSED ||
                 response.error() == ntcdns::Error::e_SERVER_FAILURE ||
                 response.error() == ntcdns::Error::e_NOT_IMPLEMENTED)
        {
            tryNextServer = true;
        }
        else if (response.error() == ntcdns::Error::e_FORMAT_ERROR) {
            operation->processError(ntsa::Error(ntsa::Error::e_INVALID));
        }
        else {
            operation->processError(ntsa::Error(ntsa::Error::e_INVALID));
        }
    }

    if (tryNextServer) {
        while (true) {
            bsl::shared_ptr<ntcdns::ClientNameServer> nameServer =
                operation->tryNextServer();

            if (nameServer) {
                error = nameServer->initiate(operation);
                if (error) {
                    continue;
                }
                else {
                    break;
                }
            }
            else {
                operation->processError(ntsa::Error(ntsa::Error::e_EOF));
                break;
            }
        }
    }
}

void ClientNameServer::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReadQueueEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);

    this->flush();
}

void ClientNameServer::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::WriteQueueEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processShutdownInitiated(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processShutdownReceive(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processShutdownSend(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processShutdownComplete(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ShutdownEvent&                   event)
{
    NTCCFG_WARNING_UNUSED(event);

    ntccfg::ConditionMutexGuard stateSocketLock(&d_stateMutex);

    LockGuard datagramSocketLock(&d_datagramSocketMutex);

    LockGuard streamSocketLock(&d_streamSocketMutex);

    if (datagramSocket == d_datagramSocket_sp) {
        d_datagramSocket_sp->registerSession(
            bsl::shared_ptr<ntci::DatagramSocketSession>());
        d_datagramSocket_sp.reset();

        if (!d_datagramSocket_sp && !d_streamSocket_sp) {
            if (d_state == e_STATE_STOPPING) {
                d_state = e_STATE_STOPPED;
                d_stateCondition.signal();
            }
        }
    }
}

void ClientNameServer::processError(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ErrorEvent&                      event)
{
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processReadQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processReadQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ReadQueueEvent&                event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processWriteQueueLowWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);

    this->flush();
}

void ClientNameServer::processWriteQueueHighWatermark(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::WriteQueueEvent&               event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processShutdownInitiated(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processShutdownReceive(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processShutdownSend(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

void ClientNameServer::processShutdownComplete(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ShutdownEvent&                 event)
{
    NTCCFG_WARNING_UNUSED(event);

    ntccfg::ConditionMutexGuard stateSocketLock(&d_stateMutex);

    LockGuard datagramSocketLock(&d_datagramSocketMutex);

    LockGuard streamSocketLock(&d_streamSocketMutex);

    if (streamSocket == d_streamSocket_sp) {
        d_streamSocket_sp->registerSession(
            bsl::shared_ptr<ntci::StreamSocketSession>());
        d_streamSocket_sp.reset();

        if (!d_datagramSocket_sp && !d_streamSocket_sp) {
            if (d_state == e_STATE_STOPPING) {
                d_state = e_STATE_STOPPED;
                d_stateCondition.signal();
            }
        }
    }
}

void ClientNameServer::processError(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const ntca::ErrorEvent&                    event)
{
    NTCCFG_WARNING_UNUSED(streamSocket);
    NTCCFG_WARNING_UNUSED(event);
}

ntsa::Error ClientNameServer::createDatagramSocket()
{
    ntsa::Error error;

    bsl::shared_ptr<ClientNameServer> self = this->getSelf(this);

    if (d_datagramSocket_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::DatagramSocketOptions datagramSocketOptions;

    if (d_endpoint.isIp()) {
        if (d_endpoint.ip().host().isV4()) {
            datagramSocketOptions.setSourceEndpoint(
                ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0)));
        }
        else if (d_endpoint.ip().host().isV6()) {
            datagramSocketOptions.setSourceEndpoint(
                ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::any(), 0)));
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (d_endpoint.isLocal()) {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        if (error) {
            return error;
        }
        datagramSocketOptions.setSourceEndpoint(ntsa::Endpoint(localName));
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    datagramSocketOptions.setMaxDatagramSize(k_UDP_MAX_PAYLOAD_SIZE);

    bsl::shared_ptr<ntci::DatagramSocket> datagramSocket =
        d_datagramSocketFactory_sp->createDatagramSocket(datagramSocketOptions,
                                                         d_allocator_p);

    error = datagramSocket->registerSession(self);
    if (error) {
        d_datagramSocket_sp->close();
        return error;
    }

    error = datagramSocket->open();
    if (error) {
        datagramSocket->close();
        return error;
    }

    ntca::ConnectOptions connectOptions;

    ntci::ConnectCallback connectCallback =
        datagramSocket->createConnectCallback(bdlf::BindUtil::bind(
            &ClientNameServer::processDatagramSocketConnected,
            self,
            datagramSocket,
            bdlf::PlaceHolders::_1,
            bdlf::PlaceHolders::_2));

    error =
        datagramSocket->connect(d_endpoint, connectOptions, connectCallback);
    if (error) {
        datagramSocket->close();
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ClientNameServer::createStreamSocket()
{
    ntsa::Error error;

    bsl::shared_ptr<ClientNameServer> self = this->getSelf(this);

    if (d_streamSocket_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::StreamSocketOptions streamSocketOptions;

    if (d_endpoint.isIp()) {
        if (d_endpoint.ip().host().isV4()) {
            streamSocketOptions.setSourceEndpoint(
                ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0)));
        }
        else if (d_endpoint.ip().host().isV6()) {
            streamSocketOptions.setSourceEndpoint(
                ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv6Address::any(), 0)));
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (d_endpoint.isLocal()) {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);
        if (error) {
            return error;
        }
        streamSocketOptions.setSourceEndpoint(ntsa::Endpoint(localName));
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    streamSocketOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    bsl::shared_ptr<ntci::StreamSocket> streamSocket =
        d_streamSocketFactory_sp->createStreamSocket(streamSocketOptions,
                                                     d_allocator_p);

    error = streamSocket->registerSession(self);
    if (error) {
        return error;
    }

    error = streamSocket->open();
    if (error) {
        return error;
    }

    ntca::ConnectOptions connectOptions;

    ntci::ConnectCallback connectCallback =
        streamSocket->createConnectCallback(
            bdlf::BindUtil::bind(
                &ClientNameServer::processStreamSocketConnected,
                self,
                streamSocket,
                bdlf::PlaceHolders::_1,
                bdlf::PlaceHolders::_2),
            d_allocator_p);

    error = streamSocket->connect(d_endpoint, connectOptions, connectCallback);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

void ClientNameServer::processDatagramSocketConnected(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const bsl::shared_ptr<ntci::Connector>&      connector,
    const ntca::ConnectEvent&                    event)
{
    NTCCFG_WARNING_UNUSED(connector);

    ntsa::Error error;

    bsl::shared_ptr<ClientNameServer> self = this->getSelf(this);

    if (event.context().error()) {
        // MRM
        return;
    }

    LockGuard datagramSocketLock(&d_datagramSocketMutex);

    d_datagramSocket_sp = datagramSocket;

    error = d_datagramSocket_sp->relaxFlowControl(
        ntca::FlowControlType::e_RECEIVE);
    if (error) {
        d_datagramSocket_sp->close();
        d_datagramSocket_sp.reset();
        return;
    }

    this->flush();
}

void ClientNameServer::processStreamSocketConnected(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket,
    const bsl::shared_ptr<ntci::Connector>&    connector,
    const ntca::ConnectEvent&                  event)
{
    NTCCFG_WARNING_UNUSED(connector);

    ntsa::Error error;

    bsl::shared_ptr<ClientNameServer> self = this->getSelf(this);

    if (event.context().error()) {
        // MRM
        return;
    }

    LockGuard streamSocketLock(&d_streamSocketMutex);

    d_streamSocket_sp = streamSocket;

    error =
        d_streamSocket_sp->relaxFlowControl(ntca::FlowControlType::e_RECEIVE);
    if (error) {
        d_streamSocket_sp->close();
        d_streamSocket_sp.reset();
        return;
    }

    this->flush();
}

void ClientNameServer::flush()
{
    ntsa::Error error;

    bsl::shared_ptr<ntcdns::ClientOperation> operation;
    while (d_operationQueue.pop(&operation)) {
        bsl::uint16_t transactionId = generateTransactionId();

        if (!d_operationMap.add(transactionId, operation)) {
            return;  // MRM: transaction ID collision
        }

        error = operation->sendRequest(d_datagramSocket_sp,
                                       d_endpoint,
                                       transactionId);
        if (error) {
            d_operationMap.remove(transactionId);

            while (true) {
                bsl::shared_ptr<ntcdns::ClientNameServer> nameServer =
                    operation->tryNextServer();

                if (nameServer) {
                    error = nameServer->initiate(operation);
                    if (error) {
                        continue;
                    }
                    else {
                        break;
                    }
                }
                else {
                    operation->processError(ntsa::Error(ntsa::Error::e_EOF));
                    break;
                }
            }
        }
    }
}

ClientNameServer::ClientNameServer(
    const bsl::shared_ptr<ntci::DatagramSocketFactory>& datagramSocketFactory,
    const bsl::shared_ptr<ntci::StreamSocketFactory>&   streamSocketFactory,
    const ntsa::Endpoint&                               endpoint,
    bsl::size_t                                         index,
    const ntcdns::ClientConfig&                         configuration,
    bslma::Allocator*                                   basicAllocator)
: d_object("ntcdns::ClientNameServer")
, d_operationQueue(basicAllocator)
, d_operationMap(basicAllocator)
, d_datagramSocketMutex()
, d_datagramSocket_sp()
, d_datagramSocketFactory_sp(datagramSocketFactory)
, d_streamSocketMutex()
, d_streamSocket_sp()
, d_streamSocketFactory_sp(streamSocketFactory)
, d_stateMutex()
, d_stateCondition()
, d_state(e_STATE_STOPPED)
, d_endpoint(endpoint)
, d_index(index)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(d_datagramSocketFactory_sp);
    BSLS_ASSERT_OPT(d_streamSocketFactory_sp);
    BSLS_ASSERT_OPT(!d_endpoint.isUndefined());
}

ClientNameServer::~ClientNameServer()
{
}

ntsa::Error ClientNameServer::start()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ClientNameServer> self = this->getSelf(this);

    ntccfg::ConditionMutexGuard stateLock(&d_stateMutex);

    if (d_state == e_STATE_STARTED) {
        return ntsa::Error();
    }
    else if (d_state == e_STATE_STOPPING) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    BSLS_ASSERT_OPT(!d_endpoint.isUndefined());

    NTCDNS_CLIENT_SERVER_LOG_STARTING(d_index, d_endpoint);

    d_state = e_STATE_STARTED;

    return ntsa::Error();
}

ntsa::Error ClientNameServer::initiate(
    const bsl::shared_ptr<ntcdns::ClientOperation>& operation)
{
    ntsa::Error error;

    ntccfg::ConditionMutexGuard stateLock(&d_stateMutex);

    if (d_state != e_STATE_STARTED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool flushFlag = false;

    d_operationQueue.push(operation);

    {
        LockGuard datagramSocketLock(
            &d_datagramSocketMutex);

        if (!d_datagramSocket_sp) {
            error = this->createDatagramSocket();
            if (error) {
                return error;
            }
        }
        else {
            flushFlag = true;
        }
    }

    if (flushFlag) {
        this->flush();
    }

    return ntsa::Error();
}

void ClientNameServer::cancel(
    const bsl::shared_ptr<ntcdns::ClientOperation>& operation)
{
    if (!d_operationMap.removeValue(operation)) {
        d_operationQueue.remove(operation);
    }

    operation->processError(ntsa::Error(ntsa::Error::e_CANCELLED));
}

void ClientNameServer::cancelAll()
{
    OperationVector operationVector(d_allocator_p);

    {
        OperationMap operationMap(d_allocator_p);
        operationMap.swap(&d_operationMap);

        operationMap.values(&operationVector);
    }

    {
        OperationQueue operationQueue(d_allocator_p);
        operationQueue.swap(&d_operationQueue);

        operationQueue.load(&operationVector);
    }

    for (OperationVector::iterator it = operationVector.begin();
         it != operationVector.end();
         ++it)
    {
        const bsl::shared_ptr<ntcdns::ClientOperation>& operation = *it;
        operation->processError(ntsa::Error(ntsa::Error::e_CANCELLED));
    }
}

void ClientNameServer::abandon(
    const bsl::shared_ptr<ntcdns::ClientOperation>& operation)
{
    if (!d_operationMap.removeValue(operation)) {
        d_operationQueue.remove(operation);
    }
}

void ClientNameServer::abandonAll()
{
    d_operationMap.clear();
    d_operationQueue.clear();
}

void ClientNameServer::shutdown()
{
    NTCI_LOG_CONTEXT();

    ntccfg::ConditionMutexGuard stateLock(&d_stateMutex);

    if (d_state != e_STATE_STARTED) {
        return;
    }

    NTCDNS_CLIENT_SERVER_LOG_STOPPING(d_index, d_endpoint);

    d_state = e_STATE_STOPPING;

    this->cancelAll();

    LockGuard datagramSocketLock(&d_datagramSocketMutex);

    LockGuard streamSocketLock(&d_streamSocketMutex);

    if (!d_datagramSocket_sp && !d_streamSocket_sp) {
        d_state = e_STATE_STOPPED;
        d_stateCondition.signal();
    }
    else {
        if (d_datagramSocket_sp) {
            d_datagramSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                          ntsa::ShutdownMode::e_IMMEDIATE);
            d_datagramSocket_sp->close();
        }

        if (d_streamSocket_sp) {
            d_streamSocket_sp->shutdown(ntsa::ShutdownType::e_BOTH,
                                        ntsa::ShutdownMode::e_IMMEDIATE);
            d_streamSocket_sp->close();
        }
    }
}

void ClientNameServer::linger()
{
    NTCI_LOG_CONTEXT();

    ntccfg::ConditionMutexGuard stateLock(&d_stateMutex);

    while (d_state != e_STATE_STOPPED) {
        d_stateCondition.wait(&d_stateMutex);
    }

    d_operationMap.clear();
    d_operationQueue.clear();

    d_datagramSocket_sp.reset();
    d_streamSocket_sp.reset();

    // MRM: d_datagramSocketFactory_sp.reset();
    // MRM: d_streamSocketFactory_sp.reset();

    NTCDNS_CLIENT_SERVER_LOG_STOPPED(d_index, d_endpoint);
}

const ntsa::Endpoint& ClientNameServer::endpoint() const
{
    return d_endpoint;
}

ntsa::Error Client::initialize()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<Client> self = this->getSelf(this);

    if (d_initialized) {
        return ntsa::Error();
    }

    BSLS_ASSERT_OPT(d_datagramSocketFactory_sp);
    BSLS_ASSERT_OPT(d_streamSocketFactory_sp);

    BSLS_ASSERT(d_config.nameServer().size() > 0);

    ServerList serverList(d_allocator_p);
    serverList.reserve(d_config.nameServer().size());

    for (bsl::size_t nameServerIndex = 0;
         nameServerIndex < d_config.nameServer().size();
         ++nameServerIndex)
    {
        const ntcdns::NameServerConfig& nameServerConfig =
            d_config.nameServer()[nameServerIndex];

        ntsa::Endpoint nameServerEndpoint;
        {
            ntsa::IpAddress nameServerIpAddress;
            if (nameServerIpAddress.parse(nameServerConfig.address().host())) {
                ntsa::Port nameServerPort = k_DNS_PORT;
                if (!nameServerConfig.address().port().isNull()) {
                    nameServerPort = nameServerConfig.address().port().value();
                }
                nameServerEndpoint = ntsa::Endpoint(
                    ntsa::IpEndpoint(nameServerIpAddress, nameServerPort));
            }
            else {
                ntsa::LocalName localName;
                localName.setValue(nameServerConfig.address().host());
                nameServerEndpoint = ntsa::Endpoint(localName);
            }
        }

        NTCI_LOG_STREAM_DEBUG << "Client starting name server to "
                              << nameServerEndpoint << NTCI_LOG_STREAM_END;

        bsl::shared_ptr<ntcdns::ClientNameServer> server;
        server.createInplace(d_allocator_p,
                             d_datagramSocketFactory_sp,
                             d_streamSocketFactory_sp,
                             nameServerEndpoint,
                             nameServerIndex,
                             d_config,
                             d_allocator_p);

        error = server->start();
        if (error) {
            return error;
        }

        serverList.push_back(server);
    }

    d_serverList.swap(serverList);
    d_initialized = true;

    return ntsa::Error();
}

Client::Client(
    const ntcdns::ClientConfig&                         configuration,
    const bsl::shared_ptr<ntcdns::Cache>&               cache,
    const bsl::shared_ptr<ntci::DatagramSocketFactory>& datagramSocketFactory,
    const bsl::shared_ptr<ntci::StreamSocketFactory>&   streamSocketFactory,
    bslma::Allocator*                                   basicAllocator)
: d_object("ntcdns::Client")
, d_mutex()
, d_datagramSocketFactory_sp(datagramSocketFactory)
, d_streamSocketFactory_sp(streamSocketFactory)
, d_cache_sp(cache)
, d_serverList(basicAllocator)
, d_state(e_STATE_STOPPED)
, d_initialized(false)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Client::~Client()
{
    this->shutdown();
    this->linger();
}

ntsa::Error Client::start()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    LockGuard lock(&d_mutex);

    if (d_state == e_STATE_STARTED) {
        return ntsa::Error();
    }
    else if (d_state == e_STATE_STOPPING) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    NTCDNS_CLIENT_LOG_STARTING(d_config);

    if (d_config.nameServer().empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_config.search().empty()) {
        if (d_config.domain().isNull()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_config.search().push_back(d_config.domain().value());
    }

    d_state = e_STATE_STARTED;

    return ntsa::Error();
}

void Client::shutdown()
{
    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    if (d_state != e_STATE_STARTED) {
        return;
    }

    NTCDNS_CLIENT_LOG_STOPPING();

    d_state = e_STATE_STOPPING;

    for (bsl::size_t nameServerIndex = 0;
         nameServerIndex < d_serverList.size();
         ++nameServerIndex)
    {
        const bsl::shared_ptr<ntcdns::ClientNameServer>& server =
            d_serverList[nameServerIndex];

        server->shutdown();
    }
}

void Client::linger()
{
    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    if (d_state == e_STATE_STOPPED) {
        return;
    }

    for (bsl::size_t nameServerIndex = 0;
         nameServerIndex < d_serverList.size();
         ++nameServerIndex)
    {
        const bsl::shared_ptr<ntcdns::ClientNameServer>& server =
            d_serverList[nameServerIndex];

        server->linger();
    }

    d_serverList.clear();
    d_initialized = false;

    // MRM: d_datagramSocketFactory_sp.reset();
    // MRM: d_streamSocketFactory_sp.reset();

    // d_cache_sp.reset();

    if (d_state == e_STATE_STOPPING) {
        d_state = e_STATE_STOPPED;
        NTCDNS_CLIENT_LOG_STOPPED();
    }
}

ntsa::Error Client::getIpAddress(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const bsl::string&                     name,
    const ntca::GetIpAddressOptions&       options,
    const ntci::GetIpAddressCallback&      callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    if (d_serverList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    ntsa::DomainName domainName;
    if (!domainName.parse(name)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    SearchList searchList;

    if (domainName.isAbsolute()) {
        searchList.reserve(1);
        searchList.push_back(name);
    }
    else {
        searchList.reserve(d_config.search().size() + 1);

        if (domainName.dots() > 0) {
            searchList.push_back(name);
        }

        for (bsl::size_t searchIndex = 0;
             searchIndex < d_config.search().size();
             ++searchIndex)
        {
            const bsl::string& searchDomain = d_config.search()[searchIndex];

            bsl::string expandedName;
            expandedName.reserve(name.size() + 1 + searchDomain.size());
            expandedName.assign(name);
            expandedName.append(1, '.');
            expandedName.append(searchDomain);

            searchList.push_back(expandedName);
        }
    }

    bsl::shared_ptr<ntcdns::ClientGetIpAddressOperation> operation;
    operation.createInplace(d_allocator_p,
                            resolver,
                            name,
                            d_serverList,
                            searchList,
                            options,
                            callback,
                            d_cache_sp,
                            d_allocator_p);

    bsl::shared_ptr<ntcdns::ClientNameServer> server = d_serverList.front();

    error = server->initiate(operation);
    if (error) {
        while (true) {
            bsl::shared_ptr<ntcdns::ClientNameServer> nameServer =
                operation->tryNextServer();

            if (nameServer) {
                error = nameServer->initiate(operation);
                if (error) {
                    continue;
                }
                else {
                    break;
                }
            }
            else {
                return ntsa::Error(ntsa::Error::e_EOF);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Client::getDomainName(
    const bsl::shared_ptr<ntci::Resolver>& resolver,
    const ntsa::IpAddress&                 ipAddress,
    const ntca::GetDomainNameOptions&      options,
    const ntci::GetDomainNameCallback&     callback)
{
    LockGuard lock(&d_mutex);

    ntsa::Error error;

    if (!d_initialized) {
        error = this->initialize();
        if (error) {
            return error;
        }
    }

    if (d_serverList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::shared_ptr<ntcdns::ClientGetDomainNameOperation> operation;
    operation.createInplace(d_allocator_p,
                            resolver,
                            ipAddress,
                            d_serverList,
                            options,
                            callback,
                            d_cache_sp,
                            d_allocator_p);

    bsl::shared_ptr<ntcdns::ClientNameServer> server = d_serverList.front();

    error = server->initiate(operation);
    if (error) {
        while (true) {
            bsl::shared_ptr<ntcdns::ClientNameServer> nameServer =
                operation->tryNextServer();

            if (nameServer) {
                error = nameServer->initiate(operation);
                if (error) {
                    continue;
                }
                else {
                    break;
                }
            }
            else {
                return ntsa::Error(ntsa::Error::e_EOF);
            }
        }
    }

    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
