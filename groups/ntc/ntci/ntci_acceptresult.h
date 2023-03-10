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

#ifndef INCLUDED_NTCI_ACCEPTRESULT
#define INCLUDED_NTCI_ACCEPTRESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_acceptevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Acceptor;
}
namespace ntci {
class StreamSocket;
}
namespace ntci {

/// Describe the result of an accept operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_accept
class AcceptResult
{
    bsl::shared_ptr<ntci::Acceptor>     d_acceptor_sp;
    bsl::shared_ptr<ntci::StreamSocket> d_streamSocket_sp;
    ntca::AcceptEvent                   d_event;

  public:
    /// Create a new accept result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit AcceptResult(bslma::Allocator* basicAllocator = 0);

    /// Create a a new accept result having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    AcceptResult(const AcceptResult& original,
                 bslma::Allocator*   basicAllocator = 0);

    /// Destroy this object.
    ~AcceptResult();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AcceptResult& operator=(const AcceptResult& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the acceptor to the specified 'value'.
    void setAcceptor(const bsl::shared_ptr<ntci::Acceptor>& value);

    /// Set the accepted stream socket to the specified 'value'.
    void setStreamSocket(const bsl::shared_ptr<ntci::StreamSocket>& value);

    /// Set the event to the specified 'value'.
    void setEvent(const ntca::AcceptEvent& value);

    /// Return the acceptor.
    const bsl::shared_ptr<ntci::Acceptor>& acceptor() const;

    /// Return the accepted stream socket.
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket() const;

    /// Return the event.
    const ntca::AcceptEvent& event() const;
};

NTCCFG_INLINE
AcceptResult::AcceptResult(bslma::Allocator* basicAllocator)
: d_acceptor_sp()
, d_streamSocket_sp()
, d_event()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
AcceptResult::AcceptResult(const AcceptResult& original,
                           bslma::Allocator*   basicAllocator)
: d_acceptor_sp(original.d_acceptor_sp)
, d_streamSocket_sp(original.d_streamSocket_sp)
, d_event(original.d_event)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
AcceptResult::~AcceptResult()
{
}

NTCCFG_INLINE
AcceptResult& AcceptResult::operator=(const AcceptResult& other)
{
    if (this != &other) {
        d_acceptor_sp     = other.d_acceptor_sp;
        d_streamSocket_sp = other.d_streamSocket_sp;
        d_event           = other.d_event;
    }
    return *this;
}

NTCCFG_INLINE
void AcceptResult::reset()
{
    d_acceptor_sp.reset();
    d_streamSocket_sp.reset();
    d_event.reset();
}

NTCCFG_INLINE
void AcceptResult::setAcceptor(const bsl::shared_ptr<ntci::Acceptor>& value)
{
    d_acceptor_sp = value;
}

NTCCFG_INLINE
void AcceptResult::setStreamSocket(
    const bsl::shared_ptr<ntci::StreamSocket>& value)
{
    d_streamSocket_sp = value;
}

NTCCFG_INLINE
void AcceptResult::setEvent(const ntca::AcceptEvent& value)
{
    d_event = value;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::Acceptor>& AcceptResult::acceptor() const
{
    return d_acceptor_sp;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::StreamSocket>& AcceptResult::streamSocket() const
{
    return d_streamSocket_sp;
}

NTCCFG_INLINE
const ntca::AcceptEvent& AcceptResult::event() const
{
    return d_event;
}

}  // close package namespace
}  // close enterprise namespace
#endif
