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



EncryptionKeyRsa::EncryptionKeyRsa(bslma::Allocator* basicAllocator)
: d_version(1)
, d_modulus(0)
, d_publicExponent(0)
, d_privateExponent(0)
, d_prime1(0)
, d_prime2(0)
, d_exponent1(0)
, d_exponent2(0)
, d_coefficient(0)
, d_extraPrimeInfo(basicAllocator)
{
}

EncryptionKeyRsa::EncryptionKeyRsa(const EncryptionKeyRsa& original,
                             bslma::Allocator*    basicAllocator)
: d_version(original.d_version)
, d_modulus(original.d_modulus)
, d_publicExponent(original.d_publicExponent)
, d_privateExponent(original.d_privateExponent)
, d_prime1(original.d_prime1)
, d_prime2(original.d_prime2)
, d_exponent1(original.d_exponent1)
, d_exponent2(original.d_exponent2)
, d_coefficient(original.d_coefficient)
, d_extraPrimeInfo(basicAllocator)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

EncryptionKeyRsa::~EncryptionKeyRsa()
{
}

EncryptionKeyRsa& EncryptionKeyRsa::operator=(const EncryptionKeyRsa& other)
{
    if (this != &other) {
        d_version = other.d_version;
        d_modulus = other.d_modulus;
        d_publicExponent = other.d_publicExponent;
        d_privateExponent = other.d_privateExponent;
        d_prime1 = other.d_prime1;
        d_prime2 = other.d_prime2;
        d_exponent1 = other.d_exponent1;
        d_exponent2 = other.d_exponent2;
        d_coefficient = other.d_coefficient;
        d_extraPrimeInfo = other.d_extraPrimeInfo;
    }

    return *this;
}

void EncryptionKeyRsa::reset()
{
    d_version = 1;
    d_modulus = 0;
    d_publicExponent = 0;
    d_privateExponent = 0;
    d_prime1 = 0;
    d_prime2 = 0;
    d_exponent1 = 0;
    d_exponent2 = 0;
    d_coefficient = 0;
    d_extraPrimeInfo.clear();
}

void EncryptionKeyRsa::setVersion(Number value)
{
    d_version = value;
}

void EncryptionKeyRsa::setModulus(Number value)
{
    d_modulus = value;
}

void EncryptionKeyRsa::setPublicExponent(Number value)
{
    d_publicExponent = value;
}

void EncryptionKeyRsa::setPrivateExponent(Number value)
{
    d_privateExponent = value;
}

void EncryptionKeyRsa::setPrime1(Number value)
{
    d_prime1 = value;
}

void EncryptionKeyRsa::setPrime2(Number value)
{
    d_prime2 = value;
}

void EncryptionKeyRsa::setExponent1(Number value)
{
    d_exponent1 = value;
}

void EncryptionKeyRsa::setExponent2(Number value)
{
    d_exponent2 = value;
}

void EncryptionKeyRsa::setCoefficient(Number value)
{
    d_coefficient = value;
}

void EncryptionKeyRsa::addExtraPrimeInfo(const Prime& value)
{
    d_extraPrimeInfo.push_back(value);
}

EncryptionKeyRsa::Number EncryptionKeyRsa::version() const
{
    return d_version;
}

EncryptionKeyRsa::Number EncryptionKeyRsa::modulus() const
{
    return d_modulus;
}

EncryptionKeyRsa::Number EncryptionKeyRsa::publicExponent() const
{
    return d_publicExponent;
}

EncryptionKeyRsa::Number EncryptionKeyRsa::privateExponent() const
{
    return d_privateExponent;
}

EncryptionKeyRsa::Number EncryptionKeyRsa::prime1() const
{
    return d_prime1;
}

EncryptionKeyRsa::Number EncryptionKeyRsa::prime2() const
{
    return d_prime2;
}

EncryptionKeyRsa::Number EncryptionKeyRsa::exponent1() const
{
    return d_exponent1;
}

EncryptionKeyRsa::Number EncryptionKeyRsa::exponent2() const
{
    return d_exponent2;
}

EncryptionKeyRsa::Number EncryptionKeyRsa::coefficient() const
{
    return d_coefficient;
}

const EncryptionKeyRsa::PrimeVector& EncryptionKeyRsa::extraPrimeInfo() const
{
    return d_extraPrimeInfo;
}

