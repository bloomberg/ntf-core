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

#include <ntsa_zerocopy.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_zerocopy_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

int ZeroCopyType::fromInt(ZeroCopyType::Value* result, int number)
{
    switch (number) {
    case ZeroCopyType::e_AVOIDED:
    case ZeroCopyType::e_DEFERRED:
        *result = static_cast<ZeroCopyType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int ZeroCopyType::fromString(ZeroCopyType::Value*     result,
                             const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "AVOIDED")) {
        *result = e_AVOIDED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DEFERRED")) {
        *result = e_DEFERRED;
        return 0;
    }

    return -1;
}

const char* ZeroCopyType::toString(ZeroCopyType::Value value)
{
    switch (value) {
    case e_AVOIDED: {
        return "AVOIDED";
    } break;
    case e_DEFERRED: {
        return "DEFERRED";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& ZeroCopyType::print(bsl::ostream&       stream,
                                  ZeroCopyType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, ZeroCopyType::Value rhs)
{
    return ZeroCopyType::print(stream, rhs);
}

bool ZeroCopy::equals(const ZeroCopy& other) const
{
    return (d_from == other.d_from && d_thru == other.d_thru &&
            d_type == other.d_type);
}

bool ZeroCopy::less(const ZeroCopy& other) const
{
    if (d_from < other.d_from) {
        return true;
    }

    if (other.d_from < d_from) {
        return false;
    }

    if (d_thru < other.d_thru) {
        return true;
    }

    if (other.d_thru < d_thru) {
        return false;
    }

    return d_type < other.d_type;
}

bsl::ostream& ZeroCopy::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("from", d_from);
    printer.printAttribute("thru", d_thru);
    printer.printAttribute("type", d_type);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
