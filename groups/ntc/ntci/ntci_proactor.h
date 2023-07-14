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

#ifndef INCLUDED_NTCI_PROACTOR
#define INCLUDED_NTCI_PROACTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_loadbalancingoptions.h>
#include <ntca_waiteroptions.h>
#include <ntccfg_platform.h>
#include <ntci_datapool.h>
#include <ntci_driver.h>
#include <ntci_executor.h>
#include <ntci_proactorpool.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_timer.h>
#include <ntci_waiter.h>
#include <ntcscm_version.h>
#include <ntsa_data.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_receivecontext.h>
#include <ntsa_receiveoptions.h>
#include <ntsa_sendcontext.h>
#include <ntsa_sendoptions.h>
#include <ntsa_shutdowntype.h>
#include <ntsi_descriptor.h>
#include <bdlbb_blob.h>
#include <bslmt_threadutil.h>
#include <bsl_functional.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntci {
class ProactorSocket;
}
namespace ntci {

/// Provide an interface to the proactor asynchronous model.
///
/// @details
/// In the proactor pattern, an operation is "proactively" initiated, then
/// asynchronously performed by an underlying system mechanism.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_proactor
class Proactor : public ntci::Driver, public ntci::ProactorPool
{
  public:
    /// Destroy this object.
    virtual ~Proactor();

    /// Attach the specified 'socket' to the proactor. Return the
    /// error.
    virtual ntsa::Error attachSocket(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket) = 0;

    /// Accept the next connection made to the specified 'socket' bound to
    /// the specified 'endpoint'. Return the error.
    virtual ntsa::Error accept(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket) = 0;

    /// Connect the specified 'socket' to the specified 'endpoint'. Return
    /// the error.
    virtual ntsa::Error connect(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const ntsa::Endpoint&                        endpoint) = 0;

    /// Enqueue the specified 'data' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Return the error.
    /// Note that 'data' must not be modified or destroyed until the
    /// operation completes or fails.
    virtual ntsa::Error send(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const bdlbb::Blob&                           data,
        const ntsa::SendOptions&                     options) = 0;

    /// Enqueue the specified 'data' to the send buffer of the specified
    /// 'socket' according to the specified 'options'. Return the error.
    /// Note that 'data' must not be modified or destroyed until the
    /// operation completes or fails.
    virtual ntsa::Error send(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        const ntsa::Data&                            data,
        const ntsa::SendOptions&                     options) = 0;

    /// Dequeue from the receive buffer of the specified 'socket' into the
    /// specified 'data' according to the specified 'options'. Return the
    /// error. Note that 'data' must not be modified or destroyed until the
    /// operation completes or fails.
    virtual ntsa::Error receive(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        bdlbb::Blob*                                 data,
        const ntsa::ReceiveOptions&                  options) = 0;

    /// Shutdown the stream socket in the specified 'direction'. Return the
    /// error.
    virtual ntsa::Error shutdown(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::ShutdownType::Value                    direction) = 0;

    /// Cancel all outstanding operations initiated for the specified
    /// 'socket'. Return the error.
    virtual ntsa::Error cancel(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket) = 0;

    /// Detach the specified 'socket' from the proactor. Return the error.
    virtual ntsa::Error detachSocket(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket) = 0;

    /// Detach the specified 'socket' from the proactor. Return the error.
    virtual ntsa::Error detachSocketAsync(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket) = 0;

    /// Close all monitored sockets and timers.
    virtual ntsa::Error closeAll() = 0;

    /// Increment the estimation of the load on the proactor according to
    /// the specified load balancing 'options'.
    virtual void incrementLoad(const ntca::LoadBalancingOptions& options) = 0;

    /// Decrement the estimation of the load on the proactor according to
    /// the specified load balancing 'options'.
    virtual void decrementLoad(const ntca::LoadBalancingOptions& options) = 0;

    /// Execute all deferred functions managed by this object.
    virtual void drainFunctions() = 0;

    /// Clear all deferred functions managed by this object.
    virtual void clearFunctions() = 0;

    /// Clear all timers managed by this object.
    virtual void clearTimers() = 0;

    /// Clear all sockets managed by this object.
    virtual void clearSockets() = 0;

    /// Clear all resources managed by this object.
    virtual void clear() = 0;

    /// Return the number of sockets currently being monitored.
    virtual bsl::size_t numSockets() const = 0;

    /// Return the maximum number of sockets capable of being monitored
    /// at one time.
    virtual bsl::size_t maxSockets() const = 0;

    /// Return the number of timers currently being monitored.
    virtual bsl::size_t numTimers() const = 0;

    /// Return the maximum number of timers capable of being monitored
    /// at one time.
    virtual bsl::size_t maxTimers() const = 0;

    /// Return the estimation of the load on the proactor.
    virtual bsl::size_t load() const = 0;

    /// Return the handle of the thread that drives this proactor, or
    /// the default value if no such thread has been set.
    virtual bslmt::ThreadUtil::Handle threadHandle() const = 0;

    /// Return the index in the thread pool of the thread that drives this
    /// proactor, or 0 if no such thread has been set.
    virtual bsl::size_t threadIndex() const = 0;

    /// Return true if the reactor has no pending deferred functors no
    /// pending timers, and no registered sockets, otherwise return false.
    virtual bool empty() const = 0;

    /// Return the data pool.
    virtual const bsl::shared_ptr<ntci::DataPool>& dataPool() const = 0;
};

}  // close package namespace
}  // close enterprise namespace
#endif
