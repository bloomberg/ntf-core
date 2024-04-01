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

#include <ntci_encryptioncertificatestorage.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_encryptioncertificatestorage_cpp, "$Id$ $CSID$")

#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bsl_fstream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntci {

EncryptionCertificateStorage::~EncryptionCertificateStorage()
{
}

ntsa::Error EncryptionCertificateStorage::loadCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            path,
    bslma::Allocator*                             basicAllocator)
{
    ntca::EncryptionResourceOptions options;
    return this->loadCertificate(result, path, options, basicAllocator);
}

ntsa::Error EncryptionCertificateStorage::loadCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            path,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    bsl::fstream fs(path.c_str(), bsl::ios_base::in);
    if (!fs) {
        return ntsa::Error::last();
    }

    return this->decodeCertificate(result,
                                   fs.rdbuf(),
                                   options,
                                   basicAllocator);
}

ntsa::Error EncryptionCertificateStorage::saveCertificate(
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const bsl::string&                                  path)
{
    ntca::EncryptionResourceOptions options;
    return this->saveCertificate(certificate, path, options);
}

ntsa::Error EncryptionCertificateStorage::saveCertificate(
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const bsl::string&                                  path,
    const ntca::EncryptionResourceOptions&              options)
{
    bsl::fstream fs(path.c_str(), bsl::ios_base::out);
    if (!fs) {
        return ntsa::Error::last();
    }

    return this->encodeCertificate(fs.rdbuf(), certificate, options);
}

ntsa::Error EncryptionCertificateStorage::encodeCertificate(
    bsl::streambuf*                                     destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    ntca::EncryptionResourceOptions options;
    return this->encodeCertificate(destination, certificate, options);
}

ntsa::Error EncryptionCertificateStorage::encodeCertificate(
    bsl::streambuf*                                     destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    NTCCFG_WARNING_UNUSED(destination);
    NTCCFG_WARNING_UNUSED(certificate);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificateStorage::encodeCertificate(
    bdlbb::Blob*                                        destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    ntca::EncryptionResourceOptions options;
    return this->encodeCertificate(destination, certificate, options);
}

ntsa::Error EncryptionCertificateStorage::encodeCertificate(
    bdlbb::Blob*                                        destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    bdlbb::OutBlobStreamBuf osb(destination);

    error = this->encodeCertificate(&osb, certificate, options);
    if (error) {
        return error;
    }

    osb.pubsync();
    return ntsa::Error();
}

ntsa::Error EncryptionCertificateStorage::encodeCertificate(
    bsl::string*                                        destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    ntca::EncryptionResourceOptions options;
    return this->encodeCertificate(destination, certificate, options);
}

ntsa::Error EncryptionCertificateStorage::encodeCertificate(
    bsl::string*                                        destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    bsl::ostringstream oss;

    error = this->encodeCertificate(oss.rdbuf(), certificate, options);
    if (error) {
        return error;
    }

    oss.flush();
    *destination = oss.str();

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateStorage::encodeCertificate(
    bsl::vector<char>*                                  destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate)
{
    ntca::EncryptionResourceOptions options;
    return this->encodeCertificate(destination, certificate, options);
}

ntsa::Error EncryptionCertificateStorage::encodeCertificate(
    bsl::vector<char>*                                  destination,
    const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
    const ntca::EncryptionResourceOptions&              options)
{
    ntsa::Error error;

    bdlsb::MemOutStreamBuf osb;

    error = this->encodeCertificate(&osb, certificate, options);
    if (error) {
        return error;
    }

    osb.pubsync();

    destination->clear();
    destination->insert(destination->begin(),
                        osb.data(),
                        osb.data() + osb.length());

    return ntsa::Error();
}

ntsa::Error EncryptionCertificateStorage::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    bsl::streambuf*                               source,
    bslma::Allocator*                             basicAllocator)
{
    ntca::EncryptionResourceOptions options;
    return this->decodeCertificate(result, source, options, basicAllocator);
}

ntsa::Error EncryptionCertificateStorage::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    bsl::streambuf*                               source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(source);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(basicAllocator);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificateStorage::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bdlbb::Blob&                            source,
    bslma::Allocator*                             basicAllocator)
{
    ntca::EncryptionResourceOptions options;
    return this->decodeCertificate(result, source, options, basicAllocator);
}

ntsa::Error EncryptionCertificateStorage::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bdlbb::Blob&                            source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    bdlbb::InBlobStreamBuf isb(&source);
    return this->decodeCertificate(result, &isb, options, basicAllocator);
}

ntsa::Error EncryptionCertificateStorage::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            source,
    bslma::Allocator*                             basicAllocator)
{
    ntca::EncryptionResourceOptions options;
    return this->decodeCertificate(result, source, options, basicAllocator);
}

ntsa::Error EncryptionCertificateStorage::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::string&                            source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    bdlsb::FixedMemInStreamBuf isb(source.data(), source.size());
    return this->decodeCertificate(result, &isb, options, basicAllocator);
}

ntsa::Error EncryptionCertificateStorage::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::vector<char>&                      source,
    bslma::Allocator*                             basicAllocator)
{
    ntca::EncryptionResourceOptions options;
    return this->decodeCertificate(result, source, options, basicAllocator);
}

ntsa::Error EncryptionCertificateStorage::decodeCertificate(
    bsl::shared_ptr<ntci::EncryptionCertificate>* result,
    const bsl::vector<char>&                      source,
    const ntca::EncryptionResourceOptions&        options,
    bslma::Allocator*                             basicAllocator)
{
    bdlsb::FixedMemInStreamBuf isb(&source.front(), source.size());
    return this->decodeCertificate(result, &isb, options, basicAllocator);
}

}  // close package namespace
}  // close enterprise namespace
