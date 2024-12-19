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

#ifndef INCLUDED_NTCA_RESOLVERCONFIG
#define INCLUDED_NTCA_RESOLVERCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <bdlb_nullablevalue.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a resolver.
///
/// @details
/// This class provides a value-semantic type that describes the configuration
/// parameters for a resolver. The default value of a configuration effectively
/// represents a resolve that ignores the system host and service database and
/// directly contacts the name servers with the configuration defined by the
/// system's DNS client configuration. On Unix-like platforms, the host
/// database is typically found at "/etc/hosts", and service database is
/// typically found at "/etc/services", and the DNS client configuration is
/// typically found at "/etc/resolve.conf". On Windows platforms, the host
/// database is typically found at "C:\Windows\System32\drivers\etc\hosts" and
/// the service database is typically found at
/// "C:\Windows\System32\drivers\etc\services", while the DNS client
/// configuration is loaded using the operating system interfaces directly, but
/// the effective behavior between Unix-like and Windows platforms is otherwise
/// identical. User may optionally choose to load either the host or service
/// database, or to enable caching, according to the functionality desired.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b hostDatabaseEnabled:
/// The flag that indicates the host database should be loaded and examined
/// when resolving domain names and IP addresses. The default value is null,
/// which indicates the host database is *not* enabled.
///
/// @li @b hostDatabasePath:
/// The path to the file that contains the definition of the host database. The
/// default value is null, which indicates the host database should be found at
/// the default path for the current platform: on Unix platforms, the default
/// path is "/etc/hosts"; on Windows, the default path is
/// "C:\Windows\System32\drivers\etc\hosts".
///
/// @li @b portDatabaseEnabled:
/// The flag that indicates the port database should be loaded and examined
/// when resolving service names and port numbers. The default value is null,
/// which indicates the port database is *not* enabled.
///
/// @li @b portDatabasePath:
/// The path to the file that contains the definition of the port database. The
/// default value is null, which indicates the port database should be found at
/// the default path for the current platform: on Unix platforms, the default
/// path is "/etc/services"; on Windows, the default path is
/// "C:\Windows\System32\drivers\etc\services".
///
/// @li @b positiveCacheEnabled:
/// The flag indicating a cache of positive results should be maintained. A
/// positive result is a successful resolution. The default value is null,
/// indicating a positive cache should *not* be maintained.
///
/// @li @b positiveCacheMinTimeToLive:
/// The minimum time-to-live for any positive result to be considered cachable.
/// The default value is null, indicating no minimum time-to-live is enforced.
///
/// @li @b positiveCacheMaxTimeToLive:
/// The maximum time-to-live that any positive result is cached. The default
/// value is null, indicating no maximum time-to-live is enforced.
///
/// @li @b negativeCacheEnabled:
/// The flag indicating a cache of negative results should be maintained. A
/// negative result is a failed resolution. The default value is null,
/// indicating a negative cache should *not* be maintained.
///
/// @li @b negativeCacheMinTimeToLive:
/// The minimum time-to-live for any negative result to be considered cachable.
/// The default value is null, indicating no minimum time-to-live is enforced.
///
/// @li @b negativeCacheMaxTimeToLive:
/// The maximum time-to-live that any negative result is cached. The default
/// value is null, indicating no maximum time-to-live is enforced.
///
/// @li @b clientEnabled:
/// The flag that indicates a DNS client should run. The default value is null,
/// which indicates a DNS client is run.
///
/// @li @b clientSpecificationPath:
/// The path to the file that contains the definition of the behavior of the
/// DNS client. The default value is null, which indicates the DNS client
/// specification should be found at the default path, or loaded using the
/// default mechanism for the current platform: on Unix platforms, the default
/// path is "/etc/resolv.conf"; on Windows, the specification is loaded
/// directly using the operating system interface.
///
/// @li @b clientRemoteEndpointList:
/// The remote endpoints of the resolver acting as a DNS client. The default
/// value is empty, indicating the value is defined by system's DNS client
/// configuration.
///
/// @li @b clientDomainSearchList:
/// The domain search path used by the DNS client when interpreting a name.
/// Typically, users use short names relative to a set of effective domain(s)
/// when resolving names within those domains. Names having fewer than a
/// configured number of dots will be attempted using each domain of the search
/// path in turn until a match is found. When specified, this field is silently
/// capped at 6 entries. The default value is null, indicating the value is
/// defined by the system's DNS client configuration.
///
/// @li @b clientAttempts:
/// The maximum number of remote name servers contacted by a DNS client to
/// resolve a name. The maximum value is silently capped at 5. The default
/// value is null, indicating the value is defined by the system's DNS client
/// configuration.
///
/// @li @b clientTimeout:
/// The timeout of each request, in seconds, send by the DNS client. The
/// maximum value is silently capped at 30 seconds. The default value is null,
/// indicating the value is defined by the system's DNS client configuration.
///
/// @li @b clientRotate:
/// Flag indicating that the DNS client should contact each name server in
/// round-robin order, instead of in descending order of priority. The default
/// value is null, indicating the value is defined by the system's DNS client
/// configuration.
///
/// @li @b clientDots:
/// The threshold used by the DNS client for the number of dots that must
/// appear in a name before an initial absolute query will be made. The maximum
/// value is silently capped at 15. The default value is null, indicating the
/// value is defined by the system's DNS client configuration.
///
/// @li @b clientDebug:
/// The flag indicating that debug output should be generated by the DNS
/// client. The default value is null, indicating the value is defined by the
/// system's DNS client configuration.
///
/// @li @b systemEnabled:
/// The flag indicating that name resolution by blocking system calls made by a
/// dedicated thread pool is enabled. When blocking system calls by a dedicated
/// thread pool are enabled, if a resolution is neither found in a database nor
/// a cache nor successfully performed by any remote name servers the operation
/// is dispatched to a dedicated thread pool, which grows and shrinks on-demand
/// to execute the blocking system call. The default value is null, which
/// indicates a blocking system calls made by a dedicated thread pool are *not*
/// enabled unless the DNS client is *not* enabled.
///
/// @li @b systemMinThreads:
/// The minimum number of threads in the dedicated thread pool used to execute
/// blocking system calls to perform resolutions. The default value is null,
/// indicating the thread pool as zero minimum threads. Note that the thread
/// pool grows and shrinks on-demand.
///
/// @li @b systemMaxThreads:
/// The maximum number of threads in the dedicated thread pool used to execute
/// blocking system calls to perform resolutions. The default value is null,
/// indicating the thread pool as one maximum thread. Note that the thread pool
/// grows and shrinks on-demand.
///
/// @li @b serverEnabled:
/// The flag that indicates a DNS server should run. The default value is null,
/// which indicates a DNS server is *not* run.
///
/// @li @b serverSourceEndpointList:
/// The source endpoints of the resolver acting as a DNS server. The default
/// value is empty, indicating the server binds to any address on port 53.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_resolve
class ResolverConfig
{
    bdlb::NullableValue<bool>        d_hostDatabaseEnabled;
    bdlb::NullableValue<bsl::string> d_hostDatabasePath;
    bdlb::NullableValue<bool>        d_portDatabaseEnabled;
    bdlb::NullableValue<bsl::string> d_portDatabasePath;
    bdlb::NullableValue<bool>        d_positiveCacheEnabled;
    bdlb::NullableValue<bsl::size_t> d_positiveCacheMinTimeToLive;
    bdlb::NullableValue<bsl::size_t> d_positiveCacheMaxTimeToLive;
    bdlb::NullableValue<bool>        d_negativeCacheEnabled;
    bdlb::NullableValue<bsl::size_t> d_negativeCacheMinTimeToLive;
    bdlb::NullableValue<bsl::size_t> d_negativeCacheMaxTimeToLive;
    bdlb::NullableValue<bool>        d_clientEnabled;
    bdlb::NullableValue<bsl::string> d_clientSpecificationPath;
    bsl::vector<ntsa::Endpoint>      d_clientRemoteEndpointList;
    bsl::vector<bsl::string>         d_clientDomainSearchList;
    bdlb::NullableValue<bsl::size_t> d_clientAttempts;
    bdlb::NullableValue<bsl::size_t> d_clientTimeout;
    bdlb::NullableValue<bool>        d_clientRotate;
    bdlb::NullableValue<bsl::size_t> d_clientDots;
    bdlb::NullableValue<bool>        d_clientDebug;
    bdlb::NullableValue<bool>        d_systemEnabled;
    bdlb::NullableValue<bsl::size_t> d_systemMinThreads;
    bdlb::NullableValue<bsl::size_t> d_systemMaxThreads;
    bdlb::NullableValue<bool>        d_serverEnabled;
    bsl::vector<ntsa::Endpoint>      d_serverSourceEndpointList;

