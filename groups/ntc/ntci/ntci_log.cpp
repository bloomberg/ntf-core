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

#include <ntci_log.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_log_cpp, "$Id$ $CSID$")

#include <ntccfg_format.h>
#include <ntccfg_tune.h>
#include <bdlb_chartype.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_spinlock.h>
#include <bsl_cstdarg.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>

#if defined(BSLS_PLATFORM_OS_LINUX)
#include <sys/syscall.h>
#include <unistd.h>
#endif

#define NTCI_LOG_UNSET_CONTEXT 0

// Define to 1 to use 'vsprintf'. Uncomment, undefine or set to zero to use
// the custom formatting functions.
#define NTCI_LOG_VSPRINTF 1

namespace BloombergLP {
namespace ntci {

/// Describe the process-wide log state.
class LogState
{
  public:
    /// Perform process-wide initialization for this component.
    LogState();

    /// Perform process-wide cleanup for this component.
    ~LogState();

    /// Allocate memory for a new log context.
    static void* allocateLogContext();

    /// Free the memory for a log context at the specified 'address'.
    static void freeLogContext(void* address);

    /// Log the specified 'format' having the specified 'args' in the
    /// specified 'logContext'.
    static void writeArgs(const ntci::LogContext* logContext,
                          bsls::LogSeverity::Enum severity,
                          const char*             file,
                          int                     line,
                          const char*             format,
                          bsl::va_list            args);

    /// Log the specified 'format' having the specified 'args' in the
    /// specified 'logContext'.
    static void writeRaw(const ntci::LogContext* logContext,
                         bsls::LogSeverity::Enum severity,
                         const char*             file,
                         int                     line,
                         const char*             data,
                         bsl::size_t             size);

    // Flush the log state.
    static void flush();

    /// The lock synchronizing access to the journal object.
    bsls::SpinLock d_lock;

    /// The process-wide log context thread-local key.
    bslmt::ThreadUtil::Key d_key;

    /// The journal.
    LogJournal* d_journal;

#if defined(BSLS_PLATFORM_OS_LINUX)
    // The memory for the main thread's log context.
    alignas(16) char d_mainThreadContext[sizeof(ntci::LogContext)];
#endif

    /// The process-wide log state.
    static LogState s_global;

  private:
    LogState(const LogState&);
    LogState& operator=(const LogState&);

    /// Destroy the log context at the specified 'key'.
    static void destroyKey(void* key);
};

LogState LogState::s_global;

LogState::LogState()
: d_lock(bsls::SpinLock::s_unlocked)
, d_key()
, d_journal(0)
{
    bsls::SpinLockGuard guard(&d_lock);

    int rc = bslmt::ThreadUtil::createKey(&d_key, &LogState::destroyKey);
    BSLS_ASSERT_OPT(rc == 0);

    bool enableJournal = false;
    if (ntccfg::Tune::configure(&enableJournal, "NTC_LOG_JOURNAL")) {
        if (enableJournal) {
            void* arena = bsl::malloc(sizeof(LogJournal));
            d_journal   = new (arena)
                LogJournal(&bslma::NewDeleteAllocator::singleton());
        }
    }
}

LogState::~LogState()
{
    bsls::SpinLockGuard guard(&d_lock);

    if (d_journal) {
        d_journal->flush();
        d_journal->~LogJournal();
        bsl::free(d_journal);
        d_journal = 0;
    }

    // int rc = bslmt::ThreadUtil::deleteKey(d_key);
    // BSLS_ASSERT_OPT(rc == 0);
}

void* LogState::allocateLogContext()
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    const pid_t tid = (pid_t)(syscall(SYS_gettid));
    const pid_t pid = (pid_t)(getpid());

    if (tid == pid) {
        BSLS_ASSERT_OPT((bsl::size_t)(bsl::uintptr_t)(
                            LogState::s_global.d_mainThreadContext) %
                            16 ==
                        0);

        return (void*)(LogState::s_global.d_mainThreadContext);
    }
    else {
        return bsl::malloc(sizeof(ntci::LogContext));
    }

#else

