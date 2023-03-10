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

#ifndef INCLUDED_NTCA_SHUTDOWNCONTEXT
#define INCLUDED_NTCA_SHUTDOWNCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_shutdownorigin.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the state of shutdown at the time of an event.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b origin:
/// The origin of the shutdown sequence.
///
/// @li @b send:
/// The socket has been shut down for sending at the time of the event.
///
/// @li @b receive:
/// The socket has been shut down for receiving at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_shutdown
class ShutdownContext
{
    ntsa::ShutdownOrigin::Value d_origin;
    bool                        d_send;
    bool                        d_receive;

  public:
    /// Create a new shutdown context having the default value.
    ShutdownContext();

    /// Create a new shutdown context having the same value as the
    /// specified 'original' object.
    ShutdownContext(const ShutdownContext& original);

    /// Destroy this object.
    ~ShutdownContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ShutdownContext& operator=(const ShutdownContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the origin of the shutdown sequence to the specified 'value'.
    void setOrigin(ntsa::ShutdownOrigin::Value value);

    /// Set the flag that indicates the socket has been shut down for
    /// sending at the time of the event to the specified 'value'.
    void setSend(bool value);

    /// Set the flag that indicates the socket has been shut down for
    /// receiving at the time of the event to the specified 'value'.
    void setReceive(bool value);

    /// Return the origin of the shutdown sequence.
    ntsa::ShutdownOrigin::Value origin() const;

    /// Return theflag that indicates the socket has been shut down for
    /// sending at the time of the event.
    bool send() const;

    /// Return the flag that indicates the socket has been shut down for
    /// receiving at the time of the event.
    bool receive() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ShutdownContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ShutdownContext& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ShutdownContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ShutdownContext
bsl::ostream& operator<<(bsl::ostream& stream, const ShutdownContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ShutdownContext
bool operator==(const ShutdownContext& lhs, const ShutdownContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ShutdownContext
bool operator!=(const ShutdownContext& lhs, const ShutdownContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ShutdownContext
bool operator<(const ShutdownContext& lhs, const ShutdownContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ShutdownContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ShutdownContext& value);

NTCCFG_INLINE
ShutdownContext::ShutdownContext()
: d_origin(ntsa::ShutdownOrigin::e_SOURCE)
, d_send(false)
, d_receive(false)
{
}

NTCCFG_INLINE
ShutdownContext::ShutdownContext(const ShutdownContext& original)
: d_origin(original.d_origin)
, d_send(original.d_send)
, d_receive(original.d_receive)
{
}

NTCCFG_INLINE
ShutdownContext::~ShutdownContext()
{
}

NTCCFG_INLINE
ShutdownContext& ShutdownContext::operator=(const ShutdownContext& other)
{
    d_origin  = other.d_origin;
    d_send    = other.d_send;
    d_receive = other.d_receive;
    return *this;
}

NTCCFG_INLINE
void ShutdownContext::reset()
{
    d_origin  = ntsa::ShutdownOrigin::e_SOURCE;
    d_send    = false;
    d_receive = false;
}

NTCCFG_INLINE
void ShutdownContext::setOrigin(ntsa::ShutdownOrigin::Value value)
{
    d_origin = value;
}

NTCCFG_INLINE
void ShutdownContext::setSend(bool value)
{
    d_send = value;
}

NTCCFG_INLINE
void ShutdownContext::setReceive(bool value)
{
    d_receive = value;
}

NTCCFG_INLINE
ntsa::ShutdownOrigin::Value ShutdownContext::origin() const
{
    return d_origin;
}

NTCCFG_INLINE
bool ShutdownContext::send() const
{
    return d_send;
}

NTCCFG_INLINE
bool ShutdownContext::receive() const
{
    return d_receive;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ShutdownContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ShutdownContext& lhs, const ShutdownContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ShutdownContext& lhs, const ShutdownContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ShutdownContext& lhs, const ShutdownContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ShutdownContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.origin());
    hashAppend(algorithm, value.send());
    hashAppend(algorithm, value.receive());
}

}  // close package namespace
}  // close enterprise namespace
#endif
