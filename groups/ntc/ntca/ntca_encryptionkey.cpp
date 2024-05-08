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

#include <bdlb_string.h>
#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

namespace {

const ntsa::AbstractSyntaxTagClass::Value k_UNIVERSAL =
    ntsa::AbstractSyntaxTagClass::e_UNIVERSAL;

const ntsa::AbstractSyntaxTagClass::Value k_CONTEXT_SPECIFIC =
    ntsa::AbstractSyntaxTagClass::e_CONTEXT_SPECIFIC;

const ntsa::AbstractSyntaxTagType::Value k_CONSTRUCTED =
    ntsa::AbstractSyntaxTagType::e_CONSTRUCTED;

const ntsa::AbstractSyntaxTagType::Value k_PRIMITIVE =
    ntsa::AbstractSyntaxTagType::e_PRIMITIVE;

const ntsa::AbstractSyntaxTagNumber::Value k_SEQUENCE =
    ntsa::AbstractSyntaxTagNumber::e_SEQUENCE;

const ntsa::AbstractSyntaxTagNumber::Value k_NULL =
    ntsa::AbstractSyntaxTagNumber::e_NULL;

const ntsa::AbstractSyntaxTagNumber::Value k_INTEGER =
    ntsa::AbstractSyntaxTagNumber::e_INTEGER;

const ntsa::AbstractSyntaxTagNumber::Value k_BIT_STRING =
    ntsa::AbstractSyntaxTagNumber::e_BIT_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_OCTET_STRING =
    ntsa::AbstractSyntaxTagNumber::e_OCTET_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_OBJECT_IDENTIFIER =
    ntsa::AbstractSyntaxTagNumber::e_OBJECT_IDENTIFIER;

} // close unnamed namespace


