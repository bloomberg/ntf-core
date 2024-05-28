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

#include <ntca_encryptionvalidation.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionvalidation_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

EncryptionValidation::EncryptionValidation(bslma::Allocator* basicAllocator)
: d_host(basicAllocator)
, d_mail(basicAllocator)
, d_usage(basicAllocator)
, d_usageExtensions(basicAllocator)
, d_allowSelfSigned()
, d_allowAll()
, d_callback(basicAllocator)
{
}

EncryptionValidation::EncryptionValidation(
    const EncryptionValidation& original,
    bslma::Allocator*           basicAllocator)
: d_host(original.d_host, basicAllocator)
, d_mail(original.d_mail, basicAllocator)
, d_usage(original.d_usage, basicAllocator)
, d_usageExtensions(original.d_usageExtensions, basicAllocator)
, d_allowSelfSigned(original.d_allowSelfSigned)
, d_allowAll(original.d_allowAll)
, d_callback(original.d_callback, basicAllocator)
{
}

EncryptionValidation::~EncryptionValidation()
{
}

EncryptionValidation& EncryptionValidation::operator=(
    const EncryptionValidation& other)
{
    if (this != &other) {
        d_host            = other.d_host;
        d_mail            = other.d_mail;
        d_usage           = other.d_usage;
        d_usageExtensions = other.d_usageExtensions;
        d_allowSelfSigned = other.d_allowSelfSigned;
        d_allowAll        = other.d_allowAll;
        d_callback        = other.d_callback;
    }

    return *this;
}

void EncryptionValidation::reset()
{
    d_host.reset();
    d_mail.reset();
    d_usage.reset();
    d_usageExtensions.reset();
    d_allowSelfSigned.reset();
    d_allowAll.reset();
    d_callback.reset();
}

void EncryptionValidation::requireHost(const bsl::string& value)
{
    ntsa::Uri uri;
    if (uri.parse(value)) {
        this->requireHost(uri);
    }
    else {
        if (d_host.isNull()) {
            d_host.makeValue();
        }
        d_host.value().push_back(value);
    }
}

void EncryptionValidation::requireHost(const ntsa::Endpoint& value)
{
    if (value.isIp()) {
        this->requireHost(value.ip());
    }
    else if (value.isLocal()) {
        this->requireHost(value.local());
    }
}

void EncryptionValidation::requireHost(const ntsa::IpEndpoint& value)
{
    this->requireHost(value.host());
}

void EncryptionValidation::requireHost(const ntsa::IpAddress& value)
{
    if (value.isV4()) {
        this->requireHost(value.v4());
    }
    else if (value.isV6()) {
        this->requireHost(value.v6());
    }
}

void EncryptionValidation::requireHost(const ntsa::Ipv4Address& value)
{
    if (d_host.isNull()) {
        d_host.makeValue();
    }

    d_host.value().push_back(value.text());
}

void EncryptionValidation::requireHost(const ntsa::Ipv6Address& value)
{
    if (d_host.isNull()) {
        d_host.makeValue();
    }

    d_host.value().push_back(value.text());
}

void EncryptionValidation::requireHost(const ntsa::LocalName& value)
{
    if (d_host.isNull()) {
        d_host.makeValue();
    }

    d_host.value().push_back(value.value());
}

void EncryptionValidation::requireHost(const ntsa::Host& value)
{
    if (value.isDomainName()) {
        this->requireHost(value.domainName());
    }
    else if (value.isIp()) {
        this->requireHost(value.ip());
    }
}

void EncryptionValidation::requireHost(const ntsa::DomainName& value)
{
    if (d_host.isNull()) {
        d_host.makeValue();
    }

    d_host.value().push_back(value.text());
}

void EncryptionValidation::requireHost(const ntsa::Uri& value)
{
    if (!value.authority().isNull()) {
        const ntsa::UriAuthority& authority = value.authority().value();
        if (!authority.host().isNull()) {
            const ntsa::Host& host = authority.host().value();
            this->requireHost(host);
        }
    }
}

void EncryptionValidation::requireMail(const bsl::string& value)
{
    if (d_mail.isNull()) {
        d_mail.makeValue();
    }

    d_mail.value().push_back(value);
}

void EncryptionValidation::requireUsage(
    ntca::EncryptionCertificateSubjectKeyUsageType::Value value)
{
    if (d_usage.isNull()) {
        d_usage.makeValue();
    }

    d_usage.value().push_back(value);
}

