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
#include <bdlat_attributeinfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

class Uri;
class UriProfile;

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
    enum {
        e_ATTRIBUTE_ID_USER      = 0,
        e_ATTRIBUTE_ID_HOST      = 1,
        e_ATTRIBUTE_ID_PORT      = 2,
        e_ATTRIBUTE_ID_TRANSPORT = 3
    };

    bdlb::NullableValue<bsl::string>            d_user;
    bdlb::NullableValue<ntsa::Host>             d_host;
    bdlb::NullableValue<ntsa::Port>             d_port;
    bdlb::NullableValue<ntsa::Transport::Value> d_transport;
    bdlb::NullableValue<ntsa::TransportSuite>   d_transportSuite;

    friend class Uri;
    friend class UriProfile;

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

    /// Set the host and port in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setEndpoint(const ntsa::Endpoint& value);

    /// Set the transport specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setTransport(ntsa::Transport::Value value);

    /// Set the transport suite specified in the authority portion of the URI
    /// to the specified 'value'. Return the error.
    ntsa::Error setTransportSuite(const ntsa::TransportSuite& value);

    /// Return the user specified in the authority portion of the URI.
    const bdlb::NullableValue<bsl::string>& user() const;

    /// Return the host specified in the authority portion of the URI.
    const bdlb::NullableValue<ntsa::Host>& host() const;

    /// Return the port specified in the authority portion of the URI.
    const bdlb::NullableValue<ntsa::Port>& port() const;

    /// Return the transport specified in the authority portion of the URI.
    const bdlb::NullableValue<ntsa::Transport::Value>& transport() const;

    /// Return the transport suite specified in the authority portion of the
    /// URI.
    const bdlb::NullableValue<ntsa::TransportSuite>& transportSuite() const;

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

    /// Invoke the specified 'manipulator' sequentially on the address of each
    /// (modifiable) attribute of this object, supplying 'manipulator' with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value. Return the value from the last
    /// invocation of 'manipulator' (i.e., the invocation that terminated the
    /// sequence).
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified 'manipulator' on the address of the (modifiable)
    /// attribute indicated by the specified 'id', supplying 'manipulator'
    /// with the corresponding attribute information structure. Return the
    /// value returned from the invocation of 'manipulator' if 'id' identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified 'manipulator' on the address of the (modifiable)
    /// attribute indicated by the specified 'name' of the specified
    /// 'nameLength', supplying 'manipulator' with the corresponding attribute
    /// information structure. Return the value returned from the invocation
    /// of 'manipulator' if 'name' identifies an attribute of this class, and
    /// -1 otherwise.
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator,
                            const char*  name,
                            int          nameLength);

    /// Invoke the specified 'accessor' sequentially on each (non-modifiable)
    /// attribute of this object, supplying 'accessor' with the corresponding
    /// attribute information structure until such invocation returns a
    /// non-zero value. Return the value from the last invocation of
    /// 'accessor' (i.e., the invocation that terminated the sequence).
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified 'accessor' on the (non-modifiable) attribute of
    /// this object indicated by the specified 'id', supplying 'accessor' with
    /// the corresponding attribute information structure. Return the value
    /// returned from the invocation of 'accessor' if 'id' identifies an
    /// attribute of this class, and -1 otherwise.
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified 'accessor' on the (non-modifiable) attribute of
    /// this object indicated by the specified 'name' of the specified
    /// 'nameLength', supplying 'accessor' with the corresponding attribute
    /// information structure. Return the value returned from the invocation
    /// of 'accessor' if 'name' identifies an attribute of this class, and -1
    /// otherwise.
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char* name,
                        int         nameLength) const;

    /// Return attribute information for the attribute indicated by the
    /// specified 'id' if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified 'name' of the specified 'nameLength' if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name,
                                                          int nameLength);

    /// Return the compiler-independant name for this class.
    static const char CLASS_NAME[19];

    /// The attribute info array, indexed by attribute index.
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[4];

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(UriAuthority);
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
    enum { e_ATTRIBUTE_ID_NAME = 0, e_ATTRIBUTE_ID_VALUE = 1 };

    bsl::string                      d_name;
    bdlb::NullableValue<bsl::string> d_value;

    friend class Uri;
    friend class UriProfile;

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

    /// Invoke the specified 'manipulator' sequentially on the address of each
    /// (modifiable) attribute of this object, supplying 'manipulator' with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value. Return the value from the last
    /// invocation of 'manipulator' (i.e., the invocation that terminated the
    /// sequence).
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified 'manipulator' on the address of the (modifiable)
    /// attribute indicated by the specified 'id', supplying 'manipulator'
    /// with the corresponding attribute information structure. Return the
    /// value returned from the invocation of 'manipulator' if 'id' identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified 'manipulator' on the address of the (modifiable)
    /// attribute indicated by the specified 'name' of the specified
    /// 'nameLength', supplying 'manipulator' with the corresponding attribute
    /// information structure. Return the value returned from the invocation
    /// of 'manipulator' if 'name' identifies an attribute of this class, and
    /// -1 otherwise.
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator,
                            const char*  name,
                            int          nameLength);

    /// Invoke the specified 'accessor' sequentially on each (non-modifiable)
    /// attribute of this object, supplying 'accessor' with the corresponding
    /// attribute information structure until such invocation returns a
    /// non-zero value. Return the value from the last invocation of
    /// 'accessor' (i.e., the invocation that terminated the sequence).
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified 'accessor' on the (non-modifiable) attribute of
    /// this object indicated by the specified 'id', supplying 'accessor' with
    /// the corresponding attribute information structure. Return the value
    /// returned from the invocation of 'accessor' if 'id' identifies an
    /// attribute of this class, and -1 otherwise.
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified 'accessor' on the (non-modifiable) attribute of
    /// this object indicated by the specified 'name' of the specified
    /// 'nameLength', supplying 'accessor' with the corresponding attribute
    /// information structure. Return the value returned from the invocation
    /// of 'accessor' if 'name' identifies an attribute of this class, and -1
    /// otherwise.
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char* name,
                        int         nameLength) const;

    /// Return attribute information for the attribute indicated by the
    /// specified 'id' if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified 'name' of the specified 'nameLength' if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name,
                                                          int nameLength);

    /// Return the compiler-independant name for this class.
    static const char CLASS_NAME[19];

    /// The attribute info array, indexed by attribute index.
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[2];

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(UriParameter);
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
    enum { e_ATTRIBUTE_ID_PARAMETER = 0 };

    bsl::vector<ntsa::UriParameter> d_parameterList;

    friend class Uri;
    friend class UriProfile;

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

    /// Invoke the specified 'manipulator' sequentially on the address of each
    /// (modifiable) attribute of this object, supplying 'manipulator' with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value. Return the value from the last
    /// invocation of 'manipulator' (i.e., the invocation that terminated the
    /// sequence).
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified 'manipulator' on the address of the (modifiable)
    /// attribute indicated by the specified 'id', supplying 'manipulator'
    /// with the corresponding attribute information structure. Return the
    /// value returned from the invocation of 'manipulator' if 'id' identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified 'manipulator' on the address of the (modifiable)
    /// attribute indicated by the specified 'name' of the specified
    /// 'nameLength', supplying 'manipulator' with the corresponding attribute
    /// information structure. Return the value returned from the invocation
    /// of 'manipulator' if 'name' identifies an attribute of this class, and
    /// -1 otherwise.
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator,
                            const char*  name,
                            int          nameLength);

    /// Invoke the specified 'accessor' sequentially on each (non-modifiable)
    /// attribute of this object, supplying 'accessor' with the corresponding
    /// attribute information structure until such invocation returns a
    /// non-zero value. Return the value from the last invocation of
    /// 'accessor' (i.e., the invocation that terminated the sequence).
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified 'accessor' on the (non-modifiable) attribute of
    /// this object indicated by the specified 'id', supplying 'accessor' with
    /// the corresponding attribute information structure. Return the value
    /// returned from the invocation of 'accessor' if 'id' identifies an
    /// attribute of this class, and -1 otherwise.
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified 'accessor' on the (non-modifiable) attribute of
    /// this object indicated by the specified 'name' of the specified
    /// 'nameLength', supplying 'accessor' with the corresponding attribute
    /// information structure. Return the value returned from the invocation
    /// of 'accessor' if 'name' identifies an attribute of this class, and -1
    /// otherwise.
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char* name,
                        int         nameLength) const;

    /// Return attribute information for the attribute indicated by the
    /// specified 'id' if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified 'name' of the specified 'nameLength' if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name,
                                                          int nameLength);

    /// Return the compiler-independant name for this class.
    static const char CLASS_NAME[15];

    /// The attribute info array, indexed by attribute index.
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[1];

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(UriQuery);
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

