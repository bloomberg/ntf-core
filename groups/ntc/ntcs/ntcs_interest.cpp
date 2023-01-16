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

#include <ntcs_interest.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_interest_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntcs {

bool Interest::equals(const Interest& other) const
{
    return (d_value == other.d_value);
}

bool Interest::less(const Interest& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& Interest::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    bool empty = true;

    if (d_value == 0) {
        stream << "(none)";
        return stream;
    }

    if ((d_value & e_READABLE) != 0) {
        if (empty) {
            stream << ' ';
        }
        stream << "READABLE";
        empty = false;
    }

    if ((d_value & e_WRITABLE) != 0) {
        if (empty) {
            stream << ' ';
        }
        stream << "WRITABLE";
        empty = false;
    }

    if ((d_value & e_EDGE) != 0) {
        if (empty) {
            stream << ' ';
        }
        stream << "EDGE";
        empty = false;
    }

    if ((d_value & e_ONE_SHOT) != 0) {
        if (empty) {
            stream << ' ';
        }
        stream << "ONESHOT";
        empty = false;
    }

    if ((d_value & e_NOTIFICATION) != 0) {
        if (empty) {
            stream << ' ';
        }
        stream << "NOTIFICATION";
        empty = false;
    }

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
