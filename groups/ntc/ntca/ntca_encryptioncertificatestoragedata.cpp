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

#include <ntca_encryptioncertificatestoragedata.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptioncertificatestoragedata_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

EncryptionCertificateStorageData::EncryptionCertificateStorageData(
    bslma::Allocator* basicAllocator)
: d_descriptor(basicAllocator)
, d_options(basicAllocator)
{
}

EncryptionCertificateStorageData::EncryptionCertificateStorageData(
    const EncryptionCertificateStorageData& original,
    bslma::Allocator*                       basicAllocator)
: d_descriptor(original.d_descriptor, basicAllocator)
, d_options(original.d_options, basicAllocator)
{
}

EncryptionCertificateStorageData::~EncryptionCertificateStorageData()
{
}

EncryptionCertificateStorageData& EncryptionCertificateStorageData::operator=(
    const EncryptionCertificateStorageData& other)
{
    if (this != &other) {
        d_descriptor = other.d_descriptor;
        d_options    = other.d_options;
    }

    return *this;
}

void EncryptionCertificateStorageData::reset()
{
    d_descriptor.reset();
    d_options.reset();
}

void EncryptionCertificateStorageData::setDescriptor(
    const ntca::EncryptionCertificateStorageDescriptor& value)
{
    d_descriptor = value;
}

void EncryptionCertificateStorageData::setOptions(
    const ntca::EncryptionCertificateStorageOptions& value)
{
    d_options = value;
}

const ntca::EncryptionCertificateStorageDescriptor&
EncryptionCertificateStorageData::descriptor() const
{
    return d_descriptor;
}

const bdlb::NullableValue<ntca::EncryptionCertificateStorageOptions>&
EncryptionCertificateStorageData::options() const
{
    return d_options;
}

bool EncryptionCertificateStorageData::equals(
    const EncryptionCertificateStorageData& other) const
{
    return d_descriptor == other.d_descriptor && d_options == other.d_options;
}

bool EncryptionCertificateStorageData::less(
    const EncryptionCertificateStorageData& other) const
{
    if (d_descriptor < other.d_descriptor) {
        return true;
    }

    if (other.d_descriptor < d_descriptor) {
        return false;
    }

    return d_options < other.d_options;
}

bsl::ostream& EncryptionCertificateStorageData::print(bsl::ostream& stream,
                                                      int           level,
                                                      int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("descriptor", d_descriptor);

    if (!d_options.isNull()) {
        printer.printAttribute("options", d_options);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                           stream,
                         const EncryptionCertificateStorageData& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateStorageData& lhs,
                const EncryptionCertificateStorageData& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateStorageData& lhs,
                const EncryptionCertificateStorageData& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateStorageData& lhs,
               const EncryptionCertificateStorageData& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
