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

#ifndef INCLUDED_NTCI_TIMERRESULT
#define INCLUDED_NTCI_TIMERRESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_timerevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Timer;
}
namespace ntci {

/// Describe the result of a timer operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_timer
class TimerResult
{
    bsl::shared_ptr<ntci::Timer> d_timer_sp;
    ntca::TimerEvent             d_event;

  public:
    /// Create a new timer result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit TimerResult(bslma::Allocator* basicAllocator = 0);

    /// Create a a new timer result having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    TimerResult(const TimerResult& original,
                bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~TimerResult();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TimerResult& operator=(const TimerResult& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the timer to the specified 'value'.
    void setTimer(const bsl::shared_ptr<ntci::Timer>& value);

    /// Set the event to the specified 'value'.
    void setEvent(const ntca::TimerEvent& value);

    /// Return the timer.
    const bsl::shared_ptr<ntci::Timer>& timer() const;

    /// Return the event.
    const ntca::TimerEvent& event() const;
};

NTCCFG_INLINE
TimerResult::TimerResult(bslma::Allocator* basicAllocator)
: d_timer_sp()
, d_event()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
TimerResult::TimerResult(const TimerResult& original,
                         bslma::Allocator*  basicAllocator)
: d_timer_sp(original.d_timer_sp)
, d_event(original.d_event)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
TimerResult::~TimerResult()
{
}

NTCCFG_INLINE
TimerResult& TimerResult::operator=(const TimerResult& other)
{
    if (this != &other) {
        d_timer_sp = other.d_timer_sp;
        d_event    = other.d_event;
    }
    return *this;
}

NTCCFG_INLINE
void TimerResult::reset()
{
    d_timer_sp.reset();
    d_event.reset();
}

NTCCFG_INLINE
void TimerResult::setTimer(const bsl::shared_ptr<ntci::Timer>& value)
{
    d_timer_sp = value;
}

NTCCFG_INLINE
void TimerResult::setEvent(const ntca::TimerEvent& value)
{
    d_event = value;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::Timer>& TimerResult::timer() const
{
    return d_timer_sp;
}

NTCCFG_INLINE
const ntca::TimerEvent& TimerResult::event() const
{
    return d_event;
}

}  // close package namespace
}  // close enterprise namespace
#endif
