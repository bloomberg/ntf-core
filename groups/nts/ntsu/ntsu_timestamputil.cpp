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
#include <bsls_log.h>
#include <bsl_cstdint.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

// clang-format off
#if defined(BSLS_PLATFORM_OS_LINUX)
#include <sys/time.h>
#include <sys/socket.h>
#include <linux/errqueue.h>
#include <linux/net_tstamp.h>
#include <linux/version.h>
#endif
// clang-format on

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <winerror.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
// clang-format on
#ifdef interface
#undef interface
#endif
#pragma comment(lib, "ws2_32")
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)

#define NTSU_TIMESTAMP_UTIL_LINUX_VERSION(major, minor, patch)                \
    static_cast<bsl::size_t>(                                                 \
        KERNEL_VERSION(static_cast<bsl::size_t>(major),                       \
                       static_cast<bsl::size_t>(minor),                       \
                       static_cast<bsl::size_t>(patch)))

#endif

namespace BloombergLP {
namespace ntsu {

/// Provide a private implementation.
class TimestampUtil::Impl
{
  public:
#if defined(BSLS_PLATFORM_OS_LINUX)
    // clang-format off

    /// The structure describing a timestamping option and its minimum required
    /// kernel version.
    struct OptionSupport {
        /// The timestamping option.
        int option;

        /// The major version number of the Linux kernel that introduced the
        /// option.
        int versionMajor;

        /// The minor version number of the Linux kernel that introduced the
        /// option.
        int versionMinor;

        /// The patch version number of the Linux kernel that introduced the
        /// option.
        int versionPatch;
    };

    /// Return true if the specified current Linux kernel 'versionMajor',
    /// 'versionMinor', and 'versionPatch' satisfies the specified 
    /// 'optionSupport', otherwise return false.
    static bool isSupported(int                  versionMajor, 
                            int                  versionMinor, 
                            int                  versionPatch, 
                            const OptionSupport& optionSupport)
    {
        const bsl::size_t currentVersion = 
            NTSU_TIMESTAMP_UTIL_LINUX_VERSION(
                versionMajor, 
                versionMinor,
                versionPatch);

        const bsl::size_t minimumVersion =
            NTSU_TIMESTAMP_UTIL_LINUX_VERSION(
                optionSupport.versionMajor,
                optionSupport.versionMinor,
                optionSupport.versionPatch);

        return (currentVersion >= minimumVersion);
    }

    /// The timestamping option support array.
    static const TimestampUtil::Impl::OptionSupport s_support[19];

    // clang-format on
#endif
};

#if defined(BSLS_PLATFORM_OS_LINUX)

// clang-format off
const TimestampUtil::Impl::OptionSupport TimestampUtil::Impl::s_support[19] = {
    { ntsu::TimestampUtil::e_SCM_TSTAMP_SND,                 4, 18, 0 },
    { ntsu::TimestampUtil::e_SCM_TSTAMP_SCHED,               4, 18, 0 },
    { ntsu::TimestampUtil::e_SCM_TSTAMP_ACK,                 4, 18, 0 },

    { ntsu::TimestampUtil::e_SO_TIMESTAMPNS,                 4, 18, 0 },
    { ntsu::TimestampUtil::e_SO_TIMESTAMPING,                4, 18, 0 },
    { ntsu::TimestampUtil::e_SCM_TIMESTAMPNS,                4, 18, 0 },
    { ntsu::TimestampUtil::e_SCM_TIMESTAMPING,               4, 18, 0 },

    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_HARDWARE,   4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SOFTWARE,   4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SCHED,      4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_ACK,        4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_HARDWARE,   4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_SOFTWARE,   4, 18, 0 },

    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_SOFTWARE,      4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RAW_HARDWARE,  4, 18, 0 },

    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_ID,        4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_ID_TCP,    6,  2, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_TSONLY,    4, 18, 0 },
    { ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_RX_FILTER, 6, 12, 0 }
};
// clang-format on

#endif

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

BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_OPT_ID ==
             static_cast<int>(SOF_TIMESTAMPING_OPT_ID));

BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_OPT_TSONLY ==
             static_cast<int>(SOF_TIMESTAMPING_OPT_TSONLY));

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 2, 0)

BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_OPT_ID_TCP ==
             static_cast<int>(SOF_TIMESTAMPING_OPT_ID_TCP));

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 21, 0)

BSLMF_ASSERT(TimestampUtil::e_SOF_TIMESTAMPING_OPT_RX_FILTER ==
             static_cast<int>(SOF_TIMESTAMPING_OPT_RX_FILTER));

#endif

BSLMF_ASSERT(sizeof(TimestampUtil::Timespec) == sizeof(timespec));
BSLMF_ASSERT(sizeof(TimestampUtil::Timespec().tv_sec) ==
             sizeof(timespec().tv_sec));
BSLMF_ASSERT(sizeof(TimestampUtil::Timespec().tv_nsec) ==
             sizeof(timespec().tv_nsec));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)

BSLMF_ASSERT(sizeof(TimestampUtil::ScmTimestamping) ==
             sizeof(scm_timestamping));
BSLMF_ASSERT(sizeof(TimestampUtil::ScmTimestamping().softwareTs) ==
             sizeof(scm_timestamping().ts[0]));
BSLMF_ASSERT((sizeof(TimestampUtil::ScmTimestamping().hardwareTs) +
              sizeof(TimestampUtil::ScmTimestamping().deprecated) +
              sizeof(TimestampUtil::ScmTimestamping().hardwareTs)) ==
             sizeof(scm_timestamping().ts[0]) * 3);

#endif

#endif

int TimestampUtil::socketOptionLevel()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    return SOL_SOCKET;

#else

    return 0;

#endif
}

int TimestampUtil::socketOptionName()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    return ntsu::TimestampUtil::e_SO_TIMESTAMPING;

#else

    return 0;

#endif
}

int TimestampUtil::socketOptionValueReporting()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    return ntsu::TimestampUtil::e_SOF_TIMESTAMPING_SOFTWARE | 
           ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RAW_HARDWARE;

#else

    return 0;

#endif
}

int TimestampUtil::socketOptionValueRxGeneration()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    return ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_HARDWARE | 
           ntsu::TimestampUtil::e_SOF_TIMESTAMPING_RX_SOFTWARE;

#else

    return 0;

#endif
}

int TimestampUtil::socketOptionValueRxFlags()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    int result = 0;

    int versionMajor = 0;
    int versionMinor = 0;
    int versionPatch = 0;
    int build        = 0;

    ntsscm::Version::systemVersion(&versionMajor,
                                   &versionMinor,
                                   &versionPatch,
                                   &build);

    NTSCFG_WARNING_UNUSED(build);

    const bsl::size_t currentVersion = 
        NTSU_TIMESTAMP_UTIL_LINUX_VERSION(versionMajor, 
                                          versionMinor, 
                                          versionPatch);

    if (currentVersion >= NTSU_TIMESTAMP_UTIL_LINUX_VERSION(6, 12, 0)) {
        result |= e_SOF_TIMESTAMPING_OPT_RX_FILTER;
    }

    return result;

#else

    return 0;

#endif
}

int TimestampUtil::socketOptionValueTxGeneration()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    return ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_HARDWARE | 
           ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SOFTWARE |
           ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_SCHED | 
           ntsu::TimestampUtil::e_SOF_TIMESTAMPING_TX_ACK;

#else

    return 0;

#endif
}

int TimestampUtil::socketOptionValueTxFlags()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    int result = 0;

    int versionMajor = 0;
    int versionMinor = 0;
    int versionPatch = 0;
    int build        = 0;

    ntsscm::Version::systemVersion(&versionMajor,
                                   &versionMinor,
                                   &versionPatch,
                                   &build);

    NTSCFG_WARNING_UNUSED(build);

    const bsl::size_t currentVersion = 
        NTSU_TIMESTAMP_UTIL_LINUX_VERSION(versionMajor, 
                                          versionMinor, 
                                          versionPatch);

    if (currentVersion >= NTSU_TIMESTAMP_UTIL_LINUX_VERSION(4, 18, 0)) {
        result |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_ID;
    }

    if (currentVersion >= NTSU_TIMESTAMP_UTIL_LINUX_VERSION(6, 2, 0)) {
        result |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_ID_TCP;
    }

    if (currentVersion >= NTSU_TIMESTAMP_UTIL_LINUX_VERSION(4, 18, 0)) {
        result |= ntsu::TimestampUtil::e_SOF_TIMESTAMPING_OPT_TSONLY;
    }

    return result;

