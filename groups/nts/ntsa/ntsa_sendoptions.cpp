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

#include <ntsa_sendoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_sendoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool SendOptions::equals(const SendOptions& other) const
{
    return (d_endpoint == other.d_endpoint && 
            d_foreignHandle == other.d_foreignHandle &&
            d_maxBytes == other.d_maxBytes &&
            d_maxBuffers == other.d_maxBuffers &&
            d_zeroCopy == other.d_zeroCopy);
}

bool SendOptions::less(const SendOptions& other) const
{
    if (d_endpoint < other.d_endpoint) {
        return true;
    }

    if (other.d_endpoint < d_endpoint) {
        return false;
    }

    if (d_foreignHandle < other.d_foreignHandle) {
        return true;
    }

    if (other.d_foreignHandle < d_foreignHandle) {
        return false;
    }

    if (d_maxBytes < other.d_maxBytes) {
        return true;
    }

    if (other.d_maxBytes < d_maxBytes) {
        return false;
    }

    if (d_maxBuffers < other.d_maxBuffers) {
        return true;
    }

    if (other.d_maxBuffers < d_maxBuffers) {
        return false;
    }
    return d_zeroCopy < other.d_zeroCopy;
}

bsl::ostream& SendOptions::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("endpoint", d_endpoint);
    printer.printAttribute("foreignHandle", d_foreignHandle);
    printer.printAttribute("maxBytes", d_maxBytes);
    printer.printAttribute("maxBuffers", d_maxBuffers);
    printer.printAttribute("zeroCopy", d_zeroCopy);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
