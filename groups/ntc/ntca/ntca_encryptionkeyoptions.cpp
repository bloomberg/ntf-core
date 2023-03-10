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

#include <ntca_encryptionkeyoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionkeyoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

namespace {

// The default number of bits to use when generating an RSA key.
const int DEFAULT_BITS = 2048;

}  // close unnamed namespace

EncryptionKeyOptions::EncryptionKeyOptions()
: d_bits(DEFAULT_BITS)
{
}

void EncryptionKeyOptions::setBits(int bits)
{
    d_bits = bits;
}

int EncryptionKeyOptions::bits() const
{
    return d_bits;
}

bsl::ostream& EncryptionKeyOptions::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("d_bits", d_bits);
    printer.end();
    return stream;
}

bool operator==(const EncryptionKeyOptions& lhs,
                const EncryptionKeyOptions& rhs)
{
    return lhs.bits() == rhs.bits();
}

bool operator!=(const EncryptionKeyOptions& lhs,
                const EncryptionKeyOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const EncryptionKeyOptions& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