#else

    return 0;

#endif
}

int TimestampUtil::setRxTimestamps(int optionValue, bool enabled)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    int result = optionValue;

    if (enabled) {
        result |= ntsu::TimestampUtil::socketOptionValueRxGeneration();
        result |= ntsu::TimestampUtil::socketOptionValueRxFlags();
        result |= ntsu::TimestampUtil::socketOptionValueReporting();
    }
    else {
        result &= ~ntsu::TimestampUtil::socketOptionValueRxGeneration();
        result &= ~ntsu::TimestampUtil::socketOptionValueRxFlags();
        if ((result &
             ntsu::TimestampUtil::socketOptionValueTxGeneration()) == 0)
        {
            result &= ~ntsu::TimestampUtil::socketOptionValueReporting();
        }
    }

    return result;

#else

    NTSCFG_WARNING_UNUSED(optionValue);
    NTSCFG_WARNING_UNUSED(enabled);

    return 0;

#endif
}

int TimestampUtil::setTxTimestamps(int optionValue, bool enabled)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    int result = optionValue;

    if (enabled) {
        result |= ntsu::TimestampUtil::socketOptionValueTxGeneration();
        result |= ntsu::TimestampUtil::socketOptionValueTxFlags();
        result |= ntsu::TimestampUtil::socketOptionValueReporting();
    }
    else {
        result &= ~ntsu::TimestampUtil::socketOptionValueTxGeneration();
        result &= ~ntsu::TimestampUtil::socketOptionValueTxFlags();

        if ((result &
             ntsu::TimestampUtil::socketOptionValueRxGeneration()) == 0)
        {
            result &= ~ntsu::TimestampUtil::socketOptionValueReporting();
        }
    }

    return result;

#else

    NTSCFG_WARNING_UNUSED(optionValue);
    NTSCFG_WARNING_UNUSED(enabled);

    return 0;

#endif
}

bool TimestampUtil::hasRxTimestamps(int optionValue)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    return ((optionValue &
             ntsu::TimestampUtil::socketOptionValueRxGeneration()) != 0);

#else

    NTSCFG_WARNING_UNUSED(optionValue);
    return false;

#endif
}

bool TimestampUtil::hasTxTimestamps(int optionValue)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    return ((optionValue &
             ntsu::TimestampUtil::socketOptionValueTxGeneration()) != 0);

#else

    NTSCFG_WARNING_UNUSED(optionValue);
    return false;

#endif
}

bool TimestampUtil::supportsOption(int option,
                                   int versionMajor,
                                   int versionMinor,
                                   int versionPatch)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    const bsl::size_t count = sizeof(TimestampUtil::Impl::s_support) /
                              sizeof(TimestampUtil::Impl::s_support[0]);

    for (bsl::size_t i = 0; i < count; ++i) {
        if ((option & TimestampUtil::Impl::s_support[i].option) != 0) {
            return TimestampUtil::Impl::isSupported(
                versionMajor,
                versionMinor,
                versionPatch,
                TimestampUtil::Impl::s_support[i]);
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

    ntsscm::Version::systemVersion(&versionMajor,
                                   &versionMinor,
                                   &versionPatch,
                                   &build);

    NTSCFG_WARNING_UNUSED(build);

    int result = options;

    int n = bdlb::BitUtil::numLeadingUnsetBits(
        static_cast<bsl::uint32_t>(options));

    for (int i = 0; i <= 32 - n; ++i) {
        const int flag = (1 << i);
        if ((options & flag) != 0) {
            const bool supportsOption =
                ntsu::TimestampUtil::supportsOption(flag,
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
