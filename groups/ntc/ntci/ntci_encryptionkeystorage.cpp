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

#include <ntci_encryptionkeystorage.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_encryptionkeystorage_cpp, "$Id$ $CSID$")

#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bsl_fstream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntci {

EncryptionKeyStorage::~EncryptionKeyStorage()
{
}

ntsa::Error EncryptionKeyStorage::loadKey(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    const bsl::string&                    path,
    bslma::Allocator*                     basicAllocator)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->loadKey(result, path, options, basicAllocator);
}

ntsa::Error EncryptionKeyStorage::loadKey(
    bsl::shared_ptr<ntci::EncryptionKey>*    result,
    const bsl::string&                       path,
    const ntca::EncryptionKeyStorageOptions& options,
    bslma::Allocator*                        basicAllocator)
{
    bsl::fstream fs(path.c_str(), bsl::ios_base::in);
    if (!fs) {
        return ntsa::Error::last();
    }

    return this->decodeKey(result, fs.rdbuf(), options, basicAllocator);
}

ntsa::Error EncryptionKeyStorage::saveKey(
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const bsl::string&                          path)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->saveKey(privateKey, path, options);
}

ntsa::Error EncryptionKeyStorage::saveKey(
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const bsl::string&                          path,
    const ntca::EncryptionKeyStorageOptions&    options)
{
    bsl::fstream fs(path.c_str(), bsl::ios_base::out);
    if (!fs) {
        return ntsa::Error::last();
    }

    return this->encodeKey(fs.rdbuf(), privateKey, options);
}

ntsa::Error EncryptionKeyStorage::encodeKey(
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->encodeKey(destination, privateKey, options);
}

ntsa::Error EncryptionKeyStorage::encodeKey(
    bsl::streambuf*                             destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionKeyStorageOptions&    options)
{
    NTCCFG_WARNING_UNUSED(destination);
    NTCCFG_WARNING_UNUSED(privateKey);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionKeyStorage::encodeKey(
    bdlbb::Blob*                                destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->encodeKey(destination, privateKey, options);
}

ntsa::Error EncryptionKeyStorage::encodeKey(
    bdlbb::Blob*                                destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionKeyStorageOptions&    options)
{
    ntsa::Error error;

    bdlbb::OutBlobStreamBuf osb(destination);

    error = this->encodeKey(&osb, privateKey, options);
    if (error) {
        return error;
    }

    osb.pubsync();
    return ntsa::Error();
}

ntsa::Error EncryptionKeyStorage::encodeKey(
    bsl::string*                                destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->encodeKey(destination, privateKey, options);
}

ntsa::Error EncryptionKeyStorage::encodeKey(
    bsl::string*                                destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionKeyStorageOptions&    options)
{
    ntsa::Error error;

    bsl::ostringstream oss;

    error = this->encodeKey(oss.rdbuf(), privateKey, options);
    if (error) {
        return error;
    }

    oss.flush();
    *destination = oss.str();

    return ntsa::Error();
}

ntsa::Error EncryptionKeyStorage::encodeKey(
    bsl::vector<char>*                          destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->encodeKey(destination, privateKey, options);
}

ntsa::Error EncryptionKeyStorage::encodeKey(
    bsl::vector<char>*                          destination,
    const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
    const ntca::EncryptionKeyStorageOptions&    options)
{
    ntsa::Error error;

    bdlsb::MemOutStreamBuf osb;

    error = this->encodeKey(&osb, privateKey, options);
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

ntsa::Error EncryptionKeyStorage::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    bsl::streambuf*                       source,
    bslma::Allocator*                     basicAllocator)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->decodeKey(result, source, options, basicAllocator);
}

ntsa::Error EncryptionKeyStorage::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>*    result,
    bsl::streambuf*                          source,
    const ntca::EncryptionKeyStorageOptions& options,
    bslma::Allocator*                        basicAllocator)
{
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(source);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(basicAllocator);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionKeyStorage::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    const bdlbb::Blob&                    source,
    bslma::Allocator*                     basicAllocator)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->decodeKey(result, source, options, basicAllocator);
}

ntsa::Error EncryptionKeyStorage::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>*    result,
    const bdlbb::Blob&                       source,
    const ntca::EncryptionKeyStorageOptions& options,
    bslma::Allocator*                        basicAllocator)
{
    bdlbb::InBlobStreamBuf isb(&source);
    return this->decodeKey(result, &isb, options, basicAllocator);
}

ntsa::Error EncryptionKeyStorage::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    const bsl::string&                    source,
    bslma::Allocator*                     basicAllocator)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->decodeKey(result, source, options, basicAllocator);
}

ntsa::Error EncryptionKeyStorage::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>*    result,
    const bsl::string&                       source,
    const ntca::EncryptionKeyStorageOptions& options,
    bslma::Allocator*                        basicAllocator)
{
    bdlsb::FixedMemInStreamBuf isb(source.data(), source.size());
    return this->decodeKey(result, &isb, options, basicAllocator);
}

ntsa::Error EncryptionKeyStorage::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>* result,
    const bsl::vector<char>&              source,
    bslma::Allocator*                     basicAllocator)
{
    ntca::EncryptionKeyStorageOptions options;
    return this->decodeKey(result, source, options, basicAllocator);
}

ntsa::Error EncryptionKeyStorage::decodeKey(
    bsl::shared_ptr<ntci::EncryptionKey>*    result,
    const bsl::vector<char>&                 source,
    const ntca::EncryptionKeyStorageOptions& options,
    bslma::Allocator*                        basicAllocator)
{
    bdlsb::FixedMemInStreamBuf isb(&source.front(), source.size());
    return this->decodeKey(result, &isb, options, basicAllocator);
}

}  // close package namespace
}  // close enterprise namespace
