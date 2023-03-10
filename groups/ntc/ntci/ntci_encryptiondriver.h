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

#ifndef INCLUDED_NTCI_ENCRYPTIONDRIVER
#define INCLUDED_NTCI_ENCRYPTIONDRIVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_encryptioncertificategenerator.h>
#include <ntci_encryptionclientfactory.h>
#include <ntci_encryptionkeygenerator.h>
#include <ntci_encryptionserverfactory.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to a system of authentication and encryption.
///
/// @details
/// An encryption driver generates certificates, private keys, and clients
/// and servers of encryption sessions as required to implement TLS over a raw
/// stream transport.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionDriver : public ntci::EncryptionKeyGenerator,
                         public ntci::EncryptionCertificateGenerator,
                         public ntci::EncryptionClientFactory,
                         public ntci::EncryptionServerFactory
{
  public:
    /// Destroy this object.
    ~EncryptionDriver() BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
