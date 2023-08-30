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

#include <ntca_timeroptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_timeroptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool TimerOptions::equals(const TimerOptions& other) const
{
    return (d_handle == other.d_handle && d_id == other.d_id &&
            d_flags == other.d_flags && d_eventMask == other.d_eventMask);
}

bool TimerOptions::less(const TimerOptions& other) const
{
    if (reinterpret_cast<bsl::uintptr_t>(d_handle) <
        reinterpret_cast<bsl::uintptr_t>(other.d_handle))
    {
        return true;
    }

    if (reinterpret_cast<bsl::uintptr_t>(d_handle) >
        reinterpret_cast<bsl::uintptr_t>(other.d_handle))
    {
        return false;
    }

    if (d_id < other.d_id) {
        return true;
    }

    if (other.d_id < d_id) {
        return false;
    }

    if (d_flags < other.d_flags) {
        return true;
    }

    if (other.d_flags < d_flags) {
        return false;
    }

    return d_eventMask < other.d_eventMask;
}

bsl::ostream& TimerOptions::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("handle",
                           reinterpret_cast<bsl::uintptr_t>(this->handle()));
    printer.printAttribute("id", d_id);
    printer.printAttribute("oneShot", this->oneShot());
    printer.printAttribute("drift", this->drift());
    printer.printAttribute("wantDeadline",
                           this->wantEvent(ntca::TimerEventType::e_DEADLINE));
    printer.printAttribute("wantCanceled",
                           this->wantEvent(ntca::TimerEventType::e_CANCELED));
    printer.printAttribute("wantClosed",
                           this->wantEvent(ntca::TimerEventType::e_CLOSED));
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
