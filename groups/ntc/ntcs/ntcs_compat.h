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

#ifndef INCLUDED_NTCS_COMPAT
#define INCLUDED_NTCS_COMPAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_bindoptions.h>
#include <ntca_connectoptions.h>
#include <ntca_datagramsocketoptions.h>
#include <ntca_getendpointoptions.h>
#include <ntca_interfaceconfig.h>
#include <ntca_listenersocketoptions.h>
#include <ntca_proactorconfig.h>
#include <ntca_reactorconfig.h>
#include <ntca_streamsocketoptions.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <ntsi_datagramsocket.h>
#include <ntsi_descriptor.h>
#include <ntsi_listenersocket.h>
#include <ntsi_streamsocket.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide utilities for sanitizing, normalizing, converting, and applying
/// configuration.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct Compat {
    /// Adjust the value of the specified 'config' to be consistent and
    /// coherent.
    static void sanitize(ntca::InterfaceConfig* config);

    /// Load into the specified 'result' the effective 'options'.
    static void convert(ntca::StreamSocketOptions*         result,
                        const ntca::ListenerSocketOptions& options);

    /// Load into the specified 'result' the effective 'options'.
    static void convert(ntca::ListenerSocketOptions*     result,
                        const ntca::StreamSocketOptions& options);

    /// Load into the specified 'result' the specified sanitized 'options',
    /// inheriting defaults from the specified 'config' where appropriate.
    static void convert(ntca::DatagramSocketOptions*       result,
                        const ntca::DatagramSocketOptions& options,
                        const ntca::InterfaceConfig&       config);

    /// Load into the specified 'result' the specified sanitized 'options',
    /// inheriting defaults from the specified 'config' where appropriate.
    static void convert(ntca::ListenerSocketOptions*       result,
                        const ntca::ListenerSocketOptions& options,
                        const ntca::InterfaceConfig&       config);

    /// Load into the specified 'result' the specified sanitized 'options',
    /// inheriting defaults from the specified 'config' where appropriate.
    static void convert(ntca::StreamSocketOptions*       result,
                        const ntca::StreamSocketOptions& options,
                        const ntca::InterfaceConfig&     config);

    /// Load into the specified 'result' the conversion of the specified
    /// 'options'.
    static void convert(ntca::GetEndpointOptions* result,
                        const ntca::BindOptions&  options);

    /// Load into the specified 'result' the conversion of the specified
    /// 'options'.
    static void convert(ntca::GetEndpointOptions*   result,
                        const ntca::ConnectOptions& options);

    /// Configure the specified 'socket' to be non-blocking and have the
    /// specified 'options'. Return the error.
    static ntsa::Error configure(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket,
        const ntca::DatagramSocketOptions&           options);

    /// Configure the specified 'socket' to be non-blocking and have the
    /// specified 'options'. Return the error.
    static ntsa::Error configure(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket,
        const ntca::ListenerSocketOptions&           options);

    /// Configure the specified 'socket' to be non-blocking and have the
    /// specified 'options'. Return the error.
    static ntsa::Error configure(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket,
        const ntca::StreamSocketOptions&           options);
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