void EncryptionValidation::requireUsageExtension(
    ntca::EncryptionCertificateSubjectKeyUsageExtendedType::Value value)
{
    if (d_usageExtensions.isNull()) {
        d_usageExtensions.makeValue();
    }

    d_usageExtensions.value().addIdentifier(value);
}

void EncryptionValidation::requireUsageExtension(
    const ntsa::AbstractObjectIdentifier& value)
{
    if (d_usageExtensions.isNull()) {
        d_usageExtensions.makeValue();
    }

    d_usageExtensions.value().addIdentifier(value);
}

void EncryptionValidation::requireUsageExtension(
    const ntca::EncryptionCertificateSubjectKeyUsageExtended& value)
{
    for (bsl::size_t i = 0; i < value.identifierList().size(); ++i) {
        if (d_usageExtensions.isNull()) {
            d_usageExtensions.makeValue();
        }

        d_usageExtensions.value().addIdentifier(value.identifierList()[i]);
    }
}

void EncryptionValidation::permitSelfSigned(bool value)
{
    d_allowSelfSigned = value;
}

void EncryptionValidation::permitAll(bool value)
{
    d_allowAll = value;
}

void EncryptionValidation::setCallback(
    const ntca::EncryptionCertificateValidator& callback)
{
    d_callback = callback;
}

const EncryptionValidation::NullableStringVector& EncryptionValidation::host()
    const
{
    return d_host;
}

const EncryptionValidation::NullableStringVector& EncryptionValidation::mail()
    const
{
    return d_mail;
}

const EncryptionValidation::NullableUsageVector& EncryptionValidation::usage()
    const
{
    return d_usage;
}

const EncryptionValidation::NullableUsageExtended& EncryptionValidation::
    usageExtensions() const
{
    return d_usageExtensions;
}

const EncryptionValidation::NullableBool& EncryptionValidation::
    allowSelfSigned() const
{
    return d_allowSelfSigned;
}

const EncryptionValidation::NullableBool& EncryptionValidation::allowAll()
    const
{
    return d_allowAll;
}

const bdlb::NullableValue<ntca::EncryptionCertificateValidator>&
EncryptionValidation::callback() const
{
    return d_callback;
}

bool EncryptionValidation::equals(const EncryptionValidation& other) const
{
    return d_host == other.d_host && d_mail == other.d_mail &&
           d_usage == other.d_usage &&
           d_usageExtensions == other.d_usageExtensions &&
           d_allowSelfSigned == other.d_allowSelfSigned &&
           d_allowAll == other.d_allowAll;
}

bool EncryptionValidation::less(const EncryptionValidation& other) const
{
    if (d_host < other.d_host) {
        return true;
    }

    if (other.d_host < d_host) {
        return false;
    }

    if (d_mail < other.d_mail) {
        return true;
    }

    if (other.d_mail < d_mail) {
        return false;
    }

    if (d_usage < other.d_usage) {
        return true;
    }

    if (other.d_usage < d_usage) {
        return false;
    }

    if (d_usageExtensions < other.d_usageExtensions) {
        return true;
    }

    if (other.d_usageExtensions < d_usageExtensions) {
        return false;
    }

    if (d_allowSelfSigned < other.d_allowSelfSigned) {
        return true;
    }

    if (other.d_allowSelfSigned < d_allowSelfSigned) {
        return false;
    }

    return d_allowAll < other.d_allowAll;
}

bsl::ostream& EncryptionValidation::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_host.isNull() && !d_host.value().empty()) {
        printer.printAttribute("host", d_host.value());
    }

    if (!d_mail.isNull() && !d_mail.value().empty()) {
        printer.printAttribute("mail", d_mail.value());
    }

    if (!d_usage.isNull() && !d_usage.value().empty()) {
        printer.printAttribute("usage", d_usage.value());
    }

    if (!d_usageExtensions.isNull() &&
        !d_usageExtensions.value().identifierList().empty())
    {
        printer.printAttribute("usageExtensions", d_usageExtensions.value());
    }

    if (!d_allowSelfSigned.isNull()) {
        printer.printAttribute("allowSelfSigned", d_allowSelfSigned);
    }

    if (!d_allowAll.isNull()) {
        printer.printAttribute("allowAll", d_allowAll);
    }

    printer.end();
    return stream;
}

bool operator==(const EncryptionValidation& lhs,
                const EncryptionValidation& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionValidation& lhs,
                const EncryptionValidation& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionValidation& lhs,
               const EncryptionValidation& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const EncryptionValidation& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
