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

#ifndef INCLUDED_NTSA_NOTIFICATIONQUEUE
#define INCLUDED_NTSA_NOTIFICATIONQUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_handle.h>
#include <ntsa_notification.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a queue of notifications.
///
/// @details
/// Provide a value-semantic type that represents a queue of notifications.
//
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class NotificationQueue
{
    ntsa::Handle                    d_handle;
    bsl::vector<ntsa::Notification> d_notifications;

  public:
    /// This is a constant which can be used as a hint for an allocator passed
    /// to constructor. Experiments showed that this number of bytes is enough
    /// to allocate memory for up to 32 notifications.
    enum { k_NUM_BYTES_TO_ALLOCATE = 1024 };

    /// Constructs a new queue using the specified 'basicAllocator' if
    /// provided.
    explicit NotificationQueue(bslma::Allocator* basicAllocator = 0);

    /// Constructs a new queue for the specified 'handle' and using the
    /// specified 'basicAllocator' if provided.
    NotificationQueue(ntsa::Handle      handle,
                      bslma::Allocator* basicAllocator = 0);

    /// Copy constructs a new queue using the specified 'queue' and the
    /// specified 'basicAllocator' if provided.
    NotificationQueue(const ntsa::NotificationQueue& queue,
                      bslma::Allocator*              basicAllocator = 0);

    /// Save the specified 'handle'.
    void setHandle(ntsa::Handle handle);

    /// Add the specified 'notification' to the end of the queue.
    void addNotification(const ntsa::Notification& notification);

    /// Return the handle.
    ntsa::Handle handle() const;

    /// Return the notifications.
    const bsl::vector<ntsa::Notification>& notifications() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const NotificationQueue& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(NotificationQueue);
};

/// Write the specified 'object' to the specified 'stream'. Return a
/// modifiable reference to the 'stream'.
///
/// @related ntsa::NotificationQueue
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const NotificationQueue& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::NotificationQueue
bool operator==(const NotificationQueue& lhs, const NotificationQueue& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::NotificationQueue
bool operator!=(const NotificationQueue& lhs, const NotificationQueue& rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
