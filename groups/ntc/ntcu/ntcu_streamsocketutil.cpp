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

#include <ntcu_streamsocketutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcu_streamsocketutil_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcu {

ntsa::Error StreamSocketUtil::getTransport(ntsa::Transport::Value* result,
                                           const ntsa::Endpoint&   endpoint)
{
    *result = endpoint.transport(ntsa::TransportMode::e_STREAM);

    if (*result == ntsa::Transport::e_UNDEFINED) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error StreamSocketUtil::validateTransport(
    const bdlb::NullableValue<ntsa::Endpoint>& sourceEndpoint,
    const bdlb::NullableValue<ntsa::Endpoint>& remoteEndpoint)
{
    if (!sourceEndpoint.isNull() && !remoteEndpoint.isNull()) {
        ntsa::Transport::Value sourceEndpointTransport =
            sourceEndpoint.value().transport(ntsa::TransportMode::e_STREAM);

        ntsa::Transport::Value remoteEndpointTransport =
            remoteEndpoint.value().transport(ntsa::TransportMode::e_STREAM);

        if (sourceEndpointTransport != remoteEndpointTransport) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error StreamSocketUtil::validateTransport(
    ntsa::Transport::Value transport)
{
    ntsa::TransportMode::Value transportMode =
        ntsa::Transport::getMode(transport);

    if (transportMode != ntsa::TransportMode::e_STREAM) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error StreamSocketUtil::validateTransport(
    ntsa::Transport::Value                     transport,
    const bdlb::NullableValue<ntsa::Endpoint>& endpoint)
{
    ntsa::Error error;

    error = ntcu::StreamSocketUtil::validateTransport(transport);
    if (error) {
        return error;
    }

    if (!endpoint.isNull()) {
        ntsa::Transport::Value endpointTransport =
            endpoint.value().transport(ntsa::TransportMode::e_STREAM);

        if (endpointTransport != transport) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error StreamSocketUtil::validateTransport(
    ntsa::Transport::Value                     transport,
    const bdlb::NullableValue<ntsa::Endpoint>& sourceEndpoint,
    const bdlb::NullableValue<ntsa::Endpoint>& remoteEndpoint)
{
    ntsa::Error error;

    error = ntcu::StreamSocketUtil::validateTransport(transport);
    if (error) {
        return error;
    }

    if (!sourceEndpoint.isNull() && !remoteEndpoint.isNull()) {
        ntsa::Transport::Value sourceEndpointTransport =
            sourceEndpoint.value().transport(ntsa::TransportMode::e_STREAM);

        ntsa::Transport::Value remoteEndpointTransport =
            remoteEndpoint.value().transport(ntsa::TransportMode::e_STREAM);

        if (sourceEndpointTransport != remoteEndpointTransport) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (sourceEndpointTransport != transport) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (!sourceEndpoint.isNull()) {
        ntsa::Transport::Value sourceEndpointTransport =
            sourceEndpoint.value().transport(ntsa::TransportMode::e_STREAM);

        if (sourceEndpointTransport != transport) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (!remoteEndpoint.isNull()) {
        ntsa::Transport::Value remoteEndpointTransport =
            remoteEndpoint.value().transport(ntsa::TransportMode::e_STREAM);

        if (remoteEndpointTransport != transport) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
