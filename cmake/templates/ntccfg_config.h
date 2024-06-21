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

#ifndef INCLUDED_NTCCFG_CONFIG
#define INCLUDED_NTCCFG_CONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcscm_version.h>
#include <ntscfg_config.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntccfg {

// The build platform name.
#define NTC_BUILD_UNAME "@NTF_BUILD_UNAME@"

// The build configuration flags.
#define NTC_BUILD_UFID "@NTF_BUILD_UFID@"

// The build branch.
#define NTC_BUILD_BRANCH "@NTC_BUILD_BRANCH@"

// The build commit hash.
#define NTC_BUILD_COMMIT_HASH "@NTC_BUILD_COMMIT_HASH@"

// The build commit hash, abbreviated.
#define NTC_BUILD_COMMIT_HASH_ABBREV "@NTC_BUILD_COMMIT_HASH_ABBREV@"

// Build with BSL.
#define NTC_BUILD_WITH_BSL @NTF_BUILD_WITH_BSL@

// Build with BDL.
#define NTC_BUILD_WITH_BDL @NTF_BUILD_WITH_BDL@

// Build with BAL.
#define NTC_BUILD_WITH_BAL @NTF_BUILD_WITH_BAL@

// Build with support for the Internet Protocol version 4 address family.
#define NTCCFG_BUILD_WITH_ADDRESS_FAMILY_IPV4 @NTF_BUILD_WITH_ADDRESS_FAMILY_IPV4@

// Build with support for the Internet Protocol version 6 address family.
#define NTCCFG_BUILD_WITH_ADDRESS_FAMILY_IPV6 @NTF_BUILD_WITH_ADDRESS_FAMILY_IPV6@

// Build with support for the local (a.k.a. Unix) address family.
#define NTCCFG_BUILD_WITH_ADDRESS_FAMILY_LOCAL @NTF_BUILD_WITH_ADDRESS_FAMILY_LOCAL@

// Build with support for the Transmission Control Protocol (TCP) transport
// protocol.
#define NTCCFG_BUILD_WITH_TRANSPORT_PROTOCOL_TCP @NTF_BUILD_WITH_TRANSPORT_PROTOCOL_TCP@

// Build with support for the User Datagram Protocol (UDP) transport protocol.
#define NTCCFG_BUILD_WITH_TRANSPORT_PROTOCOL_UDP @NTF_BUILD_WITH_TRANSPORT_PROTOCOL_UDP@

// Build with support for the local (a.k.a. Unix) transport protocol.
#define NTCCFG_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL @NTF_BUILD_WITH_TRANSPORT_PROTOCOL_LOCAL@

// Build with support for being able to configure 'select' as the driver for
// a reactor. This driver is available on all platforms.
#define NTC_BUILD_WITH_SELECT @NTF_BUILD_WITH_SELECT@

// Build with support for being able to configure 'poll' as the driver for
// a reactor. This driver is available on all platforms.
#define NTC_BUILD_WITH_POLL @NTF_BUILD_WITH_POLL@

// Build with support for being able to configure 'epoll' as the driver for
// a reactor. This driver is available on Linux.
#define NTC_BUILD_WITH_EPOLL @NTF_BUILD_WITH_EPOLL@

// Build with support for being able to configure the '/dev/poll' device as
// the driver for a reactor. This driver is avalable on Solaris.
#define NTC_BUILD_WITH_DEVPOLL @NTF_BUILD_WITH_DEVPOLL@

// Build with support for being able to configure the event port API as the
// driver for a reactor. This driver is available on Solaris.
#define NTC_BUILD_WITH_EVENTPORT @NTF_BUILD_WITH_EVENTPORT@

// Build with support for being able to configure 'pollset' as the driver for
// a reactor. This driver is available on AIX.
#define NTC_BUILD_WITH_POLLSET @NTF_BUILD_WITH_POLLSET@

