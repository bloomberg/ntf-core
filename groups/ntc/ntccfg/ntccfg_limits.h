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

#ifndef INCLUDED_NTCCFG_LIMITS
#define INCLUDED_NTCCFG_LIMITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntcscm_version.h>
#include <ntscfg_limits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntccfg {

/// The default size of each blob buffer used to store incoming data. The
/// default value is 128K.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_INCOMING_BLOB_BUFFER_SIZE 131072

/// The default size of each blob buffer used to store outgoing data. The
/// default value is 128K.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_OUTGOING_BLOB_BUFFER_SIZE 131072

/// The default accept queue low watermark limit for a stream socket, in
/// connections. The default value is 1.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_QUEUE_LOW_WATERMARK 1

/// The default accept queue high watermark limit for a stream socket, in
/// connections. The default value is 1024.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_QUEUE_HIGH_WATERMARK 1024

/// The default greediness of a listener socket when accepting. The default
/// value is false, indicating accepts are not performed greedily.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_LISTENER_SOCKET_ACCEPT_GREEDILY false

/// The default write queue low watermark limit for a stream socket, in bytes.
/// The default value is zero.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_WRITE_QUEUE_LOW_WATERMARK 0

/// The default write queue high watermark limit for a stream socket, in bytes.
/// The default value is 64M.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_WRITE_QUEUE_HIGH_WATERMARK 67108864

/// The default greediness of a stream socket when writing. The default value
/// is false, indicating writes are not performed greedily.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_WRITE_GREEDILY false

/// The default read queue low watermark limit for a stream socket, in bytes.
/// The default value is 1.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_READ_QUEUE_LOW_WATERMARK 1

/// The default read queue high watermark limit for a stream socket, in bytes.
/// The default value is 512M.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_READ_QUEUE_HIGH_WATERMARK 536870912

/// The default greediness of a stream socket when reading. The default value
/// is false, indicating reads are not performed greedily.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_READ_GREEDILY false

/// The default behavior to keep a stream socket half-open. The default value
/// is false, indicating stream sockets are not kept half-open; the stream is
/// automatically fully shutdown and closed when either the local or remote
/// side of the connection shuts down either reading or writing.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_KEEP_HALF_OPEN false

/// The default minimum number of bytes attempted to be read from a stream
/// socket each time a read operation is performed. The default value is 4K.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_MIN_INCOMING_TRANSFER_SIZE 4096

/// The default maximum number of bytes attempted to be read from a stream
/// socket each time a read operation is performed. The default value is 512K.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STREAM_SOCKET_MAX_INCOMING_TRANSFER_SIZE 524288

/// The default write queue low watermark limit for a datagram socket, in
/// bytes. The default value is 0.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DATAGRAM_SOCKET_WRITE_QUEUE_LOW_WATERMARK 0

/// The default write queue high watermark limit for a datagram socket, in
/// bytes. The default value is 64M.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DATAGRAM_SOCKET_WRITE_QUEUE_HIGH_WATERMARK 67108864

/// The default greediness of a datagram socket when writing. The default value
/// is false, indicating writes are not performed greedily.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DATAGRAM_SOCKET_WRITE_GREEDILY false

/// The default read queue low watermark limit for a datagram socket, in bytes.
/// The default value is 1.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DATAGRAM_SOCKET_READ_QUEUE_LOW_WATERMARK 1

/// The default read queue high watermark limit for a datagram socket, in bytes.
/// The default value is 512M.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DATAGRAM_SOCKET_READ_QUEUE_HIGH_WATERMARK 536870912

/// The default greediness of a stream socket when reading. The default value
/// is false, indicating reads are not performed greedily.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DATAGRAM_SOCKET_READ_GREEDILY false

/// The default behavior to keep a stream socket half-open. The default value
/// is false, indicating datagram sockets are not kept half-open; the stream is
/// automatically fully shutdown and closed when either the local or remote
/// side of the connection shuts down either reading or writing.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DATAGRAM_SOCKET_KEEP_HALF_OPEN false

/// The maximum size of a datagram socket message, unless specified by the user.
/// In the absense of other information, assume the UDP protocol. The default
/// value is 65507, which is 64K - the size of the UDP header.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DATAGRAM_SOCKET_MAX_MESSAGE_SIZE 65507

/// The default recommended threshold of data at which to begin to attempt
/// zero-copy transmission.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_ZERO_COPY_THRESHOLD 10240

#if NTC_BUILD_WITH_DYNAMIC_LOAD_BALANCING
/// The default maximum number of threads supported by this library. The
/// default value is 128.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_MAX_THREADS 128
#else
/// The default maximum number of threads supported by this library. The
/// default value is 1.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_MAX_THREADS 1
#endif

/// The default stack size for each thread. The default value is 4MB.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_STACK_SIZE 4194304

/// The default maximum number of sockets assigned to each thread before a
/// thread pool automatically expands by adding another thread, if so
/// configured. The default value is 250.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_MAX_DESIRED_SOCKETS_PER_THREAD 250

/// The default maximum number of events to discover each time the polling
/// mechanism is polled. The default value is 128.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_MAX_EVENTS_PER_WAIT 128

/// The default maximum number of timers to discover that are due after each
/// time the polling mechanism is polled. A value of zero indicates the maximum
/// number of timers is unlimited. The default value is zero.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_MAX_TIMERS_PER_WAIT 0

/// The maximum number of cycles to perform to both discover if any functions
/// have had their execution deferred to be invoked on the I/O thread and to
/// discover any timers that are due. The default value is 1.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_MAX_CYCLES_PER_WAIT 1

/// The default desire to perform dynamic load balancing, unless otherwise
/// specified. The default value is false, indicating that, by default, sockets
/// are statically load-balanced onto I/O threads.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DYNAMIC_LOAD_BALANCING false

/// The default behavior to collect driver metrics. The default value is true.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DRIVER_METRICS true

/// The default behavior to collect driver metrics per waiter (i.e. thread).
/// The default value is false.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_DRIVER_METRICS_PER_WAITER false

/// The default behavior to collect socket metrics. The default value is true.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_SOCKET_METRICS true

/// The default behavior to collect socket metrics per handle. The default
/// value is false.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_SOCKET_METRICS_PER_HANDLE false

/// The default number of scatter/gather buffers stored in a buffer array
/// arena. The default value is 64.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_MAX_INPLACE_BUFFERS NTSCFG_DEFAULT_MAX_INPLACE_BUFFERS

/// The default flag that indicates an interface or thread should run an
/// asynchronous resolver. The default value is true.
///
/// @ingroup module_ntccfg
#define NTCCFG_DEFAULT_RESOLVER_ENABLED true

}  // close package namespace
}  // close enterprise namespace
#endif
