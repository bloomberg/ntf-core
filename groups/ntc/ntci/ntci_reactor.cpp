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

#include <ntci_reactor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_reactor_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

Reactor::~Reactor()
{
}

ntsa::Error Reactor::showNotifications(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    NTCCFG_WARNING_UNUSED(socket);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::showNotifications(
    ntsa::Handle                             handle,
    const ntci::ReactorNotificationCallback& callback)
{
    NTCCFG_WARNING_UNUSED(handle);
    NTCCFG_WARNING_UNUSED(callback);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::hideNotifications(
    const bsl::shared_ptr<ntci::ReactorSocket>& socket)
{
    NTCCFG_WARNING_UNUSED(socket);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::hideNotifications(ntsa::Handle handle)
{
    NTCCFG_WARNING_UNUSED(handle);
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

}  // close package namespace
}  // close enterprise namespace
