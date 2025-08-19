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

#include <ntcf_concurrent.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcf_concurrent_cpp, "$Id$ $CSID$")

#if NTC_BUILD_WITH_COROUTINES
namespace BloombergLP {
namespace ntcf {

void Concurrent::initialize()
{
}

ntsa::Error Concurrent::receive(
    ntca::ReceiveContext*                        context,
    const bsl::shared_ptr<ntci::Scheduler>&      scheduler,
    const bsl::shared_ptr<ntci::DatagramSocket>& datagramSocket,
    const ntca::ReceiveOptions&                  options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(scheduler);
    NTCCFG_WARNING_UNUSED(datagramSocket);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

void Concurrent::exit()
{
}

}  // close namespace ntcf
}  // close namespace BloombergLP
#endif
