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

#include <ntca_timercontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_timercontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool TimerContext::equals(const TimerContext& other) const
{
    return (d_now == other.d_now && d_deadline == other.d_deadline &&
            d_drift == other.d_drift && d_error == other.d_error);
}

bool TimerContext::less(const TimerContext& other) const
{
    if (d_now < other.d_now) {
        return true;
    }

    if (other.d_now < d_now) {
        return false;
    }

    if (d_deadline < other.d_deadline) {
        return true;
    }

    if (other.d_deadline < d_deadline) {
        return false;
    }

    if (d_drift < other.d_drift) {
        return true;
    }

    if (other.d_drift < d_drift) {
        return false;
    }

    return d_error < other.d_error;
}

bsl::ostream& TimerContext::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("now", d_now);
    printer.printAttribute("deadline", d_deadline);
    printer.printAttribute("drift", d_drift);
    printer.printAttribute("error", d_error);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
