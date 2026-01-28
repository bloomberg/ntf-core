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

#include <ntsa_adapter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_adapter_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

Adapter::Adapter(bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_description(basicAllocator)
, d_index(0)
, d_ethernetAddress(basicAllocator)
, d_ipv4Address()
, d_ipv6Address()
, d_speed(0)
, d_flags(0)
{
}

Adapter::Adapter(const Adapter& other, bslma::Allocator* basicAllocator)
: d_name(other.d_name, basicAllocator)
, d_description(other.d_description, basicAllocator)
, d_index(other.d_index)
, d_ethernetAddress(other.d_ethernetAddress, basicAllocator)
, d_ipv4Address(other.d_ipv4Address)
, d_ipv6Address(other.d_ipv6Address)
, d_speed(other.d_speed)
, d_flags(other.d_flags)
{
}

Adapter::~Adapter()
{
}

Adapter& Adapter::operator=(const Adapter& other)
{
    if (this != &other) {
        d_name            = other.d_name;
        d_description     = other.d_description;
        d_index           = other.d_index;
        d_ethernetAddress = other.d_ethernetAddress;
        d_ipv4Address     = other.d_ipv4Address;
        d_ipv6Address     = other.d_ipv6Address;
        d_speed           = other.d_speed;
        d_flags           = other.d_flags;
    }

    return *this;
}

void Adapter::reset()
{
    d_name.clear();
    d_description.clear();
    d_index = 0;
    d_ethernetAddress.clear();
    d_ipv4Address.reset();
    d_ipv6Address.reset();
    d_speed = 0;
    d_flags = 0;
}

void Adapter::setName(const bsl::string& value)
{
    d_name = value;
}

void Adapter::setDescription(const bsl::string& value)
{
    d_description = value;
}

void Adapter::setIndex(bsl::uint32_t value)
{
    d_index = value;
}

void Adapter::setEthernetAddress(const bsl::string& value)
{
    d_ethernetAddress = value;
}

void Adapter::setIpv4Address(const ntsa::Ipv4Address& value)
{
    d_ipv4Address = value;
}

void Adapter::setIpv6Address(const ntsa::Ipv6Address& value)
{
    d_ipv6Address = value;
}

void Adapter::setSpeed(bsl::size_t value)
{
    d_speed = value;
}

void Adapter::setMulticast(bool value)
{
    if (value) {
        d_flags |= e_MULTICAST;
    }
    else {
        d_flags &= ~e_MULTICAST;
    }
}

void Adapter::setTxSoftwareTimestamps(bool value)
{
    if (value) {
        d_flags |= e_TX_SOFTWARE_TIMESTAMPS;
    }
    else {
        d_flags &= ~e_TX_SOFTWARE_TIMESTAMPS;
    }
}

void Adapter::setTxHardwareTimestamps(bool value)
{
    if (value) {
        d_flags |= e_TX_HARDWARE_TIMESTAMPS;
    }
    else {
        d_flags &= ~e_TX_HARDWARE_TIMESTAMPS;
    }
}

void Adapter::setRxSoftwareTimestamps(bool value)
{
    if (value) {
        d_flags |= e_RX_SOFTWARE_TIMESTAMPS;
    }
    else {
        d_flags &= ~e_RX_SOFTWARE_TIMESTAMPS;
    }
}

void Adapter::setRxHardwareTimestamps(bool value)
{
    if (value) {
        d_flags |= e_RX_HARDWARE_TIMESTAMPS;
    }
    else {
        d_flags &= ~e_RX_HARDWARE_TIMESTAMPS;
    }
}

const bsl::string& Adapter::name() const
{
    return d_name;
}

const bsl::string& Adapter::description() const
{
    return d_description;
}

bsl::uint32_t Adapter::index() const
{
    return d_index;
}

const bsl::string& Adapter::ethernetAddress() const
{
    return d_ethernetAddress;
}

const bdlb::NullableValue<ntsa::Ipv4Address>& Adapter::ipv4Address() const
{
    return d_ipv4Address;
}

const bdlb::NullableValue<ntsa::Ipv6Address>& Adapter::ipv6Address() const
{
    return d_ipv6Address;
}

bsl::size_t Adapter::speed() const
{
    return d_speed;
}

bool Adapter::multicast() const
{
    return (d_flags & e_MULTICAST) != 0;
}

bool Adapter::txSoftwareTimestamps() const
{
    return (d_flags & e_TX_SOFTWARE_TIMESTAMPS) != 0;
}

bool Adapter::txHardwareTimestamps() const
{
    return (d_flags & e_TX_HARDWARE_TIMESTAMPS) != 0;
}

bool Adapter::rxSoftwareTimestamps() const
{
    return (d_flags & e_RX_SOFTWARE_TIMESTAMPS) != 0;
}

bool Adapter::rxHardwareTimestamps() const
{
    return (d_flags & e_RX_HARDWARE_TIMESTAMPS) != 0;
}

bool Adapter::equals(const Adapter& other) const
{
    return d_name == other.d_name &&
           d_description == other.d_description &&
           d_index == other.d_index &&
           d_ethernetAddress == other.d_ethernetAddress &&
           d_ipv4Address == other.d_ipv4Address &&
           d_ipv6Address == other.d_ipv6Address &&
           d_speed == other.d_speed &&
           d_flags == other.d_flags;
}

bool Adapter::less(const Adapter& other) const
{
    if (d_name < other.d_name) {
        return true;
    }

    if (other.d_name < d_name) {
        return false;
    }

    if (d_description < other.d_description) {
        return true;
    }

    if (other.d_description < d_description) {
        return false;
    }

    if (d_index < other.d_index) {
        return true;
    }

    if (other.d_index < d_index) {
        return false;
    }

    if (d_ethernetAddress < other.d_ethernetAddress) {
        return true;
    }

    if (other.d_ethernetAddress < d_ethernetAddress) {
        return false;
    }

    if (d_ipv4Address < other.d_ipv4Address) {
        return true;
    }

    if (other.d_ipv4Address < d_ipv4Address) {
        return false;
    }

    if (d_ipv6Address < other.d_ipv6Address) {
        return true;
    }

    if (other.d_ipv6Address < d_ipv6Address) {
        return false;
    }

    if (d_speed < other.d_speed) {
        return true;
    }

    if (other.d_speed < d_speed) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& Adapter::print(bsl::ostream& stream,
                             int           level,
                             int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("index", d_index);

    if (!d_name.empty()) {
        printer.printAttribute("name", d_name);
    }

    if (!d_description.empty()) {
        printer.printAttribute("description", d_description);
    }

    if (!d_ethernetAddress.empty()) {
        printer.printAttribute("ethernetAddress", d_ethernetAddress);
    }

    if (d_ipv4Address.has_value()) {
        printer.printAttribute("ipv4Address", d_ipv4Address.value());
    }

    if (d_ipv6Address.has_value()) {
        printer.printAttribute("ipv6Address", d_ipv6Address.value());
    }

    if (d_speed > 0) {
        printer.printAttribute("speed", d_speed);
    }

    if (d_flags > 0) {
        printer.printForeign(d_flags, &Adapter::printFlags, "flags");
    }

    printer.end();
    return stream;
}

bsl::ostream& Adapter::printFlags(bsl::ostream& stream,
                                  bsl::uint32_t flags,
                                  int           level,
                                  int           spacesPerLevel)
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    bsl::size_t count = 0;

    if ((flags & e_MULTICAST) != 0) {
        if (count++ > 0) {
            stream << ' ';
        }

        stream << "MULTICAST";
    }

    if ((flags & e_TX_SOFTWARE_TIMESTAMPS) != 0) {
        if (count++ > 0) {
            stream << ' ';
        }

        stream << "TX_SOFTWARE_TIMESTAMPS";
    }

    if ((flags & e_TX_HARDWARE_TIMESTAMPS) != 0) {
        if (count++ > 0) {
            stream << ' ';
        }

        stream << "TX_HARDWARE_TIMESTAMPS";
    }

    if ((flags & e_RX_SOFTWARE_TIMESTAMPS) != 0) {
        if (count++ > 0) {
            stream << ' ';
        }

        stream << "RX_SOFTWARE_TIMESTAMPS";
    }

    if ((flags & e_RX_HARDWARE_TIMESTAMPS) != 0) {
        if (count++ > 0) {
            stream << ' ';
        }

        stream << "RX_HARDWARE_TIMESTAMPS";
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Adapter& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Adapter& lhs, const Adapter& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Adapter& lhs, const Adapter& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Adapter& lhs, const Adapter& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
