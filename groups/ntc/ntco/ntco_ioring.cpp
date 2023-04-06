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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ntco_ioring.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntco_ioring_cpp, "$Id$ $CSID$")

#if NTC_BUILD_WITH_IORING
#if defined(BSLS_PLATFORM_OS_LINUX)

#include <ntcp_datagramsocket.h>
#include <ntcp_listenersocket.h>
#include <ntcp_streamsocket.h>

#include <ntcm_monitorableregistry.h>
#include <ntcm_monitorableutil.h>

#include <ntci_log.h>
#include <ntcs_async.h>
#include <ntcs_authorization.h>
#include <ntcs_chronology.h>
#include <ntcs_datapool.h>
#include <ntcs_driver.h>
#include <ntcs_event.h>
#include <ntcs_nomenclature.h>
#include <ntcs_proactormetrics.h>
#include <ntcs_registry.h>
#include <ntcs_reservation.h>
#include <ntcs_strand.h>
#include <ntcs_user.h>
#include <ntsf_system.h>
#include <ntsu_bufferutil.h>
#include <ntsu_socketoptionutil.h>
#include <ntsu_socketutil.h>
#include <ntci_mutex.h>

#include <bdlbb_blob.h>
#include <bdlcc_objectpool.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bdlt_localtimeoffset.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_byteorder.h>
#include <bsls_keyword.h>
#include <bsls_spinlock.h>
#include <bsls_timeutil.h>

#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <linux/io_uring.h>

#include <errno.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/time_types.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>

// Initialize I/O rings with IORING_SETUP_SQE128 and cause all submission
// queue entries to have an extra 80-bytes for extra data.
#define NTCO_IORING_SUBMISSION_128 0

// Initialize I/O rings with IORING_SETUP_CQE32 and cause all completion
// queue entries to have an extra 16-bytes for extra data.
#define NTCO_IORING_COMPLETION_32 0



// MRM 
// Set during configuration if liburing is available on the build machine.
#define NTC_BUILD_WITH_IORING_LIBURING 0

// MRM
// Comment or set to zero to disable optimizations in the implementation that
// try to improve the efficiency of interacting with the I/O ring.
#define NTC_BUILD_WITH_IORING_LIBURING_OPTIMIZATIONS 1

#if NTC_BUILD_WITH_IORING_LIBURING
#include <liburing.h>
#endif

#define NTCO_IORING_READER_BARRIER() __asm__ __volatile__("" ::: "memory")
#define NTCO_IORING_WRITER_BARRIER() __asm__ __volatile__("" ::: "memory")

// #define NTCO_IORING_SQE_FLAGS IOSQE_ASYNC
#define NTCO_IORING_SQE_FLAGS 0

// Support the cancellation of pending requests. To implement this properly it
// must be determined how to use IORING_OP_ASYNC_CANCEL to identify the request
// that is to be cancelled, and ntci::ProactorSocket::cancel must be made to be
// asynchronous, since typically the IoRingContext object is deregistered as the
// proactor stream socket context before this implementation asynchronously
// learns that an operation has been cancelled.
#define NTCO_IORING_CANCELLATION 1

// Enable logging during debugging.
#define NTCO_IORING_DEBUG 0

#define NTCO_IORING_LOG_WAIT_INDEFINITE()                                     \
    NTCI_LOG_TRACE("Polling for socket events indefinitely")

#define NTCO_IORING_LOG_WAIT_TIMED(timeout)                                   \
    NTCI_LOG_TRACE(                                                           \
        "Polling for sockets events or until %d milliseconds have elapsed",   \
        timeout)

#define NTCO_IORING_LOG_WAIT_TIMED_HIGH_PRECISION(timeInterval)               \
    do {                                                                      \
        bdlt::Datetime dateTimeDue =                                          \
            bdlt::EpochUtil::convertFromTimeInterval(timeInterval);           \
        dateTimeDue.addSeconds(                                               \
            bdlt::LocalTimeOffset::localTimeOffset(bdlt::CurrentTime::utc())  \
                .totalSeconds());                                             \
        char buffer[128];                                                     \
        dateTimeDue.printToBuffer(buffer, sizeof buffer);                     \
        NTCI_LOG_TRACE("Polling for sockets events or until %s", buffer);     \
    } while (false)

#define NTCO_IORING_LOG_WAIT_FAILURE(error)                                   \
    NTCI_LOG_ERROR("Failed to poll for socket events: %s",                    \
                   error.text().c_str())

#define NTCO_IORING_LOG_WAIT_TIMEOUT()                                        \
    NTCI_LOG_TRACE("Timed out polling for socket events")

#define NTCO_IORING_LOG_WAIT_RESULT(numEvents)                                \
    NTCI_LOG_TRACE("Polled %zu socket events", (bsl::size_t)(numEvents))

#define NTCO_IORING_LOG_EVENT_STATUS(event, status)                           \
    do {                                                                      \
        if (event->d_type == ntcs::EventType::e_CALLBACK) {                   \
            NTCI_LOG_TRACE("I/O ring event %p type %s %s",                    \
                           event.get(),                                       \
                           ntcs::EventType::toString(event->d_type),          \
                           status);                                           \
        }                                                                     \
        else {                                                                \
            if (event->d_error) {                                             \
                NTCI_LOG_TRACE(                                               \
                    "I/O ring event %p type %s for socket %d %s: %s",         \
                    event.get(),                                              \
                    ntcs::EventType::toString(event->d_type),                 \
                    (int)(event->d_socket ? event->d_socket->handle() : -1),  \
                    status,                                                   \
                    event->d_error.text().c_str());                           \
            }                                                                 \
            else {                                                            \
                NTCI_LOG_TRACE(                                               \
                    "I/O ring event %p type %s for socket %d %s",             \
                    event.get(),                                              \
                    ntcs::EventType::toString(event->d_type),                 \
                    (int)(event->d_socket ? event->d_socket->handle() : -1),  \
                    status);                                                  \
            }                                                                 \
        }                                                                     \
    } while (false)

#define NTCO_IORING_LOG_EVENT_STARTING(event)                                 \
    NTCO_IORING_LOG_EVENT_STATUS(event, "starting")

#define NTCO_IORING_LOG_EVENT_COMPLETE(event)                                 \
    NTCO_IORING_LOG_EVENT_STATUS(event, "complete")

#define NTCO_IORING_LOG_EVENT_CANCELLED(event)                                \
    NTCO_IORING_LOG_EVENT_STATUS(event, "cancelled")

#define NTCO_IORING_LOG_EVENT_ABANDONED(event)                                \
    NTCO_IORING_LOG_EVENT_STATUS(event, "abandoned")

#define NTCO_IORING_LOG_EVENT_IGNORED(event)                                  \
    NTCO_IORING_LOG_EVENT_STATUS(event, "ignored")

#define NTCO_IORING_LOG_EVENT_REFUSED(event)                                  \
    NTCO_IORING_LOG_EVENT_STATUS(event, "refused")

namespace BloombergLP {
namespace ntco {

// Describe the context of a waiter.
struct IoRingWaiter {
  public:
    ntca::WaiterOptions                    d_options;
    bsl::shared_ptr<ntci::ProactorMetrics> d_metrics_sp;
    struct __kernel_timespec               d_ts;

  private:
    IoRingWaiter(const IoRingWaiter&) BSLS_KEYWORD_DELETED;
    IoRingWaiter& operator=(const IoRingWaiter&) BSLS_KEYWORD_DELETED;

  public:
    // Create a new proactor result. Optionally specify a 'basicAllocator'
    // used to supply memory. If 'basicAllocator' is 0, the currently
    // installed default allocator is used.
    explicit IoRingWaiter(bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~IoRingWaiter();
};

/// Describe the configurable parameters of an I/O ring.
class IoRingConfig
{
    bsl::uint32_t d_submissionQueueCapacity;
    bsl::uint32_t d_completionQueueCapacity;
    bsl::uint32_t d_flags;
    bsl::uint32_t d_submissionQueueThreadCpu;
    bsl::uint32_t d_submissionQueueThreadIdle;
    bsl::uint32_t d_features;
    bsl::uint32_t d_wq;
    bsl::uint32_t d_reserved[3];

    bsl::uint32_t d_submissionQueueOffsetToHead;
	bsl::uint32_t d_submissionQueueOffsetToTail;
	bsl::uint32_t d_submissionQueueOffsetToRingMask;
	bsl::uint32_t d_submissionQueueOffsetToRingEntries;
	bsl::uint32_t d_submissionQueueOffsetToFlags;
	bsl::uint32_t d_submissionQueueOffsetToDropped;
	bsl::uint32_t d_submissionQueueOffsetToArray;
	bsl::uint32_t d_submissionQueueOffsetToResv1;
	bsl::uint64_t d_submissionQueueOffsetToResv2;

    bsl::uint32_t d_completionQueueOffsetToHead;
	bsl::uint32_t d_completionQueueOffsetToTail;
	bsl::uint32_t d_completionQueueOffsetToRingMask;
	bsl::uint32_t d_completionQueueOffsetToRingEntries;
	bsl::uint32_t d_completionQueueOffsetToOverflow;
	bsl::uint32_t d_completionQueueOffsetToCQEs;
	bsl::uint32_t d_completionQueueOffsetToFlags;
	bsl::uint32_t d_completionQueueOffsetToResv1;
	bsl::uint64_t d_completionQueueOffsetToResv2;

  public:
    /// Create a new I/O ring configuration having a default value.
    IoRingConfig();

    /// Create a new I/O ring configuration having the same value as the
    /// specified 'original' object.
    IoRingConfig(const IoRingConfig& original);

    /// Destroy this object.
    ~IoRingConfig();

    /// Assign the value of the specified 'object' to this object. Return a 
    /// reference to this modifiable object.
    IoRingConfig& operator=(const IoRingConfig& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the submission queue capacity to the specified 'value'.
    void setSubmissionQueueCapacity(bsl::uint32_t value);

    /// Set the completion queue capacity to the specified 'value'.
    void setCompletionQueueCapacity(bsl::uint32_t value);

    /// Set the flags to the specified 'value'.
    void setFlags(bsl::uint32_t value);

    /// Set the features the specified 'value'.
    void setFeatures(bsl::uint32_t value);

    /// Return the submission queue capacity.
    bsl::uint32_t submissionQueueCapacity() const;

    /// Return the offset from the base of the mapped memory for the 
    /// submission queue to the head index.
    bsl::uint32_t submissionQueueOffsetToHead() const;

    /// Return the offset from the base of the mapped memory for the 
    /// submission queue to the tail index.
    bsl::uint32_t submissionQueueOffsetToTail() const;

    /// Return the offset from the base of the mapped memory for the 
    /// submission queue to the ring mask.
    bsl::uint32_t submissionQueueOffsetToRingMask() const;

    /// Return the offset from the base of the mapped memory for the 
    /// submission queue to the ring entries.
    bsl::uint32_t submissionQueueOffsetToRingEntries() const;

    /// Return the offset from the base of the mapped memory for the 
    /// submission queue to the ring flags.
    bsl::uint32_t submissionQueueOffsetToFlags() const;

    /// Return the offset from the base of the mapped memory for the 
    /// submission queue to the array index.
    bsl::uint32_t submissionQueueOffsetToArray() const;

    /// Return the completion queue capacity.
    bsl::uint32_t completionQueueCapacity() const;

    /// Return the offset from the base of the mapped memory for the 
    /// completion queue to the head index.
    bsl::uint32_t completionQueueOffsetToHead() const;

    /// Return the offset from the base of the mapped memory for the 
    /// completion queue to the tail index.
    bsl::uint32_t completionQueueOffsetToTail() const;

    /// Return the offset from the base of the mapped memory for the 
    /// completion queue to the ring mask.
    bsl::uint32_t completionQueueOffsetToRingMask() const;

    /// Return the offset from the base of the mapped memory for the 
    /// completion queue to the ring entries.
    bsl::uint32_t completionQueueOffsetToRingEntries() const;

    /// Return the offset from the base of the mapped memory for the 
    /// completion queue to the ring flags.
    bsl::uint32_t completionQueueOffsetToFlags() const;

    /// Return the offset from the base of the mapped memory for the 
    /// completion queue to the completion entries.
    bsl::uint32_t completionQueueOffsetToCQEs() const;

    /// Return the flags.
    bsl::uint32_t flags() const;

    /// Return the features.
    bsl::uint32_t features() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(IoRingConfig);
};

/// Write a formatted, human readable description of the specified 'object' to
/// the specified 'stream'.
bsl::ostream& operator<<(bsl::ostream& stream, const IoRingConfig& object);


/// Describe the capabilities of an I/O ring.
class IoRingCapabilities
{
    bsl::uint32_t d_flags;

  public:
    /// Create a new I/O ring capabilities description having a default value.
    IoRingCapabilities();

    /// Create a new I/O ring capabilities description having the same value as
    /// the specified 'original' object.
    IoRingCapabilities(const IoRingCapabilities& original);

    /// Destroy this object.
    ~IoRingCapabilities();

    /// Assign the value of the specified 'object' to this object. Return a 
    /// reference to this modifiable object.
    IoRingCapabilities& operator=(const IoRingCapabilities& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(IoRingCapabilities);
};

/// Write a formatted, human readable description of the specified 'object' to
/// the specified 'stream'.
bsl::ostream& operator<<(bsl::ostream&             stream, 
                         const IoRingCapabilities& object);

/// Describe an I/O ring submission entry.
///
/// @par Thread Safety
/// This class is not thread safe.
class IoRingSubmission
{
    bsl::uint8_t  d_operation;   // opcode
	bsl::uint8_t  d_flags;       // flags
	bsl::uint16_t d_priority;    // ioprio
	bsl::int32_t  d_handle;      // fd
    bsl::uint64_t d_size;        // off
    bsl::uint64_t d_address;     // addr
    bsl::uint32_t d_count;       // len
    bsl::uint32_t d_options;     // msg_flags etc.
    bsl::uint64_t d_event;       // user_data
    bsl::uint16_t d_index;       // buf_index/d_buf_group
    bsl::uint16_t d_personality; // personality
    bsl::uint32_t d_splice;      // splice_fd_in/d_file_index

#if NTCO_IORING_SUBMISSION_128
	bsl::uint8_t d_command[80];
#else
    bsl::uint8_t d_command[16];
#endif

  public:
    /// Create a new I/O ring submission entry having a default value.
    IoRingSubmission();

    /// Create a new I/O ring submission entry having the same value as the
    /// specified 'original' object.
    IoRingSubmission(const IoRingSubmission& original);

    /// Destroy this object.
    ~IoRingSubmission();

    /// Assign the value of the specified 'object' to this object. Return a 
    /// reference to this modifiable object.
    IoRingSubmission& operator=(const IoRingSubmission& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Prepare the submission to initiate a timeout at the specified 
    /// 'deadline', in absolute time since the Unix epoch.
    void prepareTimeout(struct __kernel_timespec* timespec,
                        const bsls::TimeInterval& deadline);

    /// Prepare the submission to initiate a callback, i.e. a "no-op"
    /// completion that invokes a callback.
    void prepareCallback(ntcs::Event*                event, 
                         const ntcs::Event::Functor& callback);

    /// Prepare the submission to initiate an operation to accept the next 
    /// connection from the backlog of the specified 'socket' identified by the
    /// specified 'handle'. Load into the specified 'event' the event that 
    /// indicates the operation is complete. Return the error.
    ntsa::Error prepareAccept(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle);

    /// Prepare the submission to initiate an operation to connect the 
    /// specified 'socket' identified by the specified 'handle' to the 
    /// specified 'endpoint'. Load into the specified 'event' the event that 
    /// indicates the operation is complete. Return the error.
    ntsa::Error prepareConnect(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::Endpoint&                        endpoint);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::Data&                            source,
        const ntsa::SendOptions&                     options);
    
    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const bdlbb::Blob&                           source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const bdlbb::BlobBuffer&                     source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::ConstBuffer&                     source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::ConstBufferArray&                source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::ConstBufferPtrArray&             source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::MutableBuffer&                   source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::MutableBufferArray&              source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::MutableBufferPtrArray&           source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::File&                            source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to enqueue the 
    /// specified 'source' to the send buffer of the specified 'socket' 
    /// identified by the specified 'handle' according to the specified 
    /// 'options'. Load into the specified 'event' the event that indicates the
    /// operation is complete. Return the error.
    ntsa::Error prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const bsl::string&                           source,
        const ntsa::SendOptions&                     options);

    /// Prepare the submission to initiate an operation to dequeue the receive
    /// buffer of the specified 'socket' identified by the specified 'handle'
    /// into the specified 'destination' according to the specified 'options'.
    /// Load into the specified 'event' the event that indicates the operation
    /// is complete. Return the error.
    ntsa::Error prepareReceive(
            ntcs::Event*                                 event, 
            const bsl::shared_ptr<ntci::ProactorSocket>& socket,
            ntsa::Handle                                 handle,
            bdlbb::Blob*                                 destination,
            const ntsa::ReceiveOptions&                  options);

    /// Prepare the submission to cancel an operation.
    void prepareCancellation(ntcs::Event* event);

    /// Return the handle. 
    ntsa::Handle handle() const;

    /// Return the event.
    ntcs::Event* event() const;

    // Return the operation code.
    bsl::uint8_t opcode() const;

    /// Return the flags.
    bsl::uint8_t flags() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(IoRingSubmission);
};

/// Write a formatted, human readable description of the specified 'object' to
/// the specified 'stream'.
bsl::ostream& operator<<(bsl::ostream& stream, const IoRingSubmission& object);

/// Provide a list of I/O uring submission queue entries waiting to the
/// submitted to an I/O uring.
///
/// @par Thread Safety
/// This class is thread safe.
class IoRingSubmissionList
{
    // Define a type alias for a sequence of I/O uring submission queue entries.
    typedef bsl::vector<ntco::IoRingSubmission> EntrySequence;

    // Define a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    // Define a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    mutable Mutex        d_mutex;
    EntrySequence        d_data;
    bslma::Allocator*    d_allocator_p;

  private:
    IoRingSubmissionList(const IoRingSubmissionList&) BSLS_KEYWORD_DELETED;
    IoRingSubmissionList& operator=(const IoRingSubmissionList&)
        BSLS_KEYWORD_DELETED;

