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

#include <ntca_inflatecontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_inflatecontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool InflateContext::equals(const InflateContext& other) const
{
    return d_compressionType == other.d_compressionType &&
           d_bytesRead == other.d_bytesRead &&
           d_bytesWritten == other.d_bytesWritten &&
           d_checksum == other.d_checksum;
}

bool InflateContext::less(const InflateContext& other) const
{
    if (d_compressionType < other.d_compressionType) {
        return true;
    }

    if (other.d_compressionType < d_compressionType) {
        return false;
    }

    if (d_bytesRead < other.d_bytesRead) {
        return true;
    }

    if (other.d_bytesRead < d_bytesRead) {
        return false;
    }

    if (d_bytesWritten < other.d_bytesWritten) {
        return true;
    }

    if (other.d_bytesWritten < d_bytesWritten) {
        return false;
    }

    return d_checksum < other.d_checksum;
}

bsl::ostream& InflateContext::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("compressionType", d_compressionType);
    printer.printAttribute("bytesRead", d_bytesRead);
    printer.printAttribute("bytesWritten", d_bytesWritten);
    if (d_checksum.has_value()) {
        printer.printAttribute("checksum", d_checksum.value());
    }
    printer.end();
    return stream;
}

}  // close namespace ntca
}  // close namespace BloombergLP
