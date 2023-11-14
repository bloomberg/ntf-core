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

#include <ntsa_zerocopy.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_zerocopy_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool ZeroCopy::equals(const ZeroCopy& other) const
{
    return (d_from == other.d_from && d_to == other.d_to &&
            d_code == other.d_code);
}

bool ZeroCopy::less(const ZeroCopy& other) const
{
    if (d_from < other.d_from) {
        return true;
    }

    if (other.d_from < d_from) {
        return false;
    }

    if (d_to < other.d_to) {
        return true;
    }

    if (other.d_to < d_to) {
        return false;
    }

    return d_code < other.d_code;
}

bsl::ostream& ZeroCopy::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("from", d_from);
    printer.printAttribute("to", d_to);
    printer.printAttribute("code", d_code);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
