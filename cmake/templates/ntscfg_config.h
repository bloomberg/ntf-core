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

#ifndef INCLUDED_NTSCFG_CONFIG
#define INCLUDED_NTSCFG_CONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsscm_version.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntscfg {

// The build platform name.
#define NTS_BUILD_UNAME "@NTF_BUILD_UNAME@"

// The build configuration flags.
#define NTS_BUILD_UFID "@NTF_BUILD_UFID@"

// The build branch.
#define NTS_BUILD_BRANCH "@NTS_BUILD_BRANCH@"

// The build commit hash.
#define NTS_BUILD_COMMIT_HASH "@NTS_BUILD_COMMIT_HASH@"

// The build commit hash, abbreviated.
#define NTS_BUILD_COMMIT_HASH_ABBREV "@NTS_BUILD_COMMIT_HASH_ABBREV@"

// Build with BSL.
#define NTS_BUILD_WITH_BSL @NTF_BUILD_WITH_BSL@

// Build with BDL.
#define NTS_BUILD_WITH_BDL @NTF_BUILD_WITH_BDL@

// Build with BAL.
#define NTS_BUILD_WITH_BAL @NTF_BUILD_WITH_BAL@

// Build with support for the Internet Protocol version 4 address family.
#define NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV4 @NTF_BUILD_WITH_ADDRESS_FAMILY_IPV4@

// Build with support for the Internet Protocol version 6 address family.
#define NTSCFG_BUILD_WITH_ADDRESS_FAMILY_IPV6 @NTF_BUILD_WITH_ADDRESS_FAMILY_IPV6@

// Build with support for the local (a.k.a. Unix) address family.
#define NTSCFG_BUILD_WITH_ADDRESS_FAMILY_LOCAL @NTF_BUILD_WITH_ADDRESS_FAMILY_LOCAL@

// Build with support for the Transmission Control Protocol (TCP) transport 
// protocol.
#define NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_TCP @NTF_BUILD_WITH_TRANSPORT_PROTOCOL_TCP@

// Build with support for the User Datagram Protocol (UDP) transport protocol.
#define NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_UDP @NTF_BUILD_WITH_TRANSPORT_PROTOCOL_UDP@

// Build with support for the local (a.k.a. Unix) transport protocol.
#define NTSCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL @NTF_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL@

// Build with support for being able to configure 'select' as the driver for
// a reactor. This driver is available on all platforms.
#define NTS_BUILD_WITH_SELECT @NTF_BUILD_WITH_SELECT@

// Build with support for being able to configure 'poll' as the driver for
// a reactor. This driver is available on all platforms.
#define NTS_BUILD_WITH_POLL @NTF_BUILD_WITH_POLL@

// Build with support for being able to configure 'epoll' as the driver for
// a reactor. This driver is available on Linux.
#define NTS_BUILD_WITH_EPOLL @NTF_BUILD_WITH_EPOLL@

// Build with support for being able to configure the '/dev/poll' device as
// the driver for a reactor. This driver is avalable on Solaris.
#define NTS_BUILD_WITH_DEVPOLL @NTF_BUILD_WITH_DEVPOLL@

// Build with support for being able to configure the event port API as the
// driver for a reactor. This driver is available on Solaris.
#define NTS_BUILD_WITH_EVENTPORT @NTF_BUILD_WITH_EVENTPORT@

// Build with support for being able to configure 'pollset' as the driver for
// a reactor. This driver is available on AIX.
#define NTS_BUILD_WITH_POLLSET @NTF_BUILD_WITH_POLLSET@

// Build with support for being able to configure 'kqueue' as the driver for
// a reactor. This driver is available on Darwin and FreeBSD.
#define NTS_BUILD_WITH_KQUEUE @NTF_BUILD_WITH_KQUEUE@

// Build with logging.
#define NTS_BUILD_WITH_LOGGING @NTF_BUILD_WITH_LOGGING@

// Build with metrics.
#define NTS_BUILD_WITH_METRICS @NTF_BUILD_WITH_METRICS@

// Build with inlining suggested.
#define NTS_BUILD_WITH_INLINING_SUGGESTED @NTF_BUILD_WITH_INLINING_SUGGESTED@

// Build with inlining forced.
#define NTS_BUILD_WITH_INLINING_FORCED @NTF_BUILD_WITH_INLINING_FORCED@

// Build with inlining disabled.
#define NTS_BUILD_WITH_INLINING_DISABLED @NTF_BUILD_WITH_INLINING_DISABLED@

// Build with branch prediction.
#define NTS_BUILD_WITH_BRANCH_PREDICTION @NTF_BUILD_WITH_BRANCH_PREDICTION@

// Build with mutually-exclusive locks implemented with spin locks.
#define NTS_BUILD_WITH_SPIN_LOCKS @NTF_BUILD_WITH_SPIN_LOCKS@

// Build with mutually-exclusive locks implemented with userspace mutexes.
#define NTS_BUILD_WITH_USERSPACE_MUTEXES @NTF_BUILD_WITH_USERSPACE_MUTEXES@

// Build with mutually-exclusive locks implemented with system mutexes.
#define NTS_BUILD_WITH_SYSTEM_MUTEXES @NTF_BUILD_WITH_SYSTEM_MUTEXES@

// Build with mutually-exclusive locks implemented with recursive mutexes.
#define NTS_BUILD_WITH_RECURSIVE_MUTEXES @NTF_BUILD_WITH_RECURSIVE_MUTEXES@

// Build with a test allocator that dumps stack traces upon memory leaks.
#define NTS_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR @NTF_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR@

// Build with compiler warnings enabled.
#define NTS_BUILD_WITH_WARNINGS @NTF_BUILD_WITH_WARNINGS@

// Build with compiler instrumentation for code coverage.
#define NTS_BUILD_WITH_COVERAGE @NTF_BUILD_WITH_COVERAGE@

// Build from a packaging environment.
#define NTS_BUILD_FROM_PACKAGING @NTF_BUILD_FROM_PACKAGING@

// Build from a continuous integration environment.
#define NTS_BUILD_FROM_CONTINUOUS_INTEGRATION @NTF_BUILD_FROM_CONTINUOUS_INTEGRATION@

}  // close package namespace
}  // close enterprise namespace
#endif
