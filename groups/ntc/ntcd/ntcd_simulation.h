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

#ifndef INCLUDED_NTCD_SIMULATION
#define INCLUDED_NTCD_SIMULATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcd_datagramsocket.h>
#include <ntcd_listenersocket.h>
#include <ntcd_machine.h>
#include <ntcd_proactor.h>
#include <ntcd_reactor.h>
#include <ntcd_streamsocket.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcd {

/// @internal @brief
/// Provide a simulation of networked machines and communication sessions.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcd
class Simulation
{
    bsl::shared_ptr<ntcd::Machine> d_machine_sp;
    bslma::Allocator*              d_allocator_p;

  private:
    Simulation(const Simulation&) BSLS_KEYWORD_DELETED;
    Simulation& operator=(const Simulation&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Simulation(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Simulation();

    /// Create a new session. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    bsl::shared_ptr<ntcd::Session> createSession(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new datagram socket on the default machine. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntcd::DatagramSocket> createDatagramSocket(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new listener socket on the default machine. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntcd::ListenerSocket> createListenerSocket(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new stream socket on the default machine. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntcd::StreamSocket> createStreamSocket(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new reactor having the specified 'configuration' operating
    /// in the environment of the specified 'user' on the default machine.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntcd::Reactor> createReactor(
        const ntca::ReactorConfig&         configuration,
        const bsl::shared_ptr<ntci::User>& user,
        bslma::Allocator*                  basicAllocator = 0);

    /// Create a new proactor having the specified 'configuration' operating
    /// in the environment of the specified 'user' on the default machine.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntcd::Proactor> createProactor(
        const ntca::ProactorConfig&        configuration,
        const bsl::shared_ptr<ntci::User>& user,
        bslma::Allocator*                  basicAllocator = 0);

    /// Start a background thread for each machine and continuously step the
    /// simulation of each session on each machine, as necessary, until
    /// each machine is stopped.
    ntsa::Error run();

    /// Step the simulation of each session on each machine, as necessary.
    /// If the specified 'block' flag is true, block until each packet queue
    /// is available to dequeue and enqueue. Return the error.
    ntsa::Error step(bool block);

    /// Stop stepping the simulation and join the background thread for each
    /// machine.
    void stop();

    /// Load into the specified 'result' the session associated with the
    /// specified 'handle', if any. Return the error.
    ntsa::Error lookupSession(bsl::weak_ptr<ntcd::Session>* result,
                              ntsa::Handle                  handle) const;

    /// Load into the specified 'client' and 'server' a connected pair of
    /// stream sockets of the specified 'type'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    static ntsa::Error createStreamSocketPair(
        bsl::shared_ptr<ntcd::StreamSocket>* client,
        bsl::shared_ptr<ntcd::StreamSocket>* server,
        ntsa::Transport::Value               type,
        bslma::Allocator*                    basicAllocator = 0);
};

}  // close package namespace
}  // close enterprise namespace
#endif
