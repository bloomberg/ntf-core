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

#ifndef INCLUDED_NTCI_LOG
#define INCLUDED_NTCI_LOG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_likely.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsi_descriptor.h>
#include <bdlb_nullablevalue.h>
#include <bdlma_bufferedsequentialallocator.h>
#include <bdlsb_memoutstreambuf.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_types.h>
#include <bsl_cstdarg.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

/// @internal @brief
/// The size of the arena on the stack into which a log message is formmated.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_ARENA_SIZE ((bsl::size_t)(1024))

#if NTC_BUILD_WITH_LOGGING

/// Define the log context for the current thread and call frame.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT()                                                    \
    BloombergLP::ntci::LogContext* __LOG_CONTEXT__ =                          \
        BloombergLP::ntci::LogContext::getThreadLocal()

/// Log a formatted message at the specified 'severity'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG(severity, ...)                                               \
    do {                                                                      \
        if (__LOG_CONTEXT__) {                                                \
            if (NTCCFG_UNLIKELY((severity) <=                                 \
                BloombergLP::bsls::Log::severityThreshold()))                 \
            {                                                                 \
                BloombergLP::ntci::Log::write(__LOG_CONTEXT__,                \
                                              (severity),                     \
                                              __FILE__,                       \
                                              __LINE__,                       \
                                              __VA_ARGS__);                   \
            }                                                                 \
        }                                                                     \
    } while (false)

/// Log a pre-formatted message contained in the specified 'data' having the
/// specified 'size' at the specified 'severity'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_RAW(severity, data, size)                                    \
    do {                                                                      \
        if (__LOG_CONTEXT__) {                                                \
            if (NTCCFG_UNLIKELY((severity) <=                                 \
                BloombergLP::bsls::Log::severityThreshold()))                 \
            {                                                                 \
                BloombergLP::ntci::Log::writeRaw(__LOG_CONTEXT__,             \
                                                 (severity),                  \
                                                 __FILE__,                    \
                                                 __LINE__,                    \
                                                 data,                        \
                                                 size);                       \
            }                                                                 \
        }                                                                     \
    } while (false)

/// Log a formatted message at the fatal severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_FATAL(...)                                                   \
    NTCI_LOG(BloombergLP::bsls::LogSeverity::e_FATAL, __VA_ARGS__)

/// Log a formatted message at the error severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_ERROR(...)                                                   \
    NTCI_LOG(BloombergLP::bsls::LogSeverity::e_ERROR, __VA_ARGS__)

/// Log a formatted message at the warning severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_WARN(...)                                                    \
    NTCI_LOG(BloombergLP::bsls::LogSeverity::e_WARN, __VA_ARGS__)

/// Log a formatted message at the informational severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_INFO(...)                                                    \
    NTCI_LOG(BloombergLP::bsls::LogSeverity::e_INFO, __VA_ARGS__)

/// Log a formatted message at the debug severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_DEBUG(...)                                                   \
    NTCI_LOG(BloombergLP::bsls::LogSeverity::e_DEBUG, __VA_ARGS__)

/// Log a formatted message at the trace severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_TRACE(...)                                                   \
    NTCI_LOG(BloombergLP::bsls::LogSeverity::e_TRACE, __VA_ARGS__)

/// Attribute subsequent logging by this thread to the specified 'owner'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_OWNER(owner)                                   \
    BloombergLP::ntci::LogOwnerGuard __LOG_CONTEXT_GUARD_OWNER__(             \
        __LOG_CONTEXT__,                                                      \
        (owner))

/// Attribute subsequent logging by this thread to the specified
/// 'monitorIndex'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_MONITOR(monitorIndex)                          \
    BloombergLP::ntci::LogMonitorGuard __LOG_CONTEXT_GUARD_MONITOR__(         \
        __LOG_CONTEXT__,                                                      \
        (monitorIndex))

/// Attribute subsequent logging by this thread to the specified 'threadIndex'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex)                            \
    BloombergLP::ntci::LogThreadGuard __LOG_CONTEXT_GUARD_THREAD__(           \
        __LOG_CONTEXT__,                                                      \
        (threadIndex))

/// Attribute subsequent logging by this thread to the specified 'sourceId'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_SOURCE(sourceId)                               \
    BloombergLP::ntci::LogSourceGuard __LOG_CONTEXT_GUARD_SOURCE__(           \
        __LOG_CONTEXT__,                                                      \
        (sourceId))

