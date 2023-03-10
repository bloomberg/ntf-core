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

#ifndef INCLUDED_NTCDNS_VOCABULARY
#define INCLUDED_NTCDNS_VOCABULARY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

/// Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_enumeratorinfo.h>

#include <bdlat_typetraits.h>

#include <bslh_hash.h>
#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlb_nullablevalue.h>

#include <bsl_string.h>

#include <bsl_vector.h>

#include <bsls_types.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace bslma {
class Allocator;
}

namespace ntcdns {
class HostEntry;
}
namespace ntcdns {
class Label;
}
namespace ntcdns {
class NameServerAddress;
}
namespace ntcdns {
class PortEntry;
}
namespace ntcdns {
class ResourceRecordDataA;
}
namespace ntcdns {
class ResourceRecordDataAAAA;
}
namespace ntcdns {
class ResourceRecordDataCname;
}
namespace ntcdns {
class ResourceRecordDataHinfo;
}
namespace ntcdns {
class ResourceRecordDataMx;
}
namespace ntcdns {
class ResourceRecordDataNs;
}
namespace ntcdns {
class ResourceRecordDataPtr;
}
namespace ntcdns {
class ResourceRecordDataRaw;
}
namespace ntcdns {
class ResourceRecordDataSoa;
}
namespace ntcdns {
class ResourceRecordDataSvr;
}
namespace ntcdns {
class ResourceRecordDataTxt;
}
namespace ntcdns {
class ResourceRecordDataWks;
}
namespace ntcdns {
class SortListItem;
}
namespace ntcdns {
class HostDatabaseConfig;
}
namespace ntcdns {
class NameServerConfig;
}
namespace ntcdns {
class PortDatabaseConfig;
}
namespace ntcdns {
class ResourceRecordData;
}
namespace ntcdns {
class SortList;
}
namespace ntcdns {
class ClientConfig;
}
namespace ntcdns {
class HostDatabaseConfigSpec;
}
namespace ntcdns {
class PortDatabaseConfigSpec;
}
namespace ntcdns {
class ServerConfig;
}
namespace ntcdns {
class ClientConfigSpec;
}
namespace ntcdns {
class ResolverConfig;
}
namespace ntcdns {

/// This struct enumerates the CLASS fields that appear in resource records.
///  CLASS values are a subset of QCLASS values.
struct Classification {
  public:
    enum Value {
        e_INTERNET = 1
        // The Internet class.
        ,
        e_CSNET = 2
        // The CSNET class.
        ,
        e_CH = 3
        // The CHAOS class.
        ,
        e_HS = 4
        // The Hesiod class.
        ,
        e_ANY = 255
        // Any class.
    };

    enum { NUM_ENUMERATORS = 5 };

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string' of the specified 'stringLength'.  Return 0 on
    /// success, and a non-zero value with no effect on 'result' otherwise
    /// (i.e., 'string' does not match any enumerator).
    static int fromString(Value* result, const char* string, int stringLength);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bsl::string& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of
    /// the specified enumeration 'value'.  Return a reference to
    /// the modifiable 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&         stream,
                                Classification::Value rhs);

}  // close package namespace

BDLAT_DECL_ENUMERATION_TRAITS(ntcdns::Classification)

namespace ntcdns {

/// This struct enumerates the values of the DNS header "QR" field.
struct Direction {
  public:
    enum Value {
        e_REQUEST = 0
        // The message is a request.
        ,
        e_RESPONSE = 1
        // The message is a response.
    };

    enum { NUM_ENUMERATORS = 2 };

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string' of the specified 'stringLength'.  Return 0 on
    /// success, and a non-zero value with no effect on 'result' otherwise
    /// (i.e., 'string' does not match any enumerator).
    static int fromString(Value* result, const char* string, int stringLength);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bsl::string& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of
    /// the specified enumeration 'value'.  Return a reference to
    /// the modifiable 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, Direction::Value rhs);

}  // close package namespace

BDLAT_DECL_ENUMERATION_TRAITS(ntcdns::Direction)

namespace ntcdns {

/// This struct enumerates the values of a DNS header "RCODE" field.
struct Error {
  public:
    enum Value {
        e_OK = 0
        // The message is successful.
        ,
        e_FORMAT_ERROR = 1
        // The name server was unable to interpret the query.
        ,
        e_SERVER_FAILURE = 2
        // The name server was unable to process the query due to a problem
        // with the name server.
        ,
        e_NAME_ERROR = 3
        // The domain name referenced in the query does not exist.
        ,
        e_NOT_IMPLEMENTED = 4
        // The name server does not support the requested kind of query.
        ,
        e_REFUSED = 5
        // The name server refused to perform the specified operation for
        // policy reasons.
    };

    enum { NUM_ENUMERATORS = 6 };

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string' of the specified 'stringLength'.  Return 0 on
    /// success, and a non-zero value with no effect on 'result' otherwise
    /// (i.e., 'string' does not match any enumerator).
    static int fromString(Value* result, const char* string, int stringLength);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bsl::string& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of
    /// the specified enumeration 'value'.  Return a reference to
    /// the modifiable 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, Error::Value rhs);

}  // close package namespace

BDLAT_DECL_ENUMERATION_TRAITS(ntcdns::Error)

namespace ntcdns {

/// This class describes an association between an address, its canonical
/// hostname, and its aliases.
class HostEntry
{
    // The aliases.
    bsl::vector<bsl::string> d_aliases;

    // The IP address.
    bsl::string d_address;

    // The canonical hostname.
    bsl::string d_canonicalHostname;

    // The expiration deadline, in absolute seconds since the Unix epoch.
    bdlb::NullableValue<bsl::uint64_t> d_expiration;

  public:
  public:
    /// Create an object of type 'HostEntry' having the default value.  Use
    /// the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit HostEntry(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'HostEntry' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    HostEntry(const HostEntry& original, bslma::Allocator* basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'HostEntry' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    HostEntry(HostEntry&& original) noexcept;

    /// Create an object of type 'HostEntry' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    HostEntry(HostEntry&& original, bslma::Allocator* basicAllocator);
#endif

    /// Destroy this object.
    ~HostEntry();

