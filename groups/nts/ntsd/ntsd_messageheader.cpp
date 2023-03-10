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

#include <ntsd_messageheader.h>

#include <bslim_printer.h>
#include <bslmf_assert.h>

namespace BloombergLP {
namespace ntsd {

BSLMF_ASSERT(sizeof(ntsd::MessageHeader) == 64);

bool MessageHeader::equals(const MessageHeader& other) const
{
    return d_magic == other.d_magic && d_version == other.d_version &&
           d_crc == other.d_crc && d_type == other.d_type &&
           d_machineId == other.d_machineId && d_userId == other.d_userId &&
           d_transactionId == other.d_transactionId &&
           d_sequenceNumber == other.d_sequenceNumber &&
           d_requestSize == other.d_requestSize &&
           d_responseSize == other.d_responseSize &&
           d_requestDelay == other.d_requestDelay &&
           d_responseDelay == other.d_responseDelay &&
           d_requestTimestamp == other.d_requestTimestamp &&
           d_responseTimestamp == other.d_responseTimestamp;
}

bool MessageHeader::less(const MessageHeader& other) const
{
    if (d_magic < other.d_magic) {
        return true;
    }

    if (d_magic > other.d_magic) {
        return false;
    }

    if (d_version < other.d_version) {
        return true;
    }

    if (d_version > other.d_version) {
        return false;
    }

    if (d_crc < other.d_crc) {
        return true;
    }

    if (d_crc > other.d_crc) {
        return false;
    }

    if (d_type < other.d_type) {
        return true;
    }

    if (d_type > other.d_type) {
        return false;
    }

    if (d_machineId < other.d_machineId) {
        return true;
    }

    if (d_machineId > other.d_machineId) {
        return false;
    }

    if (d_userId < other.d_userId) {
        return true;
    }

    if (d_userId > other.d_userId) {
        return false;
    }

    if (d_transactionId < other.d_transactionId) {
        return true;
    }

    if (d_transactionId > other.d_transactionId) {
        return false;
    }

    if (d_sequenceNumber < other.d_sequenceNumber) {
        return true;
    }

    if (d_sequenceNumber > other.d_sequenceNumber) {
        return false;
    }

    if (d_requestSize < other.d_requestSize) {
        return true;
    }

    if (d_requestSize > other.d_requestSize) {
        return false;
    }

    if (d_responseSize < other.d_responseSize) {
        return true;
    }

    if (d_responseSize > other.d_responseSize) {
        return false;
    }

    if (d_requestDelay < other.d_requestDelay) {
        return true;
    }

    if (d_requestDelay > other.d_requestDelay) {
        return false;
    }

    if (d_responseDelay < other.d_responseDelay) {
        return true;
    }

    if (d_responseDelay > other.d_responseDelay) {
        return false;
    }

    if (d_requestTimestamp < other.d_requestTimestamp) {
        return true;
    }

    if (d_requestTimestamp > other.d_requestTimestamp) {
        return false;
    }

    return d_responseTimestamp < other.d_responseTimestamp;
}

bsl::ostream& MessageHeader::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("magic", d_magic);
    printer.printAttribute("version", d_version);
    printer.printAttribute("crc", d_crc);
    printer.printAttribute("type", d_type);
    printer.printAttribute("machineId", d_machineId);
    printer.printAttribute("userId", d_userId);
    printer.printAttribute("transactionId", d_transactionId);
    printer.printAttribute("sequenceNumber", d_sequenceNumber);
    printer.printAttribute("requestSize", d_requestSize);
    printer.printAttribute("responseSize", d_responseSize);
    printer.printAttribute("requestDelay", d_requestDelay);
    printer.printAttribute("responseDelay", d_responseDelay);
    printer.printAttribute("requestTimestamp", d_requestTimestamp);
    printer.printAttribute("responseTimestamp", d_responseTimestamp);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
