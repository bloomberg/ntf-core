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

#include <ntca_getportoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_getportoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool GetPortOptions::equals(const GetPortOptions& other) const
{
    return (d_portFallback == other.d_portFallback &&
            d_portSelector == other.d_portSelector &&
            d_transport == other.d_transport &&
            d_deadline == other.d_deadline);
}

bool GetPortOptions::less(const GetPortOptions& other) const
{
    if (d_portFallback < other.d_portFallback) {
        return true;
    }

    if (other.d_portFallback < d_portFallback) {
        return false;
    }

    if (d_portSelector < other.d_portSelector) {
        return true;
    }

    if (other.d_portSelector < d_portSelector) {
        return false;
    }

    if (d_transport < other.d_transport) {
        return true;
    }

    if (other.d_transport < d_transport) {
        return false;
    }

    return d_deadline < other.d_deadline;
}

bsl::ostream& GetPortOptions::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_portFallback.isNull()) {
        printer.printAttribute("portFallback", d_portFallback);
    }

    if (!d_portSelector.isNull()) {
        printer.printAttribute("portSelector", d_portSelector);
    }

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
