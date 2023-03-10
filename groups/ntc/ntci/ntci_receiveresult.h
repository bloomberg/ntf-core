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

#ifndef INCLUDED_NTCI_RECEIVERESULT
#define INCLUDED_NTCI_RECEIVERESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_receiveevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Receiver;
}
namespace ntci {

/// Describe the result of a receive operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_receive
class ReceiveResult
{
    bsl::shared_ptr<ntci::Receiver> d_receiver_sp;
    bsl::shared_ptr<bdlbb::Blob>    d_data_sp;
    ntca::ReceiveEvent              d_event;

  public:
    /// Create a new receive result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit ReceiveResult(bslma::Allocator* basicAllocator = 0);

    /// Create a a new receive result having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    ReceiveResult(const ReceiveResult& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~ReceiveResult();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReceiveResult& operator=(const ReceiveResult& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the receiver to the specified 'value'.
    void setReceiver(const bsl::shared_ptr<ntci::Receiver>& value);

    /// Set the received data to the specified 'value'.
    void setData(const bsl::shared_ptr<bdlbb::Blob>& value);

    /// Set the event to the specified 'value'.
    void setEvent(const ntca::ReceiveEvent& value);

    /// Return the receiver.
    const bsl::shared_ptr<ntci::Receiver>& receiver() const;

    /// Return the received data.
    const bsl::shared_ptr<bdlbb::Blob>& data() const;

    /// Return the event.
    const ntca::ReceiveEvent& event() const;
};

NTCCFG_INLINE
ReceiveResult::ReceiveResult(bslma::Allocator* basicAllocator)
: d_receiver_sp()
, d_data_sp()
, d_event()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
ReceiveResult::ReceiveResult(const ReceiveResult& original,
                             bslma::Allocator*    basicAllocator)
: d_receiver_sp(original.d_receiver_sp)
, d_data_sp(original.d_data_sp)
, d_event(original.d_event)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
ReceiveResult::~ReceiveResult()
{
}

NTCCFG_INLINE
ReceiveResult& ReceiveResult::operator=(const ReceiveResult& other)
{
    if (this != &other) {
        d_receiver_sp = other.d_receiver_sp;
        d_data_sp     = other.d_data_sp;
        d_event       = other.d_event;
    }
    return *this;
}

NTCCFG_INLINE
void ReceiveResult::reset()
{
    d_receiver_sp.reset();
    d_data_sp.reset();
    d_event.reset();
}

NTCCFG_INLINE
void ReceiveResult::setReceiver(const bsl::shared_ptr<ntci::Receiver>& value)
{
    d_receiver_sp = value;
}

NTCCFG_INLINE
void ReceiveResult::setData(const bsl::shared_ptr<bdlbb::Blob>& value)
{
    d_data_sp = value;
}

NTCCFG_INLINE
void ReceiveResult::setEvent(const ntca::ReceiveEvent& value)
{
    d_event = value;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::Receiver>& ReceiveResult::receiver() const
{
    return d_receiver_sp;
}

NTCCFG_INLINE
const bsl::shared_ptr<bdlbb::Blob>& ReceiveResult::data() const
{
    return d_data_sp;
}

NTCCFG_INLINE
const ntca::ReceiveEvent& ReceiveResult::event() const
{
    return d_event;
}

}  // close package namespace
}  // close enterprise namespace
#endif
