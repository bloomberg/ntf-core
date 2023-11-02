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

#ifndef INCLUDED_NTSI_REACTOR
#define INCLUDED_NTSI_REACTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_event.h>
#include <ntsa_handle.h>
#include <ntsi_datagramsocket.h>
#include <ntsi_listenersocket.h>
#include <ntsi_streamsocket.h>
#include <ntsscm_version.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsi {

/// Provide a mechanism to poll sockets for events.
///
/// @details
/// Provide an abstract mechanism to poll sockets for events, potentially 
/// suspending a thread until one or more events occur.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsi
class Reactor
{
  public:
    /// Destroy this object.
    virtual ~Reactor();

    /// Add the specified 'socket' to the reactor. Return the error.
    virtual ntsa::Error add(ntsa::Handle socket);

    /// Add the specified 'socket' to the reactor. Return the error.
    virtual ntsa::Error add(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket);

    /// Add the specified 'socket' to the reactor. Return the error.
    virtual ntsa::Error add(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket);

    /// Add the specified 'socket' to the reactor. Return the error.
    virtual ntsa::Error add(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket);

    /// Remove the specified 'socket' from the reactor. Return the error.
    virtual ntsa::Error remove(ntsa::Handle socket);

    /// Remove the specified 'socket' from the reactor. Return the error.
    virtual ntsa::Error remove(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket);

    /// Remove the specified 'socket' from the reactor. Return the error.
    virtual ntsa::Error remove(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket);

    /// Remove the specified 'socket' from the reactor. Return the error.
    virtual ntsa::Error remove(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket);

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error showReadable(ntsa::Handle socket);

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error showReadable(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket);

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error showReadable(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket);

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error showReadable(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket);

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error showWritable(ntsa::Handle socket);

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error showWritable(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket);

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error showWritable(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket);

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error showWritable(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket);

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error hideReadable(ntsa::Handle socket);

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error hideReadable(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket);

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error hideReadable(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket);

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error hideReadable(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket);

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error hideWritable(ntsa::Handle socket);

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error hideWritable(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket);

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error hideWritable(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket);

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error hideWritable(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket);

    /// Block until at least one socket has a condition of interest or the
    /// specified absolute 'deadline', if any, elapses. Load into the specified
    /// 'result' the events that describe the sockets and their conditions.
    /// Return the error.
    virtual ntsa::Error wait(
        ntsa::EventSet*                                result,
        const bdlb::NullableValue<bsls::TimeInterval>& deadline);
};

}  // end namespace ntsi
}  // end namespace BloombergLP
#endif
