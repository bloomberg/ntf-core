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

#ifndef INCLUDED_NTCI_REACTORSOCKET
#define INCLUDED_NTCI_REACTORSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_reactorevent.h>
#include <ntccfg_platform.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <ntsa_notificationqueue.h>
#include <ntsi_descriptor.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// @internal @brief
/// Provide the storage of the context of the reactor socket within its
/// reactor.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_reactor
class ReactorSocketBase
{
    bsl::shared_ptr<void> d_reactorContext;

  private:
    ReactorSocketBase(const ReactorSocketBase&) BSLS_KEYWORD_DELETED;
    ReactorSocketBase& operator=(const ReactorSocketBase&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new reactor socket context.
    ReactorSocketBase();

    /// Destroy this object.
    ~ReactorSocketBase();

    /// Set the context of the reactor socket within its reactor.
    void setReactorContext(const bsl::shared_ptr<void>& context);

    /// Return the context of the reactor socket within its reactor.
    const bsl::shared_ptr<void>& getReactorContext() const;
};

/// Provide an interface to handle the readiness of operations detected by a
/// reactor.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_reactor
class ReactorSocket : public ntci::ReactorSocketBase, public ntsi::Descriptor
{
  public:
    /// Destroy this object.
    virtual ~ReactorSocket();

    /// Process the specified 'event' indicating the socket is readable.
    virtual void processSocketReadable(const ntca::ReactorEvent& event);

    /// Process the specified 'event' indicating the socket is writable.
    virtual void processSocketWritable(const ntca::ReactorEvent& event);

    /// Process the specified 'event' indicating an error was detected for
    /// the socket.
    virtual void processSocketError(const ntca::ReactorEvent& event);

    /// Process the specified 'notifications' of the socket.
    virtual void processNotifications(
        const ntsa::NotificationQueue& notifications);

    /// Close the stream socket.
    virtual void close() = 0;

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const;
};

NTCCFG_INLINE
ReactorSocketBase::ReactorSocketBase()
{
}

NTCCFG_INLINE
ReactorSocketBase::~ReactorSocketBase()
{
}

NTCCFG_INLINE
void ReactorSocketBase::setReactorContext(const bsl::shared_ptr<void>& context)
{
    d_reactorContext = context;
}

NTCCFG_INLINE
const bsl::shared_ptr<void>& ReactorSocketBase::getReactorContext() const
{
    return d_reactorContext;
}

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
