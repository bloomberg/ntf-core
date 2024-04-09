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
#include <bsl_list.h>

#define NTF_CAT2_(A, B) A##B
#define NTF_CAT2(A, B) NTF_CAT2_(A, B)

#define NTF_EXPAND(X) X
#define NTF_CAT(A, B) A##B
#define NTF_SELECT(NAME, NUM) NTF_CAT(NAME##_, NUM)

#define NTF_GET_COUNT(_1, _2, _3, _4, _5, _6, COUNT, ...) COUNT
#define NTF_VA_SIZE(...)                                                      \
    NTF_EXPAND(NTF_GET_COUNT(__VA_ARGS__, 4, 3, 2, 1, 0, -1))

#define NTF_VA_SELECT(NAME, ...)                                              \
    NTF_SELECT(NAME, NTF_VA_SIZE(__VA_ARGS__))(__VA_ARGS__)

namespace ntf_mock {

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

    static bool ignore()
    {
        return false;
    }

    explicit EqMatcher(const EXP& exp_)
    : exp(exp_)
    {
    }

    EXP exp;
};

struct IgnoreArg {
    static bool ignore()
    {
        return true;
    }
    template <class T>
    void process(T)
    {
    }
};

template <template <class, class> class COMPARATOR, class EXP>
EqMatcher<EXP, COMPARATOR> createEqMatcher(const EXP& val)
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
    InvocationResult()
    : d_expResult()
    {
    }
    BloombergLP::bdlb::NullableValue<RESULT> d_expResult;
    RESULT                                   get()
    {
        NTCCFG_TEST_TRUE(d_expResult.has_value());
        return d_expResult.value();
    }
};

template <class RESULT>
struct InvocationResult<RESULT&> {
    InvocationResult()
    : d_expResult(0)
    {
    }
    RESULT* d_expResult;
    RESULT& get()
    {
        NTCCFG_TEST_TRUE(d_expResult != 0);
        return *d_expResult;
    }
};

template <class RESULT>
struct InvocationResult<RESULT const&> {
    InvocationResult()
    : d_expResult(0)
    {
    }
    RESULT const* d_expResult;
    const RESULT& get()
    {
        NTCCFG_TEST_TRUE(d_expResult != 0);
        return *d_expResult;
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

  protected:
    ~InvocationBaseWillReturn()
    {
    }
};

template <class INVOCATION_DATA, class SELF, class RESULT>
struct InvocationBaseWillReturn<INVOCATION_DATA, SELF, RESULT&> {
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;

    SELF& willReturnRef(RESULT* result)
    {
        INVOCATION_DATA& invocation     = getInvocationDataBack();
        invocation.d_result.d_expResult = result;
        return *(static_cast<SELF*>(this));
    }

  protected:
    ~InvocationBaseWillReturn()
    {
    }
};

template <class INVOCATION_DATA, class SELF, class RESULT>
struct InvocationBaseWillReturn<INVOCATION_DATA, SELF, RESULT const&> {
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;

    SELF& willReturnRef(RESULT const* result)
    {
        INVOCATION_DATA& invocation     = getInvocationDataBack();
        invocation.d_result.d_expResult = result;
        return *(static_cast<SELF*>(this));
    }

  protected:
    ~InvocationBaseWillReturn()
    {
    }
};

template <class INVOCATION_DATA, class SELF>
struct InvocationBaseWillReturn<INVOCATION_DATA, SELF, void> {
};

template <class INVOCATION_DATA, class SELF>
struct InvocationBaseTimes {
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;

    SELF& willOnce()
    {
        INVOCATION_DATA& invocation = getInvocationDataBack();
        NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);
        invocation.d_expectedCalls = 1;
        return *(static_cast<SELF*>(this));
    }

    SELF& willAlways()
    {
        INVOCATION_DATA& invocation = getInvocationDataBack();
        NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);
        invocation.d_expectedCalls = INVOCATION_DATA::k_INFINITE_CALLS;
        return *(static_cast<SELF*>(this));
    }

    SELF& willTimes(int times)
    {
        NTCCFG_TEST_GT(times, 0);
        INVOCATION_DATA& invocation = getInvocationDataBack();
        NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);
        invocation.d_expectedCalls = times;
        return *(static_cast<SELF*>(this));
    }
};