  public:
    // Create a new submission list. Optionally specify a 'basicAllocator'
    // used to supply memory. If 'basicAllocator' is 0, the currently
    // installed default allocator is used.
    explicit IoRingSubmissionList(bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~IoRingSubmissionList();

    // Push the specified 'entry' onto the submission queue. Return the
    // error.
    ntsa::Error push(const IoRingSubmission& entry);

    // Load into the specified 'result' the first entry in the submission
    // list. Return the error.
    ntsa::Error pop(IoRingSubmission* result);

    // Return the number of entries in the submission list.
    bsl::size_t size() const;

    // Return true if there are no entries in the submission list, and
    // false otherwise.
    bool empty() const;
};

#if NTC_BUILD_WITH_IORING_LIBURING == 0

/// Provide a memory mapped submission queue of an I/O ring.
///
/// @par Thread Safety
/// This class is thread safe.
class IoRingSubmissionQueue
{
    // Define a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    // Define a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    mutable Mutex        d_mutex;
    int                  d_ring;
    void*                d_memoryMap_p;
    bsl::uint32_t*       d_head_p;
    bsl::uint32_t*       d_tail_p;
    bsl::uint32_t*       d_mask_p;
    bsl::uint32_t*       d_ring_entries_p;
    bsl::uint32_t*       d_flags_p;
    bsl::uint32_t*       d_array_p;
    ntco::IoRingSubmission* d_entryArray;
    ntco::IoRingConfig    d_params;

  private:
    IoRingSubmissionQueue(const IoRingSubmissionQueue&) BSLS_KEYWORD_DELETED;
    IoRingSubmissionQueue& operator=(const IoRingSubmissionQueue&)
        BSLS_KEYWORD_DELETED;

  public:
    // Create a new, initially unmapped submission queue.
    IoRingSubmissionQueue();

    // Destroy this object.
    ~IoRingSubmissionQueue();

    // Map the memory for the submission queue for the specified I/O
    // 'ring' having the specified 'parameters'.
    ntsa::Error map(int ring, const ntco::IoRingConfig& parameters);

    // Push the specified 'entry' onto the submission queue. Return the
    // error.
    ntsa::Error push(const ntco::IoRingSubmission& entry);

    // Unmap the memory for the submission queue.
    void unmap();

    // Return the index of the head entry in the submission queue.
    bsl::uint32_t headIndex() const;

    // Return the index of the tail entry in the submission queue.
    bsl::uint32_t tailIndex() const;
};

/// Describe an I/O ring completion entry.
///
/// @par Thread Safety
/// This class is not thread safe.
class IoRingCompletion
{
    bsl::uint64_t d_userData;
    bsl::int32_t  d_result;
    bsl::uint32_t d_flags;

#if NTCO_IORING_COMPLETION_32
    union {
        bsl::uint64_t d_extraAsQwords[2];
        bsl::uint8_t  d_extraAsBytes[16];
    };
#endif

  public:
    /// Create a new I/O ring completion entry having a default value.
    IoRingCompletion();

    /// Create a new I/O ring completion entry having the same value as the
    /// specified 'original' object.
    IoRingCompletion(const IoRingCompletion& original);

    /// Destroy this object.
    ~IoRingCompletion();

    /// Assign the value of the specified 'object' to this object. Return a 
    /// reference to this modifiable object.
    IoRingCompletion& operator=(const IoRingCompletion& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Return the event.
    ntcs::Event* event() const;

    /// Return the number of bytes copied during the operation. 
    bsl::size_t result() const;

    /// Return the error.
    ntsa::Error error() const;

    /// Return the flags.
    bsl::uint8_t flags() const;

    /// Return true if the operation has succeeded, otherwise return false.
    bool hasSucceeded() const;

    /// Return true if the operation has failed, otherwise return false.
    bool hasFailed() const;

    /// Return true if the operation failed because it was canceled, otherwise
    /// return false.
    bool wasCanceled() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(IoRingCompletion);
};

/// Write a formatted, human readable description of the specified 'object' to
/// the specified 'stream'.
bsl::ostream& operator<<(bsl::ostream& stream, const IoRingCompletion& object);

/// Provide memory mapped completion queue of an I/O ring.
///
/// @par Thread Safety
/// This class is thread safe.
class IoRingCompletionQueue
{
    // Define a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    // Define a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    mutable Mutex        d_mutex;
    int                  d_ring;
    void*                d_memoryMap_p;
    bsl::uint32_t*       d_head_p;
    bsl::uint32_t*       d_tail_p;
    bsl::uint32_t*       d_mask_p;
    bsl::uint32_t*       d_ring_entries_p;
    ntco::IoRingCompletion*      d_entryArray;
    ntco::IoRingConfig    d_params;

  private:
    IoRingCompletionQueue(const IoRingCompletionQueue&) BSLS_KEYWORD_DELETED;
    IoRingCompletionQueue& operator=(const IoRingCompletionQueue&)
        BSLS_KEYWORD_DELETED;

  public:
    // Create a new, initially unmapped completion queue.
    IoRingCompletionQueue();

    // Destroy this object.
    ~IoRingCompletionQueue();

    // Map the memory for the completion queue for the specified I/O
    // 'ring' having the specified 'parameters'. Return the error.
    ntsa::Error map(int ring, const ntco::IoRingConfig& parameters);

    // Load into the specified 'result' having the specified 'capacity'
    // the next entries from the completion queue. Return the number of
    // entries popped and set in the 'entryList'.
    bsl::size_t pop(ntco::IoRingCompletion* result, bsl::size_t capacity);

    // Unmap the memory for the completion queue.
    void unmap();

    // Return the index of the head entry in the completion queue.
    bsl::uint32_t headIndex() const;

    // Return the index of the tail entry in the completion queue.
    bsl::uint32_t tailIndex() const;
};

/// Provide an I/O ring device.
///
/// @par Thread Safety
/// This class is thread safe.
class IoRingDevice
{
    // Define a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    // Define a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    mutable Mutex  d_mutex;
    int                   d_ring;
    ntco::IoRingSubmissionList  d_submissionList;
    ntco::IoRingSubmissionQueue d_submissionQueue;
    ntco::IoRingCompletionQueue d_completionQueue;
    ntco::IoRingConfig     d_params;
    bslma::Allocator*     d_allocator_p;

  private:
    IoRingDevice(const IoRingDevice&) BSLS_KEYWORD_DELETED;
    IoRingDevice& operator=(const IoRingDevice&) BSLS_KEYWORD_DELETED;

  public:
    // Create a new I/O uring. Optionally specify a 'basicAllocator' used to
    // supply memory. If  'basicAllocator' is 0, the currently installed default
    // allocator is used.
    explicit IoRingDevice(bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~IoRingDevice();

    // Submit the specified 'entry' to the submission queue. Return the error.
    ntsa::Error submit(const ntco::IoRingSubmission& entry);

    // Load into the specified 'entryList' having the specified
    // 'entryListCapacity' the next entries from the completion queue. Block
    // until either an entry has completed, or the specified 'earliestTimerDue'
    // has elapsed, or an error occurs. Return the number of entries popped and
    // set in the 'entryList'.
    bsl::size_t wait(
        ntci::Waiter                                   waiter,
        ntco::IoRingCompletion*                        entryList,
        bsl::size_t                                    entryListCapacity,
        const bdlb::NullableValue<bsls::TimeInterval>& earliestTimerDue);

    // Load into the specified 'entryList' having the specified
    // 'entryListCapacity' the next entries from the completion queue. Return
    // the number of entries popped and set in the 'entryList'.
    bsl::size_t flush(ntco::IoRingCompletion* entryList,
                      bsl::size_t             entryListCapacity);
};

#else

/// Provide an I/O ring device.
///
/// @par Thread Safety
/// This class is thread safe.
class IoRingDevice
{
    bsls::SpinLock    d_sqeLock;
    bsls::SpinLock    d_cqeLock;
    ::io_uring        d_ring;
    ::io_uring_params d_params;
    bslma::Allocator* d_allocator_p;

  private:
    IoRingDevice(const IoRingDevice&) BSLS_KEYWORD_DELETED;
    IoRingDevice& operator=(const IoRingDevice&) BSLS_KEYWORD_DELETED;

  public:
    // Create a new I/O uring. Optionally specify a 'basicAllocator' used to
    // supply memory. If  'basicAllocator' is 0, the currently installed default
    // allocator is used.
    explicit IoRingDevice(bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~IoRingDevice();

    // Submit the specified 'entry' to the submission queue. Return the error.
    ntsa::Error submit(const ntco::IoRingSubmission& entry);

    // Load into the specified 'entryList' having the specified
    // 'entryListCapacity' the next entries from the completion queue. Block
    // until either an entry has completed, or the specified 'earliestTimerDue'
    // has elapsed, or an error occurs. Return the number of entries popped and
    // set in the 'entryList'.
    bsl::size_t wait(
        ntci::Waiter                                   waiter,
        ntco::IoRingCompletion*                        entryList,
        bsl::size_t                                    entryListCapacity,
        const bdlb::NullableValue<bsls::TimeInterval>& earliestTimerDue);

    //  Load into the specified 'entryList' having the specified
    // 'entryListCapacity' the next entries from the completion queue. Return
    // the number of entries popped and set in the 'entryList'.
    bsl::size_t flush(ntco::IoRingCompletion* entryList,
                      bsl::size_t             entryListCapacity);
};

#endif

/// Describe the context of a proactor socket managed by a I/O ring.
///
/// @par Thread Safety
/// This class is thread safe.
class IoRingContext
{
    // Define a set of events.
    typedef bsl::unordered_set<ntcs::Event*> EventSet;

    // Define a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    // Define a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    ntsa::Handle d_handle;
#if NTCO_IORING_CANCELLATION
    Mutex    d_pendingEventSetMutex;
    EventSet d_pendingEventSet;
#endif
    bslma::Allocator* d_allocator_p;

  private:
    IoRingContext(const IoRingContext&) BSLS_KEYWORD_DELETED;
    IoRingContext& operator=(const IoRingContext&) BSLS_KEYWORD_DELETED;

  public:
    // Define a type alias for a vector of events.
    typedef bsl::vector<ntcs::Event*> EventList;

    // Create a new context for the specified 'handle'. Optionally specify
    // a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    // the currently installed default allocator is used.
    explicit IoRingContext(ntsa::Handle      handle,
                           bslma::Allocator* basicAllocator = 0);
    // Destroy this object.
    ~IoRingContext();

    // Register the specified 'event' that will start for the socket
    // managed by this context. Return the error.
    ntsa::Error registerEvent(ntcs::Event* event);

    // Complete the specified 'event' for the socket managed by this
    // context. Return the error.
    void completeEvent(ntcs::Event* event);

    // Load all pending events into the specified 'pendingEventList'. If
    // the specified 'remove' flag is true, also remove all pending
    // events.
    void loadPending(EventList* pendingEventList, bool remove);

    // Return the handle.
    ntsa::Handle handle() const;
};

/// Provide utiltities for implementing I/O ring drivers.
///
/// @par Thread Safety
/// This struct is thread safe.
struct IoRingUtil {
    // Enumerate the Linux kernel system calls used by this implementation.
    enum SystemCall {
        // Create and configure an I/O ring.
        k_SYSTEM_CALL_SETUP = 425,

        // Enter an I/O ring.
        k_SYSTEM_CALL_ENTER = 426,

        // Register resources for an I/O ring.
        k_SYSTEM_CALL_REGISTER = 427
    };

    // Enumerate the flags that may be specified when performing a system call.
    enum SystemCallFlags {
        // Block until the minimum specified number of completetions are
        // available.
        k_SYSTEM_CALL_ENTER_GET_EVENTS = (1 << 0)
    };

    // Return the string description of the specified 'opcode'.
    static const char* describeOpCode(int opcode);

    // Create a new I/O ring configured with the specified 'parameters'
    // containing the specified number of 'entries' in each queue. Return the
    // file descriptor of the new I/O ring.
    static int setup(unsigned int entries, ntco::IoRingConfig* parameters);

    // Enter the specified 'ring', initiate the specified number of
    // 'submission', and wait for the specified number of 'completions'. If the
    // specified 'signals' mask is not null, when waiting for completions first
    // replace current the signal mask with the 'signals' then restore the
    // current signal mask when done. Return 0 on success and a non-zero value
    // otherwise.
    static int enter(int          ring,
                     unsigned int submissions,
                     unsigned int completions,
                     unsigned int flags,
                     sigset_t*    signals);

    // Perform the specified control 'operation' on the specified 'ring' using
    // the specified 'count' number of the specified 'operand' array. Return
    // 0 on success and a non-zero value otherwise.
    static int control(int          ring,
                       unsigned int operation,
                       void*        operand,
                       unsigned int count);

    // Return true if the runtime properties of the current operating system
    // support proactors produced by this factory, otherwise return false.
    static bool isSupported();
};

IoRingWaiter::IoRingWaiter(bslma::Allocator* basicAllocator)
: d_options(basicAllocator)
, d_metrics_sp()
, d_ts()
{
}

IoRingWaiter::~IoRingWaiter()
{
}

IoRingConfig::IoRingConfig()
{
    bsl::memset(this, 0, sizeof(ntco::IoRingConfig));
}

IoRingConfig::IoRingConfig(const IoRingConfig& original)
{
    bsl::memcpy(this, &original, sizeof(ntco::IoRingConfig));
}

IoRingConfig::~IoRingConfig()
{
}

IoRingConfig& IoRingConfig::operator=(const IoRingConfig& other)
{
    if (this != &other) {
        bsl::memcpy(this, &other, sizeof(ntco::IoRingConfig));
    }

    return *this;
}

void IoRingConfig::reset()
{
    bsl::memset(this, 0, sizeof(ntco::IoRingConfig));
}

void IoRingConfig::setSubmissionQueueCapacity(bsl::uint32_t value)
{
    d_submissionQueueCapacity = value;
}

void IoRingConfig::setCompletionQueueCapacity(bsl::uint32_t value)
{
    d_completionQueueCapacity = value;
}

void IoRingConfig::setFlags(bsl::uint32_t value)
{
    d_flags = value;
}

void IoRingConfig::setFeatures(bsl::uint32_t value)
{
    d_features = value;
}

bsl::uint32_t IoRingConfig::submissionQueueCapacity() const
{
    return d_submissionQueueCapacity;
}

bsl::uint32_t IoRingConfig::submissionQueueOffsetToHead() const
{
    return d_submissionQueueOffsetToHead;
}

bsl::uint32_t IoRingConfig::submissionQueueOffsetToTail() const
{
    return d_submissionQueueOffsetToTail;
}

bsl::uint32_t IoRingConfig::submissionQueueOffsetToRingMask() const
{
    return d_submissionQueueOffsetToRingMask;
}

bsl::uint32_t IoRingConfig::submissionQueueOffsetToRingEntries() const
{
    return d_submissionQueueOffsetToRingEntries;
}

bsl::uint32_t IoRingConfig::submissionQueueOffsetToFlags() const
{
    return d_submissionQueueOffsetToFlags;
}

bsl::uint32_t IoRingConfig::submissionQueueOffsetToArray() const
{
    return d_submissionQueueOffsetToArray;
}

bsl::uint32_t IoRingConfig::completionQueueCapacity() const
{
    return d_completionQueueCapacity;
}

bsl::uint32_t IoRingConfig::completionQueueOffsetToHead() const
{
    return d_completionQueueOffsetToHead;
}

bsl::uint32_t IoRingConfig::completionQueueOffsetToTail() const
{
    return d_completionQueueOffsetToTail;
}

bsl::uint32_t IoRingConfig::completionQueueOffsetToRingMask() const
{
    return d_completionQueueOffsetToRingMask;
}

bsl::uint32_t IoRingConfig::completionQueueOffsetToRingEntries() const
{
    return d_completionQueueOffsetToRingEntries;
}

bsl::uint32_t IoRingConfig::completionQueueOffsetToFlags() const
{
    return d_completionQueueOffsetToFlags;
}

bsl::uint32_t IoRingConfig::completionQueueOffsetToCQEs() const
{
    return d_completionQueueOffsetToCQEs;
}

bsl::uint32_t IoRingConfig::flags() const
{
    return d_flags;
}

bsl::uint32_t IoRingConfig::features() const
{
    return d_features;
}

bsl::ostream& IoRingConfig::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    
    printer.printAttribute(
        "submissionQueueCapacity", 
        this->submissionQueueCapacity());

    printer.printAttribute(
        "submissionQueueOffsetToHead", 
        this->submissionQueueOffsetToHead());

    printer.printAttribute(
        "submissionQueueOffsetToTail", 
        this->submissionQueueOffsetToTail());

    printer.printAttribute(
        "submissionQueueOffsetToRingMask", 
        this->submissionQueueOffsetToRingMask());

    printer.printAttribute(
        "submissionQueueOffsetToRingEntries", 
        this->submissionQueueOffsetToRingEntries());

    printer.printAttribute(
        "submissionQueueOffsetToFlags", 
        this->submissionQueueOffsetToFlags());

    printer.printAttribute(
        "submissionQueueOffsetToArray", 
        this->submissionQueueOffsetToArray());

    printer.printAttribute(
        "completionQueueCapacity", 
        this->completionQueueCapacity());

    printer.printAttribute(
        "completionQueueOffsetToHead", 
        this->completionQueueOffsetToHead());

    printer.printAttribute(
        "completionQueueOffsetToTail", 
        this->completionQueueOffsetToTail());

    printer.printAttribute(
        "completionQueueOffsetToRingMask", 
        this->completionQueueOffsetToRingMask());

    printer.printAttribute(
        "completionQueueOffsetToRingEntries", 
        this->completionQueueOffsetToRingEntries());

    printer.printAttribute(
        "completionQueueOffsetToFlags", 
        this->completionQueueOffsetToFlags());

    printer.printAttribute(
        "completionQueueOffsetToCQEs", 
        this->completionQueueOffsetToCQEs());

    printer.printAttribute(
        "flags", 
        this->flags());

