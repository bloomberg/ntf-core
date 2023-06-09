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

#include <ntci_reactorsocket.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_reactorsocket_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

ReactorSocket::~ReactorSocket()
{
}

void ReactorSocket::processSocketReadable(const ntca::ReactorEvent& event)
{
    NTCCFG_WARNING_UNUSED(event);
}

void ReactorSocket::processSocketWritable(const ntca::ReactorEvent& event)
{
    NTCCFG_WARNING_UNUSED(event);
}

void ReactorSocket::processSocketError(const ntca::ReactorEvent& event)
{
    NTCCFG_WARNING_UNUSED(event);
}

void ReactorSocket::processNotifications(
    const ntsa::NotificationQueue& notifications)
{
    NTCCFG_WARNING_UNUSED(notifications);
}

const bsl::shared_ptr<ntci::Strand>& ReactorSocket::strand() const
{
    return ntci::Strand::unspecified();
}

}  // close package namespace
}  // close enterprise namespace