    return bsl::malloc(sizeof(ntci::LogContext));

#endif
}

void LogState::freeLogContext(void* address)
{
#if defined(BSLS_PLATFORM_OS_LINUX)

    if (address != 0) {
        if (address != (void*)LogState::s_global.d_mainThreadContext) {
            bsl::free(address);
        }
    }

#else

    if (address != 0) {
        bsl::free(address);
    }

#endif
}

void LogState::destroyKey(void* key)
{
    if (key != 0) {
        typedef ntci::LogContext Type;

        Type* logContext = reinterpret_cast<Type*>(key);
        logContext->~Type();

        LogState::freeLogContext(key);
    }
}

void LogState::writeArgs(const ntci::LogContext* logContext,
                         bsls::LogSeverity::Enum severity,
                         const char*             file,
                         int                     line,
                         const char*             format,
                         bsl::va_list            args)
{
    bsls::SpinLockGuard guard(&s_global.d_lock);

    if (NTCCFG_UNLIKELY(s_global.d_journal)) {
        s_global.d_journal
            ->writeArgs(logContext, severity, file, line, format, args);
    }
    else {
        LogRecord record;
        record.writeArgs(logContext, severity, file, line, format, args);
        record.flush();
    }
}

void LogState::writeRaw(const ntci::LogContext* logContext,
                        bsls::LogSeverity::Enum severity,
                        const char*             file,
                        int                     line,
                        const char*             data,
                        bsl::size_t             size)
{
    bsls::SpinLockGuard guard(&s_global.d_lock);

    if (NTCCFG_UNLIKELY(s_global.d_journal)) {
        s_global.d_journal
            ->writeRaw(logContext, severity, file, line, data, size);
    }
    else {
        LogRecord record;
        record.writeRaw(logContext, severity, file, line, data, size);
        record.flush();
    }
}

void LogState::flush()
{
    bsls::SpinLockGuard guard(&s_global.d_lock);

    if (NTCCFG_UNLIKELY(s_global.d_journal)) {
        s_global.d_journal->flush();
    }
}

LogContext::LogContext()
: d_owner(0)
, d_monitorIndex()
, d_threadIndex()
, d_sourceId()
, d_channelId()
, d_sessionHandle()
, d_descriptorHandle()
, d_sourceEndpoint()
, d_remoteEndpoint()
{
}

LogContext::~LogContext()
{
}

bsl::size_t LogContext::format(char* destination, bsl::size_t capacity) const
{
    bdlsb::FixedMemOutStreamBuf osb(destination, capacity);

    {
        bsl::ostream os(&osb);

        bool empty = true;

        if (d_owner) {
            if (!empty) {
                os << ' ';
            }
            os << "scheduler '" << d_owner << "'";
            empty = false;
        }

        if (!d_monitorIndex.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "monitor " << d_monitorIndex.value();
            empty = false;
        }

        if (!d_threadIndex.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "thread " << d_threadIndex.value();
            empty = false;
        }

        if (!d_sourceId.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "source " << d_sourceId.value();
            empty = false;
        }

        if (!d_channelId.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "channel " << d_channelId.value();
            empty = false;
        }

        if (!d_sessionHandle.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "session " << d_sessionHandle.value();
            empty = false;
        }

        if (!d_descriptorHandle.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "descriptor " << d_descriptorHandle.value();
            empty = false;
        }

        if (!d_sourceEndpoint.isNull()) {
            if (!d_sourceEndpoint.value().isUndefined()) {
                if (!empty) {
                    os << ' ';
                }
                os << "at " << d_sourceEndpoint.value();
                empty = false;
            }
        }

        if (!d_remoteEndpoint.isNull()) {
            if (!d_remoteEndpoint.value().isUndefined()) {
                if (!empty) {
                    os << ' ';
                }
                os << "to " << d_remoteEndpoint.value();
                empty = false;
            }
        }

        if (!empty) {
            os << ": ";
        }
    }

    osb.pubsync();

    bsl::size_t n = static_cast<bsl::size_t>(osb.length());

    if (n != 0) {
        if (n == capacity) {
            --n;
        }

        destination[0] =
            static_cast<char>(bdlb::CharType::toUpper(destination[0]));
    }

    destination[n] = 0;

    return n;
}

ntci::LogContext* LogContext::getThreadLocal()
{
    int rc;

    ntci::LogContext* current = reinterpret_cast<ntci::LogContext*>(
        bslmt::ThreadUtil::getSpecific(LogState::s_global.d_key));

    if (current == 0) {
        void* arena = LogState::allocateLogContext();

        current = new (arena) ntci::LogContext();

        rc = bslmt::ThreadUtil::setSpecific(
            LogState::s_global.d_key,
            const_cast<const void*>(static_cast<void*>(current)));
        BSLS_ASSERT_OPT(rc == 0);
    }

    return current;
}

LogRecord::LogRecord()
: d_severity(bsls::LogSeverity::e_TRACE)
, d_file(0)
, d_line(0)
, d_length(0)
{
    d_buffer[0] = 0;
}

void LogRecord::write(const ntci::LogContext* logContext,
                      bsls::LogSeverity::Enum severity,
                      const char*             file,
                      int                     line,
                      const char*             format,
                      ...)
{
    bsl::va_list args;
    va_start(args, format);

    this->writeArgs(logContext, severity, file, line, format, args);

    va_end(args);
}

void LogRecord::writeArgs(const ntci::LogContext* logContext,
                          bsls::LogSeverity::Enum severity,
                          const char*             file,
                          int                     line,
                          const char*             format,
                          bsl::va_list            args)
{
    d_severity = severity;
    d_file     = file;
    d_line     = line;
    d_length   = 0;

    bsl::size_t n = 0;
    if (logContext) {
        n = logContext->format(d_buffer, sizeof d_buffer);
    }

#if NTCI_LOG_UNSET_CONTEXT
    if (n == 0) {
        const char UNSET_BANNER[] = "****** NTC UNSET CONTEXT ******: ";
        bsl::memcpy(d_buffer, UNSET_BANNER, sizeof UNSET_BANNER - 1);
        n = sizeof UNSET_BANNER - 1;
    }
#endif

#if NTCI_LOG_VSPRINTF

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY)
    int rc = bsl::vsnprintf(d_buffer + n, (sizeof d_buffer) - n, format, args);
    BSLS_ASSERT(rc >= 0);
    if (rc > static_cast<int>((sizeof d_buffer) - n - 1)) {
        d_buffer[(sizeof d_buffer) - 1] = 0;
        d_length                        = (sizeof d_buffer) - 1;
    }
    else {
        d_length = n + rc;
    }
#else
    int rc   = vsprintf(d_buffer + n, format, args);
    d_length = n + rc;
#endif

#else

