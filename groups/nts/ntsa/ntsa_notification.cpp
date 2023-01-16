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

#include <ntsa_notification.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_notification_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsa {

Notification::Notification(const Notification& original)
: d_type(original.d_type)
{
    switch (d_type) {
    case (ntsa::NotificationType::e_TIMESTAMP):
        new (d_timestamp.buffer())
            ntsa::Timestamp(original.d_timestamp.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::NotificationType::e_UNDEFINED);
    }
}

Notification& Notification::operator=(const Notification& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    d_type = other.d_type;

    switch (d_type) {
    case (ntsa::NotificationType::e_TIMESTAMP):
        new (d_timestamp.buffer()) ntsa::Timestamp(other.d_timestamp.object());
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::NotificationType::e_UNDEFINED);
    }

    return *this;
}

bool Notification::equals(const Notification& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::NotificationType::e_TIMESTAMP:
        return d_timestamp.object() == other.d_timestamp.object();
    default:
        return true;
    }
}

bool Notification::less(const Notification& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    switch (d_type) {
    case ntsa::NotificationType::e_TIMESTAMP:
        return d_timestamp.object() < other.d_timestamp.object();
    default:
        return true;
    }
}

bsl::ostream& Notification::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    switch (d_type) {
    case ntsa::NotificationType::e_TIMESTAMP:
        d_timestamp.object().print(stream, level, spacesPerLevel);
        break;
    default:
        BSLS_ASSERT(d_type == ntsa::NotificationType::e_UNDEFINED);
        stream << "UNDEFINED";
        break;
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Notification& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Notification& lhs, const Notification& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Notification& lhs, const Notification& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Notification& lhs, const Notification& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
