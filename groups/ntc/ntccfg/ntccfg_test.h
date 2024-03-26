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

#define NTF_MOCK_METHOD_INVOKE_COMMON_IMP()                                   \
    NTCCFG_TEST_FALSE(d_invocations.empty());                                 \
    InvocationData& invocation = d_invocations.front();                       \
    if (invocation.d_expectedCalls != -1) {                                   \
        NTCCFG_TEST_GE(invocation.d_expectedCalls, 1);                        \
    }                                                                         \
    NTCCFG_TEST_TRUE(invocation.d_result.has_value());                        \
    const auto result = invocation.d_result.value();                          \
    if (invocation.d_expectedCalls != -1) {                                   \
        --invocation.d_expectedCalls;                                         \
        if (invocation.d_expectedCalls == 0) {                                \
            d_invocations.pop_front();                                        \
        }                                                                     \
    }

#define NTF_MOCK_METHOD_COMMON_IMP(RESULT, METHOD_NAME)                       \
  public:                                                                     \
    Invocation_##METHOD_NAME& expect()                                        \
    {                                                                         \
        d_invocations.emplace_back();                                         \
        InvocationData& invocation = d_invocations.back();                    \
        return *this;                                                         \
    }                                                                         \
    Invocation_##METHOD_NAME& willOnce()                                      \
    {                                                                         \
        NTCCFG_TEST_FALSE(d_invocations.empty());                             \
                                                                              \
        InvocationData& invocation = d_invocations.back();                    \
        NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);                        \
                                                                              \
        invocation.d_expectedCalls = 1;                                       \
        return *this;                                                         \
    }                                                                         \
    Invocation_##METHOD_NAME& willAlways()                                    \
    {                                                                         \
        NTCCFG_TEST_FALSE(d_invocations.empty());                             \
                                                                              \
        InvocationData& invocation = d_invocations.back();                    \
        NTCCFG_TEST_EQ(invocation.d_expectedCalls, 0);                        \
                                                                              \
        invocation.d_expectedCalls = -1;                                      \
        return *this;                                                         \
    }                                                                         \
                                                                              \
    Invocation_##METHOD_NAME& willReturn(const RESULT& result)                \
    {                                                                         \
        NTCCFG_TEST_FALSE(d_invocations.empty());                             \
        InvocationData& invocation = d_invocations.back();                    \
        invocation.d_result        = result;                                  \
        return *this;                                                         \
    }                                                                         \
                                                                              \
  private:                                                                    \
    bsl::list<InvocationData> d_invocations;

#define NTF_MOCK_METHOD_0_IMP(RESULT, METHOD_NAME)                            \
  public:                                                                     \
    class Invocation_##METHOD_NAME                                            \
    {                                                                         \
      private:                                                                \
        struct InvocationData {                                               \
            int                         d_expectedCalls;                      \
            bdlb::NullableValue<RESULT> d_result;                             \
                                                                              \
            InvocationData()                                                  \
            : d_expectedCalls(0)                                              \
            , d_result()                                                      \
            {                                                                 \
            }                                                                 \
        };                                                                    \
                                                                              \
        NTF_MOCK_METHOD_COMMON_IMP(RESULT, METHOD_NAME)                       \
                                                                              \
      public:                                                                 \
        RESULT invoke()                                                       \
        {                                                                     \
            NTF_MOCK_METHOD_INVOKE_COMMON_IMP()                               \
            return result;                                                    \
        }                                                                     \
    };                                                                        \
                                                                              \
    Invocation_##METHOD_NAME& expect_##METHOD_NAME()                          \
    {                                                                         \
        return d_invocation_##METHOD_NAME.expect();                           \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable Invocation_##METHOD_NAME d_invocation_##METHOD_NAME;

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



#define NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1)                      \
  public:                                                                     \
    class Invocation_##METHOD_NAME                                            \
    {                                                                         \
      private:                                                                \
        struct InvocationData {                                               \
            int                         d_expectedCalls;                      \
            bdlb::NullableValue<RESULT> d_result;                             \
            bdlb::NullableValue<ARG1>   d_arg1;                               \
            ARG1*                       d_arg1_out;                           \
            InvocationData()                                                  \
            : d_expectedCalls(0)                                              \
            , d_result()                                                      \
            , d_arg1                                                          \
            {                                                                 \
            }                                                                 \
        };                                                                    \
                                                                              \
        NTF_MOCK_METHOD_COMMON_IMP(RESULT, METHOD_NAME)                       \
                                                                              \
        Invocation_##METHOD_NAME& saveArg1To(ARG1& arg1)                      \
        {                                                                     \
            NTCCFG_TEST_FALSE(d_invocations.empty());                         \
            InvocationData& invocation = d_invocations.back();                \
            invocation.d_arg1_out      = &arg1;                               \
            return *this;                                                     \
        }                                                                     \
                                                                              \
      public:                                                                 \
        RESULT invoke(const ARG1& arg1)                                       \
        {                                                                     \
            NTF_MOCK_METHOD_INVOKE_COMMON_IMP()                               \
            if (invocation.d_arg1.has_value()) {                              \
                NTCCFG_TEST_EQ(arg1, invocation.d_arg1.value());              \
            }                                                                 \
            if (invocation.d_arg1_out) {                                      \
                *invocation.d_arg1_out = arg1;                                \
            }                                                                 \
            return result;                                                    \
        }                                                                     \
    };                                                                        \
                                                                              \
    Invocation_##METHOD_NAME& expect_##METHOD_NAME()                          \
    {                                                                         \
        return d_invocation_##METHOD_NAME.expect();                           \
    }                                                                         \
                                                                              \
  private:                                                                    \
    mutable Invocation_##METHOD_NAME d_invocation_##METHOD_NAME;

#define NTF_MOCK_METHOD_1(RESULT, METHOD_NAME, ARG1)                          \
  public:                                                                     \
    /*TODO: invoke and correctly pass the argument, saving all CVs*/          \
    RESULT METHOD_NAME(ARG1 arg1) override                                    \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke(arg1);                       \
    }                                                                         \
    NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1)

#define NTF_MOCK_METHOD_1_CONST(RESULT, METHOD_NAME, ARG1)                    \
  public:                                                                     \
    RESULT METHOD_NAME(ARG1 arg1) const override                              \
    {                                                                         \
        return d_invocation_##METHOD_NAME.invoke(arg1);                       \
    }                                                                         \
    NTF_MOCK_METHOD_1_IMP(RESULT, METHOD_NAME, ARG1

#define NTF_EXPAND( X ) X
#define NTF_CAT( A, B ) A ## B
#define NTF_SELECT( NAME, NUM ) NTF_CAT( NAME ## _, NUM )

#define NTF_GET_COUNT( _1, _2, _3, _4, _5, _6, COUNT, ... ) COUNT
#define NTF_VA_SIZE( ... ) NTF_EXPAND( NTF_GET_COUNT( __VA_ARGS__, 4, 3, 2, 1, 0, -1 ) )

#define NTF_VA_SELECT( NAME, ... ) NTF_SELECT( NAME, NTF_VA_SIZE(__VA_ARGS__) )(__VA_ARGS__)

#define NTF_MOCK_METHOD( ... ) NTF_VA_SELECT( NTF_MOCK_METHOD, __VA_ARGS__ )
#define NTF_MOCK_METHOD_CONST( ... ) NTF_VA_SELECT( NTF_MOCK_METHOD_CONST, __VA_ARGS__ )