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

#ifndef INCLUDED_NTSU_TIMESTAMPUTIL
#define INCLUDED_NTSU_TIMESTAMPUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

namespace BloombergLP {
namespace ntsu {

/// This struct is used to define/redefine types and constants used for Linux
/// network timestamping feature.
struct TimestampUtil {
    enum {  //copied from include/linux/errqueue.h
        e_SCM_TSTAMP_SND,
        e_SCM_TSTAMP_SCHED,
        e_SCM_TSTAMP_ACK
    };

    enum {  //copied from include/asm-generic/socket.h
        e_SO_TIMESTAMPNS   = 35,
        e_SO_TIMESTAMPING  = 37,
        e_SCM_TIMESTAMPNS  = e_SO_TIMESTAMPNS,
        e_SCM_TIMESTAMPING = e_SO_TIMESTAMPING
    };

    enum {
        e_SOF_TIMESTAMPING_TX_HARDWARE = (1 << 0),
        e_SOF_TIMESTAMPING_TX_SOFTWARE = (1 << 1),
        e_SOF_TIMESTAMPING_RX_HARDWARE = (1 << 2),
        e_SOF_TIMESTAMPING_RX_SOFTWARE = (1 << 3),
        e_SOF_TIMESTAMPING_SOFTWARE    = (1 << 4),

        e_SOF_TIMESTAMPING_RAW_HARDWARE = (1 << 6),
        e_SOF_TIMESTAMPING_OPT_ID       = (1 << 7),
        e_SOF_TIMESTAMPING_TX_SCHED     = (1 << 8),
        e_SOF_TIMESTAMPING_TX_ACK       = (1 << 9),
        e_SOF_TIMESTAMPING_OPT_TSONLY   = (1 << 11)
    };

    struct Timespec {  //copy of Linux struct timespec
        long tv_sec;
        long tv_nsec;
    };

    struct ScmTimestamping {  //copy of linux struct scm_timestamping
        Timespec softwareTs;
        Timespec deprecated;
        Timespec hardwareTs;
    };
};

}  // close package namespace
}  // close enterprise namespace

#endif
