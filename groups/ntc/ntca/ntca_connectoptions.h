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

#ifndef INCLUDED_NTCA_CONNECTOPTIONS
#define INCLUDED_NTCA_CONNECTOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_connecttoken.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_ipaddress.h>
#include <ntsa_port.h>
#include <ntsa_transport.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntca {

/// Describe the parameters to a connect operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b token:
/// The token used to cancel the operation.
///
/// @li @b retryCount:
/// The number of additional attempts to attempt to connect, if and when the
/// initial attempt fails. The default value is null, which indicates that no
/// additional retries are performed.
///
/// @li @b retryInterval:
/// The interval between connection attempts, if and when the initial attempt
/// fails and the retry count is greater than zero. The default value is null,
/// which indicates an implementation-chosen default retry interval is used.
///
/// @li @b ipAddressFallback:
/// The implied IP address when no domain name or IP address is explicitly
/// defined. The default value is null, which indicates that resolution should
/// fail unless a domain name or IP address is explicitly defined.
///
/// @li @b ipAddressType:
/// The IP address type desired from the domain name resolution. The default
/// value is null, which indicates that a domain name can resolve to any IP
/// address suitable for being bound by a process on the local machine.
///
/// @li @b ipAddressSelector:
/// The round-robin selector of the chosen IP address out of the IP address
/// list assigned to a domain name. This value is always applied modulo the
/// size of the IP address list that is the result of resolving a domain name.
/// The default value is null, indicating the first IP address in the IP
/// address list is selected.
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
/// how domain names resolve to IP addresses and how service names resolve to
/// ports. The default value is null, indicating that domain names are allowed
/// to resolve to IP addresses of any type and service names are resolved to
/// ports for all transport protocols.
///
/// @li @b deadline:
/// The deadline within which the operation must complete, in absolute time
/// since the Unix epoch.
///
/// @li @b recurse:
/// Allow callbacks to be invoked immediately and recursively if their
/// constraints are already satisfied at the time the asynchronous operation
/// is initiated.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_connect
class ConnectOptions
{
    bdlb::NullableValue<ntca::ConnectToken>         d_token;
    bdlb::NullableValue<bsl::size_t>                d_retryCount;
    bdlb::NullableValue<bsls::TimeInterval>         d_retryInterval;
    bdlb::NullableValue<ntsa::IpAddress>            d_ipAddressFallback;
    bdlb::NullableValue<ntsa::IpAddressType::Value> d_ipAddressType;
    bdlb::NullableValue<bsl::size_t>                d_ipAddressSelector;
    bdlb::NullableValue<ntsa::Port>                 d_portFallback;
    bdlb::NullableValue<bsl::size_t>                d_portSelector;
    bdlb::NullableValue<ntsa::Transport::Value>     d_transport;
    bdlb::NullableValue<bsls::TimeInterval>         d_deadline;
    bool                                            d_recurse;

  public:
    /// Create new connect options having the default value.
    ConnectOptions();

    /// Create new connect options having the same value as the specified
    /// 'original' object.
    ConnectOptions(const ConnectOptions& original);

