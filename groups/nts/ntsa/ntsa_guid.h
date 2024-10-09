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

#ifndef INCLUDED_NTSA_GUID
#define INCLUDED_NTSA_GUID

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_spinlock.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>
#include <bsls_unspecifiedbool.h>
#include <bsl_c_string.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a GUID (Globally Unique IDentifier) that can be used to uniquely
/// identify a resource across a network.
///
/// @details
/// Provide a value-semantic type that represents an identifier
/// that is globally unique. A GUID (Globally Unique IDentifier) is a common
/// requirement in distributed computing to allow multiple processes on
/// multiple machines to generate identifiers which are guaranteed to be unique
/// across all machines and can therefore be shared across the network. This
/// GUID encapsulates 128 bits of information and is based on the IP address,
/// process ID and timestamp to microsecond resolution. The use of the IP
/// address does mean that this GUID representation is not suitable for use
/// across NAT environments (this could be resolved by using MAC address
/// instead of IP). The use of such a timestamp throttles the generation of new
/// GUIDs to one new GUID per microsecond.
///
/// @par Thread Safety
/// The 'ntsa::Guid::generate()' function is thread safe: it is safe to
/// generate new 'ntsa::Guid' objects concurrently in different threads. The
/// rest of the 'ntsa::Guid' class is const thread safe: it is safe to
/// concurrently compare, format, or otherwise access the same 'ntsa::Guid' in
/// different threads.
///
/// @par Usage Example
/// The following example illustrates the intended usage of this component.
/// Let's assume we have a class, 'Object', that wishes to be able to
/// globally identify its instantiation.
///
///     class Object {
///         ntsa::Guid d_guid;
///       public:
///         Object();
///         const ntsa::Guid& guid() const;
///     };
///
/// Then, we can define this class's implementation by using the automatic
/// identifer generation capabilities provided by this component.
///
///     Object::Object()
///     : d_id(ntsa::Guid::generate())
///     {
///     }
///
///     const ntsa::Guid& Object::guid() const
///     {
///         return d_guid;
///     }
///
/// Now, let's instantiate two 'Object's, compare their locally-unique
/// identifiers, and print them to to standard output.
///
///     Object object1;
///     Object object2;
///
///     BSLS_ASSERT(object1.guid() != object2.guid());
///
///     bsl::cout << object1.guid() << bsl::endl;
///     bsl::cout << object2.guid() << bsl::endl;
///
/// Notice that standard output has the following data written to it on the test
/// machine (as every GUID is globably unique, these GUIDs are just an example.)
///
///     10.0.2.15:8593:27SEP2018_15:02:58.613000396
///     10.0.2.15:8593:27SEP2018_15:02:58.613000821
///
/// @ingroup module_ntsa_identity
class Guid
{
  public:
    /// The number of bytes a Guid takes up in binary representation.
    static int const SIZE_BINARY = 16;

    /// The number of bytes a Guid takes up in text representation.
    static int const SIZE_TEXT = 32;

  private:
    union {
        /// [0] 1/16 second resolution
        /// [1] process ID bits 7-0
        /// [2] 1/4 millisecond resolution
        /// [3] process ID bits 15-8
        /// [4] fourth part IP address
        /// [5] 16 second resolution
        /// [6] third part IP address
        /// [7] 1 hour resolution
        /// [8] process ID bits 23-16
        /// [9] process ID bits 31-24
        /// [10] 10 day resolution
        /// [11] 7 year resolution
        /// [12] second part IP address
        /// [13] first part IP address
        /// [14] 1800 year resolution
        /// [15] 1/2 millennium resolution
        char d_bytes[SIZE_BINARY];

        bsl::uint64_t d_uint64s[2];
    };

    static bsl::uint64_t  s_lastTimestamp;
    static int            s_localProcessId;
    static int            s_localIpAddress;
    static bsls::SpinLock s_lock;

    /// Set the IP address component of this GUID to the specified
    /// 'ipAddress'.
    void setIpAddress(int ipAddress);

    /// Set the process ID component of this GUID to the specified
    /// 'processId'.
    void setProcessId(int processId);

    /// Set the timestamp component of this GUID to the specified
    /// 'timestamp', in microseconds since the Unix epoch.
    void setTimestamp(bsl::uint64_t timestamp);

  public:
    /// This typedef defines a tag type used to disambiguate constructors
    /// by indicating the value is a binary representation of a GUID.
    struct BinaryRepresentation {
    };

    /// This typedef defines a tag type used to disambiguate constructors
    /// by indicating the value is a textual representation of a GUID.
    struct TextRepresentation {
    };

