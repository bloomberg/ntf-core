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

#ifndef INCLUDED_NTSA_SOCKETINFOFILTER
#define INCLUDED_NTSA_SOCKETINFOFILTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a filter when reporting socket information.
///
/// @details
/// Provide a value-semantic type that describes the filter when
/// reporting information about operating system sockets.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b  transport:
/// The transport of the sockets to report. The default is null, indicating all
/// sockets from all transports should be reported.
///
/// @li @b  all:
/// The flag indicating that sockets created by all users should be reported.
/// The default value is null, indicating that only those sockets created by
/// processes having the same effective user ID as the effective user ID of the
/// process should be reported.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class SocketInfoFilter
{
    bdlb::NullableValue<ntsa::Transport::Value> d_transport;
    bdlb::NullableValue<bool>                   d_all;

  public:
    /// Create new receive options having the default value.
    SocketInfoFilter();

    /// Create new receive options having the same value as the specified
    /// 'original' object.
    SocketInfoFilter(const SocketInfoFilter& original);

    /// Destroy this object.
    ~SocketInfoFilter();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    SocketInfoFilter& operator=(const SocketInfoFilter& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the transport of the sockets to report to the specified 'value'.
    void setTransport(ntsa::Transport::Value value);

    /// Set the flag indicating that sockets created by all users should
    /// be reported to the specified 'value'.
    void setAll(bool value);

    /// Return the transport of the sockets to report.
    const bdlb::NullableValue<ntsa::Transport::Value>& transport() const;

    /// Return the flag indicating that sockets created by all users
    /// should be reported.
    const bdlb::NullableValue<bool>& all() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const SocketInfoFilter& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const SocketInfoFilter& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(SocketInfoFilter);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(SocketInfoFilter);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::SocketInfoFilter
bsl::ostream& operator<<(bsl::ostream& stream, const SocketInfoFilter& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::SocketInfoFilter
bool operator==(const SocketInfoFilter& lhs, const SocketInfoFilter& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::SocketInfoFilter
bool operator!=(const SocketInfoFilter& lhs, const SocketInfoFilter& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::SocketInfoFilter
bool operator<(const SocketInfoFilter& lhs, const SocketInfoFilter& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::SocketInfoFilter
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SocketInfoFilter& value);

NTSCFG_INLINE
SocketInfoFilter::SocketInfoFilter()
: d_transport()
, d_all()
{
}

NTSCFG_INLINE
SocketInfoFilter::SocketInfoFilter(const SocketInfoFilter& original)
: d_transport(original.d_transport)
, d_all(original.d_all)
{
}

NTSCFG_INLINE
SocketInfoFilter::~SocketInfoFilter()
{
}

NTSCFG_INLINE
SocketInfoFilter& SocketInfoFilter::operator=(const SocketInfoFilter& other)
{
    d_transport = other.d_transport;
    d_all       = other.d_all;
    return *this;
}

NTSCFG_INLINE
void SocketInfoFilter::reset()
{
    d_transport.reset();
    d_all.reset();
}

NTSCFG_INLINE
void SocketInfoFilter::setTransport(ntsa::Transport::Value value)
{
    d_transport = value;
}

NTSCFG_INLINE
void SocketInfoFilter::setAll(bool value)
{
    d_all = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Transport::Value>& SocketInfoFilter::
    transport() const
{
    return d_transport;
}

NTSCFG_INLINE
const bdlb::NullableValue<bool>& SocketInfoFilter::all() const
{
    return d_all;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const SocketInfoFilter& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const SocketInfoFilter& lhs, const SocketInfoFilter& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const SocketInfoFilter& lhs, const SocketInfoFilter& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const SocketInfoFilter& lhs, const SocketInfoFilter& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const SocketInfoFilter& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.transport());
    hashAppend(algorithm, value.all());
}

}  // close package namespace
}  // close enterprise namespace
#endif
