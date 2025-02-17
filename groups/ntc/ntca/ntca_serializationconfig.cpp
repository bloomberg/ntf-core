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

#include <ntca_serializationconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_serializationconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

bool SerializationConfig::equals(const SerializationConfig& other) const
{
    return d_berEncoderOptions == other.d_berEncoderOptions &&
           d_berDecoderOptions == other.d_berDecoderOptions &&
           d_jsnEncoderOptions == other.d_jsnEncoderOptions &&
           d_jsnDecoderOptions == other.d_jsnDecoderOptions &&
           d_xmlEncoderOptions == other.d_xmlEncoderOptions &&
           d_xmlDecoderOptions == other.d_xmlDecoderOptions;
}

bsl::ostream& SerializationConfig::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_berEncoderOptions.has_value()) {
        printer.printAttribute("berEncoderOptions",
                               d_berEncoderOptions.value());
    }

    if (d_berDecoderOptions.has_value()) {
        printer.printAttribute("berDecoderOptions",
                               d_berDecoderOptions.value());
    }

    if (d_jsnEncoderOptions.has_value()) {
        printer.printAttribute("jsnEncoderOptions",
                               d_jsnEncoderOptions.value());
    }

    if (d_jsnDecoderOptions.has_value()) {
        printer.printAttribute("jsnDecoderOptions",
                               d_jsnDecoderOptions.value());
    }

    if (d_xmlEncoderOptions.has_value()) {
        printer.printAttribute("xmlEncoderOptions",
                               d_xmlEncoderOptions.value());
    }

    if (d_xmlDecoderOptions.has_value()) {
        printer.printAttribute("xmlDecoderOptions",
                               d_xmlDecoderOptions.value());
    }

    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
