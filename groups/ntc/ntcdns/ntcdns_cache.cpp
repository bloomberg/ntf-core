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

#include <ntcdns_cache.h>

#include <ntcdns_compat.h>
#include <ntci_log.h>
#include <ntsa_host.h>
#include <ntsu_resolverutil.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcdns {

namespace {

const bool        k_DEFAULT_POSITIVE_CACHE_ENABLED          = true;
const bsl::size_t k_DEFAULT_POSITIVE_CACHE_MIN_TIME_TO_LIVE = 0;
const bsl::size_t k_DEFAULT_POSITIVE_CACHE_MAX_TIME_TO_LIVE =
    (bsl::size_t)(-1);
const bool        k_DEFAULT_NEGATIVE_CACHE_ENABLED          = true;
const bsl::size_t k_DEFAULT_NEGATIVE_CACHE_MIN_TIME_TO_LIVE = 0;
const bsl::size_t k_DEFAULT_NEGATIVE_CACHE_MAX_TIME_TO_LIVE =
    (bsl::size_t)(-1);

}  // close unnamed namespace

CacheHostEntry::CacheHostEntry(bslma::Allocator* basicAllocator)
: d_domainName(basicAllocator)
, d_ipAddress()
, d_nameServer()
, d_timeToLive(0)
, d_lastUpdate()
, d_expiration()
, d_iteratorByDomainName()
, d_iteratorByIpAddress()
{
}

CacheHostEntry::~CacheHostEntry()
{
}

void CacheHostEntry::setDomainName(const bsl::string& value)
{
    d_domainName = value;
}

void CacheHostEntry::setIpAddress(const ntsa::IpAddress& value)
{
    d_ipAddress = value;
}

void CacheHostEntry::setNameServer(const ntsa::Endpoint& value)
{
    d_nameServer = value;
}

void CacheHostEntry::setTimeToLive(bsl::size_t value)
{
    d_timeToLive = value;
}

void CacheHostEntry::setLastUpdate(const bsls::TimeInterval& value)
{
    d_lastUpdate = value;
}

void CacheHostEntry::setExpiration(const bsls::TimeInterval& value)
{
    d_expiration = value;
}

void CacheHostEntry::setIteratorByDomainName(
    ntcdns::CacheHostEntryByDomainNameIterator value)
{
    d_iteratorByDomainName = value;
}

void CacheHostEntry::setIteratorByIpAddress(
    ntcdns::CacheHostEntryByIpAddressIterator value)
{
    d_iteratorByIpAddress = value;
}

const bsl::string& CacheHostEntry::domainName() const
{
    return d_domainName;
}

const ntsa::IpAddress& CacheHostEntry::ipAddress() const
{
    return d_ipAddress;
}

const ntsa::Endpoint& CacheHostEntry::nameServer() const
{
    return d_nameServer;
}

bsl::size_t CacheHostEntry::timeToLive() const
{
    return d_timeToLive;
}

const bsls::TimeInterval& CacheHostEntry::lastUpdate() const
{
    return d_lastUpdate;
}

const bsls::TimeInterval& CacheHostEntry::expiration() const
{
    return d_expiration;
}

ntcdns::CacheHostEntryByDomainNameIterator CacheHostEntry::
    iteratorByDomainName() const
{
    return d_iteratorByDomainName;
}

ntcdns::CacheHostEntryByIpAddressIterator CacheHostEntry::iteratorByIpAddress()
    const
{
    return d_iteratorByIpAddress;
}

bsl::ostream& CacheHostEntry::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("domainName", d_domainName);
    printer.printAttribute("ipAddress", d_ipAddress);
    printer.printAttribute("nameServer", d_nameServer);
    printer.printAttribute("timeToLive", d_timeToLive);
    printer.printAttribute("lastUpdate", d_lastUpdate);
    printer.printAttribute("expiration", d_expiration);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const CacheHostEntry& object)
{
    return object.print(stream, 0, -1);
}

void Cache::privateRemove(
    const bsl::shared_ptr<ntcdns::CacheHostEntry>& cacheEntry)
{
    BSLS_ASSERT_OPT(cacheEntry->iteratorByDomainName() !=
                    d_cacheEntryByDomainName.end());

    BSLS_ASSERT_OPT(cacheEntry->iteratorByIpAddress() !=
                    d_cacheEntryByIpAddress.end());

    d_cacheEntryByDomainName.erase(cacheEntry->iteratorByDomainName());

    d_cacheEntryByIpAddress.erase(cacheEntry->iteratorByIpAddress());

    --d_cacheEntryCount;
}

