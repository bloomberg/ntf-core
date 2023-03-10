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

#ifndef INCLUDED_NTCF_TEST
#define INCLUDED_NTCF_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcf_system.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcf {

/// @internal @brief
/// Provide utilities for testing.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntci_runtime
struct Test {
    /// Create a new interface and pair of connected stream sockets. Load
    /// into the specified 'interface' the new, started interface configured
    /// with a default configuration. Load into the specified
    /// 'clientStreamSocket' the new, connected stream socket using the
    /// specified 'transport' configured with a default configuration. Load
    /// into the specified 'serverStreamSocket' the new, connected stream
    /// socket using the specified 'transport' configured with a default
    /// configuration. Return the error. Note that, on success, the
    /// 'clientStreamSocket' is connected to the specified
    /// 'serverStreamSocket'. Also note that, on success, the caller is
    /// responsible for closing both the 'clientStreamSocket' and the
    /// 'serverStreamSocket' and stopping the 'interface'.
    static ntsa::Error createStreamSocketPair(
        bsl::shared_ptr<ntci::Interface>*    interface,
        bsl::shared_ptr<ntci::StreamSocket>* clientStreamSocket,
        bsl::shared_ptr<ntci::StreamSocket>* serverStreamSocket,
        ntsa::Transport::Value               transport,
        bslma::Allocator*                    basicAllocator = 0);

    /// Create a new interface and pair of connected stream sockets. Load
    /// into the specified 'interface' the new, started interface configured
    /// with the specified 'interfaceConfig'. Load into the specified
    /// 'clientStreamSocket' the new, connected stream socket using the
    /// specified 'transport' configured with the specified
    /// 'clientStreamSocketOptions'. Load into the specified
    /// 'serverStreamSocket' the new, connected stream socket using the
    /// specified 'transport' configured with the specified
    /// 'serverStreamSocketOptions'. Return the error. Note that, on
    /// success, the 'clientStreamSocket' is connected to the specified
    /// 'serverStreamSocket'. Also note that, on success, the caller is
    /// responsible for closing both the 'clientStreamSocket' and the
    /// 'serverStreamSocket' and stopping the 'interface'.
    static ntsa::Error createStreamSocketPair(
        bsl::shared_ptr<ntci::Interface>*    interface,
        bsl::shared_ptr<ntci::StreamSocket>* clientStreamSocket,
        bsl::shared_ptr<ntci::StreamSocket>* serverStreamSocket,
        ntsa::Transport::Value               transport,
        const ntca::InterfaceConfig&         interfaceConfig,
        const ntca::StreamSocketOptions&     clientStreamSocketOptions,
        const ntca::StreamSocketOptions&     serverStreamSocketOptions,
        bslma::Allocator*                    basicAllocator = 0);

    /// Close both the specified 'clientStreamSocket' and
    /// 'serverStreamSocket' then stop the specified 'interface'. Block
    /// until both the 'clientStreamSocket' and 'serverStreamSocket' are
    /// closed, then block until the 'interface' is stopped.
    static void closeAll(
        const bsl::shared_ptr<ntci::Interface>&    interface,
        const bsl::shared_ptr<ntci::StreamSocket>& clientStreamSocket,
        const bsl::shared_ptr<ntci::StreamSocket>& serverStreamSocket);
};

/// @internal @brief
/// Provide a guard to automatically close a pair of stream sockets and stop an
/// interface and block until the interface is asynchronously stopped.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class TestGuard
{
    bsl::shared_ptr<ntci::Interface>    d_interface_sp;
    bsl::shared_ptr<ntci::StreamSocket> d_clientStreamSocket_sp;
    bsl::shared_ptr<ntci::StreamSocket> d_serverStreamSocket_sp;

  private:
    TestGuard(const TestGuard&) BSLS_KEYWORD_DELETED;
    TestGuard& operator=(const TestGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new test guard for the specified 'interface' and the
    /// specified 'clientStreamSocket' and 'serverStreamSocket' pair.
    explicit TestGuard(
        const bsl::shared_ptr<ntci::Interface>&    interface,
        const bsl::shared_ptr<ntci::StreamSocket>& clientStreamSocket,
        const bsl::shared_ptr<ntci::StreamSocket>& serverStreamSocket);

    /// Destroy this object: close the managed stream socket pair, block
    /// until both stream sockets are closed, then stop the managed
    /// interface, and block until the interface is asynchronously stopped.
    ~TestGuard();
};

}  // end namespace ntcf
}  // end namespace BloombergLP
#endif
