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

#include <ntsa_socketoptiontype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_socketoptiontype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int SocketOptionType::fromInt(SocketOptionType::Value* result, int number)
{
    switch (number) {
    case SocketOptionType::e_UNDEFINED:
    case SocketOptionType::e_REUSE_ADDRESS:
    case SocketOptionType::e_KEEP_ALIVE:
    case SocketOptionType::e_CORK:
    case SocketOptionType::e_DELAY_TRANSMISSION:
    case SocketOptionType::e_DELAY_ACKNOWLEDGEMENT:
    case SocketOptionType::e_SEND_BUFFER_SIZE:
    case SocketOptionType::e_SEND_BUFFER_LOW_WATERMARK:
    case SocketOptionType::e_RECEIVE_BUFFER_SIZE:
    case SocketOptionType::e_RECEIVE_BUFFER_LOW_WATERMARK:
    case SocketOptionType::e_DEBUG:
    case SocketOptionType::e_LINGER:
    case SocketOptionType::e_BROADCAST:
    case SocketOptionType::e_BYPASS_ROUTING:
    case SocketOptionType::e_INLINE_OUT_OF_BAND_DATA:
    case SocketOptionType::e_RX_TIMESTAMPING:
    case SocketOptionType::e_TX_TIMESTAMPING:
        *result = static_cast<SocketOptionType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int SocketOptionType::fromString(SocketOptionType::Value* result,
                                 const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "REUSE_ADDRESS")) {
        *result = e_REUSE_ADDRESS;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "KEEP_ALIVE")) {
        *result = e_KEEP_ALIVE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CORK")) {
        *result = e_CORK;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DELAY_TRANSMISSION")) {
        *result = e_DELAY_TRANSMISSION;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DELAY_ACKNOWLEDGEMENT")) {
        *result = e_DELAY_ACKNOWLEDGEMENT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SEND_BUFFER_SIZE")) {
        *result = e_SEND_BUFFER_SIZE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SEND_BUFFER_LOW_WATERMARK")) {
        *result = e_SEND_BUFFER_LOW_WATERMARK;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RECEIVE_BUFFER_SIZE")) {
        *result = e_RECEIVE_BUFFER_SIZE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RECEIVE_BUFFER_LOW_WATERMARK"))
    {
        *result = e_RECEIVE_BUFFER_LOW_WATERMARK;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DEBUG")) {
        *result = e_DEBUG;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LINGER")) {
        *result = e_LINGER;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BROADCAST")) {
        *result = e_BROADCAST;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BYPASS_ROUTING")) {
        *result = e_BYPASS_ROUTING;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "INLINE_OUT_OF_BAND_DATA")) {
        *result = e_INLINE_OUT_OF_BAND_DATA;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RX_TIMESTAMPING")) {
        *result = e_RX_TIMESTAMPING;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "TX_TIMESTAMPING")) {
        *result = e_TX_TIMESTAMPING;
        return 0;
    }

    return -1;
}

const char* SocketOptionType::toString(SocketOptionType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_REUSE_ADDRESS: {
        return "REUSE_ADDRESS";
    } break;
    case e_KEEP_ALIVE: {
        return "KEEP_ALIVE";
    } break;
    case e_CORK: {
        return "CORK";
    } break;
    case e_DELAY_TRANSMISSION: {
        return "DELAY_TRANSMISSION";
    } break;
    case e_DELAY_ACKNOWLEDGEMENT: {
        return "DELAY_ACKNOWLEDGEMENT";
    } break;
    case e_SEND_BUFFER_SIZE: {
        return "SEND_BUFFER_SIZE";
    } break;
    case e_SEND_BUFFER_LOW_WATERMARK: {
        return "SEND_BUFFER_LOW_WATERMARK";
    } break;
    case e_RECEIVE_BUFFER_SIZE: {
        return "RECEIVE_BUFFER_SIZE";
    } break;
    case e_RECEIVE_BUFFER_LOW_WATERMARK: {
        return "RECEIVE_BUFFER_LOW_WATERMARK";
    } break;
    case e_DEBUG: {
        return "DEBUG";
    } break;
    case e_LINGER: {
        return "LINGER";
    } break;
    case e_BROADCAST: {
        return "BROADCAST";
    } break;
    case e_BYPASS_ROUTING: {
        return "BYPASS_ROUTING";
    } break;
    case e_INLINE_OUT_OF_BAND_DATA: {
        return "INLINE_OUT_OF_BAND_DATA";
    } break;
    case e_RX_TIMESTAMPING: {
        return "RX_TIMESTAMPING";
    } break;
    case e_TX_TIMESTAMPING: {
        return "TX_TIMESTAMPING";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& SocketOptionType::print(bsl::ostream&           stream,
                                      SocketOptionType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, SocketOptionType::Value rhs)
{
    return SocketOptionType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