/// Attribute subsequent logging by this thread to the specified 'channelId'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_CHANNEL(channelId)                             \
    BloombergLP::ntci::LogChannelGuard __LOG_CONTEXT_GUARD_CHANNEL__(         \
        __LOG_CONTEXT__,                                                      \
        (channelId))

/// Attribute subsequent logging by this thread to the specified
/// 'sessionHandle'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_SESSION(sessionHandle)                         \
    BloombergLP::ntci::LogSessionGuard __LOG_CONTEXT_GUARD_SESSION__(         \
        __LOG_CONTEXT__,                                                      \
        (sessionHandle))

/// Attribute subsequent logging by this thread to the specified
/// 'descriptorHandle'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(descriptorHandle)                   \
    BloombergLP::ntci::LogDescriptorGuard __LOG_CONTEXT_GUARD_DESCRIPTOR__(   \
        __LOG_CONTEXT__,                                                      \
        (descriptorHandle))

/// Attribute subsequent logging by this thread to the specified
/// 'sourceEndpoint'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(sourceEndpoint)                \
    BloombergLP::ntci::LogSourceEndpointGuard                                 \
    __LOG_CONTEXT_GUARD_SOURCE_ENDPOINT__(__LOG_CONTEXT__, (sourceEndpoint))

/// Attribute subsequent logging by this thread to the specified
/// 'remoteEndpoint'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(remoteEndpoint)                \
    BloombergLP::ntci::LogRemoteEndpointGuard                                 \
    __LOG_CONTEXT_GUARD_REMOTE_ENDPOINT__(__LOG_CONTEXT__, (remoteEndpoint))

#else

/// Define the log context for the current thread and call frame.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT()

/// Log a formatted message at the specified 'severity'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG(severity, ...)

/// Log a pre-formatted message contained in the specified 'data' having the
/// specified 'size' at the specified 'severity'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_RAW(severity, data, size)

/// Log a formatted message at the fatal severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_FATAL(...)

/// Log a formatted message at the error severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_ERROR(...)

/// Log a formatted message at the warning severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_WARN(...)

/// Log a formatted message at the informational severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_INFO(...)

/// Log a formatted message at the debug severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_DEBUG(...)

/// Log a formatted message at the trace severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_TRACE(...)

/// Attribute subsequent logging by this thread to the specified 'owner'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_OWNER(owner)

/// Attribute subsequent logging by this thread to the specified
/// 'monitorIndex'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_MONITOR(monitorIndex)

/// Attribute subsequent logging by this thread to the specified 'threadIndex'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_THREAD(threadIndex)

/// Attribute subsequent logging by this thread to the specified 'sourceId'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_SOURCE(sourceId)

/// Attribute subsequent logging by this thread to the specified 'channelId'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_CHANNEL(channelId)

/// Attribute subsequent logging by this thread to the specified
/// 'sessionHandle'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_SESSION(sessionHandle)

/// Attribute subsequent logging by this thread to the specified
/// 'descriptorHandle'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(descriptorHandle)

/// Attribute subsequent logging by this thread to the specified
/// 'sourceEndpoint'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(sourceEndpoint)

/// Attribute subsequent logging by this thread to the specified
/// 'remoteEndpoint'.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_CONTEXT_GUARD_REMOTE_ENDPOINT(remoteEndpoint)

#endif

