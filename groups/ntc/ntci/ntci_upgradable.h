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

#ifndef INCLUDED_NTCI_UPGRADABLE
#define INCLUDED_NTCI_UPGRADABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_upgradeoptions.h>
#include <ntccfg_platform.h>
#include <ntci_encryption.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptionclient.h>
#include <ntci_encryptionkey.h>
#include <ntci_encryptionserver.h>
#include <ntci_strand.h>
#include <ntci_upgradecallbackfactory.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to upgrade the security of a transport.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_upgrade
class Upgradable : public ntci::UpgradeCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~Upgradable();

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryption'. If the 'encryption' was created from an encryption
    /// client, the upgrade process will proceed in the client (i.e.
    /// connector) role. If the 'encryption' was created from an encryption
    /// server, the upgrade process will proceed in the server (i.e.
    /// acceptor) role. The upgrade process will re-use state cached from
    /// previous uses the 'encryption', if any. Invoke the specified
    /// 'callback' on this object's strand, if any, when the upgrade is
    /// complete or any error occurs. Return the error. Note that callbacks
    /// created by this object will automatically be invoked on this
    /// object's strand unless an explicit strand is specified at the time
    /// the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::Encryption>& encryption,
        const ntca::UpgradeOptions&              options,
        const ntci::UpgradeFunction&             callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryption'. If the 'encryption' was created from an encryption
    /// client, the upgrade process will proceed in the client (i.e.
    /// connector) role. If the 'encryption' was created from an encryption
    /// server, the upgrade process will proceed in the server (i.e.
    /// acceptor) role. The upgrade process will re-use state cached from
    /// previous uses the 'encryption', if any. Invoke the specified
    /// 'callback' on the callback's strand, if any, when the upgrade is
    /// complete or any error occurs. Return the error. Note that callbacks
    /// created by this object will automatically be invoked on this
    /// object's strand unless an explicit strand is specified at the time
    /// the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::Encryption>& encryption,
        const ntca::UpgradeOptions&              options,
        const ntci::UpgradeCallback&             callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionClient'. The upgrade process will proceed in the client
    /// (i.e. connector) role. Invoke the specified 'callback' on this
    /// object's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeFunction&                   callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionClient'. The upgrade process will proceed in the client
    /// (i.e. connector) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionClient>& encryptionClient,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeCallback&                   callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionServer'. The upgrade process will proceed in the server
    /// (i.e. acceptor) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeFunction&                   callback) = 0;

    /// Upgrade from unencrypted to encrypted using the specified
    /// 'encryptionServer'. The upgrade process will proceed in the server
    /// (i.e. acceptor) role. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error upgrade(
        const bsl::shared_ptr<ntci::EncryptionServer>& encryptionServer,
        const ntca::UpgradeOptions&                    options,
        const ntci::UpgradeCallback&                   callback) = 0;

    /// Cancel the upgrade operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::UpgradeToken& token) = 0;

    /// Return the source certificate used by the encryption session, if
    /// any.
    virtual bsl::shared_ptr<ntci::EncryptionCertificate> sourceCertificate()
        const = 0;

    /// Return the remote certificate used by the encryption session, if
    /// any.
    virtual bsl::shared_ptr<ntci::EncryptionCertificate> remoteCertificate()
        const = 0;

    /// Return the private key used by the encryption session, if any.
    virtual bsl::shared_ptr<ntci::EncryptionKey> privateKey() const = 0;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