    printer.printAttribute(
        "features", 
        this->features());

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const IoRingConfig& object)
{
    return object.print(stream, 0, -1);
}

IoRingCapabilities::IoRingCapabilities()
: d_flags(0)
{
}

IoRingCapabilities::IoRingCapabilities(const IoRingCapabilities& original)
: d_flags(original.d_flags)
{
}

IoRingCapabilities::~IoRingCapabilities()
{
}

IoRingCapabilities& IoRingCapabilities::operator=(const IoRingCapabilities& other)
{
    if (this != &other) {
        d_flags =  other.d_flags;
    }

    return *this;
}

void IoRingCapabilities::reset()
{
    d_flags = 0;
}

bsl::ostream& IoRingCapabilities::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    
    printer.printAttribute("flags", d_flags);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&             stream, 
                         const IoRingCapabilities& object)
{
    return object.print(stream, 0, -1);
}

IoRingSubmission::IoRingSubmission()
{
#if NTCO_IORING_SUBMISSION_128 == 0
    BSLMF_ASSERT(sizeof(IoRingSubmission) == 64);
#else
    BSLMF_ASSERT(sizeof(IoRingSubmission) == 128);
#endif

    bsl::memset(this, 0, sizeof(IoRingSubmission));
}

IoRingSubmission::IoRingSubmission(const IoRingSubmission& original)
{
    bsl::memcpy(this, &original, sizeof(IoRingSubmission));
}

IoRingSubmission::~IoRingSubmission()
{
}

IoRingSubmission& IoRingSubmission::operator=(const IoRingSubmission& other)
{
    if (this != &other) {
        bsl::memcpy(this, &other, sizeof(IoRingSubmission));
    }

    return *this;
}

void IoRingSubmission::reset()
{
    bsl::memset(this, 0, sizeof(IoRingSubmission));
}

void IoRingSubmission::prepareTimeout(struct __kernel_timespec* timespec,
                                      const bsls::TimeInterval& deadline)
{
    // As of the Linux kernel 5.6.16, io_uring operations of
    // type IORING_OP_TIMEOUT must be specified in terms of a
    // __kernel_timespec in the monotonic clock (CLOCK_MONOTONIC).
    // The epoch of this clock is from an arbitrary time in the
    // past around the time the machine booted. Newer kernels
    // released after around October 2021 should support specifying
    // the clock when the operation is submitted, in
    // the SQE timeout_flags (e.g. IORING_TIMEOUT_REALTIME,
    // to specify the __kernel_timespec is in the realtime clock,
    // as ntci::Chronology reports timer deadlines) along with
    // IORING_TIMEOUT_ABS.
    //
    // TODO: IORING_TIMEOUT_REALTIME | IORING_TIMEOUT_ABS

    const bsls::TimeInterval now = bdlt::CurrentTime::now();

    bsls::TimeInterval duration;
    if (deadline > now) {
        duration = deadline - now;
    }

    timespec->tv_sec  = duration.seconds();
    timespec->tv_nsec = duration.nanoseconds();

    d_operation = IORING_OP_TIMEOUT;
    d_handle    = -1;
    d_address   = reinterpret_cast<__u64>(timespec);
    d_count     = 1;
    d_flags     = NTCO_IORING_SQE_FLAGS;
}

void IoRingSubmission::prepareCallback(
    ntcs::Event*                event, 
    const ntcs::Event::Functor& callback)
{
    event->d_type     = ntcs::EventType::e_CALLBACK;
    event->d_function = callback;

    d_operation = IORING_OP_NOP;
    d_handle    = -1;
    d_event     = reinterpret_cast<bsl::uint64_t>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
}

ntsa::Error IoRingSubmission::prepareAccept(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle)
{
    event->d_type   = ntcs::EventType::e_ACCEPT;
    event->d_socket = socket;

    sockaddr_storage* socketAddress     = event->address<sockaddr_storage>();
    socklen_t*        socketAddressSize = event->indicator<socklen_t>();

    bsl::memset(socketAddress, 0, sizeof(sockaddr_storage));
    *socketAddressSize = sizeof(sockaddr_storage);

    d_operation = IORING_OP_ACCEPT;
    d_handle    = handle;
    d_event     = reinterpret_cast<bsl::uint64_t>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address   = reinterpret_cast<bsl::uint64_t>(socketAddress);
    d_size      = reinterpret_cast<bsl::uint64_t>(socketAddressSize);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareConnect(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const ntsa::Endpoint&                        endpoint)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_CONNECT;
    event->d_socket = socket;

    sockaddr_storage* socketAddress = event->address<sockaddr_storage>();
    bsl::memset(socketAddress, 0, sizeof(sockaddr_storage));

    bsl::size_t socketAddressSize;
    error = ntsu::SocketUtil::encodeEndpoint(
        socketAddress, &socketAddressSize, endpoint);
    if (error) {
        return error;
    }

    d_operation = IORING_OP_CONNECT;
    d_handle    = handle;
    d_event     = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address   = reinterpret_cast<__u64>(socketAddress);
    d_size      = socketAddressSize;

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
        ntcs::Event*                                 event, 
        const bsl::shared_ptr<ntci::ProactorSocket>& socket,
        ntsa::Handle                                 handle,
        const ntsa::Data&                            source,
        const ntsa::SendOptions&                     options)
{
    if (NTCCFG_LIKELY(source.isBlob())) {
        return this->prepareSend(event, socket, handle, source.blob(), options);
    }
    else if (source.isSharedBlob()) {
        if (source.sharedBlob()) {
            return this->prepareSend(event, socket, handle, *source.sharedBlob(), options);
        }
        else {
            return this->prepareSend(event, socket, handle, ntsa::ConstBuffer(), options);
        }
    }
    else if (source.isBlobBuffer()) {
        return this->prepareSend(event, socket, handle, source.blobBuffer(), options);
    }
    else if (source.isConstBuffer()) {
        return this->prepareSend(event, socket, handle, source.constBuffer(), options);
    }
    else if (source.isConstBufferArray()) {
        return this->prepareSend(event, socket, handle, source.constBufferArray(), options);
    }
    else if (source.isConstBufferPtrArray()) {
        return this->prepareSend(event, socket, handle, source.constBufferPtrArray(), options);
    }
    else if (source.isMutableBuffer()) {
        return this->prepareSend(event, socket, handle, source.mutableBuffer(), options);
    }
    else if (source.isMutableBufferArray()) {
        return this->prepareSend(event, socket, handle, source.mutableBufferArray(), options);
    }
    else if (source.isMutableBufferPtrArray()) {
        return this->prepareSend(event, socket, handle, source.mutableBufferPtrArray(), options);
    }
    else if (source.isFile()) {
        return this->prepareSend(event, socket, handle, source.file(), options);
    }
    else if (source.isString()) {
        return this->prepareSend(event, socket, handle, source.string(), options);
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const bdlbb::Blob&                           source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    if (source.numDataBuffers() == 1) {
        bsl::size_t numBuffersMax;
        ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMax);

        void*       data = source.buffer(0).data();
        bsl::size_t size = source.lastDataBufferLength();
        
        iovecArray[0].iov_base = data;
        iovecArray[0].iov_len  = size;

        event->d_numBytesAttempted = size;

        message->msg_iov    = iovecArray;
        message->msg_iovlen = 1;
    }
    else {
        bsl::size_t numBuffersMaxLimit;
        ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMaxLimit);

        if (numBuffersMaxLimit > IOV_MAX) {
            numBuffersMaxLimit = IOV_MAX;
        }

        bsl::size_t numBytesMax = options.maxBytes();
        
        bsl::size_t numBuffersMax = options.maxBuffers();
        if (numBuffersMax == 0) {
            numBuffersMax = numBuffersMaxLimit;
        }
        else if (numBuffersMax > numBuffersMaxLimit) {
            numBuffersMax = numBuffersMaxLimit;
        }

        bsl::size_t numBuffersTotal;
        bsl::size_t numBytesTotal;

        ntsu::BufferUtil::gather(
            &numBuffersTotal,
            &numBytesTotal,
            reinterpret_cast<ntsa::ConstBuffer*>(iovecArray),
            numBuffersMax,
            source,
            numBytesMax);

        if (numBuffersTotal == 0) {
            return ntsa::Error::invalid();
        }

        if (numBytesTotal == 0) {
            return ntsa::Error::invalid();
        }

        event->d_numBytesAttempted = numBytesTotal;

        message->msg_iov    = iovecArray;
        message->msg_iovlen = numBuffersTotal;
    }

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation = IORING_OP_SENDMSG;
    d_handle    = handle;
    d_event     = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address   = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const bdlbb::BlobBuffer&                     source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    bsl::size_t numBuffersMax;
    ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMax);

    void*       data = source.data();
    bsl::size_t size = source.size();
    
    iovecArray[0].iov_base = data;
    iovecArray[0].iov_len  = size;

    event->d_numBytesAttempted = size;

    message->msg_iov    = iovecArray;
    message->msg_iovlen = 1;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation = IORING_OP_SENDMSG;
    d_handle    = handle;
    d_event     = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address   = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const ntsa::ConstBuffer&                     source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    bsl::size_t numBuffersMax;
    ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMax);

    void*       data = const_cast<void*>(source.data());
    bsl::size_t size = source.size();
    
    iovecArray[0].iov_base = data;
    iovecArray[0].iov_len  = size;

    event->d_numBytesAttempted = size;

    message->msg_iov    = iovecArray;
    message->msg_iovlen = 1;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation    = IORING_OP_SENDMSG;
    d_handle        = handle;
    d_event = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address      = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const ntsa::ConstBufferArray&                source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    bsl::size_t numBuffersMaxLimit;
    ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMaxLimit);

    if (numBuffersMaxLimit > IOV_MAX) {
        numBuffersMaxLimit = IOV_MAX;
    }

    bsl::size_t numBytesMax = options.maxBytes();
    
    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = numBuffersMaxLimit;
    }
    else if (numBuffersMax > numBuffersMaxLimit) {
        numBuffersMax = numBuffersMaxLimit;
    }

    bsl::size_t numBuffersTotal = 0;
    bsl::size_t numBytesTotal = 0;

    const bsl::size_t sourceBufferCount = source.numBuffers();

    for (bsl::size_t i = 0; i < sourceBufferCount; ++i) {
        const ntsa::ConstBuffer& constBuffer = source.buffer(i);

        void*       data = const_cast<void*>(constBuffer.data());
        bsl::size_t size = constBuffer.size();

        ::iovec iovec;
        iovec.iov_base = data;
        iovec.iov_len  = size;

        iovecArray[i] = iovec;

        numBuffersTotal += 1;
        numBytesTotal   += size;

        if (numBuffersMax > 0 && numBuffersTotal >= numBuffersMax) {
            break;
        }

        if (numBytesMax > 0 && numBytesTotal >= numBytesMax) {
            break;
        }
    }

    event->d_numBytesAttempted = numBytesTotal;

    message->msg_iov    = iovecArray;
    message->msg_iovlen = numBuffersTotal;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation    = IORING_OP_SENDMSG;
    d_handle        = handle;
    d_event = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address      = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const ntsa::ConstBufferPtrArray&             source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    bsl::size_t numBuffersMaxLimit;
    ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMaxLimit);

    if (numBuffersMaxLimit > IOV_MAX) {
        numBuffersMaxLimit = IOV_MAX;
    }

