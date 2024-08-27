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

#ifndef INCLUDED_NTSA_URI
#define INCLUDED_NTSA_URI

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_domainname.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_host.h>
#include <ntsa_ipaddress.h>
#include <ntsa_port.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Describe the authority portion a Uniform Resource Identifier (URI).
///
/// @details
/// URIs are formally specified in RFC 3986.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class UriAuthority
{
    bdlb::NullableValue<bsl::string>            d_user;
    bdlb::NullableValue<ntsa::Host>             d_host;
    bdlb::NullableValue<ntsa::Port>             d_port;
    bdlb::NullableValue<ntsa::Transport::Value> d_transport;

  public:
    /// Create a new URI authority. Optionally specify an 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default basicAllocator is used.
    explicit UriAuthority(bslma::Allocator* basiAllocator = 0);

    /// Create a new URI authority having the same value as the specified
    /// 'other' object. Optionally specify an 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default basicAllocator is used.
    UriAuthority(const UriAuthority& other,
                 bslma::Allocator*   basicAllocator = 0);

    /// Destroy this object.
    ~UriAuthority();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UriAuthority& operator=(const UriAuthority& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the user specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setUser(const bslstl::StringRef& value);

    /// Set the host specified in the authority portion of the URI to the
    /// // specified 'value'. Return the error.
    ntsa::Error setHost(const ntsa::Host& value);

    /// Set the host specified in the authority portion of the URI to the
    /// // specified 'value'. Return the error.
    ntsa::Error setHost(const bslstl::StringRef& value);

    /// Set the host specified in the authority portion of the URI to the
    /// // specified 'value'. Return the error.
    ntsa::Error setHost(const ntsa::IpAddress& value);

    /// Set the host specified in the authority portion of the URI to the
    /// // specified 'value'. Return the error.
    ntsa::Error setHost(const ntsa::Ipv4Address& value);

    /// Set the host specified in the authority portion of the URI to the
    /// // specified 'value'. Return the error.
    ntsa::Error setHost(const ntsa::Ipv6Address& value);

    /// Set the port specified in the authority portion of the URI.
    ntsa::Error setPort(ntsa::Port value);

    /// Set the transport specified in the authority portion of the URI. 
    ntsa::Error setTransport(ntsa::Transport::Value value);

    /// Return the user specified in the authority portion of the URI.
    const bdlb::NullableValue<bsl::string>& user() const;

    /// Return the host specified in the authority portion of the URI.
    const bdlb::NullableValue<ntsa::Host>& host() const;

    /// Return the port specified in the authority portion of the URI.
    const bdlb::NullableValue<ntsa::Port>& port() const;

    /// Return the transport specified in the authority portion of the URI. 
    const bdlb::NullableValue<ntsa::Transport::Value>& transport() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const UriAuthority& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const UriAuthority& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(UriAuthority);
};

/// Insert a formatted, human-readable description of the the specified
/// 'object' into the specified 'stream'. Return a reference to the
/// modifiable 'stream'.
///
/// @related ntsa::UriAuthority
bsl::ostream& operator<<(bsl::ostream& stream, const UriAuthority& object);

/// Return true if the specified 'lhs' value has the same value as the
/// specified 'rhs' value, otherwise return false.
///
/// @related ntsa::UriAuthority
bool operator==(const UriAuthority& lhs, const UriAuthority& rhs);

/// Return true if the specified 'lhs' value does not have the same value as
/// the specified 'rhs' value, otherwise return true.
///
/// @related ntsa::UriAuthority
bool operator!=(const UriAuthority& lhs, const UriAuthority& rhs);

/// Return true if the specified 'lhs' value is "less than" the specified
/// 'rhs' value, otherwise return false.
///
/// @related ntsa::UriAuthority
bool operator<(const UriAuthority& lhs, const UriAuthority& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UriAuthority
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UriAuthority& value);

/// Describe a key/value pair that defines a single parameter within the query
/// portion of a Uniform Resource Identifier (URI).
///
/// @details
/// URIs are formally specified in RFC 3986.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class UriParameter
{
    bsl::string                      d_name;
    bdlb::NullableValue<bsl::string> d_value;

  public:
    /// Create a new URI parameter. Optionally specify an 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default basicAllocator is used.
    explicit UriParameter(bslma::Allocator* basicAllocator = 0);

    /// Create a new URI parameter having the same value as the specified
    /// 'other' object. Optionally specify an 'basicAllocator' used to
    /// supply memory.  If 'basicAllocator' is null, the currently installed
    /// default basicAllocator is used.
    UriParameter(const UriParameter& other,
                 bslma::Allocator*   basicAllocator = 0);

    /// Destroy this object.
    ~UriParameter();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UriParameter& operator=(const UriParameter& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the name of the parameter to the specified 'value'. Return the
    /// error.
    ntsa::Error setName(const bslstl::StringRef& value);

    /// Set the value of the parameter to the specified 'value'. Return the
    /// error.
    ntsa::Error setValue(const bslstl::StringRef& value);

    // ACCCESSORS

    /// Return the name of the parameter.
    const bsl::string& name() const;

    /// Return the value of the parameter.
    const bdlb::NullableValue<bsl::string>& value() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const UriParameter& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const UriParameter& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(UriParameter);
};

/// Insert a formatted, human-readable description of the the specified
/// 'object' into the specified 'stream'. Return a reference to the
/// modifiable 'stream'.
///
/// @related ntsa::UriParameter
bsl::ostream& operator<<(bsl::ostream& stream, const UriParameter& object);

/// Return true if the specified 'lhs' value has the same value as the
/// specified 'rhs' value, otherwise return false.
///
/// @related ntsa::UriParameter
bool operator==(const UriParameter& lhs, const UriParameter& rhs);

/// Return true if the specified 'lhs' value does not have the same value as
/// the specified 'rhs' value, otherwise return true.
///
/// @related ntsa::UriParameter
bool operator!=(const UriParameter& lhs, const UriParameter& rhs);

/// Return true if the specified 'lhs' value is "less than" the specified
/// 'rhs' value, otherwise return false.
///
/// @related ntsa::UriParameter
bool operator<(const UriParameter& lhs, const UriParameter& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UriParameter
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UriParameter& value);

/// Describe the query portion of a Uniform Resource Identifier (URI).
///
/// @details
/// URIs are formally specified in RFC 3986.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class UriQuery
{
    bsl::vector<ntsa::UriParameter> d_parameterList;

  public:
    /// Create a new URI query. Optionally specify an 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is null, the currently
    /// installed default basicAllocator is used.
    explicit UriQuery(bslma::Allocator* basicAllocator = 0);

    /// Create a new URI query having the same value as the specified
    /// 'other' object. Optionally specify an 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default basicAllocator is used.
    UriQuery(const UriQuery& other, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~UriQuery();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UriQuery& operator=(const UriQuery& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the parameter list to the specified 'value'. Return the error.
    ntsa::Error setParameterList(const bsl::vector<ntsa::UriParameter>& value);

    /// Add a parameter having the specified 'value' to the parameter
    /// list. Return the error.
    ntsa::Error addParameter(const ntsa::UriParameter& value);

    /// Add a parameter having the specified 'name' with no defined value to
    /// the parameter list.  Return the error.
    ntsa::Error addParameter(const bslstl::StringRef& name);

    /// Add a parameter having the specified 'name' and the specified
    /// 'value' to the parameter list. Return the error.
    ntsa::Error addParameter(const bslstl::StringRef& name,
                             const bslstl::StringRef& value);

    /// Return the parameter list.
    const bsl::vector<ntsa::UriParameter>& parameterList() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const UriQuery& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const UriQuery& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(UriQuery);
};

/// Insert a formatted, human-readable description of the the specified
/// 'object' into the specified 'stream'. Return a reference to the
/// modifiable 'stream'.
///
/// @related ntsa::UriQuery
bsl::ostream& operator<<(bsl::ostream& stream, const UriQuery& object);

/// Return true if the specified 'lhs' value has the same value as the
/// specified 'rhs' value, otherwise return false.
///
/// @related ntsa::UriQuery
bool operator==(const UriQuery& lhs, const UriQuery& rhs);

/// Return true if the specified 'lhs' value does not have the same value as
/// the specified 'rhs' value, otherwise return true.
///
/// @related ntsa::UriQuery
bool operator!=(const UriQuery& lhs, const UriQuery& rhs);

/// Return true if the specified 'lhs' value is "less than" the specified
/// 'rhs' value, otherwise return false.
///
/// @related ntsa::UriQuery
bool operator<(const UriQuery& lhs, const UriQuery& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UriQuery
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UriQuery& value);

/// Provide a Uniform Resource Identifier (URI).
///
/// @details
/// URIs are formally specified in RFC 3986.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class Uri
{
    bdlb::NullableValue<bsl::string>        d_scheme;
    bdlb::NullableValue<ntsa::UriAuthority> d_authority;
    bdlb::NullableValue<bsl::string>        d_path;
    bdlb::NullableValue<ntsa::UriQuery>     d_query;
    bdlb::NullableValue<bsl::string>        d_fragment;

  public:
    /// Create a new URI. Optionally specify an 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default basicAllocator is used.
    explicit Uri(bslma::Allocator* basicAllocator = 0);

    /// Create a new URI having the same value as the specified 'other'
    /// object.  Optionally specify an 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is null, the currently installed default
    /// basicAllocator is used.
    Uri(const Uri& other, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Uri();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Uri& operator=(const Uri& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the specification that identifies the meaning of the components
    /// of the URI to the specified 'value'. Return the error.
    ntsa::Error setScheme(const bslstl::StringRef& value);

    /// Set the authority portion of the URI to the specified 'value'.
    /// Return the error.
    ntsa::Error setAuthority(const ntsa::UriAuthority& value);

    /// Set the user specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setUser(const bslstl::StringRef& value);

    /// Set the host specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setHost(const ntsa::Host& value);

    /// Set the host specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setHost(const bslstl::StringRef& value);

    /// Set the host specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setHost(const ntsa::IpAddress& value);

    /// Set the host specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setHost(const ntsa::Ipv4Address& value);

    /// Set the host specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setHost(const ntsa::Ipv6Address& value);

    /// Set the port specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setPort(ntsa::Port value);

    /// Set the transport specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setTransport(ntsa::Transport::Value value);

    /// Set the path to the resource to the specified 'value'. Return the
    /// error.
    ntsa::Error setPath(const bslstl::StringRef& value);

    /// Set the query portion of the URI to the specified 'value'. Return
    /// the error.
    ntsa::Error setQuery(const ntsa::UriQuery& value);

    /// Set the parameter list of the query portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setQueryParameterList(
        const bsl::vector<ntsa::UriParameter>& value);

    /// Add a parameter having the specified 'value' to the parameter
    /// list of the query portion of the URI. Return the error.
    ntsa::Error addQueryParameter(const ntsa::UriParameter& value);

    /// Add a parameter having the specified 'name' with no defined value to
    /// the parameter list of the query portion of the URI.  Return the
    /// error.
    ntsa::Error addQueryParameter(const bslstl::StringRef& name);

    /// Add a parameter having the specified 'name' and the specified
    /// 'value' to the parameter list of the query portion of the URI.
    /// Return the error.
    ntsa::Error addQueryParameter(const bslstl::StringRef& name,
                                  const bslstl::StringRef& value);

    /// Set the fragment portion of the URI to the specified 'value'.
    ntsa::Error setFragment(const bslstl::StringRef& value);

    /// Set the value of this object from the value parsed from its textual
    /// representation. Return true if the 'text' is in a valid format and
    /// was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text);

    /// Return the specification that identifies the meaning of the
    /// components of the URI.
    const bdlb::NullableValue<bsl::string>& scheme() const;

    /// Return the authority portion of the URI.
    const bdlb::NullableValue<ntsa::UriAuthority>& authority() const;

    /// Return the path to the resource.
    const bdlb::NullableValue<bsl::string>& path() const;

    /// Return the query portion of the URI.
    const bdlb::NullableValue<ntsa::UriQuery>& query() const;

    /// Return the fragment portion of the URI.
    const bdlb::NullableValue<bsl::string>& fragment() const;

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Uri& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Uri& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(Uri);
};

/// Insert a formatted, human-readable description of the the specified
/// 'object' into the specified 'stream'. Return a reference to the
/// modifiable 'stream'.
///
/// @related ntsa::Uri
bsl::ostream& operator<<(bsl::ostream& stream, const Uri& object);

/// Return true if the specified 'lhs' value has the same value as the
/// specified 'rhs' value, otherwise return false.
///
/// @related ntsa::Uri
bool operator==(const Uri& lhs, const Uri& rhs);

/// Return true if the specified 'lhs' value does not have the same value as
/// the specified 'rhs' value, otherwise return true.
///
/// @related ntsa::Uri
bool operator!=(const Uri& lhs, const Uri& rhs);

/// Return true if the specified 'lhs' value is "less than" the specified
/// 'rhs' value, otherwise return false.
///
/// @related ntsa::Uri
bool operator<(const Uri& lhs, const Uri& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Uri
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Uri& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UriAuthority& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.user());
    hashAppend(algorithm, value.host());
    hashAppend(algorithm, value.port());
    hashAppend(algorithm, value.transport());
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UriParameter& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.name());
    hashAppend(algorithm, value.value());
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UriQuery& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.parameterList());
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Uri& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.scheme());
    hashAppend(algorithm, value.authority());
    hashAppend(algorithm, value.path());
    hashAppend(algorithm, value.query());
    hashAppend(algorithm, value.fragment());
}

}  // close package namespace
}  // close enterprise namespace
#endif
