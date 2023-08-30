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

#include <ntsa_file.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_file_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool File::equals(const File& other) const
{
    return d_fileDescriptor == other.d_fileDescriptor &&
           d_filePosition == other.d_filePosition &&
           d_fileBytesRemaining == other.d_fileBytesRemaining &&
           d_fileSize == other.d_fileSize;
}

bool File::less(const File& other) const
{
    if (d_fileDescriptor < other.d_fileDescriptor) {
        return false;
    }

    if (other.d_fileDescriptor < d_fileDescriptor) {
        return true;
    }

    if (d_filePosition < other.d_filePosition) {
        return false;
    }

    if (other.d_filePosition < d_filePosition) {
        return true;
    }

    if (d_fileBytesRemaining < other.d_fileBytesRemaining) {
        return false;
    }

    if (other.d_fileBytesRemaining < d_fileBytesRemaining) {
        return true;
    }

    return d_fileSize < other.d_fileSize;
}

bsl::ostream& File::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("descriptor", d_fileDescriptor);
    printer.printAttribute("position", d_filePosition);
    printer.printAttribute("bytesRemaining", d_fileBytesRemaining);
    printer.printAttribute("size", d_fileSize);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
