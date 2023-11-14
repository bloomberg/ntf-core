// Copyright 2023 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTSA_NOTIFICATION
#define INCLUDED_NTSA_NOTIFICATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_notificationtype.h>
#include <ntsa_timestamp.h>
#include <ntsa_zerocopy.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsls_objectbuffer.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a union of notifications.
///
/// @details
/// Provide a value-semantic type that represents a discriminated union of
/// notifications.
//
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class Notification
{
    union {
        bsls::ObjectBuffer<ntsa::Timestamp> d_timestamp;
        bsls::ObjectBuffer<ntsa::ZeroCopy>  d_zeroCopy;
    };

    ntsa::NotificationType::Value d_type;

  public:
    /// Create a new notification having an undefined type.
    Notification();

    /// Create a new notification having the same value as the specified
    /// 'other' object.
    Notification(const Notification& other);

    /// Destroy this object.
    ~Notification();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Notification& operator=(const Notification& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Select the "timestamp" representation. Return a reference to the
    /// modifiable representation.
    ntsa::Timestamp& makeTimestamp();

    /// Select the "timestamp" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::Timestamp& makeTimestamp(const ntsa::Timestamp& value);

    /// Select the "zeroCopy" representation. Return a reference to the
    /// modifiable representation.
    ntsa::ZeroCopy& makeZeroCopy();

    /// Select the "zeroCopy" representation initially having the specified
    /// 'value'. Return a reference to the modifiable representation.
    ntsa::ZeroCopy& makeZeroCopy(const ntsa::ZeroCopy& value);

    /// Return a reference to the "timestamp" representation. The behavior is
    /// undefined unless 'isTimestamp()' is true.
    const ntsa::Timestamp& timestamp() const;

    /// Return a reference to the "zeroCopy" representation. The behavior is
    /// undefined unless 'isZeroCopy()' is true.
    const ntsa::ZeroCopy& zeroCopy() const;

    /// Return the type of the notification representation.
    ntsa::NotificationType::Value type() const;

    /// Return true if the "timestamp" representation is currently selected,
    /// otherwise return false.
    bool isTimestamp() const;

    /// Return true if the "zeroCopy" representation is currently selected,
    /// otherwise return false.
    bool isZeroCopy() const;

    /// Return true if the notification representation is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const Notification& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const Notification& other) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select, at
    /// compile-time, the most efficient algorithm to manipulate objects of
    /// this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Notification);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::Notification
bsl::ostream& operator<<(bsl::ostream& stream, const Notification& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Notification
bool operator==(const Notification& lhs, const Notification& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Notification
bool operator!=(const Notification& lhs, const Notification& rhs);

/// Return true if the specified 'lhs' is less than the specified 'rhs',
/// otherwise return true.
///
/// @related ntsa::Notification
bool operator<(const Notification& lhs, const Notification& rhs);

/// Contribute the values of the salient attributes of the specified 'value' to
/// the specified hash 'algorithm'.
///
/// @related ntsa::Notification
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Notification& value);

NTSCFG_INLINE
Notification::Notification()
: d_type(ntsa::NotificationType::e_UNDEFINED)
{
}

NTSCFG_INLINE
Notification::~Notification()
{
}

NTSCFG_INLINE
void Notification::reset()
{
    d_type = ntsa::NotificationType::e_UNDEFINED;
}

NTSCFG_INLINE
ntsa::Timestamp& Notification::makeTimestamp()
{
    if (d_type == ntsa::NotificationType::e_TIMESTAMP) {
        d_timestamp.object() = ntsa::Timestamp();
    }
    else {
        this->reset();
        new (d_timestamp.buffer()) ntsa::Timestamp();
        d_type = ntsa::NotificationType::e_TIMESTAMP;
    }

    return d_timestamp.object();
}

NTSCFG_INLINE
ntsa::Timestamp& Notification::makeTimestamp(const ntsa::Timestamp& value)
{
    if (d_type == ntsa::NotificationType::e_TIMESTAMP) {
        d_timestamp.object() = value;
    }
    else {
        this->reset();
        new (d_timestamp.buffer()) ntsa::Timestamp(value);
        d_type = ntsa::NotificationType::e_TIMESTAMP;
    }

    return d_timestamp.object();
}

NTSCFG_INLINE
ntsa::ZeroCopy& Notification::makeZeroCopy()
{
    if (d_type == ntsa::NotificationType::e_ZERO_COPY) {
        d_zeroCopy.object() = ntsa::ZeroCopy();
    }
    else {
        this->reset();
        new (d_zeroCopy.buffer()) ntsa::ZeroCopy();
        d_type = ntsa::NotificationType::e_ZERO_COPY;
    }

    return d_zeroCopy.object();
}

NTSCFG_INLINE
ntsa::ZeroCopy& Notification::makeZeroCopy(const ntsa::ZeroCopy& value)
{
    if (d_type == ntsa::NotificationType::e_ZERO_COPY) {
        d_zeroCopy.object() = value;
    }
    else {
        this->reset();
        new (d_zeroCopy.buffer()) ntsa::ZeroCopy(value);
        d_type = ntsa::NotificationType::e_ZERO_COPY;
    }

    return d_zeroCopy.object();
}

NTSCFG_INLINE
const ntsa::Timestamp& Notification::timestamp() const
{
    BSLS_ASSERT(d_type == ntsa::NotificationType::e_TIMESTAMP);
    return d_timestamp.object();
}

NTSCFG_INLINE
const ntsa::ZeroCopy& Notification::zeroCopy() const
{
    BSLS_ASSERT(d_type == ntsa::NotificationType::e_ZERO_COPY);
    return d_zeroCopy.object();
}

NTSCFG_INLINE
ntsa::NotificationType::Value Notification::type() const
{
    return d_type;
}

NTSCFG_INLINE
bool Notification::isTimestamp() const
{
    return d_type == ntsa::NotificationType::e_TIMESTAMP;
}

NTSCFG_INLINE
bool Notification::isZeroCopy() const
{
    return d_type == ntsa::NotificationType::e_ZERO_COPY;
}

NTSCFG_INLINE
bool Notification::isUndefined() const
{
    return d_type == ntsa::NotificationType::e_UNDEFINED;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Notification& value)
{
    using bslh::hashAppend;

    if (value.isTimestamp()) {
        hashAppend(algorithm, value.timestamp());
    }
    else if (value.isZeroCopy()) {
        hashAppend(algorithm, value.zeroCopy());
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
