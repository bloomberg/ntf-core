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

#include <ntsu_timestamputil.h>

#include <bdlb_bitutil.h>
#include <bslmf_assert.h>
#include <bsls_platform.h>
#include <bsl_cstdint.h>
#include <bsl_sstream.h>
#include <bsl_iomanip.h>

// clang-format off
#if defined(BSLS_PLATFORM_OS_LINUX)
#include <sys/time.h>
#include <sys/socket.h>
#include <linux/errqueue.h>
#include <linux/net_tstamp.h>
#include <linux/version.h>
#endif
// clang-format on

#if defined(BSLS_PLATFORM_OS_LINUX)

#define NTSU_TIMESTAMP_UTIL_LINUX_VERSION(major, minor, patch) \
    static_cast<bsl::size_t>( \
        KERNEL_VERSION( \
            static_cast<bsl::size_t>(versionMajor), \
            static_cast<bsl::size_t>(versionMinor), \
            static_cast<bsl::size_t>(versionPatch)))

#define NTSU_TIMESTAMP_UTIL_LINUX_VERSION_GE(version, major, minor, patch) \
    (static_cast<bsl::size_t>(version)) >= \
    (NTSU_TIMESTAMP_UTIL_LINUX_VERSION(major, minor, patch))

#endif

namespace BloombergLP {
namespace ntsu {

namespace {

#if defined(BSLS_PLATFORM_OS_LINUX)

struct TimestampOtionSupport {
    int option;
    int versionMajor;
    int versionMinor;
    int versionPatch;
};

// clang-format off
static TimestampOtionSupport s_timestampOptionSupport[] = {
    { ntsu::TimestampUtil::e_SCM_TSTAMP_SND,                4, 18, 0 },
    { ntsu::TimestampUtil::e_SCM_TSTAMP_SCHED,              4, 18, 0 },
    { ntsu::TimestampUtil::e_SCM_TSTAMP_ACK,                4, 18, 0 },

    { ntsu::TimestampUtil::e_SO_TIMESTAMPNS,                4, 18, 0 },
    { ntsu::TimestampUtil::e_SO_TIMESTAMPING,               4, 18, 0 },
    { ntsu::TimestampUtil::e_SCM_TIMESTAMPNS,               4, 18, 0 },
    { ntsu::TimestampUtil::e_SCM_TIMESTAMPING,              4, 18, 0 },

    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_HARDWARE,  4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SOFTWARE,  4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SCHED,     4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_ACK,       4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_HARDWARE,  4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_SOFTWARE,  4, 18, 0 },

    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_SOFTWARE,     4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RAW_HARDWARE, 4, 18, 0 },

    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_ID,       4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_TSONLY,   4, 18, 0 }
};
// clang-format on

#endif

} // close unnamed namespace

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

bool TimestampUtil::supportsOption(int option, 
                                   int versionMajor, 
                                   int versionMinor, 
                                   int versionPatch)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    bsl::size_t version = 
        NTSU_TIMESTAMP_UTIL_LINUX_VERSION(
            versionMajor, 
            versionMinor, 
            versionPatch);

    const bsl::size_t count = 
        sizeof(s_timestampOptionSupport) / sizeof(s_timestampOptionSupport[0]);

    for (bsl::size_t i = 0; i < count; ++i) {
        if ((option & s_timestampOptionSupport[i].option) != 0) {
            return NTSU_TIMESTAMP_UTIL_LINUX_VERSION_GE(
                version, 
                s_timestampOptionSupport[i].versionMajor, 
                s_timestampOptionSupport[i].versionMinor, 
                s_timestampOptionSupport[i].versionPatch);
        }
    }

    return false;

#else

    NTSCFG_WARNING_UNUSED(option);
    NTSCFG_WARNING_UNUSED(versionMajor);
    NTSCFG_WARNING_UNUSED(versionMinor);
    NTSCFG_WARNING_UNUSED(versionPatch);

    return false;

#endif
}

int TimestampUtil::removeUnsupported(int options)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    int versionMajor = 0;
    int versionMinor = 0;
    int versionPatch = 0;
    int build        = 0;

    ntsscm::Version::systemVersion(
        &versionMajor, &versionMinor, &versionPatch, &build);

    NTSCFG_WARNING_UNUSED(build);

    int result = options;

    int n = bdlb::BitUtil::numLeadingUnsetBits(
        static_cast<bsl::uint32_t>(options));

    for (int i = 0; i <= 32 - n; ++i) {
        int flag = (1 << i);

        if ((options & flag) != 0) {
            const bool supportsOption = ntsu::TimestampUtil::supportsOption(
                flag,
                versionMajor,
                versionMinor,
                versionPatch);

            if (!supportsOption) {
                result &= ~flag;
            }
        }
    }

    return result;

#else

    return options;

#endif
}

bsl::string TimestampUtil::describeDelay(const bsls::TimeInterval& duration)
{
    bsl::stringstream ss;

    if (duration.seconds() == 0) {
        if (duration.nanoseconds() < 1000) {
            ss << duration.nanoseconds() << "ns";
        }
        else if (duration.nanoseconds() < 1000 * 1000) {
            ss << duration.nanoseconds() / 1000 << "us";
        }
        else if (duration.nanoseconds() < 1000 * 1000 * 1000) {
            ss << duration.nanoseconds() / 1000 / 1000 << "ms";
        }
        else {
            ss << bsl::fixed << bsl::setprecision(2) 
               << duration.totalSecondsAsDouble() << "s";
        }
    }
    else {
        ss << bsl::fixed << bsl::setprecision(2) 
           << duration.totalSecondsAsDouble() << "s";
    }

    return ss.str();
}

}  // close package namespace
}  // close enterprise namespace
