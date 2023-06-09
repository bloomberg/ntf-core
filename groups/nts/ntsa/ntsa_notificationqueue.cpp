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

#include <ntsa_notificationqueue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_notificationqueue_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

NotificationQueue::NotificationQueue(bslma::Allocator* basicAllocator)
: d_handle(ntsa::k_INVALID_HANDLE)
, d_notifications(basicAllocator)
{
}

NotificationQueue::NotificationQueue(ntsa::Handle      handle,
                                     bslma::Allocator* basicAllocator)
: d_handle(handle)
, d_notifications(basicAllocator)
{
}

NotificationQueue::NotificationQueue(const ntsa::NotificationQueue& queue,
                                     bslma::Allocator* basicAllocator)
: d_handle(queue.d_handle)
, d_notifications(queue.d_notifications, basicAllocator)
{
}

void NotificationQueue::addNotification(const ntsa::Notification& notification)
{
    d_notifications.push_back(notification);
}

void NotificationQueue::setHandle(ntsa::Handle handle)
{
    d_handle = handle;
}

ntsa::Handle NotificationQueue::handle() const
{
    return d_handle;
}

const bsl::vector<ntsa::Notification>& NotificationQueue::notifications() const
{
    return d_notifications;
}

bool NotificationQueue::equals(const NotificationQueue& other) const
{
    if (this == &other) {
        return true;
    }

    if (d_handle != other.d_handle) {
        return false;
    }

    if (d_notifications != other.d_notifications) {
        return false;
    }

    return true;
}

bsl::ostream& NotificationQueue::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("handle", d_handle);
    printer.printAttribute("notifications",
                           d_notifications.cbegin(),
                           d_notifications.cend());
    printer.end();

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const NotificationQueue& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const NotificationQueue& lhs, const NotificationQueue& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const NotificationQueue& lhs, const NotificationQueue& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