  public:
    /// Create a new resolver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ResolverConfig(bslma::Allocator* basicAllocator = 0);

    /// Create  anew resolver configuration having the same value as the
    /// specified 'original' resolver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ResolverConfig(const ResolverConfig& original,
                   bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~ResolverConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ResolverConfig& operator=(const ResolverConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the flag indicating the host database is enabled to the
    /// specified 'value'. This flag indicates the host database should
    /// be loaded and examined when resolving domain names and IP addresses.
    /// The default value is null, which indicates the host database is
    /// *not* enabled.
    void setHostDatabaseEnabled(bool value);

    /// Set the host database path to the specified 'value'. This path
    /// contains the definition of the host database. The default value is
    /// null, which indicates the host database should be found at the
    /// default path for the current platform: on Unix platforms, the
    /// default path is "/etc/hosts"; on Windows, the default path is
    /// "C:\Windows\System32\drivers\etc\hosts".
    void setHostDatabasePath(const bsl::string& value);

    /// Set the flag indicating the port database is enabled to the
    /// specified 'value'. This flag indicates the port database should
    /// be loaded and examined when resolving service names and port
    /// numbers. The default value is null, which indicates the port
    /// database is *not* enabled.
    void setPortDatabaseEnabled(bool value);

    /// Set the port database path to the specified 'value'. This path
    /// contains the definition of the port database. The default value is
    /// null, which indicates the port database should be found at the
    /// default path for the current platform: on Unix platforms, the
    /// default path is "/etc/services"; on Windows, the default path is
    /// "C:\Windows\System32\drivers\etc\services".
    void setPortDatabasePath(const bsl::string& value);

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

    /// Set the flag indicating the DNS client is enabled to the specified
    /// 'value'. When the DNS client is enabled, if a resolution is neither
    /// found in a database nor a cache the remote name servers are
    /// requested to perform the resolution. The default value is null,
    /// which indicates a DNS client is run.
    void setClientEnabled(bool value);

    /// Set the DNS client specification path to the specified 'value'. This
    /// path defines the parameters of the DNS client. The default value is
    /// null, which indicates the DNS client specification should be found
    /// at the default path, or loaded using the default mechanism for the
    /// current platform: on Unix platforms, the default path is
    /// "/etc/resolv.conf"; on Windows, the specification is loaded directly
    /// using the operating system interface.
    void setClientSpecificationPath(const bsl::string& value);

    /// Set the DNS client remote endpoint list to the specified 'value'.
    /// These endpoints represent the names servers requested to perform a
    /// resolution if no resolution is found in a database or a cache. The
    /// default value is empty, indicating the value is defined by system's
    /// DNS client configuration.
    void setClientRemoteEndpointList(const bsl::vector<ntsa::Endpoint>& value);

    /// Set the DNS client domain search list to the specified 'value'.
    /// The domain search list defines how a DNS client interprets a name.
    /// Typically, users use short names relative to a set of effective
    /// domain(s) when resolving names within those domains. Names having
    /// fewer than a configured number of dots will be attempted using each
    /// domain of the search path in turn until a match is found. When
    /// specified, this field is silently capped at 6 entries. The default
    /// value is null, indicating the value is defined by the system's DNS
    /// client configuration.
    void setClientDomainSearchList(const bsl::vector<bsl::string>& value);

    /// Set the number of attempts made by the DNS client to the specified
    /// 'value'. The maximum value is silently capped at 5. The default
    /// value is null, indicating the value is defined by the system's DNS
    /// client configuration.
    void setClientAttempts(bsl::size_t value);

    /// Set the time of each request made by the DNS client, in seconds, to
    /// the specified 'value'. The maximum value is silently capped at 30
    /// seconds. The default value is null, indicating the value is defined
    /// by the system's DNS client configuration.
    void setClientTimeout(bsl::size_t value);

    /// Set the flag indicating the DNS client should contact each name
    /// server in round-robin order, instead of in descending order of
    /// priority, to the specified 'value'. The default value is null,
    /// indicating the value is defined by the system's DNS client
    /// configuration.
    void setClientRotate(bool value);

    /// Set the threshold used by the DNS client for the number of dots
    /// that must appear in a name before an initial absolute query will be
    /// made to the specified 'value'. The maximum value is silently capped
    /// at 15. The default value is null, indicating the value is defined by
    /// the system's DNS client configuration.
    void setClientDots(bsl::size_t value);

    /// Set the flag indicating that debug output should be generated by the
    /// DNS client to the specified 'value'. The default value is null,
    /// indicating the value is defined by the system's DNS client
    /// configuration.
    void setClientDebug(bool value);

    /// Set the flag indicating that name resolution by blocking system
    /// calls made by a dedicated thread pool is enabled to the specified
    /// 'value'. When blocking system calls by a dedicated thread pool are
    /// enabled, if a resolution is neither found in a database nor a cache
    /// nor successfully performed by any remote name servers the operation
    /// is dispatched to a dedicated thread pool, which grows and shrinks
    /// on-demand to execute the blocking system call. The default value is
    /// null, which indicates a blocking system calls made by a dedicated
    /// thread pool are *not* enabled unless the DNS client is *not*
    /// enabled.
    void setSystemEnabled(bool value);

    /// Set the minimum number of threads in the dedicated thread pool used
    /// to execute blocking system calls to perform resolutions to the
    /// specified 'value'. The default value is null, indicating the thread
    /// pool as zero minimum threads. Note that the thread pool grows and
    /// shrinks on-demand.
    void setSystemMinThreads(bsl::size_t value);

    /// Set the maximum number of threads in the dedicated thread pool used
    /// to execute blocking system calls to perform resolutions to the
    /// specified 'value'. The default value is null, indicating the thread
    /// pool as one maximum thread. Note that the thread pool grows and
    /// shrinks on-demand.
    void setSystemMaxThreads(bsl::size_t value);

    /// Set the flag indicating the DNS server is enabled to the specified
    /// 'value'. The default value is null, which indicates a DNS server is
    /// *not* run.
    void setServerEnabled(bool value);

    /// Set the DNS server source endpoint list to the specified 'value'.
    /// The DNS server binds UDP and TCP sockets to these endpoints and
    /// responds to requests from DNS clients to these endpoints. The
    /// default value is empty, indicating the server binds to any address
    /// on port 53.
    void setServerSourceEndpointList(const bsl::vector<ntsa::Endpoint>& value);

    /// Return the flag indicating the host database is enabled. This flag
    /// indicates the host database should be loaded and examined when
    /// resolving domain names and IP addresses. The default value is null,
    /// which indicates the host database is *not* enabled.
    const bdlb::NullableValue<bool>& hostDatabaseEnabled() const;

    /// Return the host database path. This path contains the definition of
    /// the host database. The default value is null, which indicates the
    /// host database should be found at the default path for the current
    /// platform: on Unix platforms, the default path is "/etc/hosts"; on
    /// Windows, the path is "C:\Windows\System32\drivers\etc\hosts".
    const bdlb::NullableValue<bsl::string>& hostDatabasePath() const;

    /// Return the flag indicating the port database is enabled. This flag
    /// indicates the port database should be loaded and examined when
    /// resolving service names and port numbers. The default value is null,
    /// which indicates the port database is *not* enabled.
    const bdlb::NullableValue<bool>& portDatabaseEnabled() const;

    /// Return the port database path. This path contains the definition of
    /// the port database. The default value is null, which indicates the
    /// port database should be found at the default path for the current
    /// platform: on Unix platforms, the default path is "/etc/services"; on
    ///  Windows, the path is "C:\Windows\System32\drivers\etc\services".
    const bdlb::NullableValue<bsl::string>& portDatabasePath() const;

    /// Return the flag indicating the positive cache is enabled. The
    /// positive cache remembers results from successful resolutions. The
    /// default value is null, indicating a positive cache should *not* be
    /// maintained.
    const bdlb::NullableValue<bool>& positiveCacheEnabled() const;

    /// Return the minimum time-to-live for each result to be cacheable in
    /// the positive cache to the specified 'value'. The default value is
    /// null, indicating no minimum time-to-live is enforced.
    const bdlb::NullableValue<bsl::size_t>& positiveCacheMinTimeToLive() const;

    /// Return the maximum time-to-live of each result to be stored in the
    /// positive cache. The default value is null, indicating no maximum
    /// time-to-live is enforced.
    const bdlb::NullableValue<bsl::size_t>& positiveCacheMaxTimeToLive() const;

    /// Return the flag indicating the negative cache is enabled. The
    /// negative cache remembers results from failed resolutions. The
    /// default value is null, indicating a negative cache should *not* be
    /// maintained.
    const bdlb::NullableValue<bool>& negativeCacheEnabled() const;

    /// Return the minimum time-to-live for each result to be cacheable in
    /// the negative cache. The default value is null, indicating no minimum
    /// time-to-live is enforced.
    const bdlb::NullableValue<bsl::size_t>& negativeCacheMinTimeToLive() const;

    /// Return the maximum time-to-live of each result to be stored in the
    /// negative cache. The default value is null, indicating no maximum
    /// time-to-live is enforced.
    const bdlb::NullableValue<bsl::size_t>& negativeCacheMaxTimeToLive() const;

    /// Return the flag indicating the DNS client is enabled. When the DNS
    /// client is enabled, if a resolution is neither found in a database
    /// nor a cache the remote name servers are requested to perform the
    /// resolution. The default value is null, which  indicates a DNS client
    /// is run.
    const bdlb::NullableValue<bool>& clientEnabled() const;

    /// Return the DNS client specification path. This path defines the
    /// parameters of the DNS client. The default value is null, which
    /// indicates the DNS client specification should be found at the
    /// default path, or loaded using the default mechanism for the current
    /// platform: on Unix platforms, the default path is "/etc/resolv.conf";
    /// on Windows, the specification is loaded directly using the operating
    /// system interface.
    const bdlb::NullableValue<bsl::string>& clientSpecificationPath() const;

    /// Return the DNS client remote endpoint list. These endpoints
    /// represent the names servers requested to perform a resolution if no
    /// resolution is found in a database or a cache. The default value is
    /// empty, indicating the value is defined by system's DNS client
    /// configuration.
    const bsl::vector<ntsa::Endpoint>& clientRemoteEndpointList() const;

    /// Return the DNS client domain search list. The domain search list
    /// defines how a DNS client interprets a name. Typically, users use
    /// short names relative to a set of effective domain(s) when resolving
    /// names within those domains. Names having fewer than a configured
    /// number of dots will be attempted using each domain of the search
    /// path in turn until a match is found. When specified, this field is
    /// silently capped at 6 entries. The default value is null, indicating
    /// the value is defined by the system's DNS client configuration.
    const bsl::vector<bsl::string>& clientDomainSearchList() const;

    /// Return the number of attempts made by the DNS client. The maximum
    /// value is silently capped at 5. The default value is null, indicating
    /// the value is defined by the system's DNS client configuration.
    const bdlb::NullableValue<bsl::size_t>& clientAttempts() const;

    /// Return the time of each request made by the DNS client, in seconds.
    /// The maximum value is silently capped at 30 seconds. The default
    /// value is null, indicating the value is defined by the system's DNS
    /// client configuration.
    const bdlb::NullableValue<bsl::size_t>& clientTimeout() const;

    /// Return the flag indicating the DNS client should contact each name
    /// server in round-robin order, instead of in descending order of
    /// priority. The default value is null, indicating the value is defined
    /// by the system's DNS client configuration.
    const bdlb::NullableValue<bool>& clientRotate() const;

    /// Return the threshold used by the DNS client for the number of dots
    /// that must appear in a name before an initial absolute query will be
    /// made. The maximum value is silently capped at 15. The default value
    /// is null, indicating the value is defined by the system's DNS client
    /// configuration.
    const bdlb::NullableValue<bsl::size_t>& clientDots() const;

    /// Return the flag indicating that debug output should be generated by
    /// the DNS client. The default value is null, indicating the value is
    /// defined by the system's DNS client configuration.
    const bdlb::NullableValue<bool>& clientDebug() const;

    /// Return the flag indicating that name resolution by blocking system
    /// calls made by a dedicated thread pool is enabled. When blocking
    /// system calls by a dedicated thread pool are enabled, if a
    /// resolution is neither found in a database nor a cache nor
    /// successfully performed by any remote name servers the operation is
    /// dispatched to a dedicated thread pool, which grows and shrinks
    /// on-demand to execute the blocking system call. The default value is
    /// null, which indicates a blocking system calls made by a dedicated
    /// thread pool are *not* enabled unless the DNS client is *not*
    /// enabled.
    const bdlb::NullableValue<bool>& systemEnabled() const;

    /// Return the minimum number of threads in the dedicated thread pool
    /// used to execute blocking system calls to perform resolutions. The
    /// default value is null, indicating the thread pool as zero minimum
    /// threads. Note that the thread pool grows and shrinks on-demand.
    const bdlb::NullableValue<bsl::size_t>& systemMinThreads() const;

    /// Return the maximum number of threads in the dedicated thread pool
    /// used to execute blocking system calls to perform resolutions. The
    /// default value is null, indicating the thread pool as one maximum
    /// thread. Note that the thread pool grows and shrinks on-demand.
    const bdlb::NullableValue<bsl::size_t>& systemMaxThreads() const;

    /// Return the flag indicating the DNS server is enabled. The default
    /// value is null, which indicates a DNS server is *not* run.
    const bdlb::NullableValue<bool>& serverEnabled() const;

    /// Return the DNS server source endpoint list. The DNS server binds UDP
    /// and TCP sockets to these endpoints and responds to requests from
    /// DNS clients to these endpoints. The default value is empty,
    /// indicating the server binds to any address on port 53.
    const bsl::vector<ntsa::Endpoint>& serverSourceEndpointList() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ResolverConfig& other) const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ResolverConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ResolverConfig
bsl::ostream& operator<<(bsl::ostream& stream, const ResolverConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ResolverConfig
bool operator==(const ResolverConfig& lhs, const ResolverConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ResolverConfig
bool operator!=(const ResolverConfig& lhs, const ResolverConfig& rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
