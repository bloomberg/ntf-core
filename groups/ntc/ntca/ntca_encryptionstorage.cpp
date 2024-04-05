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

#include <ntca_encryptionstorage.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionstorage_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

EncryptionStoragePkcs7::EncryptionStoragePkcs7(bslma::Allocator* basicAllocator)
: d_certificate(basicAllocator)
, d_flags(0)
{
}

EncryptionStoragePkcs7::EncryptionStoragePkcs7(const EncryptionStoragePkcs7& original,
                bslma::Allocator*    basicAllocator)
: d_certificate(original.d_certificate, basicAllocator)
, d_flags(original.d_flags)
{
}

EncryptionStoragePkcs7::~EncryptionStoragePkcs7()
{
}

EncryptionStoragePkcs7& EncryptionStoragePkcs7::operator=(const EncryptionStoragePkcs7& other)
{
    if (this != &other) {
        d_certificate = other.d_certificate;
        d_flags = other.d_flags;
    }

    return *this;
}

void EncryptionStoragePkcs7::reset()
{
    d_certificate.reset();
    d_flags = 0;
}

void EncryptionStoragePkcs7::setCertificate(const ntca::EncryptionCertificate& value)
{
    d_certificate = value;
}

const ntca::EncryptionCertificate& EncryptionStoragePkcs7::certificate() const
{
    return d_certificate;
}


bool EncryptionStoragePkcs7::equals(const EncryptionStoragePkcs7& other) const
{
    return d_certificate == other.d_certificate && d_flags == other.d_flags;
}

