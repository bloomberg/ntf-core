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

#include <ntcdns_database.h>

#include <ntccfg_test.h>
#include <ntcdns_compat.h>
#include <ntcdns_utility.h>
#include <ntci_log.h>
#include <ntsa_host.h>

#include <bdlb_chartype.h>
#include <bdlma_bufferedsequentialallocator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_alignedbuffer.h>
#include <bsls_assert.h>
#include <bsls_stopwatch.h>
#include <bsl_array.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>

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

/// Provide a host database for use by this test driver. This class is thread
/// safe.
class HostDatabase
{
    /// Define a type alias for a vector of IP addresses.
    typedef bsl::vector<ntsa::IpAddress> IpAddressVector;

    /// Define a type alias for a map of domain names to an
    /// associated vector of IP addresses.
    typedef bsl::unordered_map<bsl::string, IpAddressVector>
        IpAddressByDomainName;

    /// Define a type alias for a map of IP addresses to
    /// domain names.
    typedef bsl::unordered_map<ntsa::IpAddress, bsl::string>
        DomainNameByIpAddress;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex         d_mutex;
    IpAddressByDomainName d_ipAddressByDomainName;
    DomainNameByIpAddress d_domainNameByIpAddress;
    bslma::Allocator*     d_allocator_p;

  private:
    HostDatabase(const HostDatabase&) BSLS_KEYWORD_DELETED;
    HostDatabase& operator=(const HostDatabase&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new host database. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit HostDatabase(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~HostDatabase();

    /// Clear the database.
    void clear();

    /// Load the associations defined in the specified 'configuration' into
    /// the database. Return the error.
    ntsa::Error addHostEntryList(
        const ntcdns::HostDatabaseConfig& configuration);

    /// Load the specified 'entry' into the database. Return the error.
    ntsa::Error addHostEntry(const ntcdns::HostEntry& entry);

    /// Load the specified 'address' associated with the specified
    /// 'canonicalHostname' into the database. Return the error.
    ntsa::Error addHostEntry(const bsl::string& address,
                             const bsl::string& canonicalHostname);

    /// Load the specified 'address' associated with the specified
    /// 'canonicalHostname' and 'aliases' into the database. Return the error.
    ntsa::Error addHostEntry(const bsl::string&              address,
                             const bsl::string&              canonicalHostname,
                             const bsl::vector<bsl::string>& aliases);

    /// Load into the specified 'result' the IP address list assigned to the
    /// specified 'domainName' according to the specified 'options' and
    /// load into the specified 'context' the context of resolution. Return
    /// the error.
    ntsa::Error getIpAddress(ntca::GetIpAddressContext*       context,
                             bsl::vector<ntsa::IpAddress>*    result,
                             const bslstl::StringRef&         domainName,
                             const ntca::GetIpAddressOptions& options) const;

    /// Load into the specified 'result' the domain name to which the
    /// specified 'ipAddress' is assigned according to the specified
    /// 'options' and load into the specified 'context' the context of
    /// resolution. Return the error.
    ntsa::Error getDomainName(ntca::GetDomainNameContext*       context,
                              bsl::string*                      result,
                              const ntsa::IpAddress&            ipAddress,
                              const ntca::GetDomainNameOptions& options) const;
};

/// Provide a port database for use by this test driver. This class is thread
/// safe.
class PortDatabase
{
    /// Define a type alias for a vector of port numbers.
    typedef bsl::vector<ntsa::Port> PortVector;

    /// Define a type alias for a map of service names to an
    /// associated vector of port numbers.
    typedef bsl::unordered_map<bsl::string, PortVector> PortByServiceName;

    /// Define a type alias for a map of port numbers to
    /// service names.
    typedef bsl::unordered_map<ntsa::Port, bsl::string> ServiceNameByPort;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex     d_mutex;
    PortByServiceName d_tcpPortByServiceName;
    ServiceNameByPort d_tcpServiceNameByPort;
    PortByServiceName d_udpPortByServiceName;
    ServiceNameByPort d_udpServiceNameByPort;
    bslma::Allocator* d_allocator_p;

  private:
    PortDatabase(const PortDatabase&) BSLS_KEYWORD_DELETED;
    PortDatabase& operator=(const PortDatabase&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new port database. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit PortDatabase(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~PortDatabase();

    /// Clear the database.
    void clear();

    /// Load the associations defined in the specified 'configuration' into
    /// the database. Return the error.
    ntsa::Error addPortEntryList(
        const ntcdns::PortDatabaseConfig& configuration);

    /// Load the specified 'entry' into the database. Return the error.
    ntsa::Error addPortEntry(const ntcdns::PortEntry& entry);

    /// Load the specified 'service' associated with the specified 'port'
    /// and 'protocol' into the database. Return the error.
    ntsa::Error addPortEntry(const bsl::string& service,
                             ntsa::Port         port,
                             const bsl::string& protocol);

    /// Load into the specified 'result' the port list assigned to the
    /// specified 'serviceName' according to the specified 'options' and
    /// load into the specified 'context' the context of resolution. Return
    /// the error.
    ntsa::Error getPort(ntca::GetPortContext*       context,
                        bsl::vector<ntsa::Port>*    result,
                        const bslstl::StringRef&    serviceName,
                        const ntca::GetPortOptions& options) const;

    /// Load into the specified 'result' the service name to which the
    /// specified 'port' is assigned according to the specified 'options'
    /// and load into the specified 'context' the context of resolution.
    /// Return the error.
    ntsa::Error getServiceName(
        ntca::GetServiceNameContext*       context,
        bsl::string*                       result,
        const ntsa::Port&                  port,
        const ntca::GetServiceNameOptions& options) const;
};

HostDatabase::HostDatabase(bslma::Allocator* basicAllocator)
: d_mutex()
, d_ipAddressByDomainName(basicAllocator)
, d_domainNameByIpAddress(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

HostDatabase::~HostDatabase()
{
}

void HostDatabase::clear()
{
    LockGuard guard(&d_mutex);

    d_ipAddressByDomainName.clear();
    d_domainNameByIpAddress.clear();
}

ntsa::Error HostDatabase::addHostEntryList(
    const ntcdns::HostDatabaseConfig& configuration)
{
    ntsa::Error error;

    for (bsl::vector<ntcdns::HostEntry>::const_iterator it =
             configuration.entry().begin();
         it != configuration.entry().end();
         ++it)
    {
        const ntcdns::HostEntry& entry = *it;
        error                          = this->addHostEntry(entry);
        if (error) {
            continue;
        }
    }

    return ntsa::Error();
}

ntsa::Error HostDatabase::addHostEntry(const ntcdns::HostEntry& entry)
{
    NTCI_LOG_CONTEXT();

    LockGuard guard(&d_mutex);

    ntsa::Error error;

    if (entry.address().empty()) {
        NTCI_LOG_STREAM_WARN << "Failed to add host entry " << entry
                             << ": the address is empty"
                             << NTCI_LOG_STREAM_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (entry.canonicalHostname().empty()) {
        NTCI_LOG_STREAM_WARN << "Failed to add host entry " << entry
                             << ": the canonical hostname is empty"
                             << NTCI_LOG_STREAM_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::IpAddress ipAddress;
    if (!ipAddress.parse(entry.address())) {
        NTCI_LOG_STREAM_WARN << "Failed to add host entry " << entry
                             << ": the address '" << entry.address()
                             << "' is invalid" << NTCI_LOG_STREAM_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Silently skip entries with canonical hostnames like
    // "bnjp2-pl4sp29-portEthernet22/1". On some /etc/hosts files on some
    // Bloomberg machines, there are many of these entries.

    if (entry.canonicalHostname().find('/') != bsl::string::npos) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        ntsa::DomainName canonicalDomainName;
        if (!canonicalDomainName.parse(entry.canonicalHostname())) {
            NTCI_LOG_STREAM_WARN << "Failed to add host entry " << entry
                                 << ": the canonical hostname '"
                                 << entry.canonicalHostname() << "' is invalid"
                                 << NTCI_LOG_STREAM_END;
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bsl::vector<ntsa::IpAddress>& ipAddressList =
            d_ipAddressByDomainName[entry.canonicalHostname()];

        if (bsl::find(ipAddressList.begin(), ipAddressList.end(), ipAddress) ==
            ipAddressList.end())
        {
            ipAddressList.push_back(ipAddress);
        }

        bsl::string& domainName = d_domainNameByIpAddress[ipAddress];
        if (domainName.empty()) {
            domainName = entry.canonicalHostname();
        }
    }

    for (bsl::vector<bsl::string>::const_iterator jt = entry.aliases().begin();
         jt != entry.aliases().end();
         ++jt)
    {
        const bsl::string& alias = *jt;

        ntsa::DomainName aliasDomainName;
        if (!aliasDomainName.parse(alias)) {
            NTCI_LOG_STREAM_WARN << "Failed to add host entry " << entry
                                 << ": the alias '" << alias << "' is invalid"
                                 << NTCI_LOG_STREAM_END;
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bsl::vector<ntsa::IpAddress>& ipAddressList =
            d_ipAddressByDomainName[alias];

        if (bsl::find(ipAddressList.begin(), ipAddressList.end(), ipAddress) ==
            ipAddressList.end())
        {
            ipAddressList.push_back(ipAddress);
        }

        bsl::string& domainName = d_domainNameByIpAddress[ipAddress];
        if (domainName.empty()) {
            domainName = alias;
        }
    }

    return ntsa::Error();
}

ntsa::Error HostDatabase::addHostEntry(const bsl::string& address,
                                       const bsl::string& canonicalHostname)
{
    bsl::vector<bsl::string> aliases;
    return this->addHostEntry(address, canonicalHostname, aliases);
}

ntsa::Error HostDatabase::addHostEntry(const bsl::string& address,
                                       const bsl::string& canonicalHostname,
                                       const bsl::vector<bsl::string>& aliases)
{
    ntcdns::HostEntry entry;
    entry.address()           = address;
    entry.canonicalHostname() = canonicalHostname;
    entry.aliases()           = aliases;

    return this->addHostEntry(entry);
}

ntsa::Error HostDatabase::getIpAddress(
    ntca::GetIpAddressContext*       context,
    bsl::vector<ntsa::IpAddress>*    result,
    const bslstl::StringRef&         domainName,
    const ntca::GetIpAddressOptions& options) const
{
    ntsa::Error                  error;
    bsl::vector<ntsa::IpAddress> ipAddressList;

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType;
    error = ntcdns::Compat::convert(&ipAddressType, options);
    if (error) {
        return error;
    }

    {
        LockGuard lock(&d_mutex);

        IpAddressByDomainName::const_iterator it =
            d_ipAddressByDomainName.find(domainName);

        if (it == d_ipAddressByDomainName.end()) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        if (ipAddressType.isNull()) {
            ipAddressList = it->second;
        }
        else {
            const IpAddressVector& target = it->second;

            for (IpAddressVector::const_iterator jt = target.begin();
                 jt != target.end();
                 ++jt)
            {
                const ntsa::IpAddress& ipAddress = *jt;
                if (ipAddress.type() == ipAddressType.value()) {
                    ipAddressList.push_back(ipAddress);
                }
            }
        }
    }

    if (ipAddressList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    context->setDomainName(domainName);

    if (options.ipAddressSelector().isNull()) {
        *result = ipAddressList;
    }
    else {
        result->push_back(ipAddressList[options.ipAddressSelector().value() %
                                        ipAddressList.size()]);
    }

    return ntsa::Error();
}

ntsa::Error HostDatabase::getDomainName(
    ntca::GetDomainNameContext*       context,
    bsl::string*                      result,
    const ntsa::IpAddress&            ipAddress,
    const ntca::GetDomainNameOptions& options) const
{
    LockGuard guard(&d_mutex);

    DomainNameByIpAddress::const_iterator it =
        d_domainNameByIpAddress.find(ipAddress);

    if (it == d_domainNameByIpAddress.end()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (!it->second.empty()) {
        *result = it->second;
    }
    else {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    context->setIpAddress(ipAddress);

    return ntsa::Error();
}

PortDatabase::PortDatabase(bslma::Allocator* basicAllocator)
: d_mutex()
, d_tcpPortByServiceName(basicAllocator)
, d_tcpServiceNameByPort(basicAllocator)
, d_udpPortByServiceName(basicAllocator)
, d_udpServiceNameByPort(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PortDatabase::~PortDatabase()
{
}

void PortDatabase::clear()
{
    LockGuard guard(&d_mutex);

    d_tcpPortByServiceName.clear();
    d_tcpServiceNameByPort.clear();
    d_udpPortByServiceName.clear();
    d_udpServiceNameByPort.clear();
}

ntsa::Error PortDatabase::addPortEntryList(
    const ntcdns::PortDatabaseConfig& configuration)
{
    ntsa::Error error;

    for (bsl::vector<ntcdns::PortEntry>::const_iterator it =
             configuration.entry().begin();
         it != configuration.entry().end();
         ++it)
    {
        const ntcdns::PortEntry& entry = *it;
        error                          = this->addPortEntry(entry);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error PortDatabase::addPortEntry(const ntcdns::PortEntry& entry)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    LockGuard guard(&d_mutex);

    if (entry.service().empty()) {
        NTCI_LOG_STREAM_WARN << "Failed to add port entry " << entry
                             << ": the service is empty"
                             << NTCI_LOG_STREAM_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (entry.protocol().empty()) {
        NTCI_LOG_STREAM_WARN << "Failed to add port entry " << entry
                             << ": the protocol is empty"
                             << NTCI_LOG_STREAM_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (entry.port() == 0) {
        NTCI_LOG_STREAM_WARN << "Failed to add port entry " << entry
                             << ": the port is zero" << NTCI_LOG_STREAM_END;
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (entry.protocol() == "tcp") {
        PortVector& portList = d_tcpPortByServiceName[entry.service()];

        if (bsl::find(portList.begin(), portList.end(), entry.port()) ==
            portList.end())
        {
            portList.push_back(entry.port());
        }

        bsl::string& serviceName = d_tcpServiceNameByPort[entry.port()];
        if (serviceName.empty()) {
            serviceName = entry.service();
        }
    }
    else if (entry.protocol() == "udp") {
        PortVector& portList = d_udpPortByServiceName[entry.service()];

        if (bsl::find(portList.begin(), portList.end(), entry.port()) ==
            portList.end())
        {
            portList.push_back(entry.port());
        }

        bsl::string& serviceName = d_udpServiceNameByPort[entry.port()];
        if (serviceName.empty()) {
            serviceName = entry.service();
        }
    }

    return ntsa::Error();
}

ntsa::Error PortDatabase::addPortEntry(const bsl::string& service,
                                       ntsa::Port         port,
                                       const bsl::string& protocol)
{
    ntcdns::PortEntry entry;
    entry.service()  = service;
    entry.port()     = port;
    entry.protocol() = protocol;

    return this->addPortEntry(entry);
}

ntsa::Error PortDatabase::getPort(ntca::GetPortContext*       context,
                                  bsl::vector<ntsa::Port>*    result,
                                  const bslstl::StringRef&    serviceName,
                                  const ntca::GetPortOptions& options) const
{
    result->clear();

    bsl::vector<ntsa::Port> portList;

    bool examineTcpPortList = false;
    bool examineUdpPortList = false;

    if (options.transport().isNull()) {
        examineTcpPortList = true;
        examineUdpPortList = true;
    }
    else {
        if (options.transport().value() ==
                ntsa::Transport::e_TCP_IPV4_STREAM ||
            options.transport().value() == ntsa::Transport::e_TCP_IPV6_STREAM)
        {
            examineTcpPortList = true;
        }
        else if (options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
                 options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            examineUdpPortList = true;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    {
        LockGuard lock(&d_mutex);

        if (examineTcpPortList) {
            PortByServiceName::const_iterator it =
                d_tcpPortByServiceName.find(serviceName);

            if (it != d_tcpPortByServiceName.end()) {
                if (!examineUdpPortList) {
                    portList.insert(portList.end(),
                                    it->second.begin(),
                                    it->second.end());
                }
                else {
                    const PortVector& source = it->second;
                    for (PortVector::const_iterator jt = source.begin();
                         jt != source.end();
                         ++jt)
                    {
                        ntsa::Port port = *jt;
                        if (bsl::find(portList.begin(),
                                      portList.end(),
                                      port) == portList.end())
                        {
                            portList.push_back(port);
                        }
                    }
                }
            }
        }

        if (examineUdpPortList) {
            PortByServiceName::const_iterator it =
                d_udpPortByServiceName.find(serviceName);

            if (it != d_udpPortByServiceName.end()) {
                if (!examineTcpPortList) {
                    portList.insert(portList.end(),
                                    it->second.begin(),
                                    it->second.end());
                }
                else {
                    const PortVector& source = it->second;
                    for (PortVector::const_iterator jt = source.begin();
                         jt != source.end();
                         ++jt)
                    {
                        ntsa::Port port = *jt;
                        if (bsl::find(portList.begin(),
                                      portList.end(),
                                      port) == portList.end())
                        {
                            portList.push_back(port);
                        }
                    }
                }
            }
        }
    }

    if (portList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    context->setServiceName(serviceName);

    if (options.portSelector().isNull()) {
        *result = portList;
    }
    else {
        result->push_back(
            portList[options.portSelector().value() % portList.size()]);
    }

    return ntsa::Error();
}

ntsa::Error PortDatabase::getServiceName(
    ntca::GetServiceNameContext*       context,
    bsl::string*                       result,
    const ntsa::Port&                  port,
    const ntca::GetServiceNameOptions& options) const
{
    LockGuard lock(&d_mutex);

    bool found = false;

    if (!options.transport().isNull()) {
        if (options.transport().value() ==
                ntsa::Transport::e_TCP_IPV4_STREAM ||
            options.transport().value() == ntsa::Transport::e_TCP_IPV6_STREAM)
        {
            ServiceNameByPort::const_iterator it =
                d_tcpServiceNameByPort.find(port);

            if (it == d_tcpServiceNameByPort.end()) {
                return ntsa::Error(ntsa::Error::e_EOF);
            }

            if (!it->second.empty()) {
                *result = it->second;
                found   = true;
            }
        }
        else if (options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV4_DATAGRAM ||
                 options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            ServiceNameByPort::const_iterator it =
                d_udpServiceNameByPort.find(port);

            if (it == d_udpServiceNameByPort.end()) {
                return ntsa::Error(ntsa::Error::e_EOF);
            }

            if (!it->second.empty()) {
                *result = it->second;
                found   = true;
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        if (!found) {
            ServiceNameByPort::const_iterator it =
                d_tcpServiceNameByPort.find(port);

            if (it != d_tcpServiceNameByPort.end()) {
                if (!it->second.empty()) {
                    *result = it->second;
                    found   = true;
                }
            }
        }

        if (!found) {
            ServiceNameByPort::const_iterator it =
                d_udpServiceNameByPort.find(port);

            if (it != d_udpServiceNameByPort.end()) {
                if (!it->second.empty()) {
                    *result = it->second;
                    found   = true;
                }
            }
        }
    }

    if (!found) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    context->setPort(port);

    return ntsa::Error();
}

void dump(const ntcdns::PortDatabase& portDatabase)
{
    typedef bsl::vector<ntcdns::PortEntry> PortEntryVector;

    PortEntryVector portEntryVector;
    portDatabase.dump(&portEntryVector);

    for (bsl::size_t i = 0; i < portEntryVector.size(); ++i) {
        const ntcdns::PortEntry& portEntry = portEntryVector[i];

        bsl::cout << bsl::setw(10) << bsl::right << i;
        bsl::cout << bsl::setw(10) << bsl::right << portEntry.port();
        bsl::cout << bsl::setw(7) << bsl::right << portEntry.protocol();
        bsl::cout << "    " << bsl::left << portEntry.service();
        bsl::cout << bsl::endl;
    }
}

}  // close namespace test

NTCCFG_TEST_CASE(1)
{
    // Concern: Host database configurations from user-defined text.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // clang-format off
    const char ETC_HOSTS[] = ""
    "# User-defined hosts for this test driver\n"
    "\n"
    "127.0.0.1                          localhost.localdomain localhost\n"
    "\n"
    "192.168.1.101                      test-ipv4-1 # Test IPv4 host 1\n"
    "192.168.1.102                      test-ipv4-2 # Test IPv4 host 2\n"
    "192.168.1.103                      test-ipv4-2 # Test IPv4 host 2\n"
    "\n"
    "2001:0db8::1:0:0:1                 test-ipv6-1 # Test IPv6 host 1\n"
    "2001:0db8::1:0:0:2                 test-ipv6-2 # Test IPv6 host 2\n"
    "2001:0db8::1:0:0:3                 test-ipv6-2 # Test IPv6 host 2\n"
    "\n"
    "192.168.2.101                      test-both\n"
    "192.168.2.102                      test-both\n"
    "192.168.2.103                      test-both\n"
    "2001:0db8::2:0:0:1                 test-both\n"
    "2001:0db8::2:0:0:2                 test-both\n"
    "2001:0db8::2:0:0:3                 test-both\n"
    "\n"
    "# 192.168.1.200                    test-removed\n"
    "\n";
    // clang-format on

    ntccfg::TestAllocator ta;
    {
        ntcdns::HostDatabase hostDatabase(&ta);

        error = hostDatabase.loadText(ETC_HOSTS, sizeof ETC_HOSTS - 1);
        NTCCFG_TEST_OK(error);

        //
        // Test 'getIpAddress'.
        //

        // Get IP address for test-ipv4-1.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv4-1",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0], ntsa::IpAddress("192.168.1.101"));
        }

        // Get IPv4 address for test-ipv4-1.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V4);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv4-1",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0], ntsa::IpAddress("192.168.1.101"));
        }

        // Get IPv6 address for test-ipv4-1.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V6);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv4-1",
                                              options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get IP address for test-ipv4-2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv4-2",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 2);
            NTCCFG_TEST_EQ(ipAddressList[0], ntsa::IpAddress("192.168.1.102"));
            NTCCFG_TEST_EQ(ipAddressList[1], ntsa::IpAddress("192.168.1.103"));
        }

        // Get IPv4 address for test-ipv4-2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V4);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv4-2",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 2);
            NTCCFG_TEST_EQ(ipAddressList[0], ntsa::IpAddress("192.168.1.102"));
            NTCCFG_TEST_EQ(ipAddressList[1], ntsa::IpAddress("192.168.1.103"));
        }

        // Get IPv6 address for test-ipv4-2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V6);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv4-2",
                                              options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get IP address for test-ipv6-1.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv6-1",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0],
                           ntsa::IpAddress("2001:0db8::1:0:0:1"));
        }

        // Get IPv4 address for test-ipv6-1.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V4);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv6-1",
                                              options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get IPv6 address for test-ipv6-1.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V6);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv6-1",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);
            NTCCFG_TEST_EQ(ipAddressList[0],
                           ntsa::IpAddress("2001:0db8::1:0:0:1"));
        }

        // Get IP address for test-ipv6-2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv6-2",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 2);
            NTCCFG_TEST_EQ(ipAddressList[0],
                           ntsa::IpAddress("2001:0db8::1:0:0:2"));
            NTCCFG_TEST_EQ(ipAddressList[1],
                           ntsa::IpAddress("2001:0db8::1:0:0:3"));
        }

        // Get IPv4 address for test-ipv6-2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V4);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv6-2",
                                              options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get IPv6 address for test-ipv4-2.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V6);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-ipv6-2",
                                              options);
            NTCCFG_TEST_EQ(ipAddressList[0],
                           ntsa::IpAddress("2001:0db8::1:0:0:2"));
            NTCCFG_TEST_EQ(ipAddressList[1],
                           ntsa::IpAddress("2001:0db8::1:0:0:3"));
        }

        // Get IP address for test-both.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-both",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 6);
            NTCCFG_TEST_EQ(ipAddressList[0], ntsa::IpAddress("192.168.2.101"));
            NTCCFG_TEST_EQ(ipAddressList[1], ntsa::IpAddress("192.168.2.102"));
            NTCCFG_TEST_EQ(ipAddressList[2], ntsa::IpAddress("192.168.2.103"));
            NTCCFG_TEST_EQ(ipAddressList[3],
                           ntsa::IpAddress("2001:0db8::2:0:0:1"));
            NTCCFG_TEST_EQ(ipAddressList[4],
                           ntsa::IpAddress("2001:0db8::2:0:0:2"));
            NTCCFG_TEST_EQ(ipAddressList[5],
                           ntsa::IpAddress("2001:0db8::2:0:0:3"));
        }

        // Get IPv4 address for test-both.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V4);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-both",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 3);
            NTCCFG_TEST_EQ(ipAddressList[0], ntsa::IpAddress("192.168.2.101"));
            NTCCFG_TEST_EQ(ipAddressList[1], ntsa::IpAddress("192.168.2.102"));
            NTCCFG_TEST_EQ(ipAddressList[2], ntsa::IpAddress("192.168.2.103"));
        }

        // Get IPv6 address for test-both.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V6);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-both",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 3);
            NTCCFG_TEST_EQ(ipAddressList[0],
                           ntsa::IpAddress("2001:0db8::2:0:0:1"));
            NTCCFG_TEST_EQ(ipAddressList[1],
                           ntsa::IpAddress("2001:0db8::2:0:0:2"));
            NTCCFG_TEST_EQ(ipAddressList[2],
                           ntsa::IpAddress("2001:0db8::2:0:0:3"));
        }

        // Get IP address for test-both with round-robin selection.

        for (bsl::size_t i = 0; i < 2 * 6; ++i) {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressSelector(i);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-both",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);

            if (i % 6 == 0) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("192.168.2.101"));
            }

            if (i % 6 == 1) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("192.168.2.102"));
            }

            if (i % 6 == 2) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("192.168.2.103"));
            }

            if (i % 6 == 3) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("2001:0db8::2:0:0:1"));
            }

