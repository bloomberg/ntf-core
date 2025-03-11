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

#include <ntca_sendcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_sendcontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool SendContext::equals(const SendContext& other) const
{
    return (d_token            == other.d_token &&
            d_compressionType  == other.d_compressionType &&
            d_compressionRatio == other.d_compressionRatio && 
            d_error            == other.d_error);
}

bool SendContext::less(const SendContext& other) const
{
    if (d_token < other.d_token) {
        return true;
    }

    if (other.d_token < d_token) {
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

    return d_error < other.d_error;
}

bsl::ostream& SendContext::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_token.has_value()) {
        printer.printAttribute("token", d_token.value());
    }

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

    if (d_error) {
        printer.printAttribute("error", d_error);
    }

    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
