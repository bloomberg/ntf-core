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

#include <bdlb_string.h>
#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>

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

const ntsa::AbstractSyntaxTagNumber::Value k_SET =
    ntsa::AbstractSyntaxTagNumber::e_SET;

const ntsa::AbstractSyntaxTagNumber::Value k_NULL =
    ntsa::AbstractSyntaxTagNumber::e_NULL;

const ntsa::AbstractSyntaxTagNumber::Value k_BOOLEAN =
    ntsa::AbstractSyntaxTagNumber::e_BOOLEAN;

const ntsa::AbstractSyntaxTagNumber::Value k_INTEGER =
    ntsa::AbstractSyntaxTagNumber::e_INTEGER;

// const ntsa::AbstractSyntaxTagNumber::Value k_VISIBLE_STRING =
//     ntsa::AbstractSyntaxTagNumber::e_VISIBLE_STRING;

// const ntsa::AbstractSyntaxTagNumber::Value k_PRINTABLE_STRING =
//     ntsa::AbstractSyntaxTagNumber::e_PRINTABLE_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_UTF8_STRING =
    ntsa::AbstractSyntaxTagNumber::e_UTF8_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_BIT_STRING =
    ntsa::AbstractSyntaxTagNumber::e_BIT_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_OCTET_STRING =
    ntsa::AbstractSyntaxTagNumber::e_OCTET_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_OBJECT_IDENTIFIER =
    ntsa::AbstractSyntaxTagNumber::e_OBJECT_IDENTIFIER;

}  // close unnamed namespace

EncryptionCertificateObject::EncryptionCertificateObject(
    bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateObject::EncryptionCertificateObject(
    const EncryptionCertificateObject& original,
    bslma::Allocator*                  basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateObject::~EncryptionCertificateObject()
{
}

EncryptionCertificateObject& EncryptionCertificateObject::operator=(
    const EncryptionCertificateObject& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateObject::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificateObject::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificateObject::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificateObject::equals(
    const EncryptionCertificateObject& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateObject::less(
    const EncryptionCertificateObject& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateObject::print(bsl::ostream& stream,
                                                 int           level,
                                                 int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionCertificateObject& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateObject& lhs,
                const EncryptionCertificateObject& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateObject& lhs,
                const EncryptionCertificateObject& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateObject& lhs,
               const EncryptionCertificateObject& rhs)
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
    bslma::Allocator*                   basicAllocator)
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

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
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

ntsa::Error EncryptionCertificateVersion::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
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

bsl::ostream& operator<<(bsl::ostream&                       stream,
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

const char* EncryptionCertificateNameAttributeIdentifierType::toString(
    Value value)
{
    switch (value) {
    case e_NAME_PREFIX:
        return "TITLE";
    case e_NAME_GIVEN:
        return "GN";
    case e_NAME_FAMILY:
        return "SN";
    case e_NAME_PSEUDO:
        return "pseudonym";
    case e_NAME_SUFFIX:
        return "generationQualifier";
    case e_NAME_COMMON:
        return "CN";
    case e_ADDRESS_STREET:
        return "STREET";
    case e_ADDRESS_LOCALITY:
        return "L";
    case e_ADDRESS_STATE:
        return "ST";
    case e_ADDRESS_COUNTRY:
        return "C";
    case e_ORGANIZATION:
        return "O";
    case e_ORGANIZATION_UNIT:
        return "OU";
    case e_DOMAIN_COMPONENT:
        return "DC";
    case e_USER_ID:
        return "UID";
    case e_EMAIL:
        return "E";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionCertificateNameAttributeIdentifierType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result,
    Value                           value)
{
    result->reset();

    if (value == e_NAME_PREFIX) {
        result->set(2, 5, 4, 12);
    }
    else if (value == e_NAME_GIVEN) {
        result->set(2, 5, 4, 42);
    }
    else if (value == e_NAME_FAMILY) {
        result->set(2, 5, 4, 4);
    }
    else if (value == e_NAME_PSEUDO) {
        result->set(2, 5, 4, 65);
    }
    else if (value == e_NAME_SUFFIX) {
        result->set(2, 5, 4, 44);
    }
    else if (value == e_NAME_COMMON) {
        result->set(2, 5, 4, 3);
    }
    else if (value == e_ADDRESS_STREET) {
        result->set(2, 5, 4, 9);
    }
    else if (value == e_ADDRESS_LOCALITY) {
        result->set(2, 5, 4, 7);
    }
    else if (value == e_ADDRESS_STATE) {
        result->set(2, 5, 4, 8);
    }
    else if (value == e_ADDRESS_COUNTRY) {
        result->set(2, 5, 4, 6);
    }
    else if (value == e_ORGANIZATION) {
        result->set(2, 5, 4, 10);
    }
    else if (value == e_ORGANIZATION_UNIT) {
        result->set(2, 5, 4, 11);
    }
    else if (value == e_DOMAIN_COMPONENT) {
        result->set(0, 9, 2342, 19200300, 100, 1, 25);
    }
    else if (value == e_USER_ID) {
        result->set(0, 9, 2342, 19200300, 100, 1, 1);
    }
    else if (value == e_EMAIL) {
        result->set(1, 2, 840, 113549, 1, 9, 1);
    }
}

int EncryptionCertificateNameAttributeIdentifierType::fromString(
    EncryptionCertificateNameAttributeIdentifierType::Value* result,
    const bslstl::StringRef&                                 string)
{
    if (bdlb::String::areEqualCaseless(string, "TITLE")) {
        *result = e_NAME_PREFIX;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "GN")) {
        *result = e_NAME_GIVEN;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SN")) {
        *result = e_NAME_FAMILY;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "pseudonym")) {
        *result = e_NAME_PSEUDO;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "generationQualifier")) {
        *result = e_NAME_SUFFIX;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "CN")) {
        *result = e_NAME_COMMON;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "STREET")) {
        *result = e_ADDRESS_STREET;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "L")) {
        *result = e_ADDRESS_LOCALITY;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ST")) {
        *result = e_ADDRESS_STATE;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "C")) {
        *result = e_ADDRESS_COUNTRY;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "O")) {
        *result = e_ORGANIZATION;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "OU")) {
        *result = e_ORGANIZATION_UNIT;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "DC")) {
        *result = e_DOMAIN_COMPONENT;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "UID")) {
        *result = e_USER_ID;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "E")) {
        *result = e_EMAIL;
        return 0;
    }

    return -1;
}

int EncryptionCertificateNameAttributeIdentifierType::fromObjectIdentifier(
    Value*                                result,
    const ntsa::AbstractObjectIdentifier& identifier)
{
    if (identifier.equals(2, 5, 4, 12)) {
        *result = e_NAME_PREFIX;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 42)) {
        *result = e_NAME_GIVEN;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 4)) {
        *result = e_NAME_FAMILY;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 65)) {
        *result = e_NAME_PSEUDO;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 44)) {
        *result = e_NAME_SUFFIX;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 3)) {
        *result = e_NAME_COMMON;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 9)) {
        *result = e_ADDRESS_STREET;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 7)) {
        *result = e_ADDRESS_LOCALITY;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 8)) {
        *result = e_ADDRESS_STATE;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 6)) {
        *result = e_ADDRESS_COUNTRY;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 10)) {
        *result = e_ORGANIZATION;
        return 0;
    }

    if (identifier.equals(2, 5, 4, 11)) {
        *result = e_ORGANIZATION_UNIT;
        return 0;
    }

    if (identifier.equals(0, 9, 2342, 19200300, 100, 1, 25)) {
        *result = e_DOMAIN_COMPONENT;
        return 0;
    }

    if (identifier.equals(0, 9, 2342, 19200300, 100, 1, 1)) {
        *result = e_USER_ID;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 113549, 1, 9, 1)) {
        *result = e_EMAIL;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionCertificateNameAttributeIdentifierType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                           stream,
    EncryptionCertificateNameAttributeIdentifierType::Value rhs)
{
    return EncryptionCertificateNameAttributeIdentifierType::print(stream,
                                                                   rhs);
}

