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

#include <ntci_streamsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_streamsocket_cpp, "$Id$ $CSID$")

#include <bdlf_bind.h>

namespace BloombergLP {
namespace ntci {

StreamSocket::~StreamSocket()
{
}

ntsa::Error StreamSocket::setZeroCopyThreshold(bsl::size_t value)
{
    NTCCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::timestampOutgoingData(bool enable)
{
    NTCCFG_WARNING_UNUSED(enable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error StreamSocket::timestampIncomingData(bool enable)
{
    NTCCFG_WARNING_UNUSED(enable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void StreamSocketCloseGuard::complete(bslmt::Semaphore* semaphore)
{
    semaphore->post();
}

StreamSocketCloseGuard::StreamSocketCloseGuard(
    const bsl::shared_ptr<ntci::StreamSocket>& streamSocket)
: d_streamSocket_sp(streamSocket)
{
}

StreamSocketCloseGuard::~StreamSocketCloseGuard()
{
    if (d_streamSocket_sp) {
        bslmt::Semaphore semaphore;
        d_streamSocket_sp->close(d_streamSocket_sp->createCloseCallback(
            bdlf::BindUtil::bind(&StreamSocketCloseGuard::complete,
                                 &semaphore)));
        semaphore.wait();
    }
}

bsl::shared_ptr<ntci::StreamSocket> StreamSocketCloseGuard::release()
{
    bsl::shared_ptr<ntci::StreamSocket> result = d_streamSocket_sp;
    d_streamSocket_sp.reset();
    return result;
}

}  // close package namespace
}  // close enterprise namespace
