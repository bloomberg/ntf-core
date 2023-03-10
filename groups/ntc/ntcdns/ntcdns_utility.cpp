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

#include <ntcdns_utility.h>

#include <ntci_log.h>

#include <bdlb_chartype.h>
#include <bdlb_string.h>
#include <bdlb_stringrefutil.h>
#include <bdlb_tokenizer.h>
#include <bdls_filedescriptorguard.h>
#include <bdls_filesystemutil.h>
#include <bdls_memoryutil.h>
#include <bslma_deallocatorguard.h>
#include <bslma_deallocatorproctor.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>

#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
// clang-format off
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
// clang-format on
#define MAXHOSTNAMELEN 256
#pragma comment(lib, "iphlpapi.lib")
#endif

// Uncomment or set to 0 to 'read' files instead of memory-mapping them.
// #define NTCDNS_UTILITY_MEMORY_MAP_FILES 1

// Uncomment or set to to zero to avoid forcibilly trimming leading and
// trailing whitespace from each token.
#define NTCDNS_UTILITY_TRIM_TOKEN 1

namespace BloombergLP {
namespace ntcdns {

namespace {

// The default timeout, in seconds.
const bsl::size_t k_DEFAULT_TIMEOUT = 5;

// The default flag that indicates whether the name servers should be
// tried n a round-robin order.
const bool k_DEFAULT_ROTATE = false;

// The default flag that indicates whether debug information should be
// generated.
const bool k_DEFAULT_DEBUG = false;

// The default dot count threshold before a name is assumed to be an absolute
// name.
const bsl::size_t k_DEFAULT_NDOTS = 1;

// The default DNS port.
const ntsa::Port k_DEFAULT_PORT = 53;

// The maximum number of search entries.
// const bsl::size_t k_MAX_SEARCH_ENTRIES = 6;

// The maximum number of resolution attempts.
const bsl::size_t k_MAX_ATTEMPTS = 5;

// The maximum timeout, in seconds.
const bsl::size_t k_MAX_TIMEOUT = 30;

// The maximum dot count threshold before a name is assumed to be an absolute
// name.
const bsl::size_t k_MAX_NDOTS = 15;

/// Provide a guard to automatically free address information.
class AddrInfoGuard
{
    struct addrinfo* d_info_p;

