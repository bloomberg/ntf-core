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

#include <ntca_readqueueeventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_readqueueeventtype_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntca {

int ReadQueueEventType::fromInt(ReadQueueEventType::Value* result, int number)
{
    switch (number) {
    case ReadQueueEventType::e_FLOW_CONTROL_RELAXED:
    case ReadQueueEventType::e_FLOW_CONTROL_APPLIED:
    case ReadQueueEventType::e_LOW_WATERMARK:
    case ReadQueueEventType::e_HIGH_WATERMARK:
    case ReadQueueEventType::e_DISCARDED:
    case ReadQueueEventType::e_RATE_LIMIT_APPLIED:
    case ReadQueueEventType::e_RATE_LIMIT_RELAXED:
        *result = static_cast<ReadQueueEventType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ReadQueueEventType::fromString(ReadQueueEventType::Value* result,
                                   const bslstl::StringRef&   string)
{
    if (bdlb::String::areEqualCaseless(string, "FLOW_CONTROL_RELAXED")) {
        *result = e_FLOW_CONTROL_RELAXED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "FLOW_CONTROL_APPLIED")) {
        *result = e_FLOW_CONTROL_APPLIED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LOW_WATERMARK")) {
        *result = e_LOW_WATERMARK;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "HIGH_WATERMARK")) {
        *result = e_HIGH_WATERMARK;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DISCARDED")) {
        *result = e_DISCARDED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RATE_LIMIT_APPLIED")) {
        *result = e_RATE_LIMIT_APPLIED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "RATE_LIMIT_RELAXED")) {
        *result = e_RATE_LIMIT_RELAXED;
        return 0;
    }
    return -1;
}

const char* ReadQueueEventType::toString(ReadQueueEventType::Value value)
{
    switch (value) {
    case e_FLOW_CONTROL_RELAXED: {
        return "FLOW_CONTROL_RELAXED";
    }
    case e_FLOW_CONTROL_APPLIED: {
        return "FLOW_CONTROL_APPLIED";
    }
    case e_LOW_WATERMARK: {
        return "LOW_WATERMARK";
    }
    case e_HIGH_WATERMARK: {
        return "HIGH_WATERMARK";
    }
    case e_DISCARDED: {
        return "DISCARDED";
    }
    case e_RATE_LIMIT_APPLIED: {
        return "RATE_LIMIT_APPLIED";
    }
    case e_RATE_LIMIT_RELAXED: {
        return "RATE_LIMIT_RELAXED";
    }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ReadQueueEventType::print(bsl::ostream&             stream,
                                        ReadQueueEventType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ReadQueueEventType::Value rhs)
{
    return ReadQueueEventType::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
