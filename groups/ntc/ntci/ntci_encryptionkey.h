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

#ifndef INCLUDED_NTCI_ENCRYPTIONKEY
#define INCLUDED_NTCI_ENCRYPTIONKEY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionresourceoptions.h>
#include <ntca_encryptionkey.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlbb_blob.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to a private key as used in public key cryptography.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKey
{
  public:
    /// Destroy this object.
    virtual ~EncryptionKey();

    /// Decode the key in PEM format from the specified 'source'.
    virtual ntsa::Error decode(bsl::streambuf* source);

    /// Decode the key according to the specified 'options' from the specified
    /// 'source'.
    virtual ntsa::Error decode(bsl::streambuf*                        source,
                               const ntca::EncryptionResourceOptions& options);

    /// Decode the key in PEM format from the specified 'source'.
    virtual ntsa::Error decode(const bdlbb::Blob& source);

    /// Decode the key according to the specified 'options' from the specified
    /// 'source'.
    virtual ntsa::Error decode(const bdlbb::Blob&                     source,
                               const ntca::EncryptionResourceOptions& options);

    /// Decode the key in PEM format from the specified 'source'.
    virtual ntsa::Error decode(const bsl::string& source);

    /// Decode the key according to the specified 'options' from the specified
    /// 'source'.
    virtual ntsa::Error decode(const bsl::string&                     source,
                               const ntca::EncryptionResourceOptions& options);

    /// Decode the key in PEM format from the specified 'source'.
    virtual ntsa::Error decode(const bsl::vector<char>& source);

    /// Decode the key according to the specified 'options' from the specified
    /// 'source'.
    virtual ntsa::Error decode(const bsl::vector<char>&               source,
                               const ntca::EncryptionResourceOptions& options);

    /// Encode the key in PEM format to the specified 'destination'.
    virtual ntsa::Error encode(bsl::streambuf* destination) const;

    /// Encode the key according to the specified 'options' to the specified
    /// 'destination'.
    virtual ntsa::Error encode(
        bsl::streambuf*                        destination,
        const ntca::EncryptionResourceOptions& options) const;

    /// Encode the key in PEM format to the specified 'destination'.
    virtual ntsa::Error encode(bdlbb::Blob* destination) const;

    /// Encode the key according to the specified 'options' to the specified
    /// 'destination'.
    virtual ntsa::Error encode(
        bdlbb::Blob*                           destination,
        const ntca::EncryptionResourceOptions& options) const;

    /// Encode the key in PEM format to the specified 'destination'.
    virtual ntsa::Error encode(bsl::string* destination) const;

    /// Encode the key according to the specified 'options' to the specified
    /// 'destination'.
    virtual ntsa::Error encode(
        bsl::string*                           destination,
        const ntca::EncryptionResourceOptions& options) const;

    /// Encode the key in PEM format to the specified 'destination'.
    virtual ntsa::Error encode(bsl::vector<char>* destination) const;

    /// Encode the key according to the specified 'options' to the specified
    /// 'destination'.
    virtual ntsa::Error encode(
        bsl::vector<char>*                     destination,
        const ntca::EncryptionResourceOptions& options) const;

    /// Load into the specified 'result' the value-semantic representation
    /// of this key. Return the error.
    virtual ntsa::Error unwrap(ntca::EncryptionKey* result) const;

    /// Print the public key parameters to the specified stream in an
    /// unspecified but human-readable form.
    virtual void print(bsl::ostream& stream) const;

    /// Return a handle to the private implementation.
    virtual void* handle() const;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