    /// Assign to this object the value of the specified 'rhs' object.
    HostEntry& operator=(const HostEntry& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    HostEntry& operator=(HostEntry&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Address" attribute of this
    /// object.
    bsl::string& address();

    /// Return a reference to the modifiable "CanonicalHostname" attribute
    /// of this object.
    bsl::string& canonicalHostname();

    /// Return a reference to the modifiable "Aliases" attribute of this
    /// object.
    bsl::vector<bsl::string>& aliases();

    /// Return a reference to the modifiable "Expiration" attribute of this
    /// object.
    bdlb::NullableValue<bsl::uint64_t>& expiration();

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

    /// Return a reference offering non-modifiable access to the "Address"
    /// attribute of this object.
    const bsl::string& address() const;

    /// Return a reference offering non-modifiable access to the
    /// "CanonicalHostname" attribute of this object.
    const bsl::string& canonicalHostname() const;

    /// Return a reference offering non-modifiable access to the "Aliases"
    /// attribute of this object.
    const bsl::vector<bsl::string>& aliases() const;

    /// Return a reference offering non-modifiable access to the
    /// "Expiration" attribute of this object.
    const bdlb::NullableValue<bsl::uint64_t>& expiration() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const HostEntry& lhs, const HostEntry& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const HostEntry& lhs, const HostEntry& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, const HostEntry& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'HostEntry'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::HostEntry& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ntcdns::HostEntry)

namespace ntcdns {

/// This class describes a union of a potentially compressed label within a
/// domain name.
class Label
{
    union {
        /// The label is uncompressed text.
        bsls::ObjectBuffer<bsl::string> d_text;

        /// The label is an offset into the DNS header where the
        /// uncompressed, length-prefixed text begins.
        bsls::ObjectBuffer<unsigned short> d_offset;
    };

    int               d_selectionId;
    bslma::Allocator* d_allocator_p;

  public:
    enum {
        SELECTION_ID_UNDEFINED = -1,
        SELECTION_ID_TEXT      = 0,
        SELECTION_ID_OFFSET    = 1
    };

    /// Create an object of type 'Label' having the default value.  Use the
    /// optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Label(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'Label' having the value of the specified
    /// 'original' object.  Use the optionally specified 'basicAllocator' to
    /// supply memory.  If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Label(const Label& original, bslma::Allocator* basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'Label' having the value of the specified
    /// 'original' object.  After performing this action, the 'original'
    /// object will be left in a valid, but unspecified state.
    Label(Label&& original) noexcept;

    /// Create an object of type 'Label' having the value of the specified
    /// 'original' object.  After performing this action, the 'original'
    /// object will be left in a valid, but unspecified state.  Use the
    /// optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    Label(Label&& original, bslma::Allocator* basicAllocator);
#endif

    /// Destroy this object.
    ~Label();

    /// Assign to this object the value of the specified 'rhs' object.
    Label& operator=(const Label& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    Label& operator=(Label&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    bsl::string& makeText();
    bsl::string& makeText(const bsl::string& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::string& makeText(bsl::string&& value);
#endif
    // Set the value of this object to be a "Text" value.  Optionally
    // specify the 'value' of the "Text".  If 'value' is not specified, the
    // default "Text" value is used.

    /// Set the value of this object to be a "Offset" value.  Optionally
    /// specify the 'value' of the "Offset".  If 'value' is not specified,
    /// the default "Offset" value is used.
    unsigned short& makeOffset();
    unsigned short& makeOffset(unsigned short value);

    /// Return a reference to the modifiable "Text" selection of this object
    /// if "Text" is the current selection.  The behavior is undefined
    /// unless "Text" is the selection of this object.
    bsl::string& text();

    /// Return a reference to the modifiable "Offset" selection of this
    /// object if "Offset" is the current selection.  The behavior is
    /// undefined unless "Offset" is the selection of this object.
    unsigned short& offset();

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

    /// Return the id of the current selection if the selection is defined,
    /// and -1 otherwise.
    int selectionId() const;

    /// Return a reference to the non-modifiable "Text" selection of this
    /// object if "Text" is the current selection.  The behavior is
    /// undefined unless "Text" is the selection of this object.
    const bsl::string& text() const;

    /// Return a reference to the non-modifiable "Offset" selection of this
    /// object if "Offset" is the current selection.  The behavior is
    /// undefined unless "Offset" is the selection of this object.
    const unsigned short& offset() const;

    /// Return 'true' if the value of this object is a "Text" value, and
    /// return 'false' otherwise.
    bool isTextValue() const;

    /// Return 'true' if the value of this object is a "Offset" value, and
    /// return 'false' otherwise.
    bool isOffsetValue() const;

    /// Return 'true' if the value of this object is undefined, and 'false'
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char* selectionName() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' objects have the same
/// value, and 'false' otherwise.  Two 'Label' objects have the same
/// value if either the selections in both objects have the same ids and
/// the same values, or both selections are undefined.
inline bool operator==(const Label& lhs, const Label& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
/// same values, as determined by 'operator==', and 'false' otherwise.
inline bool operator!=(const Label& lhs, const Label& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, const Label& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'Label'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::Label& object);

}  // close package namespace

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ntcdns::Label)

namespace ntcdns {

/// This struct describes the address of a name server.
class NameServerAddress
{
    // The host.
    bsl::string d_host;

    // The port.  If unspecified, the port is the default DNS port 53.
    bdlb::NullableValue<unsigned short> d_port;

  public:
  public:
    /// Create an object of type 'NameServerAddress' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit NameServerAddress(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'NameServerAddress' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    NameServerAddress(const NameServerAddress& original,
                      bslma::Allocator*        basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'NameServerAddress' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    NameServerAddress(NameServerAddress&& original) noexcept;

    /// Create an object of type 'NameServerAddress' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    NameServerAddress(NameServerAddress&& original,
                      bslma::Allocator*   basicAllocator);
#endif

    /// Destroy this object.
    ~NameServerAddress();

    /// Assign to this object the value of the specified 'rhs' object.
    NameServerAddress& operator=(const NameServerAddress& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    NameServerAddress& operator=(NameServerAddress&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Host" attribute of this
    /// object.
    bsl::string& host();

    /// Return a reference to the modifiable "Port" attribute of this
    /// object.
    bdlb::NullableValue<unsigned short>& port();

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

    /// Return a reference offering non-modifiable access to the "Host"
    /// attribute of this object.
    const bsl::string& host() const;

    /// Return a reference offering non-modifiable access to the "Port"
    /// attribute of this object.
    const bdlb::NullableValue<unsigned short>& port() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const NameServerAddress& lhs,
                       const NameServerAddress& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const NameServerAddress& lhs,
                       const NameServerAddress& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&            stream,
                                const NameServerAddress& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'NameServerAddress'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                  hashAlg,
                const ntcdns::NameServerAddress& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::NameServerAddress)

namespace ntcdns {

/// This struct enumerates the values of the DNS header "OPCODE" field.
struct Operation {
  public:
    enum Value {
        e_STANDARD = 0
        // The message is a standard query.
        ,
        e_INVERSE = 1
        // The message is an inverse query.
        ,
        e_STATUS = 2
        // The message indicates server side status.
    };

    enum { NUM_ENUMERATORS = 3 };

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string' of the specified 'stringLength'.  Return 0 on
    /// success, and a non-zero value with no effect on 'result' otherwise
    /// (i.e., 'string' does not match any enumerator).
    static int fromString(Value* result, const char* string, int stringLength);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bsl::string& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of
    /// the specified enumeration 'value'.  Return a reference to
    /// the modifiable 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, Operation::Value rhs);

}  // close package namespace

BDLAT_DECL_ENUMERATION_TRAITS(ntcdns::Operation)

namespace ntcdns {

/// This class describes an association between a service, protocol, and
/// port.
class PortEntry
{
    // The service
    bsl::string d_service;

    // The protocol.
    bsl::string d_protocol;

    // The expiration deadline, in absolute seconds since the Unix epoch.
    bdlb::NullableValue<bsl::uint64_t> d_expiration;

    // The port.
    unsigned short d_port;

  public:
  public:
    /// Create an object of type 'PortEntry' having the default value.  Use
    /// the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit PortEntry(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'PortEntry' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    PortEntry(const PortEntry& original, bslma::Allocator* basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'PortEntry' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    PortEntry(PortEntry&& original) noexcept;

    /// Create an object of type 'PortEntry' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    PortEntry(PortEntry&& original, bslma::Allocator* basicAllocator);
#endif

    /// Destroy this object.
    ~PortEntry();

    /// Assign to this object the value of the specified 'rhs' object.
    PortEntry& operator=(const PortEntry& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    PortEntry& operator=(PortEntry&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Service" attribute of this
    /// object.
    bsl::string& service();

    /// Return a reference to the modifiable "Protocol" attribute of this
    /// object.
    bsl::string& protocol();

    /// Return a reference to the modifiable "Port" attribute of this
    /// object.
    unsigned short& port();

    /// Return a reference to the modifiable "Expiration" attribute of this
    /// object.
    bdlb::NullableValue<bsl::uint64_t>& expiration();

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

    /// Return a reference offering non-modifiable access to the "Service"
    /// attribute of this object.
    const bsl::string& service() const;

    /// Return a reference offering non-modifiable access to the "Protocol"
    /// attribute of this object.
    const bsl::string& protocol() const;

    /// Return the value of the "Port" attribute of this object.
    unsigned short port() const;

    /// Return a reference offering non-modifiable access to the
    /// "Expiration" attribute of this object.
    const bdlb::NullableValue<bsl::uint64_t>& expiration() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const PortEntry& lhs, const PortEntry& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const PortEntry& lhs, const PortEntry& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, const PortEntry& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'PortEntry'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::PortEntry& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ntcdns::PortEntry)

namespace ntcdns {

/// This class describes resource record data in the A format.
class ResourceRecordDataA
{
    // The 32-bit encoded IPv4 address in network byte order.
    unsigned int d_address;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataA' having the default
    /// value.
    ResourceRecordDataA();

    /// Create an object of type 'ResourceRecordDataA' having the value of
    /// the specified 'original' object.
    ResourceRecordDataA(const ResourceRecordDataA& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataA(ResourceRecordDataA&& original) = default;
    // Create an object of type 'ResourceRecordDataA' having the value of
    // the specified 'original' object.  After performing this action, the
    // 'original' object will be left in a valid, but unspecified state.
#endif

    /// Destroy this object.
    ~ResourceRecordDataA();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataA& operator=(const ResourceRecordDataA& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataA& operator=(ResourceRecordDataA&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Address" attribute of this
    /// object.
    unsigned int& address();

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

    /// Return the value of the "Address" attribute of this object.
    unsigned int address() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataA& lhs,
                       const ResourceRecordDataA& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataA& lhs,
                       const ResourceRecordDataA& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&              stream,
                                const ResourceRecordDataA& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataA'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    hashAlg,
                const ntcdns::ResourceRecordDataA& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(ntcdns::ResourceRecordDataA)

namespace ntcdns {

/// This class describes resource record data in the AAAA format.
class ResourceRecordDataAAAA
{
    // The 32-bit index 0 of the 128-bit encoded IPv6 address in network
    // byte order.
    unsigned int d_address0;

    // The 32-bit index 1 of the 128-bit encoded IPv6 address in network
    // byte order.
    unsigned int d_address1;

    // The 32-bit index 2 of the 128-bit encoded IPv6 address in network
    // byte order.
    unsigned int d_address2;

    // The 32-bit index 3 of the 128-bit encoded IPv6 address in network
    // byte order.
    unsigned int d_address3;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataAAAA' having the default
    /// value.
    ResourceRecordDataAAAA();

    /// Create an object of type 'ResourceRecordDataAAAA' having the value
    /// of the specified 'original' object.
    ResourceRecordDataAAAA(const ResourceRecordDataAAAA& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataAAAA(ResourceRecordDataAAAA&& original) = default;
    // Create an object of type 'ResourceRecordDataAAAA' having the value
    // of the specified 'original' object.  After performing this action,
    // the 'original' object will be left in a valid, but unspecified
    // state.
#endif

    /// Destroy this object.
    ~ResourceRecordDataAAAA();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataAAAA& operator=(const ResourceRecordDataAAAA& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataAAAA& operator=(ResourceRecordDataAAAA&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Address0" attribute of this
    /// object.
    unsigned int& address0();

    /// Return a reference to the modifiable "Address1" attribute of this
    /// object.
    unsigned int& address1();

    /// Return a reference to the modifiable "Address2" attribute of this
    /// object.
    unsigned int& address2();

    /// Return a reference to the modifiable "Address3" attribute of this
    /// object.
    unsigned int& address3();

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

    /// Return the value of the "Address0" attribute of this object.
    unsigned int address0() const;

    /// Return the value of the "Address1" attribute of this object.
    unsigned int address1() const;

    /// Return the value of the "Address2" attribute of this object.
    unsigned int address2() const;

    /// Return the value of the "Address3" attribute of this object.
    unsigned int address3() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataAAAA& lhs,
                       const ResourceRecordDataAAAA& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataAAAA& lhs,
                       const ResourceRecordDataAAAA& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                 stream,
                                const ResourceRecordDataAAAA& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataAAAA'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       hashAlg,
                const ntcdns::ResourceRecordDataAAAA& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(ntcdns::ResourceRecordDataAAAA)

namespace ntcdns {

/// This class describes resource record data in the CNAME format.
class ResourceRecordDataCname
{
    // A  domain name which specifies the canonical or primary name for the
    // owner.  The owner name is an alias.
    bsl::string d_cname;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataCname' having the
    /// default value.  Use the optionally specified 'basicAllocator' to
    /// supply memory.  If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit ResourceRecordDataCname(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataCname' having the value
    /// of the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataCname(const ResourceRecordDataCname& original,
                            bslma::Allocator*              basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataCname' having the value
    /// of the specified 'original' object.  After performing this action,
    /// the 'original' object will be left in a valid, but unspecified
    /// state.
    ResourceRecordDataCname(ResourceRecordDataCname&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataCname' having the value
    /// of the specified 'original' object.  After performing this action,
    /// the 'original' object will be left in a valid, but unspecified
    /// state.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ResourceRecordDataCname(ResourceRecordDataCname&& original,
                            bslma::Allocator*         basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataCname();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataCname& operator=(const ResourceRecordDataCname& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataCname& operator=(ResourceRecordDataCname&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Cname" attribute of this
    /// object.
    bsl::string& cname();

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

    /// Return a reference offering non-modifiable access to the "Cname"
    /// attribute of this object.
    const bsl::string& cname() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataCname& lhs,
                       const ResourceRecordDataCname& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataCname& lhs,
                       const ResourceRecordDataCname& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                  stream,
                                const ResourceRecordDataCname& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataCname'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                        hashAlg,
                const ntcdns::ResourceRecordDataCname& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataCname)

namespace ntcdns {

/// This class describes resource record data in the HINFO format.
class ResourceRecordDataHinfo
{
    // The CPU architecture.
    bsl::string d_cpu;

    // The operating system.
    bsl::string d_os;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataHinfo' having the
    /// default value.  Use the optionally specified 'basicAllocator' to
    /// supply memory.  If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit ResourceRecordDataHinfo(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataHinfo' having the value
    /// of the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataHinfo(const ResourceRecordDataHinfo& original,
                            bslma::Allocator*              basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataHinfo' having the value
    /// of the specified 'original' object.  After performing this action,
    /// the 'original' object will be left in a valid, but unspecified
    /// state.
    ResourceRecordDataHinfo(ResourceRecordDataHinfo&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataHinfo' having the value
    /// of the specified 'original' object.  After performing this action,
    /// the 'original' object will be left in a valid, but unspecified
    /// state.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ResourceRecordDataHinfo(ResourceRecordDataHinfo&& original,
                            bslma::Allocator*         basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataHinfo();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataHinfo& operator=(const ResourceRecordDataHinfo& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataHinfo& operator=(ResourceRecordDataHinfo&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Cpu" attribute of this object.
    bsl::string& cpu();

    /// Return a reference to the modifiable "Os" attribute of this object.
    bsl::string& os();

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

    /// Return a reference offering non-modifiable access to the "Cpu"
    /// attribute of this object.
    const bsl::string& cpu() const;

    /// Return a reference offering non-modifiable access to the "Os"
    /// attribute of this object.
    const bsl::string& os() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataHinfo& lhs,
                       const ResourceRecordDataHinfo& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataHinfo& lhs,
                       const ResourceRecordDataHinfo& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                  stream,
                                const ResourceRecordDataHinfo& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataHinfo'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                        hashAlg,
                const ntcdns::ResourceRecordDataHinfo& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataHinfo)

namespace ntcdns {

/// This class describes resource record data in the MX format.
class ResourceRecordDataMx
{
    // A domain name which specifies a host willing to act as a mail
    // exchange for the owner name.
    bsl::string d_exchange;

    // A 16 bit integer which specifies the preference given to this RR
    // among others at the same owner.  Lower values are preferred.
    unsigned short d_preference;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataMx' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordDataMx(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataMx' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataMx(const ResourceRecordDataMx& original,
                         bslma::Allocator*           basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataMx' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordDataMx(ResourceRecordDataMx&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataMx' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordDataMx(ResourceRecordDataMx&& original,
                         bslma::Allocator*      basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataMx();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataMx& operator=(const ResourceRecordDataMx& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataMx& operator=(ResourceRecordDataMx&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Preference" attribute of this
    /// object.
    unsigned short& preference();

    /// Return a reference to the modifiable "Exchange" attribute of this
    /// object.
    bsl::string& exchange();

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

    /// Return the value of the "Preference" attribute of this object.
    unsigned short preference() const;

    /// Return a reference offering non-modifiable access to the "Exchange"
    /// attribute of this object.
    const bsl::string& exchange() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataMx& lhs,
                       const ResourceRecordDataMx& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataMx& lhs,
                       const ResourceRecordDataMx& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&               stream,
                                const ResourceRecordDataMx& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataMx'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     hashAlg,
                const ntcdns::ResourceRecordDataMx& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataMx)

namespace ntcdns {

/// This class describes resource record data in the NS format.
class ResourceRecordDataNs
{
    // A domain name which specifies a host which should be authoritative
    // for the specified class and domain.
    bsl::string d_nsdname;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataNs' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordDataNs(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataNs' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataNs(const ResourceRecordDataNs& original,
                         bslma::Allocator*           basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataNs' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordDataNs(ResourceRecordDataNs&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataNs' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordDataNs(ResourceRecordDataNs&& original,
                         bslma::Allocator*      basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataNs();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataNs& operator=(const ResourceRecordDataNs& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataNs& operator=(ResourceRecordDataNs&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Nsdname" attribute of this
    /// object.
    bsl::string& nsdname();

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

    /// Return a reference offering non-modifiable access to the "Nsdname"
    /// attribute of this object.
    const bsl::string& nsdname() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataNs& lhs,
                       const ResourceRecordDataNs& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataNs& lhs,
                       const ResourceRecordDataNs& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&               stream,
                                const ResourceRecordDataNs& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataNs'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     hashAlg,
                const ntcdns::ResourceRecordDataNs& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataNs)

namespace ntcdns {

/// This class describes resource record data in the PTR format.
class ResourceRecordDataPtr
{
    // A domain name which points to some location in the domain name
    // space.
    bsl::string d_ptrdname;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataPtr' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordDataPtr(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataPtr' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataPtr(const ResourceRecordDataPtr& original,
                          bslma::Allocator*            basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataPtr' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordDataPtr(ResourceRecordDataPtr&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataPtr' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordDataPtr(ResourceRecordDataPtr&& original,
                          bslma::Allocator*       basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataPtr();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataPtr& operator=(const ResourceRecordDataPtr& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataPtr& operator=(ResourceRecordDataPtr&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Ptrdname" attribute of this
    /// object.
    bsl::string& ptrdname();

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

    /// Return a reference offering non-modifiable access to the "Ptrdname"
    /// attribute of this object.
    const bsl::string& ptrdname() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataPtr& lhs,
                       const ResourceRecordDataPtr& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataPtr& lhs,
                       const ResourceRecordDataPtr& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                stream,
                                const ResourceRecordDataPtr& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataPtr'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataPtr& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataPtr)

namespace ntcdns {

/// This class describes resource record data in the RAW format.
class ResourceRecordDataRaw
{
    // The raw encoded data.
    bsl::vector<char> d_data;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataRaw' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordDataRaw(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataRaw' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataRaw(const ResourceRecordDataRaw& original,
                          bslma::Allocator*            basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataRaw' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordDataRaw(ResourceRecordDataRaw&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataRaw' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordDataRaw(ResourceRecordDataRaw&& original,
                          bslma::Allocator*       basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataRaw();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataRaw& operator=(const ResourceRecordDataRaw& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataRaw& operator=(ResourceRecordDataRaw&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Data" attribute of this
    /// object.
    bsl::vector<char>& data();

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

    /// Return a reference offering non-modifiable access to the "Data"
    /// attribute of this object.
    const bsl::vector<char>& data() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataRaw& lhs,
                       const ResourceRecordDataRaw& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataRaw& lhs,
                       const ResourceRecordDataRaw& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                stream,
                                const ResourceRecordDataRaw& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataRaw'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataRaw& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataRaw)

namespace ntcdns {

/// This class describes resource record data in the SOA format.
class ResourceRecordDataSoa
{
    // The domain name of the name server that was the original or primary
    // source of data for this zone.
    bsl::string d_mname;

    // A domain name which specifies the mailbox of the person responsible
    // for this zone.
    bsl::string d_rname;

    // The unsigned 32 bit version number of the original copy of the zone.
    //  Zone transfers preserve this value.  This value wraps and should be
    // compared using sequence space arithmetic.
    unsigned int d_serial;

    // A 32 bit time interval before the zone should be refreshed.
    unsigned int d_refresh;

    // A 32 bit time interval that should elapse before a failed refresh
    // should be retried.
    unsigned int d_retry;

    // A 32 bit time value that specifies the upper limit on the time
    // interval that can elapse before the zone is no longer authoritative.
    unsigned int d_expire;

    // The unsigned 32 bit minimum TTL field that should be exported with
    // any RR from this zone.
    unsigned int d_minimum;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataSoa' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordDataSoa(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataSoa' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataSoa(const ResourceRecordDataSoa& original,
                          bslma::Allocator*            basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataSoa' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordDataSoa(ResourceRecordDataSoa&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataSoa' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordDataSoa(ResourceRecordDataSoa&& original,
                          bslma::Allocator*       basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataSoa();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataSoa& operator=(const ResourceRecordDataSoa& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataSoa& operator=(ResourceRecordDataSoa&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Mname" attribute of this
    /// object.
    bsl::string& mname();

    /// Return a reference to the modifiable "Rname" attribute of this
    /// object.
    bsl::string& rname();

    /// Return a reference to the modifiable "Serial" attribute of this
    /// object.
    unsigned int& serial();

    /// Return a reference to the modifiable "Refresh" attribute of this
    /// object.
    unsigned int& refresh();

    /// Return a reference to the modifiable "Retry" attribute of this
    /// object.
    unsigned int& retry();

    /// Return a reference to the modifiable "Expire" attribute of this
    /// object.
    unsigned int& expire();

    /// Return a reference to the modifiable "Minimum" attribute of this
    /// object.
    unsigned int& minimum();

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

    /// Return a reference offering non-modifiable access to the "Mname"
    /// attribute of this object.
    const bsl::string& mname() const;

    /// Return a reference offering non-modifiable access to the "Rname"
    /// attribute of this object.
    const bsl::string& rname() const;

    /// Return the value of the "Serial" attribute of this object.
    unsigned int serial() const;

    /// Return the value of the "Refresh" attribute of this object.
    unsigned int refresh() const;

    /// Return the value of the "Retry" attribute of this object.
    unsigned int retry() const;

    /// Return the value of the "Expire" attribute of this object.
    unsigned int expire() const;

    /// Return the value of the "Minimum" attribute of this object.
    unsigned int minimum() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataSoa& lhs,
                       const ResourceRecordDataSoa& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataSoa& lhs,
                       const ResourceRecordDataSoa& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                stream,
                                const ResourceRecordDataSoa& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataSoa'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataSoa& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataSoa)

namespace ntcdns {

/// This class describes resource record data in the SVR format.
class ResourceRecordDataSvr
{
    // The symbolic name of the desired service, in the form
    // "_Service._Proto.Name".  An underscore (_) is prepended to the
    // service identifier to avoid collisions with DNS labels that occur in
    // nature.
    bsl::string d_name;

    // The domain name of the target host.
    bsl::string d_target;

    // The time-to-live in seconds of this record.
    unsigned int d_ttl;

    // The classification.
    unsigned short d_classification;

    // The priority of this target host.  A client MUST attempt to contact
    // the target host with the lowest-numbered priority it can reach;
    // target hosts with the same priority SHOULD be tried in an order
    // defined by the weight field.
    unsigned short d_priority;

    // A server selection mechanism.  The weight field specifies a relative
    // weight for entries with the same priority.  Larger weights SHOULD be
    // given a proportionately higher probability of being selected.
    unsigned short d_weight;

    // The port on the target host of the service.
    unsigned short d_port;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataSvr' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordDataSvr(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataSvr' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataSvr(const ResourceRecordDataSvr& original,
                          bslma::Allocator*            basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataSvr' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordDataSvr(ResourceRecordDataSvr&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataSvr' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordDataSvr(ResourceRecordDataSvr&& original,
                          bslma::Allocator*       basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataSvr();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataSvr& operator=(const ResourceRecordDataSvr& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataSvr& operator=(ResourceRecordDataSvr&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Name" attribute of this
    /// object.
    bsl::string& name();

    /// Return a reference to the modifiable "Ttl" attribute of this object.
    unsigned int& ttl();

    /// Return a reference to the modifiable "Classification" attribute of
    /// this object.
    unsigned short& classification();

    /// Return a reference to the modifiable "Priority" attribute of this
    /// object.
    unsigned short& priority();

    /// Return a reference to the modifiable "Weight" attribute of this
    /// object.
    unsigned short& weight();

    /// Return a reference to the modifiable "Port" attribute of this
    /// object.
    unsigned short& port();

    /// Return a reference to the modifiable "Target" attribute of this
    /// object.
    bsl::string& target();

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

    /// Return a reference offering non-modifiable access to the "Name"
    /// attribute of this object.
    const bsl::string& name() const;

    /// Return the value of the "Ttl" attribute of this object.
    unsigned int ttl() const;

    /// Return the value of the "Classification" attribute of this object.
    unsigned short classification() const;

    /// Return the value of the "Priority" attribute of this object.
    unsigned short priority() const;

    /// Return the value of the "Weight" attribute of this object.
    unsigned short weight() const;

    /// Return the value of the "Port" attribute of this object.
    unsigned short port() const;

    /// Return a reference offering non-modifiable access to the "Target"
    /// attribute of this object.
    const bsl::string& target() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataSvr& lhs,
                       const ResourceRecordDataSvr& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataSvr& lhs,
                       const ResourceRecordDataSvr& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                stream,
                                const ResourceRecordDataSvr& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataSvr'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataSvr& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataSvr)

namespace ntcdns {

/// This class describes resource record data in the TXT format.
class ResourceRecordDataTxt
{
    // One or more character strings.
    bsl::vector<bsl::string> d_text;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataTxt' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordDataTxt(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataTxt' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataTxt(const ResourceRecordDataTxt& original,
                          bslma::Allocator*            basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataTxt' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordDataTxt(ResourceRecordDataTxt&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataTxt' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordDataTxt(ResourceRecordDataTxt&& original,
                          bslma::Allocator*       basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataTxt();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataTxt& operator=(const ResourceRecordDataTxt& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataTxt& operator=(ResourceRecordDataTxt&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Text" attribute of this
    /// object.
    bsl::vector<bsl::string>& text();

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

    /// Return a reference offering non-modifiable access to the "Text"
    /// attribute of this object.
    const bsl::vector<bsl::string>& text() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataTxt& lhs,
                       const ResourceRecordDataTxt& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataTxt& lhs,
                       const ResourceRecordDataTxt& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                stream,
                                const ResourceRecordDataTxt& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataTxt'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataTxt& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataTxt)

namespace ntcdns {

/// This class describes resource record data in the WKS format.
class ResourceRecordDataWks
{
    // The bit map that indicates whether service is available on a
    // particular well known port.  For example, SSH is assigned the
    // well-known port 22, so if index 22 is set, then the SSH service is
    // available at the address.
    bsl::vector<unsigned short> d_port;

    // The address on which the well-known services are supported.
    unsigned int d_address;

    // The protocol number for the higher-level protocol using the Internet
    // Protocol.  For example, 16 is TCP.
    unsigned char d_protocol;

  public:
  public:
    /// Create an object of type 'ResourceRecordDataWks' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordDataWks(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordDataWks' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordDataWks(const ResourceRecordDataWks& original,
                          bslma::Allocator*            basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordDataWks' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordDataWks(ResourceRecordDataWks&& original) noexcept;

    /// Create an object of type 'ResourceRecordDataWks' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordDataWks(ResourceRecordDataWks&& original,
                          bslma::Allocator*       basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordDataWks();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordDataWks& operator=(const ResourceRecordDataWks& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordDataWks& operator=(ResourceRecordDataWks&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Address" attribute of this
    /// object.
    unsigned int& address();

    /// Return a reference to the modifiable "Protocol" attribute of this
    /// object.
    unsigned char& protocol();

    /// Return a reference to the modifiable "Port" attribute of this
    /// object.
    bsl::vector<unsigned short>& port();

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

    /// Return the value of the "Address" attribute of this object.
    unsigned int address() const;

    /// Return the value of the "Protocol" attribute of this object.
    unsigned char protocol() const;

    /// Return a reference offering non-modifiable access to the "Port"
    /// attribute of this object.
    const bsl::vector<unsigned short>& port() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResourceRecordDataWks& lhs,
                       const ResourceRecordDataWks& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResourceRecordDataWks& lhs,
                       const ResourceRecordDataWks& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                stream,
                                const ResourceRecordDataWks& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordDataWks'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataWks& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordDataWks)

namespace ntcdns {

/// This class describes a DNS sort list item.
class SortListItem
{
    // The IP address.
    bsl::string d_address;

    // The netmask.
    bsl::string d_netmask;

  public:
  public:
    /// Create an object of type 'SortListItem' having the default value.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit SortListItem(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'SortListItem' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    SortListItem(const SortListItem& original,
                 bslma::Allocator*   basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'SortListItem' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    SortListItem(SortListItem&& original) noexcept;

    /// Create an object of type 'SortListItem' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    SortListItem(SortListItem&& original, bslma::Allocator* basicAllocator);
#endif

    /// Destroy this object.
    ~SortListItem();

    /// Assign to this object the value of the specified 'rhs' object.
    SortListItem& operator=(const SortListItem& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    SortListItem& operator=(SortListItem&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Address" attribute of this
    /// object.
    bsl::string& address();

    /// Return a reference to the modifiable "Netmask" attribute of this
    /// object.
    bsl::string& netmask();

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

    /// Return a reference offering non-modifiable access to the "Address"
    /// attribute of this object.
    const bsl::string& address() const;

    /// Return a reference offering non-modifiable access to the "Netmask"
    /// attribute of this object.
    const bsl::string& netmask() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const SortListItem& lhs, const SortListItem& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const SortListItem& lhs, const SortListItem& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, const SortListItem& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'SortListItem'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::SortListItem& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ntcdns::SortListItem)

namespace ntcdns {

/// This structure enumerates the TYPE fields used in resource records.
/// TYPE values are a subset of QTYPE values.
struct Type {
  public:
    enum Value {
        e_A = 1
        // IPv4 host address.
        ,
        e_NS = 2
        // Authoritative name server.
        ,
        e_MD = 3
        // Mail destination (obsolete, use MX).
        ,
        e_MF = 4
        // Mail forwarder (obseolete, use MX).
        ,
        e_CNAME = 5
        // Canonical name of an alias.
        ,
        e_SOA = 6
        // Start of a zone of authority.
        ,
        e_MB = 7
        // Mailbox domain name (experimental).
        ,
        e_MG = 8
        // Mail group member (experimental).
        ,
        e_MR = 9
        // Mail rename domain name (experimental).
        ,
        e_UNDEFINED = 10
        // Null resource record (experimental).
        ,
        e_WKS = 11
        // Well-known service description.
        ,
        e_PTR = 12
        // Domain name pointer.
        ,
        e_HINFO = 13
        // Host information.
        ,
        e_MINFO = 14
        // Mailbox or mail list information.
        ,
        e_MX = 15
        // Mail exchange.
        ,
        e_TXT = 16
        // Text strings.
        ,
        e_RP = 17
        // Responsible person.
        ,
        e_AFSDB = 18
        // AFS database for AFS clients.
        ,
        e_X25 = 19
        // X.25 PSDN Address provides details on encapsulation via X.25
        // (obsolete).
        ,
        e_ISDN = 20
        // Assigns the DNS name an ISDN number (obsolete).
        ,
        e_RT = 21
        // Provides route-through binding without a WAN address (obsolete).
        ,
        e_NSAP = 22
        // Enables assignment of domain names to Network Service Access
        // Points (obsolete).
        ,
        e_NSAPPTR = 23
        // Replaced by PTR (obsolete).
        ,
        e_SIG = 24
        // Signature was replaced by RRSIG (obsolete).
        ,
        e_KEY = 25
        // Key was replaced by IPSECKEY (obsolete).
        ,
        e_PX = 26
        // Pointer to X.400 specifies MIXER mapping regulations (obsolete).
        ,
        e_GPOS = 27
        // Geographical Position was replaced by LOC (obsolete).
        ,
        e_AAAA = 28
        // IPv6 host address.
        ,
        e_LOC = 29
        // Location information.
        ,
        e_NXT = 30
        // Next was replaced by NSEC (obsolete).
        ,
        e_EID = 31
        // Endpoint Identifier is intended for Nimrod Routing Architecture
        // (obsolete).
        ,
        e_NIMLOC = 32
        // Nimrod Locator is intended for Nimrod Routing Architecture
        // (obsolete).
        ,
        e_SVR = 33
        // Service locations.
        ,
        e_ATMA = 34
        // ATM Address provides information when there are asynchronous
        // transfer modes (obsolete).
        ,
        e_NAPTR = 35
        // Naming Authority Pointer is an A record extension that permits
        // the search pattern (regular expressions).
        ,
        e_KX = 36
        // Key management for cryptography.
        ,
        e_CERT = 37
        // Saves certificates.
        ,
        e_A6 = 38
        // A6 was replaced by AAAA.
        ,
        e_DNAME = 39
        // Aliases for complete domains.
        ,
        e_SINK = 40
        // Kitchen Sink enables the storage of various data (obsolete).
        ,
        e_OPT = 41
        // Pseudo-record type to supprt EDNS.
        ,
        e_APL = 42
        // Address Prefix List lists address areas in CIDR format.
        ,
        e_DS = 43
        // Delegation Signer identifies DNSSEC-signed zones.
        ,
        e_SSHFP = 44
        // SSH Public Key Fingerprint discloses the fingerprint for SSH
        // keys.
        ,
        e_IPSECKEY = 45
        // IPsec Key contains an IPsec key.
        ,
        e_RRSIG = 46
        // RR Signature contains a digital signature for DNSSEC.
        ,
        e_NSEC = 47
        // Next Secure threads signed zones in DNSSEC.
        ,
        e_DNSKEY = 48
        // DNS Key contains a public key for DNSSEC.
        ,
        e_DHCID = 49
        // DHCP Identifier links domain names with DHCP clients.
        ,
        e_NSEC3 = 50
        // Next Secure 3 is an alternative to NSEC.
        ,
        e_NSEC3PARAM = 51
        // This record contains Parameter for NSEC3.
        ,
        e_TLSA = 52
        // This record issues an TLSA Certificate Association with a domain
        // name pertaining to DANE.
        ,
        e_SMIMEA = 53
        // This record issues a S/MIME Certificate Association with a
        // domain name.
        ,
        e_HIP = 55
        // Host Identity Protocol separates endpoint markers and
        // positioning functions from IP addresses.
        ,
        e_NINFO = 56
        // NINFO provides information on the zone status (obsolete).
        ,
        e_RKEY = 57
        // RKEY saves keys (obsolete).
        ,
        e_TALINK = 58
        // Trust Anchor Link connects two domain names (obsolete).
        ,
        e_CDS = 59
        // Child DS is a child copy of a DS record.
        ,
        e_CDNSKEY = 60
        // Child DNSKEY is a child copy of a DNSKEY record.
        ,
        e_OPENPGPKEY = 61
        // OpenPGP Key discloses public keys.
        ,
        e_CSYNC = 62
        // Child-to-Parent Synchronization enables the reconciliation of
        // parent and child zones (obsolete).
        ,
        e_ZONEMD = 63
        // Message Digest for DNS Zone is experimental (obsolete).
        ,
        e_SPF = 99
        // Sender Policy Framework was replaced by the TXT record
        // (obsolete).
        ,
        e_UINFO = 100
        // Reserved.
        ,
        e_UID = 101
        // Reserved.
        ,
        e_GID = 102
        // Reserved.
        ,
        e_UNSPEC = 103
        // Reserved
        ,
        e_NID = 104
        // Node identifier (experimental).
        ,
        e_L32 = 105
        // 32-bit locator (experimental).
        ,
        e_L64 = 106
        // 64-bit locator (experimental).
        ,
        e_LP = 107
        // Locator pointer.
        ,
        e_EUI48 = 108
        // 48-bit Extended Unique Identifier encrypts addresses.
        ,
        e_EUI64 = 109
        // 64-bit Extended Unique Identifier encrypts addresses.
        ,
        e_TKEY = 249
        // Transaction Key enables the exchange of secret keys.
        ,
        e_TSIG = 250
        // Transaction Signature is used for authentication.
        ,
        e_IXFR = 251
        // Incremental Zone Transfer enables zone file components to be
        // updated on a second server (obsolete).
        ,
        e_AXFR = 252
        // Request for transfer of an entire zone.
        ,
        e_MAILB = 253
        // Request for mailbox-related records (MB, MG, or MR).
        ,
        e_MAILA = 254
        // Request for mail agent resource records.  (Obsolete, see MX).
        ,
        e_ALL = 255
        // Request for all records.
        ,
        e_URI = 256
        // Uniform Resource Identifier discloses the mapping of host names
        // to URIs.
        ,
        e_CAA = 257
        // Certificate Authority Authorization specifies a domain's
        // possible CAs.
        ,
        e_AVC = 258
        // Application Visibility and Control contains application metadata
        // for DNS-AS (obsolete).
        ,
        e_DOA = 259
        // DOA is no longer active (obsolete).
        ,
        e_AMTRELAY = 260
        // Automatic Multicast Tunneling Relay enables the finding of AMT
        // relays (obsolete).
        ,
        e_TA = 32768
        // DNSSEC Trust Authorities enables DNSSEC without signed root.
        ,
        e_DLV = 32769
        // DNSSEC Lookaside Validation discloses trust anchors beyond the
        // standard DNS chain.
    };

    enum { NUM_ENUMERATORS = 87 };

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string' of the specified 'stringLength'.  Return 0 on
    /// success, and a non-zero value with no effect on 'result' otherwise
    /// (i.e., 'string' does not match any enumerator).
    static int fromString(Value* result, const char* string, int stringLength);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bsl::string& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of
    /// the specified enumeration 'value'.  Return a reference to
    /// the modifiable 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, Type::Value rhs);

}  // close package namespace

BDLAT_DECL_ENUMERATION_TRAITS(ntcdns::Type)

namespace ntcdns {

/// This class describes an association of addresses, canonical hostnames,
/// and aliases.
class HostDatabaseConfig
{
    // The host/address pairs.
    bsl::vector<HostEntry> d_entry;

  public:
  public:
    /// Create an object of type 'HostDatabaseConfig' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit HostDatabaseConfig(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'HostDatabaseConfig' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    HostDatabaseConfig(const HostDatabaseConfig& original,
                       bslma::Allocator*         basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'HostDatabaseConfig' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    HostDatabaseConfig(HostDatabaseConfig&& original) noexcept;

    /// Create an object of type 'HostDatabaseConfig' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    HostDatabaseConfig(HostDatabaseConfig&& original,
                       bslma::Allocator*    basicAllocator);
#endif

    /// Destroy this object.
    ~HostDatabaseConfig();

    /// Assign to this object the value of the specified 'rhs' object.
    HostDatabaseConfig& operator=(const HostDatabaseConfig& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    HostDatabaseConfig& operator=(HostDatabaseConfig&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Entry" attribute of this
    /// object.
    bsl::vector<HostEntry>& entry();

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

    /// Return a reference offering non-modifiable access to the "Entry"
    /// attribute of this object.
    const bsl::vector<HostEntry>& entry() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const HostDatabaseConfig& lhs,
                       const HostDatabaseConfig& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const HostDatabaseConfig& lhs,
                       const HostDatabaseConfig& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&             stream,
                                const HostDatabaseConfig& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'HostDatabaseConfig'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   hashAlg,
                const ntcdns::HostDatabaseConfig& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::HostDatabaseConfig)

namespace ntcdns {

/// This struct describes a name server.
class NameServerConfig
{
    // The address of the name server.
    NameServerAddress d_address;

  public:
  public:
    /// Create an object of type 'NameServerConfig' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit NameServerConfig(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'NameServerConfig' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    NameServerConfig(const NameServerConfig& original,
                     bslma::Allocator*       basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'NameServerConfig' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    NameServerConfig(NameServerConfig&& original) noexcept;

    /// Create an object of type 'NameServerConfig' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    NameServerConfig(NameServerConfig&& original,
                     bslma::Allocator*  basicAllocator);
#endif

    /// Destroy this object.
    ~NameServerConfig();

    /// Assign to this object the value of the specified 'rhs' object.
    NameServerConfig& operator=(const NameServerConfig& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    NameServerConfig& operator=(NameServerConfig&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Address" attribute of this
    /// object.
    NameServerAddress& address();

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

    /// Return a reference offering non-modifiable access to the "Address"
    /// attribute of this object.
    const NameServerAddress& address() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const NameServerConfig& lhs,
                       const NameServerConfig& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const NameServerConfig& lhs,
                       const NameServerConfig& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&           stream,
                                const NameServerConfig& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'NameServerConfig'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 hashAlg,
                const ntcdns::NameServerConfig& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::NameServerConfig)

namespace ntcdns {

/// This class describes an association of services, protocols, and ports.
class PortDatabaseConfig
{
    // The service/port pairs.
    bsl::vector<PortEntry> d_entry;

  public:
  public:
    /// Create an object of type 'PortDatabaseConfig' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit PortDatabaseConfig(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'PortDatabaseConfig' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    PortDatabaseConfig(const PortDatabaseConfig& original,
                       bslma::Allocator*         basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'PortDatabaseConfig' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    PortDatabaseConfig(PortDatabaseConfig&& original) noexcept;

    /// Create an object of type 'PortDatabaseConfig' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    PortDatabaseConfig(PortDatabaseConfig&& original,
                       bslma::Allocator*    basicAllocator);
#endif

    /// Destroy this object.
    ~PortDatabaseConfig();

    /// Assign to this object the value of the specified 'rhs' object.
    PortDatabaseConfig& operator=(const PortDatabaseConfig& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    PortDatabaseConfig& operator=(PortDatabaseConfig&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Entry" attribute of this
    /// object.
    bsl::vector<PortEntry>& entry();

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

    /// Return a reference offering non-modifiable access to the "Entry"
    /// attribute of this object.
    const bsl::vector<PortEntry>& entry() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const PortDatabaseConfig& lhs,
                       const PortDatabaseConfig& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const PortDatabaseConfig& lhs,
                       const PortDatabaseConfig& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&             stream,
                                const PortDatabaseConfig& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'PortDatabaseConfig'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   hashAlg,
                const ntcdns::PortDatabaseConfig& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::PortDatabaseConfig)

namespace ntcdns {

/// This class describes a union of resource record type data.
class ResourceRecordData
{
    union {
        /// The resource record data in the CNAME format.
        bsls::ObjectBuffer<ResourceRecordDataCname> d_canonicalName;

        /// The resource record data in the HINFO format.
        bsls::ObjectBuffer<ResourceRecordDataHinfo> d_hostInfo;

        /// The resource record data in the MX format.
        bsls::ObjectBuffer<ResourceRecordDataMx> d_mailExchange;

        /// The resource record data in the NS format.
        bsls::ObjectBuffer<ResourceRecordDataNs> d_nameServer;

        /// The resource record data in the PTR format.
        bsls::ObjectBuffer<ResourceRecordDataPtr> d_pointer;

        /// The resource record data in the SOA format.
        bsls::ObjectBuffer<ResourceRecordDataSoa> d_zoneAuthority;

        /// The resource record data in the WKS format.
        bsls::ObjectBuffer<ResourceRecordDataWks> d_wellKnownService;

        /// The resource record data in the TXT format.
        bsls::ObjectBuffer<ResourceRecordDataTxt> d_text;

        /// The resource record data in the A format.
        bsls::ObjectBuffer<ResourceRecordDataA> d_ipv4;

        /// The resource record data in the AAAA format.
        bsls::ObjectBuffer<ResourceRecordDataAAAA> d_ipv6;

        /// The resource record data in the SVR format.
        bsls::ObjectBuffer<ResourceRecordDataSvr> d_server;

        /// The resource record data in a raw format.
        bsls::ObjectBuffer<ResourceRecordDataRaw> d_raw;
    };

    int               d_selectionId;
    bslma::Allocator* d_allocator_p;

  public:
    enum {
        SELECTION_ID_UNDEFINED          = -1,
        SELECTION_ID_CANONICAL_NAME     = 0,
        SELECTION_ID_HOST_INFO          = 1,
        SELECTION_ID_MAIL_EXCHANGE      = 2,
        SELECTION_ID_NAME_SERVER        = 3,
        SELECTION_ID_POINTER            = 4,
        SELECTION_ID_ZONE_AUTHORITY     = 5,
        SELECTION_ID_WELL_KNOWN_SERVICE = 6,
        SELECTION_ID_TEXT               = 7,
        SELECTION_ID_IPV4               = 8,
        SELECTION_ID_IPV6               = 9,
        SELECTION_ID_SERVER             = 10,
        SELECTION_ID_RAW                = 11
    };

    /// Create an object of type 'ResourceRecordData' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ResourceRecordData(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResourceRecordData' having the value of
    /// the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResourceRecordData(const ResourceRecordData& original,
                       bslma::Allocator*         basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResourceRecordData' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResourceRecordData(ResourceRecordData&& original) noexcept;

    /// Create an object of type 'ResourceRecordData' having the value of
    /// the specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResourceRecordData(ResourceRecordData&& original,
                       bslma::Allocator*    basicAllocator);
#endif

    /// Destroy this object.
    ~ResourceRecordData();

    /// Assign to this object the value of the specified 'rhs' object.
    ResourceRecordData& operator=(const ResourceRecordData& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResourceRecordData& operator=(ResourceRecordData&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    ResourceRecordDataCname& makeCanonicalName();
    ResourceRecordDataCname& makeCanonicalName(
        const ResourceRecordDataCname& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataCname& makeCanonicalName(
        ResourceRecordDataCname&& value);
#endif
    // Set the value of this object to be a "CanonicalName" value.
    // Optionally specify the 'value' of the "CanonicalName".  If 'value'
    // is not specified, the default "CanonicalName" value is used.

    ResourceRecordDataHinfo& makeHostInfo();
    ResourceRecordDataHinfo& makeHostInfo(
        const ResourceRecordDataHinfo& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataHinfo& makeHostInfo(ResourceRecordDataHinfo&& value);
#endif
    // Set the value of this object to be a "HostInfo" value.  Optionally
    // specify the 'value' of the "HostInfo".  If 'value' is not specified,
    // the default "HostInfo" value is used.

    ResourceRecordDataMx& makeMailExchange();
    ResourceRecordDataMx& makeMailExchange(const ResourceRecordDataMx& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataMx& makeMailExchange(ResourceRecordDataMx&& value);
#endif
    // Set the value of this object to be a "MailExchange" value.
    // Optionally specify the 'value' of the "MailExchange".  If 'value' is
    // not specified, the default "MailExchange" value is used.

    ResourceRecordDataNs& makeNameServer();
    ResourceRecordDataNs& makeNameServer(const ResourceRecordDataNs& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataNs& makeNameServer(ResourceRecordDataNs&& value);
#endif
    // Set the value of this object to be a "NameServer" value.  Optionally
    // specify the 'value' of the "NameServer".  If 'value' is not
    // specified, the default "NameServer" value is used.

    ResourceRecordDataPtr& makePointer();
    ResourceRecordDataPtr& makePointer(const ResourceRecordDataPtr& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataPtr& makePointer(ResourceRecordDataPtr&& value);
#endif
    // Set the value of this object to be a "Pointer" value.  Optionally
    // specify the 'value' of the "Pointer".  If 'value' is not specified,
    // the default "Pointer" value is used.

    ResourceRecordDataSoa& makeZoneAuthority();
    ResourceRecordDataSoa& makeZoneAuthority(
        const ResourceRecordDataSoa& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataSoa& makeZoneAuthority(ResourceRecordDataSoa&& value);
#endif
    // Set the value of this object to be a "ZoneAuthority" value.
    // Optionally specify the 'value' of the "ZoneAuthority".  If 'value'
    // is not specified, the default "ZoneAuthority" value is used.

    ResourceRecordDataWks& makeWellKnownService();
    ResourceRecordDataWks& makeWellKnownService(
        const ResourceRecordDataWks& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataWks& makeWellKnownService(ResourceRecordDataWks&& value);
#endif
    // Set the value of this object to be a "WellKnownService" value.
    // Optionally specify the 'value' of the "WellKnownService".  If
    // 'value' is not specified, the default "WellKnownService" value is
    // used.

    ResourceRecordDataTxt& makeText();
    ResourceRecordDataTxt& makeText(const ResourceRecordDataTxt& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataTxt& makeText(ResourceRecordDataTxt&& value);
#endif
    // Set the value of this object to be a "Text" value.  Optionally
    // specify the 'value' of the "Text".  If 'value' is not specified, the
    // default "Text" value is used.

    ResourceRecordDataA& makeIpv4();
    ResourceRecordDataA& makeIpv4(const ResourceRecordDataA& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataA& makeIpv4(ResourceRecordDataA&& value);
#endif
    // Set the value of this object to be a "Ipv4" value.  Optionally
    // specify the 'value' of the "Ipv4".  If 'value' is not specified, the
    // default "Ipv4" value is used.

    ResourceRecordDataAAAA& makeIpv6();
    ResourceRecordDataAAAA& makeIpv6(const ResourceRecordDataAAAA& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataAAAA& makeIpv6(ResourceRecordDataAAAA&& value);
#endif
    // Set the value of this object to be a "Ipv6" value.  Optionally
    // specify the 'value' of the "Ipv6".  If 'value' is not specified, the
    // default "Ipv6" value is used.

    ResourceRecordDataSvr& makeServer();
    ResourceRecordDataSvr& makeServer(const ResourceRecordDataSvr& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataSvr& makeServer(ResourceRecordDataSvr&& value);
#endif
    // Set the value of this object to be a "Server" value.  Optionally
    // specify the 'value' of the "Server".  If 'value' is not specified,
    // the default "Server" value is used.

    ResourceRecordDataRaw& makeRaw();
    ResourceRecordDataRaw& makeRaw(const ResourceRecordDataRaw& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ResourceRecordDataRaw& makeRaw(ResourceRecordDataRaw&& value);
#endif
    // Set the value of this object to be a "Raw" value.  Optionally
    // specify the 'value' of the "Raw".  If 'value' is not specified, the
    // default "Raw" value is used.

    /// Return a reference to the modifiable "CanonicalName" selection of
    /// this object if "CanonicalName" is the current selection.  The
    /// behavior is undefined unless "CanonicalName" is the selection of
    /// this object.
    ResourceRecordDataCname& canonicalName();

    /// Return a reference to the modifiable "HostInfo" selection of this
    /// object if "HostInfo" is the current selection.  The behavior is
    /// undefined unless "HostInfo" is the selection of this object.
    ResourceRecordDataHinfo& hostInfo();

    /// Return a reference to the modifiable "MailExchange" selection of
    /// this object if "MailExchange" is the current selection.  The
    /// behavior is undefined unless "MailExchange" is the selection of this
    /// object.
    ResourceRecordDataMx& mailExchange();

    /// Return a reference to the modifiable "NameServer" selection of this
    /// object if "NameServer" is the current selection.  The behavior is
    /// undefined unless "NameServer" is the selection of this object.
    ResourceRecordDataNs& nameServer();

    /// Return a reference to the modifiable "Pointer" selection of this
    /// object if "Pointer" is the current selection.  The behavior is
    /// undefined unless "Pointer" is the selection of this object.
    ResourceRecordDataPtr& pointer();

    /// Return a reference to the modifiable "ZoneAuthority" selection of
    /// this object if "ZoneAuthority" is the current selection.  The
    /// behavior is undefined unless "ZoneAuthority" is the selection of
    /// this object.
    ResourceRecordDataSoa& zoneAuthority();

    /// Return a reference to the modifiable "WellKnownService" selection of
    /// this object if "WellKnownService" is the current selection.  The
    /// behavior is undefined unless "WellKnownService" is the selection of
    /// this object.
    ResourceRecordDataWks& wellKnownService();

    /// Return a reference to the modifiable "Text" selection of this object
    /// if "Text" is the current selection.  The behavior is undefined
    /// unless "Text" is the selection of this object.
    ResourceRecordDataTxt& text();

    /// Return a reference to the modifiable "Ipv4" selection of this object
    /// if "Ipv4" is the current selection.  The behavior is undefined
    /// unless "Ipv4" is the selection of this object.
    ResourceRecordDataA& ipv4();

    /// Return a reference to the modifiable "Ipv6" selection of this object
    /// if "Ipv6" is the current selection.  The behavior is undefined
    /// unless "Ipv6" is the selection of this object.
    ResourceRecordDataAAAA& ipv6();

    /// Return a reference to the modifiable "Server" selection of this
    /// object if "Server" is the current selection.  The behavior is
    /// undefined unless "Server" is the selection of this object.
    ResourceRecordDataSvr& server();

    /// Return a reference to the modifiable "Raw" selection of this object
    /// if "Raw" is the current selection.  The behavior is undefined unless
    /// "Raw" is the selection of this object.
    ResourceRecordDataRaw& raw();

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

    /// Return the id of the current selection if the selection is defined,
    /// and -1 otherwise.
    int selectionId() const;

    /// Return a reference to the non-modifiable "CanonicalName" selection
    /// of this object if "CanonicalName" is the current selection.  The
    /// behavior is undefined unless "CanonicalName" is the selection of
    /// this object.
    const ResourceRecordDataCname& canonicalName() const;

    /// Return a reference to the non-modifiable "HostInfo" selection of
    /// this object if "HostInfo" is the current selection.  The behavior is
    /// undefined unless "HostInfo" is the selection of this object.
    const ResourceRecordDataHinfo& hostInfo() const;

    /// Return a reference to the non-modifiable "MailExchange" selection of
    /// this object if "MailExchange" is the current selection.  The
    /// behavior is undefined unless "MailExchange" is the selection of this
    /// object.
    const ResourceRecordDataMx& mailExchange() const;

    /// Return a reference to the non-modifiable "NameServer" selection of
    /// this object if "NameServer" is the current selection.  The behavior
    /// is undefined unless "NameServer" is the selection of this object.
    const ResourceRecordDataNs& nameServer() const;

    /// Return a reference to the non-modifiable "Pointer" selection of this
    /// object if "Pointer" is the current selection.  The behavior is
    /// undefined unless "Pointer" is the selection of this object.
    const ResourceRecordDataPtr& pointer() const;

    /// Return a reference to the non-modifiable "ZoneAuthority" selection
    /// of this object if "ZoneAuthority" is the current selection.  The
    /// behavior is undefined unless "ZoneAuthority" is the selection of
    /// this object.
    const ResourceRecordDataSoa& zoneAuthority() const;

    /// Return a reference to the non-modifiable "WellKnownService"
    /// selection of this object if "WellKnownService" is the current
    /// selection.  The behavior is undefined unless "WellKnownService" is
    /// the selection of this object.
    const ResourceRecordDataWks& wellKnownService() const;

    /// Return a reference to the non-modifiable "Text" selection of this
    /// object if "Text" is the current selection.  The behavior is
    /// undefined unless "Text" is the selection of this object.
    const ResourceRecordDataTxt& text() const;

    /// Return a reference to the non-modifiable "Ipv4" selection of this
    /// object if "Ipv4" is the current selection.  The behavior is
    /// undefined unless "Ipv4" is the selection of this object.
    const ResourceRecordDataA& ipv4() const;

    /// Return a reference to the non-modifiable "Ipv6" selection of this
    /// object if "Ipv6" is the current selection.  The behavior is
    /// undefined unless "Ipv6" is the selection of this object.
    const ResourceRecordDataAAAA& ipv6() const;

    /// Return a reference to the non-modifiable "Server" selection of this
    /// object if "Server" is the current selection.  The behavior is
    /// undefined unless "Server" is the selection of this object.
    const ResourceRecordDataSvr& server() const;

    /// Return a reference to the non-modifiable "Raw" selection of this
    /// object if "Raw" is the current selection.  The behavior is undefined
    /// unless "Raw" is the selection of this object.
    const ResourceRecordDataRaw& raw() const;

    /// Return 'true' if the value of this object is a "CanonicalName"
    /// value, and return 'false' otherwise.
    bool isCanonicalNameValue() const;

    /// Return 'true' if the value of this object is a "HostInfo" value, and
    /// return 'false' otherwise.
    bool isHostInfoValue() const;

    /// Return 'true' if the value of this object is a "MailExchange" value,
    /// and return 'false' otherwise.
    bool isMailExchangeValue() const;

    /// Return 'true' if the value of this object is a "NameServer" value,
    /// and return 'false' otherwise.
    bool isNameServerValue() const;

    /// Return 'true' if the value of this object is a "Pointer" value, and
    /// return 'false' otherwise.
    bool isPointerValue() const;

    /// Return 'true' if the value of this object is a "ZoneAuthority"
    /// value, and return 'false' otherwise.
    bool isZoneAuthorityValue() const;

    /// Return 'true' if the value of this object is a "WellKnownService"
    /// value, and return 'false' otherwise.
    bool isWellKnownServiceValue() const;

    /// Return 'true' if the value of this object is a "Text" value, and
    /// return 'false' otherwise.
    bool isTextValue() const;

    /// Return 'true' if the value of this object is a "Ipv4" value, and
    /// return 'false' otherwise.
    bool isIpv4Value() const;

    /// Return 'true' if the value of this object is a "Ipv6" value, and
    /// return 'false' otherwise.
    bool isIpv6Value() const;

    /// Return 'true' if the value of this object is a "Server" value, and
    /// return 'false' otherwise.
    bool isServerValue() const;

    /// Return 'true' if the value of this object is a "Raw" value, and
    /// return 'false' otherwise.
    bool isRawValue() const;

    /// Return 'true' if the value of this object is undefined, and 'false'
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char* selectionName() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' objects have the same
/// value, and 'false' otherwise.  Two 'ResourceRecordData' objects have the same
/// value if either the selections in both objects have the same ids and
/// the same values, or both selections are undefined.
inline bool operator==(const ResourceRecordData& lhs,
                       const ResourceRecordData& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
/// same values, as determined by 'operator==', and 'false' otherwise.
inline bool operator!=(const ResourceRecordData& lhs,
                       const ResourceRecordData& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&             stream,
                                const ResourceRecordData& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResourceRecordData'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   hashAlg,
                const ntcdns::ResourceRecordData& object);

}  // close package namespace

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResourceRecordData)

namespace ntcdns {

/// This class describes a DNS sort list.
class SortList
{
    // The IP address/netmask pairs.
    bsl::vector<SortListItem> d_item;

  public:
  public:
    /// Create an object of type 'SortList' having the default value.  Use
    /// the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit SortList(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'SortList' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    SortList(const SortList& original, bslma::Allocator* basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'SortList' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    SortList(SortList&& original) noexcept;

    /// Create an object of type 'SortList' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    SortList(SortList&& original, bslma::Allocator* basicAllocator);
#endif

    /// Destroy this object.
    ~SortList();

    /// Assign to this object the value of the specified 'rhs' object.
    SortList& operator=(const SortList& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    SortList& operator=(SortList&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Item" attribute of this
    /// object.
    bsl::vector<SortListItem>& item();

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

    /// Return a reference offering non-modifiable access to the "Item"
    /// attribute of this object.
    const bsl::vector<SortListItem>& item() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const SortList& lhs, const SortList& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const SortList& lhs, const SortList& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, const SortList& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'SortList'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::SortList& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ntcdns::SortList)

namespace ntcdns {

/// This class describes the configurable parameters of a DNS client.
class ClientConfig
{
    // The override of the domain search path.  By default, the search list
    // is determined from the local domain name.  When specified, queries
    // having fewer than 'ndots' will be attempted using each component of
    // the search path in turn until a match is found.  The definition of
    // this field is mutually exclusive with the definition of the 'domain'
    // field.  When specified, this field is silently capped at 6 entries.
    bsl::vector<bsl::string> d_search;

    // The endpoint of each name server.  If unspecified, the Google Public
    // Name Server at 8.8.8.8:53 is used.
    bsl::vector<NameServerConfig> d_nameServer;

    // The local domain name.  Most queries for names within this domain
    // can use short names relative to the local domain.  If set to '.',
    // the root domain is considered.  If unspecified, the domain is
    // determined from the local hostname, where the domain is taken to be
    // everything after the first '.', except when the local hostname does
    // not contain a domain part, in which case the root domain is assumed.
    //  The definition of this field is mutually exclusive with the
    // definition of the 'search' field.
    bdlb::NullableValue<bsl::string> d_domain;

    // The sort list.
    bdlb::NullableValue<SortList> d_sortList;

    // The maximum number of attempts to resolve a name.  If unspecified,
    // the default value is the number of name servers configured.  The
    // maximum value is silently capped at 5.
    bdlb::NullableValue<unsigned int> d_attempts;

    // The timeout of each request, in seconds.  If unspecified, the
    // default value is 5 seconds.  The maximum value is silently capped at
    // 30 seconds.
    bdlb::NullableValue<unsigned int> d_timeout;

    // The threshold for the number of dots which must appear in a name
    // before an initial absolute query will be made.  If unspecified, the
    // default is 1, meaning that if there are any dots in a name, the name
    // will be tried first as an absolute name before any search list
    // element are appended to it.  The maximum value is silently capped at
    // 15.
    bdlb::NullableValue<unsigned int> d_ndots;

    // Flag indicating that the name servers should be utilized in
    // round-robin order, not in descending order of priority.  If
    // unspecified, the default value is false.
    bdlb::NullableValue<bool> d_rotate;

    // Flag indicating that debug output should be generated.  If
    // unspecified, the default value is false.
    bdlb::NullableValue<bool> d_debug;

  public:
  public:
    /// Create an object of type 'ClientConfig' having the default value.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit ClientConfig(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ClientConfig' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ClientConfig(const ClientConfig& original,
                 bslma::Allocator*   basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ClientConfig' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ClientConfig(ClientConfig&& original) noexcept;

    /// Create an object of type 'ClientConfig' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ClientConfig(ClientConfig&& original, bslma::Allocator* basicAllocator);
#endif

    /// Destroy this object.
    ~ClientConfig();

    /// Assign to this object the value of the specified 'rhs' object.
    ClientConfig& operator=(const ClientConfig& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ClientConfig& operator=(ClientConfig&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "NameServer" attribute of this
    /// object.
    bsl::vector<NameServerConfig>& nameServer();

    /// Return a reference to the modifiable "Domain" attribute of this
    /// object.
    bdlb::NullableValue<bsl::string>& domain();

    /// Return a reference to the modifiable "Search" attribute of this
    /// object.
    bsl::vector<bsl::string>& search();

    /// Return a reference to the modifiable "SortList" attribute of this
    /// object.
    bdlb::NullableValue<SortList>& sortList();

    /// Return a reference to the modifiable "Attempts" attribute of this
    /// object.
    bdlb::NullableValue<unsigned int>& attempts();

    /// Return a reference to the modifiable "Timeout" attribute of this
    /// object.
    bdlb::NullableValue<unsigned int>& timeout();

    /// Return a reference to the modifiable "Rotate" attribute of this
    /// object.
    bdlb::NullableValue<bool>& rotate();

    /// Return a reference to the modifiable "Ndots" attribute of this
    /// object.
    bdlb::NullableValue<unsigned int>& ndots();

    /// Return a reference to the modifiable "Debug" attribute of this
    /// object.
    bdlb::NullableValue<bool>& debug();

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

    /// Return a reference offering non-modifiable access to the
    /// "NameServer" attribute of this object.
    const bsl::vector<NameServerConfig>& nameServer() const;

    /// Return a reference offering non-modifiable access to the "Domain"
    /// attribute of this object.
    const bdlb::NullableValue<bsl::string>& domain() const;

    /// Return a reference offering non-modifiable access to the "Search"
    /// attribute of this object.
    const bsl::vector<bsl::string>& search() const;

    /// Return a reference offering non-modifiable access to the "SortList"
    /// attribute of this object.
    const bdlb::NullableValue<SortList>& sortList() const;

    /// Return a reference offering non-modifiable access to the "Attempts"
    /// attribute of this object.
    const bdlb::NullableValue<unsigned int>& attempts() const;

    /// Return a reference offering non-modifiable access to the "Timeout"
    /// attribute of this object.
    const bdlb::NullableValue<unsigned int>& timeout() const;

    /// Return a reference offering non-modifiable access to the "Rotate"
    /// attribute of this object.
    const bdlb::NullableValue<bool>& rotate() const;

    /// Return a reference offering non-modifiable access to the "Ndots"
    /// attribute of this object.
    const bdlb::NullableValue<unsigned int>& ndots() const;

    /// Return a reference offering non-modifiable access to the "Debug"
    /// attribute of this object.
    const bdlb::NullableValue<bool>& debug() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ClientConfig& lhs, const ClientConfig& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ClientConfig& lhs, const ClientConfig& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, const ClientConfig& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ClientConfig'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::ClientConfig& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ntcdns::ClientConfig)

namespace ntcdns {

/// This class describes the location or contents of the host database.
class HostDatabaseConfigSpec
{
    union {
        /// The path to the host database, typically '/etc/hosts' on POSIX
        /// platforms.
        bsls::ObjectBuffer<bsl::string> d_path;

        /// The host database entries.
        bsls::ObjectBuffer<HostDatabaseConfig> d_configuration;
    };

    int               d_selectionId;
    bslma::Allocator* d_allocator_p;

  public:
    enum {
        SELECTION_ID_UNDEFINED     = -1,
        SELECTION_ID_PATH          = 0,
        SELECTION_ID_CONFIGURATION = 1
    };

    /// Create an object of type 'HostDatabaseConfigSpec' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit HostDatabaseConfigSpec(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'HostDatabaseConfigSpec' having the value
    /// of the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    HostDatabaseConfigSpec(const HostDatabaseConfigSpec& original,
                           bslma::Allocator*             basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'HostDatabaseConfigSpec' having the value
    /// of the specified 'original' object.  After performing this action,
    /// the 'original' object will be left in a valid, but unspecified
    /// state.
    HostDatabaseConfigSpec(HostDatabaseConfigSpec&& original) noexcept;

    /// Create an object of type 'HostDatabaseConfigSpec' having the value
    /// of the specified 'original' object.  After performing this action,
    /// the 'original' object will be left in a valid, but unspecified
    /// state.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    HostDatabaseConfigSpec(HostDatabaseConfigSpec&& original,
                           bslma::Allocator*        basicAllocator);
#endif

    /// Destroy this object.
    ~HostDatabaseConfigSpec();

    /// Assign to this object the value of the specified 'rhs' object.
    HostDatabaseConfigSpec& operator=(const HostDatabaseConfigSpec& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    HostDatabaseConfigSpec& operator=(HostDatabaseConfigSpec&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    bsl::string& makePath();
    bsl::string& makePath(const bsl::string& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::string& makePath(bsl::string&& value);
#endif
    // Set the value of this object to be a "Path" value.  Optionally
    // specify the 'value' of the "Path".  If 'value' is not specified, the
    // default "Path" value is used.

    HostDatabaseConfig& makeConfiguration();
    HostDatabaseConfig& makeConfiguration(const HostDatabaseConfig& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    HostDatabaseConfig& makeConfiguration(HostDatabaseConfig&& value);
#endif
    // Set the value of this object to be a "Configuration" value.
    // Optionally specify the 'value' of the "Configuration".  If 'value'
    // is not specified, the default "Configuration" value is used.

    /// Return a reference to the modifiable "Path" selection of this object
    /// if "Path" is the current selection.  The behavior is undefined
    /// unless "Path" is the selection of this object.
    bsl::string& path();

    /// Return a reference to the modifiable "Configuration" selection of
    /// this object if "Configuration" is the current selection.  The
    /// behavior is undefined unless "Configuration" is the selection of
    /// this object.
    HostDatabaseConfig& configuration();

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

    /// Return the id of the current selection if the selection is defined,
    /// and -1 otherwise.
    int selectionId() const;

    /// Return a reference to the non-modifiable "Path" selection of this
    /// object if "Path" is the current selection.  The behavior is
    /// undefined unless "Path" is the selection of this object.
    const bsl::string& path() const;

    /// Return a reference to the non-modifiable "Configuration" selection
    /// of this object if "Configuration" is the current selection.  The
    /// behavior is undefined unless "Configuration" is the selection of
    /// this object.
    const HostDatabaseConfig& configuration() const;

    /// Return 'true' if the value of this object is a "Path" value, and
    /// return 'false' otherwise.
    bool isPathValue() const;

    /// Return 'true' if the value of this object is a "Configuration"
    /// value, and return 'false' otherwise.
    bool isConfigurationValue() const;

    /// Return 'true' if the value of this object is undefined, and 'false'
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char* selectionName() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' objects have the same
/// value, and 'false' otherwise.  Two 'HostDatabaseConfigSpec' objects have the same
/// value if either the selections in both objects have the same ids and
/// the same values, or both selections are undefined.
inline bool operator==(const HostDatabaseConfigSpec& lhs,
                       const HostDatabaseConfigSpec& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
/// same values, as determined by 'operator==', and 'false' otherwise.
inline bool operator!=(const HostDatabaseConfigSpec& lhs,
                       const HostDatabaseConfigSpec& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                 stream,
                                const HostDatabaseConfigSpec& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'HostDatabaseConfigSpec'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       hashAlg,
                const ntcdns::HostDatabaseConfigSpec& object);

}  // close package namespace

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::HostDatabaseConfigSpec)

namespace ntcdns {

/// This class describes the location or contents of the port database.
class PortDatabaseConfigSpec
{
    union {
        /// The path to the port database, typically '/etc/services' on
        /// POSIX platforms.
        bsls::ObjectBuffer<bsl::string> d_path;

        /// The port database entries.
        bsls::ObjectBuffer<PortDatabaseConfig> d_configuration;
    };

    int               d_selectionId;
    bslma::Allocator* d_allocator_p;

  public:
    enum {
        SELECTION_ID_UNDEFINED     = -1,
        SELECTION_ID_PATH          = 0,
        SELECTION_ID_CONFIGURATION = 1
    };

    /// Create an object of type 'PortDatabaseConfigSpec' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit PortDatabaseConfigSpec(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'PortDatabaseConfigSpec' having the value
    /// of the specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    PortDatabaseConfigSpec(const PortDatabaseConfigSpec& original,
                           bslma::Allocator*             basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'PortDatabaseConfigSpec' having the value
    /// of the specified 'original' object.  After performing this action,
    /// the 'original' object will be left in a valid, but unspecified
    /// state.
    PortDatabaseConfigSpec(PortDatabaseConfigSpec&& original) noexcept;

    /// Create an object of type 'PortDatabaseConfigSpec' having the value
    /// of the specified 'original' object.  After performing this action,
    /// the 'original' object will be left in a valid, but unspecified
    /// state.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    PortDatabaseConfigSpec(PortDatabaseConfigSpec&& original,
                           bslma::Allocator*        basicAllocator);
#endif

    /// Destroy this object.
    ~PortDatabaseConfigSpec();

    /// Assign to this object the value of the specified 'rhs' object.
    PortDatabaseConfigSpec& operator=(const PortDatabaseConfigSpec& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    PortDatabaseConfigSpec& operator=(PortDatabaseConfigSpec&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    bsl::string& makePath();
    bsl::string& makePath(const bsl::string& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::string& makePath(bsl::string&& value);
#endif
    // Set the value of this object to be a "Path" value.  Optionally
    // specify the 'value' of the "Path".  If 'value' is not specified, the
    // default "Path" value is used.

    PortDatabaseConfig& makeConfiguration();
    PortDatabaseConfig& makeConfiguration(const PortDatabaseConfig& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    PortDatabaseConfig& makeConfiguration(PortDatabaseConfig&& value);
#endif
    // Set the value of this object to be a "Configuration" value.
    // Optionally specify the 'value' of the "Configuration".  If 'value'
    // is not specified, the default "Configuration" value is used.

    /// Return a reference to the modifiable "Path" selection of this object
    /// if "Path" is the current selection.  The behavior is undefined
    /// unless "Path" is the selection of this object.
    bsl::string& path();

    /// Return a reference to the modifiable "Configuration" selection of
    /// this object if "Configuration" is the current selection.  The
    /// behavior is undefined unless "Configuration" is the selection of
    /// this object.
    PortDatabaseConfig& configuration();

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

    /// Return the id of the current selection if the selection is defined,
    /// and -1 otherwise.
    int selectionId() const;

    /// Return a reference to the non-modifiable "Path" selection of this
    /// object if "Path" is the current selection.  The behavior is
    /// undefined unless "Path" is the selection of this object.
    const bsl::string& path() const;

    /// Return a reference to the non-modifiable "Configuration" selection
    /// of this object if "Configuration" is the current selection.  The
    /// behavior is undefined unless "Configuration" is the selection of
    /// this object.
    const PortDatabaseConfig& configuration() const;

    /// Return 'true' if the value of this object is a "Path" value, and
    /// return 'false' otherwise.
    bool isPathValue() const;

    /// Return 'true' if the value of this object is a "Configuration"
    /// value, and return 'false' otherwise.
    bool isConfigurationValue() const;

    /// Return 'true' if the value of this object is undefined, and 'false'
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char* selectionName() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' objects have the same
/// value, and 'false' otherwise.  Two 'PortDatabaseConfigSpec' objects have the same
/// value if either the selections in both objects have the same ids and
/// the same values, or both selections are undefined.
inline bool operator==(const PortDatabaseConfigSpec& lhs,
                       const PortDatabaseConfigSpec& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
/// same values, as determined by 'operator==', and 'false' otherwise.
inline bool operator!=(const PortDatabaseConfigSpec& lhs,
                       const PortDatabaseConfigSpec& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&                 stream,
                                const PortDatabaseConfigSpec& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'PortDatabaseConfigSpec'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       hashAlg,
                const ntcdns::PortDatabaseConfigSpec& object);

}  // close package namespace

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::PortDatabaseConfigSpec)

namespace ntcdns {

/// This class describes the configurable parameters of a DNS server.
class ServerConfig
{
    // The endpoint of the name server.
    NameServerConfig d_nameServer;

    // The timeout of each request, in seconds.
    unsigned int d_timeout;

  public:
  public:
    /// Create an object of type 'ServerConfig' having the default value.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit ServerConfig(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ServerConfig' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ServerConfig(const ServerConfig& original,
                 bslma::Allocator*   basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ServerConfig' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ServerConfig(ServerConfig&& original) noexcept;

    /// Create an object of type 'ServerConfig' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ServerConfig(ServerConfig&& original, bslma::Allocator* basicAllocator);
#endif

    /// Destroy this object.
    ~ServerConfig();

    /// Assign to this object the value of the specified 'rhs' object.
    ServerConfig& operator=(const ServerConfig& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ServerConfig& operator=(ServerConfig&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Timeout" attribute of this
    /// object.
    unsigned int& timeout();

    /// Return a reference to the modifiable "NameServer" attribute of this
    /// object.
    NameServerConfig& nameServer();

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

    /// Return the value of the "Timeout" attribute of this object.
    unsigned int timeout() const;

    /// Return a reference offering non-modifiable access to the
    /// "NameServer" attribute of this object.
    const NameServerConfig& nameServer() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ServerConfig& lhs, const ServerConfig& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ServerConfig& lhs, const ServerConfig& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream& stream, const ServerConfig& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ServerConfig'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::ServerConfig& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(ntcdns::ServerConfig)

namespace ntcdns {

/// This class describes the location or parameters of the client
/// configuration.
class ClientConfigSpec
{
    union {
        /// The path to the client parameters, typically '/etc/resolv.conf'
        /// on POSIX platforms.
        bsls::ObjectBuffer<bsl::string> d_path;

        /// The host database entries
        bsls::ObjectBuffer<ClientConfig> d_configuration;
    };

    int               d_selectionId;
    bslma::Allocator* d_allocator_p;

  public:
    enum {
        SELECTION_ID_UNDEFINED     = -1,
        SELECTION_ID_PATH          = 0,
        SELECTION_ID_CONFIGURATION = 1
    };

    /// Create an object of type 'ClientConfigSpec' having the default
    /// value.  Use the optionally specified 'basicAllocator' to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit ClientConfigSpec(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ClientConfigSpec' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ClientConfigSpec(const ClientConfigSpec& original,
                     bslma::Allocator*       basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ClientConfigSpec' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ClientConfigSpec(ClientConfigSpec&& original) noexcept;

    /// Create an object of type 'ClientConfigSpec' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ClientConfigSpec(ClientConfigSpec&& original,
                     bslma::Allocator*  basicAllocator);
#endif

    /// Destroy this object.
    ~ClientConfigSpec();

    /// Assign to this object the value of the specified 'rhs' object.
    ClientConfigSpec& operator=(const ClientConfigSpec& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ClientConfigSpec& operator=(ClientConfigSpec&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    bsl::string& makePath();
    bsl::string& makePath(const bsl::string& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::string& makePath(bsl::string&& value);
#endif
    // Set the value of this object to be a "Path" value.  Optionally
    // specify the 'value' of the "Path".  If 'value' is not specified, the
    // default "Path" value is used.

    ClientConfig& makeConfiguration();
    ClientConfig& makeConfiguration(const ClientConfig& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    ClientConfig& makeConfiguration(ClientConfig&& value);
#endif
    // Set the value of this object to be a "Configuration" value.
    // Optionally specify the 'value' of the "Configuration".  If 'value'
    // is not specified, the default "Configuration" value is used.

    /// Return a reference to the modifiable "Path" selection of this object
    /// if "Path" is the current selection.  The behavior is undefined
    /// unless "Path" is the selection of this object.
    bsl::string& path();

    /// Return a reference to the modifiable "Configuration" selection of
    /// this object if "Configuration" is the current selection.  The
    /// behavior is undefined unless "Configuration" is the selection of
    /// this object.
    ClientConfig& configuration();

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

    /// Return the id of the current selection if the selection is defined,
    /// and -1 otherwise.
    int selectionId() const;

    /// Return a reference to the non-modifiable "Path" selection of this
    /// object if "Path" is the current selection.  The behavior is
    /// undefined unless "Path" is the selection of this object.
    const bsl::string& path() const;

    /// Return a reference to the non-modifiable "Configuration" selection
    /// of this object if "Configuration" is the current selection.  The
    /// behavior is undefined unless "Configuration" is the selection of
    /// this object.
    const ClientConfig& configuration() const;

    /// Return 'true' if the value of this object is a "Path" value, and
    /// return 'false' otherwise.
    bool isPathValue() const;

    /// Return 'true' if the value of this object is a "Configuration"
    /// value, and return 'false' otherwise.
    bool isConfigurationValue() const;

    /// Return 'true' if the value of this object is undefined, and 'false'
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char* selectionName() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' objects have the same
/// value, and 'false' otherwise.  Two 'ClientConfigSpec' objects have the same
/// value if either the selections in both objects have the same ids and
/// the same values, or both selections are undefined.
inline bool operator==(const ClientConfigSpec& lhs,
                       const ClientConfigSpec& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
/// same values, as determined by 'operator==', and 'false' otherwise.
inline bool operator!=(const ClientConfigSpec& lhs,
                       const ClientConfigSpec& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&           stream,
                                const ClientConfigSpec& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ClientConfigSpec'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 hashAlg,
                const ntcdns::ClientConfigSpec& object);

}  // close package namespace

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ClientConfigSpec)

namespace ntcdns {

/// This class describes the configurable parameters of a resolver.
class ResolverConfig
{
    // The known associations between a service, its protocol, and its port
    // within that protocol.  This information is traditionally loaded from
    // "/etc/services" on POSIX systems.
    bdlb::NullableValue<PortDatabaseConfigSpec> d_portDatabase;

    // The known associations between an addresses, its canonical hostname,
    // and its aliases, if any.  This information is traditionally loaded
    // from "/etc/hosts" on POSIX systems.
    bdlb::NullableValue<HostDatabaseConfigSpec> d_hostDatabase;

    // The configurable parameters of the DNS client mechanism.  This
    // information is traditionally loaded from "/etc/resolv.conf" on POSIX
    // systems.
    bdlb::NullableValue<ClientConfigSpec> d_client;

  public:
  public:
    /// Create an object of type 'ResolverConfig' having the default value.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit ResolverConfig(bslma::Allocator* basicAllocator = 0);

    /// Create an object of type 'ResolverConfig' having the value of the
    /// specified 'original' object.  Use the optionally specified
    /// 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ResolverConfig(const ResolverConfig& original,
                   bslma::Allocator*     basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type 'ResolverConfig' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    ResolverConfig(ResolverConfig&& original) noexcept;

    /// Create an object of type 'ResolverConfig' having the value of the
    /// specified 'original' object.  After performing this action, the
    /// 'original' object will be left in a valid, but unspecified state.
    /// Use the optionally specified 'basicAllocator' to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ResolverConfig(ResolverConfig&&  original,
                   bslma::Allocator* basicAllocator);
#endif

    /// Destroy this object.
    ~ResolverConfig();

    /// Assign to this object the value of the specified 'rhs' object.
    ResolverConfig& operator=(const ResolverConfig& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified 'rhs' object.
    /// After performing this action, the 'rhs' object will be left in a
    /// valid, but unspecified state.
    ResolverConfig& operator=(ResolverConfig&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Return a reference to the modifiable "Client" attribute of this
    /// object.
    bdlb::NullableValue<ClientConfigSpec>& client();

    /// Return a reference to the modifiable "HostDatabase" attribute of
    /// this object.
    bdlb::NullableValue<HostDatabaseConfigSpec>& hostDatabase();

    /// Return a reference to the modifiable "PortDatabase" attribute of
    /// this object.
    bdlb::NullableValue<PortDatabaseConfigSpec>& portDatabase();

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

    /// Return a reference offering non-modifiable access to the "Client"
    /// attribute of this object.
    const bdlb::NullableValue<ClientConfigSpec>& client() const;

    /// Return a reference offering non-modifiable access to the
    /// "HostDatabase" attribute of this object.
    const bdlb::NullableValue<HostDatabaseConfigSpec>& hostDatabase() const;

    /// Return a reference offering non-modifiable access to the
    /// "PortDatabase" attribute of this object.
    const bdlb::NullableValue<PortDatabaseConfigSpec>& portDatabase() const;
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline bool operator==(const ResolverConfig& lhs, const ResolverConfig& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline bool operator!=(const ResolverConfig& lhs, const ResolverConfig& rhs);

/// Format the specified 'rhs' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
inline bsl::ostream& operator<<(bsl::ostream&         stream,
                                const ResolverConfig& rhs);

/// Pass the specified 'object' to the specified 'hashAlg'.  This function
/// integrates with the 'bslh' modular hashing system and effectively
/// provides a 'bsl::hash' specialization for 'ResolverConfig'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::ResolverConfig& object);

}  // close package namespace

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcdns::ResolverConfig)

//                         INLINE FUNCTION DEFINITIONS

namespace ntcdns {

inline int Classification::fromString(Value* result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline bsl::ostream& Classification::print(bsl::ostream&         stream,
                                           Classification::Value value)
{
    return stream << toString(value);
}

inline int Direction::fromString(Value* result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline bsl::ostream& Direction::print(bsl::ostream&    stream,
                                      Direction::Value value)
{
    return stream << toString(value);
}

inline int Error::fromString(Value* result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline bsl::ostream& Error::print(bsl::ostream& stream, Error::Value value)
{
    return stream << toString(value);
}

inline bsl::string& HostEntry::address()
{
    return d_address;
}

inline bsl::string& HostEntry::canonicalHostname()
{
    return d_canonicalHostname;
}

inline bsl::vector<bsl::string>& HostEntry::aliases()
{
    return d_aliases;
}

inline bdlb::NullableValue<bsl::uint64_t>& HostEntry::expiration()
{
    return d_expiration;
}

inline const bsl::string& HostEntry::address() const
{
    return d_address;
}

inline const bsl::string& HostEntry::canonicalHostname() const
{
    return d_canonicalHostname;
}

inline const bsl::vector<bsl::string>& HostEntry::aliases() const
{
    return d_aliases;
}

inline const bdlb::NullableValue<bsl::uint64_t>& HostEntry::expiration() const
{
    return d_expiration;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::HostEntry& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.address());
    hashAppend(hashAlg, object.canonicalHostname());
    hashAppend(hashAlg, object.aliases());
    hashAppend(hashAlg, object.expiration());
}

inline Label::Label(bslma::Allocator* basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline Label::~Label()
{
    reset();
}

inline bsl::string& Label::text()
{
    BSLS_ASSERT(SELECTION_ID_TEXT == d_selectionId);
    return d_text.object();
}

inline unsigned short& Label::offset()
{
    BSLS_ASSERT(SELECTION_ID_OFFSET == d_selectionId);
    return d_offset.object();
}

inline int Label::selectionId() const
{
    return d_selectionId;
}

inline const bsl::string& Label::text() const
{
    BSLS_ASSERT(SELECTION_ID_TEXT == d_selectionId);
    return d_text.object();
}

inline const unsigned short& Label::offset() const
{
    BSLS_ASSERT(SELECTION_ID_OFFSET == d_selectionId);
    return d_offset.object();
}

inline bool Label::isTextValue() const
{
    return SELECTION_ID_TEXT == d_selectionId;
}

inline bool Label::isOffsetValue() const
{
    return SELECTION_ID_OFFSET == d_selectionId;
}

inline bool Label::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::Label& object)
{
    typedef ntcdns::Label Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
    case Class::SELECTION_ID_TEXT:
        hashAppend(hashAlg, object.text());
        break;
    case Class::SELECTION_ID_OFFSET:
        hashAppend(hashAlg, object.offset());
        break;
    default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}

inline bsl::string& NameServerAddress::host()
{
    return d_host;
}

inline bdlb::NullableValue<unsigned short>& NameServerAddress::port()
{
    return d_port;
}

inline const bsl::string& NameServerAddress::host() const
{
    return d_host;
}

inline const bdlb::NullableValue<unsigned short>& NameServerAddress::port()
    const
{
    return d_port;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                  hashAlg,
                const ntcdns::NameServerAddress& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.host());
    hashAppend(hashAlg, object.port());
}

inline int Operation::fromString(Value* result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline bsl::ostream& Operation::print(bsl::ostream&    stream,
                                      Operation::Value value)
{
    return stream << toString(value);
}

inline bsl::string& PortEntry::service()
{
    return d_service;
}

inline bsl::string& PortEntry::protocol()
{
    return d_protocol;
}

inline unsigned short& PortEntry::port()
{
    return d_port;
}

inline bdlb::NullableValue<bsl::uint64_t>& PortEntry::expiration()
{
    return d_expiration;
}

inline const bsl::string& PortEntry::service() const
{
    return d_service;
}

inline const bsl::string& PortEntry::protocol() const
{
    return d_protocol;
}

inline unsigned short PortEntry::port() const
{
    return d_port;
}

inline const bdlb::NullableValue<bsl::uint64_t>& PortEntry::expiration() const
{
    return d_expiration;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::PortEntry& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.service());
    hashAppend(hashAlg, object.protocol());
    hashAppend(hashAlg, object.port());
    hashAppend(hashAlg, object.expiration());
}

inline unsigned int& ResourceRecordDataA::address()
{
    return d_address;
}

inline unsigned int ResourceRecordDataA::address() const
{
    return d_address;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                    hashAlg,
                const ntcdns::ResourceRecordDataA& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.address());
}

inline unsigned int& ResourceRecordDataAAAA::address0()
{
    return d_address0;
}

inline unsigned int& ResourceRecordDataAAAA::address1()
{
    return d_address1;
}

inline unsigned int& ResourceRecordDataAAAA::address2()
{
    return d_address2;
}

inline unsigned int& ResourceRecordDataAAAA::address3()
{
    return d_address3;
}

inline unsigned int ResourceRecordDataAAAA::address0() const
{
    return d_address0;
}

inline unsigned int ResourceRecordDataAAAA::address1() const
{
    return d_address1;
}

inline unsigned int ResourceRecordDataAAAA::address2() const
{
    return d_address2;
}

inline unsigned int ResourceRecordDataAAAA::address3() const
{
    return d_address3;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       hashAlg,
                const ntcdns::ResourceRecordDataAAAA& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.address0());
    hashAppend(hashAlg, object.address1());
    hashAppend(hashAlg, object.address2());
    hashAppend(hashAlg, object.address3());
}

inline bsl::string& ResourceRecordDataCname::cname()
{
    return d_cname;
}

inline const bsl::string& ResourceRecordDataCname::cname() const
{
    return d_cname;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                        hashAlg,
                const ntcdns::ResourceRecordDataCname& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.cname());
}

inline bsl::string& ResourceRecordDataHinfo::cpu()
{
    return d_cpu;
}

inline bsl::string& ResourceRecordDataHinfo::os()
{
    return d_os;
}

inline const bsl::string& ResourceRecordDataHinfo::cpu() const
{
    return d_cpu;
}

inline const bsl::string& ResourceRecordDataHinfo::os() const
{
    return d_os;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                        hashAlg,
                const ntcdns::ResourceRecordDataHinfo& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.cpu());
    hashAppend(hashAlg, object.os());
}

inline unsigned short& ResourceRecordDataMx::preference()
{
    return d_preference;
}

inline bsl::string& ResourceRecordDataMx::exchange()
{
    return d_exchange;
}

inline unsigned short ResourceRecordDataMx::preference() const
{
    return d_preference;
}

inline const bsl::string& ResourceRecordDataMx::exchange() const
{
    return d_exchange;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     hashAlg,
                const ntcdns::ResourceRecordDataMx& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.preference());
    hashAppend(hashAlg, object.exchange());
}

inline bsl::string& ResourceRecordDataNs::nsdname()
{
    return d_nsdname;
}

inline const bsl::string& ResourceRecordDataNs::nsdname() const
{
    return d_nsdname;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                     hashAlg,
                const ntcdns::ResourceRecordDataNs& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.nsdname());
}

inline bsl::string& ResourceRecordDataPtr::ptrdname()
{
    return d_ptrdname;
}

inline const bsl::string& ResourceRecordDataPtr::ptrdname() const
{
    return d_ptrdname;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataPtr& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.ptrdname());
}

inline bsl::vector<char>& ResourceRecordDataRaw::data()
{
    return d_data;
}

inline const bsl::vector<char>& ResourceRecordDataRaw::data() const
{
    return d_data;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataRaw& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.data());
}

inline bsl::string& ResourceRecordDataSoa::mname()
{
    return d_mname;
}

inline bsl::string& ResourceRecordDataSoa::rname()
{
    return d_rname;
}

inline unsigned int& ResourceRecordDataSoa::serial()
{
    return d_serial;
}

inline unsigned int& ResourceRecordDataSoa::refresh()
{
    return d_refresh;
}

inline unsigned int& ResourceRecordDataSoa::retry()
{
    return d_retry;
}

inline unsigned int& ResourceRecordDataSoa::expire()
{
    return d_expire;
}

inline unsigned int& ResourceRecordDataSoa::minimum()
{
    return d_minimum;
}

inline const bsl::string& ResourceRecordDataSoa::mname() const
{
    return d_mname;
}

inline const bsl::string& ResourceRecordDataSoa::rname() const
{
    return d_rname;
}

inline unsigned int ResourceRecordDataSoa::serial() const
{
    return d_serial;
}

inline unsigned int ResourceRecordDataSoa::refresh() const
{
    return d_refresh;
}

inline unsigned int ResourceRecordDataSoa::retry() const
{
    return d_retry;
}

inline unsigned int ResourceRecordDataSoa::expire() const
{
    return d_expire;
}

inline unsigned int ResourceRecordDataSoa::minimum() const
{
    return d_minimum;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataSoa& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.mname());
    hashAppend(hashAlg, object.rname());
    hashAppend(hashAlg, object.serial());
    hashAppend(hashAlg, object.refresh());
    hashAppend(hashAlg, object.retry());
    hashAppend(hashAlg, object.expire());
    hashAppend(hashAlg, object.minimum());
}

inline bsl::string& ResourceRecordDataSvr::name()
{
    return d_name;
}

inline unsigned int& ResourceRecordDataSvr::ttl()
{
    return d_ttl;
}

inline unsigned short& ResourceRecordDataSvr::classification()
{
    return d_classification;
}

inline unsigned short& ResourceRecordDataSvr::priority()
{
    return d_priority;
}

inline unsigned short& ResourceRecordDataSvr::weight()
{
    return d_weight;
}

inline unsigned short& ResourceRecordDataSvr::port()
{
    return d_port;
}

inline bsl::string& ResourceRecordDataSvr::target()
{
    return d_target;
}

inline const bsl::string& ResourceRecordDataSvr::name() const
{
    return d_name;
}

inline unsigned int ResourceRecordDataSvr::ttl() const
{
    return d_ttl;
}

inline unsigned short ResourceRecordDataSvr::classification() const
{
    return d_classification;
}

inline unsigned short ResourceRecordDataSvr::priority() const
{
    return d_priority;
}

inline unsigned short ResourceRecordDataSvr::weight() const
{
    return d_weight;
}

inline unsigned short ResourceRecordDataSvr::port() const
{
    return d_port;
}

inline const bsl::string& ResourceRecordDataSvr::target() const
{
    return d_target;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataSvr& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.name());
    hashAppend(hashAlg, object.ttl());
    hashAppend(hashAlg, object.classification());
    hashAppend(hashAlg, object.priority());
    hashAppend(hashAlg, object.weight());
    hashAppend(hashAlg, object.port());
    hashAppend(hashAlg, object.target());
}

inline bsl::vector<bsl::string>& ResourceRecordDataTxt::text()
{
    return d_text;
}

inline const bsl::vector<bsl::string>& ResourceRecordDataTxt::text() const
{
    return d_text;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataTxt& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.text());
}

inline unsigned int& ResourceRecordDataWks::address()
{
    return d_address;
}

inline unsigned char& ResourceRecordDataWks::protocol()
{
    return d_protocol;
}

inline bsl::vector<unsigned short>& ResourceRecordDataWks::port()
{
    return d_port;
}

inline unsigned int ResourceRecordDataWks::address() const
{
    return d_address;
}

inline unsigned char ResourceRecordDataWks::protocol() const
{
    return d_protocol;
}

inline const bsl::vector<unsigned short>& ResourceRecordDataWks::port() const
{
    return d_port;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      hashAlg,
                const ntcdns::ResourceRecordDataWks& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.address());
    hashAppend(hashAlg, object.protocol());
    hashAppend(hashAlg, object.port());
}

inline bsl::string& SortListItem::address()
{
    return d_address;
}

inline bsl::string& SortListItem::netmask()
{
    return d_netmask;
}

inline const bsl::string& SortListItem::address() const
{
    return d_address;
}

inline const bsl::string& SortListItem::netmask() const
{
    return d_netmask;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::SortListItem& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.address());
    hashAppend(hashAlg, object.netmask());
}

inline int Type::fromString(Value* result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline bsl::ostream& Type::print(bsl::ostream& stream, Type::Value value)
{
    return stream << toString(value);
}

inline bsl::vector<HostEntry>& HostDatabaseConfig::entry()
{
    return d_entry;
}

inline const bsl::vector<HostEntry>& HostDatabaseConfig::entry() const
{
    return d_entry;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   hashAlg,
                const ntcdns::HostDatabaseConfig& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.entry());
}

inline NameServerAddress& NameServerConfig::address()
{
    return d_address;
}

inline const NameServerAddress& NameServerConfig::address() const
{
    return d_address;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 hashAlg,
                const ntcdns::NameServerConfig& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.address());
}

inline bsl::vector<PortEntry>& PortDatabaseConfig::entry()
{
    return d_entry;
}

inline const bsl::vector<PortEntry>& PortDatabaseConfig::entry() const
{
    return d_entry;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   hashAlg,
                const ntcdns::PortDatabaseConfig& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.entry());
}

inline ResourceRecordData::ResourceRecordData(bslma::Allocator* basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline ResourceRecordData::~ResourceRecordData()
{
    reset();
}

inline ResourceRecordDataCname& ResourceRecordData::canonicalName()
{
    BSLS_ASSERT(SELECTION_ID_CANONICAL_NAME == d_selectionId);
    return d_canonicalName.object();
}

inline ResourceRecordDataHinfo& ResourceRecordData::hostInfo()
{
    BSLS_ASSERT(SELECTION_ID_HOST_INFO == d_selectionId);
    return d_hostInfo.object();
}

inline ResourceRecordDataMx& ResourceRecordData::mailExchange()
{
    BSLS_ASSERT(SELECTION_ID_MAIL_EXCHANGE == d_selectionId);
    return d_mailExchange.object();
}

inline ResourceRecordDataNs& ResourceRecordData::nameServer()
{
    BSLS_ASSERT(SELECTION_ID_NAME_SERVER == d_selectionId);
    return d_nameServer.object();
}

inline ResourceRecordDataPtr& ResourceRecordData::pointer()
{
    BSLS_ASSERT(SELECTION_ID_POINTER == d_selectionId);
    return d_pointer.object();
}

inline ResourceRecordDataSoa& ResourceRecordData::zoneAuthority()
{
    BSLS_ASSERT(SELECTION_ID_ZONE_AUTHORITY == d_selectionId);
    return d_zoneAuthority.object();
}

inline ResourceRecordDataWks& ResourceRecordData::wellKnownService()
{
    BSLS_ASSERT(SELECTION_ID_WELL_KNOWN_SERVICE == d_selectionId);
    return d_wellKnownService.object();
}

inline ResourceRecordDataTxt& ResourceRecordData::text()
{
    BSLS_ASSERT(SELECTION_ID_TEXT == d_selectionId);
    return d_text.object();
}

inline ResourceRecordDataA& ResourceRecordData::ipv4()
{
    BSLS_ASSERT(SELECTION_ID_IPV4 == d_selectionId);
    return d_ipv4.object();
}

inline ResourceRecordDataAAAA& ResourceRecordData::ipv6()
{
    BSLS_ASSERT(SELECTION_ID_IPV6 == d_selectionId);
    return d_ipv6.object();
}

inline ResourceRecordDataSvr& ResourceRecordData::server()
{
    BSLS_ASSERT(SELECTION_ID_SERVER == d_selectionId);
    return d_server.object();
}

inline ResourceRecordDataRaw& ResourceRecordData::raw()
{
    BSLS_ASSERT(SELECTION_ID_RAW == d_selectionId);
    return d_raw.object();
}

inline int ResourceRecordData::selectionId() const
{
    return d_selectionId;
}

inline const ResourceRecordDataCname& ResourceRecordData::canonicalName() const
{
    BSLS_ASSERT(SELECTION_ID_CANONICAL_NAME == d_selectionId);
    return d_canonicalName.object();
}

inline const ResourceRecordDataHinfo& ResourceRecordData::hostInfo() const
{
    BSLS_ASSERT(SELECTION_ID_HOST_INFO == d_selectionId);
    return d_hostInfo.object();
}

inline const ResourceRecordDataMx& ResourceRecordData::mailExchange() const
{
    BSLS_ASSERT(SELECTION_ID_MAIL_EXCHANGE == d_selectionId);
    return d_mailExchange.object();
}

inline const ResourceRecordDataNs& ResourceRecordData::nameServer() const
{
    BSLS_ASSERT(SELECTION_ID_NAME_SERVER == d_selectionId);
    return d_nameServer.object();
}

inline const ResourceRecordDataPtr& ResourceRecordData::pointer() const
{
    BSLS_ASSERT(SELECTION_ID_POINTER == d_selectionId);
    return d_pointer.object();
}

inline const ResourceRecordDataSoa& ResourceRecordData::zoneAuthority() const
{
    BSLS_ASSERT(SELECTION_ID_ZONE_AUTHORITY == d_selectionId);
    return d_zoneAuthority.object();
}

inline const ResourceRecordDataWks& ResourceRecordData::wellKnownService()
    const
{
    BSLS_ASSERT(SELECTION_ID_WELL_KNOWN_SERVICE == d_selectionId);
    return d_wellKnownService.object();
}

inline const ResourceRecordDataTxt& ResourceRecordData::text() const
{
    BSLS_ASSERT(SELECTION_ID_TEXT == d_selectionId);
    return d_text.object();
}

inline const ResourceRecordDataA& ResourceRecordData::ipv4() const
{
    BSLS_ASSERT(SELECTION_ID_IPV4 == d_selectionId);
    return d_ipv4.object();
}

inline const ResourceRecordDataAAAA& ResourceRecordData::ipv6() const
{
    BSLS_ASSERT(SELECTION_ID_IPV6 == d_selectionId);
    return d_ipv6.object();
}

inline const ResourceRecordDataSvr& ResourceRecordData::server() const
{
    BSLS_ASSERT(SELECTION_ID_SERVER == d_selectionId);
    return d_server.object();
}

inline const ResourceRecordDataRaw& ResourceRecordData::raw() const
{
    BSLS_ASSERT(SELECTION_ID_RAW == d_selectionId);
    return d_raw.object();
}

inline bool ResourceRecordData::isCanonicalNameValue() const
{
    return SELECTION_ID_CANONICAL_NAME == d_selectionId;
}

inline bool ResourceRecordData::isHostInfoValue() const
{
    return SELECTION_ID_HOST_INFO == d_selectionId;
}

inline bool ResourceRecordData::isMailExchangeValue() const
{
    return SELECTION_ID_MAIL_EXCHANGE == d_selectionId;
}

inline bool ResourceRecordData::isNameServerValue() const
{
    return SELECTION_ID_NAME_SERVER == d_selectionId;
}

inline bool ResourceRecordData::isPointerValue() const
{
    return SELECTION_ID_POINTER == d_selectionId;
}

inline bool ResourceRecordData::isZoneAuthorityValue() const
{
    return SELECTION_ID_ZONE_AUTHORITY == d_selectionId;
}

inline bool ResourceRecordData::isWellKnownServiceValue() const
{
    return SELECTION_ID_WELL_KNOWN_SERVICE == d_selectionId;
}

inline bool ResourceRecordData::isTextValue() const
{
    return SELECTION_ID_TEXT == d_selectionId;
}

inline bool ResourceRecordData::isIpv4Value() const
{
    return SELECTION_ID_IPV4 == d_selectionId;
}

inline bool ResourceRecordData::isIpv6Value() const
{
    return SELECTION_ID_IPV6 == d_selectionId;
}

inline bool ResourceRecordData::isServerValue() const
{
    return SELECTION_ID_SERVER == d_selectionId;
}

inline bool ResourceRecordData::isRawValue() const
{
    return SELECTION_ID_RAW == d_selectionId;
}

inline bool ResourceRecordData::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                   hashAlg,
                const ntcdns::ResourceRecordData& object)
{
    typedef ntcdns::ResourceRecordData Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
    case Class::SELECTION_ID_CANONICAL_NAME:
        hashAppend(hashAlg, object.canonicalName());
        break;
    case Class::SELECTION_ID_HOST_INFO:
        hashAppend(hashAlg, object.hostInfo());
        break;
    case Class::SELECTION_ID_MAIL_EXCHANGE:
        hashAppend(hashAlg, object.mailExchange());
        break;
    case Class::SELECTION_ID_NAME_SERVER:
        hashAppend(hashAlg, object.nameServer());
        break;
    case Class::SELECTION_ID_POINTER:
        hashAppend(hashAlg, object.pointer());
        break;
    case Class::SELECTION_ID_ZONE_AUTHORITY:
        hashAppend(hashAlg, object.zoneAuthority());
        break;
    case Class::SELECTION_ID_WELL_KNOWN_SERVICE:
        hashAppend(hashAlg, object.wellKnownService());
        break;
    case Class::SELECTION_ID_TEXT:
        hashAppend(hashAlg, object.text());
        break;
    case Class::SELECTION_ID_IPV4:
        hashAppend(hashAlg, object.ipv4());
        break;
    case Class::SELECTION_ID_IPV6:
        hashAppend(hashAlg, object.ipv6());
        break;
    case Class::SELECTION_ID_SERVER:
        hashAppend(hashAlg, object.server());
        break;
    case Class::SELECTION_ID_RAW:
        hashAppend(hashAlg, object.raw());
        break;
    default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}

inline bsl::vector<SortListItem>& SortList::item()
{
    return d_item;
}

inline const bsl::vector<SortListItem>& SortList::item() const
{
    return d_item;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::SortList& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.item());
}

inline bsl::vector<NameServerConfig>& ClientConfig::nameServer()
{
    return d_nameServer;
}

inline bdlb::NullableValue<bsl::string>& ClientConfig::domain()
{
    return d_domain;
}

inline bsl::vector<bsl::string>& ClientConfig::search()
{
    return d_search;
}

inline bdlb::NullableValue<SortList>& ClientConfig::sortList()
{
    return d_sortList;
}

inline bdlb::NullableValue<unsigned int>& ClientConfig::attempts()
{
    return d_attempts;
}

inline bdlb::NullableValue<unsigned int>& ClientConfig::timeout()
{
    return d_timeout;
}

inline bdlb::NullableValue<bool>& ClientConfig::rotate()
{
    return d_rotate;
}

inline bdlb::NullableValue<unsigned int>& ClientConfig::ndots()
{
    return d_ndots;
}

inline bdlb::NullableValue<bool>& ClientConfig::debug()
{
    return d_debug;
}

inline const bsl::vector<NameServerConfig>& ClientConfig::nameServer() const
{
    return d_nameServer;
}

inline const bdlb::NullableValue<bsl::string>& ClientConfig::domain() const
{
    return d_domain;
}

inline const bsl::vector<bsl::string>& ClientConfig::search() const
{
    return d_search;
}

inline const bdlb::NullableValue<SortList>& ClientConfig::sortList() const
{
    return d_sortList;
}

inline const bdlb::NullableValue<unsigned int>& ClientConfig::attempts() const
{
    return d_attempts;
}

inline const bdlb::NullableValue<unsigned int>& ClientConfig::timeout() const
{
    return d_timeout;
}

inline const bdlb::NullableValue<bool>& ClientConfig::rotate() const
{
    return d_rotate;
}

inline const bdlb::NullableValue<unsigned int>& ClientConfig::ndots() const
{
    return d_ndots;
}

inline const bdlb::NullableValue<bool>& ClientConfig::debug() const
{
    return d_debug;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::ClientConfig& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.nameServer());
    hashAppend(hashAlg, object.domain());
    hashAppend(hashAlg, object.search());
    hashAppend(hashAlg, object.sortList());
    hashAppend(hashAlg, object.attempts());
    hashAppend(hashAlg, object.timeout());
    hashAppend(hashAlg, object.rotate());
    hashAppend(hashAlg, object.ndots());
    hashAppend(hashAlg, object.debug());
}

inline HostDatabaseConfigSpec::HostDatabaseConfigSpec(
    bslma::Allocator* basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline HostDatabaseConfigSpec::~HostDatabaseConfigSpec()
{
    reset();
}

inline bsl::string& HostDatabaseConfigSpec::path()
{
    BSLS_ASSERT(SELECTION_ID_PATH == d_selectionId);
    return d_path.object();
}

inline HostDatabaseConfig& HostDatabaseConfigSpec::configuration()
{
    BSLS_ASSERT(SELECTION_ID_CONFIGURATION == d_selectionId);
    return d_configuration.object();
}

inline int HostDatabaseConfigSpec::selectionId() const
{
    return d_selectionId;
}

inline const bsl::string& HostDatabaseConfigSpec::path() const
{
    BSLS_ASSERT(SELECTION_ID_PATH == d_selectionId);
    return d_path.object();
}

inline const HostDatabaseConfig& HostDatabaseConfigSpec::configuration() const
{
    BSLS_ASSERT(SELECTION_ID_CONFIGURATION == d_selectionId);
    return d_configuration.object();
}

inline bool HostDatabaseConfigSpec::isPathValue() const
{
    return SELECTION_ID_PATH == d_selectionId;
}

inline bool HostDatabaseConfigSpec::isConfigurationValue() const
{
    return SELECTION_ID_CONFIGURATION == d_selectionId;
}

inline bool HostDatabaseConfigSpec::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       hashAlg,
                const ntcdns::HostDatabaseConfigSpec& object)
{
    typedef ntcdns::HostDatabaseConfigSpec Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
    case Class::SELECTION_ID_PATH:
        hashAppend(hashAlg, object.path());
        break;
    case Class::SELECTION_ID_CONFIGURATION:
        hashAppend(hashAlg, object.configuration());
        break;
    default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}

inline PortDatabaseConfigSpec::PortDatabaseConfigSpec(
    bslma::Allocator* basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline PortDatabaseConfigSpec::~PortDatabaseConfigSpec()
{
    reset();
}

inline bsl::string& PortDatabaseConfigSpec::path()
{
    BSLS_ASSERT(SELECTION_ID_PATH == d_selectionId);
    return d_path.object();
}

inline PortDatabaseConfig& PortDatabaseConfigSpec::configuration()
{
    BSLS_ASSERT(SELECTION_ID_CONFIGURATION == d_selectionId);
    return d_configuration.object();
}

inline int PortDatabaseConfigSpec::selectionId() const
{
    return d_selectionId;
}

inline const bsl::string& PortDatabaseConfigSpec::path() const
{
    BSLS_ASSERT(SELECTION_ID_PATH == d_selectionId);
    return d_path.object();
}

inline const PortDatabaseConfig& PortDatabaseConfigSpec::configuration() const
{
    BSLS_ASSERT(SELECTION_ID_CONFIGURATION == d_selectionId);
    return d_configuration.object();
}

inline bool PortDatabaseConfigSpec::isPathValue() const
{
    return SELECTION_ID_PATH == d_selectionId;
}

inline bool PortDatabaseConfigSpec::isConfigurationValue() const
{
    return SELECTION_ID_CONFIGURATION == d_selectionId;
}

inline bool PortDatabaseConfigSpec::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                       hashAlg,
                const ntcdns::PortDatabaseConfigSpec& object)
{
    typedef ntcdns::PortDatabaseConfigSpec Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
    case Class::SELECTION_ID_PATH:
        hashAppend(hashAlg, object.path());
        break;
    case Class::SELECTION_ID_CONFIGURATION:
        hashAppend(hashAlg, object.configuration());
        break;
    default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}

inline unsigned int& ServerConfig::timeout()
{
    return d_timeout;
}

inline NameServerConfig& ServerConfig::nameServer()
{
    return d_nameServer;
}

inline unsigned int ServerConfig::timeout() const
{
    return d_timeout;
}

inline const NameServerConfig& ServerConfig::nameServer() const
{
    return d_nameServer;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::ServerConfig& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.timeout());
    hashAppend(hashAlg, object.nameServer());
}

inline ClientConfigSpec::ClientConfigSpec(bslma::Allocator* basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline ClientConfigSpec::~ClientConfigSpec()
{
    reset();
}

inline bsl::string& ClientConfigSpec::path()
{
    BSLS_ASSERT(SELECTION_ID_PATH == d_selectionId);
    return d_path.object();
}

inline ClientConfig& ClientConfigSpec::configuration()
{
    BSLS_ASSERT(SELECTION_ID_CONFIGURATION == d_selectionId);
    return d_configuration.object();
}

inline int ClientConfigSpec::selectionId() const
{
    return d_selectionId;
}

inline const bsl::string& ClientConfigSpec::path() const
{
    BSLS_ASSERT(SELECTION_ID_PATH == d_selectionId);
    return d_path.object();
}

inline const ClientConfig& ClientConfigSpec::configuration() const
{
    BSLS_ASSERT(SELECTION_ID_CONFIGURATION == d_selectionId);
    return d_configuration.object();
}

inline bool ClientConfigSpec::isPathValue() const
{
    return SELECTION_ID_PATH == d_selectionId;
}

inline bool ClientConfigSpec::isConfigurationValue() const
{
    return SELECTION_ID_CONFIGURATION == d_selectionId;
}

inline bool ClientConfigSpec::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 hashAlg,
                const ntcdns::ClientConfigSpec& object)
{
    typedef ntcdns::ClientConfigSpec Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
    case Class::SELECTION_ID_PATH:
        hashAppend(hashAlg, object.path());
        break;
    case Class::SELECTION_ID_CONFIGURATION:
        hashAppend(hashAlg, object.configuration());
        break;
    default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}

inline bdlb::NullableValue<ClientConfigSpec>& ResolverConfig::client()
{
    return d_client;
}

inline bdlb::NullableValue<HostDatabaseConfigSpec>& ResolverConfig::
    hostDatabase()
{
    return d_hostDatabase;
}

inline bdlb::NullableValue<PortDatabaseConfigSpec>& ResolverConfig::
    portDatabase()
{
    return d_portDatabase;
}

inline const bdlb::NullableValue<ClientConfigSpec>& ResolverConfig::client()
    const
{
    return d_client;
}

inline const bdlb::NullableValue<HostDatabaseConfigSpec>& ResolverConfig::
    hostDatabase() const
{
    return d_hostDatabase;
}

inline const bdlb::NullableValue<PortDatabaseConfigSpec>& ResolverConfig::
    portDatabase() const
{
    return d_portDatabase;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const ntcdns::ResolverConfig& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.client());
    hashAppend(hashAlg, object.hostDatabase());
    hashAppend(hashAlg, object.portDatabase());
}

}  // close package namespace

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&                 stream,
                                        ntcdns::Classification::Value rhs)
{
    return ntcdns::Classification::print(stream, rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&            stream,
                                        ntcdns::Direction::Value rhs)
{
    return ntcdns::Direction::print(stream, rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&        stream,
                                        ntcdns::Error::Value rhs)
{
    return ntcdns::Error::print(stream, rhs);
}

inline bool ntcdns::operator==(const ntcdns::HostEntry& lhs,
                               const ntcdns::HostEntry& rhs)
{
    return lhs.address() == rhs.address() &&
           lhs.canonicalHostname() == rhs.canonicalHostname() &&
           lhs.aliases() == rhs.aliases() &&
           lhs.expiration() == rhs.expiration();
}

inline bool ntcdns::operator!=(const ntcdns::HostEntry& lhs,
                               const ntcdns::HostEntry& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&            stream,
                                        const ntcdns::HostEntry& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::Label& lhs,
                               const ntcdns::Label& rhs)
{
    typedef ntcdns::Label Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
        case Class::SELECTION_ID_TEXT:
            return lhs.text() == rhs.text();
        case Class::SELECTION_ID_OFFSET:
            return lhs.offset() == rhs.offset();
        default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

inline bool ntcdns::operator!=(const ntcdns::Label& lhs,
                               const ntcdns::Label& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&        stream,
                                        const ntcdns::Label& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::NameServerAddress& lhs,
                               const ntcdns::NameServerAddress& rhs)
{
    return lhs.host() == rhs.host() && lhs.port() == rhs.port();
}

inline bool ntcdns::operator!=(const ntcdns::NameServerAddress& lhs,
                               const ntcdns::NameServerAddress& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream& stream,
                                        const ntcdns::NameServerAddress& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&            stream,
                                        ntcdns::Operation::Value rhs)
{
    return ntcdns::Operation::print(stream, rhs);
}

inline bool ntcdns::operator==(const ntcdns::PortEntry& lhs,
                               const ntcdns::PortEntry& rhs)
{
    return lhs.service() == rhs.service() &&
           lhs.protocol() == rhs.protocol() && lhs.port() == rhs.port() &&
           lhs.expiration() == rhs.expiration();
}

inline bool ntcdns::operator!=(const ntcdns::PortEntry& lhs,
                               const ntcdns::PortEntry& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&            stream,
                                        const ntcdns::PortEntry& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataA& lhs,
                               const ntcdns::ResourceRecordDataA& rhs)
{
    return lhs.address() == rhs.address();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataA& lhs,
                               const ntcdns::ResourceRecordDataA& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream& stream,
                                        const ntcdns::ResourceRecordDataA& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataAAAA& lhs,
                               const ntcdns::ResourceRecordDataAAAA& rhs)
{
    return lhs.address0() == rhs.address0() &&
           lhs.address1() == rhs.address1() &&
           lhs.address2() == rhs.address2() &&
           lhs.address3() == rhs.address3();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataAAAA& lhs,
                               const ntcdns::ResourceRecordDataAAAA& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                         stream,
    const ntcdns::ResourceRecordDataAAAA& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataCname& lhs,
                               const ntcdns::ResourceRecordDataCname& rhs)
{
    return lhs.cname() == rhs.cname();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataCname& lhs,
                               const ntcdns::ResourceRecordDataCname& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                          stream,
    const ntcdns::ResourceRecordDataCname& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataHinfo& lhs,
                               const ntcdns::ResourceRecordDataHinfo& rhs)
{
    return lhs.cpu() == rhs.cpu() && lhs.os() == rhs.os();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataHinfo& lhs,
                               const ntcdns::ResourceRecordDataHinfo& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                          stream,
    const ntcdns::ResourceRecordDataHinfo& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataMx& lhs,
                               const ntcdns::ResourceRecordDataMx& rhs)
{
    return lhs.preference() == rhs.preference() &&
           lhs.exchange() == rhs.exchange();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataMx& lhs,
                               const ntcdns::ResourceRecordDataMx& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                       stream,
    const ntcdns::ResourceRecordDataMx& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataNs& lhs,
                               const ntcdns::ResourceRecordDataNs& rhs)
{
    return lhs.nsdname() == rhs.nsdname();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataNs& lhs,
                               const ntcdns::ResourceRecordDataNs& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                       stream,
    const ntcdns::ResourceRecordDataNs& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataPtr& lhs,
                               const ntcdns::ResourceRecordDataPtr& rhs)
{
    return lhs.ptrdname() == rhs.ptrdname();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataPtr& lhs,
                               const ntcdns::ResourceRecordDataPtr& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                        stream,
    const ntcdns::ResourceRecordDataPtr& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataRaw& lhs,
                               const ntcdns::ResourceRecordDataRaw& rhs)
{
    return lhs.data() == rhs.data();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataRaw& lhs,
                               const ntcdns::ResourceRecordDataRaw& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                        stream,
    const ntcdns::ResourceRecordDataRaw& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataSoa& lhs,
                               const ntcdns::ResourceRecordDataSoa& rhs)
{
    return lhs.mname() == rhs.mname() && lhs.rname() == rhs.rname() &&
           lhs.serial() == rhs.serial() && lhs.refresh() == rhs.refresh() &&
           lhs.retry() == rhs.retry() && lhs.expire() == rhs.expire() &&
           lhs.minimum() == rhs.minimum();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataSoa& lhs,
                               const ntcdns::ResourceRecordDataSoa& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                        stream,
    const ntcdns::ResourceRecordDataSoa& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataSvr& lhs,
                               const ntcdns::ResourceRecordDataSvr& rhs)
{
    return lhs.name() == rhs.name() && lhs.ttl() == rhs.ttl() &&
           lhs.classification() == rhs.classification() &&
           lhs.priority() == rhs.priority() && lhs.weight() == rhs.weight() &&
           lhs.port() == rhs.port() && lhs.target() == rhs.target();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataSvr& lhs,
                               const ntcdns::ResourceRecordDataSvr& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                        stream,
    const ntcdns::ResourceRecordDataSvr& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataTxt& lhs,
                               const ntcdns::ResourceRecordDataTxt& rhs)
{
    return lhs.text() == rhs.text();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataTxt& lhs,
                               const ntcdns::ResourceRecordDataTxt& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                        stream,
    const ntcdns::ResourceRecordDataTxt& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordDataWks& lhs,
                               const ntcdns::ResourceRecordDataWks& rhs)
{
    return lhs.address() == rhs.address() &&
           lhs.protocol() == rhs.protocol() && lhs.port() == rhs.port();
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordDataWks& lhs,
                               const ntcdns::ResourceRecordDataWks& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                        stream,
    const ntcdns::ResourceRecordDataWks& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::SortListItem& lhs,
                               const ntcdns::SortListItem& rhs)
{
    return lhs.address() == rhs.address() && lhs.netmask() == rhs.netmask();
}

inline bool ntcdns::operator!=(const ntcdns::SortListItem& lhs,
                               const ntcdns::SortListItem& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&               stream,
                                        const ntcdns::SortListItem& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&       stream,
                                        ntcdns::Type::Value rhs)
{
    return ntcdns::Type::print(stream, rhs);
}

inline bool ntcdns::operator==(const ntcdns::HostDatabaseConfig& lhs,
                               const ntcdns::HostDatabaseConfig& rhs)
{
    return lhs.entry() == rhs.entry();
}

inline bool ntcdns::operator!=(const ntcdns::HostDatabaseConfig& lhs,
                               const ntcdns::HostDatabaseConfig& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream& stream,
                                        const ntcdns::HostDatabaseConfig& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::NameServerConfig& lhs,
                               const ntcdns::NameServerConfig& rhs)
{
    return lhs.address() == rhs.address();
}

inline bool ntcdns::operator!=(const ntcdns::NameServerConfig& lhs,
                               const ntcdns::NameServerConfig& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&                   stream,
                                        const ntcdns::NameServerConfig& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::PortDatabaseConfig& lhs,
                               const ntcdns::PortDatabaseConfig& rhs)
{
    return lhs.entry() == rhs.entry();
}

inline bool ntcdns::operator!=(const ntcdns::PortDatabaseConfig& lhs,
                               const ntcdns::PortDatabaseConfig& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream& stream,
                                        const ntcdns::PortDatabaseConfig& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResourceRecordData& lhs,
                               const ntcdns::ResourceRecordData& rhs)
{
    typedef ntcdns::ResourceRecordData Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
        case Class::SELECTION_ID_CANONICAL_NAME:
            return lhs.canonicalName() == rhs.canonicalName();
        case Class::SELECTION_ID_HOST_INFO:
            return lhs.hostInfo() == rhs.hostInfo();
        case Class::SELECTION_ID_MAIL_EXCHANGE:
            return lhs.mailExchange() == rhs.mailExchange();
        case Class::SELECTION_ID_NAME_SERVER:
            return lhs.nameServer() == rhs.nameServer();
        case Class::SELECTION_ID_POINTER:
            return lhs.pointer() == rhs.pointer();
        case Class::SELECTION_ID_ZONE_AUTHORITY:
            return lhs.zoneAuthority() == rhs.zoneAuthority();
        case Class::SELECTION_ID_WELL_KNOWN_SERVICE:
            return lhs.wellKnownService() == rhs.wellKnownService();
        case Class::SELECTION_ID_TEXT:
            return lhs.text() == rhs.text();
        case Class::SELECTION_ID_IPV4:
            return lhs.ipv4() == rhs.ipv4();
        case Class::SELECTION_ID_IPV6:
            return lhs.ipv6() == rhs.ipv6();
        case Class::SELECTION_ID_SERVER:
            return lhs.server() == rhs.server();
        case Class::SELECTION_ID_RAW:
            return lhs.raw() == rhs.raw();
        default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

inline bool ntcdns::operator!=(const ntcdns::ResourceRecordData& lhs,
                               const ntcdns::ResourceRecordData& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream& stream,
                                        const ntcdns::ResourceRecordData& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::SortList& lhs,
                               const ntcdns::SortList& rhs)
{
    return lhs.item() == rhs.item();
}

inline bool ntcdns::operator!=(const ntcdns::SortList& lhs,
                               const ntcdns::SortList& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&           stream,
                                        const ntcdns::SortList& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ClientConfig& lhs,
                               const ntcdns::ClientConfig& rhs)
{
    return lhs.nameServer() == rhs.nameServer() &&
           lhs.domain() == rhs.domain() && lhs.search() == rhs.search() &&
           lhs.sortList() == rhs.sortList() &&
           lhs.attempts() == rhs.attempts() &&
           lhs.timeout() == rhs.timeout() && lhs.rotate() == rhs.rotate() &&
           lhs.ndots() == rhs.ndots() && lhs.debug() == rhs.debug();
}

inline bool ntcdns::operator!=(const ntcdns::ClientConfig& lhs,
                               const ntcdns::ClientConfig& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&               stream,
                                        const ntcdns::ClientConfig& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::HostDatabaseConfigSpec& lhs,
                               const ntcdns::HostDatabaseConfigSpec& rhs)
{
    typedef ntcdns::HostDatabaseConfigSpec Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
        case Class::SELECTION_ID_PATH:
            return lhs.path() == rhs.path();
        case Class::SELECTION_ID_CONFIGURATION:
            return lhs.configuration() == rhs.configuration();
        default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

inline bool ntcdns::operator!=(const ntcdns::HostDatabaseConfigSpec& lhs,
                               const ntcdns::HostDatabaseConfigSpec& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                         stream,
    const ntcdns::HostDatabaseConfigSpec& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::PortDatabaseConfigSpec& lhs,
                               const ntcdns::PortDatabaseConfigSpec& rhs)
{
    typedef ntcdns::PortDatabaseConfigSpec Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
        case Class::SELECTION_ID_PATH:
            return lhs.path() == rhs.path();
        case Class::SELECTION_ID_CONFIGURATION:
            return lhs.configuration() == rhs.configuration();
        default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

inline bool ntcdns::operator!=(const ntcdns::PortDatabaseConfigSpec& lhs,
                               const ntcdns::PortDatabaseConfigSpec& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(
    bsl::ostream&                         stream,
    const ntcdns::PortDatabaseConfigSpec& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ServerConfig& lhs,
                               const ntcdns::ServerConfig& rhs)
{
    return lhs.timeout() == rhs.timeout() &&
           lhs.nameServer() == rhs.nameServer();
}

inline bool ntcdns::operator!=(const ntcdns::ServerConfig& lhs,
                               const ntcdns::ServerConfig& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&               stream,
                                        const ntcdns::ServerConfig& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ClientConfigSpec& lhs,
                               const ntcdns::ClientConfigSpec& rhs)
{
    typedef ntcdns::ClientConfigSpec Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
        case Class::SELECTION_ID_PATH:
            return lhs.path() == rhs.path();
        case Class::SELECTION_ID_CONFIGURATION:
            return lhs.configuration() == rhs.configuration();
        default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

inline bool ntcdns::operator!=(const ntcdns::ClientConfigSpec& lhs,
                               const ntcdns::ClientConfigSpec& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&                   stream,
                                        const ntcdns::ClientConfigSpec& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline bool ntcdns::operator==(const ntcdns::ResolverConfig& lhs,
                               const ntcdns::ResolverConfig& rhs)
{
    return lhs.client() == rhs.client() &&
           lhs.hostDatabase() == rhs.hostDatabase() &&
           lhs.portDatabase() == rhs.portDatabase();
}

inline bool ntcdns::operator!=(const ntcdns::ResolverConfig& lhs,
                               const ntcdns::ResolverConfig& rhs)
{
    return !(lhs == rhs);
}

inline bsl::ostream& ntcdns::operator<<(bsl::ostream&                 stream,
                                        const ntcdns::ResolverConfig& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif
