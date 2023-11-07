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
    virtual ntsa::Error add(ntsa::Handle socket) = 0;

    /// Add the specified 'socket' to the reactor. Return the error.
    virtual ntsa::Error add(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket) = 0;

    /// Add the specified 'socket' to the reactor. Return the error.
    virtual ntsa::Error add(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket) = 0;

    /// Add the specified 'socket' to the reactor. Return the error.
    virtual ntsa::Error add(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket) = 0;

    /// Remove the specified 'socket' from the reactor. Return the error.
    virtual ntsa::Error remove(ntsa::Handle socket) = 0;

    /// Remove the specified 'socket' from the reactor. Return the error.
    virtual ntsa::Error remove(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket) = 0;

    /// Remove the specified 'socket' from the reactor. Return the error.
    virtual ntsa::Error remove(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket) = 0;

    /// Remove the specified 'socket' from the reactor. Return the error.
    virtual ntsa::Error remove(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket) = 0;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error showReadable(ntsa::Handle socket) = 0;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error showReadable(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket) = 0;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error showReadable(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket) = 0;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error showReadable(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket) = 0;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error showWritable(ntsa::Handle socket) = 0;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error showWritable(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket) = 0;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error showWritable(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket) = 0;

    /// Unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error showWritable(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket) = 0;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error hideReadable(ntsa::Handle socket) = 0;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error hideReadable(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket) = 0;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error hideReadable(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket) = 0;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is readable. Return the error.
    virtual ntsa::Error hideReadable(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket) = 0;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error hideWritable(ntsa::Handle socket) = 0;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error hideWritable(
        const bsl::shared_ptr<ntsi::DatagramSocket>& socket) = 0;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error hideWritable(
        const bsl::shared_ptr<ntsi::ListenerSocket>& socket) = 0;

    /// Do not unblock any thread waiting on the reactor when the specified
    /// 'socket' is writable. Return the error.
    virtual ntsa::Error hideWritable(
        const bsl::shared_ptr<ntsi::StreamSocket>& socket) = 0;

    /// Block until at least one socket has a condition of interest or the
    /// specified absolute 'deadline', if any, elapses. Load into the specified
    /// 'result' the events that describe the sockets and their conditions.
    /// Return the error.
    virtual ntsa::Error wait(
        bsl::vector<ntsa::Event>*                      result,
        const bdlb::NullableValue<bsls::TimeInterval>& deadline) = 0;

    // MRM
    #if 0
    /// Block until at least one socket in the specified 'readable',
    /// 'writable', or 'exceptional' sets matches any of those respective
    /// conditions, or the specified 'timeout' elapses, if any. Load into the
    /// specified 'result' the events that describe the sockets and their
    /// conditions. Return the error. 
    virtual ntsa::Error wait(
        bsl::vector<ntsa::Event>                       *result, 
        const bsl::vector<ntsa::Handle>&                readable, 
        const bsl::vector<ntsa::Handle>&                writable,
        const bsl::vector<ntsa::Handle>&                exceptional,
        const bdlb::NullableValue<bsls::TimeInterval>&  timeout) = 0;
    #endif
};

}  // end namespace ntsi
}  // end namespace BloombergLP
#endif
