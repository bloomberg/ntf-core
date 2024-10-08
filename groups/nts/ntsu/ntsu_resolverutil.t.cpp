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
BSLS_IDENT_RCSID(ntsu_resolverutil_t_cpp, "$Id$ $CSID$")

#include <ntsu_resolverutil.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsu {

// Provide tests for 'ntsu::ResolverUtil'.
class ResolverUtilTest
{
  public:
    // Verify concern: getHostname.
    static void verifyGetHostname();

    // Verify concern: getHostnameFullyQualified.
    static void verifyGetHostnameFullyQualified();

    // Verify concern: getIpAddress.
    static void verifyGetIpAddress();

    // Verify concern: getIpAddress(V4).
    static void verifyGetIpAddressV4();

    // Verify concern: getIpAddress(V6).
    static void verifyGetIpAddressV6();

    // Verify concern: getIpAddress(TCP_V4).
    static void verifyGetIpAddressTcpV4();

    // Verify concern: getIpAddress(UDP_V4).
    static void verifyGetIpAddressUdpV4();

    // Verify concern: getIpAddress(TCP_V6).
    static void verifyGetIpAddressTcpV6();

    // Verify concern: getIpAddress(UDP_V6).
    static void verifyGetIpAddressUdpV6();

    // Verify concern: getDomainName.
    static void verifyGetDomainName();

    // Verify concern: getPort.
    static void verifyGetPort();

    // Verify concern: getPort(TCP).
    static void verifyGetPortTcp();

    // Verify concern: getPort(UDP).
    static void verifyGetPortUdp();

    // Verify concern: getServiceName.
    static void verifyGetServiceName();

    // Verify concern: getLocalIpAddress.
    static void verifyGetLocalIpAddress();

    // Verify concern: getLocalIpAddress(V4).
    static void verifyGetLocalIpAddressV4();

    // Verify concern: getLocalIpAddress(V6).
    static void verifyGetLocalIpAddressV6();

    // Verify concern: getLocalIpAddress(TCP_V4).
    static void verifyGetLocalIpAddressTcpV4();

    // Verify concern: getLocalIpAddress(UDP_V4).
    static void verifyGetLocalIpAddressUdpV4();

    // Verify concern: getLocalIpAddress(TCP_V6).
    static void verifyGetLocalIpAddressTcpV6();

    // Verify concern: getLocalIpAddress(UDP_V6).
    static void verifyGetLocalIpAddressUdpV6();

  private:
    // The expected well-known service name.
    static const char k_IANA_SERVICE_NAME[8];

    // The expected well-known service TCP port.
    static const ntsa::Port k_IANA_SERVICE_TCP_PORT;

