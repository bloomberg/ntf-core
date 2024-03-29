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

#include <ntca_encryptionkey.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionkey_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

EncryptionKey::EncryptionKey(bslma::Allocator* basicAllocator)
: d_type(ntca::EncryptionKeyType::e_NIST_P256)
, d_flags(0)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

EncryptionKey::EncryptionKey(const EncryptionKey& original,
                             bslma::Allocator*    basicAllocator)
: d_type(original.d_type)
, d_flags(original.d_flags)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

EncryptionKey::~EncryptionKey()
{
}

EncryptionKey& EncryptionKey::operator=(const EncryptionKey& other)
{
    if (this != &other) {
        d_type  = other.d_type;
        d_flags = other.d_flags;
    }

    return *this;
}

void EncryptionKey::reset()
{
    d_type  = ntca::EncryptionKeyType::e_NIST_P256;
    d_flags = 0;
}

void EncryptionKey::setType(ntca::EncryptionKeyType::Value value)
{
    d_type = value;
}

ntca::EncryptionKeyType::Value EncryptionKey::type() const
{
    return d_type;
}

bool EncryptionKey::equals(const EncryptionKey& other) const
{
    return d_type == other.d_type && d_flags == other.d_flags;
}

bool EncryptionKey::less(const EncryptionKey& other) const
{
    if (d_type < other.d_type) {
        return true;
    }

    if (other.d_type < d_type) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& EncryptionKey::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("type", d_type);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionKey& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKey& lhs, const EncryptionKey& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKey& lhs, const EncryptionKey& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKey& lhs, const EncryptionKey& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
