#ifndef INCLUDED_NTSCFG_TEST
#define INCLUDED_NTSCFG_TEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <bdlb_bigendian.h>
#include <bdlb_chartype.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_random.h>
#include <bdlb_string.h>
#include <bdlb_stringrefutil.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlbb_pooledblobbufferfactory.h>
#include <bdlbb_simpleblobbufferfactory.h>
#include <bdld_manageddatum.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlma_bufferedsequentialallocator.h>
#include <bdlma_concurrentmultipoolallocator.h>
#include <bdlma_countingallocator.h>
#include <bdlmt_eventscheduler.h>
#include <bdlmt_fixedthreadpool.h>
#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>
#include <bdls_processutil.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bdlt_currenttime.h>
#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_epochutil.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocator.h>
#include <bslmf_assert.h>
#include <bslmt_barrier.h>
#include <bslmt_condition.h>
#include <bslmt_latch.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bslmt_turnstile.h>
#include <bsls_alignedbuffer.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>
#include <bsls_types.h>
#include <bsl_algorithm.h>
#include <bsl_array.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_ostream.h>
#include <bsl_queue.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_stack.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_utility.h>
#include <bsl_vector.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_recordstringformatter.h>
#include <ball_severity.h>
#include <ball_streamobserver.h>

#ifndef NTS_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR
#define NTS_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR 1
#endif

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
    void execute() const;

    /// Return the case number.
    int number() const;

    /// Return the case name.
    const bsl::string& name() const;

    /// Return true if the test case is defined, otherwise return false.
    bool isDefined() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    BSLALG_DECLARE_NESTED_TRAITS(TestCaseContext,
                                 bslalg::TypeTraitUsesBslmaAllocator);
};

inline TestCaseContext::TestCaseContext(bslma::Allocator* basicAllocator)
: d_number(0)
, d_name(basicAllocator ? basicAllocator : bslma::Default::globalAllocator())
, d_function(
      bsl::allocator_arg,
      basicAllocator ? basicAllocator : bslma::Default::globalAllocator())
{
}

inline TestCaseContext::TestCaseContext(const TestCaseContext& original,
                                        bslma::Allocator*      basicAllocator)
: d_number(original.d_number)
, d_name(original.d_name,
         basicAllocator ? basicAllocator : bslma::Default::globalAllocator())
, d_function(
      bsl::allocator_arg,
      basicAllocator ? basicAllocator : bslma::Default::globalAllocator(),
      original.d_function)
{
}

inline TestCaseContext::~TestCaseContext()
{
}

inline void TestCaseContext::setNumber(int value)
{
    d_number = value;
}

inline void TestCaseContext::setName(const bsl::string& value)
{
    d_name = value;
}

inline void TestCaseContext::setFunction(const bsl::function<void()>& value)
{
    d_function = value;
}

inline void TestCaseContext::execute() const
{
    BSLS_LOG_INFO("Testing: %s", d_name.c_str());
    d_function();
}

inline int TestCaseContext::number() const
{
    return d_number;
}

inline const bsl::string& TestCaseContext::name() const
{
    return d_name;
}

inline bool TestCaseContext::isDefined() const
{
    if (d_function) {
        return true;
    }
    else {
        return false;
    }
}

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

inline TestAllocator::TestAllocator()
: d_base(64)
{
}

inline TestAllocator::~TestAllocator()
{
}

inline void* TestAllocator::allocate(size_type size)
{
    return d_base.allocate(size);
}

inline void TestAllocator::deallocate(void* address)
{
    d_base.deallocate(address);
}

inline bsl::int64_t TestAllocator::numBlocksInUse() const
{
    // Intentionally report 0 blocks in use and assert in the destructor.
    return 0;
}

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

inline TestAllocator::TestAllocator()
: d_base()
{
}

inline TestAllocator::~TestAllocator()
{
}

inline void* TestAllocator::allocate(size_type size)
{
    return d_base.allocate(size);
}

inline void TestAllocator::deallocate(void* address)
{
    d_base.deallocate(address);
}

inline bsl::int64_t TestAllocator::numBlocksInUse() const
{
    return d_base.numBlocksInUse();
}

#endif

/// @internal @brief
/// Provide a logger for a test driver.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntscfg
class TestLog
{
    bslma::Allocator*                d_allocator_p;
    ball::LoggerManager*             d_manager_p;
    ball::LoggerManagerConfiguration d_config;
    ball::Severity::Level            d_severityLevel;

