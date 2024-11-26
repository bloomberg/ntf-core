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

#ifndef INCLUDED_NTCA_GETPORTEVENT
#define INCLUDED_NTCA_GETPORTEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_getportcontext.h>
#include <ntca_getporteventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for an operation to get the ports assigned to a
/// service name.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of get port event.
///
/// @li @b context:
/// The context of the get port operation at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_resolve
class GetPortEvent
{
    ntca::GetPortEventType::Value d_type;
    ntca::GetPortContext          d_context;

  public:
    /// Create a new get port event having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit GetPortEvent(bslma::Allocator* basicAllocator = 0);

    /// Create a new get port event having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    GetPortEvent(const GetPortEvent& original,
                 bslma::Allocator*   basicAllocator = 0);

    /// Destroy this object.
    ~GetPortEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    GetPortEvent& operator=(const GetPortEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of get port event to the specified 'value'.
    void setType(ntca::GetPortEventType::Value value);

    /// Set the context of the get port operation at the time of the event
    /// to the specified 'value'.
    void setContext(const ntca::GetPortContext& value);

    /// Return the type of get port event.
    ntca::GetPortEventType::Value type() const;

    /// Return the context of the get port operation at the time of the
    /// event.
    const ntca::GetPortContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const GetPortEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const GetPortEvent& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(GetPortEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::GetPortEvent
bsl::ostream& operator<<(bsl::ostream& stream, const GetPortEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::GetPortEvent
bool operator==(const GetPortEvent& lhs, const GetPortEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::GetPortEvent
bool operator!=(const GetPortEvent& lhs, const GetPortEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::GetPortEvent
bool operator<(const GetPortEvent& lhs, const GetPortEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::GetPortEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetPortEvent& value);

NTCCFG_INLINE
GetPortEvent::GetPortEvent(bslma::Allocator* basicAllocator)
: d_type(ntca::GetPortEventType::e_COMPLETE)
, d_context(basicAllocator)
{
}

NTCCFG_INLINE
GetPortEvent::GetPortEvent(const GetPortEvent& original,
                           bslma::Allocator*   basicAllocator)
: d_type(original.d_type)
, d_context(original.d_context, basicAllocator)
{
}

NTCCFG_INLINE
GetPortEvent::~GetPortEvent()
{
}

NTCCFG_INLINE
GetPortEvent& GetPortEvent::operator=(const GetPortEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void GetPortEvent::reset()
{
    d_type = ntca::GetPortEventType::e_COMPLETE;
    d_context.reset();
}

NTCCFG_INLINE
void GetPortEvent::setType(ntca::GetPortEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void GetPortEvent::setContext(const ntca::GetPortContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::GetPortEventType::Value GetPortEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::GetPortContext& GetPortEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const GetPortEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const GetPortEvent& lhs, const GetPortEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const GetPortEvent& lhs, const GetPortEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const GetPortEvent& lhs, const GetPortEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetPortEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
