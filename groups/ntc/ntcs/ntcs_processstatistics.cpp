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

#include <ntcs_processstatistics.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_processstatistics_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntcs {

bsl::ostream& ProcessStatistics::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_cpuTimeUser.isNull()) {
        printer.printAttribute("cpuTimeUser", d_cpuTimeUser.value());
    }

    if (!d_cpuTimeSystem.isNull()) {
        printer.printAttribute("cpuTimeSystem", d_cpuTimeSystem.value());
    }

    if (!d_memoryResident.isNull()) {
        printer.printAttribute("memoryResident", d_memoryResident.value());
    }

    if (!d_memoryAddressSpace.isNull()) {
        printer.printAttribute("memoryAddressSpace",
                               d_memoryAddressSpace.value());
    }

    if (!d_contextSwitchesUser.isNull()) {
        printer.printAttribute("contextSwitchesUser",
                               d_contextSwitchesUser.value());
    }

    if (!d_contextSwitchesSystem.isNull()) {
        printer.printAttribute("contextSwitchesSystem",
                               d_contextSwitchesSystem.value());
    }

    if (!d_pageFaultsMajor.isNull()) {
        printer.printAttribute("pageFaultsMajor", d_pageFaultsMajor.value());
    }

    if (!d_pageFaultsMinor.isNull()) {
        printer.printAttribute("pageFaultsMinor", d_pageFaultsMinor.value());
    }

    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