/// Describe a URI profile entry.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b scheme:
/// The scheme name.
///
/// @li @b canonicalScheme:
/// The canonical scheme name.
///
/// @li @b transportSecurity:
/// The transport security protocol (e.g., TLS or SSH).
///
/// @li @b transportProtocol:
/// The transport security protocol (e.g., TCP or UDP or the intrinsic 
/// transport of the local domain).
///
/// @li @b transportDomain:
/// The transport domain (e.g., IPv4 or IPv6 or the local domain).
///
/// @li @b transportMode:
/// The transport mode (e.g., stream or datagram).
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class UriProfileEntry
{
    bsl::string          d_scheme;
    bsl::string          d_canonicalScheme;
    ntsa::TransportSuite d_transportSuite;

  public:
    /// Create a new URI profile entry having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit UriProfileEntry(bslma::Allocator* basicAllocator = 0);

    /// Create a new URI profile entry having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    UriProfileEntry(const UriProfileEntry& original,
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~UriProfileEntry();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UriProfileEntry& operator=(const UriProfileEntry& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set scheme name to the specified 'value'.
    void setScheme(const bslstl::StringRef& value);

    /// Set canonical scheme name to the specified 'value'.
    void setCanonicalScheme(const bslstl::StringRef& value);

    /// Set the transport suite to the specified 'value'.
    void setTransportSuite(const ntsa::TransportSuite& value);

    /// Return the scheme name.
    const bsl::string& scheme() const;

    /// Return the canonical scheme name.
    const bsl::string& canonicalScheme() const;

    /// Return the transport suite.
    const ntsa::TransportSuite& transportSuite() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const UriProfileEntry& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const UriProfileEntry& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(UriProfileEntry);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::UriProfileEntry
bsl::ostream& operator<<(bsl::ostream& stream, const UriProfileEntry& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::UriProfileEntry
bool operator==(const UriProfileEntry& lhs, const UriProfileEntry& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::UriProfileEntry
bool operator!=(const UriProfileEntry& lhs, const UriProfileEntry& rhs);

/// Return true if the value of the specified 'lhs' is less than the value of
/// the specified 'rhs', otherwise return false.
///
/// @related ntsa::UriProfileEntry
bool operator<(const UriProfileEntry& lhs, const UriProfileEntry& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::UriProfileEntry
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UriProfileEntry& value);

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void UriProfileEntry::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_scheme);
    hashAppend(algorithm, d_canonicalScheme);
    hashAppend(algorithm, d_transportSuite);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&        algorithm,
                              const UriProfileEntry& value)
{
    value.hash(algorithm);
}

/// Provide a Uniform Resource Identifier (URI) profile to aid a parser in
/// disambiguating the form and components of a URI.
class UriProfile
{
    typedef ntsa::TransportProtocol TP;
    typedef ntsa::TransportDomain   TD;
    typedef ntsa::TransportMode     TM;

    typedef bsl::unordered_map<bsl::string, ntsa::UriProfileEntry> EntryMap;

    struct Data {
        const char*                    scheme;
        const char*                    canonicalScheme;
        ntsa::TransportProtocol::Value transportProtocol;
        ntsa::TransportDomain::Value   transportDomain;
        ntsa::TransportMode::Value     transportMode;
    };

    EntryMap          d_entryMap;
    bslma::Allocator* d_allocator_p;

    static const Data k_DATA[13];

private:
    UriProfile(const UriProfile&) BSLS_KEYWORD_DELETED;
    UriProfile& operator=(const UriProfile&) BSLS_KEYWORD_DELETED;

public:
    /// Create a new URI profile. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. 
    explicit UriProfile(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~UriProfile();

    /// Register default schemes unafilliated with any application protocol.
    /// Return the error.
    ntsa::Error registerImplicit();

    /// Register default schemes affiliated with the specified 'application'
    /// protocol. Return the error.
    ntsa::Error registerImplicit(const bsl::string& application);
                        
    /// Register the specified 'application' protocol, automatically generating
    /// and registering all standard schemes and subordinate schemes. Return
    /// the error.
    ntsa::Error registerExplicit(const bsl::string& application);

    /// Register the specified 'application' protocol with the specified
    /// 'transportSecurity', automatically generating and registering all
    /// standard schemes and subordinate schemes. Return the error.
    ntsa::Error registerExplicit(
        const bsl::string&             application, 
        ntsa::TransportSecurity::Value transportSecurity);

    /// Register the specified 'entry'. Return the error.
    ntsa::Error registerEntry(const ntsa::UriProfileEntry& entry);

    /// Register the specified 'scheme' translated to the specified
    /// 'canonicalScheme' for the specified 'application' protocl that uses the
    /// specified 'transportSecurity', 'transportProtocol', 'transportDomain',
    /// and 'transportMode'. Return the error.
    ntsa::Error registerEntry(
        const bslstl::StringRef&       scheme,
        const bslstl::StringRef&       canonicalScheme,
        const bslstl::StringRef&       application,
        ntsa::TransportSecurity::Value transportSecurity,
        ntsa::TransportProtocol::Value transportProtocol,
        ntsa::TransportDomain::Value   transportDomain,
        ntsa::TransportMode::Value     transportMode);

    /// Parse the specified 'scheme' and load its canonical form into the
    /// specified 'canonicalScheme'. Return the result.
    ntsa::Error parseScheme(
        bsl::string*             canonicalScheme,
        const bslstl::StringRef& scheme) const;

    /// Parse the specified 'scheme', load its canonical form into the
    /// specified 'canonicalScheme', and load its transport suite into the 
    /// specified 'transportSuite'. Return the result.
    ntsa::Error parseScheme(
        bsl::string*                    canonicalScheme,
        ntsa::TransportSuite*           transportSuite,
        const bslstl::StringRef&        scheme) const;

    /// Parse the specified 'scheme', load its canonical form into the
    /// specified 'canonicalScheme', and load its transport suite into the 
    /// specified 'transportSuite'. Return the result.
    ntsa::Error parseScheme(
        bsl::string*                               canonicalScheme,
        bdlb::NullableValue<ntsa::TransportSuite>* transportSuite,
        const bslstl::StringRef&                   scheme) const;

    /// Normalize the specified 'uri'. Return the error. 
    ntsa::Error normalize(ntsa::Uri* uri);
};

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
    enum {
        e_ATTRIBUTE_ID_SCHEME    = 0,
        e_ATTRIBUTE_ID_AUTHORITY = 1,
        e_ATTRIBUTE_ID_PATH      = 2,
        e_ATTRIBUTE_ID_QUERY     = 3,
        e_ATTRIBUTE_ID_FRAGMENT  = 4
    };

    bdlb::NullableValue<bsl::string>        d_scheme;
    bdlb::NullableValue<ntsa::UriAuthority> d_authority;
    bdlb::NullableValue<bsl::string>        d_path;
    bdlb::NullableValue<ntsa::UriQuery>     d_query;
    bdlb::NullableValue<bsl::string>        d_fragment;

    friend class UriProfile;

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

    /// Set the host and port in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setEndpoint(const ntsa::Endpoint& value);

    /// Set the transport specified in the authority portion of the URI to the
    /// specified 'value'. Return the error.
    ntsa::Error setTransport(ntsa::Transport::Value value);

    /// Set the transport suite specified in the authority portion of the URI
    /// to the specified 'value'. Return the error.
    ntsa::Error setTransportSuite(const ntsa::TransportSuite& value);

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

    /// Set the value of this object from the value parsed from its textual
    /// representation using the specified 'profile'. Return true if the 'text'
    /// is in a valid format and was parsed successfully, otherwise return
    /// false.
    bool parse(const bslstl::StringRef& text, const ntsa::UriProfile& profile);

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

    /// Write an explanation of the URI to the specified 'stream' for debugging
    /// purposes.
    void dump(const bsl::string& input, bsl::ostream& stream) const;

    /// Invoke the specified 'manipulator' sequentially on the address of each
    /// (modifiable) attribute of this object, supplying 'manipulator' with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value. Return the value from the last
    /// invocation of 'manipulator' (i.e., the invocation that terminated the
    /// sequence).
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified 'manipulator' on the address of the (modifiable)
    /// attribute indicated by the specified 'id', supplying 'manipulator'
    /// with the corresponding attribute information structure. Return the
    /// value returned from the invocation of 'manipulator' if 'id' identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified 'manipulator' on the address of the (modifiable)
    /// attribute indicated by the specified 'name' of the specified
    /// 'nameLength', supplying 'manipulator' with the corresponding attribute
    /// information structure. Return the value returned from the invocation
    /// of 'manipulator' if 'name' identifies an attribute of this class, and
    /// -1 otherwise.
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator,
                            const char*  name,
                            int          nameLength);

    /// Invoke the specified 'accessor' sequentially on each (non-modifiable)
    /// attribute of this object, supplying 'accessor' with the corresponding
    /// attribute information structure until such invocation returns a
    /// non-zero value. Return the value from the last invocation of
    /// 'accessor' (i.e., the invocation that terminated the sequence).
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified 'accessor' on the (non-modifiable) attribute of
    /// this object indicated by the specified 'id', supplying 'accessor' with
    /// the corresponding attribute information structure. Return the value
    /// returned from the invocation of 'accessor' if 'id' identifies an
    /// attribute of this class, and -1 otherwise.
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified 'accessor' on the (non-modifiable) attribute of
    /// this object indicated by the specified 'name' of the specified
    /// 'nameLength', supplying 'accessor' with the corresponding attribute
    /// information structure. Return the value returned from the invocation
    /// of 'accessor' if 'name' identifies an attribute of this class, and -1
    /// otherwise.
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char* name,
                        int         nameLength) const;

    /// Return attribute information for the attribute indicated by the
    /// specified 'id' if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified 'name' of the specified 'nameLength' if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name,
                                                          int nameLength);

    /// Return the compiler-independant name for this class.
    static const char CLASS_NAME[10];

    /// The attribute info array, indexed by attribute index.
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[5];

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Uri);
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
    hashAppend(algorithm, value.transportSuite());
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