template <class RESULT>
struct InvocationDataBase {
    enum { k_INFINITE_CALLS = -1 };

    int                      d_expectedCalls;
    InvocationResult<RESULT> d_result;

    InvocationDataBase()
    : d_expectedCalls(0)
    , d_result()
    {
    }
    InvocationDataBase(const InvocationDataBase& other)
    : d_expectedCalls(other.d_expectedCalls)
    , d_result(other.d_result)
    {
    }
};

template <class RESULT,
          class ARG1 = NO_ARG,
          class ARG2 = NO_ARG,
          class ARG3 = NO_ARG,
          class ARG4 = NO_ARG,
          class ARG5 = NO_ARG>
struct InvocationData;

template <class RESULT>
struct InvocationData<RESULT> : public InvocationDataBase<RESULT> {
    InvocationData()                            = default;
    InvocationData(const InvocationData& other) = default;
};

template <class RESULT, class ARG1>
struct InvocationData<RESULT, ARG1> : public InvocationData<RESULT> {
    bsl::shared_ptr<ProcessInterface<ARG1> > arg1Matcher;
    bsl::shared_ptr<ProcessInterface<ARG1> > arg1Extractor;
    bsl::shared_ptr<ProcessInterface<ARG1> > arg1Setter;

    void processArgs(ARG1 arg1)
    {
        if (arg1Matcher) {
            arg1Matcher->process(arg1);
        }
        if (arg1Extractor) {
            arg1Extractor->process(arg1);
        }
        if (arg1Setter) {
            arg1Setter->process(arg1);
        }
    }
};

template <class RESULT, class ARG1, class ARG2>
struct InvocationData<RESULT, ARG1, ARG2>
: public InvocationData<RESULT, ARG1> {
    typedef InvocationData<RESULT, ARG1> BASE;

    bsl::shared_ptr<ProcessInterface<ARG2> > arg2Matcher;
    bsl::shared_ptr<ProcessInterface<ARG2> > arg2Extractor;
    bsl::shared_ptr<ProcessInterface<ARG2> > arg2Setter;

    void processArgs(ARG1 arg1, ARG2 arg2)
    {
        BASE::processArgs(arg1);

        if (arg2Matcher) {
            arg2Matcher->process(arg2);
        }
        if (arg2Extractor) {
            arg2Extractor->process(arg2);
        }
        if (arg2Setter) {
            arg2Setter->process(arg2);
        }
    }
};

template <class RESULT, class ARG1, class ARG2, class ARG3>
struct InvocationData<RESULT, ARG1, ARG2, ARG3>
: public InvocationData<RESULT, ARG1, ARG2> {
    typedef InvocationData<RESULT, ARG1, ARG2> BASE;
    bsl::shared_ptr<ProcessInterface<ARG3> >   arg3Matcher;
    bsl::shared_ptr<ProcessInterface<ARG3> >   arg3Extractor;
    bsl::shared_ptr<ProcessInterface<ARG3> >   arg3Setter;

    void processArgs(ARG1 arg1, ARG2 arg2, ARG3 arg3)
    {
        BASE::processArgs(arg1, arg2);

        if (arg3Matcher) {
            arg3Matcher->process(arg3);
        }
        if (arg3Extractor) {
            arg3Extractor->process(arg3);
        }
        if (arg3Setter) {
            arg3Setter->process(arg3);
        }
    }
};

template <class RESULT, class ARG1, class ARG2, class ARG3, class ARG4>
struct InvocationData<RESULT, ARG1, ARG2, ARG3, ARG4>
: public InvocationData<RESULT, ARG1, ARG2, ARG3> {
    typedef InvocationData<RESULT, ARG1, ARG2, ARG3> BASE;
    bsl::shared_ptr<ProcessInterface<ARG4> >         arg4Matcher;
    bsl::shared_ptr<ProcessInterface<ARG4> >         arg4Extractor;
    bsl::shared_ptr<ProcessInterface<ARG4> >         arg4Setter;

    void processArgs(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)
    {
        BASE::processArgs(arg1, arg2, arg3);

        if (arg4Matcher) {
            arg4Matcher->process(arg4);
        }
        if (arg4Extractor) {
            arg4Extractor->process(arg4);
        }
        if (arg4Setter) {
            arg4Setter->process(arg4);
        }
    }
};

