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

#include <ntsa_sendcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_sendcontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool SendContext::equals(const SendContext& other) const
{
    return d_bytesSendable == other.d_bytesSendable &&
           d_bytesSent == other.d_bytesSent &&
           d_buffersSendable == other.d_buffersSendable &&
           d_buffersSent == other.d_buffersSent &&
           d_messagesSendable == other.d_messagesSendable &&
           d_messagesSent == other.d_messagesSent && 
           d_zeroCopy == other.d_zeroCopy;
}

bool SendContext::less(const SendContext& other) const
{
    if (d_bytesSendable < other.d_bytesSendable) {
        return true;
    }

    if (other.d_bytesSendable < d_bytesSendable) {
        return false;
    }

    if (d_bytesSent < other.d_bytesSent) {
        return true;
    }

    if (other.d_bytesSent < d_bytesSent) {
        return false;
    }

    if (d_buffersSendable < other.d_buffersSendable) {
        return true;
    }

    if (other.d_buffersSendable < d_buffersSendable) {
        return false;
    }

    if (d_buffersSent < other.d_buffersSent) {
        return true;
    }

    if (other.d_buffersSent < d_buffersSent) {
        return false;
    }

    if (d_messagesSendable < other.d_messagesSendable) {
        return true;
    }

    if (other.d_messagesSendable < d_messagesSendable) {
        return false;
    }

    if (d_messagesSent < other.d_messagesSent) {
        return true;
    }

    if (other.d_messagesSent < d_messagesSent) {
        return false;
    }

    return d_zeroCopy < other.d_zeroCopy;
}

bsl::ostream& SendContext::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("bytesSendable", d_bytesSendable);
    printer.printAttribute("bytesSent", d_bytesSent);
    printer.printAttribute("buffersSendable", d_buffersSendable);
    printer.printAttribute("buffersSent", d_buffersSent);
    printer.printAttribute("messagesSendable", d_messagesSendable);
    printer.printAttribute("messagesSent", d_messagesSent);
    printer.printAttribute("zeroCopy", d_zeroCopy);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