    bsl::size_t numBytesMax = options.maxBytes();
    
    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = numBuffersMaxLimit;
    }
    else if (numBuffersMax > numBuffersMaxLimit) {
        numBuffersMax = numBuffersMaxLimit;
    }

    bsl::size_t numBuffersTotal = 0;
    bsl::size_t numBytesTotal = 0;

    const bsl::size_t sourceBufferCount = source.numBuffers();

    for (bsl::size_t i = 0; i < sourceBufferCount; ++i) {
        const ntsa::ConstBuffer& constBuffer = source.buffer(i);

        void*       data = const_cast<void*>(constBuffer.data());
        bsl::size_t size = constBuffer.size();

        ::iovec iovec;
        iovec.iov_base = data;
        iovec.iov_len  = size;

        iovecArray[i] = iovec;

        numBuffersTotal += 1;
        numBytesTotal   += size;

        if (numBuffersMax > 0 && numBuffersTotal >= numBuffersMax) {
            break;
        }

        if (numBytesMax > 0 && numBytesTotal >= numBytesMax) {
            break;
        }
    }

    event->d_numBytesAttempted = numBytesTotal;

    message->msg_iov    = iovecArray;
    message->msg_iovlen = numBuffersTotal;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation    = IORING_OP_SENDMSG;
    d_handle        = handle;
    d_event = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address      = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const ntsa::MutableBuffer&                   source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    bsl::size_t numBuffersMax;
    ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMax);

    void*       data = source.data();
    bsl::size_t size = source.size();
    
    iovecArray[0].iov_base = data;
    iovecArray[0].iov_len  = size;

    event->d_numBytesAttempted = size;

    message->msg_iov    = iovecArray;
    message->msg_iovlen = 1;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation    = IORING_OP_SENDMSG;
    d_handle        = handle;
    d_event = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address      = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const ntsa::MutableBufferArray&              source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    bsl::size_t numBuffersMaxLimit;
    ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMaxLimit);

    if (numBuffersMaxLimit > IOV_MAX) {
        numBuffersMaxLimit = IOV_MAX;
    }

    bsl::size_t numBytesMax = options.maxBytes();
    
    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = numBuffersMaxLimit;
    }
    else if (numBuffersMax > numBuffersMaxLimit) {
        numBuffersMax = numBuffersMaxLimit;
    }

    bsl::size_t numBuffersTotal = 0;
    bsl::size_t numBytesTotal = 0;

    const bsl::size_t sourceBufferCount = source.numBuffers();

    for (bsl::size_t i = 0; i < sourceBufferCount; ++i) {
        const ntsa::MutableBuffer& mutableBuffer = source.buffer(i);

        void*       data = mutableBuffer.data();
        bsl::size_t size = mutableBuffer.size();

        ::iovec iovec;
        iovec.iov_base = data;
        iovec.iov_len  = size;

        iovecArray[i] = iovec;

        numBuffersTotal += 1;
        numBytesTotal   += size;

        if (numBuffersMax > 0 && numBuffersTotal >= numBuffersMax) {
            break;
        }

        if (numBytesMax > 0 && numBytesTotal >= numBytesMax) {
            break;
        }
    }

    event->d_numBytesAttempted = numBytesTotal;

    message->msg_iov    = iovecArray;
    message->msg_iovlen = numBuffersTotal;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation    = IORING_OP_SENDMSG;
    d_handle        = handle;
    d_event = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address      = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const ntsa::MutableBufferPtrArray&           source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    bsl::size_t numBuffersMaxLimit;
    ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMaxLimit);

    if (numBuffersMaxLimit > IOV_MAX) {
        numBuffersMaxLimit = IOV_MAX;
    }

    bsl::size_t numBytesMax = options.maxBytes();
    
    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = numBuffersMaxLimit;
    }
    else if (numBuffersMax > numBuffersMaxLimit) {
        numBuffersMax = numBuffersMaxLimit;
    }

    bsl::size_t numBuffersTotal = 0;
    bsl::size_t numBytesTotal = 0;

    const bsl::size_t sourceBufferCount = source.numBuffers();

    for (bsl::size_t i = 0; i < sourceBufferCount; ++i) {
        const ntsa::MutableBuffer& mutableBuffer = source.buffer(i);

        void*       data = mutableBuffer.data();
        bsl::size_t size = mutableBuffer.size();

        ::iovec iovec;
        iovec.iov_base = data;
        iovec.iov_len  = size;

        iovecArray[i] = iovec;

        numBuffersTotal += 1;
        numBytesTotal   += size;

        if (numBuffersMax > 0 && numBuffersTotal >= numBuffersMax) {
            break;
        }

        if (numBytesMax > 0 && numBytesTotal >= numBytesMax) {
            break;
        }
    }

    event->d_numBytesAttempted = numBytesTotal;

    message->msg_iov    = iovecArray;
    message->msg_iovlen = numBuffersTotal;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation    = IORING_OP_SENDMSG;
    d_handle        = handle;
    d_event = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address      = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const ntsa::File&                            source,
    const ntsa::SendOptions&                     options)
{
    NTCCFG_WARNING_UNUSED(event);
    NTCCFG_WARNING_UNUSED(socket);
    NTCCFG_WARNING_UNUSED(handle);
    NTCCFG_WARNING_UNUSED(source);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error IoRingSubmission::prepareSend(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    const bsl::string&                           source,
    const ntsa::SendOptions&                     options)
{
    ntsa::Error error;

    event->d_type   = ntcs::EventType::e_SEND;
    event->d_socket = socket;

    ::msghdr* message = event->message<::msghdr>();
    bsl::memset(message, 0, sizeof(::msghdr));

    bsl::size_t numBuffersMax;
    ::iovec* iovecArray = event->buffers<::iovec>(&numBuffersMax);

    void* data = const_cast<void*>(static_cast<const void*>(source.c_str()));
    bsl::size_t size = source.size();
    
    iovecArray[0].iov_base = data;
    iovecArray[0].iov_len  = size;

    event->d_numBytesAttempted = size;

    message->msg_iov    = iovecArray;
    message->msg_iovlen = 1;

    const bool specifyEndpoint = !options.endpoint().isNull();

    if (specifyEndpoint) {
        ::sockaddr_storage* socketAddress = 
            event->address<::sockaddr_storage>();

        bsl::size_t socketAddressSize;
        error = ntsu::SocketUtil::encodeEndpoint(
            socketAddress, &socketAddressSize, options.endpoint().value());

        message->msg_name    = socketAddress;
        message->msg_namelen = static_cast<socklen_t>(socketAddressSize);
    }

    d_operation    = IORING_OP_SENDMSG;
    d_handle        = handle;
    d_event = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address      = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

ntsa::Error IoRingSubmission::prepareReceive(
    ntcs::Event*                                 event, 
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::Handle                                 handle,
    bdlbb::Blob*                                 destination,
    const ntsa::ReceiveOptions&                  options)
{
    ntsa::Error error;

    event->d_type          = ntcs::EventType::e_RECEIVE;
    event->d_socket        = socket;
    event->d_receiveData_p = destination;

    BSLMF_ASSERT(sizeof(event->d_buffers) >= sizeof(::iovec));
    BSLMF_ASSERT(sizeof(event->d_buffers) % sizeof(::iovec) == 0);

    ::iovec* iovecArray = reinterpret_cast<::iovec*>(event->d_buffers);
    BSLS_ASSERT(bsls::AlignmentUtil::is8ByteAligned(iovecArray));

    bsl::size_t numBuffersMaxLimit = 
        sizeof(event->d_buffers) / sizeof(::iovec);

    if (numBuffersMaxLimit > IOV_MAX) {
        numBuffersMaxLimit = IOV_MAX;
    }

    bsl::size_t numBytesMax = options.maxBytes();

    bsl::size_t numBuffersMax = options.maxBuffers();
    if (numBuffersMax == 0) {
        numBuffersMax = numBuffersMaxLimit;
    }
    else if (numBuffersMax > numBuffersMaxLimit) {
        numBuffersMax = numBuffersMaxLimit;
    }

    bsl::size_t numBuffersTotal;
    bsl::size_t numBytesTotal;

    ntsu::BufferUtil::scatter(
        &numBuffersTotal,
        &numBytesTotal,
        reinterpret_cast<ntsa::MutableBuffer*>(iovecArray),
        numBuffersMax,
        destination,
        numBytesMax);

    if (numBuffersTotal == 0) {
        return ntsa::Error::invalid();
    }

    if (numBytesTotal == 0) {
        return ntsa::Error::invalid();
    }

    event->d_numBytesAttempted = numBytesTotal;

    BSLMF_ASSERT(sizeof(event->d_message) >= sizeof(struct ::msghdr));

    ::msghdr* message = reinterpret_cast<::msghdr*>(event->d_message);
    BSLS_ASSERT(bsls::AlignmentUtil::is8ByteAligned(message));

    bsl::memset(message, 0, sizeof(::msghdr));

    const bool wantEndpoint = options.wantEndpoint();

    if (wantEndpoint) {
        ::sockaddr_storage* socketAddress = 
            reinterpret_cast<::sockaddr_storage*>(event->d_address);
        BSLS_ASSERT(bsls::AlignmentUtil::is8ByteAligned(socketAddress));

        bsl::memset(socketAddress, 0, sizeof(::sockaddr_storage));

        message->msg_name = socketAddress;
        message->msg_namelen = sizeof(::sockaddr_storage);
    }

    message->msg_iov    = iovecArray;
    message->msg_iovlen = static_cast<bsl::size_t>(numBuffersTotal);

    d_operation    = IORING_OP_RECVMSG;
    d_handle        = handle;
    d_event = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
    d_address      = reinterpret_cast<__u64>(message);

    return ntsa::Error();
}

void IoRingSubmission::prepareCancellation(ntcs::Event* event)
{
    BSLS_ASSERT(event->d_status == ntcs::EventStatus::e_CANCELLED);

    d_operation    = IORING_OP_ASYNC_CANCEL;
    d_handle        = -1;
    d_address      = reinterpret_cast<__u64>(event);
    d_flags     = NTCO_IORING_SQE_FLAGS;
}

ntsa::Handle IoRingSubmission::handle() const
{
    return static_cast<ntsa::Handle>(d_handle);
}

ntcs::Event* IoRingSubmission::event() const
{
    return reinterpret_cast<ntcs::Event*>(d_event);
}

bsl::uint8_t IoRingSubmission::opcode() const
{
    return static_cast<bsl::uint8_t>(d_operation);
}

bsl::uint8_t IoRingSubmission::flags() const
{
    return static_cast<bsl::uint8_t>(d_flags);
}

bsl::ostream& IoRingSubmission::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    
    printer.printAttribute("handle", this->handle());

    printer.printAttribute("operation", 
                            IoRingUtil::describeOpCode(this->opcode()));

    printer.printAttribute("flags", this->flags());

    if (this->event()) {
        printer.printAttribute("event", *this->event());
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const IoRingSubmission& object)
{
    return object.print(stream, 0, -1);
}

IoRingSubmissionList::IoRingSubmissionList(bslma::Allocator* basicAllocator)
: d_mutex()
, d_data(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

IoRingSubmissionList::~IoRingSubmissionList()
{
}

ntsa::Error IoRingSubmissionList::push(const IoRingSubmission& entry)
{
    LockGuard lock(&d_mutex);
    d_data.push_back(entry);
    return ntsa::Error();
}

ntsa::Error IoRingSubmissionList::pop(IoRingSubmission* result)
{
    LockGuard lock(&d_mutex);

    if (d_data.empty()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    *result = d_data.front();
    d_data.erase(d_data.begin());
    return ntsa::Error();
}

bsl::size_t IoRingSubmissionList::size() const
{
    LockGuard lock(&d_mutex);
    return d_data.size();
}

bool IoRingSubmissionList::empty() const
{
    LockGuard lock(&d_mutex);
    return d_data.empty();
}

#if NTC_BUILD_WITH_IORING_LIBURING == 0

IoRingSubmissionQueue::IoRingSubmissionQueue()
: d_mutex()
, d_ring(-1)
, d_memoryMap_p(0)
, d_head_p(0)
, d_tail_p(0)
, d_mask_p(0)
, d_ring_entries_p(0)
, d_flags_p(0)
, d_array_p(0)
, d_entryArray(0)
, d_params()
{
}

IoRingSubmissionQueue::~IoRingSubmissionQueue()
{
    if (d_memoryMap_p != 0) {
        this->unmap();
    }
}

ntsa::Error IoRingSubmissionQueue::map(int                      ring,
                                       const ntco::IoRingConfig& parameters)
{
    NTCI_LOG_CONTEXT();

    LockGuard guard(&d_mutex);

    if (d_ring != -1 && d_memoryMap_p != 0) {
        return ntsa::Error::invalid();
    }

    d_ring = ring;
    d_params = parameters;

    bsl::uint8_t* submissionQueueBase = reinterpret_cast<bsl::uint8_t*>(
        ::mmap(
            0,
            d_params.submissionQueueOffsetToArray() + d_params.submissionQueueCapacity() * sizeof(bsl::uint32_t),
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_POPULATE,
            d_ring,
            IORING_OFF_SQ_RING));

    if (submissionQueueBase == MAP_FAILED) {
        ntsa::Error error(errno);
        NTCI_LOG_ERROR(
            "I/O ring failed to map submission queue ring buffer: %s",
            error.text().c_str());
        return error;
    }

    d_memoryMap_p = submissionQueueBase;

    d_head_p = reinterpret_cast<bsl::uint32_t*>(submissionQueueBase + d_params.submissionQueueOffsetToHead());

    d_tail_p = reinterpret_cast<bsl::uint32_t*>(submissionQueueBase + d_params.submissionQueueOffsetToTail());

    d_mask_p =
        reinterpret_cast<bsl::uint32_t*>(submissionQueueBase + d_params.submissionQueueOffsetToRingMask());

    d_ring_entries_p =
        reinterpret_cast<bsl::uint32_t*>(submissionQueueBase + d_params.submissionQueueOffsetToRingEntries());

    d_flags_p = reinterpret_cast<bsl::uint32_t*>(submissionQueueBase + d_params.submissionQueueOffsetToFlags());

    d_array_p = reinterpret_cast<bsl::uint32_t*>(submissionQueueBase + d_params.submissionQueueOffsetToArray());

    d_entryArray =
        reinterpret_cast<ntco::IoRingSubmission*>(
            ::mmap(0,
            d_params.submissionQueueCapacity() * sizeof(ntco::IoRingSubmission),
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_POPULATE,
            d_ring,
            IORING_OFF_SQES));

    if (d_entryArray == MAP_FAILED) {
        ntsa::Error error(errno);
        NTCI_LOG_ERROR(
            "I/O ring failed to map submission queue entry array: %s",
            error.text().c_str());
        return error;
    }

    NTCI_LOG_TRACE("I/O ring mapped submission queue ring buffer: "
                   "head = %u, tail = %u, mask = %u, count = %u",
                   *d_head_p,
                   *d_tail_p,
                   *d_mask_p,
                   *d_ring_entries_p);

    return ntsa::Error();
}

ntsa::Error IoRingSubmissionQueue::push(const ntco::IoRingSubmission& entry)
{
    NTCI_LOG_CONTEXT();

    LockGuard guard(&d_mutex);

    bsl::uint32_t tail = *d_tail_p;
    // bsl::uint32_t next = tail + 1;

    NTCO_IORING_READER_BARRIER();

    bsl::uint32_t mask  = *d_mask_p;
    bsl::uint32_t index = tail & mask;

    // TODO: Detect overflow.
    // if (tail == head) {
    //     return ntsa::Error(ENOSPACE);
    // }

    NTCI_LOG_TRACE("I/O ring pushing submission queue entry to tail index %u",
                   index);

    d_entryArray[index] = entry;

    d_array_p[index] = index;

    ++tail;

    if (*d_tail_p != tail) {
        *d_tail_p = tail;
        NTCO_IORING_WRITER_BARRIER();
    }

    return ntsa::Error();
}

void IoRingSubmissionQueue::unmap()
{
    int rc;

    if (d_memoryMap_p != 0) {
        rc =
            ::munmap(d_entryArray, 
                     d_params.submissionQueueCapacity() * sizeof(ntco::IoRingSubmission));
        BSLS_ASSERT(rc == 0);

        rc = ::munmap(d_memoryMap_p,
                      d_params.submissionQueueOffsetToArray() +
                      d_params.submissionQueueCapacity() * sizeof(bsl::uint32_t));
        BSLS_ASSERT(rc == 0);

        d_memoryMap_p = 0;
    }
}

bsl::uint32_t IoRingSubmissionQueue::headIndex() const
{
    LockGuard guard(&d_mutex);

    if (d_memoryMap_p != 0) {
        NTCO_IORING_READER_BARRIER();
        return *d_head_p;
    }
    else {
        return 0;
    }

    return *d_head_p;
}

bsl::uint32_t IoRingSubmissionQueue::tailIndex() const
{
    LockGuard guard(&d_mutex);

    if (d_memoryMap_p != 0) {
        NTCO_IORING_READER_BARRIER();
        return *d_head_p;
    }
    else {
        return 0;
    }

    return *d_tail_p;
}

IoRingCompletion::IoRingCompletion()
: d_userData(0)
, d_result(0)
, d_flags(0)
{
#if NTCO_IORING_COMPLETION_32
    BSLMF_ASSERT(sizeof(IoRingCompletion) == 32);
    d_extraAsQwords[0] = 0;
    d_extraAsQwords[1] = 0;
#else
    BSLMF_ASSERT(sizeof(IoRingCompletion) == 16);
#endif    
}

IoRingCompletion::IoRingCompletion(const IoRingCompletion& original)
: d_userData(original.d_userData)
, d_result(original.d_result)
, d_flags(original.d_flags)
{
#if NTCO_IORING_COMPLETION_32
    d_extraAsQwords[0] = original.d_extraAsQwords[0];
    d_extraAsQwords[1] = original.d_extraAsQwords[1];
#endif
}

IoRingCompletion::~IoRingCompletion()
{

}

IoRingCompletion& IoRingCompletion::operator=(const IoRingCompletion& other)
{
    if (this != &other) {
        d_userData = other.d_userData;
        d_result   = other.d_result;
        d_flags    = other.d_flags;
#if NTCO_IORING_COMPLETION_32
        d_extraAsQwords[0] = other.d_extraAsQwords[0];
        d_extraAsQwords[1] = other.d_extraAsQwords[1];
#endif
    }

    return *this;
}

void IoRingCompletion::reset()
{
    d_userData = 0;
    d_result = 0;
    d_flags = 0;
#if NTCO_IORING_COMPLETION_32
    d_extraAsQwords[0] = 0;
    d_extraAsQwords[1] = 0;
#endif  
}

ntcs::Event* IoRingCompletion::event() const
{
    return reinterpret_cast<ntcs::Event*>(d_userData);
}

bsl::size_t IoRingCompletion::result() const
{
    if (d_result >= 0) {
        return static_cast<bsl::size_t>(d_result);
    }
    else {
        return 0;
    }
}

ntsa::Error IoRingCompletion::error() const
{
    if (d_result >= 0) {
        return ntsa::Error();
    }
    else {
        return ntsa::Error(-d_result);
    }
}

bsl::uint8_t IoRingCompletion::flags() const
{
    return static_cast<bsl::uint8_t>(d_flags);
}

bool IoRingCompletion::hasSucceeded() const
{
    return d_result >= 0;
}

bool IoRingCompletion::hasFailed() const
{
    return d_result < 0;
}

bool IoRingCompletion::wasCanceled() const
{
    return d_result < 0 && d_result == -ECANCELED;
}

bsl::ostream& IoRingCompletion::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_userData != 0) {
        printer.printAttribute(
            "event", *reinterpret_cast<ntcs::Event*>(d_userData));
    }

    if (d_result >= 0) {
        printer.printAttribute(
            "result", static_cast<bsl::size_t>(d_result));
    }
    else {
        printer.printAttribute("error", ntsa::Error(-d_result));
    }

    printer.printAttribute("flags", static_cast<bsl::uint8_t>(d_flags));

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const IoRingCompletion& object)
{
    return object.print(stream, 0, -1);
}

IoRingCompletionQueue::IoRingCompletionQueue()
: d_mutex()
, d_ring(-1)
, d_memoryMap_p(0)
, d_head_p(0)
, d_tail_p(0)
, d_mask_p(0)
, d_ring_entries_p(0)
, d_entryArray(0)
, d_params()
{
}

IoRingCompletionQueue::~IoRingCompletionQueue()
{
    if (d_memoryMap_p != 0) {
        this->unmap();
    }
}

ntsa::Error IoRingCompletionQueue::map(int                      ring,
                                       const ntco::IoRingConfig& parameters)
{
    NTCI_LOG_CONTEXT();

    LockGuard guard(&d_mutex);

    if (d_ring != -1 && d_memoryMap_p != 0) {
        return ntsa::Error::invalid();
    }

    d_ring = ring;
    d_params = parameters;

    bsl::uint8_t* completionQueueBase = reinterpret_cast<bsl::uint8_t*>(
        ::mmap(
            0,
            d_params.completionQueueOffsetToCQEs() +
                d_params.completionQueueCapacity() * sizeof(ntco::IoRingCompletion),
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_POPULATE,
            d_ring,
            IORING_OFF_CQ_RING));

    if (completionQueueBase == MAP_FAILED) {
        ntsa::Error error(errno);
        NTCI_LOG_ERROR(
            "I/O ring failed to map completion queue ring buffer: %s",
            error.text().c_str());
        return error;
    }

    d_memoryMap_p = completionQueueBase;

    d_head_p = reinterpret_cast<bsl::uint32_t*>(completionQueueBase + d_params.completionQueueOffsetToHead());

    d_tail_p = reinterpret_cast<bsl::uint32_t*>(completionQueueBase + d_params.completionQueueOffsetToTail());

    d_mask_p =
        reinterpret_cast<bsl::uint32_t*>(completionQueueBase + d_params.completionQueueOffsetToRingMask());

    d_ring_entries_p =
        reinterpret_cast<bsl::uint32_t*>(completionQueueBase + d_params.completionQueueOffsetToRingEntries());

    d_entryArray =
        reinterpret_cast<ntco::IoRingCompletion*>(completionQueueBase + d_params.completionQueueOffsetToCQEs());

    NTCI_LOG_TRACE("I/O ring mapped completion queue ring buffer: "
                   "head = %u, tail = %u, mask = %u, count = %u",
                   *d_head_p,
                   *d_tail_p,
                   *d_mask_p,
                   *d_ring_entries_p);

    return ntsa::Error();
}

bsl::size_t IoRingCompletionQueue::pop(ntco::IoRingCompletion* result,
                                       bsl::size_t     capacity)
{
    NTCI_LOG_CONTEXT();

    BSLS_ASSERT(capacity > 0);

    LockGuard guard(&d_mutex);

    if (d_memoryMap_p == 0) {
        return 0;
    }

    bsl::size_t   count = 0;
    bsl::uint32_t head  = *d_head_p;

    while (true) {
        NTCO_IORING_READER_BARRIER();

        bsl::uint32_t tail = *d_tail_p;

        if (head == tail) {
            break;
        }

        bsl::uint32_t mask  = *d_mask_p;
        bsl::uint32_t index = head & mask;

        NTCI_LOG_TRACE(
            "I/O ring popping completion queue entry at head index %u",
            index);

        result[count] = *reinterpret_cast<ntco::IoRingCompletion*>(&d_entryArray[index]);
        ++count;

        ++head;

        if (count == capacity) {
            break;
        }
    }

    *d_head_p = head;

    NTCO_IORING_WRITER_BARRIER();

    return count;
}

void IoRingCompletionQueue::unmap()
{
    LockGuard guard(&d_mutex);

    if (d_memoryMap_p != 0) {
        int rc =
            ::munmap(d_memoryMap_p,
                   d_params.completionQueueOffsetToCQEs() +
                       d_params.completionQueueCapacity() * sizeof(ntco::IoRingCompletion));
        BSLS_ASSERT(rc == 0);

        d_memoryMap_p = 0;
    }
}

bsl::uint32_t IoRingCompletionQueue::headIndex() const
{
    LockGuard guard(&d_mutex);

    if (d_memoryMap_p != 0) {
        NTCO_IORING_READER_BARRIER();
        return *d_head_p;
    }
    else {
        return 0;
    }
}

bsl::uint32_t IoRingCompletionQueue::tailIndex() const
{
    LockGuard guard(&d_mutex);

    if (d_memoryMap_p != 0) {
        NTCO_IORING_READER_BARRIER();
        return *d_tail_p;
    }
    else {
        return 0;
    }
}

IoRingDevice::IoRingDevice(bslma::Allocator* basicAllocator)
: d_mutex()
, d_ring(-1)
, d_submissionList(basicAllocator)
, d_submissionQueue()
, d_completionQueue()
, d_params()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

// Ensure the rlimit MEMLOCK is sufficient.
#if 0
    {
        rlimit new_limit = { 0 };
        rlimit old_limit = { 0 };

        new_limit.rlim_cur = 1024 * 64;
        new_limit.rlim_max = 1024 * 64;

        int rc = prlimit(getpid(), RLIMIT_MEMLOCK, &new_limit, &old_limit);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTCI_LOG_WARN("Failed to set RLIMIT_MEMLOCK: %s",
                            error.text().c_str());
        }

        NTCI_LOG_INFO("I/O ring RLIMIT_MEMLOCK current = %d, maximum = %d",
                        (int)(old_limit.rlim_cur),
                        (int)(old_limit.rlim_max));
    }
#endif

    // Setup a new I/O ring.

    const bsl::uint32_t k_QUEUE_DEPTH = 1024;

    d_ring = ntco::IoRingUtil::setup(k_QUEUE_DEPTH, &d_params);
    if (d_ring < 0) {
        ntsa::Error error(errno);
        if (errno == ENOMEM) {
            NTCI_LOG_ERROR("I/O ring failed to allocate memory locked pages: "
                           "raise per-user ulimit "
                           "(see `ulimit -l` and "
                           "'setrlimit' RLIMIT_MEMLOCK)");
        }
        NTCI_LOG_ERROR("I/O ring failed to setup: %s", error.text().c_str());
    }
    BSLS_ASSERT(d_ring > 0);

    NTCI_LOG_TRACE("I/O ring file descriptor %d created", d_ring);

    // Map into the virtual memory of this process the submission queue for
    // the I/O ring.

    error = d_submissionQueue.map(d_ring, d_params);
    BSLS_ASSERT(!error);

    // Map into the virtual memory of this process the completion queue for
    // the I/O ring.

    error = d_completionQueue.map(d_ring, d_params);
    BSLS_ASSERT(!error);
}

