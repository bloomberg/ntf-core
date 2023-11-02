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

#ifndef INCLUDED_NTSO_TEST
#define INCLUDED_NTSO_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsb_datagramsocket.h>
#include <ntsb_listenersocket.h>
#include <ntsb_streamsocket.h>
#include <ntscfg_platform.h>
#include <ntscfg_test.h>
#include <ntsi_reactor.h>
#include <ntsscm_version.h>
#include <bdlt_currenttime.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_timeinterval.h>
#include <bsls_log.h>
#include <bsl_sstream.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntso {

/// @internal @brief
/// Provide utilities for testing reactors.
///
/// @par Thread Safety
// This struct is thread safe.
///
/// @ingroup module_ntso
struct Test 
{
    /// Define a type alias for a vector of reactors.
    typedef bsl::vector< bsl::shared_ptr<ntsi::Reactor> > ReactorVector;

    /// Test the usage example.
    static void usage(
        const ReactorVector& reactors, bslma::Allocator* allocator);

    /// Test the behavior of the reactor when a socket is closed while it is
    /// being polled.
    static void pollingAfterFullShutdown(
        const ReactorVector& reactors, bslma::Allocator* allocator);

    /// Test the behavior of the reactor when a socket is closed while it is
    /// being polled.
    ///
    /// select  The polling function fails, but does not indicate which file
    ///         descriptor was invalid. The reactor implementation scans each
    ///         attached file descriptor and checks if the file descriptor is
    ///         invalid. For each invalid (i.e. file descriptor), it 
    ///         synthesizes an error event and immediately detaches that file
    ///         descriptor from the reactor on behalf of the user.
    ///
    /// poll    The polling function succeeds and indicates POLLINVAL for the 
    ///         offending file descriptor. The reactor implementation returns
    ///         an error event but immediately detaches the file descriptor 
    ///         from the reactor on behalf of the user.
    ///
    /// kqueue  When the file descriptor is closed, it is automatically removed
    ///         from the kqueue interest set maintained internally in the 
    ///         kernel. The polling function thus does not return, unless it
    ///         is given a timeout in which case it times out.
    static void pollingAfterClose(
        const ReactorVector& reactors, bslma::Allocator* allocator);

    /// Log the specified 'events'. 
    static void log(const bsl::vector<ntsa::Event>& events);

    /// Log the specified 'eventSet'. 
    static void log(const ntsa::EventSet& eventSet);
};

}  // close package namespace
}  // close enterprise namespace

#endif
