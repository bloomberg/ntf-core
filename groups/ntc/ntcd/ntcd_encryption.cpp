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

#include <ntcd_encryption.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_encryption_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <ntcs_datapool.h>
#include <ntsa_data.h>
#include <bdlb_random.h>
#include <bdlb_string.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_spinlock.h>
#include <bsl_fstream.h>
#include <bsl_sstream.h>

// IMPLEMENTATION NOTES:
//
// Client: DEFAULT -> HELLO_SENT -> ACCEPT_RECEIVED -> HELLO_RECEIVED
//                 -> ACCEPT_SENT -> ESTABLISHED
// Server: DEFAULT -> HELLO_RECEIVED -> ACCEPT_SENT -> HELLO_SENT
//                 -> ACCEPT_RECEIVED -> ESTABLISHED

namespace BloombergLP {
namespace ntcd {

namespace {

bsls::SpinLock s_seedLock = BSLS_SPINLOCK_UNLOCKED;
int            s_seed     = 12345;

ntsa::Error encodeDistinguishedName(
    bsl::streambuf*                destination,
    const ntsa::DistinguishedName& distinguishedName)
{
    int rc;

    bsl::string text;
    rc = distinguishedName.generate(&text);
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        bdlb::BigEndianUint32 bigEndianTextLength;
        bigEndianTextLength = static_cast<unsigned int>(text.size());

        if (sizeof bigEndianTextLength !=
            destination->sputn(
                reinterpret_cast<const char*>(&bigEndianTextLength),
                sizeof bigEndianTextLength))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (text.size() != NTCCFG_WARNING_PROMOTE(
                               bsl::size_t,
                               destination->sputn(text.c_str(), text.size())))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error decodeDistinguishedName(ntsa::DistinguishedName* result,
                                    bsl::streambuf*          source)
{
    int rc;

    result->reset();

    bdlb::BigEndianUint32 bigEndianTextLength;

    if (sizeof bigEndianTextLength !=
        source->sgetn(reinterpret_cast<char*>(&bigEndianTextLength),
                      sizeof bigEndianTextLength))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t textLength = static_cast<unsigned int>(bigEndianTextLength);

    bsl::string text;
    text.resize(textLength);

    if (text.size() !=
        NTCCFG_WARNING_PROMOTE(
            bsl::size_t,
            source->sgetn(reinterpret_cast<char*>(&text.front()),
                          text.size())))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = result->parse(text);
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

}  // close unnamed namespace

EncryptionKey::EncryptionKey()
: d_value(0)
{
}

EncryptionKey::EncryptionKey(bsl::uint32_t value)
: d_value(value)
{
}

EncryptionKey::~EncryptionKey()
{
}

ntsa::Error EncryptionKey::generate(const ntca::EncryptionKeyOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    bsl::uint32_t value = 0;

    {
        bsls::SpinLockGuard lock(&s_seedLock);

        int seed     = s_seed;
        int nextSeed = 0;

        int random15 = bdlb::Random::generate15(&nextSeed, seed);
        value        = static_cast<bsl::uint32_t>(random15);

        s_seed = nextSeed;
    }

    d_value = value;

    return ntsa::Error();
}

ntsa::Error EncryptionKey::decode(
    bsl::streambuf*                        source,
    const ntca::EncryptionResourceOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    bdlb::BigEndianUint32 bigEndianValue;

    if (sizeof bigEndianValue !=
        source->sgetn(reinterpret_cast<char*>(&bigEndianValue),
                      sizeof bigEndianValue))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_value = static_cast<unsigned int>(bigEndianValue);

    return ntsa::Error();
}

ntsa::Error EncryptionKey::encode(
    bsl::streambuf*                        destination,
    const ntca::EncryptionResourceOptions& options) const
{
    NTCCFG_WARNING_UNUSED(options);

    int rc;

    bdlb::BigEndianUint32 bigEndianValue;
    bigEndianValue = static_cast<unsigned int>(d_value);

    if (sizeof bigEndianValue !=
        destination->sputn(reinterpret_cast<const char*>(&bigEndianValue),
                           sizeof bigEndianValue))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = destination->pubsync();
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

void* EncryptionKey::handle() const
{
    return const_cast<EncryptionKey*>(this);
}

bsl::uint32_t EncryptionKey::value() const
{
    return d_value;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const ntcd::EncryptionKey& object)
{
    return stream << "[ value = " << object.value() << " ]";
}

EncryptionCertificate::EncryptionCertificate(bslma::Allocator* basicAllocator)
: d_subject(basicAllocator)
, d_subjectKey_sp()
, d_issuer(basicAllocator)
, d_issuerKey_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificate::EncryptionCertificate(
    const ntsa::DistinguishedName&              subject,
    const bsl::shared_ptr<ntcd::EncryptionKey>& subjectKey,
    bslma::Allocator*                           basicAllocator)
: d_subject(subject, basicAllocator)
, d_subjectKey_sp(subjectKey)
, d_issuer(subject, basicAllocator)
, d_issuerKey_sp(subjectKey)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificate::EncryptionCertificate(
    const ntsa::DistinguishedName&              subject,
    const bsl::shared_ptr<ntcd::EncryptionKey>& subjectKey,
    const ntsa::DistinguishedName&              issuer,
    const bsl::shared_ptr<ntcd::EncryptionKey>& issuerKey,
    bslma::Allocator*                           basicAllocator)
: d_subject(subject, basicAllocator)
, d_subjectKey_sp(subjectKey)
, d_issuer(issuer, basicAllocator)
, d_issuerKey_sp(issuerKey)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificate::~EncryptionCertificate()
{
}

ntsa::Error EncryptionCertificate::generate(
    const ntsa::DistinguishedName&              subjectIdentity,
    const bsl::shared_ptr<ntcd::EncryptionKey>& subjectPrivateKey,
    const ntca::EncryptionCertificateOptions&   options)
{
    NTCCFG_WARNING_UNUSED(options);

    d_subject       = subjectIdentity;
    d_subjectKey_sp = subjectPrivateKey;

    d_issuer       = subjectIdentity;
    d_issuerKey_sp = subjectPrivateKey;

    return ntsa::Error();
}

ntsa::Error EncryptionCertificate::generate(
    const ntsa::DistinguishedName&                      subjectIdentity,
    const bsl::shared_ptr<ntcd::EncryptionKey>&         subjectPrivateKey,
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& issuerCertificate,
    const bsl::shared_ptr<ntcd::EncryptionKey>&         issuerPrivateKey,
    const ntca::EncryptionCertificateOptions&           options)
{
    NTCCFG_WARNING_UNUSED(options);

    d_subject       = subjectIdentity;
    d_subjectKey_sp = subjectPrivateKey;

    d_issuer       = issuerCertificate->subject();
    d_issuerKey_sp = issuerPrivateKey;

    return ntsa::Error();
}

ntsa::Error EncryptionCertificate::decode(
    bsl::streambuf*                        source,
    const ntca::EncryptionResourceOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error                     error;
    ntca::EncryptionResourceOptions keyStorageOptions;

    error = decodeDistinguishedName(&d_subject, source);
    if (error) {
        return error;
    }

    d_subjectKey_sp.createInplace(d_allocator_p);
    error = d_subjectKey_sp->decode(source, keyStorageOptions);
    if (error) {
        return error;
    }

    error = decodeDistinguishedName(&d_issuer, source);
    if (error) {
        return error;
    }

    d_issuerKey_sp.createInplace(d_allocator_p);
    error = d_issuerKey_sp->decode(source, keyStorageOptions);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionCertificate::encode(
    bsl::streambuf*                        destination,
    const ntca::EncryptionResourceOptions& options) const
{
    NTCCFG_WARNING_UNUSED(options);

    int rc;

    ntsa::Error                     error;
    ntca::EncryptionResourceOptions keyStorageOptions;

    error = encodeDistinguishedName(destination, d_subject);
    if (error) {
        return error;
    }

    if (!d_subjectKey_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_subjectKey_sp->encode(destination, keyStorageOptions);
    if (error) {
        return error;
    }

    error = encodeDistinguishedName(destination, d_issuer);
    if (error) {
        return error;
    }

    if (!d_issuerKey_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = d_issuerKey_sp->encode(destination, keyStorageOptions);
    if (error) {
        return error;
    }

    rc = destination->pubsync();
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

void EncryptionCertificate::print(bsl::ostream& stream) const
{
    d_subject.print(stream);
    d_issuer.print(stream);
}

void* EncryptionCertificate::handle() const
{
    return const_cast<EncryptionCertificate*>(this);
}

const ntsa::DistinguishedName& EncryptionCertificate::subject() const
{
    return d_subject;
}

const ntsa::DistinguishedName& EncryptionCertificate::issuer() const
{
    return d_issuer;
}

const bsl::shared_ptr<ntcd::EncryptionKey>& EncryptionCertificate::subjectKey()
    const
{
    return d_subjectKey_sp;
}

const bsl::shared_ptr<ntcd::EncryptionKey>& EncryptionCertificate::issuerKey()
    const
{
    return d_issuerKey_sp;
}

ntsa::DistinguishedName EncryptionCertificate::distinguishedName(
    const bsl::string& commonName)
{
    ntsa::DistinguishedName result;
    result[ntsa::DistinguishedName::e_COMMON_NAME].addAttribute(commonName);
    return result;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const ntcd::EncryptionCertificate& object)
{
    stream << "[ subject = " << object.subject();

    if (object.subjectKey()) {
        stream << " subjectKey = " << *object.subjectKey();
    }

    stream << " issuer = " << object.issuer();

    if (object.issuerKey()) {
        stream << " issuerKey = " << *object.issuerKey();
    }

    stream << " ]";

    return stream;
}

int EncryptionFrameType::fromInt(EncryptionFrameType::Value* result,
                                 int                         number)
{
    switch (number) {
    case EncryptionFrameType::e_HELLO:
    case EncryptionFrameType::e_ACCEPT:
    case EncryptionFrameType::e_DATA:
    case EncryptionFrameType::e_GOODBYE:
        *result = static_cast<EncryptionFrameType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int EncryptionFrameType::fromString(EncryptionFrameType::Value* result,
                                    const bslstl::StringRef&    string)
{
    if (bdlb::String::areEqualCaseless(string, "HELLO")) {
        *result = e_HELLO;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ACCEPT")) {
        *result = e_ACCEPT;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DATA")) {
        *result = e_DATA;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "GOODBYE")) {
        *result = e_GOODBYE;
        return 0;
    }

    return -1;
}

const char* EncryptionFrameType::toString(EncryptionFrameType::Value value)
{
    switch (value) {
    case e_HELLO: {
        return "HELLO";
    } break;
    case e_ACCEPT: {
        return "ACCEPT";
    } break;
    case e_DATA: {
        return "DATA";
    } break;
    case e_GOODBYE: {
        return "GOODBYE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& EncryptionFrameType::print(bsl::ostream&              stream,
                                         EncryptionFrameType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, EncryptionFrameType::Value rhs)
{
    return EncryptionFrameType::print(stream, rhs);
}

EncryptionFrameHeader::EncryptionFrameHeader()
: d_type()
, d_sequenceNumber()
, d_length()
{
}

EncryptionFrameHeader::EncryptionFrameHeader(
    const EncryptionFrameHeader& original)
: d_type(original.d_type)
, d_sequenceNumber(original.d_sequenceNumber)
, d_length(original.d_length)
{
}

EncryptionFrameHeader::~EncryptionFrameHeader()
{
}

EncryptionFrameHeader& EncryptionFrameHeader::operator=(
    const EncryptionFrameHeader& other)
{
    d_type           = other.d_type;
    d_sequenceNumber = other.d_sequenceNumber;
    d_length         = other.d_length;

    return *this;
}

void EncryptionFrameHeader::setType(ntcd::EncryptionFrameType::Value type)
{
    d_type = static_cast<unsigned int>(type);
}

void EncryptionFrameHeader::setSequenceNumber(bsl::size_t sequenceNumber)
{
    d_sequenceNumber = static_cast<bsl::uint32_t>(sequenceNumber);
}

void EncryptionFrameHeader::setLength(bsl::size_t length)
{
    d_length = static_cast<bsl::uint32_t>(length);
}

ntsa::Error EncryptionFrameHeader::decode(bsl::streambuf* source)
{
    if (sizeof d_type !=
        source->sgetn(reinterpret_cast<char*>(&d_type), sizeof d_type))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (sizeof d_sequenceNumber !=
        source->sgetn(reinterpret_cast<char*>(&d_sequenceNumber),
                      sizeof d_sequenceNumber))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (sizeof d_length !=
        source->sgetn(reinterpret_cast<char*>(&d_length), sizeof d_length))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error EncryptionFrameHeader::encode(bsl::streambuf* destination) const
{
    if (sizeof d_type !=
        destination->sputn(reinterpret_cast<const char*>(&d_type),
                           sizeof d_type))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (sizeof d_sequenceNumber !=
        destination->sputn(reinterpret_cast<const char*>(&d_sequenceNumber),
                           sizeof d_sequenceNumber))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (sizeof d_length !=
        destination->sputn(reinterpret_cast<const char*>(&d_length),
                           sizeof d_length))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntcd::EncryptionFrameType::Value EncryptionFrameHeader::type() const
{
    return static_cast<EncryptionFrameType::Value>(
        static_cast<unsigned int>(d_type));
}

bsl::size_t EncryptionFrameHeader::sequenceNumber() const
{
    return d_sequenceNumber;
}

bsl::size_t EncryptionFrameHeader::length() const
{
    return d_length;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const ntcd::EncryptionFrameHeader& object)
{
    return stream << "[ type = " << object.type()
                  << " sequenceNumber = " << object.sequenceNumber()
                  << " length = " << object.length() << " ]";
}

EncryptionHandshake::EncryptionHandshake(bslma::Allocator* basicAllocator)
: d_role(ntca::EncryptionRole::e_CLIENT)
, d_certificate_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionHandshake::EncryptionHandshake(const EncryptionHandshake& original,
                                         bslma::Allocator* basicAllocator)
: d_role(original.d_role)
, d_certificate_sp(original.d_certificate_sp)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionHandshake::~EncryptionHandshake()
{
}

EncryptionHandshake& EncryptionHandshake::operator=(
    const EncryptionHandshake& other)
{
    if (this != &other) {
        d_role           = other.d_role;
        d_certificate_sp = other.d_certificate_sp;
    }

    return *this;
}

void EncryptionHandshake::setRole(ntca::EncryptionRole::Value role)
{
    d_role = role;
}

void EncryptionHandshake::setCertificate(
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& certificate)
{
    d_certificate_sp = certificate;
}

ntsa::Error EncryptionHandshake::decode(bsl::streambuf* source)
{
    ntsa::Error error;

    {
        bdlb::BigEndianUint32 bigEndianRole;

        if (sizeof bigEndianRole !=
            source->sgetn(reinterpret_cast<char*>(&bigEndianRole),
                          sizeof bigEndianRole))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        int roleNumber =
            static_cast<int>(static_cast<unsigned int>(bigEndianRole));

        if (0 != ntca::EncryptionRole::fromInt(&d_role, roleNumber)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntca::EncryptionResourceOptions certificateStorageOptions;

    d_certificate_sp.createInplace(d_allocator_p, d_allocator_p);
    error = d_certificate_sp->decode(source, certificateStorageOptions);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionHandshake::encode(bsl::streambuf* destination) const
{
    ntsa::Error error;

    {
        bdlb::BigEndianUint32 bigEndianRole;
        bigEndianRole = static_cast<unsigned int>(d_role);

        if (sizeof bigEndianRole !=
            destination->sputn(reinterpret_cast<const char*>(&bigEndianRole),
                               sizeof bigEndianRole))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (!d_certificate_sp) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntca::EncryptionResourceOptions certificateStorageOptions;

    error = d_certificate_sp->encode(destination, certificateStorageOptions);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntca::EncryptionRole::Value EncryptionHandshake::role() const
{
    return d_role;
}

const bsl::shared_ptr<ntcd::EncryptionCertificate>& EncryptionHandshake::
    certificate() const
{
    return d_certificate_sp;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const ntcd::EncryptionHandshake& object)
{
    stream << "[ role = " << object.role();

    if (object.certificate()) {
        stream << " certificate = " << *object.certificate();
    }

    stream << " ]";

    return stream;
}

EncryptionAcceptance::EncryptionAcceptance()
: d_value()
{
}

EncryptionAcceptance::EncryptionAcceptance(
    const EncryptionAcceptance& original)
: d_value(original.d_value)
{
}

EncryptionAcceptance::~EncryptionAcceptance()
{
}

EncryptionAcceptance& EncryptionAcceptance::operator=(
    const EncryptionAcceptance& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionAcceptance::setValue(bool value)
{
    d_value = static_cast<unsigned int>(value);
}

ntsa::Error EncryptionAcceptance::decode(bsl::streambuf* source)
{
    if (sizeof d_value !=
        source->sgetn(reinterpret_cast<char*>(&d_value), sizeof d_value))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error EncryptionAcceptance::encode(bsl::streambuf* destination) const
{
    if (sizeof d_value !=
        destination->sputn(reinterpret_cast<const char*>(&d_value),
                           sizeof d_value))
    {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

bool EncryptionAcceptance::value() const
{
    return static_cast<bool>(static_cast<unsigned int>(d_value));
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const ntcd::EncryptionAcceptance& object)
{
    return stream << "[ value = " << object.value() << " ]";
}

ntsa::Error Encryption::enqueueOutgoingHello()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<bdlbb::Blob> payloadBlob =
        d_dataPool_sp->createOutgoingBlob();
    {
        ntcd::EncryptionHandshake handshake;
        handshake.setRole(d_role);
        handshake.setCertificate(d_sourceCertificate_sp);

        NTCI_LOG_TRACE("Encryption enqueuing outgoing hello in role %s",
                       ntca::EncryptionRole::toString(d_role));

        bdlbb::OutBlobStreamBuf osb(payloadBlob.get());

        error = handshake.encode(&osb);
        if (error) {
            return error;
        }

        if (0 != osb.pubsync()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bsl::shared_ptr<bdlbb::Blob> headerBlob =
        d_dataPool_sp->createOutgoingBlob();
    {
        ntcd::EncryptionFrameHeader header;

        header.setType(ntcd::EncryptionFrameType::e_HELLO);
        header.setSequenceNumber(d_sequenceNumber++);
        header.setLength(payloadBlob->length());

        bdlbb::OutBlobStreamBuf osb(headerBlob.get());

        error = header.encode(&osb);
        if (error) {
            return error;
        }

        if (0 != osb.pubsync()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bdlbb::BlobUtil::append(d_outgoingCipherText_sp.get(), *headerBlob);
    bdlbb::BlobUtil::append(d_outgoingCipherText_sp.get(), *payloadBlob);

    return ntsa::Error();
}

ntsa::Error Encryption::enqueueOutgoingAccept(bool value)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<bdlbb::Blob> payloadBlob =
        d_dataPool_sp->createOutgoingBlob();
    {
        ntcd::EncryptionAcceptance acceptance;
        acceptance.setValue(value);

        NTCI_LOG_TRACE("Encryption enqueuing outgoing accept: %s",
                       (value ? "AUTHORIZED" : "REJECTED"));

        bdlbb::OutBlobStreamBuf osb(payloadBlob.get());

        error = acceptance.encode(&osb);
        if (error) {
            return error;
        }

        if (0 != osb.pubsync()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bsl::shared_ptr<bdlbb::Blob> headerBlob =
        d_dataPool_sp->createOutgoingBlob();
    {
        ntcd::EncryptionFrameHeader header;
        header.setType(ntcd::EncryptionFrameType::e_ACCEPT);

        header.setSequenceNumber(d_sequenceNumber++);
        header.setLength(payloadBlob->length());

        bdlbb::OutBlobStreamBuf osb(headerBlob.get());

        error = header.encode(&osb);
        if (error) {
            return error;
        }

        if (0 != osb.pubsync()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bdlbb::BlobUtil::append(d_outgoingCipherText_sp.get(), *headerBlob);
    bdlbb::BlobUtil::append(d_outgoingCipherText_sp.get(), *payloadBlob);

    return ntsa::Error();
}

ntsa::Error Encryption::enqueueOutgoingData()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    while (d_outgoingPlainText_sp->length() > 0) {
        bsl::shared_ptr<bdlbb::Blob> payloadBlob =
            d_dataPool_sp->createOutgoingBlob();

        if (d_outgoingPlainText_sp->length() > k_MAX_DATA_RECORD_SIZE) {
            bdlbb::BlobUtil::append(payloadBlob.get(),
                                    *d_outgoingPlainText_sp,
                                    0,
                                    k_MAX_DATA_RECORD_SIZE);
        }
        else {
            bdlbb::BlobUtil::append(payloadBlob.get(),
                                    *d_outgoingPlainText_sp);
        }

        bsl::shared_ptr<bdlbb::Blob> headerBlob =
            d_dataPool_sp->createOutgoingBlob();
        {
            ntcd::EncryptionFrameHeader header;

            header.setType(ntcd::EncryptionFrameType::e_DATA);
            header.setSequenceNumber(d_sequenceNumber++);
            header.setLength(payloadBlob->length());

            NTCI_LOG_TRACE("Encryption enqueuing outgoing data sequence "
                           "number %d length %d",
                           (int)(header.sequenceNumber()),
                           (int)(header.length()));

            bdlbb::OutBlobStreamBuf osb(headerBlob.get());

            error = header.encode(&osb);
            if (error) {
                return error;
            }

            if (0 != osb.pubsync()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        bdlbb::BlobUtil::append(d_outgoingCipherText_sp.get(), *headerBlob);
        bdlbb::BlobUtil::append(d_outgoingCipherText_sp.get(), *payloadBlob);

        bdlbb::BlobUtil::erase(d_outgoingPlainText_sp.get(),
                               0,
                               payloadBlob->length());
    }

    return ntsa::Error();
}

ntsa::Error Encryption::enqueueOutgoingGoodbye()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<bdlbb::Blob> headerBlob =
        d_dataPool_sp->createOutgoingBlob();
    {
        ntcd::EncryptionFrameHeader header;

        header.setType(ntcd::EncryptionFrameType::e_GOODBYE);
        header.setSequenceNumber(d_sequenceNumber++);
        header.setLength(0);

        NTCI_LOG_TRACE("Encryption enqueuing outgoing goodbye");

        bdlbb::OutBlobStreamBuf osb(headerBlob.get());

        error = header.encode(&osb);
        if (error) {
            return error;
        }

        if (0 != osb.pubsync()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bdlbb::BlobUtil::append(d_outgoingCipherText_sp.get(), *headerBlob);

    return ntsa::Error();
}

ntsa::Error Encryption::processIncomingFrameHeader()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    BSLS_ASSERT(d_incomingHeader.isNull());
    BSLS_ASSERT(NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                       d_incomingCipherText_sp->length()) >=
                sizeof(ntcd::EncryptionFrameHeader));

    ntcd::EncryptionFrameHeader header;
    {
        bdlbb::InBlobStreamBuf isb(d_incomingCipherText_sp.get());
        error = header.decode(&isb);
        if (error) {
            return error;
        }
    }

    bdlbb::BlobUtil::erase(d_incomingCipherText_sp.get(),
                           0,
                           sizeof(ntcd::EncryptionFrameHeader));

    d_incomingHeader = header;

    NTCI_LOG_DEBUG(
        "Encryption parsed incoming frame header type %s length %d",
        ntcd::EncryptionFrameType::toString(d_incomingHeader.value().type()),
        (int)(d_incomingHeader.value().length()));

    return ntsa::Error();
}

ntsa::Error Encryption::processIncomingFramePayload()
{
    ntsa::Error error;

    BSLS_ASSERT(!d_incomingHeader.isNull());
    BSLS_ASSERT(NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                       d_incomingCipherText_sp->length()) >=
                d_incomingHeader.value().length());

    if (d_incomingHeader.value().type() == ntcd::EncryptionFrameType::e_HELLO)
    {
        error = this->processIncomingHello();
        if (error) {
            return error;
        }
    }
    else if (d_incomingHeader.value().type() ==
             ntcd::EncryptionFrameType::e_ACCEPT)
    {
        error = this->processIncomingAccept();
        if (error) {
            return error;
        }
    }
    else if (d_incomingHeader.value().type() ==
             ntcd::EncryptionFrameType::e_DATA)
    {
        error = this->processIncomingData();
        if (error) {
            return error;
        }
    }
    else if (d_incomingHeader.value().type() ==
             ntcd::EncryptionFrameType::e_GOODBYE)
    {
        error = this->processIncomingGoodbye();
        if (error) {
            return error;
        }
    }
    else {
        NTCCFG_UNREACHABLE();
    }

    bdlbb::BlobUtil::erase(
        d_incomingCipherText_sp.get(),
        0,
        NTSCFG_WARNING_NARROW(int, d_incomingHeader.value().length()));

    d_incomingHeader.reset();

    return ntsa::Error();
}

ntsa::Error Encryption::processIncomingHello()
{
    // Client: DEFAULT -> HELLO_SENT -> ACCEPT_RECEIVED
    // Server: DEFAULT

    ntsa::Error error;

    BSLS_ASSERT(!d_incomingHeader.isNull());
    BSLS_ASSERT(NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                       d_incomingCipherText_sp->length()) >=
                d_incomingHeader.value().length());

    BSLS_ASSERT(d_incomingHeader.value().type() ==
                ntcd::EncryptionFrameType::e_HELLO);

    if (d_role == ntca::EncryptionRole::e_CLIENT) {
        if (d_handshakeState != e_ACCEPT_RECEIVED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        if (d_handshakeState != e_DEFAULT) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntcd::EncryptionHandshake handshake;
    {
        bdlbb::InBlobStreamBuf isb(d_incomingCipherText_sp.get());
        error = handshake.decode(&isb);
        if (error) {
            return error;
        }
    }

    d_handshakeState = e_HELLO_RECEIVED;

    if (!handshake.certificate()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_remoteCertificate_sp = handshake.certificate();
    d_remoteKey_sp         = handshake.certificate()->subjectKey();

    bsl::string name;
    {
        bsl::vector<bsl::string> subjectCommonName;
        if (!d_remoteCertificate_sp->subject().findCommonName(
                &subjectCommonName))
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (subjectCommonName.size() != 1) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        name = subjectCommonName.front();
    }

    if (d_authorizationSet.count(name) == 0) {
        error = this->enqueueOutgoingAccept(false);
        if (error) {
            return error;
        }

        d_handshakeState = e_ACCEPT_SENT;

        d_shutdownState.close();
        d_handshakeState = e_FAILED;
        {
            ntccfg::UnLockGuard guard(&d_mutex);
            d_handshakeCallback(ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED),
                                bsl::shared_ptr<ntci::EncryptionCertificate>(),
                                "Not authorized");
        }
        d_handshakeCallback = ntci::Encryption::HandshakeCallback();
    }
    else {
        error = this->enqueueOutgoingAccept(true);
        if (error) {
            return error;
        }

        d_handshakeState = e_ACCEPT_SENT;

        if (d_role == ntca::EncryptionRole::e_CLIENT) {
            BSLS_ASSERT(d_remoteCertificate_sp);
            d_handshakeState = e_ESTABLISHED;
            {
                ntccfg::UnLockGuard guard(&d_mutex);
                d_handshakeCallback(ntsa::Error(), d_remoteCertificate_sp, "");
            }
            d_handshakeCallback = ntci::Encryption::HandshakeCallback();
        }
        else {
            error = this->enqueueOutgoingHello();
            if (error) {
                return error;
            }

            d_handshakeState = e_HELLO_SENT;
        }
    }

    return ntsa::Error();
}

ntsa::Error Encryption::processIncomingAccept()
{
    // Client: DEFAULT -> HELLO_SENT
    // Server: DEFAULT -> HELLO_RECEIVED -> ACCEPT_SENT -> HELLO_SENT

    ntsa::Error error;

    BSLS_ASSERT(!d_incomingHeader.isNull());
    BSLS_ASSERT(NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                       d_incomingCipherText_sp->length()) >=
                d_incomingHeader.value().length());

    BSLS_ASSERT(d_incomingHeader.value().type() ==
                ntcd::EncryptionFrameType::e_ACCEPT);

    if (d_handshakeState != e_HELLO_SENT) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_handshakeState = e_ACCEPT_RECEIVED;

    ntcd::EncryptionAcceptance acceptance;
    {
        bdlbb::InBlobStreamBuf isb(d_incomingCipherText_sp.get());
        error = acceptance.decode(&isb);
        if (error) {
            return error;
        }
    }

    if (acceptance.value()) {
        if (d_role == ntca::EncryptionRole::e_SERVER) {
            BSLS_ASSERT(d_remoteCertificate_sp);
            d_handshakeState = e_ESTABLISHED;
            {
                ntccfg::UnLockGuard guard(&d_mutex);
                d_handshakeCallback(ntsa::Error(), d_remoteCertificate_sp, "");
            }
            d_handshakeCallback = ntci::Encryption::HandshakeCallback();
        }
    }
    else {
        d_shutdownState.close();
        d_handshakeState = e_FAILED;
        {
            ntccfg::UnLockGuard guard(&d_mutex);
            d_handshakeCallback(ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED),
                                bsl::shared_ptr<ntci::EncryptionCertificate>(),
                                "Not authorized");
        }
        d_handshakeCallback = ntci::Encryption::HandshakeCallback();
    }

    return ntsa::Error();
}

ntsa::Error Encryption::processIncomingData()
{
    ntsa::Error error;

    BSLS_ASSERT(!d_incomingHeader.isNull());
    BSLS_ASSERT(NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                       d_incomingCipherText_sp->length()) >=
                d_incomingHeader.value().length());

    BSLS_ASSERT(d_incomingHeader.value().type() ==
                ntcd::EncryptionFrameType::e_DATA);

    if (d_handshakeState != e_ESTABLISHED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Decode data

    bsl::shared_ptr<bdlbb::Blob> incomingCipherTextData =
        d_dataPool_sp->createIncomingBlob();

    bdlbb::BlobUtil::append(
        incomingCipherTextData.get(),
        *d_incomingCipherText_sp,
        0,
        NTSCFG_WARNING_NARROW(int, d_incomingHeader.value().length()));

    // Transform data from cipher text to plain text.

    bsl::shared_ptr<bdlbb::Blob> incomingPlainTextData =
        incomingCipherTextData;  // Or encrypt

    bdlbb::BlobUtil::append(d_incomingPlainText_sp.get(),
                            *incomingPlainTextData);

    return ntsa::Error();
}

ntsa::Error Encryption::processIncomingGoodbye()
{
    ntsa::Error error;

    BSLS_ASSERT(!d_incomingHeader.isNull());
    BSLS_ASSERT(NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                       d_incomingCipherText_sp->length()) >=
                d_incomingHeader.value().length());

    BSLS_ASSERT(d_incomingHeader.value().type() ==
                ntcd::EncryptionFrameType::e_GOODBYE);

    ntcs::ShutdownContext shutdownContext;
    d_shutdownState.tryShutdownReceive(&shutdownContext,
                                       true,
                                       ntsa::ShutdownOrigin::e_REMOTE);

    return ntsa::Error();
}

ntsa::Error Encryption::process()
{
    ntsa::Error error;

    if (d_handshakeState == e_FAILED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    while (true) {
        if (d_incomingHeader.isNull()) {
            if (NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                       d_incomingCipherText_sp->length()) >=
                sizeof(ntcd::EncryptionFrameHeader))
            {
                error = this->processIncomingFrameHeader();
                if (error) {
                    return error;
                }
            }
            else {
                break;
            }
        }

        if (!d_incomingHeader.isNull()) {
            if (NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                       d_incomingCipherText_sp->length()) >=
                d_incomingHeader.value().length())
            {
                error = this->processIncomingFramePayload();
                if (error) {
                    return error;
                }
            }
            else {
                break;
            }
        }
    }

    if (d_handshakeState == e_ESTABLISHED) {
        error = this->enqueueOutgoingData();
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

Encryption::Encryption(
    ntca::EncryptionRole::Value                         role,
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& certificate,
    const bsl::shared_ptr<ntcd::EncryptionKey>&         key,
    const bsl::shared_ptr<ntci::DataPool>&              dataPool,
    bslma::Allocator*                                   basicAllocator)
: d_mutex(NTCCFG_LOCK_INIT)
, d_role(role)
, d_incomingHeader()
, d_incomingPlainText_sp(dataPool->createIncomingBlob())
, d_incomingCipherText_sp(dataPool->createIncomingBlob())
, d_outgoingHeader()
, d_outgoingPlainText_sp(dataPool->createOutgoingBlob())
, d_outgoingCipherText_sp(dataPool->createOutgoingBlob())
, d_dataPool_sp(dataPool)
, d_authorizationSet(basicAllocator)
, d_handshakeCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_handshakeState(e_DEFAULT)
, d_shutdownState()
, d_sequenceNumber(1)
, d_sourceCertificate_sp(certificate)
, d_sourceKey_sp(key)
, d_remoteCertificate_sp()
, d_remoteKey_sp()
, d_allocator_p(basicAllocator)
{
}

Encryption::~Encryption()
{
}

void Encryption::authorizePeer(const bsl::string& name)
{
    LockGuard guard(&d_mutex);
    d_authorizationSet.insert(name);
}

ntsa::Error Encryption::initiateHandshake(const HandshakeCallback& callback)
{
    LockGuard guard(&d_mutex);

    ntsa::Error error;

    if (d_handshakeState != e_DEFAULT) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_handshakeCallback = callback;

    if (d_role == ntca::EncryptionRole::e_CLIENT) {
        error = this->enqueueOutgoingHello();
        if (error) {
            return error;
        }

        d_handshakeState = e_HELLO_SENT;
    }

    return this->process();
}

ntsa::Error Encryption::pushIncomingCipherText(const bdlbb::Blob& input)
{
    LockGuard guard(&d_mutex);

    if (!d_shutdownState.canReceive()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bdlbb::BlobUtil::append(d_incomingCipherText_sp.get(), input);

    return this->process();
}

ntsa::Error Encryption::pushIncomingCipherText(const ntsa::Data& input)
{
    LockGuard guard(&d_mutex);

    if (!d_shutdownState.canReceive()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::DataUtil::append(d_incomingCipherText_sp.get(), input);

    return this->process();
}

ntsa::Error Encryption::pushOutgoingPlainText(const bdlbb::Blob& input)
{
    LockGuard guard(&d_mutex);

    if (!d_shutdownState.canSend()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bdlbb::BlobUtil::append(d_outgoingPlainText_sp.get(), input);

    return this->process();
}

ntsa::Error Encryption::pushOutgoingPlainText(const ntsa::Data& input)
{
    LockGuard guard(&d_mutex);

    if (!d_shutdownState.canSend()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::DataUtil::append(d_outgoingPlainText_sp.get(), input);

    return this->process();
}

ntsa::Error Encryption::popIncomingPlainText(bdlbb::Blob* output)
{
    LockGuard guard(&d_mutex);

    if (d_incomingPlainText_sp->length() > 0) {
        bdlbb::BlobUtil::append(output, *d_incomingPlainText_sp);
        bdlbb::BlobUtil::erase(d_incomingPlainText_sp.get(),
                               0,
                               d_incomingPlainText_sp->length());
    }

    return this->process();
}

ntsa::Error Encryption::popOutgoingCipherText(bdlbb::Blob* output)
{
    LockGuard guard(&d_mutex);

    if (d_outgoingCipherText_sp->length() > 0) {
        bdlbb::BlobUtil::append(output, *d_outgoingCipherText_sp);
        bdlbb::BlobUtil::erase(d_outgoingCipherText_sp.get(),
                               0,
                               d_outgoingCipherText_sp->length());
    }

    return this->process();
}

ntsa::Error Encryption::shutdown()
{
    LockGuard guard(&d_mutex);

    ntsa::Error error;

    ntcs::ShutdownContext shutdownContext;
    bool result = d_shutdownState.tryShutdownSend(&shutdownContext, true);

    if (result) {
        if (shutdownContext.shutdownSend()) {
            error = this->enqueueOutgoingGoodbye();
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

bool Encryption::hasIncomingPlainText() const
{
    LockGuard guard(&d_mutex);
    return d_incomingPlainText_sp->length() > 0;
}

bool Encryption::hasOutgoingCipherText() const
{
    LockGuard guard(&d_mutex);
    return d_outgoingCipherText_sp->length() > 0;
}

bool Encryption::getCipher(bsl::string* result) const
{
    *result = "TEST";
    return true;
}

bool Encryption::isHandshakeFinished() const
{
    LockGuard guard(&d_mutex);
    return d_handshakeState == e_ESTABLISHED || d_handshakeState == e_FAILED;
}

bool Encryption::isShutdownSent() const
{
    return !d_shutdownState.canSend();
}

bool Encryption::isShutdownReceived() const
{
    return !d_shutdownState.canReceive();
}

bool Encryption::isShutdownFinished() const
{
    return d_shutdownState.completed();
}

bsl::shared_ptr<ntci::EncryptionCertificate> Encryption::sourceCertificate()
    const
{
    LockGuard guard(&d_mutex);
    return d_sourceCertificate_sp;
}

bsl::shared_ptr<ntci::EncryptionCertificate> Encryption::remoteCertificate()
    const
{
    LockGuard guard(&d_mutex);
    return d_remoteCertificate_sp;
}

bsl::shared_ptr<ntci::EncryptionKey> Encryption::privateKey() const
{
    LockGuard guard(&d_mutex);
    return d_sourceKey_sp;
}

/// Return the allocator.
bslma::Allocator* Encryption::allocator() const
{
    return d_allocator_p;
}

EncryptionClient::EncryptionClient(
    const ntca::EncryptionClientOptions& options,
    bslma::Allocator*                    basicAllocator)
: d_options(options, basicAllocator)
, d_dataPool_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(d_allocator_p, d_allocator_p);

    d_dataPool_sp = dataPool;
}

EncryptionClient::EncryptionClient(
    const ntca::EncryptionClientOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
: d_options(options, basicAllocator)
, d_dataPool_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(d_allocator_p,
                           blobBufferFactory,
                           blobBufferFactory,
                           d_allocator_p);

    d_dataPool_sp = dataPool;
}

EncryptionClient::EncryptionClient(
    const ntca::EncryptionClientOptions&   options,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
: d_options(options, basicAllocator)
, d_dataPool_sp(dataPool)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionClient::~EncryptionClient()
{
}

ntsa::Error EncryptionClient::createEncryption(
    bsl::shared_ptr<ntci::Encryption>* result,
    bslma::Allocator*                  basicAllocator)
{
    // MRM
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(basicAllocator);
#if 0
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionCertificate> certificate;

    if (!d_options.identityFile().isNull()) {
        error =
            ntcd::EncryptionCertificate::load(&certificate,
                                              d_options.identityFile().value(),
                                              allocator);
        if (error) {
            return error;
        }
    }
    else if (!d_options.identityData().isNull()) {
        error = ntcd::EncryptionCertificate::decode(
            &certificate,
            d_options.identityData().value(),
            allocator);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntcd::Encryption> encryption;
    encryption.createInplace(allocator,
                             ntca::EncryptionRole::e_CLIENT,
                             certificate,
                             certificate->subjectKey(),
                             d_dataPool_sp,
                             allocator);

    *result = encryption;
#endif
    return ntsa::Error();
}

EncryptionServer::EncryptionServer(
    const ntca::EncryptionServerOptions& options,
    bslma::Allocator*                    basicAllocator)
: d_options(options, basicAllocator)
, d_dataPool_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(d_allocator_p, d_allocator_p);

    d_dataPool_sp = dataPool;
}

EncryptionServer::EncryptionServer(
    const ntca::EncryptionServerOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
: d_options(options, basicAllocator)
, d_dataPool_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(d_allocator_p,
                           blobBufferFactory,
                           blobBufferFactory,
                           d_allocator_p);

    d_dataPool_sp = dataPool;
}

EncryptionServer::EncryptionServer(
    const ntca::EncryptionServerOptions&   options,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
: d_options(options, basicAllocator)
, d_dataPool_sp(dataPool)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionServer::~EncryptionServer()
{
}

ntsa::Error EncryptionServer::createEncryption(
    bsl::shared_ptr<ntci::Encryption>* result,
    bslma::Allocator*                  basicAllocator)
{
    // MRM
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(basicAllocator);
#if 0
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionCertificate> certificate;

    if (!d_options.identityFile().isNull()) {
        error =
            ntcd::EncryptionCertificate::load(&certificate,
                                              d_options.identityFile().value(),
                                              allocator);
        if (error) {
            return error;
        }
    }
    else if (!d_options.identityData().isNull()) {
        error = ntcd::EncryptionCertificate::decode(
            &certificate,
            d_options.identityData().value(),
            allocator);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntcd::Encryption> encryption;
    encryption.createInplace(allocator,
                             ntca::EncryptionRole::e_SERVER,
                             certificate,
                             certificate->subjectKey(),
                             d_dataPool_sp,
                             allocator);

    *result = encryption;
#endif

    return ntsa::Error();
}

EncryptionDriver::EncryptionDriver(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionDriver::~EncryptionDriver()
{
}

ntsa::Error EncryptionDriver::generateKey(
        ntca::EncryptionKey*                  result,
        const ntca::EncryptionKeyOptions&     options,
        bslma::Allocator*                     basicAllocator) 
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionKey> key;
    key.createInplace(allocator);

    error = key->generate(options);
    if (error) {
        return error;
    }

    error = key->unwrap(result);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error EncryptionDriver::generateKey(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    const ntca::EncryptionKeyOptions&     options,
    bslma::Allocator*                     basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionKey> key;
    key.createInplace(allocator);

    error = key->generate(options);
    if (error) {
        return error;
    }

    *result = key;

    return ntsa::Error();
}

ntsa::Error EncryptionDriver::encodeKey(
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionResourceOptions&      options)
{
    return privateKey->encode(destination, options);
}

ntsa::Error EncryptionDriver::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>*  result,
    bsl::streambuf*                        source,
    const ntca::EncryptionResourceOptions& options,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionKey> key;
    key.createInplace(allocator);

    error = key->decode(source, options);
    if (error) {
        return error;
    }

    *result = key;
    return ntsa::Error();
}

ntsa::Error EncryptionDriver::generateCertificate(
    ntca::EncryptionCertificate*                  result,
    const ntsa::DistinguishedName&                subjectIdentity,
    const ntca::EncryptionKey&                    subjectPrivateKey,
    const ntca::EncryptionCertificateOptions&     options,
    bslma::Allocator*                             basicAllocator) 
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(subjectIdentity);
    NTCCFG_WARNING_UNUSED(subjectPrivateKey);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(basicAllocator);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionDriver::generateCertificate(
    ntca::EncryptionCertificate*              result,
    const ntsa::DistinguishedName&            subjectIdentity,
    const ntca::EncryptionKey&                subjectPrivateKey,
    const ntca::EncryptionCertificate&        issuerCertificate,
    const ntca::EncryptionKey&                issuerPrivateKey,
    const ntca::EncryptionCertificateOptions& options,
    bslma::Allocator*                         basicAllocator) 
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(subjectIdentity);
    NTCCFG_WARNING_UNUSED(subjectPrivateKey);
    NTCCFG_WARNING_UNUSED(issuerCertificate);
    NTCCFG_WARNING_UNUSED(issuerPrivateKey);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(basicAllocator);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionDriver::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const ntsa::DistinguishedName&                subjectIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&   subjectPrivateKey,
    const ntca::EncryptionCertificateOptions&     options,
    bslma::Allocator*                             basicAllocator)
{
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionKey> concreteSubjectPrivateKey;
    bslstl::SharedPtrUtil::dynamicCast(&concreteSubjectPrivateKey,
                                       subjectPrivateKey);
    if (!concreteSubjectPrivateKey) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntcd::EncryptionCertificate> certificate;
    certificate.createInplace(allocator, allocator);

    error = certificate->generate(subjectIdentity,
                                  concreteSubjectPrivateKey,
                                  options);
    if (error) {
        return error;
    }

    *result = certificate;

    return ntsa::Error();
}

ntsa::Error EncryptionDriver::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
    const ntsa::DistinguishedName&                      subjectIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&         subjectPrivateKey,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& issuerCertificate,
    const bsl::shared_ptr<ntci::EncryptionKey>&         issuerPrivateKey,
    const ntca::EncryptionCertificateOptions&           options,
    bslma::Allocator*                                   basicAllocator)
{
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionKey> concreteSubjectPrivateKey;
    bslstl::SharedPtrUtil::dynamicCast(&concreteSubjectPrivateKey,
                                       subjectPrivateKey);
    if (!concreteSubjectPrivateKey) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntcd::EncryptionCertificate> concreteIssuerCertificate;
    bslstl::SharedPtrUtil::dynamicCast(&concreteIssuerCertificate,
                                       issuerCertificate);
    if (!concreteIssuerCertificate) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntcd::EncryptionKey> concreteIssuerPrivateKey;
    bslstl::SharedPtrUtil::dynamicCast(&concreteIssuerPrivateKey,
                                       issuerPrivateKey);
    if (!concreteIssuerPrivateKey) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntcd::EncryptionCertificate> certificate;
    certificate.createInplace(allocator, allocator);

    error = certificate->generate(subjectIdentity,
                                  concreteSubjectPrivateKey,
                                  concreteIssuerCertificate,
                                  concreteIssuerPrivateKey,
                                  options);
    if (error) {
        return error;
    }

    *result = certificate;

    return ntsa::Error();
}

ntsa::Error EncryptionDriver::encodeCertificate(
    bsl::streambuf*                                     destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    return certificate->encode(destination, options);
}

ntsa::Error EncryptionDriver::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    bsl::streambuf*                               source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionCertificate> certificate;
    certificate.createInplace(allocator, allocator);

    error = certificate->decode(source, options);
    if (error) {
        return error;
    }

    *result = certificate;
    return ntsa::Error();
}

ntsa::Error EncryptionDriver::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionClient> client;
    client.createInplace(allocator, options, allocator);

    *result = client;
    return ntsa::Error();
}

ntsa::Error EncryptionDriver::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>*         result,
    const ntca::EncryptionClientOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionClient> client;
    client.createInplace(allocator, options, blobBufferFactory, allocator);

    *result = client;
    return ntsa::Error();
}

ntsa::Error EncryptionDriver::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionClient> client;
    client.createInplace(allocator, options, dataPool, allocator);

    *result = client;
    return ntsa::Error();
}

ntsa::Error EncryptionDriver::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionServer> server;
    server.createInplace(allocator, options, allocator);

    *result = server;
    return ntsa::Error();
}

ntsa::Error EncryptionDriver::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>*         result,
    const ntca::EncryptionServerOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionServer> server;
    server.createInplace(allocator, options, blobBufferFactory, allocator);

    *result = server;
    return ntsa::Error();
}

ntsa::Error EncryptionDriver::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntcd::EncryptionServer> server;
    server.createInplace(allocator, options, dataPool, allocator);

    *result = server;
    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
