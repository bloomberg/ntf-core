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

#include <ntci_encryptionkey.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_encryptionkey_cpp, "$Id$ $CSID$")

#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bsl_fstream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntci {

EncryptionKey::~EncryptionKey()
{
}

ntsa::Error EncryptionKey::decode(bsl::streambuf* source)
{
    ntca::EncryptionResourceOptions options;
    return this->decode(source, options);
}

ntsa::Error EncryptionKey::decode(
    bsl::streambuf*                        source,
    const ntca::EncryptionResourceOptions& options)
{
    NTCCFG_WARNING_UNUSED(source);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionKey::decode(const bdlbb::Blob& source)
{
    ntca::EncryptionResourceOptions options;
    return this->decode(source, options);
}

ntsa::Error EncryptionKey::decode(
    const bdlbb::Blob&                     source,
    const ntca::EncryptionResourceOptions& options)
{
    bdlbb::InBlobStreamBuf isb(&source);
    return this->decode(&isb, options);
}

ntsa::Error EncryptionKey::decode(const bsl::string& source)
{
    ntca::EncryptionResourceOptions options;
    return this->decode(source, options);
}

ntsa::Error EncryptionKey::decode(
    const bsl::string&                     source,
    const ntca::EncryptionResourceOptions& options)
{
    if (!source.empty()) {
        bdlsb::FixedMemInStreamBuf isb(source.data(), source.size());
        return this->decode(&isb, options);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error EncryptionKey::decode(const bsl::vector<char>& source)
{
    ntca::EncryptionResourceOptions options;
    return this->decode(source, options);
}

ntsa::Error EncryptionKey::decode(
    const bsl::vector<char>&               source,
    const ntca::EncryptionResourceOptions& options)
{
    if (!source.empty()) {
        bdlsb::FixedMemInStreamBuf isb(&source.front(), source.size());
        return this->decode(&isb, options);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

ntsa::Error EncryptionKey::encode(bsl::streambuf* destination) const
{
    ntca::EncryptionResourceOptions options;
    return this->encode(destination, options);
}

ntsa::Error EncryptionKey::encode(
    bsl::streambuf*                        destination,
    const ntca::EncryptionResourceOptions& options) const
{
    NTCCFG_WARNING_UNUSED(destination);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionKey::encode(bdlbb::Blob* destination) const
{
    ntca::EncryptionResourceOptions options;
    return this->encode(destination, options);
}

ntsa::Error EncryptionKey::encode(
    bdlbb::Blob*                           destination,
    const ntca::EncryptionResourceOptions& options) const
{
    ntsa::Error error;

    bdlbb::OutBlobStreamBuf osb(destination);

    error = this->encode(&osb, options);
    if (error) {
        return error;
    }

    osb.pubsync();
    return ntsa::Error();
}

ntsa::Error EncryptionKey::encode(bsl::string* destination) const
{
    ntca::EncryptionResourceOptions options;
    return this->encode(destination, options);
}

ntsa::Error EncryptionKey::encode(
    bsl::string*                           destination,
    const ntca::EncryptionResourceOptions& options) const
{
    ntsa::Error error;

    bsl::ostringstream oss;

    error = this->encode(oss.rdbuf(), options);
    if (error) {
        return error;
    }

    oss.flush();
    *destination = oss.str();

    return ntsa::Error();
}

ntsa::Error EncryptionKey::encode(bsl::vector<char>* destination) const
{
    ntca::EncryptionResourceOptions options;
    return this->encode(destination, options);
}

ntsa::Error EncryptionKey::encode(
    bsl::vector<char>*                     destination,
    const ntca::EncryptionResourceOptions& options) const
{
    ntsa::Error error;

    bdlsb::MemOutStreamBuf osb;

    error = this->encode(&osb, options);
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

void* EncryptionKey::handle() const
{
    return 0;
}

}  // close package namespace
}  // close enterprise namespace
