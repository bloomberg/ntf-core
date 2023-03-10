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

#include <ntca_getdomainnameevent.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_getdomainnameevent_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool GetDomainNameEvent::equals(const GetDomainNameEvent& other) const
{
    return (d_type == other.d_type && d_context == other.d_context);
}

bool GetDomainNameEvent::less(const GetDomainNameEvent& other) const
{
    if (d_type < other.d_type) {
        return true;
    }

    if (d_type > other.d_type) {
        return false;
    }

    return d_context < other.d_context;
}

bsl::ostream& GetDomainNameEvent::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("type", d_type);
    printer.printAttribute("context", d_context);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