IoRingDevice::~IoRingDevice()
{
    NTCI_LOG_CONTEXT();

    int rc;

    // Unmap the virtual memory for the submission and completion queue for
    // the I/O ring.

    d_completionQueue.unmap();
    d_submissionQueue.unmap();

    // Close the I/O ring file descriptor.

    rc = ::close(d_ring);
    BSLS_ASSERT(rc == 0);

    NTCI_LOG_TRACE("I/O ring file descriptor %d closed", d_ring);
}

// Submit the specified 'entry' to the submission queue. Return the error.
ntsa::Error IoRingDevice::submit(const ntco::IoRingSubmission& entry)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    NTCI_LOG_TRACE("I/O ring pushing submission entry: "
                   "user_data = %p, op = %s, flags = %u, fd = %d",
                   entry.event(),
                   IoRingUtil::describeOpCode(entry.opcode()),
                   entry.flags(),
                   entry.handle());

    BSLS_ASSERT(entry.handle() > 0 || entry.handle() == -1);
    BSLS_ASSERT(entry.event() != 0 || entry.opcode() == IORING_OP_TIMEOUT ||
                entry.opcode() == IORING_OP_ASYNC_CANCEL);

    // MRM: Move to individual functions that allocate the event attached
    // to the SQE.

    ntcs::Event* event = entry.event();

    if (event) {
        BSLS_ASSERT(event->d_status == ntcs::EventStatus::e_FREE);
        event->d_status = ntcs::EventStatus::e_PENDING;
    }

    error = d_submissionQueue.push(entry);
    if (error) {
        NTCI_LOG_ERROR("I/O ring failed to submit entry: %s",
                       error.text().c_str());
        return error;
    }

    rc = ntco::IoRingUtil::enter(d_ring, 1, 0, 0, 0);
    if (rc < 0) {
        error = ntsa::Error(errno);
        if (event) {
            NTCI_LOG_ERROR("I/O ring failed to enter to submit "
                           "event type %s: %s",
                           ntcs::EventType::toString(event->d_type),
                           error.text().c_str());
        }
        else {
            NTCI_LOG_ERROR("I/O ring failed to enter: %s",
                           error.text().c_str());
        }

        return error;
    }

    return ntsa::Error();
}

bsl::size_t IoRingDevice::wait(
    ntci::Waiter                                   waiter,
    ntco::IoRingCompletion*                        entryList,
    bsl::size_t                                    entryListCapacity,
    const bdlb::NullableValue<bsls::TimeInterval>& earliestTimerDue)
{
    IoRingWaiter* result = static_cast<IoRingWaiter*>(waiter);


// This implementation atomically checks the completion queue first, then,
// if no entries are available calls io_uring_enter(GET_EVENTS). Not sure
// if this is correct.
#if 0

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    bsl::size_t entryCount = 0;

    while (true) {
        entryCount =
            d_completionQueue.pop(entryList, entryListCapacity);

        if (entryCount == 0) {
            if (!earliestTimerDue.isNull()) {
                NTCO_IORING_LOG_WAIT_TIMED_HIGH_PRECISION(
                    earliestTimerDue.value());

                ntco::IoRingSubmission entry;
                entry.prepareTimeout(&result->d_ts, earliestTimerDue.value());

                error = this->submit(entry);
                if (error) {
                    NTCI_LOG_WARN("I/O ring failed to submit timer");
                }
            }
            else {
                NTCO_IORING_LOG_WAIT_INDEFINITE();
            }

            NTCI_LOG_TRACE("I/O ring calling wait");

            rc = ntco::IoRingUtil::enter(
                d_ring,
                0,
                1,
                static_cast<unsigned int>(
                    IoRingUtil::k_SYSTEM_CALL_ENTER_GET_EVENTS),
                0);

            NTCI_LOG_TRACE("I/O ring leaving wait, rc = %d", rc);

            if (rc < 0) {
                ntsa::Error error(errno);

                NTCI_LOG_ERROR("I/O ring failed to enter: %s",
                            error.text().c_str());
                return 0;
            }

            continue;
        }

        NTCO_IORING_LOG_WAIT_RESULT(entryCount);

        break;
    }

    BSLS_ASSERT(entryCount > 0);
    return entryCount;

#else

    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    if (!earliestTimerDue.isNull()) {
        NTCO_IORING_LOG_WAIT_TIMED_HIGH_PRECISION(earliestTimerDue.value());

        ntco::IoRingSubmission entry;
        entry.prepareTimeout(&result->d_ts, earliestTimerDue.value());

        error = this->submit(entry);
        if (error) {
            NTCI_LOG_WARN("I/O ring failed to submit timer");
        }
    }
    else {
        NTCO_IORING_LOG_WAIT_INDEFINITE();
    }

    NTCI_LOG_TRACE("I/O ring calling wait");

    rc = ntco::IoRingUtil::enter(
        d_ring,
        0,
        1,
        static_cast<unsigned int>(IoRingUtil::k_SYSTEM_CALL_ENTER_GET_EVENTS),
        0);

    NTCI_LOG_TRACE("I/O ring leaving wait, rc = %d", rc);

    if (rc < 0) {
        ntsa::Error error(errno);

        NTCI_LOG_ERROR("I/O ring failed to enter: %s", error.text().c_str());
        return 0;
    }

    bsl::size_t entryCount =
        d_completionQueue.pop(entryList, entryListCapacity);

    NTCO_IORING_LOG_WAIT_RESULT(entryCount);

    return entryCount;

#endif
}

bsl::size_t IoRingDevice::flush(ntco::IoRingCompletion* entryList,
                                bsl::size_t             entryListCapacity)
{
    return d_completionQueue.pop(entryList, entryListCapacity);
}

#else

IoRingDevice::IoRingDevice(bslma::Allocator* basicAllocator)
: d_sqeLock(bsls::SpinLock::s_unlocked)
, d_cqeLock(bsls::SpinLock::s_unlocked)
, d_ring()
, d_params()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    // Ensure the rlimit MEMLOCK is sufficient.
#if 0
    {
        rlimit new_limit = { 0 };
        rlimit old_limit = { 0 };

        new_limit.rlim_cur = 1024 * 64;
        new_limit.rlim_max = 1024 * 64;

        rc = prlimit(getpid(), RLIMIT_MEMLOCK, &new_limit, &old_limit);
        if (rc != 0) {
            ntsa::Error error(errno);
            NTCI_LOG_WARN("Failed to set RLIMIT_MEMLOCK: %s",
                            error.text().c_str());
        }

        NTCI_LOG_INFO("I/O ring RLIMIT_MEMLOCK current = %d, maximum = %d",
                        (int)(old_limit.rlim_cur),
                        (int)(old_limit.rlim_max));
    }
#endif

    // Zero the I/O uring structures.

    bsl::memset(&d_ring, 0, sizeof d_ring);
    bsl::memset(&d_params, 0, sizeof d_params);

    // Setup a new I/O ring.

    const bsl::uint32_t QUEUE_DEPTH = 1024;

    rc = io_uring_queue_init_params(QUEUE_DEPTH, &d_ring, &d_params);
    if (rc != 0) {
        ntsa::Error error(errno);
        if (errno == ENOMEM) {
            NTCI_LOG_ERROR("I/O ring failed to allocate memory locked pages: "
                           "raise per-user ulimit "
                           "(see `ulimit -l` and "
                           "'setrlimit' RLIMIT_MEMLOCK)");
        }
        NTCI_LOG_ERROR("I/O ring failed to setup: %s", error.text().c_str());
        bsl::abort();
    }

    BSLS_ASSERT(d_ring.ring_fd > 0);

    NTCI_LOG_TRACE("I/O ring file descriptor %d created", d_ring.ring_fd);
}

IoRingDevice::~IoRingDevice()
{
    NTCI_LOG_CONTEXT();

    // Unmap the virtual memory for the submission and completion queue for
    // the I/O ring.

    int fd = d_ring.ring_fd;
    io_uring_queue_exit(&d_ring);

    // Zero the I/O uring structures.

    bsl::memset(&d_ring, 0, sizeof d_ring);
    bsl::memset(&d_params, 0, sizeof d_params);

    NTCI_LOG_TRACE("I/O ring file descriptor %d closed", fd);
}

// Submit the specified 'entry' to the submission queue. Return the error.
ntsa::Error IoRingDevice::submit(const ntco::IoRingSubmission& entry)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    NTCI_LOG_TRACE("I/O ring pushing submission entry: "
                   "user_data = %p, op = %s, flags = %u, fd = %d",
                   entry.event(),
                   IoRingUtil::describeOpCode(entry.opcode()),
                   entry.flags(),
                   entry.handle());

    BSLS_ASSERT(entry.handle() > 0 || entry.handle() == -1);
    BSLS_ASSERT(entry.event() != 0 || entry.opcode() == IORING_OP_TIMEOUT ||
                entry.opcode() == IORING_OP_ASYNC_CANCEL);

    // MRM: Move to individual functions that allocate the event attached
    // to the SQE.

    ntcs::Event* event = entry.event();

    if (event) {
        BSLS_ASSERT(event->d_status == ntcs::EventStatus::e_FREE);
        event->d_status = ntcs::EventStatus::e_PENDING;
    }

    {
        bsls::SpinLockGuard lock(&d_sqeLock);

        ntco::IoRingSubmission* target = 
            reinterpret_cast<ntco::IoRingSubmission*>(
                io_uring_get_sqe(&d_ring));
        bsl::memcpy(target, &entry, sizeof(ntco::IoRingSubmission));

        rc = io_uring_submit(&d_ring);
    }

    if (rc < 0) {
        error = ntsa::Error(errno);
        if (event) {
            NTCI_LOG_ERROR("I/O ring failed to submit event type %s: %s",
                           ntcs::EventType::toString(event->d_type),
                           error.text().c_str());
        }
        else {
            NTCI_LOG_ERROR("I/O ring failed to submit: %s",
                           error.text().c_str());
        }

        return error;
    }

    return ntsa::Error();
}

bsl::size_t IoRingDevice::wait(
    ntci::Waiter                                   waiter,
    ntco::IoRingCompletion*                        entryList,
    bsl::size_t                                    entryListCapacity,
    const bdlb::NullableValue<bsls::TimeInterval>& earliestTimerDue)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;
    int         rc;

    IoRingWaiter* result = static_cast<IoRingWaiter*>(waiter);

    {
        bsls::SpinLockGuard lock(&d_cqeLock);

        enum { CQE_ARRAY_CAPACITY = 128 };

        ::io_uring_cqe* cqeArray[CQE_ARRAY_CAPACITY];
        unsigned int    cqeArrayCapacity = CQE_ARRAY_CAPACITY;
        if (entryListCapacity < CQE_ARRAY_CAPACITY) {
            cqeArrayCapacity = entryListCapacity;
        }

        unsigned int cqeCount =
            io_uring_peek_batch_cqe(&d_ring, cqeArray, entryListCapacity);
        BSLS_ASSERT(cqeCount <= entryListCapacity);

        if (cqeCount > 0) {
            NTCO_IORING_LOG_WAIT_RESULT(cqeCount);

            for (unsigned int cqeIndex = 0; cqeIndex < cqeCount; ++cqeIndex) {
                bsl::memcpy(&entryList[cqeIndex],
                            cqeArray[cqeIndex],
                            sizeof(::io_uring_cqe));

#if NTC_BUILD_WITH_IORING_LIBURING_OPTIMIZATIONS == 0
                io_uring_cqe_seen(&d_ring, cqeArray[cqeIndex]);
#endif
            }

#if NTC_BUILD_WITH_IORING_LIBURING_OPTIMIZATIONS
            io_uring_cq_advance(&d_ring, cqeCount);
#endif

            return cqeCount;
        }
    }

    if (!earliestTimerDue.isNull()) {
        NTCO_IORING_LOG_WAIT_TIMED_HIGH_PRECISION(earliestTimerDue.value());

        ntco::IoRingSubmission entry;
        entry.prepareTimeout(&result->d_ts, earliestTimerDue.value());

        error = this->submit(entry);
        if (error) {
            NTCI_LOG_WARN("I/O ring failed to submit timer");
        }
    }
    else {
        NTCO_IORING_LOG_WAIT_INDEFINITE();
    }

    // TOOD: Handle a resulting CQE with its user_data field set to
    // LIBURING_UDATA_TIMEOUT, which indicates an internal timeout, when
    // the liburing functions that internally implement timers are adopted.

    // MRM: We must protect against simultaneous threads waiting on the
    // completion queue, as when dynamic load balancing is configured.

    NTCI_LOG_TRACE("I/O ring calling wait");

    ::io_uring_cqe* cqe = 0;
    rc                  = io_uring_wait_cqe(&d_ring, &cqe);

    NTCI_LOG_TRACE("I/O ring leaving wait, rc = %d", rc);

    if (rc < 0) {
        ntsa::Error error(-rc);
        NTCO_IORING_LOG_WAIT_FAILURE(error);
        return 0;
    }

    NTCO_IORING_LOG_WAIT_RESULT(1);

    BSLS_ASSERT(cqe);

    bsl::memcpy(&entryList[0], cqe, sizeof(::io_uring_cqe));
    io_uring_cqe_seen(&d_ring, cqe);

    return 1;
}

bsl::size_t IoRingDevice::flush(ntco::IoRingCompletion* entryList,
                                bsl::size_t             entryListCapacity)
{
    bsls::SpinLockGuard lock(&d_cqeLock);

    enum { CQE_ARRAY_CAPACITY = 128 };

    ::io_uring_cqe* cqeArray[CQE_ARRAY_CAPACITY];
    unsigned int    cqeArrayCapacity = CQE_ARRAY_CAPACITY;
    if (entryListCapacity < CQE_ARRAY_CAPACITY) {
        cqeArrayCapacity = entryListCapacity;
    }

    unsigned int cqeCount =
        io_uring_peek_batch_cqe(&d_ring, cqeArray, entryListCapacity);
    BSLS_ASSERT(cqeCount <= entryListCapacity);

    if (cqeCount > 0) {
        for (unsigned int cqeIndex = 0; cqeIndex < cqeCount; ++cqeIndex) {
            bsl::memcpy(&entryList[cqeIndex],
                        cqeArray[cqeIndex],
                        sizeof(::io_uring_cqe));

#if NTC_BUILD_WITH_IORING_LIBURING_OPTIMIZATIONS == 0
            io_uring_cqe_seen(&d_ring, cqeArray[cqeIndex]);
#endif
        }

#if NTC_BUILD_WITH_IORING_LIBURING_OPTIMIZATIONS
        io_uring_cq_advance(&d_ring, cqeCount);
#endif

        return cqeCount;
    }

    return 0;
}

#endif

IoRingContext::IoRingContext(ntsa::Handle      handle,
                             bslma::Allocator* basicAllocator)
: d_handle(handle)
#if NTCO_IORING_CANCELLATION
, d_pendingEventSetMutex()
, d_pendingEventSet(basicAllocator)
#endif
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);
}

IoRingContext::~IoRingContext()
{
#if NTCO_IORING_CANCELLATION

    // TODO: Detect pending operations that have not been detected to have
    // been asynchronously cancelled, once cancellation is implemented.

#if NTCO_IORING_DEBUG
    if (!d_pendingEventSet.empty()) {
        NTCI_LOG_WARN("I/O ring context for socket %d has %zu events left "
                      "pending",
                      d_handle,
                      d_pendingEventSet.size());

        for (EventSet::iterator it = d_pendingEventSet.begin();
             it != d_pendingEventSet.end();
             ++it)
        {
            ntcs::Event* event = *it;

            NTCI_LOG_WARN("I/O ring context for socket %d has pending event "
                          "left of type %s",
                          d_handle,
                          ntcs::EventType::toString(event->d_type));
        }
    }
#endif

    // We cannot assert that pending events are empty because the this object
    // is unset as the proactor socket context before this implemenation
    // asynchronously learns that an event is cancelled. To implement this
    // properly, ntci::ProactorSocket::cancel must be made asynchronous and
    // invoke a callback when it is complete.

    // BSLS_ASSERT(d_pendingEventSet.empty());

#endif
}

ntsa::Error IoRingContext::registerEvent(ntcs::Event* event)
{
#if NTCO_IORING_CANCELLATION

    LockGuard guard(&d_pendingEventSetMutex);

    bool insertResult = d_pendingEventSet.insert(event).second;

    if (!insertResult) {
        return ntsa::Error::invalid();
    }

#endif

    return ntsa::Error();
}

void IoRingContext::completeEvent(ntcs::Event* event)
{
#if NTCO_IORING_CANCELLATION

    LockGuard guard(&d_pendingEventSetMutex);

    bsl::size_t n = d_pendingEventSet.erase(event);

    // Some events may be attempted to be cancelled twice if 'loadPending' is
    // called quickly in succession and the result set cancelled.
    //
    // BSLS_ASSERT(n == 1);

    NTCCFG_WARNING_UNUSED(n);

#endif
}

