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

#ifndef INCLUDED_NTCDNS_CACHE
#define INCLUDED_NTCDNS_CACHE

#include <ntcdns_database.h>
#include <ntcdns_utility.h>
#include <ntcdns_vocabulary.h>
#include <ntcscm_version.h>

#include <ntsa_domainname.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_ipaddress.h>
#include <ntsa_port.h>

#include <bslmt_mutex.h>
#include <bsls_timeinterval.h>

#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcdns {
class CacheHostEntry;
}
namespace ntcdns {

/// @internal @brief
/// Define a type alias for a multi-valued association
/// between a domain name and the cached entries that describe the
/// association between the domain name and the last known IP addresses
/// it has been assigned.
///
/// @ingroup module_ntcdns
typedef bsl::unordered_multimap<bsl::string,
                                bsl::shared_ptr<ntcdns::CacheHostEntry> >
    CacheHostEntryByDomainName;

/// @internal @brief
/// Define a type alias to a key-value pair in a host
/// entry cache keyed by domain names.
///
/// @ingroup module_ntcdns
typedef CacheHostEntryByDomainName::iterator
    CacheHostEntryByDomainNameIterator;

/// @internal @brief
/// Define a type alias to a range of key-value pairs in a
/// host entry cache keyed by domain names.
///
/// @ingroup module_ntcdns
typedef bsl::pair<CacheHostEntryByDomainName::iterator,
                  CacheHostEntryByDomainName::iterator>
    CacheHostEntryByDomainNameIteratorPair;

/// @internal @brief
/// Define a type alias for a multi-valued association
/// between an IP address and the cached entries that describe the
/// association between the IP address and the last known domain name to
/// which it has been assigned.
///
/// @ingroup module_ntcdns
typedef bsl::unordered_map<ntsa::IpAddress,
                           bsl::shared_ptr<ntcdns::CacheHostEntry> >
    CacheHostEntryByIpAddress;

/// @internal @brief
/// Define a type alias to a key-value pair in a host
/// entry cache keyed by domain names.
///
/// @ingroup module_ntcdns
typedef CacheHostEntryByIpAddress::iterator CacheHostEntryByIpAddressIterator;

/// @internal @brief
/// Describe a cached association between a domain name and an IP address.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcdns
class CacheHostEntry
{
    bsl::string        d_domainName;
    ntsa::IpAddress    d_ipAddress;
    ntsa::Endpoint     d_nameServer;
    bsl::size_t        d_timeToLive;
    bsls::TimeInterval d_lastUpdate;
    bsls::TimeInterval d_expiration;

    ntcdns::CacheHostEntryByDomainNameIterator d_iteratorByDomainName;
    ntcdns::CacheHostEntryByIpAddressIterator  d_iteratorByIpAddress;