    n        += ntccfg::Format::printArgs(d_buffer + n,
                                   (sizeof d_buffer) - n,
                                   format,
                                   args);
    d_length  = n;

#endif
}

void LogRecord::writeRaw(const ntci::LogContext* logContext,
                         bsls::LogSeverity::Enum severity,
                         const char*             file,
                         int                     line,
                         const char*             data,
                         bsl::size_t             size)
{
    d_severity = severity;
    d_file     = file;
    d_line     = line;
    d_length   = 0;

    bsl::size_t n = 0;
    if (logContext) {
        n = logContext->format(d_buffer, sizeof d_buffer);
    }

#if NTCI_LOG_UNSET_CONTEXT
    if (n == 0) {
        const char UNSET_BANNER[] = "****** NTC UNSET CONTEXT ******: ";
        bsl::memcpy(d_buffer, UNSET_BANNER, sizeof UNSET_BANNER - 1);
        n = sizeof UNSET_BANNER - 1;
    }
#endif

    const bsl::size_t numAvailable = sizeof d_buffer - n;

    bsl::size_t numToCopy = size;
    if (numToCopy > numAvailable) {
        numToCopy = numAvailable;
    }

    bsl::memcpy(d_buffer + n, data, numToCopy);
    n += numToCopy;

    BSLS_ASSERT(n <= sizeof d_buffer);

    if (n == sizeof d_buffer) {
        --n;
    }

    d_buffer[n] = 0;

    d_length = n;
}