EncryptionCertificateNameAttributeIdentifier::
    EncryptionCertificateNameAttributeIdentifier(
        bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttributeIdentifier::
    EncryptionCertificateNameAttributeIdentifier(
        const EncryptionCertificateNameAttributeIdentifier& original,
        bslma::Allocator*                                   basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttributeIdentifier::
    ~EncryptionCertificateNameAttributeIdentifier()
{
}

EncryptionCertificateNameAttributeIdentifier&
EncryptionCertificateNameAttributeIdentifier::operator=(
    const EncryptionCertificateNameAttributeIdentifier& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificateNameAttributeIdentifier&
EncryptionCertificateNameAttributeIdentifier::operator=(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateNameAttributeIdentifier&
EncryptionCertificateNameAttributeIdentifier::operator=(
    EncryptionCertificateNameAttributeIdentifierType::Value value)
{
    EncryptionCertificateNameAttributeIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
    return *this;
}

void EncryptionCertificateNameAttributeIdentifier::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateNameAttributeIdentifier::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificateNameAttributeIdentifier::setIdentifer(
    EncryptionCertificateNameAttributeIdentifierType::Value value)
{
    EncryptionCertificateNameAttributeIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
}

ntsa::Error EncryptionCertificateNameAttributeIdentifier::decode(
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

ntsa::Error EncryptionCertificateNameAttributeIdentifier::encode(
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
EncryptionCertificateNameAttributeIdentifier::identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateNameAttributeIdentifier::equals(
    const EncryptionCertificateNameAttributeIdentifier& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateNameAttributeIdentifier::equals(
    EncryptionCertificateNameAttributeIdentifierType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateNameAttributeIdentifierType::toObjectIdentifier(
        &identifier,
        value);

    return d_identifier.equals(identifier);
}

bool EncryptionCertificateNameAttributeIdentifier::less(
    const EncryptionCertificateNameAttributeIdentifier& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateNameAttributeIdentifier::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionCertificateNameAttributeIdentifierType::Value type;
    int rc = ntca::EncryptionCertificateNameAttributeIdentifierType::
        fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateNameAttributeIdentifierType::
                toString(type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                       stream,
    const EncryptionCertificateNameAttributeIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateNameAttributeIdentifier& lhs,
                const EncryptionCertificateNameAttributeIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateNameAttributeIdentifier& lhs,
                const EncryptionCertificateNameAttributeIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateNameAttributeIdentifier& lhs,
               const EncryptionCertificateNameAttributeIdentifier& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateNameAttribute::EncryptionCertificateNameAttribute(
    bslma::Allocator* basicAllocator)
: d_attribute(basicAllocator)
, d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttribute::EncryptionCertificateNameAttribute(
    const EncryptionCertificateNameAttribute& original,
    bslma::Allocator*                         basicAllocator)
: d_attribute(original.d_attribute, basicAllocator)
, d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttribute::~EncryptionCertificateNameAttribute()
{
}

EncryptionCertificateNameAttribute& EncryptionCertificateNameAttribute::
operator=(const EncryptionCertificateNameAttribute& other)
{
    if (this != &other) {
        d_attribute = other.d_attribute;
        d_value     = other.d_value;
    }

    return *this;
}

void EncryptionCertificateNameAttribute::reset()
{
    d_attribute.reset();
    d_value.reset();
}

void EncryptionCertificateNameAttribute::setAttribute(
    const EncryptionCertificateNameAttributeIdentifier& value)
{
    d_attribute = value;
}

void EncryptionCertificateNameAttribute::setAttribute(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_attribute = value;
}

void EncryptionCertificateNameAttribute::setAttribute(
    ntca::EncryptionCertificateNameAttributeIdentifierType::Value value)
{
    d_attribute = value;
}

void EncryptionCertificateNameAttribute::setValue(const bsl::string& value)
{
    d_value.setType(k_UTF8_STRING);
    d_value.setValue(value);
}

ntsa::Error EncryptionCertificateNameAttribute::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = d_attribute.decode(decoder);
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag();
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
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateNameAttribute::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = d_attribute.encode(encoder);
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, d_value.type());
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
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

const EncryptionCertificateNameAttributeIdentifier&
EncryptionCertificateNameAttribute::attribute() const
{
    return d_attribute;
}

bsl::string EncryptionCertificateNameAttribute::value() const
{
    ntsa::Error error;

    bsl::string text;
    error = d_value.convert(&text);
    if (!error) {
        return text;
    }

    // TODO: Enhance ntsa::AbstractString::convert to handle T61, etc.
    return "<encoded>";
}

bool EncryptionCertificateNameAttribute::equals(
    const EncryptionCertificateNameAttribute& other) const
{
    return d_attribute == other.d_attribute && d_value == other.d_value;
}

bool EncryptionCertificateNameAttribute::less(
    const EncryptionCertificateNameAttribute& other) const
{
    if (d_attribute < other.d_attribute) {
        return true;
    }

    if (other.d_attribute < d_attribute) {
        return false;
    }

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateNameAttribute::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    EncryptionCertificateNameAttributeIdentifierType::Value type;
    int                                                     rc =
        EncryptionCertificateNameAttributeIdentifierType::fromObjectIdentifier(
            &type,
            d_attribute.identifier());
    if (rc == 0) {
        bslim::Printer printer(&stream, level, spacesPerLevel);
        printer.start();
        printer.printAttribute(
            EncryptionCertificateNameAttributeIdentifierType::toString(type),
            d_value);
        printer.end();
    }
    else {
        bslim::Printer printer(&stream, level, spacesPerLevel);
        printer.start();
        printer.printAttribute("attribute", d_attribute);
        printer.printAttribute("value", d_value);
        printer.end();
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                             stream,
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

void EncryptionCertificateName::format(
    bsl::string*                                                  result,
    ntca::EncryptionCertificateNameAttributeIdentifierType::Value type) const
{
    for (bsl::size_t i = 0; i < d_attributeVector.size(); ++i) {
        const EncryptionCertificateNameAttribute& component =
            d_attributeVector[i];

        if (component.attribute().equals(type)) {
            if (!result->empty()) {
                result->append(1, ',');
                result->append(1, ' ');
            }

            result->append(component.value());
        }
    }
}

void EncryptionCertificateName::formatDot(
    bsl::string*                                                  result,
    ntca::EncryptionCertificateNameAttributeIdentifierType::Value type) const
{
    for (bsl::size_t i = 0; i < d_attributeVector.size(); ++i) {
        const EncryptionCertificateNameAttribute& component =
            d_attributeVector[i];

        if (component.attribute().equals(type)) {
            if (!result->empty()) {
                result->append(1, '.');
            }

            result->append(component.value());
        }
    }
}

EncryptionCertificateName::EncryptionCertificateName(
    bslma::Allocator* basicAllocator)
: d_attributeVector(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateName::EncryptionCertificateName(
    const EncryptionCertificateName& original,
    bslma::Allocator*                basicAllocator)
: d_attributeVector(original.d_attributeVector, basicAllocator)
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
        d_attributeVector = other.d_attributeVector;
    }

    return *this;
}

void EncryptionCertificateName::reset()
{
    d_attributeVector.clear();
}

void EncryptionCertificateName::setAttributeSequence(
    const bsl::vector<EncryptionCertificateNameAttribute>& value)
{
    d_attributeVector = value;
}

void EncryptionCertificateName::append(
    const EncryptionCertificateNameAttribute& value)
{
    d_attributeVector.push_back(value);
}

void EncryptionCertificateName::append(
    const EncryptionCertificateNameAttributeIdentifier& attribute,
    const bsl::string&                                  value)
{
    EncryptionCertificateNameAttribute component;
    component.setAttribute(attribute);
    component.setValue(value);

    this->append(component);
}

void EncryptionCertificateName::append(
    const ntsa::AbstractObjectIdentifier& attribute,
    const bsl::string&                    value)
{
    EncryptionCertificateNameAttribute component;
    component.setAttribute(attribute);
    component.setValue(value);

    this->append(component);
}

void EncryptionCertificateName::append(
    EncryptionCertificateNameAttributeIdentifierType::Value attribute,
    const bsl::string&                                      value)
{
    EncryptionCertificateNameAttribute component;
    component.setAttribute(attribute);
    component.setValue(value);

    this->append(component);
}

ntsa::Error EncryptionCertificateName::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    if (decoder->current().contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    while (decoder->contentBytesRemaining() > 0) {
        error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SET);
        if (error) {
            return error;
        }

        EncryptionCertificateNameAttribute attribute;
        error = attribute.decode(decoder);
        if (error) {
            return error;
        }

        d_attributeVector.push_back(attribute);

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

ntsa::Error EncryptionCertificateName::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    if (d_attributeVector.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    for (bsl::size_t i = 0; i < d_attributeVector.size(); ++i) {
        const EncryptionCertificateNameAttribute& attribute =
            d_attributeVector[i];

        error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SET);
        if (error) {
            return error;
        }

        error = attribute.encode(encoder);
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

const bsl::vector<EncryptionCertificateNameAttribute>& EncryptionCertificateName::
    attributes() const
{
    return d_attributeVector;
}

bsl::string EncryptionCertificateName::standard() const
{
    bsl::string result;

    {
        bsl::string text = this->common();
        if (!text.empty()) {
            result.append(1, ' ');
        }

        result.append("CN=");
        result.append(text);
    }

    {
        bsl::string text = this->organization();
        if (!text.empty()) {
            result.append(1, ' ');
        }

        result.append("O=");
        result.append(text);
    }

    {
        bsl::string text = this->organizationUnit();
        if (!text.empty()) {
            result.append(1, ' ');
        }

        result.append("OU=");
        result.append(text);
    }

    return result;
}

bsl::string EncryptionCertificateName::common() const
{
    bsl::string result;
    this->format(
        &result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::e_NAME_COMMON);

    return result;
}

bsl::string EncryptionCertificateName::prefix() const
{
    bsl::string result;
    this->format(
        &result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::e_NAME_PREFIX);

    return result;
}

bsl::string EncryptionCertificateName::given() const
{
    bsl::string result;
    this->format(
        &result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::e_NAME_GIVEN);

    return result;
}

bsl::string EncryptionCertificateName::family() const
{
    bsl::string result;
    this->format(
        &result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::e_NAME_FAMILY);

    return result;
}

bsl::string EncryptionCertificateName::pseudo() const
{
    bsl::string result;
    this->format(
        &result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::e_NAME_PSEUDO);

    return result;
}

bsl::string EncryptionCertificateName::suffix() const
{
    bsl::string result;
    this->format(
        &result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::e_NAME_SUFFIX);

    return result;
}

bsl::string EncryptionCertificateName::organization() const
{
    bsl::string result;
    this->format(&result,
                 ntca::EncryptionCertificateNameAttributeIdentifierType::
                     e_ORGANIZATION);

    return result;
}

bsl::string EncryptionCertificateName::organizationUnit() const
{
    bsl::string result;
    this->format(&result,
                 ntca::EncryptionCertificateNameAttributeIdentifierType::
                     e_ORGANIZATION_UNIT);

    return result;
}

bsl::string EncryptionCertificateName::addressStreet() const
{
    bsl::string result;
    this->format(&result,
                 ntca::EncryptionCertificateNameAttributeIdentifierType::
                     e_ADDRESS_STREET);

    return result;
}

bsl::string EncryptionCertificateName::addressLocality() const
{
    bsl::string result;
    this->format(&result,
                 ntca::EncryptionCertificateNameAttributeIdentifierType::
                     e_ADDRESS_LOCALITY);

    return result;
}

bsl::string EncryptionCertificateName::addressState() const
{
    bsl::string result;
    this->format(&result,
                 ntca::EncryptionCertificateNameAttributeIdentifierType::
                     e_ADDRESS_STATE);

    return result;
}

bsl::string EncryptionCertificateName::addressCountry() const
{
    bsl::string result;
    this->format(&result,
                 ntca::EncryptionCertificateNameAttributeIdentifierType::
                     e_ADDRESS_COUNTRY);

    return result;
}

bsl::string EncryptionCertificateName::domain() const
{
    bsl::string result;
    this->formatDot(&result,
                    ntca::EncryptionCertificateNameAttributeIdentifierType::
                        e_DOMAIN_COMPONENT);

    return result;
}

bsl::string EncryptionCertificateName::userId() const
{
    bsl::string result;
    this->format(
        &result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::e_USER_ID);

    return result;
}

bsl::string EncryptionCertificateName::email() const
{
    bsl::string result;
    this->format(
        &result,
        ntca::EncryptionCertificateNameAttributeIdentifierType::e_EMAIL);

    return result;
}

bool EncryptionCertificateName::matchesDomainName(
    const bsl::string& domainName) const
{
    const ntca::EncryptionCertificateNameAttributeIdentifierType::Value
        k_COMMON_NAME = ntca::
            EncryptionCertificateNameAttributeIdentifierType::e_NAME_COMMON;

    for (bsl::size_t i = 0; i < d_attributeVector.size(); ++i) {
        const EncryptionCertificateNameAttribute& component =
            d_attributeVector[i];

        if (component.attribute().equals(k_COMMON_NAME)) {
            if (EncryptionCertificateUtil::matches(domainName,
                                                   component.value()))
            {
                return true;
            }
        }
    }

    bsl::string internalDomainName = this->domain();

    if (!internalDomainName.empty()) {
        if (EncryptionCertificateUtil::matches(domainName, internalDomainName))
        {
            return true;
        }
    }

    return false;
}

bool EncryptionCertificateName::matchesIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    const ntca::EncryptionCertificateNameAttributeIdentifierType::Value
        k_COMMON_NAME = ntca::
            EncryptionCertificateNameAttributeIdentifierType::e_NAME_COMMON;

    for (bsl::size_t i = 0; i < d_attributeVector.size(); ++i) {
        const EncryptionCertificateNameAttribute& component =
            d_attributeVector[i];

        if (component.attribute().equals(k_COMMON_NAME)) {
            if (EncryptionCertificateUtil::matches(ipAddress,
                                                   component.value()))
            {
                return true;
            }
        }
    }

    return false;
}

bool EncryptionCertificateName::equals(
    const EncryptionCertificateName& other) const
{
    return d_attributeVector == other.d_attributeVector;
}

bool EncryptionCertificateName::less(
    const EncryptionCertificateName& other) const
{
    return d_attributeVector < other.d_attributeVector;
}

bsl::ostream& EncryptionCertificateName::print(bsl::ostream& stream,
                                               int           level,
                                               int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printValue(d_attributeVector);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                    stream,
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

EncryptionCertificateNameAlternative::EncryptionCertificateNameAlternative(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAlternative::EncryptionCertificateNameAlternative(
    const EncryptionCertificateNameAlternative& original,
    bslma::Allocator*                           basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_OTHER) {
        new (d_other.buffer())
            ntsa::AbstractValue(original.d_other.object(), d_allocator_p);
    }
    else if (d_type == e_EMAIL) {
        new (d_email.buffer())
            bsl::string(original.d_email.object(), d_allocator_p);
    }
    else if (d_type == e_DOMAIN) {
        new (d_domain.buffer())
            bsl::string(original.d_domain.object(), d_allocator_p);
    }
    else if (d_type == e_X400) {
        new (d_x400.buffer())
            ntsa::AbstractValue(original.d_x400.object(), d_allocator_p);
    }
    else if (d_type == e_DIRECTORY) {
        new (d_directory.buffer())
            EncryptionCertificateName(original.d_directory.object(),
                                      d_allocator_p);
    }
    else if (d_type == e_EDI) {
        new (d_edi.buffer())
            ntsa::AbstractValue(original.d_edi.object(), d_allocator_p);
    }
    else if (d_type == e_URI) {
        new (d_uri.buffer()) ntsa::Uri(original.d_uri.object(), d_allocator_p);
    }
    else if (d_type == e_IP) {
        new (d_ip.buffer()) ntsa::IpAddress(original.d_ip.object());
    }
    else if (d_type == e_IDENTIFIER) {
        new (d_identifier.buffer())
            ntsa::AbstractObjectIdentifier(original.d_identifier.object(),
                                           d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionCertificateNameAlternative::~EncryptionCertificateNameAlternative()
{
    this->reset();
}

EncryptionCertificateNameAlternative& EncryptionCertificateNameAlternative::
operator=(const EncryptionCertificateNameAlternative& other)
{
    if (this == &other) {
        return *this;
    }

    if (other.d_type == e_OTHER) {
        this->makeOther(other.d_other.object());
    }
    else if (other.d_type == e_EMAIL) {
        this->makeEmail(other.d_email.object());
    }
    else if (other.d_type == e_DOMAIN) {
        this->makeDomain(other.d_domain.object());
    }
    else if (other.d_type == e_X400) {
        this->makeX400(other.d_x400.object());
    }
    else if (other.d_type == e_DIRECTORY) {
        this->makeDirectory(other.d_directory.object());
    }
    else if (other.d_type == e_EDI) {
        this->makeEdi(other.d_edi.object());
    }
    else if (other.d_type == e_URI) {
        this->makeUri(other.d_uri.object());
    }
    else if (other.d_type == e_IP) {
        this->makeIp(other.d_ip.object());
    }
    else if (other.d_type == e_IDENTIFIER) {
        this->makeIdentifier(other.d_identifier.object());
    }
    else {
        BSLS_ASSERT(other.d_type == e_UNDEFINED);
        this->reset();
    }

    return *this;
}

void EncryptionCertificateNameAlternative::reset()
{
    if (d_type == e_OTHER) {
        typedef ntsa::AbstractValue Type;
        d_other.object().~Type();
    }
    else if (d_type == e_EMAIL) {
        typedef bsl::string Type;
        d_email.object().~Type();
    }
    else if (d_type == e_DOMAIN) {
        typedef bsl::string Type;
        d_domain.object().~Type();
    }
    else if (d_type == e_X400) {
        typedef ntsa::AbstractValue Type;
        d_x400.object().~Type();
    }
    else if (d_type == e_DIRECTORY) {
        typedef EncryptionCertificateName Type;
        d_directory.object().~Type();
    }
    else if (d_type == e_EDI) {
        typedef ntsa::AbstractValue Type;
        d_edi.object().~Type();
    }
    else if (d_type == e_URI) {
        typedef ntsa::Uri Type;
        d_uri.object().~Type();
    }
    else if (d_type == e_IP) {
        typedef ntsa::IpAddress Type;
        d_ip.object().~Type();
    }
    else if (d_type == e_IDENTIFIER) {
        typedef ntsa::AbstractObjectIdentifier Type;
        d_identifier.object().~Type();
    }

    d_type = e_UNDEFINED;
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::makeOther()
{
    if (d_type == e_OTHER) {
        d_other.object().reset();
    }
    else {
        this->reset();
        new (d_other.buffer()) ntsa::AbstractValue(d_allocator_p);
        d_type = e_OTHER;
    }

    return d_other.object();
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::makeOther(
    const ntsa::AbstractValue& value)
{
    if (d_type == e_OTHER) {
        d_other.object() = value;
    }
    else {
        this->reset();
        new (d_other.buffer()) ntsa::AbstractValue(value, d_allocator_p);
        d_type = e_OTHER;
    }

    return d_other.object();
}

bsl::string& EncryptionCertificateNameAlternative::makeEmail()
{
    if (d_type == e_EMAIL) {
        d_email.object().clear();
    }
    else {
        this->reset();
        new (d_email.buffer()) bsl::string(d_allocator_p);
        d_type = e_EMAIL;
    }

    return d_email.object();
}

bsl::string& EncryptionCertificateNameAlternative::makeEmail(
    const bsl::string& value)
{
    if (d_type == e_EMAIL) {
        d_email.object() = value;
    }
    else {
        this->reset();
        new (d_email.buffer()) bsl::string(value, d_allocator_p);
        d_type = e_EMAIL;
    }

    return d_email.object();
}

bsl::string& EncryptionCertificateNameAlternative::makeDomain()
{
    if (d_type == e_DOMAIN) {
        d_domain.object().clear();
    }
    else {
        this->reset();
        new (d_domain.buffer()) bsl::string(d_allocator_p);
        d_type = e_DOMAIN;
    }

    return d_domain.object();
}

bsl::string& EncryptionCertificateNameAlternative::makeDomain(
    const bsl::string& value)
{
    if (d_type == e_DOMAIN) {
        d_domain.object() = value;
    }
    else {
        this->reset();
        new (d_domain.buffer()) bsl::string(value, d_allocator_p);
        d_type = e_DOMAIN;
    }

    return d_domain.object();
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::makeX400()
{
    if (d_type == e_X400) {
        d_x400.object().reset();
    }
    else {
        this->reset();
        new (d_x400.buffer()) ntsa::AbstractValue(d_allocator_p);
        d_type = e_X400;
    }

    return d_x400.object();
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::makeX400(
    const ntsa::AbstractValue& value)
{
    if (d_type == e_X400) {
        d_x400.object() = value;
    }
    else {
        this->reset();
        new (d_x400.buffer()) ntsa::AbstractValue(value, d_allocator_p);
        d_type = e_X400;
    }

    return d_x400.object();
}

EncryptionCertificateName& EncryptionCertificateNameAlternative::
    makeDirectory()
{
    if (d_type == e_DIRECTORY) {
        d_directory.object().reset();
    }
    else {
        this->reset();
        new (d_directory.buffer()) EncryptionCertificateName(d_allocator_p);
        d_type = e_DIRECTORY;
    }

    return d_directory.object();
}

EncryptionCertificateName& EncryptionCertificateNameAlternative::makeDirectory(
    const EncryptionCertificateName& value)
{
    if (d_type == e_DIRECTORY) {
        d_directory.object() = value;
    }
    else {
        this->reset();
        new (d_directory.buffer())
            EncryptionCertificateName(value, d_allocator_p);
        d_type = e_DIRECTORY;
    }

    return d_directory.object();
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::makeEdi()
{
    if (d_type == e_EDI) {
        d_edi.object().reset();
    }
    else {
        this->reset();
        new (d_edi.buffer()) ntsa::AbstractValue(d_allocator_p);
        d_type = e_EDI;
    }

    return d_edi.object();
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::makeEdi(
    const ntsa::AbstractValue& value)
{
    if (d_type == e_EDI) {
        d_edi.object() = value;
    }
    else {
        this->reset();
        new (d_edi.buffer()) ntsa::AbstractValue(value, d_allocator_p);
        d_type = e_EDI;
    }

    return d_edi.object();
}

ntsa::Uri& EncryptionCertificateNameAlternative::makeUri()
{
    if (d_type == e_URI) {
        d_uri.object().reset();
    }
    else {
        this->reset();
        new (d_uri.buffer()) ntsa::Uri(d_allocator_p);
        d_type = e_URI;
    }

    return d_uri.object();
}

ntsa::Uri& EncryptionCertificateNameAlternative::makeUri(
    const ntsa::Uri& value)
{
    if (d_type == e_URI) {
        d_uri.object() = value;
    }
    else {
        this->reset();
        new (d_uri.buffer()) ntsa::Uri(value, d_allocator_p);
        d_type = e_URI;
    }

    return d_uri.object();
}

ntsa::IpAddress& EncryptionCertificateNameAlternative::makeIp()
{
    if (d_type == e_IP) {
        d_ip.object().reset();
    }
    else {
        this->reset();
        new (d_ip.buffer()) ntsa::IpAddress();
        d_type = e_IP;
    }

    return d_ip.object();
}

ntsa::IpAddress& EncryptionCertificateNameAlternative::makeIp(
    const ntsa::IpAddress& value)
{
    if (d_type == e_IP) {
        d_ip.object() = value;
    }
    else {
        this->reset();
        new (d_ip.buffer()) ntsa::IpAddress(value);
        d_type = e_IP;
    }

    return d_ip.object();
}

ntsa::AbstractObjectIdentifier& EncryptionCertificateNameAlternative::
    makeIdentifier()
{
    if (d_type == e_IDENTIFIER) {
        d_identifier.object().reset();
    }
    else {
        this->reset();
        new (d_identifier.buffer())
            ntsa::AbstractObjectIdentifier(d_allocator_p);
        d_type = e_IDENTIFIER;
    }

    return d_identifier.object();
}

ntsa::AbstractObjectIdentifier& EncryptionCertificateNameAlternative::
    makeIdentifier(const ntsa::AbstractObjectIdentifier& value)
{
    if (d_type == e_IDENTIFIER) {
        d_identifier.object() = value;
    }
    else {
        this->reset();
        new (d_identifier.buffer())
            ntsa::AbstractObjectIdentifier(value, d_allocator_p);
        d_type = e_IDENTIFIER;
    }

    return d_identifier.object();
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::other()
{
    BSLS_ASSERT(this->isOther());
    return d_other.object();
}

bsl::string& EncryptionCertificateNameAlternative::email()
{
    BSLS_ASSERT(this->isEmail());
    return d_email.object();
}

bsl::string& EncryptionCertificateNameAlternative::domain()
{
    BSLS_ASSERT(this->isDomain());
    return d_domain.object();
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::x400()
{
    BSLS_ASSERT(this->isX400());
    return d_x400.object();
}

EncryptionCertificateName& EncryptionCertificateNameAlternative::directory()
{
    BSLS_ASSERT(this->isDirectory());
    return d_directory.object();
}

ntsa::AbstractValue& EncryptionCertificateNameAlternative::edi()
{
    BSLS_ASSERT(this->isEdi());
    return d_edi.object();
}

ntsa::Uri& EncryptionCertificateNameAlternative::uri()
{
    BSLS_ASSERT(this->isUri());
    return d_uri.object();
}

ntsa::IpAddress& EncryptionCertificateNameAlternative::ip()
{
    BSLS_ASSERT(this->isIp());
    return d_ip.object();
}

ntsa::AbstractObjectIdentifier& EncryptionCertificateNameAlternative::
    identifier()
{
    BSLS_ASSERT(this->isIdentifier());
    return d_identifier.object();
}

ntsa::Error EncryptionCertificateNameAlternative::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
    if (error) {
        return error;
    }

    const ntsa::AbstractSyntaxDecoderFrame& current = decoder->current();

    if (decoder->current().tagNumber() == e_OTHER) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_CONSTRUCTED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->decodeValue(&this->makeOther());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_EMAIL) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_PRIMITIVE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->decodeValue(&this->makeEmail());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_DOMAIN) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_PRIMITIVE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->decodeValue(&this->makeDomain());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_X400) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_CONSTRUCTED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->decodeValue(&this->makeX400());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_DIRECTORY) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_CONSTRUCTED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = this->makeDirectory().decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_EDI) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_CONSTRUCTED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->decodeValue(&this->makeEdi());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_URI) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_PRIMITIVE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        bsl::string text;
        error = decoder->decodeValue(&text);
        if (error) {
            return error;
        }

        if (!this->makeUri().parse(text)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (decoder->current().tagNumber() == e_IP) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_PRIMITIVE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntsa::AbstractOctetString octets;
        error = decoder->decodeValue(&octets);
        if (error) {
            return error;
        }

        if (octets.size() == 4) {
            ntsa::Ipv4Address& ipv4Address = this->makeIp().makeV4();
            ipv4Address.copyFrom(octets.data(), octets.size());
        }
        else if (octets.size() == 16) {
            ntsa::Ipv6Address& ipv6Address = this->makeIp().makeV6();
            ipv6Address.copyFrom(octets.data(), octets.size());
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (decoder->current().tagNumber() == e_IDENTIFIER) {
        if (current.tagClass() != k_CONTEXT_SPECIFIC) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (current.tagType() != k_PRIMITIVE) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = decoder->decodeValue(&this->makeIdentifier());
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateNameAlternative::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    if (this->isOther()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, d_type);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(this->other());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }
    else if (this->isEmail()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_PRIMITIVE, d_type);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(this->email());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }
    else if (this->isDomain()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_PRIMITIVE, d_type);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(this->domain());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }
    else if (this->isX400()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, d_type);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(this->x400());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }
    else if (this->isDirectory()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, d_type);
        if (error) {
            return error;
        }

        error = this->directory().encode(encoder);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }
    else if (this->isEdi()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, d_type);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(this->edi());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }
    else if (this->isUri()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_PRIMITIVE, d_type);
        if (error) {
            return error;
        }

        bsl::string text = this->uri().text();
        error            = encoder->encodeValue(text);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }
    else if (this->isIp()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_PRIMITIVE, d_type);
        if (error) {
            return error;
        }

        ntsa::AbstractOctetString octets;

        if (this->ip().isV4()) {
            for (bsl::size_t i = 0; i < 4; ++i) {
                octets.set(i, this->ip().v4()[i]);
            }
        }
        else if (this->ip().isV6()) {
            for (bsl::size_t i = 0; i < 16; ++i) {
                octets.set(i, this->ip().v6()[i]);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = encoder->encodeValue(octets);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }
    else if (this->isIdentifier()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_PRIMITIVE, d_type);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(this->identifier());
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

    return ntsa::Error();
}

const ntsa::AbstractValue& EncryptionCertificateNameAlternative::other() const
{
    BSLS_ASSERT(this->isOther());
    return d_other.object();
}

const bsl::string& EncryptionCertificateNameAlternative::email() const
{
    BSLS_ASSERT(this->isEmail());
    return d_email.object();
}

const bsl::string& EncryptionCertificateNameAlternative::domain() const
{
    BSLS_ASSERT(this->isDomain());
    return d_domain.object();
}

const ntsa::AbstractValue& EncryptionCertificateNameAlternative::x400() const
{
    BSLS_ASSERT(this->isX400());
    return d_x400.object();
}

const EncryptionCertificateName& EncryptionCertificateNameAlternative::
    directory() const
{
    BSLS_ASSERT(this->isDirectory());
    return d_directory.object();
}

const ntsa::AbstractValue& EncryptionCertificateNameAlternative::edi() const
{
    BSLS_ASSERT(this->isEdi());
    return d_edi.object();
}

const ntsa::Uri& EncryptionCertificateNameAlternative::uri() const
{
    BSLS_ASSERT(this->isUri());
    return d_uri.object();
}

const ntsa::IpAddress& EncryptionCertificateNameAlternative::ip() const
{
    BSLS_ASSERT(this->isIp());
    return d_ip.object();
}

const ntsa::AbstractObjectIdentifier& EncryptionCertificateNameAlternative::
    identifier() const
{
    BSLS_ASSERT(this->isIdentifier());
    return d_identifier.object();
}

bool EncryptionCertificateNameAlternative::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionCertificateNameAlternative::isOther() const
{
    return d_type == e_OTHER;
}

bool EncryptionCertificateNameAlternative::isEmail() const
{
    return d_type == e_EMAIL;
}

bool EncryptionCertificateNameAlternative::isDomain() const
{
    return d_type == e_DOMAIN;
}

bool EncryptionCertificateNameAlternative::isX400() const
{
    return d_type == e_X400;
}

bool EncryptionCertificateNameAlternative::isDirectory() const
{
    return d_type == e_DIRECTORY;
}

bool EncryptionCertificateNameAlternative::isEdi() const
{
    return d_type == e_EDI;
}

bool EncryptionCertificateNameAlternative::isUri() const
{
    return d_type == e_URI;
}

bool EncryptionCertificateNameAlternative::isIp() const
{
    return d_type == e_IP;
}

bool EncryptionCertificateNameAlternative::isIdentifier() const
{
    return d_type == e_IDENTIFIER;
}

bool EncryptionCertificateNameAlternative::matchesDomainName(
    const bsl::string& domainName) const
{
    if (this->isDomain()) {
        if (EncryptionCertificateUtil::matches(domainName, this->domain())) {
            return true;
        }
    }
    else if (this->isIp()) {
        if (EncryptionCertificateUtil::matches(domainName, this->ip())) {
            return true;
        }
    }
    else if (this->isUri()) {
        if (EncryptionCertificateUtil::matches(domainName, this->uri())) {
            return true;
        }
    }

    return false;
}

bool EncryptionCertificateNameAlternative::matchesIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    if (this->isDomain()) {
        if (EncryptionCertificateUtil::matches(ipAddress, this->domain())) {
            return true;
        }
    }
    else if (this->isIp()) {
        if (EncryptionCertificateUtil::matches(ipAddress, this->ip())) {
            return true;
        }
    }
    else if (this->isUri()) {
        if (EncryptionCertificateUtil::matches(ipAddress, this->uri())) {
            return true;
        }
    }

    return false;
}

bool EncryptionCertificateNameAlternative::equals(
    const EncryptionCertificateNameAlternative& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_UNDEFINED) {
        return true;
    }
    else if (d_type == e_OTHER) {
        return d_other.object() == other.d_other.object();
    }
    else if (d_type == e_EMAIL) {
        return d_email.object() == other.d_email.object();
    }
    else if (d_type == e_DOMAIN) {
        return d_domain.object() == other.d_domain.object();
    }
    else if (d_type == e_X400) {
        return d_x400.object() == other.d_x400.object();
    }
    else if (d_type == e_DIRECTORY) {
        return d_directory.object() == other.d_directory.object();
    }
    else if (d_type == e_EDI) {
        return d_edi.object() == other.d_edi.object();
    }
    else if (d_type == e_URI) {
        return d_uri.object() == other.d_uri.object();
    }
    else if (d_type == e_IP) {
        return d_ip.object() == other.d_ip.object();
    }
    else if (d_type == e_IDENTIFIER) {
        return d_identifier.object() == other.d_identifier.object();
    }

    return false;
}

bool EncryptionCertificateNameAlternative::less(
    const EncryptionCertificateNameAlternative& other) const
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
    else if (d_type == e_OTHER) {
        return d_other.object() < other.d_other.object();
    }
    else if (d_type == e_EMAIL) {
        return d_email.object() < other.d_email.object();
    }
    else if (d_type == e_DOMAIN) {
        return d_domain.object() < other.d_domain.object();
    }
    else if (d_type == e_X400) {
        return d_x400.object() < other.d_x400.object();
    }
    else if (d_type == e_DIRECTORY) {
        return d_directory.object() < other.d_directory.object();
    }
    else if (d_type == e_EDI) {
        return d_edi.object() < other.d_edi.object();
    }
    else if (d_type == e_URI) {
        return d_uri.object() < other.d_uri.object();
    }
    else if (d_type == e_IP) {
        return d_ip.object() < other.d_ip.object();
    }
    else if (d_type == e_IDENTIFIER) {
        return d_identifier.object() < other.d_identifier.object();
    }

    return false;
}

bsl::ostream& EncryptionCertificateNameAlternative::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    if (d_type == e_OTHER) {
        stream << d_other.object();
    }
    else if (d_type == e_EMAIL) {
        stream << d_email.object();
    }
    else if (d_type == e_DOMAIN) {
        stream << d_domain.object();
    }
    else if (d_type == e_X400) {
        stream << d_x400.object();
    }
    else if (d_type == e_DIRECTORY) {
        stream << d_directory.object();
    }
    else if (d_type == e_EDI) {
        stream << d_edi.object();
    }
    else if (d_type == e_URI) {
        stream << d_uri.object();
    }
    else if (d_type == e_IP) {
        stream << d_ip.object();
    }
    else if (d_type == e_IDENTIFIER) {
        stream << d_identifier.object();
    }
    else {
        stream << "UNDEFINED";
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const EncryptionCertificateNameAlternative& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateNameAlternative& lhs,
                const EncryptionCertificateNameAlternative& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateNameAlternative& lhs,
                const EncryptionCertificateNameAlternative& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateNameAlternative& lhs,
               const EncryptionCertificateNameAlternative& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateNameAlternativeList::
    EncryptionCertificateNameAlternativeList(bslma::Allocator* basicAllocator)
: d_container(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAlternativeList::
    EncryptionCertificateNameAlternativeList(
        const EncryptionCertificateNameAlternativeList& original,
        bslma::Allocator*                               basicAllocator)
: d_container(original.d_container, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAlternativeList::
    ~EncryptionCertificateNameAlternativeList()
{
}

EncryptionCertificateNameAlternativeList&
EncryptionCertificateNameAlternativeList::operator=(
    const EncryptionCertificateNameAlternativeList& other)
{
    if (this != &other) {
        d_container = other.d_container;
    }

    return *this;
}

void EncryptionCertificateNameAlternativeList::reset()
{
    d_container.clear();
}

ntsa::Error EncryptionCertificateNameAlternativeList::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    while (decoder->contentBytesRemaining() > 0) {
        EncryptionCertificateNameAlternative element;
        error = element.decode(decoder);
        if (error) {
            return error;
        }

        d_container.push_back(element);
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateNameAlternativeList::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    if (d_container.empty()) {
        return ntsa::Error();
    }

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateNameAlternative& element = d_container[i];
        error = element.encode(encoder);
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

const bsl::vector<EncryptionCertificateNameAlternative>&
EncryptionCertificateNameAlternativeList::alternatives() const
{
    return d_container;
}

bool EncryptionCertificateNameAlternativeList::matchesDomainName(
    const bsl::string& domainName) const
{
    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateNameAlternative& element = d_container[i];

        if (element.matchesDomainName(domainName)) {
            return true;
        }
    }

    return false;
}

bool EncryptionCertificateNameAlternativeList::matchesIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateNameAlternative& element = d_container[i];

        if (element.matchesIpAddress(ipAddress)) {
            return true;
        }
    }

    return false;
}

bool EncryptionCertificateNameAlternativeList::equals(
    const EncryptionCertificateNameAlternativeList& other) const
{
    return d_container == other.d_container;
}

bool EncryptionCertificateNameAlternativeList::less(
    const EncryptionCertificateNameAlternativeList& other) const
{
    return d_container < other.d_container;
}

bsl::ostream& EncryptionCertificateNameAlternativeList::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("container", d_container);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                   stream,
    const EncryptionCertificateNameAlternativeList& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateNameAlternativeList& lhs,
                const EncryptionCertificateNameAlternativeList& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateNameAlternativeList& lhs,
                const EncryptionCertificateNameAlternativeList& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateNameAlternativeList& lhs,
               const EncryptionCertificateNameAlternativeList& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateNameConstraints::EncryptionCertificateNameConstraints(
    bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameConstraints::EncryptionCertificateNameConstraints(
    const EncryptionCertificateNameConstraints& original,
    bslma::Allocator*                           basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameConstraints::~EncryptionCertificateNameConstraints()
{
}

EncryptionCertificateNameConstraints& EncryptionCertificateNameConstraints::
operator=(const EncryptionCertificateNameConstraints& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateNameConstraints::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificateNameConstraints::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificateNameConstraints::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificateNameConstraints::equals(
    const EncryptionCertificateNameConstraints& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateNameConstraints::less(
    const EncryptionCertificateNameConstraints& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateNameConstraints::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const EncryptionCertificateNameConstraints& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateNameConstraints& lhs,
                const EncryptionCertificateNameConstraints& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateNameConstraints& lhs,
                const EncryptionCertificateNameConstraints& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateNameConstraints& lhs,
               const EncryptionCertificateNameConstraints& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateValidity::EncryptionCertificateValidity(
    bslma::Allocator* basicAllocator)
: d_fromTagNumber(ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME)
, d_from()
, d_thruTagNumber(ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME)
, d_thru()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateValidity::EncryptionCertificateValidity(
    const EncryptionCertificateValidity& original,
    bslma::Allocator*                    basicAllocator)
: d_fromTagNumber(original.d_fromTagNumber)
, d_from(original.d_from)
, d_thruTagNumber(original.d_thruTagNumber)
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
        d_fromTagNumber = other.d_fromTagNumber;
        d_from          = other.d_from;
        d_thruTagNumber = other.d_thruTagNumber;
        d_thru          = other.d_thru;
    }

    return *this;
}

void EncryptionCertificateValidity::reset()
{
    d_fromTagNumber = ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME;
    d_from          = bdlt::DatetimeTz();
    d_thruTagNumber = ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME;
    d_thru          = bdlt::DatetimeTz();
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

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagNumber() !=
                ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME &&
            decoder->current().tagNumber() !=
                ntsa::AbstractSyntaxTagNumber::e_UTC_TIME)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_fromTagNumber = static_cast<ntsa::AbstractSyntaxTagNumber::Value>(
            decoder->current().tagNumber());

        error = decoder->decodeValue(&d_from);
        if (error) {
            return error;
        }

        error = decoder->decodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagNumber() !=
                ntsa::AbstractSyntaxTagNumber::e_GENERALIZED_TIME &&
            decoder->current().tagNumber() !=
                ntsa::AbstractSyntaxTagNumber::e_UTC_TIME)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_thruTagNumber = static_cast<ntsa::AbstractSyntaxTagNumber::Value>(
            decoder->current().tagNumber());

        error = decoder->decodeValue(&d_thru);
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

ntsa::Error EncryptionCertificateValidity::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, d_fromTagNumber);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_from);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, d_thruTagNumber);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_thru);
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

const char* EncryptionCertificateSignatureAlgorithmIdentifierType::toString(
    Value value)
{
    switch (value) {
    case e_MD2_RSA:
        return "MD2_RSA";
    case e_MD4_RSA:
        return "MD4_RSA";
    case e_MD5_RSA:
        return "MD5_RSA";
    case e_SHA1_RSA:
        return "SHA1_RSA";
    case e_SHA256_RSA:
        return "SHA256_RSA";
    case e_SHA384_RSA:
        return "SHA384_RSA";
    case e_SHA512_RSA:
        return "SHA512_RSA";
    case e_ECDSA_SHA1:
        return "ECDSA_SHA1";
    case e_ECDSA_SHA224:
        return "ECDSA_SHA224";
    case e_ECDSA_SHA256:
        return "ECDSA_SHA256";
    case e_ECDSA_SHA384:
        return "ECDSA_SHA384";
    case e_ECDSA_SHA512:
        return "ECDSA_SHA512";
    case e_EDDSA_25519:
        return "EDDSA_25519";
    case e_EDDSA_448:
        return "EDDSA_448";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionCertificateSignatureAlgorithmIdentifierType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result,
    Value                           value)
{
    result->reset();

    if (value == e_MD2_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 2);
    }
    else if (value == e_MD4_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 3);
    }
    else if (value == e_MD5_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 4);
    }
    else if (value == e_SHA1_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 5);
    }
    else if (value == e_SHA256_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 11);
    }
    else if (value == e_SHA384_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 12);
    }
    else if (value == e_SHA512_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 13);
    }
    else if (value == e_ECDSA_SHA1) {
        result->set(1, 2, 840, 10045, 4, 1);
    }
    else if (value == e_ECDSA_SHA224) {
        result->set(1, 2, 840, 10045, 4, 3, 1);
    }
    else if (value == e_ECDSA_SHA256) {
        result->set(1, 2, 840, 10045, 4, 3, 2);
    }
    else if (value == e_ECDSA_SHA384) {
        result->set(1, 2, 840, 10045, 4, 3, 3);
    }
    else if (value == e_ECDSA_SHA512) {
        result->set(1, 2, 840, 10045, 4, 3, 4);
    }
    else if (value == e_EDDSA_25519) {
        result->set(1, 3, 101, 112);
    }
    else if (value == e_EDDSA_448) {
        result->set(1, 3, 101, 113);
    }
}

int EncryptionCertificateSignatureAlgorithmIdentifierType::fromString(
    Value*                   result,
    const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "MD2_RSA")) {
        *result = e_MD2_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "MD4_RSA")) {
        *result = e_MD4_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "MD5_RSA")) {
        *result = e_MD5_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SHA1_RSA")) {
        *result = e_SHA1_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SHA256_RSA")) {
        *result = e_SHA256_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SHA384_RSA")) {
        *result = e_SHA384_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SHA512_RSA")) {
        *result = e_SHA512_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA_SHA1")) {
        *result = e_ECDSA_SHA1;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA_SHA224")) {
        *result = e_ECDSA_SHA224;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA_SHA256")) {
        *result = e_ECDSA_SHA256;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA_SHA384")) {
        *result = e_ECDSA_SHA384;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA_SHA512")) {
        *result = e_ECDSA_SHA512;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "EDDSA_25519")) {
        *result = e_EDDSA_25519;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "EDDSA_448")) {
        *result = e_EDDSA_448;
        return 0;
    }

    return -1;
}

