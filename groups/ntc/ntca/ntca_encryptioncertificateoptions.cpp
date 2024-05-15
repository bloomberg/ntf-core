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

EncryptionCertificateOptions::EncryptionCertificateOptions(
    bslma::Allocator* basicAllocator)
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

EncryptionCertificateOptions::EncryptionCertificateOptions(
    const EncryptionCertificateOptions& original,
    bslma::Allocator*                   basicAllocator)
: d_serialNumber(original.d_serialNumber)
, d_startTime(original.d_startTime)
, d_expirationTime(original.d_expirationTime)
, d_authority(original.d_authority)
, d_hosts(original.d_hosts, basicAllocator)
{
}

EncryptionCertificateOptions::~EncryptionCertificateOptions()
{
}

EncryptionCertificateOptions& EncryptionCertificateOptions::operator=(
    const EncryptionCertificateOptions& other)
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

void EncryptionCertificateOptions::reset()
{
    d_serialNumber   = 0;
    d_startTime      = bdlt::DatetimeTz();
    d_expirationTime = bdlt::DatetimeTz();
    d_authority      = false;
    d_hosts.clear();
}

void EncryptionCertificateOptions::setSerialNumber(int value)
{
    d_serialNumber = value;
}

void EncryptionCertificateOptions::setStartTime(const bdlt::DatetimeTz& value)
{
    d_startTime = value;
}

void EncryptionCertificateOptions::setExpirationTime(
    const bdlt::DatetimeTz& value)
{
    d_expirationTime = value;
}

void EncryptionCertificateOptions::setAuthority(bool value)
{
    d_authority = value;
}

void EncryptionCertificateOptions::setHostList(
    const bsl::vector<bsl::string>& value)
{
    d_hosts = value;
}

void EncryptionCertificateOptions::addHost(const bsl::string& value)
{
    d_hosts.push_back(value);
}

void EncryptionCertificateOptions::addHost(const ntsa::Endpoint& value)
{
    if (value.isIp()) {
        this->addHost(value.ip());
    }
    else if (value.isLocal()) {
        this->addHost(value.local());
    }
}

void EncryptionCertificateOptions::addHost(const ntsa::IpEndpoint& value)
{
    this->addHost(value.host());
}

void EncryptionCertificateOptions::addHost(const ntsa::IpAddress& value)
{
    if (value.isV4()) {
        this->addHost(value.v4());
    }
    else if (value.isV6()) {
        this->addHost(value.v6());
    }
}

void EncryptionCertificateOptions::addHost(const ntsa::Ipv4Address& value)
{
    d_hosts.push_back(value.text());
}

void EncryptionCertificateOptions::addHost(const ntsa::Ipv6Address& value)
{
    d_hosts.push_back(value.text());
}

void EncryptionCertificateOptions::addHost(const ntsa::LocalName& value)
{
    d_hosts.push_back(value.value());
}

void EncryptionCertificateOptions::addHost(const ntsa::Host& value)
{
    if (value.isDomainName()) {
        this->addHost(value.domainName());
    }
    else if (value.isIp()) {
        this->addHost(value.ip());
    }
}

void EncryptionCertificateOptions::addHost(const ntsa::DomainName& value)
{
    d_hosts.push_back(value.text());
}

void EncryptionCertificateOptions::addHost(const ntsa::Uri& value)
{
    if (!value.authority().isNull()) {
        const ntsa::UriAuthority& authority = value.authority().value();
        if (!authority.host().isNull()) {
            const ntsa::Host& host = authority.host().value();
            this->addHost(host);
        }
    }
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

const bsl::vector<bsl::string>& EncryptionCertificateOptions::hosts() const
{
    return d_hosts;
}

bool EncryptionCertificateOptions::equals(
    const EncryptionCertificateOptions& other) const
{
    return d_serialNumber == other.d_serialNumber &&
           d_startTime == other.d_startTime &&
           d_expirationTime == other.d_expirationTime &&
           d_authority == other.d_authority && d_hosts == other.d_hosts;
    ;
}

bool EncryptionCertificateOptions::less(
    const EncryptionCertificateOptions& other) const
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

bsl::ostream& EncryptionCertificateOptions::print(bsl::ostream& stream,
                                                  int           level,
                                                  int spacesPerLevel) const
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

bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const EncryptionCertificateOptions& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionCertificateOptions& lhs,
                const EncryptionCertificateOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionCertificateOptions& lhs,
                const EncryptionCertificateOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionCertificateOptions& lhs,
               const EncryptionCertificateOptions& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
