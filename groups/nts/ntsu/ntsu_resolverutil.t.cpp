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
#include <ntsu_resolverutil.h>

#include <bsl_algorithm.h>
#include <bsl_set.h>
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

namespace test {

const char       IANA_SERVICE_NAME[]   = "discard";
const ntsa::Port IANA_SERVICE_TCP_PORT = 9;
const ntsa::Port IANA_SERVICE_UDP_PORT = 9;

}  // close namespace test

NTSCFG_TEST_CASE(1)
{
    // Concern: 'getHostname'.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::string hostname(&ta);
        error = ntsu::ResolverUtil::getHostname(&hostname);
        NTSCFG_TEST_FALSE(error);

        NTSCFG_TEST_LOG_DEBUG << "Hostname: " << hostname
                              << NTSCFG_TEST_LOG_END;
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern: 'getHostnameFullyQualified'.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::string hostname(&ta);
        error = ntsu::ResolverUtil::getHostnameFullyQualified(&hostname);
        NTSCFG_TEST_FALSE(error);

        NTSCFG_TEST_LOG_DEBUG << "Hostname: " << hostname
                              << NTSCFG_TEST_LOG_END;
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // an unspecified transport.
    //
    // Plan: Ensure 'microsoft.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::set<ntsa::IpAddress> ipAddressSet(&ta);
        ipAddressSet.insert(ntsa::IpAddress("20.53.203.50"));
        ipAddressSet.insert(ntsa::IpAddress("20.84.181.62"));

        NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
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
    // Concern: Test resolution of domain names to IPv4 addresses.
    //
    // Plan: Ensure 'microsoft.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::set<ntsa::IpAddress> ipAddressSet(&ta);
        ipAddressSet.insert(ntsa::IpAddress("20.53.203.50"));
        ipAddressSet.insert(ntsa::IpAddress("20.84.181.62"));

        NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setIpAddressType(ntsa::IpAddressType::e_V4);

        error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
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

NTSCFG_TEST_CASE(5)
{
    // Concern: Test resolution of domain names to IPv6 addresses.
    //
    // Plan: Ensure 'microsoft.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::set<ntsa::IpAddress> ipAddressSet(&ta);
        // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));
        // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));

        // TODO: NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setIpAddressType(ntsa::IpAddressType::e_V6);

        error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                                 "microsoft.com",
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
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(6)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // a specific TCP/IPv4-based transport.
    //
    // Plan: Ensure 'microsoft.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::set<ntsa::IpAddress> ipAddressSet(&ta);
        ipAddressSet.insert(ntsa::IpAddress("20.53.203.50"));
        ipAddressSet.insert(ntsa::IpAddress("20.84.181.62"));

        NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
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

NTSCFG_TEST_CASE(7)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // a specific UDP/IPv4-based transport.
    //
    // Plan: Ensure 'microsoft.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::set<ntsa::IpAddress> ipAddressSet(&ta);
        ipAddressSet.insert(ntsa::IpAddress("20.53.203.50"));
        ipAddressSet.insert(ntsa::IpAddress("20.84.181.62"));

        NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

        error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
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

NTSCFG_TEST_CASE(8)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // a specific TCP/IPv6-based transport.
    //
    // Plan: Ensure 'microsoft.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::set<ntsa::IpAddress> ipAddressSet(&ta);
        // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));
        // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));

        // TODO: NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);

        error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                                 "microsoft.com",
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
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(9)
{
    // Concern: Test resolution of domain names to IP addresses for use by
    // a specific UDP/IPv6-based transport.
    //
    // Plan: Ensure 'microsoft.com' resolves to at least two of the known
    // IP addresses at which it has been assigned, as of 2020.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::set<ntsa::IpAddress> ipAddressSet(&ta);
        // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));
        // TODO: ipAddressSet.insert(ntsa::IpAddress("???"));

        // TODO: NTSCFG_TEST_EQ(ipAddressSet.size(), 2);

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);

        error = ntsu::ResolverUtil::getIpAddress(&ipAddressList,
                                                 "microsoft.com",
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
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(10)
{
    // Concern: Test resolution of IP addresses to domain names.
    // Plan: Resolve the well-known IP address of Google's public DNS server
    // to "dns.google".

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::string domainName(&ta);
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
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(11)
{
    // Concern: Test resolution of service names to port numbers for use by
    // an unspecified transport.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::Port> portList(&ta);
        ntsa::PortOptions       portOptions;

        error = ntsu::ResolverUtil::getPort(&portList,
                                            test::IANA_SERVICE_NAME,
                                            portOptions);

        if (error) {
            NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
        }
        else {
            for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
                 it != portList.end();
                 ++it)
            {
                NTSCFG_TEST_LOG_DEBUG << "Port: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }

        NTSCFG_TEST_FALSE(error);

        {
            bsl::size_t count = bsl::count(portList.begin(),
                                           portList.end(),
                                           test::IANA_SERVICE_TCP_PORT);
            NTSCFG_TEST_GE(count, 1);
        }

        {
            bsl::size_t count = bsl::count(portList.begin(),
                                           portList.end(),
                                           test::IANA_SERVICE_UDP_PORT);
            NTSCFG_TEST_GE(count, 1);
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(12)
{
    // Concern: Test resolution of service names to port numbers for use by
    // a specific TCP-based transport.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::Port> portList(&ta);
        ntsa::PortOptions       portOptions;

        portOptions.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

        error = ntsu::ResolverUtil::getPort(&portList,
                                            test::IANA_SERVICE_NAME,
                                            portOptions);

        if (error) {
            NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
        }
        else {
            for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
                 it != portList.end();
                 ++it)
            {
                NTSCFG_TEST_LOG_DEBUG << "Port: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }

        NTSCFG_TEST_FALSE(error);

        bsl::size_t count = bsl::count(portList.begin(),
                                       portList.end(),
                                       test::IANA_SERVICE_TCP_PORT);
        NTSCFG_TEST_GE(count, 1);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(13)
{
    // Concern: Test resolution of service names to port numbers for use by
    // a specific UDP-based transport.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::Port> portList(&ta);
        ntsa::PortOptions       portOptions;

        portOptions.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

        error = ntsu::ResolverUtil::getPort(&portList,
                                            test::IANA_SERVICE_NAME,
                                            portOptions);

        if (error) {
            NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
        }
        else {
            for (bsl::vector<ntsa::Port>::const_iterator it = portList.begin();
                 it != portList.end();
                 ++it)
            {
                NTSCFG_TEST_LOG_DEBUG << "Port: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }

        NTSCFG_TEST_FALSE(error);
        bsl::size_t count = bsl::count(portList.begin(),
                                       portList.end(),
                                       test::IANA_SERVICE_UDP_PORT);
        NTSCFG_TEST_GE(count, 1);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(14)
{
    // Concern: Test resolution of port numbers to service names.
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::string serviceName(&ta);
        error = ntsu::ResolverUtil::getServiceName(
            &serviceName,
            test::IANA_SERVICE_TCP_PORT,
            ntsa::Transport::e_TCP_IPV4_STREAM);

        if (error) {
            NTSCFG_TEST_LOG_ERROR << "Error: " << error << NTSCFG_TEST_LOG_END;
        }
        else {
            NTSCFG_TEST_LOG_DEBUG << "Service name: " << serviceName
                                  << NTSCFG_TEST_LOG_END;
        }

        NTSCFG_TEST_FALSE(error);
        NTSCFG_TEST_EQ(serviceName, test::IANA_SERVICE_NAME);
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(15)
{
    // Concern: Test getting the IP addresses assigned to the local machine.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
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
                NTSCFG_TEST_LOG_DEBUG << "Address: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(16)
{
    // Concern: Test getting the IPv4 addresses assigned to the local machine.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
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
                NTSCFG_TEST_LOG_DEBUG << "Address: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(17)
{
    // Concern: Test getting the IPv6 addresses assigned to the local machine.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setIpAddressType(ntsa::IpAddressType::e_V6);

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
                NTSCFG_TEST_LOG_DEBUG << "Address: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(18)
{
    // Concern: Test getting the IP addresses assigned to the local machine
    // for use by a TCP/IPv4 transport.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
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
                NTSCFG_TEST_LOG_DEBUG << "Address: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(19)
{
    // Concern: Test getting the IP addresses assigned to the local machine
    // for use by a UDP/IPv4 transport.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
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
                NTSCFG_TEST_LOG_DEBUG << "Address: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(20)
{
    // Concern: Test getting the IP addresses assigned to the local machine
    // for use by a TCP/IPv6 transport.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setTransport(ntsa::Transport::e_TCP_IPV6_STREAM);

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
                NTSCFG_TEST_LOG_DEBUG << "Address: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(21)
{
    // Concern: Test getting the IP addresses assigned to the local machine
    // for use by a UDP/IPv6 transport.

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        bsl::vector<ntsa::IpAddress> ipAddressList(&ta);
        ntsa::IpAddressOptions       ipAddressOptions;

        ipAddressOptions.setTransport(ntsa::Transport::e_UDP_IPV6_DATAGRAM);

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
                NTSCFG_TEST_LOG_DEBUG << "Address: " << *it
                                      << NTSCFG_TEST_LOG_END;
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);   // getHostname
    NTSCFG_TEST_REGISTER(2);   // getHostnameFullyQualified
    NTSCFG_TEST_REGISTER(3);   // getIpAddress
    NTSCFG_TEST_REGISTER(4);   // getIpAddress(V4)
    NTSCFG_TEST_REGISTER(5);   // getIpAddress(V6)
    NTSCFG_TEST_REGISTER(6);   // getIpAddress(TCP_V4)
    NTSCFG_TEST_REGISTER(7);   // getIpAddress(UDP_V4)
    NTSCFG_TEST_REGISTER(8);   // getIpAddress(TCP_V6)
    NTSCFG_TEST_REGISTER(9);   // getIpAddress(UDP_V6)
    NTSCFG_TEST_REGISTER(10);  // getDomainName
    NTSCFG_TEST_REGISTER(11);  // getPort
    NTSCFG_TEST_REGISTER(12);  // getPort(TCP)
    NTSCFG_TEST_REGISTER(13);  // getPort(UDP)
    NTSCFG_TEST_REGISTER(14);  // getServiceName
    NTSCFG_TEST_REGISTER(15);  // getLocalIpAddress
    NTSCFG_TEST_REGISTER(16);  // getLocalIpAddress(V4)
    NTSCFG_TEST_REGISTER(17);  // getLocalIpAddress(V6)
    NTSCFG_TEST_REGISTER(18);  // getLocalIpAddress(TCP_V4)
    NTSCFG_TEST_REGISTER(19);  // getLocalIpAddress(UDP_V4)
    NTSCFG_TEST_REGISTER(20);  // getLocalIpAddress(TCP_V6)
    NTSCFG_TEST_REGISTER(21);  // getLocalIpAddress(UDP_V6)
}
NTSCFG_TEST_DRIVER_END;
