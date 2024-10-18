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

#ifndef INCLUDED_NTCI_ENCRYPTIONCERTIFICATEGENERATOR
#define INCLUDED_NTCI_ENCRYPTIONCERTIFICATEGENERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_encryptioncertificate.h>
#include <ntca_encryptioncertificateoptions.h>
#include <ntccfg_platform.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptionkey.h>
#include <ntcscm_version.h>
#include <ntsa_distinguishedname.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to generate and sign certificates as used in public
/// key cryptography.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
class EncryptionCertificateGenerator
{
  public:
    /// Destroy this object.
    virtual ~EncryptionCertificateGenerator();

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by itself. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    virtual ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            subjectIdentity,
        const ntca::EncryptionKey&                subjectPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator*                         basicAllocator = 0) = 0;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified
    /// by the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            subjectIdentity,
        const ntca::EncryptionKey&                subjectPrivateKey,
        const ntca::EncryptionCertificate&        issuerCertificate,
        const ntca::EncryptionKey&                issuerPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator*                         basicAllocator = 0) = 0;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by itself. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    virtual ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntsa::DistinguishedName&                subjectIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&   subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator*                             basicAllocator = 0) = 0;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified
    /// by the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
        const ntsa::DistinguishedName&                      subjectIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&         subjectPrivateKey,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& issuerCertificate,
        const bsl::shared_ptr<ntci::EncryptionKey>&         issuerPrivateKey,
        const ntca::EncryptionCertificateOptions&           options,
        bslma::Allocator* basicAllocator = 0) = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
