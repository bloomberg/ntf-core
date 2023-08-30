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

#include <ntcs_shutdowncontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_shutdowncontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntcs {

bool ShutdownContext::equals(const ShutdownContext& other) const
{
    return (d_shutdownOrigin == other.d_shutdownOrigin &&
            d_shutdownInitiated == other.d_shutdownInitiated &&
            d_shutdownSend == other.d_shutdownSend &&
            d_shutdownReceive == other.d_shutdownReceive &&
            d_shutdownCompleted == other.d_shutdownCompleted);
}

bool ShutdownContext::less(const ShutdownContext& other) const
{
    if (d_shutdownOrigin < other.d_shutdownOrigin) {
        return true;
    }

    if (other.d_shutdownOrigin < d_shutdownOrigin) {
        return false;
    }

    if (d_shutdownInitiated < other.d_shutdownInitiated) {
        return true;
    }

    if (other.d_shutdownInitiated < d_shutdownInitiated) {
        return false;
    }

    if (d_shutdownSend < other.d_shutdownSend) {
        return true;
    }

    if (other.d_shutdownSend < d_shutdownSend) {
        return false;
    }

    if (d_shutdownReceive < other.d_shutdownReceive) {
        return true;
    }

    if (other.d_shutdownReceive < d_shutdownReceive) {
        return false;
    }

    return d_shutdownCompleted < other.d_shutdownCompleted;
}

bsl::ostream& ShutdownContext::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("shutdownOrigin", d_shutdownOrigin);
    printer.printAttribute("shutdownInitiated", d_shutdownInitiated);
    printer.printAttribute("shutdownSend", d_shutdownSend);
    printer.printAttribute("shutdownReceive", d_shutdownReceive);
    printer.printAttribute("shutdownCompleted", d_shutdownCompleted);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
