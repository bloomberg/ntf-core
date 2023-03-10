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

namespace BloombergLP {
namespace ntsa {

Adapter::Adapter(bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_index(0)
, d_ethernetAddress(basicAllocator)
, d_ipv4Address()
, d_ipv6Address()
, d_multicast(false)
{
}

Adapter::Adapter(const Adapter& other, bslma::Allocator* basicAllocator)
: d_name(other.d_name, basicAllocator)
, d_index(other.d_index)
, d_ethernetAddress(other.d_ethernetAddress, basicAllocator)
, d_ipv4Address(other.d_ipv4Address)
, d_ipv6Address(other.d_ipv6Address)
, d_multicast(other.d_multicast)
{
}

Adapter::~Adapter()
{
}

Adapter& Adapter::operator=(const Adapter& other)
{
    if (this != &other) {
        d_name            = other.d_name;
        d_index           = other.d_index;
        d_ethernetAddress = other.d_ethernetAddress;
        d_ipv4Address     = other.d_ipv4Address;
        d_ipv6Address     = other.d_ipv6Address;
        d_multicast       = other.d_multicast;
    }

    return *this;
}

void Adapter::reset()
{
    d_name.clear();
    d_index = 0;
    d_ethernetAddress.clear();
    d_ipv4Address.reset();
    d_ipv6Address.reset();
    d_multicast = false;
}

void Adapter::setName(const bsl::string& name)
{
    d_name = name;
}

void Adapter::setIndex(bsl::uint32_t index)
{
    d_index = index;
}

void Adapter::setEthernetAddress(const bsl::string& ethernetAddress)
{
    d_ethernetAddress = ethernetAddress;
}

void Adapter::setIpv4Address(const ntsa::Ipv4Address& ipv4Address)
{
    d_ipv4Address = ipv4Address;
}

void Adapter::setIpv6Address(const ntsa::Ipv6Address& ipv6Address)
{
    d_ipv6Address = ipv6Address;
}

void Adapter::setMulticast(bool multicast)
{
    d_multicast = multicast;
}

const bsl::string& Adapter::name() const
{
    return d_name;
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

bool Adapter::multicast() const
{
    return d_multicast;
}

}  // close package namespace
}  // close enterprise namespace