bool EncryptionKeyRsa::equals(const EncryptionKeyRsa& other) const
{
    return d_version == other.d_version &&
            d_modulus == other.d_modulus &&
            d_publicExponent == other.d_publicExponent &&
            d_privateExponent == other.d_privateExponent &&
            d_prime1 == other.d_prime1 &&
            d_prime2 == other.d_prime2 &&
            d_exponent1 == other.d_exponent1 &&
            d_exponent2 == other.d_exponent2 &&
            d_coefficient == other.d_coefficient &&
            d_extraPrimeInfo == other.d_extraPrimeInfo;

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

    if (d_publicExponent < other.d_publicExponent) {
        return true;
    }

    if (other.d_publicExponent < d_publicExponent) {
        return false;
    }

    if (d_privateExponent < other.d_privateExponent) {
        return true;
    }

    if (other.d_privateExponent < d_privateExponent) {
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




    return d_extraPrimeInfo < other.d_extraPrimeInfo;
}

bsl::ostream& EncryptionKeyRsa::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("version", d_version);
    printer.printAttribute("modulus", d_modulus);
    printer.printAttribute("publicExponent", d_publicExponent);
    printer.printAttribute("privateExponent", d_privateExponent);
    printer.printAttribute("prime1", d_prime1);
    printer.printAttribute("prime2", d_prime2);
    printer.printAttribute("exponent1", d_exponent1);
    printer.printAttribute("exponent2", d_exponent2);
    printer.printAttribute("coefficient", d_coefficient);
    // printer.printAttribute("otherPrimeInfo", d_extraPrimeInfo);

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






















EncryptionKeyEllipticCurve::EncryptionKeyEllipticCurve(bslma::Allocator* basicAllocator)
: d_version(1)
, d_privateKey(basicAllocator)
, d_publicKey(basicAllocator)
{
}

EncryptionKeyEllipticCurve::EncryptionKeyEllipticCurve(const EncryptionKeyEllipticCurve& original,
                             bslma::Allocator*    basicAllocator)
: d_version(original.d_version)
, d_privateKey(original.d_privateKey, basicAllocator)
, d_publicKey(original.d_publicKey, basicAllocator)
{
}

EncryptionKeyEllipticCurve::~EncryptionKeyEllipticCurve()
{
}

EncryptionKeyEllipticCurve& EncryptionKeyEllipticCurve::operator=(const EncryptionKeyEllipticCurve& other)
{
    if (this != &other) {
        d_version  = other.d_version;
        d_privateKey = other.d_privateKey;
        d_publicKey = other.d_publicKey;
    }

    return *this;
}

void EncryptionKeyEllipticCurve::reset()
{
    d_version = 1;
    d_privateKey.clear();
    d_publicKey.clear();
}

void EncryptionKeyEllipticCurve::setVersion(Number value)
{
    d_version = value;
}

EncryptionKeyEllipticCurve::Number EncryptionKeyEllipticCurve::version() const
{
    return d_version;
}

bool EncryptionKeyEllipticCurve::equals(const EncryptionKeyEllipticCurve& other) const
{
    return d_version == other.d_version && d_privateKey == other.d_privateKey && d_publicKey == other.d_publicKey;
}

bool EncryptionKeyEllipticCurve::less(const EncryptionKeyEllipticCurve& other) const
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

    return d_publicKey < other.d_publicKey;
}

bsl::ostream& EncryptionKeyEllipticCurve::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("version", d_version);
    printer.printAttribute("privateKey", d_privateKey);
    printer.printAttribute("publicKey", d_publicKey);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionKeyEllipticCurve& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionKeyEllipticCurve& lhs, const EncryptionKeyEllipticCurve& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionKeyEllipticCurve& lhs, const EncryptionKeyEllipticCurve& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionKeyEllipticCurve& lhs, const EncryptionKeyEllipticCurve& rhs)
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
            ntca::EncryptionKeyEllipticCurve(original.d_ellipticCurve.object(), d_allocator_p);
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

ntca::EncryptionKeyRsa& EncryptionKey::makeRsa(const ntca::EncryptionKeyRsa& value)
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
        new (d_ellipticCurve.buffer()) ntca::EncryptionKeyEllipticCurve(d_allocator_p);
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
        new (d_ellipticCurve.buffer()) ntca::EncryptionKeyEllipticCurve(value, d_allocator_p);
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
