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

#ifndef INCLUDED_NTCA_ERROREVENT
#define INCLUDED_NTCA_ERROREVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_errorcontext.h>
#include <ntca_erroreventtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe an error event detected during the asynchronous operation of a
/// socket.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b type:
/// The type of error event.
///
/// @li @b context:
/// The description of the error at the time of the event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_failure
class ErrorEvent
{
    ntca::ErrorEventType::Value d_type;
    ntca::ErrorContext          d_context;

  public:
    /// Create a new error event having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit ErrorEvent(bslma::Allocator* basicAllocator = 0);

    /// Create a new error event having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    ErrorEvent(const ErrorEvent& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ErrorEvent();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ErrorEvent& operator=(const ErrorEvent& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the type of error event to the specified 'value'.
    void setType(ntca::ErrorEventType::Value value);

    /// Set the state of the error at the time of the event to the
    /// specified 'value'.
    void setContext(const ntca::ErrorContext& value);

    /// Return the type of error event.
    ntca::ErrorEventType::Value type() const;

    /// Return the state of the error at the time of the event.
    const ntca::ErrorContext& context() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ErrorEvent& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ErrorEvent& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ErrorEvent);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::ErrorEvent
bsl::ostream& operator<<(bsl::ostream& stream, const ErrorEvent& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::ErrorEvent
bool operator==(const ErrorEvent& lhs, const ErrorEvent& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::ErrorEvent
bool operator!=(const ErrorEvent& lhs, const ErrorEvent& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::ErrorEvent
bool operator<(const ErrorEvent& lhs, const ErrorEvent& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::ErrorEvent
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ErrorEvent& value);

NTCCFG_INLINE
ErrorEvent::ErrorEvent(bslma::Allocator* basicAllocator)
: d_type(ntca::ErrorEventType::e_UNKNOWN)
, d_context(basicAllocator)
{
}

NTCCFG_INLINE
ErrorEvent::ErrorEvent(const ErrorEvent& original,
                       bslma::Allocator* basicAllocator)
: d_type(original.d_type)
, d_context(original.d_context, basicAllocator)
{
}

NTCCFG_INLINE
ErrorEvent::~ErrorEvent()
{
}

NTCCFG_INLINE
ErrorEvent& ErrorEvent::operator=(const ErrorEvent& other)
{
    d_type    = other.d_type;
    d_context = other.d_context;
    return *this;
}

NTCCFG_INLINE
void ErrorEvent::reset()
{
    d_type = ntca::ErrorEventType::e_UNKNOWN;
    d_context.reset();
}

NTCCFG_INLINE
void ErrorEvent::setType(ntca::ErrorEventType::Value value)
{
    d_type = value;
}

NTCCFG_INLINE
void ErrorEvent::setContext(const ntca::ErrorContext& context)
{
    d_context = context;
}

NTCCFG_INLINE
ntca::ErrorEventType::Value ErrorEvent::type() const
{
    return d_type;
}

NTCCFG_INLINE
const ntca::ErrorContext& ErrorEvent::context() const
{
    return d_context;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ErrorEvent& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const ErrorEvent& lhs, const ErrorEvent& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const ErrorEvent& lhs, const ErrorEvent& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const ErrorEvent& lhs, const ErrorEvent& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ErrorEvent& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.type());
    hashAppend(algorithm, value.context());
}

}  // close package namespace
}  // close enterprise namespace
#endif
