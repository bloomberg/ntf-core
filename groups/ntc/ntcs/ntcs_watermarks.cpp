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

#include <ntcs_watermarks.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_watermarks_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntcs {

bool Watermarks::equals(const Watermarks& other) const
{
    return (d_current == other.d_current && d_low == other.d_low &&
            d_high == other.d_high);
}

bool Watermarks::less(const Watermarks& other) const
{
    if (d_current < other.d_current) {
        return true;
    }

    if (d_current > other.d_current) {
        return false;
    }

    if (d_low < other.d_low) {
        return true;
    }

    if (d_low > other.d_low) {
        return false;
    }

    return d_high < other.d_high;
}

bsl::ostream& Watermarks::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("current", d_current);
    printer.printAttribute("low", d_low);
    printer.printAttribute("high", d_high);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
