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

#include <ntcdns_compat.h>
#include <ntcdns_utility.h>
#include <ntci_log.h>
#include <ntsa_host.h>
#include <ntsu_resolverutil.h>
#include <bdlb_chartype.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>

// Uncomment or set to 0 to print various diagnostics to standard output
// while debugging.
// #define NTCDNS_DATABASE_DEBUG_COUT 1

namespace BloombergLP {
namespace ntcdns {

namespace {

/// Provide a functor to sort port entries by port.
struct PortEntrySorter {
    bool operator()(const ntcdns::PortEntry& lhs,
                    const ntcdns::PortEntry& rhs) const
    {
        return lhs.port() < rhs.port();
    }
};

/// Provide a scanner of contiguous character data.
class Scanner
{
    const char* d_begin;
    const char* d_current;
    const char* d_end;
    bool        d_eof;

  private:
    Scanner(const Scanner&) BSLS_KEYWORD_DELETED;
    Scanner& operator=(const Scanner&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new scanner of the specified 'data' having the specified
    /// 'size'.
    Scanner(const char* data, bsl::size_t size);

    /// Destroy this object.
    ~Scanner();

    /// Advance to the next character that is neither a space nor tab
    /// nor carriage-return nor new-line chacter.
    char skipUntilNotWhitespace();

    /// Advance to the next character that is neither a space nor tab
    /// character.
    char skipUntilNotSeparator();

    /// Advance to the next character that is neither a carriage-return nor
    /// new-line character
    char skipUntilNotNewLine();

    /// Advance to the next character that is the beginning of a new
    /// line.
    char skipLine();

    /// Advance to the next character that is not a valid character in an
    /// IPv4 or IPv6 address.
    char skipIpAddress();

    /// Advance to the next character that is not a valid character in an
    /// IPv4 or IPv6 address.
    char skipPort();

    /// Advance to the next character that is not a valid character in a
    /// domain name.
    char skipDomainName();

    /// Advance to the next character that is not a valid character in a
    /// service name.
    char skipServiceName();

    /// Advance to the next character that is not a valid character in a
    /// transport protocol.
    char skipProtocol();

    /// Advance to the next character that is not a slash.
    char skipSlash();

    /// Return the pointer to the start of the data.
    const char* begin() const;

    /// Return the pointer to the current character.
    const char* current() const;

    /// Return the pointer to the end of the data.
    const char* end() const;

    /// Return the pointer to the marked character, or null if no such mark
    /// has been placed.
    const char* mark() const;

    /// Return true if the pointer to the current character is at the end
    /// of the data.
    bool isEof() const;

    /// Return true if the specified 'current' character is the
    /// carriage-return or new-line character, otherwise return false.
    bool isNewLine(char current) const;

    /// Return true if the specified 'current' character is the comment
    /// character, otherwise return false.
    bool isComment(char current) const;

    /// Return true if the specified 'current' character is the slash
    /// character, otherwise return false.
    bool isSlash(char current) const;
};

NTCCFG_INLINE
Scanner::Scanner(const char* data, bsl::size_t size)
: d_begin(data)
, d_current(data)
, d_end(data + size)
, d_eof(d_begin == d_end)
{
}

NTCCFG_INLINE
Scanner::~Scanner()
{
}

NTCCFG_INLINE
char Scanner::skipUntilNotWhitespace()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

#if 1

        if (bdlb::CharType::isSpace(current)) {
            ++d_current;
            continue;
        }

#else

        if (current == ' ' || current == '\t' || current == '\r' ||
            current == '\n')
        {
            ++d_current;
            continue;
        }

#endif

        return current;
    }

    return 0;
}

NTCCFG_INLINE
char Scanner::skipUntilNotSeparator()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

        if (current == ' ' || current == '\t') {
            ++d_current;
            continue;
        }

        return current;
    }

    return 0;
}

char Scanner::skipUntilNotNewLine()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

        if (current == '\r' || current == '\n') {
            ++d_current;
            continue;
        }

        return current;
    }

    return 0;
}

