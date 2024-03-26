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

#include <ntca_encryptionkeystorageoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionkeystorageoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

EncryptionKeyStorageOptions::EncryptionKeyStorageOptions(
    bslma::Allocator* basicAllocator)
: d_type()
, d_label(basicAllocator)
, d_passphrase(basicAllocator)
, d_flags()
{
}

EncryptionKeyStorageOptions::EncryptionKeyStorageOptions(
    const EncryptionKeyStorageOptions& original,
    bslma::Allocator*                  basicAllocator)
: d_type(original.d_type)
, d_label(original.d_label, basicAllocator)
, d_passphrase(original.d_passphrase, basicAllocator)
, d_flags(original.d_flags)
{
}

EncryptionKeyStorageOptions::~EncryptionKeyStorageOptions()
{
}

EncryptionKeyStorageOptions& EncryptionKeyStorageOptions::operator=(
    const EncryptionKeyStorageOptions& other)
{
    if (this != &other) {
        d_type       = other.d_type;
        d_label      = other.d_label;
        d_passphrase = other.d_passphrase;
        d_flags      = other.d_flags;
    }

    return *this;
}

void EncryptionKeyStorageOptions::reset()
{
    d_type.reset();
    d_label.reset();
    d_passphrase.reset();
    d_flags.reset();
}

void EncryptionKeyStorageOptions::setType(
    ntca::EncryptionKeyStorageType::Value value)
{
    d_type = value;
}

void EncryptionKeyStorageOptions::setLabel(const bsl::string& value)
{
    d_label = value;
}

void EncryptionKeyStorageOptions::setPassphrase(const bsl::string& value)
{
    d_passphrase = value;
}

const bdlb::NullableValue<ntca::EncryptionKeyStorageType::Value>&
EncryptionKeyStorageOptions::type() const
{
    return d_type;
}

const bdlb::NullableValue<bsl::string>& EncryptionKeyStorageOptions::label()
    const
{
    return d_label;
}

const bdlb::NullableValue<bsl::string>& EncryptionKeyStorageOptions::
    passphrase() const
{
    return d_passphrase;
}

bool EncryptionKeyStorageOptions::equals(
    const EncryptionKeyStorageOptions& other) const
{
    return d_type == other.d_type && d_label == other.d_label &&
           d_passphrase == other.d_passphrase && d_flags == other.d_flags;
}

bool EncryptionKeyStorageOptions::less(
    const EncryptionKeyStorageOptions& other) const
{
    if (d_type < other.d_type) {
        return true;
    }

    if (other.d_type < d_type) {
        return false;
    }

    if (d_label < other.d_label) {
        return true;
    }

    if (other.d_label < d_label) {
        return false;
    }

    if (d_passphrase < other.d_passphrase) {
        return true;
    }

    if (other.d_passphrase < d_passphrase) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& EncryptionKeyStorageOptions::print(bsl::ostream& stream,
                                                 int           level,
                                                 int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_type.isNull()) {
        printer.printAttribute("type", d_type);
    }

    if (!d_label.isNull()) {
        printer.printAttribute("label", d_label);
    }

    if (!d_passphrase.isNull()) {
        printer.printAttribute("passphrase", d_passphrase);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionKeyStorageOptions& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyStorageOptions& lhs,
                const EncryptionKeyStorageOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyStorageOptions& lhs,
                const EncryptionKeyStorageOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyStorageOptions& lhs,
               const EncryptionKeyStorageOptions& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
