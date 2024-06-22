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

#ifndef INCLUDED_NTCDNS_DATABASE
#define INCLUDED_NTCDNS_DATABASE

#include <ntca_getdomainnamecontext.h>
#include <ntca_getdomainnameoptions.h>
#include <ntca_getipaddresscontext.h>
#include <ntca_getipaddressoptions.h>
#include <ntca_getportcontext.h>
#include <ntca_getportoptions.h>
#include <ntca_getservicenamecontext.h>
#include <ntca_getservicenameoptions.h>
#include <ntcdns_utility.h>
#include <ntcdns_vocabulary.h>
#include <ntcscm_version.h>
#include <ntsa_domainname.h>
#include <ntsa_error.h>
#include <ntsa_ipaddress.h>
#include <ntsa_port.h>
#include <bdlma_multipoolallocator.h>
#include <bslmt_mutex.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

// Uncomment or set to 0 to use variable-capacity vectors instead of
// fixed-capacity arrays.
// #define NTCDNS_DATABASE_FIXED_CAPACITY_ARRAYS 1

namespace BloombergLP {
namespace ntcdns {

/// @internal @brief
/// Provide functions to support the implementation of a host database.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcdns
struct HostDatabaseUtil {
    /// Return the hash of the specified 'ipv6Address'.
    static bsl::size_t hashIpv6(const ntsa::Ipv6Address& ipv6Address);
};

/// @internal @brief
/// Provide a database of domain names and addresses.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class HostDatabase
{
    /// Provide a hash functor for string references.
    struct DomainNameHash {
        NTCCFG_INLINE
        bsl::size_t operator()(bslstl::StringRef domainName) const
        {
            bsl::size_t result = 0;

            const char* current = domainName.data();
            const char* end     = current + domainName.size();

            while (current != end) {
                result = ((result << 5) + result) ^ (bsl::size_t)(*current++);
            }

            return result;
        }
    };

    /// Provide a hash functor for string references.
    struct IpAddressHash {
        NTCCFG_INLINE
        bsl::size_t operator()(const ntsa::IpAddress& ipAddress) const
        {
            if (NTCCFG_LIKELY(ipAddress.isV4())) {
                return ipAddress.v4().value();
            }
            else if (ipAddress.isV6()) {
                return ntcdns::HostDatabaseUtil::hashIpv6(ipAddress.v6());
            }
            else {
                return 0;
            }
        }
    };

#if NTCDNS_DATABASE_FIXED_CAPACITY_ARRAYS
    typedef ntcdns::IpAddressArray IpAddressArray;
    // Define a type alias for a fixed-capacity array of
    // IP addresses.
#else
    /// Define a type alias for a variable-length array of
    /// IP addresses.
    typedef bsl::vector<ntsa::IpAddress> IpAddressArray;
#endif

    /// Define a type alias for a map of domain names to an
    /// associated array of IP addresses.
    typedef bsl::
        unordered_map<bslstl::StringRef, IpAddressArray, DomainNameHash>
            IpAddressByDomainName;

    /// Define a type alias for a map of IP addresses to
    /// domain names.
    typedef bsl::
        unordered_map<ntsa::IpAddress, bslstl::StringRef, IpAddressHash>
            DomainNameByIpAddress;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                 d_mutex;
    bdlma::MultipoolAllocator     d_pool;
    IpAddressByDomainName         d_ipAddressByDomainName;
    DomainNameByIpAddress         d_domainNameByIpAddress;
    bsl::shared_ptr<ntcdns::File> d_file_sp;
    bslma::Allocator*             d_allocator_p;

  private:
    HostDatabase(const HostDatabase&) BSLS_KEYWORD_DELETED;
    HostDatabase& operator=(const HostDatabase&) BSLS_KEYWORD_DELETED;

  private:
    /// Load the DNS host database from the specified 'file'. Return the
    /// error.
    ntsa::Error load(const bsl::shared_ptr<ntcdns::File>& file);

  public:
    /// Create a new host database. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit HostDatabase(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~HostDatabase();

    /// Clear the database.
    void clear();

    /// Load the DNS host database from its default location. Return
    /// the error.
    ntsa::Error load();

    /// Load the DNS host database as defined by the file at the specified
    /// 'path'. Return the error.
    ntsa::Error loadPath(const bslstl::StringRef& path);

    /// Load the DNS host database as defined by the specified 'data' having
    /// the specified 'size'. Return the error.
    ntsa::Error loadText(const char* data, bsl::size_t size);

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

/// @internal @brief
/// Provide a database of service names and ports.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class PortDatabase
{
    /// Provide a hash functor for string references.
    struct HashFunction {
        bsl::size_t operator()(bslstl::StringRef value) const
        {
            bsl::size_t result = 0;

            const char* current = value.data();
            const char* end     = current + value.size();

            while (current != end) {
                result = ((result << 5) + result) ^ (bsl::size_t)(*current++);
            }

            return result;
        }
    };

#if NTCDNS_DATABASE_FIXED_CAPACITY_ARRAYS
    typedef ntcdns::PortArray PortArray;
    // Define a type alias for a fixed-capacity array of
    // ports.
#else
    /// Define a type alias for a variable-length array of
    /// ports.
    typedef bsl::vector<ntsa::Port> PortArray;
#endif

    /// Define a type alias for a map of service names to
    /// an associated vector of ports.
    typedef bsl::unordered_map<bslstl::StringRef, PortArray, HashFunction>
        PortByServiceName;

    /// Define a type alias for a map of ports to
    /// service names.
    typedef bsl::unordered_map<ntsa::Port, bslstl::StringRef>
        ServiceNameByPort;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                 d_mutex;
    PortByServiceName             d_tcpPortByServiceName;
    ServiceNameByPort             d_tcpServiceNameByPort;
    PortByServiceName             d_udpPortByServiceName;
    ServiceNameByPort             d_udpServiceNameByPort;
    bsl::shared_ptr<ntcdns::File> d_file_sp;
    bslma::Allocator*             d_allocator_p;

  private:
    PortDatabase(const PortDatabase&) BSLS_KEYWORD_DELETED;
    PortDatabase& operator=(const PortDatabase&) BSLS_KEYWORD_DELETED;

  private:
    /// Load the DNS port database from the specified 'file'. Return the
    /// error.
    ntsa::Error load(const bsl::shared_ptr<ntcdns::File>& file);

  public:
    /// Create a new port database. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit PortDatabase(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~PortDatabase();

    /// Clear the database.
    void clear();

    /// Load the DNS port database from its default location. Return
    /// the error.
    ntsa::Error load();

    /// Load the DNS port database as defined by the file at the specified
    /// 'path'. Return the error.
    ntsa::Error loadPath(const bslstl::StringRef& path);

    /// Load the DNS port database as defined by the specified 'data' having
    /// the specified 'size'. Return the error.
    ntsa::Error loadText(const char* data, bsl::size_t size);

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

    /// Load into the specified 'result' each port entry in the database.
    void dump(bsl::vector<ntcdns::PortEntry>* result) const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
