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

#ifndef INCLUDED_NTCA_GETENDPOINTEVENT
#define INCLUDED_NTCA_GETENDPOINTEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_getendpointcontext.h>
#include <ntca_getendpointeventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for an operation to get an endpoint from a
/// domain name or IP address and a service name or port.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of get endpoint event.
///
/// @li @b context:
/// The context of the get endpoint operation at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_resolve
class GetEndpointEvent
{
    ntca::GetEndpointEventType::Value d_type;
    ntca::GetEndpointContext          d_context;

  public:
    /// Create a new get IP address event having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit GetEndpointEvent(bslma::Allocator* basicAllocator = 0);

    /// Create a new get IP address event having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    GetEndpointEvent(const GetEndpointEvent& original,
                     bslma::Allocator*       basicAllocator = 0);

    /// Destroy this object.
    ~GetEndpointEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    GetEndpointEvent& operator=(const GetEndpointEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of get IP address event to the specified 'value'.
    void setType(ntca::GetEndpointEventType::Value value);

    /// Set the context of the get IP address operation at the time of the
    /// event to the specified 'value'.
    void setContext(const ntca::GetEndpointContext& value);

    /// Return the type of get IP address event.
    ntca::GetEndpointEventType::Value type() const;

    /// Return the context of the get IP address operation at the time of
    /// the event.
    const ntca::GetEndpointContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const GetEndpointEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const GetEndpointEvent& other) const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(GetEndpointEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::GetEndpointEvent
bsl::ostream& operator<<(bsl::ostream& stream, const GetEndpointEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::GetEndpointEvent
bool operator==(const GetEndpointEvent& lhs, const GetEndpointEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::GetEndpointEvent
bool operator!=(const GetEndpointEvent& lhs, const GetEndpointEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::GetEndpointEvent
bool operator<(const GetEndpointEvent& lhs, const GetEndpointEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::GetEndpointEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetEndpointEvent& value);

NTCCFG_INLINE
GetEndpointEvent::GetEndpointEvent(bslma::Allocator* basicAllocator)
: d_type(ntca::GetEndpointEventType::e_COMPLETE)
, d_context(basicAllocator)
{
}

NTCCFG_INLINE
GetEndpointEvent::GetEndpointEvent(const GetEndpointEvent& original,
                                   bslma::Allocator*       basicAllocator)
: d_type(original.d_type)
, d_context(original.d_context, basicAllocator)
{
}

NTCCFG_INLINE
GetEndpointEvent::~GetEndpointEvent()
{
}

NTCCFG_INLINE
GetEndpointEvent& GetEndpointEvent::operator=(const GetEndpointEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void GetEndpointEvent::reset()
{
    d_type = ntca::GetEndpointEventType::e_COMPLETE;
    d_context.reset();
}

NTCCFG_INLINE
void GetEndpointEvent::setType(ntca::GetEndpointEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void GetEndpointEvent::setContext(const ntca::GetEndpointContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::GetEndpointEventType::Value GetEndpointEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::GetEndpointContext& GetEndpointEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const GetEndpointEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const GetEndpointEvent& lhs, const GetEndpointEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const GetEndpointEvent& lhs, const GetEndpointEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const GetEndpointEvent& lhs, const GetEndpointEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetEndpointEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
