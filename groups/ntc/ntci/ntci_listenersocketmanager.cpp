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

#include <ntci_listenersocketmanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_listensocketmanager_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

ListenerSocketManager::~ListenerSocketManager()
{
}

void ListenerSocketManager::processListenerSocketEstablished(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
}

void ListenerSocketManager::processListenerSocketClosed(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
}

void ListenerSocketManager::processListenerSocketLimit(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
{
    NTCCFG_WARNING_UNUSED(listenerSocket);
}

const bsl::shared_ptr<ntci::Strand>& ListenerSocketManager::strand() const
{
    return ntci::Strand::unspecified();
}

}  // close package namespace
}  // close enterprise namespace
