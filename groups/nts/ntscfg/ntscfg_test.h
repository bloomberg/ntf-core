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

#ifndef INCLUDED_NTSCFG_TEST
#define INCLUDED_NTSCFG_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_config.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocator.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_map.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#if NTS_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR
#include <balst_stacktracetestallocator.h>
#endif

namespace BloombergLP {
namespace ntscfg {

/// @internal @brief
/// Describe a test case context.
///
/// @ingroup module_ntscfg
class TestCaseContext 
{
    int                   d_number;
    bsl::string           d_name;
    bsl::function<void()> d_function;

private:
    TestCaseContext(const TestCaseContext&) BSLS_KEYWORD_DELETED;
    TestCaseContext& operator=(const TestCaseContext&) BSLS_KEYWORD_DELETED;

public:
    /// Create a new test case context. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently 
    /// installed global allocator is used.
    explicit TestCaseContext(bslma::Allocator* basicAllocator = 0);

    /// Create a new test case context. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently 
    /// installed global allocator is used.
    TestCaseContext(const TestCaseContext& original, 
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~TestCaseContext();

    /// Set the case number to the specified 'value'.
    void setNumber(int value);

    /// Set the case name to the specified 'value'.
    void setName(const bsl::string& value);

    // Set the function that implements the test case to the specified 'value'.
    void setFunction(const bsl::function<void()>& value);

    /// Execute the test.
    void execute();

    /// Return the case number.
    int number() const;

    /// Return the case name.
    const bsl::string& name() const;

    /// Return true if the test case is defined, otherwise return false.
    bool isDefined() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(TestCaseContext);
};

NTSCFG_INLINE
TestCaseContext::TestCaseContext(bslma::Allocator* basicAllocator)
: d_number(0)
, d_name(basicAllocator ? basicAllocator : bslma::Default::globalAllocator())
, d_function(bsl::allocator_arg, 
             basicAllocator ? basicAllocator : 
             bslma::Default::globalAllocator())
{
}

NTSCFG_INLINE
TestCaseContext::TestCaseContext(const TestCaseContext& original, 
                                 bslma::Allocator*      basicAllocator)
: d_number(original.d_number)
, d_name(original.d_name, 
         basicAllocator ? basicAllocator : bslma::Default::globalAllocator())
, d_function(bsl::allocator_arg, 
             basicAllocator ? basicAllocator : 
             bslma::Default::globalAllocator(),
             original.d_function)
{
}

NTSCFG_INLINE
TestCaseContext::~TestCaseContext()
{
}

NTSCFG_INLINE
void TestCaseContext::setNumber(int value)
{
    d_number = value;
}

NTSCFG_INLINE
void TestCaseContext::setName(const bsl::string& value)
{
    d_name = value;
}

NTSCFG_INLINE
void TestCaseContext::setFunction(const bsl::function<void()>& value)
{
    d_function = value;
}

NTSCFG_INLINE
void TestCaseContext::execute()
{
    BSLS_LOG_INFO("Testing: %s", d_name.c_str());
    d_function();
}

NTSCFG_INLINE
int TestCaseContext::number() const
{
    return d_number;
}

NTSCFG_INLINE
const bsl::string& TestCaseContext::name() const
{
    return d_name;
}

NTSCFG_INLINE
bool TestCaseContext::isDefined() const
{
    if (d_function) {
        return true;
    }
    else {
        return false;
    }
}

/// @internal @brief
/// The test case number.
///
/// @ingroup module_ntscfg
extern int testCase;

/// @internal @brief
/// The test verbosity level.
///
/// @ingroup module_ntscfg
extern int testVerbosity;

/// @internal @brief
/// The number of arguments specified when the test driver was executed.
///
/// @ingroup module_ntscfg
extern int testArgc;

/// @internal @brief
/// The argument vector specified when the test driver was executed.
///
/// @ingroup module_ntscfg
extern char** testArgv;

/// @internal @brief
/// The test allocator.
///
/// @ingroup module_ntscfg
extern bslma::Allocator* testAllocator;

/// @internal @brief
/// Defines a type alias for a function invoked to initialize any global state
/// used by the test driver.
///
/// @ingroup module_ntscfg
typedef void (*TestInitCallback)();

/// @internal @brief
/// Defines a type alias for a function invoked to clean up any global state
/// used by the test driver.
///
/// @ingroup module_ntscfg
typedef void (*TestExitCallback)();

/// @internal @brief
/// Defines a type alias for a function invoked to execute a test case.
///
/// @ingroup module_ntscfg
typedef bsl::function<void()> TestCaseFunction;

/// @internal @brief
/// Defines a type alias for a map of test case numbers to the associated 
/// function to invoke to execute that test case.
///
/// @ingroup module_ntscfg
typedef bsl::map<int, ntscfg::TestCaseContext> TestCaseContextMap;

/// @internal @brief
/// Defines a type alias for a map of test case numbers to the associated 
/// function to invoke to execute that test case.
///
/// @ingroup module_ntscfg
typedef bsl::vector<TestCaseContextMap::iterator> TestCaseContextVector;

/// The function invoked to initialize any global state used by the test
/// driver.
///
/// @ingroup module_ntscfg
extern TestInitCallback testInit;

/// The function invoked to clean up any global state used by the test driver.
///
/// @ingroup module_ntscfg
extern TestExitCallback testExit;

/// @internal @brief
/// Provide a guard to automatically call any registered initialization or
/// exit functions in the context of a test driver.
///
/// @ingroup module_ntscfg
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

#if NTS_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR

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
/// @ingroup module_ntscfg
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
/// @ingroup module_ntscfg
class TestAllocator : public bslma::Allocator
{
    bslma::TestAllocator d_base;

