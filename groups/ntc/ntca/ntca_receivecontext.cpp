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

#include <ntca_receivecontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_receivecontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool ReceiveContext::equals(const ReceiveContext& other) const
{
    return (d_endpoint == other.d_endpoint &&
            d_transport == other.d_transport && 
            d_compressionType == other.d_compressionType &&
            d_compressionRatio == other.d_compressionRatio &&
            d_foreignHandle == other.d_foreignHandle &&
            d_error == other.d_error);
}

bool ReceiveContext::less(const ReceiveContext& other) const
{
    if (d_endpoint < other.d_endpoint) {
        return true;
    }

    if (other.d_endpoint < d_endpoint) {
        return false;
    }

    if (d_transport < other.d_transport) {
        return true;
    }

    if (other.d_transport < d_transport) {
        return false;
    }

    if (d_compressionType < other.d_compressionType) {
        return true;
    }

    if (other.d_compressionType < d_compressionType) {
        return false;
    }

    if (d_compressionRatio < other.d_compressionRatio) {
        return true;
    }

    if (other.d_compressionRatio < d_compressionRatio) {
        return false;
    }

    if (d_foreignHandle < other.d_foreignHandle) {
        return true;
    }

    if (other.d_foreignHandle < d_foreignHandle) {
        return false;
    }

    return d_error < other.d_error;
}

bsl::ostream& ReceiveContext::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_endpoint.isNull()) {
        printer.printAttribute("endpoint", d_endpoint);
    }

    printer.printAttribute("transport", d_transport);

    if (d_compressionType.has_value()) {
        printer.printAttribute("compressionType", d_compressionType.value());
    }

    if (d_compressionRatio.has_value()) {

        bsl::ostringstream ss;
        ss << bsl::setprecision(2);
        ss << bsl::fixed;
        ss << d_compressionRatio.value() * 100;

        printer.printAttribute("compressionRatio", ss.str());
    }

    if (d_foreignHandle.has_value()) {
        printer.printAttribute("foreignHandle", d_foreignHandle.value());
    }

    if (d_error) {
        printer.printAttribute("error", d_error);
    }

    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
