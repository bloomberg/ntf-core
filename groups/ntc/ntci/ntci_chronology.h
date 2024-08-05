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

#ifndef INCLUDED_NTCI_CHRONOLOGY
#define INCLUDED_NTCI_CHRONOLOGY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntca_timeroptions.h>
#include <ntci_executor.h>
#include <ntci_timer.h>
#include <ntci_timercallback.h>
#include <ntci_timerfactory.h>
#include <ntci_timersession.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bslmt_threadutil.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide a priority queue of functions and timers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_timer
class Chronology : public ntci::Executor, ntci::TimerFactory
{
  public:
    /// Defines a type alias for a vector of shared pointers to timers.
    typedef bsl::vector<bsl::shared_ptr<ntci::Timer> > TimerVector;

    /// Destroy this object.
    virtual ~Chronology();

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator*                          basicAllocator = 0) = 0;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) = 0;

    /// Push the specified 'functor' on the queue.
    virtual void execute(const ntci::Executor::Functor& functor) = 0;

    /// Atomically push the specified 'functorSequence' immediately followed
    /// by the specified 'functor', then clear the 'functorSequence'.
    virtual void moveAndExecute(
        ntci::Executor::FunctorSequence* functorSequence,
        const ntci::Executor::Functor&   functor) = 0;

    /// Invoke all deferred functions and announce the deadline event of any
    /// timer whose deadline is earlier than or equal to the current time.
    virtual void announce(bool single = false) = 0;

    /// Invoke all deferred functions.
    virtual void drain() = 0;

    /// Remove all functions and timers from the chronology.
    virtual void clear() = 0;

    /// Remove all functions from the chronology.
    virtual void clearFunctions() = 0;

    /// Remove all timers from the chronology.
    virtual void clearTimers() = 0;

    /// Close all timers.
    virtual void closeAll() = 0;

    /// Load into the specified 'result' all the scheduled timers in the
    /// chronology.
    virtual void load(TimerVector* result) const = 0;

    /// Return the absolute time the earliest scheduled timer is due, if any.
    virtual bdlb::NullableValue<bsls::TimeInterval> earliest() const = 0;

    /// Return the relative time interval to wait until the earliest timer
    /// is due, if any, from the current time, or null if no timer is
    /// scheduled.
    virtual bdlb::NullableValue<bsls::TimeInterval> 
      timeoutInterval() const = 0;

    /// Return the number of milliseconds to wait until the earliest timer
    /// is due, if any, from the current time, or -1 if no timer is
    /// scheduled.
    virtual int timeoutInMilliseconds() const = 0;

    /// Return the number of registered, but not necessarily scheduled
    /// timers in the chronology.
    virtual bsl::size_t numRegistered() const = 0;

    /// Return true if there are any registered, but not necessarily
    /// scheduled timers in the chronology, otherwise return false.
    virtual bool hasAnyRegistered() const = 0;

    /// Return the number of scheduled timers in the chronology.
    virtual bsl::size_t numScheduled() const = 0;

    /// Return true if there are any scheduled timers in the chronology,
    /// otherwise return false.
    virtual bool hasAnyScheduled() const = 0;

    /// Return the number of deferred functions in the chronology.
    virtual bsl::size_t numDeferred() const = 0;

    /// Return true if there are any deferred functions in the chronology,
    /// otherwise return false.
    virtual bool hasAnyDeferred() const = 0;

    /// Return true if there are any scheduled timers or deferred functions
    /// in the chronology, otherwise return false.
    virtual bool hasAnyScheduledOrDeferred() const = 0;

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;

    /// Return the current elapsed time since the Unix epoch.
    virtual bsls::TimeInterval currentTime() const = 0;
};

}  // close package namespace
}  // close enterprise namespace
#endif
