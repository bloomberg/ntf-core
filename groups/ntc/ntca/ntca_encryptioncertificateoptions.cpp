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

#include <ntca_encryptioncertificateoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptioncertificateoptions_cpp, "$Id$ $CSID$")

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>

namespace BloombergLP {
namespace ntca {

namespace {
bsls::AtomicUint s_nextSerialNumber;
}  // close unnamed namespace

EncryptionCertificateOptions::EncryptionCertificateOptions()
: d_serialNumber(s_nextSerialNumber++)
, d_startTime()
, d_expirationTime()
, d_authority(false)
{
    d_startTime = bdlt::CurrentTime::asDatetimeTz();

    bdlt::Datetime localExpirationTime = d_startTime.localDatetime();
    localExpirationTime.addDays(365);

    d_expirationTime.setDatetimeTz(localExpirationTime, d_startTime.offset());
}

void EncryptionCertificateOptions::setSerialNumber(int serialNumber)
{
    d_serialNumber = serialNumber;
}

void EncryptionCertificateOptions::setStartTime(
    const bdlt::DatetimeTz& startTime)
{
    d_startTime = startTime;
}

void EncryptionCertificateOptions::setExpirationTime(
    const bdlt::DatetimeTz& expirationTime)
{
    d_expirationTime = expirationTime;
}

void EncryptionCertificateOptions::setAuthority(bool authority)
{
    d_authority = authority;
}

int EncryptionCertificateOptions::serialNumber() const
{
    return d_serialNumber;
}

const bdlt::DatetimeTz& EncryptionCertificateOptions::startTime() const
{
    return d_startTime;
}

const bdlt::DatetimeTz& EncryptionCertificateOptions::expirationTime() const
{
    return d_expirationTime;
}

bool EncryptionCertificateOptions::authority() const
{
    return d_authority;
}

bsl::ostream& EncryptionCertificateOptions::print(bsl::ostream& stream,
                                                  int           level,
                                                  int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("d_serialNumber", d_serialNumber);
    printer.printAttribute("d_startTime", d_startTime);
    printer.printAttribute("d_expirationTime", d_expirationTime);
    printer.printAttribute("d_authority", d_authority);
    printer.end();
    return stream;
}

bool operator==(const EncryptionCertificateOptions& lhs,
                const EncryptionCertificateOptions& rhs)
{
    return lhs.serialNumber() == rhs.serialNumber() &&
           lhs.startTime() == rhs.startTime() &&
           lhs.expirationTime() == rhs.expirationTime() &&
           lhs.authority() == rhs.authority();
}

bool operator!=(const EncryptionCertificateOptions& lhs,
                const EncryptionCertificateOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const EncryptionCertificateOptions& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
