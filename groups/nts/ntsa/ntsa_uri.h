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
#include <bdlat_typetraits.h>
#include <bdlat_attributeinfo.h>
#include <bdlat_sequencefunctions.h>
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
    static const bdlat_AttributeInfo* lookupAttributeInfo(
        const char* name, int nameLength);

    /// Return the compiler-independant name for this class. 
    static const char CLASS_NAME[19];

    /// The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[4];

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
    enum {
        e_ATTRIBUTE_ID_NAME = 0,
        e_ATTRIBUTE_ID_VALUE = 1 
    };

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
    static const bdlat_AttributeInfo* lookupAttributeInfo(
        const char* name, int nameLength);

    /// Return the compiler-independant name for this class. 
    static const char CLASS_NAME[19];

    /// The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[2];

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
    enum {
        e_ATTRIBUTE_ID_PARAMETER = 0
    };

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
    static const bdlat_AttributeInfo* lookupAttributeInfo(
        const char* name, int nameLength);

    /// Return the compiler-independant name for this class. 
    static const char CLASS_NAME[15];

    /// The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[1];

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
    static const bdlat_AttributeInfo* lookupAttributeInfo(
        const char* name, int nameLength);

    /// Return the compiler-independant name for this class. 
    static const char CLASS_NAME[10];

    /// The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[5];

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
