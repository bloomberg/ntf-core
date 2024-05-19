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

#include <ntca_upgradeoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_upgradeoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool UpgradeOptions::equals(const UpgradeOptions& other) const
{
    return (d_token == other.d_token &&
            d_serverNameIndication == other.d_serverNameIndication &&
            d_serverNameVerification == other.d_serverNameVerification &&
            d_deadline == other.d_deadline && d_recurse == other.d_recurse);
}

bool UpgradeOptions::less(const UpgradeOptions& other) const
{
    if (d_token < other.d_token) {
        return true;
    }

    if (other.d_token < d_token) {
        return false;
    }

    if (d_serverNameIndication < other.d_serverNameIndication) {
        return true;
    }

    if (other.d_serverNameIndication < d_serverNameIndication) {
        return false;
    }

    if (d_serverNameVerification < other.d_serverNameVerification) {
        return true;
    }

    if (other.d_serverNameVerification < d_serverNameVerification) {
        return false;
    }

    if (d_deadline < other.d_deadline) {
        return true;
    }

    if (other.d_deadline < d_deadline) {
        return false;
    }

    return d_recurse < other.d_recurse;
}

bsl::ostream& UpgradeOptions::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_token.isNull()) {
        printer.printAttribute("token", d_token);
    }

    if (!d_serverNameIndication.isNull()) {
        printer.printAttribute("serverNameIndication", d_serverNameIndication);
    }

    if (!d_serverNameVerification.isNull()) {
        printer.printAttribute("serverNameVerification",
                               d_serverNameVerification);
    }

    if (!d_trustSelfSignedCertificates.isNull()) {
        printer.printAttribute("trustSelfSignedCertificates",
                               d_trustSelfSignedCertificates);
    }

    if (!d_deadline.isNull()) {
        printer.printAttribute("deadline", d_deadline);
    }

    printer.printAttribute("recurse", d_recurse);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