    /// Destroy this object.
    ~ConnectOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ConnectOptions& operator=(const ConnectOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the token used to cancel the operation to the specified 'value'.
    void setToken(const ntca::ConnectToken& value);

    /// Set the number of additional attempts to attempt to connect, if and
    /// when the initial attempt fails, to the specified 'value'.
    void setRetryCount(bsl::size_t value);

    /// Set the interval between connection attempts, if and when the
    /// initial attempt fails and the retry count is greater than zero, to
    /// the specified 'value'.
    void setRetryInterval(const bsls::TimeInterval& value);

    /// Set the implied IP address when no domain name or IP address is
    /// explicitly defined to the specified 'value'. The default value is
    /// null, which indicates that resolution should fail unless a domain
    /// name or IP address is explicitly defined.
    void setIpAddressFallback(const ntsa::IpAddress& value);

    /// Set the IP address type desired from the domain name resolution to
    /// the specified 'value'. The default value is null, which indicates
    /// that a domain name can resolve to any IP address suitable for being
    /// bound by a process on the local machine.
    void setIpAddressType(ntsa::IpAddressType::Value value);

    /// Set the round-robin selector of the chosen IP address out of the IP
    /// address list assigned to a domain name to the specified 'value'.
    /// This value is always applied modulo the size of the IP address list
    /// that is the result of resolving a domain name. The default value is
    /// null, indicating the first IP address in the IP address list
    /// is selected.
    void setIpAddressSelector(bsl::size_t value);

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

    /// Set the desired transport with which to use the endpoint to the
    /// specified 'value'. This value affects how domain names resolve to
    /// IP addresses and how service names resolve to ports. The default
    /// value is null, indicating that domain names are allowed to resolve
    /// to IP addresses of any type and service names are resolved to ports
    /// for all transport protocols.
    void setTransport(ntsa::Transport::Value value);

    /// Set the deadline within which the operation must complete to
    /// the specified 'value'.
    void setDeadline(const bsls::TimeInterval& value);

    /// Set the flag that allows callbacks to be invoked immediately and
    /// recursively if their constraints are already satisfied at the time
    /// the asynchronous operation is initiated.
    void setRecurse(bool value);

    /// Return the token used to cancel the operation.
    const bdlb::NullableValue<ntca::ConnectToken>& token() const;

    /// Return the number of additional attempts to attempt to connect, if
    /// and when the initial attempt fails.
    const bdlb::NullableValue<bsl::size_t>& retryCount() const;

    /// Return the interval between connection attempts, if and when the
    /// initial attempt fails and the retry count is greater than zero.
    const bdlb::NullableValue<bsls::TimeInterval>& retryInterval() const;

    /// Return the implied IP address when no domain name or IP address is
    /// explicitly defined to the specified 'value'. The default value is
    /// null, which indicates that resolution should fail unless a domain
    /// name or IP address is explicitly defined.
    const bdlb::NullableValue<ntsa::IpAddress>& ipAddressFallback() const;

    /// Return the IP address type desired from the domain name resolution
    /// to the specified 'value'. The default value is null, which indicates
    /// that a domain name can resolve to any IP address suitable for being
    /// bound by a process on the local machine.
    const bdlb::NullableValue<ntsa::IpAddressType::Value>& ipAddressType()
        const;

    /// Return the round-robin selector of the chosen IP address out of the
    /// IP address list assigned to a domain name to the specified 'value'.
    /// This value is always applied modulo the size of the IP address list
    /// that is the result of resolving a domain name. The default value is
    /// null, indicating the first IP address in the IP address list is
    /// selected.
    const bdlb::NullableValue<bsl::size_t>& ipAddressSelector() const;

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
    /// specified 'value'. This value affects how domain names resolve to
    /// IP addresses and how service names resolve to ports. The default
    /// value is null, indicating that domain names are allowed to resolve
    /// to IP addresses of any type and service names are resolved to ports
    /// for all transport protocols.
    const bdlb::NullableValue<ntsa::Transport::Value>& transport() const;

    /// Return the deadline within which the operation must complete.
    const bdlb::NullableValue<bsls::TimeInterval>& deadline() const;

    /// Return true if callbacks are allowed to be invoked immediately and
    /// recursively if their constraints are already satisfied at the time
    /// the asynchronous operation is initiated, otherwise return false.
    bool recurse() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ConnectOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ConnectOptions& other) const;

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
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ConnectOptions);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ConnectOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ConnectOptions
bsl::ostream& operator<<(bsl::ostream& stream, const ConnectOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ConnectOptions
bool operator==(const ConnectOptions& lhs, const ConnectOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ConnectOptions
bool operator!=(const ConnectOptions& lhs, const ConnectOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ConnectOptions
bool operator<(const ConnectOptions& lhs, const ConnectOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ConnectOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ConnectOptions& value);

NTCCFG_INLINE
ConnectOptions::ConnectOptions()
: d_token()
, d_retryCount()
, d_retryInterval()
, d_ipAddressFallback()
, d_ipAddressType()
, d_ipAddressSelector()
, d_portFallback()
, d_portSelector()
, d_transport()
, d_deadline()
, d_recurse(false)
{
}

NTCCFG_INLINE
ConnectOptions::ConnectOptions(const ConnectOptions& original)
: d_token(original.d_token)
, d_retryCount(original.d_retryCount)
, d_retryInterval(original.d_retryInterval)
, d_ipAddressFallback(original.d_ipAddressFallback)
, d_ipAddressType(original.d_ipAddressType)
, d_ipAddressSelector(original.d_ipAddressSelector)
, d_portFallback(original.d_portFallback)
, d_portSelector(original.d_portSelector)
, d_transport(original.d_transport)
, d_deadline(original.d_deadline)
, d_recurse(original.d_recurse)
{
}

NTCCFG_INLINE
ConnectOptions::~ConnectOptions()
{
}

NTCCFG_INLINE
ConnectOptions& ConnectOptions::operator=(const ConnectOptions& other)
{
    d_token             = other.d_token;
    d_retryCount        = other.d_retryCount;
    d_retryInterval     = other.d_retryInterval;
    d_ipAddressFallback = other.d_ipAddressFallback;
    d_ipAddressType     = other.d_ipAddressType;
    d_ipAddressSelector = other.d_ipAddressSelector;
    d_portFallback      = other.d_portFallback;
    d_portSelector      = other.d_portSelector;
    d_transport         = other.d_transport;
    d_deadline          = other.d_deadline;
    d_recurse           = other.d_recurse;
    return *this;
}

NTCCFG_INLINE
void ConnectOptions::reset()
{
    d_token.reset();
    d_retryCount.reset();
    d_retryInterval.reset();
    d_ipAddressFallback.reset();
    d_ipAddressType.reset();
    d_ipAddressSelector.reset();
    d_portFallback.reset();
    d_portSelector.reset();
    d_transport.reset();
    d_deadline.reset();
    d_recurse = false;
}

NTCCFG_INLINE
void ConnectOptions::setToken(const ntca::ConnectToken& value)
{
    d_token = value;
}

NTCCFG_INLINE
void ConnectOptions::setRetryCount(bsl::size_t value)
{
    d_retryCount = value;
}

NTCCFG_INLINE
void ConnectOptions::setRetryInterval(const bsls::TimeInterval& value)
{
    d_retryInterval = value;
}

NTSCFG_INLINE
void ConnectOptions::setIpAddressFallback(const ntsa::IpAddress& value)
{
    d_ipAddressFallback = value;
}

NTSCFG_INLINE
void ConnectOptions::setIpAddressType(ntsa::IpAddressType::Value value)
{
    d_ipAddressType = value;
}

NTSCFG_INLINE
void ConnectOptions::setIpAddressSelector(bsl::size_t value)
{
    d_ipAddressSelector = value;
}

NTSCFG_INLINE
void ConnectOptions::setPortFallback(ntsa::Port value)
{
    d_portFallback = value;
}

NTSCFG_INLINE
void ConnectOptions::setPortSelector(bsl::size_t value)
{
    d_portSelector = value;
}

NTSCFG_INLINE
void ConnectOptions::setTransport(ntsa::Transport::Value value)
{
    d_transport = value;
}

NTCCFG_INLINE
void ConnectOptions::setDeadline(const bsls::TimeInterval& value)
{
    d_deadline = value;
}

NTCCFG_INLINE
void ConnectOptions::setRecurse(bool value)
{
    d_recurse = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<ntca::ConnectToken>& ConnectOptions::token() const
{
    return d_token;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ConnectOptions::retryCount() const
{
    return d_retryCount;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& ConnectOptions::retryInterval()
    const
{
    return d_retryInterval;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::IpAddress>& ConnectOptions::ipAddressFallback()
    const
{
    return d_ipAddressFallback;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::IpAddressType::Value>& ConnectOptions::
    ipAddressType() const
{
    return d_ipAddressType;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ConnectOptions::ipAddressSelector()
    const
{
    return d_ipAddressSelector;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Port>& ConnectOptions::portFallback() const
{
    return d_portFallback;
}

NTSCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ConnectOptions::portSelector() const
{
    return d_portSelector;
}

NTSCFG_INLINE
const bdlb::NullableValue<ntsa::Transport::Value>& ConnectOptions::transport()
    const
{
    return d_transport;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& ConnectOptions::deadline() const
{
    return d_deadline;
}

NTCCFG_INLINE
bool ConnectOptions::recurse() const
{
    return d_recurse;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ConnectOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ConnectOptions& lhs, const ConnectOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ConnectOptions& lhs, const ConnectOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ConnectOptions& lhs, const ConnectOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ConnectOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.token());
    hashAppend(algorithm, value.retryCount());
    hashAppend(algorithm, value.retryInterval());
    hashAppend(algorithm, value.ipAddressFallback());
    hashAppend(algorithm, value.ipAddressType());
    hashAppend(algorithm, value.ipAddressSelector());
    hashAppend(algorithm, value.portFallback());
    hashAppend(algorithm, value.portSelector());
    hashAppend(algorithm, value.transport());
    hashAppend(algorithm, value.deadline());
    hashAppend(algorithm, value.recurse());
}

}  // close package namespace
}  // close enterprise namespace
#endif
