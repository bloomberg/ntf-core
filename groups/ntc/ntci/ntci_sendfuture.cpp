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

#include <ntci_sendfuture.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_sendfuture_cpp, "$Id$ $CSID$")

#include <bdlf_memfn.h>
#include <bslmt_lockguard.h>
#include <bslscm_versiontag.h>

#if BSL_VERSION >= BSL_MAKE_VERSION(3, 80)
#define NTCI_SENDFUTURE_TIMEOUT bslmt::Condition::e_TIMED_OUT
#else
#define NTCI_SENDFUTURE_TIMEOUT -1
#endif

namespace BloombergLP {
namespace ntci {

void SendFuture::arrive(const bsl::shared_ptr<ntci::Sender>& sender,
                        const ntca::SendEvent&               event)
{
    NTCCFG_WARNING_UNUSED(sender);

    ntccfg::ConditionMutexGuard lock(&d_mutex);

    SendResult result;
    result.setSender(sender);
    result.setEvent(event);

    d_resultQueue.push_back(result);

    d_condition.signal();
}

SendFuture::SendFuture(bslma::Allocator* basicAllocator)
: ntci::SendCallback(basicAllocator)
, d_mutex()
, d_condition()
, d_resultQueue(basicAllocator)
{
    this->setFunction(bdlf::MemFnUtil::memFn(&SendFuture::arrive, this));
}

SendFuture::~SendFuture()
{
}

ntsa::Error SendFuture::wait(ntci::SendResult* result)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (d_resultQueue.empty()) {
        d_condition.wait(&d_mutex);
    }

    *result = d_resultQueue.front();
    d_resultQueue.pop_front();

    return ntsa::Error();
}

ntsa::Error SendFuture::wait(ntci::SendResult*         result,
                             const bsls::TimeInterval& timeout)
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);

    while (d_resultQueue.empty()) {
        int rc = d_condition.timedWait(&d_mutex, timeout);
        if (rc == 0) {
            break;
        }
        else if (rc == NTCI_SENDFUTURE_TIMEOUT) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    *result = d_resultQueue.front();
    d_resultQueue.pop_front();

    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
