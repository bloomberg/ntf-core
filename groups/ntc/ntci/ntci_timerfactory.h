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

#ifndef INCLUDED_NTCI_TIMERFACTORY
#define INCLUDED_NTCI_TIMERFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_timeroptions.h>
#include <ntccfg_platform.h>
#include <ntci_authorization.h>
#include <ntci_callback.h>
#include <ntci_timer.h>
#include <ntci_timercallbackfactory.h>
#include <ntci_timersession.h>
#include <ntcscm_version.h>
#include <bsls_timeinterval.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to create timers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_timer
class TimerFactory : public TimerCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~TimerFactory();

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

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;

    /// Return the current elapsed time since the Unix epoch.
    virtual bsls::TimeInterval currentTime() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