template <class INVOCATION_DATA,
          class SELF,
          class ARG1 = NO_ARG,
          class ARG2 = NO_ARG,
          class ARG3 = NO_ARG,
          class ARG4 = NO_ARG,
          class ARG5 = NO_ARG>
struct InvocationBaseSaveSetArg {
  private:
     InvocationBaseSaveSetArg();
    ~InvocationBaseSaveSetArg();
};

template <class INVOCATION_DATA, class SELF, class ARG1>
struct InvocationBaseSaveSetArg<INVOCATION_DATA, SELF, ARG1> {
    virtual INVOCATION_DATA& getInvocationDataBack() = 0;

    template <class ARG_EXTRACTOR>
    SELF& saveArg1(const ARG_EXTRACTOR& extractor)
    {
        INVOCATION_DATA& data = this->getInvocationDataBack();

        bsl::shared_ptr<ExtractorHolder<ARG1, ARG_EXTRACTOR> >
            extractorInterface(
                new ExtractorHolder<ARG1, ARG_EXTRACTOR>(extractor));

        data.arg1Extractor = extractorInterface;

        return *(static_cast<SELF*>(this));
    }

    template <class ARG_SETTER>
    SELF& setArg1(const ARG_SETTER& setter)
    {
        INVOCATION_DATA& data = this->getInvocationDataBack();

        bsl::shared_ptr<SetterHolder<ARG1, ARG_SETTER> > setterInterface(
            new SetterHolder<ARG1, ARG_SETTER>(setter));

        data.arg1Setter = setterInterface;

        return *(static_cast<SELF*>(this));
    }
};

template <class INVOCATION_DATA, class SELF, class ARG1, class ARG2>
struct InvocationBaseSaveSetArg<INVOCATION_DATA, SELF, ARG1, ARG2>
: public InvocationBaseSaveSetArg<INVOCATION_DATA, SELF, ARG1> {
    template <class ARG_EXTRACTOR>
    SELF& saveArg2(const ARG_EXTRACTOR& extractor)
    {
        INVOCATION_DATA& data = this->getInvocationDataBack();

        bsl::shared_ptr<ExtractorHolder<ARG2, ARG_EXTRACTOR> >
            extractorInterface(
                new ExtractorHolder<ARG2, ARG_EXTRACTOR>(extractor));

        data.arg2Extractor = extractorInterface;

        return *(static_cast<SELF*>(this));
    }

    template <class ARG_SETTER>
    SELF& setArg2(const ARG_SETTER& setter)
    {
        INVOCATION_DATA& data = this->getInvocationDataBack();

        bsl::shared_ptr<SetterHolder<ARG2, ARG_SETTER> > setterInterface(
            new SetterHolder<ARG2, ARG_SETTER>(setter));

        data.arg2Setter = setterInterface;

        return *(static_cast<SELF*>(this));
    }
};

template <class INVOCATION_DATA,
          class SELF,
          class ARG1,
          class ARG2,
          class ARG3>
struct InvocationBaseSaveSetArg<INVOCATION_DATA, SELF, ARG1, ARG2, ARG3>
: public InvocationBaseSaveSetArg<INVOCATION_DATA, SELF, ARG1, ARG2> {
    template <class ARG_EXTRACTOR>
    SELF& saveArg3(const ARG_EXTRACTOR& extractor)
    {
        INVOCATION_DATA& data = this->getInvocationDataBack();

        bsl::shared_ptr<ExtractorHolder<ARG3, ARG_EXTRACTOR> >
            extractorInterface(
                new ExtractorHolder<ARG3, ARG_EXTRACTOR>(extractor));

        data.arg3Extractor = extractorInterface;

        return *(static_cast<SELF*>(this));
    }

    template <class ARG_SETTER>
    SELF& setArg3(const ARG_SETTER& setter)
    {
        INVOCATION_DATA& data = this->getInvocationDataBack();

        bsl::shared_ptr<SetterHolder<ARG3, ARG_SETTER> > setterInterface(
            new SetterHolder<ARG3, ARG_SETTER>(setter));

        data.arg3Setter = setterInterface;

        return *(static_cast<SELF*>(this));
    }
};

template <class INVOCATION_DATA, class METHOD_INFO>
struct InvocationDataStorage {
    bsl::list<INVOCATION_DATA> d_invocations;

    ~InvocationDataStorage()
    {
        for (typename bsl::list<INVOCATION_DATA>::const_iterator it =
                 d_invocations.cbegin();
             it != d_invocations.cend();
             ++it)
        {
            if (it->d_expectedCalls != INVOCATION_DATA::k_INFINITE_CALLS) {
                BSLS_LOG_FATAL(
                    "Invocation \"%s\" did not fire but was expected "
                    "to fire %d times",
                    METHOD_INFO().name,
                    it->d_expectedCalls);
                bsl::abort();
            }
        }
    }
};

template <class METHOD_INFO, class RESULT>
struct Invocation0
: public InvocationBaseWillReturn<InvocationData<RESULT>,
                                  Invocation0<METHOD_INFO, RESULT>,
                                  RESULT>,
  public InvocationBaseTimes<InvocationData<RESULT>,
                             Invocation0<METHOD_INFO, RESULT> > {
    typedef InvocationData<RESULT> InvocationDataT;

    RESULT invoke()
    {
        NTCCFG_TEST_FALSE(d_storage.d_invocations.empty());
        InvocationDataT& invocation = d_storage.d_invocations.front();
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
        }
        InvocationResult<RESULT> result = invocation.d_result;  //copy by value
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            --invocation.d_expectedCalls;
            if (invocation.d_expectedCalls == 0) {
                d_storage.d_invocations.pop_front();
            }
        }
        return result.get();
    }

    Invocation0& expect()
    {
        if (!d_storage.d_invocations.empty()) {
            NTCCFG_TEST_NE(d_storage.d_invocations.back().d_expectedCalls,
                           InvocationDataT::k_INFINITE_CALLS);
        }
        d_storage.d_invocations.emplace_back();
        return *this;
    }

    InvocationDataT& getInvocationDataBack()
    {
        NTCCFG_TEST_FALSE(d_storage.d_invocations.empty());
        return d_storage.d_invocations.back();
    }

    InvocationDataStorage<InvocationDataT, METHOD_INFO> d_storage;
};

template <class T>
struct TypeToType {
    typedef T Type;
};

template <class METHOD_INFO, class RESULT, class ARG1>
struct Invocation1

: public InvocationBaseWillReturn<InvocationData<RESULT, ARG1>,
                                  Invocation1<METHOD_INFO, RESULT, ARG1>,
                                  RESULT>,
  public InvocationBaseTimes<InvocationData<RESULT, ARG1>,
                             Invocation1<METHOD_INFO, RESULT, ARG1> >,
  public InvocationBaseSaveSetArg<InvocationData<RESULT, ARG1>,
                                  Invocation1<METHOD_INFO, RESULT, ARG1>,
                                  ARG1>

{
    typedef InvocationData<RESULT, ARG1> InvocationDataT;
    typedef ARG1                         ARG1_TYPE;
    TypeToType<ARG1>                     k_overload_key;  //TODO: make static

    RESULT invoke(ARG1 arg)
    {
        NTCCFG_TEST_FALSE(d_storage.d_invocations.empty());
        InvocationDataT& invocation = d_storage.d_invocations.front();
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
        }

        invocation.processArgs(arg);

        InvocationResult<RESULT> result = invocation.d_result;  //copy by value
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            --invocation.d_expectedCalls;
            if (invocation.d_expectedCalls == 0) {
                d_storage.d_invocations.pop_front();
            }
        }
        return result.get();
    }

    template <class ARG1_MATCHER>
    Invocation1& expect(const ARG1_MATCHER& arg1Matcher)
    {
        if (!d_storage.d_invocations.empty()) {
            NTCCFG_TEST_NE(d_storage.d_invocations.back().d_expectedCalls,
                           InvocationDataT::k_INFINITE_CALLS);
        }
        d_storage.d_invocations.emplace_back();

        if (!arg1Matcher.ignore()) {
            InvocationDataT& data = d_storage.d_invocations.back();
            bsl::shared_ptr<MatcherHolder<ARG1, ARG1_MATCHER> >
                matcherInterface(
                    new MatcherHolder<ARG1, ARG1_MATCHER>(arg1Matcher));

            data.arg1Matcher = matcherInterface;
        }
        return *this;
    }

    InvocationDataT& getInvocationDataBack()
    {
        NTCCFG_TEST_FALSE(d_storage.d_invocations.empty());
        return d_storage.d_invocations.back();
    }

    InvocationDataStorage<InvocationDataT, METHOD_INFO> d_storage;
};

