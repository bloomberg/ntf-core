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

#include <ntsa_ipv6scopeid.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6scopeid_cpp, "$Id$ $CSID$")

#include <bdlb_chartype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

bsl::size_t Ipv6ScopeIdUtil::format(char*             destination,
                                    bsl::size_t       capacity,
                                    ntsa::Ipv6ScopeId scopeId)
{
    bsl::uint16_t value = static_cast<bsl::uint16_t>(scopeId);
    bsl::size_t   size  = 0;

    char        c;
    bsl::size_t i;
    bsl::size_t j;

    do {
        if (size == capacity) {
            return size;
        }

        int remainder = value % 10;
        if (remainder > 9) {
            destination[size] = (char)((remainder - 10) + 'a');
        }
        else {
            destination[size] = (char)(remainder + '0');
        }

        size  = size + 1;
        value = value / 10;
    } while (value != 0);

    if (size < capacity) {
        destination[size] = 0;
    }

    for (i = 0, j = size - 1; i < j; ++i, --j) {
        c              = destination[i];
        destination[i] = destination[j];
        destination[j] = (char)(c);
    }

    return size;
}

bool Ipv6ScopeIdUtil::parse(ntsa::Ipv6ScopeId* result,
                            const char*        source,
                            bsl::size_t        size)
{
    const char* p = source;
    const char* e = source + size + 1;

    char c;

    if (*p == 0) {
        return false;
    }

    c = *p++;
    while (bdlb::CharType::isSpace(c)) {
        c = *p++;
    }

    if (c == '-') {
        return false;
    }

    if (c == '+') {
        c = *p++;
    }

    bsl::uint64_t value = 0;
    while (p != e) {
        if (!bdlb::CharType::isDigit(c)) {
            return false;
        }

        value = value * 10 + (c - '0');
        c     = *p++;
    }

    if (value > USHRT_MAX) {
        return false;
    }

    *result = static_cast<bsl::uint16_t>(value);
    return true;
}

}  // close package namespace
}  // close enterprise namespace
