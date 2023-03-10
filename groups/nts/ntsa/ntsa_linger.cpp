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

#include <ntsa_linger.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_linger_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool Linger::equals(const Linger& other) const
{
    return (d_enabled == other.d_enabled && d_duration == other.d_duration);
}

bool Linger::less(const Linger& other) const
{
    if (d_enabled < other.d_enabled) {
        return true;
    }

    if (d_enabled > other.d_enabled) {
        return false;
    }

    return d_duration < other.d_duration;
}

bsl::ostream& Linger::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("enabled", d_enabled);
    printer.printAttribute("duration", d_duration);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
