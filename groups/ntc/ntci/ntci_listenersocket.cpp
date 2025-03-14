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

#include <ntci_listenersocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_listenersocket_cpp, "$Id$ $CSID$")

#include <bdlf_bind.h>

namespace BloombergLP {
namespace ntci {

ListenerSocket::~ListenerSocket()
{
}

ntsa::Error ListenerSocket::release(ntsa::Handle* result)
{
    *result = ntsa::k_INVALID_HANDLE;
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::release(ntsa::Handle*              result, 
                                    const ntci::CloseFunction& callback)
{
    NTCCFG_WARNING_UNUSED(callback);

    *result = ntsa::k_INVALID_HANDLE;
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error ListenerSocket::release(ntsa::Handle*              result,
                                    const ntci::CloseCallback& callback)
{
    NTCCFG_WARNING_UNUSED(callback);

    *result = ntsa::k_INVALID_HANDLE;
    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void ListenerSocketCloseGuard::complete(bslmt::Semaphore* semaphore)
{
    semaphore->post();
}

ListenerSocketCloseGuard::ListenerSocketCloseGuard(
    const bsl::shared_ptr<ntci::ListenerSocket>& listenerSocket)
: d_listenerSocket_sp(listenerSocket)
{
}

ListenerSocketCloseGuard::~ListenerSocketCloseGuard()
{
    if (d_listenerSocket_sp) {
        bslmt::Semaphore semaphore;
        d_listenerSocket_sp->close(d_listenerSocket_sp->createCloseCallback(
            bdlf::BindUtil::bind(&ListenerSocketCloseGuard::complete,
                                 &semaphore)));
        semaphore.wait();
    }
}

bsl::shared_ptr<ntci::ListenerSocket> ListenerSocketCloseGuard::release()
{
    bsl::shared_ptr<ntci::ListenerSocket> result = d_listenerSocket_sp;
    d_listenerSocket_sp.reset();
    return result;
}

}  // close package namespace
}  // close enterprise namespace