template <class METHOD_INFO, class RESULT, class ARG1, class ARG2>
struct Invocation2

: public InvocationBaseWillReturn<InvocationData<RESULT, ARG1, ARG2>,
                                  Invocation2<METHOD_INFO, RESULT, ARG1, ARG2>,
                                  RESULT>,
  public InvocationBaseTimes<InvocationData<RESULT, ARG1, ARG2>,
                             Invocation2<METHOD_INFO, RESULT, ARG1, ARG2> >,
  public InvocationBaseSaveSetArg<InvocationData<RESULT, ARG1, ARG2>,
                                  Invocation2<METHOD_INFO, RESULT, ARG1, ARG2>,
                                  ARG1,
                                  ARG2>

{
    typedef InvocationData<RESULT, ARG1, ARG2> InvocationDataT;

    RESULT invoke(ARG1 arg1, ARG2 arg2)
    {
        NTCCFG_TEST_FALSE(d_storage.d_invocations.empty());
        InvocationDataT& invocation = d_storage.d_invocations.front();
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
        }

        invocation.processArgs(arg1, arg2);

        InvocationResult<RESULT> result = invocation.d_result;  //copy by value
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            --invocation.d_expectedCalls;
            if (invocation.d_expectedCalls == 0) {
                d_storage.d_invocations.pop_front();
            }
        }
        return result.get();
    }

    template <class ARG1_MATCHER, class ARG2_MATCHER>
    Invocation2& expect(const ARG1_MATCHER& arg1Matcher,
                        const ARG2_MATCHER& arg2Matcher)
    {
        if (!d_storage.d_invocations.empty()) {
            NTCCFG_TEST_NE(d_storage.d_invocations.back().d_expectedCalls,
                           InvocationDataT::k_INFINITE_CALLS);
        }
        d_storage.d_invocations.emplace_back();
        InvocationDataT& data = d_storage.d_invocations.back();
        if (!arg1Matcher.ignore()) {
            bsl::shared_ptr<MatcherHolder<ARG1, ARG1_MATCHER> >
                matcherInterface1(
                    new MatcherHolder<ARG1, ARG1_MATCHER>(arg1Matcher));

            data.arg1Matcher = matcherInterface1;
        }

        if (!arg2Matcher.ignore()) {
            bsl::shared_ptr<MatcherHolder<ARG2, ARG2_MATCHER> >
                matcherInterface2(
                    new MatcherHolder<ARG2, ARG2_MATCHER>(arg2Matcher));

            data.arg2Matcher = matcherInterface2;
        }
        return *this;
    }

    InvocationDataT& getInvocationDataBack()
    {
        NTCCFG_TEST_FALSE(d_storage.d_invocations.empty());
        return d_storage.d_invocations.back();
    }

    InvocationDataStorage<InvocationDataT, METHOD_INFO> d_storage;
};

template <class METHOD_INFO, class RESULT, class ARG1, class ARG2, class ARG3>
struct Invocation3

: public InvocationBaseWillReturn<
      InvocationData<RESULT, ARG1, ARG2, ARG3>,
      Invocation3<METHOD_INFO, RESULT, ARG1, ARG2, ARG3>,
      RESULT>,
  public InvocationBaseTimes<
      InvocationData<RESULT, ARG1, ARG2, ARG3>,
      Invocation3<METHOD_INFO, RESULT, ARG1, ARG2, ARG3> >,
  public InvocationBaseSaveSetArg<
      InvocationData<RESULT, ARG1, ARG2, ARG3>,
      Invocation3<METHOD_INFO, RESULT, ARG1, ARG2, ARG3>,
      ARG1,
      ARG2,
      ARG3>

