// Copyright 2020-2024 Bloomberg Finance L.P.
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

#include <ntctls_plugin.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntctls_plugin_cpp, "$Id$ $CSID$")

#if NTC_BUILD_WITH_OPENSSL

#include <ntca_encryptionauthentication.h>
#include <ntca_encryptioncertificate.h>
#include <ntca_encryptioncertificateoptions.h>
#include <ntca_encryptionclientoptions.h>
#include <ntca_encryptionkey.h>
#include <ntca_encryptionkeyoptions.h>
#include <ntca_encryptionkeytype.h>
#include <ntca_encryptionmethod.h>
#include <ntca_encryptionoptions.h>
#include <ntca_encryptionresource.h>
#include <ntca_encryptionresourcedescriptor.h>
#include <ntca_encryptionresourceoptions.h>
#include <ntca_encryptionresourcetype.h>
#include <ntca_encryptionrole.h>
#include <ntca_encryptionsecret.h>
#include <ntca_encryptionserveroptions.h>
#include <ntca_encryptionvalidation.h>
#include <ntci_encryption.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptioncertificategenerator.h>
#include <ntci_encryptioncertificatestorage.h>
#include <ntci_encryptionclient.h>
#include <ntci_encryptionclientfactory.h>
#include <ntci_encryptiondriver.h>
#include <ntci_encryptionkey.h>
#include <ntci_encryptionkeygenerator.h>
#include <ntci_encryptionkeystorage.h>
#include <ntci_encryptionserver.h>
#include <ntci_encryptionserverfactory.h>
#include <ntci_log.h>
#include <ntci_mutex.h>
#include <ntcs_plugin.h>
#include <ntcs_blobutil.h>
#include <ntsf_system.h>

#include <bdlb_bigendian.h>
#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_string.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_epochutil.h>
#include <bdlt_iso8601util.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_once.h>
#include <bslmt_threadutil.h>
#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_spinlock.h>
#include <bsls_stopwatch.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iomanip.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <bsl_fstream.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/ec.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/opensslconf.h>
#include <openssl/opensslv.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/provider.h>
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#error OpenSSL 1.1.0 or greater is required
#endif

#if !defined(OPENSSL_THREADS)
#error OpenSSL with thread support is required
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma warning(disable : 4996)
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#define PKCS12_R_PKCS12_PBE_CRYPT_ERROR PKCS12_R_PKCS12_CIPHERFINAL_ERROR
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <wincrypt.h>
// clang-format on
#ifdef X509
#undef X509
#endif
#ifdef X509_NAME
#undef X509_NAME
#endif
#ifdef X509_EXTENSIONS
#undef X509_EXTENSIONS
#endif
#pragma comment(lib, "crypt32")
#endif

namespace BloombergLP {
namespace ntctls {

/// Describes limits in the TLS protocol.
///
/// @ingroup module_ntctls
class TlsLimit {
public:
    // The TLS v1.0 protocol version.
    static const bsl::uint16_t k_PROTOCOL_VERSION_TLS_v10 = 0x0301;

    // The TLS v1.1 protocol version.
    static const bsl::uint16_t k_PROTOCOL_VERSION_TLS_v11 = 0x0302;

    // The TLS v1.2 protocol version.
    static const bsl::uint16_t k_PROTOCOL_VERSION_TLS_v12 = 0x0303;

    // The TLS v1.3 protocol version.
    static const bsl::uint16_t k_PROTOCOL_VERSION_TLS_v13 = 0x0304;

    /// The record layer fragments information blocks into TLS plaintext
    /// records carrying data in chunks of 2^14 bytes or less.
    static const bsl::size_t k_RECORD_FRAGMENTATION_LIMIT = 16384;
};

/// Enumerates the TLS versions.
///
/// @ingroup module_ntctls
class TlsVersion
{
public:
    /// Enumerates the TLS versions.
    enum Value {
        // The TLS v1.0 protocol version.
        e_TLS_v10 = 0x0301,

        // The TLS v1.1 protocol version.
        e_TLS_v11 = 0x0302,

        // The TLS v1.2 protocol version.
        e_TLS_v12 = 0x0303,

        // The TLS v1.3 protocol version.
        e_TLS_v13 = 0x0304
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

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

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntctls::TlsVersion
bsl::ostream& operator<<(bsl::ostream& stream, TlsVersion::Value rhs);

/// Enumerates the TLS alert levels.
///
/// @details
/// When an error is detected, the detecting party sends a message to its peer.
/// Upon transmission or receipt of a fatal alert message, both parties MUST
/// immediately close the connection.
///
/// @ingroup module_ntctls
class TlsRecordAlertLevel
{
public:
    /// Enumerates the TLS alert levels.
    enum Value {
        /// The alert is a warning.
        e_WARN = 1, 

        /// The alert is fatal to continue processing the protocol.
        e_FATAL = 2
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

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

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntctls::TlsRecordAlertLevel
bsl::ostream& operator<<(bsl::ostream& stream, TlsRecordAlertLevel::Value rhs);

/// Enumerates the TLS alert codes (i.e. descriptions.)
///
/// @ingroup module_ntctls
class TlsRecordAlertCode
{
public:
    /// Enumerates the TLS alert codes.
    enum Value {
        /// This alert notifies the recipient that the sender will not send any
        /// more messages on this connection. Any data received after a closure
        /// alert has been received MUST be ignored.
        e_CLOSE_NOTIFY = 0,

        /// An inappropriate message (e.g., the wrong handshake message,
        /// premature Application Data, etc.) was received. This alert should
        /// never be observed in communication between proper implementations.
        e_UNEXPECTED_MESSAGE = 10,

        /// This alert is returned if a record is received which cannot be
        /// deprotected. Because AEAD algorithms combine decryption and
        /// verification, and also to avoid side-channel attacks, this alert is
        /// used for all deprotection failures. This alert should never be
        /// observed in communication between proper implementations, except
        /// when messages were corrupted in the network.
        e_BAD_RECORD_MAC = 20,

        /// A TLSCiphertext record was received that had a length more than
        /// 2^14 + 256 bytes, or a record decrypted to a TLSPlaintext record
        /// with more than 2^14 bytes (or some other negotiated limit).  This
        /// alert should never be observed in communication between proper
        /// implementations, except when messages were corrupted in the
        /// network.
        e_RECORD_OVERFLOW = 22,

        /// Receipt of a "handshake_failure" alert message indicates that the
        /// sender was unable to negotiate an acceptable set of security
        /// parameters given the options available.
        e_HANDSHAKE_FAILURE = 40,

        /// A certificate was corrupt, contained signatures that did not verify
        /// correctly, etc.
        e_BAD_CERTIFICATE = 42,

        /// A certificate was of an unsupported type.
        e_UNSUPPORTED_CERTIFICATE = 43,

        /// A certificate was revoked by its signer.
        e_CERTIFICATE_REVOKED = 44,

        /// A certificate has expired or is not currently valid.
        e_CERTIFICATE_EXPIRED = 45,

        /// Some other (unspecified) issue arose in processing the certificate,
        /// rendering it unacceptable.
        e_CERTIFICATE_UNKNOWN = 46,

        /// A field in the handshake was incorrect or inconsistent with other
        /// fields.  This alert is used for errors which conform to the formal
        /// protocol syntax but are otherwise incorrect.
        e_ILLEGAL_PARAMETER = 47,

        /// A valid certificate chain or partial chain was received, but the
        /// certificate was not accepted because the CA certificate could not
        /// be located or could not be matched with a known trust anchor.
        e_UNKNOWN_CA = 48,

        /// A valid certificate or PSK was received, but when access control
        /// was applied, the sender decided not to proceed with negotiation.
        e_ACCESS_DENIED = 49,

        /// A message could not be decoded because some field was out of the
        /// specified range or the length of the message was incorrect.  This
        /// alert is used for errors where the message does not conform to the
        /// formal protocol syntax.  This alert should never be observed in
        /// communication between proper implementations, except when messages
        /// were corrupted in the network.
        e_DECODE_ERROR = 50,

        /// A handshake (not record layer) cryptographic operation failed,
        /// including being unable to correctly verify a signature or validate
        /// a Finished message or a PSK binder.
        e_DECRYPT_ERROR = 51,

        /// The protocol version the peer has attempted to negotiate is
        /// recognized but not supported.
        e_PROTOCOL_VERSION = 70,

        /// Returned instead of "handshake_failure" when a negotiation has
        /// failed specifically because the server requires parameters more
        /// secure than those supported by the client.
        e_INSUFFICIENT_SECURITY = 71,

        /// An internal error unrelated to the peer or the correctness of the
        /// protocol (such as a memory allocation failure) makes it impossible
        /// to continue.
        e_INTERNAL_ERROR = 80,

        /// Sent by a server in response to an invalid connection retry attempt
        /// from a client. 
        e_INAPPROPRIATE_FALLBACK = 86,

        /// This alert notifies the recipient that the sender is canceling the
        /// handshake for some reason unrelated to a protocol failure. If a
        /// user cancels an operation after the handshake is complete, just
        /// closing the connection by sending a "close_notify" is more
        /// appropriate. This alert SHOULD be followed by a "close_notify".
        /// This alert generally has the warning alert level.
        e_USER_CANCELED = 90,

        /// Sent by endpoints that receive a handshake message not containing
        /// an extension that is mandatory to send for the offered TLS version
        /// or other negotiated parameters.
        e_MISSING_EXTENSION = 109,

        /// Sent by endpoints receiving any handshake message containing an
        /// extension known to be prohibited for inclusion in the given
        /// handshake message, or including any extensions in a ServerHello or
        /// Certificate not first offered in the corresponding ClientHello or
        /// CertificateRequest.
        e_UNSUPPORTED_EXTENSION = 110,

        /// Sent by servers when no server exists identified by the name
        /// provided by the client via the "server_name" extension.
        e_UNRECOGNIZED_NAME = 112,

        /// Sent by clients when an invalid or unacceptable OCSP response is 
        /// provided by the server via the "status_request" extension.
        e_BAD_CERTIFICATE_STATUS_RESPONSE = 113,

        /// Sent by servers when PSK key establishment is desired but no
        /// acceptable PSK identity is provided by the client. Sending this
        /// alert is OPTIONAL; servers MAY instead choose to send a
        /// "decrypt_error" alert to merely indicate an invalid PSK identity.
        e_UNKNOWN_PSK_IDENTITY = 115,

        /// Sent by servers when a client certificate is desired but none was 
        /// provided by the client.
        e_CERTIFICATE_REQUIRED = 116,

        /// Sent by servers when a client
        /// "application_layer_protocol_negotiation" extension advertises only
        /// protocols that the server does not support.
        e_NO_APPLICATION_PROTOCOL = 120,
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

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

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntctls::TlsRecordAlertCode
bsl::ostream& operator<<(bsl::ostream& stream, TlsRecordAlertCode::Value rhs);

/// Describes a TLS alert.
///
/// @ingroup module_ntctls
class TlsRecordAlert 
{
    /// The alert level.
    ntctls::TlsRecordAlertLevel::Value d_level;

    /// The alert code (i.e. description.)
    ntctls::TlsRecordAlertCode::Value d_code;

public:
    TlsRecordAlert() : 
    d_level(ntctls::TlsRecordAlertLevel::e_FATAL), 
    d_code(ntctls::TlsRecordAlertCode::e_INTERNAL_ERROR) 
    {
        NTCCFG_WARNING_UNUSED(d_level);
        NTCCFG_WARNING_UNUSED(d_code);
    }
};

/// Enumerates the TLS record content types.
///
/// @ingroup module_ntctls
class TlsRecordType
{
public:
    /// Enumerates the TLS record content types.
    enum Value {
        e_INVALID = 0,
        e_CHANGE_CIPHER_SPEC = 20,
        e_ALERT = 21,
        e_HANDSHAKE = 22,
        e_APPLICATION_DATA = 23
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

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

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntctls::TlsRecordType
bsl::ostream& operator<<(bsl::ostream& stream, TlsRecordType::Value rhs);

/// Describes a TLS record header.
///
/// @ingroup module_ntctls
class TlsRecordHeader
{
    /// The record type. The higher-level protocol used to process the
    /// enclosed fragment.
    ntctls::TlsRecordType::Value d_type;

    /// The legacy record version. MUST be set to 0x0303 for all records
    /// generated by a TLS 1.3 implementation other than an initial ClientHello
    /// (i.e., one not generated after a HelloRetryRequest), where it MAY also
    /// be 0x0301 for compatibility purposes. This field is deprecated and
    /// MUST be ignored for all purposes. Previous versions of TLS would use
    /// other values in this field under some circumstances.
    ntctls::TlsVersion::Value d_version;

    /// The length (in bytes) of the following TLSPlaintext.fragment.  The
    /// length MUST NOT exceed 'ntctls::TlsLimit::k_RECORD_FRAGMENTATION_LIMIT'
    /// bytes. An endpoint that receives a record that exceeds this length MUST
    /// terminate the connection with a "record_overflow" alert.
    bsl::size_t d_length;

    /// The data being transmitted.  This value is transparent and
    /// is treated as an independent block to be dealt with by the higher-
    /// level protocol specified by the type field.
    /// 
    /// opaque fragment[d_length];

private:
    /// Assign the value of this object from the specified 'type', 'version',
    /// and 'length' encoded representation. Return the error.
    ntsa::Error decodeRep(bsl::uint8_t          type,
                          bdlb::BigEndianUint16 version,
                          bdlb::BigEndianUint16 length);

    /// Load the value of this object into the specified 'type', 'version', and
    /// 'length' encoded representation. Return the error.
    ntsa::Error encodeRep(bsl::uint8_t*          type,
                          bdlb::BigEndianUint16* version,
                          bdlb::BigEndianUint16* length) const;

public:
    /// Enumerates the constants used by this implementation.
    enum Constants {
        /// The record header size, in bytes.
        k_SIZE = 5
    };

    /// Create a new TLS record header having the default value.
    TlsRecordHeader();

    /// Create a new TLS record header having the same value as the specified
    /// 'original' object.
    TlsRecordHeader(const TlsRecordHeader& original);

    /// Destroy this object.
    ~TlsRecordHeader();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    TlsRecordHeader& operator=(const TlsRecordHeader& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the type of the record to the specified 'value'.
    void setType(ntctls::TlsRecordType::Value value);

    /// Set the TLS protocol version to the specified 'value'.
    void setVersion(ntctls::TlsVersion::Value value);

    /// Set the number of bytes in the content, not including the length of the
    /// header, to the specified 'value'.
    void setLength(bsl::size_t value);

    /// Decode the object from the specified 'source' having the specified
    /// 'size'. Increment the specified 'numBytesDecoded' with the number of
    /// bytes read from the specified 'source'. Return the error.
    ntsa::Error decode(bsl::size_t* numBytesDecoded,
                       const void*  source,
                       bsl::size_t  size);

    /// Decode the object from the specified 'source'. Increment the specified
    /// 'numBytesDecoded' with the number of bytes read from the specified
    /// 'source'. Return the error.
    ntsa::Error decode(bsl::size_t* numBytesDecoded, bsl::streambuf* source);

    /// Decode the object from the specified 'source'. Increment the specified
    /// 'numBytesDecoded' with the number of bytes read from the specified
    /// 'source'. Return the error.
    ntsa::Error decode(bsl::size_t*       numBytesDecoded,
                       const bdlbb::Blob& source);

    /// Encode the object to the specified 'destination'. Increment the
    /// specified 'numBytesEncoded' with the number of bytes written to the
    /// 'destination'. Return the error.
    ntsa::Error encode(bsl::size_t* numBytesEncoded,
                       bdlbb::Blob* destination) const;

    /// Encode the object to the specified 'destination' at the specified
    /// 'position'. Increment the specified 'numBytesEncoded' with the number
    /// of bytes written to the 'destination'. Return the error.
    ntsa::Error encode(bsl::size_t* numBytesEncoded,
                       bdlbb::Blob* destination,
                       std::size_t  position) const;

    /// Return the type of the record.
    ntctls::TlsRecordType::Value type() const;

    /// Return the TLS protocol version.
    ntctls::TlsVersion::Value version() const;

    /// Return the number of bytes in the content, not including the length
    /// of the header.
    bsl::size_t length() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const TlsRecordHeader& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const TlsRecordHeader& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(TlsRecordHeader);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(TlsRecordHeader);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntctls::TlsRecordHeader
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const TlsRecordHeader& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntctls::TlsRecordHeader
bool operator==(const TlsRecordHeader& lhs,
                const TlsRecordHeader& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntctls::TlsRecordHeader
bool operator!=(const TlsRecordHeader& lhs,
                const TlsRecordHeader& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntctls::TlsRecordHeader
bool operator<(const TlsRecordHeader& lhs,
               const TlsRecordHeader& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntctls::TlsRecordHeader
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TlsRecordHeader& value);

const char* TlsVersion::toString(Value value)
{
    switch (value) {
    case TlsVersion::e_TLS_v10:
        return "TLS_v10";
    case TlsVersion::e_TLS_v11:
        return "TLS_v11";
    case TlsVersion::e_TLS_v12:
        return "TLS_v12";
    case TlsVersion::e_TLS_v13:
        return "TLS_v13";
    }

    return "???";
}

int TlsVersion::fromInt(Value* result, int number)
{
    switch (number) {
        case TlsVersion::e_TLS_v10:
        case TlsVersion::e_TLS_v11:
        case TlsVersion::e_TLS_v12:
        case TlsVersion::e_TLS_v13:
            *result = static_cast<TlsVersion::Value>(number);
            return 0;
        default:
            return -1;
        }
}

bsl::ostream& TlsVersion::print(bsl::ostream& stream, Value value)
{
    return stream << TlsVersion::toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TlsVersion::Value rhs)
{
    return TlsVersion::print(stream, rhs);
}

const char* TlsRecordAlertLevel::toString(Value value)
{
    switch (value) {
    case TlsRecordAlertLevel::e_WARN:
        return "WARN";
    case TlsRecordAlertLevel::e_FATAL:
        return "FATAL";
    }

    return "???";
}

int TlsRecordAlertLevel::fromInt(Value* result, int number)
{
    switch (number) {
        case TlsRecordAlertLevel::e_WARN:
        case TlsRecordAlertLevel::e_FATAL:
            *result = static_cast<TlsRecordAlertLevel::Value>(number);
            return 0;
        default:
            return -1;
        }
}

bsl::ostream& TlsRecordAlertLevel::print(bsl::ostream& stream, Value value)
{
    return stream << TlsRecordAlertLevel::toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TlsRecordAlertLevel::Value rhs)
{
    return TlsRecordAlertLevel::print(stream, rhs);
}

const char* TlsRecordAlertCode::toString(Value value)
{
    switch (value) {
    case TlsRecordAlertCode::e_CLOSE_NOTIFY:
        return "CLOSE_NOTIFY";
    case TlsRecordAlertCode::e_UNEXPECTED_MESSAGE:
        return "UNEXPECTED_MESSAGE";
    case TlsRecordAlertCode::e_BAD_RECORD_MAC:
        return "BAD_RECORD_MAC";
    case TlsRecordAlertCode::e_RECORD_OVERFLOW:
        return "RECORD_OVERFLOW";
    case TlsRecordAlertCode::e_HANDSHAKE_FAILURE:
        return "HANDSHAKE_FAILURE";
    case TlsRecordAlertCode::e_BAD_CERTIFICATE:
        return "BAD_CERTIFICATE";
    case TlsRecordAlertCode::e_UNSUPPORTED_CERTIFICATE:
        return "UNSUPPORTED_CERTIFICATE";
    case TlsRecordAlertCode::e_CERTIFICATE_REVOKED:
        return "CERTIFICATE_REVOKED";
    case TlsRecordAlertCode::e_CERTIFICATE_EXPIRED:
        return "CERTIFICATE_EXPIRED";
    case TlsRecordAlertCode::e_CERTIFICATE_UNKNOWN:
        return "CERTIFICATE_UNKNOWN";
    case TlsRecordAlertCode::e_ILLEGAL_PARAMETER:
        return "ILLEGAL_PARAMETER";
    case TlsRecordAlertCode::e_UNKNOWN_CA:
        return "UNKNOWN_CA";
    case TlsRecordAlertCode::e_ACCESS_DENIED:
        return "ACCESS_DENIED";
    case TlsRecordAlertCode::e_DECODE_ERROR:
        return "DECODE_ERROR";
    case TlsRecordAlertCode::e_DECRYPT_ERROR:
        return "DECRYPT_ERROR";
    case TlsRecordAlertCode::e_PROTOCOL_VERSION:
        return "PROTOCOL_VERSION";
    case TlsRecordAlertCode::e_INSUFFICIENT_SECURITY:
        return "INSUFFICIENT_SECURITY";
    case TlsRecordAlertCode::e_INTERNAL_ERROR:
        return "INTERNAL_ERROR";
    case TlsRecordAlertCode::e_INAPPROPRIATE_FALLBACK:
        return "INAPPROPRIATE_FALLBACK";
    case TlsRecordAlertCode::e_USER_CANCELED:
        return "USER_CANCELED";
    case TlsRecordAlertCode::e_MISSING_EXTENSION:
        return "MISSING_EXTENSION";
    case TlsRecordAlertCode::e_UNSUPPORTED_EXTENSION:
        return "UNSUPPORTED_EXTENSION";
    case TlsRecordAlertCode::e_UNRECOGNIZED_NAME:
        return "UNRECOGNIZED_NAME";
    case TlsRecordAlertCode::e_BAD_CERTIFICATE_STATUS_RESPONSE:
        return "BAD_CERTIFICATE_STATUS_RESPONSE";
    case TlsRecordAlertCode::e_UNKNOWN_PSK_IDENTITY:
        return "UNKNOWN_PSK_IDENTITY";
    case TlsRecordAlertCode::e_CERTIFICATE_REQUIRED:
        return "CERTIFICATE_REQUIRED";
    case TlsRecordAlertCode::e_NO_APPLICATION_PROTOCOL:
        return "NO_APPLICATION_PROTOCOL";
    }

    return "???";
}

int TlsRecordAlertCode::fromInt(Value* result, int number)
{
    switch (number) {
    case TlsRecordAlertCode::e_CLOSE_NOTIFY:
    case TlsRecordAlertCode::e_UNEXPECTED_MESSAGE:
    case TlsRecordAlertCode::e_BAD_RECORD_MAC:
    case TlsRecordAlertCode::e_RECORD_OVERFLOW:
    case TlsRecordAlertCode::e_HANDSHAKE_FAILURE:
    case TlsRecordAlertCode::e_BAD_CERTIFICATE:
    case TlsRecordAlertCode::e_UNSUPPORTED_CERTIFICATE:
    case TlsRecordAlertCode::e_CERTIFICATE_REVOKED:
    case TlsRecordAlertCode::e_CERTIFICATE_EXPIRED:
    case TlsRecordAlertCode::e_CERTIFICATE_UNKNOWN:
    case TlsRecordAlertCode::e_ILLEGAL_PARAMETER:
    case TlsRecordAlertCode::e_UNKNOWN_CA:
    case TlsRecordAlertCode::e_ACCESS_DENIED:
    case TlsRecordAlertCode::e_DECODE_ERROR:
    case TlsRecordAlertCode::e_DECRYPT_ERROR:
    case TlsRecordAlertCode::e_PROTOCOL_VERSION:
    case TlsRecordAlertCode::e_INSUFFICIENT_SECURITY:
    case TlsRecordAlertCode::e_INTERNAL_ERROR:
    case TlsRecordAlertCode::e_INAPPROPRIATE_FALLBACK:
    case TlsRecordAlertCode::e_USER_CANCELED:
    case TlsRecordAlertCode::e_MISSING_EXTENSION:
    case TlsRecordAlertCode::e_UNSUPPORTED_EXTENSION:
    case TlsRecordAlertCode::e_UNRECOGNIZED_NAME:
    case TlsRecordAlertCode::e_BAD_CERTIFICATE_STATUS_RESPONSE:
    case TlsRecordAlertCode::e_UNKNOWN_PSK_IDENTITY:
    case TlsRecordAlertCode::e_CERTIFICATE_REQUIRED:
    case TlsRecordAlertCode::e_NO_APPLICATION_PROTOCOL:
        *result = static_cast<TlsRecordAlertCode::Value>(number);
        return 0;
    default:
        return -1;
    }
}

bsl::ostream& TlsRecordAlertCode::print(bsl::ostream& stream, Value value)
{
    return stream << TlsRecordAlertCode::toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TlsRecordAlertCode::Value rhs)
{
    return TlsRecordAlertCode::print(stream, rhs);
}

const char* TlsRecordType::toString(Value value)
{
    switch (value) {
    case TlsRecordType::e_INVALID:
        return "INVALID";
    case TlsRecordType::e_CHANGE_CIPHER_SPEC:
        return "CHANGE_CIPHER_SPEC";
    case TlsRecordType::e_ALERT:
        return "ALERT";
    case TlsRecordType::e_HANDSHAKE:
        return "HANDSHAKE";
    case TlsRecordType::e_APPLICATION_DATA:
        return "APPLICATION_DATA";
    }

    return "???";
}

int TlsRecordType::fromInt(Value* result, int number)
{
    switch (number) {
        case TlsRecordType::e_INVALID:
        case TlsRecordType::e_CHANGE_CIPHER_SPEC:
        case TlsRecordType::e_ALERT:
        case TlsRecordType::e_HANDSHAKE:
        case TlsRecordType::e_APPLICATION_DATA:
            *result = static_cast<TlsRecordType::Value>(number);
            return 0;
        default:
            return -1;
        }
}

bsl::ostream& TlsRecordType::print(bsl::ostream& stream, Value value)
{
    return stream << TlsRecordType::toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, TlsRecordType::Value rhs)
{
    return TlsRecordType::print(stream, rhs);
}

NTSCFG_INLINE
TlsRecordHeader::TlsRecordHeader()
: d_type(ntctls::TlsRecordType::e_ALERT)
, d_version(ntctls::TlsVersion::e_TLS_v10)
, d_length(0)
{
}

NTSCFG_INLINE
TlsRecordHeader::TlsRecordHeader(
    const TlsRecordHeader& original)
: d_type(original.d_type)
, d_version(original.d_version)
, d_length(original.d_length)
{
}

NTSCFG_INLINE
TlsRecordHeader::~TlsRecordHeader()
{
}

NTSCFG_INLINE
TlsRecordHeader& TlsRecordHeader::operator=(const TlsRecordHeader& other)
{
    if (this != &other) {
        d_type    = other.d_type;
        d_version = other.d_version;
        d_length  = other.d_length;
    }

    return *this;
}

NTSCFG_INLINE
void TlsRecordHeader::reset()
{
    d_type    = ntctls::TlsRecordType::e_ALERT;
    d_version = ntctls::TlsVersion::e_TLS_v10;
    d_length  = 0;
}

NTSCFG_INLINE
void TlsRecordHeader::setType(ntctls::TlsRecordType::Value value)
{
    d_type = value;
}

NTSCFG_INLINE
void TlsRecordHeader::setVersion(ntctls::TlsVersion::Value value)
{
    d_version = value;
}

NTSCFG_INLINE
void TlsRecordHeader::setLength(bsl::size_t value)
{
    BSLS_ASSERT(value <= ntctls::TlsLimit::k_RECORD_FRAGMENTATION_LIMIT);
    d_length = value;
}

NTSCFG_INLINE
ntctls::TlsRecordType::Value TlsRecordHeader::type() const
{
    return d_type;
}

NTSCFG_INLINE
ntctls::TlsVersion::Value TlsRecordHeader::version() const
{
    return d_version;
}

NTSCFG_INLINE
bsl::size_t TlsRecordHeader::length() const
{
    return d_length;
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void TlsRecordHeader::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_type);
    hashAppend(algorithm, d_version);
    hashAppend(algorithm, d_length);
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const TlsRecordHeader& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const TlsRecordHeader& lhs,
                const TlsRecordHeader& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const TlsRecordHeader& lhs,
                const TlsRecordHeader& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const TlsRecordHeader& lhs,
               const TlsRecordHeader& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void hashAppend(HASH_ALGORITHM&        algorithm,
                              const TlsRecordHeader& value)
{
    value.hash(algorithm);
}

ntsa::Error TlsRecordHeader::decodeRep(bsl::uint8_t          type,
                                       bdlb::BigEndianUint16 version,
                                       bdlb::BigEndianUint16 length)
{
    NTCI_LOG_CONTEXT();

    if (0 != ntctls::TlsRecordType::fromInt(
        &d_type,
        static_cast<int>(type)))
    {
        NTCI_LOG_TRACE("Invalid TLS record type: %d", (int)(d_type));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (0 != ntctls::TlsVersion::fromInt(
        &d_version,
        static_cast<int>(static_cast<bsl::uint16_t>(version))))
    {
        NTCI_LOG_TRACE("Invalid TLS record protocol version: %d", 
                       (int)(d_version));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (static_cast<bsl::uint16_t>(length) > 
        ntctls::TlsLimit::k_RECORD_FRAGMENTATION_LIMIT)
    {
        NTCI_LOG_TRACE("Invalid TLS record content length: %d", 
                       (int)(length));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_length = static_cast<bsl::uint16_t>(length);

    return ntsa::Error();
}


ntsa::Error TlsRecordHeader::encodeRep(bsl::uint8_t*          type,
                                       bdlb::BigEndianUint16* version,
                                       bdlb::BigEndianUint16* length) const
{
    NTCI_LOG_CONTEXT();

    *type = static_cast<bsl::uint8_t>(d_type);
    *version = static_cast<bsl::uint16_t>(d_version);

    if (d_length > ntctls::TlsLimit::k_RECORD_FRAGMENTATION_LIMIT) {
        NTCI_LOG_TRACE("Invalid TLS record content length: %d", 
                       (int)(d_length));
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *length = static_cast<bsl::uint16_t>(d_length);

    return ntsa::Error();
}

ntsa::Error TlsRecordHeader::decode(bsl::size_t* numBytesDecoded,
                                    const void*  source,
                                    bsl::size_t  size)
{
    ntsa::Error error;

    this->reset();

    if (size < TlsRecordHeader::k_SIZE) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    const bsl::uint8_t* reader = reinterpret_cast<const bsl::uint8_t*>(source);

    bsl::uint8_t type = *reader;
    ++reader;

    bdlb::BigEndianUint16 version;
    bsl::memcpy(&version, reader, sizeof version);
    reader += sizeof version;

    bdlb::BigEndianUint16 length;
    bsl::memcpy(&length, reader, sizeof length);
    reader += sizeof length;

    error = this->decodeRep(type, version, length);
    if (error) {
        return error;
    }

    *numBytesDecoded += TlsRecordHeader::k_SIZE;

    return ntsa::Error();
}


ntsa::Error TlsRecordHeader::decode(bsl::size_t*    numBytesDecoded,
                                    bsl::streambuf* source)
{
    ntsa::Error error;

    this->reset();

    bsl::streampos p0 = 
        source->pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);

    bsl::uint8_t type;
    {
        bsl::streambuf::int_type meta = source->sbumpc();
        if (meta == EOF) {
            source->pubseekpos(p0, bsl::ios_base::in);
            this->reset();
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }

        type = static_cast<bsl::uint8_t>(meta);
    }

    bdlb::BigEndianUint16 version;
    {
        bsl::streamsize n =
            source->sgetn(reinterpret_cast<char*>(&version), sizeof version);

        if (static_cast<bsl::size_t>(n) != sizeof version) {
            source->pubseekpos(p0, bsl::ios_base::in);
            this->reset();
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
    }

    bdlb::BigEndianUint16 length;
    {
        bsl::streamsize n =
            source->sgetn(reinterpret_cast<char*>(&length), sizeof length);

        if (static_cast<bsl::size_t>(n) != sizeof length) {
            source->pubseekpos(p0, bsl::ios_base::in);
            this->reset();
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
    }

    bsl::streampos p1 = 
        source->pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);

    bsl::size_t pd = static_cast<bsl::size_t>(p1 - p0);
    if (pd != ntctls::TlsRecordHeader::k_SIZE) {
        source->pubseekpos(p0, bsl::ios_base::in);
        this->reset();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = this->decodeRep(type, version, length);
    if (error) {
        return error;
    }

    *numBytesDecoded += ntctls::TlsRecordHeader::k_SIZE;

    return ntsa::Error();
}

ntsa::Error TlsRecordHeader::decode(bsl::size_t*       numBytesDecoded,
                                    const bdlbb::Blob& source)
{
    this->reset();

    bsl::size_t numBytesAvailable = static_cast<bsl::size_t>(source.length());

    if (numBytesAvailable >= ntctls::TlsRecordHeader::k_SIZE) {
        const bdlbb::BlobBuffer& buffer = source.buffer(0);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<const bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (source.numDataBuffers() == 1) {
            bufferSize =
                static_cast<bsl::size_t>(source.lastDataBufferLength());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }

        if (bufferSize >= ntctls::TlsRecordHeader::k_SIZE) {
            return this->decode(numBytesDecoded, bufferData, bufferSize);
        }
        else {
            bdlbb::InBlobStreamBuf isb(&source);
            return this->decode(numBytesDecoded, &isb);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
}

ntsa::Error TlsRecordHeader::encode(bsl::size_t* numBytesEncoded,
                                    bdlbb::Blob* destination) const
{
    ntsa::Error error;

    bsl::uint8_t          type;
    bdlb::BigEndianUint16 version;
    bdlb::BigEndianUint16 length;

    error = this->encodeRep(&type, &version, &length);
    if (error) {
        return error;
    }

    bsl::size_t p0 = static_cast<bsl::size_t>(destination->length());

    ntcs::BlobUtil::append(destination, &type, sizeof type);
    ntcs::BlobUtil::append(destination, &version, sizeof version);
    ntcs::BlobUtil::append(destination, &length, sizeof length);

    bsl::size_t p1 = static_cast<bsl::size_t>(destination->length());

    bsl::size_t pd = static_cast<bsl::size_t>(p1 - p0);
    if (pd != ntctls::TlsRecordHeader::k_SIZE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *numBytesEncoded += ntctls::TlsRecordHeader::k_SIZE;

    return ntsa::Error();
}

ntsa::Error TlsRecordHeader::encode(bsl::size_t* numBytesEncoded,
                                           bdlbb::Blob* destination,
                                           std::size_t  position) const
{
    ntsa::Error error;

    bsl::uint8_t          type;
    bdlb::BigEndianUint16 version;
    bdlb::BigEndianUint16 length;

    error = this->encodeRep(&type, &version, &length);
    if (error) {
        return error;
    }

    bdlbb::OutBlobStreamBuf osb(destination);
    std::streampos p = osb.pubseekpos(static_cast<bsl::streampos>(position),
                                      bsl::ios_base::out);
    if (static_cast<bsl::size_t>(p) != position) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::size_t p0 = static_cast<bsl::size_t>(destination->length());

    {
        bsl::streambuf::int_type meta = osb.sputc(static_cast<char>(type));
        if (meta == EOF) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    {
        bsl::streamsize n = osb.sputn(
            reinterpret_cast<const char*>(&version),
            static_cast<bsl::streamsize>(sizeof version));

        if (static_cast<bsl::size_t>(n) != sizeof version) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    {
        bsl::streamsize n = osb.sputn(
            reinterpret_cast<const char*>(&length),
            static_cast<bsl::streamsize>(sizeof length));

        if (static_cast<bsl::size_t>(n) != sizeof length) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bsl::size_t p1 = static_cast<bsl::size_t>(destination->length());

    bsl::size_t pd = static_cast<bsl::size_t>(p1 - p0);
    if (pd != ntctls::TlsRecordHeader::k_SIZE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *numBytesEncoded += ntctls::TlsRecordHeader::k_SIZE;

    return ntsa::Error();
}

bool TlsRecordHeader::equals(const TlsRecordHeader& other) const
{
    return d_type == other.d_type && 
           d_version == other.d_version &&
           d_length == other.d_length;
}

bool TlsRecordHeader::less(const TlsRecordHeader& other) const
{
    if (d_type < other.d_type) {
        return true;
    }

    if (other.d_type < d_type) {
        return false;
    }

    if (d_version < other.d_version) {
        return true;
    }

    if (other.d_version < d_version) {
        return false;
    }

    return d_length < other.d_length;
}

bsl::ostream& TlsRecordHeader::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("type",    d_type);
    printer.printAttribute("version", d_version);
    printer.printAttribute("length",  d_length);
    printer.end();
    return stream;
}

} // close namespace ntctls
} // close namespace BloombergLP

namespace BloombergLP {
namespace ntctls {

class Environment;
class Key;
class Certificate;
class Resource;
class Session;
class SessionContext;
class SessionManager;
class Driver;

/// Provide a handle to an object deleted with a function with a canonical
/// signature.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctls
template <typename TYPE>
class Handle
{
  public:
    /// Declare a type alias for the underlying type.
    typedef TYPE Type;

    /// Declare a type alias for the deleter to the type.
    typedef void (*Deleter)(Type*);

  private:
    Type*   d_ptr_p;
    Deleter d_deleter;

  private:
    Handle(const Handle&) BSLS_KEYWORD_DELETED;
    Handle& operator=(const Handle&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new handle to a null object.
    Handle();

    /// Create a new handle to the specified 'object' that frees the object
    /// by calling the standard deleter function for 'object'.
    explicit Handle(Type* object);

    /// Create a new handle to the specified 'object' that frees the object
    /// by calling the specified 'deleter' function.
    Handle(Type* object, Deleter deleter);

    /// Destroy this object.
    ~Handle();

    /// Delete the managed object, if any, and reset the managed object to
    /// null.
    void reset();

    /// Delete the managed object, if any, and reset the managed object to the
    /// specified 'object' and free the object with the standard deleter
    /// function for the 'object'.
    void reset(Type* object);

    /// Delete the managed object, if any, and reset the managed object to the
    /// specified 'object' and free the object with the specified 'deleter'.
    void reset(Type* object, Deleter deleter);

    /// Release and return the managed object. The managed object will not
    /// be deleted by this handle when this handle is destroyed.
    Type* release();

    /// Return a pointer to the modifiable object.
    Type* get() const;

    /// Return true if the managed object is not null, otherwise return false.
    operator bool() const;
};

/// Provide utilities for handles.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctls
class HandleUtil
{
  private:
    /// Delete the specified 'object'.
    static void deleteBio(BIO* object);

  public:
    /// Provide a type alias to a type-erased deleter.
    typedef void (*Deleter)(void*);

    /// The behavior is undefined if this function is called.
    template <typename TYPE>
    static Deleter deleter(TYPE*);

    /// Return the deleter for an object of type 'BIO'.
    static Deleter deleter(BIO*);

    /// Return the deleter for an object of type 'BIGNUM'.
    static Deleter deleter(BIGNUM*);

    /// Return the deleter for an object of type 'DH'.
    static Deleter deleter(DH*);

    /// Return the deleter for an object of type 'DSA'.
    static Deleter deleter(DSA*);

    /// Return the deleter for an object of type 'RSA'.
    static Deleter deleter(RSA*);

    /// Return the deleter for an object of type 'EVP_PKEY'.
    static Deleter deleter(EVP_PKEY*);

    /// Return the deleter for an object of type 'EVP_PKEY_CTX'.
    static Deleter deleter(EVP_PKEY_CTX*);

    /// Return the deleter for an object of type 'X509'.
    static Deleter deleter(X509*);

    /// Return the deleter for an object of type 'X509_NAME'.
    static Deleter deleter(X509_NAME*);

    /// Return the deleter for an object of type 'X509_EXTENSION'.
    static Deleter deleter(X509_EXTENSION*);

    /// Return the deleter for an object of type 'X509_ALGOR'.
    static Deleter deleter(X509_ALGOR*);

    /// Return the deleter for an object of type 'X509_SIG'.
    static Deleter deleter(X509_SIG*);

    /// Return the deleter for an object of type 'X509_STORE'.
    static Deleter deleter(X509_STORE*);

    /// Return the deleter for an object of type 'X509_STORE_CTX'.
    static Deleter deleter(X509_STORE_CTX*);

    /// Return the deleter for an object of type 'SSL'.
    static Deleter deleter(SSL*);

    /// Return the deleter for an object of type 'SSL_CTX'.
    static Deleter deleter(SSL_CTX*);

    /// Return the deleter for an object of type 'PKCS12'.
    static Deleter deleter(PKCS12*);

    /// Return the deleter for an object of type 'PKCS7'.
    static Deleter deleter(PKCS7*);

    /// Return the deleter for an object of type 'PKCS8_PRIV_KEY_INFO'.
    static Deleter deleter(PKCS8_PRIV_KEY_INFO*);
};

/// Define a type alias for the integer type that represents an error number.
typedef unsigned long ErrorNumber;

/// Provide error information.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctls
class ErrorInfo
{
    ntctls::ErrorNumber d_number;
    bsl::string         d_description;

  public:
    /// Create new error information with a default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit ErrorInfo(bslma::Allocator* basicAllocator = 0);

    /// Create new error information having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ErrorInfo(const ErrorInfo& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ErrorInfo();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    ErrorInfo& operator=(const ErrorInfo& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the error number to the specified 'value'.
    void setNumber(ntctls::ErrorNumber value);

    /// Set the error description to the specified 'value'.
    void setDescription(const bsl::string& value);

    /// Return the error number.
    ntctls::ErrorNumber number() const;

    // Return the code that identifies the library in which the error occurred.
    int library() const;

    // Return the code that identifies the function in which the error occurred.
    int function() const;

    // Return the code that identifies the reason in which the error occurred.
    int reason() const;

    /// Return true if the error occurred in the specified in the 'library' for
    /// the specified 'reason', otherwise return false.
    bool match(int library, int reason) const;

    /// Return the error description.
    const bsl::string& description() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ErrorInfo& other) const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ErrorInfo);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntctls::ErrorInfo
bsl::ostream& operator<<(bsl::ostream& stream, const ErrorInfo& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntctls::ErrorInfo
bool operator==(const ErrorInfo& lhs, const ErrorInfo& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntctls::ErrorInfo
bool operator!=(const ErrorInfo& lhs, const ErrorInfo& rhs);

/// Provide a stack of errors.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctls
class ErrorStack
{
    typedef bsl::vector<ntctls::ErrorInfo> Container;

    Container         d_container;
    ntctls::ErrorInfo d_sentinel;

  public:
    /// Create a new, initially empty error stack. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit ErrorStack(bslma::Allocator* basicAllocator = 0);

    /// Create a new error stack having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ErrorStack(const ErrorStack& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ErrorStack();

    // Assign the value of the specified 'other' object to this object. Return
    // a reference to this modifiable object.
    ErrorStack& operator=(const ErrorStack& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    /// Push the specified 'errorInfo' onto the stack.
    void push(const ntctls::ErrorInfo& errorInfo);

    /// Push the specified 'errorStack' onto the stack.
    void push(const ntctls::ErrorStack& errorStack);

    /// Return the last error information.
    const ntctls::ErrorInfo& last() const;

    /// Return true if any error occurred in the specified in the specified
    /// 'library' for the specified 'reason', otherwise return false.
    bool find(int library, int reason) const;

    /// Return the string description of the error stack.
    bsl::string text() const;

    /// Return true if there are no errors on the stack, otherwise return
    /// false.
    bool empty() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ErrorStack& other) const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ErrorStack);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntctls::ErrorStack
bsl::ostream& operator<<(bsl::ostream& stream, const ErrorStack& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntctls::ErrorStack
bool operator==(const ErrorStack& lhs, const ErrorStack& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntctls::ErrorStack
bool operator!=(const ErrorStack& lhs, const ErrorStack& rhs);

/// Provide internal utilities.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctls
class Internal
{
  public:
    /// Initialize the internal state.
    static void initialize();

    /// Return the handle to a new stream that operates on the specified
    /// 'buffer'. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<BIO> createStream(
        bsl::streambuf*   buffer,
        bslma::Allocator* basicAllocator = 0);

    /// Return the handle to a new stream that operates on the specified
    /// 'buffer'. The handle must be explicitly destroyed by the caller.
    static BIO* createStreamRaw(bsl::streambuf* buffer);

    /// Return the handle to a new stream that operates on the specified
    /// 'blob'. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static bsl::shared_ptr<BIO> createStream(
        bdlbb::Blob*      blob,
        bslma::Allocator* basicAllocator = 0);

    /// Return the handle to a new stream that operates on the specified
    /// 'blob'. The handle must be explicitly destroyed by the caller.
    static BIO* createStreamRaw(bdlbb::Blob* blob);

    /// Destroy the stream identified by the specified 'bio'.
    static void destroyStream(BIO* bio);

    /// Drain the OpenSSL error queue, pushing a description of each error to
    /// the specified 'errorQueue'. Note that the errors appear in the queue
    /// in the chronological order in which they occurred.
    static void drainErrorQueue(bsl::vector<bsl::string>* errorQueue);

    /// Drain the OpenSSL error queue, appending a formatted, human-readable
    /// description of each error to the specified 'description'. Note that the
    /// errors appear in the queue in the chronological order in which they
    /// occurred.
    static void drainErrorQueue(bsl::string* description);

    /// Drain the OpenSSL error queue, appending a formatted, human-readable
    /// description of each error to the specified 'description'. Note that the
    /// errors appear in the queue in the chronological order in which they
    /// occurred.
    static void drainErrorQueue(bsl::streambuf* description);

    /// Drain the OpenSSL error queue, appending a formatted, human-readable
    /// description of each error to the specified 'errorStack'. Note that the
    /// errors appear in the stack in the chronological order in which they
    /// occurred: the bottom is the oldest, the top is the newest.
    static void drainErrorQueue(ntctls::ErrorStack* errorStack);

    /// Load into the specified 'result' the specified 'dateTime' in the
    /// "YYYYMMDDHHMMSSZ" format.
    static void convertDatetimeToAsn1TimeString(
        bsl::string*          result,
        const bdlt::Datetime& dateTime);

    /// Clean up the internal state.
    static void exit();

  public:
    /// Provide implementation details.
    class Impl;
};

/// Provide implementation details.
class Internal::Impl
{
  public:
    /// Provide a guard to save and restore a the read position in a stream
    /// buffer.
    class StreamBufferPositionGuard;

    /// The "virtual table" of functions for blob-based BIOs.
    static BIO_METHOD* s_blobMethods;

    /// The "virtual table" of functions for streambuf-based BIOs.
    static BIO_METHOD* s_streambufMethods;

    /// The lock for the user data index.
    static bsls::SpinLock s_userDataIndexLock;

    /// The user data index for this implementation.
    static int s_userDataIndex;

    /// The flag that indicates whether the user data index for this
    /// implementation has been reserved.
    static bool s_userDataIndexDefined;

    /// The list of supported ciphers when using TLSv1.0 through TLSv1.2. This
    /// list is the default recommendations from OpenSSL 1.1.x.
    static const char k_DEFAULT_CIPHER_SPEC[174];

    /// The list of supported cipher suites when using TLSv1.3 and later. This
    /// list is the default recommendations from OpenSSL 1.1.x.
    static const char k_DEFAULT_CIPHER_SUITES[75];

    static int  bio_blob_new(BIO* bio);
    static int  bio_blob_free(BIO* bio);
    static int  bio_blob_write(BIO* bio, const char* data, int size);
    static int  bio_blob_read(BIO* bio, char* data, int size);
    static int  bio_blob_puts(BIO* bio, const char* data);
    static int  bio_blob_gets(BIO* bio, char* data, int size);
    static long bio_blob_ctrl(BIO* bio, int cmd, long num, void* ptr);

    static int  bio_streambuf_new(BIO* bio);
    static int  bio_streambuf_free(BIO* bio);
    static int  bio_streambuf_write(BIO* bio, const char* data, int size);
    static int  bio_streambuf_read(BIO* bio, char* data, int size);
    static int  bio_streambuf_puts(BIO* bio, const char* data);
    static int  bio_streambuf_gets(BIO* bio, char* data, int size);
    static long bio_streambuf_ctrl(BIO* bio, int cmd, long num, void* ptr);

    static BIO* BIO_new_blob(bdlbb::Blob* blob);
    static BIO* BIO_new_streambuf(bsl::streambuf* buffer);
};

BIO_METHOD*    Internal::Impl::s_blobMethods;
BIO_METHOD*    Internal::Impl::s_streambufMethods;
bsls::SpinLock Internal::Impl::s_userDataIndexLock;
int            Internal::Impl::s_userDataIndex;
bool           Internal::Impl::s_userDataIndexDefined;

// clang-format off

// The list of supported ciphers when using TLSv1.0 through TLSv1.2. This list
// is the default recommendations from OpenSSL 1.1.x.
const char Internal::Impl::k_DEFAULT_CIPHER_SPEC[174] =
    "ECDHE-ECDSA-AES128-GCM-SHA256:"
    "ECDHE-RSA-AES128-GCM-SHA256:"
    "ECDHE-ECDSA-AES256-GCM-SHA384:"
    "ECDHE-RSA-AES256-GCM-SHA384:"
    "ECDHE-ECDSA-CHACHA20-POLY1305:"
    "ECDHE-RSA-CHACHA20-POLY1305";

// The list of supported cipher suites when using TLSv1.3 and later. This list
// is the default recommendations from OpenSSL 1.1.x.
const char Internal::Impl::k_DEFAULT_CIPHER_SUITES[75] =
    "TLS_AES_256_GCM_SHA384:"
    "TLS_CHACHA20_POLY1305_SHA256:"
    "TLS_AES_128_GCM_SHA256";

// clang-format on

/// Provide a guard to save and restore a the read position in a stream buffer.
class Internal::Impl::StreamBufferPositionGuard
{
    bsl::streambuf* d_buffer_p;
    bsl::streampos  d_start;

    StreamBufferPositionGuard(const StreamBufferPositionGuard&)
        BSLS_KEYWORD_DELETED;
    StreamBufferPositionGuard& operator=(const StreamBufferPositionGuard&)
        BSLS_KEYWORD_DELETED;

  public:
    explicit StreamBufferPositionGuard(bsl::streambuf* buffer);
    ~StreamBufferPositionGuard();

    void release();
};

Internal::Impl::StreamBufferPositionGuard::StreamBufferPositionGuard(
    bsl::streambuf* buffer)
: d_buffer_p(buffer)
, d_start(d_buffer_p->pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in))
{
}

Internal::Impl::StreamBufferPositionGuard::~StreamBufferPositionGuard()
{
    if (d_buffer_p) {
        if (d_start >= 0) {
            bsl::streampos current = d_buffer_p->pubseekoff(0,
                                                            bsl::ios_base::cur,
                                                            bsl::ios_base::in);

            if (current > d_start) {
                bsl::streamoff distance =
                    static_cast<bsl::streamoff>(current - d_start);
                d_buffer_p->pubseekoff(-distance,
                                       bsl::ios_base::cur,
                                       bsl::ios_base::in);
            }
        }
    }
}

void Internal::Impl::StreamBufferPositionGuard::release()
{
    d_buffer_p = 0;
    d_start    = -1;
}

int Internal::Impl::bio_blob_new(BIO* bio)
{
    BIO_set_init(bio, 1);
    return 1;
}

int Internal::Impl::bio_blob_free(BIO* bio)
{
    if (bio == 0) {
        return 0;
    }

    BIO_set_init(bio, 0);
    BIO_set_data(bio, 0);

    return 1;
}

int Internal::Impl::bio_blob_write(BIO* bio, const char* data, int size)
{
    bdlbb::Blob* blob = reinterpret_cast<bdlbb::Blob*>(BIO_get_data(bio));

    BIO_clear_retry_flags(bio);

    if (size < 0) {
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    bdlbb::BlobUtil::append(blob, data, size);

    return size;
}

int Internal::Impl::bio_blob_read(BIO* bio, char* data, int size)
{
    bdlbb::Blob* blob = reinterpret_cast<bdlbb::Blob*>(BIO_get_data(bio));

    BIO_clear_retry_flags(bio);

    if (size < 0) {
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    const int available = blob->length();

    if (available == 0) {
        BIO_set_retry_read(bio);
        return -1;
    }

    int n = size;
    if (n > available) {
        n = available;
    }

    bdlbb::BlobUtil::copy(data, *blob, 0, n);
    bdlbb::BlobUtil::erase(blob, 0, n);

    BSLS_ASSERT(n > 0);
    return n;
}

int Internal::Impl::bio_blob_puts(BIO* bio, const char* data)
{
    bdlbb::Blob* blob = reinterpret_cast<bdlbb::Blob*>(BIO_get_data(bio));
    const int    size = static_cast<int>(bsl::strlen(data));

    bdlbb::BlobUtil::append(blob, data, 0, size);

    return size;
}

int Internal::Impl::bio_blob_gets(BIO* bio, char* data, int size)
{
    bdlbb::Blob* blob = reinterpret_cast<bdlbb::Blob*>(BIO_get_data(bio));

    if (size == 0) {
        return 0;
    }

    if (size == 1) {
        *data = 0;
        return 0;
    }

    if (blob->length() == 0) {
        *data = 0;
        return 0;
    }

    char* current = data;
    char* end     = data + size - 1;

    int bufferIndex  = 0;
    int bufferOffset = 0;

    const bdlbb::BlobBuffer* buffer = &blob->buffer(bufferIndex);

    while (true) {
        if (current == end) {
            break;
        }

        if (bufferOffset == buffer->size()) {
            bufferOffset = 0;
            ++bufferIndex;
            if (bufferIndex == blob->numDataBuffers()) {
                break;
            }
            buffer = &blob->buffer(bufferIndex);
        }

        const char ch = buffer->data()[bufferOffset];
        ++bufferOffset;

        *current = ch;
        ++current;

        if (ch == '\n') {
            break;
        }
    }

    BSLS_ASSERT(current < data + size);
    *current = 0;

    int n = static_cast<int>(current - data);
    BSLS_ASSERT(n >= 0);

    bdlbb::BlobUtil::erase(blob, 0, n);

    return n;
}

long Internal::Impl::bio_blob_ctrl(BIO* bio, int cmd, long num, void* ptr)
{
    NTCCFG_WARNING_UNUSED(num);
    NTCCFG_WARNING_UNUSED(ptr);

    if (cmd == BIO_CTRL_FLUSH) {
        return 1;
    }

    if (cmd == BIO_CTRL_RESET) {
        return 1;
    }

    if (cmd == BIO_CTRL_DUP) {
        return 1;
    }

    if (cmd == BIO_CTRL_PUSH) {
        return 0;
    }

    if (cmd == BIO_CTRL_POP) {
        return 0;
    }

    if (cmd == BIO_CTRL_EOF) {
        return 0;
    }

    if (cmd == BIO_CTRL_GET_CLOSE) {
        return BIO_CLOSE;
    }

    if (cmd == BIO_CTRL_SET_CLOSE) {
        return 1;
    }

    if (cmd == BIO_CTRL_WPENDING) {
        return 0;
    }

    if (cmd == BIO_CTRL_PENDING) {
        bdlbb::Blob* blob = reinterpret_cast<bdlbb::Blob*>(BIO_get_data(bio));
        return static_cast<long>(blob->length());
    }

    if (cmd == BIO_CTRL_INFO) {
        return 0;
    }

    if (cmd == BIO_CTRL_SET_CALLBACK) {
        return 0;
    }

    if (cmd == BIO_C_SET_NBIO) {
        return 0;
    }

#if defined(BIO_CTRL_GET_KTLS_SEND)
    if (cmd == BIO_CTRL_GET_KTLS_SEND) {
        return 0;
    }
#endif

#if defined(BIO_CTRL_GET_KTLS_RECV)
    if (cmd == BIO_CTRL_GET_KTLS_RECV) {
        return 0;
    }
#endif

#if defined(BIO_CTRL_SET_KTLS_SEND)
    if (cmd == BIO_CTRL_SET_KTLS_SEND) {
        return 0;
    }
#endif

#if defined(BIO_CTRL_SET_KTLS_TX_SEND_CTRL_MSG)
    if (cmd == BIO_CTRL_SET_KTLS_TX_SEND_CTRL_MSG) {
        return 0;
    }
#endif

#if defined(BIO_CTRL_CLEAR_KTLS_TX_CTRL_MSG)
    if (cmd == BIO_CTRL_CLEAR_KTLS_TX_CTRL_MSG) {
        return 0;
    }
#endif

#if defined(BIO_CTRL_GET_RPOLL_DESCRIPTOR)
    if (cmd == BIO_CTRL_GET_RPOLL_DESCRIPTOR) {
        return 0;
    }
#endif

#if defined(BIO_CTRL_GET_WPOLL_DESCRIPTOR)
    if (cmd == BIO_CTRL_GET_WPOLL_DESCRIPTOR) {
        return 0;
    }
#endif

#if defined(BIO_C_SSL_MODE)
    if (cmd == BIO_C_SSL_MODE) {
        return 0;
    }
#endif

#if defined(BIO_C_DO_STATE_MACHINE)
    if (cmd == BIO_C_DO_STATE_MACHINE) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_SSL)
    if (cmd == BIO_C_SET_SSL) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_SSL)
    if (cmd == BIO_C_GET_SSL) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_SSL_RENEGOTIATE_TIMEOUT)
    if (cmd == BIO_C_SET_SSL_RENEGOTIATE_TIMEOUT) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_SSL_RENEGOTIATE_BYTES)
    if (cmd == BIO_C_SET_SSL_RENEGOTIATE_BYTES) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_SSL_NUM_RENEGOTIATES)
    if (cmd == BIO_C_GET_SSL_NUM_RENEGOTIATES) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_FD)
    if (cmd == BIO_C_GET_FD) {
        return 0;
    }
#endif

#if defined(BIO_C_NREAD)
    if (cmd == BIO_C_NREAD) {
        return 0;
    }
#endif

#if defined(BIO_C_NREAD0)
    if (cmd == BIO_C_NREAD0) {
        return 0;
    }
#endif

#if defined(BIO_C_NWRITE)
    if (cmd == BIO_C_NWRITE) {
        return 0;
    }
#endif

#if defined(BIO_C_NWRITE0)
    if (cmd == BIO_C_NWRITE0) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_EX_ARG)
    if (cmd == BIO_C_SET_EX_ARG) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_WRITE_GUARANTEE)
    if (cmd == BIO_C_GET_WRITE_GUARANTEE) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_READ_REQUEST)
    if (cmd == BIO_C_GET_READ_REQUEST) {
        return 0;
    }
#endif

#if defined(BIO_C_RESET_READ_REQUEST)
    if (cmd == BIO_C_RESET_READ_REQUEST) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_CONNECT)
    if (cmd == BIO_C_SET_CONNECT) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_CONNECT_MODE)
    if (cmd == BIO_C_SET_CONNECT_MODE) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_CONNECT)
    if (cmd == BIO_C_GET_CONNECT) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_ACCEPT)
    if (cmd == BIO_C_SET_ACCEPT) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_BIND_MODE)
    if (cmd == BIO_C_SET_BIND_MODE) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_BIND_MODE)
    if (cmd == BIO_C_GET_BIND_MODE) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_FILENAME)
    if (cmd == BIO_C_SET_FILENAME) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_FILE_PTR)
    if (cmd == BIO_C_SET_FILE_PTR) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_FILE_PTR)
    if (cmd == BIO_C_GET_FILE_PTR) {
        return 0;
    }
#endif

#if defined(BIO_C_SET_FD)
    if (cmd == BIO_C_SET_FD) {
        return 0;
    }
#endif

#if defined(BIO_C_GET_FD)
    if (cmd == BIO_C_GET_FD) {
        return 0;
    }
#endif

#if defined(BIO_C_FILE_SEEK)
    if (cmd == BIO_C_FILE_SEEK) {
        return 0;
    }
#endif

#if defined(BIO_C_FILE_TELL)
    if (cmd == BIO_C_FILE_TELL) {
        return 0;
    }
#endif

#if defined(OSSL_TRACE_CTRL_BEGIN)
    if (cmd == OSSL_TRACE_CTRL_BEGIN) {
        return 0;
    }
#endif

#if defined(OSSL_TRACE_CTRL_END)
    if (cmd == OSSL_TRACE_CTRL_END) {
        return 0;
    }
#endif