NTCCFG_INLINE
char Scanner::skipLine()
{
    char current = 0;

    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        current = *d_current;

        if (NTCCFG_LIKELY(!(current == '\r' || current == '\n'))) {
            ++d_current;
            continue;
        }

        break;
    }

    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        current = *d_current;

        if (current == '\r' || current == '\n') {
            ++d_current;
            continue;
        }

        break;
    }

    return current;
}

NTCCFG_INLINE
char Scanner::skipIpAddress()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

#if 1
        if (NTCCFG_LIKELY(bdlb::CharType::isAlnum(current))) {
            ++d_current;
            continue;
        }

        if (NTCCFG_LIKELY(current == '.')) {
            ++d_current;
            continue;
        }

        if (NTCCFG_LIKELY(current == ':')) {
            ++d_current;
            continue;
        }
#else
        if (NTCCFG_LIKELY(current != ' ' && current != '\t' &&
                          current != '\r' && current != '\n' &&
                          current != '#'))
        {
            ++d_current;
            continue;
        }
#endif

        return current;
    }

    return 0;
}

NTCCFG_INLINE
char Scanner::skipPort()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

        if (NTCCFG_LIKELY(current >= '0' && current <= '9')) {
            ++d_current;
            continue;
        }

        return current;
    }

    return 0;
}

NTCCFG_INLINE
char Scanner::skipDomainName()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

#if 1

        if (NTCCFG_LIKELY(bdlb::CharType::isGraph(current))) {
            ++d_current;
            continue;
        }

#else
        if (NTCCFG_LIKELY(current != ' ' && current != '\t' &&
                          current != '\r' && current != '\n' &&
                          current != '#'))
        {
            ++d_current;
            continue;
        }
#endif

        return current;
    }

    return 0;
}

NTCCFG_INLINE
char Scanner::skipServiceName()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

        if (NTCCFG_LIKELY(current != ' ' && current != '\t' &&
                          current != '\r' && current != '\n' &&
                          current != '#'))
        {
            ++d_current;
            continue;
        }

        return current;
    }

    return 0;
}

NTCCFG_INLINE
char Scanner::skipProtocol()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

        if (NTCCFG_LIKELY(current != ' ' && current != '\t' &&
                          current != '\r' && current != '\n' &&
                          current != '#'))
        {
            ++d_current;
            continue;
        }

        return current;
    }

    return 0;
}

NTCCFG_INLINE
char Scanner::skipSlash()
{
    while (!d_eof) {
        if (NTCCFG_UNLIKELY(d_current == d_end)) {
            d_eof = true;
            return 0;
        }

        const char current = *d_current;

        if (NTCCFG_LIKELY(current == '/')) {
            ++d_current;
            continue;
        }

        return current;
    }

    return 0;
}

NTCCFG_INLINE
const char* Scanner::begin() const
{
    return d_begin;
}

NTCCFG_INLINE
const char* Scanner::current() const
{
    return d_current;
}

NTCCFG_INLINE
const char* Scanner::end() const
{
    return d_end;
}

NTCCFG_INLINE
bool Scanner::isEof() const
{
    return d_eof;
}

NTCCFG_INLINE
bool Scanner::isNewLine(char current) const
{
    return current == '\r' || current == '\n';
}

NTCCFG_INLINE
bool Scanner::isComment(char current) const
{
    return current == '#';
}

NTCCFG_INLINE
bool Scanner::isSlash(char current) const
{
    return current == '/';
}

}  // close unnamed namespace

bsl::size_t HostDatabaseUtil::hashIpv6(const ntsa::Ipv6Address& ipv6Address)
{
    bsl::size_t result = 0;

    const char* current = (const char*)(&ipv6Address);
    const char* end     = current + 16;

    while (current != end) {
        result = ((result << 5) + result) ^ (bsl::size_t)(*current++);
    }

    return result;
}