EncryptionKeyRsaParameters::
    EncryptionKeyRsaParameters(
        bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyRsaParameters::
    EncryptionKeyRsaParameters(
        const EncryptionKeyRsaParameters& original,
        bslma::Allocator* basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyRsaParameters::
    ~EncryptionKeyRsaParameters()
{
}

EncryptionKeyRsaParameters&
EncryptionKeyRsaParameters::operator=(
    const EncryptionKeyRsaParameters& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionKeyRsaParameters::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionKeyRsaParameters::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_NULL);
    if (error) {
        return error;
    }

    error = decoder->decodeValue(&d_value);
    if (error) {
        return error;
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionKeyRsaParameters::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_NULL);
    if (error) {
        return error;
    }

    error = encoder->encodeValue(d_value);
    if (error) {
        return error;
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bool EncryptionKeyRsaParameters::equals(
    const EncryptionKeyRsaParameters& other) const
{
    return d_value == other.d_value;
}

bool EncryptionKeyRsaParameters::less(
    const EncryptionKeyRsaParameters& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionKeyRsaParameters::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (d_value.tagClass() == k_UNIVERSAL &&
        d_value.tagType() == k_PRIMITIVE && d_value.tagNumber() == k_NULL)
    {
        stream << "NULL";
    }
    else {
        d_value.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                               stream,
    const EncryptionKeyRsaParameters& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(
    const EncryptionKeyRsaParameters& lhs,
    const EncryptionKeyRsaParameters& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(
    const EncryptionKeyRsaParameters& lhs,
    const EncryptionKeyRsaParameters& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyRsaParameters& lhs,
               const EncryptionKeyRsaParameters& rhs)
{
    return lhs.less(rhs);
}


EncryptionKeyRsaExtra::EncryptionKeyRsaExtra(
    bslma::Allocator* basicAllocator)
: d_prime(basicAllocator)
, d_exponent(basicAllocator)
, d_coefficient(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyRsaExtra::EncryptionKeyRsaExtra(
    const EncryptionKeyRsaExtra& original,
    bslma::Allocator*                  basicAllocator)
: d_prime(original.d_prime, basicAllocator)
, d_exponent(original.d_exponent, basicAllocator)
, d_coefficient(original.d_coefficient, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyRsaExtra::~EncryptionKeyRsaExtra()
{
}

EncryptionKeyRsaExtra& EncryptionKeyRsaExtra::operator=(
    const EncryptionKeyRsaExtra& other)
{
    if (this != &other) {
        d_prime = other.d_prime;
        d_exponent = other.d_exponent;
        d_coefficient = other.d_coefficient;
    }

    return *this;
}

void EncryptionKeyRsaExtra::reset()
{
    d_prime.reset();
    d_exponent.reset();
    d_coefficient.reset();
}

void EncryptionKeyRsaExtra::setPrime(const ntsa::AbstractInteger& value)
{
    d_prime = value;
}

void EncryptionKeyRsaExtra::setExponent(const ntsa::AbstractInteger& value)
{
    d_exponent = value;
}


void EncryptionKeyRsaExtra::setCoefficient(const ntsa::AbstractInteger& value)
{
    d_coefficient = value;
}

ntsa::Error EncryptionKeyRsaExtra::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_prime);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_exponent);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_coefficient);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionKeyRsaExtra::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_prime);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_exponent);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_coefficient);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

const ntsa::AbstractInteger& EncryptionKeyRsaExtra::prime() const
{
    return d_prime;
}

const ntsa::AbstractInteger& EncryptionKeyRsaExtra::exponent() const
{
    return d_exponent;
}

const ntsa::AbstractInteger& EncryptionKeyRsaExtra::coefficient() const
{
    return d_coefficient;
}

bool EncryptionKeyRsaExtra::equals(
    const EncryptionKeyRsaExtra& other) const
{
    return d_prime == other.d_prime && d_exponent == other.d_exponent && d_coefficient == other.d_coefficient;
}

bool EncryptionKeyRsaExtra::less(
    const EncryptionKeyRsaExtra& other) const
{
    if (d_prime < other.d_prime) {
        return true;
    }

    if (other.d_prime < d_prime) {
        return false;
    }

    if (d_exponent < other.d_exponent) {
        return true;
    }

    if (other.d_exponent < d_exponent) {
        return false;
    }

    return d_coefficient < other.d_coefficient;
}

bsl::ostream& EncryptionKeyRsaExtra::print(bsl::ostream& stream,
                                                 int           level,
                                                 int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("prime", d_prime);
    printer.printAttribute("exponent", d_exponent);
    printer.printAttribute("coefficient", d_coefficient);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionKeyRsaExtra& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyRsaExtra& lhs,
                const EncryptionKeyRsaExtra& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyRsaExtra& lhs,
                const EncryptionKeyRsaExtra& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyRsaExtra& lhs,
               const EncryptionKeyRsaExtra& rhs)
{
    return lhs.less(rhs);
}











EncryptionKeyRsa::EncryptionKeyRsa(bslma::Allocator* basicAllocator)
: d_version(basicAllocator)
, d_modulus(basicAllocator)
, d_encryptionExponent(basicAllocator)
, d_decryptionExponent(basicAllocator)
, d_prime1(basicAllocator)
, d_prime2(basicAllocator)
, d_exponent1(basicAllocator)
, d_exponent2(basicAllocator)
, d_coefficient(basicAllocator)
, d_extra(basicAllocator)
{
}

EncryptionKeyRsa::EncryptionKeyRsa(const EncryptionKeyRsa& original,
                                   bslma::Allocator*       basicAllocator)
: d_version(original.d_version, basicAllocator)
, d_modulus(original.d_modulus, basicAllocator)
, d_encryptionExponent(original.d_encryptionExponent, basicAllocator)
, d_decryptionExponent(original.d_decryptionExponent, basicAllocator)
, d_prime1(original.d_prime1, basicAllocator)
, d_prime2(original.d_prime2, basicAllocator)
, d_exponent1(original.d_exponent1, basicAllocator)
, d_exponent2(original.d_exponent2, basicAllocator)
, d_coefficient(original.d_coefficient, basicAllocator)
, d_extra(original.d_extra, basicAllocator)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

EncryptionKeyRsa::~EncryptionKeyRsa()
{
}

EncryptionKeyRsa& EncryptionKeyRsa::operator=(const EncryptionKeyRsa& other)
{
    if (this != &other) {
        d_version         = other.d_version;
        d_modulus         = other.d_modulus;
        d_encryptionExponent  = other.d_encryptionExponent;
        d_decryptionExponent = other.d_decryptionExponent;
        d_prime1          = other.d_prime1;
        d_prime2          = other.d_prime2;
        d_exponent1       = other.d_exponent1;
        d_exponent2       = other.d_exponent2;
        d_coefficient     = other.d_coefficient;
        d_extra  = other.d_extra;
    }

    return *this;
}

void EncryptionKeyRsa::reset()
{
    d_version.reset();
    d_modulus.reset();
    d_encryptionExponent.reset();
    d_decryptionExponent.reset();
    d_prime1.reset();
    d_prime2.reset();
    d_exponent1.reset();
    d_exponent2.reset();
    d_coefficient.reset();
    d_extra.clear();
}

void EncryptionKeyRsa::setVersion(const ntsa::AbstractInteger& value)
{
    d_version = value;
}

void EncryptionKeyRsa::setModulus(const ntsa::AbstractInteger& value)
{
    d_modulus = value;
}

void EncryptionKeyRsa::setEncryptionExponent(const ntsa::AbstractInteger& value)
{
    d_encryptionExponent = value;
}

void EncryptionKeyRsa::setDecryptionExponent(const ntsa::AbstractInteger& value)
{
    d_decryptionExponent = value;
}

void EncryptionKeyRsa::setPrime1(const ntsa::AbstractInteger& value)
{
    d_prime1 = value;
}

void EncryptionKeyRsa::setPrime2(const ntsa::AbstractInteger& value)
{
    d_prime2 = value;
}

void EncryptionKeyRsa::setExponent1(const ntsa::AbstractInteger& value)
{
    d_exponent1 = value;
}

void EncryptionKeyRsa::setExponent2(const ntsa::AbstractInteger& value)
{
    d_exponent2 = value;
}

void EncryptionKeyRsa::setCoefficient(const ntsa::AbstractInteger& value)
{
    d_coefficient = value;
}

void EncryptionKeyRsa::setExtra(
    const bsl::vector<ntca::EncryptionKeyRsaExtra>& value)
{
    d_extra = value;
}

void EncryptionKeyRsa::addExtra(const ntca::EncryptionKeyRsaExtra& value)
{
    d_extra.push_back(value);
}

ntsa::Error EncryptionKeyRsa::decode(ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_version);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_modulus);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_encryptionExponent);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_decryptionExponent);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_prime1);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_prime2);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_exponent1);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_exponent2);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_coefficient);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    if (decoder->contentBytesRemaining() > 0) {
        error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
        if (error) {
            return error;
        }

        while (decoder->contentBytesRemaining() > 0) {
            ntca::EncryptionKeyRsaExtra extra;

            error = extra.decode(decoder);
            if (error) {
                return error;
            }

            d_extra.push_back(extra);
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionKeyRsa::encode(ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_version);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_modulus);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_encryptionExponent);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_decryptionExponent);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_prime1);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_prime2);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_exponent1);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_exponent2);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_coefficient);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    if (!d_extra.empty()) {
        error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
        if (error) {
            return error;
        }

        for (bsl::size_t i = 0; i < d_extra.size(); ++i) {
            const ntca::EncryptionKeyRsaExtra& extra = d_extra[i];

            error = extra.encode(encoder);
            if (error) {
                return error;
            }
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

const ntsa::AbstractInteger& EncryptionKeyRsa::version() const
{
    return d_version;
}

const ntsa::AbstractInteger& EncryptionKeyRsa::modulus() const
{
    return d_modulus;
}

const ntsa::AbstractInteger& EncryptionKeyRsa::encryptionExponent() const
{
    return d_encryptionExponent;
}

const ntsa::AbstractInteger& EncryptionKeyRsa::decryptionExponent() const
{
    return d_decryptionExponent;
}

const ntsa::AbstractInteger& EncryptionKeyRsa::prime1() const
{
    return d_prime1;
}

const ntsa::AbstractInteger& EncryptionKeyRsa::prime2() const
{
    return d_prime2;
}

const ntsa::AbstractInteger& EncryptionKeyRsa::exponent1() const
{
    return d_exponent1;
}

const ntsa::AbstractInteger& EncryptionKeyRsa::exponent2() const
{
    return d_exponent2;
}

const ntsa::AbstractInteger& EncryptionKeyRsa::coefficient() const
{
    return d_coefficient;
}

const bsl::vector<ntca::EncryptionKeyRsaExtra>& EncryptionKeyRsa::extra() const
{
    return d_extra;
}

bool EncryptionKeyRsa::equals(const EncryptionKeyRsa& other) const
{
    return d_version == other.d_version && d_modulus == other.d_modulus &&
           d_encryptionExponent == other.d_encryptionExponent &&
           d_decryptionExponent == other.d_decryptionExponent &&
           d_prime1 == other.d_prime1 && d_prime2 == other.d_prime2 &&
           d_exponent1 == other.d_exponent1 &&
           d_exponent2 == other.d_exponent2 &&
           d_coefficient == other.d_coefficient &&
           d_extra == other.d_extra;
}

bool EncryptionKeyRsa::less(const EncryptionKeyRsa& other) const
{
    if (d_version < other.d_version) {
        return true;
    }

    if (other.d_version < d_version) {
        return false;
    }

    if (d_modulus < other.d_modulus) {
        return true;
    }

    if (other.d_modulus < d_modulus) {
        return false;
    }

    if (d_encryptionExponent < other.d_encryptionExponent) {
        return true;
    }

    if (other.d_encryptionExponent < d_encryptionExponent) {
        return false;
    }

    if (d_decryptionExponent < other.d_decryptionExponent) {
        return true;
    }

    if (other.d_decryptionExponent < d_decryptionExponent) {
        return false;
    }

    if (d_prime1 < other.d_prime1) {
        return true;
    }

    if (other.d_prime1 < d_prime1) {
        return false;
    }

    if (d_prime2 < other.d_prime2) {
        return true;
    }

    if (other.d_prime2 < d_prime2) {
        return false;
    }

    if (d_exponent1 < other.d_exponent1) {
        return true;
    }

    if (other.d_exponent1 < d_exponent1) {
        return false;
    }

    if (d_exponent2 < other.d_exponent2) {
        return true;
    }

    if (other.d_exponent2 < d_exponent2) {
        return false;
    }

    if (d_coefficient < other.d_coefficient) {
        return true;
    }

    if (other.d_coefficient < d_coefficient) {
        return false;
    }

    return d_extra < other.d_extra;
}

bsl::ostream& EncryptionKeyRsa::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("version", d_version);
    printer.printAttribute("modulus", d_modulus);
    printer.printAttribute("encryptionExponent", d_encryptionExponent);
    printer.printAttribute("decryptionExponent", d_decryptionExponent);
    printer.printAttribute("prime1", d_prime1);
    printer.printAttribute("prime2", d_prime2);
    printer.printAttribute("exponent1", d_exponent1);
    printer.printAttribute("exponent2", d_exponent2);
    printer.printAttribute("coefficient", d_coefficient);

    if (!d_extra.empty()) {
        printer.printAttribute("extra", d_extra);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionKeyRsa& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyRsa& lhs, const EncryptionKeyRsa& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyRsa& lhs, const EncryptionKeyRsa& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyRsa& lhs, const EncryptionKeyRsa& rhs)
{
    return lhs.less(rhs);
}


















const char*
EncryptionKeyEllipticCurveParametersIdentifierType::
    toString(Value value)
{
    switch (value) {
    case e_SEC_P256_R1:
        return "SEC_P256_R1";
    case e_SEC_P384_R1:
        return "SEC_P384_R1";
    case e_SEC_P521_R1:
        return "SEC_P521_R1";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionKeyEllipticCurveParametersIdentifierType::
    toObjectIdentifier(ntsa::AbstractObjectIdentifier* result, Value value)
{
    result->reset();

    if (value == e_SEC_P256_R1) {
        result->set(1, 2, 840, 10045, 3, 1, 7);
    }
    else if (value == e_SEC_P384_R1) {
        result->set(1, 3, 132, 0, 34);
    }
    else if (value == e_SEC_P521_R1) {
        result->set(1, 3, 132, 0, 35);
    }
}

int EncryptionKeyEllipticCurveParametersIdentifierType::
    fromString(Value* result, const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "SEC_P256_R1")) {
        *result = e_SEC_P256_R1;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SEC_P384_R1")) {
        *result = e_SEC_P384_R1;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SEC_P521_R1")) {
        *result = e_SEC_P521_R1;
        return 0;
    }

    return -1;
}

int EncryptionKeyEllipticCurveParametersIdentifierType::
    fromObjectIdentifier(Value*                                result,
                         const ntsa::AbstractObjectIdentifier& identifier)
{
    if (identifier.equals(1, 2, 840, 10045, 3, 1, 7)) {
        *result = e_SEC_P256_R1;
        return 0;
    }

    if (identifier.equals(1, 3, 132, 0, 34)) {
        *result = e_SEC_P384_R1;
        return 0;
    }

    if (identifier.equals(1, 3, 132, 0, 35)) {
        *result = e_SEC_P521_R1;
        return 0;
    }

    return -1;
}

bsl::ostream&
EncryptionKeyEllipticCurveParametersIdentifierType::
    print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream& stream,
    EncryptionKeyEllipticCurveParametersIdentifierType::
        Value rhs)
{
    return EncryptionKeyEllipticCurveParametersIdentifierType::
        print(stream, rhs);
}

EncryptionKeyEllipticCurveParametersIdentifier::
    EncryptionKeyEllipticCurveParametersIdentifier(
        bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyEllipticCurveParametersIdentifier::
    EncryptionKeyEllipticCurveParametersIdentifier(
        const EncryptionKeyEllipticCurveParametersIdentifier&
                          original,
        bslma::Allocator* basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyEllipticCurveParametersIdentifier::
    ~EncryptionKeyEllipticCurveParametersIdentifier()
{
}

EncryptionKeyEllipticCurveParametersIdentifier&
EncryptionKeyEllipticCurveParametersIdentifier::
operator=(
    const EncryptionKeyEllipticCurveParametersIdentifier&
        other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionKeyEllipticCurveParametersIdentifier&
EncryptionKeyEllipticCurveParametersIdentifier::
operator=(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionKeyEllipticCurveParametersIdentifier&
EncryptionKeyEllipticCurveParametersIdentifier::
operator=(
    EncryptionKeyEllipticCurveParametersIdentifierType::
        Value value)
{
    EncryptionKeyEllipticCurveParametersIdentifierType::
        toObjectIdentifier(&d_identifier, value);
    return *this;
}

void EncryptionKeyEllipticCurveParametersIdentifier::
    reset()
{
    d_identifier.reset();
}

void EncryptionKeyEllipticCurveParametersIdentifier::
    setIdentifier(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionKeyEllipticCurveParametersIdentifier::
    setIdentifer(
        EncryptionKeyEllipticCurveParametersIdentifierType::
            Value value)
{
    EncryptionKeyEllipticCurveParametersIdentifierType::
        toObjectIdentifier(&d_identifier, value);
}

ntsa::Error
EncryptionKeyEllipticCurveParametersIdentifier::
    decode(ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OBJECT_IDENTIFIER);
    if (error) {
        return error;
    }

    error = decoder->decodeValue(&d_identifier);
    if (error) {
        return error;
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error
EncryptionKeyEllipticCurveParametersIdentifier::
    encode(ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OBJECT_IDENTIFIER);
    if (error) {
        return error;
    }

    error = encoder->encodeValue(d_identifier);
    if (error) {
        return error;
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

const ntsa::AbstractObjectIdentifier&
EncryptionKeyEllipticCurveParametersIdentifier::
    identifier() const
{
    return d_identifier;
}

bool EncryptionKeyEllipticCurveParametersIdentifier::
    equals(
        const EncryptionKeyEllipticCurveParametersIdentifier&
            other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionKeyEllipticCurveParametersIdentifier::
    equals(
        EncryptionKeyEllipticCurveParametersIdentifierType::
            Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionKeyEllipticCurveParametersIdentifierType::
        toObjectIdentifier(&identifier, value);

    return d_identifier.equals(identifier);
}

bool EncryptionKeyEllipticCurveParametersIdentifier::
    less(
        const EncryptionKeyEllipticCurveParametersIdentifier&
            other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream&
EncryptionKeyEllipticCurveParametersIdentifier::
    print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    ntca::
        EncryptionKeyEllipticCurveParametersIdentifierType::
            Value type;
    int           rc = ntca::
        EncryptionKeyEllipticCurveParametersIdentifierType::
            fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::
                EncryptionKeyEllipticCurveParametersIdentifierType::
                    toString(type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream& stream,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        object)
{
    return object.print(stream, 0, -1);
}

bool operator==(
    const EncryptionKeyEllipticCurveParametersIdentifier&
        lhs,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(
    const EncryptionKeyEllipticCurveParametersIdentifier&
        lhs,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(
    const EncryptionKeyEllipticCurveParametersIdentifier&
        lhs,
    const EncryptionKeyEllipticCurveParametersIdentifier&
        rhs)
{
    return lhs.less(rhs);
}

EncryptionKeyEllipticCurveParameters::
    EncryptionKeyEllipticCurveParameters(
        bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyEllipticCurveParameters::
    EncryptionKeyEllipticCurveParameters(
        const EncryptionKeyEllipticCurveParameters&
                          original,
        bslma::Allocator* basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_IDENTIFIER) {
        new (d_identifier.buffer())
            EncryptionKeyEllipticCurveParametersIdentifier(
                original.d_identifier.object(),
                d_allocator_p);
    }
    else if (d_type == e_ANY) {
        new (d_any.buffer())
            ntsa::AbstractValue(original.d_any.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionKeyEllipticCurveParameters::
    ~EncryptionKeyEllipticCurveParameters()
{
    this->reset();
}

EncryptionKeyEllipticCurveParameters&
EncryptionKeyEllipticCurveParameters::operator=(
    const EncryptionKeyEllipticCurveParameters&
        other)
{
    if (this == &other) {
        return *this;
    }

    if (other.d_type == e_IDENTIFIER) {
        this->makeIdentifier(other.d_identifier.object());
    }
    else if (other.d_type == e_ANY) {
        this->makeAny(other.d_any.object());
    }
    else {
        BSLS_ASSERT(other.d_type == e_UNDEFINED);
        this->reset();
    }

    return *this;
}

void EncryptionKeyEllipticCurveParameters::reset()
{
    if (d_type == e_IDENTIFIER) {
        typedef EncryptionKeyEllipticCurveParametersIdentifier
            Type;
        d_identifier.object().~Type();
    }
    else if (d_type == e_ANY) {
        typedef ntsa::AbstractValue Type;
        d_any.object().~Type();
    }

    d_type = e_UNDEFINED;
}

EncryptionKeyEllipticCurveParametersIdentifier&
EncryptionKeyEllipticCurveParameters::
    makeIdentifier()
{
    if (d_type == e_IDENTIFIER) {
        d_identifier.object().reset();
    }
    else {
        this->reset();
        new (d_identifier.buffer())
            EncryptionKeyEllipticCurveParametersIdentifier(
                d_allocator_p);
        d_type = e_IDENTIFIER;
    }

    return d_identifier.object();
}

EncryptionKeyEllipticCurveParametersIdentifier&
EncryptionKeyEllipticCurveParameters::makeIdentifier(
    const EncryptionKeyEllipticCurveParametersIdentifier&
        value)
{
    if (d_type == e_IDENTIFIER) {
        d_identifier.object() = value;
    }
    else {
        this->reset();
        new (d_identifier.buffer())
            EncryptionKeyEllipticCurveParametersIdentifier(
                value,
                d_allocator_p);
        d_type = e_IDENTIFIER;
    }

    return d_identifier.object();
}

ntsa::AbstractValue&
EncryptionKeyEllipticCurveParameters::makeAny()
{
    if (d_type == e_ANY) {
        d_any.object().reset();
    }
    else {
        this->reset();
        new (d_any.buffer()) ntsa::AbstractValue(d_allocator_p);
        d_type = e_ANY;
    }

    return d_any.object();
}

ntsa::AbstractValue&
EncryptionKeyEllipticCurveParameters::makeAny(
    const ntsa::AbstractValue& value)
{
    if (d_type == e_ANY) {
        d_any.object() = value;
    }
    else {
        this->reset();
        new (d_any.buffer()) ntsa::AbstractValue(value, d_allocator_p);
        d_type = e_ANY;
    }

    return d_any.object();
}

EncryptionKeyEllipticCurveParametersIdentifier&
EncryptionKeyEllipticCurveParameters::identifier()
{
    BSLS_ASSERT(this->isIdentifier());
    return d_identifier.object();
}

ntsa::AbstractValue&
EncryptionKeyEllipticCurveParameters::any()
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

const EncryptionKeyEllipticCurveParametersIdentifier&
EncryptionKeyEllipticCurveParameters::identifier()
    const
{
    BSLS_ASSERT(this->isIdentifier());
    return d_identifier.object();
}

const ntsa::AbstractValue&
EncryptionKeyEllipticCurveParameters::any() const
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

bool EncryptionKeyEllipticCurveParameters::
    isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionKeyEllipticCurveParameters::
    isIdentifier() const
{
    return d_type == e_IDENTIFIER;
}

bool EncryptionKeyEllipticCurveParameters::isAny()
    const
{
    return d_type == e_ANY;
}

bool EncryptionKeyEllipticCurveParameters::equals(
    const EncryptionKeyEllipticCurveParameters&
        other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_UNDEFINED) {
        return true;
    }
    else if (d_type == e_IDENTIFIER) {
        return d_identifier.object() == other.d_identifier.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() == other.d_any.object();
    }

    return false;
}

bool EncryptionKeyEllipticCurveParameters::less(
    const EncryptionKeyEllipticCurveParameters&
        other) const
{
    if (d_type < other.d_type) {
        return false;
    }

    if (other.d_type < d_type) {
        return true;
    }

    if (d_type == e_UNDEFINED) {
        return true;
    }
    else if (d_type == e_IDENTIFIER) {
        return d_identifier.object() < other.d_identifier.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() < other.d_any.object();
    }

    return false;
}

bsl::ostream& EncryptionKeyEllipticCurveParameters::
    print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    if (d_type == e_IDENTIFIER) {
        stream << d_identifier.object();
    }
    else if (d_type == e_ANY) {
        stream << d_any.object();
    }
    else {
        stream << "UNDEFINED";
    }

    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream& stream,
    const EncryptionKeyEllipticCurveParameters&
        object)
{
    return object.print(stream, 0, -1);
}

bool operator==(
    const EncryptionKeyEllipticCurveParameters& lhs,
    const EncryptionKeyEllipticCurveParameters& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(
    const EncryptionKeyEllipticCurveParameters& lhs,
    const EncryptionKeyEllipticCurveParameters& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(
    const EncryptionKeyEllipticCurveParameters& lhs,
    const EncryptionKeyEllipticCurveParameters& rhs)
{
    return lhs.less(rhs);
}













EncryptionKeyEllipticCurve::EncryptionKeyEllipticCurve(
    bslma::Allocator* basicAllocator)
: d_version(basicAllocator)
, d_privateKey(basicAllocator)
, d_parameters(basicAllocator)
, d_publicKey(basicAllocator)
{
    d_version.assign(1);
}

EncryptionKeyEllipticCurve::EncryptionKeyEllipticCurve(
    const EncryptionKeyEllipticCurve& original,
    bslma::Allocator*                 basicAllocator)
: d_version(original.d_version)
, d_privateKey(original.d_privateKey, basicAllocator)
, d_parameters(original.d_parameters, basicAllocator)
, d_publicKey(original.d_publicKey, basicAllocator)
{
}

EncryptionKeyEllipticCurve::~EncryptionKeyEllipticCurve()
{
}

EncryptionKeyEllipticCurve& EncryptionKeyEllipticCurve::operator=(
    const EncryptionKeyEllipticCurve& other)
{
    if (this != &other) {
        d_version    = other.d_version;
        d_privateKey = other.d_privateKey;
        d_parameters = other.d_parameters;
        d_publicKey  = other.d_publicKey;
    }

    return *this;
}

void EncryptionKeyEllipticCurve::reset()
{
    d_version.reset();
    d_version.assign(1);

    d_privateKey.reset();
    d_parameters.reset();
    d_publicKey.reset();
}

ntsa::Error EncryptionKeyEllipticCurve::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_version);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    if (!d_version.equals(1)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OCTET_STRING);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_privateKey);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    bool wantParameters = true;
    bool wantPublicKey = true;

    while (decoder->contentBytesRemaining() > 0) {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagClass() == k_CONTEXT_SPECIFIC &&
            decoder->current().tagType() == k_CONSTRUCTED &&
            decoder->current().tagNumber() == 0 && wantParameters)
        {
            error = decoder->decodeTag();
            if (error) {
                return error;
            }

            if (decoder->current().tagClass() == k_UNIVERSAL &&
                decoder->current().tagType() == k_PRIMITIVE &&
                decoder->current().tagNumber() == k_OBJECT_IDENTIFIER)
            {
                error = decoder->rewindTag();
                if (error) {
                    return error;
                }

                error = d_parameters.makeValue().makeIdentifier().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else {
                error = decoder->decodeValue(&d_parameters.makeValue().makeAny());
                if (error) {
                    return error;
                }

                error = decoder->decodeTagComplete();
                if (error) {
                    return error;
                }
            }

            wantParameters = false;
        }
        else if (decoder->current().tagClass() == k_CONTEXT_SPECIFIC &&
                 decoder->current().tagType() == k_CONSTRUCTED &&
                 decoder->current().tagNumber() == 1 && wantPublicKey)
        {
            error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
            if (error) {
                return error;
            }

            error = decoder->decodeValue(&d_publicKey.makeValue());
            if (error) {
                return error;
            }

            error = decoder->decodeTagComplete();
            if (error) {
                return error;
            }

            wantPublicKey = false;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionKeyEllipticCurve::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    if (!d_version.equals(1)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_version);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OCTET_STRING);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_privateKey);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

   
    if (!d_parameters.isNull()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, 0);
        if (error) {
            return error;
        }

        if (d_parameters.value().isIdentifier()) {
            error = d_parameters.value().identifier().encode(encoder);
            if (error) {
                return error;
            }
        }
        else if (d_parameters.value().isAny()) {
            error = encoder->encodeTag(
                d_parameters.value().any().tagClass(),
                d_parameters.value().any().tagType(),
                d_parameters.value().any().tagNumber());
            if (error) {
                return error;
            }

            error = encoder->encodeValue(d_parameters.value().any());
            if (error) {
                return error;
            }

            error = encoder->encodeTagComplete();
            if (error) {
                return error;
            }
        }
        else if (d_parameters.value().isUndefined()) {
            error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_NULL);
            if (error) {
                return error;
            }

            error = encoder->encodeTagComplete();
            if (error) {
                return error;
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    if (!d_publicKey.isNull()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, 1);
        if (error) {
            return error;
        }

        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_publicKey.value());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

bool EncryptionKeyEllipticCurve::equals(
    const EncryptionKeyEllipticCurve& other) const
{
    return d_version == other.d_version &&
           d_privateKey == other.d_privateKey &&
           d_parameters == other.d_parameters &&
           d_publicKey == other.d_publicKey;
}

bool EncryptionKeyEllipticCurve::less(
    const EncryptionKeyEllipticCurve& other) const
{
    if (d_version < other.d_version) {
        return true;
    }

    if (other.d_version < d_version) {
        return false;
    }

    if (d_privateKey < other.d_privateKey) {
        return true;
    }

    if (other.d_privateKey < d_privateKey) {
        return false;
    }

    if (d_parameters < other.d_parameters) {
        return true;
    }

    if (other.d_parameters < d_parameters) {
        return false;
    }

    return d_publicKey < other.d_publicKey;
}

bsl::ostream& EncryptionKeyEllipticCurve::print(bsl::ostream& stream,
                                                int           level,
                                                int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("version", d_version);
    
    if (!d_parameters.isNull()) {
        printer.printAttribute("parameters", d_parameters.value());
    }

    printer.printAttribute("privateKey", d_privateKey);

    if (!d_publicKey.isNull()) {
        printer.printAttribute("publicKey", d_publicKey.value());
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const EncryptionKeyEllipticCurve& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyEllipticCurve& lhs,
                const EncryptionKeyEllipticCurve& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyEllipticCurve& lhs,
                const EncryptionKeyEllipticCurve& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyEllipticCurve& lhs,
               const EncryptionKeyEllipticCurve& rhs)
{
    return lhs.less(rhs);
}

EncryptionKey::EncryptionKey(bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKey::EncryptionKey(const EncryptionKey& original,
                             bslma::Allocator*    basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_RSA) {
        new (d_rsa.buffer())
            ntca::EncryptionKeyRsa(original.d_rsa.object(), d_allocator_p);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        new (d_ellipticCurve.buffer())
            ntca::EncryptionKeyEllipticCurve(original.d_ellipticCurve.object(),
                                             d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionKey::~EncryptionKey()
{
    this->reset();
}

EncryptionKey& EncryptionKey::operator=(const EncryptionKey& other)
{
    if (this != &other) {
        if (other.d_type == e_RSA) {
            this->makeRsa(other.d_rsa.object());
        }
        else if (other.d_type == e_ELLIPTIC_CURVE) {
            this->makeEllipticCurve(other.d_ellipticCurve.object());
        }
        else {
            BSLS_ASSERT(other.d_type == e_UNDEFINED);
            this->reset();
        }
    }

    return *this;
}

void EncryptionKey::reset()
{
    if (d_type == e_RSA) {
        typedef ntca::EncryptionKeyRsa Type;
        d_rsa.object().~Type();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        typedef ntca::EncryptionKeyEllipticCurve Type;
        d_ellipticCurve.object().~Type();
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    d_type = e_UNDEFINED;
}

ntca::EncryptionKeyRsa& EncryptionKey::makeRsa()
{
    if (d_type == e_RSA) {
        d_rsa.object().reset();
    }
    else {
        this->reset();
        new (d_rsa.buffer()) ntca::EncryptionKeyRsa(d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

ntca::EncryptionKeyRsa& EncryptionKey::makeRsa(
    const ntca::EncryptionKeyRsa& value)
{
    if (d_type == e_RSA) {
        d_rsa.object() = value;
    }
    else {
        this->reset();
        new (d_rsa.buffer()) ntca::EncryptionKeyRsa(value, d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

ntca::EncryptionKeyEllipticCurve& EncryptionKey::makeEllipticCurve()
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().reset();
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            ntca::EncryptionKeyEllipticCurve(d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

ntca::EncryptionKeyEllipticCurve& EncryptionKey::makeEllipticCurve(
    const ntca::EncryptionKeyEllipticCurve& value)
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object() = value;
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            ntca::EncryptionKeyEllipticCurve(value, d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

ntca::EncryptionKeyRsa& EncryptionKey::rsa()
{
    BSLS_ASSERT(d_type == e_RSA);
    return d_rsa.object();
}

ntca::EncryptionKeyEllipticCurve& EncryptionKey::ellipticCurve()
{
    BSLS_ASSERT(d_type == e_ELLIPTIC_CURVE);
    return d_ellipticCurve.object();
}

const ntca::EncryptionKeyRsa& EncryptionKey::rsa() const
{
    BSLS_ASSERT(d_type == e_RSA);
    return d_rsa.object();
}

const ntca::EncryptionKeyEllipticCurve& EncryptionKey::ellipticCurve() const
{
    BSLS_ASSERT(d_type == e_ELLIPTIC_CURVE);
    return d_ellipticCurve.object();
}

bool EncryptionKey::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionKey::isRsa() const
{
    return d_type == e_RSA;
}

bool EncryptionKey::isEllipticCurve() const
{
    return d_type == e_ELLIPTIC_CURVE;
}

bool EncryptionKey::equals(const EncryptionKey& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_RSA) {
        return d_rsa.object().equals(other.d_rsa.object());
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object().equals(other.d_ellipticCurve.object());
    }

    return true;
}

bool EncryptionKey::less(const EncryptionKey& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_RSA) {
        return d_rsa.object().less(other.d_rsa.object());
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object().less(other.d_ellipticCurve.object());
    }

    return true;
}

bsl::ostream& EncryptionKey::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    if (d_type == e_RSA) {
        return d_rsa.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object().print(stream, level, spacesPerLevel);
    }

    stream << "UNDEFINED";
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
