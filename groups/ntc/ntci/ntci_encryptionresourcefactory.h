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

#ifndef INCLUDED_NTCI_ENCRYPTIONRESOURCEFACTORY
#define INCLUDED_NTCI_ENCRYPTIONRESOURCEFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_encryptionresource.h>
#include <ntcscm_version.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to create encryption resources.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionResourceFactory
{
  public:
    /// Destroy this object.
    virtual ~EncryptionResourceFactory();

    /// Load into the specified 'result' a new encryption resource. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used. Return the
    /// error.
    virtual ntsa::Error createEncryptionResource(
        bsl::shared_ptr<ntci::EncryptionResource>* result,
        bslma::Allocator*                          basicAllocator = 0);
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
