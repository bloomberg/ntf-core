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

#include <ntsd_message.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsd {

bool Message::equals(const Message& other) const
{
    if (d_header != other.header()) {
        return false;
    }

    if (bdlbb::BlobUtil::compare(d_payload, other.payload()) != 0) {
        return false;
    }

    return true;
}

bool Message::less(const Message& other) const
{
    if (d_header < other.header()) {
        return true;
    }

    if (other.header() < d_header) {
        return false;
    }

    return bdlbb::BlobUtil::compare(d_payload, other.payload()) < 0;
}

bsl::ostream& Message::print(bsl::ostream& stream,
                             int           level,
                             int           spacesPerLevel) const
{
    return d_header.print(stream, level, spacesPerLevel);
}

}  // close package namespace
}  // close enterprise namespace
