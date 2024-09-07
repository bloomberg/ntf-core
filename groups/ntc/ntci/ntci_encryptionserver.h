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

#ifndef INCLUDED_NTCI_ENCRYPTIONSERVER
#define INCLUDED_NTCI_ENCRYPTIONSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_datapool.h>
#include <ntci_encryption.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to create an encryption mechanism in the server role.
///
/// @details
/// This class is responsible for creating an 'ntci::Encryption' implementation
/// in the server role. Such 'ntci::Encryption' interfaces are subsequently
/// used to passively wait for the peer to initiate a cryptographically secure
/// session of communication according to the Transport Layer Security (TLS)
/// protocol, within which data is transformed from from cleartext to
/// ciphertext.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionServer
{
  public:
    /// Destroy this object.
    virtual ~EncryptionServer();

    /// Load into the specified 'result' a new server-side encryption
    /// session. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used. Return the error.
    virtual ntsa::Error createEncryption(
        bsl::shared_ptr<ntci::Encryption>* result,
        bslma::Allocator*                  basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new server-side encryption session.
    /// Allocate blob buffers from the specified 'dataPool'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used. Return the error.
    virtual ntsa::Error createEncryption(
        bsl::shared_ptr<ntci::Encryption>*     result,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator*                      basicAllocator = 0) = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
