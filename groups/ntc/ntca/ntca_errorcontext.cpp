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

#include <ntca_errorcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_errorcontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool ErrorContext::equals(const ErrorContext& other) const
{
    return (d_error == other.d_error &&
            d_errorDescription == other.d_errorDescription);
}

bool ErrorContext::less(const ErrorContext& other) const
{
    if (d_error < other.d_error) {
        return true;
    }

    if (other.d_error < d_error) {
        return false;
    }

    return d_errorDescription < other.d_errorDescription;
}

bsl::ostream& ErrorContext::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("error", d_error);
    printer.printAttribute("errorDescription", d_errorDescription);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
