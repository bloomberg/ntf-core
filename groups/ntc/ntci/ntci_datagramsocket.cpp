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

#include <ntci_datagramsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_datagramsocket_cpp, "$Id$ $CSID$")

#include <bdlf_bind.h>

namespace BloombergLP {
namespace ntci {

DatagramSocket::~DatagramSocket()
{
}

ntsa::Error DatagramSocket::setZeroCopyThreshold(bsl::size_t value)
{
    NTCCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::timestampOutgoingData(bool enable)
{
    NTCCFG_WARNING_UNUSED(enable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error DatagramSocket::timestampIncomingData(bool enable)
{
    NTCCFG_WARNING_UNUSED(enable);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void DatagramSocketCloseGuard::complete(bslmt::Semaphore* semaphore)
{
    semaphore->post();
}

DatagramSocketCloseGuard::DatagramSocketCloseGuard(
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket)
: d_datagramSocket_sp(datagramSocket)
{
}

DatagramSocketCloseGuard::~DatagramSocketCloseGuard()
{
    if (d_datagramSocket_sp) {
        bslmt::Semaphore semaphore;
        d_datagramSocket_sp->close(d_datagramSocket_sp->createCloseCallback(
            bdlf::BindUtil::bind(&DatagramSocketCloseGuard::complete,
                                 &semaphore)));
        semaphore.wait();
    }
}

bsl::shared_ptr<ntci::DatagramSocket> DatagramSocketCloseGuard::release()
{
    bsl::shared_ptr<ntci::DatagramSocket> result = d_datagramSocket_sp;
    d_datagramSocket_sp.reset();
    return result;
}

}  // close package namespace
}  // close enterprise namespace
