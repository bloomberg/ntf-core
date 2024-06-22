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

#ifndef INCLUDED_NTCD_ENCRYPTION
#define INCLUDED_NTCD_ENCRYPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_datapool.h>
#include <ntci_encryption.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptiondriver.h>
#include <ntci_encryptionkey.h>
#include <ntcs_shutdownstate.h>
#include <ntcscm_version.h>
#include <ntsa_data.h>
#include <bdlb_bigendian.h>
#include <bdlb_nullablevalue.h>
#include <bdlbb_blob.h>
#include <bsls_atomic.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcd {

/// @internal @brief
/// Provide an encryption key for an encryption designed for testing.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class EncryptionKey : public ntci::EncryptionKey
{
    bsl::uint32_t d_value;

  private:
    EncryptionKey(const EncryptionKey&) BSLS_KEYWORD_DELETED;
    EncryptionKey& operator=(const EncryptionKey&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new encryption key with a default value.
    EncryptionKey();

    /// Create a new encryption key having the specified 'value'.
    explicit EncryptionKey(bsl::uint32_t value);

    /// Destroy this object.
    ~EncryptionKey() BSLS_KEYWORD_OVERRIDE;

    /// Generate a new key according to the specified 'options'. Return the
    /// error.
    ntsa::Error generate(const ntca::EncryptionKeyOptions& options);

    /// Decode the key from the specified 'source' according to the specified
    /// 'options'.
    ntsa::Error decode(bsl::streambuf*                        source,
                       const ntca::EncryptionResourceOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Encode the key to the specified 'destination' according to the
    /// specified 'options'.
    ntsa::Error encode(bsl::streambuf*                        destination,
                       const ntca::EncryptionResourceOptions& options) const
        BSLS_KEYWORD_OVERRIDE;

    /// Return a handle to the private implementation.
    void* handle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the value of the key.
    bsl::uint32_t value() const;
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntcd::EncryptionKey
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const ntcd::EncryptionKey& object);

/// @internal @brief
/// Provide an encryption certificate for an encryption designed for testing.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class EncryptionCertificate : public ntci::EncryptionCertificate
{
    ntsa::DistinguishedName              d_subject;
    bsl::shared_ptr<ntcd::EncryptionKey> d_subjectKey_sp;
    ntsa::DistinguishedName              d_issuer;
    bsl::shared_ptr<ntcd::EncryptionKey> d_issuerKey_sp;
    bslma::Allocator*                    d_allocator_p;

  private:
    EncryptionCertificate(const EncryptionCertificate&) BSLS_KEYWORD_DELETED;
    EncryptionCertificate& operator=(const EncryptionCertificate&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new encryption certificate having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionCertificate(bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption certificate having the specified 'subject'
    /// having the specified 'subjectKey' issued by itsef. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    EncryptionCertificate(
        const ntsa::DistinguishedName&              subject,
        const bsl::shared_ptr<ntcd::EncryptionKey>& subjectKey,
        bslma::Allocator*                           basicAllocator = 0);

    /// Create a new encryption certificate having the specified 'subject'
    /// having the specified 'subjectKey' issued by the specified 'issuer'
    /// having the specified 'issuerKey'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    EncryptionCertificate(
        const ntsa::DistinguishedName&              subject,
        const bsl::shared_ptr<ntcd::EncryptionKey>& subjectKey,
        const ntsa::DistinguishedName&              issuer,
        const bsl::shared_ptr<ntcd::EncryptionKey>& issuerKey,
        bslma::Allocator*                           basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionCertificate() BSLS_KEYWORD_OVERRIDE;

    /// Generate a new certificate according to the specified 'options' for the
    /// specified 'subjectIdentity' and 'subjectPrivateKey' signed by itself.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    ntsa::Error generate(
        const ntsa::DistinguishedName&              subjectIdentity,
        const bsl::shared_ptr<ntcd::EncryptionKey>& subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&   options);

    /// Generate a new certificate according to the specified 'options' for the
    /// specified 'subjectIdentity' and 'subjectPrivateKey' signed by the
    /// certificate authority identified by the specified 'issuerCertificate'
    /// that uses the specified 'issuerPrivateKey'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used. Return the error.
    ntsa::Error generate(
        const ntsa::DistinguishedName&                      subjectIdentity,
        const bsl::shared_ptr<ntcd::EncryptionKey>&         subjectPrivateKey,
        const bsl::shared_ptr<ntcd::EncryptionCertificate>& issuerCertificate,
        const bsl::shared_ptr<ntcd::EncryptionKey>&         issuerPrivateKey,
        const ntca::EncryptionCertificateOptions&           options);

    /// Decode the certificate from the specified 'source' according to the
    /// specified 'options'. Return the error.
    ntsa::Error decode(bsl::streambuf*                        source,
                       const ntca::EncryptionResourceOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Encode the certificate to the specified 'destination' according to the
    /// specified 'options'. Return the error.
    ntsa::Error encode(bsl::streambuf*                        destination,
                       const ntca::EncryptionResourceOptions& options) const
        BSLS_KEYWORD_OVERRIDE;

    /// Print the certificate to the specified stream in an unspecified but
    /// human-readable form.
    void print(bsl::ostream& stream) const BSLS_KEYWORD_OVERRIDE;

    /// Return the subject of the certificate.
    const ntsa::DistinguishedName& subject() const BSLS_KEYWORD_OVERRIDE;

    /// Return the issuer of the certificate.
    const ntsa::DistinguishedName& issuer() const BSLS_KEYWORD_OVERRIDE;

    /// Return a handle to the private implementation.
    void* handle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the subject key.
    const bsl::shared_ptr<ntcd::EncryptionKey>& subjectKey() const;

    /// Return the issuer key.
    const bsl::shared_ptr<ntcd::EncryptionKey>& issuerKey() const;

    /// Return a distinguished name having the specified 'commonName'.
    static ntsa::DistinguishedName distinguishedName(
        const bsl::string& commonName);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntcd::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const ntcd::EncryptionCertificate& object);

/// @internal @brief
/// Enumerate the types of encryption records in an encryption suitable for
/// testing.
///
/// @par Thread Safety
/// This struct is thread safe.
struct EncryptionFrameType {
  public:
    /// Enumerate the types of encryption records in an encryption suitable for
    /// testing.
    enum Value {
        /// The record describes a hello message.
        e_HELLO,

        /// The record describes an accept message.
        e_ACCEPT,

        /// The record describes encrypted data.
        e_DATA,

        /// The record describes a server goodbye message.
        e_GOODBYE
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntcd::EncryptionFrameType
bsl::ostream& operator<<(bsl::ostream& stream, EncryptionFrameType::Value rhs);

/// @internal @brief
/// Describe an encrypted data frame header of an encryption suitable for
/// testing.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcd
class EncryptionFrameHeader
{
    bdlb::BigEndianUint32 d_type;
    bdlb::BigEndianUint32 d_sequenceNumber;
    bdlb::BigEndianUint32 d_length;

  public:
    /// Create a new encryption header having a default value.
    EncryptionFrameHeader();

    /// Create a new encryption header having the same value as the
    /// specified 'original' object.
    EncryptionFrameHeader(const EncryptionFrameHeader& original);

    /// Destroy this object.
    ~EncryptionFrameHeader();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionFrameHeader& operator=(const EncryptionFrameHeader& other);

    /// Set the type of the data in the encryption record described by this
    /// header to the specified 'type'.
    void setType(ntcd::EncryptionFrameType::Value type);

    /// Set the sequence number of the data in the encryption record
    /// described by this header to the specified 'sequenceNumber'.
    void setSequenceNumber(bsl::size_t sequenceNumber);

    /// Set the length of the data in the encryption record described by
    /// this header to the specified 'length'.
    void setLength(bsl::size_t length);

    /// Decode this object from the specified 'source'. Return the error.
    ntsa::Error decode(bsl::streambuf* source);

    /// Encode this object to the specified 'destination'. Return the error.
    ntsa::Error encode(bsl::streambuf* destination) const;

    /// Return the type of the data in the encryption record described by
    /// this header to the specified 'sequenceNumber'.
    ntcd::EncryptionFrameType::Value type() const;

    /// Return the sequence number of the data in the encryption record
    /// described by this header.
    bsl::size_t sequenceNumber() const;

    /// Return the length of the data in the encryption record described by
    /// this header.
    bsl::size_t length() const;
};

// FREE OPERATORS

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntcd::EncryptionFrameHeader
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const ntcd::EncryptionFrameHeader& object);

/// @internal @brief
/// Describe the handshake of an encryption suitable for testing.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcd
class EncryptionHandshake
{
    ntca::EncryptionRole::Value                  d_role;
    bsl::shared_ptr<ntcd::EncryptionCertificate> d_certificate_sp;
    bslma::Allocator*                            d_allocator_p;

  public:
    /// Create a new encryption handshake having a default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit EncryptionHandshake(bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption handshake having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    EncryptionHandshake(const EncryptionHandshake& original,
                        bslma::Allocator*          basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionHandshake();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionHandshake& operator=(const EncryptionHandshake& other);

    /// Set the role of the encryption endpoint to the specified 'role'.
    void setRole(ntca::EncryptionRole::Value role);

    /// Set the certificate of the encryption endpoint to the specified
    /// 'certificate'.
    void setCertificate(
        const bsl::shared_ptr<ntcd::EncryptionCertificate>& certificate);

    /// Decode this object from the specified 'source'. Return the error.
    ntsa::Error decode(bsl::streambuf* source);

    /// Encode this object to the specified 'destination'. Return the error.
    ntsa::Error encode(bsl::streambuf* destination) const;

    /// Return the role of the encryption endpoint.
    ntca::EncryptionRole::Value role() const;

    /// Return the name of the encryption endpoint.
    const bsl::shared_ptr<ntcd::EncryptionCertificate>& certificate() const;
};

// FREE OPERATORS

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntcd::EncryptionHandshake
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const ntcd::EncryptionHandshake& object);

/// Describe acceptance of the handshake of an encryption suitable for testing.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcd
class EncryptionAcceptance
{
    bdlb::BigEndianUint32 d_value;

  public:
    /// Create a new encryption handshake acceptance having a default value.
    EncryptionAcceptance();

    /// Create a new encryption handshake acceptance having the same value
    /// as the specified 'original' object.
    EncryptionAcceptance(const EncryptionAcceptance& original);

    /// Destroy this object.
    ~EncryptionAcceptance();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionAcceptance& operator=(const EncryptionAcceptance& other);

    /// Set the value of the acceptance of the handshake to the specified
    /// 'value'.
    void setValue(bool value);

    /// Decode this object from the specified 'source'. Return the error.
    ntsa::Error decode(bsl::streambuf* source);

    /// Encode this object to the specified 'destination'. Return the error.
    ntsa::Error encode(bsl::streambuf* destination) const;

    /// Return the value of the acceptance of the handshake.
    bool value() const;
};

// FREE OPERATORS

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntcd::EncryptionAcceptance
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const ntcd::EncryptionAcceptance& object);

/// @internal @brief
/// Provide an encryption suitable for testing.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class Encryption : public ntci::Encryption
{
    enum HandshakeState {
        e_DEFAULT,
        e_HELLO_SENT,
        e_HELLO_RECEIVED,
        e_ACCEPT_SENT,
        e_ACCEPT_RECEIVED,
        e_ESTABLISHED,
        e_FAILED
    };

    enum { k_MAX_DATA_RECORD_SIZE = 256 };

    /// Define a type alias for a vector of peer names
    /// authorized to complete the handshake.
    typedef bsl::unordered_set<bsl::string> AuthorizationSet;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex                                    d_mutex;
    ntca::EncryptionRole::Value                      d_role;
    bdlb::NullableValue<ntcd::EncryptionFrameHeader> d_incomingHeader;
    bsl::shared_ptr<bdlbb::Blob>                     d_incomingPlainText_sp;
    bsl::shared_ptr<bdlbb::Blob>                     d_incomingCipherText_sp;
    bdlb::NullableValue<ntcd::EncryptionFrameHeader> d_outgoingHeader;
    bsl::shared_ptr<bdlbb::Blob>                     d_outgoingPlainText_sp;
    bsl::shared_ptr<bdlbb::Blob>                     d_outgoingCipherText_sp;
    bsl::shared_ptr<ntci::DataPool>                  d_dataPool_sp;
    AuthorizationSet                                 d_authorizationSet;
    ntci::Encryption::HandshakeCallback              d_handshakeCallback;
    HandshakeState                                   d_handshakeState;
    ntcs::ShutdownState                              d_shutdownState;
    bsl::size_t                                      d_sequenceNumber;
    bsl::shared_ptr<ntcd::EncryptionCertificate>     d_sourceCertificate_sp;
    bsl::shared_ptr<ntcd::EncryptionKey>             d_sourceKey_sp;
    bsl::shared_ptr<ntcd::EncryptionCertificate>     d_remoteCertificate_sp;
    bsl::shared_ptr<ntcd::EncryptionKey>             d_remoteKey_sp;
    bslma::Allocator*                                d_allocator_p;

  private:
    Encryption(const Encryption&) BSLS_KEYWORD_DELETED;
    Encryption& operator=(const Encryption&) BSLS_KEYWORD_DELETED;

  private:
    /// Encode a hello record and enqueue it to the outgoing cipher
    /// text. Return the error.
    ntsa::Error enqueueOutgoingHello();

    /// Encode an accept record having the specified 'value' and enqueue it
    /// to the outgoing cipher text. Return the error.
    ntsa::Error enqueueOutgoingAccept(bool value);

    /// Encode a data record and enqueue it to the outgoing cipher
    /// text. Return the error.
    ntsa::Error enqueueOutgoingData();

    /// Encode a goodbye record and enqueue it to the outgoing cipher
    /// text. Return the error.
    ntsa::Error enqueueOutgoingGoodbye();

    /// Process an incoming frame header. The behavior is undefined unless
    /// the there is no currently parsed frame header and the incoming
    /// cipher text size is greater than or equal to the fixed frame header
    /// size. Return the error.
    ntsa::Error processIncomingFrameHeader();

    /// Process an incoming frame payload. The behavior is undefined unless
    /// there exists a currently parsed frame header and the incoming
    /// cipher text size is greater than or equal to the size of the payload
    /// indicated in the parsed frame header. Return the error.
    ntsa::Error processIncomingFramePayload();

    /// Process an incoming hello payload. Return the error.
    ntsa::Error processIncomingHello();

    /// Process an incoming accept payload. Return the error.
    ntsa::Error processIncomingAccept();

    /// Process an incoming data payload. Return the error.
    ntsa::Error processIncomingData();

    /// Process an incoming goodbye payload. Return the error.
    ntsa::Error processIncomingGoodbye();

    /// Process the available data through the encryption state machine.
    /// Return the error.
    ntsa::Error process();

  public:
    /// Create a new encryption operating in the specified 'role' from an
    /// encryption endpoint having the specified 'name'. Allocate data using
    /// the specified 'dataPool'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Encryption(ntca::EncryptionRole::Value                         role,
               const bsl::shared_ptr<ntcd::EncryptionCertificate>& certificate,
               const bsl::shared_ptr<ntcd::EncryptionKey>&         key,
               const bsl::shared_ptr<ntci::DataPool>&              dataPool,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Encryption() BSLS_KEYWORD_OVERRIDE;

    /// Authorize handshakes with peers having the specified 'name'.
    void authorizePeer(const bsl::string& name);

    /// Initiate the handshake to begin the session. Invoke the specified
    /// 'callback' when the handshake completes. Return the error.
    ntsa::Error initiateHandshake(const HandshakeCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'input' containing ciphertext read from the peer.
    /// Return 0 on success and a non-zero value otherwise.
    ntsa::Error pushIncomingCipherText(const bdlbb::Blob& input)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'input' containing ciphertext read from the peer.
    /// Return 0 on success and a non-zero value otherwise.
    ntsa::Error pushIncomingCipherText(const ntsa::Data& input)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'input' containing plaintext to be sent to the
    /// peer. Return 0 on success and a non-zero value otherwise.
    ntsa::Error pushOutgoingPlainText(const bdlbb::Blob& input)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'input' containing plaintext to be sent to the
    /// peer. Return 0 on success and a non-zero value otherwise.
    ntsa::Error pushOutgoingPlainText(const ntsa::Data& input)
        BSLS_KEYWORD_OVERRIDE;

    /// Pop plaintext read from the peer and append it to the specified
    /// 'output'. Return 0 on success and a non-zero value otherwise.
    ntsa::Error popIncomingPlainText(bdlbb::Blob* output)
        BSLS_KEYWORD_OVERRIDE;

    /// Pop ciphertext to be read from the peer and append to the specified
    /// 'output'.
    ntsa::Error popOutgoingCipherText(bdlbb::Blob* output)
        BSLS_KEYWORD_OVERRIDE;

    /// Initiate the shutdown of the session.
    ntsa::Error shutdown() BSLS_KEYWORD_OVERRIDE;

    /// Return true if plaintext data is ready to be read.
    bool hasIncomingPlainText() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if ciphertext data is ready to be sent.
    bool hasOutgoingCipherText() const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the cipher used to encrypt data
    /// passing through the filter. Return true if such a cipher has been
    /// negotiated, and false otherwise.
    bool getCipher(bsl::string* result) const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the handshake is finished, otherwise return false.
    bool isHandshakeFinished() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the shutdown has been sent, otherwise return false.
    bool isShutdownSent() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the shutdown has been received, otherwise return
    /// false.
    bool isShutdownReceived() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the shutdown is finished, otherwise return false.
    bool isShutdownFinished() const BSLS_KEYWORD_OVERRIDE;

    /// Return the source certificate used by the encryption session, if
    /// any.
    bsl::shared_ptr<ntci::EncryptionCertificate> sourceCertificate() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the remote certificate used by the encryption session, if
    /// any.
    bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the private key used by the encryption session, if any.
    bsl::shared_ptr<ntci::EncryptionKey> privateKey() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the allocator.
    bslma::Allocator* allocator() const;
};

/// @internal @brief
/// Provide a mechanism to create an encryptor in the client role.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class EncryptionClient : public ntci::EncryptionClient
{
    ntca::EncryptionClientOptions   d_options;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    bslma::Allocator*               d_allocator_p;

  private:
    EncryptionClient(const EncryptionClient&) BSLS_KEYWORD_DELETED;
    EncryptionClient& operator=(const EncryptionClient&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new encryption client that produces encryption sessions
    /// configured according to the specified 'options'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit EncryptionClient(const ntca::EncryptionClientOptions& options,
                              bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption client that produces encryption sessions
    /// configured according to the specified 'options'. Allocate blob
    /// buffers using the specified 'blobBufferFactory'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit EncryptionClient(
        const ntca::EncryptionClientOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Create a new encryption client that produces encryption sessions
    /// configured according to the specified 'options'. Allocate blob
    /// buffers using the specified 'dataPool'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit EncryptionClient(const ntca::EncryptionClientOptions&   options,
                              const bsl::shared_ptr<ntci::DataPool>& dataPool,
                              bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionClient() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new client-side encryption
    /// session. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    ntsa::Error createEncryption(bsl::shared_ptr<ntci::Encryption>* result,
                                 bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide a mechanism to create an encryptor in the server role.
///
/// @par Thread Safety
/// This class is thread safe.
class EncryptionServer : public ntci::EncryptionServer
{
    ntca::EncryptionServerOptions   d_options;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    bslma::Allocator*               d_allocator_p;

  private:
    EncryptionServer(const EncryptionServer&) BSLS_KEYWORD_DELETED;
    EncryptionServer& operator=(const EncryptionServer&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new encryption server that produces encryption sessions
    /// configured according to the specified 'options'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit EncryptionServer(const ntca::EncryptionServerOptions& options,
                              bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption server that produces encryption sessions
    /// configured according to the specified 'options'. Allocate blob
    /// buffers using the specified 'blobBufferFactory'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit EncryptionServer(
        const ntca::EncryptionServerOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Create a new encryption server that produces encryption sessions
    /// configured according to the specified 'options'. Allocate blob
    /// buffers using the specified 'dataPool'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit EncryptionServer(const ntca::EncryptionServerOptions&   options,
                              const bsl::shared_ptr<ntci::DataPool>& dataPool,
                              bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionServer() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new client-side encryption
    /// session. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    virtual ntsa::Error createEncryption(
        bsl::shared_ptr<ntci::Encryption>* result,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

/// @internal @brief
/// Provide an encryption suitable for testing.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class EncryptionDriver : public ntci::EncryptionDriver
{
    bslma::Allocator* d_allocator_p;

  private:
    EncryptionDriver(const EncryptionDriver&) BSLS_KEYWORD_DELETED;
    EncryptionDriver& operator=(const EncryptionDriver&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new encryption driver. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit EncryptionDriver(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionDriver() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' an RSA key generated according to
    /// the specified 'options'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateKey(
        ntca::EncryptionKey*                  result,
        const ntca::EncryptionKeyOptions&     options,
        bslma::Allocator*                     basicAllocator = 0) 
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' an RSA key generated according to
    /// the specified 'options'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                            const ntca::EncryptionKeyOptions&     options,
                            bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    ntsa::Error encodeKey(
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                          bsl::streambuf*                        source,
                          const ntca::EncryptionResourceOptions& options,
                          bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;


    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by itself. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*                  result,
        const ntsa::DistinguishedName&                subjectIdentity,
        const ntca::EncryptionKey&                    subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator*                             basicAllocator = 0) 
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified
    /// by the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            subjectIdentity,
        const ntca::EncryptionKey&                subjectPrivateKey,
        const ntca::EncryptionCertificate&        issuerCertificate,
        const ntca::EncryptionKey&                issuerPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator*                         basicAllocator = 0) 
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by itself. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntsa::DistinguishedName&                subjectIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&   subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified
    /// by the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
        const ntsa::DistinguishedName&                      subjectIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&         subjectPrivateKey,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& issuerCertificate,
        const bsl::shared_ptr<ntci::EncryptionKey>&         issuerPrivateKey,
        const ntca::EncryptionCertificateOptions&           options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Encode the specified 'certificate' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    ntsa::Error encodeCertificate(
        bsl::streambuf*                                     destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const ntca::EncryptionResourceOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' according to the specified 'options'. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        bsl::streambuf*                               source,
        const ntca::EncryptionResourceOptions&        options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>*         result,
        const ntca::EncryptionClientOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>*         result,
        const ntca::EncryptionServerOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