  public:
    /// @internal @brief
    /// Create a new test allocator.
    TestAllocator();

    /// @internal @brief
    /// Destroy this object.
    virtual ~TestAllocator();

    /// @internal @brief
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

    /// @internal @brief
    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.
    virtual void deallocate(void* address);

    /// @internal @brief
    /// Return the number of blocks currently allocated from this object.
    bsl::int64_t numBlocksInUse() const;
};

#endif

}  // close package namespace
}  // close enterprise namespace




/// @internal @brief
/// Assert the 'expression' is true.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_ASSERT(expression)                                        \
    do {                                                                      \
        if (!(expression)) {                                                  \
            BSLS_LOG_FATAL("Assertion failed: %s", #expression);              \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'expression' is true.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_TRUE(expression)                                          \
    do {                                                                      \
        if (!(expression)) {                                                  \
            BSLS_LOG_FATAL("Assertion false: %s", #expression);               \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'expression' is false.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_FALSE(expression)                                         \
    do {                                                                      \
        if ((expression)) {                                                   \
            BSLS_LOG_FATAL("Assertion true: %s", #expression);                \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Assert the 'found' value equals the 'expected' value.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_EQ(found, expected)                                       \
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_NE(found, expected)                                       \
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LT(found, expected)                                       \
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LE(found, expected)                                       \
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_GT(found, expected)                                       \
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_GE(found, expected)                                       \
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_OK(found)                                                 \
    NTSCFG_TEST_EQ(found, ntsa::Error(ntsa::Error::e_OK))

/// @internal @brief
/// Assert the 'found' error value has the same value as the specified
/// 'expected' error value.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_ERROR(found, expected)                                    \
    NTSCFG_TEST_EQ(found, ntsa::Error(expected))

/// @internal @brief
/// The verbosity at which the test driver is run.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_VERBOSITY BloombergLP::ntscfg::testVerbosity

/// @internal @brief
/// The number of command line arguments.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_ARGC BloombergLP::ntscfg::testArgc

/// @internal @brief
/// The array of command line arguments.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_ARGV BloombergLP::ntscfg::testArgv

/// @internal @brief
/// The pointer to the test allocator.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_ALLOCATOR BloombergLP::ntscfg::testAllocator

/// @internal @brief
/// Begin a functional test case identified by the specified 'number'.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_CASE(number) void runTestCase##number()

/// @internal @brief
/// Define the beginning of a callback function to be run after all the common
/// test mechanisms are initialized but before any of the test cases are run.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_INIT()                                                    \
    void runTestInit();                                                       \
    struct runTestInitBinder {                                                \
        runTestInitBinder()                                                   \
        {                                                                     \
            BloombergLP::ntscfg::testInit = &runTestInit;                     \
        }                                                                     \
    } s_runTestInitBinder;                                                    \
    void runTestInit()

/// @internal @brief
/// Define the beginning of a callback function to be run after all the test
/// cases are run but before the the common test mechanisms are destroyed.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_EXIT()                                                    \
    void runTestExit();                                                       \
    struct runTestExitBinder {                                                \
        runTestExitBinder()                                                   \
        {                                                                     \
            BloombergLP::ntscfg::testExit = &runTestExit;                     \
        }                                                                     \
    } s_runTestExitBinder;                                                    \
    void runTestExit()

/// @internal @brief
/// Begin the definition of the dispatcher for a component test driver.
///
/// @par Usage Example: The skeleton of a test driver
/// This example shows the basic skeleton of a test driver.
///
///     #include <ntscfg_test.h>
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_DRIVER                                                    \
    namespace BloombergLP {                                                   \
    namespace ntscfg {                                                        \
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_DRIVER_END                                                \
    return -1;                                                                \
    }                                                                         \
                                                                              \
    int main(int argc, char** argv)                                           \
    {                                                                         \
        BloombergLP::ntscfg::Platform::initialize();                          \
        BloombergLP::ntscfg::Platform::ignore(                                \
            BloombergLP::ntscfg::Signal::e_PIPE);                             \
                                                                              \
        BloombergLP::bslmt::ThreadUtil::setThreadName("main");                \
                                                                              \
        BloombergLP::bsls::Log::setLogMessageHandler(                         \
            &BloombergLP::ntscfg::printLogMessage);                           \
                                                                              \
        BloombergLP::ntscfg::testCase      = 0;                               \
        BloombergLP::ntscfg::testVerbosity = 0;                               \
        BloombergLP::ntscfg::testArgc      = argc;                            \
        BloombergLP::ntscfg::testArgv      = argv;                            \
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
                BloombergLP::ntscfg::testCase = bsl::atoi(argv[1]);           \
            }                                                                 \
                                                                              \
            if (argc == 2) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 0;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_FATAL);                 \
            }                                                                 \
                                                                              \
            if (argc >= 3) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 2;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_WARN);                  \
            }                                                                 \
                                                                              \
            if (argc >= 4) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 3;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_INFO);                  \
            }                                                                 \
                                                                              \
            if (argc >= 5) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 4;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_DEBUG);                 \
            }                                                                 \
                                                                              \
            if (argc == 6) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 5;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_TRACE);                 \
            }                                                                 \
                                                                              \
            BloombergLP::ntscfg::TestGuard testGuard;                         \
                                                                              \
            if (BloombergLP::ntscfg::testCase == 0) {                         \
                BloombergLP::ntscfg::testCase = 1;                            \
                while (0 == runTest(BloombergLP::ntscfg::testCase,            \
                                    BloombergLP::ntscfg::testVerbosity))      \
                {                                                             \
                    ++BloombergLP::ntscfg::testCase;                          \
                }                                                             \
            }                                                                 \
            else {                                                            \
                if (0 != runTest(BloombergLP::ntscfg::testCase,               \
                                 BloombergLP::ntscfg::testVerbosity))         \
                {                                                             \
                    return -1;                                                \
                }                                                             \
            }                                                                 \
        }                                                                     \
        catch (const bsl::exception& e) {                                     \
            bsl::printf("Test %d failed: %s\n",                               \
                        BloombergLP::ntscfg::testCase,                        \
                        e.what());                                            \
            return 1;                                                         \
        }                                                                     \
        catch (...) {                                                         \
            bsl::printf("Test %d failed: %s\n",                               \
                        BloombergLP::ntscfg::testCase,                        \
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
/// @ingroup module_ntscfg
#define NTSCFG_TEST_REGISTER(number)                                          \
    case number:                                                              \
        bsl::printf("Running test case %d\n", number);                        \
        runTestCase##number();                                                \
        return 0;

#endif















/// Begin the definition of the dispatcher for a component test driver.
#define NTSCFG_TEST_SUITE                                                     \
    namespace BloombergLP {                                                   \
    namespace ntscfg {                                                        \
                                                                              \
    int               testCase;                                               \
    int               testVerbosity;                                          \
    int               testArgc;                                               \
    char**            testArgv;                                               \
    bslma::Allocator* testAllocator;                                          \
    TestInitCallback  testInit;                                               \
    TestExitCallback  testExit;                                               \
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
    int main(int argc, char** argv)                                           \
    {                                                                         \
        BloombergLP::ntscfg::Platform::initialize();                          \
        BloombergLP::ntscfg::Platform::ignore(                                \
            BloombergLP::ntscfg::Signal::e_PIPE);                             \
                                                                              \
        BloombergLP::bslmt::ThreadUtil::setThreadName("main");                \
                                                                              \
        BloombergLP::bsls::Log::setLogMessageHandler(                         \
            &BloombergLP::ntscfg::printLogMessage);                           \
                                                                              \
        BloombergLP::ntscfg::testCase      = 0;                               \
        BloombergLP::ntscfg::testVerbosity = 0;                               \
        BloombergLP::ntscfg::testArgc      = argc;                            \
        BloombergLP::ntscfg::testArgv      = argv;                            \
        BloombergLP::ntscfg::testAllocator = 0;                               \
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
                BloombergLP::ntscfg::testCase = bsl::atoi(argv[1]);           \
            }                                                                 \
                                                                              \
            if (argc == 2) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 0;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_FATAL);                 \
            }                                                                 \
                                                                              \
            if (argc >= 3) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 2;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_WARN);                  \
            }                                                                 \
                                                                              \
            if (argc >= 4) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 3;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_INFO);                  \
            }                                                                 \
                                                                              \
            if (argc >= 5) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 4;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_DEBUG);                 \
            }                                                                 \
                                                                              \
            if (argc == 6) {                                                  \
                BloombergLP::ntscfg::testVerbosity = 5;                       \
                BloombergLP::bsls::Log::setSeverityThreshold(                 \
                    BloombergLP::bsls::LogSeverity::e_TRACE);                 \
            }                                                                 \
                                                                              \
            BloombergLP::ntscfg::TestGuard testGuard;                         \
                                                                              \
            BloombergLP::ntscfg::TestCaseContextMap    testCaseMap;           \
            BloombergLP::ntscfg::TestCaseContextVector testCaseVector;        \

