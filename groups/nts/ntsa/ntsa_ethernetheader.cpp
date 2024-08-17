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

#include <ntsa_ethernetheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetheader_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

bool EthernetHeader::equals(const EthernetHeader& other) const
{
    return d_source == other.d_source && 
           d_destination == other.d_destination && 
           d_protocol == other.d_protocol;
}

bool EthernetHeader::less(const EthernetHeader& other) const
{
    if (d_source < other.d_source) {
        return true;
    }

    if (other.d_source < d_source) {
        return false;
    }

    if (d_destination < other.d_destination) {
        return true;
    }

    if (other.d_destination < d_destination) {
        return false;
    }

    return d_protocol < other.d_protocol;
}

bsl::ostream& EthernetHeader::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("source", d_source);
    printer.printAttribute("destination", d_destination);
    printer.printAttribute("protocol", d_protocol);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
