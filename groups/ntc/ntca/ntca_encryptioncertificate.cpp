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

const ntsa::AbstractSyntaxTagNumber::Value k_VISIBLE_STRING =
    ntsa::AbstractSyntaxTagNumber::e_VISIBLE_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_PRINTABLE_STRING =
    ntsa::AbstractSyntaxTagNumber::e_PRINTABLE_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_UTF8_STRING =
    ntsa::AbstractSyntaxTagNumber::e_UTF8_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_BIT_STRING =
    ntsa::AbstractSyntaxTagNumber::e_BIT_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_OCTET_STRING =
    ntsa::AbstractSyntaxTagNumber::e_OCTET_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_OBJECT_IDENTIFIER =
    ntsa::AbstractSyntaxTagNumber::e_OBJECT_IDENTIFIER;

}  // close unnamed namespace

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

    error = decoder->decodeTag(k_CONTEXT_SPECIFIC, k_CONSTRUCTED, 0);
    if (error) {
        return error;
    }

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

    error = decoder->decodeTagComplete();
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

const char* EncryptionCertificateNameAttributeType::toString(Value value)
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

void EncryptionCertificateNameAttributeType::toObjectIdentifier(
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

int EncryptionCertificateNameAttributeType::fromString(
    EncryptionCertificateNameAttributeType::Value* result,
    const bslstl::StringRef&                       string)
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

int EncryptionCertificateNameAttributeType::fromObjectIdentifier(
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

bsl::ostream& EncryptionCertificateNameAttributeType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                                 stream,
                         EncryptionCertificateNameAttributeType::Value rhs)
{
    return EncryptionCertificateNameAttributeType::print(stream, rhs);
}

EncryptionCertificateNameAttribute::EncryptionCertificateNameAttribute(
    bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameAttribute::EncryptionCertificateNameAttribute(
    const EncryptionCertificateNameAttribute& original,
    bslma::Allocator*                         basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
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
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificateNameAttribute& EncryptionCertificateNameAttribute::
operator=(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateNameAttribute& EncryptionCertificateNameAttribute::
operator=(EncryptionCertificateNameAttributeType::Value value)
{
    EncryptionCertificateNameAttributeType::toObjectIdentifier(&d_identifier,
                                                               value);
    return *this;
}

void EncryptionCertificateNameAttribute::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateNameAttribute::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificateNameAttribute::setIdentifer(
    EncryptionCertificateNameAttributeType::Value value)
{
    EncryptionCertificateNameAttributeType::toObjectIdentifier(&d_identifier,
                                                               value);
}

ntsa::Error EncryptionCertificateNameAttribute::decode(
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

ntsa::Error EncryptionCertificateNameAttribute::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const ntsa::AbstractObjectIdentifier& EncryptionCertificateNameAttribute::
    identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateNameAttribute::equals(
    const EncryptionCertificateNameAttribute& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateNameAttribute::equals(
    EncryptionCertificateNameAttributeType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateNameAttributeType::toObjectIdentifier(&identifier,
                                                               value);

    return d_identifier.equals(identifier);
}

bool EncryptionCertificateNameAttribute::less(
    const EncryptionCertificateNameAttribute& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateNameAttribute::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionCertificateNameAttributeType::Value type;
    int                                                 rc =
        ntca::EncryptionCertificateNameAttributeType::fromObjectIdentifier(
            &type,
            d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateNameAttributeType::toString(type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
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

EncryptionCertificateNameComponent::EncryptionCertificateNameComponent(
    bslma::Allocator* basicAllocator)
: d_attribute(basicAllocator)
, d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameComponent::EncryptionCertificateNameComponent(
    const EncryptionCertificateNameComponent& original,
    bslma::Allocator*                         basicAllocator)
: d_attribute(original.d_attribute, basicAllocator)
, d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameComponent::~EncryptionCertificateNameComponent()
{
}

EncryptionCertificateNameComponent& EncryptionCertificateNameComponent::
operator=(const EncryptionCertificateNameComponent& other)
{
    if (this != &other) {
        d_attribute = other.d_attribute;
        d_value     = other.d_value;
    }

    return *this;
}

void EncryptionCertificateNameComponent::reset()
{
    d_attribute.reset();
    d_value.clear();
}

void EncryptionCertificateNameComponent::setAttribute(
    const EncryptionCertificateNameAttribute& value)
{
    d_attribute = value;
}

void EncryptionCertificateNameComponent::setAttribute(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_attribute = value;
}

void EncryptionCertificateNameComponent::setAttribute(
    ntca::EncryptionCertificateNameAttributeType::Value value)
{
    d_attribute = value;
}

void EncryptionCertificateNameComponent::setValue(const bsl::string& value)
{
    d_value = value;
}

ntsa::Error EncryptionCertificateNameComponent::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    // AttributeTypeAndValue

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
        error = decoder->decodeTag(k_UNIVERSAL,
                                   k_PRIMITIVE,
                                   k_VISIBLE_STRING,
                                   k_PRINTABLE_STRING,
                                   k_UTF8_STRING);
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

ntsa::Error EncryptionCertificateNameComponent::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const EncryptionCertificateNameAttribute& EncryptionCertificateNameComponent::
    attribute() const
{
    return d_attribute;
}

const bsl::string& EncryptionCertificateNameComponent::value() const
{
    return d_value;
}

bool EncryptionCertificateNameComponent::equals(
    const EncryptionCertificateNameComponent& other) const
{
    return d_attribute == other.d_attribute && d_value == other.d_value;
}

bool EncryptionCertificateNameComponent::less(
    const EncryptionCertificateNameComponent& other) const
{
    if (d_attribute < other.d_attribute) {
        return true;
    }

    if (other.d_attribute < d_attribute) {
        return false;
    }

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateNameComponent::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute", d_attribute);
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                             stream,
                         const EncryptionCertificateNameComponent& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateNameComponent& lhs,
                const EncryptionCertificateNameComponent& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateNameComponent& lhs,
                const EncryptionCertificateNameComponent& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateNameComponent& lhs,
               const EncryptionCertificateNameComponent& rhs)
{
    return lhs.less(rhs);
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
    const bsl::vector<EncryptionCertificateNameComponent>& value)
{
    d_attributeVector = value;
}

void EncryptionCertificateName::append(
    const EncryptionCertificateNameComponent& value)
{
    d_attributeVector.push_back(value);
}

void EncryptionCertificateName::append(
    const EncryptionCertificateNameAttribute& attribute,
    const bsl::string&                        value)
{
    EncryptionCertificateNameComponent component;
    component.setAttribute(attribute);
    component.setValue(value);

    this->append(component);
}

void EncryptionCertificateName::append(
    const ntsa::AbstractObjectIdentifier& attribute,
    const bsl::string&                    value)
{
    EncryptionCertificateNameComponent component;
    component.setAttribute(attribute);
    component.setValue(value);

    this->append(component);
}

void EncryptionCertificateName::append(
    EncryptionCertificateNameAttributeType::Value attribute,
    const bsl::string&                            value)
{
    EncryptionCertificateNameComponent component;
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

        EncryptionCertificateNameComponent attribute;
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
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const bsl::vector<EncryptionCertificateNameComponent>& EncryptionCertificateName::
    attributeSequence() const
{
    return d_attributeVector;
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
    printer.printAttribute("attribute", d_attributeVector);
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

    if (decoder->current().tagNumber() == e_OTHER) {
        error = decoder->decodeValue(&this->makeOther());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_EMAIL) {
        error = decoder->decodeValue(&this->makeEmail());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_DOMAIN) {
        error = decoder->decodeValue(&this->makeDomain());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_X400) {
        error = decoder->decodeValue(&this->makeX400());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_DIRECTORY) {
        error = this->makeDirectory().decode(decoder);
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_EDI) {
        error = decoder->decodeValue(&this->makeEdi());
        if (error) {
            return error;
        }
    }
    else if (decoder->current().tagNumber() == e_URI) {
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
        ntsa::AbstractByteSequence octets;
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
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
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
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
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

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

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














const char* EncryptionCertificatePublicKeyAlgorithmIdentifierType::toString(Value value)
{
    switch (value) {
    case e_RSA:
        return "RSA";
    case e_ELLIPTIC_CURVE:
        return "ELLIPTIC_CURVE";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionCertificatePublicKeyAlgorithmIdentifierType::toObjectIdentifier(
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
}

int EncryptionCertificatePublicKeyAlgorithmIdentifierType::fromString(
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

    return -1;
}

int EncryptionCertificatePublicKeyAlgorithmIdentifierType::fromObjectIdentifier(
    Value*                                result,
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

    return -1;
}

bsl::ostream& EncryptionCertificatePublicKeyAlgorithmIdentifierType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                      stream,
    EncryptionCertificatePublicKeyAlgorithmIdentifierType::Value rhs)
{
    return EncryptionCertificatePublicKeyAlgorithmIdentifierType::print(stream, rhs);
}















EncryptionCertificatePublicKeyAlgorithmIdentifier::
    EncryptionCertificatePublicKeyAlgorithmIdentifier(bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyAlgorithmIdentifier::
    EncryptionCertificatePublicKeyAlgorithmIdentifier(
        const EncryptionCertificatePublicKeyAlgorithmIdentifier& original,
        bslma::Allocator*                              basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyAlgorithmIdentifier::
    ~EncryptionCertificatePublicKeyAlgorithmIdentifier()
{
}

EncryptionCertificatePublicKeyAlgorithmIdentifier& EncryptionCertificatePublicKeyAlgorithmIdentifier::
operator=(const EncryptionCertificatePublicKeyAlgorithmIdentifier& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificatePublicKeyAlgorithmIdentifier& EncryptionCertificatePublicKeyAlgorithmIdentifier::
operator=(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificatePublicKeyAlgorithmIdentifier& EncryptionCertificatePublicKeyAlgorithmIdentifier::
operator=(EncryptionCertificatePublicKeyAlgorithmIdentifierType::Value value)
{
    EncryptionCertificatePublicKeyAlgorithmIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
    return *this;
}

void EncryptionCertificatePublicKeyAlgorithmIdentifier::reset()
{
    d_identifier.reset();
}

void EncryptionCertificatePublicKeyAlgorithmIdentifier::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificatePublicKeyAlgorithmIdentifier::setIdentifer(
    EncryptionCertificatePublicKeyAlgorithmIdentifierType::Value value)
{
    EncryptionCertificatePublicKeyAlgorithmIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
}

ntsa::Error EncryptionCertificatePublicKeyAlgorithmIdentifier::decode(
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

ntsa::Error EncryptionCertificatePublicKeyAlgorithmIdentifier::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const ntsa::AbstractObjectIdentifier& EncryptionCertificatePublicKeyAlgorithmIdentifier::
    identifier() const
{
    return d_identifier;
}

bool EncryptionCertificatePublicKeyAlgorithmIdentifier::equals(
    const EncryptionCertificatePublicKeyAlgorithmIdentifier& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificatePublicKeyAlgorithmIdentifier::equals(
    EncryptionCertificatePublicKeyAlgorithmIdentifierType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificatePublicKeyAlgorithmIdentifierType::toObjectIdentifier(
        &identifier,
        value);

    return d_identifier.equals(identifier);
}

bool EncryptionCertificatePublicKeyAlgorithmIdentifier::less(
    const EncryptionCertificatePublicKeyAlgorithmIdentifier& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificatePublicKeyAlgorithmIdentifier::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionCertificatePublicKeyAlgorithmIdentifierType::Value type;
    int rc = ntca::EncryptionCertificatePublicKeyAlgorithmIdentifierType::
        fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificatePublicKeyAlgorithmIdentifierType::toString(
            type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                                  stream,
                         const EncryptionCertificatePublicKeyAlgorithmIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePublicKeyAlgorithmIdentifier& lhs,
                const EncryptionCertificatePublicKeyAlgorithmIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePublicKeyAlgorithmIdentifier& lhs,
                const EncryptionCertificatePublicKeyAlgorithmIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePublicKeyAlgorithmIdentifier& lhs,
               const EncryptionCertificatePublicKeyAlgorithmIdentifier& rhs)
{
    return lhs.less(rhs);
}






























EncryptionCertificatePublicKeyAlgorithmParametersRsa::EncryptionCertificatePublicKeyAlgorithmParametersRsa(
    bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTCCFG_WARNING_UNUSED(d_allocator_p);
}

EncryptionCertificatePublicKeyAlgorithmParametersRsa::EncryptionCertificatePublicKeyAlgorithmParametersRsa(
    const EncryptionCertificatePublicKeyAlgorithmParametersRsa& original,
    bslma::Allocator*                    basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTCCFG_WARNING_UNUSED(original);
    NTCCFG_WARNING_UNUSED(d_allocator_p);
}

EncryptionCertificatePublicKeyAlgorithmParametersRsa::~EncryptionCertificatePublicKeyAlgorithmParametersRsa()
{
}

EncryptionCertificatePublicKeyAlgorithmParametersRsa& EncryptionCertificatePublicKeyAlgorithmParametersRsa::operator=(
    const EncryptionCertificatePublicKeyAlgorithmParametersRsa& other)
{
    NTCCFG_WARNING_UNUSED(other);
    return *this;
}

void EncryptionCertificatePublicKeyAlgorithmParametersRsa::reset()
{
}

ntsa::Error EncryptionCertificatePublicKeyAlgorithmParametersRsa::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    // Decode null.
    
    NTCCFG_WARNING_UNUSED(decoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificatePublicKeyAlgorithmParametersRsa::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    // Encode null.

    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

bool EncryptionCertificatePublicKeyAlgorithmParametersRsa::equals(
    const EncryptionCertificatePublicKeyAlgorithmParametersRsa& other) const
{
    NTCCFG_WARNING_UNUSED(other);
    return true;
}

bool EncryptionCertificatePublicKeyAlgorithmParametersRsa::less(
    const EncryptionCertificatePublicKeyAlgorithmParametersRsa& other) const
{
    NTCCFG_WARNING_UNUSED(other);
    return false;
}

bsl::ostream& EncryptionCertificatePublicKeyAlgorithmParametersRsa::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificatePublicKeyAlgorithmParametersRsa& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePublicKeyAlgorithmParametersRsa& lhs,
                const EncryptionCertificatePublicKeyAlgorithmParametersRsa& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePublicKeyAlgorithmParametersRsa& lhs,
                const EncryptionCertificatePublicKeyAlgorithmParametersRsa& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePublicKeyAlgorithmParametersRsa& lhs,
               const EncryptionCertificatePublicKeyAlgorithmParametersRsa& rhs)
{
    return lhs.less(rhs);
}















const char* EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::toString(Value value)
{
    switch (value) {
    case e_SECP_256_R1:
        return "SECP_256_R1";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result,
    Value                           value)
{
    result->reset();

    if (value == e_SECP_256_R1) {
        result->set(1, 2, 840, 10045, 3, 1, 7);
    }
}

int EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::fromString(
    Value*                   result,
    const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "SECP_256_R1")) {
        *result = e_SECP_256_R1;
        return 0;
    }

    return -1;
}

int EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::fromObjectIdentifier(
    Value*                                result,
    const ntsa::AbstractObjectIdentifier& identifier)
{
    if (identifier.equals(1, 2, 840, 10045, 3, 1, 7)) {
        *result = e_SECP_256_R1;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                      stream,
    EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::Value rhs)
{
    return EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::print(stream, rhs);
}





























EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::
    EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier(bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::
    EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier(
        const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& original,
        bslma::Allocator*                              basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::
    ~EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier()
{
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::
operator=(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::
operator=(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::
operator=(EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::Value value)
{
    EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
    return *this;
}

void EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::reset()
{
    d_identifier.reset();
}

void EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::setIdentifer(
    EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::Value value)
{
    EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::toObjectIdentifier(
        &d_identifier,
        value);
}

ntsa::Error EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::decode(
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

ntsa::Error EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const ntsa::AbstractObjectIdentifier& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::
    identifier() const
{
    return d_identifier;
}

bool EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::equals(
    const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::equals(
    EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::toObjectIdentifier(
        &identifier,
        value);

    return d_identifier.equals(identifier);
}

bool EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::less(
    const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::Value type;
    int rc = ntca::EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::
        fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifierType::toString(
            type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                                  stream,
                         const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& lhs,
                const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& lhs,
                const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& lhs,
               const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& rhs)
{
    return lhs.less(rhs);
}























EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve(
    const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& original,
    bslma::Allocator*                          basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_IDENTIFIER) {
        new (d_identifier.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier(
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

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::~EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve()
{
    this->reset();
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::
operator=(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& other)
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

void EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::reset()
{
    if (d_type == e_IDENTIFIER) {
        typedef EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier Type;
        d_identifier.object().~Type();
    }
    else if (d_type == e_ANY) {
        typedef ntsa::AbstractValue Type;
        d_any.object().~Type();
    }

    d_type = e_UNDEFINED;
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::
    makeIdentifier()
{
    if (d_type == e_IDENTIFIER) {
        d_identifier.object().reset();
    }
    else {
        this->reset();
        new (d_identifier.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier(d_allocator_p);
        d_type = e_IDENTIFIER;
    }

    return d_identifier.object();
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::
    makeIdentifier(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& value)
{
    if (d_type == e_IDENTIFIER) {
        d_identifier.object() = value;
    }
    else {
        this->reset();
        new (d_identifier.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier(value, d_allocator_p);
        d_type = e_IDENTIFIER;
    }

    return d_identifier.object();
}

ntsa::AbstractValue& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::makeAny()
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

ntsa::AbstractValue& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::makeAny(
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

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::
    identifier()
{
    BSLS_ASSERT(this->isIdentifier());
    return d_identifier.object();
}

ntsa::AbstractValue& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::any()
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurveIdentifier&
EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::identifier() const
{
    BSLS_ASSERT(this->isIdentifier());
    return d_identifier.object();
}

const ntsa::AbstractValue& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::any() const
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

bool EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::isIdentifier() const
{
    return d_type == e_IDENTIFIER;
}

bool EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::isAny() const
{
    return d_type == e_ANY;
}

bool EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::equals(
    const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& other) const
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

bool EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::less(
    const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& other) const
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

bsl::ostream& EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
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

bsl::ostream& operator<<(bsl::ostream&                              stream,
                         const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& lhs,
                const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& lhs,
                const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& lhs,
               const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& rhs)
{
    return lhs.less(rhs);
}




































EncryptionCertificatePublicKeyAlgorithmParameters::EncryptionCertificatePublicKeyAlgorithmParameters(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyAlgorithmParameters::EncryptionCertificatePublicKeyAlgorithmParameters(
    const EncryptionCertificatePublicKeyAlgorithmParameters& original,
    bslma::Allocator*                          basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_RSA) {
        new (d_rsa.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersRsa(
                original.d_rsa.object(),
                d_allocator_p);
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        new (d_ellipticCurve.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve(
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

EncryptionCertificatePublicKeyAlgorithmParameters::~EncryptionCertificatePublicKeyAlgorithmParameters()
{
    this->reset();
}

EncryptionCertificatePublicKeyAlgorithmParameters& EncryptionCertificatePublicKeyAlgorithmParameters::
operator=(const EncryptionCertificatePublicKeyAlgorithmParameters& other)
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

void EncryptionCertificatePublicKeyAlgorithmParameters::reset()
{
    if (d_type == e_RSA) {
        typedef EncryptionCertificatePublicKeyAlgorithmParametersRsa Type;
        d_rsa.object().~Type();
    }
    else if (d_type == e_ELLIPTIC_CURVE) {
        typedef EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve Type;
        d_ellipticCurve.object().~Type();
    }
    else if (d_type == e_ANY) {
        typedef ntsa::AbstractValue Type;
        d_any.object().~Type();
    }

    d_type = e_UNDEFINED;
}





EncryptionCertificatePublicKeyAlgorithmParametersRsa& EncryptionCertificatePublicKeyAlgorithmParameters::
    makeRsa()
{
    if (d_type == e_RSA) {
        d_rsa.object().reset();
    }
    else {
        this->reset();
        new (d_rsa.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersRsa(d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}

EncryptionCertificatePublicKeyAlgorithmParametersRsa& EncryptionCertificatePublicKeyAlgorithmParameters::
    makeRsa(const EncryptionCertificatePublicKeyAlgorithmParametersRsa& value)
{
    if (d_type == e_RSA) {
        d_rsa.object() = value;
    }
    else {
        this->reset();
        new (d_rsa.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersRsa(value, d_allocator_p);
        d_type = e_RSA;
    }

    return d_rsa.object();
}






EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& EncryptionCertificatePublicKeyAlgorithmParameters::
    makeEllipticCurve()
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object().reset();
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve(d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& EncryptionCertificatePublicKeyAlgorithmParameters::
    makeEllipticCurve(const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& value)
{
    if (d_type == e_ELLIPTIC_CURVE) {
        d_ellipticCurve.object() = value;
    }
    else {
        this->reset();
        new (d_ellipticCurve.buffer())
            EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve(value, d_allocator_p);
        d_type = e_ELLIPTIC_CURVE;
    }

    return d_ellipticCurve.object();
}

ntsa::AbstractValue& EncryptionCertificatePublicKeyAlgorithmParameters::makeAny()
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

ntsa::AbstractValue& EncryptionCertificatePublicKeyAlgorithmParameters::makeAny(
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

EncryptionCertificatePublicKeyAlgorithmParametersRsa& EncryptionCertificatePublicKeyAlgorithmParameters::
    rsa()
{
    BSLS_ASSERT(this->isRsa());
    return d_rsa.object();
}


EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve& EncryptionCertificatePublicKeyAlgorithmParameters::
    ellipticCurve()
{
    BSLS_ASSERT(this->isEllipticCurve());
    return d_ellipticCurve.object();
}

ntsa::AbstractValue& EncryptionCertificatePublicKeyAlgorithmParameters::any()
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

const EncryptionCertificatePublicKeyAlgorithmParametersRsa&
EncryptionCertificatePublicKeyAlgorithmParameters::rsa() const
{
    BSLS_ASSERT(this->isRsa());
    return d_rsa.object();
}

const EncryptionCertificatePublicKeyAlgorithmParametersEllipticCurve&
EncryptionCertificatePublicKeyAlgorithmParameters::ellipticCurve() const
{
    BSLS_ASSERT(this->isEllipticCurve());
    return d_ellipticCurve.object();
}

const ntsa::AbstractValue& EncryptionCertificatePublicKeyAlgorithmParameters::any() const
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
}

bool EncryptionCertificatePublicKeyAlgorithmParameters::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionCertificatePublicKeyAlgorithmParameters::isRsa() const
{
    return d_type == e_RSA;
}

bool EncryptionCertificatePublicKeyAlgorithmParameters::isEllipticCurve() const
{
    return d_type == e_ELLIPTIC_CURVE;
}

bool EncryptionCertificatePublicKeyAlgorithmParameters::isAny() const
{
    return d_type == e_ANY;
}

bool EncryptionCertificatePublicKeyAlgorithmParameters::equals(
    const EncryptionCertificatePublicKeyAlgorithmParameters& other) const
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

bool EncryptionCertificatePublicKeyAlgorithmParameters::less(
    const EncryptionCertificatePublicKeyAlgorithmParameters& other) const
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

bsl::ostream& EncryptionCertificatePublicKeyAlgorithmParameters::print(
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

bsl::ostream& operator<<(bsl::ostream&                              stream,
                         const EncryptionCertificatePublicKeyAlgorithmParameters& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePublicKeyAlgorithmParameters& lhs,
                const EncryptionCertificatePublicKeyAlgorithmParameters& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePublicKeyAlgorithmParameters& lhs,
                const EncryptionCertificatePublicKeyAlgorithmParameters& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePublicKeyAlgorithmParameters& lhs,
               const EncryptionCertificatePublicKeyAlgorithmParameters& rhs)
{
    return lhs.less(rhs);
}























EncryptionCertificatePublicKeyAlgorithm::EncryptionCertificatePublicKeyAlgorithm(
    bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_parameters(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyAlgorithm::EncryptionCertificatePublicKeyAlgorithm(
    const EncryptionCertificatePublicKeyAlgorithm& original,
    bslma::Allocator*                    basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_parameters(original.d_parameters, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyAlgorithm::~EncryptionCertificatePublicKeyAlgorithm()
{
}

EncryptionCertificatePublicKeyAlgorithm& EncryptionCertificatePublicKeyAlgorithm::operator=(
    const EncryptionCertificatePublicKeyAlgorithm& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
        d_parameters = other.d_parameters;
    }

    return *this;
}

void EncryptionCertificatePublicKeyAlgorithm::reset()
{
    d_identifier.reset();
    d_parameters.reset();
}


void EncryptionCertificatePublicKeyAlgorithm::setIdentifier(
    const EncryptionCertificatePublicKeyAlgorithmIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificatePublicKeyAlgorithm::setParameters(
    const EncryptionCertificatePublicKeyAlgorithmParameters& value)
{
    d_parameters = value;
}

ntsa::Error EncryptionCertificatePublicKeyAlgorithm::decode(
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
            EncryptionCertificatePublicKeyAlgorithmIdentifierType::e_RSA)) 
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
            }
            else {
                error = decoder->decodeValue(&d_parameters.makeAny());
                if (error) {
                    return error;
                }
            }

            error = decoder->decodeTagComplete();
            if (error) {
                return error;
            }
        }
        else if (d_identifier.equals(
            EncryptionCertificatePublicKeyAlgorithmIdentifierType::e_ELLIPTIC_CURVE))
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

                error = 
                    d_parameters.makeEllipticCurve().makeIdentifier().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else {
                error = decoder->decodeValue(
                    &d_parameters.makeEllipticCurve().makeAny());
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
                error = decoder->decodeValue(&d_parameters.makeAny());
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

ntsa::Error EncryptionCertificatePublicKeyAlgorithm::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const EncryptionCertificatePublicKeyAlgorithmIdentifier& EncryptionCertificatePublicKeyAlgorithm::identifier() const
{
    return d_identifier;
}

const EncryptionCertificatePublicKeyAlgorithmParameters& EncryptionCertificatePublicKeyAlgorithm::parameters() const
{
    return d_parameters;
}


bool EncryptionCertificatePublicKeyAlgorithm::equals(
    const EncryptionCertificatePublicKeyAlgorithm& other) const
{
    return d_identifier == other.d_identifier && d_parameters == other.d_parameters;
}

bool EncryptionCertificatePublicKeyAlgorithm::less(
    const EncryptionCertificatePublicKeyAlgorithm& other) const
{
    if (d_identifier < other.d_identifier) {
        return true;
    }

    if (other.d_identifier < d_identifier) {
        return false;
    }

    return d_parameters < other.d_parameters;
}

bsl::ostream& EncryptionCertificatePublicKeyAlgorithm::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("identifier", d_identifier);
    printer.printAttribute("parameters", d_parameters);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const EncryptionCertificatePublicKeyAlgorithm& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificatePublicKeyAlgorithm& lhs,
                const EncryptionCertificatePublicKeyAlgorithm& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificatePublicKeyAlgorithm& lhs,
                const EncryptionCertificatePublicKeyAlgorithm& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificatePublicKeyAlgorithm& lhs,
               const EncryptionCertificatePublicKeyAlgorithm& rhs)
{
    return lhs.less(rhs);
}












EncryptionCertificatePublicKeyInfo::EncryptionCertificatePublicKeyInfo(
    bslma::Allocator* basicAllocator)
: d_algorithm(basicAllocator)
, d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyInfo::EncryptionCertificatePublicKeyInfo(
    const EncryptionCertificatePublicKeyInfo& original,
    bslma::Allocator*                         basicAllocator)
: d_algorithm(original.d_algorithm, basicAllocator)
, d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyInfo::~EncryptionCertificatePublicKeyInfo()
{
}

EncryptionCertificatePublicKeyInfo& EncryptionCertificatePublicKeyInfo::
operator=(const EncryptionCertificatePublicKeyInfo& other)
{
    if (this != &other) {
        d_algorithm = other.d_algorithm;
        d_value  = other.d_value;
    }

    return *this;
}

void EncryptionCertificatePublicKeyInfo::reset()
{
    d_algorithm.reset();
    d_value.reset();
}

ntsa::Error EncryptionCertificatePublicKeyInfo::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    {
        error = d_algorithm.decode(decoder);
        if (error) {
            return error;
        }
    }

    {
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
    }

    error = decoder->decodeTagComplete();
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

bool EncryptionCertificatePublicKeyInfo::equals(
    const EncryptionCertificatePublicKeyInfo& other) const
{
    return d_algorithm == other.d_algorithm &&
           d_value == other.d_value;
}

bool EncryptionCertificatePublicKeyInfo::less(
    const EncryptionCertificatePublicKeyInfo& other) const
{
    if (d_algorithm < other.d_algorithm) {
        return true;
    }

    if (other.d_algorithm < d_algorithm) {
        return false;
    }

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificatePublicKeyInfo::print(
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

const char* EncryptionCertificateSignatureAlgorithmType::toString(Value value)
{
    switch (value) {
    case e_ECDSA_SHA1:
        return "ECDSA-SHA1";
    case e_ECDSA_SHA224:
        return "ECDSA-SHA224";
    case e_ECDSA_SHA256:
        return "ECDSA-SHA256";
    case e_ECDSA_SHA384:
        return "ECDSA-SHA384";
    case e_ECDSA_SHA512:
        return "ECDSA-SHA512";
    case e_RSA:
        return "RSA";
    case e_SHA256_RSA:
        return "SHA256-RSA";
    case e_SHA384_RSA:
        return "SHA384-RSA";
    case e_SHA512_RSA:
        return "SHA512-RSA";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionCertificateSignatureAlgorithmType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result,
    Value                           value)
{
    result->reset();

    if (value == e_ECDSA_SHA1) {
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
    else if (value == e_RSA) {
        result->set(1, 2, 840, 113549, 1, 1, 1);
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
}

int EncryptionCertificateSignatureAlgorithmType::fromString(
    Value*                   result,
    const bslstl::StringRef& string)
{
    if (bdlb::String::areEqualCaseless(string, "ECDSA-SHA1")) {
        *result = e_ECDSA_SHA1;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA-SHA224")) {
        *result = e_ECDSA_SHA224;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA-SHA256")) {
        *result = e_ECDSA_SHA256;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA-SHA384")) {
        *result = e_ECDSA_SHA384;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "ECDSA-SHA512")) {
        *result = e_ECDSA_SHA512;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "RSA")) {
        *result = e_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SHA256-RSA")) {
        *result = e_SHA256_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SHA384-RSA")) {
        *result = e_SHA384_RSA;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SHA512-RSA")) {
        *result = e_SHA512_RSA;
        return 0;
    }

    return -1;
}

int EncryptionCertificateSignatureAlgorithmType::fromObjectIdentifier(
    Value*                                result,
    const ntsa::AbstractObjectIdentifier& identifier)
{
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

    if (identifier.equals(1, 2, 840, 113549, 1, 1, 1)) {
        *result = e_RSA;
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

    return -1;
}

bsl::ostream& EncryptionCertificateSignatureAlgorithmType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                      stream,
    EncryptionCertificateSignatureAlgorithmType::Value rhs)
{
    return EncryptionCertificateSignatureAlgorithmType::print(stream, rhs);
}

EncryptionCertificateSignatureAlgorithm::
    EncryptionCertificateSignatureAlgorithm(bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithm::
    EncryptionCertificateSignatureAlgorithm(
        const EncryptionCertificateSignatureAlgorithm& original,
        bslma::Allocator*                              basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
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
    }

    return *this;
}

EncryptionCertificateSignatureAlgorithm& EncryptionCertificateSignatureAlgorithm::
operator=(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateSignatureAlgorithm& EncryptionCertificateSignatureAlgorithm::
operator=(EncryptionCertificateSignatureAlgorithmType::Value value)
{
    EncryptionCertificateSignatureAlgorithmType::toObjectIdentifier(
        &d_identifier,
        value);
    return *this;
}

void EncryptionCertificateSignatureAlgorithm::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateSignatureAlgorithm::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificateSignatureAlgorithm::setIdentifer(
    EncryptionCertificateSignatureAlgorithmType::Value value)
{
    EncryptionCertificateSignatureAlgorithmType::toObjectIdentifier(
        &d_identifier,
        value);
}

ntsa::Error EncryptionCertificateSignatureAlgorithm::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

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

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateSignatureAlgorithm::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const ntsa::AbstractObjectIdentifier& EncryptionCertificateSignatureAlgorithm::
    identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateSignatureAlgorithm::equals(
    const EncryptionCertificateSignatureAlgorithm& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateSignatureAlgorithm::equals(
    EncryptionCertificateSignatureAlgorithmType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateSignatureAlgorithmType::toObjectIdentifier(
        &identifier,
        value);

    return d_identifier.equals(identifier);
}

bool EncryptionCertificateSignatureAlgorithm::less(
    const EncryptionCertificateSignatureAlgorithm& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateSignatureAlgorithm::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionCertificateSignatureAlgorithmType::Value type;
    int rc = ntca::EncryptionCertificateSignatureAlgorithmType::
        fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateSignatureAlgorithmType::toString(
            type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

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
    const ntsa::AbstractBitSequence& value)
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

const char* EncryptionCertificateExtensionAttributeType::toString(Value value)
{
    switch (value) {
    case e_BASIC_CONSTRAINTS:
        return "BASIC_CONSTRAINTS";
    case e_KEY_USAGE:
        return "KEY_USAGE";
    case e_KEY_USAGE_EXTENDED:
        return "KEY_USAGE_EXTENDED";
    case e_SUBJECT_KEY_IDENTIFIER:
        return "SUBJECT_KEY_IDENTIFIER";
    case e_SUBJECT_ALTERNATIVE_NAME:
        return "SUBJECT_ALTERNATIVE_NAME";
    case e_AUTHORITY_KEY_IDENTIFIER:
        return "AUTHORITY_KEY_IDENTIFIER";
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

void EncryptionCertificateExtensionAttributeType::toObjectIdentifier(
    ntsa::AbstractObjectIdentifier* result,
    Value                           value)
{
    result->reset();

    if (value == e_BASIC_CONSTRAINTS) {
        result->set(2, 5, 29, 19);
    }
    else if (value == e_KEY_USAGE) {
        result->set(2, 5, 29, 15);
    }
    else if (value == e_KEY_USAGE_EXTENDED) {
        result->set(2, 5, 29, 37);
    }
    else if (value == e_SUBJECT_KEY_IDENTIFIER) {
        result->set(2, 5, 29, 14);
    }
    else if (value == e_SUBJECT_ALTERNATIVE_NAME) {
        result->set(2, 5, 29, 17);
    }
    else if (value == e_AUTHORITY_KEY_IDENTIFIER) {
        result->set(2, 5, 29, 35);
    }
}

int EncryptionCertificateExtensionAttributeType::fromString(
    EncryptionCertificateExtensionAttributeType::Value* result,
    const bslstl::StringRef&                            string)
{
    if (bdlb::String::areEqualCaseless(string, "BASIC_CONSTRAINTS")) {
        *result = e_BASIC_CONSTRAINTS;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "KEY_USAGE")) {
        *result = e_KEY_USAGE;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "KEY_USAGE_EXTENDED")) {
        *result = e_KEY_USAGE_EXTENDED;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SUBJECT_KEY_IDENTIFIER")) {
        *result = e_SUBJECT_KEY_IDENTIFIER;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "SUBJECT_ALTERNATIVE_NAME")) {
        *result = e_SUBJECT_ALTERNATIVE_NAME;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "AUTHORITY_KEY_IDENTIFIER")) {
        *result = e_AUTHORITY_KEY_IDENTIFIER;
        return 0;
    }

    return -1;
}

int EncryptionCertificateExtensionAttributeType::fromObjectIdentifier(
    Value*                                result,
    const ntsa::AbstractObjectIdentifier& identifier)
{
    if (identifier.equals(2, 5, 29, 19)) {
        *result = e_BASIC_CONSTRAINTS;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 15)) {
        *result = e_KEY_USAGE;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 37)) {
        *result = e_KEY_USAGE_EXTENDED;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 14)) {
        *result = e_SUBJECT_KEY_IDENTIFIER;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 17)) {
        *result = e_SUBJECT_ALTERNATIVE_NAME;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 35)) {
        *result = e_AUTHORITY_KEY_IDENTIFIER;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionCertificateExtensionAttributeType::print(
    bsl::ostream& stream,
    Value         value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(
    bsl::ostream&                                      stream,
    EncryptionCertificateExtensionAttributeType::Value rhs)
{
    return EncryptionCertificateExtensionAttributeType::print(stream, rhs);
}

EncryptionCertificateExtensionAttribute::
    EncryptionCertificateExtensionAttribute(bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionAttribute::
    EncryptionCertificateExtensionAttribute(
        const EncryptionCertificateExtensionAttribute& original,
        bslma::Allocator*                              basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionAttribute::
    ~EncryptionCertificateExtensionAttribute()
{
}

EncryptionCertificateExtensionAttribute& EncryptionCertificateExtensionAttribute::
operator=(const EncryptionCertificateExtensionAttribute& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificateExtensionAttribute& EncryptionCertificateExtensionAttribute::
operator=(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateExtensionAttribute& EncryptionCertificateExtensionAttribute::
operator=(EncryptionCertificateExtensionAttributeType::Value value)
{
    EncryptionCertificateExtensionAttributeType::toObjectIdentifier(
        &d_identifier,
        value);
    return *this;
}

void EncryptionCertificateExtensionAttribute::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateExtensionAttribute::setIdentifier(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}

void EncryptionCertificateExtensionAttribute::setIdentifer(
    EncryptionCertificateExtensionAttributeType::Value value)
{
    EncryptionCertificateExtensionAttributeType::toObjectIdentifier(
        &d_identifier,
        value);
}

ntsa::Error EncryptionCertificateExtensionAttribute::decode(
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

ntsa::Error EncryptionCertificateExtensionAttribute::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const ntsa::AbstractObjectIdentifier& EncryptionCertificateExtensionAttribute::
    identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateExtensionAttribute::equals(
    const EncryptionCertificateExtensionAttribute& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateExtensionAttribute::equals(
    EncryptionCertificateExtensionAttributeType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateExtensionAttributeType::toObjectIdentifier(
        &identifier,
        value);

    return d_identifier.equals(identifier);
}

bool EncryptionCertificateExtensionAttribute::less(
    const EncryptionCertificateExtensionAttribute& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateExtensionAttribute::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    ntca::EncryptionCertificateExtensionAttributeType::Value type;
    int rc = ntca::EncryptionCertificateExtensionAttributeType::
        fromObjectIdentifier(&type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateExtensionAttributeType::toString(
            type);
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                                  stream,
                         const EncryptionCertificateExtensionAttribute& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateExtensionAttribute& lhs,
                const EncryptionCertificateExtensionAttribute& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateExtensionAttribute& lhs,
                const EncryptionCertificateExtensionAttribute& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateExtensionAttribute& lhs,
               const EncryptionCertificateExtensionAttribute& rhs)
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
    else if (d_type == e_NAME_ALTERNATIVE) {
        new (d_nameAlternative.buffer())
            EncryptionCertificateNameAlternativeList(
                original.d_nameAlternative.object(),
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
    else if (other.d_type == e_NAME_ALTERNATIVE) {
        this->makeNameAlternative(other.d_nameAlternative.object());
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
    else if (d_type == e_NAME_ALTERNATIVE) {
        typedef EncryptionCertificateNameAlternativeList Type;
        d_nameAlternative.object().~Type();
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

EncryptionCertificateNameAlternativeList& EncryptionCertificateExtensionValue::
    nameAlternative()
{
    BSLS_ASSERT(this->isNameAlternative());
    return d_nameAlternative.object();
}

ntsa::AbstractValue& EncryptionCertificateExtensionValue::any()
{
    BSLS_ASSERT(this->isAny());
    return d_any.object();
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

bool EncryptionCertificateExtensionValue::boolean() const
{
    BSLS_ASSERT(this->isBoolean());
    return d_boolean.object();
}

const EncryptionCertificateNameAlternativeList&
EncryptionCertificateExtensionValue::nameAlternative() const
{
    BSLS_ASSERT(this->isNameAlternative());
    return d_nameAlternative.object();
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

bool EncryptionCertificateExtensionValue::isNameAlternative() const
{
    return d_type == e_NAME_ALTERNATIVE;
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
    else if (d_type == e_NAME_ALTERNATIVE) {
        return d_nameAlternative.object() == other.d_nameAlternative.object();
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
    else if (d_type == e_NAME_ALTERNATIVE) {
        return d_nameAlternative.object() < other.d_nameAlternative.object();
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
    else if (d_type == e_NAME_ALTERNATIVE) {
        stream << d_nameAlternative.object();
    }
    else if (d_type == e_ANY) {
        stream << d_any.object();
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
    const EncryptionCertificateExtensionAttribute& value)
{
    d_attribute = value;
}

void EncryptionCertificateExtension::setAttribute(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_attribute = value;
}

void EncryptionCertificateExtension::setAttribute(
    ntca::EncryptionCertificateExtensionAttributeType::Value value)
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
                    EncryptionCertificateExtensionAttributeType::
                        e_SUBJECT_ALTERNATIVE_NAME))
            {
                error = d_value.makeNameAlternative().decode(decoder);
                if (error) {
                    return error;
                }
            }
            else {
                error = decoder->decodeValue(&d_value.makeAny());
                if (error) {
                    return error;
                }
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

ntsa::Error EncryptionCertificateExtension::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const EncryptionCertificateExtensionAttribute& EncryptionCertificateExtension::
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
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
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

ntsa::Error EncryptionCertificateEntity::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    // Enter TBSCertificate

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    // Decode version.

    error = d_version.decode(decoder);
    if (error) {
        return error;
    }

    // Enter TBSCertificate -> serialNumber

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_INTEGER);
    if (error) {
        return error;
    }

    error = decoder->decodeValue(&d_serialNumber);
    if (error) {
        return error;
    }

    // Leave TBSCertificate -> serialNumber

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

    // Leave TBSCertificate

    error = decoder->decodeTagComplete();
    if (error) {
        return error;
    }

    return ntsa::Error();
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