/// Create an output stream to log at the fatal severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_STREAM_FATAL                                                 \
    if (NTCCFG_UNLIKELY(BloombergLP::bsls::LogSeverity::e_FATAL <=            \
                        BloombergLP::bsls::Log::severityThreshold()))         \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_FATAL;                          \
        NTCCFG_WARNING_UNUSED(ntclSeverity);                                  \
        bdlsb::MemOutStreamBuf ntclSb(bslma::Default::globalAllocator());     \
        {                                                                     \
            bsl::ostream ntclStream(&ntclSb);                                 \
            ntclStream

/// Create an output stream to log at the error severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_STREAM_ERROR                                                 \
    if (NTCCFG_UNLIKELY(BloombergLP::bsls::LogSeverity::e_ERROR <=            \
                        BloombergLP::bsls::Log::severityThreshold()))         \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_ERROR;                          \
        NTCCFG_WARNING_UNUSED(ntclSeverity);                                  \
        bdlsb::MemOutStreamBuf ntclSb(bslma::Default::globalAllocator());     \
        {                                                                     \
            bsl::ostream ntclStream(&ntclSb);                                 \
            ntclStream

/// Create an output stream to log at the warning severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_STREAM_WARN                                                  \
    if (NTCCFG_UNLIKELY(BloombergLP::bsls::LogSeverity::e_WARN <=             \
                        BloombergLP::bsls::Log::severityThreshold()))         \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_WARN;                           \
        NTCCFG_WARNING_UNUSED(ntclSeverity);                                  \
        bdlsb::MemOutStreamBuf ntclSb(bslma::Default::globalAllocator());     \
        {                                                                     \
            bsl::ostream ntclStream(&ntclSb);                                 \
            ntclStream

/// Create an output stream to log at the informational severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_STREAM_INFO                                                  \
    if (NTCCFG_UNLIKELY(BloombergLP::bsls::LogSeverity::e_INFO <=             \
                        BloombergLP::bsls::Log::severityThreshold()))         \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_INFO;                           \
        NTCCFG_WARNING_UNUSED(ntclSeverity);                                  \
        bdlsb::MemOutStreamBuf ntclSb(bslma::Default::globalAllocator());     \
        {                                                                     \
            bsl::ostream ntclStream(&ntclSb);                                 \
            ntclStream

/// Create an output stream to log at the debug severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_STREAM_DEBUG                                                 \
    if (NTCCFG_UNLIKELY(BloombergLP::bsls::LogSeverity::e_DEBUG <=            \
                        BloombergLP::bsls::Log::severityThreshold()))         \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_DEBUG;                          \
        NTCCFG_WARNING_UNUSED(ntclSeverity);                                  \
        bdlsb::MemOutStreamBuf ntclSb(bslma::Default::globalAllocator());     \
        {                                                                     \
            bsl::ostream ntclStream(&ntclSb);                                 \
            ntclStream

/// Create an output stream to log at the trace severity level.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_STREAM_TRACE                                                 \
    if (NTCCFG_UNLIKELY(BloombergLP::bsls::LogSeverity::e_TRACE <=            \
                        BloombergLP::bsls::Log::severityThreshold()))         \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_TRACE;                          \
        NTCCFG_WARNING_UNUSED(ntclSeverity);                                  \
        bdlsb::MemOutStreamBuf ntclSb(bslma::Default::globalAllocator());     \
        {                                                                     \
            bsl::ostream ntclStream(&ntclSb);                                 \
            ntclStream

/// Flush an output stream to the log.
///
/// @ingroup module_ntci_log
#define NTCI_LOG_STREAM_END                                                   \
    '\0' << bsl::flush;                                                       \
    }                                                                         \
    ntclSb.pubsync();                                                         \
    NTCI_LOG_RAW(ntclSeverity, ntclSb.data(), ntclSb.length());               \
    }

namespace BloombergLP {
namespace ntci {

/// @internal @brief
/// Describe the context of a log.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
struct LogContext {
    LogContext()
    : d_owner(0)
    {
    }
    const char*                         d_owner;
    bdlb::NullableValue<bsl::size_t>    d_monitorIndex;
    bdlb::NullableValue<bsl::size_t>    d_threadIndex;
    bdlb::NullableValue<int>            d_sourceId;
    bdlb::NullableValue<int>            d_channelId;
    bdlb::NullableValue<int>            d_sessionHandle;
    bdlb::NullableValue<ntsa::Handle>   d_descriptorHandle;
    bdlb::NullableValue<ntsa::Endpoint> d_sourceEndpoint;
    bdlb::NullableValue<ntsa::Endpoint> d_remoteEndpoint;

    /// Return the log context to use by the current thread, if any.
    static ntci::LogContext* getThreadLocal();
};

/// @internal @brief
/// Provide utilities for writing log entries within a particular context.
///
/// @details
/// This logging system supports stack-based logging contexts that supplement
/// log entries with the context information in which the log entry is written,
/// without the need to propogate such information manually.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example
/// The following example illustrates how to define the "context" in which
/// subsequent log entries will be in deeper in the call stack. First, let's
/// define a function, 'bar()', that loads the current thread's log context
/// then writes a message to the log.
///
///     void bar()
///     {
///         NTCI_LOG_CONTEXT();
///
///         NTCI_LOG_INFO("Hello, world!");
///     }
///
/// Now, let's define a function, 'foo()', that loads the current thread's log
/// context, then guards the setting/resetting of the current descriptor,
/// source endpoint, and remote endpoint context, then invokes our function,
/// 'bar()', defined earlier.
///
///     void foo()
///     {
///         NTCI_LOG_CONTEXT();
///
///         NTCI_LOG_CONTEXT_GUARD_DESCRIPTOR(3);
///
///         NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(
///             ntsa::Endpoint("10.20.30.40:1234"));
///
///         NTCI_LOG_CONTEXT_GUARD_SOURCE_ENDPOINT(
///             ntsa::Endpoint("50.60.70.80:5678"));
///
///         bar();
///     }
///
/// When 'foo' is invoked, the output in the log will be:
///
///     Descriptor 3 at 10.20.30.40:1234 to 50.60.70.80:5678: Hello, world!
///
/// @ingroup module_ntci_log
struct Log {
    /// Initialize the log and configure it to use an in-memory journal
    /// according to the specified 'journal' flag.
    static void initialize(bool journal);

    /// Log the specified 'format' in the specified 'context'.
    static void write(const ntci::LogContext* logContext,
                      bsls::LogSeverity::Enum severity,
                      const char*             file,
                      int                     line,
                      const char*             format,
                      ...);

    /// Log the specified 'format' in the specified 'context'.
    static void writeArgs(const ntci::LogContext* logContext,
                          bsls::LogSeverity::Enum severity,
                          const char*             file,
                          int                     line,
                          const char*             format,
                          bsl::va_list            args);

    /// Log the specified 'data' having the specified 'size'.
    static void writeRaw(const ntci::LogContext* logContext,
                         bsls::LogSeverity::Enum severity,
                         const char*             file,
                         int                     line,
                         const char*             data,
                         bsl::size_t             size);

    /// Ensure any data buffered in the log is flushed to the log
    /// device.
    static void flush();

    /// Clean up the log.
    static void exit();
};

/// @internal @brief
/// Provide utilities for the implementation of logging.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_log
struct LogUtil {
    /// Format the specified 'context' into the specified 'destination'
    /// having the specified 'destinationCapacity'. Always null-terminate
    /// 'destination' but never exceed 'destianationCapacity'. The behavior
    /// is undefined unless 'destinationCapacity > 0'.
    static bsl::size_t formatContext(char*                   destination,
                                     bsl::size_t             capacity,
                                     const ntci::LogContext& context);

    /// Copy to the specified 'destination' having the specified 'capacity'
    /// the specified 'format' string replacing the escaped characters with
    /// the values in their respective variable number of arguments. Return
    /// the number of characters written.
    static bsl::size_t formatBuffer(char*       destination,
                                    bsl::size_t capacity,
                                    const char* format,
                                    ...);

    /// Copy to the specified 'destination' having the specified 'capacity'
    /// the specified 'format' string replacing the escaped characters with
    /// the values in their respective variable number of arguments. Return
    /// the number of characters written.
    static bsl::size_t formatBufferArgs(char*        destination,
                                        bsl::size_t  capacity,
                                        const char*  format,
                                        bsl::va_list args);
};

/// @internal @brief
/// Provide a guard to automatically set and reset the owner field of a log
/// context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogOwnerGuard
{
    ntci::LogContext* d_logContext_p;
    const char*       d_previous;

  private:
    LogOwnerGuard(const LogOwnerGuard&) BSLS_KEYWORD_DELETED;
    LogOwnerGuard& operator=(const LogOwnerGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified 'owner'
    /// in the specified 'logContext' and unsets it when this object is
    /// destroyed.
    explicit LogOwnerGuard(ntci::LogContext* logContext, const char* owner);

    /// Unset the owner in the underlying log context then destroy this
    /// object.
    ~LogOwnerGuard();
};

/// @internal @brief
/// Provide a guard to automatically set and reset the driver (e.g. proactor
/// or reactor) index field of a log context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogMonitorGuard
{
    ntci::LogContext*                d_logContext_p;
    bdlb::NullableValue<bsl::size_t> d_previous;

  private:
    LogMonitorGuard(const LogMonitorGuard&) BSLS_KEYWORD_DELETED;
    LogMonitorGuard& operator=(const LogMonitorGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified
    /// 'monitorIndex' in the specified 'logContext' and unsets it when this
    /// object is destroyed.
    LogMonitorGuard(ntci::LogContext* logContext, bsl::size_t monitorIndex);

    /// Unset the monitor index in the underlying log context then destroy
    /// this object.
    ~LogMonitorGuard();
};

/// @internal @brief
/// Provide a guard to automatically set and reset the thread index field of a
/// log context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogThreadGuard
{
    ntci::LogContext*                d_logContext_p;
    bdlb::NullableValue<bsl::size_t> d_previous;

  private:
    LogThreadGuard(const LogThreadGuard&) BSLS_KEYWORD_DELETED;
    LogThreadGuard& operator=(const LogThreadGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified
    /// 'threadIndex' in the specified 'logContext' and unsets it when this
    /// object is destroyed.
    LogThreadGuard(ntci::LogContext* logContext, bsl::size_t threadIndex);

    /// Unset the thread index in the underlying log context then destroy
    /// this object.
    ~LogThreadGuard();
};

/// @internal @brief
/// Provide a guard to automatically set and reset the source ID field of a log
/// context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogSourceGuard
{
    ntci::LogContext*        d_logContext_p;
    bdlb::NullableValue<int> d_previous;

  private:
    LogSourceGuard(const LogSourceGuard&) BSLS_KEYWORD_DELETED;
    LogSourceGuard& operator=(const LogSourceGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified 'sourceId'
    /// in the specified 'logContext' and unsets it when this object is
    /// destroyed.
    LogSourceGuard(ntci::LogContext* logContext, int sourceId);

    /// Unset the source ID in the underlying log context then destroy this
    /// object.
    ~LogSourceGuard();
};

/// @internal @brief
/// Provide a guard to automatically set and reset the channel ID field of a
/// log context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogChannelGuard
{
    ntci::LogContext*        d_logContext_p;
    bdlb::NullableValue<int> d_previous;

  private:
    LogChannelGuard(const LogChannelGuard&) BSLS_KEYWORD_DELETED;
    LogChannelGuard& operator=(const LogChannelGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified 'channelId'
    /// in the specified 'logContext' and unsets it when this object is
    /// destroyed.
    LogChannelGuard(ntci::LogContext* logContext, int channelId);

    /// Unset the channel ID in the underlying log context then destroy this
    /// object.
    ~LogChannelGuard();
};

/// @internal @brief
/// Provide a guard to automatically set and reset the session handle field of
/// a log context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogSessionGuard
{
    ntci::LogContext*        d_logContext_p;
    bdlb::NullableValue<int> d_previous;

  private:
    LogSessionGuard(const LogSessionGuard&) BSLS_KEYWORD_DELETED;
    LogSessionGuard& operator=(const LogSessionGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified
    /// 'sessionHandle' in the specified 'logContext' and unsets it when
    /// this object is destroyed.
    LogSessionGuard(ntci::LogContext* logContext, int sessionHandle);

    /// Unset the session handle in the underlying log context then destroy
    /// this object.
    ~LogSessionGuard();
};

/// @internal @brief
/// Provide a guard to automatically set and reset the descriptor handle field
/// of a log context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogDescriptorGuard
{
    ntci::LogContext*                 d_logContext_p;
    bdlb::NullableValue<ntsa::Handle> d_previous;

  private:
    LogDescriptorGuard(const LogDescriptorGuard&) BSLS_KEYWORD_DELETED;
    LogDescriptorGuard& operator=(const LogDescriptorGuard&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified
    /// 'descriptorHandle' in the specified 'logContext' and unsets it when
    /// this object is destroyed.
    LogDescriptorGuard(ntci::LogContext* logContext,
                       ntsa::Handle      descriptorHandle);

    /// Unset the descriptor handle in the underlying log context then
    /// destroy this object.
    ~LogDescriptorGuard();
};

/// @internal @brief
/// Provide a guard to automatically set and reset the source endpoing field
/// of a log context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogSourceEndpointGuard
{
    ntci::LogContext*                   d_logContext_p;
    bdlb::NullableValue<ntsa::Endpoint> d_previous;

  private:
    LogSourceEndpointGuard(const LogSourceEndpointGuard&) BSLS_KEYWORD_DELETED;
    LogSourceEndpointGuard& operator=(const LogSourceEndpointGuard&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified
    /// 'sourceEndpoint' in the specified 'logContext' and unsets it when
    /// this object is destroyed.
    LogSourceEndpointGuard(ntci::LogContext*     logContext,
                           const ntsa::Endpoint& sourceEndpoint);

    /// Unset the source endpoint in the underlying log context then
    /// destroy this object.
    ~LogSourceEndpointGuard();
};

/// @internal @brief
/// Provide a guard to automatically set and reset the remote endpoint field
/// of a log context.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_log
class LogRemoteEndpointGuard
{
    ntci::LogContext*                   d_logContext_p;
    bdlb::NullableValue<ntsa::Endpoint> d_previous;

  private:
    LogRemoteEndpointGuard(const LogRemoteEndpointGuard&) BSLS_KEYWORD_DELETED;
    LogRemoteEndpointGuard& operator=(const LogRemoteEndpointGuard&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard that automatically sets the specified
    /// 'remoteEndpoint' in the specified 'logContext' and unsets it when
    /// this object is destroyed.
    LogRemoteEndpointGuard(ntci::LogContext*     logContext,
                           const ntsa::Endpoint& remoteEndpoint);

    /// Unset the remote endpoint in the underlying log context then
    /// destroy this object.
    ~LogRemoteEndpointGuard();
};

NTCCFG_INLINE
LogOwnerGuard::LogOwnerGuard(ntci::LogContext* logContext, const char* owner)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous              = d_logContext_p->d_owner;
        d_logContext_p->d_owner = owner;
    }
}

NTCCFG_INLINE
LogOwnerGuard::~LogOwnerGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_owner = d_previous;
    }
}

NTCCFG_INLINE
LogMonitorGuard::LogMonitorGuard(ntci::LogContext* logContext,
                                 bsl::size_t       monitorIndex)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous                     = d_logContext_p->d_monitorIndex;
        d_logContext_p->d_monitorIndex = monitorIndex;
    }
}

NTCCFG_INLINE
LogMonitorGuard::~LogMonitorGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_monitorIndex = d_previous;
    }
}

NTCCFG_INLINE
LogThreadGuard::LogThreadGuard(ntci::LogContext* logContext,
                               bsl::size_t       threadIndex)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous                    = d_logContext_p->d_threadIndex;
        d_logContext_p->d_threadIndex = threadIndex;
    }
}

NTCCFG_INLINE
LogThreadGuard::~LogThreadGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_threadIndex = d_previous;
    }
}

NTCCFG_INLINE
LogSourceGuard::LogSourceGuard(ntci::LogContext* logContext, int sourceId)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous                 = d_logContext_p->d_sourceId;
        d_logContext_p->d_sourceId = sourceId;
    }
}

NTCCFG_INLINE
LogSourceGuard::~LogSourceGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_sourceId = d_previous;
    }
}