void IoRingContext::loadPending(EventList* pendingEventList, bool remove)
{
#if NTCO_IORING_CANCELLATION

    LockGuard guard(&d_pendingEventSetMutex);

    pendingEventList->insert(pendingEventList->end(),
                             d_pendingEventSet.begin(),
                             d_pendingEventSet.end());

    if (remove) {
        d_pendingEventSet.clear();
    }

#endif
}

ntsa::Handle IoRingContext::handle() const
{
    return d_handle;
}

const char* IoRingUtil::describeOpCode(int opcode)
{
    switch (opcode) {
    case IORING_OP_NOP:
        return "NOP";
    case IORING_OP_READV:
        return "READV";
    case IORING_OP_WRITEV:
        return "WRITEV";
    case IORING_OP_READ_FIXED:
        return "READ_FIXED";
    case IORING_OP_WRITE_FIXED:
        return "WRITE_FIXED";
    case IORING_OP_POLL_ADD:
        return "POLL_ADD";
    case IORING_OP_POLL_REMOVE:
        return "POLL_REMOVE";
    case IORING_OP_SENDMSG:
        return "SENDMSG";
    case IORING_OP_RECVMSG:
        return "RECVMSG";
    case IORING_OP_TIMEOUT:
        return "TIMEOUT";
    case IORING_OP_TIMEOUT_REMOVE:
        return "TIMEOUT_REMOVE";
    case IORING_OP_ACCEPT:
        return "ACCEPT";
    case IORING_OP_ASYNC_CANCEL:
        return "ASYNC_CANCEL";
    case IORING_OP_LINK_TIMEOUT:
        return "LINK_TIMEOUT";
    case IORING_OP_CONNECT:
        return "CONNECT";
    case IORING_OP_CLOSE:
        return "CLOSE";
    case IORING_OP_READ:
        return "READ";
    case IORING_OP_WRITE:
        return "WRITE";
    case IORING_OP_SEND:
        return "SEND";
    case IORING_OP_RECV:
        return "RECV";
    case IORING_OP_EPOLL_CTL:
        return "EPOLL_CTL";
    }

    return "???";
}

int IoRingUtil::setup(unsigned int entries, ntco::IoRingConfig* parameters)
{
    return static_cast<int>(::syscall(static_cast<long>(k_SYSTEM_CALL_SETUP),
                                      entries,
                                      parameters));
}

int IoRingUtil::enter(int          ring,
                      unsigned int submissions,
                      unsigned int completions,
                      unsigned int flags,
                      sigset_t*    signals)
{
    return static_cast<int>(::syscall(static_cast<long>(k_SYSTEM_CALL_ENTER),
                                      ring,
                                      submissions,
                                      completions,
                                      flags,
                                      signals,
                                      _NSIG / 8));
}

int IoRingUtil::control(int          ring,
                        unsigned int operation,
                        void*        operand,
                        unsigned int count)
{
    return static_cast<int>(
        ::syscall(static_cast<long>(k_SYSTEM_CALL_REGISTER),
                  ring,
                  operation,
                  operand,
                  count));
}

bool IoRingUtil::isSupported()
{
#if defined(__NR_io_uring_setup)
    BSLMF_ASSERT(IoRingUtil::k_SYSTEM_CALL_SETUP == __NR_io_uring_setup);
#endif

#if defined(__NR_io_uring_enter)
    BSLMF_ASSERT(IoRingUtil::k_SYSTEM_CALL_ENTER == __NR_io_uring_enter);
#endif

#if defined(__NR_io_uring_register)
    BSLMF_ASSERT(IoRingUtil::k_SYSTEM_CALL_REGISTER == __NR_io_uring_register);
#endif

#if defined(IORING_ENTER_GETEVENTS)
    BSLMF_ASSERT(static_cast<unsigned int>(
                     IoRingUtil::k_SYSTEM_CALL_ENTER_GET_EVENTS) ==
                 static_cast<unsigned int>(IORING_ENTER_GETEVENTS));
#endif

    errno  = 0;
    int rc = IoRingUtil::enter(-1, 1, 0, 0, 0);
    if (rc == 0) {
        return true;
    }
    else {
        int lastError = errno;
        if (lastError == ENOSYS) {
            return false;
        }
        else {
            return true;
        }
    }
}

