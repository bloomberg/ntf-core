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

#ifndef INCLUDED_NTCI_ENCRYPTIONKEYSTORAGE
#define INCLUDED_NTCI_ENCRYPTIONKEYSTORAGE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptionresourceoptions.h>
#include <ntccfg_platform.h>
#include <ntci_encryptionkey.h>
#include <ntcscm_version.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to generate and sign private keys as used in public
/// key cryptography.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionKeyStorage
{
  public:
    /// Destroy this object.
    virtual ~EncryptionKeyStorage();

    /// Load into the specified 'result' a private key stored at the specified
    /// 'path' in the Privacy Enhanced Mail (PEM) format. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.  Return the error.
    virtual ntsa::Error loadKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                const bsl::string&                    path,
                                bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key stored at the specified
    /// 'path' according to the specified 'options'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.  Return the error.
    virtual ntsa::Error loadKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                                const bsl::string&                     path,
                                const ntca::EncryptionResourceOptions& options,
                                bslma::Allocator* basicAllocator = 0);

    /// Save the specified 'privateKey' to the specified 'path' in the Privacy
    /// Enhanced Mail (PEM) format. Return the error.
    virtual ntsa::Error saveKey(
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const bsl::string&                          path);

    /// Save the specified 'privateKey' to the specified 'path' according to
    /// the specified 'options'. Return the error.
    virtual ntsa::Error saveKey(
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const bsl::string&                          path,
        const ntca::EncryptionResourceOptions&      options);

    /// Encode the specified 'privateKey' to the specified 'destination' in the
    /// Privacy Enhanced Mail (PEM) format. Return the error.
    virtual ntsa::Error encodeKey(
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey);

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    virtual ntsa::Error encodeKey(
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions&      options);

    /// Encode the specified 'privateKey' to the specified 'destination' in the
    /// Privacy Enhanced Mail (PEM) format. Return the error.
    virtual ntsa::Error encodeKey(
        bdlbb::Blob*                                destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey);

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    virtual ntsa::Error encodeKey(
        bdlbb::Blob*                                destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions&      options);

    /// Encode the specified 'privateKey' to the specified 'destination' in the
    /// Privacy Enhanced Mail (PEM) format. Return the error.
    virtual ntsa::Error encodeKey(
        bsl::string*                                destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey);

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    virtual ntsa::Error encodeKey(
        bsl::string*                                destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions&      options);

    /// Encode the specified 'privateKey' to the specified 'destination' in the
    /// Privacy Enhanced Mail (PEM) format. Return the error.
    virtual ntsa::Error encodeKey(
        bsl::vector<char>*                          destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey);

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    virtual ntsa::Error encodeKey(
        bsl::vector<char>*                          destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions&      options);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                  bsl::streambuf*                       source,
                                  bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return the
    /// error. Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual ntsa::Error decodeKey(
        bsl::shared_ptr<ntci::EncryptionKey>*  result,
        bsl::streambuf*                        source,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                  const bdlbb::Blob&                    source,
                                  bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return the
    /// error. Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual ntsa::Error decodeKey(
        bsl::shared_ptr<ntci::EncryptionKey>*  result,
        const bdlbb::Blob&                     source,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                  const bsl::string&                    source,
                                  bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return the
    /// error. Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual ntsa::Error decodeKey(
        bsl::shared_ptr<ntci::EncryptionKey>*  result,
        const bsl::string&                     source,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' in the Privacy Enhanced Mail (PEM) format. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                                  const bsl::vector<char>&              source,
                                  bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return the
    /// error. Optionally specify a 'basicAllocator' used to supply memory.  If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual ntsa::Error decodeKey(
        bsl::shared_ptr<ntci::EncryptionKey>*  result,
        const bsl::vector<char>&               source,
        const ntca::EncryptionResourceOptions& options,
        bslma::Allocator*                      basicAllocator = 0);
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
