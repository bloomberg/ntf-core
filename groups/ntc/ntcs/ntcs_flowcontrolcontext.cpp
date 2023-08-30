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

#include <ntcs_flowcontrolcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_flowcontrolcontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntcs {

bool FlowControlContext::equals(const FlowControlContext& other) const
{
    return (d_enableSend == other.d_enableSend &&
            d_enableReceive == other.d_enableReceive);
}

bool FlowControlContext::less(const FlowControlContext& other) const
{
    if (d_enableSend < other.d_enableSend) {
        return true;
    }

    if (other.d_enableSend < d_enableSend) {
        return false;
    }

    return d_enableReceive < other.d_enableReceive;
}

bsl::ostream& FlowControlContext::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("enableSend", d_enableSend);
    printer.printAttribute("enableReceive", d_enableReceive);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