/// Provide an implementation of the 'ntci::Proactor' interface implemented
/// using the 'io_uring' API.
///
/// @par Thread Safety
/// This class is thread safe.
class IoRing : public ntci::Proactor,
               public ntcs::Driver,
               public ntccfg::Shared<IoRing>
{
    // Define a type alias for a set of waiters.
    typedef bsl::unordered_set<ntci::Waiter> WaiterSet;

    // This typedef defines a type alias for a map of proactor sockets
    // to the context of the execution of their events.
    typedef bsl::unordered_map<bsl::shared_ptr<ntci::ProactorSocket>,
                               bsl::shared_ptr<ntco::IoRingContext> >
        ContextMap;

    // Define a type alias for a mutex.
    typedef ntci::Mutex Mutex;

    // Define a type alias for a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    // Enumerates the types of update.
    enum UpdateType {
        // The device is being modified to gain interest in certain
        // events.
        e_INCLUDE = 1,

        // The device is being modified to lose interest in certain
        // events.
        e_EXCLUDE = 2
    };

    ntccfg::Object                         d_object;
    ntco::IoRingDevice                     d_device;
    ntcs::EventPool                        d_eventPool;
    mutable Mutex                          d_contextMapMutex;
    ContextMap                             d_contextMap;
    mutable Mutex                          d_waiterSetMutex;
    WaiterSet                              d_waiterSet;
    ntcs::Chronology                       d_chronology;
    bsl::shared_ptr<ntci::User>            d_user_sp;
    bsl::shared_ptr<ntci::DataPool>        d_dataPool_sp;
    bsl::shared_ptr<ntci::Resolver>        d_resolver_sp;
    bsl::shared_ptr<ntci::Reservation>     d_connectionLimiter_sp;
    bsl::shared_ptr<ntci::ProactorMetrics> d_metrics_sp;
    ntcs::Event::Functor                   d_interruptsHandler;
    bsls::AtomicUint                       d_interruptsPending;
    bslmt::ThreadUtil::Handle              d_threadHandle;
    bsl::size_t                            d_threadIndex;
    bsls::AtomicUint64                     d_threadId;
    bsls::AtomicUint64                     d_load;
    bsls::AtomicBool                       d_run;
    ntca::ProactorConfig                   d_config;
    bslma::Allocator*                      d_allocator_p;

  private:
    IoRing(const IoRing&) BSLS_KEYWORD_DELETED;
    IoRing& operator=(const IoRing&) BSLS_KEYWORD_DELETED;

  private:
    // Push the specified 'entry' to the submission queue, blocking
    // if the queue is full until an entry has been popped by another
    // thread. Return the error.
    ntsa::Error submit(const ntco::IoRingSubmission& entry);

    // Process an interruption.
    void interruptComplete();

    // Execute all pending jobs.
    void flush();

    // Block the calling thread, identified by the specified 'waiter',
    // until any registered events for any descriptor in the polling set
    // occurs, or the earliest due timer in the specified 'chronology'
    // elapses, if any.  For each event that has occurred, invoke the
    // correspond processing function on the associated descriptor. Note
    // that implementations are permitted to wakeup from 'timeout'
    // prematurely. The behavior is undefined unless the calling thread
    // has previously registered the 'waiter'.
    void wait(ntci::Waiter waiter);

    // Acquire usage of the most suitable proactor selected according to
    // the specified load balancing 'options'.
    bsl::shared_ptr<ntci::Proactor> acquireProactor(
        const ntca::LoadBalancingOptions& options) BSLS_KEYWORD_OVERRIDE;

    // Release usage of the specified 'proactor' selected according to the
    // specified load balancing 'options'.
    void releaseProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                         const ntca::LoadBalancingOptions&      options)
        BSLS_KEYWORD_OVERRIDE;

    // Increment the current number of handle reservations, if permitted.
    // Return true if the resulting number of handle reservations is
    // permitted, and false otherwise.
    bool acquireHandleReservation() BSLS_KEYWORD_OVERRIDE;

    // Decrement the current number of handle reservations.
    void releaseHandleReservation() BSLS_KEYWORD_OVERRIDE;

    // Return true if the current thread is the principle waiter, i.e.,
    // the principle I/O thread in a statically load-balanced
    // configuration, otherwise return false.
    bool isWaiter() const;

    // Return the number of proactors in the thread pool.
    bsl::size_t numProactors() const BSLS_KEYWORD_OVERRIDE;

    // Return the current number of threads in the thread pool.
    bsl::size_t numThreads() const BSLS_KEYWORD_OVERRIDE;

    // Return the minimum number of threads in the thread pool.
    bsl::size_t minThreads() const BSLS_KEYWORD_OVERRIDE;

    // Return the maximum number of threads in the thread pool.
    bsl::size_t maxThreads() const BSLS_KEYWORD_OVERRIDE;

  public:
    // Define a type alias for a deferred function.
    typedef NTCCFG_FUNCTION() Functor;

    // Create a new proactor having the specified 'configuration' operating
    // in the environment of the specified 'user'. Optionally specify a
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    // the currently installed default allocator is used.
    explicit IoRing(const ntca::ProactorConfig&        configuration,
                    const bsl::shared_ptr<ntci::User>& user,
                    bslma::Allocator*                  basicAllocator = 0);

    // Destroy this object.
    ~IoRing();

    // Register a thread described by the specified 'waiterOptions' that
    // will drive this object. Return the handle to the waiter.
    ntci::Waiter registerWaiter(const ntca::WaiterOptions& waiterOptions)
        BSLS_KEYWORD_OVERRIDE;

    // Deregister the specified 'waiter'.
    void deregisterWaiter(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;

    // Create a new strand to serialize execution of functors by the
    // threads driving this proactor. Optionally specify a 'basicAllocator'
    // used to supply memory. If 'basicAllocator' is 0, the currently
    // installed default allocator is used.
    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    // Attach the specified 'socket' to the proactor. Return the
    // error.
    ntsa::Error attachSocket(const bsl::shared_ptr<ntci::ProactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    // Accept the next connection made to the specified 'socket' bound to
    // the specified 'endpoint'. Return the error.
    ntsa::Error accept(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    // Connect the specified 'socket' to the specified 'endpoint'. Return
    // the error.
    ntsa::Error connect(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                        const ntsa::Endpoint& endpoint) BSLS_KEYWORD_OVERRIDE;

    // Enqueue the specified 'data' to the send buffer of the specified
    // 'socket' according to the specified 'options'. Return the error.
    // Note that 'data' must not be modified or destroyed until the
    // operation completes or fails.
    ntsa::Error send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                     const bdlbb::Blob&                           data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    // Enqueue the specified 'data' to the send buffer of the specified
    // 'socket' according to the specified 'options'. Return the error.
    // Note that 'data' must not be modified or destroyed until the
    // operation completes or fails.
    ntsa::Error send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                     const ntsa::Data&                            data,
                     const ntsa::SendOptions& options) BSLS_KEYWORD_OVERRIDE;

    // Dequeue from the receive buffer of the specified 'socket' into the
    // specified 'data' according to the specified 'options'. Return the
    // error. Note that 'data' must not be modified or destroyed until the
    // operation completes or fails.
    ntsa::Error receive(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                        bdlbb::Blob*                                 data,
                        const ntsa::ReceiveOptions&                  options)
        BSLS_KEYWORD_OVERRIDE;

    // Shutdown the stream socket in the specified 'direction'. Return the
    // error.
    ntsa::Error shutdown(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                         ntsa::ShutdownType::Value direction)
        BSLS_KEYWORD_OVERRIDE;

    // Cancel all outstanding operations initiated for the specified
    // 'socket'. Return the error.
    ntsa::Error cancel(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
        BSLS_KEYWORD_OVERRIDE;

    // Detach the specified 'socket' from the proactor. Return the error.
    ntsa::Error detachSocket(const bsl::shared_ptr<ntci::ProactorSocket>&
                                 socket) BSLS_KEYWORD_OVERRIDE;

    // Close all monitored sockets and timers.
    ntsa::Error closeAll() BSLS_KEYWORD_OVERRIDE;

    // Increment the estimation of the load on the proactor according to
    // the specified load balancing 'options'.
    void incrementLoad(const ntca::LoadBalancingOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    // Decrement the estimation of the load on the proactor according to
    // the specified load balancing 'options'.
    void decrementLoad(const ntca::LoadBalancingOptions& options)
        BSLS_KEYWORD_OVERRIDE;

    // Block the calling thread until stopped. As each previously initiated
    // operation completes, or each timer fires, invoke the corresponding
    // processing function on the associated descriptor or timer. The
    // behavior is undefined unless the calling thread has previously
    // registered the 'waiter'. Note that after this function returns, the
    // 'restart' function must be called before this or the 'run' function
    // can be called again.
    void run(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;

    // Block the calling thread identified by the specified 'waiter', until
    // at least one socket enters the state in which interest has been
    // registered, or timer fires. For each socket that has entered the
    // state in which interest has been registered, or each timer that has
    // fired, invoke the corresponding processing function on the
    // associated descriptor or timer. The behavior is undefined unless the
    // calling thread has previously registered the 'waiter'. Note that
    // if this function returns because 'stop' was called, the 'restart'
    // function must be called before this or the 'run' function can be
    // called again.
    void poll(ntci::Waiter waiter) BSLS_KEYWORD_OVERRIDE;

    // Unblock and return one caller blocked on either 'poll' or 'run'.
    void interruptOne() BSLS_KEYWORD_OVERRIDE;

    // Unblock and return any caller blocked on either 'poll' or 'run'.
    void interruptAll() BSLS_KEYWORD_OVERRIDE;

    // Unblock and return any caller blocked on either 'poll' or 'run'.
    void stop() BSLS_KEYWORD_OVERRIDE;

    // Prepare the reactor for 'run' to be called again after previously
    // being stopped.
    void restart() BSLS_KEYWORD_OVERRIDE;

    // Execute all deferred functions managed by this object.
    void drainFunctions() BSLS_KEYWORD_OVERRIDE;

    // Clear all deferred functions managed by this object.
    void clearFunctions() BSLS_KEYWORD_OVERRIDE;

    // Clear all timers managed by this object.
    void clearTimers() BSLS_KEYWORD_OVERRIDE;

    // Clear all sockets managed by this object.
    void clearSockets() BSLS_KEYWORD_OVERRIDE;

    // Clear all resources managed by this object.
    void clear() BSLS_KEYWORD_OVERRIDE;

    // Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    // Atomically defer the execution of the specified 'functorSequence'
    // immediately followed by the specified 'functor', then clear the
    // 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    // Create a new timer according to the specified 'options' that invokes
    // the specified 'session' for each timer event on this object's
    // 'strand()', if defined, or on an unspecified thread otherwise.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    // Create a new timer according to the specified 'options' that invokes
    // the specified 'callback' for each timer event on this object's
    // 'strand()', if defined, or on an unspecified thread otherwise.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    // Create a new datagram socket with the specified 'options'.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used. Return the error.
    bsl::shared_ptr<ntci::DatagramSocket> createDatagramSocket(
        const ntca::DatagramSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    // Create a new listener socket with the specified 'options'.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used. Return the error.
    bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    // Create a new stream socket with the specified 'options'.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used. Return the error.
    bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    // Return a shared pointer to a data container suitable for storing
    // incoming data. The resulting data container is is automatically
    // returned to this pool when its reference count reaches zero.
    bsl::shared_ptr<ntsa::Data> createIncomingData() BSLS_KEYWORD_OVERRIDE;

    // Return a shared pointer to a data container suitable for storing
    // outgoing data. The resulting data container is is automatically
    // returned to this pool when its reference count reaches zero.
    bsl::shared_ptr<ntsa::Data> createOutgoingData() BSLS_KEYWORD_OVERRIDE;

    // Return a shared pointer to a blob suitable for storing incoming
    // data. The resulting blob is is automatically returned to this pool
    // when its reference count reaches zero.
    bsl::shared_ptr<bdlbb::Blob> createIncomingBlob() BSLS_KEYWORD_OVERRIDE;

    // Return a shared pointer to a blob suitable for storing incoming
    // data. The resulting blob is is automatically returned to this pool
    // when its reference count reaches zero.
    bsl::shared_ptr<bdlbb::Blob> createOutgoingBlob() BSLS_KEYWORD_OVERRIDE;

    // Load into the specified 'blobBuffer' the data and size of a new
    // buffer allocated from the incoming blob buffer factory.
    void createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;

    // Load into the specified 'blobBuffer' the data and size of a new
    // buffer allocated from the outgoing blob buffer factory.
    void createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;

    // Return the number of sockets currently being monitored.
    bsl::size_t numSockets() const BSLS_KEYWORD_OVERRIDE;

    // Return the maximum number of sockets capable of being monitored
    // at one time.
    bsl::size_t maxSockets() const BSLS_KEYWORD_OVERRIDE;

    // Return the number of timers currently being monitored.
    bsl::size_t numTimers() const BSLS_KEYWORD_OVERRIDE;

    // Return the maximum number of timers capable of being monitored
    // at one time.
    bsl::size_t maxTimers() const BSLS_KEYWORD_OVERRIDE;

    // Return the estimation of the load on the proactor.
    bsl::size_t load() const BSLS_KEYWORD_OVERRIDE;

    // Return the handle of the thread that drives this proactor, or
    // the default value if no such thread has been set.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    // Return the index in the thread pool of the thread that drives this
    // proactor, or 0 if no such thread has been set.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

    // Return the current number of registered waiters.
    bsl::size_t numWaiters() const BSLS_KEYWORD_OVERRIDE;

    // Return true if the reactor has no pending deferred functors no
    // pending timers, and no registered sockets, otherwise return false.
    bool empty() const BSLS_KEYWORD_OVERRIDE;

    // Return the data pool.
    const bsl::shared_ptr<ntci::DataPool>& dataPool() const
        BSLS_KEYWORD_OVERRIDE;

    // Return the strand that guarantees sequential, non-current execution
    // of arbitrary functors on the unspecified threads processing events
    // for this object.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    // Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;

    // Return the incoming blob buffer factory.
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;

    // Return the outgoing blob buffer factory.
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;

    // Return the name of the driver.
    const char* name() const BSLS_KEYWORD_OVERRIDE;
};

ntsa::Error IoRing::submit(const ntco::IoRingSubmission& entry)
{
    return d_device.submit(entry);
}

void IoRing::interruptComplete()
{
    NTCI_LOG_CONTEXT();
    NTCI_LOG_TRACE("I/O ring interrupt complete");

    BSLS_ASSERT(d_interruptsPending > 0);
    --d_interruptsPending;
}

void IoRing::flush()
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    while (true) {
        enum { ENTRY_LIST_CAPACITY = 128 };

        typedef bsls::ObjectBuffer<ntco::IoRingCompletion> CompletionArena;
        CompletionArena entryListArena[ENTRY_LIST_CAPACITY];

        ntco::IoRingCompletion* entryList = 
            reinterpret_cast<ntco::IoRingCompletion*>(&entryListArena[0]);

        bsl::size_t entryCount =
            d_device.flush(entryList, ENTRY_LIST_CAPACITY);

        if (entryCount == 0) {
            break;
        }

// MRM
#if NTCO_IORING_DEBUG || 1
        NTCI_LOG_DEBUG("I/O ring flushing jobs: abandoning %zu jobs",
                       entryCount);
#endif

        for (bsl::size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex)
        {
            const ntco::IoRingCompletion& entry = entryList[entryIndex];

// MRM
#if NTCO_IORING_DEBUG || 1
            if (entry.event()) {
                NTCI_LOG_DEBUG(
                    "I/O ring flushing jobs: popped completed entry: "
                    "type = %s, flags = %u, result = %zu, error = %s",
                    ntcs::EventType::toString(entry.event()->d_type),
                    entry.flags(),
                    entry.result(),
                    entry.error().text().c_str());
            }
            else {
                NTCI_LOG_DEBUG(
                    "I/O ring flushing jobs: popped completed entry: "
                    "user_data = %p, flags = %u, result = %zu, error = %s",
                    entry.event(),
                    entry.flags(),
                    entry.result(),
                    entry.error().text().c_str());
            }
#endif

            if (NTCCFG_UNLIKELY(entry.event() == 0)) {
                // Assumed to be a timer, since that is the only sqe submitted
                // that does not have the user data field set the to event
                // pointer.

                continue;
            }

            bslma::ManagedPtr<ntcs::Event> event(entry.event(), &d_eventPool);

            if (event->d_socket) {
                bsl::shared_ptr<ntco::IoRingContext> context =
                    bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
                        event->d_socket->getProactorContext());
                if (context) {
                    context->completeEvent(event.get());
                }
            }

            NTCO_IORING_LOG_EVENT_ABANDONED(event);
        }
    }
}

void IoRing::wait(ntci::Waiter waiter)
{
    NTCCFG_WARNING_UNUSED(waiter);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bdlb::NullableValue<bsls::TimeInterval> earliestTimerDue =
        d_chronology.earliest();

    enum { ENTRY_LIST_CAPACITY = 128 };

    typedef bsls::ObjectBuffer<ntco::IoRingCompletion> CompletionArena;
    CompletionArena entryListArena[ENTRY_LIST_CAPACITY];

    ntco::IoRingCompletion* entryList = 
        reinterpret_cast<ntco::IoRingCompletion*>(&entryListArena[0]);

    bsl::size_t entryCount =
        d_device.wait(waiter, 
                      entryList, 
                      ENTRY_LIST_CAPACITY, 
                      earliestTimerDue);

    for (bsl::size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex) {
        const ntco::IoRingCompletion& entry = entryList[entryIndex];

        NTCI_LOG_TRACE("I/O ring popped completed entry: "
                       "user_data = %p, flags = %u, result = %d, error = %s",
                       entry.event(),
                       entry.flags(),
                       entry.result(),
                       entry.error().text().c_str());

        if (NTCCFG_UNLIKELY(entry.event() == 0)) {
            // Assumed to be a timer or cancellation, since only timers and
            // cancellations do not have the user data field set the to event
            // pointer.

            continue;
        }

        bslma::ManagedPtr<ntcs::Event> event(entry.event(), &d_eventPool);

        ntsa::Error eventError;
        if (entry.hasFailed()) {
            eventError     = entry.error();
            event->d_error = eventError;

            if (event->d_status != ntcs::EventStatus::e_PENDING) {
                NTCI_LOG_DEBUG("I/O ring failed to process failed CQE: "
                               "event type %s found event status %s, "
                               "expected event status PENDING, error: %s",
                               ntcs::EventType::toString(event->d_type),
                               ntcs::EventStatus::toString(event->d_status),
                               event->d_error.text().c_str());

                if (event->d_status == ntcs::EventStatus::e_CANCELLED) {
                    continue;
                }
            }
            BSLS_ASSERT(event->d_status == ntcs::EventStatus::e_PENDING);
            event->d_status = ntcs::EventStatus::e_FAILED;
        }
        else {
            if (event->d_status != ntcs::EventStatus::e_PENDING) {
                NTCI_LOG_DEBUG("I/O ring failed to process CQE: "
                               "event type %s found event status %s, "
                               "expected event status PENDING",
                               ntcs::EventType::toString(event->d_type),
                               ntcs::EventStatus::toString(event->d_status));

                if (event->d_status == ntcs::EventStatus::e_CANCELLED) {
                    continue;
                }
            }
            BSLS_ASSERT(event->d_status == ntcs::EventStatus::e_PENDING);
            event->d_status = ntcs::EventStatus::e_COMPLETE;
        }

        ntsa::Handle handle = ntsa::k_INVALID_HANDLE;
        if (event->d_socket) {
            handle = event->d_socket->handle();
        }

#if NTCO_IORING_CANCELLATION
        if (event->d_socket) {
            bsl::shared_ptr<ntco::IoRingContext> context =
                bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
                    event->d_socket->getProactorContext());
            if (context) {
                // MRM: handle = context->handle();
                // MRM: BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

                context->completeEvent(event.get());
            }
#if NTCO_IORING_DEBUG
            else {
                NTCI_LOG_WARN("Unable to deregister pending event of "
                              "type %s",
                              ntcs::EventType::toString(event->d_type));
            }
#endif
        }
#endif

        if (entry.wasCanceled()) {
            NTCO_IORING_LOG_EVENT_CANCELLED(event);

            // MRM
            // NTCI_LOG_DEBUG("Event of type %s has been cancelled",
            //                ntcs::EventType::toString(event->d_type));
            continue;
        }

        // The cqe entry.res field, if negative, will have the following
        // values:
        //
        // -ETIME:     The timeout has elapsed
        // -ENOENT:    Cancellation failure?
        // -ECANCELED: Cancelled entry
        // -EINVAL:    Canceled entry?

        NTCO_IORING_LOG_EVENT_COMPLETE(event);

        if (event->d_type == ntcs::EventType::e_CALLBACK) {
            if (!eventError) {
                BSLS_ASSERT(event->d_function);
                if (event->d_function) {
                    event->d_function();
                }
            }
        }
        else if (event->d_type == ntcs::EventType::e_ACCEPT) {
            BSLS_ASSERT(event->d_socket);

            if (handle == ntsa::k_INVALID_HANDLE) {
                continue;
            }

            if (eventError) {
                ntcs::Dispatch::announceAccepted(
                    event->d_socket,
                    eventError,
                    bsl::shared_ptr<ntsi::StreamSocket>(),
                    event->d_socket->strand());
            }
            else {
                event->d_target = static_cast<ntsa::Handle>(entry.result());

                bsl::shared_ptr<ntsi::StreamSocket> streamSocket =
                    ntsf::System::createStreamSocket(event->d_target,
                                                     d_allocator_p);

                ntcs::Dispatch::announceAccepted(event->d_socket,
                                                 ntsa::Error(),
                                                 streamSocket,
                                                 event->d_socket->strand());
            }
        }
        else if (event->d_type == ntcs::EventType::e_CONNECT) {
            BSLS_ASSERT(event->d_socket);

            if (handle == ntsa::k_INVALID_HANDLE) {
                continue;
            }

            if (eventError) {
                ntcs::Dispatch::announceConnected(event->d_socket,
                                                  eventError,
                                                  event->d_socket->strand());
            }
            else {
                ntsa::Error lastError;
                ntsu::SocketOptionUtil::getLastError(&lastError, handle);
                if (lastError) {
                    ntcs::Dispatch::announceConnected(
                        event->d_socket,
                        lastError,
                        event->d_socket->strand());
                }
                else {
                    ntsa::Endpoint remoteEndpoint;
                    error = ntsu::SocketUtil::remoteEndpoint(&remoteEndpoint,
                                                             handle);
                    if (error) {
                        ntcs::Dispatch::announceConnected(
                            event->d_socket,
                            error,
                            event->d_socket->strand());
                    }
                    else {
                        ntcs::Dispatch::announceConnected(
                            event->d_socket,
                            ntsa::Error(),
                            event->d_socket->strand());
                    }
                }
            }
        }
        else if (event->d_type == ntcs::EventType::e_SEND) {
            BSLS_ASSERT(event->d_socket);
            BSLS_ASSERT(event->d_numBytesAttempted > 0);

            if (handle == ntsa::k_INVALID_HANDLE) {
                continue;
            }

            ntsa::SendContext context;
            context.setBytesSendable(event->d_numBytesAttempted);

            if (eventError) {
                BSLS_ASSERT(event->d_numBytesCompleted == 0);

                ntcs::Dispatch::announceSent(event->d_socket,
                                             eventError,
                                             context,
                                             event->d_socket->strand());
            }
            else {
                bsl::size_t numBytes = entry.result();

                event->d_numBytesCompleted = numBytes;

                context.setBytesSent(numBytes);

                ntcs::Dispatch::announceSent(event->d_socket,
                                             ntsa::Error(),
                                             context,
                                             event->d_socket->strand());
            }
        }
        else if (event->d_type == ntcs::EventType::e_RECEIVE) {
            BSLS_ASSERT(event->d_socket);
            BSLS_ASSERT(event->d_numBytesAttempted > 0);

            if (handle == ntsa::k_INVALID_HANDLE) {
                continue;
            }

            ntsa::ReceiveContext context;
            context.setBytesReceivable(event->d_numBytesAttempted);

            if (eventError) {
                BSLS_ASSERT(event->d_numBytesCompleted == 0);

                ntcs::Dispatch::announceReceived(event->d_socket,
                                                 eventError,
                                                 context,
                                                 event->d_socket->strand());
            }
            else {
                bsl::size_t numBytes = entry.result();

                BSLS_ASSERT(event->d_receiveData_p);

                BSLS_ASSERT(static_cast<bsl::size_t>(event->d_receiveData_p->length()) + numBytes <=
                            static_cast<bsl::size_t>(event->d_receiveData_p->totalSize()));

                event->d_receiveData_p->setLength(
                    event->d_receiveData_p->length() + numBytes);

                event->d_numBytesCompleted = numBytes;

                context.setBytesReceived(numBytes);

                ::msghdr* message = 
                    reinterpret_cast<::msghdr*>(event->d_message);

                ntsa::Error messageError;
                if (message->msg_namelen > 0) {
                    ntsa::Endpoint endpoint;
                    messageError = ntsu::SocketUtil::decodeEndpoint(
                            &endpoint, 
                            message->msg_name, 
                            message->msg_namelen);

                    if (!messageError) {
                        context.setEndpoint(endpoint);
                    }
                }

                if (!messageError) {
                    ntcs::Dispatch::announceReceived(
                        event->d_socket,
                        ntsa::Error(),
                        context,
                        event->d_socket->strand());
                }
                else {
                    ntcs::Dispatch::announceReceived(
                        event->d_socket,
                        messageError,
                        context,
                        event->d_socket->strand());
                }
            }
        }
        else {
            NTCO_IORING_LOG_EVENT_IGNORED(event);
        }
    }
}

bsl::shared_ptr<ntci::Proactor> IoRing::acquireProactor(
    const ntca::LoadBalancingOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    return this->getSelf(this);
}

void IoRing::releaseProactor(const bsl::shared_ptr<ntci::Proactor>& proactor,
                             const ntca::LoadBalancingOptions&      options)
{
    BSLS_ASSERT(proactor == this->getSelf(this));
    proactor->decrementLoad(options);
}

bool IoRing::acquireHandleReservation()
{
    if (d_connectionLimiter_sp) {
        return d_connectionLimiter_sp->acquire();
    }
    else {
        return true;
    }
}

void IoRing::releaseHandleReservation()
{
    if (d_connectionLimiter_sp) {
        d_connectionLimiter_sp->release();
    }
}

NTCCFG_INLINE
bool IoRing::isWaiter() const
{
    return bslmt::ThreadUtil::selfIdAsUint64() == d_threadId.load();
}

bsl::size_t IoRing::numProactors() const
{
    return 1;
}

bsl::size_t IoRing::numThreads() const
{
    return this->numWaiters();
}

bsl::size_t IoRing::minThreads() const
{
    return d_config.minThreads().value();
}

bsl::size_t IoRing::maxThreads() const
{
    return d_config.maxThreads().value();
}

IoRing::IoRing(const ntca::ProactorConfig&        configuration,
               const bsl::shared_ptr<ntci::User>& user,
               bslma::Allocator*                  basicAllocator)
: d_object("ntco::Proactor")
, d_device(basicAllocator)
, d_eventPool(basicAllocator)
, d_contextMapMutex()
, d_contextMap(basicAllocator)
, d_waiterSetMutex()
, d_waiterSet(basicAllocator)
, d_chronology(this, basicAllocator)
, d_user_sp(user)
, d_dataPool_sp()
, d_resolver_sp()
, d_connectionLimiter_sp()
, d_metrics_sp()
, d_interruptsHandler(NTCCFG_FUNCTION_INIT(basicAllocator))
, d_interruptsPending(0)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_threadIndex(0)
, d_threadId(0)
, d_load(0)
, d_run(true)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_config.metricName().isNull() ||
        d_config.metricName().value().empty())
    {
        d_config.setMetricName(ntcs::Nomenclature::createProactorName());
    }

    BSLS_ASSERT(!d_config.metricName().isNull());
    BSLS_ASSERT(!d_config.metricName().value().empty());

    if (d_config.minThreads().isNull() || d_config.minThreads().value() == 0) {
        d_config.setMinThreads(1);
    }

    BSLS_ASSERT(!d_config.minThreads().isNull());
    BSLS_ASSERT(d_config.minThreads().value() > 0);

    if (d_config.maxThreads().isNull() || d_config.maxThreads().value() == 0) {
        d_config.setMaxThreads(NTCCFG_DEFAULT_MAX_THREADS);
    }

    BSLS_ASSERT(!d_config.maxThreads().isNull());
    BSLS_ASSERT(d_config.maxThreads().value() > 0);

    if (d_config.maxThreads().value() > NTCCFG_DEFAULT_MAX_THREADS) {
        d_config.setMaxThreads(NTCCFG_DEFAULT_MAX_THREADS);
    }

    if (d_config.minThreads().value() > d_config.maxThreads().value()) {
        d_config.setMinThreads(d_config.maxThreads().value());
    }

    BSLS_ASSERT(d_config.minThreads().value() <=
                d_config.maxThreads().value());
    BSLS_ASSERT(d_config.maxThreads().value() <= NTCCFG_DEFAULT_MAX_THREADS);

    if (d_config.maxEventsPerWait().isNull()) {
        d_config.setMaxEventsPerWait(NTCCFG_DEFAULT_MAX_EVENTS_PER_WAIT);
    }

    if (d_config.maxTimersPerWait().isNull()) {
        d_config.setMaxTimersPerWait(NTCCFG_DEFAULT_MAX_TIMERS_PER_WAIT);
    }

    if (d_config.maxCyclesPerWait().isNull()) {
        d_config.setMaxCyclesPerWait(NTCCFG_DEFAULT_MAX_CYCLES_PER_WAIT);
    }

    if (d_config.metricCollection().isNull()) {
        d_config.setMetricCollection(NTCCFG_DEFAULT_DRIVER_METRICS);
    }

    if (d_config.metricCollectionPerWaiter().isNull()) {
        d_config.setMetricCollectionPerWaiter(
            NTCCFG_DEFAULT_DRIVER_METRICS_PER_WAITER);
    }

    if (d_config.metricCollectionPerSocket().isNull()) {
        d_config.setMetricCollectionPerSocket(false);
    }

    if (d_user_sp) {
        d_dataPool_sp = d_user_sp->dataPool();
    }

    if (!d_dataPool_sp) {
        bsl::shared_ptr<ntcs::DataPool> dataPool;
        dataPool.createInplace(d_allocator_p, d_allocator_p);

        d_dataPool_sp = dataPool;
    }

    if (d_user_sp) {
        d_resolver_sp = d_user_sp->resolver();
    }

    if (!d_resolver_sp) {
        // MRM: Consider implementing a resolver compatible with this object's
        // interface, namely, that it does not support a 'close' or 'shutdown'
        // and 'linger' idiom.
        //
        // bsl::shared_ptr<ntcdns::Resolver> resolver;
        // resolver.createInplace(
        //     d_allocator_p, ntca::ResolverConfig(), d_allocator_p);
        //
        // resolver->start();
        //
        // d_resolver_sp = resolver;
    }

    if (d_user_sp) {
        d_connectionLimiter_sp = d_user_sp->connectionLimiter();
    }

    if (d_user_sp) {
        d_metrics_sp = d_user_sp->proactorMetrics();
    }

    d_interruptsHandler = 
        bdlf::MemFnUtil::memFn(&IoRing::interruptComplete, this);
}

IoRing::~IoRing()
{
    // Assert all timers and functions are executed.

    BSLS_ASSERT_OPT(!d_chronology.hasAnyDeferred());
    BSLS_ASSERT_OPT(!d_chronology.hasAnyScheduled());
    BSLS_ASSERT_OPT(!d_chronology.hasAnyRegistered());

    // Assert all waiters are deregistered.

    BSLS_ASSERT_OPT(d_waiterSet.empty());
}

