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

#include <ntca_readqueuecontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_readqueuecontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool ReadQueueContext::equals(const ReadQueueContext& other) const
{
    return (d_size == other.d_size && d_lowWatermark == other.d_lowWatermark &&
            d_highWatermark == other.d_highWatermark);
}

bool ReadQueueContext::less(const ReadQueueContext& other) const
{
    if (d_size < other.d_size) {
        return true;
    }

    if (d_size > other.d_size) {
        return false;
    }

    if (d_lowWatermark < other.d_lowWatermark) {
        return true;
    }

    if (d_lowWatermark > other.d_lowWatermark) {
        return false;
    }

    return d_highWatermark < other.d_highWatermark;
}

bsl::ostream& ReadQueueContext::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("size", d_size);
    printer.printAttribute("lowWatermark", d_lowWatermark);
    printer.printAttribute("highWatermark", d_highWatermark);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