int EncryptionCertificateSignatureAlgorithmIdentifierType::
    fromObjectIdentifier(Value*                                result,
                         const ntsa::AbstractObjectIdentifier& identifier)
{
    if (identifier.equals(1, 2, 840, 113549, 1, 1, 2)) {
        *result = e_MD2_RSA;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 113549, 1, 1, 3)) {
        *result = e_MD4_RSA;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 113549, 1, 1, 4)) {
        *result = e_MD5_RSA;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 113549, 1, 1, 5)) {
        *result = e_SHA1_RSA;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 113549, 1, 1, 11)) {
        *result = e_SHA256_RSA;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 113549, 1, 1, 12)) {
        *result = e_SHA384_RSA;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 113549, 1, 1, 13)) {
        *result = e_SHA512_RSA;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 10045, 4, 1)) {
        *result = e_ECDSA_SHA1;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 10045, 4, 3, 1)) {
        *result = e_ECDSA_SHA224;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 10045, 4, 3, 2)) {
        *result = e_ECDSA_SHA256;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 10045, 4, 3, 3)) {
        *result = e_ECDSA_SHA384;
        return 0;
    }

    if (identifier.equals(1, 2, 840, 10045, 4, 3, 4)) {
        *result = e_ECDSA_SHA512;
        return 0;
    }

    if (identifier.equals(1, 3, 101, 112)) {
        *result = e_EDDSA_25519;
        return 0;
    }

    if (identifier.equals(1, 3, 101, 113)) {
        *result = e_EDDSA_448;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionCertificateSignatureAlgorithmIdentifierType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                                stream,
    EncryptionCertificateSignatureAlgorithmIdentifierType::Value rhs)
{
    return EncryptionCertificateSignatureAlgorithmIdentifierType::print(stream,
                                                                        rhs);
}