  private:
    TestLog(const TestLog&) BSLS_KEYWORD_DELETED;
    TestLog& operator=(const TestLog&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new logger with the specified 'verbosity'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// global allocator is used.
    explicit TestLog(int verbosity, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~TestLog();

    /// Log the specified 'message' from the specified 'file' at the specified
    /// 'line' under the specified 'severity' level.
    static void printLogMessage(bsls::LogSeverity::Enum severity,
                                const char*             file,
                                int                     line,
                                const char*             message);
};

inline TestLog::TestLog(int verbosity, bslma::Allocator* basicAllocator)
: d_allocator_p(basicAllocator ? basicAllocator
                               : bslma::Default::globalAllocator())
, d_manager_p(0)
, d_config(d_allocator_p)
, d_severityLevel(ball::Severity::e_OFF)
{
    int rc;

    switch (verbosity) {
    case 0:
        d_severityLevel = BloombergLP::ball::Severity::e_FATAL;
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_FATAL);
        break;
    case 1:
        d_severityLevel = BloombergLP::ball::Severity::e_ERROR;
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_ERROR);
        break;
    case 2:
        d_severityLevel = BloombergLP::ball::Severity::e_WARN;
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_WARN);
        break;
    case 3:
        d_severityLevel = BloombergLP::ball::Severity::e_INFO;
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_INFO);
        break;
    case 4:
        d_severityLevel = BloombergLP::ball::Severity::e_DEBUG;
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_DEBUG);
        break;
    default:
        d_severityLevel = BloombergLP::ball::Severity::e_TRACE;
        bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_TRACE);
        break;
    }

    rc = d_config.setDefaultThresholdLevelsIfValid(d_severityLevel);
    BSLS_ASSERT_OPT(rc == 0);

    d_manager_p = &ball::LoggerManager::initSingleton(d_config, d_allocator_p);

    bsl::shared_ptr<ball::StreamObserver> observer;
    observer.createInplace(d_allocator_p, &bsl::cout, d_allocator_p);

    // clang-format off
    observer->setRecordFormatFunctor(
        ball::RecordStringFormatter(
            //"----------------------------------------"
            "\n"
            "[ %O ][ %s ]"
            "[ %T ][ %F:%l ][ %c ]"
            ":\n%m %u\n"));
    // clang-format on

    rc = d_manager_p->registerObserver(observer, "default");
    BSLS_ASSERT_OPT(rc == 0);
}

inline TestLog::~TestLog()
{
    // Do not explicitly shut down the logger singleton because it must
    // remain alive during the execution of static destructors.
    //
    // ball::LoggerManager::shutDownSingleton();
}

void TestLog::printLogMessage(bsls::LogSeverity::Enum severity,
                              const char*             file,
                              int                     line,
                              const char*             message)
{
    bdlt::Datetime now = bdlt::CurrentTime::utc();

    char nowBuffer[256];
    now.printToBuffer(nowBuffer, sizeof nowBuffer, 3);

    bsl::uint64_t thread = bslmt::ThreadUtil::selfIdAsUint64();

    bsl::string threadName;
    bslmt::ThreadUtil::getThreadName(&threadName);

    bsl::string fileString = file;

    bsl::string::size_type n = fileString.find_last_of("/\\");
    if (n != bsl::string::npos) {
        fileString = fileString.substr(n + 1);
    }

    bsl::stringstream fileStream;
    fileStream << fileString;
    fileStream << ':' << line;
    fileStream.flush();

    char severityCode;
    switch (severity) {
    case bsls::LogSeverity::e_FATAL:
        severityCode = 'F';
        break;
    case bsls::LogSeverity::e_ERROR:
        severityCode = 'E';
        break;
    case bsls::LogSeverity::e_WARN:
        severityCode = 'W';
        break;
    case bsls::LogSeverity::e_INFO:
        severityCode = 'I';
        break;
    case bsls::LogSeverity::e_DEBUG:
        severityCode = 'D';
        break;
    case bsls::LogSeverity::e_TRACE:
        severityCode = 'T';
        break;
    default:
        severityCode = 'X';
    }

    if (threadName.empty()) {
        bsl::fprintf(stdout,
                     "[ %c ][ %s ][ %012llu ][ %40s ]: %s\n",
                     severityCode,
                     nowBuffer,
                     static_cast<unsigned long long>(thread),
                     fileStream.str().c_str(),
                     message);
    }
    else {
        bsl::fprintf(stdout,
                     "[ %c ][ %s ][ %16s ][ %40s ]: %s\n",
                     severityCode,
                     nowBuffer,
                     threadName.c_str(),
                     fileStream.str().c_str(),
                     message);
    }

    bsl::fflush(stdout);
}

/// Provide utilities for reading and writing to contiguous locations in
/// memory.
///
/// @par Thread Safety
/// This class is thread safe.
class TestMemoryUtil
{
  public:
    /// Generate a byte at the specified 'index' in the specified repeating
    /// 'pattern' of the specified 'size' starting at the specified
    /// 'position'. The behavior is undefined unless 'size > 0'. Note that
    /// if 'position > size' then 'position = position % size'. Also note
    /// that if 'index > size' then 'index = (position + index) % size'.
    static char cycle(bsl::size_t index,
                      const void* pattern,
                      bsl::size_t size,
                      bsl::size_t position);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' the specified repeating
    /// 'pattern' of the specified 'size' starting at the specified
    /// 'position', truncating that pattern appropriately if 'capacity' is
    /// not evenly divisible by 'size'. The behavior is undefined unless
    /// 'capacity > 0' and 'size > 0'.  Note that if 'position > size' then
    /// 'position = position % size'.
    static void write(void*       address,
                      bsl::size_t capacity,
                      const void* pattern,
                      bsl::size_t size,
                      bsl::size_t position);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0xDEADBEEF.
    static void writeDeadBeef(void* address, bsl::size_t capacity);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0xFACEFEED.
    static void writeFaceFeed(void* address, bsl::size_t capacity);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0xCAFEBABE.
    static void writeCafeBabe(void* address, bsl::size_t capacity);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0x00000000.
    static void write0s(void* address, bsl::size_t capacity);