/// Register a functional test case inside a dispatcher for a component test
/// driver.
#define NTSCFG_TEST_FUNCTION_INDEX(number, function)                          \
    testCaseMap[number] = function

/// Register a functional test case inside a dispatcher for a component test
/// driver.
#define NTSCFG_TEST_FUNCTION(function)                                        \
    {                                                                         \
        const int testCaseNumber =                                            \
            static_cast<int>(testCaseVector.size() + 1);                      \
                                                                              \
        ntscfg::TestCaseContext& testCaseContext =                            \
            testCaseMap[testCaseNumber];                                      \
                                                                              \
        testCaseContext.setNumber(testCaseNumber);                            \
        testCaseContext.setName(#function);                                   \
        testCaseContext.setFunction(function);                                \
                                                                              \
        testCaseVector.push_back(testCaseMap.find(testCaseNumber));           \
    }

/// End the definition of the dispatcher for a component test driver.
#define NTSCFG_TEST_SUITE_END                                                 \
    if (BloombergLP::ntscfg::testCase == 0) {                                 \
        BloombergLP::ntscfg::testCase = 1;                                    \
        while (true) {                                                        \
            BloombergLP::ntscfg::TestCaseContext& testCaseContext =           \
                testCaseMap[BloombergLP::ntscfg::testCase];                   \
                                                                              \
            if (testCaseContext.isDefined()) {                                \
                BloombergLP::ntscfg::TestAllocator ta;                        \
                BloombergLP::ntscfg::testAllocator = &ta;                     \
                testCaseContext.execute();                                    \
                NTSCFG_TEST_EQ(ta.numBlocksInUse(), 0);                       \
                ++BloombergLP::ntscfg::testCase;                              \
            }                                                                 \
            else {                                                            \
                break;                                                        \
            }                                                                 \
        }                                                                     \
    }                                                                         \
    else {                                                                    \
        BloombergLP::ntscfg::TestCaseContext& testCaseContext =               \
            testCaseMap[BloombergLP::ntscfg::testCase];                       \
                                                                              \
        if (testCaseContext.isDefined()) {                                    \
            BloombergLP::ntscfg::TestAllocator ta;                            \
            BloombergLP::ntscfg::testAllocator = &ta;                         \
            testCaseContext.execute();                                        \
            NTSCFG_TEST_EQ(ta.numBlocksInUse(), 0);                           \
        }                                                                     \
        else {                                                                \
            return -1;                                                        \
        }                                                                     \
    }                                                                         \
    }                                                                         \
    catch (const bsl::exception& e) {                                         \
        bsl::printf("Test %d failed: %s\n",                                   \
                    BloombergLP::ntscfg::testCase,                            \
                    e.what());                                                \
        return 1;                                                             \
    }                                                                         \
    catch (...) {                                                             \
        bsl::printf("Test %d failed: %s\n",                                   \
                    BloombergLP::ntscfg::testCase,                            \
                    "Unknown exception");                                     \
        return 1;                                                             \
    }                                                                         \
                                                                              \
    BloombergLP::ntscfg::Platform::exit();                                    \
                                                                              \
    return 0;                                                                 \
    }











