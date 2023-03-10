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

#include <ntsa_socketinfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_socketinfo_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool SocketInfo::equals(const SocketInfo& other) const
{
    return d_transport == other.d_transport &&
           d_sourceEndpoint == other.d_sourceEndpoint &&
           d_remoteEndpoint == other.d_remoteEndpoint &&
           d_state == other.d_state &&
           d_sendQueueSize == other.d_sendQueueSize &&
           d_receiveQueueSize == other.d_receiveQueueSize &&
           d_userId == other.d_userId;
}

bool SocketInfo::less(const SocketInfo& other) const
{
    if (d_transport < other.d_transport) {
        return true;
    }

    if (other.d_transport < d_transport) {
        return false;
    }

    if (d_sourceEndpoint < other.d_sourceEndpoint) {
        return true;
    }

    if (other.d_sourceEndpoint < d_sourceEndpoint) {
        return false;
    }

    if (d_remoteEndpoint < other.d_remoteEndpoint) {
        return true;
    }

    if (other.d_remoteEndpoint < d_remoteEndpoint) {
        return false;
    }

    if (d_state < other.d_state) {
        return true;
    }

    if (other.d_state < d_state) {
        return false;
    }

    if (d_sendQueueSize < other.d_sendQueueSize) {
        return true;
    }

    if (other.d_sendQueueSize < d_sendQueueSize) {
        return false;
    }

    if (d_receiveQueueSize < other.d_receiveQueueSize) {
        return true;
    }

    if (other.d_receiveQueueSize < d_receiveQueueSize) {
        return false;
    }

    return d_userId < other.d_userId;
}

bsl::ostream& SocketInfo::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("transport", d_transport);
    printer.printAttribute("sourceEndpoint", d_sourceEndpoint);
    printer.printAttribute("remoteEndpoint", d_remoteEndpoint);
    printer.printAttribute("state", d_state);
    printer.printAttribute("sendQueueSize", d_sendQueueSize);
    printer.printAttribute("receiveQueueSize", d_receiveQueueSize);
    printer.printAttribute("userId", d_userId);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
