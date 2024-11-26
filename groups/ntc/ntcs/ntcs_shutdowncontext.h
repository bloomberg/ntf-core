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

#ifndef INCLUDED_NTCS_SHUTDOWNCONTEXT
#define INCLUDED_NTCS_SHUTDOWNCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_shutdowncontext.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_shutdownorigin.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Describe the result of a shutdown operation.
///
/// @details
/// This component provides a value-semantic type that describes the result of
/// a shutdown operation. This result informs the performer of the shutdown
/// operation which subsequent operations should be performed.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b shutdownInitiated:
/// The shutdown sequence has been initiated.
///
/// @li @b shutdownSend:
/// The socket should be subsequently shut down for sending.
///
/// @li @b shutdownReceive:
/// The socket should be subsequently shut down for receiving.
///
/// @li @b shutdownCompleted:
/// The shutdown sequence has been completed.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class ShutdownContext
{
    ntsa::ShutdownOrigin::Value d_shutdownOrigin;
    bool                        d_shutdownInitiated;
    bool                        d_shutdownSend;
    bool                        d_shutdownReceive;
    bool                        d_shutdownCompleted;

  public:
    /// Create new send options having the default value.
    ShutdownContext();

    /// Create new send options having the same value as the specified
    /// 'original' object.
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
    void setShutdownOrigin(ntsa::ShutdownOrigin::Value value);

    /// Set the flag indicating shutdown has been initiated to the specified
    /// 'value'.
    void setShutdownInitiated(bool value);

    /// Set the flag indicating the socket should be shut down in the send
    /// direction to the specified 'value'.
    void setShutdownSend(bool value);

    /// Set the flag indicating the socket should be shut down in the
    /// receive direction to the specified 'value'.
    void setShutdownReceive(bool value);

    /// Set the flag indicating shutdown has been completed to the specified
    /// 'value'.
    void setShutdownCompleted(bool value);

    /// Return the origin of the shutdown sequence.
    ntsa::ShutdownOrigin::Value shutdownOrigin() const;

    /// Return the flag indicating shutdown has been initiated.
    bool shutdownInitiated() const;

    /// Return the flag indicating the socket should be shut down in the
    /// send direction.
    bool shutdownSend() const;

    /// Return the flag indicating the socket should be shut down in the
    /// receive direction.
    bool shutdownReceive() const;

    /// Return the flag indicating shutdown has been completed.
    bool shutdownCompleted() const;

    /// Return the event context.
    ntca::ShutdownContext base() const;

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

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ShutdownContext);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ShutdownContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntcs::ShutdownContext
bsl::ostream& operator<<(bsl::ostream& stream, const ShutdownContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcs::ShutdownContext
bool operator==(const ShutdownContext& lhs, const ShutdownContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcs::ShutdownContext
bool operator!=(const ShutdownContext& lhs, const ShutdownContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntcs::ShutdownContext
bool operator<(const ShutdownContext& lhs, const ShutdownContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcs::ShutdownContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ShutdownContext& value);

NTCCFG_INLINE
ShutdownContext::ShutdownContext()
: d_shutdownOrigin(ntsa::ShutdownOrigin::e_SOURCE)
, d_shutdownInitiated(false)
, d_shutdownSend(false)
, d_shutdownReceive(false)
, d_shutdownCompleted(false)
{
}

NTCCFG_INLINE
ShutdownContext::ShutdownContext(const ShutdownContext& original)
: d_shutdownOrigin(original.d_shutdownOrigin)
, d_shutdownInitiated(original.d_shutdownInitiated)
, d_shutdownSend(original.d_shutdownSend)
, d_shutdownReceive(original.d_shutdownReceive)
, d_shutdownCompleted(original.d_shutdownCompleted)
{
}

NTCCFG_INLINE
ShutdownContext::~ShutdownContext()
{
}

NTCCFG_INLINE
ShutdownContext& ShutdownContext::operator=(const ShutdownContext& other)
{
    d_shutdownOrigin    = other.d_shutdownOrigin;
    d_shutdownInitiated = other.d_shutdownInitiated;
    d_shutdownSend      = other.d_shutdownSend;
    d_shutdownReceive   = other.d_shutdownReceive;
    d_shutdownCompleted = other.d_shutdownCompleted;
    return *this;
}

NTCCFG_INLINE
void ShutdownContext::reset()
{
    d_shutdownOrigin    = ntsa::ShutdownOrigin::e_SOURCE;
    d_shutdownInitiated = false;
    d_shutdownSend      = false;
    d_shutdownReceive   = false;
    d_shutdownCompleted = false;
}

NTCCFG_INLINE
void ShutdownContext::setShutdownOrigin(ntsa::ShutdownOrigin::Value value)
{
    d_shutdownOrigin = value;
}

NTCCFG_INLINE
void ShutdownContext::setShutdownInitiated(bool value)
{
    d_shutdownInitiated = value;
}

NTCCFG_INLINE
void ShutdownContext::setShutdownSend(bool value)
{
    d_shutdownSend = value;
}

NTCCFG_INLINE
void ShutdownContext::setShutdownReceive(bool value)
{
    d_shutdownReceive = value;
}

NTCCFG_INLINE
void ShutdownContext::setShutdownCompleted(bool value)
{
    d_shutdownCompleted = value;
}

NTCCFG_INLINE
ntsa::ShutdownOrigin::Value ShutdownContext::shutdownOrigin() const
{
    return d_shutdownOrigin;
}

NTCCFG_INLINE
bool ShutdownContext::shutdownInitiated() const
{
    return d_shutdownInitiated;
}

NTCCFG_INLINE
bool ShutdownContext::shutdownSend() const
{
    return d_shutdownSend;
}

NTCCFG_INLINE
bool ShutdownContext::shutdownReceive() const
{
    return d_shutdownReceive;
}

NTCCFG_INLINE
bool ShutdownContext::shutdownCompleted() const
{
    return d_shutdownCompleted;
}

NTCCFG_INLINE
ntca::ShutdownContext ShutdownContext::base() const
{
    ntca::ShutdownContext context;

    context.setOrigin(d_shutdownOrigin);
    context.setSend(d_shutdownSend);
    context.setReceive(d_shutdownReceive);

    return context;
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

    hashAppend(algorithm, value.shutdownInitiated());
    hashAppend(algorithm, value.shutdownSend());
    hashAppend(algorithm, value.shutdownReceive());
    hashAppend(algorithm, value.shutdownCompleted());
}

}  // close package namespace
}  // close enterprise namespace
#endif
