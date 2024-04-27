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

// MRM
#if 0
const ntsa::AbstractSyntaxTagNumber::Value k_BOOLEAN = 
    ntsa::AbstractSyntaxTagNumber::e_BOOLEAN;
#endif

const ntsa::AbstractSyntaxTagNumber::Value k_INTEGER = 
    ntsa::AbstractSyntaxTagNumber::e_INTEGER;

const ntsa::AbstractSyntaxTagNumber::Value k_UTF8_STRING =
    ntsa::AbstractSyntaxTagNumber::e_UTF8_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_BIT_STRING = 
    ntsa::AbstractSyntaxTagNumber::e_BIT_STRING;

const ntsa::AbstractSyntaxTagNumber::Value k_OCTET_STRING = 
    ntsa::AbstractSyntaxTagNumber::e_OCTET_STRING;

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

int EncryptionCertificateNameAttributeType::fromString(EncryptionCertificateNameAttributeType::Value* result,
                                   const bslstl::StringRef&   string)
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

bsl::ostream& EncryptionCertificateNameAttributeType::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                 stream,
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
    bslma::Allocator*                    basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
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
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificateNameAttribute& EncryptionCertificateNameAttribute::operator=(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateNameAttribute& EncryptionCertificateNameAttribute::operator=(
    EncryptionCertificateNameAttributeType::Value value)
{
    EncryptionCertificateNameAttributeType::toObjectIdentifier(&d_identifier, value);
    return *this;
}

void EncryptionCertificateNameAttribute::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateNameAttribute::setIdentifier(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}


void EncryptionCertificateNameAttribute::setIdentifer(EncryptionCertificateNameAttributeType::Value value)
{
    EncryptionCertificateNameAttributeType::toObjectIdentifier(&d_identifier, value);
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

const ntsa::AbstractObjectIdentifier& EncryptionCertificateNameAttribute::identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateNameAttribute::equals(
    const EncryptionCertificateNameAttribute& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateNameAttribute::equals(EncryptionCertificateNameAttributeType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateNameAttributeType::toObjectIdentifier(
        &identifier, value);

    return d_identifier.equals(identifier);
    
}

bool EncryptionCertificateNameAttribute::less(
    const EncryptionCertificateNameAttribute& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateNameAttribute::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    ntca::EncryptionCertificateNameAttributeType::Value type;
    int rc = ntca::EncryptionCertificateNameAttributeType::fromObjectIdentifier(
        &type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateNameAttributeType::toString(type); 
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

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






























EncryptionCertificateNameComponent::EncryptionCertificateNameComponent(
    bslma::Allocator* basicAllocator)
: d_attribute(basicAllocator)
, d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameComponent::EncryptionCertificateNameComponent(
    const EncryptionCertificateNameComponent& original,
    bslma::Allocator*                    basicAllocator)
: d_attribute(original.d_attribute, basicAllocator)
, d_value(original.d_value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateNameComponent::~EncryptionCertificateNameComponent()
{
}

EncryptionCertificateNameComponent& EncryptionCertificateNameComponent::operator=(
    const EncryptionCertificateNameComponent& other)
{
    if (this != &other) {
        d_attribute = other.d_attribute;
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateNameComponent::reset()
{
    d_attribute.reset();
    d_value.clear();
}

void EncryptionCertificateNameComponent::setAttribute(const EncryptionCertificateNameAttribute& value)
{
    d_attribute = value;
}

void EncryptionCertificateNameComponent::setAttribute(const ntsa::AbstractObjectIdentifier& value)
{
    d_attribute = value;
}

void EncryptionCertificateNameComponent::setAttribute(ntca::EncryptionCertificateNameAttributeType::Value value)
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
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_UTF8_STRING);
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

const EncryptionCertificateNameAttribute& 
EncryptionCertificateNameComponent::attribute() const
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

bsl::ostream& EncryptionCertificateNameComponent::print(bsl::ostream& stream,
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

bsl::ostream& operator<<(bsl::ostream&                        stream,
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
    bslma::Allocator*                    basicAllocator)
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
    // MRM
    #if 0
    subject Name SEQUENCE (5 elem)
      RelativeDistinguishedName SET (1 elem)
        AttributeTypeAndValue SEQUENCE (2 elem)
          type AttributeType OBJECT IDENTIFIER 2.5.4.3 commonName (X.520 DN component)
          value AttributeValue [?] UTF8String TEST.USER
      RelativeDistinguishedName SET (1 elem)
        AttributeTypeAndValue SEQUENCE (2 elem)
          type AttributeType OBJECT IDENTIFIER 2.5.4.10 organizationName (X.520 DN component)
          value AttributeValue [?] UTF8String Bloomberg LP
      RelativeDistinguishedName SET (1 elem)
        AttributeTypeAndValue SEQUENCE (2 elem)
          type AttributeType OBJECT IDENTIFIER 2.5.4.11 organizationalUnitName (X.520 DN component)
          value AttributeValue [?] UTF8String Engineering
      RelativeDistinguishedName SET (1 elem)
        AttributeTypeAndValue SEQUENCE (2 elem)
          type AttributeType OBJECT IDENTIFIER 2.5.4.11 organizationalUnitName (X.520 DN component)
          value AttributeValue [?] UTF8String Platform Services
      RelativeDistinguishedName SET (1 elem)
        AttributeTypeAndValue SEQUENCE (2 elem)
          type AttributeType OBJECT IDENTIFIER 2.5.4.11 organizationalUnitName (X.520 DN component)
          value AttributeValue [?] UTF8String Middleware Transport
    #endif

    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    if (decoder->current().contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t contentBegin = decoder->current().contentPosition();

    const bsl::size_t contentEnd = 
        contentBegin + decoder->current().contentLength().value();

    while (decoder->position() < contentEnd) {
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

const bsl::vector<EncryptionCertificateNameComponent>& EncryptionCertificateName::attributeSequence() const
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


















EncryptionCertificatePublicKeyInfo::EncryptionCertificatePublicKeyInfo(
    bslma::Allocator* basicAllocator)
: d_publicKeyFamily(basicAllocator)
, d_publicKeyType(basicAllocator)
, d_publicKeyValue(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificatePublicKeyInfo::EncryptionCertificatePublicKeyInfo(
    const EncryptionCertificatePublicKeyInfo& original,
    bslma::Allocator*                    basicAllocator)
: d_publicKeyFamily(original.d_publicKeyFamily, basicAllocator)
, d_publicKeyType(original.d_publicKeyType, basicAllocator)
, d_publicKeyValue(original.d_publicKeyValue, basicAllocator)
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
        d_publicKeyFamily = other.d_publicKeyFamily;
        d_publicKeyType = other.d_publicKeyType;
        d_publicKeyValue = other.d_publicKeyValue;
    }

    return *this;
}

void EncryptionCertificatePublicKeyInfo::reset()
{
    d_publicKeyFamily.reset();
    d_publicKeyType.reset();
    d_publicKeyValue.reset();
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
        error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
        if (error) {
            return error;
        }

        {
            error = decoder->decodeTag(k_UNIVERSAL, 
                                       k_PRIMITIVE, 
                                       k_OBJECT_IDENTIFIER);
            if (error) {
                return error;
            }

            error = decoder->decodeValue(&d_publicKeyFamily);
            if (error) {
                return error;
            }

            error = decoder->decodeTagComplete();
            if (error) {
                return error;
            }
        }

        if (decoder->position() < decoder->current().contentPosition() + 
                                  decoder->current().contentLength().value())
        {
            error = decoder->decodeTag();
            if (error) {
                return error;
            }

            if (decoder->current().tagClass()  == k_UNIVERSAL &&
                decoder->current().tagType()   == k_PRIMITIVE &&
                decoder->current().tagNumber() == k_NULL)
            {
                d_publicKeyType.reset();
            }
            else if (decoder->current().tagClass()  == k_UNIVERSAL &&
                     decoder->current().tagType()   == k_PRIMITIVE &&
                     decoder->current().tagNumber() == k_OBJECT_IDENTIFIER)
            {
                error = decoder->decodeValue(&d_publicKeyType);
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
    }

    {
        error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_BIT_STRING);
        if (error) {
            return error;
        }

        error = decoder->decodeValue(&d_publicKeyValue);
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
    return d_publicKeyFamily == other.d_publicKeyFamily && d_publicKeyType == other.d_publicKeyType && d_publicKeyValue == other.d_publicKeyValue;
}

bool EncryptionCertificatePublicKeyInfo::less(
    const EncryptionCertificatePublicKeyInfo& other) const
{
    if (d_publicKeyFamily < other.d_publicKeyFamily) {
        return true;
    }

    if (other.d_publicKeyFamily < d_publicKeyFamily) {
        return false;
    }


    if (d_publicKeyType < other.d_publicKeyType) {
        return true;
    }

    if (other.d_publicKeyType < d_publicKeyType) {
        return false;
    }


    return d_publicKeyValue < other.d_publicKeyValue;
}

bsl::ostream& EncryptionCertificatePublicKeyInfo::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("family", d_publicKeyFamily);
    printer.printAttribute("type", d_publicKeyType);
    printer.printAttribute("value", d_publicKeyValue);
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
}

int EncryptionCertificateSignatureAlgorithmType::fromString(Value* result,
                                   const bslstl::StringRef&   string)
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

    return -1;
}

bsl::ostream& EncryptionCertificateSignatureAlgorithmType::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         EncryptionCertificateSignatureAlgorithmType::Value rhs)
{
    return EncryptionCertificateSignatureAlgorithmType::print(stream, rhs);
}


















EncryptionCertificateSignatureAlgorithm::EncryptionCertificateSignatureAlgorithm(
    bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithm::EncryptionCertificateSignatureAlgorithm(
    const EncryptionCertificateSignatureAlgorithm& original,
    bslma::Allocator*                    basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateSignatureAlgorithm::~EncryptionCertificateSignatureAlgorithm()
{
}

EncryptionCertificateSignatureAlgorithm& EncryptionCertificateSignatureAlgorithm::operator=(
    const EncryptionCertificateSignatureAlgorithm& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificateSignatureAlgorithm& EncryptionCertificateSignatureAlgorithm::operator=(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateSignatureAlgorithm& EncryptionCertificateSignatureAlgorithm::operator=(
    EncryptionCertificateSignatureAlgorithmType::Value value)
{
    EncryptionCertificateSignatureAlgorithmType::toObjectIdentifier(&d_identifier, value);
    return *this;
}

void EncryptionCertificateSignatureAlgorithm::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateSignatureAlgorithm::setIdentifier(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}


void EncryptionCertificateSignatureAlgorithm::setIdentifer(EncryptionCertificateSignatureAlgorithmType::Value value)
{
    EncryptionCertificateSignatureAlgorithmType::toObjectIdentifier(&d_identifier, value);
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

const ntsa::AbstractObjectIdentifier& EncryptionCertificateSignatureAlgorithm::identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateSignatureAlgorithm::equals(
    const EncryptionCertificateSignatureAlgorithm& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateSignatureAlgorithm::equals(EncryptionCertificateSignatureAlgorithmType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateSignatureAlgorithmType::toObjectIdentifier(
        &identifier, value);

    return d_identifier.equals(identifier);
    
}

bool EncryptionCertificateSignatureAlgorithm::less(
    const EncryptionCertificateSignatureAlgorithm& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateSignatureAlgorithm::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    ntca::EncryptionCertificateSignatureAlgorithmType::Value type;
    int rc = ntca::EncryptionCertificateSignatureAlgorithmType::fromObjectIdentifier(
        &type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateSignatureAlgorithmType::toString(type); 
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
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



















const char* EncryptionCertificateExtensionAttributeType::toString(Value value)
{
    switch (value) {
    case e_BASIC_CONSTRAINTS:
        return "BASIC_CONSTRAINTS";
    case e_AUTHORITY_KEY_IDENTIFIER:
        return "AUTHORITY_KEY_IDENTIFIER";
    case e_SUBJECT_KEY_IDENTIFIER:
        return "SUBJECT_KEY_IDENTIFIER";
    case e_SUBJECT_ALTERNATIVE_NAME:
        return "SUBJECT_ALTERNATIVE_NAME";
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
    else if (value == e_SUBJECT_KEY_IDENTIFIER) {
        result->set(2, 5, 29, 14);
    }
    else if (value == e_AUTHORITY_KEY_IDENTIFIER) {
        result->set(2, 5, 29, 35);
    }
    else if (value == e_SUBJECT_ALTERNATIVE_NAME) {
        result->set(2, 5, 29, 17);
    }
}

int EncryptionCertificateExtensionAttributeType::fromString(EncryptionCertificateExtensionAttributeType::Value* result,
                                   const bslstl::StringRef&   string)
{
    if (bdlb::String::areEqualCaseless(string, "BASIC_CONSTRAINTS")) {
        *result = e_BASIC_CONSTRAINTS;
        return 0;
    }

    if (bdlb::String::areEqualCaseless(string, "AUTHORITY_KEY_IDENTIFIER")) {
        *result = e_AUTHORITY_KEY_IDENTIFIER;
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

    if (identifier.equals(2, 5, 29, 14)) {
        *result = e_SUBJECT_KEY_IDENTIFIER;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 35)) {
        *result = e_AUTHORITY_KEY_IDENTIFIER;
        return 0;
    }

    if (identifier.equals(2, 5, 29, 17)) {
        *result = e_SUBJECT_ALTERNATIVE_NAME;
        return 0;
    }

    return -1;
}

bsl::ostream& EncryptionCertificateExtensionAttributeType::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         EncryptionCertificateExtensionAttributeType::Value rhs)
{
    return EncryptionCertificateExtensionAttributeType::print(stream, rhs);
}




























EncryptionCertificateExtensionAttribute::EncryptionCertificateExtensionAttribute(
    bslma::Allocator* basicAllocator)
: d_identifier(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionAttribute::EncryptionCertificateExtensionAttribute(
    const EncryptionCertificateExtensionAttribute& original,
    bslma::Allocator*                    basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionAttribute::~EncryptionCertificateExtensionAttribute()
{
}

EncryptionCertificateExtensionAttribute& EncryptionCertificateExtensionAttribute::operator=(
    const EncryptionCertificateExtensionAttribute& other)
{
    if (this != &other) {
        d_identifier = other.d_identifier;
    }

    return *this;
}

EncryptionCertificateExtensionAttribute& EncryptionCertificateExtensionAttribute::operator=(
    const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
    return *this;
}

EncryptionCertificateExtensionAttribute& EncryptionCertificateExtensionAttribute::operator=(
    EncryptionCertificateExtensionAttributeType::Value value)
{
    EncryptionCertificateExtensionAttributeType::toObjectIdentifier(&d_identifier, value);
    return *this;
}

void EncryptionCertificateExtensionAttribute::reset()
{
    d_identifier.reset();
}

void EncryptionCertificateExtensionAttribute::setIdentifier(const ntsa::AbstractObjectIdentifier& value)
{
    d_identifier = value;
}


void EncryptionCertificateExtensionAttribute::setIdentifer(EncryptionCertificateExtensionAttributeType::Value value)
{
    EncryptionCertificateExtensionAttributeType::toObjectIdentifier(&d_identifier, value);
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

const ntsa::AbstractObjectIdentifier& EncryptionCertificateExtensionAttribute::identifier() const
{
    return d_identifier;
}

bool EncryptionCertificateExtensionAttribute::equals(
    const EncryptionCertificateExtensionAttribute& other) const
{
    return d_identifier == other.d_identifier;
}

bool EncryptionCertificateExtensionAttribute::equals(EncryptionCertificateExtensionAttributeType::Value value) const
{
    ntsa::AbstractObjectIdentifier identifier;
    EncryptionCertificateExtensionAttributeType::toObjectIdentifier(
        &identifier, value);

    return d_identifier.equals(identifier);
    
}

bool EncryptionCertificateExtensionAttribute::less(
    const EncryptionCertificateExtensionAttribute& other) const
{
    return d_identifier < other.d_identifier;
}

bsl::ostream& EncryptionCertificateExtensionAttribute::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    ntca::EncryptionCertificateExtensionAttributeType::Value type;
    int rc = ntca::EncryptionCertificateExtensionAttributeType::fromObjectIdentifier(
        &type, d_identifier);
    if (rc == 0) {
        stream << ntca::EncryptionCertificateExtensionAttributeType::toString(type); 
    }
    else {
        d_identifier.print(stream, level, spacesPerLevel);
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
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
: d_attribute(basicAllocator)
, d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtension::EncryptionCertificateExtension(
    const EncryptionCertificateExtension& original,
    bslma::Allocator*                    basicAllocator)
: d_attribute(original.d_attribute, basicAllocator)
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
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateExtension::reset()
{
    d_attribute.reset();
    d_value.reset();
}


void EncryptionCertificateExtension::setAttribute(const EncryptionCertificateExtensionAttribute& value)
{
    d_attribute = value;
}

void EncryptionCertificateExtension::setAttribute(const ntsa::AbstractObjectIdentifier& value)
{
    d_attribute = value;
}

void EncryptionCertificateExtension::setAttribute(ntca::EncryptionCertificateExtensionAttributeType::Value value)
{
    d_attribute = value;
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

    error = decoder->decodeTag(k_UNIVERSAL, k_PRIMITIVE, k_OCTET_STRING);
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

ntsa::Error EncryptionCertificateExtension::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

const EncryptionCertificateExtensionAttribute& 
EncryptionCertificateExtension::attribute() const
{
    return d_attribute;
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
: d_extensionVector(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateExtensionList::EncryptionCertificateExtensionList(
    const EncryptionCertificateExtensionList& original,
    bslma::Allocator*                    basicAllocator)
: d_extensionVector(original.d_extensionVector, basicAllocator)
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
        d_extensionVector = other.d_extensionVector;
    }

    return *this;
}

void EncryptionCertificateExtensionList::reset()
{
    d_extensionVector.clear();
}

ntsa::Error EncryptionCertificateExtensionList::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    ntsa::Error error;

    error = decoder->decodeTag(k_UNIVERSAL, k_CONSTRUCTED, k_SEQUENCE);
    if (error) {
        return error;
    }

    while (decoder->position() < decoder->current().contentPosition() + 
                                 decoder->current().contentLength().value())
    {
        EncryptionCertificateExtension extension;
        error = extension.decode(decoder);
        if (error) {
            return error;
        }

        d_extensionVector.push_back(extension);
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
    return d_extensionVector == other.d_extensionVector;
}

bool EncryptionCertificateExtensionList::less(
    const EncryptionCertificateExtensionList& other) const
{
    return d_extensionVector < other.d_extensionVector;
}

bsl::ostream& EncryptionCertificateExtensionList::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("extension", d_extensionVector);
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


    while (decoder->position() < decoder->current().contentPosition() + 
                                 decoder->current().contentLength().value())
    {
        error = decoder->decodeTag();
        if (error) {
            return error;
        }

        if (decoder->current().tagClass()  == k_CONTEXT_SPECIFIC &&
            decoder->current().tagType()   == k_PRIMITIVE &&
            decoder->current().tagNumber() == 1)
        {
            error = decoder->decodeValue(
                &d_issuerUniqueId.makeValue());
            if (error) {
                return error;
            }
        }
        else if (decoder->current().tagClass()  == k_CONTEXT_SPECIFIC &&
                 decoder->current().tagType()   == k_PRIMITIVE &&
                 decoder->current().tagNumber() == 2)
        {
            error = decoder->decodeValue(
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

const ntca::EncryptionCertificateSignatureAlgorithm& EncryptionCertificate::signatureAlgorithm() const
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
