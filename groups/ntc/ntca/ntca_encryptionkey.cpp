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


EncryptionKeyRsaValuePrivateExtra::EncryptionKeyRsaValuePrivateExtra(
    bslma::Allocator* basicAllocator)
: d_prime(basicAllocator)
, d_exponent(basicAllocator)
, d_coefficient(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyRsaValuePrivateExtra::EncryptionKeyRsaValuePrivateExtra(
    const EncryptionKeyRsaValuePrivateExtra& original,
    bslma::Allocator*                  basicAllocator)
: d_prime(original.d_prime, basicAllocator)
, d_exponent(original.d_exponent, basicAllocator)
, d_coefficient(original.d_coefficient, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyRsaValuePrivateExtra::~EncryptionKeyRsaValuePrivateExtra()
{
}

EncryptionKeyRsaValuePrivateExtra& EncryptionKeyRsaValuePrivateExtra::operator=(
    const EncryptionKeyRsaValuePrivateExtra& other)
{
    if (this != &other) {
        d_prime = other.d_prime;
        d_exponent = other.d_exponent;
        d_coefficient = other.d_coefficient;
    }

    return *this;
}

void EncryptionKeyRsaValuePrivateExtra::reset()
{
    d_prime.reset();
    d_exponent.reset();
    d_coefficient.reset();
}

void EncryptionKeyRsaValuePrivateExtra::setPrime(const ntsa::AbstractInteger& value)
{
    d_prime = value;
}

void EncryptionKeyRsaValuePrivateExtra::setExponent(const ntsa::AbstractInteger& value)
{
    d_exponent = value;
}


void EncryptionKeyRsaValuePrivateExtra::setCoefficient(const ntsa::AbstractInteger& value)
{
    d_coefficient = value;
}

ntsa::Error EncryptionKeyRsaValuePrivateExtra::decode(
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

ntsa::Error EncryptionKeyRsaValuePrivateExtra::encode(
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

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivateExtra::prime() const
{
    return d_prime;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivateExtra::exponent() const
{
    return d_exponent;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivateExtra::coefficient() const
{
    return d_coefficient;
}

bool EncryptionKeyRsaValuePrivateExtra::equals(
    const EncryptionKeyRsaValuePrivateExtra& other) const
{
    return d_prime == other.d_prime && d_exponent == other.d_exponent && d_coefficient == other.d_coefficient;
}

bool EncryptionKeyRsaValuePrivateExtra::less(
    const EncryptionKeyRsaValuePrivateExtra& other) const
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

bsl::ostream& EncryptionKeyRsaValuePrivateExtra::print(bsl::ostream& stream,
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
                         const EncryptionKeyRsaValuePrivateExtra& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyRsaValuePrivateExtra& lhs,
                const EncryptionKeyRsaValuePrivateExtra& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyRsaValuePrivateExtra& lhs,
                const EncryptionKeyRsaValuePrivateExtra& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyRsaValuePrivateExtra& lhs,
               const EncryptionKeyRsaValuePrivateExtra& rhs)
{
    return lhs.less(rhs);
}











EncryptionKeyRsaValuePrivate::EncryptionKeyRsaValuePrivate(bslma::Allocator* basicAllocator)
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

EncryptionKeyRsaValuePrivate::EncryptionKeyRsaValuePrivate(const EncryptionKeyRsaValuePrivate& original,
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

EncryptionKeyRsaValuePrivate::~EncryptionKeyRsaValuePrivate()
{
}

EncryptionKeyRsaValuePrivate& EncryptionKeyRsaValuePrivate::operator=(const EncryptionKeyRsaValuePrivate& other)
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

void EncryptionKeyRsaValuePrivate::reset()
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

void EncryptionKeyRsaValuePrivate::setVersion(const ntsa::AbstractInteger& value)
{
    d_version = value;
}

void EncryptionKeyRsaValuePrivate::setModulus(const ntsa::AbstractInteger& value)
{
    d_modulus = value;
}

void EncryptionKeyRsaValuePrivate::setEncryptionExponent(const ntsa::AbstractInteger& value)
{
    d_encryptionExponent = value;
}

void EncryptionKeyRsaValuePrivate::setDecryptionExponent(const ntsa::AbstractInteger& value)
{
    d_decryptionExponent = value;
}

void EncryptionKeyRsaValuePrivate::setPrime1(const ntsa::AbstractInteger& value)
{
    d_prime1 = value;
}

void EncryptionKeyRsaValuePrivate::setPrime2(const ntsa::AbstractInteger& value)
{
    d_prime2 = value;
}

void EncryptionKeyRsaValuePrivate::setExponent1(const ntsa::AbstractInteger& value)
{
    d_exponent1 = value;
}

void EncryptionKeyRsaValuePrivate::setExponent2(const ntsa::AbstractInteger& value)
{
    d_exponent2 = value;
}

void EncryptionKeyRsaValuePrivate::setCoefficient(const ntsa::AbstractInteger& value)
{
    d_coefficient = value;
}

void EncryptionKeyRsaValuePrivate::setExtra(
    const bsl::vector<ntca::EncryptionKeyRsaValuePrivateExtra>& value)
{
    d_extra = value;
}

void EncryptionKeyRsaValuePrivate::addExtra(const ntca::EncryptionKeyRsaValuePrivateExtra& value)
{
    d_extra.push_back(value);
}

ntsa::Error EncryptionKeyRsaValuePrivate::decode(ntsa::AbstractSyntaxDecoder* decoder)
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
            ntca::EncryptionKeyRsaValuePrivateExtra extra;

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

ntsa::Error EncryptionKeyRsaValuePrivate::encode(ntsa::AbstractSyntaxEncoder* encoder) const
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
            const ntca::EncryptionKeyRsaValuePrivateExtra& extra = d_extra[i];

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

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::version() const
{
    return d_version;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::modulus() const
{
    return d_modulus;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::encryptionExponent() const
{
    return d_encryptionExponent;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::decryptionExponent() const
{
    return d_decryptionExponent;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::prime1() const
{
    return d_prime1;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::prime2() const
{
    return d_prime2;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::exponent1() const
{
    return d_exponent1;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::exponent2() const
{
    return d_exponent2;
}

const ntsa::AbstractInteger& EncryptionKeyRsaValuePrivate::coefficient() const
{
    return d_coefficient;
}

const bsl::vector<ntca::EncryptionKeyRsaValuePrivateExtra>& EncryptionKeyRsaValuePrivate::extra() const
{
    return d_extra;
}

bool EncryptionKeyRsaValuePrivate::equals(const EncryptionKeyRsaValuePrivate& other) const
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

bool EncryptionKeyRsaValuePrivate::less(const EncryptionKeyRsaValuePrivate& other) const
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

bsl::ostream& EncryptionKeyRsaValuePrivate::print(bsl::ostream& stream,
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

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionKeyRsaValuePrivate& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyRsaValuePrivate& lhs, const EncryptionKeyRsaValuePrivate& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyRsaValuePrivate& lhs, const EncryptionKeyRsaValuePrivate& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyRsaValuePrivate& lhs, const EncryptionKeyRsaValuePrivate& rhs)
{
    return lhs.less(rhs);
}









EncryptionKeyRsaValuePublic::EncryptionKeyRsaValuePublic(
    bslma::Allocator* basicAllocator)
: d_modulus(basicAllocator)
, d_encryptionExponent(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyRsaValuePublic::EncryptionKeyRsaValuePublic(
    const EncryptionKeyRsaValuePublic& original,
    bslma::Allocator*                             basicAllocator)
: d_modulus(original.d_modulus, basicAllocator)
, d_encryptionExponent(original.d_encryptionExponent, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyRsaValuePublic::
    ~EncryptionKeyRsaValuePublic()
{
}

EncryptionKeyRsaValuePublic& EncryptionKeyRsaValuePublic::
operator=(const EncryptionKeyRsaValuePublic& other)
{
    if (this != &other) {
        d_modulus            = other.d_modulus;
        d_encryptionExponent = other.d_encryptionExponent;
    }

    return *this;
}

void EncryptionKeyRsaValuePublic::reset()
{
    d_modulus.reset();
    d_encryptionExponent.reset();
}

ntsa::Error EncryptionKeyRsaValuePublic::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
    if (error) {
        return error;
    }

    bsl::uint8_t numBitsOmitted = 0;

    error = decoder->decodeByte(&numBitsOmitted);
    if (error) {
        return error;
    }

    if (numBitsOmitted != 0) {
        return error;
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
        if (error) {
            return error;
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

ntsa::Error EncryptionKeyRsaValuePublic::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
    if (error) {
        return error;
    }

    bsl::uint8_t numBitsOmitted = 0;

    error = encoder->encodeLiteral(numBitsOmitted);
    if (error) {
        return error;
    }

    if (numBitsOmitted != 0) {
        return error;
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
        if (error) {
            return error;
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

bool EncryptionKeyRsaValuePublic::equals(
    const EncryptionKeyRsaValuePublic& other) const
{
    return d_modulus == other.d_modulus &&
           d_encryptionExponent == other.d_encryptionExponent;
}

bool EncryptionKeyRsaValuePublic::less(
    const EncryptionKeyRsaValuePublic& other) const
{
    if (d_modulus < other.d_modulus) {
        return true;
    }

    if (other.d_modulus < d_modulus) {
        return false;
    }

    return d_encryptionExponent < other.d_encryptionExponent;
}

bsl::ostream& EncryptionKeyRsaValuePublic::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    // d_value.print(stream, level, spacesPerLevel);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("modulus", d_modulus);
    printer.printAttribute("encryptionExponent", d_encryptionExponent);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                                 stream,
                         const EncryptionKeyRsaValuePublic& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyRsaValuePublic& lhs,
                const EncryptionKeyRsaValuePublic& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyRsaValuePublic& lhs,
                const EncryptionKeyRsaValuePublic& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyRsaValuePublic& lhs,
               const EncryptionKeyRsaValuePublic& rhs)
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













EncryptionKeyEllipticCurveValuePrivate::EncryptionKeyEllipticCurveValuePrivate(
    bslma::Allocator* basicAllocator)
: d_version(basicAllocator)
, d_privateKey(basicAllocator)
, d_parameters(basicAllocator)
, d_publicKey(basicAllocator)
{
    d_version.assign(1);
}

EncryptionKeyEllipticCurveValuePrivate::EncryptionKeyEllipticCurveValuePrivate(
    const EncryptionKeyEllipticCurveValuePrivate& original,
    bslma::Allocator*                 basicAllocator)
: d_version(original.d_version)
, d_privateKey(original.d_privateKey, basicAllocator)
, d_parameters(original.d_parameters, basicAllocator)
, d_publicKey(original.d_publicKey, basicAllocator)
{
}

EncryptionKeyEllipticCurveValuePrivate::~EncryptionKeyEllipticCurveValuePrivate()
{
}

EncryptionKeyEllipticCurveValuePrivate& EncryptionKeyEllipticCurveValuePrivate::operator=(
    const EncryptionKeyEllipticCurveValuePrivate& other)
{
    if (this != &other) {
        d_version    = other.d_version;
        d_privateKey = other.d_privateKey;
        d_parameters = other.d_parameters;
        d_publicKey  = other.d_publicKey;
    }

    return *this;
}

void EncryptionKeyEllipticCurveValuePrivate::reset()
{
    d_version.reset();
    d_version.assign(1);

    d_privateKey.reset();
    d_parameters.reset();
    d_publicKey.reset();
}

ntsa::Error EncryptionKeyEllipticCurveValuePrivate::decode(
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

ntsa::Error EncryptionKeyEllipticCurveValuePrivate::encode(
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

bool EncryptionKeyEllipticCurveValuePrivate::equals(
    const EncryptionKeyEllipticCurveValuePrivate& other) const
{
    return d_version == other.d_version &&
           d_privateKey == other.d_privateKey &&
           d_parameters == other.d_parameters &&
           d_publicKey == other.d_publicKey;
}

bool EncryptionKeyEllipticCurveValuePrivate::less(
    const EncryptionKeyEllipticCurveValuePrivate& other) const
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

bsl::ostream& EncryptionKeyEllipticCurveValuePrivate::print(bsl::ostream& stream,
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
                         const EncryptionKeyEllipticCurveValuePrivate& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyEllipticCurveValuePrivate& lhs,
                const EncryptionKeyEllipticCurveValuePrivate& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyEllipticCurveValuePrivate& lhs,
                const EncryptionKeyEllipticCurveValuePrivate& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyEllipticCurveValuePrivate& lhs,
               const EncryptionKeyEllipticCurveValuePrivate& rhs)
{
    return lhs.less(rhs);
}









EncryptionKeyEllipticCurveValuePublic::
    EncryptionKeyEllipticCurveValuePublic(
        bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyEllipticCurveValuePublic::
    EncryptionKeyEllipticCurveValuePublic(
        const EncryptionKeyEllipticCurveValuePublic& original,
        bslma::Allocator*                                       basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyEllipticCurveValuePublic::
    ~EncryptionKeyEllipticCurveValuePublic()
{
}

EncryptionKeyEllipticCurveValuePublic&
EncryptionKeyEllipticCurveValuePublic::operator=(
    const EncryptionKeyEllipticCurveValuePublic& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionKeyEllipticCurveValuePublic::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionKeyEllipticCurveValuePublic::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
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

ntsa::Error EncryptionKeyEllipticCurveValuePublic::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
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

bool EncryptionKeyEllipticCurveValuePublic::equals(
    const EncryptionKeyEllipticCurveValuePublic& other) const
{
    return d_value == other.d_value;
}

bool EncryptionKeyEllipticCurveValuePublic::less(
    const EncryptionKeyEllipticCurveValuePublic& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionKeyEllipticCurveValuePublic::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bsl::ostream::fmtflags previousFlags = stream.flags();
    stream.setf(previousFlags | bsl::ostream::hex);
    d_value.print(stream, level, spacesPerLevel);
    stream.flags(previousFlags);
    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                           stream,
    const EncryptionKeyEllipticCurveValuePublic& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyEllipticCurveValuePublic& lhs,
                const EncryptionKeyEllipticCurveValuePublic& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyEllipticCurveValuePublic& lhs,
                const EncryptionKeyEllipticCurveValuePublic& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyEllipticCurveValuePublic& lhs,
               const EncryptionKeyEllipticCurveValuePublic& rhs)
{
    return lhs.less(rhs);
}













const char* EncryptionKeyAlgorithmIdentifierType::toString(
    Value value)
{
    switch (value) {
    case e_RSA:
        return "RSA";
    case e_ELLIPTIC_CURVE:
        return "ELLIPTIC_CURVE";
    case e_EDWARDS_CURVE_25519:
        return "EDWARDS_CURVE_25519";
    case e_EDWARDS_CURVE_448:
        return "EDWARDS_CURVE_448";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionKeyAlgorithmIdentifierType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result,
    Value                           value)
{
    result->reset();

    if (value == e_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 1);
    }
    else if (value == e_ELLIPTIC_CURVE) {
        result->set(1, 2, 840, 10045, 2, 1);
    }
    else if (value == e_EDWARDS_CURVE_25519) {
        result->set(1, 3, 101, 112);
    }
    else if (value == e_EDWARDS_CURVE_448) {
        result->set(1, 3, 101, 113);
    }
}

int EncryptionKeyAlgorithmIdentifierType::fromString(
    Value*                   result,
    const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "RSA")) {
        *result = e_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ELLIPTIC_CURVE")) {
        *result = e_ELLIPTIC_CURVE;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "EDWARDS_CURVE_25519")) {
        *result = e_EDWARDS_CURVE_25519;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "EDWARDS_CURVE_448")) {
        *result = e_EDWARDS_CURVE_448;
        return 0;
    }

    return -1;
}

int EncryptionKeyAlgorithmIdentifierType::
    fromObjectIdentifier(Value*                                result,
                         const ntsa::AbstractObjectIdentifier& identifier)
{
    if (identifier.equals(1, 2, 840, 113549, 1, 1, 1)) {
        *result = e_RSA;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 10045, 2, 1)) {
        *result = e_ELLIPTIC_CURVE;
        return 0;
    }

    if (identifier.equals(1, 3, 101, 112)) {
        *result = e_EDWARDS_CURVE_25519;
        return 0;
    }

    if (identifier.equals(1, 3, 101, 113)) {
        *result = e_EDWARDS_CURVE_448;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionKeyAlgorithmIdentifierType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                                stream,
    EncryptionKeyAlgorithmIdentifierType::Value rhs)
{
    return EncryptionKeyAlgorithmIdentifierType::print(stream,
                                                                        rhs);
}

EncryptionKeyAlgorithmIdentifier::
    EncryptionKeyAlgorithmIdentifier(
        bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyAlgorithmIdentifier::
    EncryptionKeyAlgorithmIdentifier(
        const EncryptionKeyAlgorithmIdentifier& original,
        bslma::Allocator* basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyAlgorithmIdentifier::
    ~EncryptionKeyAlgorithmIdentifier()
{
}

EncryptionKeyAlgorithmIdentifier&
EncryptionKeyAlgorithmIdentifier::operator=(
    const EncryptionKeyAlgorithmIdentifier& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionKeyAlgorithmIdentifier&
EncryptionKeyAlgorithmIdentifier::operator=(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionKeyAlgorithmIdentifier&
EncryptionKeyAlgorithmIdentifier::operator=(
    EncryptionKeyAlgorithmIdentifierType::Value value)
{
    EncryptionKeyAlgorithmIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
    return *this;
}

void EncryptionKeyAlgorithmIdentifier::reset()
{
    d_identifier.reset();
}

void EncryptionKeyAlgorithmIdentifier::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionKeyAlgorithmIdentifier::setIdentifer(
    EncryptionKeyAlgorithmIdentifierType::Value value)
{
    EncryptionKeyAlgorithmIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
}

ntsa::Error EncryptionKeyAlgorithmIdentifier::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
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

ntsa::Error EncryptionKeyAlgorithmIdentifier::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
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
EncryptionKeyAlgorithmIdentifier::identifier() const
{
    return d_identifier;
}

bool EncryptionKeyAlgorithmIdentifier::equals(
    const EncryptionKeyAlgorithmIdentifier& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionKeyAlgorithmIdentifier::equals(
    EncryptionKeyAlgorithmIdentifierType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionKeyAlgorithmIdentifierType::toObjectIdentifier(
        &identifier,
        value);

    return d_identifier.equals(identifier);
}

bool EncryptionKeyAlgorithmIdentifier::less(
    const EncryptionKeyAlgorithmIdentifier& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionKeyAlgorithmIdentifier::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionKeyAlgorithmIdentifierType::Value type;
    int rc = ntca::EncryptionKeyAlgorithmIdentifierType::
        fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionKeyAlgorithmIdentifierType::
                toString(type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                            stream,
    const EncryptionKeyAlgorithmIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyAlgorithmIdentifier& lhs,
                const EncryptionKeyAlgorithmIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyAlgorithmIdentifier& lhs,
                const EncryptionKeyAlgorithmIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyAlgorithmIdentifier& lhs,
               const EncryptionKeyAlgorithmIdentifier& rhs)
{
    return lhs.less(rhs);
}







EncryptionKeyAlgorithmParameters::
    EncryptionKeyAlgorithmParameters(
        bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyAlgorithmParameters::
    EncryptionKeyAlgorithmParameters(
        const EncryptionKeyAlgorithmParameters& original,
        bslma::Allocator* basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_RSA) {
        new (d_rsa.buffer())
            EncryptionKeyRsaParameters(
                original.d_rsa.object(),
                d_allocator_p);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveParameters(
                original.d_ellipticCurve.object(),
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

EncryptionKeyAlgorithmParameters::
    ~EncryptionKeyAlgorithmParameters()
{
    this->reset();
}

EncryptionKeyAlgorithmParameters&
EncryptionKeyAlgorithmParameters::operator=(
    const EncryptionKeyAlgorithmParameters& other)
{
    if (this == &other) {
        return *this;
    }

    if (other.d_type == e_RSA) {
        this->makeRsa(other.d_rsa.object());
    }
    else if (other.d_type == e_ELLIPTIC_CURVE) {
        this->makeEllipticCurve(other.d_ellipticCurve.object());
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

void EncryptionKeyAlgorithmParameters::reset()
{
    if (d_type == e_RSA) {
        d_rsa.object().~RsaType();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().~EllipticCurveType();
    }
    else if (d_type == e_ANY) {
        typedef ntsa::AbstractValue Type;
        d_any.object().~Type();
    }

    d_type = e_UNDEFINED;
}

EncryptionKeyRsaParameters&
EncryptionKeyAlgorithmParameters::makeRsa()
{
    if (d_type == e_RSA) {
        d_rsa.object().reset();
    }
    else {
        this->reset();
        new (d_rsa.buffer())
            EncryptionKeyRsaParameters(
                d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

EncryptionKeyRsaParameters&
EncryptionKeyAlgorithmParameters::makeRsa(
    const EncryptionKeyRsaParameters& value)
{
    if (d_type == e_RSA) {
        d_rsa.object() = value;
    }
    else {
        this->reset();
        new (d_rsa.buffer())
            EncryptionKeyRsaParameters(
                value,
                d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

EncryptionKeyEllipticCurveParameters&
EncryptionKeyAlgorithmParameters::makeEllipticCurve()
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().reset();
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveParameters(
                d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

EncryptionKeyEllipticCurveParameters&
EncryptionKeyAlgorithmParameters::makeEllipticCurve(
    const EncryptionKeyEllipticCurveParameters&
        value)
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object() = value;
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveParameters(
                value,
                d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

ntsa::AbstractValue& EncryptionKeyAlgorithmParameters::
    makeAny()
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

ntsa::AbstractValue& EncryptionKeyAlgorithmParameters::
    makeAny(const ntsa::AbstractValue& value)
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

EncryptionKeyRsaParameters&
EncryptionKeyAlgorithmParameters::rsa()
{
    BSLS_ASSERT(this->isRsa());
    return d_rsa.object();
}

EncryptionKeyEllipticCurveParameters&
EncryptionKeyAlgorithmParameters::ellipticCurve()
{
    BSLS_ASSERT(this->isEllipticCurve());
    return d_ellipticCurve.object();
}

ntsa::AbstractValue& EncryptionKeyAlgorithmParameters::any()
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

const EncryptionKeyRsaParameters&
EncryptionKeyAlgorithmParameters::rsa() const
{
    BSLS_ASSERT(this->isRsa());
    return d_rsa.object();
}

const EncryptionKeyEllipticCurveParameters&
EncryptionKeyAlgorithmParameters::ellipticCurve() const
{
    BSLS_ASSERT(this->isEllipticCurve());
    return d_ellipticCurve.object();
}

const ntsa::AbstractValue& EncryptionKeyAlgorithmParameters::
    any() const
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

bool EncryptionKeyAlgorithmParameters::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionKeyAlgorithmParameters::isRsa() const
{
    return d_type == e_RSA;
}

bool EncryptionKeyAlgorithmParameters::isEllipticCurve() const
{
    return d_type == e_ELLIPTIC_CURVE;
}

bool EncryptionKeyAlgorithmParameters::isAny() const
{
    return d_type == e_ANY;
}

bool EncryptionKeyAlgorithmParameters::equals(
    const EncryptionKeyAlgorithmParameters& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_UNDEFINED) {
        return true;
    }
    else if (d_type == e_RSA) {
        return d_rsa.object() == other.d_rsa.object();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object() == other.d_ellipticCurve.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() == other.d_any.object();
    }

    return false;
}

bool EncryptionKeyAlgorithmParameters::less(
    const EncryptionKeyAlgorithmParameters& other) const
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
    else if (d_type == e_RSA) {
        return d_rsa.object() < other.d_rsa.object();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object() < other.d_ellipticCurve.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() < other.d_any.object();
    }

    return false;
}

bsl::ostream& EncryptionKeyAlgorithmParameters::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    if (d_type == e_RSA) {
        stream << d_rsa.object();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        stream << d_ellipticCurve.object();
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
    bsl::ostream&                                            stream,
    const EncryptionKeyAlgorithmParameters& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyAlgorithmParameters& lhs,
                const EncryptionKeyAlgorithmParameters& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyAlgorithmParameters& lhs,
                const EncryptionKeyAlgorithmParameters& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyAlgorithmParameters& lhs,
               const EncryptionKeyAlgorithmParameters& rhs)
{
    return lhs.less(rhs);
}

EncryptionKeyAlgorithm::
    EncryptionKeyAlgorithm(bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_parameters(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyAlgorithm::
    EncryptionKeyAlgorithm(
        const EncryptionKeyAlgorithm& original,
        bslma::Allocator*                              basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_parameters(original.d_parameters, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyAlgorithm::
    ~EncryptionKeyAlgorithm()
{
}

EncryptionKeyAlgorithm& EncryptionKeyAlgorithm::
operator=(const EncryptionKeyAlgorithm& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
        d_parameters = other.d_parameters;
    }

    return *this;
}

void EncryptionKeyAlgorithm::reset()
{
    d_identifier.reset();
    d_parameters.reset();
}

void EncryptionKeyAlgorithm::setIdentifier(
    const EncryptionKeyAlgorithmIdentifier& value)
{
    d_identifier = value;
}

void EncryptionKeyAlgorithm::setParameters(
    const EncryptionKeyAlgorithmParameters& value)
{
    d_parameters = value;
}

ntsa::Error EncryptionKeyAlgorithm::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = d_identifier.decode(decoder);
    if (error) {
        return error;
    }

    if (decoder->contentBytesRemaining() > 0) {
        if (d_identifier.equals(
                EncryptionKeyAlgorithmIdentifierType::e_RSA))
        {
            error = d_parameters.makeValue().makeRsa().decode(decoder);
            if (error) {
                return error;
            }
        }
        else if (d_identifier.equals(
                     EncryptionKeyAlgorithmIdentifierType::
                         e_ELLIPTIC_CURVE))
        {
            error = decoder->decodeTag();
            if (error) {
                return error;
            }

            if (decoder->current().tagClass() == k_UNIVERSAL &&
                decoder->current().tagType() == k_PRIMITIVE &&
                decoder->current().tagNumber() == k_NULL)
            {
                d_parameters.reset();

                error = decoder->decodeTagComplete();
                if (error) {
                    return error;
                }
            }
            else if (decoder->current().tagClass() == k_UNIVERSAL &&
                     decoder->current().tagType() == k_PRIMITIVE &&
                     decoder->current().tagNumber() == k_OBJECT_IDENTIFIER)
            {
                error = decoder->rewindTag();
                if (error) {
                    return error;
                }

                error = d_parameters.makeValue()
                            .makeEllipticCurve()
                            .makeIdentifier()
                            .decode(decoder);
                if (error) {
                    return error;
                }
            }
            else {
                error = decoder->decodeValue(
                    &d_parameters.makeValue().makeEllipticCurve().makeAny());
                if (error) {
                    return error;
                }

                error = decoder->decodeTagComplete();
                if (error) {
                    return error;
                }
            }
        }
        else {
            error = decoder->decodeTag();
            if (error) {
                return error;
            }

            if (decoder->current().tagClass() == k_UNIVERSAL &&
                decoder->current().tagType() == k_PRIMITIVE &&
                decoder->current().tagNumber() == k_NULL)
            {
                d_parameters.reset();
            }
            else {
                error =
                    decoder->decodeValue(&d_parameters.makeValue().makeAny());
                if (error) {
                    return error;
                }
            }

            error = decoder->decodeTagComplete();
            if (error) {
                return error;
            }
        }
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionKeyAlgorithm::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = d_identifier.encode(encoder);
    if (error) {
        return error;
    }

    if (!d_parameters.isNull()) {
        if (d_parameters.value().isRsa()) {
            error = d_parameters.value().rsa().encode(encoder);
            if (error) {
                return error;
            }
        }
        else if (d_parameters.value().isEllipticCurve()) {
            if (d_parameters.value().ellipticCurve().isIdentifier()) {
                error = d_parameters.value().ellipticCurve().identifier().encode(encoder);
                if (error) {
                    return error;
                }
            }
            else if (d_parameters.value().ellipticCurve().isAny()) {
                error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OCTET_STRING);
                if (error) {
                    return error;
                }

                error = encoder->encodeValue(d_parameters.value().ellipticCurve().any());
                if (error) {
                    return error;
                }

                error = encoder->encodeTagComplete();
                if (error) {
                    return error;
                }
            }
        }
        else if (d_parameters.value().isAny()) {
            error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OCTET_STRING);
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
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

const EncryptionKeyAlgorithmIdentifier&
EncryptionKeyAlgorithm::identifier() const
{
    return d_identifier;
}

const bdlb::NullableValue<EncryptionKeyAlgorithmParameters>&
EncryptionKeyAlgorithm::parameters() const
{
    return d_parameters;
}

bool EncryptionKeyAlgorithm::equals(
    const EncryptionKeyAlgorithm& other) const
{
    return d_identifier == other.d_identifier &&
           d_parameters == other.d_parameters;
}

bool EncryptionKeyAlgorithm::less(
    const EncryptionKeyAlgorithm& other) const
{
    if (d_identifier < other.d_identifier) {
        return true;
    }

    if (other.d_identifier < d_identifier) {
        return false;
    }

    return d_parameters < other.d_parameters;
}

bsl::ostream& EncryptionKeyAlgorithm::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("identifier", d_identifier);

    if (!d_parameters.isNull()) {
        if (!d_parameters.value().isUndefined()) {
            printer.printAttribute("parameters", d_parameters);
        }
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                                  stream,
                         const EncryptionKeyAlgorithm& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyAlgorithm& lhs,
                const EncryptionKeyAlgorithm& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyAlgorithm& lhs,
                const EncryptionKeyAlgorithm& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyAlgorithm& lhs,
               const EncryptionKeyAlgorithm& rhs)
{
    return lhs.less(rhs);
}












EncryptionKeyValuePrivate::EncryptionKeyValuePrivate(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyValuePrivate::EncryptionKeyValuePrivate(
    const EncryptionKeyValuePrivate& original,
    bslma::Allocator*                          basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_RSA) {
        new (d_rsa.buffer())
            EncryptionKeyRsaValuePrivate(original.d_rsa.object(),
                                                   d_allocator_p);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveValuePrivate(
                original.d_ellipticCurve.object(),
                d_allocator_p);
    }
    else if (d_type == e_ANY) {
        new (d_any.buffer())
            ntsa::AbstractOctetString(original.d_any.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionKeyValuePrivate::~EncryptionKeyValuePrivate()
{
    this->reset();
}

EncryptionKeyValuePrivate& EncryptionKeyValuePrivate::
operator=(const EncryptionKeyValuePrivate& other)
{
    if (this == &other) {
        return *this;
    }

    if (other.d_type == e_RSA) {
        this->makeRsa(other.d_rsa.object());
    }
    else if (other.d_type == e_ELLIPTIC_CURVE) {
        this->makeEllipticCurve(other.d_ellipticCurve.object());
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

void EncryptionKeyValuePrivate::reset()
{
    if (d_type == e_RSA) {
        d_rsa.object().~RsaType();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().~EllipticCurveType();
    }
    else if (d_type == e_ANY) {
        d_any.object().~AnyType();
    }

    d_type = e_UNDEFINED;
}

EncryptionKeyRsaValuePrivate& EncryptionKeyValuePrivate::
    makeRsa()
{
    if (d_type == e_RSA) {
        d_rsa.object().reset();
    }
    else {
        this->reset();
        new (d_rsa.buffer())
            EncryptionKeyRsaValuePrivate(d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

EncryptionKeyRsaValuePrivate& EncryptionKeyValuePrivate::
    makeRsa(const EncryptionKeyRsaValuePrivate& value)
{
    if (d_type == e_RSA) {
        d_rsa.object() = value;
    }
    else {
        this->reset();
        new (d_rsa.buffer())
            EncryptionKeyRsaValuePrivate(value, d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

EncryptionKeyEllipticCurveValuePrivate&
EncryptionKeyValuePrivate::makeEllipticCurve()
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().reset();
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveValuePrivate(d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

EncryptionKeyEllipticCurveValuePrivate&
EncryptionKeyValuePrivate::makeEllipticCurve(
    const EncryptionKeyEllipticCurveValuePrivate& value)
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object() = value;
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveValuePrivate(value,
                                                             d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

ntsa::AbstractOctetString& EncryptionKeyValuePrivate::makeAny()
{
    if (d_type == e_ANY) {
        d_any.object().reset();
    }
    else {
        this->reset();
        new (d_any.buffer()) ntsa::AbstractOctetString(d_allocator_p);
        d_type = e_ANY;
    }

    return d_any.object();
}

ntsa::AbstractOctetString& EncryptionKeyValuePrivate::makeAny(
    const ntsa::AbstractOctetString& value)
{
    if (d_type == e_ANY) {
        d_any.object() = value;
    }
    else {
        this->reset();
        new (d_any.buffer()) ntsa::AbstractOctetString(value, d_allocator_p);
        d_type = e_ANY;
    }

    return d_any.object();
}

EncryptionKeyRsaValuePrivate& EncryptionKeyValuePrivate::
    rsa()
{
    BSLS_ASSERT(this->isRsa());
    return d_rsa.object();
}

EncryptionKeyEllipticCurveValuePrivate&
EncryptionKeyValuePrivate::ellipticCurve()
{
    BSLS_ASSERT(this->isEllipticCurve());
    return d_ellipticCurve.object();
}

ntsa::AbstractOctetString& EncryptionKeyValuePrivate::any()
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

const EncryptionKeyRsaValuePrivate&
EncryptionKeyValuePrivate::rsa() const
{
    BSLS_ASSERT(this->isRsa());
    return d_rsa.object();
}

const EncryptionKeyEllipticCurveValuePrivate&
EncryptionKeyValuePrivate::ellipticCurve() const
{
    BSLS_ASSERT(this->isEllipticCurve());
    return d_ellipticCurve.object();
}

const ntsa::AbstractOctetString& EncryptionKeyValuePrivate::any() const
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

bool EncryptionKeyValuePrivate::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionKeyValuePrivate::isRsa() const
{
    return d_type == e_RSA;
}

bool EncryptionKeyValuePrivate::isEllipticCurve() const
{
    return d_type == e_ELLIPTIC_CURVE;
}

bool EncryptionKeyValuePrivate::isAny() const
{
    return d_type == e_ANY;
}

bool EncryptionKeyValuePrivate::equals(
    const EncryptionKeyValuePrivate& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_UNDEFINED) {
        return true;
    }
    else if (d_type == e_RSA) {
        return d_rsa.object() == other.d_rsa.object();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object() == other.d_ellipticCurve.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() == other.d_any.object();
    }

    return false;
}

bool EncryptionKeyValuePrivate::less(
    const EncryptionKeyValuePrivate& other) const
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
    else if (d_type == e_RSA) {
        return d_rsa.object() < other.d_rsa.object();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object() < other.d_ellipticCurve.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() < other.d_any.object();
    }

    return false;
}

bsl::ostream& EncryptionKeyValuePrivate::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    if (d_type == e_RSA) {
        d_rsa.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_ANY) {
        d_any.object().print(stream, level, spacesPerLevel);
    }
    else {
        stream << "UNDEFINED";
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                              stream,
                         const EncryptionKeyValuePrivate& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyValuePrivate& lhs,
                const EncryptionKeyValuePrivate& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyValuePrivate& lhs,
                const EncryptionKeyValuePrivate& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyValuePrivate& lhs,
               const EncryptionKeyValuePrivate& rhs)
{
    return lhs.less(rhs);
}

























EncryptionKeyInfoPrivate::EncryptionKeyInfoPrivate(
    bslma::Allocator* basicAllocator)
: d_version(basicAllocator)
, d_algorithm(basicAllocator)
, d_privateKey(basicAllocator)
, d_attributes(basicAllocator)
, d_publicKey(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyInfoPrivate::EncryptionKeyInfoPrivate(
    const EncryptionKeyInfoPrivate& original,
    bslma::Allocator*                         basicAllocator)
: d_version(original.d_version, basicAllocator)
, d_algorithm(original.d_algorithm, basicAllocator)
, d_privateKey(original.d_privateKey, basicAllocator)
, d_attributes(original.d_attributes, basicAllocator)
, d_publicKey(original.d_publicKey, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyInfoPrivate::~EncryptionKeyInfoPrivate()
{
}

EncryptionKeyInfoPrivate& EncryptionKeyInfoPrivate::
operator=(const EncryptionKeyInfoPrivate& other)
{
    if (this != &other) {
        d_version = other.d_version;
        d_algorithm = other.d_algorithm;
        d_privateKey     = other.d_privateKey;
        d_attributes = other.d_attributes;
        d_publicKey = other.d_publicKey;
    }

    return *this;
}

void EncryptionKeyInfoPrivate::reset()
{
    d_version.reset();
    d_algorithm.reset();
    d_privateKey.reset();
    d_attributes.reset();
    d_publicKey.reset();
}

ntsa::Error EncryptionKeyInfoPrivate::decode(
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

    error = d_algorithm.decode(decoder);
    if (error) {
        return error;
    }

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OCTET_STRING);
    if (error) {
        return error;
    }

    if (d_algorithm.identifier().equals(
            ntca::EncryptionKeyAlgorithmIdentifierType::
                e_RSA))
    {
        error = d_privateKey.makeRsa().decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_algorithm.identifier().equals(
                 ntca::EncryptionKeyAlgorithmIdentifierType::
                     e_ELLIPTIC_CURVE))
    {
        error = d_privateKey.makeEllipticCurve().decode(decoder);
        if (error) {
            return error;
        }
    }
    else {
        error = decoder->decodeValue(&d_privateKey.makeAny());
        if (error) {
            return error;
        }
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    bool wantAttributes = true;
    bool wantPublicKey = true;

    while (decoder->contentBytesRemaining() > 0) {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagClass() == k_CONTEXT_SPECIFIC &&
            decoder->current().tagType() == k_CONSTRUCTED &&
            decoder->current().tagNumber() == 0 && wantAttributes)
        {
            error = decoder->decodeTag();
            if (error) {
                return error;
            }

            error = decoder->decodeValue(&d_attributes.makeValue());
            if (error) {
                return error;
            }

            error = decoder->decodeTagComplete();
            if (error) {
                return error;
            }

            wantAttributes = false;
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

ntsa::Error EncryptionKeyInfoPrivate::encode(
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
        error = d_algorithm.encode(encoder);
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OCTET_STRING);
        if (error) {
            return error;
        }

        if (d_privateKey.isRsa()) {
            error = d_privateKey.rsa().encode(encoder);
            if (error) {
                return error;
            }
        }
        else if (d_privateKey.isEllipticCurve()) {
            error = d_privateKey.ellipticCurve().encode(encoder);
            if (error) {
                return error;
            }
        }
        else if (d_privateKey.isAny()) {
            error = encoder->encodeValue(d_privateKey.any());
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

    if (!d_attributes.isNull()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, 0);
        if (error) {
            return error;
        }

        error = encoder->encodeTag(d_attributes.value().tagClass(), 
                                   d_attributes.value().tagType(), 
                                   d_attributes.value().tagNumber());
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_attributes.value());
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

const ntca::EncryptionKeyAlgorithm& 
EncryptionKeyInfoPrivate::algorithm() const
{
    return d_algorithm;
}

const ntca::EncryptionKeyValuePrivate& 
EncryptionKeyInfoPrivate::privateKey() const
{
    return d_privateKey;
}

bool EncryptionKeyInfoPrivate::equals(
    const EncryptionKeyInfoPrivate& other) const
{
    return d_version == other.d_version && 
           d_algorithm == other.d_algorithm && 
           d_privateKey == other.d_privateKey &&
           d_attributes == other.d_attributes &&
           d_publicKey == other.d_publicKey;
}

bool EncryptionKeyInfoPrivate::less(
    const EncryptionKeyInfoPrivate& other) const
{
    if (d_version < other.d_version) {
        return true;
    }

    if (other.d_version < d_version) {
        return false;
    }

    if (d_algorithm < other.d_algorithm) {
        return true;
    }

    if (other.d_algorithm < d_algorithm) {
        return false;
    }

    if (d_privateKey < other.d_privateKey) {
        return true;
    }

    if (other.d_privateKey < d_privateKey) {
        return false;
    }

    if (d_attributes < other.d_attributes) {
        return true;
    }

    if (other.d_attributes < d_attributes) {
        return false;
    }

    return d_publicKey < other.d_publicKey;
}

bsl::ostream& EncryptionKeyInfoPrivate::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("version", d_version);
    printer.printAttribute("algorithm", d_algorithm);
    printer.printAttribute("privateKey", d_privateKey);

    if (!d_attributes.isNull()) {
        printer.printAttribute("attributes", d_attributes);
    }

    if (!d_publicKey.isNull()) {
        printer.printAttribute("publicKey", d_publicKey);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                             stream,
                         const EncryptionKeyInfoPrivate& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyInfoPrivate& lhs,
                const EncryptionKeyInfoPrivate& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyInfoPrivate& lhs,
                const EncryptionKeyInfoPrivate& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyInfoPrivate& lhs,
               const EncryptionKeyInfoPrivate& rhs)
{
    return lhs.less(rhs);
}






































EncryptionKeyValuePublic::EncryptionKeyValuePublic(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyValuePublic::EncryptionKeyValuePublic(
    const EncryptionKeyValuePublic& original,
    bslma::Allocator*                          basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_RSA) {
        new (d_rsa.buffer())
            EncryptionKeyRsaValuePublic(original.d_rsa.object(),
                                                   d_allocator_p);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveValuePublic(
                original.d_ellipticCurve.object(),
                d_allocator_p);
    }
    else if (d_type == e_ANY) {
        new (d_any.buffer())
            ntsa::AbstractBitString(original.d_any.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionKeyValuePublic::~EncryptionKeyValuePublic()
{
    this->reset();
}

EncryptionKeyValuePublic& EncryptionKeyValuePublic::
operator=(const EncryptionKeyValuePublic& other)
{
    if (this == &other) {
        return *this;
    }

    if (other.d_type == e_RSA) {
        this->makeRsa(other.d_rsa.object());
    }
    else if (other.d_type == e_ELLIPTIC_CURVE) {
        this->makeEllipticCurve(other.d_ellipticCurve.object());
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

void EncryptionKeyValuePublic::reset()
{
    if (d_type == e_RSA) {
        typedef EncryptionKeyRsaValuePublic Type;
        d_rsa.object().~Type();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        typedef EncryptionKeyEllipticCurveValuePublic Type;
        d_ellipticCurve.object().~Type();
    }
    else if (d_type == e_ANY) {
        typedef ntsa::AbstractBitString Type;
        d_any.object().~Type();
    }

    d_type = e_UNDEFINED;
}

EncryptionKeyRsaValuePublic& EncryptionKeyValuePublic::
    makeRsa()
{
    if (d_type == e_RSA) {
        d_rsa.object().reset();
    }
    else {
        this->reset();
        new (d_rsa.buffer())
            EncryptionKeyRsaValuePublic(d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

EncryptionKeyRsaValuePublic& EncryptionKeyValuePublic::
    makeRsa(const EncryptionKeyRsaValuePublic& value)
{
    if (d_type == e_RSA) {
        d_rsa.object() = value;
    }
    else {
        this->reset();
        new (d_rsa.buffer())
            EncryptionKeyRsaValuePublic(value, d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

EncryptionKeyEllipticCurveValuePublic&
EncryptionKeyValuePublic::makeEllipticCurve()
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().reset();
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveValuePublic(d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

EncryptionKeyEllipticCurveValuePublic&
EncryptionKeyValuePublic::makeEllipticCurve(
    const EncryptionKeyEllipticCurveValuePublic& value)
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object() = value;
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            EncryptionKeyEllipticCurveValuePublic(value,
                                                             d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

ntsa::AbstractBitString& EncryptionKeyValuePublic::makeAny()
{
    if (d_type == e_ANY) {
        d_any.object().reset();
    }
    else {
        this->reset();
        new (d_any.buffer()) ntsa::AbstractBitString(d_allocator_p);
        d_type = e_ANY;
    }

    return d_any.object();
}

ntsa::AbstractBitString& EncryptionKeyValuePublic::makeAny(
    const ntsa::AbstractBitString& value)
{
    if (d_type == e_ANY) {
        d_any.object() = value;
    }
    else {
        this->reset();
        new (d_any.buffer()) ntsa::AbstractBitString(value, d_allocator_p);
        d_type = e_ANY;
    }

    return d_any.object();
}

EncryptionKeyRsaValuePublic& EncryptionKeyValuePublic::
    rsa()
{
    BSLS_ASSERT(this->isRsa());
    return d_rsa.object();
}

EncryptionKeyEllipticCurveValuePublic&
EncryptionKeyValuePublic::ellipticCurve()
{
    BSLS_ASSERT(this->isEllipticCurve());
    return d_ellipticCurve.object();
}

ntsa::AbstractBitString& EncryptionKeyValuePublic::any()
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

const EncryptionKeyRsaValuePublic&
EncryptionKeyValuePublic::rsa() const
{
    BSLS_ASSERT(this->isRsa());
    return d_rsa.object();
}

const EncryptionKeyEllipticCurveValuePublic&
EncryptionKeyValuePublic::ellipticCurve() const
{
    BSLS_ASSERT(this->isEllipticCurve());
    return d_ellipticCurve.object();
}

const ntsa::AbstractBitString& EncryptionKeyValuePublic::any() const
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

bool EncryptionKeyValuePublic::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionKeyValuePublic::isRsa() const
{
    return d_type == e_RSA;
}

bool EncryptionKeyValuePublic::isEllipticCurve() const
{
    return d_type == e_ELLIPTIC_CURVE;
}

bool EncryptionKeyValuePublic::isAny() const
{
    return d_type == e_ANY;
}

bool EncryptionKeyValuePublic::equals(
    const EncryptionKeyValuePublic& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_UNDEFINED) {
        return true;
    }
    else if (d_type == e_RSA) {
        return d_rsa.object() == other.d_rsa.object();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object() == other.d_ellipticCurve.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() == other.d_any.object();
    }

    return false;
}

bool EncryptionKeyValuePublic::less(
    const EncryptionKeyValuePublic& other) const
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
    else if (d_type == e_RSA) {
        return d_rsa.object() < other.d_rsa.object();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        return d_ellipticCurve.object() < other.d_ellipticCurve.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() < other.d_any.object();
    }

    return false;
}

bsl::ostream& EncryptionKeyValuePublic::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    if (d_type == e_RSA) {
        d_rsa.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_ANY) {
        d_any.object().print(stream, level, spacesPerLevel);
    }
    else {
        stream << "UNDEFINED";
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                              stream,
                         const EncryptionKeyValuePublic& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyValuePublic& lhs,
                const EncryptionKeyValuePublic& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyValuePublic& lhs,
                const EncryptionKeyValuePublic& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyValuePublic& lhs,
               const EncryptionKeyValuePublic& rhs)
{
    return lhs.less(rhs);
}

EncryptionKeyInfoPublic::EncryptionKeyInfoPublic(
    bslma::Allocator* basicAllocator)
: d_algorithm(basicAllocator)
, d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyInfoPublic::EncryptionKeyInfoPublic(
    const EncryptionKeyInfoPublic& original,
    bslma::Allocator*                         basicAllocator)
: d_algorithm(original.d_algorithm, basicAllocator)
, d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionKeyInfoPublic::~EncryptionKeyInfoPublic()
{
}

EncryptionKeyInfoPublic& EncryptionKeyInfoPublic::
operator=(const EncryptionKeyInfoPublic& other)
{
    if (this != &other) {
        d_algorithm = other.d_algorithm;
        d_value     = other.d_value;
    }

    return *this;
}

void EncryptionKeyInfoPublic::reset()
{
    d_algorithm.reset();
    d_value.reset();
}

ntsa::Error EncryptionKeyInfoPublic::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = d_algorithm.decode(decoder);
    if (error) {
        return error;
    }

    if (d_algorithm.identifier().equals(
            ntca::EncryptionKeyAlgorithmIdentifierType::
                e_RSA))
    {
        error = d_value.makeRsa().decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (d_algorithm.identifier().equals(
                 ntca::EncryptionKeyAlgorithmIdentifierType::
                     e_ELLIPTIC_CURVE))
    {
        error = d_value.makeEllipticCurve().decode(decoder);
        if (error) {
            return error;
        }
    }
    else {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_value.makeAny());
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

ntsa::Error EncryptionKeyInfoPublic::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = d_algorithm.encode(encoder);
    if (error) {
        return error;
    }

    if (d_value.isRsa()) {
        error = d_value.rsa().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isEllipticCurve()) {
        error = d_value.ellipticCurve().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isAny()) {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_value.any());
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

    return ntsa::Error();
}

const ntca::EncryptionKeyAlgorithm& 
EncryptionKeyInfoPublic::algorithm() const
{
    return d_algorithm;
}

const ntca::EncryptionKeyValuePublic& 
EncryptionKeyInfoPublic::value() const
{
    return d_value;
}

bool EncryptionKeyInfoPublic::equals(
    const EncryptionKeyInfoPublic& other) const
{
    return d_algorithm == other.d_algorithm && d_value == other.d_value;
}

bool EncryptionKeyInfoPublic::less(
    const EncryptionKeyInfoPublic& other) const
{
    if (d_algorithm < other.d_algorithm) {
        return true;
    }

    if (other.d_algorithm < d_algorithm) {
        return false;
    }

    return d_value < other.d_value;
}

bsl::ostream& EncryptionKeyInfoPublic::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("algorithm", d_algorithm);
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                             stream,
                         const EncryptionKeyInfoPublic& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyInfoPublic& lhs,
                const EncryptionKeyInfoPublic& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyInfoPublic& lhs,
                const EncryptionKeyInfoPublic& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyInfoPublic& lhs,
               const EncryptionKeyInfoPublic& rhs)
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
            ntca::EncryptionKeyRsaValuePrivate(original.d_rsa.object(), d_allocator_p);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        new (d_ellipticCurve.buffer())
            ntca::EncryptionKeyEllipticCurveValuePrivate(original.d_ellipticCurve.object(),
                                             d_allocator_p);
    }
    else if (d_type == e_PRIVATE_KEY_INFO) {
        new (d_info.buffer())
            ntca::EncryptionKeyInfoPrivate(original.d_info.object(),
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
        else if (other.d_type == e_PRIVATE_KEY_INFO) {
            this->makeInfo(other.d_info.object());
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
        d_rsa.object().~RsaType();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().~EllipticCurveType();
    }
    else if (d_type == e_PRIVATE_KEY_INFO) {
        d_info.object().~InfoType();
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    d_type = e_UNDEFINED;
}

ntca::EncryptionKeyRsaValuePrivate& EncryptionKey::makeRsa()
{
    if (d_type == e_RSA) {
        d_rsa.object().reset();
    }
    else {
        this->reset();
        new (d_rsa.buffer()) ntca::EncryptionKeyRsaValuePrivate(d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

ntca::EncryptionKeyRsaValuePrivate& EncryptionKey::makeRsa(
    const ntca::EncryptionKeyRsaValuePrivate& value)
{
    if (d_type == e_RSA) {
        d_rsa.object() = value;
    }
    else {
        this->reset();
        new (d_rsa.buffer()) ntca::EncryptionKeyRsaValuePrivate(value, d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

ntca::EncryptionKeyEllipticCurveValuePrivate& EncryptionKey::makeEllipticCurve()
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().reset();
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            ntca::EncryptionKeyEllipticCurveValuePrivate(d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

ntca::EncryptionKeyEllipticCurveValuePrivate& EncryptionKey::makeEllipticCurve(
    const ntca::EncryptionKeyEllipticCurveValuePrivate& value)
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object() = value;
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            ntca::EncryptionKeyEllipticCurveValuePrivate(value, d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

ntca::EncryptionKeyInfoPrivate& EncryptionKey::makeInfo()
{
    if (d_type == e_PRIVATE_KEY_INFO) {
        d_info.object().reset();
    }
    else {
        this->reset();
        new (d_info.buffer()) ntca::EncryptionKeyInfoPrivate(d_allocator_p);
        d_type = e_PRIVATE_KEY_INFO;
    }

    return d_info.object();
}

ntca::EncryptionKeyInfoPrivate& EncryptionKey::makeInfo(
    const ntca::EncryptionKeyInfoPrivate& value)
{
    if (d_type == e_PRIVATE_KEY_INFO) {
        d_info.object() = value;
    }
    else {
        this->reset();
        new (d_info.buffer()) ntca::EncryptionKeyInfoPrivate(value, d_allocator_p);
        d_type = e_PRIVATE_KEY_INFO;
    }

    return d_info.object();
}

ntca::EncryptionKeyRsaValuePrivate& EncryptionKey::rsa()
{
    BSLS_ASSERT(d_type == e_RSA);
    return d_rsa.object();
}

ntca::EncryptionKeyEllipticCurveValuePrivate& EncryptionKey::ellipticCurve()
{
    BSLS_ASSERT(d_type == e_ELLIPTIC_CURVE);
    return d_ellipticCurve.object();
}

ntca::EncryptionKeyInfoPrivate& EncryptionKey::info()
{
    BSLS_ASSERT(d_type == e_PRIVATE_KEY_INFO);
    return d_info.object();
}


ntsa::Error EncryptionKey::decode(ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    bsl::uint64_t position = decoder->position();

    this->makeInfo();

    error = d_info.object().decode(decoder);
    if (!error) {
        return ntsa::Error();
    }

    error = decoder->seek(position);
    if (error) {
        return ntsa::Error();
    }

    this->makeEllipticCurve();

    error = d_ellipticCurve.object().decode(decoder);
    if (!error) {
        return ntsa::Error();
    }

    error = decoder->seek(position);
    if (error) {
        return ntsa::Error();
    }

    this->makeRsa();

    error = d_rsa.object().decode(decoder);
    if (!error) {
        return ntsa::Error();
    }

    this->reset();

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error EncryptionKey::encode(ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    if (this->isRsa()) {
        error = this->rsa().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (this->isEllipticCurve()) {
        error = this->ellipticCurve().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (this->isInfo()) {
        error = this->info().encode(encoder);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

const ntca::EncryptionKeyRsaValuePrivate& EncryptionKey::rsa() const
{
    BSLS_ASSERT(d_type == e_RSA);
    return d_rsa.object();
}

const ntca::EncryptionKeyEllipticCurveValuePrivate& EncryptionKey::ellipticCurve() const
{
    BSLS_ASSERT(d_type == e_ELLIPTIC_CURVE);
    return d_ellipticCurve.object();
}

const ntca::EncryptionKeyInfoPrivate& EncryptionKey::info() const
{
    BSLS_ASSERT(d_type == e_PRIVATE_KEY_INFO);
    return d_info.object();
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

bool EncryptionKey::isInfo() const
{
    return d_type == e_PRIVATE_KEY_INFO;
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
    else if (d_type == e_PRIVATE_KEY_INFO) {
        return d_info.object().equals(other.d_info.object());
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
    else if (d_type == e_PRIVATE_KEY_INFO) {
        return d_info.object().less(other.d_info.object());
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
    else if (d_type == e_PRIVATE_KEY_INFO) {
        return d_info.object().print(stream, level, spacesPerLevel);
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
