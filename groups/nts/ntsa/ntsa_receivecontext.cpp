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

#include <ntsa_receivecontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_receivecontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool ReceiveContext::equals(const ReceiveContext& other) const
{
    return d_endpoint == other.d_endpoint &&
           d_bytesReceivable == other.d_bytesReceivable &&
           d_bytesReceived == other.d_bytesReceived &&
           d_buffersReceivable == other.d_buffersReceivable &&
           d_buffersReceived == other.d_buffersReceived &&
           d_messagesReceivable == other.d_messagesReceivable &&
           d_messagesReceived == other.d_messagesReceived &&
           d_softwareTimestamp == other.d_softwareTimestamp &&
           d_hardwareTimestamp == other.d_hardwareTimestamp &&
           d_foreignHandle == other.d_foreignHandle;
}

bool ReceiveContext::less(const ReceiveContext& other) const
{
    if (d_endpoint < other.d_endpoint) {
        return true;
    }

    if (other.d_endpoint < d_endpoint) {
        return false;
    }

    if (d_bytesReceivable < other.d_bytesReceivable) {
        return true;
    }

    if (other.d_bytesReceivable < d_bytesReceivable) {
        return false;
    }

    if (d_bytesReceived < other.d_bytesReceived) {
        return true;
    }

    if (other.d_bytesReceived < d_bytesReceived) {
        return false;
    }

    if (d_buffersReceivable < other.d_buffersReceivable) {
        return true;
    }

    if (other.d_buffersReceivable < d_buffersReceivable) {
        return false;
    }

    if (d_buffersReceived < other.d_buffersReceived) {
        return true;
    }

    if (other.d_buffersReceived < d_buffersReceived) {
        return false;
    }

    if (d_messagesReceivable < other.d_messagesReceivable) {
        return true;
    }

    if (other.d_messagesReceivable < d_messagesReceivable) {
        return false;
    }

    if (d_messagesReceived < other.d_messagesReceived) {
        return true;
    }

    if (other.d_messagesReceived < d_messagesReceived) {
        return false;
    }

    if (d_softwareTimestamp < other.d_softwareTimestamp) {
        return true;
    }

    if (other.d_softwareTimestamp < d_softwareTimestamp) {
        return false;
    }

    if (d_hardwareTimestamp < other.d_hardwareTimestamp) {
        return true;
    }

    if (other.d_hardwareTimestamp < d_hardwareTimestamp) {
        return false;
    }

    return d_foreignHandle < other.d_foreignHandle;
}

bsl::ostream& ReceiveContext::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("endpoint", d_endpoint);
    printer.printAttribute("bytesReceivable", d_bytesReceivable);
    printer.printAttribute("bytesReceived", d_bytesReceived);
    printer.printAttribute("buffersReceivable", d_buffersReceivable);
    printer.printAttribute("buffersReceived", d_buffersReceived);
    printer.printAttribute("messagesReceivable", d_messagesReceivable);
    printer.printAttribute("messagesReceived", d_messagesReceived);
    printer.printAttribute("softwareTimestamp", d_softwareTimestamp);
    printer.printAttribute("hardwareTimestamp", d_hardwareTimestamp);
    printer.printAttribute("foreignHandle", d_foreignHandle);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
