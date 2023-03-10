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

#ifndef INCLUDED_NTCA_GETIPADDRESSEVENT
#define INCLUDED_NTCA_GETIPADDRESSEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_getipaddresscontext.h>
#include <ntca_getipaddresseventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for an operation to get the IP addresses
/// assigned to a domain name.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of get IP address event.
///
/// @li @b context:
/// The context of the get IP address operation at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_resolve
class GetIpAddressEvent
{
    ntca::GetIpAddressEventType::Value d_type;
    ntca::GetIpAddressContext          d_context;

  public:
    /// Create a new get IP address event having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit GetIpAddressEvent(bslma::Allocator* basicAllocator = 0);

    /// Create a new get IP address event having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    GetIpAddressEvent(const GetIpAddressEvent& original,
                      bslma::Allocator*        basicAllocator = 0);

    /// Destroy this object.
    ~GetIpAddressEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    GetIpAddressEvent& operator=(const GetIpAddressEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of get IP address event to the specified 'value'.
    void setType(ntca::GetIpAddressEventType::Value value);

    /// Set the context of the get IP address operation at the time of the
    /// event to the specified 'value'.
    void setContext(const ntca::GetIpAddressContext& value);

    /// Return the type of get IP address event.
    ntca::GetIpAddressEventType::Value type() const;

    /// Return the context of the get IP address operation at the time of
    /// the event.
    const ntca::GetIpAddressContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const GetIpAddressEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const GetIpAddressEvent& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(GetIpAddressEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::GetIpAddressEvent
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const GetIpAddressEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::GetIpAddressEvent
bool operator==(const GetIpAddressEvent& lhs, const GetIpAddressEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::GetIpAddressEvent
bool operator!=(const GetIpAddressEvent& lhs, const GetIpAddressEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::GetIpAddressEvent
bool operator<(const GetIpAddressEvent& lhs, const GetIpAddressEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::GetIpAddressEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetIpAddressEvent& value);

NTCCFG_INLINE
GetIpAddressEvent::GetIpAddressEvent(bslma::Allocator* basicAllocator)
: d_type(ntca::GetIpAddressEventType::e_COMPLETE)
, d_context(basicAllocator)
{
}

NTCCFG_INLINE
GetIpAddressEvent::GetIpAddressEvent(const GetIpAddressEvent& original,
                                     bslma::Allocator*        basicAllocator)
: d_type(original.d_type)
, d_context(original.d_context, basicAllocator)
{
}

NTCCFG_INLINE
GetIpAddressEvent::~GetIpAddressEvent()
{
}

NTCCFG_INLINE
GetIpAddressEvent& GetIpAddressEvent::operator=(const GetIpAddressEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void GetIpAddressEvent::reset()
{
    d_type = ntca::GetIpAddressEventType::e_COMPLETE;
    d_context.reset();
}

NTCCFG_INLINE
void GetIpAddressEvent::setType(ntca::GetIpAddressEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void GetIpAddressEvent::setContext(const ntca::GetIpAddressContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::GetIpAddressEventType::Value GetIpAddressEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::GetIpAddressContext& GetIpAddressEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const GetIpAddressEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const GetIpAddressEvent& lhs, const GetIpAddressEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const GetIpAddressEvent& lhs, const GetIpAddressEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const GetIpAddressEvent& lhs, const GetIpAddressEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetIpAddressEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