  private:
    AddrInfoGuard(const AddrInfoGuard&) BSLS_KEYWORD_DELETED;
    AddrInfoGuard& operator=(const AddrInfoGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically releases resources for the
    /// specified 'info'.
    explicit AddrInfoGuard(struct addrinfo* info);

    /// Destroy this object.
    ~AddrInfoGuard();
};

AddrInfoGuard::AddrInfoGuard(struct addrinfo* info)
: d_info_p(info)
{
}

AddrInfoGuard::~AddrInfoGuard()
{
    freeaddrinfo(d_info_p);
}

ntsa::Error getHostnameFullyQualified(bsl::string* result)
{
    // Load into the specified 'result' the fully-qualified domain name of
    // the local host. Return the error.

#if defined(BSLS_PLATFORM_OS_UNIX)

    int rc;

    char buffer[MAXHOSTNAMELEN + 1];
    if (0 != gethostname(buffer, sizeof buffer)) {
        return ntsa::Error(errno);
    }

    struct addrinfo hints;
    bsl::memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags  = AI_CANONNAME;

    struct addrinfo* info = 0;
    rc                    = getaddrinfo(buffer, 0, &hints, &info);
    if (rc != 0) {
        return ntsa::Error(errno);
    }

    AddrInfoGuard guard(info);

    result->assign(info->ai_canonname);

    return ntsa::Error();

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    char  buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof buffer;

    if (0 == GetComputerNameEx(ComputerNameDnsFullyQualified, buffer, &size)) {
        return ntsa::Error(GetLastError());
    }

    result->assign(buffer, size);
    return ntsa::Error();

#else
#error Not implemented
#endif
}

ntsa::Error parseUnsignedInteger(unsigned short* result,
                                 const char*     data,
                                 bsl::size_t     size)
{
    bsl::string       s(data, size);
    bsl::stringstream ss;
    ss << s;
    ss >> *result;
    return ntsa::Error();
}

ntsa::Error parseUnsignedInteger(unsigned int* result,
                                 const char*   data,
                                 bsl::size_t   size)
{
    bsl::string       s(data, size);
    bsl::stringstream ss;
    ss << s;
    ss >> *result;
    return ntsa::Error();
}

ntsa::Error parseUnsignedInteger(unsigned short*          result,
                                 const bslstl::StringRef& token)
{
    return parseUnsignedInteger(result, token.data(), token.size());
}

ntsa::Error parseUnsignedInteger(unsigned int*            result,
                                 const bslstl::StringRef& token)
{
    return parseUnsignedInteger(result, token.data(), token.size());
}

ntsa::Error parseClientConfigLine(ntcdns::ClientConfig*    config,
                                  const bslstl::StringRef& line)
{
    // Load into the specified 'config' the DNS resolver configuration parsed
    // from the specified 'line'.

    NTCI_LOG_CONTEXT();

    bslstl::StringRef uncommentedLine = line;
    for (bsl::size_t i = 0; i < uncommentedLine.size(); ++i) {
        if (uncommentedLine[i] == '#' || uncommentedLine[i] == ';') {
            uncommentedLine.assign(line.data(), line.data() + i);
            break;
        }
    }

    if (uncommentedLine.empty()) {
        return ntsa::Error();
    }

    ntsa::Error error;

    bdlb::Tokenizer tokenizer(uncommentedLine, " \t", "");

    while (tokenizer.isValid()) {
        bslstl::StringRef token = bdlb::StringRefUtil::trim(tokenizer.token());

        // GDE_LOG('D') << "Token: '" << token << "'" << GDE_LOG_END;

        if (bdlb::StringRefUtil::areEqualCaseless(
                token,
                bslstl::StringRef("nameserver", 10)))
        {
            ++tokenizer;

            if (!tokenizer.isValid()) {
                NTCI_LOG_STREAM_ERROR
                    << "Failed to parse resolver configuration: " << line
                    << NTCI_LOG_STREAM_END;

                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            token = bdlb::StringRefUtil::trim(tokenizer.token());

            ntcdns::NameServerConfig nameServer;
            nameServer.address().host() = token;
            config->nameServer().push_back(nameServer);

            ++tokenizer;
        }
        else if (bdlb::StringRefUtil::areEqualCaseless(
                     token,
                     bslstl::StringRef("domain", 6)))
        {
            ++tokenizer;

            if (!tokenizer.isValid()) {
                NTCI_LOG_STREAM_ERROR
                    << "Failed to parse resolver configuration: " << line
                    << NTCI_LOG_STREAM_END;

                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            config->domain() = bdlb::StringRefUtil::trim(tokenizer.token());

            ++tokenizer;
        }
        else if (bdlb::StringRefUtil::areEqualCaseless(
                     token,
                     bslstl::StringRef("search", 6)))
        {
            ++tokenizer;

            while (tokenizer.isValid()) {
                config->search().push_back(
                    bdlb::StringRefUtil::trim(tokenizer.token()));
                ++tokenizer;
            }
        }
        else if (bdlb::StringRefUtil::areEqualCaseless(
                     token,
                     bslstl::StringRef("sortlist", 8)))
        {
            ++tokenizer;

            while (tokenizer.isValid()) {
                token = bdlb::StringRefUtil::trim(tokenizer.token());

                bslstl::StringRef entry = token;

                bdlb::Tokenizer subtokenizer(entry, "/", "");
                if (subtokenizer.isValid()) {
                    ntcdns::SortListItem sortListItem;
                    sortListItem.address() =
                        bdlb::StringRefUtil::trim(subtokenizer.token());

                    ++subtokenizer;

                    if (subtokenizer.isValid()) {
                        sortListItem.netmask() =
                            bdlb::StringRefUtil::trim(subtokenizer.token());
                    }

                    if (config->sortList().isNull()) {
                        config->sortList().makeValue();
                    }

                    config->sortList().value().item().push_back(sortListItem);
                }

                ++tokenizer;
            }
        }
        else if (bdlb::StringRefUtil::areEqualCaseless(
                     token,
                     bslstl::StringRef("options", 7)))
        {
            ++tokenizer;

            while (tokenizer.isValid()) {
                token = bdlb::StringRefUtil::trim(tokenizer.token());

                bslstl::StringRef entry = token;

                bdlb::Tokenizer subtokenizer(entry, ":", "");

                if (subtokenizer.isValid()) {
                    bslstl::StringRef key =
                        bdlb::StringRefUtil::trim(subtokenizer.token());

                    if (bdlb::StringRefUtil::areEqualCaseless(
                            key,
                            bslstl::StringRef("debug", 5)))
                    {
                        config->debug() = true;
                    }
                    else if (bdlb::StringRefUtil::areEqualCaseless(
                                 key,
                                 bslstl::StringRef("rotate", 6)))
                    {
                        config->rotate() = true;
                    }
                    else if (bdlb::StringRefUtil::areEqualCaseless(
                                 key,
                                 bslstl::StringRef("ndots", 5)))
                    {
                        ++subtokenizer;

                        if (subtokenizer.isValid()) {
                            bslstl::StringRef value =
                                bdlb::StringRefUtil::trim(
                                    subtokenizer.token());

                            error = parseUnsignedInteger(
                                &config->ndots().makeValue(),
                                value);

                            if (error) {
                                NTCI_LOG_STREAM_WARN
                                    << "Unsupported DNS resolver "
                                       "configuration option '"
                                    << "ndots"
                                    << "' value: " << value
                                    << NTCI_LOG_STREAM_END;

                                config->ndots().reset();
                            }
                        }
                    }
                    else if (bdlb::StringRefUtil::areEqualCaseless(
                                 key,
                                 bslstl::StringRef("timeout", 7)))
                    {
                        ++subtokenizer;

                        if (subtokenizer.isValid()) {
                            bslstl::StringRef value =
                                bdlb::StringRefUtil::trim(
                                    subtokenizer.token());

                            error = parseUnsignedInteger(
                                &config->timeout().makeValue(),
                                value);

                            if (error) {
                                NTCI_LOG_STREAM_WARN
                                    << "Unsupported DNS resolver "
                                       "configuration option '"
                                    << "timeout"
                                    << "' value: " << value
                                    << NTCI_LOG_STREAM_END;

                                config->timeout().reset();
                            }
                        }
                    }
                    else if (bdlb::StringRefUtil::areEqualCaseless(
                                 key,
                                 bslstl::StringRef("attempts", 8)))
                    {
                        ++subtokenizer;

                        if (subtokenizer.isValid()) {
                            bslstl::StringRef value =
                                bdlb::StringRefUtil::trim(
                                    subtokenizer.token());

                            error = parseUnsignedInteger(
                                &config->attempts().makeValue(),
                                value);

                            if (error) {
                                NTCI_LOG_STREAM_WARN
                                    << "Unsupported DNS resolver "
                                       "configuration option '"
                                    << "attempts"
                                    << "' value: " << value
                                    << NTCI_LOG_STREAM_END;

                                config->attempts().reset();
                            }
                        }
                    }
                    else {
                        NTCI_LOG_STREAM_WARN << "Unsupported DNS resolver "
                                                "configuration option '"
                                             << entry << "'"
                                             << NTCI_LOG_STREAM_END;
                    }
                }

                ++tokenizer;
            }
        }
        else {
            NTCI_LOG_STREAM_WARN << "Unsupported DNS resolver configuration '"
                                 << token << "'" << NTCI_LOG_STREAM_END;

            ++tokenizer;
        }
    }

    return ntsa::Error();
}

ntsa::Error parseHostLine(ntcdns::HostDatabaseConfig* config,
                          const bslstl::StringRef&    line)
{
    // Load into the specified 'config' the DNS resolver configuration parsed
    // from the specified 'line'.

    NTCI_LOG_CONTEXT();

    bslstl::StringRef uncommentedLine = line;
    for (bsl::size_t i = 0; i < uncommentedLine.size(); ++i) {
        if (uncommentedLine[i] == '#' || uncommentedLine[i] == ';') {
            uncommentedLine.assign(line.data(), line.data() + i);
            break;
        }
    }

    uncommentedLine = bdlb::StringRefUtil::trim(uncommentedLine);

    if (uncommentedLine.empty()) {
        return ntsa::Error();
    }

    ntsa::Error error;

    bdlb::Tokenizer tokenizer(uncommentedLine, " \t", "");

    bslstl::StringRef token;

    config->entry().resize(config->entry().size() + 1);
    ntcdns::HostEntry& item = config->entry().back();

    if (!tokenizer.isValid()) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse host list item '" << line
                              << "': missing address" << NTCI_LOG_STREAM_END;
        config->entry().pop_back();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    token = tokenizer.token();
#if NTCDNS_UTILITY_TRIM_TOKEN
    token = bdlb::StringRefUtil::trim(token);
#endif
    ++tokenizer;

    item.address() = token;

    if (!tokenizer.isValid()) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse host list item '" << line
                              << "': missing canonical hostname"
                              << NTCI_LOG_STREAM_END;
        config->entry().pop_back();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    token = tokenizer.token();
#if NTCDNS_UTILITY_TRIM_TOKEN
    token = bdlb::StringRefUtil::trim(token);
#endif
    ++tokenizer;

    item.canonicalHostname() = token;

    while (tokenizer.isValid()) {
        token = tokenizer.token();
#if NTCDNS_UTILITY_TRIM_TOKEN
        token = bdlb::StringRefUtil::trim(token);
#endif
        ++tokenizer;

        item.aliases().push_back(token);
    }

    return ntsa::Error();
}

ntsa::Error parsePortLine(ntcdns::PortDatabaseConfig* config,
                          const bslstl::StringRef&    line)
{
    // Load into the specified 'config' the DNS resolver configuration parsed
    // from the specified 'line'.

    NTCI_LOG_CONTEXT();

    bslstl::StringRef uncommentedLine = line;
    for (bsl::size_t i = 0; i < uncommentedLine.size(); ++i) {
        if (uncommentedLine[i] == '#' || uncommentedLine[i] == ';') {
            uncommentedLine.assign(line.data(), line.data() + i);
            break;
        }
    }

    uncommentedLine = bdlb::StringRefUtil::trim(uncommentedLine);

    if (uncommentedLine.empty()) {
        return ntsa::Error();
    }

    ntsa::Error error;

    bdlb::Tokenizer tokenizer(uncommentedLine, " \t", "");

    config->entry().resize(config->entry().size() + 1);
    ntcdns::PortEntry& item = config->entry().back();

    if (!tokenizer.isValid()) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse port list item '" << line
                              << "': missing service" << NTCI_LOG_STREAM_END;
        config->entry().pop_back();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    item.service() = bdlb::StringRefUtil::trim(tokenizer.token());
    ++tokenizer;

    if (tokenizer.isValid()) {
        bdlb::Tokenizer subtokenizer(
            bdlb::StringRefUtil::trim(tokenizer.token()),
            " \t/",
            "");

        if (!subtokenizer.isValid()) {
            NTCI_LOG_STREAM_ERROR << "Failed to parse port list item '" << line
                                  << "': missing port" << NTCI_LOG_STREAM_END;
            config->entry().pop_back();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = parseUnsignedInteger(
            &item.port(),
            bdlb::StringRefUtil::trim(subtokenizer.token()));
        if (error) {
            NTCI_LOG_STREAM_ERROR
                << "Failed to parse port list item '" << line
                << "': invalid port '"
                << bdlb::StringRefUtil::trim(subtokenizer.token()) << "'"
                << NTCI_LOG_STREAM_END;
            config->entry().pop_back();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        // Some entries may have invalid ports. Ignore them.
        if (item.port() == 0) {
            NTCI_LOG_STREAM_DEBUG
                << "Failed to parse port list item '" << line
                << "': invalid port '"
                << bdlb::StringRefUtil::trim(subtokenizer.token()) << "'"
                << NTCI_LOG_STREAM_END;
            config->entry().pop_back();
            return ntsa::Error();
        }

        ++subtokenizer;

        if (!subtokenizer.isValid()) {
            NTCI_LOG_STREAM_ERROR << "Failed to parse port list item '" << line
                                  << "': missing protocol"
                                  << NTCI_LOG_STREAM_END;
            config->entry().pop_back();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        item.protocol() = bdlb::StringRefUtil::trim(subtokenizer.token());
    }
    else {
        bsl::string nonstandardEntry = item.service();
        item.service().clear();

        NTCI_LOG_STREAM_WARN
            << "Parsing non-standard entry: " << nonstandardEntry
            << NTCI_LOG_STREAM_END;

        bdlb::Tokenizer subtokenizer(nonstandardEntry, " \t/", "");

        if (!subtokenizer.isValid()) {
            NTCI_LOG_STREAM_ERROR << "Failed to parse port list item '" << line
                                  << "': missing port" << NTCI_LOG_STREAM_END;
            config->entry().pop_back();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = parseUnsignedInteger(
            &item.port(),
            bdlb::StringRefUtil::trim(subtokenizer.token()));

        if (error) {
            NTCI_LOG_STREAM_ERROR
                << "Failed to parse port list item '" << line
                << "': invalid port '"
                << bdlb::StringRefUtil::trim(subtokenizer.token()) << "'"
                << NTCI_LOG_STREAM_END;
            config->entry().pop_back();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        // Some entries may have invalid ports. Ignore them.
        if (item.port() == 0) {
            NTCI_LOG_STREAM_DEBUG
                << "Failed to parse port list item '" << line
                << "': invalid port '"
                << bdlb::StringRefUtil::trim(subtokenizer.token()) << "'"
                << NTCI_LOG_STREAM_END;
            config->entry().pop_back();
            return ntsa::Error();
        }

        ++subtokenizer;

        if (!subtokenizer.isValid()) {
            NTCI_LOG_STREAM_ERROR << "Failed to parse port list item '" << line
                                  << "': missing protocol"
                                  << NTCI_LOG_STREAM_END;
            config->entry().pop_back();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        item.protocol() = bdlb::StringRefUtil::trim(subtokenizer.token());
    }

    return ntsa::Error();
}

#if defined(BSLS_PLATFORM_OS_WINDOWS)

ntsa::Error convertWideString(bsl::string* destination, const WCHAR* source)
{
    // Load into the specified 'destination' string the specified
    // wide-character 'source' string converted into UTF-8.

    destination->clear();

    bsl::size_t sourceLength = wcslen(source);

    if (sourceLength == 0) {
        return ntsa::Error();
    }

    int rc;

    rc = WideCharToMultiByte(CP_UTF8,
                             0,
                             (LPCWCH)(&source[0]),
                             (int)sourceLength,
                             0,
                             0,
                             0,
                             0);

    if (rc <= 0) {
        return ntsa::Error(GetLastError());
    }

    destination->resize(rc, 0);

    rc = WideCharToMultiByte(CP_UTF8,
                             0,
                             (LPCWCH)(&source[0]),
                             (int)sourceLength,
                             &((*destination)[0]),
                             rc,
                             0,
                             0);

    if (rc <= 0) {
        return ntsa::Error(GetLastError());
    }

    return ntsa::Error();
}

ntsa::Error loadNameServers(bsl::vector<ntcdns::NameServerConfig>* nameServers)
{
    // Load into the specified 'name_servers' the name servers assigned by
    // DHCP.

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    const bsl::size_t ADDRESS_LIST_SIZE = 15 * 1024;

    IP_ADAPTER_ADDRESSES* interfaceAddressList =
        static_cast<IP_ADAPTER_ADDRESSES*>(
            allocator->allocate(ADDRESS_LIST_SIZE));

    bslma::DeallocatorGuard<bslma::Allocator> guard(interfaceAddressList,
                                                    allocator);

    ULONG interfaceAddressList_size = ADDRESS_LIST_SIZE;
    ULONG rc                        = GetAdaptersAddresses(AF_INET,
                                    GAA_FLAG_SKIP_MULTICAST,
                                    0,
                                    interfaceAddressList,
                                    &interfaceAddressList_size);

    if (rc != ERROR_SUCCESS) {
        DWORD lastError = GetLastError();

        if (lastError == ERROR_NO_DATA) {
            nameServers->clear();
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        error = ntsa::Error(lastError);

        NTCI_LOG_STREAM_ERROR
            << "Failed to get network interface addresses: " << error
            << NTCI_LOG_STREAM_END;

        return error;
    }

    for (IP_ADAPTER_ADDRESSES* interfaceAddress = interfaceAddressList;
         interfaceAddress != 0;
         interfaceAddress = interfaceAddress->Next)
    {
        // Skip over host side VMware adapters, which have names like:
        // "VMware Virtual Ethernet Adapter for VMnet1".

        bsl::string friendlyName;
        convertWideString(&friendlyName, interfaceAddress->FriendlyName);

        bsl::string description;
        convertWideString(&description, interfaceAddress->Description);

        bool skip =
            (bsl::strstr(interfaceAddress->AdapterName, "VMnet") != 0) ||
            (bsl::strstr(friendlyName.c_str(), "VMnet") != 0) ||
            (bsl::strstr(description.c_str(), "VMnet") != 0);

        if (skip) {
            continue;
        }

        for (IP_ADAPTER_DNS_SERVER_ADDRESS_XP* dnsServer =
                 interfaceAddress->FirstDnsServerAddress;
             dnsServer != 0;
             dnsServer = dnsServer->Next)
        {
            if (dnsServer->Address.lpSockaddr->sa_family == AF_INET) {
                struct sockaddr_in* socketAddressIpv4 =
                    reinterpret_cast<struct sockaddr_in*>(
                        dnsServer->Address.lpSockaddr);

                ntsa::Ipv4Address ipv4Address;
                ipv4Address.copyFrom(&socketAddressIpv4->sin_addr,
                                     sizeof socketAddressIpv4->sin_addr);

                ntcdns::NameServerConfig nameServer;
                nameServer.address().host() = ipv4Address.text();
                nameServer.address().port() = k_DEFAULT_PORT;

                nameServers->push_back(nameServer);
            }
            else if (dnsServer->Address.lpSockaddr->sa_family == AF_INET6) {
                struct sockaddr_in6* socketAddressIpv6 =
                    reinterpret_cast<struct sockaddr_in6*>(
                        dnsServer->Address.lpSockaddr);

                ntsa::Ipv6Address ipv6Address;
                ipv6Address.copyFrom(&socketAddressIpv6->sin6_addr,
                                     sizeof socketAddressIpv6->sin6_addr);

                ntcdns::NameServerConfig nameServer;
                nameServer.address().host() = ipv6Address.text();
                nameServer.address().port() = k_DEFAULT_PORT;

                nameServers->push_back(nameServer);
            }
        }
    }

    return ntsa::Error();
}

#endif

void sanitizeClientConfig(ntcdns::ClientConfig* config)
{
    if (config->nameServer().size() > 0) {
        for (bsl::size_t i = 0; i < config->nameServer().size(); ++i) {
            if (config->nameServer()[i].address().port().isNull()) {
                config->nameServer()[i].address().port() = k_DEFAULT_PORT;
            }
        }
    }
    else {
        ntcdns::NameServerConfig nameServer;
        nameServer.address().host() = "8.8.8.8";
        nameServer.address().port() = k_DEFAULT_PORT;

        config->nameServer().push_back(nameServer);
    }

    if (config->domain().isNull() && config->search().empty()) {
        bsl::string canonicalName;
        ntsa::Error error = getHostnameFullyQualified(&canonicalName);

        if (!error) {
            bsl::string::size_type n = canonicalName.find('.');
            if (n != bsl::string::npos) {
                config->domain() = canonicalName.substr(n + 1);
            }
            else {
                config->domain() = ".";
            }
        }
        else {
            config->domain() = ".";
        }
    }

    if (!config->domain().isNull() && config->search().size() > 0) {
        config->domain().reset();
    }

    if (config->attempts().isNull()) {
        config->attempts() =
            NTCCFG_WARNING_NARROW(unsigned int, config->nameServer().size());
    }

    if (config->attempts().value() > k_MAX_ATTEMPTS) {
        config->attempts() =
            NTCCFG_WARNING_NARROW(unsigned int, k_MAX_ATTEMPTS);
    }

    if (config->attempts().value() > config->nameServer().size()) {
        config->attempts() =
            NTCCFG_WARNING_NARROW(unsigned int, config->nameServer().size());
    }

    if (config->timeout().isNull()) {
        config->timeout() =
            NTCCFG_WARNING_NARROW(unsigned int, k_DEFAULT_TIMEOUT);
    }

    if (config->timeout().value() > k_MAX_TIMEOUT) {
        config->timeout() = NTCCFG_WARNING_NARROW(unsigned int, k_MAX_TIMEOUT);
    }

    if (config->rotate().isNull()) {
        config->rotate() = k_DEFAULT_ROTATE;
    }

    if (config->ndots().isNull()) {
        config->ndots() = NTCCFG_WARNING_NARROW(unsigned int, k_DEFAULT_NDOTS);
    }

    if (config->ndots().value() > k_MAX_NDOTS) {
        config->ndots() = NTCCFG_WARNING_NARROW(unsigned int, k_MAX_NDOTS);
    }

    if (config->debug().isNull()) {
        config->debug() = k_DEFAULT_DEBUG;
    }
}

}  // close unnamed namespace

File::File(bslma::Allocator* basicAllocator)
: d_data("")
, d_size(0)
, d_path(basicAllocator)
, d_foreign(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

File::~File()
{
    this->close();
}

ntsa::Error File::import(const char* data, bsl::size_t size)
{
    if (size > 0) {
        char* memory = (char*)(d_allocator_p->allocate(size));
        bsl::memcpy(memory, data, size);

        d_data    = memory;
        d_size    = size;
        d_foreign = true;
    }
    else {
        d_data    = "";
        d_size    = 0;
        d_foreign = false;
    }

    return ntsa::Error();
}

ntsa::Error File::load(const bslstl::StringRef& path)
{
#if NTCDNS_UTILITY_MEMORY_MAP_FILES

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    bsls::Stopwatch stopwatch;
    stopwatch.start();

    bdls::FilesystemUtil::FileDescriptor file =
        bdls::FilesystemUtil::open(path,
                                   bdls::FilesystemUtil::e_OPEN,
                                   bdls::FilesystemUtil::e_READ_ONLY);

    if (file == bdls::FilesystemUtil::k_INVALID_FD) {
        error = ntsa::Error::last();
        NTCI_LOG_STREAM_ERROR << "Failed to open '" << path << "': " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    bdls::FileDescriptorGuard guard(file);

#if defined(BSLS_PLATFORM_OS_UNIX)
    bdls::FilesystemUtil::Offset fileSize =
        bdls::FilesystemUtil::getFileSize(file);
#else
    bdls::FilesystemUtil::Offset fileSize =
        bdls::FilesystemUtil::getFileSize(path);
#endif

    if (fileSize < 0) {
        error = ntsa::Error::last();
        NTCI_LOG_STREAM_ERROR << "Failed to get size of '" << path
                              << "': " << error << NTCI_LOG_STREAM_END;
        return error;
    }

    if (fileSize > bsl::numeric_limits<int>::max()) {
        error = ntsa::Error::last();
        NTCI_LOG_STREAM_ERROR << "Failed to load '" << path
                              << "': the file size " << fileSize
                              << " is too large" << NTCI_LOG_STREAM_END;
        return error;
    }

    if (fileSize > 0) {
        void* address = 0;
        rc            = bdls::FilesystemUtil::map(file,
                                       &address,
                                       0,
                                       fileSize,
                                       bdls::MemoryUtil::k_ACCESS_READ);
        if (rc != 0) {
            error = ntsa::Error::last();
            NTCI_LOG_STREAM_ERROR << "Failed to map '" << path
                                  << "': " << error << NTCI_LOG_STREAM_END;
            return error;
        }

        stopwatch.stop();

        NTCI_LOG_STREAM_DEBUG
            << "Mapped " << fileSize << " bytes from '" << path << "' in "
            << bsls::TimeInterval(stopwatch.elapsedTime()).totalMicroseconds()
            << " microseconds" << NTCI_LOG_STREAM_END;

        d_data = static_cast<const char*>(address);
        d_size = static_cast<bsl::size_t>(fileSize);
    }
    else {
        d_data = "";
        d_size = 0;
    }

    d_path = path;

    return ntsa::Error();

#else

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsls::Stopwatch stopwatch;
    stopwatch.start();

    bdls::FilesystemUtil::FileDescriptor file =
        bdls::FilesystemUtil::open(path,
                                   bdls::FilesystemUtil::e_OPEN,
                                   bdls::FilesystemUtil::e_READ_ONLY);

    if (file == bdls::FilesystemUtil::k_INVALID_FD) {
        error = ntsa::Error::last();
        NTCI_LOG_STREAM_ERROR << "Failed to open '" << path << "': " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    bdls::FileDescriptorGuard    guard(file);

#if defined(BSLS_PLATFORM_OS_UNIX)
    bdls::FilesystemUtil::Offset fileSize =
        bdls::FilesystemUtil::getFileSize(file);
#else
    bdls::FilesystemUtil::Offset fileSize =
        bdls::FilesystemUtil::getFileSize(path);
#endif

    if (fileSize < 0) {
        error = ntsa::Error::last();
        NTCI_LOG_STREAM_ERROR << "Failed to get size of '" << path
                              << "': " << error << NTCI_LOG_STREAM_END;
        return error;
    }

    if (fileSize > bsl::numeric_limits<int>::max()) {
        error = ntsa::Error::last();
        NTCI_LOG_STREAM_ERROR << "Failed to load '" << path
                              << "': the file size " << fileSize
                              << " is too large" << NTCI_LOG_STREAM_END;
        return error;
    }

    if (fileSize > 0) {
        char* data = (char*)(d_allocator_p->allocate(
            NTCCFG_WARNING_NARROW(bsl::size_t, fileSize)));

        bslma::DeallocatorProctor<bslma::Allocator> dataGuard(data,
                                                              d_allocator_p);

        {
            char* position      = data;
            int   numBytesLeft  = NTCCFG_WARNING_NARROW(int, fileSize);
            int   numBytesTotal = 0;

            while (true) {
                int numBytesRead =
                    bdls::FilesystemUtil::read(file, position, numBytesLeft);
                if (numBytesRead < 0) {
                    error = ntsa::Error::last();

#if defined(BSLS_PLATFORM_OS_UNIX)
                    if (error.number() == EINTR) {
                        continue;
                    }
#endif

                    NTCI_LOG_STREAM_ERROR << "Failed to read '" << path
                                          << "': " << error
                                          << NTCI_LOG_STREAM_END;
                    return error;
                }

                if (numBytesRead == 0) {
                    break;
                }

                BSLS_ASSERT_OPT(numBytesRead <= numBytesLeft);

                numBytesLeft  -= numBytesRead;
                numBytesTotal += numBytesRead;
                position      += numBytesRead;

                if (numBytesLeft == 0) {
                    break;
                }
            }

            fileSize = numBytesTotal;
        }

        stopwatch.stop();

        NTCI_LOG_STREAM_DEBUG
            << "Read " << fileSize << " bytes from '" << path << "' in "
            << bsls::TimeInterval(stopwatch.elapsedTime()).totalMicroseconds()
            << " microseconds" << NTCI_LOG_STREAM_END;

        dataGuard.release();

        d_data = data;
        d_size = static_cast<bsl::size_t>(fileSize);
    }
    else {
        d_data = "";
        d_size = 0;
    }

    d_path = path;

    return ntsa::Error();

#endif
}

ntsa::Error File::close()
{
#if NTCDNS_UTILITY_MEMORY_MAP_FILES

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    if (d_foreign) {
        d_allocator_p->deallocate(const_cast<char*>(d_data));
    }
    else if (d_size > 0) {
        void*                        address = (void*)(d_data);
        bdls::FilesystemUtil::Offset fileSize =
            (bdls::FilesystemUtil::Offset)(d_size);

        rc = bdls::FilesystemUtil::unmap(address, fileSize);
        if (rc != 0) {
            error = ntsa::Error::last();
            NTCI_LOG_STREAM_ERROR << "Failed to unmap '" << d_path
                                  << "': " << error << NTCI_LOG_STREAM_END;

            d_path.clear();
            d_data = 0;
            d_size = 0;

            return error;
        }
    }

    d_path.clear();
    d_data = 0;
    d_size = 0;

    return ntsa::Error();

#else

    if (d_size > 0) {
        d_allocator_p->deallocate(const_cast<char*>(d_data));
    }

    d_path.clear();
    d_data = 0;
    d_size = 0;

    return ntsa::Error();

#endif
}

const bsl::string& File::path() const
{
    return d_path;
}

const char* File::data() const
{
    return d_data;
}

bsl::size_t File::size() const
{
    return d_size;
}

ntsa::Error Utility::loadResolverConfig(ntcdns::ResolverConfig* result)
{
    ntsa::Error error;

    if (result->client().isNull()) {
        error = loadClientConfig(
            &result->client().makeValue().makeConfiguration());
        if (error) {
            return error;
        }
    }
    else {
        if (result->client().value().isPathValue()) {
            bsl::string path = result->client().value().path();
            error            = loadClientConfigFromPath(
                &result->client().value().makeConfiguration(),
                path);
            if (error) {
                return error;
            }
        }
        else if (result->client().value().isUndefinedValue()) {
            error = loadClientConfig(
                &result->client().value().makeConfiguration());
            if (error) {
                return error;
            }
        }
    }

    BSLS_ASSERT(!result->client().isNull());
    BSLS_ASSERT(result->client().value().isConfigurationValue());

    sanitizeClientConfig(&result->client().value().configuration());

    if (result->hostDatabase().isNull()) {
        error = loadHostDatabaseConfig(
            &result->hostDatabase().makeValue().makeConfiguration());
        if (error) {
            return error;
        }
    }
    else {
        if (result->hostDatabase().value().isPathValue()) {
            bsl::string path = result->hostDatabase().value().path();
            error            = loadHostDatabaseConfigFromPath(
                &result->hostDatabase().value().makeConfiguration(),
                path);
            if (error) {
                return error;
            }
        }
        else if (result->hostDatabase().value().isUndefinedValue()) {
            error = loadHostDatabaseConfig(
                &result->hostDatabase().value().makeConfiguration());
            if (error) {
                return error;
            }
        }
    }

    BSLS_ASSERT(!result->hostDatabase().isNull());
    BSLS_ASSERT(result->hostDatabase().value().isConfigurationValue());

    if (result->portDatabase().isNull()) {
        error = loadPortDatabaseConfig(
            &result->portDatabase().makeValue().makeConfiguration());
        if (error) {
            return error;
        }
    }
    else {
        if (result->portDatabase().value().isPathValue()) {
            bsl::string path = result->portDatabase().value().path();
            error            = loadPortDatabaseConfigFromPath(
                &result->portDatabase().value().makeConfiguration(),
                path);
            if (error) {
                return error;
            }
        }
        else if (result->portDatabase().value().isUndefinedValue()) {
            error = loadPortDatabaseConfig(
                &result->portDatabase().value().makeConfiguration());
            if (error) {
                return error;
            }
        }
    }

    BSLS_ASSERT(!result->portDatabase().isNull());
    BSLS_ASSERT(result->portDatabase().value().isConfigurationValue());

    return ntsa::Error();
}

ntsa::Error Utility::loadClientConfig(ntcdns::ClientConfig* result)
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    return loadClientConfigFromPath(result, "/etc/resolv.conf");

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    ntsa::Error error = loadNameServers(&result->nameServer());
    if (error) {
        return error;
    }

    sanitizeClientConfig(result);

    return ntsa::Error();

#else
#error Not implemented
#endif
}

ntsa::Error Utility::loadClientConfigFromPath(ntcdns::ClientConfig* result,
                                              const bsl::string&    path)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntcdns::File file;
    error = file.load(path);
    if (error) {
        return error;
    }

    if (file.size() == 0) {
        return ntsa::Error();
    }

    error = loadClientConfigFromText(result, file.data(), file.size());
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse '" << path << "': " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Utility::loadClientConfigFromText(ntcdns::ClientConfig* result,
                                              const char*           data,
                                              bsl::size_t           size)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    result->reset();

    bsls::Stopwatch stopwatch;
    stopwatch.start();

    bslstl::StringRef text(data, size);
    bdlb::Tokenizer   tokenizer(text, "\r\n", "");

    while (tokenizer.isValid()) {
        bslstl::StringRef line = tokenizer.token();
        error                  = parseClientConfigLine(result, line);
        if (error) {
            return error;
        }

        ++tokenizer;
    }

    stopwatch.stop();

    NTCI_LOG_STREAM_DEBUG
        << "Parsed and loaded client configuration in "
        << bsls::TimeInterval(stopwatch.elapsedTime()).totalMilliseconds()
        << " milliseconds" << NTCI_LOG_STREAM_END;

    sanitizeClientConfig(result);

    return ntsa::Error();
}

ntsa::Error Utility::loadHostDatabaseConfig(ntcdns::HostDatabaseConfig* result)
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    return loadHostDatabaseConfigFromPath(result, "/etc/hosts");

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    return loadHostDatabaseConfigFromPath(
        result,
        "C:\\Windows\\System32\\drivers\\etc\\hosts");

#else
#error Not implemented
#endif
}

ntsa::Error Utility::loadHostDatabaseConfigFromPath(
    ntcdns::HostDatabaseConfig* result,
    const bsl::string&          path)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntcdns::File file;
    error = file.load(path);
    if (error) {
        return error;
    }

    if (file.size() == 0) {
        return ntsa::Error();
    }

    error = loadHostDatabaseConfigFromText(result, file.data(), file.size());
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse '" << path << "': " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Utility::loadHostDatabaseConfigFromText(
    ntcdns::HostDatabaseConfig* result,
    const char*                 data,
    bsl::size_t                 size)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    result->reset();
    result->entry().reserve(256);

    bsls::Stopwatch stopwatch;
    stopwatch.start();

    bslstl::StringRef text(data, size);
    bdlb::Tokenizer   tokenizer(text, "\r\n", "");

    while (tokenizer.isValid()) {
        bslstl::StringRef line = tokenizer.token();
        error                  = parseHostLine(result, line);
        if (error) {
            return error;
        }

        ++tokenizer;
    }

    stopwatch.stop();

    NTCI_LOG_STREAM_DEBUG
        << "Parsed and loaded host database in "
        << bsls::TimeInterval(stopwatch.elapsedTime()).totalMilliseconds()
        << " milliseconds" << NTCI_LOG_STREAM_END;

    return ntsa::Error();
}

ntsa::Error Utility::loadPortDatabaseConfig(ntcdns::PortDatabaseConfig* result)
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    return loadPortDatabaseConfigFromPath(result, "/etc/services");

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    return loadPortDatabaseConfigFromPath(
        result,
        "C:\\Windows\\System32\\drivers\\etc\\services");

#else
#error Not implemented
#endif
}

ntsa::Error Utility::loadPortDatabaseConfigFromPath(
    ntcdns::PortDatabaseConfig* result,
    const bsl::string&          path)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntcdns::File file;
    error = file.load(path);
    if (error) {
        return error;
    }

    if (file.size() == 0) {
        return ntsa::Error();
    }

    error = loadPortDatabaseConfigFromText(result, file.data(), file.size());
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse '" << path << "': " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Utility::loadPortDatabaseConfigFromText(
    ntcdns::PortDatabaseConfig* result,
    const char*                 data,
    bsl::size_t                 size)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    result->reset();
    result->entry().reserve(256);

    bsls::Stopwatch stopwatch;
    stopwatch.start();

    bslstl::StringRef text(data, size);
    bdlb::Tokenizer   tokenizer(text, "\r\n", "");

    while (tokenizer.isValid()) {
        bslstl::StringRef line = tokenizer.token();
        error                  = parsePortLine(result, line);
        if (error) {
            return error;
        }

        ++tokenizer;
    }

    stopwatch.stop();

    NTCI_LOG_STREAM_DEBUG
        << "Parsed and loaded port database in "
        << bsls::TimeInterval(stopwatch.elapsedTime()).totalMilliseconds()
        << " milliseconds" << NTCI_LOG_STREAM_END;

    return ntsa::Error();
}

void Utility::sanitize(ntcdns::ResolverConfig* config)
{
    if (config->client().isNull()) {
        config->client().makeValue().makeConfiguration();
    }

    sanitizeClientConfig(&config->client().value().configuration());

    if (config->hostDatabase().isNull()) {
        config->hostDatabase().makeValue().makeConfiguration();
    }

    if (config->portDatabase().isNull()) {
        config->portDatabase().makeValue().makeConfiguration();
    }
}

}  // close package namespace
}  // close enterprise namespace
