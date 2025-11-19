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

#include <ntcdns_compat.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcdns_compat_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcdns {

void Compat::convert(ntca::GetIpAddressOptions*      result,
                     const ntca::GetEndpointOptions& options)
{
    // Load into the specified 'result' the conversion of the specified
    // 'options'.

    if (!options.ipAddressFallback().isNull()) {
        result->setIpAddressFallback(options.ipAddressFallback().value());
    }

    if (!options.ipAddressSelector().isNull()) {
        result->setIpAddressSelector(options.ipAddressSelector().value());
    }

    if (!options.ipAddressFilter().isNull()) {
        result->setIpAddressFilter(options.ipAddressFilter().value());
    }

    if (!options.ipAddressType().isNull()) {
        result->setIpAddressType(options.ipAddressType().value());
    }

    if (!options.transport().isNull()) {
        result->setTransport(options.transport().value());
    }

    if (!options.deadline().isNull()) {
        result->setDeadline(options.deadline().value());
    }
}

void Compat::convert(ntca::GetPortOptions*           result,
                     const ntca::GetEndpointOptions& options)
{
    // Load into the specified 'result' the conversion of the specified
    // 'options'.

    if (!options.portFallback().isNull()) {
        result->setPortFallback(options.portFallback().value());
    }

    if (!options.portSelector().isNull()) {
        result->setPortSelector(options.portSelector().value());
    }

    if (!options.portFilter().isNull()) {
        result->setPortFilter(options.portFilter().value());
    }

    if (!options.transport().isNull()) {
        result->setTransport(options.transport().value());
    }

    if (!options.deadline().isNull()) {
        result->setDeadline(options.deadline().value());
    }
}

void Compat::convert(ntsa::IpAddressOptions*          result,
                     const ntca::GetIpAddressOptions& options)
{
    // Load into the specified 'result' the conversion of the specified
    // 'options'.

    if (!options.ipAddressFallback().isNull()) {
        result->setIpAddressFallback(options.ipAddressFallback().value());
    }

    if (!options.ipAddressSelector().isNull()) {
        result->setIpAddressSelector(options.ipAddressSelector().value());
    }

    if (!options.ipAddressFilter().isNull()) {
        result->setIpAddressFilter(options.ipAddressFilter().value());
    }

    if (!options.ipAddressType().isNull()) {
        result->setIpAddressType(options.ipAddressType().value());
    }

    if (!options.transport().isNull()) {
        result->setTransport(options.transport().value());
    }
}

void Compat::convert(ntsa::PortOptions*          result,
                     const ntca::GetPortOptions& options)
{
    // Load into the specified 'result' the conversion of the specified
    // 'options'.

    if (!options.portFallback().isNull()) {
        result->setPortFallback(options.portFallback().value());
    }

    if (!options.portSelector().isNull()) {
        result->setPortSelector(options.portSelector().value());
    }

    if (!options.portFilter().isNull()) {
        result->setPortFilter(options.portFilter().value());
    }

    if (!options.transport().isNull()) {
        result->setTransport(options.transport().value());
    }
}

ntsa::Error Compat::convert(
    bdlb::NullableValue<ntsa::IpAddressType::Value>* result,
    const ntca::GetIpAddressOptions&                 options)
{
    result->reset();

    bdlb::NullableValue<ntsa::IpAddressType::Value> ipAddressType =
        options.ipAddressType();

    if (!ipAddressType.isNull()) {
        if (ipAddressType.value() == ntsa::IpAddressType::e_UNDEFINED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!options.transport().isNull()) {
            if (options.transport().value() ==
                    ntsa::Transport::e_TCP_IPV4_STREAM ||
                options.transport().value() ==
                    ntsa::Transport::e_UDP_IPV4_DATAGRAM)
            {
                if (ipAddressType.value() != ntsa::IpAddressType::e_V4) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
            else if (options.transport().value() ==
                         ntsa::Transport::e_TCP_IPV6_STREAM ||
                     options.transport().value() ==
                         ntsa::Transport::e_UDP_IPV6_DATAGRAM)
            {
                if (ipAddressType.value() != ntsa::IpAddressType::e_V6) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }
    else if (!options.transport().isNull()) {
        if (options.transport().value() ==
                ntsa::Transport::e_TCP_IPV4_STREAM ||
            options.transport().value() ==
                ntsa::Transport::e_UDP_IPV4_DATAGRAM)
        {
            ipAddressType = ntsa::IpAddressType::e_V4;
        }
        else if (options.transport().value() ==
                     ntsa::Transport::e_TCP_IPV6_STREAM ||
                 options.transport().value() ==
                     ntsa::Transport::e_UDP_IPV6_DATAGRAM)
        {
            ipAddressType = ntsa::IpAddressType::e_V6;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    *result = ipAddressType;
    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
