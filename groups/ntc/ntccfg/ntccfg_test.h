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

#ifndef INCLUDED_NTCCFG_TEST
#define INCLUDED_NTCCFG_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocator.h>
#include <bslmt_threadutil.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>

#if NTC_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR
#include <balst_stacktracetestallocator.h>
#endif

namespace BloombergLP {
namespace ntccfg {

/// @internal @brief
/// The test case number.
///
/// @ingroup module_ntccfg
extern int testCase;

/// @internal @brief
/// The test verbosity level.
///
/// @ingroup module_ntccfg
extern int testVerbosity;

/// @internal @brief
/// The number of arguments specified when the test driver was executed.
///
/// @ingroup module_ntccfg
extern int testArgc;

/// @internal @brief
/// The argument vector specified when the test driver was executed.
///
/// @ingroup module_ntccfg
extern char** testArgv;

/// @internal @brief
/// Defines a type alias for a function invoked to initialize any global state
/// used by the test driver.
///
/// @ingroup module_ntccfg
typedef void (*TestInitCallback)();

/// Defines a type alias for a function invoked to clean up any global state
/// used by the test driver.
///
/// @ingroup module_ntccfg
typedef void (*TestExitCallback)();

/// The function invoked to initialize any global state used by the test
/// driver.
///
/// @ingroup module_ntccfg
extern TestInitCallback testInit;

/// The function invoked to clean up any global state used by the test driver.
///
/// @ingroup module_ntccfg
extern TestExitCallback testExit;

/// @internal @brief
/// Provide a guard to automatically call any registered initialization or
/// exit functions in the context of a test driver.
///
/// @ingroup module_ntccfg
struct TestGuard {
    /// Construct the guard and automatically call any registered
    /// initialization function.
    TestGuard()
    {
        if (testInit) {
            testInit();
        }
    }

    /// Construct the guard and automatically call any registered exit
    /// function.
    ~TestGuard()
    {
        if (testExit) {
            testExit();
        }
    }
};

#if NTC_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR

/// @internal @brief
/// Provide an allocator suitable for a test driver.
///
/// @details
/// Provide an implementation of the 'bslma::Allocator'
/// protocol suitable for use as a test allocator for this library.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
class TestAllocator : public bslma::Allocator
{
    balst::StackTraceTestAllocator d_base;

  public:
    /// Create a new test allocator.
    TestAllocator();

    /// Destroy this object.
    virtual ~TestAllocator();

    /// Return a newly allocated block of memory of (at least) the specified
    /// positive 'size' (in bytes).  If 'size' is 0, a null pointer is
    /// returned with no other effect.  If this allocator cannot return the
    /// requested number of bytes, then it will throw a 'bsl::bad_alloc'
    /// exception in an exception-enabled build, or else will abort the
    /// program in a non-exception build.  The behavior is undefined unless
    /// '0 <= size'.  Note that the alignment of the address returned
    /// conforms to the platform requirement for any object of the specified
    /// 'size'.
    virtual void* allocate(size_type size);

    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.
    virtual void deallocate(void* address);

    /// Return the number of blocks currently allocated from this object.
    bsl::int64_t numBlocksInUse() const;
};

#else

/// @internal @brief
/// Provide an allocator suitable for a test driver.
///
/// @details
/// Provide an implementation of the 'bslma::Allocator'
/// protocol suitable for use as a test allocator for this library.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
class TestAllocator : public bslma::Allocator
{
    bslma::TestAllocator d_base;

  public:
    /// Create a new test allocator.
    TestAllocator();

    /// Destroy this object.
    virtual ~TestAllocator();

    /// Return a newly allocated block of memory of (at least) the specified
    /// positive 'size' (in bytes).  If 'size' is 0, a null pointer is
    /// returned with no other effect.  If this allocator cannot return the
    /// requested number of bytes, then it will throw a 'bsl::bad_alloc'
    /// exception in an exception-enabled build, or else will abort the
    /// program in a non-exception build.  The behavior is undefined unless
    /// '0 <= size'.  Note that the alignment of the address returned
    /// conforms to the platform requirement for any object of the specified
    /// 'size'.
    virtual void* allocate(size_type size);

    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.
    virtual void deallocate(void* address);

    /// Return the number of blocks currently allocated from this object.
    /// Note that 'numBlocksInUse() <= numBlocksMax()'.
    bsl::int64_t numBlocksInUse() const;
};

#endif

}  // close package namespace
}  // close enterprise namespace

