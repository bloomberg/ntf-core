// Copyright 2024 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTCD_TIMER
#define INCLUDED_NTCD_TIMER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntci_timer.h>
#include <ntccfg_test.h>

namespace BloombergLP {
namespace ntcd {

#if NTCCFG_TEST_MOCK_ENABLED

NTF_MOCK_CLASS(TimerMock, ntci::Timer)
NTF_MOCK_METHOD(ntsa::Error,
                schedule,
                const bsls::TimeInterval&,
                const bsls::TimeInterval&)
NTF_MOCK_METHOD(ntsa::Error, cancel)
NTF_MOCK_METHOD(ntsa::Error, close)
NTF_MOCK_METHOD(void,
                arrive,
                const bsl::shared_ptr<ntci::Timer>&,
                const bsls::TimeInterval&,
                const bsls::TimeInterval&)
NTF_MOCK_METHOD_CONST(void*, handle)
NTF_MOCK_METHOD_CONST(int, id)
NTF_MOCK_METHOD_CONST(bool, oneShot)
NTF_MOCK_METHOD_CONST(bdlb::NullableValue<bsls::TimeInterval>, deadline)
NTF_MOCK_METHOD_CONST(bdlb::NullableValue<bsls::TimeInterval>, period)
NTF_MOCK_METHOD_CONST(bslmt::ThreadUtil::Handle, threadHandle)
NTF_MOCK_METHOD_CONST(size_t, threadIndex)
NTF_MOCK_METHOD_CONST(const bsl::shared_ptr<ntci::Strand>&, strand)
NTF_MOCK_METHOD_CONST(bsls::TimeInterval, currentTime)
NTF_MOCK_CLASS_END;

#endif

}  // close package namespace
}  // close enterprise namespace

#endif
