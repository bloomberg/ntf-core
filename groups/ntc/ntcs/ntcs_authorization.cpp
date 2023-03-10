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

#include <ntcs_authorization.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_authorization_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcs {

Authorization::Authorization()
: d_count(0)
, d_limit(bsl::numeric_limits<CountType>::max())
{
}

Authorization::Authorization(CountType limit)
: d_count(0)
, d_limit(limit)
{
}

Authorization::~Authorization()
{
}

void Authorization::reset()
{
    d_count = 0;
}

ntsa::Error Authorization::acquire()
{
    while (true) {
        CountType c = d_count.load();

        if (c < 0) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        if (c == d_limit) {
            return ntsa::Error(ntsa::Error::e_LIMIT);
        }

        CountType n = c + 1;

        CountType p = d_count.testAndSwap(c, n);
        if (p != c) {
            continue;
        }

        break;
    }

    return ntsa::Error();
}

ntsa::Error Authorization::release()
{
    while (true) {
        CountType c = d_count.load();

        if (c < 0) {
            return ntsa::Error(ntsa::Error::e_CANCELLED);
        }

        if (c == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        CountType n = c - 1;

        CountType p = d_count.testAndSwap(c, n);
        if (p != c) {
            continue;
        }

        break;
    }

    return ntsa::Error();
}

bool Authorization::abort()
{
    while (true) {
        CountType c = d_count.load();

        if (c < 0) {
            return true;
        }

        if (c > 0) {
            return false;
        }

        CountType n = -1;

        CountType p = d_count.testAndSwap(c, n);
        if (p != c) {
            continue;
        }

        break;
    }

    return true;
}

Authorization::CountType Authorization::count() const
{
    return d_count.load();
}

bool Authorization::canceled() const
{
    CountType c = d_count.load();
    return c < 0;
}

}  // close package namespace
}  // close enterprise namespace