    /// This typedef a boolean type used to convey the truthiness of the
    /// value of a GUID.
    typedef bsls::UnspecifiedBool<Guid>::BoolType BoolType;

    /// Return a new instance of a GUID. This is guaranteed not to be the
    /// same as any other Guid returned in any other process on any other
    /// machine as long as there are no duplicate IP addresses (ie no NAT)
    /// and machines do not adjust their system clocks negatively (skewing
    /// slowly under control of NNTP is fine).
    static Guid generate();

    /// Return 'true' if the specified 'text' contains a valid text
    /// representation of a GUID, and false otherwise.
    static bool isValidTextRepresentation(const char*);

    /// Create a new GUID object having an unset value. A GUID created using
    /// the default constructor will return false to valid() and evaluate to
    /// false as a boolean. The only valid operations to perform on a Guid
    /// created using the default constructor are assignment to it and test
    /// for validity.
    Guid();

    /// Create a new GUID object from the specified portable binary
    /// representation at 'binary'. The behavior is undefined unless the
    /// 'binary' representation was created using the 'writeBinary()' method
    /// on a valid GUID.
    Guid(BinaryRepresentation, const void* binary);

    /// Create a new GUID object from the specified text representation at
    /// 'text'.  The behavior is undefined unless the 'text' representation
    /// must was created using the 'writeText()' method on a valid GUID.
    Guid(TextRepresentation, const char* text);

    /// Create a new GUID object having the same value as the specified
    /// 'original' object.
    Guid(const Guid& original);

    /// Destroy this Guid.
    ~Guid();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Guid& operator=(const Guid& other);

    /// Returns true if this GUID is a valid: either generated by
    /// 'generate()', constructed from a valid binary or text
    /// representation, or copied or assigned from another valid GUID
    /// object. Otherwise returns false, notably for default-constructed
    /// GUID objects.
    bool valid() const;

    /// Write this GUID in portable binary format to the specified
    /// 'destination'. The behavior is undefined if there is not
    /// Guid::SIZE_BINARY bytes available at the specified 'destination'.
    /// The data is written in the same order it is compared so direct
    /// comparisons of the binary exported form of GUIDs will be as unbiased
    /// as comparisons on GUID objects - see compare().
    void writeBinary(void* destination) const;

    /// Write this GUID in portable text format to the specified
    /// 'destination'. The behavior is undefined if there is not
    /// Guid::SIZE_TEXT bytes available at the specified 'destination'. The
    /// text written IS NOT null-terminated. The data is written in the same
    /// order it is compared so direct comparisons of the text exported form
    /// of GUIDs will be as unbiased as comparisons on GUID objects - see
    /// compare().
    void writeText(char* destination) const;

    /// Return a negative value if the value of this GUID object is "less
    /// than" the value of the specified 'other' GUID object, a positive
    /// value if the value of this GUID object is "greater than" the value
    /// of the specified 'other' GUID object, and zero if this GUID object
    /// has the same value as the 'other GUID object.
    ///
    /// The comparison logic is arbitrary but guaranteed to be stable and
    /// consistent over time and across platforms.
    ///
    /// The comparison strives to be "fair" and avoid any built in biases.
    /// For example: a "lower" IP address always beating a "higher" IP
    /// address; an operating system which uses larger values for process
    /// IDs - AIX - tending to beat an operating system which uses smaller
    /// values for process IDs - Solaris; an operating system that provides
    /// sub-millisecond timer resolution beating one that does not.
    ///
    /// To achieve this the comparison slices the contents of the GUID
    /// into single bytes and compares them in this order
    ///
    /// - Timestamp portion between 1/16 second and 1/4 millisecond resolution
    /// - Process ID bits 7-0
    /// - Timestamp portion below 1/4 millisecond resolution
    /// - Process ID bits 15-8
    /// - Fourth part IP address
    /// - Timestamp portions between 16 second resolution and 1/16 second
    ///   resolution
    /// - Third part IP address
    /// - Timestamp portion between 1 hour resolution and 16 second resolution
    /// - Process ID bits 23-16
    /// - Process ID bits 31-24
    /// - Timestamp portion between 10 day resolution and 1 hour resolution
    /// - Timestamp portion between 7 year and 10 day resolution
    /// - Second part IP address
    /// - First part IP address
    /// - Timestamp portion between 1800 year and 7 year resolution
    /// - Timestamp portion between 1/2 millennium and 1800 year resolution
    int compare(const Guid& other) const;

    /// Return the result of calling 'valid(') on this object.
    operator BoolType() const;

