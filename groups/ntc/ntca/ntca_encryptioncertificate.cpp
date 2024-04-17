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

#include <ntca_encryptioncertificate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptioncertificate_cpp, "$Id$ $CSID$")

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




EncryptionCertificateTemplate::EncryptionCertificateTemplate(bslma::Allocator* basicAllocator)
: d_value(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{

}

EncryptionCertificateTemplate::EncryptionCertificateTemplate(
    const EncryptionCertificateTemplate& original,
    bslma::Allocator*            basicAllocator)
: d_value(original.d_value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionCertificateTemplate::~EncryptionCertificateTemplate()
{
}

EncryptionCertificateTemplate& EncryptionCertificateTemplate::operator=(
    const EncryptionCertificateTemplate& other)
{
    if (this != &other) {
        d_value = other.d_value;
    }

    return *this;
}

void EncryptionCertificateTemplate::reset()
{
    d_value = 0;
}

void EncryptionCertificateTemplate::setValue(bsl::size_t value)
{
    d_value = value;
}

bsl::size_t EncryptionCertificateTemplate::value() const
{
    return d_value;
}

bool EncryptionCertificateTemplate::equals(const EncryptionCertificateTemplate& other) const
{
    return d_value == other.d_value;
}

bool EncryptionCertificateTemplate::less(const EncryptionCertificateTemplate& other) const
{
    #if 0
    if (d_value < other.d_value) {
        return true;
    }

    if (other.d_value < d_value) {
        return false;
    }
    #endif

    return d_value < other.d_value;
}

bsl::ostream& EncryptionCertificateTemplate::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const EncryptionCertificateTemplate& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateTemplate& lhs,
                const EncryptionCertificateTemplate& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateTemplate& lhs,
                const EncryptionCertificateTemplate& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateTemplate& lhs,
               const EncryptionCertificateTemplate& rhs)
{
    return lhs.less(rhs);
}







































EncryptionCertificate::EncryptionCertificate(bslma::Allocator* basicAllocator)
: d_serialNumber(0)
, d_startTime()
, d_expirationTime()
, d_authority(false)
, d_hosts(basicAllocator)
{
    d_startTime = bdlt::CurrentTime::asDatetimeTz();

    bdlt::Datetime localExpirationTime = d_startTime.localDatetime();
    localExpirationTime.addDays(365);

    d_expirationTime.setDatetimeTz(localExpirationTime, d_startTime.offset());
}

EncryptionCertificate::EncryptionCertificate(
    const EncryptionCertificate& original,
    bslma::Allocator*            basicAllocator)
: d_serialNumber(original.d_serialNumber)
, d_startTime(original.d_startTime)
, d_expirationTime(original.d_expirationTime)
, d_authority(original.d_authority)
, d_hosts(original.d_hosts, basicAllocator)
{
}

EncryptionCertificate::~EncryptionCertificate()
{
}

EncryptionCertificate& EncryptionCertificate::operator=(
    const EncryptionCertificate& other)
{
    if (this != &other) {
        d_serialNumber   = other.d_serialNumber;
        d_startTime      = other.d_startTime;
        d_expirationTime = other.d_expirationTime;
        d_authority      = other.d_authority;
        d_hosts          = other.d_hosts;
    }

    return *this;
}

void EncryptionCertificate::reset()
{
    d_serialNumber   = 0;
    d_startTime      = bdlt::DatetimeTz();
    d_expirationTime = bdlt::DatetimeTz();
    d_authority      = false;
    d_hosts.clear();
}

void EncryptionCertificate::setSerialNumber(int value)
{
    d_serialNumber = value;
}

void EncryptionCertificate::setStartTime(const bdlt::DatetimeTz& value)
{
    d_startTime = value;
}

void EncryptionCertificate::setExpirationTime(const bdlt::DatetimeTz& value)
{
    d_expirationTime = value;
}

void EncryptionCertificate::setAuthority(bool value)
{
    d_authority = value;
}

void EncryptionCertificate::setHostList(const bsl::vector<bsl::string>& value)
{
    d_hosts = value;
}

void EncryptionCertificate::addHost(const bsl::string& value)
{
    d_hosts.push_back(value);
}


ntsa::Error EncryptionCertificate::decode(
    ntsa::AbstractSyntaxDecoder* decoder)
{
    NTCCFG_WARNING_UNUSED(decoder);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error EncryptionCertificate::encode(
    ntsa::AbstractSyntaxEncoder* encoder) const
{
    NTCCFG_WARNING_UNUSED(encoder);

    NTCCFG_NOT_IMPLEMENTED();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

int EncryptionCertificate::serialNumber() const
{
    ntsa::Error error;
    
    int result = 0;
    error = d_serialNumber.convert(&result);
    if (error) {
        return 0;
    }

    return result;
}

const bdlt::DatetimeTz& EncryptionCertificate::startTime() const
{
    return d_startTime;
}

const bdlt::DatetimeTz& EncryptionCertificate::expirationTime() const
{
    return d_expirationTime;
}

bool EncryptionCertificate::authority() const
{
    return d_authority;
}

const bsl::vector<bsl::string>& EncryptionCertificate::hosts() const
{
    return d_hosts;
}

bool EncryptionCertificate::equals(const EncryptionCertificate& other) const
{
    return d_serialNumber == other.d_serialNumber &&
           d_startTime == other.d_startTime &&
           d_expirationTime == other.d_expirationTime &&
           d_authority == other.d_authority && d_hosts == other.d_hosts;
    ;
}

bool EncryptionCertificate::less(const EncryptionCertificate& other) const
{
    if (d_serialNumber < other.d_serialNumber) {
        return true;
    }

    if (other.d_serialNumber < d_serialNumber) {
        return false;
    }

    if (d_startTime.gmtDatetime() < other.d_startTime.gmtDatetime()) {
        return true;
    }

    if (other.d_startTime.gmtDatetime() < d_startTime.gmtDatetime()) {
        return false;
    }

    if (d_expirationTime.gmtDatetime() < other.d_expirationTime.gmtDatetime())
    {
        return true;
    }

    if (other.d_expirationTime.gmtDatetime() < d_expirationTime.gmtDatetime())
    {
        return false;
    }

    if (d_authority < other.d_authority) {
        return true;
    }

    if (other.d_authority < d_authority) {
        return false;
    }

    return d_hosts < other.d_hosts;
}

bsl::ostream& EncryptionCertificate::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("serialNumber", d_serialNumber);
    printer.printAttribute("startTime", d_startTime);
    printer.printAttribute("expirationTime", d_expirationTime);
    printer.printAttribute("authority", d_authority);
    printer.printAttribute("hosts", d_hosts);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const EncryptionCertificate& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificate& lhs,
                const EncryptionCertificate& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificate& lhs,
                const EncryptionCertificate& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificate& lhs,
               const EncryptionCertificate& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
