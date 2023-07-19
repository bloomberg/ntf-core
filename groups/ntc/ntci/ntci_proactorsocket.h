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

#ifndef INCLUDED_NTCI_PROACTORSOCKET
#define INCLUDED_NTCI_PROACTORSOCKET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <ntsa_receivecontext.h>
#include <ntsa_sendcontext.h>
#include <ntsi_descriptor.h>
#include <ntsi_streamsocket.h>

namespace BloombergLP {
namespace ntci {

/// @internal @brief
/// Provide the storage of the context of the proactor socket within its
/// proactor.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_proactor
class ProactorSocketBase
{
    bsl::shared_ptr<void> d_proactorContext;

  private:
    ProactorSocketBase(const ProactorSocketBase&) BSLS_KEYWORD_DELETED;
    ProactorSocketBase& operator=(const ProactorSocketBase&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new proactor socket context.
    ProactorSocketBase();

    /// Destroy this object.
    ~ProactorSocketBase();

    /// Set the context of the proactor socket within its proactor.
    void setProactorContext(const bsl::shared_ptr<void>& context);

    /// Return the context of the proactor socket within its proactor.
    const bsl::shared_ptr<void>& getProactorContext() const;
};

/// Provide an interface to handle the completion of operations initiated
/// proactively.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_proactor
class ProactorSocket : public ntci::ProactorSocketBase, public ntsi::Descriptor
{
  public:
    /// Destroy this object.
    virtual ~ProactorSocket();

    /// Process the completion of the acceptance of the specified
    /// 'streamSocket' or the specified 'error'.
    virtual void processSocketAccepted(
        const ntsa::Error&                         error,
        const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket);

    /// Process the completion of the establishment of the connection or the
    /// specified 'error'.
    virtual void processSocketConnected(const ntsa::Error& error);

    /// Process the completion of the reception of data described by the
    /// specified 'context' or the specified 'error'.
    virtual void processSocketReceived(const ntsa::Error&          error,
                                       const ntsa::ReceiveContext& context);

    /// Process the completion of the transmission of data described by the
    /// specified 'context' or the specified 'error'.
    virtual void processSocketSent(const ntsa::Error&       error,
                                   const ntsa::SendContext& context);

    /// Process the specified 'error' that has occurred on the socket.
    virtual void processSocketError(const ntsa::Error& error);

    /// Close the stream socket.
    virtual void close() = 0;

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const;

    /// Return true if the proactor socket has stream semantics, otherwise
    /// return false.
    virtual bool isStream() const;

    /// Return true if the proactor socket has datagram semantics, otherwise
    /// return false.
    virtual bool isDatagram() const;

    /// Return true if the proactor socket is a listener for incoming
    /// connection requests, otherwise return false.
    virtual bool isListener() const;

    /// Return the transport used by the socket.
    virtual ntsa::Transport::Value transport() const;
};

NTCCFG_INLINE
ProactorSocketBase::ProactorSocketBase()
{
}

NTCCFG_INLINE
ProactorSocketBase::~ProactorSocketBase()
{
}

NTCCFG_INLINE
void ProactorSocketBase::setProactorContext(
    const bsl::shared_ptr<void>& context)
{
    d_proactorContext = context;
}

NTCCFG_INLINE
const bsl::shared_ptr<void>& ProactorSocketBase::getProactorContext() const
{
    return d_proactorContext;
}

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