ntsa::Error HostDatabase::load(const bsl::shared_ptr<ntcdns::File>& file)
{
    bsls::Stopwatch stopwatch;
    stopwatch.start();

    Scanner scanner(file->data(), file->size());

    IpAddressByDomainName ipAddressByDomainName(d_allocator_p);
    DomainNameByIpAddress domainNameByIpAddress(d_allocator_p);

    if (file->size() > 1024 * 1024) {
        ipAddressByDomainName.reserve(1024 * 1024);
        domainNameByIpAddress.reserve(1024 * 1024);
    }

    char        current = 0;
    bsl::size_t lines   = 0;
    NTCCFG_WARNING_UNUSED(lines);

    while (true) {
        current = scanner.skipUntilNotWhitespace();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        if (NTCCFG_UNLIKELY(scanner.isComment(current))) {
            current = scanner.skipLine();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            continue;
        }

        const char* const ipAddressBegin = scanner.current();

        current = scanner.skipIpAddress();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        const char* const ipAddressEnd = scanner.current();

        bslstl::StringRef ipAddressStringRef(ipAddressBegin, ipAddressEnd);

        ntsa::IpAddress ipAddress;
        if (!ipAddress.parse(ipAddressStringRef)) {
            current = scanner.skipLine();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            continue;
        }

        while (true) {
            current = scanner.skipUntilNotSeparator();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            if (NTCCFG_UNLIKELY(scanner.isComment(current))) {
                current = scanner.skipLine();
                break;
            }

            if (scanner.isNewLine(current)) {
                current = scanner.skipUntilNotNewLine();
                break;
            }

            const char* const domainNameBegin = scanner.current();

            current = scanner.skipDomainName();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            const char* const domainNameEnd = scanner.current();

            bslstl::StringRef domainNameStringRef(domainNameBegin,
                                                  domainNameEnd);

            IpAddressArray& ipAddressArray =
                ipAddressByDomainName[domainNameStringRef];
            if (bsl::find(ipAddressArray.begin(),
                          ipAddressArray.end(),
                          ipAddress) == ipAddressArray.end())
            {
                ipAddressArray.push_back(ipAddress);
            }

            bslstl::StringRef& domainName = domainNameByIpAddress[ipAddress];
            if (domainName.empty()) {
                domainName = domainNameStringRef;
            }
        }

        ++lines;
    }

    stopwatch.stop();

#if NTCDNS_DATABASE_DEBUG_COUT
    bsl::cout
        << "Scanned " << file->size() << " bytes (" << lines << " lines, "
        << domainNameByIpAddress.size() << " IP addresses) from '"
        << file->path() << "' in "
        << bsls::TimeInterval(stopwatch.elapsedTime()).totalMilliseconds()
        << " milliseconds" << bsl::endl;
#endif

    {
        LockGuard lock(&d_mutex);

        d_ipAddressByDomainName.swap(ipAddressByDomainName);
        d_domainNameByIpAddress.swap(domainNameByIpAddress);
        d_file_sp = file;
    }

    return ntsa::Error();
}

HostDatabase::HostDatabase(bslma::Allocator* basicAllocator)
: d_mutex()
, d_pool(8, basicAllocator)
, d_ipAddressByDomainName(&d_pool)
, d_domainNameByIpAddress(&d_pool)
, d_file_sp()
, d_allocator_p(&d_pool)
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
    d_file_sp.reset();
}

ntsa::Error HostDatabase::load()
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    return this->loadPath("/etc/hosts");

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    return this->loadPath("C:\\Windows\\System32\\drivers\\etc\\hosts");

#else
#error Not implemented
#endif
}

ntsa::Error HostDatabase::loadPath(const bslstl::StringRef& path)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntcdns::File> file;
    file.createInplace(d_allocator_p, d_allocator_p);

    error = file->load(path);
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to load host database '" << path
                              << "': " << error << NTCI_LOG_STREAM_END;
        return error;
    }

    error = this->load(file);
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse host database '" << path
                              << "': " << error << NTCI_LOG_STREAM_END;
        return error;
    }

    return ntsa::Error();
}

