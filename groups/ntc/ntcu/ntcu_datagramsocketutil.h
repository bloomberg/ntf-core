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

#ifndef INCLUDED_NTCU_DATAGRAMSOCKETUTIL
#define INCLUDED_NTCU_DATAGRAMSOCKETUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntsa_transport.h>
#include <bdlb_nullablevalue.h>

namespace BloombergLP {
namespace ntcu {

/// @internal @brief
/// Provide utilities for implementing datagram sockets.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcu
struct DatagramSocketUtil {
    /// Load into the specified 'result' the effective transport for a
    /// datagram socket using the specified source or remote 'endpoint'.
    /// Return the error.
    static ntsa::Error getTransport(ntsa::Transport::Value* result,
                                    const ntsa::Endpoint&   endpoint);

    /// Validate that the datagram transport used by the specified
    /// 'sourceEndpoint' is the same as the transport used by the specified
    /// 'remoteEndpoint'. Return the error.
    static ntsa::Error validateTransport(
        const bdlb::NullableValue<ntsa::Endpoint>& sourceEndpoint,
        const bdlb::NullableValue<ntsa::Endpoint>& remoteEndpoint);

    /// Validate that the specified 'transport' is a valid datagram
    /// transport. Return the error.
    static ntsa::Error validateTransport(ntsa::Transport::Value transport);

    /// Validate that the specified 'transport' is a valid datagram
    /// transport and the datagram transport used by the specified
    /// 'endpoint' matches the specified 'transport'. Return the error.
    static ntsa::Error validateTransport(
        ntsa::Transport::Value                     transport,
        const bdlb::NullableValue<ntsa::Endpoint>& endpoint);

    /// Validate that the specified 'transport' is a valid datagram
    /// transport and the datagram transport used by the specified
    /// 'sourceEndpoint' is the same as the transport used by the specified
    /// 'remoteEndpoint' and both match the specified 'transport'. Return
    /// the error.
    static ntsa::Error validateTransport(
        ntsa::Transport::Value                     transport,
        const bdlb::NullableValue<ntsa::Endpoint>& sourceEndpoint,
        const bdlb::NullableValue<ntsa::Endpoint>& remoteEndpoint);
};

}  // end namespace ntcu
}  // end namespace BloombergLP
#endif
