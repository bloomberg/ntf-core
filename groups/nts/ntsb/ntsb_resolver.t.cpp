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

#include <ntsb_resolver.h>

#include <ntscfg_test.h>

#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

// Uncomment or set to 0 to disable tests for invalid domain names, which
// tend to take multiple seconds for each test case variation, since the
// calls block until the name servers time out.
#define NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID 0

// Uncomment or set to 0 to disable tests for invalid service names, which
// tend to take multiple seconds for each test case variation, since the
// calls block until the name servers time out.
#define NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID 0

namespace test {

bsl::string makeEndpointSpecifier(ntsa::Port port)
{
    bsl::stringstream ss;
    ss << port;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const bsl::string& host)
{
    bsl::stringstream ss;
    ss << host;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const bsl::string& host,
                                  const bsl::string& port)
{
    bsl::stringstream ss;
    ss << host << ':' << port;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const bsl::string& host, ntsa::Port port)
{
    bsl::stringstream ss;
    ss << host << ':' << port;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const ntsa::Ipv4Address& host)
{
    bsl::stringstream ss;
    ss << host;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const ntsa::Ipv4Address& host,
                                  const bsl::string&       port)
{
    bsl::stringstream ss;
    ss << host << ':' << port;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const ntsa::Ipv4Address& host,
                                  ntsa::Port               port)
{
    bsl::stringstream ss;
    ss << host << ':' << port;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const ntsa::Ipv6Address& host)
{
    bsl::stringstream ss;
    ss << host;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const ntsa::Ipv6Address& host,
                                  const bsl::string&       port)
{
    bsl::stringstream ss;
    ss << '[' << host << ']' << ':' << port;

    return ss.str();
}

bsl::string makeEndpointSpecifier(const ntsa::Ipv6Address& host,
                                  ntsa::Port               port)
{
    bsl::stringstream ss;
    ss << '[' << host << ']' << ':' << port;

    return ss.str();
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        ntsa::Port                   port)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(port);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const bsl::string&           host)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const bsl::string&           host,
                        const bsl::string&           port)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host, port);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const bsl::string&           host,
                        ntsa::Port                   port)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host, port);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const ntsa::Ipv4Address&     host)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const ntsa::Ipv4Address&     host,
                        const bsl::string&           port)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host, port);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const ntsa::Ipv4Address&     host,
                        ntsa::Port                   port)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host, port);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const ntsa::Ipv6Address&     host)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const ntsa::Ipv6Address&     host,
                        const bsl::string&           port)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host, port);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

ntsa::Error getEndpoint(ntsb::Resolver*              resolver,
                        ntsa::Endpoint*              endpoint,
                        const ntsa::EndpointOptions& options,
                        const ntsa::Ipv6Address&     host,
                        ntsa::Port                   port)
{
    ntsa::Error error;

    bsl::string text = test::makeEndpointSpecifier(host, port);

    NTSCFG_TEST_LOG_DEBUG << "--" << NTSCFG_TEST_LOG_END;
    NTSCFG_TEST_LOG_DEBUG << "Parsing: '" << text << "' using options "
                          << options << NTSCFG_TEST_LOG_END;

    error = resolver->getEndpoint(endpoint, text, options);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Endpoint = " << *endpoint
                              << NTSCFG_TEST_LOG_END;
    }

    return error;
}

}  // close namespace test

