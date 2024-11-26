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

#ifndef INCLUDED_NTCA_DOWNGRADEEVENT
#define INCLUDED_NTCA_DOWNGRADEEVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_downgradecontext.h>
#include <ntca_downgradeeventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an event detected for an asynchronous downgrade operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of downgrade event.
///
/// @li @b context:
/// The state of downgrade at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_downgrade
class DowngradeEvent
{
    ntca::DowngradeEventType::Value d_type;
    ntca::DowngradeContext          d_context;

  public:
    /// Create a new downgrade event having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit DowngradeEvent(bslma::Allocator* basicAllocator = 0);

    /// Create a new downgrade event having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    DowngradeEvent(const DowngradeEvent& original,
                   bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~DowngradeEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    DowngradeEvent& operator=(const DowngradeEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of downgrade event to the specified 'value'.
    void setType(ntca::DowngradeEventType::Value value);

    /// Set the state of the downgrade at the time of the event to the
    /// specified 'value'.
    void setContext(const ntca::DowngradeContext& value);

    /// Return the type of downgrade event.
    ntca::DowngradeEventType::Value type() const;

    /// Return the state of the downgrade at the time of the event.
    const ntca::DowngradeContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const DowngradeEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const DowngradeEvent& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(DowngradeEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::DowngradeEvent
bsl::ostream& operator<<(bsl::ostream& stream, const DowngradeEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::DowngradeEvent
bool operator==(const DowngradeEvent& lhs, const DowngradeEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::DowngradeEvent
bool operator!=(const DowngradeEvent& lhs, const DowngradeEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::DowngradeEvent
bool operator<(const DowngradeEvent& lhs, const DowngradeEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::DowngradeEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DowngradeEvent& value);

NTCCFG_INLINE
DowngradeEvent::DowngradeEvent(bslma::Allocator* basicAllocator)
: d_type(ntca::DowngradeEventType::e_INITIATED)
, d_context(basicAllocator)
{
}

NTCCFG_INLINE
DowngradeEvent::DowngradeEvent(const DowngradeEvent& original,
                               bslma::Allocator*     basicAllocator)
: d_type(original.d_type)
, d_context(original.d_context, basicAllocator)
{
}

NTCCFG_INLINE
DowngradeEvent::~DowngradeEvent()
{
}

NTCCFG_INLINE
DowngradeEvent& DowngradeEvent::operator=(const DowngradeEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void DowngradeEvent::reset()
{
    d_type = ntca::DowngradeEventType::e_INITIATED;
    d_context.reset();
}

NTCCFG_INLINE
void DowngradeEvent::setType(ntca::DowngradeEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void DowngradeEvent::setContext(const ntca::DowngradeContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::DowngradeEventType::Value DowngradeEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::DowngradeContext& DowngradeEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const DowngradeEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const DowngradeEvent& lhs, const DowngradeEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const DowngradeEvent& lhs, const DowngradeEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const DowngradeEvent& lhs, const DowngradeEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DowngradeEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
