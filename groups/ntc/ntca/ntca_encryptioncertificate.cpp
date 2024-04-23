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

#include <ntca_encryptioncertificate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptioncertificate_cpp, "$Id$ $CSID$")

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>

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

// MRM
#if 0
const ntsa::AbstractSyntaxTagNumber::Value k_BOOLEAN = 
    ntsa::AbstractSyntaxTagNumber::e_BOOLEAN;
#endif

const ntsa::AbstractSyntaxTagNumber::Value k_INTEGER = 
    ntsa::AbstractSyntaxTagNumber::e_INTEGER;

const ntsa::AbstractSyntaxTagNumber::Value k_OBJECT_IDENTIFIER = 
    ntsa::AbstractSyntaxTagNumber::e_OBJECT_IDENTIFIER;

} // close unnamed namespace




EncryptionCertificateTemplate::EncryptionCertificateTemplate(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateTemplate::EncryptionCertificateTemplate(
    const EncryptionCertificateTemplate& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateTemplate::~EncryptionCertificateTemplate()
{
}

EncryptionCertificateTemplate& EncryptionCertificateTemplate::operator=(
    const EncryptionCertificateTemplate& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateTemplate::reset()
{
    d_value = 0;
}

void EncryptionCertificateTemplate::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateTemplate::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    NTCCFG_WARNING_UNUSED(decoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificateTemplate::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateTemplate::value() const
{
    return d_value;
}

bool EncryptionCertificateTemplate::equals(
    const EncryptionCertificateTemplate& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateTemplate::less(
    const EncryptionCertificateTemplate& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateTemplate::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateTemplate& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateTemplate& lhs,
                const EncryptionCertificateTemplate& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateTemplate& lhs,
                const EncryptionCertificateTemplate& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateTemplate& lhs,
               const EncryptionCertificateTemplate& rhs)
{
    return lhs.less(rhs);
}



















EncryptionCertificateVersion::EncryptionCertificateVersion(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateVersion::EncryptionCertificateVersion(
    const EncryptionCertificateVersion& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateVersion::~EncryptionCertificateVersion()
{
}

EncryptionCertificateVersion& EncryptionCertificateVersion::operator=(
    const EncryptionCertificateVersion& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateVersion::reset()
{
    d_value = 0;
}

void EncryptionCertificateVersion::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateVersion::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeContext(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, 0);
    if (error) {
        return error;
    }

    error = decoder->decodeContext(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
    if (error) {
        return error;
    }

    error = decoder->decodePrimitiveValue(&d_value);
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateVersion::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateVersion::value() const
{
    return d_value;
}

bool EncryptionCertificateVersion::equals(
    const EncryptionCertificateVersion& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateVersion::less(
    const EncryptionCertificateVersion& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateVersion::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateVersion& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateVersion& lhs,
                const EncryptionCertificateVersion& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateVersion& lhs,
                const EncryptionCertificateVersion& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateVersion& lhs,
               const EncryptionCertificateVersion& rhs)
{
    return lhs.less(rhs);
}















EncryptionCertificateNameAttributeType::EncryptionCertificateNameAttributeType(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttributeType::EncryptionCertificateNameAttributeType(
    const EncryptionCertificateNameAttributeType& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttributeType::~EncryptionCertificateNameAttributeType()
{
}

EncryptionCertificateNameAttributeType& EncryptionCertificateNameAttributeType::operator=(
    const EncryptionCertificateNameAttributeType& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateNameAttributeType::reset()
{
    d_value = 0;
}

void EncryptionCertificateNameAttributeType::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateNameAttributeType::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    NTCCFG_WARNING_UNUSED(decoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificateNameAttributeType::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateNameAttributeType::value() const
{
    return d_value;
}

bool EncryptionCertificateNameAttributeType::equals(
    const EncryptionCertificateNameAttributeType& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateNameAttributeType::less(
    const EncryptionCertificateNameAttributeType& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateNameAttributeType::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateNameAttributeType& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateNameAttributeType& lhs,
                const EncryptionCertificateNameAttributeType& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateNameAttributeType& lhs,
                const EncryptionCertificateNameAttributeType& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateNameAttributeType& lhs,
               const EncryptionCertificateNameAttributeType& rhs)
{
    return lhs.less(rhs);
}













EncryptionCertificateNameAttribute::EncryptionCertificateNameAttribute(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttribute::EncryptionCertificateNameAttribute(
    const EncryptionCertificateNameAttribute& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttribute::~EncryptionCertificateNameAttribute()
{
}

EncryptionCertificateNameAttribute& EncryptionCertificateNameAttribute::operator=(
    const EncryptionCertificateNameAttribute& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateNameAttribute::reset()
{
    d_value = 0;
}

void EncryptionCertificateNameAttribute::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateNameAttribute::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    NTCCFG_WARNING_UNUSED(decoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificateNameAttribute::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateNameAttribute::value() const
{
    return d_value;
}

bool EncryptionCertificateNameAttribute::equals(
    const EncryptionCertificateNameAttribute& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateNameAttribute::less(
    const EncryptionCertificateNameAttribute& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateNameAttribute::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateNameAttribute& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateNameAttribute& lhs,
                const EncryptionCertificateNameAttribute& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateNameAttribute& lhs,
                const EncryptionCertificateNameAttribute& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateNameAttribute& lhs,
               const EncryptionCertificateNameAttribute& rhs)
{
    return lhs.less(rhs);
}


















EncryptionCertificateName::EncryptionCertificateName(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateName::EncryptionCertificateName(
    const EncryptionCertificateName& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateName::~EncryptionCertificateName()
{
}

EncryptionCertificateName& EncryptionCertificateName::operator=(
    const EncryptionCertificateName& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateName::reset()
{
    d_value = 0;
}

void EncryptionCertificateName::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateName::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeContext(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = decoder->skip();
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateName::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateName::value() const
{
    return d_value;
}

bool EncryptionCertificateName::equals(
    const EncryptionCertificateName& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateName::less(
    const EncryptionCertificateName& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateName::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateName& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateName& lhs,
                const EncryptionCertificateName& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateName& lhs,
                const EncryptionCertificateName& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateName& lhs,
               const EncryptionCertificateName& rhs)
{
    return lhs.less(rhs);
}








EncryptionCertificateValidity::EncryptionCertificateValidity(
    bslma::Allocator* basicAllocator)
: d_from()
, d_thru()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // MRM
#if 0
    d_from = bdlt::CurrentTime::asDatetimeTz();

    bdlt::Datetime localExpirationTime = d_from.localDatetime();
    localExpirationTime.addDays(365);

    d_thru.setDatetimeTz(localExpirationTime, d_from.offset());
#endif
}

EncryptionCertificateValidity::EncryptionCertificateValidity(
    const EncryptionCertificateValidity& original,
    bslma::Allocator*                    basicAllocator)
: d_from(original.d_from)
, d_thru(original.d_thru)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateValidity::~EncryptionCertificateValidity()
{
}

EncryptionCertificateValidity& EncryptionCertificateValidity::operator=(
    const EncryptionCertificateValidity& other)
{
    if (this != &other) {
        d_from = other.d_from;
        d_thru = other.d_thru;
    }

    return *this;
}

void EncryptionCertificateValidity::reset()
{
    d_from = bdlt::DatetimeTz();
    d_thru = bdlt::DatetimeTz();
}

void EncryptionCertificateValidity::setFrom(const bdlt::DatetimeTz& value)
{
    d_from = value;
}

void EncryptionCertificateValidity::setThru(const bdlt::DatetimeTz& value)
{
    d_thru = value;
}


ntsa::Error EncryptionCertificateValidity::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeContext(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = decoder->decodeContext();
        if (error) {
            return error;
        }

        error = decoder->decodePrimitiveValue(&d_from);
        if (error) {
            return error;
        }

        error = decoder->decodeContextComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeContext();
        if (error) {
            return error;
        }

        error = decoder->decodePrimitiveValue(&d_thru);
        if (error) {
            return error;
        }

        error = decoder->decodeContextComplete();
        if (error) {
            return error;
        }
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateValidity::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const bdlt::DatetimeTz& EncryptionCertificateValidity::from() const
{
    return d_from;
}

const bdlt::DatetimeTz& EncryptionCertificateValidity::thru() const
{
    return d_thru;
}

bool EncryptionCertificateValidity::equals(
    const EncryptionCertificateValidity& other) const
{
    return d_from == other.d_from && d_thru == other.d_thru;
}

bool EncryptionCertificateValidity::less(
    const EncryptionCertificateValidity& other) const
{
    if (d_from.utcDatetime() < other.d_from.utcDatetime()) {
        return true;
    }

    if (other.d_from.utcDatetime() < d_from.utcDatetime()) {
        return false;
    }

    return d_thru.utcDatetime() < other.d_thru.utcDatetime();
}

bsl::ostream& EncryptionCertificateValidity::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("from", d_from);
    printer.printAttribute("thru", d_thru);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateValidity& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateValidity& lhs,
                const EncryptionCertificateValidity& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateValidity& lhs,
                const EncryptionCertificateValidity& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateValidity& lhs,
               const EncryptionCertificateValidity& rhs)
{
    return lhs.less(rhs);
}


















EncryptionCertificatePublicKeyInfo::EncryptionCertificatePublicKeyInfo(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyInfo::EncryptionCertificatePublicKeyInfo(
    const EncryptionCertificatePublicKeyInfo& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyInfo::~EncryptionCertificatePublicKeyInfo()
{
}

EncryptionCertificatePublicKeyInfo& EncryptionCertificatePublicKeyInfo::operator=(
    const EncryptionCertificatePublicKeyInfo& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificatePublicKeyInfo::reset()
{
    d_value = 0;
}

void EncryptionCertificatePublicKeyInfo::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificatePublicKeyInfo::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeContext(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = decoder->skip();
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificatePublicKeyInfo::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificatePublicKeyInfo::value() const
{
    return d_value;
}

bool EncryptionCertificatePublicKeyInfo::equals(
    const EncryptionCertificatePublicKeyInfo& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificatePublicKeyInfo::less(
    const EncryptionCertificatePublicKeyInfo& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificatePublicKeyInfo::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificatePublicKeyInfo& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePublicKeyInfo& lhs,
                const EncryptionCertificatePublicKeyInfo& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePublicKeyInfo& lhs,
                const EncryptionCertificatePublicKeyInfo& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePublicKeyInfo& lhs,
               const EncryptionCertificatePublicKeyInfo& rhs)
{
    return lhs.less(rhs);
}

















EncryptionCertificateAlgorithm::EncryptionCertificateAlgorithm(
    bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateAlgorithm::EncryptionCertificateAlgorithm(
    const EncryptionCertificateAlgorithm& original,
    bslma::Allocator*                    basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateAlgorithm::~EncryptionCertificateAlgorithm()
{
}

EncryptionCertificateAlgorithm& EncryptionCertificateAlgorithm::operator=(
    const EncryptionCertificateAlgorithm& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

void EncryptionCertificateAlgorithm::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateAlgorithm::setIdentifier(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}


ntsa::Error EncryptionCertificateAlgorithm::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeContext(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = decoder->decodeContext(k_UNIVERSAL, k_PRIMITIVE, k_OBJECT_IDENTIFIER);
    if (error) {
        return error;
    }

    error = decoder->decodePrimitiveValue(&d_identifier);
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateAlgorithm::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const ntsa::AbstractObjectIdentifier& EncryptionCertificateAlgorithm::identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateAlgorithm::equals(
    const EncryptionCertificateAlgorithm& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateAlgorithm::less(
    const EncryptionCertificateAlgorithm& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateAlgorithm::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("identifier", d_identifier);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateAlgorithm& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateAlgorithm& lhs,
                const EncryptionCertificateAlgorithm& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateAlgorithm& lhs,
                const EncryptionCertificateAlgorithm& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateAlgorithm& lhs,
               const EncryptionCertificateAlgorithm& rhs)
{
    return lhs.less(rhs);
}













EncryptionCertificateSignature::EncryptionCertificateSignature(
    bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignature::EncryptionCertificateSignature(
    const EncryptionCertificateSignature& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignature::~EncryptionCertificateSignature()
{
}

EncryptionCertificateSignature& EncryptionCertificateSignature::operator=(
    const EncryptionCertificateSignature& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateSignature::reset()
{
    d_value.reset();
}

void EncryptionCertificateSignature::setValue(const ntsa::AbstractBitSequence& value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateSignature::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeContext(ntsa::AbstractSyntaxTagClass::e_UNIVERSAL, 
                                   ntsa::AbstractSyntaxTagType::e_PRIMITIVE, 
                                   ntsa::AbstractSyntaxTagNumber::e_BIT_STRING);
    if (error) {
        return error;
    }

    error = decoder->decodePrimitiveValue(&d_value);
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateSignature::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const ntsa::AbstractBitSequence& EncryptionCertificateSignature::value() const
{
    return d_value;
}

bool EncryptionCertificateSignature::equals(
    const EncryptionCertificateSignature& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateSignature::less(
    const EncryptionCertificateSignature& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateSignature::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateSignature& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSignature& lhs,
                const EncryptionCertificateSignature& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSignature& lhs,
                const EncryptionCertificateSignature& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSignature& lhs,
               const EncryptionCertificateSignature& rhs)
{
    return lhs.less(rhs);
}


















EncryptionCertificateExtensionType::EncryptionCertificateExtensionType(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionType::EncryptionCertificateExtensionType(
    const EncryptionCertificateExtensionType& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionType::~EncryptionCertificateExtensionType()
{
}

EncryptionCertificateExtensionType& EncryptionCertificateExtensionType::operator=(
    const EncryptionCertificateExtensionType& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateExtensionType::reset()
{
    d_value = 0;
}

void EncryptionCertificateExtensionType::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateExtensionType::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    NTCCFG_WARNING_UNUSED(decoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificateExtensionType::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateExtensionType::value() const
{
    return d_value;
}

bool EncryptionCertificateExtensionType::equals(
    const EncryptionCertificateExtensionType& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateExtensionType::less(
    const EncryptionCertificateExtensionType& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateExtensionType::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateExtensionType& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateExtensionType& lhs,
                const EncryptionCertificateExtensionType& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateExtensionType& lhs,
                const EncryptionCertificateExtensionType& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateExtensionType& lhs,
               const EncryptionCertificateExtensionType& rhs)
{
    return lhs.less(rhs);
}













EncryptionCertificateExtensionValue::EncryptionCertificateExtensionValue(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionValue::EncryptionCertificateExtensionValue(
    const EncryptionCertificateExtensionValue& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionValue::~EncryptionCertificateExtensionValue()
{
}

EncryptionCertificateExtensionValue& EncryptionCertificateExtensionValue::operator=(
    const EncryptionCertificateExtensionValue& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateExtensionValue::reset()
{
    d_value = 0;
}

void EncryptionCertificateExtensionValue::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateExtensionValue::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    NTCCFG_WARNING_UNUSED(decoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificateExtensionValue::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateExtensionValue::value() const
{
    return d_value;
}

bool EncryptionCertificateExtensionValue::equals(
    const EncryptionCertificateExtensionValue& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateExtensionValue::less(
    const EncryptionCertificateExtensionValue& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateExtensionValue::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateExtensionValue& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateExtensionValue& lhs,
                const EncryptionCertificateExtensionValue& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateExtensionValue& lhs,
                const EncryptionCertificateExtensionValue& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateExtensionValue& lhs,
               const EncryptionCertificateExtensionValue& rhs)
{
    return lhs.less(rhs);
}















EncryptionCertificateExtension::EncryptionCertificateExtension(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtension::EncryptionCertificateExtension(
    const EncryptionCertificateExtension& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtension::~EncryptionCertificateExtension()
{
}

EncryptionCertificateExtension& EncryptionCertificateExtension::operator=(
    const EncryptionCertificateExtension& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateExtension::reset()
{
    d_value = 0;
}

void EncryptionCertificateExtension::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateExtension::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    NTCCFG_WARNING_UNUSED(decoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificateExtension::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateExtension::value() const
{
    return d_value;
}

bool EncryptionCertificateExtension::equals(
    const EncryptionCertificateExtension& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateExtension::less(
    const EncryptionCertificateExtension& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateExtension::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateExtension& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateExtension& lhs,
                const EncryptionCertificateExtension& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateExtension& lhs,
                const EncryptionCertificateExtension& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateExtension& lhs,
               const EncryptionCertificateExtension& rhs)
{
    return lhs.less(rhs);
}




















EncryptionCertificateExtensionList::EncryptionCertificateExtensionList(
    bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionList::EncryptionCertificateExtensionList(
    const EncryptionCertificateExtensionList& original,
    bslma::Allocator*                    basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionList::~EncryptionCertificateExtensionList()
{
}

EncryptionCertificateExtensionList& EncryptionCertificateExtensionList::operator=(
    const EncryptionCertificateExtensionList& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateExtensionList::reset()
{
    d_value = 0;
}

void EncryptionCertificateExtensionList::setValue(bsl::size_t value)
{
    d_value = value;
}


ntsa::Error EncryptionCertificateExtensionList::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeContext(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = decoder->skip();
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateExtensionList::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bsl::size_t EncryptionCertificateExtensionList::value() const
{
    return d_value;
}

bool EncryptionCertificateExtensionList::equals(
    const EncryptionCertificateExtensionList& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateExtensionList::less(
    const EncryptionCertificateExtensionList& other) const
{
#if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
#endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateExtensionList::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificateExtensionList& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateExtensionList& lhs,
                const EncryptionCertificateExtensionList& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateExtensionList& lhs,
                const EncryptionCertificateExtensionList& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateExtensionList& lhs,
               const EncryptionCertificateExtensionList& rhs)
{
    return lhs.less(rhs);
}

















EncryptionCertificateEntity::EncryptionCertificateEntity(bslma::Allocator* basicAllocator)
: d_version(basicAllocator)
, d_serialNumber(basicAllocator)
, d_signatureAlgorithm(basicAllocator)
, d_issuer(basicAllocator)
, d_validity(basicAllocator)
, d_subject(basicAllocator)
, d_subjectPublicKeyInfo(basicAllocator)
, d_issuerUniqueId(basicAllocator)
, d_subjectUniqueId(basicAllocator)
, d_extensionList(basicAllocator)
{
}

EncryptionCertificateEntity::EncryptionCertificateEntity(
    const EncryptionCertificateEntity& original,
    bslma::Allocator*            basicAllocator)
: d_version(original.d_version, basicAllocator)
, d_serialNumber(original.d_serialNumber, basicAllocator)
, d_signatureAlgorithm(original.d_signatureAlgorithm, basicAllocator)
, d_issuer(original.d_issuer, basicAllocator)
, d_validity(original.d_validity, basicAllocator)
, d_subject(original.d_subject, basicAllocator)
, d_subjectPublicKeyInfo(original.d_subjectPublicKeyInfo, basicAllocator)
, d_issuerUniqueId(original.d_issuerUniqueId, basicAllocator)
, d_subjectUniqueId(original.d_subjectUniqueId, basicAllocator)
, d_extensionList(original.d_extensionList, basicAllocator)
{
}

EncryptionCertificateEntity::~EncryptionCertificateEntity()
{
}

EncryptionCertificateEntity& EncryptionCertificateEntity::operator=(
    const EncryptionCertificateEntity& other)
{
    if (this != &other) {
        d_version = other.d_version;
        d_serialNumber = other.d_serialNumber;
        d_signatureAlgorithm = other.d_signatureAlgorithm;
        d_issuer = other.d_issuer;
        d_validity = other.d_validity;
        d_subject = other.d_subject;
        d_subjectPublicKeyInfo = other.d_subjectPublicKeyInfo;
        d_issuerUniqueId = other.d_issuerUniqueId;
        d_subjectUniqueId = other.d_subjectUniqueId;
        d_extensionList = other.d_extensionList;
    }

    return *this;
}

void EncryptionCertificateEntity::reset()
{
    d_version.reset();
    d_serialNumber.reset();
    d_signatureAlgorithm.reset();
    d_issuer.reset();
    d_validity.reset();
    d_subject.reset();
    d_subjectPublicKeyInfo.reset();
    d_issuerUniqueId.reset();
    d_subjectUniqueId.reset();
    d_extensionList.reset();
}

// MRM
#if 0
void EncryptionCertificateEntity::setSerialNumber(int value)
{
    d_serialNumber = value;
}

void EncryptionCertificateEntity::setStartTime(const bdlt::DatetimeTz& value)
{
    d_startTime = value;
}

void EncryptionCertificateEntity::setExpirationTime(const bdlt::DatetimeTz& value)
{
    d_expirationTime = value;
}

void EncryptionCertificateEntity::setAuthority(bool value)
{
    d_authority = value;
}

void EncryptionCertificateEntity::setHostList(const bsl::vector<bsl::string>& value)
{
    d_hosts = value;
}

void EncryptionCertificateEntity::addHost(const bsl::string& value)
{
    d_hosts.push_back(value);
}
#endif

ntsa::Error EncryptionCertificateEntity::decode(ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    // Enter TBSCertificate

    error = decoder->decodeContext(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    // Decode version.

    error = d_version.decode(decoder);
    if (error) {
        return error;
    }

    // Enter TBSCertificate -> serialNumber

    error = decoder->decodeContext(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
    if (error) {
        return error;
    }

    error = decoder->decodePrimitiveValue(&d_serialNumber);
    if (error) {
        return error;
    }

    // Leave TBSCertificate -> serialNumber

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    error = d_signatureAlgorithm.decode(decoder);
    if (error) {
        return error;
    }

    error = d_issuer.decode(decoder);
    if (error) {
        return error;
    }

    error = d_validity.decode(decoder);
    if (error) {
        return error;
    }

    error = d_subject.decode(decoder);
    if (error) {
        return error;
    }

    error = d_subjectPublicKeyInfo.decode(decoder);
    if (error) {
        return error;
    }


    while (decoder->position() < decoder->current().contentPosition() + 
                                 decoder->current().contentLength().value())
    {
        error = decoder->decodeContext();
        if (error) {
            return error;
        }

        if (decoder->current().tagClass()  == k_CONTEXT_SPECIFIC &&
            decoder->current().tagType()   == k_PRIMITIVE &&
            decoder->current().tagNumber() == 1)
        {
            error = decoder->decodePrimitiveValue(
                &d_issuerUniqueId.makeValue());
            if (error) {
                return error;
            }
        }
        else if (decoder->current().tagClass()  == k_CONTEXT_SPECIFIC &&
                 decoder->current().tagType()   == k_PRIMITIVE &&
                 decoder->current().tagNumber() == 2)
        {
            error = decoder->decodePrimitiveValue(
                &d_subjectUniqueId.makeValue());
            if (error) {
                return error;
            }
        }
        else if (decoder->current().tagClass()  == k_CONTEXT_SPECIFIC &&
                 decoder->current().tagType()   == k_CONSTRUCTED &&
                 decoder->current().tagNumber() == 3)
        {
            error = d_extensionList.makeValue().decode(decoder);
            if (error) {
                return error;
            }
        }
        else
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->decodeContextComplete();
        if (error) {
            return error;
        }
    }




    

    // Leave TBSCertificate

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();

#if 0

    typedef ntsa::AbstractSyntaxTagClass  TC;
    typedef ntsa::AbstractSyntaxTagType   TT;
    typedef ntsa::AbstractSyntaxTagNumber TN;

    const TC::Value universal       = TC::e_UNIVERSAL;
    const TC::Value contextSpecific = TC::e_CONTEXT_SPECIFIC;
    const TT::Value constructed     = TT::e_CONSTRUCTED;
    const TT::Value primitive       = TT::e_PRIMITIVE;

    // Enter Certificate.

    error = decoder->decodeContext(universal, constructed, TN::e_SEQUENCE);
    if (error) {
        return error;
    }

    // Enter Certificate -> TBSCertificate.

    error = decoder->decodeContext(universal, constructed, TN::e_SEQUENCE);
    if (error) {
        return error;
    }

    // Enter Certificate -> TBSCertificate -> Version.

    error = decoder->decodeContext(contextSpecific, constructed, 0);
    if (error) {
        return error;
    }

    error = decoder->decodeContext(universal, primitive, TN::e_INTEGER);
    if (error) {
        return error;
    }

    error = decoder->decodePrimitiveValue(&d_version);
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    // Leave Certificate -> TBSCertificate -> Version.

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }






    // Enter Certificate -> TBSCertificate -> serialNumber.

    error = decoder->decodeContext(universal, primitive, TN::e_INTEGER);
    if (error) {
        return error;
    }

    error = decoder->decodePrimitiveValue(&d_serialNumber);
    if (error) {
        return error;
    }

    // Leave Certificate -> TBSCertificate -> serialNumber.

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }




    // Enter Certificate -> TBSCertificate -> signature.

    error = decoder->decodeContext(universal, constructed, TN::e_SEQUENCE);
    if (error) {
        return error;
    }

    // Enter Certificate -> TBSCertificate -> signature -> algorithm.

    error = decoder->decodeContext(universal, primitive, TN::e_OBJECT_IDENTIFIER);
    if (error) {
        return error;
    }

    error = decoder->decodePrimitiveValue(&d_signatureAlgorithm);
    if (error) {
        return error;
    }

    // Leave Certificate -> TBSCertificate -> signature -> algorithm.

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    // Leave Certificate -> TBSCertificate -> signature.

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }









    // Leave Certificate -> TBSCertificate end.

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    // Leave Certificate end.

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();

#endif
}

ntsa::Error EncryptionCertificateEntity::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

// MRM
#if 0
int EncryptionCertificateEntity::serialNumber() const
{
    ntsa::Error error;

    int result = 0;
    error      = d_serialNumber.convert(&result);
    if (error) {
        return 0;
    }

    return result;
}

const bdlt::DatetimeTz& EncryptionCertificateEntity::startTime() const
{
    return d_startTime;
}

const bdlt::DatetimeTz& EncryptionCertificateEntity::expirationTime() const
{
    return d_expirationTime;
}

bool EncryptionCertificateEntity::authority() const
{
    return d_authority;
}

const bsl::vector<bsl::string>& EncryptionCertificateEntity::hosts() const
{
    return d_hosts;
}
#endif

bool EncryptionCertificateEntity::equals(const EncryptionCertificateEntity& other) const
{
    return d_version == other.d_version &&
           d_serialNumber == other.d_serialNumber &&
           d_signatureAlgorithm == other.d_signatureAlgorithm &&
           d_issuer == other.d_issuer &&
           d_validity == other.d_validity &&
           d_subject == other.d_subject &&
           d_subjectPublicKeyInfo == other.d_subjectPublicKeyInfo &&
           d_issuerUniqueId == other.d_issuerUniqueId &&
           d_subjectUniqueId == other.d_subjectUniqueId &&
           d_extensionList == other.d_extensionList;
}

bool EncryptionCertificateEntity::less(const EncryptionCertificateEntity& other) const
{
    if (d_version < other.d_version) {
        return true;
    }

    if (other.d_version < d_version) {
        return false;
    }

    if (d_serialNumber < other.d_serialNumber) {
        return true;
    }

    if (other.d_serialNumber < d_serialNumber) {
        return false;
    }

    if (d_signatureAlgorithm < other.d_signatureAlgorithm) {
        return true;
    }

    if (other.d_signatureAlgorithm < d_signatureAlgorithm) {
        return false;
    }

    if (d_issuer < other.d_issuer) {
        return true;
    }

    if (other.d_issuer < d_issuer) {
        return false;
    }

    if (d_validity < other.d_validity) {
        return true;
    }

    if (other.d_validity < d_validity) {
        return false;
    }

    if (d_subject < other.d_subject) {
        return true;
    }

    if (other.d_subject < d_subject) {
        return false;
    }
    
    if (d_subjectPublicKeyInfo < other.d_subjectPublicKeyInfo) {
        return true;
    }

    if (other.d_subjectPublicKeyInfo < d_subjectPublicKeyInfo) {
        return false;
    }

    if (d_issuerUniqueId < other.d_issuerUniqueId) {
        return true;
    }

    if (other.d_issuerUniqueId < d_issuerUniqueId) {
        return false;
    }

    if (d_subjectUniqueId < other.d_subjectUniqueId) {
        return true;
    }

    if (other.d_subjectUniqueId < d_subjectUniqueId) {
        return false;
    }

    return d_extensionList < other.d_extensionList;
}

bsl::ostream& EncryptionCertificateEntity::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("version", d_version);
    printer.printAttribute("serialNumber", d_serialNumber);
    printer.printAttribute("signatureAlgorithm", d_signatureAlgorithm);
    printer.printAttribute("issuer", d_issuer);
    printer.printAttribute("validity", d_validity);
    printer.printAttribute("subject", d_subject);
    printer.printAttribute("subjectPublicKeyInfo", d_subjectPublicKeyInfo);
    printer.printAttribute("issuerUniqueId", d_issuerUniqueId);
    printer.printAttribute("subjectUniqueId", d_subjectUniqueId);
    printer.printAttribute("extensionList", d_extensionList);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const EncryptionCertificateEntity& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateEntity& lhs,
                const EncryptionCertificateEntity& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateEntity& lhs,
                const EncryptionCertificateEntity& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateEntity& lhs,
               const EncryptionCertificateEntity& rhs)
{
    return lhs.less(rhs);
}
















































EncryptionCertificate::EncryptionCertificate(bslma::Allocator* basicAllocator)
: d_entity(basicAllocator)
, d_signatureAlgorithm(basicAllocator)
, d_signature(basicAllocator)
{
}

EncryptionCertificate::EncryptionCertificate(
    const EncryptionCertificate& original,
    bslma::Allocator*            basicAllocator)
: d_entity(original.d_entity, basicAllocator)
, d_signatureAlgorithm(original.d_signatureAlgorithm, basicAllocator)
, d_signature(original.d_signature, basicAllocator)
{
}

EncryptionCertificate::~EncryptionCertificate()
{
}

EncryptionCertificate& EncryptionCertificate::operator=(
    const EncryptionCertificate& other)
{
    if (this != &other) {
        d_entity             = other.d_entity;
        d_signatureAlgorithm = other.d_signatureAlgorithm;
        d_signature     = other.d_signature;
    }

    return *this;
}

void EncryptionCertificate::reset()
{
    d_entity.reset();
    d_signatureAlgorithm.reset();
    d_signature.reset();
}

// MRM
#if 0
void EncryptionCertificate::setSerialNumber(int value)
{
    d_serialNumber = value;
}

void EncryptionCertificate::setStartTime(const bdlt::DatetimeTz& value)
{
    d_startTime = value;
}

void EncryptionCertificate::setExpirationTime(const bdlt::DatetimeTz& value)
{
    d_expirationTime = value;
}

void EncryptionCertificate::setAuthority(bool value)
{
    d_authority = value;
}

void EncryptionCertificate::setHostList(const bsl::vector<bsl::string>& value)
{
    d_hosts = value;
}

void EncryptionCertificate::addHost(const bsl::string& value)
{
    d_hosts.push_back(value);
}
#endif

ntsa::Error EncryptionCertificate::decode(ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeContext(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = d_entity.decode(decoder);
    if (error) {
        return error;
    }

    error = d_signatureAlgorithm.decode(decoder);
    if (error) {
        return error;
    }

    error = d_signature.decode(decoder);
    if (error) {
        return error;
    }

    error = decoder->decodeContextComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificate::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

// MRM
#if 0
int EncryptionCertificate::serialNumber() const
{
    ntsa::Error error;

    int result = 0;
    error      = d_serialNumber.convert(&result);
    if (error) {
        return 0;
    }

    return result;
}

const bdlt::DatetimeTz& EncryptionCertificate::startTime() const
{
    return d_startTime;
}

const bdlt::DatetimeTz& EncryptionCertificate::expirationTime() const
{
    return d_expirationTime;
}

bool EncryptionCertificate::authority() const
{
    return d_authority;
}

const bsl::vector<bsl::string>& EncryptionCertificate::hosts() const
{
    return d_hosts;
}
#endif


const ntca::EncryptionCertificateEntity& EncryptionCertificate::entity() const
{
    return d_entity;
}

const ntca::EncryptionCertificateAlgorithm& EncryptionCertificate::signatureAlgorithm() const
{
    return d_signatureAlgorithm;
}

const ntca::EncryptionCertificateSignature& EncryptionCertificate::signature() const
{
    return d_signature;
}

bool EncryptionCertificate::equals(const EncryptionCertificate& other) const
{
    return d_entity == other.d_entity &&
           d_signatureAlgorithm == other.d_signatureAlgorithm &&
           d_signature == other.d_signature;
}

bool EncryptionCertificate::less(const EncryptionCertificate& other) const
{
    if (d_entity < other.d_entity) {
        return true;
    }

    if (other.d_entity < d_entity) {
        return false;
    }


    if (d_signatureAlgorithm < other.d_signatureAlgorithm) {
        return true;
    }

    if (other.d_signatureAlgorithm < d_signatureAlgorithm) {
        return false;
    }

    return d_signature < other.d_signature;
}

bsl::ostream& EncryptionCertificate::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("entity", d_entity);
    printer.printAttribute("signatureAlgorithm", d_signatureAlgorithm);
    printer.printAttribute("signature", d_signature);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const EncryptionCertificate& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificate& lhs,
                const EncryptionCertificate& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificate& lhs,
                const EncryptionCertificate& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificate& lhs,
               const EncryptionCertificate& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
