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

#ifndef INCLUDED_NTCR_THREAD
#define INCLUDED_NTCR_THREAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_resolverconfig.h>
#include <ntca_threadconfig.h>
#include <ntccfg_platform.h>
#include <ntci_log.h>
#include <ntci_reactor.h>
#include <ntci_reactorfactory.h>
#include <ntci_reactormetrics.h>
#include <ntci_resolver.h>
#include <ntci_thread.h>
#include <ntci_timer.h>
#include <ntcs_metrics.h>
#include <ntcs_user.h>
#include <ntcscm_version.h>
#include <ntsi_descriptor.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bsls_atomic.h>
#include <bsl_functional.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcr {

/// @internal @brief
/// Provide asynchronous, multiplexed sockets and timers using a reactor
/// driven by a single thread.
///
/// @details
/// Provide a mechanism that runs a thread to automatically block
/// on a reactor to implement the asynchronous behavior required by sockets
/// and timers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcr
class Thread : public ntci::Thread, public ntccfg::Shared<Thread>
{
    ntccfg::Object                 d_object;
    bsl::shared_ptr<ntci::Reactor> d_reactor_sp;
    bslmt::ThreadUtil::Handle      d_threadHandle;
    bslmt::ThreadAttributes        d_threadAttributes;
    ntccfg::ConditionMutex         d_runMutex;
    ntccfg::Condition              d_runCondition;
    bsls::AtomicInt                d_runState;
    ntca::ThreadConfig             d_config;
    bslma::Allocator*              d_allocator_p;

  private:
    Thread(const Thread&) BSLS_KEYWORD_DELETED;
    Thread& operator=(const Thread&) BSLS_KEYWORD_DELETED;

  private:
    /// Wait for I/O events.
    static void* run(ntcr::Thread* thread);

    /// Initialize this object.
    void initialize();

  public:
    /// Create a new thread having the specified 'configuration' using
    /// a reactor produced by the specified 'reactorFactory'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    Thread(const ntca::ThreadConfig&                    configuration,
           const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory,
           bslma::Allocator*                            basicAllocator = 0);

    /// Create a new thread having the specified 'configuration' using
    /// a reactor produced by the specified 'reactorFactory'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    Thread(const ntca::ThreadConfig&             configuration,
           const bsl::shared_ptr<ntci::Reactor>& reactor,
           bslma::Allocator*                     basicAllocator = 0);

    /// Destroy this object.
    ~Thread() BSLS_KEYWORD_OVERRIDE;

    /// Start the thread. Return the error.
    ntsa::Error start() BSLS_KEYWORD_OVERRIDE;

    /// Start the thread with the speicfied 'threadAttributes'.
    ntsa::Error start(const bslmt::ThreadAttributes& threadAttributes)
        BSLS_KEYWORD_OVERRIDE;

    /// Begin stopping the thread.
    void shutdown() BSLS_KEYWORD_OVERRIDE;

    /// Wait for the thread to stop.
    void linger() BSLS_KEYWORD_OVERRIDE;

    /// Close all sockets and timers.
    ntsa::Error closeAll() BSLS_KEYWORD_OVERRIDE;

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new strand to serialize execution of functors by the
    /// reactor driven by this thread. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new datagram socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::DatagramSocket> createDatagramSocket(
        const ntca::DatagramSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new listener socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new stream socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Return the handle to the thread.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the thread index.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