  private:
    CacheHostEntry(const CacheHostEntry&) BSLS_KEYWORD_DELETED;
    CacheHostEntry& operator=(const CacheHostEntry&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new cache entry.  Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit CacheHostEntry(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~CacheHostEntry();

    /// Set the domain name to the specified 'value'.
    void setDomainName(const bsl::string& value);

    /// Set the IP address to the specified 'value'.
    void setIpAddress(const ntsa::IpAddress& value);

    /// Set the name server to the specified 'value'.
    void setNameServer(const ntsa::Endpoint& value);

    /// Set the time-to-live to the specified 'value'.
    void setTimeToLive(bsl::size_t value);

    /// Set the absolute time, since the Unix epoch, when this entry was
    /// last updated to the specified 'value'.
    void setLastUpdate(const bsls::TimeInterval& value);

    /// Set the absolute time, since the Unix epoch, when this entry's
    /// validity expires to the specified 'value'.
    void setExpiration(const bsls::TimeInterval& value);

    /// Set the iterator to the entry in the map keyed by domain name to the
    /// specified 'value'.
    void setIteratorByDomainName(
        ntcdns::CacheHostEntryByDomainNameIterator value);

    /// Set the iterator to the entry in the map keyed by IP address to the
    /// specified 'value'.
    void setIteratorByIpAddress(
        ntcdns::CacheHostEntryByIpAddressIterator value);

    /// Return the domain name.
    const bsl::string& domainName() const;

    /// Return the IP address.
    const ntsa::IpAddress& ipAddress() const;

    /// Return the name server.
    const ntsa::Endpoint& nameServer() const;

    /// Return the time-to-live.
    bsl::size_t timeToLive() const;

    /// Return the absolute time, since the Unix epoch, when this entry was
    /// last updated.
    const bsls::TimeInterval& lastUpdate() const;

    /// Return the absolute time, since the Unix epoch, when this entry's
    /// validity expires.
    const bsls::TimeInterval& expiration() const;

    /// Return the iterator to the entry in the map keyed by domain name.
    ntcdns::CacheHostEntryByDomainNameIterator iteratorByDomainName() const;

    /// Return the iterator to the entry in the map keyed by IP address.
    ntcdns::CacheHostEntryByIpAddressIterator iteratorByIpAddress() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcdns::CacheHostEntry
bsl::ostream& operator<<(bsl::ostream& stream, const CacheHostEntry& object);

/// @internal @brief
/// Provide a cache of names, addresses, and ports.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class Cache
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                              d_mutex;
    mutable ntcdns::CacheHostEntryByDomainName d_cacheEntryByDomainName;
    mutable ntcdns::CacheHostEntryByIpAddress  d_cacheEntryByIpAddress;
    mutable bsl::size_t                        d_cacheEntryCount;
    bool                                       d_positiveCacheEnabled;
    bsl::size_t                                d_positiveCacheMinTimeToLive;
    bsl::size_t                                d_positiveCacheMaxTimeToLive;
    bool                                       d_negativeCacheEnabled;
    bsl::size_t                                d_negativeCacheMinTimeToLive;
    bsl::size_t                                d_negativeCacheMaxTimeToLive;
    bslma::Allocator*                          d_allocator_p;

  private:
    Cache(const Cache&) BSLS_KEYWORD_DELETED;
    Cache& operator=(const Cache&) BSLS_KEYWORD_DELETED;

  private:
    /// Remove the specified 'cacheEntry'.
    void privateRemove(
        const bsl::shared_ptr<ntcdns::CacheHostEntry>& cacheEntry);

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Cache(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Cache();

    /// Clear the cache.
    void clear();

    /// Set the flag indicating the positive cache is enabled to the
    /// specified 'value'. The positive cache remembers results from
    /// successful resolutions. The default value is null, indicating a
    /// positive cache should *not* be maintained.
    void setPositiveCacheEnabled(bool value);

    /// Set the minimum time-to-live for each result to be cacheable in the
    /// positive cache to the specified 'value'. The default value is null,
    /// indicating no minimum time-to-live is enforced.
    void setPositiveCacheMinTimeToLive(bsl::size_t value);

    /// Set the maximum time-to-live of each result to be stored in the
    /// positive cache to the specified 'value'. The default value is null,
    /// indicating no maximum time-to-live is enforced.
    void setPositiveCacheMaxTimeToLive(bsl::size_t value);

    /// Set the flag indicating the negative cache is enabled to the
    /// specified 'value'. The negative cache remembers results from
    /// failed resolutions. The default value is null, indicating a
    /// negative cache should *not* be maintained.
    void setNegativeCacheEnabled(bool value);

    /// Set the minimum time-to-live for each result to be cacheable in the
    /// negative cache to the specified 'value'. The default value is null,
    /// indicating no minimum time-to-live is enforced.
    void setNegativeCacheMinTimeToLive(bsl::size_t value);

    /// Set the maximum time-to-live of each result to be stored in the
    /// negative cache to the specified 'value'. The default value is null,
    /// indicating no maximum time-to-live is enforced.
    void setNegativeCacheMaxTimeToLive(bsl::size_t value);

    /// Insert or update the host entry for the specified 'domainName' to be
    /// associated with the specified 'ipAddress' starting from the
    /// specified 'now' for the specified 'timeToLive'.
    void updateHost(const bsl::string&        domainName,
                    const ntsa::IpAddress&    ipAddress,
                    const ntsa::Endpoint&     nameServer,
                    bsl::size_t               timeToLive,
                    const bsls::TimeInterval& now);

    /// Load into the specified 'result' the IP address list assigned to the
    /// specified 'domainName' according to the specified 'options' and
    /// load into the specified 'context' the context of resolution. Return
    /// the error.
    ntsa::Error getIpAddress(ntca::GetIpAddressContext*       context,
                             bsl::vector<ntsa::IpAddress>*    result,
                             const bslstl::StringRef&         domainName,
                             const ntca::GetIpAddressOptions& options,
                             const bsls::TimeInterval&        now) const;

    /// Load into the specified 'result' the domain name to which the
    /// specified 'ipAddress' is assigned according to the specified
    /// 'options' and load into the specified 'context' the context of
    /// resolution. Return the error.
    ntsa::Error getDomainName(ntca::GetDomainNameContext*       context,
                              bsl::string*                      result,
                              const ntsa::IpAddress&            ipAddress,
                              const ntca::GetDomainNameOptions& options,
                              const bsls::TimeInterval&         now) const;

    /// Load into the specified 'result' the port list assigned to the
    /// specified 'serviceName' according to the specified 'options' and
    /// load into the specified 'context' the context of resolution. Return
    /// the error.
    ntsa::Error getPort(ntca::GetPortContext*       context,
                        bsl::vector<ntsa::Port>*    result,
                        const bslstl::StringRef&    serviceName,
                        const ntca::GetPortOptions& options,
                        const bsls::TimeInterval&   now) const;

    /// Load into the specified 'result' the service name to which the
    /// specified 'port' is assigned according to the specified 'options'
    /// and load into the specified 'context' the context of resolution.
    /// Return the error.
    ntsa::Error getServiceName(ntca::GetServiceNameContext*       context,
                               bsl::string*                       result,
                               const ntsa::Port&                  port,
                               const ntca::GetServiceNameOptions& options,
                               const bsls::TimeInterval&          now) const;

    /// Return the number of cached domain name to IP address associations.
    bsl::size_t numHostEntries() const;

    /// Return the number of cached service name to port associations.
    bsl::size_t numPortEntries() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
