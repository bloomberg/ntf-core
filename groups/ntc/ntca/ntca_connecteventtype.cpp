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

#include <ntca_connecteventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_connecteventtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ConnectEventType::fromInt(ConnectEventType::Value* result, int number)
{
    switch (number) {
    case ConnectEventType::e_COMPLETE:
    case ConnectEventType::e_ERROR:
        *result = static_cast<ConnectEventType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ConnectEventType::fromString(ConnectEventType::Value* result,
                                 const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "COMPLETE")) {
        *result = e_COMPLETE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ERROR")) {
        *result = e_ERROR;
        return 0;
    }

    return -1;
}

const char* ConnectEventType::toString(ConnectEventType::Value value)
{
    switch (value) {
    case e_COMPLETE: {
        return "COMPLETE";
    } break;
    case e_ERROR: {
        return "ERROR";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ConnectEventType::print(bsl::ostream&           stream,
                                      ConnectEventType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ConnectEventType::Value rhs)
{
    return ConnectEventType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