            if (i % 6 == 4) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("2001:0db8::2:0:0:2"));
            }

            if (i % 6 == 5) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("2001:0db8::2:0:0:3"));
            }
        }

        // Get IPv4 address for test-both with round-robin selection.

        for (bsl::size_t i = 0; i < 2 * 3; ++i) {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V4);
            options.setIpAddressSelector(i);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-both",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);

            if (i % 3 == 0) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("192.168.2.101"));
            }

            if (i % 3 == 1) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("192.168.2.102"));
            }

            if (i % 3 == 2) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("192.168.2.103"));
            }
        }

        // Get IPv6 address for test-both with round-robin selection.

        for (bsl::size_t i = 0; i < 2 * 3; ++i) {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            options.setIpAddressType(ntsa::IpAddressType::e_V6);
            options.setIpAddressSelector(i);

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-both",
                                              options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(ipAddressList.size(), 1);

            if (i % 3 == 0) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("2001:0db8::2:0:0:1"));
            }

            if (i % 3 == 1) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("2001:0db8::2:0:0:2"));
            }

            if (i % 3 == 2) {
                NTCCFG_TEST_EQ(ipAddressList[0],
                               ntsa::IpAddress("2001:0db8::2:0:0:3"));
            }
        }

        // Get IP address for test-removed.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-removed",
                                              options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get IP address for test-nonexistant.

        {
            ntca::GetIpAddressContext context;
            ntca::GetIpAddressOptions options;

            bsl::vector<ntsa::IpAddress> ipAddressList;
            error = hostDatabase.getIpAddress(&context,
                                              &ipAddressList,
                                              "test-nonexistant",
                                              options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        //
        // Test 'getDomainName'.
        //

        // Get domain name for 127.0.0.1.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = hostDatabase.getDomainName(&context,
                                               &domainName,
                                               ntsa::IpAddress("127.0.0.1"),
                                               options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(domainName, "localhost.localdomain");
        }

        // Get domain name for an IP address that results in 'test-ipv4-1'.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error =
                hostDatabase.getDomainName(&context,
                                           &domainName,
                                           ntsa::IpAddress("192.168.1.101"),
                                           options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(domainName, "test-ipv4-1");
        }

        // Get domain name for an IP address that results in 'test-ipv4-2'.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error =
                hostDatabase.getDomainName(&context,
                                           &domainName,
                                           ntsa::IpAddress("192.168.1.102"),
                                           options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(domainName, "test-ipv4-2");
        }

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error =
                hostDatabase.getDomainName(&context,
                                           &domainName,
                                           ntsa::IpAddress("192.168.1.103"),
                                           options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(domainName, "test-ipv4-2");
        }

        // Get domain name for an IP address that results in 'test-ipv6-1'.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = hostDatabase.getDomainName(
                &context,
                &domainName,
                ntsa::IpAddress("2001:0db8::1:0:0:1"),
                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(domainName, "test-ipv6-1");
        }

        // Get domain name for an IP address that results in 'test-ipv6-2'.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = hostDatabase.getDomainName(
                &context,
                &domainName,
                ntsa::IpAddress("2001:0db8::1:0:0:2"),
                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(domainName, "test-ipv6-2");
        }

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = hostDatabase.getDomainName(
                &context,
                &domainName,
                ntsa::IpAddress("2001:0db8::1:0:0:3"),
                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(domainName, "test-ipv6-2");
        }

        // Get a domain name for an unknown IP address.

        {
            ntca::GetDomainNameContext context;
            ntca::GetDomainNameOptions options;

            bsl::string domainName;
            error = hostDatabase.getDomainName(&context,
                                               &domainName,
                                               ntsa::IpAddress("10.10.0.1"),
                                               options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Port database configurations from user-defined text.
    // Plan:

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // clang-format off
    const char ETC_SERVICES[] = ""
    "# User-defined hosts for this test driver\n"
    "\n"
    "echo                7/tcp    # Echo\n"
    "echo                7/udp    # Echo\n"
    "\n"
    "                    24/tcp   # Any private service\n"
    "                    24/udp   # Any private service\n"
    "\n"
    "test-shared     50000/tcp\n"
    "test-shared     50000/udp\n"
    "\n"
    "no-port-1\n"
    "no-port-2        # 30/tcp\n"
    "no-protocol-1a     30\n"
    "no-protocol-1b     30 # /tcp\n"
    "no-protocol-2a     31/\n"
    "no-protocol-2a     31/ # tcp\n"
    "\n"
    "test-both       50001/tcp\n"
    "test-both       50002/udp\n"
    "\n"
    "test-tcp        50003/tcp    # Only TCP\n"
    "test-udp        50004/udp    # Only UDP\n"
    "\n"
    "#\n"
    "# Many ports assigned to the same service name\n"
    "#\n"
    "test-many       20001/tcp\n"
    "test-many       20002/tcp\n"
    "test-many       20003/tcp\n"
    "test-many       20004/tcp\n"
    "test-many       20005/tcp\n"
    "test-many       20006/tcp\n"
    "test-many       20001/udp\n"
    "test-many       20002/udp\n"
    "test-many       20003/udp\n"
    "test-many       20007/udp\n"
    "test-many       20008/udp\n"
    "test-many       20009/udp\n"
    "\n"
    "# test-removed  50005/tcp\n"
    "# test-removed  50006/udp\n"
    "\n";
    // clang-format on

    ntccfg::TestAllocator ta;
    {
        ntcdns::PortDatabase portDatabase(&ta);

        error = portDatabase.loadText(ETC_SERVICES, sizeof ETC_SERVICES - 1);
        NTCCFG_TEST_OK(error);

        //
        // Test 'getPort'.
        //

        // Get port for 'test-shared'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-shared",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50000);
        }

        // Get TCP port for 'test-shared'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-shared",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50000);
        }

        // Get UDP port for 'test-shared'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-shared",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50000);
        }

        // Get port for 'test-both'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-both",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 2);
            NTCCFG_TEST_EQ(portList[0], 50001);
            NTCCFG_TEST_EQ(portList[1], 50002);
        }

        // Get TCP port for 'test-both'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-both",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50001);
        }

        // Get UDP port for 'test-both'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-both",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50002);
        }

        // Get port for 'test-tcp'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            bsl::vector<ntsa::Port> portList;
            error =
                portDatabase.getPort(&context, &portList, "test-tcp", options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50003);
        }

        // Get TCP port for 'test-tcp'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::vector<ntsa::Port> portList;
            error =
                portDatabase.getPort(&context, &portList, "test-tcp", options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50003);
        }

        // Get UDP port for 'test-tcp'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::vector<ntsa::Port> portList;
            error =
                portDatabase.getPort(&context, &portList, "test-tcp", options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get port for 'test-udp'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            bsl::vector<ntsa::Port> portList;
            error =
                portDatabase.getPort(&context, &portList, "test-udp", options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50004);
        }

        // Get TCP port for 'test-udp'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::vector<ntsa::Port> portList;
            error =
                portDatabase.getPort(&context, &portList, "test-udp", options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get UDP port for 'test-udp'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::vector<ntsa::Port> portList;
            error =
                portDatabase.getPort(&context, &portList, "test-udp", options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);
            NTCCFG_TEST_EQ(portList[0], 50004);
        }

        // Get port for 'test-many'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-many",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 9);
            NTCCFG_TEST_EQ(portList[0], 20001);
            NTCCFG_TEST_EQ(portList[1], 20002);
            NTCCFG_TEST_EQ(portList[2], 20003);
            NTCCFG_TEST_EQ(portList[3], 20004);
            NTCCFG_TEST_EQ(portList[4], 20005);
            NTCCFG_TEST_EQ(portList[5], 20006);
            NTCCFG_TEST_EQ(portList[6], 20007);
            NTCCFG_TEST_EQ(portList[7], 20008);
            NTCCFG_TEST_EQ(portList[8], 20009);
        }

        // Get TCP port for 'test-many'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-many",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 6);
            NTCCFG_TEST_EQ(portList[0], 20001);
            NTCCFG_TEST_EQ(portList[1], 20002);
            NTCCFG_TEST_EQ(portList[2], 20003);
            NTCCFG_TEST_EQ(portList[3], 20004);
            NTCCFG_TEST_EQ(portList[4], 20005);
            NTCCFG_TEST_EQ(portList[5], 20006);
        }

        // Get UDP port for 'test-many'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-many",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 6);
            NTCCFG_TEST_EQ(portList[0], 20001);
            NTCCFG_TEST_EQ(portList[1], 20002);
            NTCCFG_TEST_EQ(portList[2], 20003);
            NTCCFG_TEST_EQ(portList[3], 20007);
            NTCCFG_TEST_EQ(portList[4], 20008);
            NTCCFG_TEST_EQ(portList[5], 20009);
        }

        // Get port for 'test-many' with round-robin selection.

        for (bsl::size_t i = 0; i < 2 * 9; ++i) {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setPortSelector(i);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-many",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);

            if (i % 9 == 0) {
                NTCCFG_TEST_EQ(portList[0], 20001);
            }

            if (i % 9 == 1) {
                NTCCFG_TEST_EQ(portList[0], 20002);
            }

            if (i % 9 == 2) {
                NTCCFG_TEST_EQ(portList[0], 20003);
            }

            if (i % 9 == 3) {
                NTCCFG_TEST_EQ(portList[0], 20004);
            }

            if (i % 9 == 4) {
                NTCCFG_TEST_EQ(portList[0], 20005);
            }

            if (i % 9 == 5) {
                NTCCFG_TEST_EQ(portList[0], 20006);
            }

            if (i % 9 == 6) {
                NTCCFG_TEST_EQ(portList[0], 20007);
            }

            if (i % 9 == 7) {
                NTCCFG_TEST_EQ(portList[0], 20008);
            }

            if (i % 9 == 8) {
                NTCCFG_TEST_EQ(portList[0], 20009);
            }
        }

        // Get TCP port for 'test-many' with round-robin selection.

        for (bsl::size_t i = 0; i < 2 * 6; ++i) {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setPortSelector(i);
            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-many",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);

            if (i % 6 == 0) {
                NTCCFG_TEST_EQ(portList[0], 20001);
            }

            if (i % 6 == 1) {
                NTCCFG_TEST_EQ(portList[0], 20002);
            }

            if (i % 6 == 2) {
                NTCCFG_TEST_EQ(portList[0], 20003);
            }

            if (i % 6 == 3) {
                NTCCFG_TEST_EQ(portList[0], 20004);
            }

            if (i % 6 == 4) {
                NTCCFG_TEST_EQ(portList[0], 20005);
            }

            if (i % 6 == 5) {
                NTCCFG_TEST_EQ(portList[0], 20006);
            }
        }

        // Get UDP port for 'test-many' with round-robin selection.

        for (bsl::size_t i = 0; i < 2 * 6; ++i) {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            options.setPortSelector(i);
            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-many",
                                         options);
            NTCCFG_TEST_OK(error);

            NTCCFG_TEST_EQ(portList.size(), 1);

            if (i % 6 == 0) {
                NTCCFG_TEST_EQ(portList[0], 20001);
            }

            if (i % 6 == 1) {
                NTCCFG_TEST_EQ(portList[0], 20002);
            }

            if (i % 6 == 2) {
                NTCCFG_TEST_EQ(portList[0], 20003);
            }

            if (i % 6 == 3) {
                NTCCFG_TEST_EQ(portList[0], 20007);
            }

            if (i % 6 == 4) {
                NTCCFG_TEST_EQ(portList[0], 20008);
            }

            if (i % 6 == 5) {
                NTCCFG_TEST_EQ(portList[0], 20009);
            }
        }

        // Get port for 'test-remove'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-removed",
                                         options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get port for 'test-nonexistant'.

        {
            ntca::GetPortContext context;
            ntca::GetPortOptions options;

            bsl::vector<ntsa::Port> portList;
            error = portDatabase.getPort(&context,
                                         &portList,
                                         "test-nonexistant",
                                         options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        //
        // Test 'getServiceName'.
        //

        // Get service name for a port assigned to 'test-shared'.

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50000,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-shared");
        }

        // Get service name for a TCP port assigned to 'test-shared'.

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50000,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-shared");
        }

        // Get service name for a UDP port assigned to 'test-shared'.

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50000,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-shared");
        }

        // Get service name for a port assigned to 'test-both'.

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50001,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-both");
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50001,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-both");
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50001,
                                                options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50002,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-both");
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50002,
                                                options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50002,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-both");
        }

        // Get service name for a port assigned to 'test-tcp'.

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50003,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-tcp");
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50003,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-tcp");
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50003,
                                                options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        // Get service name for a port assigned to 'test-udp'.

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50004,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-udp");
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_TCP_IPV4_STREAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50004,
                                                options);
            NTCCFG_TEST_ERROR(error, ntsa::Error::e_EOF);
        }

        {
            ntca::GetServiceNameContext context;
            ntca::GetServiceNameOptions options;

            options.setTransport(ntsa::Transport::e_UDP_IPV4_DATAGRAM);

            bsl::string serviceName;
            error = portDatabase.getServiceName(&context,
                                                &serviceName,
                                                50004,
                                                options);
            NTCCFG_TEST_OK(error);
            NTCCFG_TEST_EQ(serviceName, "test-udp");
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Host database configurations from "/etc/hosts".
    // Plan:

    if (!ntscfg::Platform::hasHostDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        bsls::Stopwatch stopwatch;
        stopwatch.start();

        ntcdns::HostDatabase hostDatabase(&ta);

        error = hostDatabase.load();
        NTCCFG_TEST_ASSERT(!error);

        stopwatch.stop();

        if (NTCCFG_TEST_VERBOSITY > 0) {
            bsl::cout << "Loaded host database in "
                      << bsls::TimeInterval(stopwatch.elapsedTime())
                             .totalMilliseconds()
                      << " milliseconds" << bsl::endl;
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Port database configurations from "/etc/services".
    // Plan:

    if (!ntscfg::Platform::hasPortDatabase()) {
        return;
    }

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    ntccfg::TestAllocator ta;
    {
        bsls::Stopwatch stopwatch;
        stopwatch.start();

        ntcdns::PortDatabase portDatabase(&ta);

        error = portDatabase.load();
        NTCCFG_TEST_ASSERT(!error);

        stopwatch.stop();

        if (NTCCFG_TEST_VERBOSITY > 0) {
            test::dump(portDatabase);

            bsl::cout << "Loaded port database in "
                      << bsls::TimeInterval(stopwatch.elapsedTime())
                             .totalMilliseconds()
                      << " milliseconds" << bsl::endl;
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
}
NTCCFG_TEST_DRIVER_END;