// Build with support for being able to configure 'kqueue' as the driver for
// a reactor. This driver is available on Darwin and FreeBSD.
#define NTC_BUILD_WITH_KQUEUE @NTF_BUILD_WITH_KQUEUE@

// Build with support for being able to configure I/O completion ports as the
// driver for a proactor. This driver is available on Windows.
#define NTC_BUILD_WITH_IOCP @NTF_BUILD_WITH_IOCP@

// Build with support for being able to configure 'io_uring' as the
// driver for a proactor. This driver is available on Linux.
#define NTC_BUILD_WITH_IORING @NTF_BUILD_WITH_IORING@

// Build with support for being able to configure a processing model where
// any thread can process I/O for a socket, rather than just a particular
// thread chosen at the time the socket was created.
#define NTC_BUILD_WITH_DYNAMIC_LOAD_BALANCING @NTF_BUILD_WITH_DYNAMIC_LOAD_BALANCING@

// Build with support for automatically scaling I/O threads from a configured
// minimum to a configured maximum.
#define NTC_BUILD_WITH_THREAD_SCALING @NTF_BUILD_WITH_THREAD_SCALING@

// Build with support for deprecated features.
#define NTC_BUILD_WITH_DEPRECATED_FEATURES @NTF_BUILD_WITH_DEPRECATED_FEATURES@

// Build with logging.
#define NTC_BUILD_WITH_LOGGING @NTF_BUILD_WITH_LOGGING@

// Build with metrics.
#define NTC_BUILD_WITH_METRICS @NTF_BUILD_WITH_METRICS@

// Build with inlining suggested.
#define NTC_BUILD_WITH_INLINING_SUGGESTED @NTF_BUILD_WITH_INLINING_SUGGESTED@

// Build with inlining forced.
#define NTC_BUILD_WITH_INLINING_FORCED @NTF_BUILD_WITH_INLINING_FORCED@

// Build with inlining disabled.
#define NTC_BUILD_WITH_INLINING_DISABLED @NTF_BUILD_WITH_INLINING_DISABLED@

// Build with branch prediction.
#define NTC_BUILD_WITH_BRANCH_PREDICTION @NTF_BUILD_WITH_BRANCH_PREDICTION@

// Build with mutually-exclusive locks implemented with spin locks.
#define NTC_BUILD_WITH_SPIN_LOCKS @NTF_BUILD_WITH_SPIN_LOCKS@

// Build with mutually-exclusive locks implemented with userspace mutexes.
#define NTC_BUILD_WITH_USERSPACE_MUTEXES @NTF_BUILD_WITH_USERSPACE_MUTEXES@

// Build with mutually-exclusive locks implemented with system mutexes.
#define NTC_BUILD_WITH_SYSTEM_MUTEXES @NTF_BUILD_WITH_SYSTEM_MUTEXES@

// Build with mutually-exclusive locks implemented with recursive mutexes.
#define NTC_BUILD_WITH_RECURSIVE_MUTEXES @NTF_BUILD_WITH_RECURSIVE_MUTEXES@

// Build with a chronology that dumps stack traces upon function and timer leaks.
#define NTC_BUILD_WITH_STACK_TRACE_LEAK_REPORT @NTF_BUILD_WITH_STACK_TRACE_LEAK_REPORT@

// Build with a test allocator that dumps stack traces upon memory leaks.
#define NTC_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR @NTF_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR@

// Build with compiler warnings enabled.
#define NTC_BUILD_WITH_WARNINGS @NTF_BUILD_WITH_WARNINGS@

// Build with compiler instrumentation for code coverage.
#define NTC_BUILD_WITH_COVERAGE @NTF_BUILD_WITH_COVERAGE@

// Build from a packaging environment.
#define NTC_BUILD_FROM_PACKAGING @NTF_BUILD_FROM_PACKAGING@

// Build from a continuous integration environment.
#define NTC_BUILD_FROM_CONTINUOUS_INTEGRATION @NTF_BUILD_FROM_CONTINUOUS_INTEGRATION@

}  // close package namespace
}  // close enterprise namespace
#endif
