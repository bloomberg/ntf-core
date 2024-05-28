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

EncryptionKeyOptions::EncryptionKeyOptions(bslma::Allocator* basicAllocator)
: d_type()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

EncryptionKeyOptions::EncryptionKeyOptions(
    const EncryptionKeyOptions& original,
    bslma::Allocator*           basicAllocator)
: d_type(original.d_type)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

EncryptionKeyOptions::~EncryptionKeyOptions()
{
}

EncryptionKeyOptions& EncryptionKeyOptions::operator=(
    const EncryptionKeyOptions& other)
{
    if (this != &other) {
        d_type = other.d_type;
    }

    return *this;
}

void EncryptionKeyOptions::reset()
{
    d_type.reset();
}

void EncryptionKeyOptions::setType(ntca::EncryptionKeyType::Value value)
{
    d_type = value;
}

const bdlb::NullableValue<ntca::EncryptionKeyType::Value>& EncryptionKeyOptions::
    type() const
{
    return d_type;
}

bool EncryptionKeyOptions::equals(const EncryptionKeyOptions& other) const
{
    return d_type == other.d_type;
}

bool EncryptionKeyOptions::less(const EncryptionKeyOptions& other) const
{
    return d_type < other.d_type;
}

bsl::ostream& EncryptionKeyOptions::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_type.isNull()) {
        printer.printAttribute("type", d_type);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const EncryptionKeyOptions& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyOptions& lhs,
                const EncryptionKeyOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyOptions& lhs,
                const EncryptionKeyOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyOptions& lhs,
               const EncryptionKeyOptions& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