template <typename MANIPULATOR>
int UriParameter::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_NAME);
    if (rc != 0) {
        return rc;
    }

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_VALUE);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int UriParameter::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    int rc;

    if (id == e_ATTRIBUTE_ID_NAME) {
        rc = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_VALUE) {
        rc = manipulator(&d_value, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else {
        return -1;
    }

    return 0;
}

template <typename MANIPULATOR>
int UriParameter::manipulateAttribute(MANIPULATOR& manipulator,
                                      const char*  name,
                                      int          nameLength)
{
    const bdlat_AttributeInfo* info =
        ntsa::UriParameter::lookupAttributeInfo(name, nameLength);
    if (info == 0) {
        return -1;
    }

    return this->manipulateAttribute(manipulator, info->d_id);
}

template <typename ACCESSOR>
int UriParameter::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_NAME);
    if (rc != 0) {
        return rc;
    }

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_VALUE);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int UriParameter::accessAttribute(ACCESSOR& accessor, int id) const
{
    int rc;

    if (id == e_ATTRIBUTE_ID_NAME) {
        rc = accessor(d_name, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_VALUE) {
        rc = accessor(d_value, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else {
        return -1;
    }

    return 0;
}

template <typename ACCESSOR>
int UriParameter::accessAttribute(ACCESSOR&   accessor,
                                  const char* name,
                                  int         nameLength) const
{
    const bdlat_AttributeInfo* info =
        ntsa::UriParameter::lookupAttributeInfo(name, nameLength);
    if (info == 0) {
        return -1;
    }

    return this->accessAttribute(accessor, info->d_id);
}

template <typename MANIPULATOR>
int UriQuery::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_PARAMETER);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int UriQuery::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    int rc;

    if (id == e_ATTRIBUTE_ID_PARAMETER) {
        rc = manipulator(&d_parameterList, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else {
        return -1;
    }

    return 0;
}

template <typename MANIPULATOR>
int UriQuery::manipulateAttribute(MANIPULATOR& manipulator,
                                  const char*  name,
                                  int          nameLength)
{
    const bdlat_AttributeInfo* info =
        ntsa::UriQuery::lookupAttributeInfo(name, nameLength);
    if (info == 0) {
        return -1;
    }

    return this->manipulateAttribute(manipulator, info->d_id);
}

template <typename ACCESSOR>
int UriQuery::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_PARAMETER);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int UriQuery::accessAttribute(ACCESSOR& accessor, int id) const
{
    int rc;

    if (id == e_ATTRIBUTE_ID_PARAMETER) {
        rc = accessor(d_parameterList, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else {
        return -1;
    }

    return 0;
}

template <typename ACCESSOR>
int UriQuery::accessAttribute(ACCESSOR&   accessor,
                              const char* name,
                              int         nameLength) const
{
    const bdlat_AttributeInfo* info =
        ntsa::UriQuery::lookupAttributeInfo(name, nameLength);
    if (info == 0) {
        return -1;
    }

    return this->accessAttribute(accessor, info->d_id);
}

template <typename MANIPULATOR>
int UriAuthority::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_USER);
    if (rc != 0) {
        return rc;
    }

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_HOST);
    if (rc != 0) {
        return rc;
    }

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_PORT);
    if (rc != 0) {
        return rc;
    }

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_TRANSPORT);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int UriAuthority::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    int rc;

    if (id == e_ATTRIBUTE_ID_USER) {
        rc = manipulator(&d_user, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_HOST) {
        rc = manipulator(&d_host, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_PORT) {
        rc = manipulator(&d_port, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_TRANSPORT) {
        rc = manipulator(&d_transport, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else {
        return -1;
    }

    return 0;
}

template <typename MANIPULATOR>
int UriAuthority::manipulateAttribute(MANIPULATOR& manipulator,
                                      const char*  name,
                                      int          nameLength)
{
    const bdlat_AttributeInfo* info =
        ntsa::UriAuthority::lookupAttributeInfo(name, nameLength);
    if (info == 0) {
        return -1;
    }

    return this->manipulateAttribute(manipulator, info->d_id);
}

template <typename ACCESSOR>
int UriAuthority::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_USER);
    if (rc != 0) {
        return rc;
    }

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_HOST);
    if (rc != 0) {
        return rc;
    }

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_PORT);
    if (rc != 0) {
        return rc;
    }

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_TRANSPORT);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int UriAuthority::accessAttribute(ACCESSOR& accessor, int id) const
{
    int rc;

    if (id == e_ATTRIBUTE_ID_USER) {
        rc = accessor(d_user, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_HOST) {
        rc = accessor(d_host, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_PORT) {
        rc = accessor(d_port, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_TRANSPORT) {
        rc = accessor(d_transport, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else {
        return -1;
    }

    return 0;
}

template <typename ACCESSOR>
int UriAuthority::accessAttribute(ACCESSOR&   accessor,
                                  const char* name,
                                  int         nameLength) const
{
    const bdlat_AttributeInfo* info =
        ntsa::UriAuthority::lookupAttributeInfo(name, nameLength);
    if (info == 0) {
        return -1;
    }

    return this->accessAttribute(accessor, info->d_id);
}

template <typename MANIPULATOR>
int Uri::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_SCHEME);
    if (rc != 0) {
        return rc;
    }

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_AUTHORITY);
    if (rc != 0) {
        return rc;
    }

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_PATH);
    if (rc != 0) {
        return rc;
    }

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_QUERY);
    if (rc != 0) {
        return rc;
    }

    rc = this->manipulateAttribute(manipulator, e_ATTRIBUTE_ID_FRAGMENT);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int Uri::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    int rc;

    if (id == e_ATTRIBUTE_ID_SCHEME) {
        rc = manipulator(&d_scheme, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_AUTHORITY) {
        rc = manipulator(&d_authority, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_PATH) {
        rc = manipulator(&d_path, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_QUERY) {
        rc = manipulator(&d_query, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_FRAGMENT) {
        rc = manipulator(&d_fragment, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else {
        return -1;
    }

    return 0;
}

template <typename MANIPULATOR>
int Uri::manipulateAttribute(MANIPULATOR& manipulator,
                             const char*  name,
                             int          nameLength)
{
    const bdlat_AttributeInfo* info =
        ntsa::Uri::lookupAttributeInfo(name, nameLength);
    if (info == 0) {
        return -1;
    }

    return this->manipulateAttribute(manipulator, info->d_id);
}

template <typename ACCESSOR>
int Uri::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_SCHEME);
    if (rc != 0) {
        return rc;
    }

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_AUTHORITY);
    if (rc != 0) {
        return rc;
    }

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_PATH);
    if (rc != 0) {
        return rc;
    }

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_QUERY);
    if (rc != 0) {
        return rc;
    }

    rc = this->accessAttribute(accessor, e_ATTRIBUTE_ID_FRAGMENT);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int Uri::accessAttribute(ACCESSOR& accessor, int id) const
{
    int rc;

    if (id == e_ATTRIBUTE_ID_SCHEME) {
        rc = accessor(d_scheme, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_AUTHORITY) {
        rc = accessor(d_authority, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_PATH) {
        rc = accessor(d_path, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_QUERY) {
        rc = accessor(d_query, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else if (id == e_ATTRIBUTE_ID_FRAGMENT) {
        rc = accessor(d_fragment, ATTRIBUTE_INFO_ARRAY[id]);
        if (rc != 0) {
            return rc;
        }
    }
    else {
        return -1;
    }

    return 0;
}

template <typename ACCESSOR>
int Uri::accessAttribute(ACCESSOR&   accessor,
                         const char* name,
                         int         nameLength) const
{
    const bdlat_AttributeInfo* info =
        ntsa::Uri::lookupAttributeInfo(name, nameLength);
    if (info == 0) {
        return -1;
    }

    return this->accessAttribute(accessor, info->d_id);
}

}  // close package namespace

BDLAT_DECL_SEQUENCE_TRAITS(ntsa::UriAuthority)
BDLAT_DECL_SEQUENCE_TRAITS(ntsa::UriParameter)
BDLAT_DECL_SEQUENCE_TRAITS(ntsa::UriQuery)
BDLAT_DECL_SEQUENCE_TRAITS(ntsa::Uri)

}  // close enterprise namespace
#endif