ntsa::Error HostDatabase::loadText(const char* data, bsl::size_t size)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntcdns::File> file;
    file.createInplace(d_allocator_p, d_allocator_p);

    error = file->import(data, size);
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to import host database: " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    error = this->load(file);
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse host database: " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    return ntsa::Error();
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
            ipAddressList.assign(it->second.begin(), it->second.end());
        }
        else {
            const IpAddressArray& target = it->second;

            for (IpAddressArray::const_iterator jt = target.begin();
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

    if (ipAddressType.isNull()) {
        ntsu::ResolverUtil::sortIpAddressList(&ipAddressList);
    }

    if (ipAddressList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    context->setDomainName(domainName);
    context->setSource(ntca::ResolverSource::e_DATABASE);

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
    NTCCFG_WARNING_UNUSED(options);

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
    context->setSource(ntca::ResolverSource::e_DATABASE);

    return ntsa::Error();
}

ntsa::Error PortDatabase::load(const bsl::shared_ptr<ntcdns::File>& file)
{
    ntsa::Error error;

    bsls::Stopwatch stopwatch;
    stopwatch.start();

    Scanner scanner(file->data(), file->size());

    PortByServiceName tcpPortByServiceName(d_allocator_p);
    ServiceNameByPort tcpServiceNameByPort(d_allocator_p);
    PortByServiceName udpPortByServiceName(d_allocator_p);
    ServiceNameByPort udpServiceNameByPort(d_allocator_p);

    if (file->size() >= 1024 * 1024) {
        tcpPortByServiceName.reserve(1024);
        tcpServiceNameByPort.reserve(1024);

        udpPortByServiceName.reserve(1024);
        udpServiceNameByPort.reserve(1024);
    }

    char        current = 0;
    bsl::size_t lines   = 0;
    NTCCFG_WARNING_UNUSED(lines);

    while (true) {
        current = scanner.skipUntilNotWhitespace();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        if (NTCCFG_UNLIKELY(scanner.isComment(current))) {
            current = scanner.skipLine();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            continue;
        }

        const char* const serviceNameBegin = scanner.current();

        current = scanner.skipServiceName();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        const char* const serviceNameEnd = scanner.current();

        bslstl::StringRef serviceNameStringRef(serviceNameBegin,
                                               serviceNameEnd);

        // Scan <port>.

        current = scanner.skipUntilNotSeparator();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        if (NTCCFG_UNLIKELY(scanner.isComment(current))) {
            current = scanner.skipLine();
            continue;
        }

        if (scanner.isNewLine(current)) {
            current = scanner.skipUntilNotNewLine();
            continue;
        }

        const char* const portBegin = scanner.current();

        current = scanner.skipPort();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        const char* const portEnd = scanner.current();

        ntsa::Port port;
        if (!ntsa::PortUtil::parse(&port, portBegin, portEnd - portBegin)) {
            current = scanner.skipLine();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            continue;
        }

        // Scan '/'.

        current = scanner.skipUntilNotSeparator();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        if (NTCCFG_UNLIKELY(scanner.isComment(current))) {
            current = scanner.skipLine();
            continue;
        }

        if (scanner.isNewLine(current)) {
            current = scanner.skipUntilNotNewLine();
            continue;
        }

        if (!scanner.isSlash(current)) {
            current = scanner.skipLine();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            continue;
        }

        scanner.skipSlash();

        // Scan <protocol>.

        current = scanner.skipUntilNotSeparator();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        if (NTCCFG_UNLIKELY(scanner.isComment(current))) {
            current = scanner.skipLine();
            continue;
        }

        if (scanner.isNewLine(current)) {
            current = scanner.skipUntilNotNewLine();
            continue;
        }

        const char* const protocolBegin = scanner.current();

        current = scanner.skipProtocol();
        if (NTCCFG_UNLIKELY(current == 0)) {
            break;
        }

        const char* const protocolEnd = scanner.current();

        bslstl::StringRef protocolStringRef(protocolBegin, protocolEnd);

        enum ProtocolType { e_TCP = 0, e_UDP = 1 };

        ProtocolType protocolType;
        if (protocolStringRef == "tcp") {
            protocolType = e_TCP;
        }
        else if (protocolStringRef == "udp") {
            protocolType = e_UDP;
        }
        else {
            current = scanner.skipLine();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            continue;
        }

        if (protocolType == e_TCP) {
            PortArray& portArray = tcpPortByServiceName[serviceNameStringRef];
            if (bsl::find(portArray.begin(), portArray.end(), port) ==
                portArray.end())
            {
#if NTCDNS_DATABASE_FIXED_CAPACITY_ARRAYS
                error = portArray.push_back(port);
                if (error) {
#if NTCDNS_DATABASE_DEBUG_COUT
                    bsl::cout << "Unable to associate service name '"
                              << serviceNameStringRef << "' with TCP port "
                              << port << ": max capacity" << bsl::endl;
#endif
                }
#else
                portArray.push_back(port);
#endif
            }

            bslstl::StringRef& serviceName = tcpServiceNameByPort[port];
            if (serviceName.empty()) {
                serviceName = serviceNameStringRef;
            }
        }
        else if (protocolType == e_UDP) {
            PortArray& portArray = udpPortByServiceName[serviceNameStringRef];
            if (bsl::find(portArray.begin(), portArray.end(), port) ==
                portArray.end())
            {
#if NTCDNS_DATABASE_FIXED_CAPACITY_ARRAYS
                error = portArray.push_back(port);
                if (error) {
#if NTCDNS_DATABASE_DEBUG_COUT
                    bsl::cout << "Unable to associate service name '"
                              << serviceNameStringRef << "' with UDP port "
                              << port << ": max capacity" << bsl::endl;
#endif
                }
#else
                portArray.push_back(port);
#endif
            }

            bslstl::StringRef& serviceName = udpServiceNameByPort[port];
            if (serviceName.empty()) {
                serviceName = serviceNameStringRef;
            }
        }

        // Scan <service-name-alias>.

        while (true) {
            current = scanner.skipUntilNotSeparator();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            if (NTCCFG_UNLIKELY(scanner.isComment(current))) {
                current = scanner.skipLine();
                break;
            }

            if (scanner.isNewLine(current)) {
                current = scanner.skipUntilNotNewLine();
                break;
            }

            const char* const serviceNameAliasBegin = scanner.current();

            current = scanner.skipServiceName();
            if (NTCCFG_UNLIKELY(current == 0)) {
                break;
            }

            const char* const serviceNameAliasEnd = scanner.current();

            bslstl::StringRef serviceNameAliasStringRef(serviceNameAliasBegin,
                                                        serviceNameAliasEnd);

            if (protocolType == e_TCP) {
                PortArray& portArray =
                    tcpPortByServiceName[serviceNameAliasStringRef];
                if (bsl::find(portArray.begin(), portArray.end(), port) ==
                    portArray.end())
                {
#if NTCDNS_DATABASE_FIXED_CAPACITY_ARRAYS
                    error = portArray.push_back(port);
                    if (error) {
#if NTCDNS_DATABASE_DEBUG_COUT
                        bsl::cout << "Unable to associate service name alias '"
                                  << serviceNameAliasStringRef
                                  << "' with TCP port " << port
                                  << ": max capacity" << bsl::endl;
#endif
                    }
#else
                    portArray.push_back(port);
#endif
                }

                bslstl::StringRef& serviceName = tcpServiceNameByPort[port];
                if (serviceName.empty()) {
                    serviceName = serviceNameAliasStringRef;
                }
            }
            else if (protocolType == e_UDP) {
                PortArray& portArray =
                    udpPortByServiceName[serviceNameAliasStringRef];
                if (bsl::find(portArray.begin(), portArray.end(), port) ==
                    portArray.end())
                {
#if NTCDNS_DATABASE_FIXED_CAPACITY_ARRAYS
                    error = portArray.push_back(port);
                    if (error) {
#if NTCDNS_DATABASE_DEBUG_COUT
                        bsl::cout << "Unable to associate service name alias '"
                                  << serviceNameAliasStringRef
                                  << "' with UDP port " << port
                                  << ": max capacity" << bsl::endl;
#endif
                    }
#else
                    portArray.push_back(port);
#endif
                }

                bslstl::StringRef& serviceName = udpServiceNameByPort[port];
                if (serviceName.empty()) {
                    serviceName = serviceNameAliasStringRef;
                }
            }
        }

        ++lines;
    }

    stopwatch.stop();

#if NTCDNS_DATABASE_DEBUG_COUT
    bsl::cout
        << "Scanned " << file->size() << " bytes (" << lines << " lines, "
        << tcpPortByServiceName.size() << " IP addresses) from '"
        << file->path() << "' in "
        << bsls::TimeInterval(stopwatch.elapsedTime()).totalMilliseconds()
        << " milliseconds" << bsl::endl;
#endif

    {
        LockGuard lock(&d_mutex);

        d_tcpPortByServiceName.swap(tcpPortByServiceName);
        d_tcpServiceNameByPort.swap(tcpServiceNameByPort);

        d_udpPortByServiceName.swap(udpPortByServiceName);
        d_udpServiceNameByPort.swap(udpServiceNameByPort);

        d_file_sp = file;
    }

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

    d_file_sp.reset();
}

ntsa::Error PortDatabase::load()
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    return this->loadPath("/etc/services");

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    return this->loadPath("C:\\Windows\\System32\\drivers\\etc\\services");

#else
#error Not implemented
#endif
}

ntsa::Error PortDatabase::loadPath(const bslstl::StringRef& path)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntcdns::File> file;
    file.createInplace(d_allocator_p, d_allocator_p);

    error = file->load(path);
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to load port database '" << path
                              << "': " << error << NTCI_LOG_STREAM_END;
        return error;
    }

    error = this->load(file);
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse port database '" << path
                              << "': " << error << NTCI_LOG_STREAM_END;
        return error;
    }

    return ntsa::Error();
}

