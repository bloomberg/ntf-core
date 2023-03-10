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

#include <ntcs_reservation.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_reservation_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcs {

Reservation::Reservation(bsl::size_t maximum)
: d_current(0)
, d_maximum(maximum)
{
}

Reservation::~Reservation()
{
}

bool Reservation::acquire()
{
    while (true) {
        bsl::uint64_t old = d_current.load();
        if (old >= d_maximum) {
            return false;
        }

        bsl::uint64_t previous = d_current.testAndSwap(old, old + 1);
        if (previous == old) {
            break;
        }
    }

    return true;
}

void Reservation::release()
{
    while (true) {
        bsl::uint64_t old = d_current.load();
        if (old == 0) {
            return;
        }

        bsl::uint64_t previous = d_current.testAndSwap(old, old - 1);
        if (previous == old) {
            break;
        }
    }

    return;
}

bsl::size_t Reservation::current() const
{
    return static_cast<bsl::size_t>(d_current.load());
}

bsl::size_t Reservation::maximum() const
{
    return d_maximum;
}

}  // close package namespace
}  // close enterprise namespace
