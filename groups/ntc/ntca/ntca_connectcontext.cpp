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

#include <ntca_connectcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_connectcontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool ConnectContext::equals(const ConnectContext& other) const
{
    return (d_endpoint == other.d_endpoint && d_name == other.d_name &&
            d_latency == other.d_latency && d_source == other.d_source &&
            d_nameServer == other.d_nameServer &&
            d_attemptsRemaining == other.d_attemptsRemaining &&
            d_error == other.d_error);
}

bool ConnectContext::less(const ConnectContext& other) const
{
    if (d_endpoint < other.d_endpoint) {
        return true;
    }

    if (other.d_endpoint < d_endpoint) {
        return false;
    }

    if (d_name < other.d_name) {
        return true;
    }

    if (other.d_name < d_name) {
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

    if (d_attemptsRemaining < other.d_attemptsRemaining) {
        return true;
    }

    if (other.d_attemptsRemaining < d_attemptsRemaining) {
        return false;
    }

    return d_error < other.d_error;
}

bsl::ostream& ConnectContext::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_endpoint.isUndefined()) {
        printer.printAttribute("endpoint", d_endpoint);
    }

    if (!d_name.isNull()) {
        printer.printAttribute("name", d_name);
    }

    if (!d_latency.isNull()) {
        printer.printAttribute("latency", d_latency);
    }

    if (!d_source.isNull()) {
        printer.printAttribute("source", d_source);
    }

    if (!d_nameServer.isNull()) {
        printer.printAttribute("nameServer", d_nameServer);
    }

    if (d_error) {
        printer.printAttribute("attemptsRemaining", d_attemptsRemaining);
        printer.printAttribute("error", d_error);
    }

    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