    /// Write the value of this GUID object to the specified 'stream' in
    /// human readable form. This presents the different parts of the Guid
    /// in decoded formats - IP, process and timestamp. No code should
    /// depend on the format of this output. The only guarantee is that this
    /// output will be "human readable".
    template <class STREAM>
    void output(STREAM& os) const;

    /// Return the IP address component of this GUID object value.
    int ipAddress() const;

    /// Return the process ID component of this GUID object value.
    int processId() const;

    /// Return the timestamp component of this GUID object value.
    bsl::uint64_t timestamp() const;

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Guid);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Guid);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Guid);

    /// This type's equality-comparison operator is equivalent to comparing
    /// each byte of one comparand's footprint to each corresponding byte of
    /// the other comparand's footprint. Note that this trait implies that an
    /// object of this type has no padding bytes between data members.
    NTSCFG_TYPE_TRAIT_BITWISE_COMPARABLE(Guid);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Guid
bool operator==(const Guid& lhs, const Guid& rhs);

/// Return true if the specified 'lhs' does not have same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Guid
bool operator!=(const Guid& lhs, const Guid& rhs);

/// Return true if the specified 'lhs' is "less than" the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Guid
bool operator<(const Guid& lhs, const Guid& rhs);

/// Return true if the specified 'lhs' is "greater than" the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Guid
bool operator>(const Guid& lhs, const Guid& rhs);

/// Write the specified 'value' to the specified 'stream' if
/// a human-readable format. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::Guid
template <class STREAM>
STREAM& operator<<(STREAM& stream, const Guid& value);

NTSCFG_INLINE
Guid::Guid()
{
    d_uint64s[0] = 0;
    d_uint64s[1] = 0;
}

NTSCFG_INLINE
Guid::Guid(const Guid& original)
{
    d_uint64s[0] = original.d_uint64s[0];
    d_uint64s[1] = original.d_uint64s[1];
}

NTSCFG_INLINE
Guid::~Guid()
{
}

NTSCFG_INLINE
Guid& Guid::operator=(const Guid& other)
{
    if (this != &other) {
        d_uint64s[0] = other.d_uint64s[0];
        d_uint64s[1] = other.d_uint64s[1];
    }
    return *this;
}

NTSCFG_INLINE
bool Guid::valid() const
{
    return (d_uint64s[0] != 0 || d_uint64s[1] != 0);
}

NTSCFG_INLINE
Guid::operator Guid::BoolType() const
{
    return bsls::UnspecifiedBool<Guid>::makeValue(valid());
}

template <class STREAM>
NTSCFG_INLINE void Guid::output(STREAM& stream) const
{
    if (!valid()) {
        stream << "Invalid Guid";
        return;
    }

    bsl::ios_base::fmtflags flags =
        stream.flags(bsl::ios_base::dec | bsl::ios_base::right);

    const int                  ipAddressComposite = ipAddress();
    const unsigned char* const ipAddressParts =
        reinterpret_cast<const unsigned char*>(&ipAddressComposite);
    bsl::uint64_t      timestampUsec = timestamp();
    bsls::TimeInterval timestamp;
    timestamp.addMicroseconds(timestampUsec);
    bdlt::Datetime asDatetime(bdlt::EpochUtil::epoch());
    asDatetime.addMilliseconds(timestamp.totalMilliseconds());

    stream << static_cast<int>(ipAddressParts[0]) << "."
           << static_cast<int>(ipAddressParts[1]) << "."
           << static_cast<int>(ipAddressParts[2]) << "."
           << static_cast<int>(ipAddressParts[3]) << ":" << processId() << ":"
           << asDatetime;

    bsl::streamsize            width = stream.width(bsl::streamsize(3));
    typename STREAM::char_type fill  = stream.fill('0');

    stream << (timestampUsec % 1000UL);

    stream.fill(fill);
    stream.width(width);
    stream.setf(flags);
}

NTSCFG_INLINE
bool operator==(const Guid& lhs, const Guid& rhs)
{
    return (lhs.compare(rhs) == 0);
}

NTSCFG_INLINE
bool operator!=(const Guid& lhs, const Guid& rhs)
{
    return (lhs.compare(rhs) != 0);
}

NTSCFG_INLINE
bool operator<(const Guid& lhs, const Guid& rhs)
{
    return (lhs.compare(rhs) < 0);
}

NTSCFG_INLINE
bool operator>(const Guid& lhs, const Guid& rhs)
{
    return (lhs.compare(rhs) > 0);
}

template <class STREAM>
NTSCFG_INLINE STREAM& operator<<(STREAM& stream, const Guid& value)
{
    value.output(stream);
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

#endif
