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

#ifndef INCLUDED_NTCA_GETDOMAINNAMEEVENT
#define INCLUDED_NTCA_GETDOMAINNAMEEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_getdomainnamecontext.h>
#include <ntca_getdomainnameeventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for an operation to get the domain name to which
/// an IP address is assigned.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of get domain name event.
///
/// @li @b context:
/// The context of the get domain name operation at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_resolve
class GetDomainNameEvent
{
    ntca::GetDomainNameEventType::Value d_type;
    ntca::GetDomainNameContext          d_context;

  public:
    /// Create a new get domain name event having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit GetDomainNameEvent(bslma::Allocator* basicAllocator = 0);

    /// Create a new get domain name event having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    GetDomainNameEvent(const GetDomainNameEvent& original,
                       bslma::Allocator*         basicAllocator = 0);

    /// Destroy this object.
    ~GetDomainNameEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    GetDomainNameEvent& operator=(const GetDomainNameEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of get domain name event to the specified 'value'.
    void setType(ntca::GetDomainNameEventType::Value value);

    /// Set the context of the get domain name operation at the time of the
    /// event to the specified 'value'.
    void setContext(const ntca::GetDomainNameContext& value);

    /// Return the type of get domain name event.
    ntca::GetDomainNameEventType::Value type() const;

    /// Return the context of the get domain name operation at the time of
    /// the event.
    const ntca::GetDomainNameContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const GetDomainNameEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const GetDomainNameEvent& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(GetDomainNameEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::GetDomainNameEvent
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const GetDomainNameEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::GetDomainNameEvent
bool operator==(const GetDomainNameEvent& lhs, const GetDomainNameEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::GetDomainNameEvent
bool operator!=(const GetDomainNameEvent& lhs, const GetDomainNameEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::GetDomainNameEvent
bool operator<(const GetDomainNameEvent& lhs, const GetDomainNameEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::GetDomainNameEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetDomainNameEvent& value);

NTCCFG_INLINE
GetDomainNameEvent::GetDomainNameEvent(bslma::Allocator* basicAllocator)
: d_type(ntca::GetDomainNameEventType::e_COMPLETE)
, d_context(basicAllocator)
{
}

NTCCFG_INLINE
GetDomainNameEvent::GetDomainNameEvent(const GetDomainNameEvent& original,
                                       bslma::Allocator* basicAllocator)
: d_type(original.d_type)
, d_context(original.d_context, basicAllocator)
{
}

NTCCFG_INLINE
GetDomainNameEvent::~GetDomainNameEvent()
{
}

NTCCFG_INLINE
GetDomainNameEvent& GetDomainNameEvent::operator=(
    const GetDomainNameEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void GetDomainNameEvent::reset()
{
    d_type = ntca::GetDomainNameEventType::e_COMPLETE;
    d_context.reset();
}

NTCCFG_INLINE
void GetDomainNameEvent::setType(ntca::GetDomainNameEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void GetDomainNameEvent::setContext(const ntca::GetDomainNameContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::GetDomainNameEventType::Value GetDomainNameEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::GetDomainNameContext& GetDomainNameEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const GetDomainNameEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const GetDomainNameEvent& lhs, const GetDomainNameEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const GetDomainNameEvent& lhs, const GetDomainNameEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const GetDomainNameEvent& lhs, const GetDomainNameEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetDomainNameEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
