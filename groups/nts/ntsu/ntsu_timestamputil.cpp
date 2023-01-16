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

#include "ntsu_timestamputil.h"

#include <bslmf_assert.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_LINUX)
//keep this include separate from below includes
//as it must precede errqueue.h inclusion
#include <linux/time.h>
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <asm-generic/socket.h>
#include <linux/errqueue.h>
#include <linux/net_tstamp.h>
#include <linux/version.h>
#endif

namespace BloombergLP {
namespace ntsu {

#if defined(BSLS_PLATFORM_OS_LINUX)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)

BSLMF_ASSERT(TimestampUtil::e_SCM_TSTAMP_SND ==
             static_cast<int>(SCM_TSTAMP_SND));
BSLMF_ASSERT(TimestampUtil::e_SCM_TSTAMP_SCHED ==
             static_cast<int>(SCM_TSTAMP_SCHED));
BSLMF_ASSERT(TimestampUtil::e_SCM_TSTAMP_ACK ==
             static_cast<int>(SCM_TSTAMP_ACK));

BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_TX_SCHED ==
             static_cast<int>(SOF_TIMESTAMPING_TX_SCHED));
BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_TX_ACK ==
             static_cast<int>(SOF_TIMESTAMPING_TX_ACK));
BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_OPT_TSONLY ==
             static_cast<int>(SOF_TIMESTAMPING_OPT_TSONLY));

#endif

BSLMF_ASSERT(TimestampUtil::e_SO_TIMESTAMPNS == SO_TIMESTAMPNS);
BSLMF_ASSERT(TimestampUtil::e_SO_TIMESTAMPING == SO_TIMESTAMPING);

BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_TX_HARDWARE ==
             static_cast<int>(SOF_TIMESTAMPING_TX_HARDWARE));
BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_TX_SOFTWARE ==
             static_cast<int>(SOF_TIMESTAMPING_TX_SOFTWARE));
BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_RX_HARDWARE ==
             static_cast<int>(SOF_TIMESTAMPING_RX_HARDWARE));
BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_RX_SOFTWARE ==
             static_cast<int>(SOF_TIMESTAMPING_RX_SOFTWARE));
BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_SOFTWARE ==
             static_cast<int>(SOF_TIMESTAMPING_SOFTWARE));
BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_RAW_HARDWARE ==
             static_cast<int>(SOF_TIMESTAMPING_RAW_HARDWARE));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)

BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_OPT_TSONLY ==
             static_cast<int>(SOF_TIMESTAMPING_OPT_TSONLY));

#endif

BSLMF_ASSERT(sizeof(TimestampUtil::Timespec) == sizeof(timespec));
BSLMF_ASSERT(sizeof(TimestampUtil::Timespec::tv_sec) ==
             sizeof(timespec::tv_sec));
BSLMF_ASSERT(sizeof(TimestampUtil::Timespec::tv_nsec) ==
             sizeof(timespec::tv_nsec));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)

BSLMF_ASSERT(sizeof(TimestampUtil::ScmTimestamping) ==
             sizeof(scm_timestamping));
BSLMF_ASSERT(sizeof(TimestampUtil::ScmTimestamping::softwareTs) ==
             sizeof(scm_timestamping::ts[0]));
BSLMF_ASSERT((sizeof(TimestampUtil::ScmTimestamping::hardwareTs) +
              sizeof(TimestampUtil::ScmTimestamping::deprecated) +
              sizeof(TimestampUtil::ScmTimestamping::hardwareTs)) ==
             sizeof(scm_timestamping::ts[0]) * 3);

#endif

#endif

}  // close package namespace
}  // close enterprise namespace
