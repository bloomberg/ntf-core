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

#include <ntca_compressiongoal.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_compressiongoal_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int CompressionGoal::fromInt(CompressionGoal::Value* result, int number)
{
    switch (number) {
    case CompressionGoal::e_UNDEFINED:
    case CompressionGoal::e_BEST_SIZE:
    case CompressionGoal::e_BETTER_SIZE:
    case CompressionGoal::e_BALANCED:
    case CompressionGoal::e_BETTER_SPEED:
    case CompressionGoal::e_BEST_SPEED:
        *result = static_cast<CompressionGoal::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int CompressionGoal::fromString(CompressionGoal::Value*  result,
                                const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BEST_SIZE")) {
        *result = e_BEST_SIZE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BETTER_SIZE")) {
        *result = e_BETTER_SIZE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BALANCED")) {
        *result = e_BALANCED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BETTER_SPEED")) {
        *result = e_BETTER_SPEED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BEST_SPEED")) {
        *result = e_BEST_SPEED;
        return 0;
    }

    return -1;
}

const char* CompressionGoal::toString(CompressionGoal::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_BEST_SIZE: {
        return "BEST_SIZE";
    } break;
    case e_BETTER_SIZE: {
        return "BETTER_SIZE";
    } break;
    case e_BALANCED: {
        return "BALANCED";
    } break;
    case e_BETTER_SPEED: {
        return "BETTER_SPEED";
    } break;
    case e_BEST_SPEED: {
        return "BEST_SPEED";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& CompressionGoal::print(bsl::ostream&          stream,
                                     CompressionGoal::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, CompressionGoal::Value rhs)
{
    return CompressionGoal::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
