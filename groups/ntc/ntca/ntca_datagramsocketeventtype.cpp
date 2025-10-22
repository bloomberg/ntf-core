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

#include <ntca_datagramsocketeventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_datagramsocketeventtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int DatagramSocketEventType::fromInt(DatagramSocketEventType::Value* result,
                                     int                             number)
{
    switch (number) {
    case DatagramSocketEventType::e_UNDEFINED:
    case DatagramSocketEventType::e_CONNECT:
    case DatagramSocketEventType::e_READ_QUEUE:
    case DatagramSocketEventType::e_WRITE_QUEUE:
    case DatagramSocketEventType::e_DOWNGRADE:
    case DatagramSocketEventType::e_SHUTDOWN:
    case DatagramSocketEventType::e_ERROR:
        *result = static_cast<DatagramSocketEventType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int DatagramSocketEventType::fromString(DatagramSocketEventType::Value* result,
                                        const bslstl::StringRef&        string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CONNECT")) {
        *result = e_CONNECT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "READ_QUEUE")) {
        *result = e_READ_QUEUE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "WRITE_QUEUE")) {
        *result = e_WRITE_QUEUE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DOWNGRADE")) {
        *result = e_DOWNGRADE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SHUTDOWN")) {
        *result = e_SHUTDOWN;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ERROR")) {
        *result = e_ERROR;
        return 0;
    }

    return -1;
}

const char* DatagramSocketEventType::toString(
    DatagramSocketEventType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_CONNECT: {
        return "CONNECT";
    } break;
    case e_READ_QUEUE: {
        return "READ_QUEUE";
    } break;
    case e_WRITE_QUEUE: {
        return "WRITE_QUEUE";
    } break;
    case e_DOWNGRADE: {
        return "DOWNGRADE";
    } break;
    case e_SHUTDOWN: {
        return "SHUTDOWN";
    } break;
    case e_ERROR: {
        return "ERROR";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& DatagramSocketEventType::print(
    bsl::ostream&                  datagram,
    DatagramSocketEventType::Value value)
{
    return datagram << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                  datagram,
                         DatagramSocketEventType::Value rhs)
{
    return DatagramSocketEventType::print(datagram, rhs);
}

}  // close package namespace
}  // close enterprise namespace
