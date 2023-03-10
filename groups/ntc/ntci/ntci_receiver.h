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

#ifndef INCLUDED_NTCI_RECEIVER
#define INCLUDED_NTCI_RECEIVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_receivecontext.h>
#include <ntca_receiveoptions.h>
#include <ntccfg_platform.h>
#include <ntci_receivecallbackfactory.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_transport.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to receive data from a remote endpoint.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_receive
class Receiver : public ntci::ReceiveCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~Receiver();

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to fill the 'data', synchronously
    /// copy the read queue into the specified 'data'. Otherwise,
    /// asynchronously copy the socket receive buffer onto the read queue
    /// as data in the socket receive buffer becomes available, at the
    /// configured read rate limit, if any, up to the read queue high
    /// watermark. After satisfying any queued read operations, when the
    /// read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to fill the 'data', or
    /// 'ntsa::Error::e_EOF' if the read queue is empty and the socket
    /// receive buffer has been shut down. All other errors indicate no more
    /// received data is available at this time or will become available in
    /// the future.
    virtual ntsa::Error receive(ntca::ReceiveContext*       context,
                                bdlbb::Blob*                data,
                                const ntca::ReceiveOptions& options) = 0;

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to satisfy the read operation,
    /// synchronously copy the read queue into an internally allocated data
    /// structure and invoke the specified 'callback' on this object's
    /// strand, if any, with that data structure. Otherwise, queue the read
    /// operation and asynchronously copy the socket receive buffer onto the
    /// read queue as data in the socket receive buffer becomes available,
    /// at the configured read rate limit, if any, up to the read queue high
    /// watermark. When the read queue is asynchronously filled to a
    /// sufficient size to satisfy the read operation, synchronously copy
    /// the read queue into an internally allocated data structure and
    /// invoke the 'callback' on the callback's strand, if any, with that
    /// data structure. After satisfying any queued read operations, when
    /// the read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to synchronously satisfy
    /// the read operation, or 'ntsa::Error::e_EOF' if the read queue is
    /// empty and the socket receive buffer has been shut down. All other
    /// errors indicate no more received data is available at this time or
    /// will become available in the future. Note that callbacks created by
    /// this object will automatically be invoked on this object's strand
    /// unless an explicit strand is specified at the time the callback is
    /// created.
    virtual ntsa::Error receive(const ntca::ReceiveOptions&  options,
                                const ntci::ReceiveFunction& callback) = 0;

    /// Dequeue received data according to the specified 'options'. If the
    /// read queue has sufficient size to satisfy the read operation,
    /// synchronously copy the read queue into an internally allocated data
    /// structure and invoke the specified 'callback' on the callback's
    /// strand, if any, with that data structure. Otherwise, queue the read
    /// operation and asynchronously copy the socket receive buffer onto the
    /// read queue as data in the socket receive buffer becomes available,
    /// at the configured read rate limit, if any, up to the read queue high
    /// watermark. When the read queue is asynchronously filled to a
    /// sufficient size to satisfy the read operation, synchronously copy
    /// the read queue into an internally allocated data structure and
    /// invoke the 'callback' on the callback's strand, if any, with that
    /// data structure. After satisfying any queued read operations, when
    /// the read queue is asynchronously filled up to the read queue low
    /// watermark, announce a read queue low watermark event. When
    /// asynchronously enqueing data onto the read queue causes the read
    /// queue high watermark to become breached, stop asynchronously copying
    /// the socket receive buffer to the read queue and announce a read
    /// queue high watermark event. Return the error, notably
    /// 'ntsa::Error::e_WOULD_BLOCK' if neither the read queue nor the
    /// socket receive buffer has sufficient size to synchronously satisfy
    /// the read operation, or 'ntsa::Error::e_EOF' if the read queue is
    /// empty and the socket receive buffer has been shut down. All other
    /// errors indicate no more received data is available at this time or
    /// will become available in the future. Note that callbacks created by
    /// this object will automatically be invoked on this object's strand
    /// unless an explicit strand is specified at the time the callback is
    /// created.
    virtual ntsa::Error receive(const ntca::ReceiveOptions&  options,
                                const ntci::ReceiveCallback& callback) = 0;

    /// Cancel the receive operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::ReceiveToken& token) = 0;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
