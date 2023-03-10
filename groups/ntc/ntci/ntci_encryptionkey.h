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

    /// Encode the key in PEM format to the specified 'destination'.
    virtual ntsa::Error encode(bsl::streambuf* destination) const = 0;

    /// Encode the key in PEM format to the specified 'destination'.
    virtual ntsa::Error encode(bdlbb::Blob* destination) const = 0;

    /// Encode the key in PEM format to the specified 'destination'.
    virtual ntsa::Error encode(bsl::string* destination) const = 0;

    /// Encode the key in PEM format to the specified 'destination'.
    virtual ntsa::Error encode(bsl::vector<char>* destination) const = 0;

    /// Return a handle to the private implementation.
    virtual void* handle() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