bool EncryptionStoragePkcs7::less(const EncryptionStoragePkcs7& other) const
{
    if (d_certificate < other.d_certificate) {
        return true;
    }

    if (other.d_certificate < d_certificate) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& EncryptionStoragePkcs7::print(bsl::ostream& stream,
                    int           level,
                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("certificate", d_certificate);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionStoragePkcs7& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionStoragePkcs7& lhs, const EncryptionStoragePkcs7& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionStoragePkcs7& lhs, const EncryptionStoragePkcs7& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionStoragePkcs7& lhs, const EncryptionStoragePkcs7& rhs)
{
    return lhs.less(rhs);
}






















EncryptionStoragePkcs8::EncryptionStoragePkcs8(bslma::Allocator* basicAllocator)
: d_key(basicAllocator)
, d_flags(0)
{
}

EncryptionStoragePkcs8::EncryptionStoragePkcs8(const EncryptionStoragePkcs8& original,
                bslma::Allocator*    basicAllocator)
: d_key(original.d_key, basicAllocator)
, d_flags(original.d_flags)
{
}

EncryptionStoragePkcs8::~EncryptionStoragePkcs8()
{
}

EncryptionStoragePkcs8& EncryptionStoragePkcs8::operator=(const EncryptionStoragePkcs8& other)
{
    if (this != &other) {
        d_key = other.d_key;
        d_flags = other.d_flags;
    }

    return *this;
}

void EncryptionStoragePkcs8::reset()
{
    d_key.reset();
    d_flags = 0;
}

void EncryptionStoragePkcs8::setKey(const ntca::EncryptionKey& value)
{
    d_key = value;
}

const ntca::EncryptionKey& EncryptionStoragePkcs8::key() const
{
    return d_key;
}


bool EncryptionStoragePkcs8::equals(const EncryptionStoragePkcs8& other) const
{
    return d_key == other.d_key && d_flags == other.d_flags;
}

bool EncryptionStoragePkcs8::less(const EncryptionStoragePkcs8& other) const
{
    if (d_key < other.d_key) {
        return true;
    }

    if (other.d_key < d_key) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& EncryptionStoragePkcs8::print(bsl::ostream& stream,
                    int           level,
                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("key", d_key);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionStoragePkcs8& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionStoragePkcs8& lhs, const EncryptionStoragePkcs8& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionStoragePkcs8& lhs, const EncryptionStoragePkcs8& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionStoragePkcs8& lhs, const EncryptionStoragePkcs8& rhs)
{
    return lhs.less(rhs);
}















EncryptionStoragePkcs12::EncryptionStoragePkcs12(bslma::Allocator* basicAllocator)
: d_key(basicAllocator)
, d_certificate(basicAllocator)
, d_certificateAuthorityList(basicAllocator)
, d_flags(0)
{
}

EncryptionStoragePkcs12::EncryptionStoragePkcs12(const EncryptionStoragePkcs12& original,
                bslma::Allocator*    basicAllocator)
: d_key(original.d_key, basicAllocator)
, d_certificate(original.d_certificate, basicAllocator)
, d_certificateAuthorityList(d_certificateAuthorityList, basicAllocator)
, d_flags(original.d_flags)
{
}

EncryptionStoragePkcs12::~EncryptionStoragePkcs12()
{
}

EncryptionStoragePkcs12& EncryptionStoragePkcs12::operator=(const EncryptionStoragePkcs12& other)
{
    if (this != &other) {
        d_key = other.d_key;
        d_certificate = other.d_certificate;
        d_certificateAuthorityList = other.d_certificateAuthorityList;
        d_flags = other.d_flags;
    }

    return *this;
}

void EncryptionStoragePkcs12::reset()
{
    d_key.reset();
    d_certificate.reset();
    d_certificateAuthorityList.clear();
    d_flags = 0;
}

void EncryptionStoragePkcs12::setKey(const ntca::EncryptionKey& value)
{
    d_key = value;
}

void EncryptionStoragePkcs12::setCertificate(const ntca::EncryptionCertificate& value)
{
    d_certificate = value;
}

void EncryptionStoragePkcs12::addCertificateAuthority(const ntca::EncryptionCertificate& value)
{
    d_certificateAuthorityList.push_back(value);
}

const bdlb::NullableValue<ntca::EncryptionKey>& EncryptionStoragePkcs12::key() const
{
    return d_key;
}

const bdlb::NullableValue<ntca::EncryptionCertificate>& EncryptionStoragePkcs12::certificate() const
{
    return d_certificate;
}

const ntca::EncryptionCertificateVector& EncryptionStoragePkcs12::certificateAuthorityList() const
{
    return d_certificateAuthorityList;
}


bool EncryptionStoragePkcs12::equals(const EncryptionStoragePkcs12& other) const
{
    return d_key == other.d_key && 
        d_certificate == other.d_certificate &&
        d_certificateAuthorityList == other.d_certificateAuthorityList &&
        d_flags == other.d_flags;
    }

bool EncryptionStoragePkcs12::less(const EncryptionStoragePkcs12& other) const
{
    if (d_key < other.d_key) {
        return true;
    }

    if (other.d_key < d_key) {
        return false;
    }


    if (d_certificate < other.d_certificate) {
        return true;
    }

    if (other.d_certificate < d_certificate) {
        return false;
    }


    if (d_certificateAuthorityList < other.d_certificateAuthorityList) {
        return true;
    }

    if (other.d_certificateAuthorityList < d_certificateAuthorityList) {
        return false;
    }


    return d_flags < other.d_flags;
}

bsl::ostream& EncryptionStoragePkcs12::print(bsl::ostream& stream,
                    int           level,
                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_key.isNull()) {
        printer.printAttribute("key", d_key);
    }

    if (!d_certificate.isNull()) {
        printer.printAttribute("certificate", d_key);
    }

    if (!d_certificateAuthorityList.empty()) {
        printer.printAttribute("certificateAuthorityList", d_certificateAuthorityList);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionStoragePkcs12& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionStoragePkcs12& lhs, const EncryptionStoragePkcs12& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionStoragePkcs12& lhs, const EncryptionStoragePkcs12& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionStoragePkcs12& lhs, const EncryptionStoragePkcs12& rhs)
{
    return lhs.less(rhs);
}











EncryptionStoragePem::EncryptionStoragePem(bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionStoragePem::EncryptionStoragePem(const EncryptionStoragePem& original,
                        bslma::Allocator*           basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_KEY) {
        new (d_key.buffer())
            ntca::EncryptionKey(original.d_key.object(), d_allocator_p);
    }
    else if (d_type == e_CERTIFICATE) {
        new (d_certificate.buffer())
            ntca::EncryptionCertificate(original.d_certificate.object(), d_allocator_p);
    }
    else if (d_type == e_PKCS7) {
        new (d_pkcs7.buffer())
            ntca::EncryptionStoragePkcs7(original.d_pkcs7.object(), d_allocator_p);
    }
    else if (d_type == e_PKCS8) {
        new (d_pkcs8.buffer())
            ntca::EncryptionStoragePkcs8(original.d_pkcs8.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionStoragePem::~EncryptionStoragePem()
{
    this->reset();
}

EncryptionStoragePem& EncryptionStoragePem::operator=(const EncryptionStoragePem& other)
{
    if (this != &other) {
        if (other.d_type == e_KEY) {
            this->makeKey(other.d_key.object());
        }
        else if (other.d_type == e_CERTIFICATE) {
            this->makeCertificate(other.d_certificate.object());
        }
        else if (other.d_type == e_PKCS7) {
            this->makePkcs7(other.d_pkcs7.object());
        }
        else if (other.d_type == e_PKCS8) {
            this->makePkcs8(other.d_pkcs8.object());
        }
        else {
            BSLS_ASSERT(other.d_type == e_UNDEFINED);
            this->reset();
        }
    }

    return *this;
}

void EncryptionStoragePem::reset()
{
    if (d_type == e_KEY) {
        typedef ntca::EncryptionKey Type;
        d_key.object().~Type();
    }
    else if (d_type == e_CERTIFICATE) {
        typedef ntca::EncryptionCertificate Type;
        d_certificate.object().~Type();
    }
    else if (d_type == e_PKCS7) {
        typedef ntca::EncryptionStoragePkcs7 Type;
        d_pkcs7.object().~Type();
    }
    else if (d_type == e_PKCS8) {
        typedef ntca::EncryptionStoragePkcs8 Type;
        d_pkcs8.object().~Type();
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    d_type = e_UNDEFINED;
}

ntca::EncryptionKey& EncryptionStoragePem::makeKey()
{
    if (d_type == e_KEY) {
        d_key.object().reset();
    }
    else {
        this->reset();
        new (d_key.buffer()) ntca::EncryptionKey(d_allocator_p);
        d_type = e_KEY;
    }

    return d_key.object();
}

ntca::EncryptionKey& EncryptionStoragePem::makeKey(const ntca::EncryptionKey& value)
{
    if (d_type == e_KEY) {
        d_key.object() = value;
    }
    else {
        this->reset();
        new (d_key.buffer()) ntca::EncryptionKey(value, d_allocator_p);
        d_type = e_KEY;
    }

    return d_key.object();
}

ntca::EncryptionCertificate& EncryptionStoragePem::makeCertificate()
{
    if (d_type == e_CERTIFICATE) {
        d_certificate.object().reset();
    }
    else {
        this->reset();
        new (d_certificate.buffer()) ntca::EncryptionCertificate(d_allocator_p);
        d_type = e_CERTIFICATE;
    }

    return d_certificate.object();
}

ntca::EncryptionCertificate& EncryptionStoragePem::makeCertificate(
    const ntca::EncryptionCertificate& value)
{
    if (d_type == e_CERTIFICATE) {
        d_certificate.object() = value;
    }
    else {
        this->reset();
        new (d_certificate.buffer()) ntca::EncryptionCertificate(value, d_allocator_p);
        d_type = e_CERTIFICATE;
    }

    return d_certificate.object();
}

ntca::EncryptionStoragePkcs7& EncryptionStoragePem::makePkcs7()
{
    if (d_type == e_PKCS7) {
        d_pkcs7.object().reset();
    }
    else {
        this->reset();
        new (d_pkcs7.buffer()) ntca::EncryptionStoragePkcs7(d_allocator_p);
        d_type = e_PKCS7;
    }

    return d_pkcs7.object();
}

ntca::EncryptionStoragePkcs7& EncryptionStoragePem::makePkcs7(
    const ntca::EncryptionStoragePkcs7& value)
{
    if (d_type == e_PKCS7) {
        d_pkcs7.object() = value;
    }
    else {
        this->reset();
        new (d_pkcs7.buffer()) ntca::EncryptionStoragePkcs7(value, d_allocator_p);
        d_type = e_PKCS7;
    }

    return d_pkcs7.object();
}

ntca::EncryptionStoragePkcs8& EncryptionStoragePem::makePkcs8()
{
    if (d_type == e_PKCS8) {
        d_pkcs8.object().reset();
    }
    else {
        this->reset();
        new (d_pkcs8.buffer()) ntca::EncryptionStoragePkcs8(d_allocator_p);
        d_type = e_PKCS8;
    }

    return d_pkcs8.object();
}

ntca::EncryptionStoragePkcs8& EncryptionStoragePem::makePkcs8(
    const ntca::EncryptionStoragePkcs8& value)
{
    if (d_type == e_PKCS8) {
        d_pkcs8.object() = value;
    }
    else {
        this->reset();
        new (d_pkcs8.buffer()) ntca::EncryptionStoragePkcs8(value, d_allocator_p);
        d_type = e_PKCS8;
    }

    return d_pkcs8.object();
}

ntca::EncryptionKey& EncryptionStoragePem::key()
{
    BSLS_ASSERT(d_type == e_KEY);
    return d_key.object();
}

ntca::EncryptionCertificate& EncryptionStoragePem::certificate()
{
    BSLS_ASSERT(d_type == e_CERTIFICATE);
    return d_certificate.object();
}

ntca::EncryptionStoragePkcs7& EncryptionStoragePem::pkcs7()
{
    BSLS_ASSERT(d_type == e_PKCS7);
    return d_pkcs7.object();
}

ntca::EncryptionStoragePkcs8& EncryptionStoragePem::pkcs8()
{
    BSLS_ASSERT(d_type == e_PKCS8);
    return d_pkcs8.object();
}

const ntca::EncryptionKey& EncryptionStoragePem::key() const
{
    BSLS_ASSERT(d_type == e_KEY);
    return d_key.object();
}

const ntca::EncryptionCertificate& EncryptionStoragePem::certificate() const
{
    BSLS_ASSERT(d_type == e_CERTIFICATE);
    return d_certificate.object();
}

const ntca::EncryptionStoragePkcs7& EncryptionStoragePem::pkcs7() const
{
    BSLS_ASSERT(d_type == e_PKCS7);
    return d_pkcs7.object();
}

const ntca::EncryptionStoragePkcs8& EncryptionStoragePem::pkcs8() const
{
    BSLS_ASSERT(d_type == e_PKCS8);
    return d_pkcs8.object();
}

bool EncryptionStoragePem::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionStoragePem::isKey() const
{
    return d_type == e_KEY;
}

bool EncryptionStoragePem::isCertificate() const
{
    return d_type == e_CERTIFICATE;
}

bool EncryptionStoragePem::isPkcs7() const
{
    return d_type == e_PKCS7;
}

bool EncryptionStoragePem::isPkcs8() const
{
    return d_type == e_PKCS8;
}

bool EncryptionStoragePem::equals(const EncryptionStoragePem& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_KEY) {
        return d_key.object().equals(other.d_key.object());
    }
    else if (d_type == e_CERTIFICATE) {
        return d_certificate.object().equals(other.d_certificate.object());
    }
    else if (d_type == e_PKCS7) {
        return d_pkcs7.object().equals(other.d_pkcs7.object());
    }
    else if (d_type == e_PKCS8) {
        return d_pkcs8.object().equals(other.d_pkcs8.object());
    }

    return true;
}

bool EncryptionStoragePem::less(const EncryptionStoragePem& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_KEY) {
        return d_key.object().less(other.d_key.object());
    }
    else if (d_type == e_CERTIFICATE) {
        return d_certificate.object().less(other.d_certificate.object());
    }
    else if (d_type == e_PKCS7) {
        return d_pkcs7.object().less(other.d_pkcs7.object());
    }
    else if (d_type == e_PKCS8) {
        return d_pkcs8.object().less(other.d_pkcs8.object());
    }

    return true;
}

bsl::ostream& EncryptionStoragePem::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    if (d_type == e_KEY) {
        return d_key.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_CERTIFICATE) {
        return d_certificate.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_PKCS7) {
        return d_pkcs7.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_PKCS8) {
        return d_pkcs8.object().print(stream, level, spacesPerLevel);
    }

    stream << "UNDEFINED";
    return stream;
}


bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionStoragePem& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionStoragePem& lhs, const EncryptionStoragePem& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionStoragePem& lhs, const EncryptionStoragePem& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionStoragePem& lhs, const EncryptionStoragePem& rhs)
{
    return lhs.less(rhs);
}










































EncryptionStorage::EncryptionStorage(bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionStorage::EncryptionStorage(const EncryptionStorage& original,
                        bslma::Allocator*           basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_KEY) {
        new (d_key.buffer())
            ntca::EncryptionKey(original.d_key.object(), d_allocator_p);
    }
    else if (d_type == e_CERTIFICATE) {
        new (d_certificate.buffer())
            ntca::EncryptionCertificate(original.d_certificate.object(), d_allocator_p);
    }
    else if (d_type == e_PKCS7) {
        new (d_pkcs7.buffer())
            ntca::EncryptionStoragePkcs7(original.d_pkcs7.object(), d_allocator_p);
    }
    else if (d_type == e_PKCS8) {
        new (d_pkcs8.buffer())
            ntca::EncryptionStoragePkcs8(original.d_pkcs8.object(), d_allocator_p);
    }
    else if (d_type == e_PKCS12) {
        new (d_pkcs12.buffer())
            ntca::EncryptionStoragePkcs12(original.d_pkcs12.object(), d_allocator_p);
    }
    else if (d_type == e_PEM) {
        new (d_pem.buffer())
            ntca::EncryptionStoragePem(original.d_pem.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionStorage::~EncryptionStorage()
{
    this->reset();
}

EncryptionStorage& EncryptionStorage::operator=(const EncryptionStorage& other)
{
    if (this != &other) {
        if (other.d_type == e_KEY) {
            this->makeKey(other.d_key.object());
        }
        else if (other.d_type == e_CERTIFICATE) {
            this->makeCertificate(other.d_certificate.object());
        }
        else if (other.d_type == e_PKCS7) {
            this->makePkcs7(other.d_pkcs7.object());
        }
        else if (other.d_type == e_PKCS8) {
            this->makePkcs8(other.d_pkcs8.object());
        }
        else if (other.d_type == e_PKCS12) {
            this->makePkcs12(other.d_pkcs12.object());
        }
        else if (other.d_type == e_PEM) {
            this->makePem(other.d_pem.object());
        }
        else {
            BSLS_ASSERT(other.d_type == e_UNDEFINED);
            this->reset();
        }
    }

    return *this;
}

void EncryptionStorage::reset()
{
    if (d_type == e_KEY) {
        typedef ntca::EncryptionKey Type;
        d_key.object().~Type();
    }
    else if (d_type == e_CERTIFICATE) {
        typedef ntca::EncryptionCertificate Type;
        d_certificate.object().~Type();
    }
    else if (d_type == e_PKCS7) {
        typedef ntca::EncryptionStoragePkcs7 Type;
        d_pkcs7.object().~Type();
    }
    else if (d_type == e_PKCS8) {
        typedef ntca::EncryptionStoragePkcs8 Type;
        d_pkcs8.object().~Type();
    }
    else if (d_type == e_PKCS12) {
        typedef ntca::EncryptionStoragePkcs12 Type;
        d_pkcs12.object().~Type();
    }
    else if (d_type == e_PEM) {
        typedef ntca::EncryptionStoragePem Type;
        d_pem.object().~Type();
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    d_type = e_UNDEFINED;
}

ntca::EncryptionKey& EncryptionStorage::makeKey()
{
    if (d_type == e_KEY) {
        d_key.object().reset();
    }
    else {
        this->reset();
        new (d_key.buffer()) ntca::EncryptionKey(d_allocator_p);
        d_type = e_KEY;
    }

    return d_key.object();
}

ntca::EncryptionKey& EncryptionStorage::makeKey(const ntca::EncryptionKey& value)
{
    if (d_type == e_KEY) {
        d_key.object() = value;
    }
    else {
        this->reset();
        new (d_key.buffer()) ntca::EncryptionKey(value, d_allocator_p);
        d_type = e_KEY;
    }

    return d_key.object();
}

ntca::EncryptionCertificate& EncryptionStorage::makeCertificate()
{
    if (d_type == e_CERTIFICATE) {
        d_certificate.object().reset();
    }
    else {
        this->reset();
        new (d_certificate.buffer()) ntca::EncryptionCertificate(d_allocator_p);
        d_type = e_CERTIFICATE;
    }

    return d_certificate.object();
}

ntca::EncryptionCertificate& EncryptionStorage::makeCertificate(
    const ntca::EncryptionCertificate& value)
{
    if (d_type == e_CERTIFICATE) {
        d_certificate.object() = value;
    }
    else {
        this->reset();
        new (d_certificate.buffer()) ntca::EncryptionCertificate(value, d_allocator_p);
        d_type = e_CERTIFICATE;
    }

    return d_certificate.object();
}

ntca::EncryptionStoragePkcs7& EncryptionStorage::makePkcs7()
{
    if (d_type == e_PKCS7) {
        d_pkcs7.object().reset();
    }
    else {
        this->reset();
        new (d_pkcs7.buffer()) ntca::EncryptionStoragePkcs7(d_allocator_p);
        d_type = e_PKCS7;
    }

    return d_pkcs7.object();
}

ntca::EncryptionStoragePkcs7& EncryptionStorage::makePkcs7(
    const ntca::EncryptionStoragePkcs7& value)
{
    if (d_type == e_PKCS7) {
        d_pkcs7.object() = value;
    }
    else {
        this->reset();
        new (d_pkcs7.buffer()) ntca::EncryptionStoragePkcs7(value, d_allocator_p);
        d_type = e_PKCS7;
    }

    return d_pkcs7.object();
}

ntca::EncryptionStoragePkcs8& EncryptionStorage::makePkcs8()
{
    if (d_type == e_PKCS8) {
        d_pkcs8.object().reset();
    }
    else {
        this->reset();
        new (d_pkcs8.buffer()) ntca::EncryptionStoragePkcs8(d_allocator_p);
        d_type = e_PKCS8;
    }

    return d_pkcs8.object();
}

ntca::EncryptionStoragePkcs8& EncryptionStorage::makePkcs8(
    const ntca::EncryptionStoragePkcs8& value)
{
    if (d_type == e_PKCS8) {
        d_pkcs8.object() = value;
    }
    else {
        this->reset();
        new (d_pkcs8.buffer()) ntca::EncryptionStoragePkcs8(value, d_allocator_p);
        d_type = e_PKCS8;
    }

    return d_pkcs8.object();
}






ntca::EncryptionStoragePkcs12& EncryptionStorage::makePkcs12()
{
    if (d_type == e_PKCS12) {
        d_pkcs12.object().reset();
    }
    else {
        this->reset();
        new (d_pkcs12.buffer()) ntca::EncryptionStoragePkcs12(d_allocator_p);
        d_type = e_PKCS12;
    }

    return d_pkcs12.object();
}

ntca::EncryptionStoragePkcs12& EncryptionStorage::makePkcs12(
    const ntca::EncryptionStoragePkcs12& value)
{
    if (d_type == e_PKCS12) {
        d_pkcs12.object() = value;
    }
    else {
        this->reset();
        new (d_pkcs12.buffer()) ntca::EncryptionStoragePkcs12(value, d_allocator_p);
        d_type = e_PKCS12;
    }

    return d_pkcs12.object();
}




ntca::EncryptionStoragePem& EncryptionStorage::makePem()
{
    if (d_type == e_PEM) {
        d_pem.object().reset();
    }
    else {
        this->reset();
        new (d_pem.buffer()) ntca::EncryptionStoragePem(d_allocator_p);
        d_type = e_PEM;
    }

    return d_pem.object();
}

ntca::EncryptionStoragePem& EncryptionStorage::makePem(
    const ntca::EncryptionStoragePem& value)
{
    if (d_type == e_PEM) {
        d_pem.object() = value;
    }
    else {
        this->reset();
        new (d_pem.buffer()) ntca::EncryptionStoragePem(value, d_allocator_p);
        d_type = e_PEM;
    }

    return d_pem.object();
}




ntca::EncryptionKey& EncryptionStorage::key()
{
    BSLS_ASSERT(d_type == e_KEY);
    return d_key.object();
}

ntca::EncryptionCertificate& EncryptionStorage::certificate()
{
    BSLS_ASSERT(d_type == e_CERTIFICATE);
    return d_certificate.object();
}

ntca::EncryptionStoragePkcs7& EncryptionStorage::pkcs7()
{
    BSLS_ASSERT(d_type == e_PKCS7);
    return d_pkcs7.object();
}

ntca::EncryptionStoragePkcs8& EncryptionStorage::pkcs8()
{
    BSLS_ASSERT(d_type == e_PKCS8);
    return d_pkcs8.object();
}

ntca::EncryptionStoragePkcs12& EncryptionStorage::pkcs12()
{
    BSLS_ASSERT(d_type == e_PKCS12);
    return d_pkcs12.object();
}

ntca::EncryptionStoragePem& EncryptionStorage::pem()
{
    BSLS_ASSERT(d_type == e_PEM);
    return d_pem.object();
}

const ntca::EncryptionKey& EncryptionStorage::key() const
{
    BSLS_ASSERT(d_type == e_KEY);
    return d_key.object();
}

const ntca::EncryptionCertificate& EncryptionStorage::certificate() const
{
    BSLS_ASSERT(d_type == e_CERTIFICATE);
    return d_certificate.object();
}

const ntca::EncryptionStoragePkcs7& EncryptionStorage::pkcs7() const
{
    BSLS_ASSERT(d_type == e_PKCS7);
    return d_pkcs7.object();
}

const ntca::EncryptionStoragePkcs8& EncryptionStorage::pkcs8() const
{
    BSLS_ASSERT(d_type == e_PKCS8);
    return d_pkcs8.object();
}

const ntca::EncryptionStoragePkcs12& EncryptionStorage::pkcs12() const
{
    BSLS_ASSERT(d_type == e_PKCS12);
    return d_pkcs12.object();
}

const ntca::EncryptionStoragePem& EncryptionStorage::pem() const
{
    BSLS_ASSERT(d_type == e_PEM);
    return d_pem.object();
}

bool EncryptionStorage::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionStorage::isKey() const
{
    return d_type == e_KEY;
}

bool EncryptionStorage::isCertificate() const
{
    return d_type == e_CERTIFICATE;
}

bool EncryptionStorage::isPkcs7() const
{
    return d_type == e_PKCS7;
}

bool EncryptionStorage::isPkcs8() const
{
    return d_type == e_PKCS8;
}

bool EncryptionStorage::isPkcs12() const
{
    return d_type == e_PKCS12;
}

bool EncryptionStorage::isPem() const
{
    return d_type == e_PEM;
}

bool EncryptionStorage::equals(const EncryptionStorage& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_KEY) {
        return d_key.object().equals(other.d_key.object());
    }
    else if (d_type == e_CERTIFICATE) {
        return d_certificate.object().equals(other.d_certificate.object());
    }
    else if (d_type == e_PKCS7) {
        return d_pkcs7.object().equals(other.d_pkcs7.object());
    }
    else if (d_type == e_PKCS8) {
        return d_pkcs8.object().equals(other.d_pkcs8.object());
    }
    else if (d_type == e_PKCS12) {
        return d_pkcs12.object().equals(other.d_pkcs12.object());
    }
    else if (d_type == e_PEM) {
        return d_pem.object().equals(other.d_pem.object());
    }

    return true;
}

bool EncryptionStorage::less(const EncryptionStorage& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_KEY) {
        return d_key.object().less(other.d_key.object());
    }
    else if (d_type == e_CERTIFICATE) {
        return d_certificate.object().less(other.d_certificate.object());
    }
    else if (d_type == e_PKCS7) {
        return d_pkcs7.object().less(other.d_pkcs7.object());
    }
    else if (d_type == e_PKCS8) {
        return d_pkcs8.object().less(other.d_pkcs8.object());
    }
    else if (d_type == e_PKCS12) {
        return d_pkcs12.object().less(other.d_pkcs12.object());
    }
    else if (d_type == e_PEM) {
        return d_pem.object().less(other.d_pem.object());
    }

    return true;
}

bsl::ostream& EncryptionStorage::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    if (d_type == e_KEY) {
        return d_key.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_CERTIFICATE) {
        return d_certificate.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_PKCS7) {
        return d_pkcs7.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_PKCS8) {
        return d_pkcs8.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_PKCS12) {
        return d_pkcs12.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_PEM) {
        return d_pem.object().print(stream, level, spacesPerLevel);
    }

    stream << "UNDEFINED";
    return stream;
}


bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionStorage& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionStorage& lhs, const EncryptionStorage& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionStorage& lhs, const EncryptionStorage& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionStorage& lhs, const EncryptionStorage& rhs)
{
    return lhs.less(rhs);
}












}  // close package namespace
}  // close enterprise namespace