{
    typedef InvocationData<RESULT, ARG1, ARG2, ARG3> InvocationDataT;

    RESULT invoke(ARG1 arg1, ARG2 arg2, ARG3 arg3)
    {
        NTCCFG_TEST_FALSE(d_storage.d_invocations.empty());
        InvocationDataT& invocation = d_storage.d_invocations.front();
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);
        }

        invocation.processArgs(arg1, arg2, arg3);

        InvocationResult<RESULT> result = invocation.d_result;  //copy by value
        if (invocation.d_expectedCalls != InvocationDataT::k_INFINITE_CALLS) {
            --invocation.d_expectedCalls;
            if (invocation.d_expectedCalls == 0) {
                d_storage.d_invocations.pop_front();
            }
        }
        return result.get();
    }

    template <class ARG1_MATCHER, class ARG2_MATCHER, class ARG3_MATCHER>
    Invocation3& expect(const ARG1_MATCHER& arg1Matcher,
                        const ARG2_MATCHER& arg2Matcher,
                        const ARG3_MATCHER& arg3Matcher)
    {
        if (!d_storage.d_invocations.empty()) {
            NTCCFG_TEST_NE(d_storage.d_invocations.back().d_expectedCalls,
                           InvocationDataT::k_INFINITE_CALLS);
        }
        d_storage.d_invocations.emplace_back();
        InvocationDataT& data = d_storage.d_invocations.back();
        if (!arg1Matcher.ignore()) {
            bsl::shared_ptr<MatcherHolder<ARG1, ARG1_MATCHER> >
                matcherInterface1(
                    new MatcherHolder<ARG1, ARG1_MATCHER>(arg1Matcher));

            data.arg1Matcher = matcherInterface1;
        }

        if (!arg2Matcher.ignore()) {
            bsl::shared_ptr<MatcherHolder<ARG2, ARG2_MATCHER> >
                matcherInterface2(
                    new MatcherHolder<ARG2, ARG2_MATCHER>(arg2Matcher));

            data.arg2Matcher = matcherInterface2;
        }

        if (!arg3Matcher.ignore()) {
            bsl::shared_ptr<MatcherHolder<ARG3, ARG3_MATCHER> >
                matcherInterface3(
                    new MatcherHolder<ARG3, ARG3_MATCHER>(arg3Matcher));

            data.arg3Matcher = matcherInterface3;
        }

        return *this;
    }

    InvocationDataT& getInvocationDataBack()
    {
        NTCCFG_TEST_FALSE(d_storage.d_invocations.empty());
        return d_storage.d_invocations.back();
    }

    InvocationDataStorage<InvocationDataT, METHOD_INFO> d_storage;
};

}

#define NTF_METHOD_INFO(METHOD_NAME)                                          \
    struct NTF_CAT2(MethodInfo, __LINE__) {                                   \
        const char* name;                                                     \
        NTF_CAT2(MethodInfo, __LINE__)                                        \
        ()                                                                    \
        : name(#METHOD_NAME)                                                  \
        {                                                                     \
        }                                                                     \
    };

#define NTF_MOCK_METHOD_0_IMP(RESULT, METHOD_NAME)                            \
    NTF_METHOD_INFO(METHOD_NAME)                                              \
                                                                              \
  public:                                                                     \
    ntf_mock::Invocation0<NTF_CAT2(MethodInfo, __LINE__), RESULT>&            \
        expect_##METHOD_NAME()                                                \
    {                                                                         \
        return d_invocation_##METHOD_NAME.expect();                           \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable ntf_mock::Invocation0<NTF_CAT2(MethodInfo, __LINE__), RESULT>     \
        d_invocation_##METHOD_NAME;

#define NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1)                      \
    NTF_METHOD_INFO(METHOD_NAME)                                              \
  public:                                                                     \
    template <class MATCHER>                                                  \
    ntf_mock::Invocation1<NTF_CAT2(MethodInfo, __LINE__), RESULT, ARG1>&      \
        expect_##METHOD_NAME(const MATCHER& arg1,                             \
                             ntf_mock::TypeToType<ARG1> =                     \
                                 ntf_mock::TypeToType<ARG1>())                \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__).expect(arg1);   \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable ntf_mock::                                                        \
        Invocation1<NTF_CAT2(MethodInfo, __LINE__), RESULT, ARG1>             \
            NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__);

