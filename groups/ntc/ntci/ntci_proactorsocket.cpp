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

#include <ntci_proactorsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_proactorsocket_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

ProactorSocket::~ProactorSocket()
{
}

void ProactorSocket::processSocketAccepted(
    const ntsa::Error&                         error,
    const bsl::shared_ptr<ntsi::StreamSocket>& streamSocket)
{
    NTCCFG_WARNING_UNUSED(error);
    NTCCFG_WARNING_UNUSED(streamSocket);
}

void ProactorSocket::processSocketConnected(const ntsa::Error& error)
{
    NTCCFG_WARNING_UNUSED(error);
}

void ProactorSocket::processSocketReceived(const ntsa::Error&          error,
                                           const ntsa::ReceiveContext& context)
{
    NTCCFG_WARNING_UNUSED(error);
    NTCCFG_WARNING_UNUSED(context);
}

void ProactorSocket::processSocketSent(const ntsa::Error&       error,
                                       const ntsa::SendContext& context)
{
    NTCCFG_WARNING_UNUSED(error);
    NTCCFG_WARNING_UNUSED(context);
}

void ProactorSocket::processSocketError(const ntsa::Error& error)
{
    NTCCFG_WARNING_UNUSED(error);
}

void ProactorSocket::processSocketDetached()
{
}

const bsl::shared_ptr<ntci::Strand>& ProactorSocket::strand() const
{
    return ntci::Strand::unspecified();
}

bool ProactorSocket::isStream() const
{
    return false;
}

bool ProactorSocket::isDatagram() const
{
    return false;
}

bool ProactorSocket::isListener() const
{
    return false;
}

ntsa::Transport::Value ProactorSocket::transport() const
{
    return ntsa::Transport::e_UNDEFINED;
}

void ProactorSocket::getInfo(ntsa::SocketInfo* result) const
{
    result->reset();
}

}  // close package namespace
}  // close enterprise namespace
