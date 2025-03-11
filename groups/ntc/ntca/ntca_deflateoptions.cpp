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

#include <ntca_deflateoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_deflateoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool DeflateOptions::equals(const DeflateOptions& other) const
{
    return (d_partial == other.d_partial && d_checksum == other.d_checksum);
}

bool DeflateOptions::less(const DeflateOptions& other) const
{
    if (d_partial < other.d_partial) {
        return true;
    }

    if (other.d_partial < d_partial) {
        return false;
    }

    return d_checksum < other.d_checksum;
}

bsl::ostream& DeflateOptions::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (d_partial.has_value()) {
        printer.printAttribute("partial", d_partial);
    }
    if (d_checksum.has_value()) {
        printer.printAttribute("checksum", d_checksum);
    }
    printer.end();
    return stream;
}

}  // close namespace ntca
}  // close namespace BloombergLP
