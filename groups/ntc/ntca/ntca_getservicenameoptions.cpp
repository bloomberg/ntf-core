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

#include <ntca_getservicenameoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_getservicenameoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool GetServiceNameOptions::equals(const GetServiceNameOptions& other) const
{
    return (d_transport == other.d_transport &&
            d_deadline == other.d_deadline);
}

bool GetServiceNameOptions::less(const GetServiceNameOptions& other) const
{
    if (d_transport < other.d_transport) {
        return true;
    }

    if (d_transport > other.d_transport) {
        return false;
    }

    return d_deadline < other.d_deadline;
}

bsl::ostream& GetServiceNameOptions::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_transport.isNull()) {
        printer.printAttribute("transport", d_transport);
    }

    if (!d_deadline.isNull()) {
        printer.printAttribute("deadline", d_deadline);
    }

    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
