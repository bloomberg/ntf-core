// Copyright 2023 Bloomberg Finance L.P.
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

#include <ntsu_zerocopyutil.h>

#include <bslmf_assert.h>
#include <bsls_platform.h>

// clang-format off
#if defined(BSLS_PLATFORM_OS_LINUX)
#include <sys/time.h>
#include <linux/errqueue.h>
#include <linux/version.h>
#include <sys/socket.h>
#endif
// clang-format on

namespace BloombergLP {
namespace ntsu {

#if defined(BSLS_PLATFORM_OS_LINUX)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)

BSLMF_ASSERT(ZeroCopyUtil::e_SO_ZEROCOPY == static_cast<int>(SO_ZEROCOPY));

BSLMF_ASSERT(ZeroCopyUtil::e_MSG_ZEROCOPY ==
             static_cast<int>(MSG_ZEROCOPY));

BSLMF_ASSERT(ZeroCopyUtil::e_SO_EE_ORIGIN_ZEROCOPY ==
             static_cast<int>(SO_EE_ORIGIN_ZEROCOPY));
BSLMF_ASSERT(ZeroCopyUtil::e_SO_EE_CODE_ZEROCOPY_COPIED ==
             static_cast<int>(SO_EE_CODE_ZEROCOPY_COPIED));

#endif
#endif

}  // close package namespace
}  // close enterprise namespace