void LogRecord::flush()
{
    bsls::Log::logMessage(d_severity, d_file, d_line, d_buffer);
}

LogJournal::LogJournal(bslma::Allocator* basicAllocator)
: d_mutex()
, d_records(basicAllocator)
, d_position(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_records.resize(LogJournal::k_MAX_LOG_RECORDS);
}

LogJournal::~LogJournal()
{
}

void LogJournal::write(const ntci::LogContext* logContext,
                       bsls::LogSeverity::Enum severity,
                       const char*             file,
                       int                     line,
                       const char*             format,
                       ...)
{
    bsl::va_list args;
    va_start(args, format);

    this->writeArgs(logContext, severity, file, line, format, args);

    va_end(args);
}

void LogJournal::writeArgs(const ntci::LogContext* logContext,
                           bsls::LogSeverity::Enum severity,
                           const char*             file,
                           int                     line,
                           const char*             format,
                           bsl::va_list            args)
{
    LockGuard guard(&d_mutex);

    d_records[d_position]
        .writeArgs(logContext, severity, file, line, format, args);

    ++d_position;

    if (d_position == LogJournal::k_MAX_LOG_RECORDS) {
        for (int i = 0; i < d_position; ++i) {
            d_records[i].flush();
        }

        d_position = 0;
    }
}

void LogJournal::writeRaw(const ntci::LogContext* logContext,
                          bsls::LogSeverity::Enum severity,
                          const char*             file,
                          int                     line,
                          const char*             data,
                          bsl::size_t             size)
{
    LockGuard guard(&d_mutex);

    d_records[d_position]
        .writeRaw(logContext, severity, file, line, data, size);

    ++d_position;

    if (d_position == LogJournal::k_MAX_LOG_RECORDS) {
        for (int i = 0; i < d_position; ++i) {
            d_records[i].flush();
        }

        d_position = 0;
    }
}

void LogJournal::flush()
{
    LockGuard guard(&d_mutex);

    for (int i = 0; i < d_position; ++i) {
        BSLS_ASSERT(d_position < LogJournal::k_MAX_LOG_RECORDS);
        d_records[i].flush();
    }

    d_position = 0;
}

void Log::initialize(bool journal)
{
    NTCCFG_WARNING_UNUSED(journal);
}

void Log::write(const ntci::LogContext* logContext,
                bsls::LogSeverity::Enum severity,
                const char*             file,
                int                     line,
                const char*             format,
                ...)
{
    bsl::va_list args;
    va_start(args, format);

    LogState::writeArgs(logContext, severity, file, line, format, args);

    va_end(args);
}

void Log::writeArgs(const ntci::LogContext* logContext,
                    bsls::LogSeverity::Enum severity,
                    const char*             file,
                    int                     line,
                    const char*             format,
                    bsl::va_list            args)
{
    LogState::writeArgs(logContext, severity, file, line, format, args);
}

void Log::writeRaw(const ntci::LogContext* logContext,
                   bsls::LogSeverity::Enum severity,
                   const char*             file,
                   int                     line,
                   const char*             data,
                   bsl::size_t             size)
{
    LogState::writeRaw(logContext, severity, file, line, data, size);
}

void Log::flush()
{
    LogState::flush();
}

void Log::exit()
{
}

}  // close package namespace
}  // close enterprise namespace
