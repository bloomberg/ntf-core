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

#include <ntca_listenersocketeventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_listenersocketeventtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ListenerSocketEventType::fromInt(ListenerSocketEventType::Value* result,
                                     int                             number)
{
    switch (number) {
    case ListenerSocketEventType::e_UNDEFINED:
    case ListenerSocketEventType::e_ACCEPT_QUEUE:
    case ListenerSocketEventType::e_SHUTDOWN:
    case ListenerSocketEventType::e_ERROR:
        *result = static_cast<ListenerSocketEventType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ListenerSocketEventType::fromString(ListenerSocketEventType::Value* result,
                                        const bslstl::StringRef&        string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ACCEPT_QUEUE")) {
        *result = e_ACCEPT_QUEUE;
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

const char* ListenerSocketEventType::toString(
    ListenerSocketEventType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_ACCEPT_QUEUE: {
        return "ACCEPT_QUEUE";
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

bsl::ostream& ListenerSocketEventType::print(
    bsl::ostream&                  listener,
    ListenerSocketEventType::Value value)
{
    return listener << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                  listener,
                         ListenerSocketEventType::Value rhs)
{
    return ListenerSocketEventType::print(listener, rhs);
}

}  // close package namespace
}  // close enterprise namespace
