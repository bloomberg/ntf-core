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

#include <ntca_reactorcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_reactorcontext_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool ReactorContext::equals(const ReactorContext& other) const
{
    return (d_bytesReadable == other.d_bytesReadable &&
            d_bytesWritable == other.d_bytesWritable &&
            d_error == other.d_error);
}

bool ReactorContext::less(const ReactorContext& other) const
{
    if (d_bytesReadable < other.d_bytesReadable) {
        return true;
    }

    if (other.d_bytesReadable < d_bytesReadable) {
        return false;
    }

    if (d_bytesWritable < other.d_bytesWritable) {
        return true;
    }

    if (other.d_bytesWritable < d_bytesWritable) {
        return false;
    }

    return d_error < other.d_error;
}

bsl::ostream& ReactorContext::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("bytesReadable", d_bytesReadable);
    printer.printAttribute("bytesWritable", d_bytesWritable);
    printer.printAttribute("error", d_error);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
