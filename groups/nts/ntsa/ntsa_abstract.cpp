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

#include <ntsa_abstract.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_abstract_cpp, "$Id$ $CSID$")

#include <bdlb_bitutil.h>
#include <bdlb_chartype.h>
#include <bdlb_numericparseutil.h>
#include <bdlt_iso8601util.h>
#include <bdlt_iso8601utilconfiguration.h>
#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {
namespace ntsa {

namespace {

const bsl::uint8_t k_TAG_MASK_CLASS  = 0xC0;
const bsl::uint8_t k_TAG_MASK_TYPE   = 0x20;
const bsl::uint8_t k_TAG_MASK_NUMBER = 0x1F;

const bsl::size_t k_NUM_VALUE_BITS_IN_TAG_OCTET = 7;
const bsl::size_t k_MAX_TAG_NUMBER_OCTETS =
    (4 * 8) / k_NUM_VALUE_BITS_IN_TAG_OCTET + 1;

const ntsa::AbstractIntegerBase::Value k_DEFAULT_BASE =
    AbstractIntegerBase::e_NATIVE;

struct AbstractIntegerBaseTraits {
    bsl::uint64_t d_radix;
    bsl::uint64_t d_minValue;
    bsl::uint64_t d_maxValue;
};

// clang-format off
static const AbstractIntegerBaseTraits k_TRAITS[5] = {
    {  1ULL << (sizeof(AbstractIntegerRepresentation::Block) * 8), 
       0, 
       (1ULL << (sizeof(AbstractIntegerRepresentation::Block) * 8)) - 1 
    },
    {     2, 0,     1 },
    {     8, 0,     7 },
    {    10, 0,     9 },
    {    16, 0,    15 }
};
// clang-format on

}  // close unnamed namespace

ntsa::Error AbstractSyntaxTagClass::fromValue(Value* result, bsl::size_t value)
{
    switch (value) {
    case e_UNIVERSAL:
    case e_APPLICATION:
    case e_CONTEXT_SPECIFIC:
    case e_PRIVATE:
        *result = static_cast<Value>(value);
        return ntsa::Error();
    default:
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

const char* AbstractSyntaxTagClass::toString(Value value)
{
    switch (value) {
    case e_UNIVERSAL: {
        return "UNIVERSAL";
    } break;
    case e_APPLICATION: {
        return "APPLICATION";
    } break;
    case e_CONTEXT_SPECIFIC: {
        return "CONTEXT_SPECIFIC";
    } break;
    case e_PRIVATE: {
        return "PRIVATE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& AbstractSyntaxTagClass::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         AbstractSyntaxTagClass::Value rhs)
{
    return AbstractSyntaxTagClass::print(stream, rhs);
}

ntsa::Error AbstractSyntaxTagType::fromValue(Value* result, bsl::size_t value)
{
    switch (value) {
    case e_PRIMITIVE:
    case e_CONSTRUCTED:
        *result = static_cast<Value>(value);
        return ntsa::Error();
    default:
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

const char* AbstractSyntaxTagType::toString(Value value)
{
    switch (value) {
    case e_PRIMITIVE: {
        return "PRIMITIVE";
    } break;
    case e_CONSTRUCTED: {
        return "CONSTRUCTED";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& AbstractSyntaxTagType::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         AbstractSyntaxTagType::Value rhs)
{
    return AbstractSyntaxTagType::print(stream, rhs);
}

ntsa::Error AbstractSyntaxTagNumber::validate(bsl::size_t value)
{
    if (value > 256) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

const char* AbstractSyntaxTagNumber::toString(Value value)
{
    switch (value) {
    case e_END_OF_CONTENTS: {
        return "END_OF_CONTENTS";
    } break;
    case e_BOOLEAN: {
        return "BOOLEAN";
    } break;
    case e_INTEGER: {
        return "INTEGER";
    } break;
    case e_BIT_STRING: {
        return "BIT_STRING";
    } break;
    case e_OCTET_STRING: {
        return "OCTET_STRING";
    } break;
    case e_NULL: {
        return "NULL";
    } break;
    case e_OBJECT_IDENTIFIER: {
        return "OBJECT_IDENTIFIER";
    } break;
    case e_OBJECT_DESCRIPTOR: {
        return "OBJECT_DESCRIPTOR";
    } break;
    case e_EXTERNAL: {
        return "EXTERNAL";
    } break;
    case e_REAL: {
        return "REAL";
    } break;
    case e_ENUMERATED: {
        return "ENUMERATED";
    } break;
    case e_EMBEDDED_PDV: {
        return "EMBEDDED_PDV";
    } break;
    case e_UTF8_STRING: {
        return "UTF8_STRING";
    } break;
    case e_RELATIVE_OID: {
        return "RELATIVE_OID";
    } break;
    case e_SEQUENCE: {
        return "SEQUENCE";
    } break;
    case e_SET: {
        return "SET";
    } break;
    case e_NUMERIC_STRING: {
        return "NUMERIC_STRING";
    } break;
    case e_PRINTABLE_STRING: {
        return "PRINTABLE_STRING";
    } break;
    case e_T61_STRING: {
        return "T61_STRING";
    } break;
    case e_VIDEOTEXT_STRING: {
        return "VIDEOTEXT_STRING";
    } break;
    case e_IA5_STRING: {
        return "IA5_STRING";
    } break;
    case e_UTC_TIME: {
        return "UTC_TIME";
    } break;
    case e_GENERALIZED_TIME: {
        return "GENERALIZED_TIME";
    } break;
    case e_GRAPHIC_STRING: {
        return "GRAPHIC_STRING";
    } break;
    case e_VISIBLE_STRING: {
        return "VISIBLE_STRING";
    } break;
    case e_GENERAL_STRING: {
        return "GENERAL_STRING";
    } break;
    case e_UNIVERSAL_STRING: {
        return "UNIVERSAL_STRING";
    } break;
    case e_CHARACTER_STRING: {
        return "CHARACTER_STRING";
    } break;
    case e_BMP_STRING: {
        return "BMP_STRING";
    } break;
    case e_LONG_FORM: {
        return "LONG_FORM";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& AbstractSyntaxTagNumber::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                  stream,
                         AbstractSyntaxTagNumber::Value rhs)
{
    return AbstractSyntaxTagNumber::print(stream, rhs);
}

const char* AbstractSyntaxFormat::toString(Value value)
{
    switch (value) {
    case e_DISTINGUISHED: {
        return "DISTINGUISHED";
    } break;
    case e_CANONICAL: {
        return "CANONICAL";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& AbstractSyntaxFormat::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, AbstractSyntaxFormat::Value rhs)
{
    return AbstractSyntaxFormat::print(stream, rhs);
}

AbstractSyntaxEncoderOptions::AbstractSyntaxEncoderOptions(
    bslma::Allocator* basicAllocator)
: d_format()
{
    NTSCFG_WARNING_UNUSED(basicAllocator);
}

AbstractSyntaxEncoderOptions::AbstractSyntaxEncoderOptions(
    const AbstractSyntaxEncoderOptions& original,
    bslma::Allocator*                   basicAllocator)
: d_format(original.d_format)
{
    NTSCFG_WARNING_UNUSED(basicAllocator);
}

AbstractSyntaxEncoderOptions::~AbstractSyntaxEncoderOptions()
{
}

AbstractSyntaxEncoderOptions& AbstractSyntaxEncoderOptions::operator=(
    const AbstractSyntaxEncoderOptions& other)
{
    if (this != &other) {
        d_format = other.d_format;
    }

    return *this;
}

void AbstractSyntaxEncoderOptions::reset()
{
    d_format.reset();
}

void AbstractSyntaxEncoderOptions::setFormat(
    const ntsa::AbstractSyntaxFormat::Value& value)
{
    d_format = value;
}

const bdlb::NullableValue<ntsa::AbstractSyntaxFormat::Value>&
AbstractSyntaxEncoderOptions::format() const
{
    return d_format;
}

bool AbstractSyntaxEncoderOptions::equals(
    const AbstractSyntaxEncoderOptions& other) const
{
    return d_format == other.d_format;
}

bool AbstractSyntaxEncoderOptions::less(
    const AbstractSyntaxEncoderOptions& other) const
{
    return d_format < other.d_format;
}

bsl::ostream& AbstractSyntaxEncoderOptions::print(bsl::ostream& stream,
                                                  int           level,
                                                  int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_format.isNull()) {
        printer.printAttribute("format", d_format);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const AbstractSyntaxEncoderOptions& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractSyntaxEncoderOptions& lhs,
                const AbstractSyntaxEncoderOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractSyntaxEncoderOptions& lhs,
                const AbstractSyntaxEncoderOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractSyntaxEncoderOptions& lhs,
               const AbstractSyntaxEncoderOptions& rhs)
{
    return lhs.less(rhs);
}

AbstractSyntaxEncoderFrame::AbstractSyntaxEncoderFrame(
    AbstractSyntaxEncoderFrame* parent,
    bsl::streambuf*             buffer,
    bslma::Allocator*           basicAllocator)
: d_tagClass(AbstractSyntaxTagClass::e_UNIVERSAL)
, d_tagType(AbstractSyntaxTagType::e_PRIMITIVE)
, d_tagNumber(AbstractSyntaxTagNumber::e_NULL)
, d_length(0)
, d_buffer_p(buffer)
, d_header(basicAllocator)
, d_content(basicAllocator)
, d_parent_p(parent)
, d_children(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractSyntaxEncoderFrame::~AbstractSyntaxEncoderFrame()
{
    while (!d_children.empty()) {
        this->destroyLast();
    }
}

AbstractSyntaxEncoderFrame* AbstractSyntaxEncoderFrame::createNext(
    AbstractSyntaxTagClass::Value  tagClass,
    AbstractSyntaxTagType::Value   tagType,
    AbstractSyntaxTagNumber::Value tagNumber)
{
    return this->createNext(tagClass,
                            tagType,
                            static_cast<bsl::size_t>(tagNumber));
}

AbstractSyntaxEncoderFrame* AbstractSyntaxEncoderFrame::createNext(
    AbstractSyntaxTagClass::Value tagClass,
    AbstractSyntaxTagType::Value  tagType,
    bsl::size_t                   tagNumber)
{
    AbstractSyntaxEncoderFrame* next = new (*d_allocator_p)
        AbstractSyntaxEncoderFrame(this, d_buffer_p, d_allocator_p);

    next->setTagClass(tagClass);
    next->setTagType(tagType);
    next->setTagNumber(tagNumber);

    d_children.push_back(next);

    return next;
}

void AbstractSyntaxEncoderFrame::destroyLast()
{
    if (!d_children.empty()) {
        AbstractSyntaxEncoderFrame* child = d_children.back();
        d_allocator_p->deleteObject(child);
        d_children.pop_back();
    }
}

void AbstractSyntaxEncoderFrame::setTagClass(
    AbstractSyntaxTagClass::Value value)
{
    d_tagClass = value;
}

void AbstractSyntaxEncoderFrame::setTagType(AbstractSyntaxTagType::Value value)
{
    d_tagType = value;
}

void AbstractSyntaxEncoderFrame::setTagNumber(
    AbstractSyntaxTagNumber::Value value)
{
    d_tagNumber = static_cast<bsl::size_t>(value);
}

void AbstractSyntaxEncoderFrame::setTagNumber(bsl::size_t value)
{
    d_tagNumber = value;
}

ntsa::Error AbstractSyntaxEncoderFrame::writeHeader(bsl::uint8_t data)
{
    return AbstractSyntaxEncoderUtil::write(&d_header, data);
}

ntsa::Error AbstractSyntaxEncoderFrame::writeHeader(const void* data,
                                                    bsl::size_t size)
{
    return AbstractSyntaxEncoderUtil::write(&d_header, data, size);
}

ntsa::Error AbstractSyntaxEncoderFrame::writeContent(bsl::uint8_t data)
{
    return AbstractSyntaxEncoderUtil::write(&d_content, data);
}

ntsa::Error AbstractSyntaxEncoderFrame::writeContent(const void* data,
                                                     bsl::size_t size)
{
    return AbstractSyntaxEncoderUtil::write(&d_content, data, size);
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeContentBase128(
    bsl::uint64_t value)
{
    return AbstractSyntaxEncoderUtil::encodeIntegerBase128(&d_content, value);
}


ntsa::Error AbstractSyntaxEncoderFrame::encodeNull()
{
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(bool value)
{
    ntsa::Error error;

    if (value) {
        error = this->writeContent(0x00);
    }
    else {
        error = this->writeContent(0xFF);
    }

    if (error) {
        return error;
    }

    return error;
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(short value)
{
    return this->encodeValue(static_cast<long long>(value));
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(unsigned short value)
{
    return this->encodeValue(static_cast<unsigned long long>(value));
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(int value)
{
    return this->encodeValue(static_cast<long long>(value));
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(unsigned int value)
{
    return this->encodeValue(static_cast<unsigned long long>(value));
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(long value)
{
    return this->encodeValue(static_cast<long long>(value));
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(unsigned long value)
{
    return this->encodeValue(static_cast<unsigned long long>(value));
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(long long value)
{
    ntsa::Error error;

    long long bigEndianValue = BSLS_BYTEORDER_HOST_TO_BE(value);

    const bsl::uint8_t* bigEndianByteArray =
        reinterpret_cast<bsl::uint8_t*>(&bigEndianValue);

    bsl::size_t bigEndianByteArraySize = sizeof bigEndianValue;

    bsl::size_t numSkipped = 0;

    for (bsl::size_t i = 0; i < sizeof bigEndianValue - 1; ++i) {
        const bsl::uint8_t bigEndianByte     = bigEndianByteArray[i];
        const bsl::uint8_t bigEndianByteNext = bigEndianByteArray[i + 1];

        if ((bigEndianByte == 0xFF) && ((bigEndianByteNext & 0x80) != 0)) {
            ++numSkipped;
        }
        else {
            break;
        }
    }

    bigEndianByteArray     += numSkipped;
    bigEndianByteArraySize -= numSkipped;

    for (bsl::size_t i = 0; i < bigEndianByteArraySize; ++i) {
        const bsl::uint8_t bigEndianByte = bigEndianByteArray[i];
        error = this->writeContent(bigEndianByte);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(unsigned long long value)
{
    ntsa::Error error;

    if (value == 0) {
        error = this->writeContent(0x00);
        if (error) {
            return error;
        }
    }
    else {
        const unsigned long long bigEndianValue =
            BSLS_BYTEORDER_HOST_TO_BE(value);

        const bsl::uint8_t* bigEndianByteArray =
            reinterpret_cast<const bsl::uint8_t*>(&bigEndianValue);

        bsl::size_t bigEndianByteArraySize = sizeof bigEndianValue;

        bsl::size_t numSkipped = 0;

        for (bsl::size_t i = 0; i < sizeof bigEndianValue; ++i) {
            const bsl::uint8_t bigEndianByte = bigEndianByteArray[i];

            if (bigEndianByte == 0x00) {
                ++numSkipped;
            }
            else {
                break;
            }
        }

        bigEndianByteArray     += numSkipped;
        bigEndianByteArraySize -= numSkipped;

        if (bigEndianByteArraySize > 0) {
            const bsl::uint8_t bigEndianByte = bigEndianByteArray[0];
            if ((bigEndianByte & 0x80) != 0) {
                error = this->writeContent(0x00);
                if (error) {
                    return error;
                }
            }
        }

        for (bsl::size_t i = 0; i < bigEndianByteArraySize; ++i) {
            const bsl::uint8_t bigEndianByte = bigEndianByteArray[i];
            error = this->writeContent(bigEndianByte);
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(const bsl::string& value)
{
    ntsa::Error error;

    error = this->writeContent(value.data(), value.size());
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(const bdlt::Datetime& value)
{
    return this->encodeValue(bdlt::DatetimeTz(value, 0));
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(const bdlt::DatetimeTz& value)
{
    return AbstractSyntaxEncoderUtil::encodeDatetimeTz(
        &d_content, 
        d_tagClass, 
        d_tagType,
        d_tagNumber,
        value);
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(const ntsa::AbstractInteger& value)
{
    ntsa::Error error;

    if (value.isZero()) {
        error = this->writeContent(0x00);
        if (error) {
            return error;
        }
    }
    else {
        bsl::vector<bsl::uint8_t> data;
        value.encode(&data);

        if (data.empty()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = this->writeContent(&data.front(), data.size());
        if (error) {
            return error;
        }
    }
    
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(const ntsa::AbstractString& value)
{
    ntsa::Error error;

    error = this->writeContent(value.data(), value.size());
    if (error) {
        return error;
    }
    
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(const ntsa::AbstractBitSequence& value)
{
    ntsa::Error error;

    error = this->writeContent(value.data(), value.size());
    if (error) {
        return error;
    }
    
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(const ntsa::AbstractByteSequence& value)
{
    ntsa::Error error;

    error = this->writeContent(value.data(), value.size());
    if (error) {
        return error;
    }
    
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::encodeValue(const ntsa::AbstractObjectIdentifier& value)
{
    ntsa::Error error;

    if (value.size() < 2) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint64_t v0 = value.get(0);
    const bsl::uint64_t v1 = value.get(1);

    if (v0 > 3) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (v0 == 0 || v0 == 1) {
        if (v1 > 39) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    const bsl::uint64_t vx = (v0 * 40) + v1;

    error = this->writeContent(vx);
    if (error) {
        return error;
    }

    for (bsl::size_t i = 2; i < value.size(); ++i) {
        error = this->encodeContentBase128(value.get(i));
        if (error) {
            return error;
        }
    }
    
    return ntsa::Error();
}


ntsa::Error AbstractSyntaxEncoderFrame::synchronize(bsl::size_t* length)
{
    ntsa::Error error;

    if (d_length > 0) {
        *length += d_length;
        return ntsa::Error();
    }

    bsl::size_t contentLength = 0;

    if (!d_children.empty()) {
        if (d_content.length() != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const bsl::size_t numChildren = d_children.size();

        for (bsl::size_t i = 0; i < numChildren; ++i) {
            AbstractSyntaxEncoderFrame* child = d_children[i];
            error = child->synchronize(&contentLength);
            if (error) {
                return error;
            }
        }
    }
    else {
        contentLength = d_content.length();
    }

    error = AbstractSyntaxEncoderUtil::encodeTag(&d_header,
                                                 d_tagClass,
                                                 d_tagType,
                                                 d_tagNumber);
    if (error) {
        return error;
    }

    error = AbstractSyntaxEncoderUtil::encodeLength(&d_header, contentLength);
    if (error) {
        return error;
    }

    error = AbstractSyntaxEncoderUtil::synchronize(&d_header);
    if (error) {
        return error;
    }

    d_length = contentLength + d_header.length();

    *length += d_length;

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderFrame::flush(bsl::streambuf* buffer)
{
    ntsa::Error error;

    if (d_header.length() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = AbstractSyntaxEncoderUtil::write(buffer,
                                             d_header.data(),
                                             d_header.length());
    if (error) {
        return error;
    }

    if (!d_children.empty()) {
        const bsl::size_t numChildren = d_children.size();

        for (bsl::size_t i = 0; i < numChildren; ++i) {
            AbstractSyntaxEncoderFrame* child = d_children[i];
            error                             = child->flush(buffer);
            if (error) {
                return error;
            }
        }
    }
    else {
        if (d_content.length() == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = AbstractSyntaxEncoderUtil::write(buffer,
                                                 d_content.data(),
                                                 d_content.length());
        if (error) {
            return error;
        }
    }

    error = AbstractSyntaxEncoderUtil::synchronize(buffer);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

AbstractSyntaxTagClass::Value AbstractSyntaxEncoderFrame::tagClass() const
{
    return d_tagClass;
}

AbstractSyntaxTagType::Value AbstractSyntaxEncoderFrame::tagType() const
{
    return d_tagType;
}

bsl::size_t AbstractSyntaxEncoderFrame::tagNumber() const
{
    return d_tagNumber;
}

const bsl::uint8_t* AbstractSyntaxEncoderFrame::header() const
{
    if (d_header.length() != 0) {
        return reinterpret_cast<const bsl::uint8_t*>(d_header.data());
    }
    else {
        return 0;
    }
}

bsl::size_t AbstractSyntaxEncoderFrame::headerLength() const
{
    return static_cast<bsl::size_t>(d_header.length());
}

const bsl::uint8_t* AbstractSyntaxEncoderFrame::content() const
{
    if (d_content.length() != 0) {
        return reinterpret_cast<const bsl::uint8_t*>(d_content.data());
    }
    else {
        return 0;
    }
}

bsl::size_t AbstractSyntaxEncoderFrame::contentLength() const
{
    return static_cast<bsl::size_t>(d_content.length());
}

AbstractSyntaxEncoderFrame* AbstractSyntaxEncoderFrame::childIndex(
    bsl::size_t index) const
{
    if (index < d_children.size()) {
        return d_children[index];
    }
    else {
        return 0;
    }
}

bsl::size_t AbstractSyntaxEncoderFrame::childCount() const
{
    return d_children.size();
}

AbstractSyntaxEncoderFrame* AbstractSyntaxEncoderFrame::parent() const
{
    return d_parent_p;
}

AbstractSyntaxEncoder::AbstractSyntaxEncoder(bsl::streambuf*   buffer,
                                             bslma::Allocator* basicAllocator)
: d_buffer_p(buffer)
, d_root_p(0)
, d_current_p(0)
, d_config(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractSyntaxEncoder::AbstractSyntaxEncoder(
    const AbstractSyntaxEncoderOptions& configuration,
    bsl::streambuf*                     buffer,
    bslma::Allocator*                   basicAllocator)
: d_buffer_p(buffer)
, d_root_p(0)
, d_current_p(0)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractSyntaxEncoder::~AbstractSyntaxEncoder()
{
    if (d_root_p) {
        d_allocator_p->deleteObject(d_root_p);
    }
}

ntsa::Error AbstractSyntaxEncoder::encodeTag(
    AbstractSyntaxTagClass::Value  tagClass,
    AbstractSyntaxTagType::Value   tagType,
    AbstractSyntaxTagNumber::Value tagNumber)
{
    return this->encodeTag(tagClass,
                            tagType,
                            static_cast<bsl::size_t>(tagNumber));
}

ntsa::Error AbstractSyntaxEncoder::encodeTag(
    AbstractSyntaxTagClass::Value tagClass,
    AbstractSyntaxTagType::Value  tagType,
    bsl::size_t                   tagNumber)
{
    if (d_current_p != 0) {
        AbstractSyntaxEncoderFrame* next =
            d_current_p->createNext(tagClass, tagType, tagNumber);

        d_current_p = next;
    }
    else {
        d_root_p = new (*d_allocator_p)
            AbstractSyntaxEncoderFrame(0, d_buffer_p, d_allocator_p);

        d_root_p->setTagClass(tagClass);
        d_root_p->setTagType(tagType);
        d_root_p->setTagNumber(tagNumber);

        d_current_p = d_root_p;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeNull()
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeNull();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(bool value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(short value)
{
    return this->encodeValue(static_cast<long long>(value));
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(unsigned short value)
{
    return this->encodeValue(static_cast<unsigned long long>(value));
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(int value)
{
    return this->encodeValue(static_cast<long long>(value));
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(unsigned int value)
{
    return this->encodeValue(static_cast<unsigned long long>(value));
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(long value)
{
    return this->encodeValue(static_cast<long long>(value));
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(unsigned long value)
{
    return this->encodeValue(static_cast<unsigned long long>(value));
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(long long value)
{
    if (value >= 0) {
        return this->encodeValue(
            static_cast<unsigned long long>(value));
    }

    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    unsigned long long value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    const AbstractInteger& value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    const bsl::string& value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    const AbstractString& value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    const AbstractBitSequence& value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    const AbstractByteSequence& value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    const bdlt::Datetime& value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    const bdlt::DatetimeTz& value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeValue(
    const AbstractObjectIdentifier& value)
{
    ntsa::Error error;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_current_p->encodeValue(value);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoder::encodeTagComplete()
{
    ntsa::Error error;
    int         rc;

    if (d_current_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t length = 0;
    NTSCFG_WARNING_UNUSED(length);

    error = d_current_p->synchronize(&length);
    if (error) {
        return error;
    }

    d_current_p = d_current_p->parent();

    if (d_current_p == 0) {
        error = d_root_p->flush(d_buffer_p);
        if (error) {
            return error;
        }

        d_allocator_p->deleteObject(d_root_p);
        d_root_p = 0;

        rc = d_buffer_p->pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

const AbstractSyntaxEncoderOptions& AbstractSyntaxEncoder::configuration()
    const
{
    return d_config;
}

bsl::streambuf* AbstractSyntaxEncoder::buffer() const
{
    return d_buffer_p;
}

bsl::size_t AbstractSyntaxEncoderUtil::numSignificantBits(bsl::uint64_t value)
{
    if (value == 0) {
        return 1;
    }

    std::size_t i = 0;

    while (value > 255) {
        value  = value >> 8;
        i     += 8;
    }

    while (value != 0) {
        value = value >> 1;
        ++i;
    }

    return i;
}

ntsa::Error AbstractSyntaxEncoderUtil::write(bsl::streambuf* destination,
                                             bsl::uint8_t    data)
{
    bsl::streambuf::int_type meta =
        destination->sputc(static_cast<char>(data));
    if (bsl::streambuf::traits_type::eq_int_type(
            meta,
            bsl::streambuf::traits_type::eof()))
    {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderUtil::write(bsl::streambuf* destination,
                                             const void*     data,
                                             bsl::size_t     size)
{
    bsl::streamsize n = destination->sputn(static_cast<const char*>(data),
                                           static_cast<bsl::streamsize>(size));

    if (static_cast<bsl::size_t>(n) != size) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderUtil::encodeTag(
    bsl::streambuf*                destination,
    AbstractSyntaxTagClass::Value  tagClass,
    AbstractSyntaxTagType::Value   tagType,
    AbstractSyntaxTagNumber::Value tagNumber)
{
    return AbstractSyntaxEncoderUtil::encodeTag(
        destination,
        tagClass,
        tagType,
        static_cast<bsl::size_t>(tagNumber));
}

ntsa::Error AbstractSyntaxEncoderUtil::encodeTag(
    bsl::streambuf*               destination,
    AbstractSyntaxTagClass::Value tagClass,
    AbstractSyntaxTagType::Value  tagType,
    bsl::size_t                   tagNumber)
{
    ntsa::Error error;

    const std::size_t k_MAX_TAG_NUMBER_IN_ONE_OCTET = 30;
    const std::size_t k_TAG_MASK_NUMBER             = 0x1F;

    bsl::uint8_t firstOctet = 0;

    firstOctet |= static_cast<bsl::uint8_t>(tagClass);
    firstOctet |= static_cast<bsl::uint8_t>(tagType);

    if (tagNumber <= k_MAX_TAG_NUMBER_IN_ONE_OCTET) {
        firstOctet |= static_cast<bsl::uint8_t>(tagNumber);

        error = AbstractSyntaxEncoderUtil::write(destination, firstOctet);
        if (error) {
            return error;
        }
    }
    else {
        firstOctet |= static_cast<bsl::uint8_t>(k_TAG_MASK_NUMBER);

        error = AbstractSyntaxEncoderUtil::write(destination, firstOctet);
        if (error) {
            return error;
        }

        error = AbstractSyntaxEncoderUtil::encodeIntegerBase128(destination,
                                                                tagNumber);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderUtil::encodeLength(
    bsl::streambuf* destination,
    bsl::size_t     length)
{
    ntsa::Error error;

    const bsl::size_t k_MAX_LENGTH_IN_ONE_OCTET = 127;

    if (length <= k_MAX_LENGTH_IN_ONE_OCTET) {
        error = AbstractSyntaxEncoderUtil::write(
            destination,
            static_cast<bsl::uint8_t>(length));
        if (error) {
            return error;
        }
    }
    else {
        std::size_t numOctets =
            (AbstractSyntaxEncoderUtil::numSignificantBits(length) + 7) / 8;

        bsl::uint8_t firstOctet = static_cast<bsl::uint8_t>(numOctets | 0x80);

        error = AbstractSyntaxEncoderUtil::write(destination, firstOctet);
        if (error) {
            return error;
        }

        std::size_t i = numOctets;
        std::size_t j = (i - 1) * 8;

        std::size_t n = length;

        while (i > 0) {
            const bsl::uint8_t octet =
                static_cast<bsl::uint8_t>((n >> j) & 0xFF);

            error = AbstractSyntaxEncoderUtil::write(destination, octet);
            if (error) {
                return error;
            }

            j = j - 8;
            i = i - 1;
        }
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderUtil::encodeLengthIndefinite(
    bsl::streambuf* destination)
{
    ntsa::Error error;

    error = AbstractSyntaxEncoderUtil::write(destination, 0x80);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderUtil::encodeTerminator(
    bsl::streambuf* destination)
{
    ntsa::Error error;

    error = AbstractSyntaxEncoderUtil::write(destination, 0x00);
    if (error) {
        return error;
    }

    error = AbstractSyntaxEncoderUtil::write(destination, 0x00);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderUtil::encodeIntegerBase128(
    bsl::streambuf* destination,
    bsl::uint64_t   value)
{
    ntsa::Error error;

    const std::size_t length =
        (AbstractSyntaxEncoderUtil::numSignificantBits(value) + 6) / 7;

    std::size_t i = length;
    std::size_t j = (i - 1) * 7;

    std::size_t n = value;

    while (i > 1) {
        const bsl::uint8_t octet =
            static_cast<bsl::uint8_t>(((n >> j) & 0x7F) | 0x80);

        error = AbstractSyntaxEncoderUtil::write(destination, octet);
        if (error) {
            return error;
        }

        j = j - 7;
        i = i - 1;
    }

    {
        const bsl::uint8_t octet = static_cast<bsl::uint8_t>(n & 0x7F);

        error = AbstractSyntaxEncoderUtil::write(destination, octet);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderUtil::encodeDatetimeTz(
        bsl::streambuf*                     destination,
        ntsa::AbstractSyntaxTagClass::Value tagClass,
        ntsa::AbstractSyntaxTagType::Value  tagType,
        bsl::size_t                         tagNumber,
        const bdlt::DatetimeTz&             value)
{
    ntsa::Error error;

    enum Format {
        e_UNKNOWN,
        e_UTC,
        e_GENERALIZED,
        e_ISO8601
    };

    Format format = e_UNKNOWN;

    if (tagClass == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (tagNumber == AbstractSyntaxTagNumber::e_UTC_TIME) {
            format = e_UTC;
        }
        else if (tagNumber == AbstractSyntaxTagNumber::e_GENERALIZED_TIME) {
            format = e_GENERALIZED;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (tagClass == AbstractSyntaxTagClass::e_CONTEXT_SPECIFIC) {
        format = e_ISO8601;
    }

    if (tagType != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (format == e_UNKNOWN) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!bdlt::DatetimeTz::isValid(value.localDatetime(), value.offset())) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (format == e_ISO8601) {
        bsl::ostream os(destination);
        bdlt::Iso8601Util::generate(os, value);
        if (!os) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        return ntsa::Error();
    }

    bsl::ostream os(destination);

    int year        = value.localDatetime().year();
    int month       = value.localDatetime().month();
    int day         = value.localDatetime().day();
    int hour        = value.localDatetime().hour();
    int minute      = value.localDatetime().minute();
    int second      = value.localDatetime().second();
    int millisecond = value.localDatetime().millisecond();
    int offset      = value.offset();

    if (format == e_UTC) {
        if (year < 2000) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        os << bsl::setw(2) << bsl::setfill('0') << (year - 2000);
    }
    else {
        os << bsl::setw(4) << bsl::setfill('0') << year;
    }

    os << bsl::setw(2) << bsl::setfill('0') << month;
    os << bsl::setw(2) << bsl::setfill('0') << day;
    os << bsl::setw(2) << bsl::setfill('0') << hour;

    if (format == e_UTC || minute > 0 || second > 0 || millisecond > 0) {
        os << bsl::setw(2) << bsl::setfill('0') << minute;
        if (format == e_UTC || second > 0 || millisecond > 0) {
            os << bsl::setw(2) << bsl::setfill('0') << second;
            if (format == e_GENERALIZED && millisecond > 0) {
                os << millisecond;
            }
        }
    }

    if (offset == 0) {
        os << 'Z';
    }
    else {

        if (offset > 0) {
            os << '+';
        }
        else {
            os << '-';
        }

        const int gmtOffsetHour   = value.offset() / 60;
        const int gmtOffsetMinute = value.offset() % 60;

        os << bsl::setw(2) << bsl::setfill('0') << gmtOffsetHour;
        os << bsl::setw(2) << bsl::setfill('0') << gmtOffsetMinute;
    }

    os.flush();

    if (!os) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxEncoderUtil::synchronize(bsl::streambuf* destination)
{
    int rc = destination->pubsync();
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

AbstractSyntaxDecoderOptions::AbstractSyntaxDecoderOptions(
    bslma::Allocator* basicAllocator)
: d_format()
{
    NTSCFG_WARNING_UNUSED(basicAllocator);
}

AbstractSyntaxDecoderOptions::AbstractSyntaxDecoderOptions(
    const AbstractSyntaxDecoderOptions& original,
    bslma::Allocator*                   basicAllocator)
: d_format(original.d_format)
{
    NTSCFG_WARNING_UNUSED(basicAllocator);
}

AbstractSyntaxDecoderOptions::~AbstractSyntaxDecoderOptions()
{
}

AbstractSyntaxDecoderOptions& AbstractSyntaxDecoderOptions::operator=(
    const AbstractSyntaxDecoderOptions& other)
{
    if (this != &other) {
        d_format = other.d_format;
    }

    return *this;
}

void AbstractSyntaxDecoderOptions::reset()
{
    d_format.reset();
}

void AbstractSyntaxDecoderOptions::setFormat(
    const ntsa::AbstractSyntaxFormat::Value& value)
{
    d_format = value;
}

const bdlb::NullableValue<ntsa::AbstractSyntaxFormat::Value>&
AbstractSyntaxDecoderOptions::format() const
{
    return d_format;
}

bool AbstractSyntaxDecoderOptions::equals(
    const AbstractSyntaxDecoderOptions& other) const
{
    return d_format == other.d_format;
}

bool AbstractSyntaxDecoderOptions::less(
    const AbstractSyntaxDecoderOptions& other) const
{
    return d_format < other.d_format;
}

bsl::ostream& AbstractSyntaxDecoderOptions::print(bsl::ostream& stream,
                                                  int           level,
                                                  int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_format.isNull()) {
        printer.printAttribute("format", d_format);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const AbstractSyntaxDecoderOptions& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractSyntaxDecoderOptions& lhs,
                const AbstractSyntaxDecoderOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractSyntaxDecoderOptions& lhs,
                const AbstractSyntaxDecoderOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractSyntaxDecoderOptions& lhs,
               const AbstractSyntaxDecoderOptions& rhs)
{
    return lhs.less(rhs);
}

AbstractSyntaxDecoderFrame::AbstractSyntaxDecoderFrame()
: d_tagClass(AbstractSyntaxTagClass::e_UNIVERSAL)
, d_tagType(AbstractSyntaxTagType::e_PRIMITIVE)
, d_tagNumber(AbstractSyntaxTagNumber::e_NULL)
, d_tagPosition(0)
, d_tagLength(0)
, d_contentPosition(0)
, d_contentLength()
{
}

AbstractSyntaxDecoderFrame::AbstractSyntaxDecoderFrame(
    const AbstractSyntaxDecoderFrame& original)
: d_tagClass(original.d_tagClass)
, d_tagType(original.d_tagType)
, d_tagNumber(original.d_tagNumber)
, d_tagPosition(original.d_tagPosition)
, d_tagLength(original.d_tagLength)
, d_contentPosition(original.d_contentPosition)
, d_contentLength(original.d_contentLength)
{
}

AbstractSyntaxDecoderFrame::~AbstractSyntaxDecoderFrame()
{
}

AbstractSyntaxDecoderFrame& AbstractSyntaxDecoderFrame::operator=(
    const AbstractSyntaxDecoderFrame& other)
{
    if (this != &other) {
        d_tagClass  = other.d_tagClass;
        d_tagType   = other.d_tagType;
        d_tagNumber = other.d_tagNumber;
        d_tagPosition = other.d_tagPosition;
        d_tagLength = other.d_tagLength;
        d_contentPosition = other.d_contentPosition;
        d_contentLength    = other.d_contentLength;
    }

    return *this;
}

void AbstractSyntaxDecoderFrame::reset()
{
    d_tagClass  = AbstractSyntaxTagClass::e_UNIVERSAL;
    d_tagType   = AbstractSyntaxTagType::e_PRIMITIVE;
    d_tagNumber = AbstractSyntaxTagNumber::e_NULL;
    d_tagPosition = 0;
    d_tagLength = 0;
    d_contentPosition = 0;
    d_contentLength.reset();
}

void AbstractSyntaxDecoderFrame::setTagClass(
    AbstractSyntaxTagClass::Value value)
{
    d_tagClass = value;
}

void AbstractSyntaxDecoderFrame::setTagType(AbstractSyntaxTagType::Value value)
{
    d_tagType = value;
}

void AbstractSyntaxDecoderFrame::setTagNumber(
    AbstractSyntaxTagNumber::Value value)
{
    d_tagNumber = value;
}

void AbstractSyntaxDecoderFrame::setTagNumber(bsl::size_t value)
{
    d_tagNumber = value;
}

void AbstractSyntaxDecoderFrame::setTagPosition(bsl::uint64_t value)
{
    d_tagPosition = value;
}

void AbstractSyntaxDecoderFrame::setTagLength(bsl::size_t value)
{
    d_tagLength = value;
}

void AbstractSyntaxDecoderFrame::setContentPosition(bsl::uint64_t value)
{
    d_contentPosition = value;
}

void AbstractSyntaxDecoderFrame::setContentLength(bsl::size_t value)
{
    d_contentLength = value;
}

AbstractSyntaxTagClass::Value AbstractSyntaxDecoderFrame::tagClass() const
{
    return d_tagClass;
}

AbstractSyntaxTagType::Value AbstractSyntaxDecoderFrame::tagType() const
{
    return d_tagType;
}

bsl::size_t AbstractSyntaxDecoderFrame::tagNumber() const
{
    return d_tagNumber;
}

bsl::uint64_t AbstractSyntaxDecoderFrame::tagPosition() const
{
    return d_tagPosition;
}

bsl::size_t AbstractSyntaxDecoderFrame::tagLength() const
{
    return d_tagLength;
}

bsl::uint64_t AbstractSyntaxDecoderFrame::contentPosition() const
{
    return d_contentPosition;
}

const bdlb::NullableValue<bsl::size_t>& AbstractSyntaxDecoderFrame::contentLength()
    const
{
    return d_contentLength;
}

bool AbstractSyntaxDecoderFrame::equals(
    const AbstractSyntaxDecoderFrame& other) const
{
    return d_tagClass == other.d_tagClass && 
           d_tagType == other.d_tagType &&
           d_tagNumber == other.d_tagNumber && 
           d_tagPosition == other.d_tagPosition &&
           d_tagLength == other.d_tagLength &&
           d_contentPosition == other.d_contentPosition &&
           d_contentLength == other.d_contentLength;
}

bool AbstractSyntaxDecoderFrame::less(
    const AbstractSyntaxDecoderFrame& other) const
{
    if (d_tagClass < other.d_tagClass) {
        return true;
    }

    if (other.d_tagClass < d_tagClass) {
        return false;
    }

    if (d_tagType < other.d_tagType) {
        return true;
    }

    if (other.d_tagType < d_tagType) {
        return false;
    }

    if (d_tagNumber < other.d_tagNumber) {
        return true;
    }

    if (other.d_tagNumber < d_tagNumber) {
        return false;
    }

    if (d_tagPosition < other.d_tagPosition) {
        return true;
    }

    if (other.d_tagPosition < d_tagPosition) {
        return false;
    }

    if (d_tagLength < other.d_tagLength) {
        return true;
    }

    if (other.d_tagLength < d_tagLength) {
        return false;
    }

    if (d_contentPosition < other.d_contentPosition) {
        return true;
    }

    if (other.d_contentPosition < d_contentPosition) {
        return false;
    }

    return d_contentLength < other.d_contentLength;
}

bsl::ostream& AbstractSyntaxDecoderFrame::print(bsl::ostream& stream,
                                                int           level,
                                                int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("class", d_tagClass);
    printer.printAttribute("type", d_tagType);

    if (d_tagClass == AbstractSyntaxTagClass::e_UNIVERSAL && 
        d_tagNumber <= 30) 
    {
        printer.printAttribute("number",
                               static_cast<AbstractSyntaxTagNumber::Value>(
                                   static_cast<int>(d_tagNumber)));
    }
    else {
        printer.printAttribute("number", d_tagNumber);
    }

    if (!d_contentLength.isNull()) {
        printer.printAttribute("length", d_contentLength);
    }

    printer.printAttribute("position", d_tagPosition);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const AbstractSyntaxDecoderFrame& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractSyntaxDecoderFrame& lhs,
                const AbstractSyntaxDecoderFrame& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractSyntaxDecoderFrame& lhs,
                const AbstractSyntaxDecoderFrame& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractSyntaxDecoderFrame& lhs,
               const AbstractSyntaxDecoderFrame& rhs)
{
    return lhs.less(rhs);
}

AbstractSyntaxDecoder::AbstractSyntaxDecoder(bsl::streambuf*   buffer,
                                             bslma::Allocator* basicAllocator)
: d_buffer_p(buffer)
, d_contextStack(basicAllocator)
, d_contextDefault()
, d_config(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTSCFG_WARNING_UNUSED(d_allocator_p);
}

AbstractSyntaxDecoder::AbstractSyntaxDecoder(
    const AbstractSyntaxDecoderOptions& configuration,
    bsl::streambuf*                     buffer,
    bslma::Allocator*                   basicAllocator)
: d_buffer_p(buffer)
, d_contextStack(basicAllocator)
, d_contextDefault()
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTSCFG_WARNING_UNUSED(d_allocator_p);
}

AbstractSyntaxDecoder::~AbstractSyntaxDecoder()
{
}

ntsa::Error AbstractSyntaxDecoder::decodeTag()
{
    ntsa::Error error;

    ntsa::AbstractSyntaxTagClass::Value tagClass =
        ntsa::AbstractSyntaxTagClass::e_UNIVERSAL;

    ntsa::AbstractSyntaxTagType::Value tagType =
        ntsa::AbstractSyntaxTagType::e_PRIMITIVE;

    bsl::size_t tagNumber = 0;

    bsl::uint64_t tagPosition = 
        AbstractSyntaxDecoderUtil::position(d_buffer_p);

    error = AbstractSyntaxDecoderUtil::decodeTag(&tagClass,
                                                 &tagType,
                                                 &tagNumber,
                                                 d_buffer_p);
    if (error) {
        return error;
    }

    bdlb::NullableValue<bsl::size_t> length;
    error = AbstractSyntaxDecoderUtil::decodeLength(&length, d_buffer_p);
    if (error) {
        return error;
    }

    bsl::uint64_t contentPosition = 
        AbstractSyntaxDecoderUtil::position(d_buffer_p);

    d_contextStack.resize(d_contextStack.size() + 1);
    AbstractSyntaxDecoderFrame* context = &d_contextStack.back();

    context->setTagClass(tagClass);
    context->setTagType(tagType);
    context->setTagNumber(tagNumber);
    context->setTagPosition(tagPosition);
    context->setTagLength(contentPosition - tagPosition);
    context->setContentPosition(contentPosition);

    if (!length.isNull()) {
        context->setContentLength(length.value());
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeTag(
    AbstractSyntaxDecoderFrame* result)
{
    ntsa::Error error;

    result->reset();

    error = this->decodeTag();
    if (error) {
        return error;
    }

    *result = this->current();
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeTag(
    AbstractSyntaxTagClass::Value  tagClass,
    AbstractSyntaxTagType::Value   tagType,
    AbstractSyntaxTagNumber::Value tagNumber)
{
    ntsa::Error error;

    error = this->decodeTag();
    if (error) {
        return error;
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() != tagClass) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.tagType() != tagType) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.tagNumber() != tagNumber) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeTag(
    AbstractSyntaxTagClass::Value tagClass,
    AbstractSyntaxTagType::Value  tagType,
    bsl::size_t                   tagNumber)
{
    ntsa::Error error;

    error = this->decodeTag();
    if (error) {
        return error;
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() != tagClass) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.tagType() != tagType) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.tagNumber() != tagNumber) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeNull()
{
    ntsa::Error error;

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_NULL) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeEnd()
{
    ntsa::Error error;

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_END_OF_CONTENTS)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::uint8_t nextOctet = 0;
    error = AbstractSyntaxDecoderUtil::read(&nextOctet, d_buffer_p);
    if (error) {
        return error;
    }

    if (nextOctet != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(bool* result)
{
    ntsa::Error error;

    *result = false;

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_BOOLEAN) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::uint8_t nextOctet = 0;
    error = AbstractSyntaxDecoderUtil::read(&nextOctet, d_buffer_p);
    if (error) {
        return error;
    }

    if (nextOctet == 0) {
        *result = false;
    }
    else if (nextOctet == 1) {
        *result = true;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(short* result)
{
    ntsa::Error error;

    long long temp;
    error = this->decodeValue(&temp);
    if (error) {
        return error;
    }

    if (temp < bsl::numeric_limits<short>::min() ||
        temp > bsl::numeric_limits<short>::max())
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = static_cast<short>(temp);
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(unsigned short* result)
{
    ntsa::Error error;

    unsigned long long temp;
    error = this->decodeValue(&temp);
    if (error) {
        return error;
    }

    if (temp < bsl::numeric_limits<unsigned short>::min() ||
        temp > bsl::numeric_limits<unsigned short>::max())
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = static_cast<unsigned short>(temp);
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(int* result)
{
    ntsa::Error error;

    long long temp;
    error = this->decodeValue(&temp);
    if (error) {
        return error;
    }

    if (temp < bsl::numeric_limits<int>::min() ||
        temp > bsl::numeric_limits<int>::max())
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = static_cast<int>(temp);
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(unsigned int* result)
{
    ntsa::Error error;

    unsigned long long temp;
    error = this->decodeValue(&temp);
    if (error) {
        return error;
    }

    if (temp < bsl::numeric_limits<unsigned int>::min() ||
        temp > bsl::numeric_limits<unsigned int>::max())
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = static_cast<unsigned int>(temp);
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(long* result)
{
    ntsa::Error error;

    long long temp;
    error = this->decodeValue(&temp);
    if (error) {
        return error;
    }

    if (temp < bsl::numeric_limits<long>::min() ||
        temp > bsl::numeric_limits<long>::max())
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = static_cast<long>(temp);
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(unsigned long* result)
{
    ntsa::Error error;

    unsigned long long temp;
    error = this->decodeValue(&temp);
    if (error) {
        return error;
    }

    if (temp < bsl::numeric_limits<unsigned long>::min() ||
        temp > bsl::numeric_limits<unsigned long>::max())
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *result = static_cast<unsigned long>(temp);
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(long long* result)
{
    ntsa::Error error;

    *result = 0;

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_INTEGER) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    long long    value        = 0;
    bsl::uint8_t nextOctet    = 0;
    bsl::size_t  numRemaining = context.contentLength().value();

    error = AbstractSyntaxDecoderUtil::read(&nextOctet, d_buffer_p);
    if (error) {
        return error;
    }

    if ((nextOctet & 0x80) != 0) {
        value = -1;
    }

    while (true) {
        value <<= 8;
        value  |= nextOctet;

        if (--numRemaining != 0) {
            error = AbstractSyntaxDecoderUtil::read(&nextOctet, d_buffer_p);
            if (error) {
                return error;
            }
        }
        else {
            break;
        }
    }

    *result = value;

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(
    unsigned long long* result)
{
    ntsa::Error error;

    *result = 0;

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_INTEGER) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    unsigned long long value        = 0;
    bsl::uint8_t       nextOctet    = 0;
    bsl::size_t        numRemaining = context.contentLength().value();

    error = AbstractSyntaxDecoderUtil::read(&nextOctet, d_buffer_p);
    if (error) {
        return error;
    }

    if ((nextOctet & 0x80) != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    while (true) {
        value <<= 8;
        value  |= nextOctet;

        if (--numRemaining != 0) {
            error = AbstractSyntaxDecoderUtil::read(&nextOctet, d_buffer_p);
            if (error) {
                return error;
            }
        }
        else {
            break;
        }
    }

    *result = value;

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(bsl::string* result)
{
    ntsa::Error error;

    result->clear();

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_UTF8_STRING &&
            context.tagNumber() != AbstractSyntaxTagNumber::e_VISIBLE_STRING &&
            context.tagNumber() != AbstractSyntaxTagNumber::e_PRINTABLE_STRING)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    result->resize(context.contentLength().value());

    error = AbstractSyntaxDecoderUtil::read(result->data(),
                                            context.contentLength().value(),
                                            d_buffer_p);
    if (error) {
        result->clear();
        return error;
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(bdlt::Datetime* result)
{
    ntsa::Error error;

    bdlt::DatetimeTz dateTimeTz;
    error = this->decodeValue(&dateTimeTz);
    if (error) {
        return error;
    }

    *result = dateTimeTz.utcDatetime();
    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(
    bdlt::DatetimeTz* result)
{
    ntsa::Error error;

    *result = bdlt::DatetimeTz();

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() !=
            AbstractSyntaxTagNumber::e_UTC_TIME &&
            context.tagNumber() != 
            AbstractSyntaxTagNumber::e_GENERALIZED_TIME)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        // Context-specific date/time is not supported.
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() > bdlt::Iso8601Util::k_MAX_STRLEN) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t length = context.contentLength().value();

    char buffer[bdlt::Iso8601Util::k_MAX_STRLEN + 1];
    bsl::memset(buffer, 0, sizeof buffer);
    
    error = AbstractSyntaxDecoderUtil::read(buffer, length, d_buffer_p);
    if (error) {
        return error;
    }

    error = AbstractSyntaxDecoderUtil::decodeDatetimeTz(
        result, 
        context.tagClass(), context.tagType(), context.tagNumber(), 
        buffer, length);
    if (error) {
        return error;
    }

    return ntsa::Error();
}









ntsa::Error AbstractSyntaxDecoder::decodeValue(
    AbstractInteger* result)
{
    ntsa::Error error;

    result->reset();

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_INTEGER) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::vector<bsl::uint8_t> data;
    data.resize(context.contentLength().value());

    error = AbstractSyntaxDecoderUtil::read(&data.front(),
                                            data.size(),
                                            d_buffer_p);
    if (error) {
        return error;
    }

    result->decode(data);

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeValue(AbstractString* result)
{
    ntsa::Error error;

    result->reset();

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_UTF8_STRING &&
            context.tagNumber() != AbstractSyntaxTagNumber::e_VISIBLE_STRING &&
            context.tagNumber() != AbstractSyntaxTagNumber::e_PRINTABLE_STRING)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    result->resize(context.contentLength().value());

    error = AbstractSyntaxDecoderUtil::read(
        const_cast<bsl::uint8_t*>(result->data()),
        context.contentLength().value(),
        d_buffer_p);
    if (error) {
        result->reset();
        return error;
    }

    return ntsa::Error();
}









ntsa::Error AbstractSyntaxDecoder::decodeValue(AbstractBitSequence* result)
{
    ntsa::Error error;

    result->reset();

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_BIT_STRING)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    result->resize(context.contentLength().value());

    error = AbstractSyntaxDecoderUtil::read(
        const_cast<bsl::uint8_t*>(result->data()),
        context.contentLength().value(),
        d_buffer_p);
    if (error) {
        result->reset();
        return error;
    }

    return ntsa::Error();
}





ntsa::Error AbstractSyntaxDecoder::decodeValue(AbstractByteSequence* result)
{
    ntsa::Error error;

    result->reset();

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() != AbstractSyntaxTagNumber::e_OCTET_STRING)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    result->resize(context.contentLength().value());

    error = AbstractSyntaxDecoderUtil::read(
        const_cast<bsl::uint8_t*>(result->data()),
        context.contentLength().value(),
        d_buffer_p);
    if (error) {
        result->reset();
        return error;
    }

    return ntsa::Error();
}







ntsa::Error AbstractSyntaxDecoder::decodeValue(
    AbstractObjectIdentifier* result)
{
    ntsa::Error error;

    result->reset();

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.tagClass() == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (context.tagNumber() !=
            AbstractSyntaxTagNumber::e_OBJECT_IDENTIFIER)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (context.tagType() != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (context.contentLength().value() == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t numBytesRemaining = context.contentLength().value();

    bsl::uint8_t nextOctet = 0;
    error = AbstractSyntaxDecoderUtil::read(&nextOctet, d_buffer_p);
    if (error) {
        return error;
    }

    --numBytesRemaining;

    if (nextOctet >= 80) {
        result->append(2);
        result->append(nextOctet - 80);
    }
    else {
        result->append(nextOctet / 40);
        result->append(nextOctet % 40);
    }

    while (numBytesRemaining > 0) {
        bsl::uint64_t value = 0;

        while (true) {
            bsl::uint8_t nextOctet;
            error = AbstractSyntaxDecoderUtil::read(&nextOctet, d_buffer_p);
            if (error) {
                return error;
            }

            --numBytesRemaining;

            value <<= 7;
            value  |= nextOctet & 0x7F;

            if ((nextOctet & 0x80) == 0) {
                break;
            }
        }

        result->append(value);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::skip()
{
    ntsa::Error error;

    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const AbstractSyntaxDecoderFrame& context = this->current();

    if (context.contentLength().isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::uint64_t startPosition = context.contentPosition();

    const bsl::uint64_t currentPosition = 
        AbstractSyntaxDecoderUtil::position(d_buffer_p);

    const bsl::uint64_t endPosition = 
        startPosition + context.contentLength().value();

    if (currentPosition > endPosition) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (currentPosition < endPosition) {
        error = AbstractSyntaxDecoderUtil::seek(d_buffer_p, endPosition);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoder::decodeTagComplete()
{
    if (d_contextStack.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_contextStack.pop_back();

    return ntsa::Error();
}


ntsa::Error AbstractSyntaxDecoder::seek(bsl::uint64_t position)
{
    return AbstractSyntaxDecoderUtil::seek(d_buffer_p, position);
}

bsl::uint64_t AbstractSyntaxDecoder::position() const
{
    return AbstractSyntaxDecoderUtil::position(d_buffer_p);
}

bsl::size_t AbstractSyntaxDecoder::depth() const
{
    return d_contextStack.size();
}

const AbstractSyntaxDecoderFrame& AbstractSyntaxDecoder::current() const
{
    if (!d_contextStack.empty()) {
        return d_contextStack.back();
    }
    else {
        return d_contextDefault;
    }
}

const AbstractSyntaxDecoderOptions& AbstractSyntaxDecoder::configuration()
    const
{
    return d_config;
}

bsl::streambuf* AbstractSyntaxDecoder::buffer() const
{
    return d_buffer_p;
}


bsl::uint64_t AbstractSyntaxDecoderUtil::position(bsl::streambuf* source)
{
    bsl::streampos result = 
        source->pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);

    if (result < 0) {
        return 0;
    }

    return static_cast<bsl::uint64_t>(result);
}

ntsa::Error AbstractSyntaxDecoderUtil::seek(bsl::streambuf* source, 
                                            bsl::uint64_t   position)
{
    bsl::streampos result = 
        source->pubseekpos(static_cast<bsl::streampos>(position), 
                           bsl::ios_base::in);

    if (result < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoderUtil::read(bsl::uint8_t*   result,
                                            bsl::streambuf* source)
{
    bsl::streambuf::int_type meta = source->sbumpc();
    if (bsl::streambuf::traits_type::eq_int_type(
            meta,
            bsl::streambuf::traits_type::eof()))
    {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *result = static_cast<bsl::uint8_t>(
        bsl::streambuf::traits_type::to_char_type(meta));

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoderUtil::read(void*           result,
                                            bsl::size_t     size,
                                            bsl::streambuf* source)
{
    bsl::streamsize n = source->sgetn(reinterpret_cast<char*>(result),
                                      static_cast<bsl::streamsize>(size));

    if (static_cast<bsl::size_t>(n) != size) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoderUtil::decodeTag(
    ntsa::AbstractSyntaxTagClass::Value* tagClass,
    ntsa::AbstractSyntaxTagType::Value*  tagType,
    bsl::size_t*                         tagNumber,
    bsl::streambuf*                      source)
{
    ntsa::Error error;

    *tagClass  = ntsa::AbstractSyntaxTagClass::e_UNIVERSAL;
    *tagType   = ntsa::AbstractSyntaxTagType::e_PRIMITIVE;
    *tagNumber = 0;

    bsl::uint8_t nextOctet;

    error = AbstractSyntaxDecoderUtil::read(&nextOctet, source);
    if (error) {
        return error;
    }

    bsl::size_t tagClassCandidate = nextOctet & k_TAG_MASK_CLASS;
    error = AbstractSyntaxTagClass::fromValue(tagClass, tagClassCandidate);
    if (error) {
        return error;
    }

    bsl::size_t tagTypeCandidate = nextOctet & k_TAG_MASK_TYPE;
    error = AbstractSyntaxTagType::fromValue(tagType, tagTypeCandidate);
    if (error) {
        return error;
    }

    bsl::size_t tagNumberCandidate = nextOctet & k_TAG_MASK_NUMBER;

    if (tagNumberCandidate == k_TAG_MASK_NUMBER) {
        tagNumberCandidate = 0;

        bsl::size_t index = 0;
        while (true) {
            if (index == k_MAX_TAG_NUMBER_OCTETS) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            error = AbstractSyntaxDecoderUtil::read(&nextOctet, source);
            if (error) {
                return error;
            }

            tagNumberCandidate <<= k_NUM_VALUE_BITS_IN_TAG_OCTET;
            tagNumberCandidate  |= nextOctet & 0x7F;

            if ((nextOctet & 0x80) == 0) {
                break;
            }

            ++index;
        }
    }

    error = AbstractSyntaxTagNumber::validate(tagNumberCandidate);
    if (error) {
        return error;
    }

    *tagNumber = tagNumberCandidate;

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoderUtil::decodeLength(
    bdlb::NullableValue<bsl::size_t>* result,
    bsl::streambuf*                   source)
{
    ntsa::Error error;

    result->reset();

    bsl::uint8_t nextOctet = 0;
    bsl::size_t  numRead   = 0;

    NTSCFG_WARNING_UNUSED(numRead);

    error = AbstractSyntaxDecoderUtil::read(&nextOctet, source);
    if (error) {
        return error;
    }

    ++numRead;

    if (nextOctet != 0x80) {
        bsl::size_t numOctets = nextOctet;

        if ((numOctets & 0x80) == 0) {
            result->makeValue(numOctets);
        }
        else {
            numOctets &= 0x7F;
            if (numOctets > 4) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::size_t lengthCandidate = 0;

            for (bsl::size_t i = 0; i < numOctets; ++i) {
                error = AbstractSyntaxDecoderUtil::read(&nextOctet, source);
                if (error) {
                    return error;
                }

                ++numRead;

                lengthCandidate <<= 8;
                lengthCandidate  |= nextOctet;
            }

            result->makeValue(lengthCandidate);
        }
    }

    return ntsa::Error();
}

ntsa::Error AbstractSyntaxDecoderUtil::decodeIntegerBase128(
    bsl::uint64_t*  result,
    bsl::streambuf* source)
{
    ntsa::Error error;

    *result = 0;

    while (true) {
        bsl::uint8_t nextOctet;
        error = AbstractSyntaxDecoderUtil::read(&nextOctet, source);
        if (error) {
            return error;
        }

        *result <<= 7;
        *result  |= nextOctet & 0x7F;

        if ((nextOctet & 0x80) == 0) {
            break;
        }
    }

    return ntsa::Error();
}


ntsa::Error AbstractSyntaxDecoderUtil::decodeDatetimeTz(
    bdlt::DatetimeTz*                   result, 
    ntsa::AbstractSyntaxTagClass::Value tagClass,
    ntsa::AbstractSyntaxTagType::Value  tagType,
    bsl::size_t                         tagNumber,
    const char*                         buffer, 
    bsl::size_t                         length)
{
    ntsa::Error error;
    int         rc;

    enum Format {
        e_UNKNOWN,
        e_UTC,
        e_GENERALIZED,
        e_ISO8601
    };

    *result = bdlt::DatetimeTz();

    Format format = e_UNKNOWN;

    if (tagClass == AbstractSyntaxTagClass::e_UNIVERSAL) {
        if (tagNumber == AbstractSyntaxTagNumber::e_UTC_TIME) {
            format = e_UTC;
        }
        else if (tagNumber == AbstractSyntaxTagNumber::e_GENERALIZED_TIME) {
            format = e_GENERALIZED;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (tagClass == AbstractSyntaxTagClass::e_CONTEXT_SPECIFIC) {
        format = e_ISO8601;
    }

    if (tagType != AbstractSyntaxTagType::e_PRIMITIVE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (format == e_UNKNOWN) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (length == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (format == e_ISO8601) {
        rc = bdlt::Iso8601Util::parse(
            result, bsl::string_view(buffer, buffer + length));
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        return ntsa::Error();
    }
    
    const char* begin   = buffer;
    const char* current = begin;
    const char* end     = begin + length;

    const int base = 10;

    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int milliseconds = 0;
    int gmtOffset = 0;

    {
        bsls::Types::Uint64 yearHi = 0;
        {
            const char* mark = current + 2;

            if (current >= end || mark > end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::string_view input(current, mark);
            bsl::string_view remainder;

            rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                &yearHi, &remainder, input, base, 99, 2);
            if (rc != 0 || !remainder.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            current = mark;
        }

        if (format == e_UTC) 
        {
            year = static_cast<int>(2000 + yearHi);
        }
        else if (format == e_GENERALIZED) 
        {
            bsls::Types::Uint64 yearLo = 0;
            {
                const char* mark = current + 2;

                if (current >= end || mark > end) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                bsl::string_view input(current, mark);
                bsl::string_view remainder;

                rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                    &yearLo, &remainder, input, base, 99, 2);
                if (rc != 0 || !remainder.empty()) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                current = mark;
            }

            year = static_cast<int>((100 * yearHi) + yearLo);
        }
        else {
            // Context-specific date/time is not supported.
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    {
        bsls::Types::Uint64 value = 0;
        {
            const char* mark = current + 2;

            if (current >= end || mark > end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::string_view input(current, mark);
            bsl::string_view remainder;

            rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                &value, &remainder, input, base, 12, 2);
            if (rc != 0 || !remainder.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            current = mark;
        }

        month = static_cast<int>(value);
    }

    {
        bsls::Types::Uint64 value = 0;
        {
            const char* mark = current + 2;

            if (current >= end || mark > end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::string_view input(current, mark);
            bsl::string_view remainder;

            rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                &value, &remainder, input, base, 31, 2);
            if (rc != 0 || !remainder.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            current = mark;
        }

        day = static_cast<int>(value);
    }

    {
        bsls::Types::Uint64 value = 0;
        {
            const char* mark = current + 2;

            if (current >= end || mark > end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::string_view input(current, mark);
            bsl::string_view remainder;

            rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                &value, &remainder, input, base, 24, 2);
            if (rc != 0 || !remainder.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            current = mark;
        }

        hour = static_cast<int>(value);
    }

    if (current < end && bdlb::CharType::isDigit(*current)) {
        bsls::Types::Uint64 value = 0;
        {
            const char* mark = current + 2;

            if (current >= end || mark > end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::string_view input(current, mark);
            bsl::string_view remainder;

            rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                &value, &remainder, input, base, 59, 2);
            if (rc != 0 || !remainder.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            current = mark;
        }

        minute = static_cast<int>(value);
    }

    if (current < end && bdlb::CharType::isDigit(*current)) {
        bsls::Types::Uint64 value = 0;
        {
            const char* mark = current + 2;

            if (current >= end || mark > end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::string_view input(current, mark);
            bsl::string_view remainder;

            rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                &value, &remainder, input, base, 59, 2);
            if (rc != 0 || !remainder.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            current = mark;
        }

        second = static_cast<int>(value);
    }

    if (current < end && *current == '.') {
        ++current;

        bsls::Types::Uint64 value = 0;
        {
            const char* mark = current;
            while (mark < end && bdlb::CharType::isDigit(*mark)) {
                ++mark;
            }

            if (current == mark) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::string_view input(current, mark);
            bsl::string_view remainder;

            rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                &value, &remainder, input, base, 999, 3);
            if (rc != 0 || !remainder.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            current = mark;
        }

        milliseconds = static_cast<int>(value);
    }

    if (current < end) {
        if (*current == 'Z') {
            ++current;
        }
        else {
            int gmtSign = 0;
            if (*current == '+') {
                gmtSign = 1;
                ++current;
            }
            else if (*current == '-') {
                gmtSign = -1;
                ++current;
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
            
            int gmtOffsetHour = 0;
            int gmtOffsetMinute = 0;

            {
                bsls::Types::Uint64 value = 0;
                {
                    const char* mark = current + 2;

                    if (current >= end || mark > end) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    bsl::string_view input(current, mark);
                    bsl::string_view remainder;

                    rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                        &value, &remainder, input, base, 24, 2);
                    if (rc != 0 || !remainder.empty()) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    current = mark;
                }

                gmtOffsetHour = static_cast<int>(value);
            }

            {
                bsls::Types::Uint64 value = 0;
                {
                    const char* mark = current + 2;

                    if (current >= end || mark > end) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    bsl::string_view input(current, mark);
                    bsl::string_view remainder;

                    rc = bdlb::NumericParseUtil::parseUnsignedInteger(
                        &value, &remainder, input, base, 59, 2);
                    if (rc != 0 || !remainder.empty()) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    current = mark;
                }

                gmtOffsetMinute = static_cast<int>(value);
            }

            gmtOffset = gmtSign * ((gmtOffsetHour * 60) + gmtOffsetMinute);
        }
    }

    if (current != end) {
        // Internal implementation error: unconsumed data
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bdlt::Datetime localDatetime;
    rc = localDatetime.setDatetimeIfValid(
        year, month, day, hour, minute, second, milliseconds);
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = result->setDatetimeTzIfValid(localDatetime, gmtOffset);
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

AbstractObjectIdentifier::AbstractObjectIdentifier(
    bslma::Allocator* basicAllocator)
: d_data(basicAllocator)
{
}

AbstractObjectIdentifier::AbstractObjectIdentifier(
    const AbstractObjectIdentifier& original,
    bslma::Allocator*               basicAllocator)
: d_data(original.d_data, basicAllocator)
{
}

AbstractObjectIdentifier::~AbstractObjectIdentifier()
{
}

AbstractObjectIdentifier& AbstractObjectIdentifier::operator=(
    const AbstractObjectIdentifier& other)
{
    if (this != &other) {
        d_data = other.d_data;
    }

    return *this;
}

void AbstractObjectIdentifier::reset()
{
    d_data.clear();
}

void AbstractObjectIdentifier::resize(bsl::size_t size)
{
    d_data.resize(size);
}

void AbstractObjectIdentifier::append(bsl::uint64_t value)
{
    d_data.push_back(value);
}

void AbstractObjectIdentifier::append(const bsl::uint64_t* data,
                                      bsl::size_t          size)
{
    d_data.insert(d_data.end(), data, data + size);
}

void AbstractObjectIdentifier::set(bsl::size_t index, bsl::uint64_t value)
{
    if (index >= d_data.size()) {
        d_data.resize(index + 1);
    }

    BSLS_ASSERT_OPT(index < d_data.size());
    d_data[index] = value;
}

bsl::uint64_t AbstractObjectIdentifier::get(bsl::size_t index) const
{
    if (index < d_data.size()) {
        return d_data[index];
    }
    else {
        return 0;
    }
}

const bsl::uint64_t* AbstractObjectIdentifier::data() const
{
    if (!d_data.empty()) {
        return &d_data.front();
    }
    else {
        return 0;
    }
}

bsl::size_t AbstractObjectIdentifier::size() const
{
    return d_data.size();
}

bool AbstractObjectIdentifier::equals(
    const AbstractObjectIdentifier& other) const
{
    return (d_data == other.d_data);
}

bool AbstractObjectIdentifier::less(
    const AbstractObjectIdentifier& other) const
{
    return d_data < other.d_data;
}

bsl::ostream& AbstractObjectIdentifier::print(bsl::ostream& stream,
                                              int           level,
                                              int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("data", d_data);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const AbstractObjectIdentifier& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractObjectIdentifier& lhs,
                const AbstractObjectIdentifier& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractObjectIdentifier& lhs,
                const AbstractObjectIdentifier& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractObjectIdentifier& lhs,
               const AbstractObjectIdentifier& rhs)
{
    return lhs.less(rhs);
}

AbstractString::AbstractString(bslma::Allocator* basicAllocator)
: d_type(AbstractSyntaxTagNumber::e_OCTET_STRING)
, d_data(basicAllocator)
{
}

AbstractString::AbstractString(const AbstractString& original,
                               bslma::Allocator*     basicAllocator)
: d_type(original.d_type)
, d_data(original.d_data, basicAllocator)
{
}

AbstractString::~AbstractString()
{
}

AbstractString& AbstractString::operator=(const AbstractString& other)
{
    if (this != &other) {
        d_type = other.d_type;
        d_data = other.d_data;
    }

    return *this;
}

void AbstractString::reset()
{
    d_type = AbstractSyntaxTagNumber::e_OCTET_STRING;
    d_data.clear();
}

void AbstractString::resize(bsl::size_t size)
{
    d_data.resize(size);
}

void AbstractString::append(bsl::uint8_t value)
{
    d_data.push_back(value);
}

void AbstractString::set(bsl::size_t index, bsl::uint8_t value)
{
    if (index >= d_data.size()) {
        d_data.resize(index + 1);
    }

    BSLS_ASSERT_OPT(index < d_data.size());
    d_data[index] = value;
}

void AbstractString::setType(AbstractSyntaxTagNumber::Value value)
{
    d_type = value;
}

AbstractSyntaxTagNumber::Value AbstractString::type() const
{
    return d_type;
}

bsl::uint8_t AbstractString::get(bsl::size_t index) const
{
    if (index < d_data.size()) {
        return d_data[index];
    }
    else {
        return 0;
    }
}

const bsl::uint8_t* AbstractString::data() const
{
    if (!d_data.empty()) {
        return &d_data.front();
    }
    else {
        return 0;
    }
}

bsl::size_t AbstractString::size() const
{
    return d_data.size();
}

ntsa::Error AbstractString::convert(bsl::string* result) const
{
    result->clear();

    if (d_type != AbstractSyntaxTagNumber::e_PRINTABLE_STRING &&
        d_type != AbstractSyntaxTagNumber::e_CHARACTER_STRING &&
        d_type != AbstractSyntaxTagNumber::e_UTF8_STRING)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_data.empty()) {
        result->assign(reinterpret_cast<const char*>(&d_data.front()),
                       d_data.size());
    }

    return ntsa::Error();
}

bool AbstractString::equals(const AbstractString& other) const
{
    return (d_data == other.d_data);
}

bool AbstractString::less(const AbstractString& other) const
{
    return d_data < other.d_data;
}

bsl::ostream& AbstractString::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("data", d_data);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const AbstractString& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractString& lhs, const AbstractString& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractString& lhs, const AbstractString& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractString& lhs, const AbstractString& rhs)
{
    return lhs.less(rhs);
}























AbstractBitSequence::AbstractBitSequence(bslma::Allocator* basicAllocator)
: d_type(AbstractSyntaxTagNumber::e_BIT_STRING)
, d_data(basicAllocator)
{
}

AbstractBitSequence::AbstractBitSequence(const AbstractBitSequence& original,
                               bslma::Allocator*     basicAllocator)
: d_type(original.d_type)
, d_data(original.d_data, basicAllocator)
{
}

AbstractBitSequence::~AbstractBitSequence()
{
}

AbstractBitSequence& AbstractBitSequence::operator=(const AbstractBitSequence& other)
{
    if (this != &other) {
        d_type = other.d_type;
        d_data = other.d_data;
    }

    return *this;
}

void AbstractBitSequence::reset()
{
    d_type = AbstractSyntaxTagNumber::e_BIT_STRING;
    d_data.clear();
}

void AbstractBitSequence::resize(bsl::size_t size)
{
    d_data.resize(size);
}

void AbstractBitSequence::append(AbstractBit value)
{
    d_data.push_back(value);
}

void AbstractBitSequence::set(bsl::size_t index, AbstractBit value)
{
    if (index >= d_data.size()) {
        d_data.resize(index + 1);
    }

    BSLS_ASSERT_OPT(index < d_data.size());
    d_data[index] = value;
}

void AbstractBitSequence::setType(AbstractSyntaxTagNumber::Value value)
{
    d_type = value;
}

AbstractSyntaxTagNumber::Value AbstractBitSequence::type() const
{
    return d_type;
}

AbstractBit AbstractBitSequence::get(bsl::size_t index) const
{
    if (index < d_data.size()) {
        return d_data[index];
    }
    else {
        return 0;
    }
}

const bsl::uint8_t* AbstractBitSequence::data() const
{
    if (!d_data.empty()) {
        return &d_data.front();
    }
    else {
        return 0;
    }
}

bsl::size_t AbstractBitSequence::size() const
{
    return d_data.size();
}

ntsa::Error AbstractBitSequence::convert(bsl::string* result) const
{
    result->clear();

    if (d_type != AbstractSyntaxTagNumber::e_PRINTABLE_STRING &&
        d_type != AbstractSyntaxTagNumber::e_CHARACTER_STRING &&
        d_type != AbstractSyntaxTagNumber::e_UTF8_STRING)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_data.empty()) {
        result->assign(reinterpret_cast<const char*>(&d_data.front()),
                       d_data.size());
    }

    return ntsa::Error();
}

bool AbstractBitSequence::equals(const AbstractBitSequence& other) const
{
    return (d_data == other.d_data);
}

bool AbstractBitSequence::less(const AbstractBitSequence& other) const
{
    return d_data < other.d_data;
}

bsl::ostream& AbstractBitSequence::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("data", d_data);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const AbstractBitSequence& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractBitSequence& lhs, const AbstractBitSequence& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractBitSequence& lhs, const AbstractBitSequence& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractBitSequence& lhs, const AbstractBitSequence& rhs)
{
    return lhs.less(rhs);
}
























AbstractByteSequence::AbstractByteSequence(bslma::Allocator* basicAllocator)
: d_type(AbstractSyntaxTagNumber::e_OCTET_STRING)
, d_data(basicAllocator)
{
}

AbstractByteSequence::AbstractByteSequence(const AbstractByteSequence& original,
                               bslma::Allocator*     basicAllocator)
: d_type(original.d_type)
, d_data(original.d_data, basicAllocator)
{
}

AbstractByteSequence::~AbstractByteSequence()
{
}

AbstractByteSequence& AbstractByteSequence::operator=(const AbstractByteSequence& other)
{
    if (this != &other) {
        d_type = other.d_type;
        d_data = other.d_data;
    }

    return *this;
}

void AbstractByteSequence::reset()
{
    d_type = AbstractSyntaxTagNumber::e_OCTET_STRING;
    d_data.clear();
}

void AbstractByteSequence::resize(bsl::size_t size)
{
    d_data.resize(size);
}

void AbstractByteSequence::append(AbstractByte value)
{
    d_data.push_back(value);
}

void AbstractByteSequence::set(bsl::size_t index, AbstractByte value)
{
    if (index >= d_data.size()) {
        d_data.resize(index + 1);
    }

    BSLS_ASSERT_OPT(index < d_data.size());
    d_data[index] = value;
}

void AbstractByteSequence::setType(AbstractSyntaxTagNumber::Value value)
{
    d_type = value;
}

AbstractSyntaxTagNumber::Value AbstractByteSequence::type() const
{
    return d_type;
}

AbstractByte AbstractByteSequence::get(bsl::size_t index) const
{
    if (index < d_data.size()) {
        return d_data[index];
    }
    else {
        return 0;
    }
}

const bsl::uint8_t* AbstractByteSequence::data() const
{
    if (!d_data.empty()) {
        return &d_data.front();
    }
    else {
        return 0;
    }
}

bsl::size_t AbstractByteSequence::size() const
{
    return d_data.size();
}

ntsa::Error AbstractByteSequence::convert(bsl::string* result) const
{
    result->clear();

    if (d_type != AbstractSyntaxTagNumber::e_PRINTABLE_STRING &&
        d_type != AbstractSyntaxTagNumber::e_CHARACTER_STRING &&
        d_type != AbstractSyntaxTagNumber::e_UTF8_STRING)
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!d_data.empty()) {
        result->assign(reinterpret_cast<const char*>(&d_data.front()),
                       d_data.size());
    }

    return ntsa::Error();
}

bool AbstractByteSequence::equals(const AbstractByteSequence& other) const
{
    return (d_data == other.d_data);
}

bool AbstractByteSequence::less(const AbstractByteSequence& other) const
{
    return d_data < other.d_data;
}

bsl::ostream& AbstractByteSequence::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("data", d_data);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const AbstractByteSequence& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractByteSequence& lhs, const AbstractByteSequence& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractByteSequence& lhs, const AbstractByteSequence& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractByteSequence& lhs, const AbstractByteSequence& rhs)
{
    return lhs.less(rhs);
}




















AbstractIntegerSign::Value AbstractIntegerSign::flip(Value sign)
{
    if (sign == e_POSITIVE) {
        return e_NEGATIVE;
    }
    else {
        return e_POSITIVE;
    }
}

int AbstractIntegerSign::multiplier(Value sign)
{
    return static_cast<int>(sign);
}

const char* AbstractIntegerSign::toString(Value value)
{
    switch (value) {
    case e_NEGATIVE: {
        return "NEGATIVE";
    } break;
    case e_POSITIVE: {
        return "POSITIVE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& AbstractIntegerSign::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, AbstractIntegerSign::Value rhs)
{
    return AbstractIntegerSign::print(stream, rhs);
}

bsl::uint64_t AbstractIntegerBase::radix(AbstractIntegerBase::Value base)
{
    return k_TRAITS[static_cast<bsl::size_t>(base)].d_radix;
}

bool AbstractIntegerBase::validate(AbstractIntegerBase::Value base,
                                   bsl::uint64_t              value)
{
    return value <= k_TRAITS[static_cast<bsl::size_t>(base)].d_maxValue;
}

const char* AbstractIntegerBase::toString(Value value)
{
    switch (value) {
    case e_NATIVE: {
        return "NATIVE";
    } break;
    case e_BINARY: {
        return "BINARY";
    } break;
    case e_OCTAL: {
        return "OCTAL";
    } break;
    case e_DECIMAL: {
        return "DECIMAL";
    } break;
    case e_HEXADECIMAL: {
        return "HEXADECIMAL";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& AbstractIntegerBase::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, AbstractIntegerBase::Value rhs)
{
    return AbstractIntegerBase::print(stream, rhs);
}

bsl::size_t AbstractIntegerRepresentation::countLeadingZeroes(
    bsl::uint8_t value)
{
    const bsl::size_t bits = sizeof(bsl::uint8_t) << 3;

    bsl::size_t count = 0;
    bsl::size_t found = 0;

    for (bsl::size_t i = bits - 1; i != bsl::size_t(-1); --i) {
        count += !(found |= value & bsl::uint8_t(1) << i ? 1 : 0);
    }

    return count;
}

bsl::size_t AbstractIntegerRepresentation::countLeadingZeroes(
    bsl::uint16_t value)
{
    const bsl::size_t bits = sizeof(bsl::uint16_t) << 3;

    bsl::size_t count = 0;
    bsl::size_t found = 0;

    for (bsl::size_t i = bits - 1; i != bsl::size_t(-1); --i) {
        count += !(found |= value & bsl::uint16_t(1) << i ? 1 : 0);
    }

    return count;
}

bsl::size_t AbstractIntegerRepresentation::countLeadingZeroes(
    bsl::uint32_t value)
{
    const bsl::size_t bits = sizeof(bsl::uint32_t) << 3;

    bsl::size_t count = 0;
    bsl::size_t found = 0;

    for (bsl::size_t i = bits - 1; i != bsl::size_t(-1); --i) {
        count += !(found |= value & bsl::uint32_t(1) << i ? 1 : 0);
    }

    return count;
}

bsl::size_t AbstractIntegerRepresentation::countLeadingZeroes(
    bsl::uint64_t value)
{
    const bsl::size_t bits = sizeof(bsl::uint64_t) << 3;

    bsl::size_t count = 0;
    bsl::size_t found = 0;

    for (bsl::size_t i = bits - 1; i != bsl::size_t(-1); --i) {
        count += !(found |= value & bsl::uint64_t(1) << i ? 1 : 0);
    }

    return count;
}

AbstractIntegerRepresentation::AbstractIntegerRepresentation(
    bslma::Allocator* basicAllocator)
: d_base(k_DEFAULT_BASE)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractIntegerRepresentation::AbstractIntegerRepresentation(
    AbstractIntegerBase::Value base,
    bslma::Allocator*          basicAllocator)
: d_base(base)
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractIntegerRepresentation::AbstractIntegerRepresentation(
    const AbstractIntegerRepresentation& original,
    bslma::Allocator*                    basicAllocator)
: d_base(original.d_base)
, d_data(original.d_data, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

AbstractIntegerRepresentation::~AbstractIntegerRepresentation()
{
}

AbstractIntegerRepresentation& AbstractIntegerRepresentation::operator=(
    const AbstractIntegerRepresentation& other)
{
    this->assign(other);
    return *this;
}

void AbstractIntegerRepresentation::reset()
{
    d_base = k_DEFAULT_BASE;

    d_data.clear();
    d_data.shrink_to_fit();
}

void AbstractIntegerRepresentation::resize(bsl::size_t size)
{
    d_data.resize(size);
}

void AbstractIntegerRepresentation::swap(AbstractIntegerRepresentation& other)
{
    bsl::swap(d_base, other.d_base);
    d_data.swap(other.d_data);
}

void AbstractIntegerRepresentation::assign(
    const AbstractIntegerRepresentation& other)
{
    if (this != &other) {
        d_base = other.d_base;
        d_data = other.d_data;
    }
}

void AbstractIntegerRepresentation::assign(bsl::uint64_t value)
{
    d_data.clear();

    const bsl::uint64_t radix = this->radix();

    bsl::uint64_t remaining = value;

    while (remaining != 0) {
        bsl::uint64_t place  = remaining % radix;
        remaining           /= radix;

        this->push(place);
    }
}

void AbstractIntegerRepresentation::normalize()
{
    while (!d_data.empty()) {
        if (d_data.back() == 0) {
            d_data.pop_back();
        }
        else {
            break;
        }
    }
}

void AbstractIntegerRepresentation::set(bsl::size_t index, Block value)
{
    BSLS_ASSERT_OPT(ntsa::AbstractIntegerBase::validate(d_base, value));

    if (index >= d_data.size()) {
        d_data.resize(index + 1);
    }

    BSLS_ASSERT_OPT(index < d_data.size());
    d_data[index] = value;
}

void AbstractIntegerRepresentation::push(Block value)
{
    BSLS_ASSERT_OPT(ntsa::AbstractIntegerBase::validate(d_base, value));
    d_data.push_back(value);
}

void AbstractIntegerRepresentation::pop()
{
    d_data.pop_back();
}

AbstractIntegerRepresentation::Block AbstractIntegerRepresentation::get(
    bsl::size_t index) const
{
    if (index < d_data.size()) {
        return static_cast<Block>(d_data[index]);
    }
    else {
        return 0;
    }
}

AbstractIntegerBase::Value AbstractIntegerRepresentation::base() const
{
    return d_base;
}

bsl::uint64_t AbstractIntegerRepresentation::radix() const
{
    return AbstractIntegerBase::radix(d_base);
}

bool AbstractIntegerRepresentation::equals(
    const AbstractIntegerRepresentation& other) const
{
    return d_data == other.d_data;
}

int AbstractIntegerRepresentation::compare(
    const AbstractIntegerRepresentation& other) const
{
    int result = 0;

    const bsl::size_t lhsSize = d_data.size();
    const bsl::size_t rhsSize = other.d_data.size();

    if (lhsSize < rhsSize) {
        result = -1;
    }
    else if (rhsSize < lhsSize) {
        result = +1;
    }
    else {
        bsl::size_t i = lhsSize;
        while (i > 0) {
            --i;
            if (d_data[i] < other.d_data[i]) {
                result = -1;
                break;
            }
            else if (other.d_data[i] < d_data[i]) {
                result = +1;
                break;
            }
            else {
                continue;
            }
        }
    }

    return result;
}

bsl::size_t AbstractIntegerRepresentation::size() const
{
    return d_data.size();
}

bool AbstractIntegerRepresentation::isZero() const
{
    return d_data.empty() || (d_data.size() == 1 && d_data.front() == 0);
}

bool AbstractIntegerRepresentation::isOne() const
{
    return d_data.size() == 1 && d_data.front() == 1;
}

bool AbstractIntegerRepresentation::isNotAliasOf(
    const AbstractIntegerRepresentation* other) const
{
    return this != other;
}

bool AbstractIntegerRepresentation::isNotAliasOf(
    const AbstractIntegerRepresentation& other) const
{
    return this != &other;
}

bslma::Allocator* AbstractIntegerRepresentation::allocator() const
{
    return d_allocator_p;
}

bsl::ostream& AbstractIntegerRepresentation::print(bsl::ostream& stream,
                                                   int           level,
                                                   int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("base", d_base);

    if (!d_data.empty()) {
        printer.printAttribute("data", d_data.rbegin(), d_data.rend());
    }
    else {
        printer.printAttribute("data", static_cast<Block>(0));
    }

    printer.end();
    return stream;
}

void AbstractIntegerRepresentation::add(
    AbstractIntegerRepresentation*       sum,
    const AbstractIntegerRepresentation& addend1,
    const AbstractIntegerRepresentation& addend2)
{
    BSLS_ASSERT_OPT(sum->isNotAliasOf(addend1));
    BSLS_ASSERT_OPT(sum->isNotAliasOf(addend2));

    BSLS_ASSERT_OPT(sum->base() == addend1.base());
    BSLS_ASSERT_OPT(sum->base() == addend2.base());

    sum->reset();

    if (addend1.isZero()) {
        *sum = addend2;
        return;
    }

    if (addend2.isZero()) {
        *sum = addend1;
        return;
    }

    const bsl::uint64_t radix = sum->radix();

    const AbstractIntegerRepresentation& lhs = addend1;
    const AbstractIntegerRepresentation& rhs = addend2;

    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = rhs.size();
    const bsl::size_t maxSize = bsl::max(lhsSize, rhsSize);

    bool carry = false;

    for (bsl::size_t i = 0; i < maxSize; ++i) {
        bsl::uint64_t lhsValue = lhs.get(i);
        bsl::uint64_t rhsValue = rhs.get(i);

        bsl::uint64_t temp = lhsValue + rhsValue;

        if (carry) {
            ++temp;
        }

        if (temp >= radix) {
            temp  -= radix;
            carry  = true;
        }
        else {
            carry = false;
        }

        sum->push(temp);
    }

    if (carry) {
        sum->push(1);
    }

    sum->normalize();
}

void AbstractIntegerRepresentation::subtract(
    AbstractIntegerRepresentation*       difference,
    const AbstractIntegerRepresentation& minuend,
    const AbstractIntegerRepresentation& subtrahend)
{
    BSLS_ASSERT_OPT(difference->isNotAliasOf(minuend));
    BSLS_ASSERT_OPT(difference->isNotAliasOf(subtrahend));

    BSLS_ASSERT_OPT(difference->base() == minuend.base());
    BSLS_ASSERT_OPT(difference->base() == subtrahend.base());

    difference->reset();

    if (subtrahend.isZero()) {
        *difference = minuend;
        return;
    }

    const bsl::uint64_t radix = difference->radix();

    const AbstractIntegerRepresentation& lhs = minuend;
    const AbstractIntegerRepresentation& rhs = subtrahend;

    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = rhs.size();
    const bsl::size_t maxSize = bsl::max(lhsSize, rhsSize);

    if (lhsSize < rhsSize) {
        return;
    }

    const int comparison = lhs.compare(rhs);
    if (comparison < 0) {
        return;
    }

    bool borrow = false;

    for (bsl::size_t i = 0; i < maxSize; ++i) {
        bsl::uint64_t lhsValue = lhs.get(i);
        bsl::uint64_t rhsValue = rhs.get(i);

        bsl::uint64_t temp = lhsValue - rhsValue;

        if (borrow) {
            --temp;
        }

        if (temp >= radix) {
            temp   += radix;
            borrow  = true;
        }
        else {
            borrow = false;
        }

        difference->push(temp);
    }

    difference->normalize();
}

void AbstractIntegerRepresentation::multiply(
    AbstractIntegerRepresentation*       product,
    const AbstractIntegerRepresentation& multiplicand,
    const AbstractIntegerRepresentation& multiplier)
{
    BSLS_ASSERT_OPT(product->isNotAliasOf(multiplicand));
    BSLS_ASSERT_OPT(product->isNotAliasOf(multiplier));

    BSLS_ASSERT_OPT(product->base() == multiplicand.base());
    BSLS_ASSERT_OPT(product->base() == multiplier.base());

    if (multiplicand.isZero()) {
        product->reset();
        return;
    }

    if (multiplicand.isOne()) {
        *product = multiplier;
        return;
    }

    if (multiplier.isZero()) {
        product->reset();
        return;
    }

    if (multiplier.isOne()) {
        *product = multiplicand;
        return;
    }

    const bsl::uint64_t radix = product->radix();
    const bsl::uint64_t b     = radix;

    const AbstractIntegerRepresentation& u = multiplicand;
    const AbstractIntegerRepresentation& v = multiplier;
    AbstractIntegerRepresentation*       w = product;

    const bsl::size_t m = u.size();
    const bsl::size_t n = v.size();

    // M1

    bsl::size_t j = 0;

    while (true) {
        // M2

        if (v.get(j) == 0) {
            w->set(j + m, 0);
            // goto M6
        }
        else {
            // M3

            bsl::uint64_t i = 0;
            bsl::uint64_t k = 0;

            while (true) {
                // M4

                bsl::uint64_t t =
                    bsl::uint64_t(u.get(i)) * bsl::uint64_t(v.get(j)) +
                    w->get(i + j) + k;

                w->set(i + j, t % b);
                k = t / b;

                BSLS_ASSERT_OPT(k >= 0);
                BSLS_ASSERT_OPT(k < b);

                // M5

                ++i;
                if (i < m) {
                    // goto M4
                    continue;
                }
                else {
                    w->set(j + m, k);
                    break;
                }
            }
        }

        // M6

        ++j;
        if (j < n) {
            // goto M2
            continue;
        }
        else {
            // done
            break;
        }
    }

    product->normalize();
}

void AbstractIntegerRepresentation::divide(
    AbstractIntegerRepresentation*       quotient,
    AbstractIntegerRepresentation*       remainder,
    const AbstractIntegerRepresentation& dividend,
    const AbstractIntegerRepresentation& divisor)
{
    // The following implementation is derived from algorithm D ("division of
    // non-negative integers") in section 4.3.1 of Volume 2 of "The Art of
    // Computer Programming", by Donald Knuth, and "Hacker's Delight", by Henry
    // S. Warren, Jr. Note that it is crucial for the correctness of this
    // implementation that numbers are stored in base 2^k_BITS_PER_BLOCK and
    // that the integer storage type for temporaries is the exact type as the
    // block type.

    BSLS_ASSERT_OPT(quotient->isNotAliasOf(dividend));
    BSLS_ASSERT_OPT(quotient->isNotAliasOf(divisor));

    BSLS_ASSERT_OPT(remainder->isNotAliasOf(dividend));
    BSLS_ASSERT_OPT(remainder->isNotAliasOf(divisor));

    BSLS_ASSERT_OPT(quotient->base() == remainder->base());
    BSLS_ASSERT_OPT(quotient->base() == dividend.base());
    BSLS_ASSERT_OPT(quotient->base() == divisor.base());

    quotient->reset();
    remainder->reset();

    if (dividend.isZero()) {
        return;
    }

    if (divisor.isZero()) {
        remainder->assign(dividend);
        return;
    }

    if (dividend.isOne()) {
        if (divisor.isOne()) {
            quotient->assign(1);
        }
        else {
            remainder->assign(dividend);
        }
        return;
    }

    if (divisor.isOne()) {
        quotient->assign(dividend);
        return;
    }

    const int comparison = dividend.compare(divisor);
    if (comparison < 0) {
        *remainder = dividend;
        return;
    }

    AbstractIntegerRepresentation& q = *quotient;
    AbstractIntegerRepresentation& r = *remainder;

    const AbstractIntegerRepresentation& u = dividend;
    const AbstractIntegerRepresentation& v = divisor;

    const bsl::size_t m = u.size();
    const bsl::size_t n = v.size();

    const bsl::uint64_t radix = quotient->radix();
    const bsl::uint64_t b     = radix;

    if (divisor.size() == 1) {
        bsl::uint64_t k = 0;

        for (bsl::size_t j = m - 1; j < m; --j) {
            q.set(j, (k * b + u.get(j)) / v.get(0));
            k = (k * b + u.get(j)) - q.get(j) * v.get(0);
        }

        r.set(0, k);
        q.normalize();
        r.normalize();

        return;
    }

    const bsl::size_t s = countLeadingZeroes(Block(v.get(n - 1)));

    BSLS_ASSERT_OPT(s >= 0);
    BSLS_ASSERT_OPT(s <= k_BITS_PER_BLOCK);

    AbstractIntegerRepresentation vn;

    for (bsl::size_t i = n - 1; i > 0; --i) {
        vn.set(i, (v.get(i) << s) | (v.get(i - 1) >> (k_BITS_PER_BLOCK - s)));
    }

    vn.set(0, v.get(0) << s);

    AbstractIntegerRepresentation un;

    un.set(m, u.get(m - 1) >> (k_BITS_PER_BLOCK - s));
    for (bsl::size_t i = m - 1; i > 0; --i) {
        un.set(i, (u.get(i) << s) | (u.get(i - 1) >> (k_BITS_PER_BLOCK - s)));
    }

    un.set(0, u.get(0) << s);

    for (bsl::size_t j = m - n; j != bsl::size_t(-1); --j) {
        bsl::uint64_t qhat =
            (un.get(j + n) * b + un.get(j + n - 1)) / vn.get(n - 1);

        bsl::uint64_t rhat =
            (un.get(j + n) * b + un.get(j + n - 1)) - qhat * vn.get(n - 1);

        while (true) {
            if (qhat >= b ||
                qhat * vn.get(n - 2) > b * rhat + un.get(j + n - 2))
            {
                qhat = qhat - 1;
                rhat = rhat + vn.get(n - 1);
                if (rhat < b) {
                    continue;
                }
            }
            break;
        }

        typedef bsl::uint64_t U64;
        typedef bsl::int64_t  S64;

        U64 k = 0;
        S64 t = 0;

        for (bsl::size_t i = 0; i < n; ++i) {
            U64 p = qhat * vn.get(i);
            t     = un.get(i + j) - k - (p & ((1ULL << k_BITS_PER_BLOCK) - 1));
            un.set(i + j, Block(t));
            k = (p >> k_BITS_PER_BLOCK) - (t >> k_BITS_PER_BLOCK);
        }

        t = un.get(j + n) - k;
        un.set(j + n, Block(t));

        q.set(j, Block(qhat));

        if (t < 0) {
            q.set(j, q.get(j) - 1);
            k = 0;
            for (bsl::size_t i = 0; i < n; ++i) {
                t = un.get(i + j) + vn.get(i) + k;
                un.set(i + j, Block(t));
                k = t >> k_BITS_PER_BLOCK;
            }
            un.set(j + n, Block(un.get(j + n) + k));
        }
    }

    for (bsl::size_t i = 0; i < n; ++i) {
        r.set(i, (un.get(i) >> s) | (un.get(i + 1) << (k_BITS_PER_BLOCK - s)));
    }

    q.normalize();
    r.normalize();
}

bool AbstractIntegerRepresentation::parse(
    AbstractIntegerRepresentation* result,
    AbstractIntegerSign::Value*    sign,
    const bsl::string_view&        text)
{
    result->reset();
    *sign = AbstractIntegerSign::e_POSITIVE;

    if (text.empty()) {
        return false;
    }

    *sign = AbstractIntegerSign::e_POSITIVE;

    int base = 10;

    bsl::string_view temp;
    {
        bsl::string_view::const_iterator it = text.begin();
        bsl::string_view::const_iterator et = text.end();

        if (it == et) {
            return false;
        }

        if (*it == '0') {
            ++it;
            if (it == et) {
                return true;
            }
            else if (*it == 'x' || *it == 'X') {
                base = 16;
                ++it;
            }
            else {
                return false;
            }
        }

        if (*it == '+') {
            if (base != 10) {
                return false;
            }

            ++it;
        }
        else if (*it == '-') {
            if (base != 10) {
                return false;
            }

            *sign = AbstractIntegerSign::e_NEGATIVE;
            ++it;
        }

        if (it == et) {
            return false;
        }

        temp = bsl::string_view(it, et);
    }

    // Vector of digits in the base, least-significant first.

    typedef bsl::uint64_t      Digit;
    typedef bsl::vector<Digit> DigitVector;

    DigitVector digits;
    {
        bsl::string_view::const_reverse_iterator it = temp.rbegin();
        bsl::string_view::const_reverse_iterator et = temp.rend();

        for (; it != et; ++it) {
            const char ch = *it;

            if (base == 10) {
                if (ch >= '0' && ch <= '9') {
                    Digit digit = ch - '0';
                    digits.push_back(digit);
                }
                else {
                    return false;
                }
            }
            else if (base == 16) {
                if (ch >= '0' && ch <= '9') {
                    Digit digit = ch - '0';
                    digits.push_back(digit);
                }
                else if (ch >= 'a' && ch <= 'f') {
                    Digit digit = ch - 'a';
                    digits.push_back(digit);
                }
                else if (ch >= 'A' && ch <= 'F') {
                    Digit digit = ch - 'A';
                    digits.push_back(digit);
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }

        while (!digits.empty()) {
            if (digits.back() == 0) {
                digits.pop_back();
            }
            else {
                break;
            }
        }
    }

    BSLS_ASSERT_OPT(!digits.empty());

    {
        DigitVector::const_reverse_iterator it = digits.rbegin();
        DigitVector::const_reverse_iterator et = digits.rend();

        for (; it != et; ++it) {
            Digit value = *it;

            {
                AbstractIntegerRepresentation product(result->allocator());

                AbstractIntegerRepresentation lhs;
                lhs.assign(*result);

                AbstractIntegerRepresentation rhs;
                rhs.assign(base);

                AbstractIntegerRepresentation::multiply(&product, lhs, rhs);

                result->swap(product);
            }

            {
                AbstractIntegerRepresentation sum(result->allocator());

                AbstractIntegerRepresentation lhs;
                lhs.assign(*result);

                AbstractIntegerRepresentation rhs;
                rhs.assign(value);

                AbstractIntegerRepresentation::add(&sum, lhs, rhs);

                result->swap(sum);
            }
        }
    }

    result->normalize();

    return true;
}

void AbstractIntegerRepresentation::generate(
    bsl::string*                         result,
    const AbstractIntegerRepresentation& value,
    AbstractIntegerSign::Value           sign,
    AbstractIntegerBase::Value           base)
{
    result->clear();

    const bsl::size_t size = value.size();

    if (size == 0) {
        result->push_back('0');
    }
    else if (base == value.base()) {
        if (sign == AbstractIntegerSign::e_NEGATIVE) {
            result->push_back('-');
        }

        bsl::size_t i = size - 1;
        while (true) {
            bsl::uint64_t temp = value.get(i);

            if (temp < 10) {
                result->push_back(static_cast<char>('0' + temp));
            }
            else {
                result->push_back(static_cast<char>('a' + (temp - 10)));
            }

            if (i == 0) {
                break;
            }

            --i;
        }
    }
    else {
        typedef bsl::uint64_t      Digit;
        typedef bsl::vector<Digit> DigitVector;

        DigitVector digits;
        {
            AbstractIntegerRepresentation dividend(value);
            AbstractIntegerRepresentation divisor;
            divisor.assign(AbstractIntegerBase::radix(base));

            while (!dividend.isZero()) {
                AbstractIntegerRepresentation quotient;
                AbstractIntegerRepresentation remainder;

                AbstractIntegerRepresentation::divide(&quotient,
                                                      &remainder,
                                                      dividend,
                                                      divisor);

                Digit digit = static_cast<Digit>(remainder.get(0));

                digits.push_back(digit);
                dividend.swap(quotient);
            }
        }

        DigitVector::const_iterator it = digits.begin();
        DigitVector::const_iterator et = digits.end();

        for (; it != et; ++it) {
            Digit value = *it;

            if (value < 10) {
                result->push_back(static_cast<char>('0' + value));
            }
            else {
                result->push_back(static_cast<char>('a' + (value - 10)));
            }
        }

        if (sign == AbstractIntegerSign::e_NEGATIVE) {
            result->push_back('-');
        }

        bsl::reverse(result->begin(), result->end());
    }
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const AbstractIntegerRepresentation& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractIntegerRepresentation& lhs,
               const AbstractIntegerRepresentation& rhs)
{
    return lhs.compare(rhs) < 0;
}

bool operator<=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs)
{
    return lhs.compare(rhs) <= 0;
}

bool operator>(const AbstractIntegerRepresentation& lhs,
               const AbstractIntegerRepresentation& rhs)
{
    return lhs.compare(rhs) > 0;
}

bool operator>=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs)
{
    return lhs.compare(rhs) >= 0;
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    short             value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    unsigned short    value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    int               value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    unsigned int      value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    long              value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    unsigned long     value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    long long         value,
    bslma::Allocator* basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    unsigned long long value,
    bslma::Allocator*  basicAllocator)
: d_rep(basicAllocator)
{
    this->assign(value);
}

AbstractIntegerQuantity::AbstractIntegerQuantity(
    const AbstractIntegerQuantity& original,
    bslma::Allocator*              basicAllocator)
: d_rep(original.d_rep, basicAllocator)
{
}

AbstractIntegerQuantity::~AbstractIntegerQuantity()
{
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(
    const AbstractIntegerQuantity& other)
{
    this->assign(other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(short value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(
    unsigned short value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(int value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(unsigned int value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(long value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(
    unsigned long value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(long long value)
{
    this->assign(value);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::operator=(
    unsigned long long value)
{
    this->assign(value);
    return *this;
}

void AbstractIntegerQuantity::reset()
{
    d_rep.reset();
}

void AbstractIntegerQuantity::resize(bsl::size_t size)
{
    d_rep.resize(size);
}

void AbstractIntegerQuantity::swap(AbstractIntegerQuantity& other)
{
    if (this != &other) {
        d_rep.swap(other.d_rep);
    }
}

bool AbstractIntegerQuantity::parse(AbstractIntegerSign::Value* sign,
                                    const bsl::string_view&     text)
{
    return AbstractIntegerRepresentation::parse(&d_rep, sign, text);
}

void AbstractIntegerQuantity::decode(const bsl::vector<bsl::uint8_t>& data)
{
    this->reset();

    if (data.empty()) {
        return;
    }

    const bsl::size_t bytesPerBlock =
        sizeof(AbstractIntegerRepresentation::Block);

    bsl::size_t                          j     = 0;
    AbstractIntegerRepresentation::Block block = 0;

    const bsl::uint8_t* input    = &data.front();
    const bsl::uint8_t* inputEnd = input + data.size();

    while (input < inputEnd) {
        if (*input == 0) {
            ++input;
        }
        else {
            break;
        }
    }

    const bsl::size_t inputSize = inputEnd - input;

    for (bsl::size_t i = inputSize - 1; i < inputSize; --i) {
        block |= AbstractIntegerRepresentation::Block(input[i]) << (8 * j);

        if (++j == bytesPerBlock) {
            d_rep.push(block);
            j     = 0;
            block = 0;
        }
    }

    if (block != 0) {
        d_rep.push(block);
    }

    d_rep.normalize();
}

void AbstractIntegerQuantity::encode(AbstractIntegerSign::Value sign,
                                     bsl::vector<bsl::uint8_t>* result) const
{
    result->clear();

    if (d_rep.isZero()) {
        result->push_back(0);
    }
    else if (sign == AbstractIntegerSign::e_POSITIVE) {
        if (d_rep.isOne()) {
            result->push_back(0x01);
        }
        else {
            bsl::size_t blockCount = d_rep.size();

            for (bsl::size_t i = blockCount - 1; i < blockCount; --i) {
                AbstractIntegerRepresentation::Block value = d_rep.get(i);

                AbstractIntegerRepresentation::Block bigEndianValue =
                    BSLS_BYTEORDER_HOST_TO_BE(value);

                const bsl::uint8_t* bigEndianByteArray =
                    reinterpret_cast<const bsl::uint8_t*>(&bigEndianValue);

                const bsl::uint8_t* bigEndianByteArrayEnd =
                    bigEndianByteArray + sizeof bigEndianValue;

                bsl::size_t bigEndianByteArraySize =
                    bigEndianByteArrayEnd - bigEndianByteArray;

                if (result->empty()) {
                    bsl::size_t numSkipped = 0;
                    for (bsl::size_t i = 0; i < sizeof bigEndianValue; ++i) {
                        const bsl::uint8_t bigEndianByte =
                            bigEndianByteArray[i];

                        if (bigEndianByte == 0x00) {
                            ++numSkipped;
                        }
                        else {
                            break;
                        }
                    }

                    bigEndianByteArray     += numSkipped;
                    bigEndianByteArraySize -= numSkipped;

                    if (bigEndianByteArraySize > 0) {
                        const bsl::uint8_t bigEndianByte =
                            bigEndianByteArray[0];
                        if ((bigEndianByte & 0x80) != 0) {
                            result->push_back(0x00);
                        }
                    }
                }

                for (bsl::size_t i = 0; i < bigEndianByteArraySize; ++i) {
                    const bsl::uint8_t bigEndianByte = bigEndianByteArray[i];
                    result->push_back(bigEndianByte);
                }
            }
        }
    }
    else {
        if (d_rep.isOne()) {
            result->push_back(0xFF);
        }
        else {
            bsl::size_t blockCount = d_rep.size();

            for (bsl::size_t i = blockCount - 1; i < blockCount; --i) {
                AbstractIntegerRepresentation::Block value = d_rep.get(i);

                value  = ~value;
                value += 1;

                AbstractIntegerRepresentation::Block bigEndianValue =
                    BSLS_BYTEORDER_HOST_TO_BE(value);

                const bsl::uint8_t* bigEndianByteArray =
                    reinterpret_cast<const bsl::uint8_t*>(&bigEndianValue);

                const bsl::uint8_t* bigEndianByteArrayEnd =
                    bigEndianByteArray + sizeof bigEndianValue;

                bsl::size_t bigEndianByteArraySize =
                    bigEndianByteArrayEnd - bigEndianByteArray;

                if (result->empty()) {
                    bsl::size_t numSkipped = 0;
                    for (bsl::size_t i = 0; i < sizeof bigEndianValue - 1; ++i)
                    {
                        const bsl::uint8_t bigEndianByte =
                            bigEndianByteArray[i];
                        const bsl::uint8_t bigEndianByteNext =
                            bigEndianByteArray[i + 1];

                        if ((bigEndianByte == 0xFF) &&
                            ((bigEndianByteNext & 0x80) != 0))
                        {
                            ++numSkipped;
                        }
                        else {
                            break;
                        }
                    }

                    bigEndianByteArray     += numSkipped;
                    bigEndianByteArraySize -= numSkipped;
                }

                for (bsl::size_t i = 0; i < bigEndianByteArraySize; ++i) {
                    const bsl::uint8_t bigEndianByte = bigEndianByteArray[i];
                    result->push_back(bigEndianByte);
                }
            }
        }
    }
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(short value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(unsigned short value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(int value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(unsigned int value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(long value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(unsigned long value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(long long value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(
    unsigned long long value)
{
    d_rep.assign(static_cast<bsl::uint64_t>(value));
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::assign(
    const AbstractIntegerQuantity& value)
{
    if (this != &value) {
        d_rep = value.d_rep;
    }

    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::increment()
{
    return this->add(1);
}

AbstractIntegerQuantity& AbstractIntegerQuantity::decrement()
{
    return this->subtract(1);
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(short value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(unsigned short value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(int value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(unsigned int value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(long value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(unsigned long value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(long long value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(unsigned long long value)
{
    return this->add(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::add(
    const AbstractIntegerQuantity& other)
{
    AbstractIntegerQuantityUtil::add(this, *this, other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(short value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(
    unsigned short value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(int value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(unsigned int value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(long value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(unsigned long value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(long long value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(
    unsigned long long value)
{
    return this->subtract(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::subtract(
    const AbstractIntegerQuantity& other)
{
    AbstractIntegerQuantityUtil::subtract(this, *this, other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(short value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(
    unsigned short value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(int value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(unsigned int value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(long value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(unsigned long value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(long long value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(
    unsigned long long value)
{
    return this->multiply(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::multiply(
    const AbstractIntegerQuantity& other)
{
    AbstractIntegerQuantityUtil::multiply(this, *this, other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(short value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(unsigned short value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(int value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(unsigned int value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(long value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(unsigned long value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(long long value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(
    unsigned long long value)
{
    return this->divide(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(
    const AbstractIntegerQuantity& other)
{
    ntsa::AbstractIntegerQuantity remainder;
    return this->divide(other, &remainder);
}

AbstractIntegerQuantity& AbstractIntegerQuantity::divide(
    const AbstractIntegerQuantity& other,
    AbstractIntegerQuantity*       remainder)
{
    AbstractIntegerQuantityUtil::divide(this, remainder, *this, other);
    return *this;
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(short value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(unsigned short value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(int value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(unsigned int value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(long value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(unsigned long value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(long long value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(
    unsigned long long value)
{
    return this->modulus(AbstractIntegerQuantity(value));
}

AbstractIntegerQuantity& AbstractIntegerQuantity::modulus(
    const AbstractIntegerQuantity& other)
{
    AbstractIntegerQuantity quotient;
    AbstractIntegerQuantityUtil::divide(&quotient, this, *this, other);
    return *this;
}

bool AbstractIntegerQuantity::equals(short value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(unsigned short value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(int value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(unsigned int value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(long value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(unsigned long value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(long long value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(unsigned long long value) const
{
    return this->equals(AbstractIntegerQuantity(value));
}

bool AbstractIntegerQuantity::equals(
    const AbstractIntegerQuantity& other) const
{
    return d_rep.equals(other.d_rep);
}

int AbstractIntegerQuantity::compare(short value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(unsigned short value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(int value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(unsigned int value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(long value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(unsigned long value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(long long value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(unsigned long long value) const
{
    return this->compare(AbstractIntegerQuantity(value));
}

int AbstractIntegerQuantity::compare(
    const AbstractIntegerQuantity& other) const
{
    return d_rep.compare(other.d_rep);
}

ntsa::Error AbstractIntegerQuantity::convert(short* result) const
{
    const bsl::size_t size = d_rep.size();

    if (size == 0) {
        *result = 0;
        return ntsa::Error();
    }

    if (size > 1) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const bsl::uint64_t value = d_rep.get(0);

    if (value > static_cast<bsl::uint64_t>(bsl::numeric_limits<short>::max()))
    {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *result = static_cast<short>(value);
    return ntsa::Error();
}

ntsa::Error AbstractIntegerQuantity::convert(unsigned short* result) const
{
    const bsl::size_t size = d_rep.size();

    if (size == 0) {
        *result = 0;
        return ntsa::Error();
    }

    if (size > 1) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const bsl::uint64_t value = d_rep.get(0);

    if (value > static_cast<bsl::uint64_t>(bsl::numeric_limits<short>::max()))
    {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *result = static_cast<short>(value);
    return ntsa::Error();
}

ntsa::Error AbstractIntegerQuantity::convert(int* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(unsigned int* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(long* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(unsigned long* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(long long* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractIntegerQuantity::convert(unsigned long long* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void AbstractIntegerQuantity::generate(bsl::string*               result,
                                       AbstractIntegerSign::Value sign,
                                       AbstractIntegerBase::Value base) const
{
    AbstractIntegerRepresentation::generate(result, d_rep, sign, base);
}

bsl::size_t AbstractIntegerQuantity::size() const
{
    return d_rep.size();
}

AbstractIntegerBase::Value AbstractIntegerQuantity::base() const
{
    return d_rep.base();
}

bsl::uint64_t AbstractIntegerQuantity::radix() const
{
    return d_rep.radix();
}

bool AbstractIntegerQuantity::isZero() const
{
    return d_rep.isZero();
}

bool AbstractIntegerQuantity::isOne() const
{
    return d_rep.isOne();
}

bool AbstractIntegerQuantity::isNotAliasOf(
    const AbstractIntegerQuantity* other) const
{
    return this != other;
}

bool AbstractIntegerQuantity::isNotAliasOf(
    const AbstractIntegerQuantity& other) const
{
    return this != &other;
}

bslma::Allocator* AbstractIntegerQuantity::allocator() const
{
    return d_rep.allocator();
}

bsl::ostream& AbstractIntegerQuantity::print(bsl::ostream& stream,
                                             int           level,
                                             int spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    bsl::string                result;
    AbstractIntegerBase::Value base = AbstractIntegerBase::e_DECIMAL;

    const bsl::ostream::fmtflags flags = stream.flags();

    if ((flags & std::ios_base::hex) != 0) {
        base = AbstractIntegerBase::e_HEXADECIMAL;
    }
    else if ((flags & std::ios_base::oct) != 0) {
        base = AbstractIntegerBase::e_OCTAL;
    }

    this->generate(&result, AbstractIntegerSign::e_POSITIVE, base);
    stream << result;
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const AbstractIntegerQuantity& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractIntegerQuantity& lhs,
               const AbstractIntegerQuantity& rhs)
{
    return lhs.compare(rhs) < 0;
}

bool operator<=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs)
{
    return lhs.compare(rhs) <= 0;
}

bool operator>(const AbstractIntegerQuantity& lhs,
               const AbstractIntegerQuantity& rhs)
{
    return lhs.compare(rhs) > 0;
}

bool operator>=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs)
{
    return lhs.compare(rhs) >= 0;
}

void AbstractIntegerQuantityUtil::add(AbstractIntegerQuantity*       sum,
                                      const AbstractIntegerQuantity& addend1,
                                      const AbstractIntegerQuantity& addend2)
{
    if (sum->isNotAliasOf(addend1) && sum->isNotAliasOf(addend2)) {
        AbstractIntegerRepresentation::add(&sum->d_rep,
                                           addend1.d_rep,
                                           addend2.d_rep);
    }
    else {
        AbstractIntegerQuantity tempSum(sum->allocator());
        AbstractIntegerRepresentation::add(&tempSum.d_rep,
                                           addend1.d_rep,
                                           addend2.d_rep);
        sum->swap(tempSum);
    }
}

void AbstractIntegerQuantityUtil::subtract(
    AbstractIntegerQuantity*       difference,
    const AbstractIntegerQuantity& minuend,
    const AbstractIntegerQuantity& subtrahend)
{
    if (difference->isNotAliasOf(minuend) &&
        difference->isNotAliasOf(subtrahend))
    {
        AbstractIntegerRepresentation::subtract(&difference->d_rep,
                                                minuend.d_rep,
                                                subtrahend.d_rep);
    }
    else {
        AbstractIntegerQuantity tempDifference(difference->allocator());
        AbstractIntegerRepresentation::subtract(&tempDifference.d_rep,
                                                minuend.d_rep,
                                                subtrahend.d_rep);
        difference->swap(tempDifference);
    }
}

void AbstractIntegerQuantityUtil::multiply(
    AbstractIntegerQuantity*       product,
    const AbstractIntegerQuantity& multiplicand,
    const AbstractIntegerQuantity& multiplier)
{
    if (product->isNotAliasOf(multiplicand) &&
        product->isNotAliasOf(multiplier))
    {
        AbstractIntegerRepresentation::multiply(&product->d_rep,
                                                multiplicand.d_rep,
                                                multiplier.d_rep);
    }
    else {
        AbstractIntegerQuantity tempProduct(product->allocator());
        AbstractIntegerRepresentation::multiply(&tempProduct.d_rep,
                                                multiplicand.d_rep,
                                                multiplier.d_rep);
        product->swap(tempProduct);
    }
}

void AbstractIntegerQuantityUtil::divide(
    AbstractIntegerQuantity*       quotient,
    AbstractIntegerQuantity*       remainder,
    const AbstractIntegerQuantity& dividend,
    const AbstractIntegerQuantity& divisor)
{
    bdlb::NullableValue<AbstractIntegerQuantity> defaultQuotient;
    bdlb::NullableValue<AbstractIntegerQuantity> defaultRemainder;

    if (!quotient) {
        quotient = &defaultQuotient.makeValue();
    }

    if (!remainder) {
        remainder = &defaultRemainder.makeValue();
    }

    if (quotient->isNotAliasOf(dividend) && quotient->isNotAliasOf(divisor) &&
        remainder->isNotAliasOf(dividend) && remainder->isNotAliasOf(divisor))
    {
        AbstractIntegerRepresentation::divide(&quotient->d_rep,
                                              &remainder->d_rep,
                                              dividend.d_rep,
                                              divisor.d_rep);
    }
    else {
        AbstractIntegerQuantity tempQuotient(quotient->allocator());
        AbstractIntegerQuantity tempRemainder(remainder->allocator());
        AbstractIntegerRepresentation::divide(&tempQuotient.d_rep,
                                              &tempRemainder.d_rep,
                                              dividend.d_rep,
                                              divisor.d_rep);
        quotient->swap(tempQuotient);
        remainder->swap(tempRemainder);
    }
}

void AbstractInteger::normalize()
{
    if (d_magnitude.isZero()) {
        d_sign = AbstractIntegerSign::e_POSITIVE;
    }
}

AbstractInteger::AbstractInteger(bslma::Allocator* basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
}

AbstractInteger::AbstractInteger(short value, bslma::Allocator* basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(unsigned short    value,
                                 bslma::Allocator* basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(int value, bslma::Allocator* basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(unsigned int      value,
                                 bslma::Allocator* basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(long value, bslma::Allocator* basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(unsigned long     value,
                                 bslma::Allocator* basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(long long         value,
                                 bslma::Allocator* basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(unsigned long long value,
                                 bslma::Allocator*  basicAllocator)
: d_sign(AbstractIntegerSign::e_POSITIVE)
, d_magnitude(basicAllocator)
{
    this->assign(value);
}

AbstractInteger::AbstractInteger(const AbstractInteger& original,
                                 bslma::Allocator*      basicAllocator)
: d_sign(original.d_sign)
, d_magnitude(original.d_magnitude, basicAllocator)
{
}

AbstractInteger::~AbstractInteger()
{
}

AbstractInteger& AbstractInteger::operator=(const AbstractInteger& other)
{
    this->assign(other);
    return *this;
}

AbstractInteger& AbstractInteger::operator=(short value)
{
    this->assign(value);
    return *this;
}

AbstractInteger& AbstractInteger::operator=(unsigned short value)
{
    this->assign(value);
    return *this;
}

AbstractInteger& AbstractInteger::operator=(int value)
{
    this->assign(value);
    return *this;
}

AbstractInteger& AbstractInteger::operator=(unsigned int value)
{
    this->assign(value);
    return *this;
}

AbstractInteger& AbstractInteger::operator=(long value)
{
    this->assign(value);
    return *this;
}

AbstractInteger& AbstractInteger::operator=(unsigned long value)
{
    this->assign(value);
    return *this;
}

AbstractInteger& AbstractInteger::operator=(long long value)
{
    this->assign(value);
    return *this;
}

AbstractInteger& AbstractInteger::operator=(unsigned long long value)
{
    this->assign(value);
    return *this;
}

void AbstractInteger::reset()
{
    d_sign = AbstractIntegerSign::e_POSITIVE;
    d_magnitude.reset();
}

void AbstractInteger::swap(AbstractInteger& other)
{
    if (this != &other) {
        bsl::swap(d_sign, other.d_sign);
        d_magnitude.swap(other.d_magnitude);
    }
}

bool AbstractInteger::parse(const bsl::string_view& text)
{
    return d_magnitude.parse(&d_sign, text);
}

void AbstractInteger::decode(const bsl::vector<bsl::uint8_t>& data)
{
    this->reset();

    if (data.empty()) {
        return;
    }

    if ((data[0] & 0x80) == 0) {
        d_magnitude.decode(data);
        return;
    }

    bsl::vector<bsl::uint8_t> ud = data;
    bsl::vector<bsl::uint8_t> vd = data;

    ud[0] &= ~0x80;

    vd[0] &= ~0x7F;
    for (bsl::size_t i = 1; i < vd.size(); ++i) {
        vd[i] = 0;
    }

    AbstractIntegerQuantity u;
    u.decode(ud);

    AbstractIntegerQuantity v;
    v.decode(vd);

    BSLS_ASSERT_OPT(v.compare(u) > 0);

    AbstractIntegerQuantity w(d_magnitude.allocator());
    AbstractIntegerQuantityUtil::subtract(&w, v, u);

    d_magnitude.swap(w);
    d_sign = AbstractIntegerSign::e_NEGATIVE;
}

void AbstractInteger::encode(bsl::vector<bsl::uint8_t>* result) const
{
    d_magnitude.encode(d_sign, result);
}

AbstractInteger& AbstractInteger::assign(short value)
{
    return this->assign(static_cast<long long>(value));
}

AbstractInteger& AbstractInteger::assign(unsigned short value)
{
    return this->assign(static_cast<unsigned long long>(value));
}

AbstractInteger& AbstractInteger::assign(int value)
{
    return this->assign(static_cast<long long>(value));
}

AbstractInteger& AbstractInteger::assign(unsigned int value)
{
    return this->assign(static_cast<unsigned long long>(value));
}

AbstractInteger& AbstractInteger::assign(long value)
{
    return this->assign(static_cast<long long>(value));
}

AbstractInteger& AbstractInteger::assign(unsigned long value)
{
    return this->assign(static_cast<unsigned long long>(value));
}

AbstractInteger& AbstractInteger::assign(long long value)
{
    if (value == 0) {
        d_sign = AbstractIntegerSign::e_POSITIVE;
        d_magnitude.reset();
    }
    else {
        if (value > 0) {
            d_sign = AbstractIntegerSign::e_POSITIVE;
            d_magnitude.assign(static_cast<unsigned long long>(value));
        }
        else if (value < 0) {
            d_sign = AbstractIntegerSign::e_NEGATIVE;
            unsigned long long temp =
                static_cast<unsigned long long>(-1 * value);
            d_magnitude.assign(static_cast<unsigned long long>(temp));
        }
    }

    return *this;
}

AbstractInteger& AbstractInteger::assign(unsigned long long value)
{
    d_sign = AbstractIntegerSign::e_POSITIVE;

    if (value == 0) {
        d_magnitude.reset();
    }
    else {
        d_magnitude.assign(value);
    }

    return *this;
}

AbstractInteger& AbstractInteger::assign(const AbstractInteger& value)
{
    if (this != &value) {
        d_sign      = value.d_sign;
        d_magnitude = value.d_magnitude;
    }

    return *this;
}

AbstractInteger& AbstractInteger::negate()
{
    d_sign = AbstractIntegerSign::flip(d_sign);
    return *this;
}

AbstractInteger& AbstractInteger::increment()
{
    return this->add(1);
}

AbstractInteger& AbstractInteger::decrement()
{
    return this->subtract(1);
}

AbstractInteger& AbstractInteger::add(short value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(unsigned short value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(int value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(unsigned int value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(long value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(unsigned long value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(long long value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(unsigned long long value)
{
    return this->add(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::add(const AbstractInteger& other)
{
    AbstractIntegerUtil::add(this, *this, other);
    return *this;
}

AbstractInteger& AbstractInteger::subtract(short value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(unsigned short value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(int value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(unsigned int value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(long value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(unsigned long value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(long long value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(unsigned long long value)
{
    return this->subtract(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::subtract(const AbstractInteger& other)
{
    AbstractIntegerUtil::subtract(this, *this, other);
    return *this;
}

AbstractInteger& AbstractInteger::multiply(short value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(unsigned short value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(int value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(unsigned int value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(long value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(unsigned long value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(long long value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(unsigned long long value)
{
    return this->multiply(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::multiply(const AbstractInteger& other)
{
    AbstractIntegerUtil::multiply(this, *this, other);
    return *this;
}

AbstractInteger& AbstractInteger::divide(short value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(unsigned short value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(int value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(unsigned int value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(long value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(unsigned long value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(long long value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(unsigned long long value)
{
    return this->divide(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::divide(const AbstractInteger& other)
{
    ntsa::AbstractInteger remainder;
    return this->divide(other, &remainder);
}

AbstractInteger& AbstractInteger::divide(const AbstractInteger& other,
                                         AbstractInteger*       remainder)
{
    AbstractIntegerUtil::divide(this, remainder, *this, other);
    return *this;
}

AbstractInteger& AbstractInteger::modulus(short value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(unsigned short value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(int value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(unsigned int value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(long value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(unsigned long value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(long long value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(unsigned long long value)
{
    return this->modulus(AbstractInteger(value));
}

AbstractInteger& AbstractInteger::modulus(const AbstractInteger& other)
{
    AbstractInteger quotient;
    AbstractIntegerUtil::divide(&quotient, this, *this, other);
    return *this;
}

bool AbstractInteger::equals(short value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(unsigned short value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(int value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(unsigned int value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(long value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(unsigned long value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(long long value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(unsigned long long value) const
{
    return this->equals(AbstractInteger(value));
}

bool AbstractInteger::equals(const AbstractInteger& other) const
{
    return d_sign == other.d_sign && d_magnitude.equals(other.d_magnitude);
}

int AbstractInteger::compare(short value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(unsigned short value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(int value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(unsigned int value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(long value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(unsigned long value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(long long value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(unsigned long long value) const
{
    return this->compare(AbstractInteger(value));
}

int AbstractInteger::compare(const AbstractInteger& other) const
{
    int result = 0;

    if (d_sign < other.d_sign) {
        result = -1;
    }
    else if (other.d_sign < d_sign) {
        result = +1;
    }
    else {
        result = d_magnitude.compare(other.d_magnitude);
    }

    return result;
}

ntsa::Error AbstractInteger::convert(short* result) const
{
    ntsa::Error error;

    bsl::int64_t value = 0;
    error              = d_magnitude.convert(&value);
    if (error) {
        return error;
    }

    if (value > static_cast<bsl::int64_t>(bsl::numeric_limits<short>::max())) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    if (d_sign == AbstractIntegerSign::e_NEGATIVE) {
        value = -value;
    }

    *result = static_cast<short>(value);
    return ntsa::Error();
}

ntsa::Error AbstractInteger::convert(unsigned short* result) const
{
    ntsa::Error error;

    if (d_sign == AbstractIntegerSign::e_NEGATIVE) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::uint64_t value = 0;
    error               = d_magnitude.convert(&value);
    if (error) {
        return error;
    }

    if (value >
        static_cast<bsl::uint64_t>(bsl::numeric_limits<unsigned short>::max()))
    {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    *result = static_cast<short>(value);
    return ntsa::Error();
}

ntsa::Error AbstractInteger::convert(int* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(unsigned int* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(long* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(unsigned long* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(long long* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error AbstractInteger::convert(unsigned long long* result) const
{
    NTSCFG_WARNING_UNUSED(result);
    NTSCFG_NOT_IMPLEMENTED();
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void AbstractInteger::generate(bsl::string*               result,
                               AbstractIntegerBase::Value base) const
{
    d_magnitude.generate(result, d_sign, base);
}

bool AbstractInteger::isZero() const
{
    return d_magnitude.isZero();
}

bool AbstractInteger::isPositive() const
{
    return d_sign == AbstractIntegerSign::e_POSITIVE;
}

bool AbstractInteger::isNegative() const
{
    return d_sign == AbstractIntegerSign::e_NEGATIVE;
}

bool AbstractInteger::isNotAliasOf(const AbstractInteger* other) const
{
    return this != other;
}

bool AbstractInteger::isNotAliasOf(const AbstractInteger& other) const
{
    return this != &other;
}

bslma::Allocator* AbstractInteger::allocator() const
{
    return d_magnitude.allocator();
}

bsl::ostream& AbstractInteger::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    bsl::string                result;
    AbstractIntegerBase::Value base = AbstractIntegerBase::e_DECIMAL;

    const bsl::ostream::fmtflags flags = stream.flags();

    if ((flags & std::ios_base::hex) != 0) {
        base = AbstractIntegerBase::e_HEXADECIMAL;
    }
    else if ((flags & std::ios_base::oct) != 0) {
        base = AbstractIntegerBase::e_OCTAL;
    }

    d_magnitude.generate(&result, d_sign, base);
    stream << result;
    return stream;
}

AbstractInteger AbstractInteger::operator+(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.add(other);
    return result;
}

AbstractInteger AbstractInteger::operator-(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.subtract(other);
    return result;
}

AbstractInteger AbstractInteger::operator*(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.multiply(other);
    return result;
}

AbstractInteger AbstractInteger::operator/(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.divide(other);
    return result;
}

AbstractInteger AbstractInteger::operator%(const AbstractInteger& other) const
{
    AbstractInteger result(*this);
    result.modulus(other);
    return result;
}

AbstractInteger AbstractInteger::operator-() const
{
    AbstractInteger result(*this);
    result.negate();
    return result;
}

AbstractInteger& AbstractInteger::operator+=(const AbstractInteger& other)
{
    return this->add(other);
}

AbstractInteger& AbstractInteger::operator-=(const AbstractInteger& other)
{
    return this->subtract(other);
}

AbstractInteger& AbstractInteger::operator*=(const AbstractInteger& other)
{
    return this->multiply(other);
}

AbstractInteger& AbstractInteger::operator/=(const AbstractInteger& other)
{
    return this->divide(other);
}

AbstractInteger& AbstractInteger::operator%=(const AbstractInteger& other)
{
    return this->modulus(other);
}

AbstractInteger& AbstractInteger::operator++()
{
    return this->increment();
}

AbstractInteger AbstractInteger::operator++(int)
{
    AbstractInteger result(*this);
    result.increment();
    return result;
}

AbstractInteger& AbstractInteger::operator--()
{
    return this->decrement();
}

AbstractInteger AbstractInteger::operator--(int)
{
    AbstractInteger result(*this);
    result.decrement();
    return result;
}

bsl::ostream& operator<<(bsl::ostream& stream, const AbstractInteger& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.compare(rhs) < 0;
}

bool operator<=(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.compare(rhs) <= 0;
}

bool operator>(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.compare(rhs) > 0;
}

bool operator>=(const AbstractInteger& lhs, const AbstractInteger& rhs)
{
    return lhs.compare(rhs) >= 0;
}

void AbstractIntegerUtil::add(AbstractInteger*       sum,
                              const AbstractInteger& addend1,
                              const AbstractInteger& addend2)
{
    bdlb::NullableValue<AbstractInteger> tempSum(sum->allocator());

    AbstractInteger* w;
    if (sum->isNotAliasOf(addend1) && sum->isNotAliasOf(addend2)) {
        w = sum;
    }
    else {
        w = &tempSum.makeValue();
    }

    const AbstractInteger* u = &addend1;
    const AbstractInteger* v = &addend2;

    if (u->isZero()) {
        w->d_magnitude = v->d_magnitude;
        w->d_sign      = v->d_sign;
    }
    else if (v->isZero()) {
        w->d_magnitude = u->d_magnitude;
        w->d_sign      = u->d_sign;
    }
    else if (u->d_sign == v->d_sign) {
        AbstractIntegerQuantityUtil::add(&w->d_magnitude,
                                         u->d_magnitude,
                                         v->d_magnitude);
        w->d_sign = u->d_sign;
    }
    else {
        const int comparison = u->d_magnitude.compare(v->d_magnitude);
        if (comparison == 0) {
            w->reset();
        }
        else if (comparison > 0) {
            AbstractIntegerQuantityUtil::subtract(&w->d_magnitude,
                                                  u->d_magnitude,
                                                  v->d_magnitude);
            w->d_sign = u->d_sign;
        }
        else {
            AbstractIntegerQuantityUtil::subtract(&w->d_magnitude,
                                                  v->d_magnitude,
                                                  u->d_magnitude);
            w->d_sign = v->d_sign;
        }
    }

    if (w != sum) {
        sum->swap(*w);
    }

    sum->normalize();
}

void AbstractIntegerUtil::subtract(AbstractInteger*       difference,
                                   const AbstractInteger& minuend,
                                   const AbstractInteger& subtrahend)
{
    bdlb::NullableValue<AbstractInteger> tempDifference(
        difference->allocator());

    AbstractInteger* w;
    if (difference->isNotAliasOf(minuend) &&
        difference->isNotAliasOf(subtrahend))
    {
        w = difference;
    }
    else {
        w = &tempDifference.makeValue();
    }

    const AbstractInteger* u = &minuend;
    const AbstractInteger* v = &subtrahend;

    if (u->isZero()) {
        w->d_magnitude = v->d_magnitude;
        w->d_sign      = AbstractIntegerSign::flip(v->d_sign);
    }
    else if (v->isZero()) {
        w->d_magnitude = u->d_magnitude;
        w->d_sign      = u->d_sign;
    }
    else if (u->d_sign != v->d_sign) {
        AbstractIntegerQuantityUtil::add(&w->d_magnitude,
                                         u->d_magnitude,
                                         v->d_magnitude);
        w->d_sign = u->d_sign;
    }
    else {
        const int comparison = u->d_magnitude.compare(v->d_magnitude);
        if (comparison == 0) {
            w->reset();
        }
        else if (comparison > 0) {
            AbstractIntegerQuantityUtil::subtract(&w->d_magnitude,
                                                  u->d_magnitude,
                                                  v->d_magnitude);
            w->d_sign = u->d_sign;
        }
        else {
            AbstractIntegerQuantityUtil::subtract(&w->d_magnitude,
                                                  v->d_magnitude,
                                                  u->d_magnitude);
            w->d_sign = AbstractIntegerSign::flip(v->d_sign);
        }
    }

    if (w != difference) {
        difference->swap(*w);
    }

    difference->normalize();
}

void AbstractIntegerUtil::multiply(AbstractInteger*       product,
                                   const AbstractInteger& multiplicand,
                                   const AbstractInteger& multiplier)
{
    bdlb::NullableValue<AbstractInteger> tempProduct(product->allocator());

    AbstractInteger* w;
    if (product->isNotAliasOf(multiplicand) &&
        product->isNotAliasOf(multiplier))
    {
        w = product;
    }
    else {
        w = &tempProduct.makeValue();
    }

    const AbstractInteger* u = &multiplicand;
    const AbstractInteger* v = &multiplier;

    if (u->isZero() || v->isZero()) {
        w->reset();
    }
    else {
        AbstractIntegerQuantityUtil::multiply(&w->d_magnitude,
                                              u->d_magnitude,
                                              v->d_magnitude);
        if (u->d_sign == v->d_sign) {
            w->d_sign = AbstractIntegerSign::e_POSITIVE;
        }
        else {
            w->d_sign = AbstractIntegerSign::e_NEGATIVE;
        }
    }

    if (w != product) {
        product->swap(*w);
    }

    product->normalize();
}

void AbstractIntegerUtil::divide(AbstractInteger*       quotient,
                                 AbstractInteger*       remainder,
                                 const AbstractInteger& dividend,
                                 const AbstractInteger& divisor)
{
    bdlb::NullableValue<AbstractInteger> tempQuotient(quotient->allocator());

    bdlb::NullableValue<AbstractInteger> tempRemainder(remainder->allocator());

    AbstractInteger* w;
    if (quotient->isNotAliasOf(dividend) && quotient->isNotAliasOf(divisor) &&
        quotient->isNotAliasOf(remainder))
    {
        w = quotient;
    }
    else {
        w = &tempQuotient.makeValue();
    }

    AbstractInteger* x;
    if (remainder->isNotAliasOf(dividend) &&
        remainder->isNotAliasOf(divisor) && remainder->isNotAliasOf(quotient))
    {
        x = remainder;
    }
    else {
        x = &tempRemainder.makeValue();
    }

    const AbstractInteger* u = &dividend;
    const AbstractInteger* v = &divisor;

    if (u->isZero()) {
        w->reset();
        x->reset();
    }
    else if (v->isZero()) {
        w->reset();
        x->assign(*v);
    }
    else if (u->d_sign == v->d_sign) {
        AbstractIntegerQuantityUtil::divide(&w->d_magnitude,
                                            &x->d_magnitude,
                                            u->d_magnitude,
                                            v->d_magnitude);

        w->d_sign = AbstractIntegerSign::e_POSITIVE;

        if (v->d_sign == AbstractIntegerSign::e_NEGATIVE ||
            u->d_sign == AbstractIntegerSign::e_NEGATIVE)
        {
            x->d_sign = AbstractIntegerSign::e_NEGATIVE;
        }
        else {
            x->d_sign = AbstractIntegerSign::e_POSITIVE;
        }
    }
    else {
        AbstractIntegerQuantity un(u->d_magnitude, u->allocator());

        AbstractIntegerQuantityUtil::divide(&w->d_magnitude,
                                            &x->d_magnitude,
                                            un,
                                            v->d_magnitude);

        w->d_sign = AbstractIntegerSign::e_NEGATIVE;

        if (u->d_sign == AbstractIntegerSign::e_NEGATIVE) {
            x->d_sign = AbstractIntegerSign::e_NEGATIVE;
        }
        else {
            x->d_sign = AbstractIntegerSign::e_POSITIVE;
        }
    }

    if (w != quotient) {
        quotient->swap(*w);
    }

    if (x != remainder) {
        remainder->swap(*x);
    }

    quotient->normalize();
    remainder->normalize();
}

}  // close package namespace
}  // close enterprise namespace
