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

#include <ntca_acceptoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_acceptoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool AcceptOptions::equals(const AcceptOptions& other) const
{
    return (d_token == other.d_token && d_deadline == other.d_deadline &&
            d_recurse == other.d_recurse);
}

bool AcceptOptions::less(const AcceptOptions& other) const
{
    if (d_token < other.d_token) {
        return true;
    }

    if (other.d_token < d_token) {
        return false;
    }

    if (d_deadline < other.d_deadline) {
        return true;
    }

    if (d_deadline > other.d_deadline) {
        return false;
    }

    return d_recurse < other.d_recurse;
}

bsl::ostream& AcceptOptions::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("token", d_token);
    printer.printAttribute("deadline", d_deadline);
    printer.printAttribute("recurse", d_recurse);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
