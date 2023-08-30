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

#include <ntca_shutdowncontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_shutdowncontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool ShutdownContext::equals(const ShutdownContext& other) const
{
    return (d_origin == other.d_origin && d_send == other.d_send &&
            d_receive == other.d_receive);
}

bool ShutdownContext::less(const ShutdownContext& other) const
{
    if (d_origin < other.d_origin) {
        return true;
    }

    if (other.d_origin < d_origin) {
        return false;
    }

    if (d_send < other.d_send) {
        return true;
    }

    if (other.d_send < d_send) {
        return false;
    }

    return d_receive < other.d_receive;
}

bsl::ostream& ShutdownContext::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("origin", d_origin);
    printer.printAttribute("send", d_send);
    printer.printAttribute("receive", d_receive);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