    return 0;
}

int Internal::Impl::bio_streambuf_new(BIO* bio)
{
    BIO_set_init(bio, 1);
    return 1;
}

int Internal::Impl::bio_streambuf_free(BIO* bio)
{
    if (bio == 0) {
        return 0;
    }

    BIO_set_init(bio, 0);
    BIO_set_data(bio, 0);
    return 1;
}

int Internal::Impl::bio_streambuf_write(BIO* bio, const char* data, int size)
{
    bsl::streambuf* sb = reinterpret_cast<bsl::streambuf*>(BIO_get_data(bio));

    BIO_clear_retry_flags(bio);

    if (size < 0) {
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    bsl::streamsize n = sb->sputn(data, size);
    if (n <= 0) {
        return -1;
    }

    return static_cast<int>(n);
}

int Internal::Impl::bio_streambuf_read(BIO* bio, char* data, int size)
{
    bsl::streambuf* sb = reinterpret_cast<bsl::streambuf*>(BIO_get_data(bio));

    BIO_clear_retry_flags(bio);

    if (size < 0) {
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    bsl::streamsize n = sb->sgetn(data, size);
    if (n == 0) {
        BIO_set_retry_read(bio);
        return -1;
    }

    return static_cast<int>(n);
}

int Internal::Impl::bio_streambuf_puts(BIO* bio, const char* data)
{
    bsl::streambuf* sb = reinterpret_cast<bsl::streambuf*>(BIO_get_data(bio));

    const int size = static_cast<int>(bsl::strlen(data));

    bsl::streamsize n = sb->sputn(data, size);
    if (n < 0) {
        return -1;
    }

    return static_cast<int>(n);
}

int Internal::Impl::bio_streambuf_gets(BIO* bio, char* data, int size)
{
    bsl::streambuf* sb = reinterpret_cast<bsl::streambuf*>(BIO_get_data(bio));

    if (size == 0) {
        return 0;
    }

    if (size == 1) {
        *data = 0;
        return 0;
    }

    char* current = data;
    char* end     = data + size - 1;

    while (true) {
        if (current == end) {
            break;
        }

        bsl::streambuf::int_type meta = sb->sbumpc();

        if (bsl::streambuf::traits_type::eq_int_type(
                meta,
                bsl::streambuf::traits_type::eof()))
        {
            break;
        }

        char ch = bsl::streambuf::traits_type::to_char_type(meta);

        if (ch == '\r') {
            continue;
        }

        *current = ch;
        ++current;

        if (ch == '\n') {
            break;
        }
    }

    BSLS_ASSERT(current < data + size);
    *current = 0;

    int n = static_cast<int>(current - data);
    BSLS_ASSERT(n >= 0);

    return n;
}

long Internal::Impl::bio_streambuf_ctrl(BIO* bio, int cmd, long num, void* ptr)
{
    NTCCFG_WARNING_UNUSED(num);
    NTCCFG_WARNING_UNUSED(ptr);

    bsl::streambuf* sb = reinterpret_cast<bsl::streambuf*>(BIO_get_data(bio));

    if (cmd == BIO_CTRL_FLUSH) {
        int rc = sb->pubsync();
        if (rc != 0) {
            return 0;
        }
        return 1;
    }

    if (cmd == BIO_CTRL_RESET) {
        return 1;
    }

    if (cmd == BIO_CTRL_DUP) {
        return 1;
    }

    if (cmd == BIO_CTRL_PUSH) {
        return 0;
    }

    if (cmd == BIO_CTRL_POP) {
        return 0;
    }

    if (cmd == BIO_CTRL_EOF) {
        return 0;
    }

    if (cmd == BIO_CTRL_GET_CLOSE) {
        return BIO_CLOSE;
    }

    if (cmd == BIO_CTRL_SET_CLOSE) {
        return 1;
    }

    if (cmd == BIO_CTRL_WPENDING) {
        return 0;
    }

    if (cmd == BIO_CTRL_PENDING) {
        return static_cast<long>(sb->in_avail());
    }

    return 1;
}

BIO* Internal::Impl::BIO_new_blob(bdlbb::Blob* blob)
{
    BIO* bio = BIO_new(Internal::Impl::s_blobMethods);
    BSLS_ASSERT_OPT(bio);

    BIO_set_data(bio, blob);

    return bio;
}

BIO* Internal::Impl::BIO_new_streambuf(bsl::streambuf* buffer)
{
    BIO* bio = BIO_new(Internal::Impl::s_streambufMethods);
    BSLS_ASSERT_OPT(bio);

    BIO_set_data(bio, buffer);

    return bio;
}

template <typename TYPE>
NTCCFG_INLINE Handle<TYPE>::Handle()
: d_ptr_p(0)
, d_deleter(reinterpret_cast<Deleter>(0))
{
}

template <typename TYPE>
NTCCFG_INLINE Handle<TYPE>::Handle(Type* object)
: d_ptr_p(object)
, d_deleter(reinterpret_cast<Deleter>(HandleUtil::deleter(object)))
{
    BSLS_ASSERT_OPT(d_deleter);
}

template <typename TYPE>
NTCCFG_INLINE Handle<TYPE>::Handle(Type* object, Deleter deleter)
: d_ptr_p(object)
, d_deleter(deleter)
{
    BSLS_ASSERT_OPT(d_deleter);
}

template <typename TYPE>
NTCCFG_INLINE Handle<TYPE>::~Handle()
{
    if (d_ptr_p) {
        if (d_deleter) {
            d_deleter(d_ptr_p);
        }
    }
}

template <typename TYPE>
NTCCFG_INLINE void Handle<TYPE>::reset()
{
    if (d_ptr_p) {
        if (d_deleter) {
            d_deleter(d_ptr_p);
            d_deleter = 0;
        }

        d_ptr_p = 0;
    }
}

template <typename TYPE>
NTCCFG_INLINE void Handle<TYPE>::reset(Type* object)
{
    if (d_ptr_p) {
        if (d_deleter) {
            d_deleter(d_ptr_p);
        }
    }

    d_ptr_p   = object;
    d_deleter = reinterpret_cast<Deleter>(HandleUtil::deleter(object));

    BSLS_ASSERT_OPT(d_deleter);
}

template <typename TYPE>
NTCCFG_INLINE void Handle<TYPE>::reset(Type* object, Deleter deleter)
{
    if (d_ptr_p) {
        if (d_deleter) {
            d_deleter(d_ptr_p);
        }
    }

    d_ptr_p   = object;
    d_deleter = deleter;

    BSLS_ASSERT_OPT(d_deleter);
}

template <typename TYPE>
NTCCFG_INLINE typename Handle<TYPE>::Type* Handle<TYPE>::release()
{
    Type* result = d_ptr_p;
    d_ptr_p      = 0;
    return result;
}

template <typename TYPE>
NTCCFG_INLINE typename Handle<TYPE>::Type* Handle<TYPE>::get() const
{
    return d_ptr_p;
}

template <typename TYPE>
NTCCFG_INLINE Handle<TYPE>::operator bool() const
{
    return d_ptr_p != 0;
}

NTCCFG_INLINE
void HandleUtil::deleteBio(BIO* object)
{
    int rc = BIO_free(object);
    NTCCFG_WARNING_UNUSED(rc);
}

template <typename TYPE>
NTCCFG_INLINE HandleUtil::Deleter HandleUtil::deleter(TYPE*)
{
    BSLS_ASSERT_OPT(!"Not implemented");
    return 0;
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(BIO*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&HandleUtil::deleteBio);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(BIGNUM*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&BN_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(DH*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&DH_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(DSA*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&DSA_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(RSA*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&RSA_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(EVP_PKEY*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&EVP_PKEY_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(EVP_PKEY_CTX*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&EVP_PKEY_CTX_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(X509*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&X509_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(X509_NAME*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&X509_NAME_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(X509_EXTENSION*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&X509_EXTENSION_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(X509_ALGOR*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&X509_ALGOR_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(X509_SIG*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&X509_SIG_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(X509_STORE*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&X509_STORE_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(X509_STORE_CTX*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&X509_STORE_CTX_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(SSL*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&SSL_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(SSL_CTX*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&SSL_CTX_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(PKCS12*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&PKCS12_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(PKCS7*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&PKCS7_free);
}

NTCCFG_INLINE
HandleUtil::Deleter HandleUtil::deleter(PKCS8_PRIV_KEY_INFO*)
{
    return reinterpret_cast<HandleUtil::Deleter>(&PKCS8_PRIV_KEY_INFO_free);
}

ErrorInfo::ErrorInfo(bslma::Allocator* basicAllocator)
: d_number(0)
, d_description(basicAllocator)
{
}

ErrorInfo::ErrorInfo(const ErrorInfo&  original,
                     bslma::Allocator* basicAllocator)
: d_number(original.d_number)
, d_description(original.d_description, basicAllocator)
{
}

ErrorInfo::~ErrorInfo()
{
}

ErrorInfo& ErrorInfo::operator=(const ErrorInfo& other)
{
    if (this != &other) {
        d_number      = other.d_number;
        d_description = other.d_description;
    }

    return *this;
}

void ErrorInfo::reset()
{
    d_number = 0;
    d_description.clear();
}

void ErrorInfo::setNumber(ntctls::ErrorNumber value)
{
    d_number = value;
}

void ErrorInfo::setDescription(const bsl::string& value)
{
    d_description = value;
}

ntctls::ErrorNumber ErrorInfo::number() const
{
    return d_number;
}

int ErrorInfo::library() const
{
    return ERR_GET_LIB(d_number);
}

int ErrorInfo::function() const
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    return 0;
#else
    return ERR_GET_FUNC(d_number);
#endif
}

int ErrorInfo::reason() const
{
    return ERR_GET_REASON(d_number);
}

bool ErrorInfo::match(int library, int reason) const
{
    const int sourceLibrary = ERR_GET_LIB(d_number);
    const int sourceReason  = ERR_GET_REASON(d_number);

    return library == sourceLibrary && reason == sourceReason;
}

const bsl::string& ErrorInfo::description() const
{
    return d_description;
}

bool ErrorInfo::equals(const ErrorInfo& other) const
{
    return d_number == other.d_number && d_description == other.d_description;
}

bsl::ostream& ErrorInfo::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_number != 0) {
        const int library  = ERR_GET_LIB(d_number);
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        const int function = 0;
#else
        const int function = ERR_GET_FUNC(d_number);
#endif
        const int reason   = ERR_GET_REASON(d_number);

        printer.printAttribute("library", library);
        printer.printAttribute("function", function);
        printer.printAttribute("reason", reason);
    }

    if (!d_description.empty()) {
        printer.printAttribute("description", d_description);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const ErrorInfo& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const ErrorInfo& lhs, const ErrorInfo& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const ErrorInfo& lhs, const ErrorInfo& rhs)
{
    return !operator==(lhs, rhs);
}

ErrorStack::ErrorStack(bslma::Allocator* basicAllocator)
: d_container(basicAllocator)
, d_sentinel(basicAllocator)
{
}

ErrorStack::ErrorStack(const ErrorStack& original,
                       bslma::Allocator* basicAllocator)
: d_container(original.d_container, basicAllocator)
, d_sentinel(basicAllocator)
{
}

ErrorStack::~ErrorStack()
{
}

ErrorStack& ErrorStack::operator=(const ErrorStack& other)
{
    if (this != &other) {
        d_container = other.d_container;
    }

    return *this;
}

void ErrorStack::reset()
{
    d_container.clear();
}

void ErrorStack::push(const ntctls::ErrorInfo& errorInfo)
{
    d_container.push_back(errorInfo);
}

void ErrorStack::push(const ntctls::ErrorStack& errorStack)
{
    d_container.insert(d_container.end(),
                       errorStack.d_container.begin(),
                       errorStack.d_container.end());
}

const ntctls::ErrorInfo& ErrorStack::last() const
{
    if (!d_container.empty()) {
        return d_container.back();
    }
    else {
        return d_sentinel;
    }
}

bool ErrorStack::find(int library, int reason) const
{
    if (!d_container.empty()) {
        for (Container::const_iterator it = d_container.begin();
             it != d_container.end();
             ++it)
        {
            const bool match = it->match(library, reason);
            if (match) {
                return true;
            }
        }
    }

    return false;
}

bsl::string ErrorStack::text() const
{
    bsl::stringstream ss;
    this->print(ss, 0, -1);
    ss.flush();
    return ss.str();
}

bool ErrorStack::empty() const
{
    return d_container.empty();
}

bool ErrorStack::equals(const ErrorStack& other) const
{
    return d_container == other.d_container;
}

bsl::ostream& ErrorStack::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_container.empty()) {
        printer.printAttribute("error", d_container);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const ErrorStack& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const ErrorStack& lhs, const ErrorStack& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const ErrorStack& lhs, const ErrorStack& rhs)
{
    return !operator==(lhs, rhs);
}

/// Provide a mechanism to initializer and clean up the process-wide resources
/// required by this implementation.
class Initializer
{
  public:
    // Initialize the process-wide resources required by this implemention.
    Initializer();

    // Clean up the process-wide resources required by this implemention.
    ~Initializer();
};

void Internal::initialize()
{
    BSLMT_ONCE_DO
    {
        static ntctls::Initializer initializer;
    }
}

void Internal::exit()
{
}

void Internal::drainErrorQueue(bsl::vector<bsl::string>* errorQueue)
{
    unsigned long rc;
    while (0 != (rc = ERR_get_error())) {
        char buffer[512];
        buffer[0] = 0;

        ERR_error_string_n(rc, buffer, sizeof(buffer));

        errorQueue->push_back(buffer);
    }
}

void Internal::drainErrorQueue(bsl::string* description)
{
    unsigned long rc;
    while (0 != (rc = ERR_get_error())) {
        char buffer[512];
        buffer[0] = 0;

        ERR_error_string_n(rc, buffer, sizeof(buffer));

        description->append("[ ", 2);
        description->append(buffer);
        description->append(" ]", 2);
    }
}

void Internal::drainErrorQueue(bsl::streambuf* description)
{
    unsigned long rc;
    while (0 != (rc = ERR_get_error())) {
        char buffer[512];
        buffer[0] = 0;

        ERR_error_string_n(rc, buffer, sizeof(buffer));

        description->sputn("[ ", 2);
        description->sputn(buffer, bsl::strlen(buffer));
        description->sputn(" ]", 2);
    }
}

void Internal::drainErrorQueue(ntctls::ErrorStack* errorStack)
{
    while (true) {
        unsigned long rc = ERR_get_error();
        if (rc == 0) {
            break;
        }

        char buffer[512];
        buffer[0] = 0;

        ERR_error_string_n(rc, buffer, sizeof(buffer));

        ntctls::ErrorInfo errorInfo;
        errorInfo.setNumber(rc);
        errorInfo.setDescription(buffer);

        errorStack->push(errorInfo);
    }
}

void Internal::convertDatetimeToAsn1TimeString(bsl::string*          result,
                                               const bdlt::Datetime& dateTime)
{
    bsl::stringstream ss;

    ss << bsl::setw(4);
    ss << bsl::setfill('0');
    ss << dateTime.year();

    ss << bsl::setw(2);
    ss << bsl::setfill('0');
    ss << dateTime.month();

    ss << bsl::setw(2);
    ss << bsl::setfill('0');
    ss << dateTime.day();

    ss << bsl::setw(2);
    ss << bsl::setfill('0');
    ss << dateTime.hour();

    ss << bsl::setw(2);
    ss << bsl::setfill('0');
    ss << dateTime.minute();

    ss << bsl::setw(2);
    ss << bsl::setfill('0');
    ss << dateTime.second();

    ss << "Z";

    *result = ss.str();
}

bsl::shared_ptr<BIO> Internal::createStream(bsl::streambuf*   buffer,
                                            bslma::Allocator* basicAllocator)
{
    ntctls::Internal::initialize();

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<BIO> bio_sp;

    BIO* bio = Internal::Impl::BIO_new_streambuf(buffer);
    if (bio) {
        bio_sp.reset(bio, &BIO_free, allocator);
    }

    return bio_sp;
}

BIO* Internal::createStreamRaw(bsl::streambuf* buffer)
{
    ntctls::Internal::initialize();
    return Internal::Impl::BIO_new_streambuf(buffer);
}

bsl::shared_ptr<BIO> Internal::createStream(bdlbb::Blob*      blob,
                                            bslma::Allocator* basicAllocator)
{
    ntctls::Internal::initialize();

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<BIO> bio_sp;

    BIO* bio = Internal::Impl::BIO_new_blob(blob);
    if (bio) {
        bio_sp.reset(bio, &BIO_free, allocator);
    }

    return bio_sp;
}

BIO* Internal::createStreamRaw(bdlbb::Blob* blob)
{
    ntctls::Internal::initialize();
    return Internal::Impl::BIO_new_blob(blob);
}

void Internal::destroyStream(BIO* bio)
{
    BIO_free(bio);
}

/// Provide a container for a private key.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctls
class Key : public ntci::EncryptionKey, public ntccfg::Shared<Key>
{
    /// Enumerate the constants used by the implementation.
    enum Constant {
        k_DEFAULT_DSA_BITS     = 2048,
        k_DEFAULT_RSA_BITS     = 2048,
        k_DEFAULT_RSA_EXPONENT = 65537
    };

    ntctls::Handle<EVP_PKEY> d_pkey;
    ntca::EncryptionKey      d_record;
    bslma::Allocator*        d_allocator_p;

  private:
    Key(const Key&) BSLS_KEYWORD_DELETED;
    Key& operator=(const Key& other) BSLS_KEYWORD_DELETED;

  private:
    /// Load into the specified 'result' a new DSA key. Return the error.
    static ntsa::Error generateDsa(ntctls::Handle<EVP_PKEY>* result);

    /// Load into the specified 'result' a new RSA key. Return the error.
    static ntsa::Error generateRsa(ntctls::Handle<EVP_PKEY>* result);

    /// Load into the specified 'result' a new elliptic curve key having the
    /// well-known parameters identified by the specified 'parameterId'. Return
    /// the error.
    static ntsa::Error generateEllipticCurve(ntctls::Handle<EVP_PKEY>* result,
                                             int parameterId);

    /// Load into the specified 'result' a new Edwards curve key whose curve
    /// type is identified by the specified 'typeId'. Return the error.
    static ntsa::Error generateEdwardsCurve(ntctls::Handle<EVP_PKEY>* result,
                                            int                       typeId);

  public:
    /// Create a new, initially empty private key. Optionally specify a
    /// 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit Key(bslma_Allocator* basicAllocator = 0);

    /// Create a new private key implemented using the specified 'pkey'.
    /// Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Key(EVP_PKEY* pkey, bslma_Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Key() BSLS_KEYWORD_OVERRIDE;

    /// Generate a new key according to the specified 'configuration'.
    ntsa::Error generate(const ntca::EncryptionKeyOptions& configuration);

    /// Decode the key according to the specified 'options' from the specified
    /// 'source'.
    ntsa::Error decode(bsl::streambuf*                        source,
                       const ntca::EncryptionResourceOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Encode the key according to the specified 'options' to the specified
    /// 'destination'.
    ntsa::Error encode(bsl::streambuf*                        destination,
                       const ntca::EncryptionResourceOptions& options) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load the value-semantic representation of the certificate into the
    /// specified 'result'.
    ntsa::Error unwrap(ntca::EncryptionKey* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Print the public key parameters to the specified stream in an
    /// unspecified but human-readable form.
    void print(bsl::ostream& stream) const BSLS_KEYWORD_OVERRIDE;

    /// Return a handle to the private implementation.
    void* handle() const BSLS_KEYWORD_OVERRIDE;

    /// Return a handle to the private implementation.
    EVP_PKEY* native() const;

    /// Return the value-semantic representation.
    const ntca::EncryptionKey& record() const;

    /// Return true if the specified 'other' key has the same public key
    /// components and parameters, otherwise return false.
    bool equals(const ntci::EncryptionKey& other) const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified result a newly-generated key according to the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static ntsa::Error generateKey(
        bsl::shared_ptr<ntci::EncryptionKey>* result,
        const ntca::EncryptionKeyOptions&     options,
        bslma::Allocator*                     basicAllocator);

    /// Load into the specified result a newly-generated key according to the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static ntsa::Error generateKey(ntca::EncryptionKey*              result,
                                   const ntca::EncryptionKeyOptions& options,
                                   bslma::Allocator* basicAllocator);
};

/// Define a type alias for a vector of keys.
typedef bsl::vector<bsl::shared_ptr<ntctls::Key> > KeyVector;

/// Provide a container for an X.509 digital certificate.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctls
class Certificate : public ntci::EncryptionCertificate,
                    public ntccfg::Shared<Certificate>
{
    ntctls::Handle<X509>        d_x509;
    ntca::EncryptionCertificate d_record;
    ntsa::DistinguishedName     d_subject;
    ntsa::DistinguishedName     d_issuer;
    bslma::Allocator*           d_allocator_p;

    class Impl;

  private:
    Certificate(const Certificate&) BSLS_KEYWORD_DELETED;
    Certificate& operator=(const Certificate& other) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new, initially empty certificate. Optionally specify a
    /// 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit Certificate(bslma_Allocator* basicAllocator = 0);

    /// Create a new certificate implemented using the specified 'x509'.
    /// Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Certificate(X509* x509, bslma_Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Certificate() BSLS_KEYWORD_OVERRIDE;

    /// Load into this object a newly-generated certificate for the specified
    /// 'userIdentity' and 'userPrivateKey' signed by itself. Return the error.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    ntsa::Error generate(
        const ntsa::DistinguishedName&            userIdentity,
        const bsl::shared_ptr<Key>&               userPrivateKey,
        const ntca::EncryptionCertificateOptions& configuration);

    /// Load into this object a newly-generated certificate for the specified
    /// 'userIdentity' and 'userPrivateKey' signed by the certificate authority
    /// identified by the specified 'authorityCertificate' that uses the
    /// specified 'authorityPrivateKey'. Return the error. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    ntsa::Error generate(
        const ntsa::DistinguishedName&            userIdentity,
        const bsl::shared_ptr<Key>&               userPrivateKey,
        const bsl::shared_ptr<Certificate>&       authorityCertificate,
        const bsl::shared_ptr<Key>&               authorityPrivateKey,
        const ntca::EncryptionCertificateOptions& configuration);

    /// Decode the certificate according to the specified 'options' from the
    /// specified 'source'.
    ntsa::Error decode(bsl::streambuf*                        source,
                       const ntca::EncryptionResourceOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Encode the certificate according to the specified 'options' to the
    /// specified 'destination'.
    ntsa::Error encode(bsl::streambuf*                        destination,
                       const ntca::EncryptionResourceOptions& options) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load the value-semantic representation of the certificate into the
    /// specified 'result'.
    ntsa::Error unwrap(ntca::EncryptionCertificate* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Print the certificate to the specified stream in an unspecified but
    /// human-readable form.
    void print(bsl::ostream& stream) const BSLS_KEYWORD_OVERRIDE;

    /// Return the subject of the certificate.
    const ntsa::DistinguishedName& subject() const BSLS_KEYWORD_OVERRIDE;

    /// Return the issuer of the certificate.
    const ntsa::DistinguishedName& issuer() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if this certificate is a certificate authority, otherwise
    /// return false.
    bool isAuthority() const;

    /// Return true if this certificate is self-signed, otherwise return false.
    bool isSelfSigned() const;

    /// Return a handle to the private implementation.
    void* handle() const BSLS_KEYWORD_OVERRIDE;

    /// Return a handle to the native implementation.
    X509* native() const;

    /// Return the value-semantic representation.
    const ntca::EncryptionCertificate& record() const;

    /// Return true if the specified 'other' certificate has the same value,
    /// otherwise return false.
    bool equals(const ntci::EncryptionCertificate& other) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a newly-generated certificate for the
    /// specified 'userIdentity' and 'userPrivateKey' signed by itself.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntsa::DistinguishedName&                userIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&   userPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator*                             basicAllocator);

    /// Load into the specified 'result' a newly-generated certificate for the
    /// specified 'userIdentity' and 'userPrivateKey' signed by the certificate
    /// authority identified by the specified 'authorityCertificate' that uses
    /// the specified 'authorityPrivateKey'.  Return 0 on success and a
    /// non-zero value otherwise. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntsa::DistinguishedName&                userIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&   userPrivateKey,
        const bsl::shared_ptr<ntci::EncryptionCertificate>&
                                                    authorityCertificate,
        const bsl::shared_ptr<ntci::EncryptionKey>& authorityPrivateKey,
        const ntca::EncryptionCertificateOptions&   options,
        bslma::Allocator*                           basicAllocator);

    /// Load into the specified 'result' a newly-generated certificate for the
    /// specified 'userIdentity' and 'userPrivateKey' signed by itself.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    static ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            userIdentity,
        const ntca::EncryptionKey&                userPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator*                         basicAllocator);

    /// Load into the specified 'result' a newly-generated certificate for the
    /// specified 'userIdentity' and 'userPrivateKey' signed by the certificate
    /// authority identified by the specified 'authorityCertificate' that uses
    /// the specified 'authorityPrivateKey'.  Return 0 on success and a
    /// non-zero value otherwise. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    static ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            userIdentity,
        const ntca::EncryptionKey&                userPrivateKey,
        const ntca::EncryptionCertificate&        authorityCertificate,
        const ntca::EncryptionKey&                authorityPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator*                         basicAllocator);
};

/// Define a type alias for a vector of certificates.
typedef bsl::vector<bsl::shared_ptr<ntctls::Certificate> > CertificateVector;

/// Provide a storage of private keys and certificates as used
/// in public key cryptography.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctls
class Resource : public ntci::EncryptionResource
{
    bsl::shared_ptr<ntctls::Key>         d_privateKey_sp;
    bsl::shared_ptr<ntctls::Certificate> d_certificate_sp;
    ntctls::CertificateVector            d_caList;
    bslma::Allocator*                    d_allocator_p;

  private:
    Resource(const Resource&) BSLS_KEYWORD_DELETED;
    Resource& operator=(const Resource&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new resource. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Resource(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Resource() BSLS_KEYWORD_OVERRIDE;

    /// Set the private key to the specified 'key'. Return the error.
    ntsa::Error setPrivateKey(const ntca::EncryptionKey& key)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the private key to the specified 'key'. Return the error.
    ntsa::Error setPrivateKey(const bsl::shared_ptr<ntci::EncryptionKey>& key)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the user's certificate to the specified 'certificate'. Return the
    /// error.
    ntsa::Error setCertificate(const ntca::EncryptionCertificate& certificate)
        BSLS_KEYWORD_OVERRIDE;

    /// Set the user's certificate to the specified 'certificate'. Return the
    /// error.
    ntsa::Error setCertificate(
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'certificate' to the list of trusted certificates.
    /// Return the error.
    ntsa::Error addCertificateAuthority(
        const ntca::EncryptionCertificate& certificate)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'certificate' to the list of trusted certificates.
    /// Return the error.
    ntsa::Error addCertificateAuthority(
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
        BSLS_KEYWORD_OVERRIDE;

    /// Decode the resource according to the specified 'options' from the
    /// specified 'source'. Return the error.
    ntsa::Error decode(bsl::streambuf*                        source,
                       const ntca::EncryptionResourceOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    /// Encode the resource according to the specified 'options' to the
    /// specified 'destination'. Return the error.
    ntsa::Error encode(bsl::streambuf*                        destination,
                       const ntca::EncryptionResourceOptions& options) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the private key stored in the
    /// resource. Return the error.
    ntsa::Error getPrivateKey(bsl::shared_ptr<ntci::EncryptionKey>* result)
        const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the user's certificate stored in the
    /// resource. Return the error.
    ntsa::Error getCertificate(bsl::shared_ptr<ntci::EncryptionCertificate>*
                                   result) const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the user's certificate stored in the
    /// resource. Return the error.
    ntsa::Error getCertificateAuthoritySet(
        bsl::vector<bsl::shared_ptr<ntci::EncryptionCertificate> >* result)
        const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the specified 'other' resource has the same value,
    /// otherwise return false.
    bool equals(const Resource& other) const;

    /// Return true if this resource contains the keys of the specified 'other'
    /// resource (if the specified 'includePrivateKeys' flag is true) and
    /// this resource contains the certificate and certificate authorities of
    /// the 'other' resource (if the specified 'includeCertificates' flag is
    /// true.
    bool contains(const Resource& other,
                  bool            includePrivateKeys,
                  bool            includeCertificates) const;

    /// Load into the specified 'result' the driver representation of the
    /// specified 'certificate'. Return the error.
    static ntsa::Error convert(
        bsl::shared_ptr<ntctls::Certificate>*               result,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'certificate'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error convert(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntca::EncryptionCertificate&            certificate,
        bslma::Allocator*                             basicAllocator = 0);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'certificate'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error convert(bsl::shared_ptr<ntctls::Certificate>* result,
                               const ntca::EncryptionCertificate& certificate,
                               bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'certificate'. Return the error.
    static ntsa::Error convert(ntctls::Handle<X509>*              result,
                               const ntca::EncryptionCertificate& certificate);

    /// Load into the specified 'result' the description of the specified
    /// 'certificate'. Return the error.
    static ntsa::Error convert(
        ntca::EncryptionCertificate*                result,
        const bsl::shared_ptr<ntctls::Certificate>& certificate);

    /// Load into the specified 'result' the description of the specified
    /// 'certificate'. Return the error.
    static ntsa::Error convert(ntca::EncryptionCertificate* result,
                               const ntctls::Handle<X509>&  certificate);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'key'. Return the error.
    static ntsa::Error convert(
        bsl::shared_ptr<ntctls::Key>*               result,
        const bsl::shared_ptr<ntci::EncryptionKey>& key);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'key'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error convert(bsl::shared_ptr<ntci::EncryptionKey>* result,
                               const ntca::EncryptionKey&            key,
                               bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'key'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error convert(bsl::shared_ptr<ntctls::Key>* result,
                               const ntca::EncryptionKey&    key,
                               bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'key'. Return the error.
    static ntsa::Error convert(ntctls::Handle<EVP_PKEY>*  result,
                               const ntca::EncryptionKey& key);

    /// Load into the specified 'result' the description of the specified
    /// 'key'. Return the error.
    static ntsa::Error convert(ntca::EncryptionKey*                result,
                               const bsl::shared_ptr<ntctls::Key>& key);

    /// Load into the specified 'result' the description of the specified
    /// 'key'. Return the error.
    static ntsa::Error convert(ntca::EncryptionKey*            result,
                               const ntctls::Handle<EVP_PKEY>& key);
};

#define NTCTLS_KEY_LOG_GENERAL_ERROR(reason)                                  \
    do {                                                                      \
        bsl::string description;                                              \
        ntctls::Internal::drainErrorQueue(&description);                      \
        BSLS_LOG_TRACE("%s: %s", reason, description.c_str());                \
    } while (false)

#define NTCTLS_KEY_LOG_ENCODE_ERROR()                                         \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to encode private key")

#define NTCTLS_KEY_LOG_DECODE_ERROR()                                         \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to decode private key")

#define NTCTLS_KEY_LOG_BN_ERROR_ALLOCATE()                                    \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to allocate big number")

#define NTCTLS_KEY_LOG_BN_ERROR_SET_EXPONENT()                                \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to set exponent")

#define NTCTLS_KEY_LOG_DSA_ERROR_ALLOCATE()                                   \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to allocate DSA key")

#define NTCTLS_KEY_LOG_DSA_ERROR_GENERATE()                                   \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to generate DSA key")

#define NTCTLS_KEY_LOG_RSA_ERROR_ALLOCATE()                                   \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to allocate RSA key")

#define NTCTLS_KEY_LOG_RSA_ERROR_GENERATE()                                   \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to generate RSA key")

#define NTCTLS_KEY_LOG_EV_PKEY_ERROR_ALLOCATE()                               \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to allocate private key")

#define NTCTLS_KEY_LOG_EV_PKEY_ERROR_ASSIGN()                                 \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to assign private key")

#define NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_ALLOCATE()                           \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to allocate private key context")

#define NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_KEYGEN_INIT()                        \
    NTCTLS_KEY_LOG_GENERAL_ERROR(                                             \
        "Failed to initialize private key context generator")

#define NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_KEYGEN_SET_CURVE()                   \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to set private key context type")

#define NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_KEYGEN_GENERATE()                    \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Failed to generate private key")

#define NTCTLS_KEY_LOG_PASSPHRASE_INVALID()                                   \
    NTCTLS_KEY_LOG_GENERAL_ERROR("Invalid passphrase")

#define NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR(reason)                          \
    do {                                                                      \
        bsl::string description;                                              \
        ntctls::Internal::drainErrorQueue(&description);                      \
        BSLS_LOG_TRACE("%s: %s", reason, description.c_str());                \
    } while (false)

#define NTCTLS_CERTIFICATE_LOG_X509_EXTENSION_ERROR(nid, text)                \
    do {                                                                      \
        bsl::string description;                                              \
        ntctls::Internal::drainErrorQueue(&description);                      \
        BSLS_LOG_TRACE("Failed to set extension '%s': %s",                    \
                       bsl::string(text).c_str(),                             \
                       description.c_str());                                  \
    } while (false)

#define NTCTLS_CERTIFICATE_LOG_PRINT_ERROR()                                  \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to print certificate")

#define NTCTLS_CERTIFICATE_LOG_ENCODE_ERROR()                                 \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to encode "                  \
                                         "certificate")

#define NTCTLS_CERTIFICATE_LOG_DECODE_ERROR()                                 \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to decode "                  \
                                         "certificate")

#define NTCTLS_CERTIFICATE_LOG_X509_NAME_ERROR_ALLOCATE()                     \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to allocate certificate "    \
                                         "name")

#define NTCTLS_CERTIFICATE_LOG_X509_NAME_ERROR_GENERATE()                     \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to generate certificate "    \
                                         "name")

#define NTCTLS_CERTIFICATE_LOG_X509_ERROR_ALLOCATE()                          \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to allocate certificate")

#define NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_SUBJECT_NAME()                  \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to set subject name")

#define NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_ISSUER_NAME()                   \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to set issuer name")

#define NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_PUBLIC_KEY()                    \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to set public key")

#define NTCTLS_CERTIFICATE_LOG_X509_ERROR_SIGN()                              \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Failed to sign certificate")

#define NTCTLS_CERTIFICATE_LOG_PASSPHRASE_INVALID()                           \
    NTCTLS_CERTIFICATE_LOG_GENERAL_ERROR("Invalid passphrase")

#if 0
#define NTCTLS_RESOURCE_LOG_ERROR_STACK(errorStack)                           \
    do {                                                                      \
        bsl::string errorDescription = (errorStack).text();                   \
        if (!errorDescription.empty()) {                                      \
            BSLS_LOG_ERROR("Codec failure in %s: %s",                         \
                           __FUNCTION__,                                      \
                           errorDescription.c_str());                         \
        }                                                                     \
    } while (false)
#else
#define NTCTLS_RESOURCE_LOG_ERROR_STACK(errorStack)
#endif

#define NTCTLS_RESOURCE_LOG_ENCODER_ERROR(diagnostics)                        \
    do {                                                                      \
        bsl::string errorDescription = (diagnostics).text();                  \
        if (!errorDescription.empty()) {                                      \
            BSLS_LOG_TRACE("Failed to encode resource: %s",                   \
                           errorDescription.c_str());                         \
        }                                                                     \
    } while (false)

#define NTCTLS_RESOURCE_LOG_DECODER_ERROR(diagnostics)                        \
    do {                                                                      \
        bsl::string errorDescription = (diagnostics).text();                  \
        if (!errorDescription.empty()) {                                      \
            BSLS_LOG_TRACE("Failed to decode resource: %s",                   \
                           errorDescription.c_str());                         \
        }                                                                     \
    } while (false)

#define NTCTLS_RESOURCE_LOG_SECRET_UNAVAILABLE()                              \
    BSLS_LOG_TRACE("The resource requires a passphrase but no passphrase "    \
                   "resolution is available")

#define NTCTLS_RESOURCE_LOG_ALREADY_HAVE_KEY()                                \
    BSLS_LOG_ERROR("The resource contains more than one private key")

#define NTCTLS_RESOURCE_LOG_ALREADY_HAVE_CERTIFICATE()                        \
    BSLS_LOG_ERROR("The resource contains more than one certificate")

#define NTCTLS_RESOURCE_LOG_INVALID_DRIVER()                                  \
    BSLS_LOG_ERROR("The parameters are implemented with a different driver")

/// Provide a private implementation.
class Certificate::Impl
{
  public:
    /// Convert the specified 'name' into the specified 'identity'.
    static void parseDistinguishedName(ntsa::DistinguishedName* identity,
                                       X509_NAME*               name);

    /// Append the specified Distinguished Name 'component' to the specified
    /// 'result'. Return 0 on success and a non-zero value otherwise.
    static int generateX509Name(
        X509_NAME*                                result,
        const ntsa::DistinguishedName::Component& component);

    /// Add the specified 'extension' identified by the specified 'nid' to the
    /// specified 'x509' have the specified 'x509v3Ctx'. Return the error.
    static ntsa::Error addExtension(X509*       x509,
                                    X509V3_CTX* x509v3Ctx,
                                    int         nid,
                                    const char* extension);

    /// Return the extension in the specified 'x509' identified by the
    /// specified 'nid', or null if no such extension is found.
    static X509_EXTENSION* getExtension(const X509* x509, int nid);
};

void Certificate::Impl::parseDistinguishedName(
    ntsa::DistinguishedName* identity,
    X509_NAME*               name)
{
    identity->reset();

    if (name == 0) {
        return;
    }

    const bsl::size_t numEntries = X509_NAME_entry_count(name);

    for (bsl::size_t i = 0; i < numEntries; ++i) {
        X509_NAME_ENTRY* entry =
            X509_NAME_get_entry(name, static_cast<int>(i));

        const int n = OBJ_obj2nid(X509_NAME_ENTRY_get_object(entry));

        char scratch[80] = {};

        const char* entryNameData = 0;

        if ((n == NID_undef) || ((entryNameData = OBJ_nid2sn(n)) == 0)) {
            i2t_ASN1_OBJECT(scratch,
                            sizeof scratch,
                            X509_NAME_ENTRY_get_object(entry));
            entryNameData = scratch;
        }

        const bsl::size_t entryNameSize = bsl::strlen(entryNameData);

        ASN1_STRING* entry_value = X509_NAME_ENTRY_get_data(entry);

        const unsigned char* entryValueData = entry_value->data;
        const int            entryValueSize = entry_value->length;

        bsl::string component;
        if (entryNameData && entryNameSize > 0) {
            component.assign(entryNameData, entryNameSize);
        }
        else {
            component.assign("?");
        }

        bsl::string attribute;
        if (entryValueData && entryValueSize > 0) {
            attribute.assign((const char*)(entryValueData), entryValueSize);
        }

        (*identity)[component].addAttribute(attribute);
    }
}

int Certificate::Impl::generateX509Name(
    X509_NAME*                                result,
    const ntsa::DistinguishedName::Component& component)
{
    for (int i = 0; i < component.numAttributes(); ++i) {
        int loc = -1;
        int set = i == 0 ? 0 : 1;

        const bsl::string& value = component[i];
        if (!X509_NAME_add_entry_by_txt(result,
                                        component.id().c_str(),
                                        MBSTRING_ASC,
                                        (const unsigned char*)value.c_str(),
                                        static_cast<int>(value.size()),
                                        loc,
                                        set))
        {
            return -1;
        }
    }

    return 0;
}

ntsa::Error Certificate::Impl::addExtension(X509*       x509,
                                            X509V3_CTX* x509v3Ctx,
                                            int         nid,
                                            const char* extension)
{
    int rc;

    ntctls::Handle<X509_EXTENSION> ex(
        X509V3_EXT_conf_nid(0, x509v3Ctx, nid, extension));
    if (!ex) {
        NTCTLS_CERTIFICATE_LOG_X509_EXTENSION_ERROR(nid, extension);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = X509_add_ext(x509, ex.get(), -1);
    if (rc <= 0) {
        NTCTLS_CERTIFICATE_LOG_X509_EXTENSION_ERROR(nid, extension);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

X509_EXTENSION* Certificate::Impl::getExtension(const X509* x509, int nid)
{
    int rc = X509_get_ext_by_NID(x509, nid, -1);
    if (rc < 0) {
        return 0;
    }

    return X509_get_ext(x509, rc);
}

// Provide utilities to encode and decode certificates and keys to and from
// various storage formats.
class ResourceUtil
{
  private:
    // Define a type alias for a function to encode a single key.
    typedef ntsa::Error (*EncodeKey)(
        ntctls::ErrorStack*                 diagnostics,
        bsl::streambuf*                     destination,
        const bsl::shared_ptr<ntctls::Key>& privateKey,
        ntca::EncryptionResourceOptions*    options);

    // Define a type alias for a function to encode a single certificate.
    typedef ntsa::Error (*EncodeCertificate)(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        ntca::EncryptionResourceOptions*            options);

    // Define a type alias for a function to decode a single key.
    typedef ntsa::Error (*DecodeKey)(ntctls::ErrorStack*           diagnostics,
                                     bsl::streambuf*               source,
                                     bsl::shared_ptr<ntctls::Key>* privateKey,
                                     ntca::EncryptionResourceOptions* options,
                                     bslma::Allocator* allocator);

    // Define a type alias for a function to decode a single certificate.
    typedef ntsa::Error (*DecodeCertificate)(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

    // Skip and consecutive leading blank lines and/or leading lines with
    // comments.
    static void chomp(bsl::streambuf* source);

    // Detect the storage type of the specified 'source' and load the detected
    // type, if any, into the specified 'result'. Return the error.
    static ntsa::Error detectType(
        bdlb::NullableValue<ntca::EncryptionResourceType::Value>* result,
        bsl::streambuf*                                           source);

    static ntsa::Error encodeType(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Key>&         privateKey,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        const ntctls::CertificateVector&            caList,
        ntca::EncryptionResourceOptions*            options);

    static ntsa::Error encodeKeyAsn1(
        ntctls::ErrorStack*                 diagnostics,
        bsl::streambuf*                     destination,
        const bsl::shared_ptr<ntctls::Key>& privateKey,
        ntca::EncryptionResourceOptions*    options);

    static ntsa::Error encodeKeyAsn1(ntctls::ErrorStack* diagnostics,
                                     bsl::streambuf*     destination,
                                     EVP_PKEY*           privateKey,
                                     ntca::EncryptionResourceOptions* options);

    static ntsa::Error encodeCertificateAsn1(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        ntca::EncryptionResourceOptions*            options);

    static ntsa::Error encodeCertificateAsn1(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  destination,
        X509*                            certificate,
        ntca::EncryptionResourceOptions* options);

    static ntsa::Error encodeKeyAsn1Pem(
        ntctls::ErrorStack*                 diagnostics,
        bsl::streambuf*                     destination,
        const bsl::shared_ptr<ntctls::Key>& privateKey,
        ntca::EncryptionResourceOptions*    options);

    static ntsa::Error encodeCertificateAsn1Pem(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        ntca::EncryptionResourceOptions*            options);

    static ntsa::Error encodeKeyPkcs8(
        ntctls::ErrorStack*                 diagnostics,
        bsl::streambuf*                     destination,
        const bsl::shared_ptr<ntctls::Key>& privateKey,
        ntca::EncryptionResourceOptions*    options);

    static ntsa::Error encodeCertificatePkcs8(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        ntca::EncryptionResourceOptions*            options);

    static ntsa::Error encodeKeyPkcs8Pem(
        ntctls::ErrorStack*                 diagnostics,
        bsl::streambuf*                     destination,
        const bsl::shared_ptr<ntctls::Key>& privateKey,
        ntca::EncryptionResourceOptions*    options);

    static ntsa::Error encodeCertificatePkcs8Pem(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        ntca::EncryptionResourceOptions*            options);

    static ntsa::Error encodeResourcePkcs12(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Key>&         privateKey,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        const ntctls::CertificateVector&            caList,
        ntca::EncryptionResourceOptions*            options);

    static ntsa::Error encodeResourcePkcs7(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Key>&         privateKey,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        const ntctls::CertificateVector&            caList,
        ntca::EncryptionResourceOptions*            options);

    static ntsa::Error encodeResourcePkcs7Pem(
        ntctls::ErrorStack*                         diagnostics,
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Key>&         privateKey,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        const ntctls::CertificateVector&            caList,
        ntca::EncryptionResourceOptions*            options);

    static ntsa::Error decodeType(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Key>*         privateKey,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntctls::CertificateVector*            caList,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

    static ntsa::Error decodeKeyAsn1(ntctls::ErrorStack*           diagnostics,
                                     bsl::streambuf*               source,
                                     bsl::shared_ptr<ntctls::Key>* privateKey,
                                     ntca::EncryptionResourceOptions* options,
                                     bslma::Allocator* allocator);

    static ntsa::Error decodeKeyAsn1(ntctls::ErrorStack*       diagnostics,
                                     bsl::streambuf*           source,
                                     ntctls::Handle<EVP_PKEY>* privateKey,
                                     ntca::EncryptionResourceOptions* options,
                                     bslma::Allocator* allocator);

    static ntsa::Error decodeCertificateAsn1(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

    static ntsa::Error decodeCertificateAsn1(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        ntctls::Handle<X509>*            certificate,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeKeyAsn1Pem(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        bsl::shared_ptr<ntctls::Key>*    privateKey,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeKeyAsn1Pem(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        ntctls::Handle<EVP_PKEY>*        privateKey,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeCertificateAsn1Pem(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

    static ntsa::Error decodeCertificateAsn1Pem(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        ntctls::Handle<X509>*            certificate,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeKeyPkcs8(ntctls::ErrorStack* diagnostics,
                                      bsl::streambuf*     source,
                                      bsl::shared_ptr<ntctls::Key>* privateKey,
                                      ntca::EncryptionResourceOptions* options,
                                      bslma::Allocator* allocator);

    static ntsa::Error decodeKeyPkcs8(ntctls::ErrorStack*       diagnostics,
                                      bsl::streambuf*           source,
                                      ntctls::Handle<EVP_PKEY>* privateKey,
                                      ntca::EncryptionResourceOptions* options,
                                      bslma::Allocator* allocator);

    static ntsa::Error decodeKeyPkcs8E(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        ntctls::Handle<EVP_PKEY>*        privateKey,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeKeyPkcs8U(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        ntctls::Handle<EVP_PKEY>*        privateKey,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeCertificatePkcs8(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

    static ntsa::Error decodeKeyPkcs8Pem(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        bsl::shared_ptr<ntctls::Key>*    privateKey,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeKeyPkcs8Pem(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        ntctls::Handle<EVP_PKEY>*        privateKey,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeKeyPkcs8PemE(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        ntctls::Handle<EVP_PKEY>*        privateKey,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeKeyPkcs8PemU(
        ntctls::ErrorStack*              diagnostics,
        bsl::streambuf*                  source,
        ntctls::Handle<EVP_PKEY>*        privateKey,
        ntca::EncryptionResourceOptions* options,
        bslma::Allocator*                allocator);

    static ntsa::Error decodeCertificatePkcs8Pem(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

    static ntsa::Error decodeResourcePkcs12(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Key>*         privateKey,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntctls::CertificateVector*            caList,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

    static ntsa::Error decodeResourcePkcs7(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntctls::CertificateVector*            caList,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

    static ntsa::Error decodeResourcePkcs7Pem(
        ntctls::ErrorStack*                   diagnostics,
        bsl::streambuf*                       source,
        bsl::shared_ptr<ntctls::Certificate>* certificate,
        ntctls::CertificateVector*            caList,
        ntca::EncryptionResourceOptions*      options,
        bslma::Allocator*                     allocator);

  public:
    // Encode the specified 'privateKey', user 'certificate', and
    // 'caList' according to the specified 'options' to the
    // specified 'destination'.
    static ntsa::Error encode(
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntctls::Key>&         privateKey,
        const bsl::shared_ptr<ntctls::Certificate>& certificate,
        const ntctls::CertificateVector&            caList,
        const ntca::EncryptionResourceOptions&      options);

    // Decode the specified 'privateKey', user 'certificate', and
    // 'caList' according to the specified 'options' from the
    // specified 'source'. Return the error.
    static ntsa::Error decode(
        bsl::streambuf*                        source,
        bsl::shared_ptr<ntctls::Key>*          privateKey,
        bsl::shared_ptr<ntctls::Certificate>*  certificate,
        ntctls::CertificateVector*             caList,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'certificate'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error convertCertificate(
        bsl::shared_ptr<ntctls::Certificate>* result,
        const ntca::EncryptionCertificate&    certificate,
        bslma::Allocator*                     basicAllocator);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'certificate'. Return the error.
    static ntsa::Error convertCertificate(
        ntctls::Handle<X509>*              result,
        const ntca::EncryptionCertificate& certificate);

    /// Load into the specified 'result' the description of the specified
    /// 'certificate'. Return the error.
    static ntsa::Error convertCertificate(
        ntca::EncryptionCertificate*                result,
        const bsl::shared_ptr<ntctls::Certificate>& certificate);

    /// Load into the specified 'result' the description of the specified
    /// 'certificate'. Return the error.
    static ntsa::Error convertCertificate(
        ntca::EncryptionCertificate* result,
        const ntctls::Handle<X509>&  certificate);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'key'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error convertKey(bsl::shared_ptr<ntctls::Key>* result,
                                  const ntca::EncryptionKey&    key,
                                  bslma::Allocator* basicAllocator);

    /// Load into the specified 'result' the driver representation of the
    /// specified 'key'. Return the error.
    static ntsa::Error convertKey(ntctls::Handle<EVP_PKEY>*  result,
                                  const ntca::EncryptionKey& key);

    /// Load into the specified 'result' the description of the specified
    /// 'key'. Return the error.
    static ntsa::Error convertKey(ntca::EncryptionKey*                result,
                                  const bsl::shared_ptr<ntctls::Key>& key);

    /// Load into the specified 'result' the description of the specified
    /// 'key'. Return the error.
    static ntsa::Error convertKey(ntca::EncryptionKey*            result,
                                  const ntctls::Handle<EVP_PKEY>& key);

    // Resolve the secret contained in the specified 'options'. Return the
    // error. Note that when this function returns success then
    // 'options->secret().has_value()' is guaranteed
    static ntsa::Error resolveSecret(ntca::EncryptionResourceOptions* options);
};

extern "C" int ntctls_resource_password_cb(char* buffer,
                                           int   bufferCapacity,
                                           int   rw,
                                           void* userData)
{
    // Load into the specified 'buffer' having the specified 'bufferCapacity'
    // the passphrase indicated by the context represented by the specified
    // 'userData'. If the specified 'rw' flag is 1, the passphrase is being
    // used to write (i.e. create) a file, otherwise, the passphrase is being
    // used to read a file. On success, return the number of bytes written to
    // 'buffer' (not including the null-terminator), otherwise return -1.

    NTCCFG_WARNING_UNUSED(rw);

    ntsa::Error error;

    if (userData == 0) {
        NTCTLS_RESOURCE_LOG_SECRET_UNAVAILABLE();
        return -1;
    }

    ntca::EncryptionResourceOptions* options =
        (ntca::EncryptionResourceOptions*)(userData);

    error = ResourceUtil::resolveSecret(options);
    if (error) {
        return -1;
    }

    if (options->secret().isNull()) {
        NTCTLS_RESOURCE_LOG_SECRET_UNAVAILABLE();
        return -1;
    }

    bsl::memset(buffer, 0, (bsl::size_t)(bufferCapacity));

    return (
        int)(options->secret().value().copy(buffer,
                                            (bsl::size_t)(bufferCapacity)));
}

void ResourceUtil::chomp(bsl::streambuf* source)
{
    // OpenSSL does not robustly handle newlines or comments in concatenated
    // PEM-encoded certificates and keys.

    while (true) {
        Internal::Impl::StreamBufferPositionGuard guard(source);

        bsl::istream is(source);

        bsl::string line;
        bsl::getline(is, line);

        if (!is) {
            break;
        }

        bdlb::String::trim(&line);

        if (line.empty()) {
            guard.release();
            continue;
        }

        if (line[0] == '#') {
            guard.release();
            continue;
        }

        break;
    }
}

ntsa::Error ResourceUtil::detectType(
    bdlb::NullableValue<ntca::EncryptionResourceType::Value>* result,
    bsl::streambuf*                                           source)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(source);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ResourceUtil::encodeType(
    ntctls::ErrorStack*                         diagnostics,
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Key>&         privateKey,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    const ntctls::CertificateVector&            caList,
    ntca::EncryptionResourceOptions*            options)
{
    ntsa::Error error;

    BSLS_ASSERT(options);
    BSLS_ASSERT(!options->type().isNull());

    const ntca::EncryptionResourceType::Value type = options->type().value();

    if (type == ntca::EncryptionResourceType::e_PKCS12) {
        error = ResourceUtil::encodeResourcePkcs12(diagnostics,
                                                   destination,
                                                   privateKey,
                                                   certificate,
                                                   caList,
                                                   options);
        if (error) {
            return error;
        }
    }
    else if (type == ntca::EncryptionResourceType::e_PKCS7) {
        error = ResourceUtil::encodeResourcePkcs7(diagnostics,
                                                  destination,
                                                  privateKey,
                                                  certificate,
                                                  caList,
                                                  options);
        if (error) {
            return error;
        }
    }
    else if (type == ntca::EncryptionResourceType::e_PKCS7_PEM) {
        error = ResourceUtil::encodeResourcePkcs7Pem(diagnostics,
                                                     destination,
                                                     privateKey,
                                                     certificate,
                                                     caList,
                                                     options);
        if (error) {
            return error;
        }
    }
    else {
        EncodeKey         encodeKey         = 0;
        EncodeCertificate encodeCertificate = 0;

        if (type == ntca::EncryptionResourceType::e_ASN1) {
            encodeKey         = &ResourceUtil::encodeKeyAsn1;
            encodeCertificate = &ResourceUtil::encodeCertificateAsn1;
        }
        else if (type == ntca::EncryptionResourceType::e_ASN1_PEM) {
            encodeKey         = &ResourceUtil::encodeKeyAsn1Pem;
            encodeCertificate = &ResourceUtil::encodeCertificateAsn1Pem;
        }
        else if (type == ntca::EncryptionResourceType::e_PKCS8) {
            encodeKey         = &ResourceUtil::encodeKeyPkcs8;
            encodeCertificate = &ResourceUtil::encodeCertificatePkcs8;
        }
        else if (type == ntca::EncryptionResourceType::e_PKCS8_PEM) {
            encodeKey         = &ResourceUtil::encodeKeyPkcs8Pem;
            encodeCertificate = &ResourceUtil::encodeCertificatePkcs8Pem;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        BSLS_ASSERT(encodeKey);
        BSLS_ASSERT(encodeCertificate);

        if (privateKey) {
            error = encodeKey(diagnostics, destination, privateKey, options);
            if (error) {
                return error;
            }
        }

        if (certificate) {
            error = encodeCertificate(diagnostics,
                                      destination,
                                      certificate,
                                      options);
            if (error) {
                return error;
            }
        }

        for (ntctls::CertificateVector::const_iterator it = caList.begin();
             it != caList.end();
             ++it)
        {
            const bsl::shared_ptr<ntctls::Certificate>& certificateAuthority =
                *it;

            if (certificateAuthority) {
                error =
                    encodeCertificate(diagnostics, destination, *it, options);
                if (error) {
                    return error;
                }
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeKeyAsn1(
    ntctls::ErrorStack*                 diagnostics,
    bsl::streambuf*                     destination,
    const bsl::shared_ptr<ntctls::Key>& privateKey,
    ntca::EncryptionResourceOptions*    options)
{
    return ResourceUtil::encodeKeyAsn1(diagnostics,
                                       destination,
                                       privateKey->native(),
                                       options);
}

ntsa::Error ResourceUtil::encodeKeyAsn1(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  destination,
    EVP_PKEY*                        privateKey,
    ntca::EncryptionResourceOptions* options)
{
    NTCCFG_WARNING_UNUSED(options);

    int rc;

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    rc = i2d_PrivateKey_bio(bio.get(), privateKey);
    if (rc == 0) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeCertificateAsn1(
    ntctls::ErrorStack*                         diagnostics,
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    ntca::EncryptionResourceOptions*            options)
{
    return ResourceUtil::encodeCertificateAsn1(diagnostics,
                                               destination,
                                               certificate->native(),
                                               options);
}

ntsa::Error ResourceUtil::encodeCertificateAsn1(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  destination,
    X509*                            certificate,
    ntca::EncryptionResourceOptions* options)
{
    NTCCFG_WARNING_UNUSED(options);

    int rc;

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    rc = i2d_X509_bio(bio.get(), certificate);
    if (rc == 0) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeKeyAsn1Pem(
    ntctls::ErrorStack*                 diagnostics,
    bsl::streambuf*                     destination,
    const bsl::shared_ptr<ntctls::Key>& privateKey,
    ntca::EncryptionResourceOptions*    options)
{
    int rc;

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const bool encrypted = options->encrypted().value_or(false);

    if (encrypted) {
        const EVP_CIPHER* cipher = EVP_des_ede3_cbc();

        unsigned char* passphrase       = 0;
        int            passphraseLength = 0;

        pem_password_cb* passphraseCallback =
            (pem_password_cb*)(&ntctls_resource_password_cb);

        void* userData = options;

        rc = PEM_write_bio_PrivateKey(bio.get(),
                                      privateKey->native(),
                                      cipher,
                                      passphrase,
                                      passphraseLength,
                                      passphraseCallback,
                                      userData);
        if (rc == 0) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        rc = PEM_write_bio_PrivateKey(bio.get(),
                                      privateKey->native(),
                                      0,
                                      0,
                                      0,
                                      0,
                                      0);
        if (rc == 0) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeCertificateAsn1Pem(
    ntctls::ErrorStack*                         diagnostics,
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    ntca::EncryptionResourceOptions*            options)
{
    NTCCFG_WARNING_UNUSED(options);

    int rc;

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    rc = PEM_write_bio_X509(bio.get(), certificate->native());
    if (rc == 0) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeKeyPkcs8(
    ntctls::ErrorStack*                 diagnostics,
    bsl::streambuf*                     destination,
    const bsl::shared_ptr<ntctls::Key>& privateKey,
    ntca::EncryptionResourceOptions*    options)
{
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;
    int         rc;

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const bool encrypted = options->encrypted().value_or(false);

    if (encrypted) {
        error = ResourceUtil::resolveSecret(options);
        if (error) {
            return error;
        }

        ntctls::Handle<PKCS8_PRIV_KEY_INFO> pkcs8PrivKeyInfo(
            EVP_PKEY2PKCS8(privateKey->native()));
        if (!pkcs8PrivKeyInfo) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        const EVP_CIPHER* cipher = EVP_aes_256_cbc();

        ntctls::Handle<X509_ALGOR> pbe(
            PKCS5_pbe2_set_iv(cipher, 1024, NULL, 0, NULL, -1));
        if (!pbe) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntctls::Handle<X509_SIG> pkcs8(
            PKCS8_set0_pbe((const char*)(options->secret().value().data()),
                           (int)(options->secret().value().size()),
                           pkcs8PrivKeyInfo.get(),
                           pbe.get()));
        if (!pkcs8) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        pbe.release();

        rc = i2d_PKCS8_bio(bio.get(), pkcs8.get());
        if (rc == 0) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        ntctls::Handle<PKCS8_PRIV_KEY_INFO> pkcs8PrivKeyInfo(
            EVP_PKEY2PKCS8(privateKey->native()));
        if (!pkcs8PrivKeyInfo) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        rc = i2d_PKCS8_PRIV_KEY_INFO_bio(bio.get(), pkcs8PrivKeyInfo.get());
        if (rc == 0) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeCertificatePkcs8(
    ntctls::ErrorStack*                         diagnostics,
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    ntca::EncryptionResourceOptions*            options)
{
    // Certificates cannot be encoded into PKCS8, it is a key format only.

    NTCCFG_WARNING_UNUSED(diagnostics);
    NTCCFG_WARNING_UNUSED(destination);
    NTCCFG_WARNING_UNUSED(certificate);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error ResourceUtil::encodeKeyPkcs8Pem(
    ntctls::ErrorStack*                 diagnostics,
    bsl::streambuf*                     destination,
    const bsl::shared_ptr<ntctls::Key>& privateKey,
    ntca::EncryptionResourceOptions*    options)
{
    NTCCFG_WARNING_UNUSED(options);

    int rc;

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const bool encrypted = options->encrypted().value_or(false);

    if (encrypted) {
        char*     passphrase       = 0;
        const int passphraseLength = 0;

        const EVP_CIPHER* cipher = EVP_des_ede3_cbc();

        pem_password_cb* passphraseCallback =
            (pem_password_cb*)(&ntctls_resource_password_cb);

        void* userData = options;

        rc = PEM_write_bio_PKCS8PrivateKey(bio.get(),
                                           privateKey->native(),
                                           cipher,
                                           passphrase,
                                           passphraseLength,
                                           passphraseCallback,
                                           userData);
        if (rc == 0) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        ntctls::Handle<PKCS8_PRIV_KEY_INFO> pkcs8PrivKeyInfo(
            EVP_PKEY2PKCS8(privateKey->native()));
        if (!pkcs8PrivKeyInfo) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        rc = PEM_write_bio_PKCS8_PRIV_KEY_INFO(bio.get(),
                                               pkcs8PrivKeyInfo.get());
        if (rc == 0) {
            ntctls::Internal::drainErrorQueue(diagnostics);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeCertificatePkcs8Pem(
    ntctls::ErrorStack*                         diagnostics,
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    ntca::EncryptionResourceOptions*            options)
{
    // Certificates cannot be encoded into PKCS8, it is a key format only.

    NTCCFG_WARNING_UNUSED(diagnostics);
    NTCCFG_WARNING_UNUSED(destination);
    NTCCFG_WARNING_UNUSED(certificate);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error ResourceUtil::encodeResourcePkcs12(
    ntctls::ErrorStack*                         diagnostics,
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Key>&         privateKey,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    const ntctls::CertificateVector&            caList,
    ntca::EncryptionResourceOptions*            options)
{
    ntsa::Error error;
    int         rc;

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    const bool encrypted = options->encrypted().value_or(false);

    bsl::string passphraseStorage;

    if (encrypted) {
        error = ResourceUtil::resolveSecret(options);
        if (error) {
            return error;
        }

        passphraseStorage.resize(options->secret().value().size());
        options->secret().value().copy(passphraseStorage.data(),
                                       passphraseStorage.size());
    }

    const char* passphrase = 0;
    if (!passphraseStorage.empty()) {
        passphrase = passphraseStorage.c_str();
    }

    bsl::string friendlyName;
    if (!options->label().isNull()) {
        friendlyName = options->label().value();
    }

    EVP_PKEY* pkey         = 0;
    X509*     x509         = 0;
    STACK_OF(X509)* x509CA = 0;

    if (privateKey) {
        pkey = privateKey->native();
    }

    if (certificate) {
        x509 = certificate->native();
    }

    if (!caList.empty()) {
        x509CA = sk_X509_new_null();

        for (ntctls::CertificateVector::const_iterator it = caList.begin();
             it != caList.end();
             ++it)
        {
            sk_X509_push(x509CA, (*it)->native());
        }
    }

    ntctls::Handle<PKCS12> pkcs12(PKCS12_create(passphrase,
                                                friendlyName.c_str(),
                                                pkey,
                                                x509,
                                                x509CA,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0));

    if (!passphraseStorage.empty()) {
        OPENSSL_cleanse(passphraseStorage.data(), passphraseStorage.size());
    }

    if (!pkcs12) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = i2d_PKCS12_bio(bio.get(), pkcs12.get());
    if (rc == 0) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeResourcePkcs7(
    ntctls::ErrorStack*                         diagnostics,
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Key>&         privateKey,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    const ntctls::CertificateVector&            caList,
    ntca::EncryptionResourceOptions*            options)
{
    NTCCFG_WARNING_UNUSED(options);

    int rc;

    if (privateKey) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    STACK_OF(X509)* x509Stack = sk_X509_new_null();

    if (certificate) {
        sk_X509_push(x509Stack, certificate->native());
    }

    if (!caList.empty()) {
        for (ntctls::CertificateVector::const_iterator it = caList.begin();
             it != caList.end();
             ++it)
        {
            sk_X509_push(x509Stack, (*it)->native());
        }
    }

    ntctls::Handle<PKCS7> pkcs7(PKCS7_sign(0, 0, x509Stack, 0, 0));
    sk_X509_free(x509Stack);

    if (!pkcs7) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = i2d_PKCS7_bio(bio.get(), pkcs7.get());
    if (rc == 0) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encodeResourcePkcs7Pem(
    ntctls::ErrorStack*                         diagnostics,
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Key>&         privateKey,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    const ntctls::CertificateVector&            caList,
    ntca::EncryptionResourceOptions*            options)
{
    NTCCFG_WARNING_UNUSED(options);

    int rc;

    if (privateKey) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    bsl::shared_ptr<BIO> bio = Internal::createStream(destination);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    STACK_OF(X509)* x509Stack = sk_X509_new_null();

    if (certificate) {
        sk_X509_push(x509Stack, certificate->native());
    }

    if (!caList.empty()) {
        for (ntctls::CertificateVector::const_iterator it = caList.begin();
             it != caList.end();
             ++it)
        {
            sk_X509_push(x509Stack, (*it)->native());
        }
    }

    ntctls::Handle<PKCS7> pkcs7(PKCS7_sign(0, 0, x509Stack, 0, 0));
    sk_X509_free(x509Stack);

    if (!pkcs7) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = PEM_write_bio_PKCS7(bio.get(), pkcs7.get());
    if (rc == 0) {
        ntctls::Internal::drainErrorQueue(diagnostics);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeType(
    ntctls::ErrorStack*                   diagnostics,
    bsl::streambuf*                       source,
    bsl::shared_ptr<ntctls::Key>*         privateKey,
    bsl::shared_ptr<ntctls::Certificate>* certificate,
    ntctls::CertificateVector*            caList,
    ntca::EncryptionResourceOptions*      options,
    bslma::Allocator*                     allocator)
{
    ntsa::Error error;

    BSLS_ASSERT(diagnostics);
    BSLS_ASSERT(source);
    BSLS_ASSERT(certificate);
    BSLS_ASSERT(caList);
    BSLS_ASSERT(options);
    BSLS_ASSERT(allocator);

    BSLS_ASSERT(!options->type().isNull());

    privateKey->reset();
    certificate->reset();
    caList->clear();

    const ntca::EncryptionResourceType::Value type = options->type().value();

    if (type == ntca::EncryptionResourceType::e_PKCS12) {
        return ResourceUtil::decodeResourcePkcs12(diagnostics,
                                                  source,
                                                  privateKey,
                                                  certificate,
                                                  caList,
                                                  options,
                                                  allocator);
    }
    else if (type == ntca::EncryptionResourceType::e_PKCS7) {
        return ResourceUtil::decodeResourcePkcs7(diagnostics,
                                                 source,
                                                 certificate,
                                                 caList,
                                                 options,
                                                 allocator);
    }
    else if (type == ntca::EncryptionResourceType::e_PKCS7_PEM) {
        return ResourceUtil::decodeResourcePkcs7Pem(diagnostics,
                                                    source,
                                                    certificate,
                                                    caList,
                                                    options,
                                                    allocator);
    }

    DecodeKey         decodeKey         = 0;
    DecodeCertificate decodeCertificate = 0;

    if (type == ntca::EncryptionResourceType::e_ASN1) {
        decodeKey         = &ResourceUtil::decodeKeyAsn1;
        decodeCertificate = &ResourceUtil::decodeCertificateAsn1;
    }
    else if (type == ntca::EncryptionResourceType::e_ASN1_PEM) {
        decodeKey         = &ResourceUtil::decodeKeyAsn1Pem;
        decodeCertificate = &ResourceUtil::decodeCertificateAsn1Pem;
    }
    else if (type == ntca::EncryptionResourceType::e_PKCS8) {
        decodeKey         = &ResourceUtil::decodeKeyPkcs8;
        decodeCertificate = &ResourceUtil::decodeCertificatePkcs8;
    }
    else if (type == ntca::EncryptionResourceType::e_PKCS8_PEM) {
        decodeKey         = &ResourceUtil::decodeKeyPkcs8Pem;
        decodeCertificate = &ResourceUtil::decodeCertificatePkcs8Pem;
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    BSLS_ASSERT(decodeKey);
    BSLS_ASSERT(decodeCertificate);

    ntsa::Error decodeKeyError;
    ntsa::Error decodeCertificateError;
    ntsa::Error decodeCertificateAuthorityError;

    bsl::streampos start =
        source->pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);

    if (type != ntca::EncryptionResourceType::e_PKCS7 &&
        type != ntca::EncryptionResourceType::e_PKCS7_PEM)
    {
        decodeKeyError =
            decodeKey(diagnostics, source, privateKey, options, allocator);

        if (decodeKeyError == ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED)) {
            return decodeKeyError;
        }
    }

    if (type == ntca::EncryptionResourceType::e_ASN1_PEM ||
        type == ntca::EncryptionResourceType::e_PKCS7_PEM ||
        type == ntca::EncryptionResourceType::e_PKCS8_PEM)
    {
        bsl::streampos rewoundPosition =
            source->pubseekpos(start, bsl::ios_base::in);

        if (rewoundPosition != start) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (type != ntca::EncryptionResourceType::e_PKCS8 &&
        type != ntca::EncryptionResourceType::e_PKCS8_PEM)
    {
        decodeCertificateError = decodeCertificate(diagnostics,
                                                   source,
                                                   certificate,
                                                   options,
                                                   allocator);

        while (true) {
            bsl::shared_ptr<ntctls::Certificate> ca;
            decodeCertificateAuthorityError = decodeCertificate(diagnostics,
                                                                source,
                                                                &ca,
                                                                options,
                                                                allocator);
            if (decodeCertificateAuthorityError) {
                break;
            }

            caList->push_back(ca);
        }
    }

    if (!error && decodeKeyError) {
        error = decodeKeyError;
    }

    if (!error && decodeCertificateError) {
        error = decodeCertificateError;
    }

    if (!error && decodeCertificateAuthorityError) {
        error = decodeCertificateAuthorityError;
    }

    if (error) {
        if (!*privateKey && !*certificate && caList->empty()) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyAsn1(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    bsl::shared_ptr<ntctls::Key>*    privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    ntsa::Error error;

    privateKey->reset();

    ntctls::Handle<EVP_PKEY> pkey;
    error = ResourceUtil::decodeKeyAsn1(diagnostics,
                                        source,
                                        &pkey,
                                        options,
                                        allocator);
    if (error) {
        return error;
    }

    privateKey->createInplace(allocator, pkey.release(), allocator);
    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyAsn1(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<EVP_PKEY>*        privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(allocator);

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    ntctls::Handle<EVP_PKEY> pkey(d2i_PrivateKey_bio(bio.get(), 0));
    if (!pkey) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_ASN1, ASN1_R_NOT_ENOUGH_DATA)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    privateKey->reset(pkey.release());
    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeCertificateAsn1(
    ntctls::ErrorStack*                   diagnostics,
    bsl::streambuf*                       source,
    bsl::shared_ptr<ntctls::Certificate>* certificate,
    ntca::EncryptionResourceOptions*      options,
    bslma::Allocator*                     allocator)
{
    ntsa::Error error;

    certificate->reset();

    ntctls::Handle<X509> x509;
    error = ResourceUtil::decodeCertificateAsn1(diagnostics,
                                                source,
                                                &x509,
                                                options,
                                                allocator);
    if (error) {
        return error;
    }

    certificate->createInplace(allocator, x509.release(), allocator);
    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeCertificateAsn1(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<X509>*            certificate,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(allocator);

    certificate->reset();

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    ntctls::Handle<X509> x509(d2i_X509_bio(bio.get(), 0));
    if (!x509) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_ASN1, ASN1_R_NOT_ENOUGH_DATA)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    certificate->reset(x509.release());
    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyAsn1Pem(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    bsl::shared_ptr<ntctls::Key>*    privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    ntsa::Error error;

    privateKey->reset();

    ntctls::Handle<EVP_PKEY> pkey;
    error = ResourceUtil::decodeKeyAsn1Pem(diagnostics,
                                           source,
                                           &pkey,
                                           options,
                                           allocator);
    if (error) {
        return error;
    }

    privateKey->createInplace(allocator, pkey.release(), allocator);
    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyAsn1Pem(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<EVP_PKEY>*        privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    NTCCFG_WARNING_UNUSED(allocator);

    privateKey->reset();

    ResourceUtil::chomp(source);

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    pem_password_cb* passphraseCallback =
        (pem_password_cb*)(&ntctls_resource_password_cb);

    void* userData = options;

    ntctls::Handle<EVP_PKEY> pkey(
        PEM_read_bio_PrivateKey(bio.get(), 0, passphraseCallback, userData));
    if (!pkey) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_PEM, PEM_R_NO_START_LINE)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else if (currentErrorStack.find(ERR_LIB_PEM, PEM_R_BAD_PASSWORD_READ))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        // error:1C80009F:Provider routes::unable to get passphrase
        else if (currentErrorStack.find(57, 159))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
#endif
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    privateKey->reset(pkey.release());
    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeCertificateAsn1Pem(
    ntctls::ErrorStack*                   diagnostics,
    bsl::streambuf*                       source,
    bsl::shared_ptr<ntctls::Certificate>* certificate,
    ntca::EncryptionResourceOptions*      options,
    bslma::Allocator*                     allocator)
{
    ntsa::Error error;

    certificate->reset();

    ntctls::Handle<X509> x509;
    error = ResourceUtil::decodeCertificateAsn1Pem(diagnostics,
                                                   source,
                                                   &x509,
                                                   options,
                                                   allocator);
    if (error) {
        return error;
    }

    certificate->createInplace(allocator, x509.release(), allocator);
    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeCertificateAsn1Pem(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<X509>*            certificate,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    NTCCFG_WARNING_UNUSED(allocator);

    certificate->reset();

    ResourceUtil::chomp(source);

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    pem_password_cb* passphraseCallback =
        (pem_password_cb*)(&ntctls_resource_password_cb);

    void* userData = options;

    ntctls::Handle<X509> x509(
        PEM_read_bio_X509(bio.get(), 0, passphraseCallback, userData));
    if (!x509) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_PEM, PEM_R_NO_START_LINE)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else if (currentErrorStack.find(ERR_LIB_PEM, PEM_R_BAD_PASSWORD_READ))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        // error:1C80009F:Provider routes::unable to get passphrase
        else if (currentErrorStack.find(57, 159))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
#endif
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    certificate->reset(x509.release());
    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyPkcs8(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    bsl::shared_ptr<ntctls::Key>*    privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    ntsa::Error error;

    privateKey->reset();

    ntctls::Handle<EVP_PKEY> pkey;
    error = ResourceUtil::decodeKeyPkcs8(diagnostics,
                                         source,
                                         &pkey,
                                         options,
                                         allocator);
    if (error) {
        return error;
    }

    privateKey->createInplace(allocator, pkey.release(), allocator);
    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyPkcs8(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<EVP_PKEY>*        privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    ntsa::Error error;

    privateKey->reset();

    if (!*privateKey) {
        error = ResourceUtil::decodeKeyPkcs8U(diagnostics,
                                              source,
                                              privateKey,
                                              options,
                                              allocator);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (!*privateKey) {
        error = ResourceUtil::decodeKeyPkcs8E(diagnostics,
                                              source,
                                              privateKey,
                                              options,
                                              allocator);
        if (!error) {
            return ntsa::Error();
        }
    }

    return error;
}

ntsa::Error ResourceUtil::decodeKeyPkcs8E(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<EVP_PKEY>*        privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    NTCCFG_WARNING_UNUSED(allocator);

    ntsa::Error error;

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    ntctls::Handle<X509_SIG> pkcs8(d2i_PKCS8_bio(bio.get(), 0));
    if (!pkcs8) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_ASN1, ASN1_R_NOT_ENOUGH_DATA)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    error = ResourceUtil::resolveSecret(options);
    if (error) {
        return error;
    }

    const char* passphrase = (const char*)(options->secret().value().data());

    const int passphraseLength = (int)(options->secret().value().size());

    ntctls::Handle<PKCS8_PRIV_KEY_INFO> pkcs8PrivKeyInfo(
        PKCS8_decrypt(pkcs8.get(), passphrase, passphraseLength));
    if (!pkcs8PrivKeyInfo) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_PKCS12,
                                   PKCS12_R_PKCS12_PBE_CRYPT_ERROR))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntctls::Handle<EVP_PKEY> pkey(EVP_PKCS82PKEY(pkcs8PrivKeyInfo.get()));
    if (!pkey) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    privateKey->reset(pkey.release());
    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyPkcs8U(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<EVP_PKEY>*        privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(allocator);

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    ntctls::Handle<PKCS8_PRIV_KEY_INFO> pkcs8PrivKeyInfo(
        d2i_PKCS8_PRIV_KEY_INFO_bio(bio.get(), 0));
    if (!pkcs8PrivKeyInfo) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_ASN1, ASN1_R_NOT_ENOUGH_DATA)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntctls::Handle<EVP_PKEY> pkey(EVP_PKCS82PKEY(pkcs8PrivKeyInfo.get()));
    if (!pkey) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    privateKey->reset(pkey.release());
    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeCertificatePkcs8(
    ntctls::ErrorStack*                   diagnostics,
    bsl::streambuf*                       source,
    bsl::shared_ptr<ntctls::Certificate>* certificate,
    ntca::EncryptionResourceOptions*      options,
    bslma::Allocator*                     allocator)
{
    // Certificates cannot be encoded from PKCS8, it is a key format only.

    NTCCFG_WARNING_UNUSED(diagnostics);
    NTCCFG_WARNING_UNUSED(source);
    NTCCFG_WARNING_UNUSED(certificate);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(allocator);

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error ResourceUtil::decodeKeyPkcs8Pem(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    bsl::shared_ptr<ntctls::Key>*    privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    ntsa::Error error;

    privateKey->reset();

    ntctls::Handle<EVP_PKEY> pkey;
    error = ResourceUtil::decodeKeyPkcs8Pem(diagnostics,
                                            source,
                                            &pkey,
                                            options,
                                            allocator);
    if (error) {
        return error;
    }

    privateKey->createInplace(allocator, pkey.release(), allocator);
    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyPkcs8Pem(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<EVP_PKEY>*        privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    ntsa::Error error;

    privateKey->reset();

    if (!*privateKey) {
        error = ResourceUtil::decodeKeyPkcs8PemU(diagnostics,
                                                 source,
                                                 privateKey,
                                                 options,
                                                 allocator);
        if (!error) {
            return ntsa::Error();
        }
    }

    if (!*privateKey) {
        error = ResourceUtil::decodeKeyPkcs8PemE(diagnostics,
                                                 source,
                                                 privateKey,
                                                 options,
                                                 allocator);
        if (!error) {
            return ntsa::Error();
        }
    }

    return error;
}

ntsa::Error ResourceUtil::decodeKeyPkcs8PemE(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<EVP_PKEY>*        privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    NTCCFG_WARNING_UNUSED(allocator);

    ntsa::Error error;

    privateKey->reset();

    ResourceUtil::chomp(source);

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    pem_password_cb* passphraseCallback =
        (pem_password_cb*)(&ntctls_resource_password_cb);

    void* userData = options;

    ntctls::Handle<X509_SIG> pkcs8(
        PEM_read_bio_PKCS8(bio.get(), 0, passphraseCallback, userData));
    if (!pkcs8) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_PEM, PEM_R_NO_START_LINE)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else if (currentErrorStack.find(ERR_LIB_PEM, PEM_R_BAD_PASSWORD_READ))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        // error:1C80009F:Provider routes::unable to get passphrase
        else if (currentErrorStack.find(57, 159))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
#endif
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    error = ResourceUtil::resolveSecret(options);
    if (error) {
        return error;
    }

    const char* passphrase = (const char*)(options->secret().value().data());

    const int passphraseLength = (int)(options->secret().value().size());

    ntctls::Handle<PKCS8_PRIV_KEY_INFO> pkcs8PrivKeyInfo(
        PKCS8_decrypt(pkcs8.get(), passphrase, passphraseLength));
    if (!pkcs8PrivKeyInfo) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_PKCS12,
                                   PKCS12_R_PKCS12_PBE_CRYPT_ERROR))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntctls::Handle<EVP_PKEY> pkey(EVP_PKCS82PKEY(pkcs8PrivKeyInfo.get()));
    if (!pkey) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    privateKey->reset(pkey.release());
    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeKeyPkcs8PemU(
    ntctls::ErrorStack*              diagnostics,
    bsl::streambuf*                  source,
    ntctls::Handle<EVP_PKEY>*        privateKey,
    ntca::EncryptionResourceOptions* options,
    bslma::Allocator*                allocator)
{
    NTCCFG_WARNING_UNUSED(allocator);

    privateKey->reset();

    ResourceUtil::chomp(source);

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    pem_password_cb* passphraseCallback =
        (pem_password_cb*)(&ntctls_resource_password_cb);

    void* userData = options;

    ntctls::Handle<PKCS8_PRIV_KEY_INFO> pkcs8PrivKeyInfo(
        PEM_read_bio_PKCS8_PRIV_KEY_INFO(bio.get(),
                                         0,
                                         passphraseCallback,
                                         userData));
    if (!pkcs8PrivKeyInfo) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_PEM, PEM_R_NO_START_LINE)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else if (currentErrorStack.find(ERR_LIB_PEM, PEM_R_BAD_PASSWORD_READ))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        // error:1C80009F:Provider routes::unable to get passphrase
        else if (currentErrorStack.find(57, 159))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
#endif
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntctls::Handle<EVP_PKEY> pkey(EVP_PKCS82PKEY(pkcs8PrivKeyInfo.get()));
    if (!pkey) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    privateKey->reset(pkey.release());
    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeCertificatePkcs8Pem(
    ntctls::ErrorStack*                   diagnostics,
    bsl::streambuf*                       source,
    bsl::shared_ptr<ntctls::Certificate>* certificate,
    ntca::EncryptionResourceOptions*      options,
    bslma::Allocator*                     allocator)
{
    // Certificates cannot be encoded from PKCS8, it is a key format only.

    NTCCFG_WARNING_UNUSED(diagnostics);
    NTCCFG_WARNING_UNUSED(source);
    NTCCFG_WARNING_UNUSED(certificate);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(allocator);

    return ntsa::Error(ntsa::Error::e_INVALID);
}

ntsa::Error ResourceUtil::decodeResourcePkcs12(
    ntctls::ErrorStack*                   diagnostics,
    bsl::streambuf*                       source,
    bsl::shared_ptr<ntctls::Key>*         privateKey,
    bsl::shared_ptr<ntctls::Certificate>* certificate,
    ntctls::CertificateVector*            caList,
    ntca::EncryptionResourceOptions*      options,
    bslma::Allocator*                     allocator)
{
    ntsa::Error error;
    int         rc;

    privateKey->reset();
    certificate->reset();
    caList->clear();

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    ntctls::Handle<PKCS12> pkcs12(d2i_PKCS12_bio(bio.get(), 0));
    if (!pkcs12) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_ASN1, ASN1_R_NOT_ENOUGH_DATA)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bdlb::NullableValue<ntca::EncryptionSecret> secret;

    if (secret.isNull()) {
        rc = PKCS12_verify_mac(pkcs12.get(), "", 0);
        if (rc != 0) {
            secret.makeValue();
        }
    }

    if (secret.isNull()) {
        rc = PKCS12_verify_mac(pkcs12.get(), 0, 0);
        if (rc != 0) {
            secret.makeValue();
        }
    }

    if (secret.isNull()) {
        error = ResourceUtil::resolveSecret(options);
        if (error) {
            return error;
        }

        const char* passphraseLiteral =
            (const char*)(options->secret().value().data());

        const int passphraseLength = (int)(options->secret().value().size());

        rc = PKCS12_verify_mac(pkcs12.get(),
                               passphraseLiteral,
                               passphraseLength);
        if (rc != 0) {
            secret.makeValue(options->secret().value());
        }
    }

    if (secret.isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const char* passphrase = 0;
    bsl::string passphraseStorage;

    if (!secret.value().empty()) {
        passphraseStorage.resize(secret.value().size() + 1);
        secret.value().copy(passphraseStorage.data(),
                            passphraseStorage.size());

        passphrase = passphraseStorage.c_str();
    }

    EVP_PKEY* pkey              = 0;
    X509*     x509              = 0;
    STACK_OF(X509)* x509CAStack = 0;

    rc = PKCS12_parse(pkcs12.get(), passphrase, &pkey, &x509, &x509CAStack);

    if (!passphraseStorage.empty()) {
        OPENSSL_cleanse(&passphraseStorage.front(), passphraseStorage.size());
    }

    if (rc != 1) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_PKCS12,
                                   PKCS12_R_PKCS12_PBE_CRYPT_ERROR))
        {
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (pkey) {
        privateKey->createInplace(allocator, pkey, allocator);
    }

    if (x509) {
        certificate->createInplace(allocator, x509, allocator);
    }

    if (x509CAStack) {
        int x509CAStackSize = sk_X509_num(x509CAStack);
        for (int i = 0; i < x509CAStackSize; ++i) {
            X509* x509CA = sk_X509_pop(x509CAStack);

            bsl::shared_ptr<ntctls::Certificate> ca;
            ca.createInplace(allocator, x509CA, allocator);

            if (ca->isAuthority()) {
                caList->push_back(ca);
            }
            else if (!*certificate) {
                *certificate = ca;
            }
            else {
                BSLS_LOG_ERROR(
                    "PKCS12 contains more than one non-CA certificate");
                sk_X509_free(x509CAStack);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    sk_X509_free(x509CAStack);

    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeResourcePkcs7(
    ntctls::ErrorStack*                   diagnostics,
    bsl::streambuf*                       source,
    bsl::shared_ptr<ntctls::Certificate>* certificate,
    ntctls::CertificateVector*            caList,
    ntca::EncryptionResourceOptions*      options,
    bslma::Allocator*                     allocator)
{
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    certificate->reset();
    caList->clear();

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    ntctls::Handle<PKCS7> pkcs7(d2i_PKCS7_bio(bio.get(), 0));
    if (!pkcs7) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_ASN1, ASN1_R_NOT_ENOUGH_DATA)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    STACK_OF(X509)* x509Stack = 0;

    const int type = OBJ_obj2nid(pkcs7.get()->type);
    if (type == NID_pkcs7_signed) {
        if (pkcs7.get()->d.sign != 0) {
            x509Stack = pkcs7.get()->d.sign->cert;
        }
        else {
            BSLS_LOG_ERROR("Missing PKCS7 signed certificate");
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (type == NID_pkcs7_signedAndEnveloped) {
        if (pkcs7.get()->d.signed_and_enveloped != 0) {
            x509Stack = pkcs7.get()->d.signed_and_enveloped->cert;
        }
        else {
            BSLS_LOG_ERROR("Missing PKCS7 signed-and-enveloped certificate");
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        BSLS_LOG_ERROR("Uknown PKCS7 type NID %d", type);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (x509Stack != 0) {
        const int x509StackSize = sk_X509_num(x509Stack);
        for (int i = 0; i < x509StackSize; ++i) {
            X509* x509 = sk_X509_pop(x509Stack);

            bsl::shared_ptr<ntctls::Certificate> object;
            object.createInplace(allocator, x509, allocator);

            if (object->isAuthority()) {
                caList->push_back(object);
            }
            else if (!*certificate) {
                *certificate = object;
            }
            else {
                BSLS_LOG_ERROR(
                    "PKCS7 contains more than one non-CA certificate");
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decodeResourcePkcs7Pem(
    ntctls::ErrorStack*                   diagnostics,
    bsl::streambuf*                       source,
    bsl::shared_ptr<ntctls::Certificate>* certificate,
    ntctls::CertificateVector*            caList,
    ntca::EncryptionResourceOptions*      options,
    bslma::Allocator*                     allocator)
{
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    certificate->reset();
    caList->clear();

    Internal::Impl::StreamBufferPositionGuard guard(source);

    bsl::shared_ptr<BIO> bio = Internal::createStream(source);
    if (!bio) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    pem_password_cb* passphraseCallback =
        (pem_password_cb*)(&ntctls_resource_password_cb);

    void* userData = options;

    ntctls::Handle<PKCS7> pkcs7(
        PEM_read_bio_PKCS7(bio.get(), 0, passphraseCallback, userData));
    if (!pkcs7) {
        ntctls::ErrorStack currentErrorStack;
        ntctls::Internal::drainErrorQueue(&currentErrorStack);

        NTCTLS_RESOURCE_LOG_ERROR_STACK(currentErrorStack);
        diagnostics->push(currentErrorStack);

        if (currentErrorStack.find(ERR_LIB_ASN1, ASN1_R_NOT_ENOUGH_DATA)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    STACK_OF(X509)* x509Stack = 0;

    const int type = OBJ_obj2nid(pkcs7.get()->type);
    if (type == NID_pkcs7_signed) {
        if (pkcs7.get()->d.sign != 0) {
            x509Stack = pkcs7.get()->d.sign->cert;
        }
        else {
            BSLS_LOG_ERROR("Missing PKCS7 signed certificate");
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (type == NID_pkcs7_signedAndEnveloped) {
        if (pkcs7.get()->d.signed_and_enveloped != 0) {
            x509Stack = pkcs7.get()->d.signed_and_enveloped->cert;
        }
        else {
            BSLS_LOG_ERROR("Missing PKCS7 signed-and-enveloped certificate");
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        BSLS_LOG_ERROR("Uknown PKCS7 type NID %d", type);
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (x509Stack != 0) {
        const int x509StackSize = sk_X509_num(x509Stack);
        for (int i = 0; i < x509StackSize; ++i) {
            X509* x509 = sk_X509_pop(x509Stack);

            bsl::shared_ptr<ntctls::Certificate> object;
            object.createInplace(allocator, x509, allocator);

            if (object->isAuthority()) {
                caList->push_back(object);
            }
            else if (!*certificate) {
                *certificate = object;
            }
            else {
                BSLS_LOG_ERROR(
                    "PKCS7 contains more than one non-CA certificate");
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    guard.release();

    return ntsa::Error();
}

ntsa::Error ResourceUtil::encode(
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntctls::Key>&         privateKey,
    const bsl::shared_ptr<ntctls::Certificate>& certificate,
    const ntctls::CertificateVector&            caList,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error        error;
    ntctls::ErrorStack diagnostics;

    ntca::EncryptionResourceOptions effectiveOptions = options;
    if (effectiveOptions.type().isNull()) {
        effectiveOptions.setType(ntca::EncryptionResourceType::e_ASN1_PEM);
    }

    error = ResourceUtil::encodeType(&diagnostics,
                                     destination,
                                     privateKey,
                                     certificate,
                                     caList,
                                     &effectiveOptions);
    if (error) {
        NTCTLS_RESOURCE_LOG_ENCODER_ERROR(diagnostics);
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::decode(
    bsl::streambuf*                        source,
    bsl::shared_ptr<ntctls::Key>*          privateKey,
    bsl::shared_ptr<ntctls::Certificate>*  certificate,
    ntctls::CertificateVector*             caList,
    const ntca::EncryptionResourceOptions& options,
    bslma::Allocator*                      basicAllocator)
{
    ntsa::Error        error;
    ntctls::ErrorStack diagnostics;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntca::EncryptionResourceOptions effectiveOptions = options;
    if (effectiveOptions.type().isNull()) {
        bdlb::NullableValue<ntca::EncryptionResourceType::Value> type;
        error = ResourceUtil::detectType(&type, source);
        if (!error && !type.isNull()) {
            effectiveOptions.setType(type.value());
        }
    }

    bsl::shared_ptr<ntctls::Key>         effectivePrivateKey;
    bsl::shared_ptr<ntctls::Certificate> effectiveCertificate;
    ntctls::CertificateVector            effectiveCaList;

    if (!effectiveOptions.type().isNull()) {
        BSLS_LOG_TRACE("Decoding explicit type %s",
                       ntca::EncryptionResourceType::toString(
                           effectiveOptions.type().value()));

        error = ResourceUtil::decodeType(&diagnostics,
                                         source,
                                         &effectivePrivateKey,
                                         &effectiveCertificate,
                                         &effectiveCaList,
                                         &effectiveOptions,
                                         allocator);
        if (error) {
            NTCTLS_RESOURCE_LOG_DECODER_ERROR(diagnostics);
            return error;
        }
    }
    else {
        bsl::vector<ntca::EncryptionResourceType::Value> types;
        types.push_back(ntca::EncryptionResourceType::e_ASN1_PEM);
        types.push_back(ntca::EncryptionResourceType::e_ASN1);
        types.push_back(ntca::EncryptionResourceType::e_PKCS12);
        types.push_back(ntca::EncryptionResourceType::e_PKCS8_PEM);
        types.push_back(ntca::EncryptionResourceType::e_PKCS8);

        for (bsl::size_t i = 0; i < types.size(); ++i) {
            BSLS_LOG_TRACE("Decoding guessed type %s",
                           ntca::EncryptionResourceType::toString(types[i]));

            effectiveOptions.setType(types[i]);

            effectivePrivateKey.reset();
            effectiveCertificate.reset();
            effectiveCaList.clear();

            error = ResourceUtil::decodeType(&diagnostics,
                                             source,
                                             &effectivePrivateKey,
                                             &effectiveCertificate,
                                             &effectiveCaList,
                                             &effectiveOptions,
                                             allocator);
            if (!error) {
                break;
            }
        }

        if (error) {
            NTCTLS_RESOURCE_LOG_DECODER_ERROR(diagnostics);
            return error;
        }
    }

    if (privateKey) {
        if (effectivePrivateKey) {
            *privateKey = effectivePrivateKey;
        }
    }

    if (certificate) {
        if (effectiveCertificate) {
            *certificate = effectiveCertificate;
        }
    }

    if (caList) {
        if (effectiveCaList.size() > 0) {
            caList->insert(caList->end(),
                           effectiveCaList.begin(),
                           effectiveCaList.end());
        }
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::resolveSecret(
    ntca::EncryptionResourceOptions* options)
{
    ntsa::Error error;

    if (options->secret().isNull()) {
        if (options->secretCallback().isNull()) {
            NTCTLS_RESOURCE_LOG_SECRET_UNAVAILABLE();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntca::EncryptionSecretCallback callback =
            options->secretCallback().value();

        if (!callback) {
            NTCTLS_RESOURCE_LOG_SECRET_UNAVAILABLE();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ntca::EncryptionSecret secret;
        error = callback(&secret);
        if (error) {
            NTCTLS_RESOURCE_LOG_SECRET_UNAVAILABLE();
            return error;
        }

        options->setSecret(secret);
    }

    BSLS_ASSERT(options->secret().has_value());
    return ntsa::Error();
}

ntsa::Error ResourceUtil::convertCertificate(
    bsl::shared_ptr<ntctls::Certificate>* result,
    const ntca::EncryptionCertificate&    certificate,
    bslma::Allocator*                     basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntctls::Handle<X509> x509;
    error = ResourceUtil::convertCertificate(&x509, certificate);
    if (error) {
        return error;
    }

    result->createInplace(allocator, x509.release(), allocator);
    return ntsa::Error();
}

ntsa::Error ResourceUtil::convertCertificate(
    ntctls::Handle<X509>*              result,
    const ntca::EncryptionCertificate& certificate)
{
    ntsa::Error error;
    int         rc;

    bslma::Allocator* allocator = bslma::Default::allocator();

    bdlsb::MemOutStreamBuf osb;
    {
        ntsa::AbstractSyntaxEncoder encoder(&osb);
        error = certificate.encode(&encoder);
        if (error) {
            return error;
        }

        rc = osb.pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntctls::ErrorStack diagnostics;

    ntca::EncryptionResourceOptions resourceOptions;
    resourceOptions.setType(ntca::EncryptionResourceType::e_ASN1);

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    error = ResourceUtil::decodeCertificateAsn1(&diagnostics,
                                                &isb,
                                                result,
                                                &resourceOptions,
                                                allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::convertCertificate(
    ntca::EncryptionCertificate*                result,
    const bsl::shared_ptr<ntctls::Certificate>& certificate)
{
    ntsa::Error error;
    int         rc;

    bdlsb::MemOutStreamBuf osb;
    {
        ntctls::ErrorStack diagnostics;

        ntca::EncryptionResourceOptions resourceOptions;
        resourceOptions.setType(ntca::EncryptionResourceType::e_ASN1);

        error = ResourceUtil::encodeCertificateAsn1(&diagnostics,
                                                    &osb,
                                                    certificate,
                                                    &resourceOptions);
        if (error) {
            return error;
        }

        rc = osb.pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bdlsb::FixedMemInStreamBuf  isb(osb.data(), osb.length());
    ntsa::AbstractSyntaxDecoder decoder(&isb);

    error = result->decode(&decoder);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::convertCertificate(
    ntca::EncryptionCertificate* result,
    const ntctls::Handle<X509>&  certificate)
{
    ntsa::Error error;
    int         rc;

    bdlsb::MemOutStreamBuf osb;
    {
        ntctls::ErrorStack diagnostics;

        ntca::EncryptionResourceOptions resourceOptions;
        resourceOptions.setType(ntca::EncryptionResourceType::e_ASN1);

        error = ResourceUtil::encodeCertificateAsn1(&diagnostics,
                                                    &osb,
                                                    certificate.get(),
                                                    &resourceOptions);
        if (error) {
            return error;
        }

        rc = osb.pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bdlsb::FixedMemInStreamBuf  isb(osb.data(), osb.length());
    ntsa::AbstractSyntaxDecoder decoder(&isb);

    error = result->decode(&decoder);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::convertKey(bsl::shared_ptr<ntctls::Key>* result,
                                     const ntca::EncryptionKey&    key,
                                     bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    ntctls::Handle<EVP_PKEY> pkey;
    error = ResourceUtil::convertKey(&pkey, key);
    if (error) {
        return error;
    }

    result->createInplace(allocator, pkey.release(), allocator);
    return ntsa::Error();
}

ntsa::Error ResourceUtil::convertKey(ntctls::Handle<EVP_PKEY>*  result,
                                     const ntca::EncryptionKey& key)
{
    ntsa::Error error;
    int         rc;

    bslma::Allocator* allocator = bslma::Default::allocator();

    bdlsb::MemOutStreamBuf osb;
    {
        ntsa::AbstractSyntaxEncoder encoder(&osb);
        error = key.encode(&encoder);
        if (error) {
            return error;
        }

        rc = osb.pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    ntctls::ErrorStack diagnostics;

    ntca::EncryptionResourceOptions resourceOptions;
    resourceOptions.setType(ntca::EncryptionResourceType::e_ASN1);

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    error = ResourceUtil::decodeKeyAsn1(&diagnostics,
                                        &isb,
                                        result,
                                        &resourceOptions,
                                        allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::convertKey(ntca::EncryptionKey* result,
                                     const bsl::shared_ptr<ntctls::Key>& key)
{
    ntsa::Error error;
    int         rc;

    bdlsb::MemOutStreamBuf osb;
    {
        ntctls::ErrorStack diagnostics;

        ntca::EncryptionResourceOptions resourceOptions;
        resourceOptions.setType(ntca::EncryptionResourceType::e_ASN1);

        error = ResourceUtil::encodeKeyAsn1(&diagnostics,
                                            &osb,
                                            key,
                                            &resourceOptions);
        if (error) {
            return error;
        }

        rc = osb.pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bdlsb::FixedMemInStreamBuf  isb(osb.data(), osb.length());
    ntsa::AbstractSyntaxDecoder decoder(&isb);

    error = result->decode(&decoder);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error ResourceUtil::convertKey(ntca::EncryptionKey*            result,
                                     const ntctls::Handle<EVP_PKEY>& key)
{
    ntsa::Error error;
    int         rc;

    bdlsb::MemOutStreamBuf osb;
    {
        ntctls::ErrorStack diagnostics;

        ntca::EncryptionResourceOptions resourceOptions;
        resourceOptions.setType(ntca::EncryptionResourceType::e_ASN1);

        error = ResourceUtil::encodeKeyAsn1(&diagnostics,
                                            &osb,
                                            key.get(),
                                            &resourceOptions);
        if (error) {
            return error;
        }

        rc = osb.pubsync();
        if (rc != 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    bdlsb::FixedMemInStreamBuf  isb(osb.data(), osb.length());
    ntsa::AbstractSyntaxDecoder decoder(&isb);

    error = result->decode(&decoder);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

Key::Key(bslma_Allocator* basicAllocator)
: d_pkey()
, d_record(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Key::Key(EVP_PKEY* pkey, bslma_Allocator* basicAllocator)
: d_pkey(pkey)
, d_record(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error = Resource::convert(&d_record, d_pkey);
    if (error) {
        BSLS_LOG_ERROR("Failed to decode private key");
    }
}

Key::~Key()
{
}

ntsa::Error Key::generateDsa(ntctls::Handle<EVP_PKEY>* result)
{
    ntctls::Handle<DSA> dsa(DSA_new());
    if (!dsa) {
        NTCTLS_KEY_LOG_DSA_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const int bits = k_DEFAULT_DSA_BITS;

    if (!DSA_generate_parameters_ex(dsa.get(), bits, 0, 0, 0, 0, 0)) {
        NTCTLS_KEY_LOG_DSA_ERROR_GENERATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!DSA_generate_key(dsa.get())) {
        NTCTLS_KEY_LOG_DSA_ERROR_GENERATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntctls::Handle<EVP_PKEY> pkey(EVP_PKEY_new());
    if (!pkey) {
        NTCTLS_KEY_LOG_EV_PKEY_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!EVP_PKEY_assign_DSA(pkey.get(), dsa.get())) {
        NTCTLS_KEY_LOG_EV_PKEY_ERROR_ASSIGN();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    dsa.release();

    result->reset(pkey.release());

    BSLS_LOG_WARN("DSA keys are not recommended: "
                  "use RSA or NIST P-256 instead");

    return ntsa::Error();
}

ntsa::Error Key::generateRsa(ntctls::Handle<EVP_PKEY>* result)
{
    ntctls::Handle<BIGNUM> bn(BN_new());
    if (!bn) {
        NTCTLS_KEY_LOG_BN_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!BN_set_word(bn.get(), k_DEFAULT_RSA_EXPONENT)) {
        NTCTLS_KEY_LOG_BN_ERROR_SET_EXPONENT();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntctls::Handle<RSA> rsa(RSA_new());
    if (!rsa) {
        NTCTLS_KEY_LOG_RSA_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const int bits = k_DEFAULT_RSA_BITS;

    if (!RSA_generate_key_ex(rsa.get(), bits, bn.get(), 0)) {
        NTCTLS_KEY_LOG_RSA_ERROR_GENERATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntctls::Handle<EVP_PKEY> pkey(EVP_PKEY_new());
    if (!pkey) {
        NTCTLS_KEY_LOG_EV_PKEY_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!EVP_PKEY_assign_RSA(pkey.get(), rsa.get())) {
        NTCTLS_KEY_LOG_EV_PKEY_ERROR_ASSIGN();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rsa.release();

    result->reset(pkey.release());

    return ntsa::Error();
}

ntsa::Error Key::generateEllipticCurve(ntctls::Handle<EVP_PKEY>* result,
                                       int                       parameterId)
{
    int rc;

    ntctls::Handle<EVP_PKEY_CTX> pkeyCtx(EVP_PKEY_CTX_new_id(EVP_PKEY_EC, 0));
    if (!pkeyCtx) {
        NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = EVP_PKEY_keygen_init(pkeyCtx.get());
    if (rc <= 0) {
        NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_KEYGEN_INIT();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pkeyCtx.get(), parameterId);
    if (rc <= 0) {
        NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_KEYGEN_SET_CURVE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    EVP_PKEY* pkey = 0;

    rc = EVP_PKEY_keygen(pkeyCtx.get(), &pkey);
    if (rc <= 0) {
        NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_KEYGEN_GENERATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    result->reset(pkey);

    return ntsa::Error();
}

ntsa::Error Key::generateEdwardsCurve(ntctls::Handle<EVP_PKEY>* result,
                                      int                       typeId)
{
    int rc;

    ntctls::Handle<EVP_PKEY_CTX> pkeyCtx(EVP_PKEY_CTX_new_id(typeId, 0));
    if (!pkeyCtx) {
        NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = EVP_PKEY_keygen_init(pkeyCtx.get());
    if (rc <= 0) {
        NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_KEYGEN_INIT();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    EVP_PKEY* pkey = 0;

    rc = EVP_PKEY_keygen(pkeyCtx.get(), &pkey);
    if (rc <= 0) {
        NTCTLS_KEY_LOG_EV_PKEY_CTX_ERROR_KEYGEN_GENERATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    result->reset(pkey);

    return ntsa::Error();
}

ntsa::Error Key::generate(const ntca::EncryptionKeyOptions& configuration)
{
    ntsa::Error error;

    ntca::EncryptionKeyType::Value type =
        configuration.type().value_or(ntca::EncryptionKeyType::e_NIST_P256);

    if (type == ntca::EncryptionKeyType::e_DSA) {
        error = ntctls::Key::generateDsa(&d_pkey);
    }
    else if (type == ntca::EncryptionKeyType::e_RSA) {
        error = ntctls::Key::generateRsa(&d_pkey);
    }
    else if (type == ntca::EncryptionKeyType::e_NIST_P256) {
        error =
            ntctls::Key::generateEllipticCurve(&d_pkey, NID_X9_62_prime256v1);
    }
    else if (type == ntca::EncryptionKeyType::e_NIST_P384) {
        error = ntctls::Key::generateEllipticCurve(&d_pkey, NID_secp384r1);
    }
    else if (type == ntca::EncryptionKeyType::e_NIST_P521) {
        error = ntctls::Key::generateEllipticCurve(&d_pkey, NID_secp521r1);
    }
    else if (type == ntca::EncryptionKeyType::e_ED25519) {
        error = ntctls::Key::generateEdwardsCurve(&d_pkey, EVP_PKEY_ED25519);
    }
    else if (type == ntca::EncryptionKeyType::e_ED448) {
        error = ntctls::Key::generateEdwardsCurve(&d_pkey, EVP_PKEY_ED448);
    }
    else {
        error = ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    if (error) {
        return error;
    }

    error = Resource::convert(&d_record, d_pkey);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Key::decode(bsl::streambuf*                        source,
                        const ntca::EncryptionResourceOptions& options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntctls::Key>         privateKey;
    bsl::shared_ptr<ntctls::Certificate> certificate;
    ntctls::CertificateVector            caList;

    error = ResourceUtil::decode(source,
                                 &privateKey,
                                 &certificate,
                                 &caList,
                                 options,
                                 d_allocator_p);
    if (error) {
        return error;
    }

    if (!privateKey) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    d_pkey.reset(privateKey->d_pkey.release());

    error = Resource::convert(&d_record, d_pkey);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Key::encode(bsl::streambuf*                        destination,
                        const ntca::EncryptionResourceOptions& options) const
{
    bsl::shared_ptr<ntctls::Key>         privateKey;
    bsl::shared_ptr<ntctls::Certificate> certificate;
    ntctls::CertificateVector            caList;

    privateKey = const_cast<Key*>(this)->getSelf(const_cast<Key*>(this));

    return ResourceUtil::encode(destination,
                                privateKey,
                                certificate,
                                caList,
                                options);
}

ntsa::Error Key::unwrap(ntca::EncryptionKey* result) const
{
    *result = d_record;
    return ntsa::Error();
}

void Key::print(bsl::ostream& stream) const
{
    bsl::shared_ptr<BIO> bio = Internal::createStream(stream.rdbuf());
    if (bio) {
        EVP_PKEY_print_private(bio.get(), d_pkey.get(), 0, 0);
    }
}

void* Key::handle() const
{
    return d_pkey.get();
}

EVP_PKEY* Key::native() const
{
    return d_pkey.get();
}

const ntca::EncryptionKey& Key::record() const
{
    return d_record;
}

bool Key::equals(const ntci::EncryptionKey& other) const
{
    const Key* concreteKey = dynamic_cast<const Key*>(&other);

    if (!concreteKey) {
        return false;
    }

    enum { k_EVP_PKEY_CMP_EQUAL = 1 };

    int rc = EVP_PKEY_cmp(d_pkey.get(), concreteKey->d_pkey.get());
    return rc == k_EVP_PKEY_CMP_EQUAL;
}

ntsa::Error Key::generateKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                             const ntca::EncryptionKeyOptions&     options,
                             bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Key> effectiveResult;
    effectiveResult.createInplace(allocator, allocator);

    error = effectiveResult->generate(options);
    if (error) {
        return error;
    }

    *result = effectiveResult;
    return ntsa::Error();
}

ntsa::Error Key::generateKey(ntca::EncryptionKey*              result,
                             const ntca::EncryptionKeyOptions& options,
                             bslma::Allocator*                 basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Key> effectiveResult;
    effectiveResult.createInplace(allocator, allocator);

    error = effectiveResult->generate(options);
    if (error) {
        return error;
    }

    error = ntctls::Resource::convert(result, effectiveResult);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

Certificate::Certificate(bslma_Allocator* basicAllocator)
: d_x509()
, d_record(basicAllocator)
, d_subject(basicAllocator)
, d_issuer(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Certificate::Certificate(X509* x509, bslma_Allocator* basicAllocator)
: d_x509(x509)
, d_record(basicAllocator)
, d_subject(basicAllocator)
, d_issuer(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ntsa::Error error = Resource::convert(&d_record, d_x509);
    if (error) {
        BSLS_LOG_ERROR("Failed to decode certificate");
    }

    Impl::parseDistinguishedName(&d_subject, X509_get_subject_name(x509));
    Impl::parseDistinguishedName(&d_issuer, X509_get_issuer_name(x509));
}

Certificate::~Certificate()
{
}

ntsa::Error Certificate::generate(
    const ntsa::DistinguishedName&            userIdentity,
    const bsl::shared_ptr<Key>&               userPrivateKey,
    const ntca::EncryptionCertificateOptions& configuration)
{
    ntsa::Error error;
    int         rc;

    ntctls::Handle<X509> x509(X509_new());
    if (!x509) {
        NTCTLS_CERTIFICATE_LOG_X509_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    X509_set_version(x509.get(), 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509.get()),
                     configuration.serialNumber());

    bdlt::Datetime startTime = configuration.startTime().gmtDatetime();
    if (startTime < bdlt::EpochUtil::epoch()) {
        startTime = bdlt::EpochUtil::epoch();
    }

    ASN1_TIME_set(X509_getm_notBefore(x509.get()),
                  bdlt::EpochUtil::convertToTimeT(startTime));

    bdlt::Datetime expirationTime =
        configuration.expirationTime().gmtDatetime();
    if (expirationTime >= bdlt::Datetime(2038, 1, 1)) {
        expirationTime = bdlt::Datetime(2038, 1, 1);
    }

    ASN1_TIME_set(X509_getm_notAfter(x509.get()),
                  bdlt::EpochUtil::convertToTimeT(expirationTime));

    {
        ntctls::Handle<X509_NAME> x509Name(X509_NAME_new());
        if (!x509Name) {
            NTCTLS_CERTIFICATE_LOG_X509_NAME_ERROR_ALLOCATE();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        for (ntsa::DistinguishedName::ComponentList::const_iterator it =
                 userIdentity.begin();
             it != userIdentity.end();
             ++it)
        {
            rc = Impl::generateX509Name(x509Name.get(), *it);
            if (0 != rc) {
                NTCTLS_CERTIFICATE_LOG_X509_NAME_ERROR_GENERATE();
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (!X509_set_subject_name(x509.get(), x509Name.get())) {
            NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_SUBJECT_NAME();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!X509_set_issuer_name(x509.get(), x509Name.get())) {
            NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_ISSUER_NAME();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (!X509_set_pubkey(x509.get(), userPrivateKey->native())) {
        NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_PUBLIC_KEY();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    X509V3_CTX x509v3Ctx = {};

    X509V3_set_ctx(&x509v3Ctx, x509.get(), x509.get(), 0, 0, 0);

    if (configuration.authority()) {
        error = Impl::addExtension(x509.get(),
                                   &x509v3Ctx,
                                   NID_basic_constraints,
                                   "critical,CA:TRUE");
        if (error) {
            return error;
        }

        error = Impl::addExtension(x509.get(),
                                   &x509v3Ctx,
                                   NID_key_usage,
                                   "keyCertSign,cRLSign");
        if (error) {
            return error;
        }
    }
    else {
        error = Impl::addExtension(x509.get(),
                                   &x509v3Ctx,
                                   NID_basic_constraints,
                                   "critical,CA:FALSE");
        if (error) {
            return error;
        }
    }

    error = Impl::addExtension(x509.get(),
                               &x509v3Ctx,
                               NID_subject_key_identifier,
                               "hash");
    if (error) {
        return error;
    }

    error = Impl::addExtension(x509.get(),
                               &x509v3Ctx,
                               NID_authority_key_identifier,
                               "keyid:always");
    if (error) {
        return error;
    }

    if (!configuration.hosts().empty()) {
        bsl::string subjectAlternativeName;
        {
            bsl::stringstream ss;
            for (bsl::size_t i = 0; i < configuration.hosts().size(); ++i) {
                if (i != 0) {
                    ss << ",";
                }

                ntsa::IpAddress ipAddress;
                if (ipAddress.parse(configuration.hosts()[i])) {
                    ss << "IP:" << ipAddress.text();
                }
                else {
                    ss << "DNS:" << configuration.hosts()[i];
                }
            }

            subjectAlternativeName = ss.str();
        }

        error = Impl::addExtension(x509.get(),
                                   &x509v3Ctx,
                                   NID_subject_alt_name,
                                   subjectAlternativeName.c_str());
        if (error) {
            return error;
        }
    }

    const int keyId = EVP_PKEY_id(userPrivateKey->native());

    if (keyId == NID_ED25519 || keyId == NID_ED448) {
        if (!X509_sign(x509.get(), userPrivateKey->native(), 0)) {
            NTCTLS_CERTIFICATE_LOG_X509_ERROR_SIGN();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        if (!X509_sign(x509.get(), userPrivateKey->native(), EVP_sha256())) {
            NTCTLS_CERTIFICATE_LOG_X509_ERROR_SIGN();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    d_x509.reset(x509.release());

    error = Resource::convert(&d_record, d_x509);
    if (error) {
        return error;
    }

    Impl::parseDistinguishedName(&d_subject,
                                 X509_get_subject_name(d_x509.get()));
    Impl::parseDistinguishedName(&d_issuer,
                                 X509_get_issuer_name(d_x509.get()));

    return ntsa::Error();
}

ntsa::Error Certificate::generate(
    const ntsa::DistinguishedName&            userIdentity,
    const bsl::shared_ptr<Key>&               userPrivateKey,
    const bsl::shared_ptr<Certificate>&       authorityCertificate,
    const bsl::shared_ptr<Key>&               authorityPrivateKey,
    const ntca::EncryptionCertificateOptions& configuration)
{
    // Review: Consider supporting certificate revocation (CRL).

    ntsa::Error error;
    int         rc;

    ntctls::Handle<X509> x509(X509_new());
    if (!x509) {
        NTCTLS_CERTIFICATE_LOG_X509_ERROR_ALLOCATE();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    X509_set_version(x509.get(), 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509.get()),
                     configuration.serialNumber());

    bdlt::Datetime startTime = configuration.startTime().gmtDatetime();

    bsl::string startTimeString;
    ntctls::Internal::convertDatetimeToAsn1TimeString(&startTimeString,
                                                      startTime);

    rc = ASN1_TIME_set_string(X509_getm_notBefore(x509.get()),
                              startTimeString.c_str());
    if (rc != 1) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bdlt::Datetime expirationTime =
        configuration.expirationTime().gmtDatetime();

    bsl::string expirationTimeString;
    ntctls::Internal::convertDatetimeToAsn1TimeString(&expirationTimeString,
                                                      expirationTime);

    rc = ASN1_TIME_set_string(X509_getm_notAfter(x509.get()),
                              expirationTimeString.c_str());
    if (rc != 1) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        ntctls::Handle<X509_NAME> x509Name(X509_NAME_new());
        if (!x509Name) {
            NTCTLS_CERTIFICATE_LOG_X509_NAME_ERROR_ALLOCATE();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        for (ntsa::DistinguishedName::ComponentList::const_iterator it =
                 userIdentity.begin();
             it != userIdentity.end();
             ++it)
        {
            rc = Impl::generateX509Name(x509Name.get(), *it);
            if (0 != rc) {
                NTCTLS_CERTIFICATE_LOG_X509_NAME_ERROR_GENERATE();
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }

        if (!X509_set_subject_name(x509.get(), x509Name.get())) {
            NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_SUBJECT_NAME();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (!X509_set_issuer_name(
            x509.get(),
            X509_get_subject_name(authorityCertificate->native())))
    {
        NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_ISSUER_NAME();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!X509_set_pubkey(x509.get(), userPrivateKey->native())) {
        NTCTLS_CERTIFICATE_LOG_X509_ERROR_SET_PUBLIC_KEY();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    X509V3_CTX x509v3Ctx = {};

    X509V3_set_ctx(&x509v3Ctx,
                   authorityCertificate->native(),
                   x509.get(),
                   0,
                   0,
                   0);

    if (configuration.authority()) {
        error = Impl::addExtension(x509.get(),
                                   &x509v3Ctx,
                                   NID_basic_constraints,
                                   "critical,CA:TRUE");
        if (error) {
            return error;
        }

        error = Impl::addExtension(x509.get(),
                                   &x509v3Ctx,
                                   NID_key_usage,
                                   "keyCertSign,cRLSign");
        if (error) {
            return error;
        }
    }
    else {
        error = Impl::addExtension(x509.get(),
                                   &x509v3Ctx,
                                   NID_basic_constraints,
                                   "critical,CA:FALSE");
        if (error) {
            return error;
        }
    }

    error = Impl::addExtension(x509.get(),
                               &x509v3Ctx,
                               NID_subject_key_identifier,
                               "hash");
    if (error) {
        return error;
    }

    error = Impl::addExtension(x509.get(),
                               &x509v3Ctx,
                               NID_authority_key_identifier,
                               "keyid:always");
    if (error) {
        return error;
    }

    if (!configuration.hosts().empty()) {
        bsl::string subjectAlternativeName;
        {
            bsl::stringstream ss;
            for (bsl::size_t i = 0; i < configuration.hosts().size(); ++i) {
                if (i != 0) {
                    ss << ",";
                }

                ntsa::IpAddress ipAddress;
                if (ipAddress.parse(configuration.hosts()[i])) {
                    ss << "IP:" << ipAddress.text();
                }
                else {
                    ss << "DNS:" << configuration.hosts()[i];
                }
            }

            subjectAlternativeName = ss.str();
        }

        error = Impl::addExtension(x509.get(),
                                   &x509v3Ctx,
                                   NID_subject_alt_name,
                                   subjectAlternativeName.c_str());
        if (error) {
            return error;
        }
    }

    const int keyId = EVP_PKEY_id(authorityPrivateKey->native());

    if (keyId == NID_ED25519 || keyId == NID_ED448) {
        if (!X509_sign(x509.get(), authorityPrivateKey->native(), 0)) {
            NTCTLS_CERTIFICATE_LOG_X509_ERROR_SIGN();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        if (!X509_sign(x509.get(),
                       authorityPrivateKey->native(),
                       EVP_sha256()))
        {
            NTCTLS_CERTIFICATE_LOG_X509_ERROR_SIGN();
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    d_x509.reset(x509.release());

    error = Resource::convert(&d_record, d_x509);
    if (error) {
        return error;
    }

    Impl::parseDistinguishedName(&d_subject,
                                 X509_get_subject_name(d_x509.get()));
    Impl::parseDistinguishedName(&d_issuer,
                                 X509_get_issuer_name(d_x509.get()));

    return ntsa::Error();
}

ntsa::Error Certificate::decode(bsl::streambuf*                        source,
                                const ntca::EncryptionResourceOptions& options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntctls::Key>         privateKey;
    bsl::shared_ptr<ntctls::Certificate> certificate;
    ntctls::CertificateVector            caList;

    error = ResourceUtil::decode(source,
                                 &privateKey,
                                 &certificate,
                                 &caList,
                                 options,
                                 d_allocator_p);
    if (error) {
        return error;
    }

    if (!certificate) {
        if (caList.empty()) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        certificate = caList.front();
    }

    d_x509.reset(certificate->d_x509.release());

    error = Resource::convert(&d_record, d_x509);
    if (error) {
        return error;
    }

    d_subject = certificate->d_subject;
    d_issuer  = certificate->d_issuer;

    return ntsa::Error();
}

ntsa::Error Certificate::encode(
    bsl::streambuf*                        destination,
    const ntca::EncryptionResourceOptions& options) const
{
    bsl::shared_ptr<ntctls::Key>         privateKey;
    bsl::shared_ptr<ntctls::Certificate> certificate;
    ntctls::CertificateVector            caList;

    certificate = const_cast<Certificate*>(this)->getSelf(
        const_cast<Certificate*>(this));

    return ResourceUtil::encode(destination,
                                privateKey,
                                certificate,
                                caList,
                                options);
}

ntsa::Error Certificate::unwrap(ntca::EncryptionCertificate* result) const
{
    *result = d_record;
    return ntsa::Error();
}

void Certificate::print(bsl::ostream& stream) const
{
    bsl::shared_ptr<BIO> bio = Internal::createStream(stream.rdbuf());
    if (bio) {
        if (!X509_print_ex(bio.get(), d_x509.get(), XN_FLAG_SEP_COMMA_PLUS, 0))
        {
            NTCTLS_CERTIFICATE_LOG_PRINT_ERROR();
            return;
        }
    }
}

const ntsa::DistinguishedName& Certificate::subject() const
{
    return d_subject;
}

const ntsa::DistinguishedName& Certificate::issuer() const
{
    return d_issuer;
}

bool Certificate::isAuthority() const
{
    bool result = false;

    X509_EXTENSION* extension =
        Impl::getExtension(d_x509.get(), NID_basic_constraints);
    if (extension == 0) {
        return false;
    }

    int critical = 0;

    BASIC_CONSTRAINTS* bs =
        (BASIC_CONSTRAINTS*)(X509_get_ext_d2i(d_x509.get(),
                                              NID_basic_constraints,
                                              &critical,
                                              0));
    if (bs != 0) {
        result = bs->ca;
        BASIC_CONSTRAINTS_free(bs);
    }

    return result;
}

bool Certificate::isSelfSigned() const
{
    if (X509_check_purpose(d_x509.get(), -1, 0) != 1) {
        return false;
    }

    const bsl::uint32_t extension_flags =
        X509_get_extension_flags(d_x509.get());

    if ((extension_flags & EXFLAG_SS) != 0) {
        return true;
    }

    return false;
}

void* Certificate::handle() const
{
    return d_x509.get();
}

X509* Certificate::native() const
{
    return d_x509.get();
}

const ntca::EncryptionCertificate& Certificate::record() const
{
    return d_record;
}

bool Certificate::equals(const ntci::EncryptionCertificate& other) const
{
    const Certificate* concreteCertificate =
        dynamic_cast<const Certificate*>(&other);

    if (!concreteCertificate) {
        return false;
    }

    enum { k_X509_CMP_EQUAL = 0 };

    X509* x509_1 = d_x509.get();
    X509* x509_2 = concreteCertificate->d_x509.get();

    int rc = X509_cmp(x509_1, x509_2);
    return rc == k_X509_CMP_EQUAL;
}

ntsa::Error Certificate::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const ntsa::DistinguishedName&                userIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&   userPrivateKey,
    const ntca::EncryptionCertificateOptions&     options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Key> effectiveSubjectPrivateKey;
    error =
        ntctls::Resource::convert(&effectiveSubjectPrivateKey, userPrivateKey);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::Certificate> effectiveResult;
    effectiveResult.createInplace(allocator, allocator);

    error = effectiveResult->generate(userIdentity,
                                      effectiveSubjectPrivateKey,
                                      options);
    if (error) {
        return error;
    }

    *result = effectiveResult;
    return ntsa::Error();
}

ntsa::Error Certificate::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
    const ntsa::DistinguishedName&                      userIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&         userPrivateKey,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& authorityCertificate,
    const bsl::shared_ptr<ntci::EncryptionKey>&         authorityPrivateKey,
    const ntca::EncryptionCertificateOptions&           options,
    bslma::Allocator*                                   basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Key> effectiveSubjectPrivateKey;
    error =
        ntctls::Resource::convert(&effectiveSubjectPrivateKey, userPrivateKey);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::Certificate> effectiveIssuerCertificate;
    error = ntctls::Resource::convert(&effectiveIssuerCertificate,
                                      authorityCertificate);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::Key> effectiveIssuerPrivateKey;
    error = ntctls::Resource::convert(&effectiveIssuerPrivateKey,
                                      authorityPrivateKey);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::Certificate> effectiveResult;
    effectiveResult.createInplace(allocator, allocator);

    error = effectiveResult->generate(userIdentity,
                                      effectiveSubjectPrivateKey,
                                      effectiveIssuerCertificate,
                                      effectiveIssuerPrivateKey,
                                      options);
    if (error) {
        return error;
    }

    *result = effectiveResult;
    return ntsa::Error();
}

ntsa::Error Certificate::generateCertificate(
    ntca::EncryptionCertificate*              result,
    const ntsa::DistinguishedName&            userIdentity,
    const ntca::EncryptionKey&                userPrivateKey,
    const ntca::EncryptionCertificateOptions& options,
    bslma::Allocator*                         basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Key> concreteSubjectPrivateKey;
    error = ntctls::Resource::convert(&concreteSubjectPrivateKey,
                                      userPrivateKey,
                                      allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::Certificate> effectiveResult;
    effectiveResult.createInplace(allocator, allocator);

    error = effectiveResult->generate(userIdentity,
                                      concreteSubjectPrivateKey,
                                      options);
    if (error) {
        return error;
    }

    error = ntctls::Resource::convert(result, effectiveResult);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Certificate::generateCertificate(
    ntca::EncryptionCertificate*              result,
    const ntsa::DistinguishedName&            userIdentity,
    const ntca::EncryptionKey&                userPrivateKey,
    const ntca::EncryptionCertificate&        authorityCertificate,
    const ntca::EncryptionKey&                authorityPrivateKey,
    const ntca::EncryptionCertificateOptions& options,
    bslma::Allocator*                         basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Key> concreteSubjectPrivateKey;
    error = ntctls::Resource::convert(&concreteSubjectPrivateKey,
                                      userPrivateKey,
                                      allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::Certificate> concreteIssuerCertificate;
    error = ntctls::Resource::convert(&concreteIssuerCertificate,
                                      authorityCertificate,
                                      allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::Key> concreteIssuerPrivateKey;
    error = ntctls::Resource::convert(&concreteIssuerPrivateKey,
                                      authorityPrivateKey,
                                      allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::Certificate> concreteResult;
    concreteResult.createInplace(allocator, allocator);

    error = concreteResult->generate(userIdentity,
                                     concreteSubjectPrivateKey,
                                     concreteIssuerCertificate,
                                     concreteIssuerPrivateKey,
                                     options);
    if (error) {
        return error;
    }

    error = ntctls::Resource::convert(result, concreteResult);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

Resource::Resource(bslma::Allocator* basicAllocator)
: d_privateKey_sp()
, d_certificate_sp()
, d_caList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Resource::~Resource()
{
}

ntsa::Error Resource::setPrivateKey(const ntca::EncryptionKey& key)
{
    ntsa::Error error;

    if (d_privateKey_sp) {
        NTCTLS_RESOURCE_LOG_ALREADY_HAVE_KEY();
        return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
    }

    bsl::shared_ptr<ntctls::Key> concreteKey;
    error = ResourceUtil::convertKey(&concreteKey, key, d_allocator_p);
    if (error) {
        return error;
    }

    d_privateKey_sp = concreteKey;

    return ntsa::Error();
}

ntsa::Error Resource::setPrivateKey(
    const bsl::shared_ptr<ntci::EncryptionKey>& key)
{
    if (d_privateKey_sp) {
        NTCTLS_RESOURCE_LOG_ALREADY_HAVE_KEY();
        return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
    }

    bsl::shared_ptr<ntctls::Key> concreteKey =
        bsl::dynamic_pointer_cast<ntctls::Key>(key);

    if (!concreteKey) {
        NTCTLS_RESOURCE_LOG_INVALID_DRIVER();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_privateKey_sp = concreteKey;

    return ntsa::Error();
}

ntsa::Error Resource::setCertificate(
    const ntca::EncryptionCertificate& certificate)
{
    ntsa::Error error;

    if (d_certificate_sp) {
        NTCTLS_RESOURCE_LOG_ALREADY_HAVE_CERTIFICATE();
        return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
    }

    bsl::shared_ptr<ntctls::Certificate> concreteCertificate;
    error = ResourceUtil::convertCertificate(&concreteCertificate,
                                             certificate,
                                             d_allocator_p);
    if (error) {
        return error;
    }

    d_certificate_sp = concreteCertificate;

    return ntsa::Error();
}

ntsa::Error Resource::setCertificate(
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    if (d_certificate_sp) {
        NTCTLS_RESOURCE_LOG_ALREADY_HAVE_CERTIFICATE();
        return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
    }

    bsl::shared_ptr<ntctls::Certificate> concreteCertificate =
        bsl::dynamic_pointer_cast<ntctls::Certificate>(certificate);

    if (!concreteCertificate) {
        NTCTLS_RESOURCE_LOG_INVALID_DRIVER();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_certificate_sp = concreteCertificate;

    return ntsa::Error();
}

ntsa::Error Resource::addCertificateAuthority(
    const ntca::EncryptionCertificate& certificate)
{
    ntsa::Error error;

    bsl::shared_ptr<ntctls::Certificate> concreteCertificate;
    error = ResourceUtil::convertCertificate(&concreteCertificate,
                                             certificate,
                                             d_allocator_p);
    if (error) {
        return error;
    }

    d_caList.push_back(concreteCertificate);

    return ntsa::Error();
}

ntsa::Error Resource::addCertificateAuthority(
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    bsl::shared_ptr<ntctls::Certificate> concreteCertificate =
        bsl::dynamic_pointer_cast<ntctls::Certificate>(certificate);

    if (!concreteCertificate) {
        NTCTLS_RESOURCE_LOG_INVALID_DRIVER();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_caList.push_back(concreteCertificate);

    return ntsa::Error();
}

ntsa::Error Resource::decode(bsl::streambuf*                        source,
                             const ntca::EncryptionResourceOptions& options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntctls::Key>         privateKey;
    bsl::shared_ptr<ntctls::Certificate> certificate;
    ntctls::CertificateVector            caList;

    error = ResourceUtil::decode(source,
                                 &privateKey,
                                 &certificate,
                                 &caList,
                                 options,
                                 d_allocator_p);

    if (error) {
        return error;
    }

    if (privateKey) {
        if (d_privateKey_sp) {
            NTCTLS_RESOURCE_LOG_ALREADY_HAVE_KEY();
            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }

        d_privateKey_sp = privateKey;
    }

    if (certificate) {
        const bool isCa = certificate->isAuthority();
        if (isCa) {
            d_caList.push_back(certificate);
        }
        else {
            if (d_certificate_sp) {
                NTCTLS_RESOURCE_LOG_ALREADY_HAVE_CERTIFICATE();
                return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
            }

            d_certificate_sp = certificate;
        }
    }

    if (!caList.empty()) {
        d_caList.insert(d_caList.end(), caList.begin(), caList.end());
    }

    return ntsa::Error();
}

ntsa::Error Resource::encode(
    bsl::streambuf*                        destination,
    const ntca::EncryptionResourceOptions& options) const
{
    if (!d_privateKey_sp && !d_certificate_sp && d_caList.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ResourceUtil::encode(destination,
                                d_privateKey_sp,
                                d_certificate_sp,
                                d_caList,
                                options);
}

ntsa::Error Resource::getPrivateKey(
    bsl::shared_ptr<ntci::EncryptionKey>* result) const
{
    if (!d_privateKey_sp) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *result = d_privateKey_sp;

    return ntsa::Error();
}

ntsa::Error Resource::getCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result) const
{
    if (!d_certificate_sp) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *result = d_certificate_sp;

    return ntsa::Error();
}

ntsa::Error Resource::getCertificateAuthoritySet(
    bsl::vector<bsl::shared_ptr<ntci::EncryptionCertificate> >* result) const
{
    if (d_caList.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    result->reserve(result->size() + d_caList.size());

    for (CertificateVector::const_iterator it = d_caList.begin();
         it != d_caList.end();
         ++it)
    {
        result->push_back(*it);
    }

    return ntsa::Error();
}

bool Resource::equals(const Resource& other) const
{
    return this->contains(other, true, true);
}

bool Resource::contains(const Resource& other,
                        bool            includePrivateKeys,
                        bool            includeCertificates) const
{
    if (includePrivateKeys) {
        if (d_privateKey_sp) {
            if (!other.d_privateKey_sp) {
                return false;
            }
        }

        if (other.d_privateKey_sp) {
            if (!d_privateKey_sp) {
                return false;
            }
        }

        if (d_privateKey_sp && other.d_privateKey_sp) {
            if (!d_privateKey_sp->equals(*other.d_privateKey_sp)) {
                return false;
            }
        }
    }

    if (includeCertificates) {
        if (d_certificate_sp) {
            if (!other.d_certificate_sp) {
                return false;
            }
        }

        if (other.d_certificate_sp) {
            if (!d_certificate_sp) {
                return false;
            }
        }

        if (d_certificate_sp && other.d_certificate_sp) {
            if (!d_certificate_sp->equals(*other.d_certificate_sp)) {
                return false;
            }
        }

        if (d_caList.size() != other.d_caList.size()) {
            return false;
        }

        for (bsl::size_t i = 0; i < d_caList.size(); ++i) {
            if (!d_caList[i]->equals(*other.d_caList[i])) {
                return false;
            }
        }
    }

    return true;
}

ntsa::Error Resource::convert(
    bsl::shared_ptr<ntctls::Certificate>*               result,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    *result = bsl::dynamic_pointer_cast<ntctls::Certificate>(certificate);
    if (!*result) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Resource::convert(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const ntca::EncryptionCertificate&            certificate,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntctls::Certificate> concreteResult;
    error = ResourceUtil::convertCertificate(&concreteResult,
                                             certificate,
                                             basicAllocator);
    if (error) {
        return error;
    }

    *result = concreteResult;

    return ntsa::Error();
}

ntsa::Error Resource::convert(bsl::shared_ptr<ntctls::Certificate>* result,
                              const ntca::EncryptionCertificate& certificate,
                              bslma::Allocator* basicAllocator)
{
    return ResourceUtil::convertCertificate(result,
                                            certificate,
                                            basicAllocator);
}

ntsa::Error Resource::convert(ntctls::Handle<X509>*              result,
                              const ntca::EncryptionCertificate& certificate)
{
    return ResourceUtil::convertCertificate(result, certificate);
}

ntsa::Error Resource::convert(
    ntca::EncryptionCertificate*                result,
    const bsl::shared_ptr<ntctls::Certificate>& certificate)
{
    return ResourceUtil::convertCertificate(result, certificate);
}

ntsa::Error Resource::convert(ntca::EncryptionCertificate* result,
                              const ntctls::Handle<X509>&  certificate)
{
    return ResourceUtil::convertCertificate(result, certificate);
}

ntsa::Error Resource::convert(bsl::shared_ptr<ntctls::Key>* result,
                              const bsl::shared_ptr<ntci::EncryptionKey>& key)
{
    *result = bsl::dynamic_pointer_cast<ntctls::Key>(key);
    if (!*result) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Resource::convert(bsl::shared_ptr<ntci::EncryptionKey>* result,
                              const ntca::EncryptionKey&            key,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    bsl::shared_ptr<ntctls::Key> concreteKey;
    error = ResourceUtil::convertKey(&concreteKey, key, basicAllocator);
    if (error) {
        return error;
    }

    *result = concreteKey;
    return ntsa::Error();
}

ntsa::Error Resource::convert(bsl::shared_ptr<ntctls::Key>* result,
                              const ntca::EncryptionKey&    key,
                              bslma::Allocator*             basicAllocator)
{
    return ResourceUtil::convertKey(result, key, basicAllocator);
}

ntsa::Error Resource::convert(ntctls::Handle<EVP_PKEY>*  result,
                              const ntca::EncryptionKey& key)
{
    return ResourceUtil::convertKey(result, key);
}

ntsa::Error Resource::convert(ntca::EncryptionKey*                result,
                              const bsl::shared_ptr<ntctls::Key>& key)
{
    return ResourceUtil::convertKey(result, key);
}

ntsa::Error Resource::convert(ntca::EncryptionKey*            result,
                              const ntctls::Handle<EVP_PKEY>& key)
{
    return ResourceUtil::convertKey(result, key);
}

/// Provide a TLS session state machine to encrypt/decrypt data.
///
/// @details
/// This component provides a mechanism, 'ntctls::Session', that
/// implements the 'ntci::Encryption' interface to establish a TLS session in
/// either the client or server role, then encrypt and decrypt a data stream,
/// then perform a bidirectional shutdown.
///
/// @par Shutting Down the TLS session
/// This component implements a full bidirectional TLS shutdown. Each TLS
/// peer is responsible for shutting down its side of the TLS session. After
/// a TLS shutdown has been initiated, it is still possible to push incoming
/// ciphertext and pop incoming plaintext, but it is no longer possible to push
/// outgoing plaintext. Similarly, after a TLS shutdown has been received, it
/// is still possible to push outgoing plaintext and pop outgoing ciphertext,
/// but it is no longer possible to push incoming ciphertext.
///
/// Similar to the Berkeley Sockets API, when a TLS shutdown has been received
/// from the peer and fully processed by the internal TLS state machine,
/// 'hasIncomingPlainText' will return true and 'popIncomingPlainText' will
/// return with no errors but append zero bytes to its output blob parameter.
///
/// Users should avoid calling 'isShutdownFinished' because this function will
/// return true immediately after 'pushIncomingCipherText' is called that
/// contains the TLS shutdown from the peer, regardless of whether the all the
/// incoming plaintext has been popped.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctls
class Session : public ntci::Encryption
{
    // Enumerates the OpenSSL shutdown state flags.
    enum { k_TLS_SHUTDOWN_SENT = 0x01, k_TLS_SHUTDOWN_RECEIVED = 0x02 };

    typedef ntci::Mutex       Mutex;
    typedef ntci::LockGuard   LockGuard;
    typedef ntci::UnLockGuard UnLockGuard;

    mutable Mutex                             d_mutex;
    bsl::shared_ptr<ntctls::SessionContext>   d_context_sp;
    bsl::shared_ptr<ntctls::SessionManager>   d_sessionManager_sp;
    bsl::shared_ptr<bdlbb::BlobBufferFactory> d_blobBufferFactory_sp;
    bsl::shared_ptr<ntctls::Certificate>      d_sourceCertificate_sp;
    bsl::shared_ptr<ntctls::Certificate>      d_remoteCertificate_sp;
    bdlb::NullableValue<bsl::string>          d_serverNameIndication;
    SSL*                                      d_ssl_p;
    bdlbb::Blob                               d_incomingCipherText;
    bdlbb::Blob                               d_outgoingCipherText;
    bdlbb::Blob                               d_incomingPlainText;
    bdlbb::Blob                               d_outgoingPlainText;
    bdlbb::Blob                               d_incomingLeftovers;
    bdlbb::Blob                               d_outgoingLeftovers;
    ntca::UpgradeOptions                      d_upgradeOptions;
    ntci::Encryption::HandshakeCallback       d_handshakeCallback;
    bslma::Allocator*                         d_allocator_p;

  private:
    Session(const Session&) BSLS_KEYWORD_DELETED;
    Session& operator=(const Session&) BSLS_KEYWORD_DELETED;

  private:
    /// Initialize a new session.
    ntsa::Error init();

    /// Process the available data through the TLS state machine under the
    /// specified 'lock' guard. Return the error.
    ntsa::Error process(LockGuard* lock);

    /// Analyze incoming ciphertext data, detect any non-TLS protocol data,
    /// and if any is found, save it to 'd_incomingLeftovers'.
    ntsa::Error analyzeIncoming();

    /// Analyze outgoing data.
    ntsa::Error analyzeOutgoing();

    /// Process the information callback for the specified 'ssl' session from
    /// the specified 'where' indication in the state machine according to the
    /// specified 'ret' code.
    static void infoCallback(const SSL* ssl, int where, int ret);

    /// Check the specified 'result' for the specified 'ssl' session. Return
    /// true if there is an error, otherwise return false.
    static bool check(SSL* ssl, int result);

  public:
    /// Create a new session in the specified 'sessionManager'. Allocate blob
    /// buffers using the specified 'blobBufferFactory'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    Session(const bsl::shared_ptr<ntctls::SessionManager>&   sessionManager,
            const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
            bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Session() BSLS_KEYWORD_OVERRIDE;

    /// Switch to the context associated with the specified 'serverName'
    /// indication. Load into the speciifed 'found' flag whether the context
    /// was found. Return the error.
    ntsa::Error activate(bool* found, const bsl::string& serverName);

    /// Authenticate the specfiied 'x509' certificate that is part of the
    /// specified 'x509Store' having the specified 'x509StoreCtx'. Return the
    /// error.
    ntsa::Error authenticate(X509_STORE_CTX* x509StoreCtx);

    /// Authenticate the specfiied 'x509' certificate that is part of the
    /// specified 'x509Store' having the specified 'x509StoreCtx'. Return the
    /// error.
    ntsa::Error authenticate(X509_STORE_CTX* x509StoreCtx, X509* x509);

    /// Initiate the handshake to begin the session. Invoke the specified
    /// 'callback' when the handshake completes. Return the error.
    ntsa::Error initiateHandshake(const HandshakeCallback& callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Initiate the handshake to begin the session according to the specified
    /// 'upgradeOptions'. Invoke the specified 'callback' when the handshake
    /// completes. Return the error.
    ntsa::Error initiateHandshake(const ntca::UpgradeOptions& upgradeOptions,
                                  const HandshakeCallback&    callback)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'input' containing ciphertext read from the peer.
    /// Return the error. 
    ntsa::Error pushIncomingCipherText(const bdlbb::Blob& input)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'input' containing ciphertext read from the peer.
    /// Return the error. 
    ntsa::Error pushIncomingCipherText(const ntsa::Data& input)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'input' containing plaintext to be sent to the peer.
    /// Return the error. 
    ntsa::Error pushOutgoingPlainText(const bdlbb::Blob& input)
        BSLS_KEYWORD_OVERRIDE;

    /// Add the specified 'input' containing plaintext to be sent to the peer.
    /// Return the error. 
    ntsa::Error pushOutgoingPlainText(const ntsa::Data& input)
        BSLS_KEYWORD_OVERRIDE;

    /// Pop plaintext read from the peer and append it to the specified
    /// 'output'. Return the error. 
    ntsa::Error popIncomingPlainText(bdlbb::Blob* output)
        BSLS_KEYWORD_OVERRIDE;

    /// Pop ciphertext to be read from the peer and append to the specified
    /// 'output'. Return the error. 
    ntsa::Error popOutgoingCipherText(bdlbb::Blob* output)
        BSLS_KEYWORD_OVERRIDE;

    /// Pop incoming plaintext leftover after reading across a
    /// ciphertext/plaintext boundary from the peer and append it to the
    /// specified 'output'. Return the error. 
    ntsa::Error popIncomingLeftovers(bdlbb::Blob* output)
        BSLS_KEYWORD_OVERRIDE;

    /// Pop plaintext queue for transmission during the handshake but leftover
    /// because the handshake has failed and append it to the specified 
    /// 'output'. Return the error. 
    ntsa::Error popOutgoingLeftovers(bdlbb::Blob* output) 
        BSLS_KEYWORD_OVERRIDE;

    /// Initiate the shutdown of the session.
    ntsa::Error shutdown() BSLS_KEYWORD_OVERRIDE;

    /// Return true if plaintext data is ready to be read.
    bool hasIncomingPlainText() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if ciphertext data is ready to be sent.
    bool hasOutgoingCipherText() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if plaintext data leftover after reading across a 
    /// ciphertext/plaintext boundary is ready to be read, otherwise return
    /// false.
    bool hasIncomingLeftovers() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if plaintext data for transmission leftover after the
    /// handshake fails is ready to be sent, otherwise return false.
    bool hasOutgoingLeftovers() const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the source certificate used by the
    /// encryption session. Return true if such a certificate is defined, and
    /// false otherwise.
    bool getSourceCertificate(ntca::EncryptionCertificate* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the source certificate used by the
    /// encryption session. Return true if such a certificate is defined, and
    /// false otherwise.
    bool getSourceCertificate(bsl::shared_ptr<ntci::EncryptionCertificate>*
                                  result) const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the remote certificate used by the
    /// encryption session. Return true if such a certificate is defined, and
    /// false otherwise.
    bool getRemoteCertificate(ntca::EncryptionCertificate* result) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the remote certificate used by the
    /// encryption session. Return true if such a certificate is defined, and
    /// false otherwise.
    bool getRemoteCertificate(bsl::shared_ptr<ntci::EncryptionCertificate>*
                                  result) const BSLS_KEYWORD_OVERRIDE;

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

    /// Load into the specified 'result' the server name indication, if any.
    /// Return the error, for example, when no server name indication is
    /// explicitly requested or accepted.
    ntsa::Error serverNameIndication(bsl::string* result) const
        BSLS_KEYWORD_OVERRIDE;
};

/// Provide utilities for sessions.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctls
class SessionUtil
{
    static ntsa::Error configureHost(
        X509_VERIFY_PARAM*                                      parameters,
        const ntca::EncryptionValidation::NullableStringVector& hostVector);

    static ntsa::Error configureHost(
        X509_VERIFY_PARAM*                              parameters,
        const ntca::EncryptionValidation::StringVector& hostVector);

    static ntsa::Error configureMail(
        X509_VERIFY_PARAM*                                      parameters,
        const ntca::EncryptionValidation::NullableStringVector& mailVector);

    static ntsa::Error configureMail(
        X509_VERIFY_PARAM*                              parameters,
        const ntca::EncryptionValidation::StringVector& mailVector);

    static ntsa::Error configureUsage(
        X509_VERIFY_PARAM*                                     parameters,
        const ntca::EncryptionValidation::NullableUsageVector& usageVector);

    static ntsa::Error configureUsage(
        X509_VERIFY_PARAM*                             parameters,
        const ntca::EncryptionValidation::UsageVector& usageVector);

    static ntsa::Error configureUsage(
        X509_VERIFY_PARAM* parameters,
        const ntca::EncryptionValidation::NullableUsageExtended&
            usageExtended);

    static ntsa::Error configureUsage(
        X509_VERIFY_PARAM* parameters,
        const ntca::EncryptionCertificateSubjectKeyUsageExtended&
            usageExtended);

  public:
    static ntsa::Error configure(
        X509_VERIFY_PARAM*                                     parameters,
        const bdlb::NullableValue<ntca::EncryptionValidation>& validation);

    static ntsa::Error configure(X509_VERIFY_PARAM*                parameters,
                                 const ntca::EncryptionValidation& validation);
};

/// Provide a context for authenticating SSL connections.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntctls
class SessionContext
{
    ntctls::Handle<SSL_CTX>                         d_context;
    bsl::shared_ptr<ntctls::Certificate>            d_certificate_sp;
    ntctls::CertificateVector                       d_authorities;
    ntctls::CertificateVector                       d_intermediaries;
    ntca::EncryptionRole::Value                     d_role;
    ntca::EncryptionMethod::Value                   d_minMethod;
    ntca::EncryptionMethod::Value                   d_maxMethod;
    ntca::EncryptionAuthentication::Value           d_authentication;
    bdlb::NullableValue<ntca::EncryptionValidation> d_validation;
    bslma::Allocator*                               d_allocator_p;

  private:
    SessionContext(const SessionContext&) BSLS_KEYWORD_DELETED;
    SessionContext& operator=(const SessionContext&) BSLS_KEYWORD_DELETED;

  private:
    /// Add the private keys, and/or certificate, and/or trusted certificate
    /// authorities contained in the specified 'resource'. Return the error.
    ntsa::Error addResource(const ntca::EncryptionResource& resource);

    /// Add the private keys, and/or certificate, and/or trusted certificate
    /// authorities contained in the specified 'resource'. Return the error.
    ntsa::Error addResourceList(
        const ntca::EncryptionResourceVector& resourceList);

    /// Add the identity described in the specified 'certificate' as a
    /// certificate authority. Return the error.
    ntsa::Error addCertificateAuthority(
        const bsl::shared_ptr<ntctls::Certificate>& certificate);

    /// Set the specified 'directoryPath' as the directory from which to load
    /// certificate authorities. Return the error.
    ntsa::Error setCertificateAuthorityDirectory(
        const bsl::string& directoryPath);

    /// Set the directory from which to load certificate authorities to the
    /// default directory for the system. Return the error.
    ntsa::Error setCertificateAuthorityDirectoryToDefault();

    /// Add the identity described in the specified 'certificate' as a
    /// certificate authority, signed by another certificate authority,
    /// the is used to sign an end-entity certificate but is not itself
    /// explicitly trusted by a authenticating peer.
    ntsa::Error addCertificateIntermediary(
        const bsl::shared_ptr<ntctls::Certificate>& certificate);

    /// Trust all certificate authorities installed into the default system
    /// certificate store.
    ntsa::Error trustSystemDefaults();

    /// Set the certificate to the specified 'certificate'. Return the error.
    ntsa::Error setCertificate(
        const bsl::shared_ptr<ntctls::Certificate>& certificate);

    /// Set the private key to the specified 'privateKey'. Return the error.
    ntsa::Error setPrivateKey(const bsl::shared_ptr<ntctls::Key>& privateKey);

  public:
    /// Create a new SSL authentication context. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit SessionContext(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~SessionContext();

    /// Configure the SSL authentication context for the specified 'role'
    /// according to the specified 'options'. Return the error.
    ntsa::Error configure(ntca::EncryptionRole::Value    role,
                          const ntca::EncryptionOptions& options);

    /// The role played by this context when establishing TLS sessions.
    ntca::EncryptionRole::Value role() const;

    /// The minimum method supported by this context when establishing TLS
    /// sessions, inclusive.
    ntca::EncryptionMethod::Value minMethod() const;

    /// The maximum method supported by this context when establishing TLS
    /// sessions, inclusive.
    ntca::EncryptionMethod::Value maxMethod() const;

    /// The style of authentication used by this context when establishing
    /// TLS sessions.
    ntca::EncryptionAuthentication::Value authentication() const;

    /// Return the certificate validation parameters.
    const bdlb::NullableValue<ntca::EncryptionValidation>& validation() const;

    /// Return the certificate that identifies the user of this context.
    bsl::shared_ptr<ntctls::Certificate> certificate() const;

#if 0
    /// Return the private key of the user of this context.
    bsl::shared_ptr<ntctls::Key> privateKey() const;
#endif

    /// Return a handle to the private implementation.
    void* handle() const;

    /// Return a handle to the native implementation.
    SSL_CTX* native() const;

    /// Load into the specified 'result' a new context configured for the
    /// specified 'role' according to the specified 'options'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used. Return the
    /// error.
    static ntsa::Error createContext(
        bsl::shared_ptr<ntctls::SessionContext>* result,
        ntca::EncryptionRole::Value              role,
        const ntca::EncryptionOptions&           options,
        bslma::Allocator*                        basicAllocator = 0);
};

/// Provide a map of contexts for authenticating SSL connections with specific
/// server names.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctls
class SessionManager
{
    typedef bsl::map<bsl::string, bsl::shared_ptr<ntctls::SessionContext> >
        Container;

    bslmt::Mutex                            d_mutex;
    ntca::EncryptionRole::Value             d_role;
    bsl::shared_ptr<ntctls::SessionContext> d_context_sp;
    Container                               d_container;
    bslma::Allocator*                       d_allocator_p;

    SessionManager(const SessionManager&) BSLS_KEYWORD_DELETED;
    SessionManager& operator=(const SessionManager&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new context map for the specified 'role'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit SessionManager(ntca::EncryptionRole::Value role,
                            bslma::Allocator*           basicAllocator = 0);

    /// Destroy this object.
    ~SessionManager();

    /// Configure the default context with the specified 'options'.
    ntsa::Error configure(const ntca::EncryptionOptions& options);

    /// Configure a context for the specified 'serverName' the specified
    /// 'options'. Return the error. Note that 'serverName' may be an IP
    /// address, domain name, or domain name wildcard such as "*.example.com".
    /// Also that an empty 'serverName' or a 'serverName' of "*" is interpreted
    /// as identifying the options for the default context.
    ntsa::Error configure(const bsl::string&             serverName,
                          const ntca::EncryptionOptions& options);

    /// Load into the specified 'result' the default context.
    ntsa::Error lookup(bsl::shared_ptr<ntctls::SessionContext>* result);

    /// Load into the specified 'result' the context for the specified
    /// 'serverName'. If the specified 'fallback' is true, fallback to the
    /// default context if no context associated with the 'serverName' is
    /// found. Return the error. Note that 'serverName' may be an IP address,
    /// domain name, or domain name wildcard such as "*.example.com". Also that
    /// an empty 'serverName' or a 'serverName' of "*" is interpreted as
    /// identifying the options for the default context.
    ntsa::Error lookup(bsl::shared_ptr<ntctls::SessionContext>* result,
                       const bsl::string&                       serverName,
                       bool                                     fallback);

    /// The role played by this context when establishing TLS sessions.
    ntca::EncryptionRole::Value role() const;

    /// Load into the specified 'result' a new client context map configured
    /// according to the specified 'options'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used. Return the error.
    static ntsa::Error createClientSessionManager(
        bsl::shared_ptr<ntctls::SessionManager>* result,
        const ntca::EncryptionClientOptions&     options,
        bslma::Allocator*                        basicAllocator = 0);

    /// Load into the specified 'result' a new client context map configured
    /// according to the specified 'options'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used. Return the error.
    static ntsa::Error createServerSessionManager(
        bsl::shared_ptr<ntctls::SessionManager>* result,
        const ntca::EncryptionServerOptions&     options,
        bslma::Allocator*                        basicAllocator = 0);
};

/// Provide a factory to create an encryptor in the client role.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctls
class SessionClient : public ntci::EncryptionClient
{
    bsl::shared_ptr<ntctls::SessionManager>   d_sessionManager_sp;
    bsl::shared_ptr<bdlbb::BlobBufferFactory> d_blobBufferFactory_sp;
    bslma::Allocator*                         d_allocator_p;

  private:
    SessionClient(const SessionClient&) BSLS_KEYWORD_DELETED;
    SessionClient& operator=(const SessionClient&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new client that establishes TLS session using the specified
    /// 'sessionManager'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    SessionClient(
        const bsl::shared_ptr<ntctls::SessionManager>&   sessionManager,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Destroy this object.
    ~SessionClient() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new client-side encryption
    /// session. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    ntsa::Error createEncryption(bsl::shared_ptr<ntci::Encryption>* result,
                                 bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new client-side encryption session.
    /// Allocate blob buffers from the specified 'dataPool'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used. Return the error.
    ntsa::Error createEncryption(
        bsl::shared_ptr<ntci::Encryption>*     result,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        bslma::Allocator*                        basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>*         result,
        const ntca::EncryptionClientOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    static ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator*                        basicAllocator = 0);
};

/// Provide a factory to create an encryptor in the server role.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctls
class SessionServer : public ntci::EncryptionServer
{
    bsl::shared_ptr<ntctls::SessionManager>   d_sessionManager_sp;
    bsl::shared_ptr<bdlbb::BlobBufferFactory> d_blobBufferFactory_sp;
    bslma::Allocator*                         d_allocator_p;

  private:
    SessionServer(const SessionServer&) BSLS_KEYWORD_DELETED;
    SessionServer& operator=(const SessionServer&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new server that establishes TLS session using the specified
    /// 'sessionManager'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    SessionServer(
        const bsl::shared_ptr<ntctls::SessionManager>&   sessionManager,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Destroy this object.
    ~SessionServer() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new server-side encryption session.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    ntsa::Error createEncryption(bsl::shared_ptr<ntci::Encryption>* result,
                                 bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new server-side encryption session.
    /// Allocate blob buffers from the specified 'dataPool'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used. Return the error.
    ntsa::Error createEncryption(
        bsl::shared_ptr<ntci::Encryption>*     result,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        bslma::Allocator*                        basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    static ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>*         result,
        const ntca::EncryptionServerOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator*                                basicAllocator = 0);

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    static ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator*                        basicAllocator = 0);
};

// Uncomment to implement reads from the SSL state machine using a new
// blob buffer allocated for each read (very inefficient).
// #define NTCTLS_SESSION_ALWAYS_ALLOCATE_NEW_BLOB_BUFFER 1

// Uncomment to log all state transitions from the TLS state machine.
#define NTCTLS_SESSION_LOG_CALLBACK_VERBOSE 1

// Uncomment to log all errors from the TLS state machine.
// #define NTCTLS_SESSION_LOG_ERROR_VERBOSE 1

#define NTCTLS_SESSION_LOG_ERROR(phrase)                                      \
    do {                                                                      \
        bsl::string description;                                              \
        ntctls::Internal::drainErrorQueue(&description);                      \
        if (!description.empty()) {                                           \
            NTCI_LOG_ERROR("%s: %s", phrase, description.c_str());            \
        }                                                                     \
        else {                                                                \
            NTCI_LOG_ERROR("%s", phrase);                                     \
        }                                                                     \
    } while (false)

#define NTCTLS_SESSION_LOG_DEBUG(phrase)                                      \
    do {                                                                      \
        bsl::string description;                                              \
        ntctls::Internal::drainErrorQueue(&description);                      \
        if (!description.empty()) {                                           \
            NTCI_LOG_TRACE("%s: %s", phrase, description.c_str());            \
        }                                                                     \
        else {                                                                \
            NTCI_LOG_TRACE("%s", phrase);                                     \
        }                                                                     \
    } while (false)

extern "C" int ntctls_context_sni_callback(SSL* ssl, int* al, void* userData)
{
    // SSL_TLSEXT_ERR_OK
    //     This is used to indicate that the servername requested by the client
    //     has been accepted. Typically a server will call SSL_set_SSL_CTX()
    //     in the callback to set up a different configuration for the selected
    //     servername in this case.
    //
    // SSL_TLSEXT_ERR_ALERT_FATAL
    //     In this case the servername requested by the client is not accepted
    //     and the handshake will be aborted. The value of the alert to be used
    //     should be stored in the location pointed to by the al parameter to
    //     the callback. By default this value is initialised to
    //     SSL_AD_UNRECOGNIZED_NAME.
    //
    // SSL_TLSEXT_ERR_NOACK
    //     This return value indicates that the servername is not accepted by
    //     the server. No alerts are sent and the server will not acknowledge
    //     the requested servername.

    NTCCFG_WARNING_UNUSED(userData);

    ntsa::Error error;

    *al = SSL_AD_UNRECOGNIZED_NAME;

    if (ssl == 0) {
        return SSL_TLSEXT_ERR_ALERT_FATAL;
    }

    const int serverNameType = SSL_get_servername_type(ssl);

    if (serverNameType == -1) {
        return SSL_TLSEXT_ERR_OK;
    }

    if (serverNameType != TLSEXT_NAMETYPE_host_name) {
        return SSL_TLSEXT_ERR_OK;
    }

    const char* serverNamePtr = SSL_get_servername(ssl, serverNameType);
    if (serverNamePtr == 0) {
        return SSL_TLSEXT_ERR_OK;
    }

    const bsl::size_t serverNameLength = bsl::strlen(serverNamePtr);
    if (serverNameLength == 0) {
        return SSL_TLSEXT_ERR_OK;
    }

    bsl::string serverName(serverNamePtr, serverNamePtr + serverNameLength);

    ntctls::Session* session = reinterpret_cast<ntctls::Session*>(
        SSL_get_ex_data(ssl, Internal::Impl::s_userDataIndex));

    if (session == 0) {
        return SSL_TLSEXT_ERR_ALERT_FATAL;
    }

    bool found = false;

    error = session->activate(&found, serverName);

    if (error) {
        return SSL_TLSEXT_ERR_ALERT_FATAL;
    }

    if (!found) {
        return SSL_TLSEXT_ERR_NOACK;
    }

    return SSL_TLSEXT_ERR_OK;
}

extern "C" int ntctls_context_verify_callback(X509_STORE_CTX* x509StoreCtx,
                                              void*           userData)
{
    // Verify the certificate chain according to the certificate store defined
    // in the specified 'parameters'.  Return 1 to indicate success (i.e., the
    // certificate is verified) and 0 to indicate verification failure.

    NTCCFG_WARNING_UNUSED(userData);

    NTCI_LOG_CONTEXT();

    enum { e_SUCCESS = 1, e_FAILURE = 0 };

    ntsa::Error error;

    if (x509StoreCtx == 0) {
        NTCTLS_SESSION_LOG_ERROR(
            "Failed to verify certificate: invalid certificate store context");
        return e_FAILURE;
    }

    SSL* ssl = reinterpret_cast<SSL*>(
        X509_STORE_CTX_get_ex_data(x509StoreCtx,
                                   SSL_get_ex_data_X509_STORE_CTX_idx()));
    if (ssl == 0) {
        NTCTLS_SESSION_LOG_ERROR(
            "Failed to verify certificate: invalid session");
        return e_FAILURE;
    }

    ntctls::Session* session = reinterpret_cast<ntctls::Session*>(
        SSL_get_ex_data(ssl, Internal::Impl::s_userDataIndex));
    if (session == 0) {
        NTCTLS_SESSION_LOG_ERROR(
            "Failed to verify certificate: invalid session");
        return e_FAILURE;
    }

    error = session->authenticate(x509StoreCtx);
    if (error) {
        return e_FAILURE;
    }

    return e_SUCCESS;
}

// This struct provides utilities for allocating blob buffers. This struct
// is thread safe.
struct BlobBufferUtil {
    // Return the number of bytes to allocate to accomodate a new read into a
    // read queue having the specified 'size' and 'capacity' to satisfy the
    // specified 'lowWatermark', ensuring at least the specified
    // 'minReceiveSize' but no more than the specified 'maxReceiveSize',
    // inclusive.
    static size_t calculateNumBytesToAllocate(size_t size,
                                              size_t capacity,
                                              size_t lowWatermark,
                                              size_t minReceiveSize,
                                              size_t maxReceiveSize);

    // Load more capacity buffers allocated from the specified
    // 'blobBufferFactory' into the specified 'readQueue' to accomodate a new
    // read into the unused capacity buffers of the 'readQueue' to satisfy the
    // specified 'lowWatermark', ensuring at least the specified 'minReadSize'
    // but no more than the specified 'maxReceiveSize', inclusive.
    static void reserveCapacity(bdlbb::Blob*              readQueue,
                                bdlbb::BlobBufferFactory* blobBufferFactory,
                                void*                     metrics,
                                size_t                    lowWatermark,
                                size_t                    minReceiveSize,
                                size_t                    maxReceiveSize);
};

size_t BlobBufferUtil::calculateNumBytesToAllocate(size_t size,
                                                   size_t capacity,
                                                   size_t lowWatermark,
                                                   size_t minReceiveSize,
                                                   size_t maxReceiveSize)
{
    BSLS_ASSERT(capacity >= size);
    BSLS_ASSERT(minReceiveSize > 0);
    BSLS_ASSERT(maxReceiveSize > 0);

    if (minReceiveSize > maxReceiveSize) {
        minReceiveSize = maxReceiveSize;
    }

    bsl::size_t numBytesToAllocate = 0;
    if (lowWatermark > capacity) {
        numBytesToAllocate = lowWatermark - capacity;
    }

    bsl::size_t numBytesToBeAvailable = (capacity - size) + numBytesToAllocate;

    if (numBytesToBeAvailable < minReceiveSize) {
        bsl::size_t numBytesToAdjust  = minReceiveSize - numBytesToBeAvailable;
        numBytesToAllocate           += numBytesToAdjust;
    }

    if (numBytesToBeAvailable > maxReceiveSize) {
        bsl::size_t numBytesToAdjust = numBytesToBeAvailable - maxReceiveSize;
        if (numBytesToAdjust > numBytesToAllocate) {
            numBytesToAllocate = 0;
        }
        else {
            numBytesToAllocate -= numBytesToAdjust;
        }
    }

    BSLS_ASSERT((capacity - size) + numBytesToAllocate >= 1);
    BSLS_ASSERT(numBytesToAllocate <= maxReceiveSize);

    return numBytesToAllocate;
}

void BlobBufferUtil::reserveCapacity(
    bdlbb::Blob*              readQueue,
    bdlbb::BlobBufferFactory* blobBufferFactory,
    void*                     metrics,
    size_t                    lowWatermark,
    size_t                    minReceiveSize,
    size_t                    maxReceiveSize)
{
    BSLS_ASSERT(minReceiveSize > 0);
    BSLS_ASSERT(maxReceiveSize > 0);

    size_t numBytesToAllocate =
        BlobBufferUtil::calculateNumBytesToAllocate(readQueue->length(),
                                                    readQueue->totalSize(),
                                                    lowWatermark,
                                                    minReceiveSize,
                                                    maxReceiveSize);

    bsl::size_t numBytesAllocated = 0;
    while (numBytesAllocated < numBytesToAllocate) {
        bdlbb::BlobBuffer buffer;
        blobBufferFactory->allocate(&buffer);

        bsl::size_t blobBufferCapacity = buffer.size();

        readQueue->appendBuffer(buffer);
        numBytesAllocated += blobBufferCapacity;

#if 1
        NTCCFG_WARNING_UNUSED(metrics);
#else
        if (metrics) {
            metrics->logBlobBufferAllocation(blobBufferCapacity);
        }
#endif
    }

    BSLS_ASSERT(static_cast<bsl::size_t>(readQueue->totalSize() -
                                         readQueue->length()) >=
                bsl::min(minReceiveSize, maxReceiveSize));
}

#if NTCTLS_SESSION_LOG_CHECK_ERROR_VERBOSE
#define NTCTLS_SESSION_LOG_CHECK_ERROR(message) NTCI_LOG_TRACE(message)
#else
#define NTCTLS_SESSION_LOG_CHECK_ERROR(message)
#endif

void Session::infoCallback(const SSL* ssl, int where, int ret)
{
    NTCCFG_WARNING_UNUSED(ssl);
    NTCCFG_WARNING_UNUSED(ret);

    NTCI_LOG_CONTEXT();

    const bsls::LogSeverity::Enum severity = bsls::Log::severityThreshold();
    if (static_cast<int>(severity) <
        static_cast<int>(bsls::LogSeverity::e_TRACE))
    {
        return;
    }

    if ((where & SSL_CB_HANDSHAKE_START) != 0) {
        NTCI_LOG_TRACE("SSL_CB_HANDSHAKE_START: %s",
                       SSL_state_string_long(ssl));
    }

    if ((where & SSL_CB_HANDSHAKE_DONE) != 0) {
        NTCI_LOG_TRACE("SSL_CB_HANDSHAKE_DONE: %s",
                       SSL_state_string_long(ssl));
    }

    if ((where & SSL_CB_LOOP) != 0) {
        NTCI_LOG_TRACE("SSL_CB_LOOP: %s", SSL_state_string_long(ssl));
    }

    if ((where & SSL_CB_EXIT) != 0) {
        NTCI_LOG_TRACE("SSL_CB_EXIT: %s", SSL_state_string_long(ssl));
    }

    if ((where & SSL_CB_READ) != 0) {
        NTCI_LOG_TRACE("SSL_CB_READ: %s", SSL_state_string_long(ssl));
    }

    if ((where & SSL_CB_WRITE) != 0) {
        NTCI_LOG_TRACE("SSL_CB_WRITE: %s", SSL_state_string_long(ssl));
    }

    if ((where & SSL_CB_ALERT) != 0) {
        NTCI_LOG_TRACE("SSL_CB_ALERT: %s (%s)",
                       SSL_state_string_long(ssl),
                       SSL_alert_desc_string_long(ret));
    }
}

bool Session::check(SSL* ssl, int result)
{
    int error = SSL_get_error(ssl, result);

    if (error == SSL_ERROR_NONE) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_NONE");
        return true;
    }

    if (error == SSL_ERROR_WANT_READ) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_WANT_READ");
        return true;
    }

    if (error == SSL_ERROR_WANT_WRITE) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_WANT_WRITE");
        return true;
    }

    if (error == SSL_ERROR_SYSCALL) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_SYSCALL");
        return false;
    }

    if (error == SSL_ERROR_SSL) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_SSL");
        return false;
    }

    if (error == SSL_ERROR_WANT_X509_LOOKUP) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_WANT_X509_LOOKUP");
        return false;
    }

    if (error == SSL_ERROR_ZERO_RETURN) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_ZERO_RETURN");
        return true;
    }

    if (error == SSL_ERROR_WANT_CONNECT) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_WANT_CONNECT");
        return false;
    }

    if (error == SSL_ERROR_WANT_ACCEPT) {
        NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_WANT_ACCEPT");
        return false;
    }

    NTCTLS_SESSION_LOG_CHECK_ERROR("SSL_ERROR_UNKNOWN");
    return false;
}

ntsa::Error Session::init()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    if (d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    {
        bsls::SpinLockGuard lock(&Internal::Impl::s_userDataIndexLock);
        if (!Internal::Impl::s_userDataIndexDefined) {
            Internal::Impl::s_userDataIndex =
                SSL_get_ex_new_index(0, 0, NULL, NULL, NULL);
            Internal::Impl::s_userDataIndexDefined = true;
        }
    }

    if (d_sessionManager_sp->role() == ntca::EncryptionRole::e_CLIENT) {
        bdlb::NullableValue<bsl::string> serverNameIndication =
            d_upgradeOptions.serverName();

        if (!serverNameIndication.isNull()) {
            error = d_sessionManager_sp->lookup(&d_context_sp,
                                                serverNameIndication.value(),
                                                true);
            if (error) {
                return error;
            }
        }
        else {
            error = d_sessionManager_sp->lookup(&d_context_sp);
            if (error) {
                return error;
            }
        }

        if (!d_context_sp) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_ssl_p = SSL_new(d_context_sp->native());
        if (!d_ssl_p) {
            bsl::string description;
            Internal::drainErrorQueue(&description);

            NTCI_LOG_TRACE("Failed to allocate SSL session: %s",
                           description.c_str());

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        rc = SSL_set_ex_data(d_ssl_p, Internal::Impl::s_userDataIndex, this);
        if (rc == 0) {
            bsl::string description;
            Internal::drainErrorQueue(&description);

            NTCI_LOG_TRACE("Failed to set SSL session user data: %s",
                           description.c_str());

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        SSL_set_connect_state(d_ssl_p);

        if (!serverNameIndication.isNull()) {
            rc =
                SSL_set_tlsext_host_name(d_ssl_p,
                                         serverNameIndication.value().c_str());
            if (rc == 0) {
                bsl::string description;
                Internal::drainErrorQueue(&description);

                NTCI_LOG_TRACE(
                    "Failed to set server name indication to '%s': %s",
                    serverNameIndication.value().c_str(),
                    description.c_str());

                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            d_serverNameIndication = serverNameIndication.value();
        }
    }
    else {
        error = d_sessionManager_sp->lookup(&d_context_sp);
        if (error) {
            return error;
        }

        if (!d_context_sp) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_ssl_p = SSL_new(d_context_sp->native());
        if (!d_ssl_p) {
            bsl::string description;
            Internal::drainErrorQueue(&description);

            NTCI_LOG_TRACE("Failed to allocate SSL session: %s",
                           description.c_str());

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        rc = SSL_set_ex_data(d_ssl_p, Internal::Impl::s_userDataIndex, this);
        if (rc == 0) {
            bsl::string description;
            Internal::drainErrorQueue(&description);

            NTCI_LOG_TRACE("Failed to set SSL session user data: %s",
                           description.c_str());

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        SSL_set_accept_state(d_ssl_p);
    }

    SSL_set_quiet_shutdown(d_ssl_p, 0);

    ntctls::Handle<BIO> incomingStream(
        ntctls::Internal::createStreamRaw(&d_incomingCipherText));

    if (!incomingStream) {
        bsl::string description;
        Internal::drainErrorQueue(&description);

        NTCI_LOG_TRACE("Failed to allocate incoming stream BIO: %s",
                       description.c_str());

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntctls::Handle<BIO> outgoingStream(
        ntctls::Internal::createStreamRaw(&d_outgoingCipherText));

    if (!outgoingStream) {
        bsl::string description;
        Internal::drainErrorQueue(&description);

        NTCI_LOG_TRACE("Failed to allocate outgoing stream BIO: %s",
                       description.c_str());

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    BIO_set_nbio(incomingStream.get(), 1);
    BIO_set_nbio(outgoingStream.get(), 1);

    SSL_set_bio(d_ssl_p, incomingStream.release(), outgoingStream.release());

    SSL_set_info_callback(d_ssl_p, &infoCallback);

    return ntsa::Error();
}

ntsa::Error Session::process(LockGuard* lock)
{
    NTCI_LOG_CONTEXT();

    if (!d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool announceHandshakeComplete = false;

    if (SSL_in_init(d_ssl_p)) {
        int rc = SSL_do_handshake(d_ssl_p);
        if (rc != 1) {
            if (rc < 0) {
                int error = SSL_get_error(d_ssl_p, rc);
                if (error == SSL_ERROR_WANT_READ ||
                    error == SSL_ERROR_WANT_WRITE)
                {
                    return ntsa::Error();
                }
            }

            bsl::string description;
            Internal::drainErrorQueue(&description);

            NTCI_LOG_TRACE("Failed to complete TLS handshake: %s",
                           description.c_str());

            ntci::Encryption::HandshakeCallback handshakeCallback =
                d_handshakeCallback;

            d_handshakeCallback = ntci::Encryption::HandshakeCallback();

            lock->release()->unlock();

            handshakeCallback(
                ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED),
                bsl::shared_ptr<ntci::EncryptionCertificate>(),
                description);

            return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
        }

        rc = SSL_is_init_finished(d_ssl_p);
        BSLS_ASSERT(rc == 1);

        OSSL_HANDSHAKE_STATE state = SSL_get_state(d_ssl_p);
        BSLS_ASSERT(state == TLS_ST_OK);
        NTCCFG_WARNING_UNUSED(state);

        X509* sourceX509 = SSL_get_certificate(d_ssl_p);
        if (sourceX509) {
            X509_up_ref(sourceX509);
            d_sourceCertificate_sp.createInplace(d_allocator_p,
                                                 sourceX509,
                                                 d_allocator_p);
        }

        X509* remoteX509 = SSL_get_peer_certificate(d_ssl_p);
        if (remoteX509) {
            d_remoteCertificate_sp.createInplace(d_allocator_p,
                                                 remoteX509,
                                                 d_allocator_p);
        }

        if (d_outgoingLeftovers.length() > 0) {
            bdlbb::BlobUtil::append(&d_outgoingPlainText, d_outgoingLeftovers);
            d_outgoingLeftovers.removeAll();
        }

        announceHandshakeComplete = true;
    }

    if (d_outgoingPlainText.length() > 0) {
        const int numOutgoingPlainTextBuffers =
            d_outgoingPlainText.numDataBuffers();

        int numOutgoingPlainTextBytesWritten = 0;

        for (int i = 0; i < numOutgoingPlainTextBuffers; ++i) {
            const bdlbb::BlobBuffer& outgoingPlainTextBuffer =
                d_outgoingPlainText.buffer(i);

            const char* outgoingPlainTextBufferData =
                outgoingPlainTextBuffer.data();

            const int outgoingPlainTextBufferSize =
                i == numOutgoingPlainTextBuffers - 1
                    ? d_outgoingPlainText.lastDataBufferLength()
                    : outgoingPlainTextBuffer.size();

            const int n = SSL_write(d_ssl_p,
                                    outgoingPlainTextBufferData,
                                    outgoingPlainTextBufferSize);

            if (!check(d_ssl_p, n)) {
                bsl::string description;
                Internal::drainErrorQueue(&description);

                NTCI_LOG_TRACE("Failed to write outgoing data: %s",
                               description.c_str());

                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            if (n > 0) {
                numOutgoingPlainTextBytesWritten += n;
            }

            if (n == 0 || n != outgoingPlainTextBufferSize) {
                break;
            }
        }

        if (numOutgoingPlainTextBytesWritten > 0) {
            bdlbb::BlobUtil::erase(&d_outgoingPlainText,
                                   0,
                                   numOutgoingPlainTextBytesWritten);
        }
    }

    while (true) {
#if NTCTLS_SESSION_ALWAYS_ALLOCATE_NEW_BLOB_BUFFER
        bdlbb::BlobBuffer incomingPlainTextBuffer;
        d_blobBufferFactory_sp->allocate(&incomingPlainTextBuffer);

        const int incomingPlainTextBufferCapacity =
            incomingPlainTextBuffer.size();

        const int n = SSL_read(d_ssl_p,
                               incomingPlainTextBuffer.data(),
                               incomingPlainTextBufferCapacity);
#else
        if (d_incomingPlainText.length() == d_incomingPlainText.totalSize()) {
            BlobBufferUtil::reserveCapacity(
                &d_incomingPlainText,
                d_blobBufferFactory_sp.get(),
                0,
                0,
                NTCCFG_DEFAULT_STREAM_SOCKET_MIN_INCOMING_TRANSFER_SIZE,
                NTCCFG_DEFAULT_STREAM_SOCKET_MAX_INCOMING_TRANSFER_SIZE);
        }

        ntsa::MutableBufferSequence<ntsa::MutableBuffer> mutableBufferSequence(
            &d_incomingPlainText);

        ntsa::MutableBufferSequence<ntsa::MutableBuffer>::Iterator
            mutableBufferSequenceCurrent = mutableBufferSequence.begin();

        ntsa::MutableBufferSequence<ntsa::MutableBuffer>::Iterator
            mutableBufferSequenceEnd = mutableBufferSequence.end();

        BSLS_ASSERT(mutableBufferSequenceCurrent != mutableBufferSequenceEnd);

        ntsa::MutableBuffer mutableBuffer = *mutableBufferSequenceCurrent;

        void* incomingPlainTextBufferData = mutableBuffer.data();

        int incomingPlainTextBufferCapacity;
        if (mutableBuffer.size() <=
            NTCCFG_WARNING_PROMOTE(bsl::size_t,
                                   bsl::numeric_limits<int>::max()))
        {
            incomingPlainTextBufferCapacity =
                NTCCFG_WARNING_NARROW(int, mutableBuffer.size());
        }
        else {
            incomingPlainTextBufferCapacity = bsl::numeric_limits<int>::max();
        }

        const int n = SSL_read(d_ssl_p,
                               incomingPlainTextBufferData,
                               incomingPlainTextBufferCapacity);
#endif

        if (!check(d_ssl_p, n)) {
            bsl::string description;
            Internal::drainErrorQueue(&description);

            NTCI_LOG_TRACE("Failed to read incoming data: %s",
                           description.c_str());

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (n <= 0) {
            BSLS_ASSERT(SSL_get_error(d_ssl_p, n) == SSL_ERROR_WANT_READ ||
                        SSL_get_error(d_ssl_p, n) == SSL_ERROR_WANT_WRITE ||
                        SSL_get_error(d_ssl_p, n) == SSL_ERROR_ZERO_RETURN);
            break;
        }

        BSLS_ASSERT(n > 0);
        BSLS_ASSERT(n <= incomingPlainTextBufferCapacity);
        BSLS_ASSERT(SSL_get_error(d_ssl_p, n) == SSL_ERROR_NONE);

#if NTCTLS_SESSION_ALWAYS_ALLOCATE_NEW_BLOB_BUFFER
        incomingPlainTextBuffer.setSize(n);
        d_incomingPlainText.appendDataBuffer(incomingPlainTextBuffer);
#else
        d_incomingPlainText.setLength(d_incomingPlainText.length() + n);
#endif
    }

    if (announceHandshakeComplete) {
        ntci::Encryption::HandshakeCallback handshakeCallback =
            d_handshakeCallback;

        bsl::shared_ptr<BloombergLP::ntctls::Certificate> remoteCertificate =
            d_remoteCertificate_sp;

        d_handshakeCallback = ntci::Encryption::HandshakeCallback();

        if (handshakeCallback) {
            lock->release()->unlock();
            handshakeCallback(ntsa::Error(), remoteCertificate, "");
        }
    }

    return ntsa::Error();
}

ntsa::Error Session::analyzeIncoming()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::size_t numBytesDecoded = 0;
    bsl::size_t numBytesLeftover = 0;
    {
        bdlbb::InBlobStreamBuf isb(&d_incomingCipherText);

        bsl::size_t incomingCipherTextLength = 
                    static_cast<bsl::size_t>(d_incomingCipherText.length());
        
        while (numBytesDecoded < incomingCipherTextLength) {
            ntctls::TlsRecordHeader record;
            error = record.decode(&numBytesDecoded, &isb);
            if (error) {
                bdlbb::Blob leftoverData = d_incomingCipherText;

                if (incomingCipherTextLength > numBytesDecoded) {
                    numBytesLeftover = 
                        incomingCipherTextLength - numBytesDecoded;
                }

                bdlbb::BlobUtil::erase(
                    &leftoverData, 
                    static_cast<int>(0), 
                    static_cast<int>(numBytesDecoded));

                BSLS_ASSERT(static_cast<bsl::size_t>(leftoverData.length()) == 
                            numBytesLeftover);

                NTCI_LOG_STREAM_TRACE 
                    << "Invalid TLS record (kept "
                    << numBytesDecoded
                    << " bytes, leftover " 
                    << leftoverData.length() 
                    << " bytes):\n" 
                    << bdlbb::BlobUtilHexDumper(&leftoverData, 4096)
                    << NTCI_LOG_STREAM_END;

                if (d_upgradeOptions.keepIncomingLeftovers().value_or(false)) {
                    bdlbb::BlobUtil::append(&d_incomingLeftovers, 
                                            leftoverData);
                }
                
                break;
            }

            NTCI_LOG_STREAM_TRACE << "Incoming TLS record " << record 
                                  << NTCI_LOG_STREAM_END;

            isb.pubseekoff(
                static_cast<bsl::streamoff>(record.length()), 
                bsl::ios_base::cur, 
                bsl::ios_base::in);

            numBytesDecoded += record.length();
        }
    }

    if (numBytesLeftover > 0) {
        BSLS_ASSERT(
            numBytesDecoded + numBytesLeftover <= 
            static_cast<bsl::size_t>(d_incomingCipherText.length()));

        bdlbb::BlobUtil::erase(
            &d_incomingCipherText, 
            static_cast<int>(numBytesDecoded),
            static_cast<int>(numBytesLeftover));
    }

    return ntsa::Error();
}

ntsa::Error Session::analyzeOutgoing()
{
    return ntsa::Error();
}

Session::Session(
    const bsl::shared_ptr<ntctls::SessionManager>&   sessionManager,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
: d_mutex()
, d_context_sp()
, d_sessionManager_sp(sessionManager)
, d_blobBufferFactory_sp(blobBufferFactory)
, d_sourceCertificate_sp()
, d_remoteCertificate_sp()
, d_serverNameIndication(basicAllocator)
, d_ssl_p(0)
, d_incomingCipherText(blobBufferFactory.get(), basicAllocator)
, d_outgoingCipherText(blobBufferFactory.get(), basicAllocator)
, d_incomingPlainText(blobBufferFactory.get(), basicAllocator)
, d_outgoingPlainText(blobBufferFactory.get(), basicAllocator)
, d_incomingLeftovers(blobBufferFactory.get(), basicAllocator)
, d_outgoingLeftovers(blobBufferFactory.get(), basicAllocator)
, d_upgradeOptions(basicAllocator)
, d_handshakeCallback(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Session::~Session()
{
    if (d_ssl_p) {
        SSL_free(d_ssl_p);
    }
}

ntsa::Error Session::activate(bool* found, const bsl::string& serverName)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    *found = false;

    if (d_context_sp->role() == ntca::EncryptionRole::e_CLIENT) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_ssl_p == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::shared_ptr<ntctls::SessionContext> context;
    error = d_sessionManager_sp->lookup(&context, serverName, false);
    if (error) {
        error = d_sessionManager_sp->lookup(&context);
        if (error) {
            NTCI_LOG_TRACE("Failed to find server name '%s'",
                           serverName.c_str());
            return error;
        }
    }
    else {
        *found = true;
    }

    if (context != d_context_sp) {
        SSL_CTX* newSslCtx = SSL_set_SSL_CTX(d_ssl_p, context->native());
        if (newSslCtx == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_context_sp = context;
    }

    return ntsa::Error();
}

ntsa::Error Session::authenticate(X509_STORE_CTX* x509StoreCtx)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    if (x509StoreCtx == 0) {
        NTCI_LOG_ERROR("Failed to verify certificate: "
                       "invalid certificate store context");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    X509_VERIFY_PARAM* param = X509_STORE_CTX_get0_param(x509StoreCtx);
    if (param == 0) {
        NTCI_LOG_ERROR("Failed to verify certificate: "
                       "invalid certificate store context parameters");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bdlb::NullableValue<ntca::EncryptionValidation>& validation =
        d_upgradeOptions.validation().has_value()
            ? d_upgradeOptions.validation()
            : d_context_sp->validation();

    error = ntctls::SessionUtil::configure(param, validation);
    if (error) {
        NTCI_LOG_ERROR("Failed to verify certificate: "
                       "failed to set parameters");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = X509_verify_cert(x509StoreCtx);

    int errorCode = X509_STORE_CTX_get_error(x509StoreCtx);
    int depth     = X509_STORE_CTX_get_error_depth(x509StoreCtx);

    X509* x509 = X509_STORE_CTX_get_current_cert(x509StoreCtx);

    if (x509 != 0 && depth == 0) {
        if (rc == 1) {
            error = this->authenticate(x509StoreCtx, x509);
            if (error) {
                errorCode = X509_V_ERR_APPLICATION_VERIFICATION;
                X509_STORE_CTX_set_error(x509StoreCtx, errorCode);
                error = ntsa::Error();
            }
        }

        if (errorCode == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT) {
            bool allowSelfSigned = false;
            if (validation.has_value()) {
                if (validation.value().allowSelfSigned().has_value()) {
                    allowSelfSigned =
                        validation.value().allowSelfSigned().value();
                }
            }

            if (allowSelfSigned) {
                errorCode = X509_V_OK;
                X509_STORE_CTX_set_error(x509StoreCtx, errorCode);
                error = ntsa::Error();
            }
        }
    }

    if (errorCode != X509_V_OK) {
        bsl::string description;
        ntctls::Internal::drainErrorQueue(&description);

        NTCI_LOG_TRACE("Failed to verify certificate: %s: %s",
                       X509_verify_cert_error_string(errorCode),
                       description.c_str());

        return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
    }

    return ntsa::Error();
}

ntsa::Error Session::authenticate(X509_STORE_CTX* x509StoreCtx, X509* x509)
{
    NTCI_LOG_CONTEXT();

    if (x509StoreCtx == 0) {
        NTCI_LOG_ERROR(
            "Failed to verify certificate: invalid certificate store context");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (x509 == 0) {
        NTCTLS_SESSION_LOG_ERROR(
            "Failed to verify certificate: invalid certificate");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bdlb::NullableValue<ntca::EncryptionValidation>& validation =
        d_upgradeOptions.validation().has_value()
            ? d_upgradeOptions.validation()
            : d_context_sp->validation();

    if (validation.has_value()) {
        if (validation.value().callback().has_value()) {
            // MRM: Review docs.
            // X509_STORE_CTX_get1_certs
            // X509_STORE_CTX_get1_chain

            bsl::vector<ntca::EncryptionCertificate> certificateVector;
            {
                STACK_OF(X509)* chain =
                    X509_STORE_CTX_get1_chain(x509StoreCtx);
                if (chain != 0) {
                    const int chainSize = sk_X509_num(chain);
                    certificateVector.resize(chainSize);

                    for (int i = 0; i < chainSize; ++i) {
                        X509* x509CA = sk_X509_pop(chain);

                        bsl::shared_ptr<ntctls::Certificate> ca;
                        ca.createInplace(d_allocator_p, x509CA, d_allocator_p);

                        certificateVector[chainSize - i - 1] = ca->record();
                    }

                    sk_X509_free(chain);
                }
            }

            ntca::EncryptionCertificate certificate;
            {
                bsl::shared_ptr<ntctls::Certificate> nativeCertificate;
                nativeCertificate.createInplace(d_allocator_p,
                                                X509_dup(x509),
                                                d_allocator_p);


                certificate = nativeCertificate->record();
            }

            // for (bsl::size_t i = 0; i < certificateVector.size(); ++i) {
            //     NTCI_LOG_STREAM_DEBUG << "Certificate chain ["
            //                           << i
            //                           << "] = "
            //                           << certificateVector[i]
            //                           << NTCI_LOG_STREAM_END;
            // }

            const ntca::EncryptionCertificateValidator& validator =
                validation.value().callback().value();

            const bool isValid = validator(certificate);

            if (!isValid) {
                bsl::stringstream ss;
                ss << "Failed to verify the authenticity of "
                   << certificate;

                NTCI_LOG_TRACE("%s", ss.str().c_str());

                return ntsa::Error(ntsa::Error::e_NOT_AUTHORIZED);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error Session::initiateHandshake(const HandshakeCallback& callback)
{
    return this->initiateHandshake(ntca::UpgradeOptions(), callback);
}

ntsa::Error Session::initiateHandshake(
    const ntca::UpgradeOptions& upgradeOptions,
    const HandshakeCallback&    callback)
{
    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    ntsa::Error error;
    int         rc;

    d_upgradeOptions = upgradeOptions;

    if (!d_ssl_p) {
        error = this->init();
        if (error) {
            return error;
        }
    }
    else {
        rc = SSL_clear(d_ssl_p);
        if (rc != 1) {
            bsl::string description;
            Internal::drainErrorQueue(&description);

            NTCI_LOG_TRACE("Failed to reset the TLS session for reuse: %s",
                           description.c_str());

            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        BSLS_ASSERT(d_incomingCipherText.length() == 0);
        BSLS_ASSERT(d_incomingPlainText.length() == 0);
        BSLS_ASSERT(d_outgoingCipherText.length() == 0);
        BSLS_ASSERT(d_outgoingPlainText.length() == 0);
    }

    d_handshakeCallback = callback;

    rc = SSL_do_handshake(d_ssl_p);

    if (rc != 1) {
        if (rc < 0) {
            int error = SSL_get_error(d_ssl_p, rc);

            if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE)
            {
                return this->process(&lock);
            }
        }

        bsl::string description;
        Internal::drainErrorQueue(&description);

        NTCI_LOG_TRACE("Failed to initiate TLS handshake: %s",
                       description.c_str());

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Session::pushIncomingCipherText(const bdlbb::Blob& input)
{
    ntsa::Error error;

    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if ((SSL_get_shutdown(d_ssl_p) & k_TLS_SHUTDOWN_RECEIVED) != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_incomingLeftovers.length() > 0) {
        bdlbb::BlobUtil::append(&d_incomingLeftovers, input);
        return ntsa::Error();
    }

    bdlbb::BlobUtil::append(&d_incomingCipherText, input);

    if (d_upgradeOptions.keepIncomingLeftovers().value_or(false)) {
        error = this->analyzeIncoming();
        if (error) {
            return error;
        }
    }

    return this->process(&lock);
}

ntsa::Error Session::pushIncomingCipherText(const ntsa::Data& input)
{
    ntsa::Error error;

    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if ((SSL_get_shutdown(d_ssl_p) & k_TLS_SHUTDOWN_RECEIVED) != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_incomingLeftovers.length() > 0) {
        ntsa::DataUtil::append(&d_incomingLeftovers, input);
        return ntsa::Error();
    }

    ntsa::DataUtil::append(&d_incomingCipherText, input);

    if (d_upgradeOptions.keepIncomingLeftovers().value_or(false)) {
        error = this->analyzeIncoming();
        if (error) {
            return error;
        }
    }

    return this->process(&lock);
}

ntsa::Error Session::pushOutgoingPlainText(const bdlbb::Blob& input)
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if ((SSL_get_shutdown(d_ssl_p) & k_TLS_SHUTDOWN_SENT) != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_UNLIKELY(
            d_upgradeOptions.keepOutgoingLeftovers().value_or(false) &&
            SSL_in_init(d_ssl_p)))
    {
        bdlbb::BlobUtil::append(&d_outgoingLeftovers, input);
    }
    else {
        bdlbb::BlobUtil::append(&d_outgoingPlainText, input);
    }

    return this->process(&lock);
}

ntsa::Error Session::pushOutgoingPlainText(const ntsa::Data& input)
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if ((SSL_get_shutdown(d_ssl_p) & k_TLS_SHUTDOWN_SENT) != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_UNLIKELY(
            d_upgradeOptions.keepOutgoingLeftovers().value_or(false) &&
            SSL_in_init(d_ssl_p)))
    {
        if (input.isFile()) {
            bsl::size_t inputSize = input.size();
            bsl::size_t outputSize =
                ntsa::DataUtil::append(&d_outgoingLeftovers, input);
            if (inputSize != outputSize) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            ntsa::DataUtil::append(&d_outgoingLeftovers, input);
        }
    }
    else {
        if (input.isFile()) {
            bsl::size_t inputSize = input.size();
            bsl::size_t outputSize =
                ntsa::DataUtil::append(&d_outgoingPlainText, input);
            if (inputSize != outputSize) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            ntsa::DataUtil::append(&d_outgoingPlainText, input);
        }
    }

    return this->process(&lock);
}

ntsa::Error Session::popIncomingPlainText(bdlbb::Blob* output)
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_incomingPlainText.length() > 0) {
        bdlbb::BlobUtil::append(output, d_incomingPlainText);
        bdlbb::BlobUtil::erase(&d_incomingPlainText,
                               0,
                               d_incomingPlainText.length());
    }

    return this->process(&lock);
}

ntsa::Error Session::popOutgoingCipherText(bdlbb::Blob* output)
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_outgoingCipherText.length() > 0) {
        bdlbb::BlobUtil::append(output, d_outgoingCipherText);
        bdlbb::BlobUtil::erase(&d_outgoingCipherText,
                               0,
                               d_outgoingCipherText.length());
    }

    return this->process(&lock);
}

ntsa::Error Session::popIncomingLeftovers(bdlbb::Blob* output)
{
    LockGuard lock(&d_mutex);

    if (d_incomingLeftovers.length() > 0) {
        bdlbb::BlobUtil::append(output, d_incomingLeftovers);
        bdlbb::BlobUtil::erase(&d_incomingLeftovers,
                               0,
                               d_incomingLeftovers.length());
    }

    return ntsa::Error();
}

ntsa::Error Session::popOutgoingLeftovers(bdlbb::Blob* output)
{
    LockGuard lock(&d_mutex);

    if (d_outgoingLeftovers.length() > 0) {
        bdlbb::BlobUtil::append(output, d_outgoingLeftovers);
        bdlbb::BlobUtil::erase(&d_outgoingLeftovers,
                               0,
                               d_outgoingLeftovers.length());
    }

    return ntsa::Error();
}

ntsa::Error Session::shutdown()
{
    NTCI_LOG_CONTEXT();

    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if ((SSL_get_shutdown(d_ssl_p) & k_TLS_SHUTDOWN_SENT) != 0) {
        // Redundant shutdown is not an error.
        return ntsa::Error();
    }

    enum { k_SHUTDOWN_STARTING = 0, k_SHUTDOWN_COMPLETE = 1 };

    int rc = SSL_shutdown(d_ssl_p);

    if (rc != k_SHUTDOWN_STARTING && rc != k_SHUTDOWN_COMPLETE) {
        bsl::string description;
        Internal::drainErrorQueue(&description);

        NTCI_LOG_TRACE("Failed to initiate TLS shutdown: %s",
                       description.c_str());

        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return this->process(&lock);
}

bool Session::hasIncomingPlainText() const
{
    LockGuard lock(&d_mutex);

    return d_incomingPlainText.length() > 0;
}

bool Session::hasOutgoingCipherText() const
{
    LockGuard lock(&d_mutex);

    return d_outgoingCipherText.length() > 0;
}

bool Session::hasIncomingLeftovers() const
{
    LockGuard lock(&d_mutex);

    return d_incomingLeftovers.length() > 0;
}

bool Session::hasOutgoingLeftovers() const
{
    LockGuard lock(&d_mutex);

    return d_outgoingLeftovers.length() > 0;
}

bool Session::getSourceCertificate(ntca::EncryptionCertificate* result) const
{
    LockGuard lock(&d_mutex);

    if (d_sourceCertificate_sp) {
        d_sourceCertificate_sp->unwrap(result);
        return true;
    }

    return false;
}

bool Session::getSourceCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result) const
{
    LockGuard lock(&d_mutex);

    if (d_sourceCertificate_sp) {
        *result = d_sourceCertificate_sp;
        return true;
    }

    return false;
}

bool Session::getRemoteCertificate(ntca::EncryptionCertificate* result) const
{
    LockGuard lock(&d_mutex);

    if (d_remoteCertificate_sp) {
        d_remoteCertificate_sp->unwrap(result);
        return true;
    }

    return false;
}

bool Session::getRemoteCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result) const
{
    LockGuard lock(&d_mutex);

    if (d_remoteCertificate_sp) {
        *result = d_remoteCertificate_sp;
        return true;
    }

    return false;
}

bool Session::getCipher(bsl::string* result) const
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return false;
    }

    const SSL_CIPHER* cipher = SSL_get_current_cipher(d_ssl_p);
    if (!cipher) {
        return false;
    }

    char buffer[256];
    SSL_CIPHER_description(cipher, buffer, sizeof buffer);

    bsl::size_t size = bsl::strlen(buffer);

    const char* current = buffer;
    const char* end     = buffer + size;
    bool        space   = false;

    result->clear();
    result->reserve(size);

    while (current != end) {
        char ch = *current++;
        if (bdlb::CharType::isSpace(ch)) {
            if (ch != '\n' && !space) {
                result->push_back(ch);
                space = true;
            }
        }
        else {
            result->push_back(ch);
            space = false;
        }
    }

    return true;
}

bool Session::isHandshakeFinished() const
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return false;
    }

    return static_cast<bool>(SSL_is_init_finished(d_ssl_p));
}

bool Session::isShutdownSent() const
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return false;
    }

    int shutdownState = SSL_get_shutdown(d_ssl_p);

    if ((shutdownState & k_TLS_SHUTDOWN_SENT) != 0) {
        return true;
    }

    return false;
}

bool Session::isShutdownReceived() const
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return false;
    }

    int shutdownState = SSL_get_shutdown(d_ssl_p);

    if ((shutdownState & k_TLS_SHUTDOWN_RECEIVED) != 0) {
        return true;
    }

    return false;
}

bool Session::isShutdownFinished() const
{
    LockGuard lock(&d_mutex);

    if (!d_ssl_p) {
        return false;
    }

    int shutdownState = SSL_get_shutdown(d_ssl_p);

    if ((shutdownState & k_TLS_SHUTDOWN_SENT) == 0) {
        return false;
    }

    if ((shutdownState & k_TLS_SHUTDOWN_RECEIVED) == 0) {
        return false;
    }

    return true;
}

bsl::shared_ptr<ntci::EncryptionCertificate> Session::sourceCertificate() const
{
    LockGuard lock(&d_mutex);
    return d_sourceCertificate_sp;
}

bsl::shared_ptr<ntci::EncryptionCertificate> Session::remoteCertificate() const
{
    LockGuard lock(&d_mutex);
    return d_remoteCertificate_sp;
}

bsl::shared_ptr<ntci::EncryptionKey> Session::privateKey() const
{
    return bsl::shared_ptr<ntci::EncryptionKey>();
}

ntsa::Error Session::serverNameIndication(bsl::string* result) const
{
    LockGuard lock(&d_mutex);

    result->clear();

    if (!d_serverNameIndication.isNull()) {
        *result = d_serverNameIndication.value();
    }

    return ntsa::Error(ntsa::Error::e_EOF);
}

ntsa::Error SessionUtil::configureHost(
    X509_VERIFY_PARAM*                                      parameters,
    const ntca::EncryptionValidation::NullableStringVector& hostVector)
{
    if (hostVector.has_value()) {
        return ntctls::SessionUtil::configureHost(parameters,
                                                  hostVector.value());
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error SessionUtil::configureHost(
    X509_VERIFY_PARAM*                              parameters,
    const ntca::EncryptionValidation::StringVector& hostVector)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    for (bsl::size_t i = 0; i < hostVector.size(); ++i) {
        const bsl::string& text = hostVector[i];

        ntsa::IpAddress ipAddress;
        if (ipAddress.parse(text)) {
            if (ipAddress.isV4()) {
                unsigned char buffer[4];
                ipAddress.v4().copyTo(buffer, sizeof buffer);
                rc = X509_VERIFY_PARAM_set1_ip(parameters,
                                               buffer,
                                               sizeof buffer);
            }
            else if (ipAddress.isV6()) {
                unsigned char buffer[16];
                ipAddress.v6().copyTo(buffer, sizeof buffer);
                rc = X509_VERIFY_PARAM_set1_ip(parameters,
                                               buffer,
                                               sizeof buffer);
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            if (rc == 0) {
                NTCTLS_SESSION_LOG_ERROR(
                    "Failed to add IP address verification");
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            rc = X509_VERIFY_PARAM_add1_host(parameters,
                                             text.c_str(),
                                             text.size());

            if (rc == 0) {
                NTCTLS_SESSION_LOG_ERROR(
                    "Failed to add domain name verification");
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            rc = X509_VERIFY_PARAM_set_flags(
                parameters,
                X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
            if (rc == 0) {
                NTCTLS_SESSION_LOG_ERROR("Failed to set verification flags");
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error SessionUtil::configureMail(
    X509_VERIFY_PARAM*                                      parameters,
    const ntca::EncryptionValidation::NullableStringVector& mailVector)
{
    if (mailVector.has_value()) {
        return ntctls::SessionUtil::configureMail(parameters,
                                                  mailVector.value());
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error SessionUtil::configureMail(
    X509_VERIFY_PARAM*                              parameters,
    const ntca::EncryptionValidation::StringVector& mailVector)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    for (bsl::size_t i = 0; i < mailVector.size(); ++i) {
        const bsl::string& text = mailVector[i];

        rc = X509_VERIFY_PARAM_set1_email(parameters,
                                          text.c_str(),
                                          text.size());
        if (rc == 0) {
            NTCTLS_SESSION_LOG_ERROR("Failed to add email verification");
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error SessionUtil::configureUsage(
    X509_VERIFY_PARAM*                                     parameters,
    const ntca::EncryptionValidation::NullableUsageVector& usageVector)
{
    if (usageVector.has_value()) {
        return ntctls::SessionUtil::configureUsage(parameters,
                                                   usageVector.value());
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error SessionUtil::configureUsage(
    X509_VERIFY_PARAM*                             parameters,
    const ntca::EncryptionValidation::UsageVector& usageVector)
{
    NTCCFG_WARNING_UNUSED(parameters);
    NTCCFG_WARNING_UNUSED(usageVector);

    return ntsa::Error();
}

ntsa::Error SessionUtil::configureUsage(
    X509_VERIFY_PARAM*                                       parameters,
    const ntca::EncryptionValidation::NullableUsageExtended& usageExtended)
{
    if (usageExtended.has_value()) {
        return ntctls::SessionUtil::configureUsage(parameters,
                                                   usageExtended.value());
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error SessionUtil::configureUsage(
    X509_VERIFY_PARAM*                                        parameters,
    const ntca::EncryptionCertificateSubjectKeyUsageExtended& usageExtended)
{
    NTCCFG_WARNING_UNUSED(parameters);
    NTCCFG_WARNING_UNUSED(usageExtended);

    return ntsa::Error();
}

ntsa::Error SessionUtil::configure(
    X509_VERIFY_PARAM*                                     parameters,
    const bdlb::NullableValue<ntca::EncryptionValidation>& validation)
{
    if (validation.has_value()) {
        return ntctls::SessionUtil::configure(parameters, validation.value());
    }
    else {
        return ntsa::Error();
    }
}

ntsa::Error SessionUtil::configure(
    X509_VERIFY_PARAM*                parameters,
    const ntca::EncryptionValidation& validation)
{
    ntsa::Error error;

    error = ntctls::SessionUtil::configureHost(parameters, validation.host());
    if (error) {
        return error;
    }

    error = ntctls::SessionUtil::configureMail(parameters, validation.mail());
    if (error) {
        return error;
    }

    error =
        ntctls::SessionUtil::configureUsage(parameters, validation.usage());
    if (error) {
        return error;
    }

    error = ntctls::SessionUtil::configureUsage(parameters,
                                                validation.usageExtensions());
    if (error) {
        return error;
    }

    return ntsa::Error();
}

SessionContext::SessionContext(bslma::Allocator* basicAllocator)
: d_context()
, d_certificate_sp()
, d_authorities(basicAllocator)
, d_intermediaries(basicAllocator)
, d_role(ntca::EncryptionRole::e_CLIENT)
, d_minMethod(ntca::EncryptionMethod::e_DEFAULT)
, d_maxMethod(ntca::EncryptionMethod::e_DEFAULT)
, d_authentication(ntca::EncryptionAuthentication::e_DEFAULT)
, d_validation(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SessionContext::~SessionContext()
{
}

ntsa::Error SessionContext::configure(ntca::EncryptionRole::Value    role,
                                      const ntca::EncryptionOptions& options)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    if (d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_role           = role;
    d_minMethod      = options.minMethod();
    d_maxMethod      = options.maxMethod();
    d_authentication = options.authentication();
    d_validation     = options.validation();

    if (d_minMethod == ntca::EncryptionMethod::e_DEFAULT) {
        d_minMethod = ntca::EncryptionMethod::e_TLS_V1_2;
    }

    if (d_maxMethod == ntca::EncryptionMethod::e_DEFAULT) {
        d_maxMethod = ntca::EncryptionMethod::e_TLS_V1_X;
    }

    if (d_authentication == ntca::EncryptionAuthentication::e_DEFAULT) {
        if (d_role == ntca::EncryptionRole::e_CLIENT) {
            d_authentication = ntca::EncryptionAuthentication::e_VERIFY;
        }
        else if (d_role == ntca::EncryptionRole::e_SERVER) {
            d_authentication = ntca::EncryptionAuthentication::e_NONE;
        }
        else {
            NTCTLS_SESSION_LOG_ERROR("Unsupported role");
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    // Create the SSL context in the desired role.

    const SSL_METHOD* method = 0;
    if (role == ntca::EncryptionRole::e_CLIENT) {
        method = TLS_client_method();
    }
    else if (role == ntca::EncryptionRole::e_SERVER) {
        method = TLS_server_method();
    }
    else {
        NTCTLS_SESSION_LOG_ERROR("Unsupported role");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_context.reset(SSL_CTX_new(method));
    if (!d_context) {
        NTCTLS_SESSION_LOG_ERROR("Failed to allocate SSL context");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Explicitly disable SSLv2 and SSLv3. These methods are never supported.

    SSL_CTX_set_options(d_context.get(), SSL_OP_NO_SSLv2);
    SSL_CTX_set_options(d_context.get(), SSL_OP_NO_SSLv3);

    // Set the minimum supported version.

    if (d_minMethod == ntca::EncryptionMethod::e_TLS_V1_0) {
        NTCTLS_SESSION_LOG_ERROR("TLSv1.0 is not supported");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (d_minMethod == ntca::EncryptionMethod::e_TLS_V1_1) {
        NTCTLS_SESSION_LOG_ERROR("TLSv1.1 is not supported");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (d_minMethod == ntca::EncryptionMethod::e_TLS_V1_2) {
        SSL_CTX_set_min_proto_version(d_context.get(), TLS1_2_VERSION);
    }
    else if (d_minMethod == ntca::EncryptionMethod::e_TLS_V1_3) {
        SSL_CTX_set_min_proto_version(d_context.get(), TLS1_3_VERSION);
    }
    else {
        SSL_CTX_set_min_proto_version(d_context.get(), TLS1_2_VERSION);
    }

    // Set the maximum supported version.

    if (d_maxMethod == ntca::EncryptionMethod::e_TLS_V1_0) {
        NTCTLS_SESSION_LOG_ERROR("TLSv1.0 is not supported");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (d_maxMethod == ntca::EncryptionMethod::e_TLS_V1_1) {
        NTCTLS_SESSION_LOG_ERROR("TLSv1.1 is not supported");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (d_maxMethod == ntca::EncryptionMethod::e_TLS_V1_2) {
        SSL_CTX_set_max_proto_version(d_context.get(), TLS1_2_VERSION);
    }
    else if (d_maxMethod == ntca::EncryptionMethod::e_TLS_V1_3) {
        SSL_CTX_set_max_proto_version(d_context.get(), TLS1_3_VERSION);
    }
    else {
        SSL_CTX_set_max_proto_version(d_context.get(), TLS1_3_VERSION);
    }

    // Disable usage of compression.

    SSL_CTX_set_options(d_context.get(), SSL_OP_NO_COMPRESSION);

    // Always create a new key when using tmp_dh parameters.

    SSL_CTX_set_options(d_context.get(), SSL_OP_SINGLE_DH_USE);

    // Always create a new key when using tmp_ecdh parameters.

    SSL_CTX_set_options(d_context.get(), SSL_OP_SINGLE_ECDH_USE);

    // Make it possible to retry SSL_write() with changed buffer location (the
    // buffer contents must stay the same).  This is required because on
    // non-blocking writes, a copy of the write buffer is made to a chararray.

    SSL_CTX_set_mode(d_context.get(), SSL_MODE_ENABLE_PARTIAL_WRITE);

    // Allow SSL_write(..., n) to return r with 0 < r < n (i.e. report success
    // when just a single record has been written). When not set (the default),
    // SSL_write() will only report success once the complete chunk was
    // written.  Once SSL_write() returns with r, r bytes have been
    // successfully written and the next call to SSL_write() must only send the
    // n-r bytes left, imitating the behavior of write().

    SSL_CTX_set_mode(d_context.get(), SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

    // Perform full duplex shutdown.

    SSL_CTX_set_quiet_shutdown(d_context.get(), 0);

    // Configure elliptic curve selection.

    SSL_CTX_set1_curves_list(d_context.get(), "P-384:P-256");

    // Disable temporary Diffie-Hellman parameters.

    SSL_CTX_set_dh_auto(d_context.get(), 0);

    // Disable session caching.

    SSL_CTX_set_session_cache_mode(d_context.get(), SSL_SESS_CACHE_OFF);

    // Set the ciphers supported for TLSv1.0 through TLSv1.2.

    rc = SSL_CTX_set_cipher_list(d_context.get(),
                                 Internal::Impl::k_DEFAULT_CIPHER_SPEC);
    if (rc == 0) {
        NTCTLS_SESSION_LOG_ERROR(
            "Failed to configure SSL context cipher spec");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Set the cipher suites supported for TLSv1.3 and later.

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    rc = SSL_CTX_set_ciphersuites(d_context.get(),
                                  OSSL_default_ciphersuites());
#else
    rc = SSL_CTX_set_ciphersuites(d_context.get(),
                                  Internal::Impl::k_DEFAULT_CIPHER_SUITES);
#endif

    if (rc == 0) {
        NTCTLS_SESSION_LOG_ERROR(
            "Failed to configure SSL context cipher suites");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Set the certificate verification behavior.

    if (d_authentication == ntca::EncryptionAuthentication::e_NONE) {
        SSL_CTX_set_verify(d_context.get(), SSL_VERIFY_NONE, 0);
    }
    else if (d_authentication == ntca::EncryptionAuthentication::e_VERIFY) {
        SSL_CTX_set_verify(d_context.get(),
                           SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                           0);
        SSL_CTX_set_cert_verify_callback(d_context.get(),
                                         &ntctls_context_verify_callback,
                                         this);
    }
    else {
        NTCTLS_SESSION_LOG_ERROR("Unsupported authentication mode");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_role == ntca::EncryptionRole::e_SERVER) {
        SSL_CTX_set_tlsext_servername_callback(d_context.get(),
                                               &ntctls_context_sni_callback);
    }

    // Add the certificates and keys used by this context.

    error = this->addResourceList(options.resources());
    if (error) {
        return error;
    }

    if (!options.authorityDirectory().isNull()) {
        error = this->setCertificateAuthorityDirectory(
            options.authorityDirectory().value());
        if (error) {
            return error;
        }
    }
    else {
        error = this->setCertificateAuthorityDirectoryToDefault();
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error SessionContext::addResource(
    const ntca::EncryptionResource& resource)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntctls::Resource resourceLoader(d_allocator_p);

    ntca::EncryptionResourceOptions options;
    if (!resource.options().isNull()) {
        options = resource.options().value();
    }

    if (resource.descriptor().isCertificate()) {
        const ntca::EncryptionCertificate& certificate =
            resource.descriptor().certificate();

        if (certificate.isAuthority()) {
            error = resourceLoader.addCertificateAuthority(certificate);
            if (error) {
                return error;
            }
        }
        else {
            error = resourceLoader.setCertificate(certificate);
            if (error) {
                return error;
            }
        }
    }
    else if (resource.descriptor().isKey()) {
        const ntca::EncryptionKey& privateKey = resource.descriptor().key();
        error = resourceLoader.setPrivateKey(privateKey);
        if (error) {
            return error;
        }
    }
    else if (resource.descriptor().isPath()) {
        bsl::fstream fs(resource.descriptor().path().c_str(),
                        bsl::ios_base::in);
        if (!fs) {
            error = ntsa::Error::last();
            NTCI_LOG_ERROR("Failed to open resource file '%s': %s",
                           resource.descriptor().path().c_str(),
                           error.text().c_str());
            return error;
        }

        error = resourceLoader.decode(fs.rdbuf(), options);
        if (error) {
            NTCI_LOG_ERROR("Failed to decode resource file '%s': %s",
                           resource.descriptor().path().c_str(),
                           error.text().c_str());
            return error;
        }
    }
    else if (resource.descriptor().isData()) {
        if (resource.descriptor().data().empty()) {
            NTCI_LOG_ERROR("Failed to decode resource: no data");
            return error;
        }

        bdlsb::FixedMemInStreamBuf isb(&resource.descriptor().data().front(),
                                       resource.descriptor().data().size());

        error = resourceLoader.decode(&isb, options);
        if (error) {
            NTCI_LOG_ERROR("Failed to decode resource: %s",
                           error.text().c_str());
            return error;
        }
    }
    else {
        NTCI_LOG_ERROR("Failed to decode resource: unsupported descriptor");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::vector<bsl::shared_ptr<ntci::EncryptionCertificate> > caList;
    error = resourceLoader.getCertificateAuthoritySet(&caList);
    if (!error) {
        for (bsl::size_t i = 0; i < caList.size(); ++i) {
            bsl::shared_ptr<ntctls::Certificate> concreteCertificate;
            error = ntctls::Resource::convert(&concreteCertificate, caList[i]);
            if (error) {
                NTCI_LOG_ERROR("Failed to decode resource: invalid driver");
                return error;
            }

            if (!concreteCertificate->isSelfSigned()) {
                error = this->addCertificateIntermediary(concreteCertificate);
                if (error) {
                    return error;
                }
            }

            error = this->addCertificateAuthority(concreteCertificate);
            if (error) {
                return error;
            }
        }
    }

    bsl::shared_ptr<ntci::EncryptionKey> privateKey;
    error = resourceLoader.getPrivateKey(&privateKey);
    if (!error) {
        bsl::shared_ptr<ntctls::Key> concretePrivateKey;
        error = ntctls::Resource::convert(&concretePrivateKey, privateKey);
        if (error) {
            NTCI_LOG_ERROR("Failed to decode resource: invalid driver");
            return error;
        }

        error = this->setPrivateKey(concretePrivateKey);
        if (error) {
            return error;
        }
    }

    bsl::shared_ptr<ntci::EncryptionCertificate> certificate;
    error = resourceLoader.getCertificate(&certificate);
    if (!error) {
        bsl::shared_ptr<ntctls::Certificate> concreteCertificate;
        error = ntctls::Resource::convert(&concreteCertificate, certificate);
        if (error) {
            NTCI_LOG_ERROR("Failed to decode resource: invalid driver");
            return error;
        }

        error = this->setCertificate(concreteCertificate);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error SessionContext::addResourceList(
    const ntca::EncryptionResourceVector& resourceList)
{
    ntsa::Error error;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    for (ntca::EncryptionResourceVector::const_iterator it =
             resourceList.begin();
         it != resourceList.end();
         ++it)
    {
        const ntca::EncryptionResource& resource = *it;

        error = this->addResource(resource);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error SessionContext::addCertificateAuthority(
    const bsl::shared_ptr<ntctls::Certificate>& certificate)
{
    NTCI_LOG_CONTEXT();

    int rc;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    X509_STORE* store = SSL_CTX_get_cert_store(d_context.get());
    if (store == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = X509_STORE_add_cert(store, certificate->native());
    if (rc == 0) {
        NTCTLS_SESSION_LOG_ERROR("Failed add certificate authority");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_authorities.push_back(certificate);

    return ntsa::Error();
}

ntsa::Error SessionContext::setCertificateAuthorityDirectory(
    const bsl::string& directoryPath)
{
    NTCI_LOG_CONTEXT();

    int rc;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = SSL_CTX_load_verify_locations(d_context.get(),
                                       0,
                                       directoryPath.c_str());
    if (rc == 0) {
        NTCTLS_SESSION_LOG_ERROR("Failed add certificate authority directory");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error SessionContext::addCertificateIntermediary(
    const bsl::shared_ptr<ntctls::Certificate>& certificate)
{
    NTCI_LOG_CONTEXT();

    int rc;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = SSL_CTX_add1_chain_cert(d_context.get(), certificate->native());
    if (rc == 0) {
        NTCTLS_SESSION_LOG_ERROR("Failed add certificate intermediary");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_intermediaries.push_back(certificate);

    return ntsa::Error();
}

ntsa::Error SessionContext::trustSystemDefaults()
{
#if defined(BSLS_PLATFORM_OS_WINDOWS)

    NTCI_LOG_CONTEXT();

    int rc;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    X509_STORE* store = SSL_CTX_get_cert_store(d_context.get());
    BSLS_ASSERT(store);

    HCERTSTORE systemStore = CertOpenSystemStore(0, "ROOT");
    if (systemStore == 0) {
        return ntsa::Error::last();
    }

    PCCERT_CONTEXT certificateContext = 0;

    while (true) {
        certificateContext =
            CertEnumCertificatesInStore(systemStore, certificateContext);
        if (certificateContext == 0) {
            break;
        }

        X509* x509 = d2i_X509(
            0,
            (const unsigned char**)(&certificateContext->pbCertEncoded),
            static_cast<long>(certificateContext->cbCertEncoded));

        if (x509) {
            rc = X509_STORE_add_cert(store, x509);
            if (rc == 0) {
                X509_free(x509);
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            X509_free(x509);
        }
    }

    CertFreeCertificateContext(certificateContext);
    CertCloseStore(systemStore, 0);

    return ntsa::Error();

#else

    // Note: On non-Windows platforms, the default CAs are assumed to solely
    // reside in the default OpenSSL CA directory, and not augmented by CAs in
    // any other locations.

    return ntsa::Error();

#endif
}

ntsa::Error SessionContext::setCertificateAuthorityDirectoryToDefault()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = SSL_CTX_set_default_verify_paths(d_context.get());
    if (rc == 0) {
        NTCTLS_SESSION_LOG_ERROR("Failed add certificate authority directory");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = this->trustSystemDefaults();
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error SessionContext::setCertificate(
    const bsl::shared_ptr<ntctls::Certificate>& certificate)
{
    NTCI_LOG_CONTEXT();

    int rc;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = SSL_CTX_use_certificate(d_context.get(), certificate->native());
    if (rc == 0) {
        NTCTLS_SESSION_LOG_ERROR("Failed to set certificate");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_certificate_sp = certificate;

    return ntsa::Error();
}

ntsa::Error SessionContext::setPrivateKey(
    const bsl::shared_ptr<ntctls::Key>& privateKey)
{
    NTCI_LOG_CONTEXT();

    int rc;

    if (!d_context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    rc = SSL_CTX_use_PrivateKey(d_context.get(), privateKey->native());
    if (rc == 0) {
        NTCTLS_SESSION_LOG_ERROR("Failed to set private key");
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntca::EncryptionRole::Value SessionContext::role() const
{
    return d_role;
}

ntca::EncryptionMethod::Value SessionContext::minMethod() const
{
    return d_minMethod;
}

ntca::EncryptionMethod::Value SessionContext::maxMethod() const
{
    return d_maxMethod;
}

ntca::EncryptionAuthentication::Value SessionContext::authentication() const
{
    return d_authentication;
}

const bdlb::NullableValue<ntca::EncryptionValidation>& SessionContext::
    validation() const
{
    return d_validation;
}

bsl::shared_ptr<ntctls::Certificate> SessionContext::certificate() const
{
    return d_certificate_sp;
}

void* SessionContext::handle() const
{
    return d_context.get();
}

SSL_CTX* SessionContext::native() const
{
    return d_context.get();
}

ntsa::Error SessionContext::createContext(
    bsl::shared_ptr<ntctls::SessionContext>* result,
    ntca::EncryptionRole::Value              role,
    const ntca::EncryptionOptions&           options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionContext> context;
    context.createInplace(allocator, allocator);

    error = context->configure(role, options);
    if (error) {
        return error;
    }

    *result = context;
    return ntsa::Error();
}

SessionManager::SessionManager(ntca::EncryptionRole::Value role,
                               bslma::Allocator*           basicAllocator)
: d_mutex()
, d_role(role)
, d_context_sp()
, d_container(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SessionManager::~SessionManager()
{
}

ntsa::Error SessionManager::configure(const ntca::EncryptionOptions& options)
{
    ntsa::Error error;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    d_context_sp.reset();

    error = ntctls::SessionContext::createContext(&d_context_sp,
                                                  d_role,
                                                  options,
                                                  d_allocator_p);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error SessionManager::configure(const bsl::string& serverName,
                                      const ntca::EncryptionOptions& options)
{
    ntsa::Error error;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (serverName.empty() || serverName == "*") {
        d_context_sp.reset();

        error = ntctls::SessionContext::createContext(&d_context_sp,
                                                      d_role,
                                                      options,
                                                      d_allocator_p);
        if (error) {
            return error;
        }
    }
    else {
        bsl::shared_ptr<ntctls::SessionContext>& context =
            d_container[serverName];

        if (context) {
            context.reset();
        }

        error = ntctls::SessionContext::createContext(&context,
                                                      d_role,
                                                      options,
                                                      d_allocator_p);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error SessionManager::lookup(
    bsl::shared_ptr<ntctls::SessionContext>* result)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    *result = d_context_sp;
    return ntsa::Error();
}

ntsa::Error SessionManager::lookup(
    bsl::shared_ptr<ntctls::SessionContext>* result,
    const bsl::string&                       serverName,
    bool                                     fallback)
{
    ntsa::Error error;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (serverName.empty() || serverName == "*") {
        *result = d_context_sp;
    }
    else {
        Container::const_iterator it = d_container.find(serverName);
        if (it == d_container.end()) {
            if (fallback) {
                *result = d_context_sp;
            }
            else {
                return ntsa::Error(ntsa::Error::e_EOF);
            }
        }
        else {
            *result = it->second;
        }
    }

    return ntsa::Error();
}

ntca::EncryptionRole::Value SessionManager::role() const
{
    return d_role;
}

ntsa::Error SessionManager::createClientSessionManager(
    bsl::shared_ptr<ntctls::SessionManager>* result,
    const ntca::EncryptionClientOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionManager> sessionManager;
    sessionManager.createInplace(allocator,
                                 ntca::EncryptionRole::e_CLIENT,
                                 allocator);

    bsl::vector<bsl::string> serverNameVector;
    options.loadServerNameList(&serverNameVector);

    for (bsl::size_t i = 0; i < serverNameVector.size(); ++i) {
        const bsl::string& serverName = serverNameVector[i];

        ntca::EncryptionOptions nameSpecificOptions;
        if (!options.loadServerNameOptions(&nameSpecificOptions, serverName)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        error = sessionManager->configure(serverName, nameSpecificOptions);
        if (error) {
            return error;
        }
    }

    *result = sessionManager;
    return ntsa::Error();
}

ntsa::Error SessionManager::createServerSessionManager(
    bsl::shared_ptr<ntctls::SessionManager>* result,
    const ntca::EncryptionServerOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionManager> sessionManager;
    sessionManager.createInplace(allocator,
                                 ntca::EncryptionRole::e_SERVER,
                                 allocator);

    bsl::vector<bsl::string> serverNameVector;
    options.loadServerNameList(&serverNameVector);

    for (bsl::size_t i = 0; i < serverNameVector.size(); ++i) {
        const bsl::string& serverName = serverNameVector[i];

        ntca::EncryptionOptions nameSpecificOptions;
        if (!options.loadServerNameOptions(&nameSpecificOptions, serverName)) {
            return ntsa::Error(ntsa::Error::e_EOF);
        }

        error = sessionManager->configure(serverName, nameSpecificOptions);
        if (error) {
            return error;
        }
    }

    *result = sessionManager;
    return ntsa::Error();
}

SessionClient::SessionClient(
    const bsl::shared_ptr<ntctls::SessionManager>&   sessionManager,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
: d_sessionManager_sp(sessionManager)
, d_blobBufferFactory_sp(blobBufferFactory)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_sessionManager_sp);
    BSLS_ASSERT(d_blobBufferFactory_sp);
}

SessionClient::~SessionClient()
{
}

ntsa::Error SessionClient::createEncryption(
    bsl::shared_ptr<ntci::Encryption>* result,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Session> session;
    session.createInplace(allocator,
                          d_sessionManager_sp,
                          d_blobBufferFactory_sp,
                          allocator);

    *result = session;
    return ntsa::Error();
}

ntsa::Error SessionClient::createEncryption(
    bsl::shared_ptr<ntci::Encryption>*     result,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Session> session;
    session.createInplace(allocator,
                          d_sessionManager_sp,
                          dataPool->incomingBlobBufferFactory(),
                          allocator);

    *result = session;
    return ntsa::Error();
}

ntsa::Error SessionClient::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionManager> sessionManager;
    error = ntctls::SessionManager::createClientSessionManager(&sessionManager,
                                                               options,
                                                               allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<bdlbb::PooledBlobBufferFactory> blobBufferFactory;
    blobBufferFactory.createInplace(allocator,
                                    NTCCFG_DEFAULT_INCOMING_BLOB_BUFFER_SIZE,
                                    allocator);

    bsl::shared_ptr<ntctls::SessionClient> client;
    client.createInplace(allocator,
                         sessionManager,
                         blobBufferFactory,
                         allocator);

    *result = client;
    return ntsa::Error();
}

ntsa::Error SessionClient::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>*         result,
    const ntca::EncryptionClientOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionManager> sessionManager;
    error = ntctls::SessionManager::createClientSessionManager(&sessionManager,
                                                               options,
                                                               allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::SessionClient> client;
    client.createInplace(allocator,
                         sessionManager,
                         blobBufferFactory,
                         allocator);

    *result = client;
    return ntsa::Error();
}

ntsa::Error SessionClient::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionManager> sessionManager;
    error = ntctls::SessionManager::createClientSessionManager(&sessionManager,
                                                               options,
                                                               allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::SessionClient> client;
    client.createInplace(allocator,
                         sessionManager,
                         dataPool->incomingBlobBufferFactory(),
                         allocator);

    *result = client;
    return ntsa::Error();
}

SessionServer::SessionServer(
    const bsl::shared_ptr<ntctls::SessionManager>&   sessionManager,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
: d_sessionManager_sp(sessionManager)
, d_blobBufferFactory_sp(blobBufferFactory)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_sessionManager_sp);
    BSLS_ASSERT(d_blobBufferFactory_sp);
}

SessionServer::~SessionServer()
{
}

ntsa::Error SessionServer::createEncryption(
    bsl::shared_ptr<ntci::Encryption>* result,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Session> session;
    session.createInplace(allocator,
                          d_sessionManager_sp,
                          d_blobBufferFactory_sp,
                          allocator);

    *result = session;
    return ntsa::Error();
}

ntsa::Error SessionServer::createEncryption(
    bsl::shared_ptr<ntci::Encryption>*     result,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Session> session;
    session.createInplace(allocator,
                          d_sessionManager_sp,
                          dataPool->incomingBlobBufferFactory(),
                          allocator);

    *result = session;
    return ntsa::Error();
}

ntsa::Error SessionServer::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionManager> sessionManager;
    error = ntctls::SessionManager::createServerSessionManager(&sessionManager,
                                                               options,
                                                               allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<bdlbb::PooledBlobBufferFactory> blobBufferFactory;
    blobBufferFactory.createInplace(allocator,
                                    NTCCFG_DEFAULT_INCOMING_BLOB_BUFFER_SIZE,
                                    allocator);

    bsl::shared_ptr<ntctls::SessionServer> server;
    server.createInplace(allocator,
                         sessionManager,
                         blobBufferFactory,
                         allocator);

    *result = server;
    return ntsa::Error();
}

ntsa::Error SessionServer::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>*         result,
    const ntca::EncryptionServerOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionManager> sessionManager;
    error = ntctls::SessionManager::createServerSessionManager(&sessionManager,
                                                               options,
                                                               allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::SessionServer> server;
    server.createInplace(allocator,
                         sessionManager,
                         blobBufferFactory,
                         allocator);

    *result = server;
    return ntsa::Error();
}

ntsa::Error SessionServer::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::SessionManager> sessionManager;
    error = ntctls::SessionManager::createServerSessionManager(&sessionManager,
                                                               options,
                                                               allocator);
    if (error) {
        return error;
    }

    bsl::shared_ptr<ntctls::SessionServer> server;
    server.createInplace(allocator,
                         sessionManager,
                         dataPool->incomingBlobBufferFactory(),
                         allocator);

    *result = server;
    return ntsa::Error();
}

/// Provide an OpenSSL encryption driver.
///
/// @details
/// This class implements the 'ntci::EncryptionDriver' interface to
/// implement TLS in NTC using the 'openssl' third-party library.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctls
class Driver : public ntci::EncryptionDriver
{
    bslma::Allocator* d_allocator_p;

  private:
    Driver(const Driver&) BSLS_KEYWORD_DELETED;
    Driver& operator=(const Driver&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new driver. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Driver(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Driver() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' an RSA key generated according to
    /// the specified 'options'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateKey(ntca::EncryptionKey*              result,
                            const ntca::EncryptionKeyOptions& options,
                            bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' an RSA key generated according to the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
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
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            subjectIdentity,
        const ntca::EncryptionKey&                subjectPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

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
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

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

    /// Load into the specified 'result' a certificate generated according to
    /// the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified by
    /// the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
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
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
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
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption resource. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used. Return the
    /// error.
    ntsa::Error createEncryptionResource(
        bsl::shared_ptr<ntci::EncryptionResource>* result,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;
};

Driver::Driver(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Driver::~Driver()
{
}

ntsa::Error Driver::generateKey(ntca::EncryptionKey*              result,
                                const ntca::EncryptionKeyOptions& options,
                                bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::Key::generateKey(result, options, allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::generateKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                const ntca::EncryptionKeyOptions&     options,
                                bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::Key::generateKey(result, options, allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::encodeKey(
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionResourceOptions&      options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntctls::Key> effectivePrivateKey;
    error = ntctls::Resource::convert(&effectivePrivateKey, privateKey);
    if (error) {
        return error;
    }

    return effectivePrivateKey->encode(destination, options);
}

ntsa::Error Driver::decodeKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                              bsl::streambuf*                        source,
                              const ntca::EncryptionResourceOptions& options,
                              bslma::Allocator* basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Key> effectiveResult;
    effectiveResult.createInplace(allocator, allocator);

    error = effectiveResult->decode(source, options);
    if (error) {
        return error;
    }

    *result = effectiveResult;
    return ntsa::Error();
}

ntsa::Error Driver::generateCertificate(
    ntca::EncryptionCertificate*              result,
    const ntsa::DistinguishedName&            subjectIdentity,
    const ntca::EncryptionKey&                subjectPrivateKey,
    const ntca::EncryptionCertificateOptions& options,
    bslma::Allocator*                         basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::Certificate::generateCertificate(result,
                                                     subjectIdentity,
                                                     subjectPrivateKey,
                                                     options,
                                                     allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::generateCertificate(
    ntca::EncryptionCertificate*              result,
    const ntsa::DistinguishedName&            subjectIdentity,
    const ntca::EncryptionKey&                subjectPrivateKey,
    const ntca::EncryptionCertificate&        issuerCertificate,
    const ntca::EncryptionKey&                issuerPrivateKey,
    const ntca::EncryptionCertificateOptions& options,
    bslma::Allocator*                         basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::Certificate::generateCertificate(result,
                                                     subjectIdentity,
                                                     subjectPrivateKey,
                                                     issuerCertificate,
                                                     issuerPrivateKey,
                                                     options,
                                                     allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const ntsa::DistinguishedName&                subjectIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&   subjectPrivateKey,
    const ntca::EncryptionCertificateOptions&     options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::Certificate::generateCertificate(result,
                                                     subjectIdentity,
                                                     subjectPrivateKey,
                                                     options,
                                                     allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::generateCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
    const ntsa::DistinguishedName&                      subjectIdentity,
    const bsl::shared_ptr<ntci::EncryptionKey>&         subjectPrivateKey,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& issuerCertificate,
    const bsl::shared_ptr<ntci::EncryptionKey>&         issuerPrivateKey,
    const ntca::EncryptionCertificateOptions&           options,
    bslma::Allocator*                                   basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::Certificate::generateCertificate(result,
                                                     subjectIdentity,
                                                     subjectPrivateKey,
                                                     issuerCertificate,
                                                     issuerPrivateKey,
                                                     options,
                                                     allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::encodeCertificate(
    bsl::streambuf*                                     destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    bsl::shared_ptr<ntctls::Certificate> effectiveCertificate;
    error = ntctls::Resource::convert(&effectiveCertificate, certificate);
    if (error) {
        return error;
    }

    return effectiveCertificate->encode(destination, options);
}

ntsa::Error Driver::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    bsl::streambuf*                               source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Certificate> effectiveResult;
    effectiveResult.createInplace(allocator, allocator);

    error = effectiveResult->decode(source, options);
    if (error) {
        return error;
    }

    *result = effectiveResult;
    return ntsa::Error();
}

ntsa::Error Driver::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::SessionClient::createEncryptionClient(result,
                                                          options,
                                                          allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>*         result,
    const ntca::EncryptionClientOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::SessionClient::createEncryptionClient(result,
                                                          options,
                                                          blobBufferFactory,
                                                          allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::createEncryptionClient(
    bsl::shared_ptr<ntci::EncryptionClient>* result,
    const ntca::EncryptionClientOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::SessionClient::createEncryptionClient(result,
                                                          options,
                                                          dataPool,
                                                          allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::SessionServer::createEncryptionServer(result,
                                                          options,
                                                          allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>*         result,
    const ntca::EncryptionServerOptions&             options,
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
    bslma::Allocator*                                basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::SessionServer::createEncryptionServer(result,
                                                          options,
                                                          blobBufferFactory,
                                                          allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::createEncryptionServer(
    bsl::shared_ptr<ntci::EncryptionServer>* result,
    const ntca::EncryptionServerOptions&     options,
    const bsl::shared_ptr<ntci::DataPool>&   dataPool,
    bslma::Allocator*                        basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    error = ntctls::SessionServer::createEncryptionServer(result,
                                                          options,
                                                          dataPool,
                                                          allocator);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error Driver::createEncryptionResource(
    bsl::shared_ptr<ntci::EncryptionResource>* result,
    bslma::Allocator*                          basicAllocator)
{
    ntsa::Error error;

    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntctls::Resource> resource;
    resource.createInplace(allocator, allocator);

    *result = resource;
    return ntsa::Error();
}

Initializer::Initializer()
{
    ntcs::Plugin::initialize();

    const int blobTypeIndex = BIO_get_new_index() | BIO_TYPE_SOURCE_SINK;

    Internal::Impl::s_blobMethods = BIO_meth_new(blobTypeIndex, "blob");

    BIO_meth_set_write(Internal::Impl::s_blobMethods,
                       &Internal::Impl::bio_blob_write);
    BIO_meth_set_read(Internal::Impl::s_blobMethods,
                      &Internal::Impl::bio_blob_read);
    BIO_meth_set_puts(Internal::Impl::s_blobMethods,
                      &Internal::Impl::bio_blob_puts);
    BIO_meth_set_gets(Internal::Impl::s_blobMethods,
                      &Internal::Impl::bio_blob_gets);
    BIO_meth_set_ctrl(Internal::Impl::s_blobMethods,
                      &Internal::Impl::bio_blob_ctrl);
    BIO_meth_set_create(Internal::Impl::s_blobMethods,
                        &Internal::Impl::bio_blob_new);
    BIO_meth_set_destroy(Internal::Impl::s_blobMethods,
                         &Internal::Impl::bio_blob_free);

    const int streamTypeIndex = BIO_get_new_index() | BIO_TYPE_SOURCE_SINK;

    Internal::Impl::s_streambufMethods =
        BIO_meth_new(streamTypeIndex, "streambuf");

    BIO_meth_set_write(Internal::Impl::s_streambufMethods,
                       &Internal::Impl::bio_streambuf_write);
    BIO_meth_set_read(Internal::Impl::s_streambufMethods,
                      &Internal::Impl::bio_streambuf_read);
    BIO_meth_set_puts(Internal::Impl::s_streambufMethods,
                      &Internal::Impl::bio_streambuf_puts);
    BIO_meth_set_gets(Internal::Impl::s_streambufMethods,
                      &Internal::Impl::bio_streambuf_gets);
    BIO_meth_set_ctrl(Internal::Impl::s_streambufMethods,
                      &Internal::Impl::bio_streambuf_ctrl);
    BIO_meth_set_create(Internal::Impl::s_streambufMethods,
                        &Internal::Impl::bio_streambuf_new);
    BIO_meth_set_destroy(Internal::Impl::s_streambufMethods,
                         &Internal::Impl::bio_streambuf_free);

    bslma::Allocator* allocator = &bslma::NewDeleteAllocator::singleton();

    bsl::shared_ptr<ntci::EncryptionDriver> encryptionDriver(
        new (*allocator) ntctls::Driver(allocator),
        allocator);

    ntcs::Plugin::registerEncryptionDriver(encryptionDriver);
}

Initializer::~Initializer()
{
    BIO_meth_free(Internal::Impl::s_blobMethods);
    Internal::Impl::s_blobMethods = 0;

    BIO_meth_free(Internal::Impl::s_streambufMethods);
    Internal::Impl::s_streambufMethods = 0;
}

void Plugin::initialize(bslma::Allocator* basicAllocator)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
    ntctls::Internal::initialize();
}

void Plugin::load(bsl::shared_ptr<ntci::EncryptionDriver>* result)
{
    ntsa::Error error;

    ntctls::Internal::initialize();

    error = ntcs::Plugin::lookupEncryptionDriver(result);
    if (error) {
        bslma::Allocator* allocator = &bslma::NewDeleteAllocator::singleton();

        bsl::shared_ptr<ntctls::Driver> driver;
        driver.createInplace(allocator, allocator);

        *result = driver;
    }
}

void Plugin::exit()
{
    ntctls::Internal::exit();
}

PluginGuard::PluginGuard(bslma::Allocator* basicAllocator)
{
    ntctls::Plugin::initialize(basicAllocator);
}

PluginGuard::~PluginGuard()
{
    ntctls::Plugin::exit();
}

}  // close package namespace
}  // close enterprise namespace

#endif
