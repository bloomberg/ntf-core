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

#ifndef INCLUDED_NTCI_SENDRESULT
#define INCLUDED_NTCI_SENDRESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_sendevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Sender;
}
namespace ntci {

/// Describe the result of a send operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_send
class SendResult
{
    bsl::shared_ptr<ntci::Sender> d_sender_sp;
    ntca::SendEvent               d_event;

  public:
    /// Create a new send result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit SendResult(bslma::Allocator* basicAllocator = 0);

    /// Create a a new send result having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    SendResult(const SendResult& original,
               bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~SendResult();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    SendResult& operator=(const SendResult& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the sender to the specified 'value'.
    void setSender(const bsl::shared_ptr<ntci::Sender>& sender);

    /// Set the event to the specified 'value'.
    void setEvent(const ntca::SendEvent& value);

    /// Return the sender.
    const bsl::shared_ptr<ntci::Sender>& sender() const;

    /// Return the event.
    const ntca::SendEvent& event() const;
};

NTCCFG_INLINE
SendResult::SendResult(bslma::Allocator* basicAllocator)
: d_sender_sp()
, d_event()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
SendResult::SendResult(const SendResult& original,
                       bslma::Allocator* basicAllocator)
: d_sender_sp(original.d_sender_sp)
, d_event(original.d_event)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
SendResult::~SendResult()
{
}

NTCCFG_INLINE
SendResult& SendResult::operator=(const SendResult& other)
{
    if (this != &other) {
        d_sender_sp = other.d_sender_sp;
        d_event     = other.d_event;
    }
    return *this;
}

NTCCFG_INLINE
void SendResult::reset()
{
    d_sender_sp.reset();
    d_event.reset();
}

NTCCFG_INLINE
void SendResult::setSender(const bsl::shared_ptr<ntci::Sender>& value)
{
    d_sender_sp = value;
}

NTCCFG_INLINE
void SendResult::setEvent(const ntca::SendEvent& value)
{
    d_event = value;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::Sender>& SendResult::sender() const
{
    return d_sender_sp;
}

NTCCFG_INLINE
const ntca::SendEvent& SendResult::event() const
{
    return d_event;
}

}  // close package namespace
}  // close enterprise namespace
#endif