ntci::Waiter IoRing::registerWaiter(const ntca::WaiterOptions& waiterOptions)
{
    IoRingWaiter* result =
        new (*d_allocator_p) IoRingWaiter(d_allocator_p);

    result->d_options = waiterOptions;

    bdlb::NullableValue<bslmt::ThreadUtil::Handle> principleThreadHandle;

    {
        LockGuard lockGuard(&d_waiterSetMutex);

        if (result->d_options.threadHandle() == bslmt::ThreadUtil::Handle()) {
            result->d_options.setThreadHandle(bslmt::ThreadUtil::self());
        }

        if (d_waiterSet.empty()) {
            d_threadHandle = result->d_options.threadHandle();
            principleThreadHandle.makeValue(d_threadHandle);

            if (!result->d_options.threadIndex().isNull()) {
                d_threadIndex = result->d_options.threadIndex().value();
            }
        }

        if (d_config.metricCollection().value()) {
            if (d_config.metricCollectionPerWaiter().value()) {
                if (result->d_options.metricName().empty()) {
                    bsl::stringstream ss;
                    ss << d_config.metricName().value() << "-"
                       << d_waiterSet.size();
                    result->d_options.setMetricName(ss.str());
                }

                bsl::shared_ptr<ntcs::ProactorMetrics> metrics;
                metrics.createInplace(d_allocator_p,
                                      "thread",
                                      result->d_options.metricName(),
                                      d_metrics_sp,
                                      d_allocator_p);

                result->d_metrics_sp = metrics;

                ntcm::MonitorableUtil::registerMonitorable(
                    result->d_metrics_sp);
            }
            else {
                result->d_metrics_sp = d_metrics_sp;
            }
        }

        d_waiterSet.insert(result);
    }

    if (!principleThreadHandle.isNull()) {
        d_threadId.store(bslmt::ThreadUtil::idAsUint64(
            bslmt::ThreadUtil::handleToId(principleThreadHandle.value())));
    }

    return result;
}

void IoRing::deregisterWaiter(ntci::Waiter waiter)
{
    IoRingWaiter* result = static_cast<IoRingWaiter*>(waiter);

    bool nowEmpty = false;

    {
        LockGuard lockGuard(&d_waiterSetMutex);

        bsl::size_t n = d_waiterSet.erase(result);
        BSLS_ASSERT_OPT(n == 1);

        if (d_waiterSet.empty()) {
            d_threadHandle = bslmt::ThreadUtil::invalidHandle();
            nowEmpty       = true;
        }
    }

    if (nowEmpty) {
        this->flush();
        d_threadId.store(0);
    }

    if (d_config.metricCollection().value()) {
        if (d_config.metricCollectionPerWaiter().value()) {
            ntcm::MonitorableUtil::deregisterMonitorable(result->d_metrics_sp);
        }
    }

    d_allocator_p->deleteObject(result);
}

bsl::shared_ptr<ntci::Strand> IoRing::createStrand(
    bslma::Allocator* basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<Proactor> self = this->getSelf(this);

    bsl::shared_ptr<ntcs::Strand> strand;
    strand.createInplace(allocator, self, allocator);

    return strand;
}

ntsa::Error IoRing::attachSocket(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    ntsa::Handle handle = socket->handle();

    if (handle == ntsa::k_INVALID_HANDLE) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    error = ntsu::SocketOptionUtil::setBlocking(handle, true);
    if (error) {
        NTCI_LOG_ERROR("I/O ring failed to set socket %d to blocking mode: %s",
                       handle,
                       error.text().c_str());
        return error;
    }

    bsl::shared_ptr<ntco::IoRingContext> context;
    context.createInplace(d_allocator_p, handle, d_allocator_p);

    {
        LockGuard lockGuard(&d_contextMapMutex);

        bool insertResult =
            d_contextMap.insert(ContextMap::value_type(socket, context))
                .second;

        if (!insertResult) {
            return ntsa::Error::invalid();
        }
    }

    socket->setProactorContext(context);

    return ntsa::Error();
}

ntsa::Error IoRing::accept(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntco::IoRingContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle = context->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

    ntco::IoRingSubmission entry;
    error = entry.prepareAccept(event.get(), socket, handle);
    if (error) {
        return error;
    }

    context->registerEvent(event.get());

    NTCO_IORING_LOG_EVENT_STARTING(event);

    error = this->submit(entry);
    if (error) {
        context->completeEvent(event.get());
        return error;
    }

    event.release();

    return ntsa::Error();
}

ntsa::Error IoRing::connect(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    const ntsa::Endpoint&                        endpoint)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntco::IoRingContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle = context->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

    ntco::IoRingSubmission entry;
    error = entry.prepareConnect(event.get(), socket, handle, endpoint);
    if (error) {
        return error;
    }

    context->registerEvent(event.get());

    NTCO_IORING_LOG_EVENT_STARTING(event);

    error = this->submit(entry);
    if (error) {
        context->completeEvent(event.get());
        return error;
    }

    event.release();

    return ntsa::Error();
}

ntsa::Error IoRing::send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                         const bdlbb::Blob&                           data,
                         const ntsa::SendOptions&                     options)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntco::IoRingContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle = context->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

    ntco::IoRingSubmission entry;
    error = entry.prepareSend(event.get(), socket, handle, data, options);
    if (error) {
        return error;
    }

    context->registerEvent(event.get());

    NTCO_IORING_LOG_EVENT_STARTING(event);

    error = this->submit(entry);
    if (error) {
        context->completeEvent(event.get());
        return error;
    }

    event.release();

    return ntsa::Error();
}

ntsa::Error IoRing::send(const bsl::shared_ptr<ntci::ProactorSocket>& socket,
                         const ntsa::Data&                            data,
                         const ntsa::SendOptions&                     options)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntco::IoRingContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle = context->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

    ntco::IoRingSubmission entry;
    error = entry.prepareSend(event.get(), socket, handle, data, options);
    if (error) {
        return error;
    }

    context->registerEvent(event.get());

    NTCO_IORING_LOG_EVENT_STARTING(event);

    error = this->submit(entry);
    if (error) {
        context->completeEvent(event.get());
        return error;
    }

    event.release();

    return ntsa::Error();
}

ntsa::Error IoRing::receive(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    bdlbb::Blob*                                 data,
    const ntsa::ReceiveOptions&                  options)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntco::IoRingContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle = context->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

    ntco::IoRingSubmission entry;
    error = entry.prepareReceive(event.get(), socket, handle, data, options);
    if (error) {
        return error;
    }

    context->registerEvent(event.get());

    NTCO_IORING_LOG_EVENT_STARTING(event);

    error = this->submit(entry);
    if (error) {
        context->completeEvent(event.get());
        return error;
    }

    event.release();

    return ntsa::Error();
}

ntsa::Error IoRing::shutdown(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket,
    ntsa::ShutdownType::Value                    direction)
{
    ntsa::Error error;

    bsl::shared_ptr<ntco::IoRingContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle = context->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    error = ntsu::SocketUtil::shutdown(direction, handle);
    if (error && error != ntsa::Error::e_INVALID) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error IoRing::cancel(const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
#if NTCO_IORING_CANCELLATION

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::shared_ptr<ntco::IoRingContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Handle handle = context->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    ntco::IoRingContext::EventList eventList;
    context->loadPending(&eventList, true);

    for (ntco::IoRingContext::EventList::const_iterator it = eventList.begin();
         it != eventList.end();
         ++it)
    {
        ntcs::Event* event = *it;

        NTCI_LOG_DEBUG("I/O ring cancelling event type %s",
                       ntcs::EventType::toString(event->d_type));

        if (event->d_status != ntcs::EventStatus::e_PENDING) {
            NTCI_LOG_DEBUG("I/O ring failed to cancel event type %s: "
                           "found event status %s, "
                           "expected event status PENDING",
                           ntcs::EventType::toString(event->d_type),
                           ntcs::EventStatus::toString(event->d_status),
                           event->d_error.text().c_str());
            continue;
        }

        BSLS_ASSERT(event->d_status == ntcs::EventStatus::e_PENDING);
        event->d_status = ntcs::EventStatus::e_CANCELLED;

        ntco::IoRingSubmission entry;
        entry.prepareCancellation(event);

        error = this->submit(entry);
        if (error) {
            return error;
        }
    }

#endif

    return ntsa::Error();
}

ntsa::Error IoRing::detachSocket(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    ntsa::Error error;

    bsl::shared_ptr<ntco::IoRingContext> context =
        bslstl::SharedPtrUtil::staticCast<ntco::IoRingContext>(
            socket->getProactorContext());
    if (!context) {
        return ntsa::Error();
    }

    ntsa::Handle handle = context->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    ntsu::SocketOptionUtil::setBlocking(handle, false);

    {
        LockGuard lockGuard(&d_contextMapMutex);

        bsl::size_t n = d_contextMap.erase(socket);
        if (n == 0) {
            return ntsa::Error();
        }
    }

    socket->setProactorContext(bsl::shared_ptr<void>());

    return ntsa::Error();
}

ntsa::Error IoRing::closeAll()
{
    ContextMap contextMap;
    {
        LockGuard lockGuard(&d_contextMapMutex);
        contextMap = d_contextMap;
    }

    for (ContextMap::iterator it = contextMap.begin(); it != contextMap.end();
         ++it)
    {
        const bsl::shared_ptr<ntci::ProactorSocket>& proactorSocket =
            it->first;
        proactorSocket->close();
    }

    contextMap.clear();

    return ntsa::Error();
}

void IoRing::incrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load += weight;
}

void IoRing::decrementLoad(const ntca::LoadBalancingOptions& options)
{
    bsl::size_t weight =
        options.weight().isNull() ? 1 : options.weight().value();

    d_load -= weight;
}

void IoRing::run(ntci::Waiter waiter)
{
    while (d_run) {
        // Wait for an operation to complete or a timeout.

        this->wait(waiter);

        // Invoke functions deferred while processing each polled event and
        // process all expired timers.

        bsl::size_t numCycles = d_config.maxCyclesPerWait().value();
        while (numCycles != 0) {
            if (d_chronology.hasAnyScheduledOrDeferred()) {
                d_chronology.announce();
                --numCycles;
            }
            else {
                break;
            }
        }
    }
}

void IoRing::poll(ntci::Waiter waiter)
{
    // Wait for an operation to complete or a timeout.

    this->wait(waiter);

    // Invoke functions deferred while processing each polled event and process
    // all expired timers.

    bsl::size_t numCycles = d_config.maxCyclesPerWait().value();
    while (numCycles != 0) {
        if (d_chronology.hasAnyScheduledOrDeferred()) {
            d_chronology.announce();
            --numCycles;
        }
        else {
            break;
        }
    }
}

void IoRing::interruptOne()
{
    if (NTCCFG_LIKELY(isWaiter())) {
        return;
    }

    NTCI_LOG_CONTEXT();

    unsigned int numInterruptsPending = d_interruptsPending;

    if (numInterruptsPending == 0) {
        return;
    }

    NTCI_LOG_TRACE("I/O ring submitting interrupt");

    ++d_interruptsPending;

    bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

    ntco::IoRingSubmission entry;
    entry.prepareCallback(event.get(), d_interruptsHandler);

    NTCO_IORING_LOG_EVENT_STARTING(event);

    ntsa::Error error = this->submit(entry);
    if (error) {
        --d_interruptsPending;
        return;
    }

    event.release();
}

void IoRing::interruptAll()
{
    NTCI_LOG_CONTEXT();

    unsigned int numInterruptsPending = d_interruptsPending;

    bsl::size_t numInterruptsToPost = 0;

    if (NTCCFG_LIKELY(d_config.maxThreads().value() == 1)) {
        if (NTCCFG_LIKELY(isWaiter())) {
            return;
        }

        if (numInterruptsPending == 0) {
            numInterruptsToPost = 1;
        }
    }
    else {
        bsl::size_t numWaiters;
        {
            LockGuard lockGuard(&d_waiterSetMutex);
            numWaiters = d_waiterSet.size();
        }

        if (numWaiters > numInterruptsPending) {
            numInterruptsToPost = numWaiters - numInterruptsPending;
        }
    }

    if (numInterruptsToPost == 0) {
        return;
    }

    NTCI_LOG_TRACE("I/O ring submitting %zu interrupt(s)",
                   numInterruptsToPost);

    for (bsl::size_t i = 0; i < numInterruptsToPost; ++i) {
        ++d_interruptsPending;

        bslma::ManagedPtr<ntcs::Event> event = d_eventPool.getManagedObject();

        ntco::IoRingSubmission entry;
        entry.prepareCallback(event.get(), d_interruptsHandler);

        ntsa::Error error = this->submit(entry);
        if (error) {
            --d_interruptsPending;
            continue;
        }

        event.release();
    }
}

void IoRing::stop()
{
    d_run = false;
    this->interruptAll();
}

void IoRing::restart()
{
    d_run = true;
}

void IoRing::drainFunctions()
{
    d_chronology.drain();
}

void IoRing::clearFunctions()
{
    d_chronology.clearFunctions();
}

void IoRing::clearTimers()
{
    d_chronology.clearTimers();
}

void IoRing::clearSockets()
{
    LockGuard lockGuard(&d_contextMapMutex);
    d_contextMap.clear();
}

void IoRing::clear()
{
    d_chronology.clear();

    LockGuard lockGuard(&d_contextMapMutex);
    d_contextMap.clear();
}

void IoRing::execute(const Functor& functor)
{
    d_chronology.defer(functor);
    this->interruptAll();
}

void IoRing::moveAndExecute(FunctorSequence* functorSequence,
                            const Functor&   functor)
{
    d_chronology.defer(functorSequence, functor);
    this->interruptAll();
}

bsl::shared_ptr<ntci::Timer> IoRing::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    return d_chronology.createTimer(options, session, basicAllocator);
}

bsl::shared_ptr<ntci::Timer> IoRing::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    return d_chronology.createTimer(options, callback, basicAllocator);
}

bsl::shared_ptr<ntci::DatagramSocket> IoRing::createDatagramSocket(
    const ntca::DatagramSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::DatagramSocket> datagramSocket;
    datagramSocket.createInplace(allocator,
                                 options,
                                 d_resolver_sp,
                                 this->getSelf(this),
                                 this->getSelf(this),
                                 metrics,
                                 allocator);

    return datagramSocket;
}

bsl::shared_ptr<ntci::ListenerSocket> IoRing::createListenerSocket(
    const ntca::ListenerSocketOptions& options,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::ListenerSocket> listenerSocket;
    listenerSocket.createInplace(allocator,
                                 options,
                                 d_resolver_sp,
                                 this->getSelf(this),
                                 this->getSelf(this),
                                 metrics,
                                 allocator);

    return listenerSocket;
}

bsl::shared_ptr<ntci::StreamSocket> IoRing::createStreamSocket(
    const ntca::StreamSocketOptions& options,
    bslma::Allocator*                basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    // MRM: Define how metrics are injected into sockets.
    bsl::shared_ptr<ntcs::Metrics> metrics;

    bsl::shared_ptr<ntcp::StreamSocket> streamSocket;
    streamSocket.createInplace(allocator,
                               options,
                               d_resolver_sp,
                               this->getSelf(this),
                               this->getSelf(this),
                               metrics,
                               allocator);

    return streamSocket;
}

bsl::shared_ptr<ntsa::Data> IoRing::createIncomingData()
{
    return d_dataPool_sp->createIncomingData();
}

bsl::shared_ptr<ntsa::Data> IoRing::createOutgoingData()
{
    return d_dataPool_sp->createOutgoingData();
}

bsl::shared_ptr<bdlbb::Blob> IoRing::createIncomingBlob()
{
    return d_dataPool_sp->createIncomingBlob();
}

bsl::shared_ptr<bdlbb::Blob> IoRing::createOutgoingBlob()
{
    return d_dataPool_sp->createOutgoingBlob();
}

void IoRing::createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createIncomingBlobBuffer(blobBuffer);
}

void IoRing::createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
{
    d_dataPool_sp->createOutgoingBlobBuffer(blobBuffer);
}

bsl::size_t IoRing::numSockets() const
{
    LockGuard lockGuard(&d_contextMapMutex);
    return d_contextMap.size();
}

bsl::size_t IoRing::maxSockets() const
{
    return static_cast<bsl::size_t>(-1);
}

bsl::size_t IoRing::numTimers() const
{
    return d_chronology.numScheduled();
}

bsl::size_t IoRing::maxTimers() const
{
    return static_cast<bsl::size_t>(-1);
}

bsl::size_t IoRing::load() const
{
    return static_cast<bsl::size_t>(d_load);
}

bslmt::ThreadUtil::Handle IoRing::threadHandle() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadHandle;
}

bsl::size_t IoRing::threadIndex() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_threadIndex;
}

bsl::size_t IoRing::numWaiters() const
{
    LockGuard lock(&d_waiterSetMutex);
    return d_waiterSet.size();
}

bool IoRing::empty() const
{
    if (d_chronology.hasAnyScheduledOrDeferred()) {
        return false;
    }

    if (d_chronology.hasAnyRegistered()) {
        return false;
    }

    if (this->numSockets() != 0) {
        return false;
    }

    return true;
}

const bsl::shared_ptr<ntci::DataPool>& IoRing::dataPool() const
{
    return d_dataPool_sp;
}

const bsl::shared_ptr<ntci::Strand>& IoRing::strand() const
{
    return ntci::Strand::unspecified();
}

bsls::TimeInterval IoRing::currentTime() const
{
    return bdlt::CurrentTime::now();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& IoRing::
    incomingBlobBufferFactory() const
{
    return d_dataPool_sp->incomingBlobBufferFactory();
}

const bsl::shared_ptr<bdlbb::BlobBufferFactory>& IoRing::
    outgoingBlobBufferFactory() const
{
    return d_dataPool_sp->outgoingBlobBufferFactory();
}

const char* IoRing::name() const
{
    return "IORING";
}

IoRingFactory::IoRingFactory(bslma::Allocator* basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

IoRingFactory::~IoRingFactory()
{
}

bsl::shared_ptr<ntci::Proactor> IoRingFactory::createProactor(
    const ntca::ProactorConfig&        configuration,
    const bsl::shared_ptr<ntci::User>& user,
    bslma::Allocator*                  basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<ntco::IoRing> proactor;
    proactor.createInplace(allocator, configuration, user, allocator);

    return proactor;
}

bool IoRingFactory::isSupported()
{
    return IoRingUtil::isSupported();
}

}  // close package namespace
}  // close enterprise namespace

#endif
#endif