/// @internal @brief
/// Assert the 'expression' is true.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_ASSERT(expression)                                        \
    do {                                                                      \
        if (!(expression)) {                                                  \
            BSLS_LOG_FATAL("Assertion failed: %s", #expression);              \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'expression' is true.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_TRUE(expression)                                          \
    do {                                                                      \
        if (!(expression)) {                                                  \
            BSLS_LOG_FATAL("Assertion false: %s", #expression);               \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'expression' is false.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_FALSE(expression)                                         \
    do {                                                                      \
        if ((expression)) {                                                   \
            BSLS_LOG_FATAL("Assertion true: %s", #expression);                \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'found' value equals the 'expected' value.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_EQ(found, expected)                                       \
    do {                                                                      \
        if ((found) != (expected)) {                                          \
            bsl::stringstream foundStream;                                    \
            foundStream << found;                                             \
            bsl::stringstream expectedStream;                                 \
            expectedStream << expected;                                       \
            BSLS_LOG_FATAL("Assertion failed: %s == %s\n"                     \
                           "Found:    %s (%s)\n"                              \
                           "Expected: %s (%s)",                               \
                           #found,                                            \
                           #expected,                                         \
                           foundStream.str().c_str(),                         \
                           #found,                                            \
                           expectedStream.str().c_str(),                      \
                           #expected);                                        \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'found' value does not equal the 'expected' value.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_NE(found, expected)                                       \
    do {                                                                      \
        if ((found) == (expected)) {                                          \
            bsl::stringstream foundStream;                                    \
            foundStream << found;                                             \
            bsl::stringstream expectedStream;                                 \
            expectedStream << expected;                                       \
            BSLS_LOG_FATAL("Assertion failed: %s != %s\n"                     \
                           "Found:    %s (%s)\n"                              \
                           "Expected: %s (%s)",                               \
                           #found,                                            \
                           #expected,                                         \
                           foundStream.str().c_str(),                         \
                           #found,                                            \
                           expectedStream.str().c_str(),                      \
                           #expected);                                        \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'found' value is less than the 'expected' value.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LT(found, expected)                                       \
    do {                                                                      \
        if (!((found) < (expected))) {                                        \
            bsl::stringstream foundStream;                                    \
            foundStream << found;                                             \
            bsl::stringstream expectedStream;                                 \
            expectedStream << expected;                                       \
            BSLS_LOG_FATAL("Assertion failed: %s < %s\n"                      \
                           "Found:    %s (%s)\n"                              \
                           "Expected: %s (%s)",                               \
                           #found,                                            \
                           #expected,                                         \
                           foundStream.str().c_str(),                         \
                           #found,                                            \
                           expectedStream.str().c_str(),                      \
                           #expected);                                        \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'found' value is less than or equal to the 'expected' value.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LE(found, expected)                                       \
    do {                                                                      \
        if (!((found) <= (expected))) {                                       \
            bsl::stringstream foundStream;                                    \
            foundStream << found;                                             \
            bsl::stringstream expectedStream;                                 \
            expectedStream << expected;                                       \
            BSLS_LOG_FATAL("Assertion failed: %s <= %s\n"                     \
                           "Found:    %s (%s)\n"                              \
                           "Expected: %s (%s)",                               \
                           #found,                                            \
                           #expected,                                         \
                           foundStream.str().c_str(),                         \
                           #found,                                            \
                           expectedStream.str().c_str(),                      \
                           #expected);                                        \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'found' value is greater than the 'expected' value.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_GT(found, expected)                                       \
    do {                                                                      \
        if (!((found) > (expected))) {                                        \
            bsl::stringstream foundStream;                                    \
            foundStream << found;                                             \
            bsl::stringstream expectedStream;                                 \
            expectedStream << expected;                                       \
            BSLS_LOG_FATAL("Assertion failed: %s > %s\n"                      \
                           "Found:    %s (%s)\n"                              \
                           "Expected: %s (%s)",                               \
                           #found,                                            \
                           #expected,                                         \
                           foundStream.str().c_str(),                         \
                           #found,                                            \
                           expectedStream.str().c_str(),                      \
                           #expected);                                        \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'found' value is greater than or equal to the 'expected' value.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_GE(found, expected)                                       \
    do {                                                                      \
        if (!((found) >= (expected))) {                                       \
            bsl::stringstream foundStream;                                    \
            foundStream << found;                                             \
            bsl::stringstream expectedStream;                                 \
            expectedStream << expected;                                       \
            BSLS_LOG_FATAL("Assertion failed: %s >= %s\n"                     \
                           "Found:    %s (%s)\n"                              \
                           "Expected: %s (%s)",                               \
                           #found,                                            \
                           #expected,                                         \
                           foundStream.str().c_str(),                         \
                           #found,                                            \
                           expectedStream.str().c_str(),                      \
                           #expected);                                        \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'found' error value does not indicate an error.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_OK(found)                                                 \
    NTCCFG_TEST_EQ(found, ntsa::Error(ntsa::Error::e_OK))

/// @internal @brief
/// Assert the 'found' error value has the same value as the specified
/// 'expected' error value.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_ERROR(found, expected)                                    \
    NTCCFG_TEST_EQ(found, ntsa::Error(expected))

/// @internal @brief
/// The verbosity at which the test driver is run.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_VERBOSITY BloombergLP::ntccfg::testVerbosity

/// @internal @brief
/// The number of command line arguments.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_ARGC BloombergLP::ntccfg::testArgc

/// @internal @brief
/// Return the array of command line arguments.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_ARGV BloombergLP::ntccfg::testArgv

/// @internal @brief
/// Begin a functional test case identified by the specified 'number'.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_CASE(number) void runTestCase##number()

/// @internal @brief
/// Define the beginning of a callback function to be run after all the common
/// test mechanisms are initialized but before any of the test cases are run.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_INIT()                                                    \
    void runTestInit();                                                       \
    struct runTestInitBinder {                                                \
        runTestInitBinder()                                                   \
        {                                                                     \
            BloombergLP::ntccfg::testInit = &runTestInit;                     \
        }                                                                     \
    } s_runTestInitBinder;                                                    \
    void runTestInit()

/// @internal @brief
/// Define the beginning of a callback function to be run after all the test
/// cases are run but before the the common test mechanisms are destroyed.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_EXIT()                                                    \
    void runTestExit();                                                       \
    struct runTestExitBinder {                                                \
        runTestExitBinder()                                                   \
        {                                                                     \
            BloombergLP::ntccfg::testExit = &runTestExit;                     \
        }                                                                     \
    } s_runTestExitBinder;                                                    \
    void runTestExit()

/// @internal @brief
/// Begin the definition of the dispatcher for a component test driver.
///
/// @par Usage Example: The skeleton of a test driver
/// This example shows the basic skeleton of a test driver.
///
///     #include <ntccfg_test.h>
///
///     NTSCFG_TEST_CASE(1)
///     {
///         NTSCFG_TEST_TRUE(true);
///         NTSCFG_TEST_FALSE(false);
///     }
///
///     NTSCFG_TEST_CASE(2)
///     {
///         NTSCFG_TEST_EQ(1, 1);
///         NTSCFG_TEST_NE(1, 2);
///     }
///
///     NTSCFG_TEST_DRIVER
///     {
///         NTSCFG_TEST_REGISTER(1);
///         NTSCFG_TEST_REGISTER(2);
///     }
///     NTSCFG_TEST_DRIVER_END;
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_DRIVER                                                    \
    namespace BloombergLP {                                                   \
    namespace ntccfg {                                                        \
                                                                              \
    int              testCase;                                                \
    int              testVerbosity;                                           \
    int              testArgc;                                                \
    char**           testArgv;                                                \
    TestInitCallback testInit;                                                \
    TestExitCallback testExit;                                                \
                                                                              \
    void printLogMessage(bsls::LogSeverity::Enum severity,                    \
                         const char*             file,                        \
                         int                     line,                        \
                         const char*             message)                     \
    {                                                                         \
        bdlt::Datetime now = bdlt::CurrentTime::utc();                        \
                                                                              \
        char nowBuffer[256];                                                  \
        now.printToBuffer(nowBuffer, sizeof nowBuffer, 3);                    \
                                                                              \
        bsl::uint64_t thread = bslmt::ThreadUtil::selfIdAsUint64();           \
                                                                              \
        bsl::string threadName;                                               \
        bslmt::ThreadUtil::getThreadName(&threadName);                        \
                                                                              \
        bsl::string fileString = file;                                        \
                                                                              \
        bsl::string::size_type n = fileString.find_last_of("/\\");            \
        if (n != bsl::string::npos) {                                         \
            fileString = fileString.substr(n + 1);                            \
        }                                                                     \
                                                                              \
        bsl::stringstream fileStream;                                         \
        fileStream << fileString;                                             \
        fileStream << ':' << line;                                            \
        fileStream.flush();                                                   \
                                                                              \
        char severityCode;                                                    \
        switch (severity) {                                                   \
        case bsls::LogSeverity::e_FATAL:                                      \
            severityCode = 'F';                                               \
            break;                                                            \
        case bsls::LogSeverity::e_ERROR:                                      \
            severityCode = 'E';                                               \
            break;                                                            \
        case bsls::LogSeverity::e_WARN:                                       \
            severityCode = 'W';                                               \
            break;                                                            \
        case bsls::LogSeverity::e_INFO:                                       \
            severityCode = 'I';                                               \
            break;                                                            \
        case bsls::LogSeverity::e_DEBUG:                                      \
            severityCode = 'D';                                               \
            break;                                                            \
        case bsls::LogSeverity::e_TRACE:                                      \
            severityCode = 'T';                                               \
            break;                                                            \
        default:                                                              \
            severityCode = 'X';                                               \
        }                                                                     \
                                                                              \
        if (threadName.empty()) {                                             \
            bsl::fprintf(stdout,                                              \
                         "[ %c ][ %s ][ %012llu ][ %40s ]: %s\n",             \
                         severityCode,                                        \
                         nowBuffer,                                           \
                         static_cast<unsigned long long>(thread),             \
                         fileStream.str().c_str(),                            \
                         message);                                            \
        }                                                                     \
        else {                                                                \
            bsl::fprintf(stdout,                                              \
                         "[ %c ][ %s ][ %16s ][ %40s ]: %s\n",                \
                         severityCode,                                        \
                         nowBuffer,                                           \
                         threadName.c_str(),                                  \
                         fileStream.str().c_str(),                            \
                         message);                                            \
        }                                                                     \
                                                                              \
        bsl::fflush(stdout);                                                  \
    }                                                                         \
    }                                                                         \
    }                                                                         \
                                                                              \
    int runTest(int testCase, int);                                           \
    int main(int argc, char** argv);                                          \
                                                                              \
    int runTest(int testCase, int verbosity)                                  \
    {                                                                         \
        (void)verbosity;                                                      \
        switch (testCase)

/// @internal @brief
/// End the definition of the dispatcher for a component test driver.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_DRIVER_END                                                \
    return -1;                                                                \
    }                                                                         \
                                                                              \
    int main(int argc, char** argv)                                           \
    {                                                                         \
        BloombergLP::ntccfg::Platform::initialize();                          \
        BloombergLP::ntccfg::Platform::ignore(                                \
            BloombergLP::ntscfg::Signal::e_PIPE);                             \
                                                                              \
        BloombergLP::bslmt::ThreadUtil::setThreadName("main");                \
                                                                              \
        BloombergLP::bsls::Log::setLogMessageHandler(                         \
            &BloombergLP::ntccfg::printLogMessage);                           \
                                                                              \
        BloombergLP::ntccfg::testCase      = 0;                               \
        BloombergLP::ntccfg::testVerbosity = 0;                               \
        BloombergLP::ntccfg::testArgc      = argc;                            \
        BloombergLP::ntccfg::testArgv      = argv;                            \
                                                                              \
        try {                                                                 \
            if (argc > 6) {                                                   \
                bsl::printf("%s\n",                                           \
                            "usage: <test-driver>.exe "                       \
                            "[ <test-case> ] [ <verbose> ]");                 \
                return 1;                                                     \
            }                                                                 \
                                                                              \
            if (argc >= 2) {                                                  \
                BloombergLP::ntccfg::testCase = bsl::atoi(argv[1]);           \
            }                                                                 \
                                                                              \
            if (argc == 2) {                                                  \
                BloombergLP::ntccfg::testVerbosity = 0;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_FATAL);                 \
            }                                                                 \
                                                                              \
            if (argc >= 3) {                                                  \
                BloombergLP::ntccfg::testVerbosity = 2;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_WARN);                  \
            }                                                                 \
                                                                              \
            if (argc >= 4) {                                                  \
                BloombergLP::ntccfg::testVerbosity = 3;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_INFO);                  \
            }                                                                 \
                                                                              \
            if (argc >= 5) {                                                  \
                BloombergLP::ntccfg::testVerbosity = 4;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_DEBUG);                 \
            }                                                                 \
                                                                              \
            if (argc == 6) {                                                  \
                BloombergLP::ntccfg::testVerbosity = 5;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_TRACE);                 \
            }                                                                 \
                                                                              \
            BloombergLP::ntccfg::TestGuard testGuard;                         \
                                                                              \
            if (BloombergLP::ntccfg::testCase == 0) {                         \
                BloombergLP::ntccfg::testCase = 1;                            \
                while (0 == runTest(BloombergLP::ntccfg::testCase,            \
                                    BloombergLP::ntccfg::testVerbosity))      \
                {                                                             \
                    ++BloombergLP::ntccfg::testCase;                          \
                }                                                             \
            }                                                                 \
            else {                                                            \
                if (0 != runTest(BloombergLP::ntccfg::testCase,               \
                                 BloombergLP::ntccfg::testVerbosity))         \
                {                                                             \
                    return -1;                                                \
                }                                                             \
            }                                                                 \
        }                                                                     \
        catch (const bsl::exception& e) {                                     \
            bsl::printf("Test %d failed: %s\n",                               \
                        BloombergLP::ntccfg::testCase,                        \
                        e.what());                                            \
            return 1;                                                         \
        }                                                                     \
        catch (...) {                                                         \
            bsl::printf("Test %d failed: %s\n",                               \
                        BloombergLP::ntccfg::testCase,                        \
                        "Unknown exception");                                 \
            return 1;                                                         \
        }                                                                     \
                                                                              \
        return 0;                                                             \
    }

/// @internal @brief
/// Register a functional test case inside a dispatcher for a component test
/// driver.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_REGISTER(number)                                          \
    case number:                                                              \
        bsl::printf("Running test case %d\n", number);                        \
        runTestCase##number();                                                \
        return 0;

#endif

/// @internal @brief
/// Log at the fatal severity level.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LOG_FATAL                                                 \
    if (BloombergLP::bsls::LogSeverity::e_FATAL <=                            \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_FATAL;                          \
        bsl::stringstream ntclStream;                                         \
        ntclStream

/// @internal @brief
/// Log at the error severity level.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LOG_ERROR                                                 \
    if (BloombergLP::bsls::LogSeverity::e_ERROR <=                            \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_ERROR;                          \
        bsl::stringstream ntclStream;                                         \
        ntclStream

/// @internal @brief
/// Log at the warn severity level.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LOG_WARN                                                  \
    if (BloombergLP::bsls::LogSeverity::e_WARN <=                             \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_WARN;                           \
        bsl::stringstream ntclStream;                                         \
        ntclStream

/// @internal @brief
/// Log at the info severity level.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LOG_INFO                                                  \
    if (BloombergLP::bsls::LogSeverity::e_INFO <=                             \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_INFO;                           \
        bsl::stringstream ntclStream;                                         \
        ntclStream

/// @internal @brief
/// Log at the debug severity level.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LOG_DEBUG                                                 \
    if (BloombergLP::bsls::LogSeverity::e_DEBUG <=                            \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_DEBUG;                          \
        bsl::stringstream ntclStream;                                         \
        ntclStream

/// @internal @brief
/// Log at the trace severity level.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LOG_TRACE                                                 \
    if (BloombergLP::bsls::LogSeverity::e_TRACE <=                            \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntclSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_TRACE;                          \
        bsl::stringstream ntclStream;                                         \
        ntclStream

/// @internal @brief
/// End logging.
///
/// @ingroup module_ntccfg
#define NTCCFG_TEST_LOG_END                                                   \
    bsl::flush;                                                               \
    BloombergLP::bsls::Log::logFormattedMessage(ntclSeverity,                 \
                                                __FILE__,                     \
                                                __LINE__,                     \
                                                "%s",                         \
                                                ntclStream.str().c_str());    \
    }

#if defined(BSLS_PLATFORM_CMP_GNU)

#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-compare"

#elif defined(BSLS_PLATFORM_CMP_CLANG)

#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wsign-compare"

#elif defined(BSLS_PLATFORM_CMP_SUN)

// ...

#elif defined(BSLS_PLATFORM_CMP_IBM)

// ...

#elif defined(BSLS_PLATFORM_CMP_MSVC)

#pragma warning(push, 0)

#else
#error Not implemented
#endif

#include <bdlb_nullablevalue.h>
#include <bslmf_removecvref.h>
#include <bslmf_removepointer.h>
#include <bsl_list.h>

namespace framework {
namespace internal {

template <typename R>
struct InvocationResult {
    BloombergLP::bdlb::NullableValue<R> d_result;

    InvocationResult()
    : d_result()
    {
    }
};

template <>
struct InvocationResult<void> {
    InvocationResult()
    {
    }
};

template <typename ARG1>
struct InvocationArg1 {
    typedef typename bsl::remove_cvref<ARG1>::type
        ARG1_type;  //TODO: do I need it?

    BloombergLP::bdlb::NullableValue<ARG1_type> d_arg1_exp;
    ARG1_type*                                  d_arg1_out;

    InvocationArg1()
    : d_arg1_exp()
    , d_arg1_out(0)
    {
    }
};

template <typename ARG1>
struct InvocationArg1<ARG1*> {
    // typedef typename bsl::remove_cvref<ARG1>::type        ARG1_type;
    // typedef typename bsl::remove_pointer<ARG1_type>::type ARG1_pure_type;
    //
    // BloombergLP::bdlb::NullableValue<ARG1_type*>     d_arg1_exp;
    // BloombergLP::bdlb::NullableValue<ARG1_pure_type> d_arg1_in;
    // ARG1_type**                                      d_arg1_out;

    typedef typename bsl::remove_pointer<ARG1>::type ARG1_pure_type;

    BloombergLP::bdlb::NullableValue<ARG1*>          d_arg1_exp;
    BloombergLP::bdlb::NullableValue<ARG1_pure_type> d_arg1_in;
    ARG1**                                           d_arg1_out;

    InvocationArg1()
    : d_arg1_exp()
    , d_arg1_in()
    , d_arg1_out(0)
    {
    }
};

template <typename ARG1>
struct InvocationArg1<ARG1&> {
    typedef typename bsl::remove_cvref<ARG1>::type        ARG1_type;
    typedef typename bsl::remove_pointer<ARG1_type>::type ARG1_pure_type;

    BloombergLP::bdlb::NullableValue<ARG1_type>      d_arg1_exp;
    BloombergLP::bdlb::NullableValue<ARG1_pure_type> d_arg1_in;
    ARG1_type*                                       d_arg1_out;

    InvocationArg1()
    : d_arg1_exp()
    , d_arg1_in()
    , d_arg1_out(0)
    {
    }
};

/*
template <typename ARG1>
struct InvocationArg1<ARG1 const &> {
    typedef typename bsl::remove_cvref<ARG1>::type        ARG1_type;
    typedef typename bsl::remove_pointer<ARG1_type>::type ARG1_pure_type;

    BloombergLP::bdlb::NullableValue<ARG1_type>      d_arg1_exp;
    ARG1_type*                                       d_arg1_out;

    InvocationArg1()
    : d_arg1_exp()
    , d_arg1_out(0)
    {
    }
};

template <typename ARG1>
struct InvocationArg1<ARG1 const *> {
    typedef typename bsl::remove_cvref<ARG1>::type        ARG1_type;
    typedef typename bsl::remove_pointer<ARG1_type>::type ARG1_pure_type;

    BloombergLP::bdlb::NullableValue<ARG1_type>      d_arg1_exp;
    ARG1_type*                                       d_arg1_out;

    InvocationArg1()
    : d_arg1_exp()
    , d_arg1_out(0)
    {
    }
};*/

template <typename R>
struct InvocationDataArg0 : public InvocationResult<R> {
    int d_expectedCalls;
        InvocationDataArg0()
    : d_expectedCalls(0)
    {
    }
};

template <typename R, typename ARG1>
struct InvocationDataArg1 : public InvocationResult<R>,
                            public InvocationArg1<ARG1> {
    int d_expectedCalls;
        InvocationDataArg1()
    : d_expectedCalls(0)
    {
    }
};

template <typename INVOCATION_DATA, typename SELF, typename RESULT>
class InvocationBaseWillReturn
{
  public:
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;

    SELF& willReturn(const RESULT& result)
    {
        INVOCATION_DATA& invocation = getInvocationDataBack();
        invocation.d_result         = result;
        return *(static_cast<SELF*>(this));
    }
};

template <typename INVOCATION_DATA, typename SELF>
class InvocationBaseWillReturn<INVOCATION_DATA, SELF, void>
{
  public:
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;
};

template <typename INVOCATION_DATA, typename RESULT>
class InvocationBaseInvoke
{
  public:
    RESULT finalizeInvocation()
    {
        INVOCATION_DATA& invocation = this->getInvocationDataFront();
        NTCCFG_TEST_TRUE(invocation.d_result.has_value());
        const auto result = invocation.d_result.value();
        if (invocation.d_expectedCalls != -1) {
            --invocation.d_expectedCalls;
            if (invocation.d_expectedCalls == 0) {
                this->removeInvocationDataFront();
            }
        }
        return result;
    }

  private:
    virtual INVOCATION_DATA& getInvocationDataFront()    = 0;
    virtual void             removeInvocationDataFront() = 0;
};

template <typename INVOCATION_DATA>
class InvocationBaseInvoke<INVOCATION_DATA, void>
{
  public:
    void finalizeInvocation()
    {
        INVOCATION_DATA& invocation = this->getInvocationDataFront();
        if (invocation.d_expectedCalls != -1) {
            --invocation.d_expectedCalls;
            if (invocation.d_expectedCalls == 0) {
                this->removeInvocationDataFront();
            }
        }
    }

  private:
    virtual INVOCATION_DATA& getInvocationDataFront()    = 0;
    virtual void             removeInvocationDataFront() = 0;
};

template <typename SELF, typename INVOCATION_DATA>
class InvocationBaseOnceAlways
{
  public:
    SELF& willOnce()
    {
        INVOCATION_DATA& invocation = this->getInvocationDataBack();
        NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);

        invocation.d_expectedCalls = 1;
        return *(static_cast<SELF*>(this));
    }
    SELF& willAlways()
    {
        INVOCATION_DATA& invocation = this->getInvocationDataBack();
        NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);

        invocation.d_expectedCalls = -1;
        return *(static_cast<SELF*>(this));
    }

  private:
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;
};

struct NoArgProcessor {
};

template <typename INVOCATION_DATA,
          typename SELF,
          typename RESULT,
          typename INVOCATION_ARG_PROCESSOR = NoArgProcessor>
class InvocationBase
: public InvocationBaseWillReturn<INVOCATION_DATA, SELF, RESULT>,
  public InvocationBaseOnceAlways<SELF, INVOCATION_DATA>,
  public InvocationBaseInvoke<INVOCATION_DATA, RESULT>,
  public INVOCATION_ARG_PROCESSOR
{
  protected:
    INVOCATION_DATA& getInvocationDataBack() override
    {
        NTCCFG_TEST_FALSE(this->d_invocations.empty());
        return d_invocations.back();
    }

    INVOCATION_DATA& getInvocationDataFront() override
    {
        NTCCFG_TEST_FALSE(this->d_invocations.empty());
        return d_invocations.front();
    }

    void removeInvocationDataFront() override
    {
        NTCCFG_TEST_FALSE(this->d_invocations.empty());
        d_invocations.pop_front();
    }

    bsl::list<INVOCATION_DATA> d_invocations;
};

template <typename SELF, typename INVOCATION_DATA, typename ARG1>
class InvocationArg1Processor
{
  public:
    typedef typename bsl::remove_cvref<ARG1>::type        ARG1_type;
    typedef typename bsl::remove_pointer<ARG1_type>::type ARG1_pure_type;

    SELF& saveArg1(ARG1_type* arg1_out)
    {
        INVOCATION_DATA& invocation = this->getInvocationDataBack();
        invocation.d_arg1_out       = arg1_out;
        return *(static_cast<SELF*>(this));
    }

    virtual INVOCATION_DATA& getInvocationDataBack() = 0;
};

template <typename SELF, typename INVOCATION_DATA, typename ARG1>
class InvocationArg1Processor<SELF, INVOCATION_DATA, ARG1*>
{
    typedef typename bsl::remove_cvref<ARG1>::type        ARG1_type;
    typedef typename bsl::remove_pointer<ARG1_type>::type ARG1_pure_type;

  public:
    SELF& setArg1To(const ARG1_pure_type& arg1_in)
    {
        INVOCATION_DATA& invocation = this->getInvocationDataBack();
        invocation.d_arg1_in        = arg1_in;
        return *(static_cast<SELF*>(this));
    }
    SELF& saveArg1(ARG1_type** arg1_out)
    {
        INVOCATION_DATA& invocation = this->getInvocationDataBack();
        invocation.d_arg1_out       = arg1_out;
        return *(static_cast<SELF*>(this));
    }

  protected:
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;
};

template <typename SELF, typename INVOCATION_DATA, typename ARG1>
class InvocationArg1Processor<SELF, INVOCATION_DATA, ARG1&>
{
    typedef typename bsl::remove_cvref<ARG1>::type        ARG1_type;
    typedef typename bsl::remove_pointer<ARG1_type>::type ARG1_pure_type;

  public:
    SELF& setArg1To(const ARG1_pure_type& arg1_in)
    {
        INVOCATION_DATA& invocation = this->getInvocationDataBack();
        invocation.d_arg1_in        = arg1_in;
        return *(static_cast<SELF*>(this));
    }
    SELF& saveArg1(ARG1_type* arg1_out)
    {
        INVOCATION_DATA& invocation = this->getInvocationDataBack();
        invocation.d_arg1_out       = arg1_out;
        return *(static_cast<SELF*>(this));
    }

  protected:
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;
};

template <typename RESULT>
class Invocation0 : public InvocationBase<InvocationDataArg0<RESULT>,
                                          Invocation0<RESULT>,
                                          RESULT>
{
    typedef InvocationDataArg0<RESULT> InvocationDataImpl;

  public:
    Invocation0& expect()
    {
        this->d_invocations.emplace_back();
        return *this;
    }

    RESULT invoke()
    {
        NTCCFG_TEST_FALSE(this->d_invocations.empty());
        InvocationDataImpl& invocation = this->d_invocations.front();
        if (invocation.d_expectedCalls != -1) {
            NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
        }

        return this->finalizeInvocation();
    }
};

template <typename RESULT, typename ARG1>
class Invocation1
: public InvocationBase<
      InvocationDataArg1<RESULT, ARG1>,
      Invocation1<RESULT, ARG1>,
      RESULT,
      InvocationArg1Processor<Invocation1<RESULT, ARG1>,
                              InvocationDataArg1<RESULT, ARG1>,
                              ARG1> >
{
    typedef InvocationDataArg1<RESULT, ARG1> InvocationDataImpl;

  public:
    typedef typename bsl::remove_cvref<ARG1>::type        ARG1_type;
    typedef typename bsl::remove_pointer<ARG1_type>::type ARG1_pure_type;

    Invocation1& expect(
        const BloombergLP::bdlb::NullableValue<ARG1_type>& arg1)
    {
        this->d_invocations.emplace_back();
        InvocationDataImpl& invocation = this->d_invocations.back();
        invocation.d_arg1_exp          = arg1;
        return *this;
    }

    // Invocation1& setArg1To(const ARG1_pure_type& arg1In)
    // {
    //     NTCCFG_TEST_FALSE(this->d_invocations.empty());
    //     InvocationDataImpl& invocation = this->d_invocations.back();
    //     invocation.d_arg1_in           = arg1In;
    //     return *this;
    // }

    RESULT invoke(ARG1 arg1)
    {
        NTCCFG_TEST_FALSE(this->d_invocations.empty());
        InvocationDataImpl& invocation = this->d_invocations.front();
        if (invocation.d_expectedCalls != -1) {
            NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
        }
        if (invocation.d_arg1_exp.has_value()) {
            NTCCFG_TEST_EQ(arg1, invocation.d_arg1_exp.value());
        }
        // if (invocation.d_arg1_out) {
        //     *invocation.d_arg1_out = arg1;
        // }
        // if (invocation.d_arg1_in.has_value()) {
        //     arg1 = invocation.d_arg1_in.value();
        // }
        return this->finalizeInvocation();
    }
};

}
}

#define NTF_EXPAND(X) X
#define NTF_CAT(A, B) A##B
#define NTF_SELECT(NAME, NUM) NTF_CAT(NAME##_, NUM)

#define NTF_GET_COUNT(_1, _2, _3, _4, _5, _6, COUNT, ...) COUNT
#define NTF_VA_SIZE(...)                                                      \
    NTF_EXPAND(NTF_GET_COUNT(__VA_ARGS__, 4, 3, 2, 1, 0, -1))

#define NTF_VA_SELECT(NAME, ...)                                              \
    NTF_SELECT(NAME, NTF_VA_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define NTF_MOCK_METHOD_0_IMP(RESULT, METHOD_NAME)                            \
  public:                                                                     \
    framework::internal::Invocation0<RESULT>& expect_##METHOD_NAME()          \
    {                                                                         \
        return d_invocation_##METHOD_NAME.expect();                           \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable framework::internal::Invocation0<RESULT>                          \
        d_invocation_##METHOD_NAME;

#define NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1)                      \
  public:                                                                     \
    framework::internal::Invocation1<RESULT, ARG1>& expect_##METHOD_NAME(     \
        const bdlb::NullableValue<bsl::remove_cvref<ARG1>::type>& arg1)       \
    {                                                                         \
        return d_invocation_##METHOD_NAME.expect(arg1);                       \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable framework::internal::Invocation1<RESULT, ARG1>                    \
        d_invocation_##METHOD_NAME;

#define NTF_MOCK_METHOD_0(RESULT, METHOD_NAME)                                \
  public:                                                                     \
    RESULT METHOD_NAME() override                                             \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke();                           \
    }                                                                         \
    NTF_MOCK_METHOD_0_IMP(RESULT, METHOD_NAME)

#define NTF_MOCK_METHOD_CONST_0(RESULT, METHOD_NAME)                          \
  public:                                                                     \
    RESULT METHOD_NAME() const override                                       \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke();                           \
    }                                                                         \
    NTF_MOCK_METHOD_0_IMP(RESULT, METHOD_NAME)

#define NTF_MOCK_METHOD_1(RESULT, METHOD_NAME, ARG1)                          \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1) override                                    \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke(arg1);                       \
    }                                                                         \
    NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1)

#define NTF_MOCK_METHOD_CONST_1(RESULT, METHOD_NAME, ARG1)                    \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1) const override                              \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke(arg1);                       \
    }                                                                         \
    NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1)

#define NTF_MOCK_METHOD(...) NTF_VA_SELECT(NTF_MOCK_METHOD, __VA_ARGS__)

#define NTF_MOCK_METHOD_CONST(...)                                            \
    NTF_VA_SELECT(NTF_MOCK_METHOD_CONST, __VA_ARGS__)

namespace NewMock {

template <class ARG>
struct ProcessInterface {
    virtual void process(const ARG& arg) = 0;
    virtual ~    ProcessInterface()
    {
    }
};

template <class OUT, class IN>
struct DerefSetter {
    static void set(OUT out, const IN& in)
    {
        *out = in;
    }
};

template <class OUT, class IN>
struct DefaultSetter {
    static void set(OUT out, const IN& in)
    {
        out = in;
    }
};

template <class IN, template <class, class> class SET_POLICY>
struct Setter {
    template <class ARG>
    void process(ARG arg)
    {
        SET_POLICY<ARG, IN>::set(arg, d_in);
    }

    explicit Setter(const IN& in)
    : d_in(in)
    {
    }

    const IN& d_in;
};

template <template <class, class> class SET_POLICY, class IN>
Setter<IN, SET_POLICY> createSetter(const IN& val)
{
    Setter<IN, SET_POLICY> setter(val);
    return setter;
}

template <class ARG, class SETTER>
struct SetterHolder : ProcessInterface<ARG> {
    explicit SetterHolder(const SETTER& setter)
    : d_setter(setter)
    {
    }

    void process(const ARG& arg) override
    {
        d_setter.process(arg);
    }

    SETTER d_setter;
};

template <class FROM, class TO>
struct DerefPolicy {
    static void extract(const FROM& from, TO* to)
    {
        *to = *from;
    }
};

template <class FROM, class TO>
struct NoDerefPolicy {
    static void extract(const FROM& from, TO* to)
    {
        *to = from;
    }
};

template <class TO, template <class, class> class EXTRACT_POLICY>
struct Extractor {
    template <class ARG>
    void process(const ARG& arg)
    {
        EXTRACT_POLICY<ARG, TO>::extract(arg, d_to);
    }

    explicit Extractor(TO* to)
    : d_to(to)
    {
    }

    TO* d_to;
};

template <template <class, class> class EXTRACT_POLICY, class TO>
Extractor<TO, EXTRACT_POLICY> createExtractor(TO* val)
{
    Extractor<TO, EXTRACT_POLICY> extractor(val);
    return extractor;
}

template <class ARG, class EXTRACTOR>
struct ExtractorHolder : ProcessInterface<ARG> {
    ExtractorHolder(const EXTRACTOR& extractor)
    : d_extractor(extractor)
    {
    }

    void process(const ARG& arg) override
    {
        d_extractor.process(arg);
    }

    EXTRACTOR d_extractor;
};

template <class ARG, class EXP>
struct DirectComparator {
    static void compare(const ARG& arg1, const EXP& arg2)
    {
        NTCCFG_TEST_EQ(arg1, arg2);
    }
};

template <class ARG, class EXP>
struct DerefComparator {
    static void compare(const ARG& arg1, const EXP& arg2)
    {
        NTCCFG_TEST_EQ(*arg1, arg2);
    }
};

template <class EXP, template <class, class> class COMP>
struct EqMatcher {
    template <class ARG>
    void process(const ARG& arg) const
    {
        COMP<ARG, EXP>::compare(arg, exp);
    }

    EqMatcher(const EXP& exp_)
    : exp(exp_)
    {
    }

    EXP exp;
};

template <template <class, class> class COMPARATOR, class EXP>
BloombergLP::bdlb::NullableValue<EqMatcher<EXP, COMPARATOR> > createEqMatcher(
    const EXP& val)
{
    EqMatcher<EXP, COMPARATOR> matcher(val);
    return matcher;
}

template <class ARG, class MATCHER>
struct MatcherHolder : ProcessInterface<ARG> {
    MatcherHolder(const MATCHER& matcher)
    : d_matcher(matcher)
    {
    }

    void process(const ARG& arg) override
    {
        d_matcher.process(arg);
    }

    MATCHER d_matcher;
};

template <class RESULT>
struct InvocationResult {
    BloombergLP::bdlb::NullableValue<RESULT>
           d_expResult;  //TODO: wont work with ref
    RESULT get()
    {
        NTCCFG_TEST_TRUE(d_expResult.has_value());
        return d_expResult.value();
    }
};

template <>
struct InvocationResult<void> {
    static void get()
    {
    }
};

struct NO_ARG {
};

template <class INVOCATION_DATA, class SELF, class RESULT>
struct InvocationBaseWillReturn {
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;

    SELF& willReturn(const RESULT& result)
    {
        INVOCATION_DATA& invocation     = getInvocationDataBack();
        invocation.d_result.d_expResult = result;
        return *(static_cast<SELF*>(this));
    }
};

template <class INVOCATION_DATA, class SELF>
struct InvocationBaseWillReturn<INVOCATION_DATA, SELF, void> {
};

template <class INVOCATION_DATA, class SELF>
struct InvocationBaseWillOnce {
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;

    SELF& willOnce()
    {
        INVOCATION_DATA& invocation = getInvocationDataBack();
        invocation.d_expectedCalls  = 1;
        return *(static_cast<SELF*>(this));
    }
};

template <class RESULT, class ARG1, class ARG2, class ARG3>
struct InvocationData {
    enum { k_INFINITE_CALLS = -1 };

    int                      d_expectedCalls;
    InvocationResult<RESULT> d_result;

    bsl::shared_ptr<ProcessInterface<ARG1> > arg1Matcher;
    bsl::shared_ptr<ProcessInterface<ARG2> > arg2Matcher;
    bsl::shared_ptr<ProcessInterface<ARG3> > arg3Matcher;

    bsl::shared_ptr<ProcessInterface<ARG1> > arg1Extractor;
    bsl::shared_ptr<ProcessInterface<ARG2> > arg2Extractor;
    bsl::shared_ptr<ProcessInterface<ARG3> > arg3Extractor;

    bsl::shared_ptr<ProcessInterface<ARG1> > arg1Setter;
    bsl::shared_ptr<ProcessInterface<ARG2> > arg2Setter;
    bsl::shared_ptr<ProcessInterface<ARG3> > arg3Setter;
};

template <class RESULT>
struct Invocation0
: public InvocationBaseWillReturn<
      InvocationData<RESULT, NO_ARG, NO_ARG, NO_ARG>,
      Invocation0<RESULT>,
      RESULT>,
  public InvocationBaseWillOnce<InvocationData<RESULT, NO_ARG, NO_ARG, NO_ARG>,
                                Invocation0<RESULT> > {
    typedef InvocationData<RESULT, NO_ARG, NO_ARG, NO_ARG> InvocationDataT;

    RESULT invoke()
    {
        NTCCFG_TEST_FALSE(d_invocations.empty());
        InvocationDataT& invocation = d_invocations.front();
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
        }
        InvocationResult<RESULT> result = invocation.d_result;  //copy by value
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            --invocation.d_expectedCalls;
            if (invocation.d_expectedCalls == 0) {
                d_invocations.pop_front();
            }
        }
        return result.get();
    }

    Invocation0& expect()
    {
        d_invocations.emplace_back();
        return *this;
    }

    InvocationDataT& getInvocationDataBack()
    {
        NTCCFG_TEST_FALSE(d_invocations.empty());
        return d_invocations.back();
    }

    bsl::list<InvocationData<RESULT, NO_ARG, NO_ARG, NO_ARG> > d_invocations;
};

inline void test_f111(int x, int y)
{
    printf("%d", x);
}

template <class RESULT, class ARG1>
struct Invocation1

: public InvocationBaseWillReturn<InvocationData<RESULT, ARG1, NO_ARG, NO_ARG>,
                                  Invocation1<RESULT, ARG1>,
                                  RESULT>,
  public InvocationBaseWillOnce<InvocationData<RESULT, ARG1, NO_ARG, NO_ARG>,
                                Invocation1<RESULT, ARG1> >

{
    typedef InvocationData<RESULT, ARG1, NO_ARG, NO_ARG> InvocationDataT;
    typedef ARG1                                         ARG1_TYPE;

    RESULT invoke(ARG1 arg)
    {
        NTCCFG_TEST_FALSE(d_invocations.empty());
        InvocationDataT& invocation = d_invocations.front();
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
        }

        if (invocation.arg1Matcher) {
            invocation.arg1Matcher->process(arg);
        }
        if (invocation.arg1Extractor) {
            invocation.arg1Extractor->process();
        }
        if (invocation.arg1Setter) {
            invocation.arg1Setter->process();
        }

        InvocationResult<RESULT> result = invocation.d_result;  //copy by value
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            --invocation.d_expectedCalls;
            if (invocation.d_expectedCalls == 0) {
                d_invocations.pop_front();
            }
        }
        return result.get();
    }

    template <class ARG1_MATCHER>
    Invocation1& expect(
        const ::BloombergLP::bdlb::NullableValue<ARG1_MATCHER>& arg1Matcher)
    {
        d_invocations.emplace_back();
        InvocationDataT& data = d_invocations.back();
        if (arg1Matcher.has_value()) {
            bsl::shared_ptr<MatcherHolder<ARG1, ARG1_MATCHER> >
                matcherInterface(new MatcherHolder<ARG1, ARG1_MATCHER>(
                    arg1Matcher.value()));

            data.arg1Matcher = matcherInterface;
        }
        return *this;
    }

    template <class ARG_EXTRACTOR>
    Invocation1& saveArg1(const ARG_EXTRACTOR& extractor)
    {
        InvocationDataT& data = getInvocationDataBack();

        bsl::shared_ptr<ExtractorHolder<ARG1, ARG_EXTRACTOR> >
            extractorInterface(
                new ExtractorHolder<ARG1, ARG_EXTRACTOR>(extractor));

        data.arg1Extractor = extractorInterface;

        return *this;
    }

    template <class ARG_SETTER>
    Invocation1& setArg1(const ARG_SETTER& setter)
    {
        InvocationDataT& data = getInvocationDataBack();

        bsl::shared_ptr<SetterHolder<ARG1, ARG_SETTER> > setterInterface(
            new SetterHolder<ARG1, ARG_SETTER>(setter));

        data.arg1Setter = setterInterface;

        return *this;
    }

    InvocationDataT& getInvocationDataBack()
    {
        NTCCFG_TEST_FALSE(d_invocations.empty());
        return d_invocations.back();
    }

    bsl::list<InvocationData<RESULT, ARG1, NO_ARG, NO_ARG> > d_invocations;
};

template <class RESULT, class ARG1, class ARG2>
struct Invocation2 {
    bsl::list<InvocationData<RESULT, ARG1, ARG2, NO_ARG> > d_invocations;
};

}

#define NTF_MOCK_METHOD_0_IMP_NEW(RESULT, METHOD_NAME)                        \
  public:                                                                     \
    NewMock::Invocation0<RESULT>& expect_##METHOD_NAME()                      \
    {                                                                         \
        return d_invocation_##METHOD_NAME.expect();                           \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable NewMock::Invocation0<RESULT> d_invocation_##METHOD_NAME;

#define NTF_MOCK_METHOD_1_IMP_NEW(RESULT, METHOD_NAME, ARG1)                  \
  public:                                                                     \
    template <class MATCHER>                                                  \
    NewMock::Invocation1<RESULT, ARG1>& expect_##METHOD_NAME(                 \
        const BloombergLP::bdlb::NullableValue<MATCHER>& arg1)                \
    {                                                                         \
        return d_invocation_##METHOD_NAME.expect(arg1);                       \
        /* return NewMock::Invocation1<RESULT, ARG1>(); */                    \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable NewMock::Invocation1<RESULT, ARG1> d_invocation_##METHOD_NAME;

#define NTF_MOCK_METHOD_NEW_0(RESULT, METHOD_NAME)                            \
  public:                                                                     \
    RESULT METHOD_NAME() override                                             \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke();                           \
    }                                                                         \
    NTF_MOCK_METHOD_0_IMP_NEW(RESULT, METHOD_NAME)

#define NTF_MOCK_METHOD_CONST_NEW_1(RESULT, METHOD_NAME)                      \
  public:                                                                     \
    RESULT METHOD_NAME() const override                                       \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke();                           \
    }                                                                         \
    NTF_MOCK_METHOD_0_IMP_NEW(RESULT, METHOD_NAME)

#define NTF_MOCK_METHOD_NEW_1(RESULT, METHOD_NAME, ARG1)                      \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1) override                                    \
    {                                                                         \
        return /*d_invocation_##METHOD_NAME.invoke(arg1) TODO: */ RESULT();   \
    }                                                                         \
    NTF_MOCK_METHOD_1_IMP_NEW(RESULT, METHOD_NAME, ARG1)

#define NTF_MOCK_METHOD_CONST_NEW_1(RESULT, METHOD_NAME, ARG1)                \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1) const override                              \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke(arg1);                       \
    }                                                                         \
    NTF_MOCK_METHOD_1_IMP_NEW(RESULT, METHOD_NAME, ARG1)

#define NTF_MOCK_METHOD_NEW(...)                                              \
    NTF_VA_SELECT(NTF_MOCK_METHOD_NEW, __VA_ARGS__)

#define NTF_MOCK_METHOD_CONST_NEW(...)                                        \
    NTF_VA_SELECT(NTF_MOCK_METHOD_CONST_NEW, __VA_ARGS__)

#define NTF_EQ(ARG) NewMock::createEqMatcher<NewMock::DirectComparator>(ARG)
#define NTF_EQ_DEREF(ARG)                                                     \
    NewMock::createEqMatcher<NewMock::DerefComparator>(ARG)

#define NTF_EXPECT_0(MOCK_OBJECT, METHOD) MOCK_OBJECT.expect_##METHOD()
#define NTF_EXPECT_1(MOCK_OBJECT, METHOD, ...)                                \
    MOCK_OBJECT.expect_##METHOD(__VA_ARGS__)

#define ONCE() willOnce()
#define RETURN(VAL) willReturn(VAL)

#define TO(ARG) NewMock::createExtractor<NewMock::NoDerefPolicy>(ARG)
#define TO_DEREF(ARG) NewMock::createExtractor<NewMock::DerefPolicy>(ARG)

#define SAVE_ARG_1(...) saveArg1(__VA_ARGS__)

#define FROM(ARG) NewMock::createSetter<NewMock::DefaultSetter>(ARG)

#define FROM_DEREF(ARG) NewMock::createSetter<NewMock::DerefSetter>(ARG)

#define SET_ARG_1(...) setArg1(__VA_ARGS__)
