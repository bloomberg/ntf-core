// Copyright 22023 Bloomberg Finance L.P.
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

#include <ntsa_timestamp.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_timestamp_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool Timestamp::equals(const Timestamp& other) const
{
    return (d_type == other.d_type && d_id == other.d_id &&
            d_time == other.d_time);
}

bool Timestamp::less(const Timestamp& other) const
{
    if (d_type < other.d_type) {
        return true;
    }

    if (other.d_type < d_type) {
        return false;
    }

    if (d_id < other.d_id) {
        return true;
    }

    if (other.d_id < d_id) {
        return true;
    }

    return d_time < other.d_time;
}

bsl::ostream& Timestamp::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("type", d_type);
    printer.printAttribute("id", d_id);
    printer.printAttribute("time", d_time);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