Cache::Cache(bslma::Allocator* basicAllocator)
: d_mutex()
, d_cacheEntryByDomainName(basicAllocator)
, d_cacheEntryByIpAddress(basicAllocator)
, d_cacheEntryCount(0)
, d_positiveCacheEnabled(k_DEFAULT_POSITIVE_CACHE_ENABLED)
, d_positiveCacheMinTimeToLive(k_DEFAULT_POSITIVE_CACHE_MIN_TIME_TO_LIVE)
, d_positiveCacheMaxTimeToLive(k_DEFAULT_POSITIVE_CACHE_MAX_TIME_TO_LIVE)
, d_negativeCacheEnabled(k_DEFAULT_NEGATIVE_CACHE_ENABLED)
, d_negativeCacheMinTimeToLive(k_DEFAULT_NEGATIVE_CACHE_MIN_TIME_TO_LIVE)
, d_negativeCacheMaxTimeToLive(k_DEFAULT_NEGATIVE_CACHE_MAX_TIME_TO_LIVE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Cache::~Cache()
{
}

void Cache::setPositiveCacheEnabled(bool value)
{
    d_positiveCacheEnabled = value;
}

void Cache::setPositiveCacheMinTimeToLive(bsl::size_t value)
{
    d_positiveCacheMinTimeToLive = value;
}

void Cache::setPositiveCacheMaxTimeToLive(bsl::size_t value)
{
    d_positiveCacheMaxTimeToLive = value;
}

void Cache::setNegativeCacheEnabled(bool value)
{
    d_negativeCacheEnabled = value;
}

void Cache::setNegativeCacheMinTimeToLive(bsl::size_t value)
{
    d_negativeCacheMinTimeToLive = value;
}

void Cache::setNegativeCacheMaxTimeToLive(bsl::size_t value)
{
    d_negativeCacheMaxTimeToLive = value;
}

void Cache::clear()
{
    LockGuard lock(&d_mutex);

    d_cacheEntryByDomainName.clear();
    d_cacheEntryByIpAddress.clear();
    d_cacheEntryCount = 0;
}

void Cache::updateHost(const bsl::string&        domainName,
                       const ntsa::IpAddress&    ipAddress,
                       const ntsa::Endpoint&     nameServer,
                       bsl::size_t               timeToLive,
                       const bsls::TimeInterval& now)
{
    NTCI_LOG_CONTEXT();

    bsl::shared_ptr<ntcdns::CacheHostEntry> newCacheEntry;
    bool                                    oldCacheEntryUpdated = false;

    LockGuard lock(&d_mutex);

    {
        bool mustInsert = true;

        ntcdns::CacheHostEntryByDomainNameIteratorPair range =
            d_cacheEntryByDomainName.equal_range(domainName);

        if (range.first != d_cacheEntryByDomainName.end()) {
            ntcdns::CacheHostEntryByDomainNameIterator it = range.first;
            ntcdns::CacheHostEntryByDomainNameIterator et = range.second;

            while (true) {
                if (it == et) {
                    break;
                }

                const bsl::shared_ptr<ntcdns::CacheHostEntry>& cacheEntry =
                    it->second;

                if (cacheEntry->ipAddress() == ipAddress) {
                    if (!oldCacheEntryUpdated) {
                        cacheEntry->setNameServer(nameServer);
                        cacheEntry->setTimeToLive(timeToLive);
                        cacheEntry->setLastUpdate(now);
                        cacheEntry->setExpiration(
                            now + bsls::TimeInterval(timeToLive, 0));

                        NTCI_LOG_STREAM_TRACE
                            << "DNS cache updated host entry " << *cacheEntry
                            << NTCI_LOG_STREAM_END;

                        oldCacheEntryUpdated = true;
                    }

                    mustInsert = false;
                    ++it;
                }
                else if (now >= cacheEntry->expiration()) {
                    ntcdns::CacheHostEntryByDomainNameIterator jt = it;

                    bsl::shared_ptr<ntcdns::CacheHostEntry> cacheEntry =
                        jt->second;

                    ++it;

                    BSLS_ASSERT_OPT(jt == cacheEntry->iteratorByDomainName());

                    this->privateRemove(cacheEntry);

                    NTCI_LOG_STREAM_TRACE
                        << "DNS cache removed host entry " << *cacheEntry
                        << ": expiration at " << cacheEntry->expiration()
                        << " is greater than or equal to now at " << now
                        << NTCI_LOG_STREAM_END;
                }
                else {
                    ++it;
                }
            }
        }

        if (mustInsert) {
            if (!newCacheEntry) {
                newCacheEntry.createInplace(d_allocator_p, d_allocator_p);

                newCacheEntry->setDomainName(domainName);
                newCacheEntry->setIpAddress(ipAddress);
                newCacheEntry->setNameServer(nameServer);
                newCacheEntry->setTimeToLive(timeToLive);
                newCacheEntry->setLastUpdate(now);
                newCacheEntry->setExpiration(
                    now + bsls::TimeInterval(timeToLive, 0));

                newCacheEntry->setIteratorByDomainName(
                    d_cacheEntryByDomainName.end());

                newCacheEntry->setIteratorByIpAddress(
                    d_cacheEntryByIpAddress.end());

                ++d_cacheEntryCount;
            }

            ntcdns::CacheHostEntryByDomainNameIterator
                newCacheEntryIteratorByDomainName =
                    d_cacheEntryByDomainName.insert(
                        ntcdns::CacheHostEntryByDomainName::value_type(
                            domainName,
                            newCacheEntry));

            newCacheEntry->setIteratorByDomainName(
                newCacheEntryIteratorByDomainName);

            NTCI_LOG_STREAM_TRACE << "DNS cache inserted host entry "
                                  << *newCacheEntry << NTCI_LOG_STREAM_END;
        }
    }

    {
        bool mustInsert = true;

        ntcdns::CacheHostEntryByIpAddressIterator it =
            d_cacheEntryByIpAddress.find(ipAddress);

        if (it != d_cacheEntryByIpAddress.end()) {
            const bsl::shared_ptr<ntcdns::CacheHostEntry>& cacheEntry =
                it->second;

            if (cacheEntry->ipAddress() == ipAddress) {
                if (!oldCacheEntryUpdated) {
                    cacheEntry->setNameServer(nameServer);
                    cacheEntry->setTimeToLive(timeToLive);
                    cacheEntry->setLastUpdate(now);
                    cacheEntry->setExpiration(
                        now + bsls::TimeInterval(timeToLive, 0));

                    NTCI_LOG_STREAM_TRACE << "DNS cache updated host entry "
                                          << *cacheEntry
                                          << NTCI_LOG_STREAM_END;

                    oldCacheEntryUpdated = true;
                }

                mustInsert = false;
            }
            else if (now >= cacheEntry->expiration()) {
                bsl::shared_ptr<ntcdns::CacheHostEntry> cacheEntry =
                    it->second;

                BSLS_ASSERT_OPT(it == cacheEntry->iteratorByIpAddress());

                this->privateRemove(cacheEntry);

                NTCI_LOG_STREAM_TRACE
                    << "DNS cache removed host entry " << *cacheEntry
                    << ": expiration at " << cacheEntry->expiration()
                    << " is greater than or equal to now at " << now
                    << NTCI_LOG_STREAM_END;
            }
        }

        if (mustInsert) {
            if (!newCacheEntry) {
                // MRM: Logically, the new cache entry must have been created.
                BSLS_ASSERT_OPT(newCacheEntry);

                newCacheEntry.createInplace(d_allocator_p, d_allocator_p);

                newCacheEntry->setDomainName(domainName);
                newCacheEntry->setIpAddress(ipAddress);
                newCacheEntry->setNameServer(nameServer);
                newCacheEntry->setTimeToLive(timeToLive);
                newCacheEntry->setLastUpdate(now);
                newCacheEntry->setExpiration(
                    now + bsls::TimeInterval(timeToLive, 0));

                newCacheEntry->setIteratorByDomainName(
                    d_cacheEntryByDomainName.end());

                newCacheEntry->setIteratorByIpAddress(
                    d_cacheEntryByIpAddress.end());

                ++d_cacheEntryCount;
            }

            bsl::pair<ntcdns::CacheHostEntryByIpAddressIterator, bool>
                insertResult = d_cacheEntryByIpAddress.insert(
                    ntcdns::CacheHostEntryByIpAddress::value_type(
                        ipAddress,
                        newCacheEntry));

            BSLS_ASSERT_OPT(insertResult.second);

            ntcdns::CacheHostEntryByIpAddressIterator
                newCacheEntryIteratorByIpAddress = insertResult.first;

            newCacheEntry->setIteratorByIpAddress(
                newCacheEntryIteratorByIpAddress);

            NTCI_LOG_STREAM_TRACE << "DNS cache inserted host entry "
                                  << *newCacheEntry << NTCI_LOG_STREAM_END;
        }
    }
}

ntsa::Error Cache::getIpAddress(ntca::GetIpAddressContext*       context,
                                bsl::vector<ntsa::IpAddress>*    result,
                                const bslstl::StringRef&         domainName,
                                const ntca::GetIpAddressOptions& options,
                                const bsls::TimeInterval&        now) const
{
    // Some versions of GCC erroneously warn when 'timeToLive.value()' is
    // called even when protected by a check of '!timeToLive.isNull()'.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    NTCI_LOG_STREAM_TRACE
        << "DNS cache looking up host entry for domain name '" << domainName
        << "' at time " << now << NTCI_LOG_STREAM_END;

    bsl::vector<ntsa::IpAddress>            ipAddressList;
    bdlb::NullableValue<ntsa::Endpoint>     nameServer;
    bdlb::NullableValue<bsls::TimeInterval> timeToLive;

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType;
    error = ntcdns::Compat::convert(&ipAddressType, options);
    if (error) {
        return error;
    }

    LockGuard lock(&d_mutex);

    bsl::string key = domainName;

    ntcdns::CacheHostEntryByDomainNameIteratorPair range =
        d_cacheEntryByDomainName.equal_range(key);

    if (range.first == d_cacheEntryByDomainName.end()) {
        NTCI_LOG_STREAM_TRACE
            << "DNS cache found no host entry for domain name '" << domainName
            << "'" << NTCI_LOG_STREAM_END;
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    ntcdns::CacheHostEntryByDomainNameIterator it = range.first;
    ntcdns::CacheHostEntryByDomainNameIterator et = range.second;

    while (true) {
        if (it == et) {
            break;
        }

        const bsl::shared_ptr<ntcdns::CacheHostEntry>& cacheEntry = it->second;

        if (now >= cacheEntry->expiration()) {
            ntcdns::CacheHostEntryByDomainNameIterator jt = it;

            bsl::shared_ptr<ntcdns::CacheHostEntry> cacheEntry = jt->second;

            ++it;

            BSLS_ASSERT_OPT(jt == cacheEntry->iteratorByDomainName());

            const_cast<Cache*>(this)->privateRemove(cacheEntry);

            NTCI_LOG_STREAM_TRACE
                << "DNS cache removed host entry " << *cacheEntry
                << ": expiration at " << cacheEntry->expiration()
                << " is greater than or equal to now at " << now
                << NTCI_LOG_STREAM_END;
        }
        else {
            if (ipAddressType.isNull() ||
                cacheEntry->ipAddress().type() == ipAddressType.value())
            {
                if (bsl::find(ipAddressList.begin(),
                              ipAddressList.end(),
                              cacheEntry->ipAddress()) == ipAddressList.end())
                {
                    NTCI_LOG_STREAM_TRACE
                        << "DNS cache found host entry " << *cacheEntry
                        << " for domain name '" << domainName << "'"
                        << NTCI_LOG_STREAM_END;
                    ipAddressList.push_back(cacheEntry->ipAddress());

                    if (nameServer.isNull()) {
                        nameServer.makeValue(cacheEntry->nameServer());
                    }
                    else if (nameServer.value() != cacheEntry->nameServer()) {
                        // MRM: Warn
                    }

                    bsls::TimeInterval newTimeToLive =
                        cacheEntry->expiration() - now;

                    if (timeToLive.isNull()) {
                        timeToLive.makeValue(newTimeToLive);
                    }
                    else if (timeToLive.value() > newTimeToLive) {
                        // MRM: Warn
                        timeToLive.makeValue(newTimeToLive);
                    }
                }
            }
            ++it;
        }
    }

    if (ipAddressType.isNull()) {
        ntsu::ResolverUtil::sortIpAddressList(&ipAddressList);
    }

    if (ipAddressList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    context->setDomainName(domainName);
    context->setSource(ntca::ResolverSource::e_CACHE);

    if (!nameServer.isNull()) {
        context->setNameServer(nameServer.value());
    }

    if (!timeToLive.isNull()) {
        context->setTimeToLive(
            NTCCFG_WARNING_NARROW(bsl::size_t,
                                  timeToLive.value().totalSeconds()));
    }

    if (options.ipAddressSelector().isNull()) {
        *result = ipAddressList;
    }
    else {
        result->push_back(ipAddressList[options.ipAddressSelector().value() %
                                        ipAddressList.size()]);
    }

    return ntsa::Error();

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic pop
#endif
}

ntsa::Error Cache::getDomainName(ntca::GetDomainNameContext*       context,
                                 bsl::string*                      result,
                                 const ntsa::IpAddress&            ipAddress,
                                 const ntca::GetDomainNameOptions& options,
                                 const bsls::TimeInterval&         now) const
{
    // Some versions of GCC erroneously warn when 'timeToLive.value()' is
    // called even when protected by a check of '!timeToLive.isNull()'.
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    NTCI_LOG_STREAM_TRACE << "DNS cache looking up host entry for IP address '"
                          << ipAddress << "' at time " << now
                          << NTCI_LOG_STREAM_END;

    bsl::string                             domainName;
    bdlb::NullableValue<ntsa::Endpoint>     nameServer;
    bdlb::NullableValue<bsls::TimeInterval> timeToLive;

    LockGuard lock(&d_mutex);

    ntcdns::CacheHostEntryByIpAddressIterator it =
        d_cacheEntryByIpAddress.find(ipAddress);

    if (it == d_cacheEntryByIpAddress.end()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bsl::shared_ptr<ntcdns::CacheHostEntry> cacheEntry = it->second;

    if (now >= cacheEntry->expiration()) {
        BSLS_ASSERT_OPT(it == cacheEntry->iteratorByIpAddress());

        const_cast<Cache*>(this)->privateRemove(cacheEntry);

        NTCI_LOG_STREAM_TRACE << "DNS cache removed host entry " << *cacheEntry
                              << ": expiration at " << cacheEntry->expiration()
                              << " is greater than or equal to now at " << now
                              << NTCI_LOG_STREAM_END;
    }
    else {
        NTCI_LOG_STREAM_TRACE << "DNS cache found host entry " << *cacheEntry
                              << " for IP address " << ipAddress
                              << NTCI_LOG_STREAM_END;

        domainName = cacheEntry->domainName();

        if (nameServer.isNull()) {
            nameServer.makeValue(cacheEntry->nameServer());
        }
        else if (nameServer.value() != cacheEntry->nameServer()) {
            // MRM: Warn
        }

        bsls::TimeInterval newTimeToLive = cacheEntry->expiration() - now;

        if (timeToLive.isNull()) {
            timeToLive.makeValue(newTimeToLive);
        }
        else if (timeToLive.value() > newTimeToLive) {
            // MRM: Warn
            timeToLive.makeValue(newTimeToLive);
        }
    }

    if (domainName.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    context->setIpAddress(ipAddress);
    context->setSource(ntca::ResolverSource::e_CACHE);

    if (!nameServer.isNull()) {
        context->setNameServer(nameServer.value());
    }

    if (!timeToLive.isNull()) {
        context->setTimeToLive(
            NTCCFG_WARNING_NARROW(bsl::size_t,
                                  timeToLive.value().totalSeconds()));
    }

    *result = domainName;

    return ntsa::Error();

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic pop
#endif
}

ntsa::Error Cache::getPort(ntca::GetPortContext*       context,
                           bsl::vector<ntsa::Port>*    result,
                           const bslstl::StringRef&    serviceName,
                           const ntca::GetPortOptions& options,
                           const bsls::TimeInterval&   now) const
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(serviceName);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(now);

    return ntsa::Error(ntsa::Error::e_EOF);
}

ntsa::Error Cache::getServiceName(ntca::GetServiceNameContext*       context,
                                  bsl::string*                       result,
                                  const ntsa::Port&                  port,
                                  const ntca::GetServiceNameOptions& options,
                                  const bsls::TimeInterval&          now) const
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(port);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(now);

    return ntsa::Error(ntsa::Error::e_EOF);
}

bsl::size_t Cache::numHostEntries() const
{
    return d_cacheEntryCount;
}

bsl::size_t Cache::numPortEntries() const
{
    return 0;
}

}  // close package namespace
}  // close enterprise namespace
