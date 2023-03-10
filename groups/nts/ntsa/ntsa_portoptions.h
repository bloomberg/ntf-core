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

#ifndef INCLUDED_NTSA_PORTOPTIONS
#define INCLUDED_NTSA_PORTOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_port.h>
#include <ntsa_transport.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide options to get a port from a service name.
///
/// @details
/// Provide a value-semantic type that describes the options to get
/// a port from a service name.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b portFallback:
/// The implied port when no service name or port is explicitly defined. The
/// default value is null, which indicates that resolution should fail unless a
/// service name or port is explicitly defined.
///
/// @li @b portSelector:
/// The round-robin selector of the chosen port out of the port list assigned
/// to a service name. This value is always applied modulo the size of the port
/// list that is the result of resolving a service name. The default value is
/// null, indicating the first port in the port list is selected.
///
/// @li @b transport:
/// The desired transport with which to use the endpoint. This value affects
/// how service names resolve to ports. The default value is null, indicating
/// that service names are resolved to ports for all transport protocols.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_operation
class PortOptions
{
    bdlb::NullableValue<ntsa::Port>             d_portFallback;
    bdlb::NullableValue<bsl::size_t>            d_portSelector;
    bdlb::NullableValue<ntsa::Transport::Value> d_transport;

  public:
    /// Create new send options having the default value.
    PortOptions();

    /// Create new send options having the same value as the specified
    /// 'original' object.
    PortOptions(const PortOptions& original);

    /// Destroy this object.
    ~PortOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    PortOptions& operator=(const PortOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the implied port when no service name or port is explicitly
    /// defined to the specified 'value'. The default value is null,
    /// which indicates that resolution should fail unless a service name or
    /// port is explicitly defined.
    void setPortFallback(ntsa::Port value);

    /// Set the round-robin selector of the chosen port out of the port list
    /// assigned to a service name to the specified 'value'. This value is
    /// always applied modulo the size of the port list that is the result
    /// of resolving a service name. The default value is null, indicating
    /// the first port in the port list is selected.
    void setPortSelector(bsl::size_t value);

    /// Set the desired transport with which to use the port to the
    /// specified 'value'. This value affects how service names resolve to
    /// ports. The default value is null, indicating that service names are
    /// resolved to ports for all transport protocols.
    void setTransport(ntsa::Transport::Value value);

    /// Return the implied port when no service name or port is explicitly
    /// defined to the specified 'value'. The default value is null, which
    /// indicates that resolution should fail unless a service name or port
    /// is explicitly defined.
    const bdlb::NullableValue<ntsa::Port>& portFallback() const;

    /// Return the round-robin selector of the chosen port out of the port
    /// list assigned to a service name to the specified 'value'. This value
    /// is always applied modulo the size of the port list that is the
    /// result of resolving a service name. The default value is null,
    /// indicating the first port in the port list is selected.
    const bdlb::NullableValue<bsl::size_t>& portSelector() const;

    /// Return the desired transport with which to use the endpoint to the
    /// specified 'value'. This value affects how service names resolve to
    /// ports. The default value is null, indicating that service names are
    /// resolved to ports for all transport protocols.
    const bdlb::NullableValue<ntsa::Transport::Value>& transport() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const PortOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const PortOptions& other) const;

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
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(PortOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::PortOptions
bsl::ostream& operator<<(bsl::ostream& stream, const PortOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::PortOptions
bool operator==(const PortOptions& lhs, const PortOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::PortOptions
bool operator!=(const PortOptions& lhs, const PortOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::PortOptions
bool operator<(const PortOptions& lhs, const PortOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::PortOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const PortOptions& value);

NTSCFG_INLINE
PortOptions::PortOptions()
: d_portFallback()
, d_portSelector()
, d_transport()
{
}

NTSCFG_INLINE
PortOptions::PortOptions(const PortOptions& original)
: d_portFallback(original.d_portFallback)
, d_portSelector(original.d_portSelector)
, d_transport(original.d_transport)
{
}

NTSCFG_INLINE
PortOptions::~PortOptions()
{
}

NTSCFG_INLINE
PortOptions& PortOptions::operator=(const PortOptions& other)
{
    d_portFallback = other.d_portFallback;
    d_portSelector = other.d_portSelector;
    d_transport    = other.d_transport;
    return *this;
}

NTSCFG_INLINE
void PortOptions::reset()
{
    d_portFallback.reset();
    d_portSelector.reset();
    d_transport.reset();
}

NTSCFG_INLINE
void PortOptions::setPortFallback(ntsa::Port value)
{
    d_portFallback = value;
}

NTSCFG_INLINE
void PortOptions::setPortSelector(bsl::size_t value)
{
    d_portSelector = value;
}

NTSCFG_INLINE
void PortOptions::setTransport(ntsa::Transport::Value value)
{
    d_transport = value;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Port>& PortOptions::portFallback() const
{
    return d_portFallback;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& PortOptions::portSelector() const
{
    return d_portSelector;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Transport::Value>& PortOptions::transport()
    const
{
    return d_transport;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const PortOptions& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const PortOptions& lhs, const PortOptions& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const PortOptions& lhs, const PortOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const PortOptions& lhs, const PortOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const PortOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.portFallback());
    hashAppend(algorithm, value.portSelector());
    hashAppend(algorithm, value.transport());
}

}  // close package namespace
}  // close enterprise namespace
#endif