NTCCFG_INLINE
LogChannelGuard::LogChannelGuard(ntci::LogContext* logContext, int channelId)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous                  = d_logContext_p->d_channelId;
        d_logContext_p->d_channelId = channelId;
    }
}

NTCCFG_INLINE
LogChannelGuard::~LogChannelGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_channelId = d_previous;
    }
}

NTCCFG_INLINE
LogSessionGuard::LogSessionGuard(ntci::LogContext* logContext,
                                 int               sessionHandle)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous                      = d_logContext_p->d_sessionHandle;
        d_logContext_p->d_sessionHandle = sessionHandle;
    }
}

NTCCFG_INLINE
LogSessionGuard::~LogSessionGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_sessionHandle = d_previous;
    }
}

NTCCFG_INLINE
LogDescriptorGuard::LogDescriptorGuard(ntci::LogContext* logContext,
                                       ntsa::Handle      descriptorHandle)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous = d_logContext_p->d_descriptorHandle;
        d_logContext_p->d_descriptorHandle = descriptorHandle;
    }
}

NTCCFG_INLINE
LogDescriptorGuard::~LogDescriptorGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_descriptorHandle = d_previous;
    }
}

NTCCFG_INLINE
LogSourceEndpointGuard::LogSourceEndpointGuard(
    ntci::LogContext*     logContext,
    const ntsa::Endpoint& sourceEndpoint)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous                       = d_logContext_p->d_sourceEndpoint;
        d_logContext_p->d_sourceEndpoint = sourceEndpoint;
    }
}

NTCCFG_INLINE
LogSourceEndpointGuard::~LogSourceEndpointGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_sourceEndpoint = d_previous;
    }
}

NTCCFG_INLINE
LogRemoteEndpointGuard::LogRemoteEndpointGuard(
    ntci::LogContext*     logContext,
    const ntsa::Endpoint& remoteEndpoint)
: d_logContext_p(logContext)
{
    if (d_logContext_p) {
        d_previous                       = d_logContext_p->d_remoteEndpoint;
        d_logContext_p->d_remoteEndpoint = remoteEndpoint;
    }
}

NTCCFG_INLINE
LogRemoteEndpointGuard::~LogRemoteEndpointGuard()
{
    if (d_logContext_p) {
        d_logContext_p->d_remoteEndpoint = d_previous;
    }
}

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