NTSCFG_TEST_CASE(1)
{
    // Concern: Test 'getHostname' from the system.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsb::Resolver resolver(&ta);

        bsl::string hostname;
        ntsa::Error error = resolver.getHostname(&hostname);
        NTSCFG_TEST_FALSE(error);

        NTSCFG_TEST_LOG_DEBUG << "Hostname: " << hostname
                              << NTSCFG_TEST_LOG_END;
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Test 'getHostnameFullyQualified' from the system.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsb::Resolver resolver(&ta);

        bsl::string hostname;
        ntsa::Error error = resolver.getHostnameFullyQualified(&hostname);
        NTSCFG_TEST_FALSE(error);

        NTSCFG_TEST_LOG_DEBUG << "Hostname: " << hostname
                              << NTSCFG_TEST_LOG_END;
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Test resolution of domain names to IP addresses from the
    // system.
    //
    // Plan: Ensure 'microsoft.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsb::Resolver resolver(&ta);

        bsl::set<ntsa::IpAddress> ipAddressSet;
        ipAddressSet.insert(ntsa::IpAddress("20.53.203.50"));
        ipAddressSet.insert(ntsa::IpAddress("20.84.181.62"));

        NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList;
        ntsa::IpAddressOptions       ipAddressOptions;

        error = resolver.getIpAddress(&ipAddressList,
                                      "microsoft.com",
                                      ipAddressOptions);
        NTSCFG_TEST_FALSE(error);

        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << it->text()
                                  << NTSCFG_TEST_LOG_END;

            ipAddressSet.erase(*it);
        }

        NTSCFG_TEST_TRUE(ipAddressSet.empty());
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(4)
{
    // Concern: Test resolution of IP addresses to domain names from the
    // system.
    //
    // Plan: Resolve the well-known IP address of Google's public DNS server
    // to "dns.google".

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsb::Resolver resolver(&ta);

        bsl::string domainName;
        error =
            resolver.getDomainName(&domainName, ntsa::IpAddress("8.8.8.8"));

        if (error) {
            NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
        }
        else {
            NTSCFG_TEST_LOG_DEBUG << "Domain name: " << domainName
                                  << NTSCFG_TEST_LOG_END;
        }

        NTSCFG_TEST_FALSE(error);
        NTSCFG_TEST_EQ(domainName, "dns.google");
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(5)
{
    // Concern: Test 'getHostname' from the cache.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsb::Resolver resolver(&ta);

        resolver.setHostname("test");

        bsl::string hostname;
        error = resolver.getHostname(&hostname);
        NTSCFG_TEST_FALSE(error);

        NTSCFG_TEST_EQ(hostname, bsl::string("test"));

        NTSCFG_TEST_LOG_DEBUG << "Hostname: " << hostname
                              << NTSCFG_TEST_LOG_END;
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(6)
{
    // Concern: Test 'getHostnameFullyQualified' from the cache.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsb::Resolver resolver(&ta);

        resolver.setHostnameFullyQualified("test.home");

        bsl::string hostname;
        error = resolver.getHostnameFullyQualified(&hostname);
        NTSCFG_TEST_FALSE(error);

        NTSCFG_TEST_EQ(hostname, bsl::string("test.home"));

        NTSCFG_TEST_LOG_DEBUG << "Hostname: " << hostname
                              << NTSCFG_TEST_LOG_END;
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(7)
{
    // Concern: Test resolution of domain names to IP addresses from the
    // overrides.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsb::Resolver resolver(&ta);

        bsl::vector<ntsa::IpAddress> ipAddressListOverride;
        ipAddressListOverride.push_back(ntsa::IpAddress("4.4.4.4"));
        ipAddressListOverride.push_back(ntsa::IpAddress("8.8.8.8"));

        resolver.setIpAddress("example.com", ipAddressListOverride);

        bsl::set<ntsa::IpAddress> ipAddressSet(ipAddressListOverride.begin(),
                                               ipAddressListOverride.end());

        NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList;
        ntsa::IpAddressOptions       ipAddressOptions;

        error = resolver.getIpAddress(&ipAddressList,
                                      "example.com",
                                      ipAddressOptions);
        NTSCFG_TEST_FALSE(error);

        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << it->text()
                                  << NTSCFG_TEST_LOG_END;

            bsl::size_t n = ipAddressSet.erase(*it);
            NTSCFG_TEST_EQ(n, 1);
        }

        NTSCFG_TEST_TRUE(ipAddressSet.empty());
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(8)
{
    // Concern: Test resolution of IP addresses to domain names from the
    // overrides.
    //
    // Plan: Resolve the well-known IP address of Google's public DNS server
    // to "dns.google".

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsb::Resolver resolver(&ta);

        bsl::vector<ntsa::IpAddress> ipAddressListOverride;
        ipAddressListOverride.push_back(ntsa::IpAddress("4.4.4.4"));
        ipAddressListOverride.push_back(ntsa::IpAddress("8.8.8.8"));

        resolver.setIpAddress("example.com", ipAddressListOverride);

        bsl::string domainName;
        error =
            resolver.getDomainName(&domainName, ntsa::IpAddress("8.8.8.8"));

        if (error) {
            NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
        }
        else {
            NTSCFG_TEST_LOG_DEBUG << "Domain name: " << domainName
                                  << NTSCFG_TEST_LOG_END;
        }

        NTSCFG_TEST_FALSE(error);
        NTSCFG_TEST_EQ(domainName, "example.com");
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(9)
{
    // Concern: Test resolution of endpoints.
    //
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        const bsl::string DOMAIN_NAME = "test.example.com";

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        const bsl::string DOMAIN_NAME_INVALID = "invalid.example.com";
#endif

        bsl::vector<ntsa::Ipv4Address> ipv4AddressList;
        ipv4AddressList.push_back(ntsa::Ipv4Address("192.168.1.101"));
        ipv4AddressList.push_back(ntsa::Ipv4Address("192.168.1.102"));
        ipv4AddressList.push_back(ntsa::Ipv4Address("192.168.1.103"));
        ipv4AddressList.push_back(ntsa::Ipv4Address("192.168.1.104"));
        ipv4AddressList.push_back(ntsa::Ipv4Address("192.168.1.105"));

        bsl::vector<ntsa::Ipv6Address> ipv6AddressList;
        ipv6AddressList.push_back(
            ntsa::Ipv6Address("2606:2800:220:1:248:1893:25c8:1946"));
        ipv6AddressList.push_back(
            ntsa::Ipv6Address("2001:41c0::645:a65e:60ff:feda:589d"));
        ipv6AddressList.push_back(ntsa::Ipv6Address("2001:0db8::1:0:0:1"));
        ipv6AddressList.push_back(ntsa::Ipv6Address("::"));
        ipv6AddressList.push_back(ntsa::Ipv6Address("::1"));

        bsl::vector<ntsa::IpAddress> ipAddressList;
        ipAddressList.insert(ipAddressList.end(),
                             ipv4AddressList.begin(),
                             ipv4AddressList.end());
        ipAddressList.insert(ipAddressList.end(),
                             ipv6AddressList.begin(),
                             ipv6AddressList.end());

        ntsa::Ipv4Address ipv4AddressFallback("10.10.1.20");
        ntsa::Ipv6Address ipv6AddressFallback(
            "2001:0db8:85a3:0000:0000:8a2e:0370:7334");

        const bsl::string SERVICE_NAME = "ntsp";

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        const bsl::string SERVICE_NAME_INVALID = "invalid-ntsp";
#endif

        bsl::vector<ntsa::Port> tcpPortList;
        tcpPortList.push_back(5801);
        tcpPortList.push_back(5802);
        tcpPortList.push_back(5803);

        bsl::vector<ntsa::Port> udpPortList;
        udpPortList.push_back(9801);
        udpPortList.push_back(9802);
        udpPortList.push_back(9803);

        bsl::vector<ntsa::Port> portList;
        portList.insert(portList.end(),
                        tcpPortList.begin(),
                        tcpPortList.end());
        portList.insert(portList.end(),
                        udpPortList.begin(),
                        udpPortList.end());

        ntsa::Port portFallback = 6484;

        ntsb::Resolver resolver(&ta);

        error = resolver.setIpAddress(DOMAIN_NAME, ipAddressList);
        NTSCFG_TEST_FALSE(error);

        error = resolver.setPort(SERVICE_NAME,
                                 tcpPortList,
                                 ntsa::Transport::e_TCP_IPV4_STREAM);
        NTSCFG_TEST_FALSE(error);

        error = resolver.setPort(SERVICE_NAME,
                                 udpPortList,
                                 ntsa::Transport::e_UDP_IPV4_DATAGRAM);
        NTSCFG_TEST_FALSE(error);

        //
        // Case: (empty)
        //

        // Test: (empty)

        // Test: (empty) [TCP/IPv4]

        // Test: (empty) [TCP/IPv6]

        // Test: (empty) [UDP/IPv4]

        // Test: (empty) [UDP/IPv6]

        // Test: (empty) with default IPv4 address

        // Test: (empty) with default IPv4 address [TCP/IPv4]

        // Test: (empty) with default IPv4 address [TCP/IPv6]

        // Test: (empty) with default IPv4 address [UDP/IPv4]

        // Test: (empty) with default IPv4 address [UDP/IPv6]

        // Test: (empty) with default IPv6 address

        // Test: (empty) with default IPv6 address [TCP/IPv4]

        // Test: (empty) with default IPv6 address [TCP/IPv6]

        // Test: (empty) with default IPv6 address [UDP/IPv4]

        // Test: (empty) with default IPv6 address [UDP/IPv6]

        // Test: (empty) with default port

        // Test: (empty) with default port [TCP/IPv4]

        // Test: (empty) with default port [TCP/IPv6]

        // Test: (empty) with default port [UDP/IPv4]

        // Test: (empty) with default port [UDP/IPv6]

        // Test: (empty) with default IPv4 address and port

        // Test: (empty) with default IPv4 address and port [TCP/IPv4]

        // Test: (empty) with default IPv4 address and port [TCP/IPv6]

        // Test: (empty) with default IPv4 address and port [UDP/IPv4]

        // Test: (empty) with default IPv4 address and port [UDP/IPv6]

        // Test: (empty) with default IPv6 address and port

        // Test: (empty) with default IPv6 address and port [TCP/IPv4]

        // Test: (empty) with default IPv6 address and port [TCP/IPv6]

        // Test: (empty) with default IPv6 address and port [UDP/IPv4]

        // Test: (empty) with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <port>
        //

        // Test: <port>

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error =
                test::getEndpoint(&resolver, &endpoint, options, portList[0]);
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_INVALID);
        }

        // Test: <port> [TCP/IPv4]

        // Test: <port> [TCP/IPv6]

        // Test: <port> [UDP/IPv4]

        // Test: <port> [UDP/IPv6]

        // Test: <port> with default IPv4 address

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            error =
                test::getEndpoint(&resolver, &endpoint, options, portList[0]);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressFallback);
            NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
        }

        // Test: <port> with default IPv4 address [TCP/IPv4]

        // Test: <port> with default IPv4 address [TCP/IPv6]

        // Test: <port> with default IPv4 address [UDP/IPv4]

        // Test: <port> with default IPv4 address [UDP/IPv6]

        // Test: <port> with default IPv6 address

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv6AddressFallback));

            error =
                test::getEndpoint(&resolver, &endpoint, options, portList[0]);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressFallback);
            NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
        }

        // Test: <port> with default IPv6 address [TCP/IPv4]

        // Test: <port> with default IPv6 address [TCP/IPv6]

        // Test: <port> with default IPv6 address [UDP/IPv4]

        // Test: <port> with default IPv6 address [UDP/IPv6]

        // Test: <port> with default port

        // Test: <port> with default port [TCP/IPv4]

        // Test: <port> with default port [TCP/IPv6]

        // Test: <port> with default port [UDP/IPv4]

        // Test: <port> with default port [UDP/IPv6]

        // Test: <port> with default IPv4 address and port

        // Test: <port> with default IPv4 address and port [TCP/IPv4]

        // Test: <port> with default IPv4 address and port [TCP/IPv6]

        // Test: <port> with default IPv4 address and port [UDP/IPv4]

        // Test: <port> with default IPv4 address and port [UDP/IPv6]

        // Test: <port> with default IPv6 address and port

        // Test: <port> with default IPv6 address and port [TCP/IPv4]

        // Test: <port> with default IPv6 address and port [TCP/IPv6]

        // Test: <port> with default IPv6 address and port [UDP/IPv4]

        // Test: <port> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <ipv4-address>
        //

        // Test: <ipv4-address>

        // Test: <ipv4-address> [TCP/IPv4]

        // Test: <ipv4-address> [TCP/IPv6]

        // Test: <ipv4-address> [UDP/IPv4]

        // Test: <ipv4-address> [UDP/IPv6]

        // Test: <ipv4-address> with default IPv4 address

        // Test: <ipv4-address> with default IPv4 address [TCP/IPv4]

        // Test: <ipv4-address> with default IPv4 address [TCP/IPv6]

        // Test: <ipv4-address> with default IPv4 address [UDP/IPv4]

        // Test: <ipv4-address> with default IPv4 address [UDP/IPv6]

        // Test: <ipv4-address> with default IPv6 address

        // Test: <ipv4-address> with default IPv6 address [TCP/IPv4]

        // Test: <ipv4-address> with default IPv6 address [TCP/IPv6]

        // Test: <ipv4-address> with default IPv6 address [UDP/IPv4]

        // Test: <ipv4-address> with default IPv6 address [UDP/IPv6]

        // Test: <ipv4-address> with default port

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv4AddressList[0]);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
            NTSCFG_TEST_EQ(endpoint.ip().port(), portFallback);
        }

        // Test: <ipv4-address> with default port [TCP/IPv4]

        // Test: <ipv4-address> with default port [TCP/IPv6]

        // Test: <ipv4-address> with default port [UDP/IPv4]

        // Test: <ipv4-address> with default port [UDP/IPv6]

        // Test: <ipv4-address> with default IPv4 address and port

        // Test: <ipv4-address> with default IPv4 address and port [TCP/IPv4]

        // Test: <ipv4-address> with default IPv4 address and port [TCP/IPv6]

        // Test: <ipv4-address> with default IPv4 address and port [UDP/IPv4]

        // Test: <ipv4-address> with default IPv4 address and port [UDP/IPv6]

        // Test: <ipv4-address> with default IPv6 address and port

        // Test: <ipv4-address> with default IPv6 address and port [TCP/IPv4]

        // Test: <ipv4-address> with default IPv6 address and port [TCP/IPv6]

        // Test: <ipv4-address> with default IPv6 address and port [UDP/IPv4]

        // Test: <ipv4-address> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <ipv4-address>:<port>
        //

        // Test: <ipv4-address>:<port>

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv4AddressList[0],
                                      portList[0]);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
            NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
        }

        // Test: <ipv4-address>:<port> with default IPv4 address

        // Test: <ipv4-address>:<port> with default IPv4 address [TCP/IPv4]

        // Test: <ipv4-address>:<port> with default IPv4 address [TCP/IPv6]

        // Test: <ipv4-address>:<port> with default IPv4 address [UDP/IPv4]

        // Test: <ipv4-address>:<port> with default IPv4 address [UDP/IPv6]

        // Test: <ipv4-address>:<port> with default IPv6 address

        // Test: <ipv4-address>:<port> with default IPv6 address [TCP/IPv4]

        // Test: <ipv4-address>:<port> with default IPv6 address [TCP/IPv6]

        // Test: <ipv4-address>:<port> with default IPv6 address [UDP/IPv4]

        // Test: <ipv4-address>:<port> with default IPv6 address [UDP/IPv6]

        // Test: <ipv4-address>:<port> with default port

        // Test: <ipv4-address>:<port> with default port [TCP/IPv4]

        // Test: <ipv4-address>:<port> with default port [TCP/IPv6]

        // Test: <ipv4-address>:<port> with default port [UDP/IPv4]

        // Test: <ipv4-address>:<port> with default port [UDP/IPv6]

        // Test: <ipv4-address>:<port> with default IPv4 address and port

        // Test: <ipv4-address>:<port> with default IPv4 address and port [TCP/IPv4]

        // Test: <ipv4-address>:<port> with default IPv4 address and port [TCP/IPv6]

        // Test: <ipv4-address>:<port> with default IPv4 address and port [UDP/IPv4]

        // Test: <ipv4-address>:<port> with default IPv4 address and port [UDP/IPv6]

        // Test: <ipv4-address>:<port> with default IPv6 address and port

        // Test: <ipv4-address>:<port> with default IPv6 address and port [TCP/IPv4]

        // Test: <ipv4-address>:<port> with default IPv6 address and port [TCP/IPv6]

        // Test: <ipv4-address>:<port> with default IPv6 address and port [UDP/IPv4]

        // Test: <ipv4-address>:<port> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <ipv4-address>:<service-name>
        //

        // Test: <ipv4-address>:<service-name>

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv4AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv4AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               portList[j % portList.size()]);
            }
        }

        // Test: <ipv4-address>:<service-name> [TCP/IPv4]

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv4AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), tcpPortList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * tcpPortList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv4AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               tcpPortList[j % tcpPortList.size()]);
            }
        }

        // Test: <ipv4-address>:<service-name> [TCP/IPv6]

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv4AddressList[0],
                                      SERVICE_NAME);
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_INVALID);
        }

        // Test: <ipv4-address>:<service-name> [UDP/IPv4]

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv4AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), udpPortList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * udpPortList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv4AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               udpPortList[j % udpPortList.size()]);
            }
        }

        // Test: <ipv4-address>:<service-name> [UDP/IPv6]

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv4AddressList[0],
                                      SERVICE_NAME);
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_INVALID);
        }

        // Test: <ipv4-address>:<service-name> with default IPv4 address

        // Test: <ipv4-address>:<service-name> with default IPv4 address [TCP/IPv4]

        // Test: <ipv4-address>:<service-name> with default IPv4 address [TCP/IPv6]

        // Test: <ipv4-address>:<service-name> with default IPv4 address [UDP/IPv4]

        // Test: <ipv4-address>:<service-name> with default IPv4 address [UDP/IPv6]

        // Test: <ipv4-address>:<service-name> with default IPv6 address

        // Test: <ipv4-address>:<service-name> with default IPv6 address [TCP/IPv4]

        // Test: <ipv4-address>:<service-name> with default IPv6 address [TCP/IPv6]

        // Test: <ipv4-address>:<service-name> with default IPv6 address [UDP/IPv4]

        // Test: <ipv4-address>:<service-name> with default IPv6 address [UDP/IPv6]

        // Test: <ipv4-address>:<service-name> with default port

        // Test: <ipv4-address>:<service-name> with default port [TCP/IPv4]

        // Test: <ipv4-address>:<service-name> with default port [TCP/IPv6]

        // Test: <ipv4-address>:<service-name> with default port [UDP/IPv4]

        // Test: <ipv4-address>:<service-name> with default port [UDP/IPv6]

        // Test: <ipv4-address>:<service-name> with default IPv4 address and port

        // Test: <ipv4-address>:<service-name> with default IPv4 address and port [TCP/IPv4]

        // Test: <ipv4-address>:<service-name> with default IPv4 address and port [TCP/IPv6]

        // Test: <ipv4-address>:<service-name> with default IPv4 address and port [UDP/IPv4]

        // Test: <ipv4-address>:<service-name> with default IPv4 address and port [UDP/IPv6]

        // Test: <ipv4-address>:<service-name> with default IPv6 address and port

        // Test: <ipv4-address>:<service-name> with default IPv6 address and port [TCP/IPv4]

        // Test: <ipv4-address>:<service-name> with default IPv6 address and port [TCP/IPv6]

        // Test: <ipv4-address>:<service-name> with default IPv6 address and port [UDP/IPv4]

        // Test: <ipv4-address>:<service-name> with default IPv6 address and port [UDP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid>

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv4AddressList[0],
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <ipv4-address>:<service-name-invalid> [TCP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> [TCP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> [UDP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> [UDP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv4AddressList[0],
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address [TCP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address [TCP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address [UDP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address [UDP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv6AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv4AddressList[0],
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address [TCP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address [TCP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address [UDP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address [UDP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default port

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv4AddressList[0],
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <ipv4-address>:<service-name-invalid> with default port [TCP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default port [TCP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default port [UDP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default port [UDP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address and port

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address and port [TCP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address and port [TCP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address and port [UDP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv4 address and port [UDP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address and port

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address and port [TCP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address and port [TCP/IPv6]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address and port [UDP/IPv4]

        // Test: <ipv4-address>:<service-name-invalid> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <ipv6-address>
        //

        // Test: <ipv6-address>

        // Test: <ipv6-address> [TCP/IPv4]

        // Test: <ipv6-address> [TCP/IPv6]

        // Test: <ipv6-address> [UDP/IPv4]

        // Test: <ipv6-address> [UDP/IPv6]

        // Test: <ipv6-address> with default IPv4 address

        // Test: <ipv6-address> with default IPv4 address [TCP/IPv4]

        // Test: <ipv6-address> with default IPv4 address [TCP/IPv6]

        // Test: <ipv6-address> with default IPv4 address [UDP/IPv4]

        // Test: <ipv6-address> with default IPv4 address [UDP/IPv6]

        // Test: <ipv6-address> with default IPv6 address

        // Test: <ipv6-address> with default IPv6 address [TCP/IPv4]

        // Test: <ipv6-address> with default IPv6 address [TCP/IPv6]

        // Test: <ipv6-address> with default IPv6 address [UDP/IPv4]

        // Test: <ipv6-address> with default IPv6 address [UDP/IPv6]

        // Test: <ipv6-address> with default port

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv6AddressList[0]);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
            NTSCFG_TEST_EQ(endpoint.ip().port(), portFallback);
        }

        // Test: <ipv6-address> with default port [TCP/IPv4]

        // Test: <ipv6-address> with default port [TCP/IPv6]

        // Test: <ipv6-address> with default port [UDP/IPv4]

        // Test: <ipv6-address> with default port [UDP/IPv6]

        // Test: <ipv6-address> with default IPv4 address and port

        // Test: <ipv6-address> with default IPv4 address and port [TCP/IPv4]

        // Test: <ipv6-address> with default IPv4 address and port [TCP/IPv6]

        // Test: <ipv6-address> with default IPv4 address and port [UDP/IPv4]

        // Test: <ipv6-address> with default IPv4 address and port [UDP/IPv6]

        // Test: <ipv6-address> with default IPv6 address and port

        // Test: <ipv6-address> with default IPv6 address and port [TCP/IPv4]

        // Test: <ipv6-address> with default IPv6 address and port [TCP/IPv6]

        // Test: <ipv6-address> with default IPv6 address and port [UDP/IPv4]

        // Test: <ipv6-address> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <ipv6-address>:<port>
        //

        // Test: <ipv6-address>:<port>

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv6AddressList[0],
                                      portList[0]);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
            NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
        }

        // Test: <ipv6-address>:<port> with default IPv4 address

        // Test: <ipv6-address>:<port> with default IPv4 address [TCP/IPv4]

        // Test: <ipv6-address>:<port> with default IPv4 address [TCP/IPv6]

        // Test: <ipv6-address>:<port> with default IPv4 address [UDP/IPv4]

        // Test: <ipv6-address>:<port> with default IPv4 address [UDP/IPv6]

        // Test: <ipv6-address>:<port> with default IPv6 address

        // Test: <ipv6-address>:<port> with default IPv6 address [TCP/IPv4]

        // Test: <ipv6-address>:<port> with default IPv6 address [TCP/IPv6]

        // Test: <ipv6-address>:<port> with default IPv6 address [UDP/IPv4]

        // Test: <ipv6-address>:<port> with default IPv6 address [UDP/IPv6]

        // Test: <ipv6-address>:<port> with default port

        // Test: <ipv6-address>:<port> with default port [TCP/IPv4]

        // Test: <ipv6-address>:<port> with default port [TCP/IPv6]

        // Test: <ipv6-address>:<port> with default port [UDP/IPv4]

        // Test: <ipv6-address>:<port> with default port [UDP/IPv6]

        // Test: <ipv6-address>:<port> with default IPv4 address and port

        // Test: <ipv6-address>:<port> with default IPv4 address and port [TCP/IPv4]

        // Test: <ipv6-address>:<port> with default IPv4 address and port [TCP/IPv6]

        // Test: <ipv6-address>:<port> with default IPv4 address and port [UDP/IPv4]

        // Test: <ipv6-address>:<port> with default IPv4 address and port [UDP/IPv6]

        // Test: <ipv6-address>:<port> with default IPv6 address and port

        // Test: <ipv6-address>:<port> with default IPv6 address and port [TCP/IPv4]

        // Test: <ipv6-address>:<port> with default IPv6 address and port [TCP/IPv6]

        // Test: <ipv6-address>:<port> with default IPv6 address and port [UDP/IPv4]

        // Test: <ipv6-address>:<port> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <ipv6-address>:<service-name>
        //

        // Test: <ipv6-address>:<service-name>

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv6AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv6AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               portList[j % portList.size()]);
            }
        }

        // Test: <ipv6-address>:<service-name> [TCP/IPv4]

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv6AddressList[0],
                                      SERVICE_NAME);
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_INVALID);
        }

        // Test: <ipv6-address>:<service-name> [TCP/IPv6]

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv6AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), tcpPortList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * tcpPortList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);
                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv6AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               tcpPortList[j % tcpPortList.size()]);
            }
        }

        // Test: <ipv6-address>:<service-name> [UDP/IPv4]

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv6AddressList[0],
                                      SERVICE_NAME);
            NTSCFG_TEST_ERROR(error, ntsa::Error::e_INVALID);
        }

        // Test: <ipv6-address>:<service-name> [UDP/IPv6]

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv6AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), udpPortList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * tcpPortList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          ipv6AddressList[0],
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               udpPortList[j % udpPortList.size()]);
            }
        }

        // Test: <ipv6-address>:<service-name> with default IPv4 address

        // Test: <ipv6-address>:<service-name> with default IPv4 address [TCP/IPv4]

        // Test: <ipv6-address>:<service-name> with default IPv4 address [TCP/IPv6]

        // Test: <ipv6-address>:<service-name> with default IPv4 address [UDP/IPv4]

        // Test: <ipv6-address>:<service-name> with default IPv4 address [UDP/IPv6]

        // Test: <ipv6-address>:<service-name> with default IPv6 address

        // Test: <ipv6-address>:<service-name> with default IPv6 address [TCP/IPv4]

        // Test: <ipv6-address>:<service-name> with default IPv6 address [TCP/IPv6]

        // Test: <ipv6-address>:<service-name> with default IPv6 address [UDP/IPv4]

        // Test: <ipv6-address>:<service-name> with default IPv6 address [UDP/IPv6]

        // Test: <ipv6-address>:<service-name> with default port

        // Test: <ipv6-address>:<service-name> with default port [TCP/IPv4]

        // Test: <ipv6-address>:<service-name> with default port [TCP/IPv6]

        // Test: <ipv6-address>:<service-name> with default port [UDP/IPv4]

        // Test: <ipv6-address>:<service-name> with default port [UDP/IPv6]

        // Test: <ipv6-address>:<service-name> with default IPv4 address and port

        // Test: <ipv6-address>:<service-name> with default IPv4 address and port [TCP/IPv4]

        // Test: <ipv6-address>:<service-name> with default IPv4 address and port [TCP/IPv6]

        // Test: <ipv6-address>:<service-name> with default IPv4 address and port [UDP/IPv4]

        // Test: <ipv6-address>:<service-name> with default IPv4 address and port [UDP/IPv6]

        // Test: <ipv6-address>:<service-name> with default IPv6 address and port

        // Test: <ipv6-address>:<service-name> with default IPv6 address and port [TCP/IPv4]

        // Test: <ipv6-address>:<service-name> with default IPv6 address and port [TCP/IPv6]

        // Test: <ipv6-address>:<service-name> with default IPv6 address and port [UDP/IPv4]

        // Test: <ipv6-address>:<service-name> with default IPv6 address and port [UDP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid>

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv6AddressList[0],
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <ipv6-address>:<service-name-invalid> [TCP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> [TCP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> [UDP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> [UDP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv6AddressList[0],
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address [TCP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address [TCP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address [UDP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address [UDP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv6AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv6AddressList[0],
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address [TCP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address [TCP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address [UDP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address [UDP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default port

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      ipv6AddressList[0],
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <ipv6-address>:<service-name-invalid> with default port [TCP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default port [TCP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default port [UDP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default port [UDP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address and port

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address and port [TCP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address and port [TCP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address and port [UDP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv4 address and port [UDP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address and port

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address and port [TCP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address and port [TCP/IPv6]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address and port [UDP/IPv4]

        // Test: <ipv6-address>:<service-name-invalid> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <domain-name>
        //

        // Test: <domain-name>

        // Test: <domain-name> [TCP/IPv4]

        // Test: <domain-name> [TCP/IPv6]

        // Test: <domain-name> [UDP/IPv4]

        // Test: <domain-name> [UDP/IPv6]

        // Test: <domain-name> with default IPv4 address

        // Test: <domain-name> with default IPv4 address [TCP/IPv4]

        // Test: <domain-name> with default IPv4 address [TCP/IPv6]

        // Test: <domain-name> with default IPv4 address [UDP/IPv4]

        // Test: <domain-name> with default IPv4 address [UDP/IPv6]

        // Test: <domain-name> with default IPv6 address

        // Test: <domain-name> with default IPv6 address [TCP/IPv4]

        // Test: <domain-name> with default IPv6 address [TCP/IPv6]

        // Test: <domain-name> with default IPv6 address [UDP/IPv4]

        // Test: <domain-name> with default IPv6 address [UDP/IPv6]

        // Test: <domain-name> with default port

        // Test: <domain-name> with default port [TCP/IPv4]

        // Test: <domain-name> with default port [TCP/IPv6]

        // Test: <domain-name> with default port [UDP/IPv4]

        // Test: <domain-name> with default port [UDP/IPv6]

        // Test: <domain-name> with default IPv4 address and port

        // Test: <domain-name> with default IPv4 address and port [TCP/IPv4]

        // Test: <domain-name> with default IPv4 address and port [TCP/IPv6]

        // Test: <domain-name> with default IPv4 address and port [UDP/IPv4]

        // Test: <domain-name> with default IPv4 address and port [UDP/IPv6]

        // Test: <domain-name> with default IPv6 address and port

        // Test: <domain-name> with default IPv6 address and port [TCP/IPv4]

        // Test: <domain-name> with default IPv6 address and port [TCP/IPv6]

        // Test: <domain-name> with default IPv6 address and port [UDP/IPv4]

        // Test: <domain-name> with default IPv6 address and port [UDP/IPv6]

        // Test: <domain-name-invalid>

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid> [TCP/IPv4]

        // Test: <domain-name-invalid> [TCP/IPv6]

        // Test: <domain-name-invalid> [UDP/IPv4]

        // Test: <domain-name-invalid> [UDP/IPv6]

        // Test: <domain-name-invalid> with default IPv4 address

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid> with default IPv4 address [TCP/IPv4]

        // Test: <domain-name-invalid> with default IPv4 address [TCP/IPv6]

        // Test: <domain-name-invalid> with default IPv4 address [UDP/IPv4]

        // Test: <domain-name-invalid> with default IPv4 address [UDP/IPv6]

        // Test: <domain-name-invalid> with default IPv6 address

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv6AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid> with default IPv6 address [TCP/IPv4]

        // Test: <domain-name-invalid> with default IPv6 address [TCP/IPv6]

        // Test: <domain-name-invalid> with default IPv6 address [UDP/IPv4]

        // Test: <domain-name-invalid> with default IPv6 address [UDP/IPv6]

        // Test: <domain-name-invalid> with default port

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid> with default port [TCP/IPv4]

        // Test: <domain-name-invalid> with default port [TCP/IPv6]

        // Test: <domain-name-invalid> with default port [UDP/IPv4]

        // Test: <domain-name-invalid> with default port [UDP/IPv6]

        // Test: <domain-name-invalid> with default IPv4 address and port

        // Test: <domain-name-invalid> with default IPv4 address and port [TCP/IPv4]

        // Test: <domain-name-invalid> with default IPv4 address and port [TCP/IPv6]

        // Test: <domain-name-invalid> with default IPv4 address and port [UDP/IPv4]

        // Test: <domain-name-invalid> with default IPv4 address and port [UDP/IPv6]

        // Test: <domain-name-invalid> with default IPv6 address and port

        // Test: <domain-name-invalid> with default IPv6 address and port [TCP/IPv4]

        // Test: <domain-name-invalid> with default IPv6 address and port [TCP/IPv6]

        // Test: <domain-name-invalid> with default IPv6 address and port [UDP/IPv4]

        // Test: <domain-name-invalid> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <domain-name>:<port>
        //

        // Test: <domain-name>:<port>

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          portList[0]);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host(), ipAddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setIpAddressSelector(i);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          portList[0]);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host(),
                               ipAddressList[i % ipAddressList.size()]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
            }
        }

        // Test: <domain-name>:<port> [TCP/IPv4]

        // Test: <domain-name>:<port> [TCP/IPv6]

        // Test: <domain-name>:<port> [UDP/IPv4]

        // Test: <domain-name>:<port> [UDP/IPv6]

        // Test: <domain-name>:<port> with default IPv4 address

        // Test: <domain-name>:<port> with default IPv4 address [TCP/IPv4]

        // Test: <domain-name>:<port> with default IPv4 address [TCP/IPv6]

        // Test: <domain-name>:<port> with default IPv4 address [UDP/IPv4]

        // Test: <domain-name>:<port> with default IPv4 address [UDP/IPv6]

        // Test: <domain-name>:<port> with default IPv6 address

        // Test: <domain-name>:<port> with default IPv6 address [TCP/IPv4]

        // Test: <domain-name>:<port> with default IPv6 address [TCP/IPv6]

        // Test: <domain-name>:<port> with default IPv6 address [UDP/IPv4]

        // Test: <domain-name>:<port> with default IPv6 address [UDP/IPv6]

        // Test: <domain-name>:<port> with default port

        // Test: <domain-name>:<port> with default port [TCP/IPv4]

        // Test: <domain-name>:<port> with default port [TCP/IPv6]

        // Test: <domain-name>:<port> with default port [UDP/IPv4]

        // Test: <domain-name>:<port> with default port [UDP/IPv6]

        // Test: <domain-name>:<port> with default IPv4 address and port

        // Test: <domain-name>:<port> with default IPv4 address and port [TCP/IPv4]

        // Test: <domain-name>:<port> with default IPv4 address and port [TCP/IPv6]

        // Test: <domain-name>:<port> with default IPv4 address and port [UDP/IPv4]

        // Test: <domain-name>:<port> with default IPv4 address and port [UDP/IPv6]

        // Test: <domain-name>:<port> with default IPv6 address and port

        // Test: <domain-name>:<port> with default IPv6 address and port [TCP/IPv4]

        // Test: <domain-name>:<port> with default IPv6 address and port [TCP/IPv6]

        // Test: <domain-name>:<port> with default IPv6 address and port [UDP/IPv4]

        // Test: <domain-name>:<port> with default IPv6 address and port [UDP/IPv6]

        // Test: <domain-name-invalid>:<port>

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      portList[0]);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<port> [TCP/IPv4]

        // Test: <domain-name-invalid>:<port> [TCP/IPv6]

        // Test: <domain-name-invalid>:<port> [UDP/IPv4]

        // Test: <domain-name-invalid>:<port> [UDP/IPv6]

        // Test: <domain-name-invalid>:<port> with default IPv4 address

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      portList[0]);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<port> with default IPv4 address [TCP/IPv4]

        // Test: <domain-name-invalid>:<port> with default IPv4 address [TCP/IPv6]

        // Test: <domain-name-invalid>:<port> with default IPv4 address [UDP/IPv4]

        // Test: <domain-name-invalid>:<port> with default IPv4 address [UDP/IPv6]

        // Test: <domain-name-invalid>:<port> with default IPv6 address

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv6AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      portList[0]);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<port> with default IPv6 address [TCP/IPv4]

        // Test: <domain-name-invalid>:<port> with default IPv6 address [TCP/IPv6]

        // Test: <domain-name-invalid>:<port> with default IPv6 address [UDP/IPv4]

        // Test: <domain-name-invalid>:<port> with default IPv6 address [UDP/IPv6]

        // Test: <domain-name-invalid>:<port> with default port

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      portList[0]);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<port> with default port [TCP/IPv4]

        // Test: <domain-name-invalid>:<port> with default port [TCP/IPv6]

        // Test: <domain-name-invalid>:<port> with default port [UDP/IPv4]

        // Test: <domain-name-invalid>:<port> with default port [UDP/IPv6]

        // Test: <domain-name-invalid>:<port> with default IPv4 address and port

        // Test: <domain-name-invalid>:<port> with default IPv4 address and port [TCP/IPv4]

        // Test: <domain-name-invalid>:<port> with default IPv4 address and port [TCP/IPv6]

        // Test: <domain-name-invalid>:<port> with default IPv4 address and port [UDP/IPv4]

        // Test: <domain-name-invalid>:<port> with default IPv4 address and port [UDP/IPv6]

        // Test: <domain-name-invalid>:<port> with default IPv6 address and port

        // Test: <domain-name-invalid>:<port> with default IPv6 address and port [TCP/IPv4]

        // Test: <domain-name-invalid>:<port> with default IPv6 address and port [TCP/IPv6]

        // Test: <domain-name-invalid>:<port> with default IPv6 address and port [UDP/IPv4]

        // Test: <domain-name-invalid>:<port> with default IPv6 address and port [UDP/IPv6]

        //
        // Case: <domain-name>:<service-name>
        //

        // Test: <domain-name>:<service-name>

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host(), ipAddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setIpAddressSelector(i);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host(),
                               ipAddressList[i % ipAddressList.size()]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), portList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host(), ipAddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               portList[j % portList.size()]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                    ntsa::Endpoint        endpoint;
                    ntsa::EndpointOptions options;

                    options.setIpAddressSelector(i);
                    options.setPortSelector(j);

                    error = test::getEndpoint(&resolver,
                                              &endpoint,
                                              options,
                                              DOMAIN_NAME,
                                              SERVICE_NAME);
                    NTSCFG_TEST_OK(error);

                    NTSCFG_TEST_EQ(endpoint.ip().host(),
                                   ipAddressList[i % ipAddressList.size()]);
                    NTSCFG_TEST_EQ(endpoint.ip().port(),
                                   portList[j % portList.size()]);
                }
            }
        }

        // Test: <domain-name>:<service-name> [TCP/IPv4]

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), tcpPortList[0]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
                options.setIpAddressSelector(i);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(),
                               ipv4AddressList[i % ipv4AddressList.size()]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), tcpPortList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               tcpPortList[j % tcpPortList.size()]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                    ntsa::Endpoint        endpoint;
                    ntsa::EndpointOptions options;

                    options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);
                    options.setIpAddressSelector(i);
                    options.setPortSelector(j);

                    error = test::getEndpoint(&resolver,
                                              &endpoint,
                                              options,
                                              DOMAIN_NAME,
                                              SERVICE_NAME);
                    NTSCFG_TEST_OK(error);

                    NTSCFG_TEST_EQ(
                        endpoint.ip().host().v4(),
                        ipv4AddressList[i % ipv4AddressList.size()]);
                    NTSCFG_TEST_EQ(endpoint.ip().port(),
                                   tcpPortList[j % tcpPortList.size()]);
                }
            }
        }

        // Test: <domain-name>:<service-name> [TCP/IPv6]

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), tcpPortList[0]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);
                options.setIpAddressSelector(i);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(),
                               ipv6AddressList[i % ipv6AddressList.size()]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), tcpPortList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);
                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               tcpPortList[j % tcpPortList.size()]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                    ntsa::Endpoint        endpoint;
                    ntsa::EndpointOptions options;

                    options.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);
                    options.setIpAddressSelector(i);
                    options.setPortSelector(j);

                    error = test::getEndpoint(&resolver,
                                              &endpoint,
                                              options,
                                              DOMAIN_NAME,
                                              SERVICE_NAME);
                    NTSCFG_TEST_OK(error);

                    NTSCFG_TEST_EQ(
                        endpoint.ip().host().v6(),
                        ipv6AddressList[i % ipv6AddressList.size()]);
                    NTSCFG_TEST_EQ(endpoint.ip().port(),
                                   tcpPortList[j % tcpPortList.size()]);
                }
            }
        }

        // Test: <domain-name>:<service-name> [UDP/IPv4]

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), udpPortList[0]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                options.setIpAddressSelector(i);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(),
                               ipv4AddressList[i % ipv4AddressList.size()]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), udpPortList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               udpPortList[j % udpPortList.size()]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                    ntsa::Endpoint        endpoint;
                    ntsa::EndpointOptions options;

                    options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);
                    options.setIpAddressSelector(i);
                    options.setPortSelector(j);

                    error = test::getEndpoint(&resolver,
                                              &endpoint,
                                              options,
                                              DOMAIN_NAME,
                                              SERVICE_NAME);
                    NTSCFG_TEST_OK(error);

                    NTSCFG_TEST_EQ(
                        endpoint.ip().host().v4(),
                        ipv4AddressList[i % ipv4AddressList.size()]);
                    NTSCFG_TEST_EQ(endpoint.ip().port(),
                                   udpPortList[j % udpPortList.size()]);
                }
            }
        }

        // Test: <domain-name>:<service-name> [UDP/IPv6]

        {
            {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), udpPortList[0]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                options.setIpAddressSelector(i);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(),
                               ipv6AddressList[i % ipv6AddressList.size()]);
                NTSCFG_TEST_EQ(endpoint.ip().port(), udpPortList[0]);
            }

            for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                ntsa::Endpoint        endpoint;
                ntsa::EndpointOptions options;

                options.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                options.setPortSelector(j);

                error = test::getEndpoint(&resolver,
                                          &endpoint,
                                          options,
                                          DOMAIN_NAME,
                                          SERVICE_NAME);
                NTSCFG_TEST_OK(error);

                NTSCFG_TEST_EQ(endpoint.ip().host().v6(), ipv6AddressList[0]);
                NTSCFG_TEST_EQ(endpoint.ip().port(),
                               udpPortList[j % udpPortList.size()]);
            }

            for (bsl::size_t i = 0; i < 2 * ipAddressList.size(); ++i) {
                for (bsl::size_t j = 0; j < 2 * portList.size(); ++j) {
                    ntsa::Endpoint        endpoint;
                    ntsa::EndpointOptions options;

                    options.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);
                    options.setIpAddressSelector(i);
                    options.setPortSelector(j);

                    error = test::getEndpoint(&resolver,
                                              &endpoint,
                                              options,
                                              DOMAIN_NAME,
                                              SERVICE_NAME);
                    NTSCFG_TEST_OK(error);

                    NTSCFG_TEST_EQ(
                        endpoint.ip().host().v6(),
                        ipv6AddressList[i % ipv6AddressList.size()]);
                    NTSCFG_TEST_EQ(endpoint.ip().port(),
                                   udpPortList[j % udpPortList.size()]);
                }
            }
        }

        // Test: <domain-name>:<service-name> with default IPv4 address

        // Test: <domain-name>:<service-name> with default IPv4 address [TCP/IPv4]

        // Test: <domain-name>:<service-name> with default IPv4 address [TCP/IPv6]

        // Test: <domain-name>:<service-name> with default IPv4 address [UDP/IPv4]

        // Test: <domain-name>:<service-name> with default IPv4 address [UDP/IPv6]

        // Test: <domain-name>:<service-name> with default IPv6 address

        // Test: <domain-name>:<service-name> with default IPv6 address [TCP/IPv4]

        // Test: <domain-name>:<service-name> with default IPv6 address [TCP/IPv6]

        // Test: <domain-name>:<service-name> with default IPv6 address [UDP/IPv4]

        // Test: <domain-name>:<service-name> with default IPv6 address [UDP/IPv6]

        // Test: <domain-name>:<service-name> with default port

        // Test: <domain-name>:<service-name> with default port [TCP/IPv4]

        // Test: <domain-name>:<service-name> with default port [TCP/IPv6]

        // Test: <domain-name>:<service-name> with default port [UDP/IPv4]

        // Test: <domain-name>:<service-name> with default port [UDP/IPv6]

        // Test: <domain-name>:<service-name> with default IPv4 address and port

        // Test: <domain-name>:<service-name> with default IPv4 address and port [TCP/IPv4]

        // Test: <domain-name>:<service-name> with default IPv4 address and port [TCP/IPv6]

        // Test: <domain-name>:<service-name> with default IPv4 address and port [UDP/IPv4]

        // Test: <domain-name>:<service-name> with default IPv4 address and port [UDP/IPv6]

        // Test: <domain-name>:<service-name> with default IPv6 address and port

        // Test: <domain-name>:<service-name> with default IPv6 address and port [TCP/IPv4]

        // Test: <domain-name>:<service-name> with default IPv6 address and port [TCP/IPv6]

        // Test: <domain-name>:<service-name> with default IPv6 address and port [UDP/IPv4]

        // Test: <domain-name>:<service-name> with default IPv6 address and port [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name>

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      SERVICE_NAME);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<service-name> [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name> [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name> [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name> [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      SERVICE_NAME);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv6AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      SERVICE_NAME);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default port

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      SERVICE_NAME);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<service-name> with default port [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default port [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default port [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default port [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address and port

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address and port [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address and port [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address and port [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default IPv4 address and port [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address and port

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address and port [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address and port [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address and port [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name> with default IPv6 address and port [UDP/IPv6]

        // Test: <domain-name>:<service-name-invalid>

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME,
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name>:<service-name-invalid> [TCP/IPv4]

        // Test: <domain-name>:<service-name-invalid> [TCP/IPv6]

        // Test: <domain-name>:<service-name-invalid> [UDP/IPv4]

        // Test: <domain-name>:<service-name-invalid> [UDP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME,
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address [TCP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address [TCP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address [UDP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address [UDP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv6AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME,
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address [TCP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address [TCP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address [UDP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address [UDP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default port

#if NTSB_RESOLVER_TEST_SERVICE_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME,
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name>:<service-name-invalid> with default port [TCP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default port [TCP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default port [UDP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default port [UDP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address and port

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address and port [TCP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address and port [TCP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address and port [UDP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default IPv4 address and port [UDP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address and port

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address and port [TCP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address and port [TCP/IPv6]

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address and port [UDP/IPv4]

        // Test: <domain-name>:<service-name-invalid> with default IPv6 address and port [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid>

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<service-name-invalid> [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv6AddressFallback));

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default port

#if NTSB_RESOLVER_TEST_DOMAIN_NAME_INVALID
        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            error = test::getEndpoint(&resolver,
                                      &endpoint,
                                      options,
                                      DOMAIN_NAME_INVALID,
                                      SERVICE_NAME_INVALID);
            NTSCFG_TEST_TRUE(error);
        }
#endif

        // Test: <domain-name-invalid>:<service-name-invalid> with default port [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default port [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default port [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default port [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address and port

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address and port [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address and port [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address and port [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv4 address and port [UDP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address and port

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address and port [TCP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address and port [TCP/IPv6]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address and port [UDP/IPv4]

        // Test: <domain-name-invalid>:<service-name-invalid> with default IPv6 address and port [UDP/IPv6]
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(10)
{
    // Concern: Test resolution of endpoints with empty inputs but both an
    // IP address fallback and port fallback specified.
    //
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsb::Resolver resolver(&ta);

        const ntsa::Ipv4Address ipv4AddressFallback("192.168.0.100");
        const ntsa::Port        portFallback = 12345;

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            bsl::string emptyString;

            error =
                test::getEndpoint(&resolver, &endpoint, options, emptyString);
            NTSCFG_TEST_TRUE(error);
        }

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));

            bsl::string emptyString;

            error =
                test::getEndpoint(&resolver, &endpoint, options, emptyString);
            NTSCFG_TEST_TRUE(error);
        }

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setPortFallback(portFallback);

            bsl::string emptyString;

            error =
                test::getEndpoint(&resolver, &endpoint, options, emptyString);
            NTSCFG_TEST_TRUE(error);
        }

        {
            ntsa::Endpoint        endpoint;
            ntsa::EndpointOptions options;

            options.setIpAddressFallback(ntsa::IpAddress(ipv4AddressFallback));
            options.setPortFallback(portFallback);

            bsl::string emptyString;

            error =
                test::getEndpoint(&resolver, &endpoint, options, emptyString);
            NTSCFG_TEST_OK(error);

            NTSCFG_TEST_EQ(endpoint.ip().host().v4(), ipv4AddressFallback);
            NTSCFG_TEST_EQ(endpoint.ip().port(), portFallback);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(11)
{
    // Concern: Test resolution of a service name expressed as a port number.
    //
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        ntsb::Resolver resolver(&ta);

        {
            ntsa::PortOptions       portOptions;
            bsl::vector<ntsa::Port> portList;

            error = resolver.getPort(
                &portList, bslstl::StringRef("7000", 4), portOptions);
            NTSCFG_TEST_EQ(error, ntsa::Error());

            NTSCFG_TEST_EQ(portList.size(), 1);

            NTSCFG_TEST_LOG_DEBUG << "Port = " << portList[0]
                                  << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(portList[0], 7000);
        }

        {
            ntsa::PortOptions       portOptions;
            bsl::vector<ntsa::Port> portList;

            error = resolver.getPort(
                &portList, bslstl::StringRef("70000", 5), portOptions);
            NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
        }

    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
    NTSCFG_TEST_REGISTER(5);
    NTSCFG_TEST_REGISTER(6);
    NTSCFG_TEST_REGISTER(7);
    NTSCFG_TEST_REGISTER(8);

    NTSCFG_TEST_REGISTER(9);

    NTSCFG_TEST_REGISTER(10);

    NTSCFG_TEST_REGISTER(11);
}
NTSCFG_TEST_DRIVER_END;