EncryptionCertificateSignatureAlgorithmIdentifier::
    EncryptionCertificateSignatureAlgorithmIdentifier(
        bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithmIdentifier::
    EncryptionCertificateSignatureAlgorithmIdentifier(
        const EncryptionCertificateSignatureAlgorithmIdentifier& original,
        bslma::Allocator* basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithmIdentifier::
    ~EncryptionCertificateSignatureAlgorithmIdentifier()
{
}

EncryptionCertificateSignatureAlgorithmIdentifier&
EncryptionCertificateSignatureAlgorithmIdentifier::operator=(
    const EncryptionCertificateSignatureAlgorithmIdentifier& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificateSignatureAlgorithmIdentifier&
EncryptionCertificateSignatureAlgorithmIdentifier::operator=(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateSignatureAlgorithmIdentifier&
EncryptionCertificateSignatureAlgorithmIdentifier::operator=(
    EncryptionCertificateSignatureAlgorithmIdentifierType::Value value)
{
    EncryptionCertificateSignatureAlgorithmIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
    return *this;
}

void EncryptionCertificateSignatureAlgorithmIdentifier::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateSignatureAlgorithmIdentifier::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificateSignatureAlgorithmIdentifier::setIdentifer(
    EncryptionCertificateSignatureAlgorithmIdentifierType::Value value)
{
    EncryptionCertificateSignatureAlgorithmIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
}

ntsa::Error EncryptionCertificateSignatureAlgorithmIdentifier::decode(
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

ntsa::Error EncryptionCertificateSignatureAlgorithmIdentifier::encode(
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
EncryptionCertificateSignatureAlgorithmIdentifier::identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateSignatureAlgorithmIdentifier::equals(
    const EncryptionCertificateSignatureAlgorithmIdentifier& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateSignatureAlgorithmIdentifier::equals(
    EncryptionCertificateSignatureAlgorithmIdentifierType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateSignatureAlgorithmIdentifierType::toObjectIdentifier(
        &identifier,
        value);

    return d_identifier.equals(identifier);
}

bool EncryptionCertificateSignatureAlgorithmIdentifier::less(
    const EncryptionCertificateSignatureAlgorithmIdentifier& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateSignatureAlgorithmIdentifier::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionCertificateSignatureAlgorithmIdentifierType::Value type;
    int rc = ntca::EncryptionCertificateSignatureAlgorithmIdentifierType::
        fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateSignatureAlgorithmIdentifierType::
                toString(type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                            stream,
    const EncryptionCertificateSignatureAlgorithmIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSignatureAlgorithmIdentifier& lhs,
                const EncryptionCertificateSignatureAlgorithmIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSignatureAlgorithmIdentifier& lhs,
                const EncryptionCertificateSignatureAlgorithmIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSignatureAlgorithmIdentifier& lhs,
               const EncryptionCertificateSignatureAlgorithmIdentifier& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateSignatureAlgorithmParameters::
    EncryptionCertificateSignatureAlgorithmParameters(
        bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithmParameters::
    EncryptionCertificateSignatureAlgorithmParameters(
        const EncryptionCertificateSignatureAlgorithmParameters& original,
        bslma::Allocator* basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithmParameters::
    ~EncryptionCertificateSignatureAlgorithmParameters()
{
}

EncryptionCertificateSignatureAlgorithmParameters&
EncryptionCertificateSignatureAlgorithmParameters::operator=(
    const EncryptionCertificateSignatureAlgorithmParameters& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateSignatureAlgorithmParameters::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificateSignatureAlgorithmParameters::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificateSignatureAlgorithmParameters::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificateSignatureAlgorithmParameters::equals(
    const EncryptionCertificateSignatureAlgorithmParameters& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateSignatureAlgorithmParameters::less(
    const EncryptionCertificateSignatureAlgorithmParameters& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateSignatureAlgorithmParameters::print(
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
    bsl::ostream&                                            stream,
    const EncryptionCertificateSignatureAlgorithmParameters& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSignatureAlgorithmParameters& lhs,
                const EncryptionCertificateSignatureAlgorithmParameters& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSignatureAlgorithmParameters& lhs,
                const EncryptionCertificateSignatureAlgorithmParameters& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSignatureAlgorithmParameters& lhs,
               const EncryptionCertificateSignatureAlgorithmParameters& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateSignatureAlgorithm::
    EncryptionCertificateSignatureAlgorithm(bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_parameters(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithm::
    EncryptionCertificateSignatureAlgorithm(
        const EncryptionCertificateSignatureAlgorithm& original,
        bslma::Allocator*                              basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_parameters(original.d_parameters, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithm::
    ~EncryptionCertificateSignatureAlgorithm()
{
}

EncryptionCertificateSignatureAlgorithm& EncryptionCertificateSignatureAlgorithm::
operator=(const EncryptionCertificateSignatureAlgorithm& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
        d_parameters = other.d_parameters;
    }

    return *this;
}

void EncryptionCertificateSignatureAlgorithm::reset()
{
    d_identifier.reset();
    d_parameters.reset();
}

void EncryptionCertificateSignatureAlgorithm::setIdentifier(
    const EncryptionCertificateSignatureAlgorithmIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificateSignatureAlgorithm::setParameters(
    const EncryptionCertificateSignatureAlgorithmParameters& value)
{
    d_parameters = value;
}

ntsa::Error EncryptionCertificateSignatureAlgorithm::decode(
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
        error = d_parameters.makeValue().decode(decoder);
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

ntsa::Error EncryptionCertificateSignatureAlgorithm::encode(
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
        error = d_parameters.value().encode(encoder);
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

const EncryptionCertificateSignatureAlgorithmIdentifier&
EncryptionCertificateSignatureAlgorithm::identifier() const
{
    return d_identifier;
}

const bdlb::NullableValue<EncryptionCertificateSignatureAlgorithmParameters>&
EncryptionCertificateSignatureAlgorithm::parameters() const
{
    return d_parameters;
}

bool EncryptionCertificateSignatureAlgorithm::equals(
    const EncryptionCertificateSignatureAlgorithm& other) const
{
    return d_identifier == other.d_identifier &&
           d_parameters == other.d_parameters;
}

bool EncryptionCertificateSignatureAlgorithm::less(
    const EncryptionCertificateSignatureAlgorithm& other) const
{
    if (d_identifier < other.d_identifier) {
        return true;
    }

    if (other.d_identifier < d_identifier) {
        return false;
    }

    return d_parameters < other.d_parameters;
}

bsl::ostream& EncryptionCertificateSignatureAlgorithm::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("identifier", d_identifier);

    if (!d_parameters.isNull()) {
        printer.printAttribute("parameters", d_parameters);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                                  stream,
                         const EncryptionCertificateSignatureAlgorithm& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSignatureAlgorithm& lhs,
                const EncryptionCertificateSignatureAlgorithm& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSignatureAlgorithm& lhs,
                const EncryptionCertificateSignatureAlgorithm& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSignatureAlgorithm& lhs,
               const EncryptionCertificateSignatureAlgorithm& rhs)
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
    bslma::Allocator*                     basicAllocator)
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

void EncryptionCertificateSignature::setValue(
    const ntsa::AbstractBitString& value)
{
    d_value = value;
}

ntsa::Error EncryptionCertificateSignature::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(ntsa::AbstractSyntaxTagClass::e_UNIVERSAL,
                               ntsa::AbstractSyntaxTagType::e_PRIMITIVE,
                               ntsa::AbstractSyntaxTagNumber::e_BIT_STRING);
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

ntsa::Error EncryptionCertificateSignature::encode(
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

const ntsa::AbstractBitString& EncryptionCertificateSignature::value() const
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
    bsl::ostream::fmtflags previousFlags = stream.flags();
    stream.setf(previousFlags | bsl::ostream::hex);
    d_value.print(stream, level, spacesPerLevel);
    stream.flags(previousFlags);
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                         stream,
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

EncryptionCertificatePolicyConstraints::EncryptionCertificatePolicyConstraints(
    bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePolicyConstraints::EncryptionCertificatePolicyConstraints(
    const EncryptionCertificatePolicyConstraints& original,
    bslma::Allocator*                             basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePolicyConstraints::
    ~EncryptionCertificatePolicyConstraints()
{
}

EncryptionCertificatePolicyConstraints& EncryptionCertificatePolicyConstraints::
operator=(const EncryptionCertificatePolicyConstraints& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificatePolicyConstraints::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificatePolicyConstraints::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificatePolicyConstraints::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificatePolicyConstraints::equals(
    const EncryptionCertificatePolicyConstraints& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificatePolicyConstraints::less(
    const EncryptionCertificatePolicyConstraints& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificatePolicyConstraints::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                                 stream,
                         const EncryptionCertificatePolicyConstraints& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePolicyConstraints& lhs,
                const EncryptionCertificatePolicyConstraints& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePolicyConstraints& lhs,
                const EncryptionCertificatePolicyConstraints& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePolicyConstraints& lhs,
               const EncryptionCertificatePolicyConstraints& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificatePolicyMappings::EncryptionCertificatePolicyMappings(
    bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePolicyMappings::EncryptionCertificatePolicyMappings(
    const EncryptionCertificatePolicyMappings& original,
    bslma::Allocator*                          basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePolicyMappings::~EncryptionCertificatePolicyMappings()
{
}

EncryptionCertificatePolicyMappings& EncryptionCertificatePolicyMappings::
operator=(const EncryptionCertificatePolicyMappings& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificatePolicyMappings::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificatePolicyMappings::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificatePolicyMappings::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificatePolicyMappings::equals(
    const EncryptionCertificatePolicyMappings& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificatePolicyMappings::less(
    const EncryptionCertificatePolicyMappings& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificatePolicyMappings::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                              stream,
                         const EncryptionCertificatePolicyMappings& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePolicyMappings& lhs,
                const EncryptionCertificatePolicyMappings& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePolicyMappings& lhs,
                const EncryptionCertificatePolicyMappings& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePolicyMappings& lhs,
               const EncryptionCertificatePolicyMappings& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificatePolicy::EncryptionCertificatePolicy(
    bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePolicy::EncryptionCertificatePolicy(
    const EncryptionCertificatePolicy& original,
    bslma::Allocator*                  basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePolicy::~EncryptionCertificatePolicy()
{
}

EncryptionCertificatePolicy& EncryptionCertificatePolicy::operator=(
    const EncryptionCertificatePolicy& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificatePolicy::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificatePolicy::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificatePolicy::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificatePolicy::equals(
    const EncryptionCertificatePolicy& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificatePolicy::less(
    const EncryptionCertificatePolicy& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificatePolicy::print(bsl::ostream& stream,
                                                 int           level,
                                                 int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionCertificatePolicy& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePolicy& lhs,
                const EncryptionCertificatePolicy& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePolicy& lhs,
                const EncryptionCertificatePolicy& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePolicy& lhs,
               const EncryptionCertificatePolicy& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateIssuerKeyIdentifier::
    EncryptionCertificateIssuerKeyIdentifier(bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateIssuerKeyIdentifier::
    EncryptionCertificateIssuerKeyIdentifier(
        const EncryptionCertificateIssuerKeyIdentifier& original,
        bslma::Allocator*                               basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateIssuerKeyIdentifier::
    ~EncryptionCertificateIssuerKeyIdentifier()
{
}

EncryptionCertificateIssuerKeyIdentifier&
EncryptionCertificateIssuerKeyIdentifier::operator=(
    const EncryptionCertificateIssuerKeyIdentifier& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateIssuerKeyIdentifier::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificateIssuerKeyIdentifier::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificateIssuerKeyIdentifier::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificateIssuerKeyIdentifier::equals(
    const EncryptionCertificateIssuerKeyIdentifier& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateIssuerKeyIdentifier::less(
    const EncryptionCertificateIssuerKeyIdentifier& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateIssuerKeyIdentifier::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                   stream,
    const EncryptionCertificateIssuerKeyIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateIssuerKeyIdentifier& lhs,
                const EncryptionCertificateIssuerKeyIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateIssuerKeyIdentifier& lhs,
                const EncryptionCertificateIssuerKeyIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateIssuerKeyIdentifier& lhs,
               const EncryptionCertificateIssuerKeyIdentifier& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateIssuerInformationAccess::
    EncryptionCertificateIssuerInformationAccess(
        bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateIssuerInformationAccess::
    EncryptionCertificateIssuerInformationAccess(
        const EncryptionCertificateIssuerInformationAccess& original,
        bslma::Allocator*                                   basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateIssuerInformationAccess::
    ~EncryptionCertificateIssuerInformationAccess()
{
}

EncryptionCertificateIssuerInformationAccess&
EncryptionCertificateIssuerInformationAccess::operator=(
    const EncryptionCertificateIssuerInformationAccess& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateIssuerInformationAccess::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificateIssuerInformationAccess::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificateIssuerInformationAccess::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificateIssuerInformationAccess::equals(
    const EncryptionCertificateIssuerInformationAccess& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateIssuerInformationAccess::less(
    const EncryptionCertificateIssuerInformationAccess& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateIssuerInformationAccess::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                       stream,
    const EncryptionCertificateIssuerInformationAccess& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateIssuerInformationAccess& lhs,
                const EncryptionCertificateIssuerInformationAccess& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateIssuerInformationAccess& lhs,
                const EncryptionCertificateIssuerInformationAccess& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateIssuerInformationAccess& lhs,
               const EncryptionCertificateIssuerInformationAccess& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateIssuer::EncryptionCertificateIssuer(
    bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateIssuer::EncryptionCertificateIssuer(
    const EncryptionCertificateIssuer& original,
    bslma::Allocator*                  basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateIssuer::~EncryptionCertificateIssuer()
{
}

EncryptionCertificateIssuer& EncryptionCertificateIssuer::operator=(
    const EncryptionCertificateIssuer& other)
{
    if (this != &other) {
        d_name = other.d_name;
    }

    return *this;
}

void EncryptionCertificateIssuer::reset()
{
    d_name.reset();
}

ntsa::Error EncryptionCertificateIssuer::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    return d_name.decode(decoder);
}

ntsa::Error EncryptionCertificateIssuer::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    return d_name.encode(encoder);
}

bool EncryptionCertificateIssuer::matchesDomainName(
    const bsl::string& domainName) const
{
    return d_name.matchesDomainName(domainName);
}

bool EncryptionCertificateIssuer::matchesIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    return d_name.matchesIpAddress(ipAddress);
}

bool EncryptionCertificateIssuer::equals(
    const EncryptionCertificateIssuer& other) const
{
    return d_name == other.d_name;
}

bool EncryptionCertificateIssuer::less(
    const EncryptionCertificateIssuer& other) const
{
    return d_name < other.d_name;
}

bsl::ostream& EncryptionCertificateIssuer::print(bsl::ostream& stream,
                                                 int           level,
                                                 int spacesPerLevel) const
{
    d_name.print(stream, level, spacesPerLevel);
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const EncryptionCertificateIssuer& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateIssuer& lhs,
                const EncryptionCertificateIssuer& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateIssuer& lhs,
                const EncryptionCertificateIssuer& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateIssuer& lhs,
               const EncryptionCertificateIssuer& rhs)
{
    return lhs.less(rhs);
}

const char* EncryptionCertificateSubjectKeyUsageExtendedType::toString(
    Value value)
{
    switch (value) {
    case e_TLS_CLIENT:
        return "TLS_CLIENT";
    case e_TLS_SERVER:
        return "TLS_SERVER";
    case e_SSH_CLIENT:
        return "TLS_CLIENT";
    case e_SSH_SERVER:
        return "TLS_SERVER";
    case e_IPSEC_USER:
        return "IPSEC_USER";
    case e_IPSEC_TUNNEL:
        return "IPSEC_TUNNEL";
    case e_IPSEC_ENDPOINT:
        return "IPSEC_ENDPOINT";
    case e_KEY_TIMESTAMPING:
        return "KEY_TIMESTAMPING";
    case e_CODE_SIGNING:
        return "CODE_SIGNING";
    case e_EMAIL:
        return "EMAIL";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionCertificateSubjectKeyUsageExtendedType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result,
    Value                           value)
{
    result->reset();

    if (value == e_TLS_CLIENT) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 2);
    }
    else if (value == e_TLS_SERVER) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 1);
    }
    else if (value == e_SSH_CLIENT) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 21);
    }
    else if (value == e_SSH_SERVER) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 22);
    }
    else if (value == e_IPSEC_USER) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 7);
    }
    else if (value == e_IPSEC_TUNNEL) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 6);
    }
    else if (value == e_IPSEC_ENDPOINT) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 5);
    }
    else if (value == e_KEY_TIMESTAMPING) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 8);
    }
    else if (value == e_CODE_SIGNING) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 3);
    }
    else if (value == e_EMAIL) {
        result->set(1, 3, 6, 1, 5, 5, 7, 3, 4);
    }
}

int EncryptionCertificateSubjectKeyUsageExtendedType::fromString(
    EncryptionCertificateSubjectKeyUsageExtendedType::Value* result,
    const bslstl::StringRef&                                 string)
{
    if (bdlb::String::areEqualCaseless(string, "TLS_CLIENT")) {
        *result = e_TLS_CLIENT;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "TLS_SERVER")) {
        *result = e_TLS_SERVER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SSH_CLIENT")) {
        *result = e_SSH_CLIENT;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SSH_SERVER")) {
        *result = e_SSH_SERVER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "IPSEC_USER")) {
        *result = e_IPSEC_USER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "IPSEC_TUNNEL")) {
        *result = e_IPSEC_TUNNEL;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "IPSEC_ENDPOINT")) {
        *result = e_IPSEC_ENDPOINT;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "KEY_TIMESTAMPING")) {
        *result = e_KEY_TIMESTAMPING;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "CODE_SIGNING")) {
        *result = e_CODE_SIGNING;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "EMAIL")) {
        *result = e_EMAIL;
        return 0;
    }

    return -1;
}

int EncryptionCertificateSubjectKeyUsageExtendedType::fromObjectIdentifier(
    Value*                                result,
    const ntsa::AbstractObjectIdentifier& identifier)
{
    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 2)) {
        *result = e_TLS_CLIENT;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 1)) {
        *result = e_TLS_SERVER;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 21)) {
        *result = e_SSH_CLIENT;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 22)) {
        *result = e_SSH_SERVER;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 7)) {
        *result = e_IPSEC_USER;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 6)) {
        *result = e_IPSEC_TUNNEL;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 5)) {
        *result = e_IPSEC_ENDPOINT;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 8)) {
        *result = e_KEY_TIMESTAMPING;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 3)) {
        *result = e_CODE_SIGNING;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 3, 4)) {
        *result = e_EMAIL;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionCertificateSubjectKeyUsageExtendedType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                           stream,
    EncryptionCertificateSubjectKeyUsageExtendedType::Value rhs)
{
    return EncryptionCertificateSubjectKeyUsageExtendedType::print(stream,
                                                                   rhs);
}

EncryptionCertificateSubjectKeyUsageExtended::
    EncryptionCertificateSubjectKeyUsageExtended(
        bslma::Allocator* basicAllocator)
: d_identifiers(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubjectKeyUsageExtended::
    EncryptionCertificateSubjectKeyUsageExtended(
        const EncryptionCertificateSubjectKeyUsageExtended& original,
        bslma::Allocator*                                   basicAllocator)
: d_identifiers(original.d_identifiers, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubjectKeyUsageExtended::
    ~EncryptionCertificateSubjectKeyUsageExtended()
{
}

EncryptionCertificateSubjectKeyUsageExtended&
EncryptionCertificateSubjectKeyUsageExtended::operator=(
    const EncryptionCertificateSubjectKeyUsageExtended& other)
{
    if (this != &other) {
        d_identifiers = other.d_identifiers;
    }

    return *this;
}

void EncryptionCertificateSubjectKeyUsageExtended::reset()
{
    d_identifiers.clear();
}

void EncryptionCertificateSubjectKeyUsageExtended::setIdentifierList(
    const bsl::vector<ntsa::AbstractObjectIdentifier>& value)
{
    d_identifiers = value;
}

void EncryptionCertificateSubjectKeyUsageExtended::addIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifiers.push_back(value);
}

void EncryptionCertificateSubjectKeyUsageExtended::addIdentifier(
    EncryptionCertificateSubjectKeyUsageExtendedType::Value value)
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateSubjectKeyUsageExtendedType::toObjectIdentifier(
        &identifier,
        value);

    d_identifiers.push_back(identifier);
}

ntsa::Error EncryptionCertificateSubjectKeyUsageExtended::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    while (decoder->contentBytesRemaining() > 0) {
        error =
            decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OBJECT_IDENTIFIER);
        if (error) {
            return error;
        }

        ntsa::AbstractObjectIdentifier identifier;
        error = decoder->decodeValue(&identifier);
        if (error) {
            return error;
        }

        d_identifiers.push_back(identifier);

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

ntsa::Error EncryptionCertificateSubjectKeyUsageExtended::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < d_identifiers.size(); ++i) {
        error =
            encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OBJECT_IDENTIFIER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_identifiers[i]);
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

const bsl::vector<ntsa::AbstractObjectIdentifier>&
EncryptionCertificateSubjectKeyUsageExtended::identifierList() const
{
    return d_identifiers;
}

bool EncryptionCertificateSubjectKeyUsageExtended::allows(
    const ntsa::AbstractObjectIdentifier& identifier) const
{
    bool result = false;

    for (bsl::size_t i = 0; i < d_identifiers.size(); ++i) {
        if (d_identifiers[i] == identifier) {
            result = true;
            break;
        }
    }

    return result;
}

bool EncryptionCertificateSubjectKeyUsageExtended::allows(
    EncryptionCertificateSubjectKeyUsageExtendedType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateSubjectKeyUsageExtendedType::toObjectIdentifier(
        &identifier,
        value);

    return this->allows(identifier);
}

bool EncryptionCertificateSubjectKeyUsageExtended::equals(
    const EncryptionCertificateSubjectKeyUsageExtended& other) const
{
    return d_identifiers == other.d_identifiers;
}

bool EncryptionCertificateSubjectKeyUsageExtended::less(
    const EncryptionCertificateSubjectKeyUsageExtended& other) const
{
    return d_identifiers < other.d_identifiers;
}

bsl::ostream& EncryptionCertificateSubjectKeyUsageExtended::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    for (bsl::size_t i = 0; i < d_identifiers.size(); ++i) {
        stream << ' ';

        EncryptionCertificateSubjectKeyUsageExtendedType::Value value;
        int rc = EncryptionCertificateSubjectKeyUsageExtendedType::
            fromObjectIdentifier(&value, d_identifiers[i]);
        if (rc == 0) {
            stream
                << EncryptionCertificateSubjectKeyUsageExtendedType::toString(
                       value);
        }
        else {
            stream << d_identifiers[i];
        }
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                       stream,
    const EncryptionCertificateSubjectKeyUsageExtended& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSubjectKeyUsageExtended& lhs,
                const EncryptionCertificateSubjectKeyUsageExtended& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSubjectKeyUsageExtended& lhs,
                const EncryptionCertificateSubjectKeyUsageExtended& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSubjectKeyUsageExtended& lhs,
               const EncryptionCertificateSubjectKeyUsageExtended& rhs)
{
    return lhs.less(rhs);
}

const char* EncryptionCertificateSubjectKeyUsageType::toString(Value value)
{
    switch (value) {
    case e_DIGITAL_SIGNATURE:
        return "DIGITAL_SIGNATURE";
    case e_CONTENT_COMMITMENT:
        return "CONTENT_COMMITMENT";
    case e_KEY_ENCIPHER:
        return "KEY_ENCIPHER";
    case e_DATA_ENCIPHER:
        return "DATA_ENCIPHER";
    case e_KEY_AGREEMENT:
        return "KEY_AGREEMENT";
    case e_KEY_CERTFICATE_SIGNATURE:
        return "KEY_CERTFICATE_SIGNATURE";
    case e_CERTIFICATE_REVOCATION_LIST_SIGNATURE:
        return "CERTIFICATE_REVOCATION_LIST_SIGNATURE";
    case e_ENCIPHER_ONLY:
        return "ENCIPHER_ONLY";
    case e_DECIPHER_ONLY:
        return "DECIPHER_ONLY";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

int EncryptionCertificateSubjectKeyUsageType::fromString(
    EncryptionCertificateSubjectKeyUsageType::Value* result,
    const bslstl::StringRef&                         string)
{
    if (bdlb::String::areEqualCaseless(string, "DIGITAL_SIGNATURE")) {
        *result = e_DIGITAL_SIGNATURE;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "CONTENT_COMMITMENT")) {
        *result = e_CONTENT_COMMITMENT;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "KEY_ENCIPHER")) {
        *result = e_KEY_ENCIPHER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "DATA_ENCIPHER")) {
        *result = e_DATA_ENCIPHER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "KEY_AGREEMENT")) {
        *result = e_KEY_AGREEMENT;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "KEY_CERTFICATE_SIGNATURE")) {
        *result = e_KEY_CERTFICATE_SIGNATURE;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(
            string,
            "CERTIFICATE_REVOCATION_LIST_SIGNATURE"))
    {
        *result = e_CERTIFICATE_REVOCATION_LIST_SIGNATURE;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ENCIPHER_ONLY")) {
        *result = e_ENCIPHER_ONLY;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "DECIPHER_ONLY")) {
        *result = e_DECIPHER_ONLY;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionCertificateSubjectKeyUsageType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream,
                         EncryptionCertificateSubjectKeyUsageType::Value rhs)
{
    return EncryptionCertificateSubjectKeyUsageType::print(stream, rhs);
}

EncryptionCertificateSubjectKeyUsage::EncryptionCertificateSubjectKeyUsage(
    bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubjectKeyUsage::EncryptionCertificateSubjectKeyUsage(
    const EncryptionCertificateSubjectKeyUsage& original,
    bslma::Allocator*                           basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubjectKeyUsage::~EncryptionCertificateSubjectKeyUsage()
{
}

EncryptionCertificateSubjectKeyUsage& EncryptionCertificateSubjectKeyUsage::
operator=(const EncryptionCertificateSubjectKeyUsage& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateSubjectKeyUsage::reset()
{
    d_value.reset();
}

void EncryptionCertificateSubjectKeyUsage::enable(
    EncryptionCertificateSubjectKeyUsageType::Value value)
{
    d_value.setBit(static_cast<bsl::size_t>(value), true);
}

void EncryptionCertificateSubjectKeyUsage::disable(
    EncryptionCertificateSubjectKeyUsageType::Value value)
{
    d_value.setBit(static_cast<bsl::size_t>(value), false);
}

ntsa::Error EncryptionCertificateSubjectKeyUsage::decode(
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

ntsa::Error EncryptionCertificateSubjectKeyUsage::encode(
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

bool EncryptionCertificateSubjectKeyUsage::allows(
    EncryptionCertificateSubjectKeyUsageType::Value value) const
{
    return d_value.getBit(static_cast<bsl::size_t>(value));
}

bool EncryptionCertificateSubjectKeyUsage::equals(
    const EncryptionCertificateSubjectKeyUsage& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateSubjectKeyUsage::less(
    const EncryptionCertificateSubjectKeyUsage& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateSubjectKeyUsage::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    for (bsl::size_t enumerator = 0; enumerator <= 8; ++enumerator) {
        if (d_value.getBit(enumerator)) {
            stream << ' ';
            stream << static_cast<
                EncryptionCertificateSubjectKeyUsageType::Value>(enumerator);
        }
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const EncryptionCertificateSubjectKeyUsage& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSubjectKeyUsage& lhs,
                const EncryptionCertificateSubjectKeyUsage& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSubjectKeyUsage& lhs,
                const EncryptionCertificateSubjectKeyUsage& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSubjectKeyUsage& lhs,
               const EncryptionCertificateSubjectKeyUsage& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateSubjectKeyIdentifier::
    EncryptionCertificateSubjectKeyIdentifier(bslma::Allocator* basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubjectKeyIdentifier::
    EncryptionCertificateSubjectKeyIdentifier(
        const EncryptionCertificateSubjectKeyIdentifier& original,
        bslma::Allocator*                                basicAllocator)
: d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubjectKeyIdentifier::
    ~EncryptionCertificateSubjectKeyIdentifier()
{
}

EncryptionCertificateSubjectKeyIdentifier&
EncryptionCertificateSubjectKeyIdentifier::operator=(
    const EncryptionCertificateSubjectKeyIdentifier& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateSubjectKeyIdentifier::reset()
{
    d_value.reset();
}

ntsa::Error EncryptionCertificateSubjectKeyIdentifier::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag();
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

ntsa::Error EncryptionCertificateSubjectKeyIdentifier::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(d_value.tagClass(),
                               d_value.tagType(),
                               d_value.tagNumber());
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

bool EncryptionCertificateSubjectKeyIdentifier::equals(
    const EncryptionCertificateSubjectKeyIdentifier& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateSubjectKeyIdentifier::less(
    const EncryptionCertificateSubjectKeyIdentifier& other) const
{
    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateSubjectKeyIdentifier::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                    stream,
    const EncryptionCertificateSubjectKeyIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSubjectKeyIdentifier& lhs,
                const EncryptionCertificateSubjectKeyIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSubjectKeyIdentifier& lhs,
                const EncryptionCertificateSubjectKeyIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSubjectKeyIdentifier& lhs,
               const EncryptionCertificateSubjectKeyIdentifier& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateSubjectConstraints::
    EncryptionCertificateSubjectConstraints(bslma::Allocator* basicAllocator)
: d_authority()
, d_pathLength()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubjectConstraints::
    EncryptionCertificateSubjectConstraints(
        const EncryptionCertificateSubjectConstraints& original,
        bslma::Allocator*                              basicAllocator)
: d_authority(original.d_authority)
, d_pathLength(original.d_pathLength)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubjectConstraints::
    ~EncryptionCertificateSubjectConstraints()
{
}

EncryptionCertificateSubjectConstraints& EncryptionCertificateSubjectConstraints::
operator=(const EncryptionCertificateSubjectConstraints& other)
{
    if (this != &other) {
        d_authority  = other.d_authority;
        d_pathLength = other.d_pathLength;
    }

    return *this;
}

void EncryptionCertificateSubjectConstraints::reset()
{
    d_authority.reset();
    d_pathLength.reset();
}

void EncryptionCertificateSubjectConstraints::setAuthority(bool value)
{
    d_authority = value;
}

void EncryptionCertificateSubjectConstraints::setPathLength(bsl::size_t value)
{
    d_pathLength = value;
}

ntsa::Error EncryptionCertificateSubjectConstraints::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    bool wantAuthority  = true;
    bool wantPathLength = true;

    while (decoder->contentBytesRemaining() > 0) {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagClass() == k_UNIVERSAL &&
            decoder->current().tagType() == k_PRIMITIVE &&
            decoder->current().tagNumber() == k_BOOLEAN && wantAuthority)
        {
            error = decoder->decodeValue(&d_authority.makeValue());
            if (error) {
                return error;
            }

            wantAuthority = false;
        }
        else if (decoder->current().tagClass() == k_UNIVERSAL &&
                 decoder->current().tagType() == k_PRIMITIVE &&
                 decoder->current().tagNumber() == k_INTEGER && wantPathLength)
        {
            error = decoder->decodeValue(&d_pathLength.makeValue());
            if (error) {
                return error;
            }

            wantPathLength = false;
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

ntsa::Error EncryptionCertificateSubjectConstraints::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    bdlb::NullableValue<bool>        authority  = d_authority;
    bdlb::NullableValue<bsl::size_t> pathLength = d_pathLength;

    if (!pathLength.isNull() && authority.isNull()) {
        authority.makeValue(false);
    }

    if (!authority.isNull()) {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BOOLEAN);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(authority.value());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    if (!pathLength.isNull()) {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(pathLength.value());
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

const bdlb::NullableValue<bool>& EncryptionCertificateSubjectConstraints::
    authority() const
{
    return d_authority;
}

const bdlb::NullableValue<bsl::size_t>& EncryptionCertificateSubjectConstraints::
    pathLength() const
{
    return d_pathLength;
}

bool EncryptionCertificateSubjectConstraints::equals(
    const EncryptionCertificateSubjectConstraints& other) const
{
    return d_authority == other.d_authority &&
           d_pathLength == other.d_pathLength;
}

bool EncryptionCertificateSubjectConstraints::less(
    const EncryptionCertificateSubjectConstraints& other) const
{
    if (d_authority < other.d_authority) {
        return true;
    }

    if (other.d_authority < d_authority) {
        return false;
    }

    return d_pathLength < other.d_pathLength;
}

bsl::ostream& EncryptionCertificateSubjectConstraints::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_authority.isNull()) {
        printer.printAttribute("authority", d_authority.value());
    }

    if (!d_pathLength.isNull()) {
        printer.printAttribute("pathLength", d_pathLength.value());
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                                  stream,
                         const EncryptionCertificateSubjectConstraints& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSubjectConstraints& lhs,
                const EncryptionCertificateSubjectConstraints& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSubjectConstraints& lhs,
                const EncryptionCertificateSubjectConstraints& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSubjectConstraints& lhs,
               const EncryptionCertificateSubjectConstraints& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateSubject::EncryptionCertificateSubject(
    bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubject::EncryptionCertificateSubject(
    const EncryptionCertificateSubject& original,
    bslma::Allocator*                   basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSubject::~EncryptionCertificateSubject()
{
}

EncryptionCertificateSubject& EncryptionCertificateSubject::operator=(
    const EncryptionCertificateSubject& other)
{
    if (this != &other) {
        d_name = other.d_name;
    }

    return *this;
}

void EncryptionCertificateSubject::reset()
{
    d_name.reset();
}

void EncryptionCertificateSubject::setName(
    const ntca::EncryptionCertificateName& value)
{
    d_name = value;
}

ntsa::Error EncryptionCertificateSubject::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    return d_name.decode(decoder);
}

ntsa::Error EncryptionCertificateSubject::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    return d_name.encode(encoder);
}

const ntca::EncryptionCertificateName& EncryptionCertificateSubject::name()
    const
{
    return d_name;
}

bool EncryptionCertificateSubject::matchesDomainName(
    const bsl::string& domainName) const
{
    return d_name.matchesDomainName(domainName);
}

bool EncryptionCertificateSubject::matchesIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    return d_name.matchesIpAddress(ipAddress);
}

bool EncryptionCertificateSubject::equals(
    const EncryptionCertificateSubject& other) const
{
    return d_name == other.d_name;
}

bool EncryptionCertificateSubject::less(
    const EncryptionCertificateSubject& other) const
{
    return d_name < other.d_name;
}

bsl::ostream& EncryptionCertificateSubject::print(bsl::ostream& stream,
                                                  int           level,
                                                  int spacesPerLevel) const
{
    return d_name.print(stream, level, spacesPerLevel);
}

bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const EncryptionCertificateSubject& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateSubject& lhs,
                const EncryptionCertificateSubject& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateSubject& lhs,
                const EncryptionCertificateSubject& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateSubject& lhs,
               const EncryptionCertificateSubject& rhs)
{
    return lhs.less(rhs);
}

const char* EncryptionCertificateExtensionIdentifierType::toString(Value value)
{
    switch (value) {
    case e_SUBJECT_ALTERNATIVE_NAME:
        return "SUBJECT_ALTERNATIVE_NAME";
    case e_SUBJECT_KEY_IDENTIFIER:
        return "SUBJECT_KEY_IDENTIFIER";
    case e_SUBJECT_KEY_USAGE:
        return "SUBJECT_KEY_USAGE";
    case e_SUBJECT_KEY_USAGE_EXTENDED:
        return "SUBJECT_KEY_USAGE_EXTENDED";
    case e_SUBJECT_CONSTRAINTS:
        return "SUBJECT_CONSTRAINTS";
    case e_ISSUER_ALTERNATIVE_NAME:
        return "ISSUER_ALTERNATIVE_NAME";
    case e_ISSUER_KEY_IDENTIFIER:
        return "ISSUER_KEY_IDENTIFIER";
    case e_ISSUER_INFORMATION_ACCESS:
        return "ISSUER_INFORMATION_ACCESS";
    case e_POLICY:
        return "POLICY";
    case e_POLICY_MAPPINGS:
        return "POLICY_MAPPINGS";
    case e_POLICY_CONSTRAINTS:
        return "POLICY_CONSTRAINTS";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionCertificateExtensionIdentifierType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result,
    Value                           value)
{
    result->reset();

    if (value == e_SUBJECT_ALTERNATIVE_NAME) {
        result->set(2, 5, 29, 17);
    }
    else if (value == e_SUBJECT_KEY_IDENTIFIER) {
        result->set(2, 5, 29, 14);
    }
    else if (value == e_SUBJECT_KEY_USAGE) {
        result->set(2, 5, 29, 15);
    }
    else if (value == e_SUBJECT_KEY_USAGE_EXTENDED) {
        result->set(2, 5, 29, 37);
    }
    // else if (value == e_SUBJECT_INFORMATION_ACCESS) {
    //    result->set(1, 3, 6, 1, 5, 5, 7, 1, 11);
    // }
    else if (value == e_SUBJECT_CONSTRAINTS) {
        result->set(2, 5, 29, 19);
    }

    else if (value == e_ISSUER_ALTERNATIVE_NAME) {
        result->set(2, 5, 29, 18);
    }
    else if (value == e_ISSUER_KEY_IDENTIFIER) {
        result->set(2, 5, 29, 35);
    }
    else if (value == e_ISSUER_INFORMATION_ACCESS) {
        result->set(1, 3, 6, 1, 5, 5, 7, 1, 1);
    }
    else if (value == e_POLICY) {
        result->set(2, 5, 29, 32);
    }
    else if (value == e_POLICY_MAPPINGS) {
        result->set(2, 5, 29, 33);
    }
    else if (value == e_POLICY_CONSTRAINTS) {
        result->set(2, 5, 29, 36);
    }
}

int EncryptionCertificateExtensionIdentifierType::fromString(
    EncryptionCertificateExtensionIdentifierType::Value* result,
    const bslstl::StringRef&                             string)
{
    if (bdlb::String::areEqualCaseless(string, "SUBJECT_ALTERNATIVE_NAME")) {
        *result = e_SUBJECT_ALTERNATIVE_NAME;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SUBJECT_KEY_IDENTIFIER")) {
        *result = e_SUBJECT_KEY_IDENTIFIER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SUBJECT_KEY_USAGE")) {
        *result = e_SUBJECT_KEY_USAGE;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SUBJECT_KEY_USAGE_EXTENDED")) {
        *result = e_SUBJECT_KEY_USAGE_EXTENDED;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SUBJECT_CONSTRAINTS")) {
        *result = e_SUBJECT_CONSTRAINTS;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ISSUER_ALTERNATIVE_NAME")) {
        *result = e_ISSUER_ALTERNATIVE_NAME;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ISSUER_KEY_IDENTIFIER")) {
        *result = e_ISSUER_KEY_IDENTIFIER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ISSUER_INFORMATION_ACCESS")) {
        *result = e_ISSUER_INFORMATION_ACCESS;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "POLICY")) {
        *result = e_POLICY;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "POLICY_MAPPINGS")) {
        *result = e_POLICY_MAPPINGS;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "POLICY_CONSTRAINTS")) {
        *result = e_POLICY_CONSTRAINTS;
        return 0;
    }

    return -1;
}

int EncryptionCertificateExtensionIdentifierType::fromObjectIdentifier(
    Value*                                result,
    const ntsa::AbstractObjectIdentifier& identifier)
{
    if (identifier.equals(2, 5, 29, 17)) {
        *result = e_SUBJECT_ALTERNATIVE_NAME;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 14)) {
        *result = e_SUBJECT_KEY_IDENTIFIER;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 15)) {
        *result = e_SUBJECT_KEY_USAGE;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 37)) {
        *result = e_SUBJECT_KEY_USAGE_EXTENDED;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 19)) {
        *result = e_SUBJECT_CONSTRAINTS;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 18)) {
        *result = e_ISSUER_ALTERNATIVE_NAME;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 35)) {
        *result = e_ISSUER_KEY_IDENTIFIER;
        return 0;
    }

    if (identifier.equals(1, 3, 6, 1, 5, 5, 7, 1, 1)) {
        *result = e_ISSUER_INFORMATION_ACCESS;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 32)) {
        *result = e_POLICY;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 33)) {
        *result = e_POLICY_MAPPINGS;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 36)) {
        *result = e_POLICY_CONSTRAINTS;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionCertificateExtensionIdentifierType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                       stream,
    EncryptionCertificateExtensionIdentifierType::Value rhs)
{
    return EncryptionCertificateExtensionIdentifierType::print(stream, rhs);
}

EncryptionCertificateExtensionIdentifier::
    EncryptionCertificateExtensionIdentifier(bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionIdentifier::
    EncryptionCertificateExtensionIdentifier(
        const EncryptionCertificateExtensionIdentifier& original,
        bslma::Allocator*                               basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionIdentifier::
    ~EncryptionCertificateExtensionIdentifier()
{
}

EncryptionCertificateExtensionIdentifier&
EncryptionCertificateExtensionIdentifier::operator=(
    const EncryptionCertificateExtensionIdentifier& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificateExtensionIdentifier&
EncryptionCertificateExtensionIdentifier::operator=(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateExtensionIdentifier&
EncryptionCertificateExtensionIdentifier::operator=(
    EncryptionCertificateExtensionIdentifierType::Value value)
{
    EncryptionCertificateExtensionIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
    return *this;
}

void EncryptionCertificateExtensionIdentifier::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateExtensionIdentifier::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificateExtensionIdentifier::setIdentifer(
    EncryptionCertificateExtensionIdentifierType::Value value)
{
    EncryptionCertificateExtensionIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
}

ntsa::Error EncryptionCertificateExtensionIdentifier::decode(
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

ntsa::Error EncryptionCertificateExtensionIdentifier::encode(
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

const ntsa::AbstractObjectIdentifier& EncryptionCertificateExtensionIdentifier::
    identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateExtensionIdentifier::equals(
    const EncryptionCertificateExtensionIdentifier& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateExtensionIdentifier::equals(
    EncryptionCertificateExtensionIdentifierType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateExtensionIdentifierType::toObjectIdentifier(
        &identifier,
        value);

    return d_identifier.equals(identifier);
}

bool EncryptionCertificateExtensionIdentifier::less(
    const EncryptionCertificateExtensionIdentifier& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateExtensionIdentifier::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionCertificateExtensionIdentifierType::Value type;
    int rc = ntca::EncryptionCertificateExtensionIdentifierType::
        fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateExtensionIdentifierType::toString(
            type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(
    bsl::ostream&                                   stream,
    const EncryptionCertificateExtensionIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateExtensionIdentifier& lhs,
                const EncryptionCertificateExtensionIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateExtensionIdentifier& lhs,
                const EncryptionCertificateExtensionIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateExtensionIdentifier& lhs,
               const EncryptionCertificateExtensionIdentifier& rhs)
{
    return lhs.less(rhs);
}

EncryptionCertificateExtensionValue::EncryptionCertificateExtensionValue(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionValue::EncryptionCertificateExtensionValue(
    const EncryptionCertificateExtensionValue& original,
    bslma::Allocator*                          basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_BOOLEAN) {
        new (d_boolean.buffer()) bool(original.d_boolean.object());
    }
    else if (d_type == e_NAME) {
        new (d_name.buffer())
            EncryptionCertificateName(original.d_name.object(), d_allocator_p);
    }
    else if (d_type == e_NAME_ALTERNATIVE) {
        new (d_nameAlternative.buffer())
            EncryptionCertificateNameAlternativeList(
                original.d_nameAlternative.object(),
                d_allocator_p);
    }
    else if (d_type == e_SUBJECT) {
        new (d_subject.buffer())
            EncryptionCertificateSubject(original.d_subject.object(),
                                         d_allocator_p);
    }
    else if (d_type == e_SUBJECT_KEY_IDENTIFIER) {
        new (d_subjectKeyIdentifier.buffer())
            EncryptionCertificateSubjectKeyIdentifier(
                original.d_subjectKeyIdentifier.object(),
                d_allocator_p);
    }
    else if (d_type == e_SUBJECT_KEY_USAGE) {
        new (d_subjectKeyUsage.buffer()) EncryptionCertificateSubjectKeyUsage(
            original.d_subjectKeyUsage.object(),
            d_allocator_p);
    }
    else if (d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        new (d_subjectKeyUsageExtended.buffer())
            EncryptionCertificateSubjectKeyUsageExtended(
                original.d_subjectKeyUsageExtended.object(),
                d_allocator_p);
    }
    else if (d_type == e_SUBJECT_CONSTRAINTS) {
        new (d_subjectConstraints.buffer())
            EncryptionCertificateSubjectConstraints(
                original.d_subjectConstraints.object(),
                d_allocator_p);
    }
    else if (d_type == e_ISSUER) {
        new (d_issuer.buffer())
            EncryptionCertificateIssuer(original.d_issuer.object(),
                                        d_allocator_p);
    }
    else if (d_type == e_ISSUER_KEY_IDENTIFIER) {
        new (d_issuerKeyIdentifier.buffer())
            EncryptionCertificateIssuerKeyIdentifier(
                original.d_issuerKeyIdentifier.object(),
                d_allocator_p);
    }
    else if (d_type == e_ISSUER_INFORMATION_ACCESS) {
        new (d_issuerInformationAccess.buffer())
            EncryptionCertificateIssuerInformationAccess(
                original.d_issuerInformationAccess.object(),
                d_allocator_p);
    }
    else if (d_type == e_POLICY) {
        new (d_policy.buffer())
            EncryptionCertificatePolicy(original.d_policy.object(),
                                        d_allocator_p);
    }
    else if (d_type == e_POLICY_MAPPINGS) {
        new (d_policyMappings.buffer()) EncryptionCertificatePolicyMappings(
            original.d_policyMappings.object(),
            d_allocator_p);
    }
    else if (d_type == e_POLICY_CONSTRAINTS) {
        new (d_policyConstraints.buffer())
            EncryptionCertificatePolicyConstraints(
                original.d_policyConstraints.object(),
                d_allocator_p);
    }
    else if (d_type == e_BYTE_SEQUENCE) {
        new (d_byteSequence.buffer())
            ntsa::AbstractOctetString(original.d_byteSequence.object(),
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

EncryptionCertificateExtensionValue::~EncryptionCertificateExtensionValue()
{
    this->reset();
}

EncryptionCertificateExtensionValue& EncryptionCertificateExtensionValue::
operator=(const EncryptionCertificateExtensionValue& other)
{
    if (this == &other) {
        return *this;
    }

    if (other.d_type == e_BOOLEAN) {
        this->makeBoolean(other.d_boolean.object());
    }
    else if (other.d_type == e_NAME) {
        this->makeName(other.d_name.object());
    }
    else if (other.d_type == e_NAME_ALTERNATIVE) {
        this->makeNameAlternative(other.d_nameAlternative.object());
    }
    else if (other.d_type == e_SUBJECT) {
        this->makeSubject(other.d_subject.object());
    }
    else if (other.d_type == e_SUBJECT_KEY_IDENTIFIER) {
        this->makeSubjectKeyIdentifier(other.d_subjectKeyIdentifier.object());
    }
    else if (other.d_type == e_SUBJECT_KEY_USAGE) {
        this->makeSubjectKeyUsage(other.d_subjectKeyUsage.object());
    }
    else if (other.d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        this->makeSubjectKeyUsageExtended(
            other.d_subjectKeyUsageExtended.object());
    }
    else if (other.d_type == e_SUBJECT_CONSTRAINTS) {
        this->makeSubjectConstraints(other.d_subjectConstraints.object());
    }
    else if (other.d_type == e_ISSUER) {
        this->makeIssuer(other.d_issuer.object());
    }
    else if (other.d_type == e_ISSUER_KEY_IDENTIFIER) {
        this->makeIssuerKeyIdentifier(other.d_issuerKeyIdentifier.object());
    }
    else if (other.d_type == e_ISSUER_INFORMATION_ACCESS) {
        this->makeIssuerInformationAccess(
            other.d_issuerInformationAccess.object());
    }
    else if (other.d_type == e_POLICY) {
        this->makePolicy(other.d_policy.object());
    }
    else if (other.d_type == e_POLICY_MAPPINGS) {
        this->makePolicyMappings(other.d_policyMappings.object());
    }
    else if (other.d_type == e_POLICY_CONSTRAINTS) {
        this->makePolicyConstraints(other.d_policyConstraints.object());
    }
    else if (other.d_type == e_BYTE_SEQUENCE) {
        this->makeByteSequence(other.d_byteSequence.object());
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

void EncryptionCertificateExtensionValue::reset()
{
    if (d_type == e_BOOLEAN) {
        typedef bool Type;
        d_boolean.object().~Type();
    }
    else if (d_type == e_NAME) {
        typedef EncryptionCertificateName Type;
        d_name.object().~Type();
    }
    else if (d_type == e_NAME_ALTERNATIVE) {
        typedef EncryptionCertificateNameAlternativeList Type;
        d_nameAlternative.object().~Type();
    }
    else if (d_type == e_SUBJECT) {
        typedef EncryptionCertificateSubject Type;
        d_subject.object().~Type();
    }
    else if (d_type == e_SUBJECT_KEY_IDENTIFIER) {
        typedef EncryptionCertificateSubjectKeyIdentifier Type;
        d_subjectKeyIdentifier.object().~Type();
    }
    else if (d_type == e_SUBJECT_KEY_USAGE) {
        typedef EncryptionCertificateSubjectKeyUsage Type;
        d_subjectKeyUsage.object().~Type();
    }
    else if (d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        typedef EncryptionCertificateSubjectKeyUsageExtended Type;
        d_subjectKeyUsageExtended.object().~Type();
    }
    else if (d_type == e_SUBJECT_CONSTRAINTS) {
        typedef EncryptionCertificateSubjectConstraints Type;
        d_subjectConstraints.object().~Type();
    }
    else if (d_type == e_ISSUER) {
        typedef EncryptionCertificateIssuer Type;
        d_issuer.object().~Type();
    }
    else if (d_type == e_ISSUER_KEY_IDENTIFIER) {
        typedef EncryptionCertificateIssuerKeyIdentifier Type;
        d_issuerKeyIdentifier.object().~Type();
    }
    else if (d_type == e_ISSUER_INFORMATION_ACCESS) {
        typedef EncryptionCertificateIssuerInformationAccess Type;
        d_issuerInformationAccess.object().~Type();
    }
    else if (d_type == e_POLICY) {
        typedef EncryptionCertificatePolicy Type;
        d_policy.object().~Type();
    }
    else if (d_type == e_POLICY_MAPPINGS) {
        typedef EncryptionCertificatePolicyMappings Type;
        d_policyMappings.object().~Type();
    }
    else if (d_type == e_POLICY_CONSTRAINTS) {
        typedef EncryptionCertificatePolicyConstraints Type;
        d_policyConstraints.object().~Type();
    }
    else if (d_type == e_BYTE_SEQUENCE) {
        typedef ntsa::AbstractOctetString Type;
        d_byteSequence.object().~Type();
    }
    else if (d_type == e_ANY) {
        typedef ntsa::AbstractValue Type;
        d_any.object().~Type();
    }

    d_type = e_UNDEFINED;
}

bool& EncryptionCertificateExtensionValue::makeBoolean()
{
    if (d_type == e_BOOLEAN) {
        d_boolean.object() = false;
    }
    else {
        this->reset();
        new (d_boolean.buffer()) bool(false);
        d_type = e_BOOLEAN;
    }

    return d_boolean.object();
}

bool& EncryptionCertificateExtensionValue::makeBoolean(bool value)
{
    if (d_type == e_BOOLEAN) {
        d_boolean.object() = value;
    }
    else {
        this->reset();
        new (d_boolean.buffer()) bool(value);
        d_type = e_BOOLEAN;
    }

    return d_boolean.object();
}

EncryptionCertificateName& EncryptionCertificateExtensionValue::makeName()
{
    if (d_type == e_NAME) {
        d_name.object().reset();
    }
    else {
        this->reset();
        new (d_name.buffer()) EncryptionCertificateName(d_allocator_p);
        d_type = e_NAME;
    }

    return d_name.object();
}

EncryptionCertificateName& EncryptionCertificateExtensionValue::makeName(
    const EncryptionCertificateName& value)
{
    if (d_type == e_NAME) {
        d_name.object() = value;
    }
    else {
        this->reset();
        new (d_name.buffer()) EncryptionCertificateName(value, d_allocator_p);
        d_type = e_NAME;
    }

    return d_name.object();
}

EncryptionCertificateNameAlternativeList& EncryptionCertificateExtensionValue::
    makeNameAlternative()
{
    if (d_type == e_NAME_ALTERNATIVE) {
        d_nameAlternative.object().reset();
    }
    else {
        this->reset();
        new (d_nameAlternative.buffer())
            EncryptionCertificateNameAlternativeList(d_allocator_p);
        d_type = e_NAME_ALTERNATIVE;
    }

    return d_nameAlternative.object();
}

EncryptionCertificateNameAlternativeList& EncryptionCertificateExtensionValue::
    makeNameAlternative(const EncryptionCertificateNameAlternativeList& value)
{
    if (d_type == e_NAME_ALTERNATIVE) {
        d_nameAlternative.object() = value;
    }
    else {
        this->reset();
        new (d_nameAlternative.buffer())
            EncryptionCertificateNameAlternativeList(value, d_allocator_p);
        d_type = e_NAME_ALTERNATIVE;
    }

    return d_nameAlternative.object();
}

EncryptionCertificateSubject& EncryptionCertificateExtensionValue::
    makeSubject()
{
    if (d_type == e_SUBJECT) {
        d_subject.object().reset();
    }
    else {
        this->reset();
        new (d_subject.buffer()) EncryptionCertificateSubject(d_allocator_p);
        d_type = e_SUBJECT;
    }

    return d_subject.object();
}

EncryptionCertificateSubject& EncryptionCertificateExtensionValue::makeSubject(
    const EncryptionCertificateSubject& value)
{
    if (d_type == e_SUBJECT) {
        d_subject.object() = value;
    }
    else {
        this->reset();
        new (d_subject.buffer())
            EncryptionCertificateSubject(value, d_allocator_p);
        d_type = e_SUBJECT;
    }

    return d_subject.object();
}

EncryptionCertificateSubjectKeyIdentifier& EncryptionCertificateExtensionValue::
    makeSubjectKeyIdentifier()
{
    if (d_type == e_SUBJECT_KEY_IDENTIFIER) {
        d_subjectKeyIdentifier.object().reset();
    }
    else {
        this->reset();
        new (d_subjectKeyIdentifier.buffer())
            EncryptionCertificateSubjectKeyIdentifier(d_allocator_p);
        d_type = e_SUBJECT_KEY_IDENTIFIER;
    }

    return d_subjectKeyIdentifier.object();
}

EncryptionCertificateSubjectKeyIdentifier& EncryptionCertificateExtensionValue::
    makeSubjectKeyIdentifier(
        const EncryptionCertificateSubjectKeyIdentifier& value)
{
    if (d_type == e_SUBJECT_KEY_IDENTIFIER) {
        d_subjectKeyIdentifier.object() = value;
    }
    else {
        this->reset();
        new (d_subjectKeyIdentifier.buffer())
            EncryptionCertificateSubjectKeyIdentifier(value, d_allocator_p);
        d_type = e_SUBJECT_KEY_IDENTIFIER;
    }

    return d_subjectKeyIdentifier.object();
}

EncryptionCertificateSubjectKeyUsage& EncryptionCertificateExtensionValue::
    makeSubjectKeyUsage()
{
    if (d_type == e_SUBJECT_KEY_USAGE) {
        d_subjectKeyUsage.object().reset();
    }
    else {
        this->reset();
        new (d_subjectKeyUsage.buffer())
            EncryptionCertificateSubjectKeyUsage(d_allocator_p);
        d_type = e_SUBJECT_KEY_USAGE;
    }

    return d_subjectKeyUsage.object();
}

EncryptionCertificateSubjectKeyUsage& EncryptionCertificateExtensionValue::
    makeSubjectKeyUsage(const EncryptionCertificateSubjectKeyUsage& value)
{
    if (d_type == e_SUBJECT_KEY_USAGE) {
        d_subjectKeyUsage.object() = value;
    }
    else {
        this->reset();
        new (d_subjectKeyUsage.buffer())
            EncryptionCertificateSubjectKeyUsage(value, d_allocator_p);
        d_type = e_SUBJECT_KEY_USAGE;
    }

    return d_subjectKeyUsage.object();
}

EncryptionCertificateSubjectKeyUsageExtended&
EncryptionCertificateExtensionValue::makeSubjectKeyUsageExtended()
{
    if (d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        d_subjectKeyUsageExtended.object().reset();
    }
    else {
        this->reset();
        new (d_subjectKeyUsageExtended.buffer())
            EncryptionCertificateSubjectKeyUsageExtended(d_allocator_p);
        d_type = e_SUBJECT_KEY_USAGE_EXTENDED;
    }

    return d_subjectKeyUsageExtended.object();
}

EncryptionCertificateSubjectKeyUsageExtended&
EncryptionCertificateExtensionValue::makeSubjectKeyUsageExtended(
    const EncryptionCertificateSubjectKeyUsageExtended& value)
{
    if (d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        d_subjectKeyUsageExtended.object() = value;
    }
    else {
        this->reset();
        new (d_subjectKeyUsageExtended.buffer())
            EncryptionCertificateSubjectKeyUsageExtended(value, d_allocator_p);
        d_type = e_SUBJECT_KEY_USAGE_EXTENDED;
    }

    return d_subjectKeyUsageExtended.object();
}

EncryptionCertificateSubjectConstraints& EncryptionCertificateExtensionValue::
    makeSubjectConstraints()
{
    if (d_type == e_SUBJECT_CONSTRAINTS) {
        d_subjectConstraints.object().reset();
    }
    else {
        this->reset();
        new (d_subjectConstraints.buffer())
            EncryptionCertificateSubjectConstraints(d_allocator_p);
        d_type = e_SUBJECT_CONSTRAINTS;
    }

    return d_subjectConstraints.object();
}

EncryptionCertificateSubjectConstraints& EncryptionCertificateExtensionValue::
    makeSubjectConstraints(
        const EncryptionCertificateSubjectConstraints& value)
{
    if (d_type == e_SUBJECT_CONSTRAINTS) {
        d_subjectConstraints.object() = value;
    }
    else {
        this->reset();
        new (d_subjectConstraints.buffer())
            EncryptionCertificateSubjectConstraints(value, d_allocator_p);
        d_type = e_SUBJECT_CONSTRAINTS;
    }

    return d_subjectConstraints.object();
}

EncryptionCertificateIssuer& EncryptionCertificateExtensionValue::makeIssuer()
{
    if (d_type == e_ISSUER) {
        d_issuer.object().reset();
    }
    else {
        this->reset();
        new (d_issuer.buffer()) EncryptionCertificateIssuer(d_allocator_p);
        d_type = e_ISSUER;
    }

    return d_issuer.object();
}

EncryptionCertificateIssuer& EncryptionCertificateExtensionValue::makeIssuer(
    const EncryptionCertificateIssuer& value)
{
    if (d_type == e_ISSUER) {
        d_issuer.object() = value;
    }
    else {
        this->reset();
        new (d_issuer.buffer())
            EncryptionCertificateIssuer(value, d_allocator_p);
        d_type = e_ISSUER;
    }

    return d_issuer.object();
}

EncryptionCertificateIssuerKeyIdentifier& EncryptionCertificateExtensionValue::
    makeIssuerKeyIdentifier()
{
    if (d_type == e_ISSUER_KEY_IDENTIFIER) {
        d_issuerKeyIdentifier.object().reset();
    }
    else {
        this->reset();
        new (d_issuerKeyIdentifier.buffer())
            EncryptionCertificateIssuerKeyIdentifier(d_allocator_p);
        d_type = e_ISSUER_KEY_IDENTIFIER;
    }

    return d_issuerKeyIdentifier.object();
}

EncryptionCertificateIssuerKeyIdentifier& EncryptionCertificateExtensionValue::
    makeIssuerKeyIdentifier(
        const EncryptionCertificateIssuerKeyIdentifier& value)
{
    if (d_type == e_ISSUER_KEY_IDENTIFIER) {
        d_issuerKeyIdentifier.object() = value;
    }
    else {
        this->reset();
        new (d_issuerKeyIdentifier.buffer())
            EncryptionCertificateIssuerKeyIdentifier(value, d_allocator_p);
        d_type = e_ISSUER_KEY_IDENTIFIER;
    }

    return d_issuerKeyIdentifier.object();
}

EncryptionCertificateIssuerInformationAccess&
EncryptionCertificateExtensionValue::makeIssuerInformationAccess()
{
    if (d_type == e_ISSUER_INFORMATION_ACCESS) {
        d_issuerInformationAccess.object().reset();
    }
    else {
        this->reset();
        new (d_issuerInformationAccess.buffer())
            EncryptionCertificateIssuerInformationAccess(d_allocator_p);
        d_type = e_ISSUER_INFORMATION_ACCESS;
    }

    return d_issuerInformationAccess.object();
}

EncryptionCertificateIssuerInformationAccess&
EncryptionCertificateExtensionValue::makeIssuerInformationAccess(
    const EncryptionCertificateIssuerInformationAccess& value)
{
    if (d_type == e_ISSUER_INFORMATION_ACCESS) {
        d_issuerInformationAccess.object() = value;
    }
    else {
        this->reset();
        new (d_issuerInformationAccess.buffer())
            EncryptionCertificateIssuerInformationAccess(value, d_allocator_p);
        d_type = e_ISSUER_INFORMATION_ACCESS;
    }

    return d_issuerInformationAccess.object();
}

EncryptionCertificatePolicy& EncryptionCertificateExtensionValue::makePolicy()
{
    if (d_type == e_POLICY) {
        d_policy.object().reset();
    }
    else {
        this->reset();
        new (d_policy.buffer()) EncryptionCertificatePolicy(d_allocator_p);
        d_type = e_POLICY;
    }

    return d_policy.object();
}

EncryptionCertificatePolicy& EncryptionCertificateExtensionValue::makePolicy(
    const EncryptionCertificatePolicy& value)
{
    if (d_type == e_POLICY) {
        d_policy.object() = value;
    }
    else {
        this->reset();
        new (d_policy.buffer())
            EncryptionCertificatePolicy(value, d_allocator_p);
        d_type = e_POLICY;
    }

    return d_policy.object();
}

EncryptionCertificatePolicyMappings& EncryptionCertificateExtensionValue::
    makePolicyMappings()
{
    if (d_type == e_POLICY_MAPPINGS) {
        d_policyMappings.object().reset();
    }
    else {
        this->reset();
        new (d_policyMappings.buffer())
            EncryptionCertificatePolicyMappings(d_allocator_p);
        d_type = e_POLICY_MAPPINGS;
    }

    return d_policyMappings.object();
}

EncryptionCertificatePolicyMappings& EncryptionCertificateExtensionValue::
    makePolicyMappings(const EncryptionCertificatePolicyMappings& value)
{
    if (d_type == e_POLICY_MAPPINGS) {
        d_policyMappings.object() = value;
    }
    else {
        this->reset();
        new (d_policyMappings.buffer())
            EncryptionCertificatePolicyMappings(value, d_allocator_p);
        d_type = e_POLICY_MAPPINGS;
    }

    return d_policyMappings.object();
}

EncryptionCertificatePolicyConstraints& EncryptionCertificateExtensionValue::
    makePolicyConstraints()
{
    if (d_type == e_POLICY_CONSTRAINTS) {
        d_policyConstraints.object().reset();
    }
    else {
        this->reset();
        new (d_policyConstraints.buffer())
            EncryptionCertificatePolicyConstraints(d_allocator_p);
        d_type = e_POLICY_CONSTRAINTS;
    }

    return d_policyConstraints.object();
}

EncryptionCertificatePolicyConstraints& EncryptionCertificateExtensionValue::
    makePolicyConstraints(const EncryptionCertificatePolicyConstraints& value)
{
    if (d_type == e_POLICY_CONSTRAINTS) {
        d_policyConstraints.object() = value;
    }
    else {
        this->reset();
        new (d_policyConstraints.buffer())
            EncryptionCertificatePolicyConstraints(value, d_allocator_p);
        d_type = e_POLICY_CONSTRAINTS;
    }

    return d_policyConstraints.object();
}

ntsa::AbstractOctetString& EncryptionCertificateExtensionValue::
    makeByteSequence()
{
    if (d_type == e_BYTE_SEQUENCE) {
        d_byteSequence.object().reset();
    }
    else {
        this->reset();
        new (d_byteSequence.buffer()) ntsa::AbstractOctetString(d_allocator_p);
        d_type = e_BYTE_SEQUENCE;
    }

    return d_byteSequence.object();
}

ntsa::AbstractOctetString& EncryptionCertificateExtensionValue::
    makeByteSequence(const ntsa::AbstractOctetString& value)
{
    if (d_type == e_BYTE_SEQUENCE) {
        d_byteSequence.object() = value;
    }
    else {
        this->reset();
        new (d_byteSequence.buffer())
            ntsa::AbstractOctetString(value, d_allocator_p);
        d_type = e_BYTE_SEQUENCE;
    }

    return d_byteSequence.object();
}

ntsa::AbstractValue& EncryptionCertificateExtensionValue::makeAny()
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

ntsa::AbstractValue& EncryptionCertificateExtensionValue::makeAny(
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

bool& EncryptionCertificateExtensionValue::boolean()
{
    BSLS_ASSERT(this->isBoolean());
    return d_boolean.object();
}

EncryptionCertificateName& EncryptionCertificateExtensionValue::name()
{
    BSLS_ASSERT(this->isName());
    return d_name.object();
}

EncryptionCertificateNameAlternativeList& EncryptionCertificateExtensionValue::
    nameAlternative()
{
    BSLS_ASSERT(this->isNameAlternative());
    return d_nameAlternative.object();
}

EncryptionCertificateSubject& EncryptionCertificateExtensionValue::subject()
{
    BSLS_ASSERT(this->isSubject());
    return d_subject.object();
}

EncryptionCertificateSubjectKeyIdentifier& EncryptionCertificateExtensionValue::
    subjectKeyIdentifier()
{
    BSLS_ASSERT(this->isSubjectKeyIdentifier());
    return d_subjectKeyIdentifier.object();
}

EncryptionCertificateSubjectKeyUsage& EncryptionCertificateExtensionValue::
    subjectKeyUsage()
{
    BSLS_ASSERT(this->isSubjectKeyUsage());
    return d_subjectKeyUsage.object();
}

EncryptionCertificateSubjectKeyUsageExtended&
EncryptionCertificateExtensionValue::subjectKeyUsageExtended()
{
    BSLS_ASSERT(this->isSubjectKeyUsageExtended());
    return d_subjectKeyUsageExtended.object();
}

EncryptionCertificateSubjectConstraints& EncryptionCertificateExtensionValue::
    subjectConstraints()
{
    BSLS_ASSERT(this->isSubjectConstraints());
    return d_subjectConstraints.object();
}

EncryptionCertificateIssuer& EncryptionCertificateExtensionValue::issuer()
{
    BSLS_ASSERT(this->isIssuer());
    return d_issuer.object();
}

EncryptionCertificateIssuerKeyIdentifier& EncryptionCertificateExtensionValue::
    issuerKeyIdentifier()
{
    BSLS_ASSERT(this->isIssuerKeyIdentifier());
    return d_issuerKeyIdentifier.object();
}

EncryptionCertificateIssuerInformationAccess&
EncryptionCertificateExtensionValue::issuerInformationAccess()
{
    BSLS_ASSERT(this->isIssuerInformationAccess());
    return d_issuerInformationAccess.object();
}

EncryptionCertificatePolicy& EncryptionCertificateExtensionValue::policy()
{
    BSLS_ASSERT(this->isPolicy());
    return d_policy.object();
}

EncryptionCertificatePolicyMappings& EncryptionCertificateExtensionValue::
    policyMappings()
{
    BSLS_ASSERT(this->isPolicyMappings());
    return d_policyMappings.object();
}

EncryptionCertificatePolicyConstraints& EncryptionCertificateExtensionValue::
    policyConstraints()
{
    BSLS_ASSERT(this->isPolicyConstraints());
    return d_policyConstraints.object();
}

ntsa::AbstractOctetString& EncryptionCertificateExtensionValue::byteSequence()
{
    BSLS_ASSERT(this->isByteSequence());
    return d_byteSequence.object();
}

ntsa::AbstractValue& EncryptionCertificateExtensionValue::any()
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

bool EncryptionCertificateExtensionValue::boolean() const
{
    BSLS_ASSERT(this->isBoolean());
    return d_boolean.object();
}

const EncryptionCertificateName& EncryptionCertificateExtensionValue::name()
    const
{
    BSLS_ASSERT(this->isName());
    return d_name.object();
}

const EncryptionCertificateNameAlternativeList&
EncryptionCertificateExtensionValue::nameAlternative() const
{
    BSLS_ASSERT(this->isNameAlternative());
    return d_nameAlternative.object();
}

const EncryptionCertificateSubject& EncryptionCertificateExtensionValue::
    subject() const
{
    BSLS_ASSERT(this->isSubject());
    return d_subject.object();
}

const EncryptionCertificateSubjectKeyIdentifier&
EncryptionCertificateExtensionValue::subjectKeyIdentifier() const
{
    BSLS_ASSERT(this->isSubjectKeyIdentifier());
    return d_subjectKeyIdentifier.object();
}

const EncryptionCertificateSubjectKeyUsage& EncryptionCertificateExtensionValue::
    subjectKeyUsage() const
{
    BSLS_ASSERT(this->isSubjectKeyUsage());
    return d_subjectKeyUsage.object();
}

const EncryptionCertificateSubjectKeyUsageExtended&
EncryptionCertificateExtensionValue::subjectKeyUsageExtended() const
{
    BSLS_ASSERT(this->isSubjectKeyUsageExtended());
    return d_subjectKeyUsageExtended.object();
}

const EncryptionCertificateSubjectConstraints&
EncryptionCertificateExtensionValue::subjectConstraints() const
{
    BSLS_ASSERT(this->isSubjectConstraints());
    return d_subjectConstraints.object();
}

const EncryptionCertificateIssuer& EncryptionCertificateExtensionValue::
    issuer() const
{
    BSLS_ASSERT(this->isIssuer());
    return d_issuer.object();
}

const EncryptionCertificateIssuerKeyIdentifier&
EncryptionCertificateExtensionValue::issuerKeyIdentifier() const
{
    BSLS_ASSERT(this->isIssuerKeyIdentifier());
    return d_issuerKeyIdentifier.object();
}

const EncryptionCertificateIssuerInformationAccess&
EncryptionCertificateExtensionValue::issuerInformationAccess() const
{
    BSLS_ASSERT(this->isIssuerInformationAccess());
    return d_issuerInformationAccess.object();
}

const EncryptionCertificatePolicy& EncryptionCertificateExtensionValue::
    policy() const
{
    BSLS_ASSERT(this->isPolicy());
    return d_policy.object();
}

const EncryptionCertificatePolicyMappings& EncryptionCertificateExtensionValue::
    policyMappings() const
{
    BSLS_ASSERT(this->isPolicyMappings());
    return d_policyMappings.object();
}

const EncryptionCertificatePolicyConstraints&
EncryptionCertificateExtensionValue::policyConstraints() const
{
    BSLS_ASSERT(this->isPolicyConstraints());
    return d_policyConstraints.object();
}

const ntsa::AbstractOctetString& EncryptionCertificateExtensionValue::
    byteSequence() const
{
    BSLS_ASSERT(this->isByteSequence());
    return d_byteSequence.object();
}

const ntsa::AbstractValue& EncryptionCertificateExtensionValue::any() const
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

bool EncryptionCertificateExtensionValue::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionCertificateExtensionValue::isBoolean() const
{
    return d_type == e_BOOLEAN;
}

bool EncryptionCertificateExtensionValue::isName() const
{
    return d_type == e_NAME;
}

bool EncryptionCertificateExtensionValue::isNameAlternative() const
{
    return d_type == e_NAME_ALTERNATIVE;
}

bool EncryptionCertificateExtensionValue::isSubject() const
{
    return d_type == e_SUBJECT;
}

bool EncryptionCertificateExtensionValue::isSubjectKeyIdentifier() const
{
    return d_type == e_SUBJECT_KEY_IDENTIFIER;
}

bool EncryptionCertificateExtensionValue::isSubjectKeyUsage() const
{
    return d_type == e_SUBJECT_KEY_USAGE;
}

bool EncryptionCertificateExtensionValue::isSubjectKeyUsageExtended() const
{
    return d_type == e_SUBJECT_KEY_USAGE_EXTENDED;
}

bool EncryptionCertificateExtensionValue::isSubjectConstraints() const
{
    return d_type == e_SUBJECT_CONSTRAINTS;
}

bool EncryptionCertificateExtensionValue::isIssuer() const
{
    return d_type == e_ISSUER;
}

bool EncryptionCertificateExtensionValue::isIssuerKeyIdentifier() const
{
    return d_type == e_ISSUER_KEY_IDENTIFIER;
}

bool EncryptionCertificateExtensionValue::isIssuerInformationAccess() const
{
    return d_type == e_ISSUER_INFORMATION_ACCESS;
}

bool EncryptionCertificateExtensionValue::isPolicy() const
{
    return d_type == e_POLICY;
}

bool EncryptionCertificateExtensionValue::isPolicyMappings() const
{
    return d_type == e_POLICY_MAPPINGS;
}

bool EncryptionCertificateExtensionValue::isPolicyConstraints() const
{
    return d_type == e_POLICY_CONSTRAINTS;
}

bool EncryptionCertificateExtensionValue::isByteSequence() const
{
    return d_type == e_BYTE_SEQUENCE;
}

bool EncryptionCertificateExtensionValue::isAny() const
{
    return d_type == e_ANY;
}

bool EncryptionCertificateExtensionValue::equals(
    const EncryptionCertificateExtensionValue& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_UNDEFINED) {
        return true;
    }
    else if (d_type == e_BOOLEAN) {
        return d_boolean.object() == other.d_boolean.object();
    }
    else if (d_type == e_NAME) {
        return d_name.object() == other.d_name.object();
    }
    else if (d_type == e_NAME_ALTERNATIVE) {
        return d_nameAlternative.object() == other.d_nameAlternative.object();
    }
    else if (d_type == e_SUBJECT) {
        return d_subject.object() == other.d_subject.object();
    }
    else if (d_type == e_SUBJECT_KEY_IDENTIFIER) {
        return d_subjectKeyIdentifier.object() ==
               other.d_subjectKeyIdentifier.object();
    }
    else if (d_type == e_SUBJECT_KEY_USAGE) {
        return d_subjectKeyUsage.object() == other.d_subjectKeyUsage.object();
    }
    else if (d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        return d_subjectKeyUsageExtended.object() ==
               other.d_subjectKeyUsageExtended.object();
    }
    else if (d_type == e_SUBJECT_CONSTRAINTS) {
        return d_subjectConstraints.object() ==
               other.d_subjectConstraints.object();
    }
    else if (d_type == e_ISSUER) {
        return d_issuer.object() == other.d_issuer.object();
    }
    else if (d_type == e_ISSUER_KEY_IDENTIFIER) {
        return d_issuerKeyIdentifier.object() ==
               other.d_issuerKeyIdentifier.object();
    }
    else if (d_type == e_ISSUER_INFORMATION_ACCESS) {
        return d_issuerInformationAccess.object() ==
               other.d_issuerInformationAccess.object();
    }
    else if (d_type == e_POLICY) {
        return d_policy.object() == other.d_policy.object();
    }
    else if (d_type == e_POLICY_MAPPINGS) {
        return d_policyMappings.object() == other.d_policyMappings.object();
    }
    else if (d_type == e_POLICY_CONSTRAINTS) {
        return d_policyConstraints.object() ==
               other.d_policyConstraints.object();
    }
    else if (d_type == e_BYTE_SEQUENCE) {
        return d_byteSequence.object() == other.d_byteSequence.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() == other.d_any.object();
    }

    return false;
}

bool EncryptionCertificateExtensionValue::less(
    const EncryptionCertificateExtensionValue& other) const
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
    else if (d_type == e_BOOLEAN) {
        return d_boolean.object() < other.d_boolean.object();
    }
    else if (d_type == e_NAME) {
        return d_name.object() < other.d_name.object();
    }
    else if (d_type == e_NAME_ALTERNATIVE) {
        return d_nameAlternative.object() < other.d_nameAlternative.object();
    }
    else if (d_type == e_SUBJECT) {
        return d_subject.object() < other.d_subject.object();
    }
    else if (d_type == e_SUBJECT_KEY_IDENTIFIER) {
        return d_subjectKeyIdentifier.object() <
               other.d_subjectKeyIdentifier.object();
    }
    else if (d_type == e_SUBJECT_KEY_USAGE) {
        return d_subjectKeyUsage.object() < other.d_subjectKeyUsage.object();
    }
    else if (d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        return d_subjectKeyUsageExtended.object() <
               other.d_subjectKeyUsageExtended.object();
    }
    else if (d_type == e_SUBJECT_CONSTRAINTS) {
        return d_subjectConstraints.object() <
               other.d_subjectConstraints.object();
    }
    else if (d_type == e_ISSUER) {
        return d_issuer.object() < other.d_issuer.object();
    }
    else if (d_type == e_ISSUER_KEY_IDENTIFIER) {
        return d_issuerKeyIdentifier.object() <
               other.d_issuerKeyIdentifier.object();
    }
    else if (d_type == e_ISSUER_INFORMATION_ACCESS) {
        return d_issuerInformationAccess.object() <
               other.d_issuerInformationAccess.object();
    }
    else if (d_type == e_POLICY) {
        return d_policy.object() < other.d_policy.object();
    }
    else if (d_type == e_POLICY_MAPPINGS) {
        return d_policyMappings.object() < other.d_policyMappings.object();
    }
    else if (d_type == e_POLICY_CONSTRAINTS) {
        return d_policyConstraints.object() <
               other.d_policyConstraints.object();
    }
    else if (d_type == e_BYTE_SEQUENCE) {
        return d_byteSequence.object() < other.d_byteSequence.object();
    }
    else if (d_type == e_ANY) {
        return d_any.object() < other.d_any.object();
    }

    return false;
}

bsl::ostream& EncryptionCertificateExtensionValue::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    if (d_type == e_BOOLEAN) {
        stream << d_boolean.object();
    }
    else if (d_type == e_NAME) {
        d_name.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_NAME_ALTERNATIVE) {
        d_nameAlternative.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_SUBJECT) {
        d_subject.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_SUBJECT_KEY_IDENTIFIER) {
        d_subjectKeyIdentifier.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_SUBJECT_KEY_USAGE) {
        d_subjectKeyUsage.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_SUBJECT_KEY_USAGE_EXTENDED) {
        d_subjectKeyUsageExtended.object().print(stream,
                                                 level,
                                                 spacesPerLevel);
    }
    else if (d_type == e_SUBJECT_CONSTRAINTS) {
        d_subjectConstraints.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_ISSUER) {
        d_issuer.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_ISSUER_KEY_IDENTIFIER) {
        d_issuerKeyIdentifier.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_ISSUER_INFORMATION_ACCESS) {
        d_issuerInformationAccess.object().print(stream,
                                                 level,
                                                 spacesPerLevel);
    }
    else if (d_type == e_POLICY) {
        d_policy.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_POLICY_MAPPINGS) {
        d_policyMappings.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_POLICY_CONSTRAINTS) {
        d_policyConstraints.object().print(stream, level, spacesPerLevel);
    }
    else if (d_type == e_BYTE_SEQUENCE) {
        d_byteSequence.object().print(stream, level, spacesPerLevel);
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
: d_attribute(basicAllocator)
, d_critical()
, d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtension::EncryptionCertificateExtension(
    const EncryptionCertificateExtension& original,
    bslma::Allocator*                     basicAllocator)
: d_attribute(original.d_attribute, basicAllocator)
, d_critical(original.d_critical)
, d_value(original.d_value, basicAllocator)
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
        d_attribute = other.d_attribute;
        d_critical  = other.d_critical;
        d_value     = other.d_value;
    }

    return *this;
}

void EncryptionCertificateExtension::reset()
{
    d_attribute.reset();
    d_critical.reset();
    d_value.reset();
}

void EncryptionCertificateExtension::setAttribute(
    const EncryptionCertificateExtensionIdentifier& value)
{
    d_attribute = value;
}

void EncryptionCertificateExtension::setAttribute(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_attribute = value;
}

void EncryptionCertificateExtension::setAttribute(
    ntca::EncryptionCertificateExtensionIdentifierType::Value value)
{
    d_attribute = value;
}

void EncryptionCertificateExtension::setValue(bool value)
{
    d_value.makeBoolean(value);
}

void EncryptionCertificateExtension::setValue(
    const EncryptionCertificateNameAlternativeList& value)
{
    d_value.makeNameAlternative(value);
}

void EncryptionCertificateExtension::setValue(const ntsa::AbstractValue& value)
{
    d_value.makeAny(value);
}

void EncryptionCertificateExtension::setValue(
    const EncryptionCertificateExtensionValue& value)
{
    d_value = value;
}

void EncryptionCertificateExtension::setCritical(bool value)
{
    d_critical = value;
}

ntsa::Error EncryptionCertificateExtension::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = d_attribute.decode(decoder);
    if (error) {
        return error;
    }

    while (decoder->contentBytesRemaining() > 0) {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagClass() == k_UNIVERSAL &&
            decoder->current().tagType() == k_PRIMITIVE &&
            decoder->current().tagNumber() == k_BOOLEAN)
        {
            error = decoder->decodeValue(&d_critical.makeValue());
            if (error) {
                return error;
            }
        }
        else if (decoder->current().tagClass() == k_UNIVERSAL &&
                 decoder->current().tagType() == k_PRIMITIVE &&
                 decoder->current().tagNumber() == k_OCTET_STRING)
        {
            if (d_attribute.equals(
                    EncryptionCertificateExtensionIdentifierType::
                        e_SUBJECT_ALTERNATIVE_NAME))
            {
                error = d_value.makeNameAlternative().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_SUBJECT_KEY_USAGE))
            {
                error = d_value.makeSubjectKeyUsage().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_SUBJECT_KEY_USAGE_EXTENDED))
            {
                error = d_value.makeSubjectKeyUsageExtended().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_SUBJECT_CONSTRAINTS))
            {
                error = d_value.makeSubjectConstraints().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_ISSUER_ALTERNATIVE_NAME))
            {
                error = d_value.makeNameAlternative().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_ISSUER_KEY_IDENTIFIER))
            {
                error = d_value.makeIssuerKeyIdentifier().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_ISSUER_INFORMATION_ACCESS))
            {
                error = d_value.makeIssuerInformationAccess().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_POLICY))
            {
                error = d_value.makePolicy().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_POLICY_MAPPINGS))
            {
                error = d_value.makePolicyMappings().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else if (d_attribute.equals(
                         EncryptionCertificateExtensionIdentifierType::
                             e_POLICY_CONSTRAINTS))
            {
                error = d_value.makePolicyConstraints().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else {
                error = decoder->decodeValue(&d_value.makeByteSequence());
                if (error) {
                    return error;
                }
            }
        }
        else {
            error = decoder->decodeValue(&d_value.makeAny());
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

ntsa::Error EncryptionCertificateExtension::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = d_attribute.encode(encoder);
    if (error) {
        return error;
    }

    if (!d_critical.isNull()) {
        error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BOOLEAN);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_critical.value());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OCTET_STRING);
    if (error) {
        return error;
    }

    if (d_value.isName()) {
        error = encoder->encodeValue(d_value.boolean());
        if (error) {
            return error;
        }
    }
    else if (d_value.isName()) {
        error = d_value.name().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isNameAlternative()) {
        error = d_value.nameAlternative().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isSubject()) {
        error = d_value.subject().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isSubjectKeyIdentifier()) {
        error = d_value.subjectKeyIdentifier().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isSubjectKeyUsage()) {
        error = d_value.subjectKeyUsage().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isSubjectKeyUsageExtended()) {
        error = d_value.subjectKeyUsageExtended().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isSubjectConstraints()) {
        error = d_value.subjectConstraints().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isIssuer()) {
        error = d_value.issuer().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isIssuerKeyIdentifier()) {
        error = d_value.issuerKeyIdentifier().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isIssuerInformationAccess()) {
        error = d_value.issuerInformationAccess().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isPolicy()) {
        error = d_value.policy().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isPolicyMappings()) {
        error = d_value.policyMappings().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isPolicyConstraints()) {
        error = d_value.policyConstraints().encode(encoder);
        if (error) {
            return error;
        }
    }
    else if (d_value.isByteSequence()) {
        error = encoder->encodeValue(d_value.byteSequence());
        if (error) {
            return error;
        }
    }
    else if (d_value.isAny()) {
        error = encoder->encodeValue(d_value.any());
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

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

const EncryptionCertificateExtensionIdentifier& EncryptionCertificateExtension::
    attribute() const
{
    return d_attribute;
}

const EncryptionCertificateExtensionValue& EncryptionCertificateExtension::
    value() const
{
    return d_value;
}

const bdlb::NullableValue<bool>& EncryptionCertificateExtension::critical()
    const
{
    return d_critical;
}

bool EncryptionCertificateExtension::equals(
    const EncryptionCertificateExtension& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateExtension::less(
    const EncryptionCertificateExtension& other) const
{
    if (d_attribute < other.d_attribute) {
        return true;
    }

    if (other.d_attribute < d_attribute) {
        return false;
    }

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateExtension::print(bsl::ostream& stream,
                                                    int           level,
                                                    int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute", d_attribute);
    printer.printAttribute("value", d_value);

    if (!d_critical.isNull()) {
        printer.printAttribute("critical", d_critical.value());
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                         stream,
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
: d_container(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionList::EncryptionCertificateExtensionList(
    const EncryptionCertificateExtensionList& original,
    bslma::Allocator*                         basicAllocator)
: d_container(original.d_container, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionList::~EncryptionCertificateExtensionList()
{
}

EncryptionCertificateExtensionList& EncryptionCertificateExtensionList::
operator=(const EncryptionCertificateExtensionList& other)
{
    if (this != &other) {
        d_container = other.d_container;
    }

    return *this;
}

void EncryptionCertificateExtensionList::reset()
{
    d_container.clear();
}

ntsa::Error EncryptionCertificateExtensionList::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    while (decoder->contentBytesRemaining() > 0) {
        EncryptionCertificateExtension element;
        error = element.decode(decoder);
        if (error) {
            return error;
        }

        d_container.push_back(element);
    }

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateExtensionList::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    if (d_container.empty()) {
        return ntsa::Error();
    }

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateExtension& element = d_container[i];
        error = element.encode(encoder);
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

bool EncryptionCertificateExtensionList::isAuthority() const
{
    const ntca::EncryptionCertificateExtensionIdentifierType::Value
        k_SUBJECT_CONSTRAINTS =
            ntca::EncryptionCertificateExtensionIdentifierType::
                e_SUBJECT_CONSTRAINTS;

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateExtension& element = d_container[i];

        if (element.attribute().equals(k_SUBJECT_CONSTRAINTS)) {
            if (element.value().isSubjectConstraints()) {
                const EncryptionCertificateSubjectConstraints&
                    subjectConstraints = element.value().subjectConstraints();

                if (subjectConstraints.authority().value_or(false)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EncryptionCertificateExtensionList::matchesSubjectDomainName(
    const bsl::string& domainName) const
{
    const ntca::EncryptionCertificateExtensionIdentifierType::Value
        k_SUBJECT_ALTERNATIVE_NAME =
            ntca::EncryptionCertificateExtensionIdentifierType::
                e_SUBJECT_ALTERNATIVE_NAME;

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateExtension& element = d_container[i];

        if (element.attribute().equals(k_SUBJECT_ALTERNATIVE_NAME)) {
            if (element.value().isNameAlternative()) {
                const EncryptionCertificateNameAlternativeList&
                    nameAlternativeList = element.value().nameAlternative();

                if (nameAlternativeList.matchesDomainName(domainName)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EncryptionCertificateExtensionList::matchesSubjectIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    const ntca::EncryptionCertificateExtensionIdentifierType::Value
        k_SUBJECT_ALTERNATIVE_NAME =
            ntca::EncryptionCertificateExtensionIdentifierType::
                e_SUBJECT_ALTERNATIVE_NAME;

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateExtension& element = d_container[i];

        if (element.attribute().equals(k_SUBJECT_ALTERNATIVE_NAME)) {
            if (element.value().isNameAlternative()) {
                const EncryptionCertificateNameAlternativeList&
                    nameAlternativeList = element.value().nameAlternative();

                if (nameAlternativeList.matchesIpAddress(ipAddress)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EncryptionCertificateExtensionList::matchesIssuerDomainName(
    const bsl::string& domainName) const
{
    const ntca::EncryptionCertificateExtensionIdentifierType::Value
        k_ISSUER_ALTERNATIVE_NAME =
            ntca::EncryptionCertificateExtensionIdentifierType::
                e_ISSUER_ALTERNATIVE_NAME;

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateExtension& element = d_container[i];

        if (element.attribute().equals(k_ISSUER_ALTERNATIVE_NAME)) {
            if (element.value().isNameAlternative()) {
                const EncryptionCertificateNameAlternativeList&
                    nameAlternativeList = element.value().nameAlternative();

                if (nameAlternativeList.matchesDomainName(domainName)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EncryptionCertificateExtensionList::matchesIssuerIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    const ntca::EncryptionCertificateExtensionIdentifierType::Value
        k_ISSUER_ALTERNATIVE_NAME =
            ntca::EncryptionCertificateExtensionIdentifierType::
                e_ISSUER_ALTERNATIVE_NAME;

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateExtension& element = d_container[i];

        if (element.attribute().equals(k_ISSUER_ALTERNATIVE_NAME)) {
            if (element.value().isNameAlternative()) {
                const EncryptionCertificateNameAlternativeList&
                    nameAlternativeList = element.value().nameAlternative();

                if (nameAlternativeList.matchesIpAddress(ipAddress)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EncryptionCertificateExtensionList::allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageType::Value usage) const
{
    const ntca::EncryptionCertificateExtensionIdentifierType::Value
        k_KEY_USAGE =
            ntca::EncryptionCertificateExtensionIdentifierType::
                e_SUBJECT_KEY_USAGE;

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateExtension& element = d_container[i];

        if (element.attribute().equals(k_KEY_USAGE)) {
            if (element.value().isSubjectKeyUsage()) {
                const EncryptionCertificateSubjectKeyUsage&
                    subjectKeyUsage = element.value().subjectKeyUsage();

                if (subjectKeyUsage.allows(usage)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EncryptionCertificateExtensionList::allowsKeyUsage(
        ntca::EncryptionCertificateSubjectKeyUsageExtendedType::Value usage) 
        const
{
    const ntca::EncryptionCertificateExtensionIdentifierType::Value
        k_KEY_USAGE_EXTENDED =
            ntca::EncryptionCertificateExtensionIdentifierType::
                e_SUBJECT_KEY_USAGE_EXTENDED;

    for (bsl::size_t i = 0; i < d_container.size(); ++i) {
        const EncryptionCertificateExtension& element = d_container[i];

        if (element.attribute().equals(k_KEY_USAGE_EXTENDED)) {
            if (element.value().isSubjectKeyUsageExtended()) {
                const EncryptionCertificateSubjectKeyUsageExtended&
                    subjectKeyUsageExtended = 
                        element.value().subjectKeyUsageExtended();

                if (subjectKeyUsageExtended.allows(usage)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EncryptionCertificateExtensionList::equals(
    const EncryptionCertificateExtensionList& other) const
{
    return d_container == other.d_container;
}

bool EncryptionCertificateExtensionList::less(
    const EncryptionCertificateExtensionList& other) const
{
    return d_container < other.d_container;
}

bsl::ostream& EncryptionCertificateExtensionList::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("container", d_container);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                             stream,
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

EncryptionCertificateEntity::EncryptionCertificateEntity(
    bslma::Allocator* basicAllocator)
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
    bslma::Allocator*                  basicAllocator)
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
        d_version              = other.d_version;
        d_serialNumber         = other.d_serialNumber;
        d_signatureAlgorithm   = other.d_signatureAlgorithm;
        d_issuer               = other.d_issuer;
        d_validity             = other.d_validity;
        d_subject              = other.d_subject;
        d_subjectPublicKeyInfo = other.d_subjectPublicKeyInfo;
        d_issuerUniqueId       = other.d_issuerUniqueId;
        d_subjectUniqueId      = other.d_subjectUniqueId;
        d_extensionList        = other.d_extensionList;
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

ntsa::Error EncryptionCertificateEntity::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagClass() == k_CONTEXT_SPECIFIC &&
            decoder->current().tagType() == k_CONSTRUCTED &&
            decoder->current().tagNumber() == 0)
        {
            error = d_version.makeValue().decode(decoder);
            if (error) {
                return error;
            }

            error = decoder->decodeTagComplete();
            if (error) {
                return error;
            }
        }
        else {
            error = decoder->rewindTag();
            if (error) {
                return error;
            }
        }
    }

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
    if (error) {
        return error;
    }

    error = decoder->decodeValue(&d_serialNumber);
    if (error) {
        return error;
    }

    error = decoder->decodeTagComplete();
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

    while (decoder->contentBytesRemaining() > 0) {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagClass() == k_CONTEXT_SPECIFIC &&
            decoder->current().tagType() == k_PRIMITIVE &&
            decoder->current().tagNumber() == 1)
        {
            error = decoder->decodeValue(&d_issuerUniqueId.makeValue());
            if (error) {
                return error;
            }
        }
        else if (decoder->current().tagClass() == k_CONTEXT_SPECIFIC &&
                 decoder->current().tagType() == k_PRIMITIVE &&
                 decoder->current().tagNumber() == 2)
        {
            error = decoder->decodeValue(&d_subjectUniqueId.makeValue());
            if (error) {
                return error;
            }
        }
        else if (decoder->current().tagClass() == k_CONTEXT_SPECIFIC &&
                 decoder->current().tagType() == k_CONSTRUCTED &&
                 decoder->current().tagNumber() == 3)
        {
            error = d_extensionList.makeValue().decode(decoder);
            if (error) {
                return error;
            }
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

ntsa::Error EncryptionCertificateEntity::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    if (!d_version.isNull()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, 0);
        if (error) {
            return error;
        }

        error = d_version.value().encode(encoder);
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    error = encoder->encodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
    if (error) {
        return error;
    }

    error = encoder->encodeValue(d_serialNumber);
    if (error) {
        return error;
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    error = d_signatureAlgorithm.encode(encoder);
    if (error) {
        return error;
    }

    error = d_issuer.encode(encoder);
    if (error) {
        return error;
    }

    error = d_validity.encode(encoder);
    if (error) {
        return error;
    }

    error = d_subject.encode(encoder);
    if (error) {
        return error;
    }

    error = d_subjectPublicKeyInfo.encode(encoder);
    if (error) {
        return error;
    }

    if (!d_issuerUniqueId.isNull()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_PRIMITIVE, 1);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_issuerUniqueId.value());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    if (!d_subjectUniqueId.isNull()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_PRIMITIVE, 2);
        if (error) {
            return error;
        }

        error = encoder->encodeValue(d_subjectUniqueId.value());
        if (error) {
            return error;
        }

        error = encoder->encodeTagComplete();
        if (error) {
            return error;
        }
    }

    if (!d_extensionList.isNull()) {
        error = encoder->encodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, 3);
        if (error) {
            return error;
        }

        error = d_extensionList.value().encode(encoder);
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

const ntsa::AbstractInteger& EncryptionCertificateEntity::serialNumber() const
{
    return d_serialNumber;
}

const ntca::EncryptionCertificateSubject& EncryptionCertificateEntity::
    subject() const
{
    return d_subject;
}

const bdlb::NullableValue<ntsa::AbstractBitString>& EncryptionCertificateEntity::
    subjectUniqueId() const
{
    return d_subjectUniqueId;
}

const ntca::EncryptionKeyInfoPublic& EncryptionCertificateEntity::
    subjectPublicKeyInfo() const
{
    return d_subjectPublicKeyInfo;
}

const ntca::EncryptionCertificateIssuer& EncryptionCertificateEntity::issuer()
    const
{
    return d_issuer;
}

const bdlb::NullableValue<ntsa::AbstractBitString>& EncryptionCertificateEntity::
    issuerUniqueId() const
{
    return d_issuerUniqueId;
}

const bdlb::NullableValue<ntca::EncryptionCertificateExtensionList>&
EncryptionCertificateEntity::extensionList() const
{
    return d_extensionList;
}

const ntca::EncryptionCertificateValidity& EncryptionCertificateEntity::
    validity() const
{
    return d_validity;
}

const ntca::EncryptionCertificateSignatureAlgorithm& EncryptionCertificateEntity::
    signatureAlgorithm() const
{
    return d_signatureAlgorithm;
}

bool EncryptionCertificateEntity::isAuthority() const
{
    if (!d_extensionList.isNull()) {
        return d_extensionList.value().isAuthority();
    }

    return false;
}

bool EncryptionCertificateEntity::matchesSubjectDomainName(
    const bsl::string& domainName) const
{
    bool result = d_subject.matchesDomainName(domainName);
    if (result) {
        return result;
    }

    if (!d_extensionList.isNull()) {
        result = d_extensionList.value().matchesSubjectDomainName(domainName);
        if (result) {
            return result;
        }
    }

    return false;
}

bool EncryptionCertificateEntity::matchesSubjectIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    bool result = d_subject.matchesIpAddress(ipAddress);
    if (result) {
        return result;
    }

    if (!d_extensionList.isNull()) {
        result = d_extensionList.value().matchesSubjectIpAddress(ipAddress);
        if (result) {
            return result;
        }
    }

    return false;
}

bool EncryptionCertificateEntity::matchesIssuerDomainName(
    const bsl::string& domainName) const
{
    bool result = d_issuer.matchesDomainName(domainName);
    if (result) {
        return result;
    }

    if (!d_extensionList.isNull()) {
        result = d_extensionList.value().matchesIssuerDomainName(domainName);
        if (result) {
            return result;
        }
    }

    return false;
}

bool EncryptionCertificateEntity::matchesIssuerIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    bool result = d_issuer.matchesIpAddress(ipAddress);
    if (result) {
        return result;
    }

    if (!d_extensionList.isNull()) {
        result = d_extensionList.value().matchesIssuerIpAddress(ipAddress);
        if (result) {
            return result;
        }
    }

    return false;
}

bool EncryptionCertificateEntity::allowsKeyUsage(
    ntca::EncryptionCertificateSubjectKeyUsageType::Value usage) const
{
    if (!d_extensionList.isNull()) {
        return d_extensionList.value().allowsKeyUsage(usage);
    }

    return false;
}

bool EncryptionCertificateEntity::allowsKeyUsage(
    ntca::EncryptionCertificateSubjectKeyUsageExtendedType::Value usage) 
    const
{
    if (!d_extensionList.isNull()) {
        return d_extensionList.value().allowsKeyUsage(usage);
    }

    return false;
}

bool EncryptionCertificateEntity::equals(
    const EncryptionCertificateEntity& other) const
{
    return d_version == other.d_version &&
           d_serialNumber == other.d_serialNumber &&
           d_signatureAlgorithm == other.d_signatureAlgorithm &&
           d_issuer == other.d_issuer && d_validity == other.d_validity &&
           d_subject == other.d_subject &&
           d_subjectPublicKeyInfo == other.d_subjectPublicKeyInfo &&
           d_issuerUniqueId == other.d_issuerUniqueId &&
           d_subjectUniqueId == other.d_subjectUniqueId &&
           d_extensionList == other.d_extensionList;
}

bool EncryptionCertificateEntity::less(
    const EncryptionCertificateEntity& other) const
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
                                                 int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (!d_version.isNull()) {
        printer.printAttribute("version", d_version);
    }
    printer.printAttribute("serialNumber", d_serialNumber);
    printer.printAttribute("subject", d_subject);
    if (!d_subjectUniqueId.isNull()) {
        printer.printAttribute("subjectUniqueId", d_subjectUniqueId);
    }
    printer.printAttribute("subjectPublicKeyInfo", d_subjectPublicKeyInfo);
    printer.printAttribute("issuer", d_issuer);
    if (!d_issuerUniqueId.isNull()) {
        printer.printAttribute("issuerUniqueId", d_issuerUniqueId);
    }
    printer.printAttribute("validity", d_validity);
    if (!d_extensionList.isNull()) {
        printer.printAttribute("extensionList", d_extensionList);
    }
    printer.printAttribute("signatureAlgorithm", d_signatureAlgorithm);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                      stream,
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
        d_signature          = other.d_signature;
    }

    return *this;
}

void EncryptionCertificate::reset()
{
    d_entity.reset();
    d_signatureAlgorithm.reset();
    d_signature.reset();
}

ntsa::Error EncryptionCertificate::decode(ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
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

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificate::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    ntsa::Error error;

    error = encoder->encodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    error = d_entity.encode(encoder);
    if (error) {
        return error;
    }

    error = d_signatureAlgorithm.encode(encoder);
    if (error) {
        return error;
    }

    error = d_signature.encode(encoder);
    if (error) {
        return error;
    }

    error = encoder->encodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

const ntca::EncryptionCertificateEntity& EncryptionCertificate::entity() const
{
    return d_entity;
}

const ntsa::AbstractInteger& EncryptionCertificate::serialNumber() const
{
    return d_entity.serialNumber();
}

const ntca::EncryptionCertificateSubject& EncryptionCertificate::subject()
    const
{
    return d_entity.subject();
}

const bdlb::NullableValue<ntsa::AbstractBitString>& EncryptionCertificate::
    subjectUniqueId() const
{
    return d_entity.subjectUniqueId();
}

const ntca::EncryptionKeyInfoPublic& EncryptionCertificate::
    subjectPublicKeyInfo() const
{
    return d_entity.subjectPublicKeyInfo();
}

const ntca::EncryptionCertificateIssuer& EncryptionCertificate::issuer() const
{
    return d_entity.issuer();
}

const bdlb::NullableValue<ntsa::AbstractBitString>& EncryptionCertificate::
    issuerUniqueId() const
{
    return d_entity.issuerUniqueId();
}

const bdlb::NullableValue<ntca::EncryptionCertificateExtensionList>&
EncryptionCertificate::extensionList() const
{
    return d_entity.extensionList();
}

const ntca::EncryptionCertificateValidity& EncryptionCertificate::validity()
    const
{
    return d_entity.validity();
}

const ntca::EncryptionCertificateSignatureAlgorithm& EncryptionCertificate::
    signatureAlgorithm() const
{
    return d_signatureAlgorithm;
}

const ntca::EncryptionCertificateSignature& EncryptionCertificate::signature()
    const
{
    return d_signature;
}

bool EncryptionCertificate::isAuthority() const
{
    return d_entity.isAuthority();
}

bool EncryptionCertificate::matchesSubjectDomainName(
    const bsl::string& domainName) const
{
    return d_entity.matchesSubjectDomainName(domainName);
}

bool EncryptionCertificate::matchesSubjectIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    return d_entity.matchesSubjectIpAddress(ipAddress);
}

bool EncryptionCertificate::matchesIssuerDomainName(
    const bsl::string& domainName) const
{
    return d_entity.matchesIssuerDomainName(domainName);
}

bool EncryptionCertificate::matchesIssuerIpAddress(
    const ntsa::IpAddress& ipAddress) const
{
    return d_entity.matchesIssuerIpAddress(ipAddress);
}

bool EncryptionCertificate::allowsKeyUsage(
    ntca::EncryptionCertificateSubjectKeyUsageType::Value usage) const
{
    return d_entity.allowsKeyUsage(usage);
}

bool EncryptionCertificate::allowsKeyUsage(
    ntca::EncryptionCertificateSubjectKeyUsageExtendedType::Value usage) 
    const
{
    return d_entity.allowsKeyUsage(usage);
}

bool EncryptionCertificate::usesSubjectPublicKeyAlgorithm(
    ntca::EncryptionKeyAlgorithmIdentifierType::Value algorithm) const
{
    const ntca::EncryptionKeyAlgorithm& publicKeyAlgorithm = 
        d_entity.subjectPublicKeyInfo().algorithm();

    return publicKeyAlgorithm.identifier().equals(algorithm);
}

bool EncryptionCertificate::usesSubjectPublicKeyAlgorithmParameters(
    ntca::EncryptionKeyEllipticCurveParametersIdentifierType::Value 
    parameters) const
{
    const ntca::EncryptionKeyAlgorithm& publicKeyAlgorithm = 
        d_entity.subjectPublicKeyInfo().algorithm();

    if (publicKeyAlgorithm.parameters().isNull()) {
        return false;
    }

    const ntca::EncryptionKeyAlgorithmParameters& 
    algorithmParameters = publicKeyAlgorithm.parameters().value();

    if (!algorithmParameters.isEllipticCurve()) {
        return false;
    }

    return algorithmParameters.ellipticCurve().identifier().equals(parameters);
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

bool EncryptionCertificateUtil::matches(const bsl::string& requested,
                                        const bsl::string& certified)
{
    if (requested == certified) {
        return true;
    }

    {
        ntsa::IpAddress requestedIpAddress;
        if (requestedIpAddress.parse(requested)) {
            return requested == certified;
        }
    }

    {
        ntsa::IpAddress certifiedIpAddress;
        if (certifiedIpAddress.parse(certified)) {
            return requested == certified;
        }
    }

    bsl::vector<bsl::string> requestedDomains;
    {
        bsl::string::size_type u = 0U;
        bsl::string::size_type v = requested.find('.');

        while (v != std::string::npos) {
            requestedDomains.push_back(requested.substr(u, v - u));
            u = v + 1;
            v = requested.find('.', u);
        }

        requestedDomains.push_back(requested.substr(u, v));
    }

    bsl::vector<bsl::string> certifiedDomains;
    {
        bsl::string::size_type u = 0U;
        bsl::string::size_type v = certified.find('.');

        while (v != std::string::npos) {
            certifiedDomains.push_back(certified.substr(u, v - u));
            u = v + 1;
            v = certified.find('.', u);
        }

        certifiedDomains.push_back(certified.substr(u, v));
    }

    typedef bsl::vector<bsl::string>::const_reverse_iterator ReverseIterator;

    ReverseIterator requestedCurrent = requestedDomains.rbegin();
    ReverseIterator requestedEnd     = requestedDomains.rend();

    ReverseIterator certifiedCurrent = certifiedDomains.rbegin();
    ReverseIterator certifiedEnd     = certifiedDomains.rend();

    while (true) {
        if (requestedCurrent == requestedEnd) {
            return false;
        }

        if (certifiedCurrent == certifiedEnd) {
            return false;
        }

        if (*certifiedCurrent == "*") {
            break;
        }

        if (*requestedCurrent != *certifiedCurrent) {
            return false;
        }

        ++requestedCurrent;
        ++certifiedCurrent;
    }

    return true;
}

bool EncryptionCertificateUtil::matches(const bsl::string&     requested,
                                        const ntsa::IpAddress& certified)
{
    return requested == certified.text();
}

bool EncryptionCertificateUtil::matches(const bsl::string& requested,
                                        const ntsa::Uri&   certified)
{
    if (requested == certified.text()) {
        return true;
    }

    if (!certified.authority().isNull()) {
        const ntsa::UriAuthority& authority = certified.authority().value();
        if (!authority.host().isNull()) {
            const ntsa::Host& host = authority.host().value();

            if (bdlb::String::areEqualCaseless(requested, host.text())) {
                return true;
            }
        }
    }

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::Endpoint& requested,
                                        const bsl::string&    certified)
{
    if (requested.isIp()) {
        return EncryptionCertificateUtil::matches(requested.ip().host(),
                                                  certified);
    }
    else if (requested.isLocal()) {
        return EncryptionCertificateUtil::matches(requested.local(),
                                                  certified);
    }

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::Endpoint&  requested,
                                        const ntsa::IpAddress& certified)
{
    if (requested.isIp()) {
        return EncryptionCertificateUtil::matches(requested.ip().host(),
                                                  certified);
    }
    else if (requested.isLocal()) {
        return EncryptionCertificateUtil::matches(requested.local(),
                                                  certified);
    }

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::Endpoint& requested,
                                        const ntsa::Uri&      certified)
{
    if (requested.isIp()) {
        return EncryptionCertificateUtil::matches(requested.ip().host(),
                                                  certified);
    }
    else if (requested.isLocal()) {
        return EncryptionCertificateUtil::matches(requested.local(),
                                                  certified);
    }

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::IpAddress& requested,
                                        const bsl::string&     certified)
{
    return requested.text() == certified;
}

bool EncryptionCertificateUtil::matches(const ntsa::IpAddress& requested,
                                        const ntsa::IpAddress& certified)
{
    return requested == certified;
}

bool EncryptionCertificateUtil::matches(const ntsa::IpAddress& requested,
                                        const ntsa::Uri&       certified)
{
    if (!certified.authority().isNull()) {
        const ntsa::UriAuthority& authority = certified.authority().value();
        if (!authority.host().isNull()) {
            const ntsa::Host& host = authority.host().value();
            if (host.isIp()) {
                return requested == host.ip();
            }
        }
    }

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::LocalName& requested,
                                        const bsl::string&     certified)
{
    return requested.value() == certified;
}

bool EncryptionCertificateUtil::matches(const ntsa::LocalName& requested,
                                        const ntsa::IpAddress& certified)
{
    NTCCFG_WARNING_UNUSED(requested);
    NTCCFG_WARNING_UNUSED(certified);

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::LocalName& requested,
                                        const ntsa::Uri&       certified)
{
    NTCCFG_WARNING_UNUSED(requested);
    NTCCFG_WARNING_UNUSED(certified);

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::Uri&   requested,
                                        const bsl::string& certified)
{
    if (!requested.authority().isNull()) {
        const ntsa::UriAuthority& authority = requested.authority().value();
        if (!authority.host().isNull()) {
            const ntsa::Host& host = authority.host().value();

            if (host.text() == certified) {
                return true;
            }
        }
    }

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::Uri&       requested,
                                        const ntsa::IpAddress& certified)
{
    if (!requested.authority().isNull()) {
        const ntsa::UriAuthority& authority = requested.authority().value();
        if (!authority.host().isNull()) {
            const ntsa::Host& host = authority.host().value();

            if (host.text() == certified.text()) {
                return true;
            }
        }
    }

    return false;
}

bool EncryptionCertificateUtil::matches(const ntsa::Uri& requested,
                                        const ntsa::Uri& certified)
{
    if (requested.authority().isNull()) {
        return false;
    }

    if (requested.authority().value().host().isNull()) {
        return false;
    }

    if (certified.authority().isNull()) {
        return false;
    }

    if (certified.authority().value().host().isNull()) {
        return false;
    }

    if (requested.authority().value().host().value() ==
        certified.authority().value().host().value())
    {
        return true;
    }

    return false;
}

}  // close package namespace
}  // close enterprise namespace