/// @internal @brief
/// Log at the fatal severity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LOG_FATAL                                                 \
    if (BloombergLP::bsls::LogSeverity::e_FATAL <=                            \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntslSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_FATAL;                          \
        bsl::stringstream ntslStream;                                         \
        ntslStream

/// @internal @brief
/// Log at the error severity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LOG_ERROR                                                 \
    if (BloombergLP::bsls::LogSeverity::e_ERROR <=                            \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntslSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_ERROR;                          \
        bsl::stringstream ntslStream;                                         \
        ntslStream

/// @internal @brief
/// Log at the warn severity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LOG_WARN                                                  \
    if (BloombergLP::bsls::LogSeverity::e_WARN <=                             \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntslSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_WARN;                           \
        bsl::stringstream ntslStream;                                         \
        ntslStream

/// @internal @brief
/// Log at the info severity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LOG_INFO                                                  \
    if (BloombergLP::bsls::LogSeverity::e_INFO <=                             \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntslSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_INFO;                           \
        bsl::stringstream ntslStream;                                         \
        ntslStream

/// @internal @brief
/// Log at the debug severity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LOG_DEBUG                                                 \
    if (BloombergLP::bsls::LogSeverity::e_DEBUG <=                            \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntslSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_DEBUG;                          \
        bsl::stringstream ntslStream;                                         \
        ntslStream

/// @internal @brief
/// Log at the trace severity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LOG_TRACE                                                 \
    if (BloombergLP::bsls::LogSeverity::e_TRACE <=                            \
        BloombergLP::bsls::Log::severityThreshold())                          \
    {                                                                         \
        BloombergLP::bsls::LogSeverity::Enum ntslSeverity =                   \
            BloombergLP::bsls::LogSeverity::e_TRACE;                          \
        bsl::stringstream ntslStream;                                         \
        ntslStream

/// @internal @brief
/// End logging.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_LOG_END                                                   \
    bsl::flush;                                                               \
    BloombergLP::bsls::Log::logFormattedMessage(ntslSeverity,                 \
                                                __FILE__,                     \
                                                __LINE__,                     \
                                                "%s",                         \
                                                ntslStream.str().c_str());    \
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
