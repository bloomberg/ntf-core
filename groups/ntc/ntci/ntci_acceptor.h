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

#ifndef INCLUDED_NTCI_ACCEPTOR
#define INCLUDED_NTCI_ACCEPTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_acceptcontext.h>
#include <ntca_acceptoptions.h>
#include <ntccfg_platform.h>
#include <ntci_acceptcallbackfactory.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>

namespace BloombergLP {
namespace ntci {
class StreamSocket;
}
namespace ntci {

/// Provide an interface to accept a stream socket connected to a remote
/// endpoint.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_accept
class Acceptor : public ntci::AcceptCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~Acceptor();

    /// Dequeue a connection from the backlog according to the specified
    /// 'options'. If the accept queue is not empty, synchronously pop the
    /// front of the accept queue into the specified 'streamSocket'.
    /// Otherwise, asynchronously accept a connection from the backlog onto
    /// the accept queue as a connection in the backlog becomes accepted, at
    /// the configured accept rate limit, if any, up to the accept queue
    /// high watermark. After satisfying any queued accept operations, when
    /// the accept queue is asynchronously filled up to the accept queue low
    /// watermark, announce an accept queue low watermark event. When
    /// asynchronously enqueing connections onto the accept queue causes the
    /// accept queue high watermark to become breached, stop asynchronously
    /// accepting connections from the backlog onto the accept queue and
    /// announce an accept queue high watermark event. Return the error,
    /// notably 'ntsa::Error::e_WOULD_BLOCK' if neither the accept queue nor
    /// the backlog is non-empty. All other errors indicate no more
    /// connections have been accepted at this time or will become accepted
    /// in the future.
    virtual ntsa::Error accept(
        ntca::AcceptContext*                 context,
        bsl::shared_ptr<ntci::StreamSocket>* streamSocket,
        const ntca::AcceptOptions&           options) = 0;

    /// Dequeue a connection from the backlog according to the specified
    /// 'options'. If the accept queue is not empty, synchronously pop the
    /// front of the accept queue into an internally allocated stream socket
    /// and invoke the specified 'callback' on the this object's strand, if
    /// any, with that stream socket. Otherwise, queue the accept operation
    /// and asynchronously accept connections from the backlog onto the
    /// accept queue as connections in the backlog become accepted, at the
    /// configured accept rate limit, if any, up to the accept queue high
    /// watermark. When the accept queue becomes non-empty, synchronously
    /// pop the front of the accept queue into an internally allocated
    /// stream socket and invoke the 'callback' on the callbacks strand, if
    /// any, with that stream socket. After satisfying any queued accept
    /// operations, when the accept queue is asynchronously filled up to the
    /// accept queue low watermark, announce an accept queue low watermark
    /// event. When asynchronously enqueing connections onto the accept
    /// queue causes the accept queue high watermark to become breached,
    /// stop asynchronously accepting connections from the backlog onto the
    /// accept queue and announce an accept queue high watermark event.
    /// Return the error, notably 'ntsa::Error::e_WOULD_BLOCK' if neither
    /// the accept queue nor the backlog is non-empty. All other errors
    /// indicate no more connections have been accepted at this time or will
    /// become accepted in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error accept(const ntca::AcceptOptions&  options,
                               const ntci::AcceptFunction& callback) = 0;

    /// Dequeue a connection from the backlog according to the specified
    /// 'options'. If the accept queue is not empty, synchronously pop the
    /// front of the accept queue into an internally allocated stream socket
    /// and invoke the specified 'callback' on the callback's strand, if
    /// any, with that stream socket. Otherwise, queue the accept operation
    /// and asynchronously accept connections from the backlog onto the
    /// accept queue as connections in the backlog become accepted, at the
    /// configured accept rate limit, if any, up to the accept queue high
    /// watermark. When the accept queue becomes non-empty, synchronously
    /// pop the front of the accept queue into an internally allocated
    /// stream socket and invoke the 'callback' on the callbacks strand, if
    /// any, with that stream socket. After satisfying any queued accept
    /// operations, when the accept queue is asynchronously filled up to the
    /// accept queue low watermark, announce an accept queue low watermark
    /// event. When asynchronously enqueing connections onto the accept
    /// queue causes the accept queue high watermark to become breached,
    /// stop asynchronously accepting connections from the backlog onto the
    /// accept queue and announce an accept queue high watermark event.
    /// Return the error, notably 'ntsa::Error::e_WOULD_BLOCK' if neither
    /// the accept queue nor the backlog is non-empty. All other errors
    /// indicate no more connections have been accepted at this time or will
    /// become accepted in the future. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error accept(const ntca::AcceptOptions&  options,
                               const ntci::AcceptCallback& callback) = 0;

    /// Cancel the accept operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::AcceptToken& token) = 0;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