#define NTF_MOCK_METHOD_2_IMP(RESULT, METHOD_NAME, ARG1, ARG2)                \
    NTF_METHOD_INFO(METHOD_NAME)                                              \
  public:                                                                     \
    template <class MATCHER1, class MATCHER2>                                 \
    ntf_mock::                                                                \
        Invocation2<NTF_CAT2(MethodInfo, __LINE__), RESULT, ARG1, ARG2>&      \
            expect_##METHOD_NAME(const MATCHER1& arg1, const MATCHER2& arg2)  \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__)                 \
            .expect(arg1, arg2);                                              \
    }                                                                         \
                                                                              \
    template <class MATCHER1, class MATCHER2>                                 \
    ntf_mock::                                                                \
        Invocation2<NTF_CAT2(MethodInfo, __LINE__), RESULT, ARG1, ARG2>&      \
            expect_##METHOD_NAME(const MATCHER1& arg1,                        \
                                 ntf_mock::TypeToType<ARG1>,                  \
                                 const MATCHER2& arg2,                        \
                                 ntf_mock::TypeToType<ARG2> =                 \
                                     ntf_mock::TypeToType<ARG2>())            \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__)                 \
            .expect(arg1, arg2);                                              \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable ntf_mock::                                                        \
        Invocation2<NTF_CAT2(MethodInfo, __LINE__), RESULT, ARG1, ARG2>       \
            NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__);

#define NTF_MOCK_METHOD_3_IMP(RESULT, METHOD_NAME, ARG1, ARG2, ARG3)          \
    NTF_METHOD_INFO(METHOD_NAME)                                              \
  public:                                                                     \
    template <class MATCHER1, class MATCHER2, class MATCHER3>                 \
    ntf_mock::Invocation3<NTF_CAT2(MethodInfo, __LINE__),                     \
                          RESULT,                                             \
                          ARG1,                                               \
                          ARG2,                                               \
                          ARG3>& expect_##METHOD_NAME(const MATCHER1& arg1,   \
                                                      const MATCHER2& arg2,   \
                                                      const MATCHER3& arg3)   \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__)                 \
            .expect(arg1, arg2, arg3);                                        \
    }                                                                         \
                                                                              \
    template <class MATCHER1, class MATCHER2, class MATCHER3>                 \
    ntf_mock::Invocation3<NTF_CAT2(MethodInfo, __LINE__),                     \
                          RESULT,                                             \
                          ARG1,                                               \
                          ARG2,                                               \
                          ARG3>&                                              \
        expect_##METHOD_NAME(const MATCHER1& arg1,                            \
                             ntf_mock::TypeToType<ARG1>,                      \
                             const MATCHER2& arg2,                            \
                             ntf_mock::TypeToType<ARG2>,                      \
                             const MATCHER3& arg3,                            \
                             ntf_mock::TypeToType<ARG3>)                      \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__)                 \
            .expect(arg1, arg2, arg3);                                        \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable ntf_mock::                                                        \
        Invocation3<NTF_CAT2(MethodInfo, __LINE__), RESULT, ARG1, ARG2, ARG3> \
            NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__);

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
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__).invoke(arg1);   \
    }                                                                         \
    NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1)

#define NTF_MOCK_METHOD_CONST_1(RESULT, METHOD_NAME, ARG1)                    \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1) const override                              \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__).invoke(arg1);   \
    }                                                                         \
    NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1)

#define NTF_MOCK_METHOD_2(RESULT, METHOD_NAME, ARG1, ARG2)                    \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1, ARG2 arg2) override                         \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__)                 \
            .invoke(arg1, arg2);                                              \
    }                                                                         \
    NTF_MOCK_METHOD_2_IMP(RESULT, METHOD_NAME, ARG1, ARG2)

#define NTF_MOCK_METHOD_CONST_2(RESULT, METHOD_NAME, ARG1, ARG2)              \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1, ARG2 arg2) const override                   \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__)                 \
            .invoke(arg1, arg2);                                              \
    }                                                                         \
    NTF_MOCK_METHOD_2_IMP(RESULT, METHOD_NAME, ARG1, ARG2)

