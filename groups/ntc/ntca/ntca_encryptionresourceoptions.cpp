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

#include <ntca_encryptionresourceoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionresourceoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

EncryptionResourceOptions::EncryptionResourceOptions(
    bslma::Allocator* basicAllocator)
: d_hint(e_ANY)
, d_type()
, d_label(basicAllocator)
, d_secret(basicAllocator)
, d_secretCallback(basicAllocator)
, d_encrypted()
{
}

EncryptionResourceOptions::EncryptionResourceOptions(
    const EncryptionResourceOptions& original,
    bslma::Allocator*                basicAllocator)
: d_hint(original.d_hint)
, d_type(original.d_type)
, d_label(original.d_label, basicAllocator)
, d_secret(original.d_secret, basicAllocator)
, d_secretCallback(original.d_secretCallback, basicAllocator)
, d_encrypted(original.d_encrypted)
{
}

EncryptionResourceOptions::~EncryptionResourceOptions()
{
}

EncryptionResourceOptions& EncryptionResourceOptions::operator=(
    const EncryptionResourceOptions& other)
{
    if (this != &other) {
        d_hint           = other.d_hint;
        d_type           = other.d_type;
        d_label          = other.d_label;
        d_secret         = other.d_secret;
        d_secretCallback = other.d_secretCallback;
        d_encrypted      = other.d_encrypted;
    }

    return *this;
}

void EncryptionResourceOptions::reset()
{
    d_hint = e_ANY;
    d_type.reset();
    d_label.reset();
    d_secret.reset();
    d_secretCallback.reset();
    d_encrypted.reset();
}

void EncryptionResourceOptions::setHint(Hint value)
{
    d_hint = value;
}

void EncryptionResourceOptions::setType(
    ntca::EncryptionResourceType::Value value)
{
    d_type = value;
}

void EncryptionResourceOptions::setLabel(const bsl::string& value)
{
    d_label = value;
}

void EncryptionResourceOptions::setSecret(const ntca::EncryptionSecret& value)
{
    d_secret = value;
}

void EncryptionResourceOptions::setSecretCallback(
    const ntca::EncryptionSecretCallback& value)
{
    d_secretCallback = value;
}

void EncryptionResourceOptions::setEncrypted(bool value)
{
    d_encrypted = value;
}

EncryptionResourceOptions::Hint EncryptionResourceOptions::hint() const
{
    return d_hint;
}

const bdlb::NullableValue<ntca::EncryptionResourceType::Value>&
EncryptionResourceOptions::type() const
{
    return d_type;
}

const bdlb::NullableValue<bsl::string>& EncryptionResourceOptions::label()
    const
{
    return d_label;
}

const bdlb::NullableValue<ntca::EncryptionSecret>& EncryptionResourceOptions::
    secret() const
{
    return d_secret;
}

const bdlb::NullableValue<ntca::EncryptionSecretCallback>&
EncryptionResourceOptions::secretCallback() const
{
    return d_secretCallback;
}

const bdlb::NullableValue<bool>& EncryptionResourceOptions::encrypted() const
{
    return d_encrypted;
}

bool EncryptionResourceOptions::equals(
    const EncryptionResourceOptions& other) const
{
    return d_type == other.d_type && d_label == other.d_label &&
           d_secret == other.d_secret && d_encrypted == other.d_encrypted &&
           d_hint == other.d_hint;
}

bool EncryptionResourceOptions::less(
    const EncryptionResourceOptions& other) const
{
    if (d_hint < other.d_hint) {
        return true;
    }

    if (other.d_hint < d_hint) {
        return false;
    }

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

    if (d_secret < other.d_secret) {
        return true;
    }

    if (other.d_secret < d_secret) {
        return false;
    }

    return d_encrypted < other.d_encrypted;
}

bsl::ostream& EncryptionResourceOptions::print(bsl::ostream& stream,
                                               int           level,
                                               int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_hint != e_ANY) {
        printer.printAttribute("hint", d_hint);
    }

    if (!d_type.isNull()) {
        printer.printAttribute("type", d_type);
    }

    if (!d_label.isNull()) {
        printer.printAttribute("label", d_label);
    }

    if (!d_secret.isNull()) {
        printer.printAttribute("secret", d_secret);
    }

    if (!d_secretCallback.isNull()) {
        printer.printAttribute("secretCallback", "<private>");
    }

    if (!d_encrypted.isNull()) {
        printer.printAttribute("encrypted", d_encrypted);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const EncryptionResourceOptions& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionResourceOptions& lhs,
                const EncryptionResourceOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionResourceOptions& lhs,
                const EncryptionResourceOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionResourceOptions& lhs,
               const EncryptionResourceOptions& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