ntsa::Error PortDatabase::loadText(const char* data, bsl::size_t size)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntcdns::File> file;
    file.createInplace(d_allocator_p, d_allocator_p);

    error = file->import(data, size);
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to import port database: " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    error = this->load(file);
    if (error) {
        NTCI_LOG_STREAM_ERROR << "Failed to parse port database: " << error
                              << NTCI_LOG_STREAM_END;
        return error;
    }

    return ntsa::Error();
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
                    const PortArray& source = it->second;
                    for (PortArray::const_iterator jt = source.begin();
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
                    const PortArray& source = it->second;
                    for (PortArray::const_iterator jt = source.begin();
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
    context->setSource(ntca::ResolverSource::e_DATABASE);

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
    context->setSource(ntca::ResolverSource::e_DATABASE);

    return ntsa::Error();
}

void PortDatabase::dump(bsl::vector<ntcdns::PortEntry>* result) const
{
    result->clear();

    LockGuard lock(&d_mutex);

    result->reserve(d_tcpServiceNameByPort.size() +
                    d_udpServiceNameByPort.size());

    if (!d_tcpServiceNameByPort.empty()) {
        for (ServiceNameByPort::const_iterator it =
                 d_tcpServiceNameByPort.begin();
             it != d_tcpServiceNameByPort.end();
             ++it)
        {
            ntcdns::PortEntry portEntry;
            portEntry.service()  = it->second;
            portEntry.port()     = it->first;
            portEntry.protocol() = "tcp";

            result->push_back(portEntry);
        }
    }

    if (!d_udpServiceNameByPort.empty()) {
        for (ServiceNameByPort::const_iterator it =
                 d_udpServiceNameByPort.begin();
             it != d_udpServiceNameByPort.end();
             ++it)
        {
            ntcdns::PortEntry portEntry;
            portEntry.service()  = it->second;
            portEntry.port()     = it->first;
            portEntry.protocol() = "udp";

            result->push_back(portEntry);
        }
    }

    bsl::sort(result->begin(), result->end(), PortEntrySorter());
}

}  // close package namespace
}  // close enterprise namespace