#define NTF_MOCK_METHOD_3(RESULT, METHOD_NAME, ARG1, ARG2, ARG3)              \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1, ARG2 arg2, ARG3 arg3) override              \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__)                 \
            .invoke(arg1, arg2, arg3);                                        \
    }                                                                         \
    NTF_MOCK_METHOD_3_IMP(RESULT, METHOD_NAME, ARG1, ARG2, ARG3)

#define NTF_MOCK_METHOD_CONST_3(RESULT, METHOD_NAME, ARG1, ARG2, ARG3)        \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1, ARG2 arg2, ARG3 arg3) const override        \
    {                                                                         \
        return NTF_CAT2(d_invocation_##METHOD_NAME, __LINE__)                 \
            .invoke(arg1, arg2, arg3);                                        \
    }                                                                         \
    NTF_MOCK_METHOD_3_IMP(RESULT, METHOD_NAME, ARG1, ARG2, ARG3)

#define NTF_MOCK_METHOD(...) NTF_VA_SELECT(NTF_MOCK_METHOD, __VA_ARGS__)

#define NTF_MOCK_METHOD_CONST(...)                                            \
    NTF_VA_SELECT(NTF_MOCK_METHOD_CONST, __VA_ARGS__)

#define NTF_EQ(ARG) ntf_mock::createEqMatcher<ntf_mock::DirectComparator>(ARG)
#define NTF_EQ_SPEC(ARG, SPEC)                                                \
    ntf_mock::createEqMatcher<ntf_mock::DirectComparator>(ARG),               \
        ntf_mock::TypeToType<SPEC>()
#define IGNORE_ARG ntf_mock::IgnoreArg()
#define IGNORE_ARG_S(SPEC) ntf_mock::IgnoreArg(), ntf_mock::TypeToType<SPEC>()

#define NTF_EQ_DEREF(ARG)                                                     \
    ntf_mock::createEqMatcher<ntf_mock::DerefComparator>(ARG)
#define NTF_EQ_DEREF_SPEC(ARG, SPEC)                                          \
    ntf_mock::createEqMatcher<ntf_mock::DerefComparator>(ARG),                \
        ntf_mock::TypeToType<SPEC>()

#define NTF_EXPECT_0(MOCK_OBJECT, METHOD) (MOCK_OBJECT).expect_##METHOD()
#define NTF_EXPECT_1(MOCK_OBJECT, METHOD, ...)                                \
    (MOCK_OBJECT).expect_##METHOD(__VA_ARGS__)
#define NTF_EXPECT_2(MOCK_OBJECT, METHOD, ...)                                \
    (MOCK_OBJECT).expect_##METHOD(__VA_ARGS__)
#define NTF_EXPECT_3(MOCK_OBJECT, METHOD, ...)                                \
    (MOCK_OBJECT).expect_##METHOD(__VA_ARGS__)
#define NTF_EXPECT_4(MOCK_OBJECT, METHOD, ...)                                \
    (MOCK_OBJECT).expect_##METHOD(__VA_ARGS__)

#define ONCE() willOnce()
#define ALWAYS() willAlways()
#define TIMES(x) willTimes(x)
#define RETURN(VAL) willReturn(VAL)
#define RETURNREF(VAL) willReturnRef(&VAL)

#define TO(ARG) ntf_mock::createExtractor<ntf_mock::NoDerefPolicy>(ARG)
#define TO_DEREF(ARG) ntf_mock::createExtractor<ntf_mock::DerefPolicy>(ARG)

#define SAVE_ARG_1(...) saveArg1(__VA_ARGS__)
#define SAVE_ARG_2(...) saveArg2(__VA_ARGS__)
#define SAVE_ARG_3(...) saveArg1(__VA_ARGS__)
#define SAVE_ARG_4(...) saveArg2(__VA_ARGS__)

#define FROM(ARG) ntf_mock::createSetter<ntf_mock::DefaultSetter>(ARG)

#define FROM_DEREF(ARG) ntf_mock::createSetter<ntf_mock::DerefSetter>(ARG)

#define SET_ARG_1(...) setArg1(__VA_ARGS__)
#define SET_ARG_2(...) setArg2(__VA_ARGS__)
#define SET_ARG_3(...) setArg3(__VA_ARGS__)
#define SET_ARG_4(...) setArg4(__VA_ARGS__)