    /// Write to the region of memory beginning at the specified 'address'
    /// and having the specified 'capacity' a repeating pattern of
    /// 0xFFFFFFFF.
    static void write1s(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating'pattern' of the specified 'size'
    /// starting at the specified 'position', truncated appropriately if
    /// 'capacity' is not evenly divisible by 'size'. The behavior is
    /// undefined unless 'capacity > 0' and 'size > 0'. Note that if
    /// 'position > size' then 'position = position % size'.
    static bool check(const void* address,
                      bsl::size_t capacity,
                      const void* pattern,
                      bsl::size_t size,
                      bsl::size_t position);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0xDEADBEEF.
    static bool checkDeadBeef(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0xFACEFEED.
    static bool checkFaceFeed(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0xCAFEBABE.
    static bool checkCafebabe(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0x00000000.
    static bool check0s(void* address, bsl::size_t capacity);

    /// Verify that the region of memory beginning at the specified
    /// 'address' and having the specified 'capacity' of defined data
    /// follows the specified repeating pattern of 0xFFFFFFFF.
    static bool check1s(void* address, bsl::size_t capacity);

    /// The standard pattern.
    static const char PATTERN[26];
};

const char TestMemoryUtil::PATTERN[26] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

char TestMemoryUtil::cycle(bsl::size_t index,
                           const void* pattern,
                           bsl::size_t size,
                           bsl::size_t position)
{
    BSLS_ASSERT_OPT(pattern);
    BSLS_ASSERT_OPT(size > 0);

    return ((const char*)(pattern))[(position + index) % size];
}

void TestMemoryUtil::write(void*       address,
                           bsl::size_t capacity,
                           const void* pattern,
                           bsl::size_t size,
                           bsl::size_t position)
{
    BSLS_ASSERT_OPT(address);
    BSLS_ASSERT_OPT(pattern);

    BSLS_ASSERT_OPT(capacity > 0);
    BSLS_ASSERT_OPT(size > 0);

    char* dc = (char*)(address);
    char* de = dc + capacity;

    const bsl::size_t so = position % size;
    const char*       sb = (const char*)(pattern);
    const char*       sc = sb + so;
    const char*       se = sb + size;

    while (dc != de) {
        *dc++ = *sc++;

        if (sc == se) {
            sc = sb;
        }
    }
}

void TestMemoryUtil::writeDeadBeef(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t DEADBEEF_ARRAY[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    bsl::uint32_t DEADBEEF;
    bsl::memcpy(&DEADBEEF, DEADBEEF_ARRAY, sizeof DEADBEEF_ARRAY);

    TestMemoryUtil::write(address, capacity, &DEADBEEF, sizeof DEADBEEF, 0);
}

void TestMemoryUtil::writeFaceFeed(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t FACEFEED_ARRAY[4] = {0xFA, 0xCE, 0xFE, 0xED};

    bsl::uint32_t FACEFEED;
    bsl::memcpy(&FACEFEED, FACEFEED_ARRAY, sizeof FACEFEED_ARRAY);

    TestMemoryUtil::write(address, capacity, &FACEFEED, sizeof FACEFEED, 0);
}

void TestMemoryUtil::writeCafeBabe(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t CAFEBABE_ARRAY[4] = {0xCA, 0xFE, 0xBA, 0xBE};

    bsl::uint32_t CAFEBABE;
    bsl::memcpy(&CAFEBABE, CAFEBABE_ARRAY, sizeof CAFEBABE_ARRAY);

    TestMemoryUtil::write(address, capacity, &CAFEBABE, sizeof CAFEBABE, 0);
}

void TestMemoryUtil::write0s(void* address, bsl::size_t capacity)
{
    bsl::memset(address, 0x00, capacity);
}

void TestMemoryUtil::write1s(void* address, bsl::size_t capacity)
{
    bsl::memset(address, 0xFF, capacity);
}

bool TestMemoryUtil::check(const void* address,
                           bsl::size_t capacity,
                           const void* pattern,
                           bsl::size_t size,
                           bsl::size_t position)
{
    BSLS_ASSERT_OPT(address);
    BSLS_ASSERT_OPT(pattern);

    BSLS_ASSERT_OPT(capacity > 0);
    BSLS_ASSERT_OPT(size > 0);

    const char* dc = (const char*)(address);
    const char* de = dc + capacity;

    const bsl::size_t so = position % size;
    const char*       sb = (const char*)(pattern);
    const char*       sc = sb + so;
    const char*       se = sb + size;

    while (dc != de) {
        if (*dc++ != *sc++) {
            return false;
        }

        if (sc == se) {
            sc = sb;
        }
    }

    return true;
}

bool TestMemoryUtil::checkDeadBeef(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t DEADBEEF_ARRAY[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    bsl::uint32_t DEADBEEF;
    bsl::memcpy(&DEADBEEF, DEADBEEF_ARRAY, sizeof DEADBEEF_ARRAY);

    return TestMemoryUtil::check(address,
                                 capacity,
                                 &DEADBEEF,
                                 sizeof DEADBEEF,
                                 0);
}

bool TestMemoryUtil::checkFaceFeed(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t FACEFEED_ARRAY[4] = {0xFA, 0xCE, 0xFE, 0xED};

    bsl::uint32_t FACEFEED;
    bsl::memcpy(&FACEFEED, FACEFEED_ARRAY, sizeof FACEFEED_ARRAY);

    return TestMemoryUtil::check(address,
                                 capacity,
                                 &FACEFEED,
                                 sizeof FACEFEED,
                                 0);
}

bool TestMemoryUtil::checkCafebabe(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t CAFEBABE_ARRAY[4] = {0xCA, 0xFE, 0xBA, 0xBE};

    bsl::uint32_t CAFEBABE;
    bsl::memcpy(&CAFEBABE, CAFEBABE_ARRAY, sizeof CAFEBABE_ARRAY);

    return TestMemoryUtil::check(address,
                                 capacity,
                                 &CAFEBABE,
                                 sizeof CAFEBABE,
                                 0);
}

bool TestMemoryUtil::check0s(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t ALL_ZEROES_ARRAY[4] = {0x00, 0x00, 0x00, 0x00};

    bsl::uint32_t ALL_ZEROES;
    bsl::memcpy(&ALL_ZEROES, ALL_ZEROES_ARRAY, sizeof ALL_ZEROES_ARRAY);

    return TestMemoryUtil::check(address,
                                 capacity,
                                 &ALL_ZEROES,
                                 sizeof ALL_ZEROES,
                                 0);
}

bool TestMemoryUtil::check1s(void* address, bsl::size_t capacity)
{
    const bsl::uint8_t ALL_ONES_ARRAY[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    bsl::uint32_t ALL_ONES;
    bsl::memcpy(&ALL_ONES, ALL_ONES_ARRAY, sizeof ALL_ONES_ARRAY);

    return TestMemoryUtil::check(address,
                                 capacity,
                                 &ALL_ONES,
                                 sizeof ALL_ONES,
                                 0);
}

/// @internal @brief
/// Provide a suite of utilities for generating test data.
///
/// @par Thread Safety
/// This class is thread safe.
class TestDataUtil
{
  public:
    /// Enumerate the constants used by this implementation.
    enum Constant {
        /// The dataset from which the data is generated is associated with
        /// transmissions from a client.
        k_DATASET_CLIENT = 0,

        /// The dataset from which the data is generated is associated with
        /// transmissions from a server.
        k_DATASET_SERVER = 1,

        /// The dataset from which the data is generated is associated with
        /// transmissions from a client, but in the form that is likely to be
        /// well-compressed.
        k_DATASET_CLIENT_COMPRESSABLE = 2,

        /// The dataset from which the data is generated is associated with
        /// transmissions from a server, but in the form that is likely to be
        /// well-compressed.
        k_DATASET_SERVER_COMPRESSABLE = 3
    };

    /// Return the byte at the specified 'position' in the specified
    /// 'dataset'.
    static char generateByte(bsl::size_t position, bsl::size_t dataset);

    /// Load into the specified 'result' the specified 'size' sequence of
    /// bytes from the specified 'dataset' starting at the specified
    /// 'offset'.
    static void generateData(bsl::string* result,
                             bsl::size_t  size,
                             bsl::size_t  offset  = 0,
                             bsl::size_t  dataset = 0);

    /// Load into the specified 'result' the specified 'size' sequence of
    /// bytes from the specified 'dataset' starting at the specified
    /// 'offset'.
    static void generateData(bdlbb::Blob* result,
                             bsl::size_t  size,
                             bsl::size_t  offset  = 0,
                             bsl::size_t  dataset = 0);
};

inline char TestDataUtil::generateByte(bsl::size_t position,
                                       bsl::size_t dataset)
{
    // clang-format off
    struct {
        const char* source;
        bsl::size_t length;
    } DATA[] = {
        {"abcdefghijklmnopqrstuvwxyz", 26},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26},
        {
        "a"
        "bb"
        "ccc"
        "dddd"
        "eeeee"
        "ffffff"
        "ggggggg"
        "hhhhhhhh"
        "iiiiiiiii"
        "jjjjjjjjjj"
        "kkkkkkkkkkk"
        "llllllllllll"
        "mmmmmmmmmmmmm"
        "nnnnnnnnnnnnnn"
        "ooooooooooooooo"
        "pppppppppppppppp"
        "qqqqqqqqqqqqqqqqq"
        "rrrrrrrrrrrrrrrrrr"
        "sssssssssssssssssss"
        "tttttttttttttttttttt"
        "uuuuuuuuuuuuuuuuuuuuu"
        "vvvvvvvvvvvvvvvvvvvvvv"
        "wwwwwwwwwwwwwwwwwwwwwww"
        "xxxxxxxxxxxxxxxxxxxxxxxx"
        "yyyyyyyyyyyyyyyyyyyyyyyyy"
        "zzzzzzzzzzzzzzzzzzzzzzzzzz", 351 },
        {
        "A"
        "BB"
        "CCC"
        "DDDD"
        "EEEEE"
        "FFFFFF"
        "GGGGGGG"
        "HHHHHHHH"
        "IIIIIIIII"
        "JJJJJJJJJJ"
        "KKKKKKKKKKK"
        "LLLLLLLLLLLL"
        "MMMMMMMMMMMMM"
        "NNNNNNNNNNNNNN"
        "OOOOOOOOOOOOOOO"
        "PPPPPPPPPPPPPPPP"
        "QQQQQQQQQQQQQQQQQ"
        "RRRRRRRRRRRRRRRRRR"
        "SSSSSSSSSSSSSSSSSSS"
        "TTTTTTTTTTTTTTTTTTTT"
        "UUUUUUUUUUUUUUUUUUUUU"
        "VVVVVVVVVVVVVVVVVVVVVV"
        "WWWWWWWWWWWWWWWWWWWWWWW"
        "XXXXXXXXXXXXXXXXXXXXXXXX"
        "YYYYYYYYYYYYYYYYYYYYYYYYY"
        "ZZZZZZZZZZZZZZZZZZZZZZZZZZ", 351 },
    };
    // clang-format on

    enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

    dataset = dataset % NUM_DATA;
    return DATA[dataset].source[position % DATA[dataset].length];
}

inline void TestDataUtil::generateData(bsl::string* result,
                                       bsl::size_t  size,
                                       bsl::size_t  offset,
                                       bsl::size_t  dataset)
{
    result->clear();
    result->resize(size);

    for (bsl::size_t i = offset; i < offset + size; ++i) {
        (*result)[i] = generateByte(offset + i, dataset);
    }
}

inline void TestDataUtil::generateData(bdlbb::Blob* result,
                                       bsl::size_t  size,
                                       bsl::size_t  offset,
                                       bsl::size_t  dataset)
{
    result->removeAll();
    result->setLength(static_cast<int>(size));

    bsl::size_t k = 0;

    for (int i = 0; i < result->numDataBuffers(); ++i) {
        const bdlbb::BlobBuffer& buffer = result->buffer(i);

        bsl::size_t numBytesToWrite = i == result->numDataBuffers() - 1
                                          ? result->lastDataBufferLength()
                                          : buffer.size();

        for (bsl::size_t j = 0; j < numBytesToWrite; ++j) {
            buffer.data()[j] = generateByte(offset + k, dataset);
            ++k;
        }
    }
}

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
/// Defines a type alias for a vector of iterators to test case contexts,
/// indexed by test case number.
///
/// @ingroup module_ntscfg
typedef bsl::vector<TestCaseContextMap::iterator> TestCaseContextVector;

/// @internal @brief
/// The test case number.
///
/// @ingroup module_ntscfg
static int testCase = 0;

/// @internal @brief
/// The test verbosity level.
///
/// @ingroup module_ntscfg
static int testVerbosity = 0;

/// @internal @brief
/// The number of arguments specified when the test driver was executed.
///
/// @ingroup module_ntscfg
static int testArgc = 0;

/// @internal @brief
/// The argument vector specified when the test driver was executed.
///
/// @ingroup module_ntscfg
static char** testArgv = 0;

/// @internal @brief
/// The test allocator.
///
/// @ingroup module_ntscfg
static bslma::Allocator* testAllocator =
    &bslma::NewDeleteAllocator::singleton();

/// @internal @brief
/// The map of test case numbers to the associated function to invoke to
/// execute that test case.
///
/// @ingroup module_ntscfg
static ntscfg::TestCaseContextMap testCaseMap(
    &bslma::NewDeleteAllocator::singleton());

/// @internal @brief
/// The vector of iterators to test case contexts, indexed by test case number.
///
/// @ingroup module_ntscfg
static ntscfg::TestCaseContextVector testCaseVector(
    &bslma::NewDeleteAllocator::singleton());

/// @internal @brief
/// Provide utilities for a test driver.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntscfg
class TestUtil
{
  public:
    /// Register the test case with the specified 'name' implemented by the
    /// specified 'function'.
    static void registerTestCase(const char*             name,
                                 const TestCaseFunction& function);

    /// Parse the specified 'text' and load the integer result into the 
    /// specified 'result'. Return true if 'text was parsed successfully, 
    /// otherwise return false.
    static bool parseInt(int* result, const char* text);

    /// List all available test cases.
    static void list();

    /// List the specified 'testCaseContext'.
    static void list(const ntscfg::TestCaseContext& testCaseContext);

    /// List each test described in the specified 'testCaseContextVector'.
    static void list(
        const bsl::vector<const ntscfg::TestCaseContext*>& 
        testCaseContextVector);
    
    /// Run all test cases.
    static void call();

    /// Run the test described by the specified 'testCaseContext'.
    static void call(const ntscfg::TestCaseContext& testCaseContext);

    /// Run each test described in the specified 'testCaseContextVector'.
    static void call(
        const bsl::vector<const ntscfg::TestCaseContext*>& 
        testCaseContextVector);

    /// Return true if the specified 'name' matches the specified 'pattern',
    /// otherwise return false.
    static bool match(const bsl::string& name, const bsl::string& pattern);

    /// Show the command line usage.
    static void help(const char* executableName);
};

void TestUtil::registerTestCase(const char*             name,
                                const TestCaseFunction& function)
{
    const int testCaseNumber =
        static_cast<int>(BloombergLP::ntscfg::testCaseVector.size() + 1);

    BloombergLP::ntscfg::TestCaseContext& testCaseContext =
        BloombergLP::ntscfg::testCaseMap[testCaseNumber];

    testCaseContext.setNumber(testCaseNumber);
    testCaseContext.setName(name);
    testCaseContext.setFunction(function);

    BloombergLP::ntscfg::testCaseVector.push_back(
        BloombergLP::ntscfg::testCaseMap.find(testCaseNumber));
}

bool TestUtil::parseInt(int* result, const char* text)
{
    char* end  = 0;
    int   temp = static_cast<int>(strtol(text, &end, 10));
    if (end == 0 || *end != 0) {
        return false;
    }

    *result = static_cast<int>(temp);
    return true;
}

void TestUtil::list()
{
    bsl::size_t maxDigits = 0;
    {
        bsl::size_t numTests = ntscfg::testCaseVector.size();
        do {
            numTests /= 10;
            ++maxDigits;
        } while (numTests != 0);
    }

    for (ntscfg::TestCaseContextMap::const_iterator it =
             ntscfg::testCaseMap.begin();
         it != ntscfg::testCaseMap.end();
         ++it)
    {
        const ntscfg::TestCaseContext& testCaseContext = it->second;

        bsl::cout << bsl::setw(maxDigits);
        bsl::cout << bsl::right;
        bsl::cout << testCaseContext.number();
        bsl::cout << ") ";
        bsl::cout << testCaseContext.name();
        bsl::cout << bsl::endl;
    }
}

void TestUtil::list(const ntscfg::TestCaseContext& testCaseContext)
{
    bsl::size_t maxDigits = 0;
    {
        bsl::size_t numTests = ntscfg::testCaseVector.size();
        do {
            numTests /= 10;
            ++maxDigits;
        } while (numTests != 0);
    }

    {
        bsl::cout << bsl::setw(maxDigits);
        bsl::cout << bsl::right;
        bsl::cout << testCaseContext.number();
        bsl::cout << ") ";
        bsl::cout << testCaseContext.name();
        bsl::cout << bsl::endl;
    }
}

void TestUtil::list(
    const bsl::vector<const ntscfg::TestCaseContext*>& testCaseContextVector)
{
    bsl::size_t maxDigits = 0;
    {
        bsl::size_t numTests = ntscfg::testCaseVector.size();
        do {
            numTests /= 10;
            ++maxDigits;
        } while (numTests != 0);
    }

    for (bsl::size_t i = 0; i < testCaseContextVector.size(); ++i) {
        const ntscfg::TestCaseContext& testCaseContext = 
            *testCaseContextVector[i];

        bsl::cout << bsl::setw(maxDigits);
        bsl::cout << bsl::right;
        bsl::cout << testCaseContext.number();
        bsl::cout << ") ";
        bsl::cout << testCaseContext.name();
        bsl::cout << bsl::endl;
    }
}

void TestUtil::call()
{
    for (ntscfg::TestCaseContextMap::const_iterator it =
             ntscfg::testCaseMap.begin();
         it != ntscfg::testCaseMap.end();
         ++it)
    {
        const ntscfg::TestCaseContext& testCaseContext = it->second;
        ntscfg::TestUtil::call(testCaseContext);
    }
}

void TestUtil::call(const ntscfg::TestCaseContext& testCaseContext)
{
    if (testCaseContext.isDefined()) {
        BloombergLP::ntscfg::testCase = testCaseContext.number();

        BloombergLP::ntscfg::TestAllocator ta;
        BloombergLP::ntscfg::testAllocator = &ta;

        testCaseContext.execute();
        
        if (ta.numBlocksInUse() != 0) {
            bsl::cerr << "Leaked " << ta.numBlocksInUse()
                      << " memory blocks" << bsl::endl;
            bsl::abort();
        }
    }
}

void TestUtil::call(
    const bsl::vector<const ntscfg::TestCaseContext*>& testCaseContextVector)
{
    for (bsl::size_t i = 0; i < testCaseContextVector.size(); ++i) {
        const BloombergLP::ntscfg::TestCaseContext& testCaseContext =
            *testCaseContextVector[i];

        ntscfg::TestUtil::call(testCaseContext);
    }
}

bool TestUtil::match(const bsl::string& name,
                     const bsl::string& expression)
{
    if (expression.empty()) {
        return true;
    }

    const char *np = name.data();
    const char *ne = np + name.size();

    const char *ep = expression.data();
    const char *ee = ep + expression.size();

    const char *cp = NULL;
    const char *mp = NULL;

    while ((np != ne) && (*ep != '*')) {
        if ((*ep != *np) && (*ep != '?')) {
            return false;
        }
        ep++;
        np++;
    }

    while (np != ne) {
        if (*ep == '*') {
            if (++ep == ee) {
                return true;
            }
            mp = ep;
            cp = np + 1;
        }
        else if ((*ep == *np) || (*ep == '?')) {
            ep++;
            np++;
        }
        else {
            ep = mp;
            np = cp++;
        }
    }

    while (*ep == '*') {
        ep++;
    }

    if (ep == ee) {
        return true;
    }
    else {
        return false;
    }
}

void TestUtil::help(const char* executableName)
{
    bsl::printf("usage: %s "
                "[-v <verbosity>] "
                "[--list] "
                "[<case-number>|<case-name>|<case-name-pattern>]\n",
                executableName);
}

/// @internal @brief
/// Provide an automatic test function registrar.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntscfg
class TestRegistration
{
  public:
    /// Create a new automatic test registrar for the test case having the
    /// specified 'name' implemented by the specified 'function'.
    TestRegistration(const char* name, const TestCaseFunction& function);

    /// Destroy this object.
    ~TestRegistration();

  private:
    TestRegistration(const TestRegistration&) BSLS_KEYWORD_DELETED;
    TestRegistration& operator=(const TestRegistration&) BSLS_KEYWORD_DELETED;
};

TestRegistration::TestRegistration(const char*             name,
                                   const TestCaseFunction& function)
{
    BloombergLP::ntscfg::TestUtil::registerTestCase(name, function);
}

TestRegistration::~TestRegistration()
{
}

}  // close namespace ntscfg
}  // close namespace BloombergLP

int main(int argc, char** argv)
{
    BloombergLP::ntscfg::Platform::initialize();
    BloombergLP::ntscfg::Platform::ignore(BloombergLP::ntscfg::Signal::e_PIPE);

    BloombergLP::bslmt::ThreadUtil::setThreadName("main");

    BloombergLP::bsls::Log::setLogMessageHandler(
        &BloombergLP::ntscfg::TestLog::printLogMessage);

    BloombergLP::ntscfg::testCase      = 0;
    BloombergLP::ntscfg::testVerbosity = 0;
    BloombergLP::ntscfg::testArgc      = argc;
    BloombergLP::ntscfg::testArgv      = argv;

    try {
        BloombergLP::bdlb::NullableValue<bsl::string> concern;

        bool list = false;

        int i = 1;
        while (i < argc) {
            const char* arg = argv[i];

            if ((0 == bsl::strcmp(arg, "-?")) ||
                (0 == bsl::strcmp(arg, "--help")))
            {
                BloombergLP::ntscfg::TestUtil::help(argv[0]);
                return 0;
            }

            if ((0 == bsl::strcmp(arg, "-l")) ||
                (0 == bsl::strcmp(arg, "--list")))
            {
                list = true;
                ++i;
                continue;
            }

            if ((0 == bsl::strcmp(arg, "-v")) ||
                (0 == bsl::strcmp(arg, "--verbosity")))
            {
                ++i;
                if (i >= argc) {
                    BloombergLP::ntscfg::TestUtil::help(argv[0]);
                    return 1;
                }
                arg = argv[i];

                int level;
                if (BloombergLP::ntscfg::TestUtil::parseInt(&level, arg)) {
                    BloombergLP::ntscfg::testVerbosity = level;
                }
                else {
                    bsl::cerr << "The log verbosity " << level << " is invalid"
                              << bsl::endl;
                    return 1;
                }

                ++i;
                continue;
            }

            if (0 == bsl::strcmp(arg, "--concern")) {
                ++i;
                if (i >= argc) {
                    BloombergLP::ntscfg::TestUtil::help(argv[0]);
                    return 1;
                }
                arg = argv[i];

                concern.makeValue(bsl::string(arg));

                ++i;
                continue;
            }

            if (concern.isNull()) {
                concern.makeValue(bsl::string(arg));
            }
            else {
                BloombergLP::ntscfg::testVerbosity++;
            }

            ++i;
        }

        BloombergLP::ntscfg::TestLog testLog(
            BloombergLP::ntscfg::testVerbosity);

        if (concern.has_value() && concern.value() == "0") {
            concern.reset();
        }

        if (concern.has_value()) {
            int number = 0;
            if (BloombergLP::ntscfg::TestUtil::parseInt(
                    &number,
                    concern.value().c_str()))
            {
                if (BloombergLP::ntscfg::testCaseMap.contains(number)) {
                    BloombergLP::ntscfg::testCase = number;

                    const BloombergLP::ntscfg::TestCaseContext& 
                    testCaseContext = 
                        BloombergLP::ntscfg::testCaseMap
                            [BloombergLP::ntscfg::testCase];

                    if (list) {
                        BloombergLP::ntscfg::TestUtil::list(
                            testCaseContext);
                    }
                    else {
                        BloombergLP::ntscfg::TestUtil::call(
                            testCaseContext);
                    }
                }
                else {
                    if (BloombergLP::ntscfg::testVerbosity >= 3) {
                        bsl::cerr << "The test case number " << number
                                  << " is not found" << bsl::endl;
                    }
                    return -1;
                }
            }
            else {
                bsl::vector<const BloombergLP::ntscfg::TestCaseContext*> 
                testCaseContextVector;
                
                bool pattern = false;
                if (concern.value().find('*') != bsl::string::npos) {
                    testCaseContextVector.reserve(
                        BloombergLP::ntscfg::testCaseMap.size());
                    pattern = true;
                }

                for (BloombergLP::ntscfg::TestCaseContextMap::const_iterator
                         it  = BloombergLP::ntscfg::testCaseMap.begin();
                         it != BloombergLP::ntscfg::testCaseMap.end();
                       ++it)
                {
                    const BloombergLP::ntscfg::TestCaseContext&
                        testCaseContext = it->second;

                    if (pattern) {
                        if (BloombergLP::ntscfg::TestUtil::match(
                            testCaseContext.name(), 
                            concern.value())) 
                        {
                            testCaseContextVector.push_back(&testCaseContext);
                        }
                    }
                    else {
                        if (testCaseContext.name() == concern.value()) {
                            testCaseContextVector.push_back(&testCaseContext);
                            break;
                        }
                    }
                }

                if (testCaseContextVector.empty()) {
                    if (BloombergLP::ntscfg::testVerbosity >= 3) {
                        bsl::cerr << "No test case(s) matching \"" 
                                  << concern.value()
                                  << "\" are found" << bsl::endl;
                    }
                    return -1;
                }

                if (list) {
                    BloombergLP::ntscfg::TestUtil::list(
                        testCaseContextVector);
                }
                else {
                    BloombergLP::ntscfg::TestUtil::call(
                        testCaseContextVector);
                }
            }
        }
        else {
            if (list) {
                BloombergLP::ntscfg::TestUtil::list();
            }
            else {
                BloombergLP::ntscfg::TestUtil::call();
            }
        }
    }
    catch (const bsl::exception& e) {
        bsl::printf("Test %d failed: %s\n",
                    BloombergLP::ntscfg::testCase,
                    e.what());
        return 1;
    }
    catch (...) {
        bsl::printf("Test %d failed: %s\n",
                    BloombergLP::ntscfg::testCase,
                    "Unknown exception");
        return 1;
    }

    return 0;
}

// Concatenate two compile-type identifier into a single identifier. Note that
// this macro is private to the implementation and should not be used.
#define NTSCFG_TEST_FUNCTION_NAME_JOIN(a, b) a##b

// The identifier of a lock scope, formed from the specified 'prefix'
// concatenated with the specified 'disambiguator'. Note that this macro is
// private to the implementation and should not be used.
#define NTSCFG_TEST_FUNCTION_NAME(prefix, disambiguator)                      \
    NTSCFG_TEST_FUNCTION_NAME_JOIN(prefix, disambiguator)

#if defined(BSLS_PLATFORM_CMP_SUN)

// Implement the declaration of a test function for the specified
// 'concernStringLiteral'. Note that this macro is private to the
// implementation and should not be used.
#define NTSCFG_TEST_FUNCTION(function)                                        \
    BloombergLP::ntscfg::TestRegistration NTSCFG_TEST_FUNCTION_NAME(          \
        test_,                                                                \
        __LINE__)(#function, &(function));                                    \
    void function()

#else

// Implement the declaration of a test function for the specified
// 'concernStringLiteral'. Note that this macro is private to the
// implementation and should not be used.
#define NTSCFG_TEST_FUNCTION(function)                                        \
    BloombergLP::ntscfg::TestRegistration NTSCFG_TEST_FUNCTION_NAME(          \
        test_,                                                                \
        __COUNTER__)(#function, &(function));                                 \
    void function()

#endif

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
/// The error verbosity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_VERBOSITY_ERROR 1

/// @internal @brief
/// The warning verbosity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_VERBOSITY_WARN 2

/// @internal @brief
/// The information verbosity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_VERBOSITY_INFO 3

/// @internal @brief
/// The debug verbosity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_VERBOSITY_DEBUG 4

/// @internal @brief
/// The trace verbosity level.
///
/// @ingroup module_ntscfg
#define NTSCFG_TEST_VERBOSITY_TRACE 5

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

#endif
