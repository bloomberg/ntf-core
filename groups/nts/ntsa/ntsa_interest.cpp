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

#include <ntsa_interest.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_interest_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntsa {

int InterestType::fromInt(InterestType::Value* result, int number)
{
    switch (number) {
    case InterestType::e_READABLE:
    case InterestType::e_WRITABLE:
        *result = static_cast<InterestType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int InterestType::fromString(InterestType::Value*     result,
                             const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "READABLE")) {
        *result = e_READABLE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "WRITABLE")) {
        *result = e_WRITABLE;
        return 0;
    }

    return -1;
}

const char* InterestType::toString(InterestType::Value value)
{
    switch (value) {
    case e_READABLE:
        return "READABLE";
    case e_WRITABLE:
        return "WRITABLE";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& InterestType::print(bsl::ostream&       stream,
                                  InterestType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, InterestType::Value rhs)
{
    return InterestType::print(stream, rhs);
}

bool Interest::equals(const Interest& other) const
{
    return (d_handle == other.d_handle && d_state == other.d_state);
}

bool Interest::less(const Interest& other) const
{
    if (d_handle < other.d_handle) {
        return true;
    }

    if (other.d_handle < d_handle) {
        return false;
    }

    return d_state < other.d_state;
}

bsl::ostream& Interest::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_handle != ntsa::k_INVALID_HANDLE) {
        printer.printAttribute("handle", d_handle);
    }
    else {
        printer.printAttribute("handle", "INVALID");
    }

    if (d_state != 0) {
        bsl::string stateDescription;
        bsl::size_t numFlags = 0;

        if ((d_state & (1U << InterestType::e_READABLE)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("READABLE", 8);
            ++numFlags;
        }

        if ((d_state & (1U << InterestType::e_WRITABLE)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("WRITABLE", 8);
            ++numFlags;
        }

        printer.printAttribute("state", stateDescription);
    }

    printer.end();
    return stream;
}

bool InterestSet::equals(const InterestSet& other) const
{
    return (d_vector == other.d_vector);
}

bsl::ostream& InterestSet::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);

    printer.printValue(this->begin(), this->end());

    printer.end(true);
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
