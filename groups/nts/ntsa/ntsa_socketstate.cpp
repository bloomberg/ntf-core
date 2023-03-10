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

#include <ntsa_socketstate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_socketstate_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int SocketState::fromInt(SocketState::Value* result, int number)
{
    switch (number) {
    case SocketState::e_UNDEFINED:
    case SocketState::e_ESTABLISHED:
    case SocketState::e_SYN_SENT:
    case SocketState::e_SYN_RECV:
    case SocketState::e_FIN_WAIT1:
    case SocketState::e_FIN_WAIT2:
    case SocketState::e_TIME_WAIT:
    case SocketState::e_CLOSED:
    case SocketState::e_CLOSE_WAIT:
    case SocketState::e_LAST_ACK:
    case SocketState::e_LISTEN:
    case SocketState::e_CLOSING:
        *result = static_cast<SocketState::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int SocketState::fromString(SocketState::Value*      result,
                            const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ESTABLISHED")) {
        *result = e_ESTABLISHED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SYN_SENT")) {
        *result = e_SYN_SENT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SYN_RECV")) {
        *result = e_SYN_RECV;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "FIN_WAIT1")) {
        *result = e_FIN_WAIT1;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "FIN_WAIT2")) {
        *result = e_FIN_WAIT2;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TIME_WAIT")) {
        *result = e_TIME_WAIT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CLOSED")) {
        *result = e_CLOSED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CLOSE_WAIT")) {
        *result = e_CLOSE_WAIT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LAST_ACK")) {
        *result = e_LAST_ACK;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LISTEN")) {
        *result = e_LISTEN;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CLOSING")) {
        *result = e_CLOSING;
        return 0;
    }
    return -1;
}

const char* SocketState::toString(SocketState::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_ESTABLISHED: {
        return "ESTABLISHED";
    } break;
    case e_SYN_SENT: {
        return "SYN_SENT";
    } break;
    case e_SYN_RECV: {
        return "SYN_RECV";
    } break;
    case e_FIN_WAIT1: {
        return "FIN_WAIT1";
    } break;
    case e_FIN_WAIT2: {
        return "FIN_WAIT2";
    } break;
    case e_TIME_WAIT: {
        return "TIME_WAIT";
    } break;
    case e_CLOSED: {
        return "CLOSED";
    } break;
    case e_CLOSE_WAIT: {
        return "CLOSE_WAIT";
    } break;
    case e_LAST_ACK: {
        return "LAST_ACK";
    } break;
    case e_LISTEN: {
        return "LISTEN";
    } break;
    case e_CLOSING: {
        return "CLOSING";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& SocketState::print(bsl::ostream&      stream,
                                 SocketState::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, SocketState::Value rhs)
{
    return SocketState::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
