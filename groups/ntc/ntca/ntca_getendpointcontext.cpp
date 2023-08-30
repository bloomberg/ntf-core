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

#include <ntca_getendpointcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_getendpointcontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool GetEndpointContext::equals(const GetEndpointContext& other) const
{
    return (d_authority == other.d_authority && d_latency == other.d_latency &&
            d_source == other.d_source && d_nameServer == other.d_nameServer &&
            d_timeToLive == other.d_timeToLive && d_error == other.d_error);
}

bool GetEndpointContext::less(const GetEndpointContext& other) const
{
    if (d_authority < other.d_authority) {
        return true;
    }

    if (other.d_authority < d_authority) {
        return false;
    }

    if (d_latency < other.d_latency) {
        return true;
    }

    if (other.d_latency < d_latency) {
        return false;
    }

    if (d_source < other.d_source) {
        return true;
    }

    if (other.d_source < d_source) {
        return false;
    }

    if (d_nameServer < other.d_nameServer) {
        return true;
    }

    if (other.d_nameServer < d_nameServer) {
        return false;
    }

    if (d_timeToLive < other.d_timeToLive) {
        return true;
    }

    if (other.d_timeToLive < d_timeToLive) {
        return false;
    }

    return d_error < other.d_error;
}

bsl::ostream& GetEndpointContext::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("authority", d_authority);
    printer.printAttribute("latency", d_latency);
    printer.printAttribute("source", d_source);
    printer.printAttribute("nameServer", d_nameServer);
    printer.printAttribute("timeToLive", d_timeToLive);
    printer.printAttribute("error", d_error);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
