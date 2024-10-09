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

#ifndef INCLUDED_NTSA_ID
#define INCLUDED_NTSA_ID

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsls_atomic.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an identity for run-time objects unique to the local process.
///
/// @details
/// This class represents an identifer of an object unique to the local
/// process. The value of the identifier is automatically generated as a
/// sequence of monotonically increasing 32-bit integers starting at 1.
///
/// @par Thread Safety
/// The 'ntsa::Id::generate()' function is thread safe: it is safe to generate
/// new 'ntsa::Id' objects concurrently in different threads. The rest of the
/// 'ntsa::Id' class is const thread safe: it is safe to concurrently compare,
/// format, or otherwise access the same 'ntsa::Id' in different threads.
///
/// @par Usage Example
/// The following example illustrates the intended usage of this component.
/// Let's assume we have a class, 'Object', that wishes to be able to uniquely
/// identify its instantiation in the current process.
///
///     class Object {
///         ntsa::Id d_id;
///       public:
///         Object();
///             // Create a new object and automatally assign it an identifier
///             // locally unique to the current process.
///
///         const ntsa::Id& id() const;
///             // Return the identifier of this object that is locally-unique
///             to the current process.
///     };
///
/// Then, we can define this class's implementation by using the automatic
/// identifer generation capabilities provided by this component.
///
///     Object::Object()
///     : d_id(ntsa::Id::generate())
///     {
///     }
///
///     const ntsa::Id& Object::id() const
///     {
///         return d_id;
///     }
///
/// Now, let's instantiate two 'Object's, compare their locally-unique
/// identifiers, and print them to to standard output. For the purposes of this
/// example, let's assume these instantiations are the first two objects
/// requesting locally-unique identification in the process.
///
///     Object object1;
///     Object object2;
///
///     ASSERT(object.id() != object2.id());
///
///     bsl::cout << object1.id() << bsl::endl;
///     bsl::cout << object2.id() << bsl::endl;
///
/// Notice that standard output has the following data written to it:
///
///     id:000001
///     id:000002
///
/// @ingroup module_ntsa_identity
class Id
{
    int d_value;  // The generated identifer value

    static bsls::AtomicInt s_next;  // The next available identifier value

    /// Create a new locally-unique identifier having the specified 'value'.
    explicit Id(int value);

  public:
    /// Create a new locally-unique identifier having the same value as the
    /// specified 'original' object.
    Id(const Id& original);

    /// Destroy this object.
    ~Id();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Id& operator=(const Id& other);

    /// Return the value of this object converted to an 'int'.
    operator int() const;

    /// Return the value of this object converted to an 'int'.
    int value() const;

    /// Return a new identifier locally unique to the current process.
    static Id generate();

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Id);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Id);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Id);
};

/// Write the specified 'value' to the specified 'stream' in a
/// human-readable format. Return a reference to the modifiable 'stream'.
///
/// @related ntsa::Id
bsl::ostream& operator<<(bsl::ostream& stream, const Id& value);

NTSCFG_INLINE
Id::Id(int value)
: d_value(value)
{
}

NTSCFG_INLINE
Id Id::generate()
{
    return Id(++s_next);
}

NTSCFG_INLINE
Id::Id(const Id& original)
: d_value(original.d_value)
{
}

NTSCFG_INLINE
Id::~Id()
{
}

NTSCFG_INLINE
Id& Id::operator=(const Id& other)
{
    d_value = other.d_value;
    return *this;
}

NTSCFG_INLINE
Id::operator int() const
{
    return d_value;
}

NTSCFG_INLINE
int Id::value() const
{
    return d_value;
}

// FREE OPERATORS
NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Id& value)
{
    stream << "id:";

    bsl::ios_base::fmtflags flags =
        stream.flags(bsl::ios_base::dec | bsl::ios_base::right);

    bsl::streamsize width = stream.width(bsl::streamsize(6));

    bsl::ostream::char_type fill = stream.fill('0');

    stream << value.value();
    stream.fill(fill);
    stream.width(width);
    stream.setf(flags);

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

#endif