    // The expected well-known service UDP port.
    static const ntsa::Port k_IANA_SERVICE_UDP_PORT;
};

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetHostname)
{
    ntsa::Error error;

    bsl::string hostname(NTSCFG_TEST_ALLOCATOR);
    error = ntsu::ResolverUtil::getHostname(&hostname);
    NTSCFG_TEST_FALSE(error);

    NTSCFG_TEST_LOG_DEBUG << "Hostname: " << hostname << NTSCFG_TEST_LOG_END;
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetHostnameFullyQualified)
{
    ntsa::Error error;

    bsl::string hostname(NTSCFG_TEST_ALLOCATOR);
    error = ntsu::ResolverUtil::getHostnameFullyQualified(&hostname);
    NTSCFG_TEST_FALSE(error);

    NTSCFG_TEST_LOG_DEBUG << "Hostname: " << hostname << NTSCFG_TEST_LOG_END;
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetIpAddress)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // an unspecified transport.
    //
    // Plan: Ensure 'dns.google.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntsa::Error error;

    bsl::set<ntsa::IpAddress> ipAddressSet(NTSCFG_TEST_ALLOCATOR);
    ipAddressSet.insert(ntsa::IpAddress("8.8.8.8"));
    ipAddressSet.insert(ntsa::IpAddress("8.8.4.4"));

    NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                             "dns.google.com",
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

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetIpAddressV4)
{
    // Concern: Test resolution of domain names to IPv4 addresses.
    //
    // Plan: Ensure 'dns.google.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntsa::Error error;

    bsl::set<ntsa::IpAddress> ipAddressSet(NTSCFG_TEST_ALLOCATOR);
    ipAddressSet.insert(ntsa::IpAddress("8.8.8.8"));
    ipAddressSet.insert(ntsa::IpAddress("8.8.4.4"));

    NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setIpAddressType(ntsa::IpAddressType::e_V4);

    error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                             "dns.google.com",
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

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetIpAddressV6)
{
    // Concern: Test resolution of domain names to IPv6 addresses.
    //
    // Plan: Ensure 'dns.google.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntsa::Error error;

    bsl::set<ntsa::IpAddress> ipAddressSet(NTSCFG_TEST_ALLOCATOR);
    // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));
    // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));

    // TODO: NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setIpAddressType(ntsa::IpAddressType::e_V6);

    error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                             "dns.google.com",
                                             ipAddressOptions);
    // TODO: NTSCFG_TEST_FALSE(error);

    for (bsl::vector<ntsa::IpAddress>::const_iterator it =
             ipAddressList.begin();
         it != ipAddressList.end();
         ++it)
    {
        NTSCFG_TEST_LOG_DEBUG << "Address: " << it->text()
                              << NTSCFG_TEST_LOG_END;

        // TODO: bsl::size_t n = ipAddressSet.erase(*it);
        // TODO: NTSCFG_TEST_EQ(n, 1);
    }

    NTSCFG_TEST_TRUE(ipAddressSet.empty());
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetIpAddressTcpV4)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // a specific TCP/IPv4-based transport.
    //
    // Plan: Ensure 'dns.google.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntsa::Error error;

    bsl::set<ntsa::IpAddress> ipAddressSet(NTSCFG_TEST_ALLOCATOR);
    ipAddressSet.insert(ntsa::IpAddress("8.8.8.8"));
    ipAddressSet.insert(ntsa::IpAddress("8.8.4.4"));

    NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                             "dns.google.com",
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

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetIpAddressUdpV4)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // a specific UDP/IPv4-based transport.
    //
    // Plan: Ensure 'dns.google.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntsa::Error error;

    bsl::set<ntsa::IpAddress> ipAddressSet(NTSCFG_TEST_ALLOCATOR);
    ipAddressSet.insert(ntsa::IpAddress("8.8.8.8"));
    ipAddressSet.insert(ntsa::IpAddress("8.8.4.4"));

    NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                             "dns.google.com",
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

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetIpAddressTcpV6)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // a specific TCP/IPv6-based transport.
    //
    // Plan: Ensure 'dns.google.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntsa::Error error;

    bsl::set<ntsa::IpAddress> ipAddressSet(NTSCFG_TEST_ALLOCATOR);
    // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));
    // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));

    // TODO: NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);

    error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                             "dns.google.com",
                                             ipAddressOptions);
    // TODO: NTSCFG_TEST_FALSE(error);

    for (bsl::vector<ntsa::IpAddress>::const_iterator it =
             ipAddressList.begin();
         it != ipAddressList.end();
         ++it)
    {
        NTSCFG_TEST_LOG_DEBUG << "Address: " << it->text()
                              << NTSCFG_TEST_LOG_END;

        // TODO: bsl::size_t n = ipAddressSet.erase(*it);
        // TODO: NTSCFG_TEST_EQ(n, 1);
    }

    NTSCFG_TEST_TRUE(ipAddressSet.empty());
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetIpAddressUdpV6)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // a specific UDP/IPv6-based transport.
    //
    // Plan: Ensure 'dns.google.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntsa::Error error;

    bsl::set<ntsa::IpAddress> ipAddressSet(NTSCFG_TEST_ALLOCATOR);
    // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));
    // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));

    // TODO: NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);

    error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                             "dns.google.com",
                                             ipAddressOptions);
    // TODO: NTSCFG_TEST_FALSE(error);

    for (bsl::vector<ntsa::IpAddress>::const_iterator it =
             ipAddressList.begin();
         it != ipAddressList.end();
         ++it)
    {
        NTSCFG_TEST_LOG_DEBUG << "Address: " << it->text()
                              << NTSCFG_TEST_LOG_END;

        // TODO: bsl::size_t n = ipAddressSet.erase(*it);
        // TODO: NTSCFG_TEST_EQ(n, 1);
    }

    NTSCFG_TEST_TRUE(ipAddressSet.empty());
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetDomainName)
{
    // Concern: Test resolution of IP addresses to domain names.
    // Plan: Resolve the well-known IP address of Google's public DNS server
    // to "dns.google".

    ntsa::Error error;

    bsl::string domainName(NTSCFG_TEST_ALLOCATOR);
    error = ntsu::ResolverUtil::getDomainName(&domainName,
                                              ntsa::IpAddress("8.8.8.8"));

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Domain name: " << domainName
                              << NTSCFG_TEST_LOG_END;
    }

    NTSCFG_TEST_FALSE(error);
    NTSCFG_TEST_EQ(domainName, "dns.google");
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetPort)
{
    // Concern: Test resolution of service names to port numbers for use by
    // an unspecified transport.

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    ntsa::Error error;

    bsl::vector<ntsa::Port> portList(NTSCFG_TEST_ALLOCATOR);
    ntsa::PortOptions       portOptions;

    error = ntsu::ResolverUtil::getPort(&portList,
                                        k_IANA_SERVICE_NAME,
                                        portOptions);

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
             it != portList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Port: " << *it << NTSCFG_TEST_LOG_END;
        }
    }

    NTSCFG_TEST_FALSE(error);

    {
        bsl::size_t count = bsl::count(portList.begin(),
                                       portList.end(),
                                       k_IANA_SERVICE_TCP_PORT);
        NTSCFG_TEST_GE(count, 1);
    }

    {
        bsl::size_t count = bsl::count(portList.begin(),
                                       portList.end(),
                                       k_IANA_SERVICE_UDP_PORT);
        NTSCFG_TEST_GE(count, 1);
    }
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetPortTcp)
{
    // Concern: Test resolution of service names to port numbers for use by
    // a specific TCP-based transport.

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    ntsa::Error error;

    bsl::vector<ntsa::Port> portList(NTSCFG_TEST_ALLOCATOR);
    ntsa::PortOptions       portOptions;

    portOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    error = ntsu::ResolverUtil::getPort(&portList,
                                        k_IANA_SERVICE_NAME,
                                        portOptions);

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
             it != portList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Port: " << *it << NTSCFG_TEST_LOG_END;
        }
    }

    NTSCFG_TEST_FALSE(error);

    bsl::size_t count =
        bsl::count(portList.begin(), portList.end(), k_IANA_SERVICE_TCP_PORT);
    NTSCFG_TEST_GE(count, 1);
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetPortUdp)
{
    // Concern: Test resolution of service names to port numbers for use by
    // a specific UDP-based transport.

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    ntsa::Error error;

    bsl::vector<ntsa::Port> portList(NTSCFG_TEST_ALLOCATOR);
    ntsa::PortOptions       portOptions;

    portOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    error = ntsu::ResolverUtil::getPort(&portList,
                                        k_IANA_SERVICE_NAME,
                                        portOptions);

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
             it != portList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Port: " << *it << NTSCFG_TEST_LOG_END;
        }
    }

    NTSCFG_TEST_FALSE(error);
    bsl::size_t count =
        bsl::count(portList.begin(), portList.end(), k_IANA_SERVICE_UDP_PORT);
    NTSCFG_TEST_GE(count, 1);
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetServiceName)
{
    // Concern: Test resolution of port numbers to service names.
    // Plan:

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    ntsa::Error error;

    bsl::string serviceName(NTSCFG_TEST_ALLOCATOR);
    error =
        ntsu::ResolverUtil::getServiceName(&serviceName,
                                           k_IANA_SERVICE_TCP_PORT,
                                           ntsa::Transport::e_TCP_IPV4_STREAM);

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        NTSCFG_TEST_LOG_DEBUG << "Service name: " << serviceName
                              << NTSCFG_TEST_LOG_END;
    }

    NTSCFG_TEST_FALSE(error);
    NTSCFG_TEST_EQ(serviceName, k_IANA_SERVICE_NAME);
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetLocalIpAddress)
{
    // Concern: Test getting the IP addresses assigned to the local machine.

    ntsa::Error error;

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    error = ntsu::ResolverUtil::getLocalIpAddress(&ipAddressList,
                                                  ipAddressOptions);

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << *it << NTSCFG_TEST_LOG_END;
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetLocalIpAddressV4)
{
    // Concern: Test getting the IPv4 addresses assigned to the local machine.

    ntsa::Error error;

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setIpAddressType(ntsa::IpAddressType::e_V4);

    error = ntsu::ResolverUtil::getLocalIpAddress(&ipAddressList,
                                                  ipAddressOptions);

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << *it << NTSCFG_TEST_LOG_END;
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetLocalIpAddressV6)
{
    // Concern: Test getting the IPv6 addresses assigned to the local machine.

    ntsa::Error error;

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setIpAddressType(ntsa::IpAddressType::e_V6);

    error = ntsu::ResolverUtil::getLocalIpAddress(&ipAddressList,
                                                  ipAddressOptions);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << *it << NTSCFG_TEST_LOG_END;
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetLocalIpAddressTcpV4)
{
    // Concern: Test getting the IP addresses assigned to the local machine
    // for use by a TCP/IPv4 transport.

    ntsa::Error error;

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

    error = ntsu::ResolverUtil::getLocalIpAddress(&ipAddressList,
                                                  ipAddressOptions);

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << *it << NTSCFG_TEST_LOG_END;
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetLocalIpAddressUdpV4)
{
    // Concern: Test getting the IP addresses assigned to the local machine
    // for use by a UDP/IPv4 transport.

    ntsa::Error error;

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

    error = ntsu::ResolverUtil::getLocalIpAddress(&ipAddressList,
                                                  ipAddressOptions);

    if (error) {
        NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << *it << NTSCFG_TEST_LOG_END;
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetLocalIpAddressTcpV6)
{
    // Concern: Test getting the IP addresses assigned to the local machine
    // for use by a TCP/IPv6 transport.

    ntsa::Error error;

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);

    error = ntsu::ResolverUtil::getLocalIpAddress(&ipAddressList,
                                                  ipAddressOptions);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << *it << NTSCFG_TEST_LOG_END;
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsu::ResolverUtilTest::verifyGetLocalIpAddressUdpV6)
{
    // Concern: Test getting the IP addresses assigned to the local machine
    // for use by a UDP/IPv6 transport.

    ntsa::Error error;

    bsl::vector<ntsa::IpAddress> ipAddressList(NTSCFG_TEST_ALLOCATOR);
    ntsa::IpAddressOptions       ipAddressOptions;

    ipAddressOptions.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);

    error = ntsu::ResolverUtil::getLocalIpAddress(&ipAddressList,
                                                  ipAddressOptions);

    if (error) {
        NTSCFG_TEST_LOG_DEBUG << "Error: " << error << NTSCFG_TEST_LOG_END;
    }
    else {
        for (bsl::vector<ntsa::IpAddress>::const_iterator it =
                 ipAddressList.begin();
             it != ipAddressList.end();
             ++it)
        {
            NTSCFG_TEST_LOG_DEBUG << "Address: " << *it << NTSCFG_TEST_LOG_END;
        }
    }
}

const char       ResolverUtilTest::k_IANA_SERVICE_NAME[8]  = "discard";
const ntsa::Port ResolverUtilTest::k_IANA_SERVICE_TCP_PORT = 9;
const ntsa::Port ResolverUtilTest::k_IANA_SERVICE_UDP_PORT = 9;

}  // close namespace ntsu
}  // close namespace BloombergLP
