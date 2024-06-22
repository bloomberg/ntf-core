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

#include <ntcm_periodiccollector.h>

#include <ntccfg_test.h>

#include <ntci_monitorable.h>
#include <ntcm_monitorableregistry.h>

#include <bdlb_random.h>
#include <bdld_manageddatum.h>
#include <bdlt_currenttime.h>

#include <bslma_testallocator.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>

using namespace BloombergLP;
using namespace ntcm;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test driver excercises the functionality to periodically collect and
// publish statistics measured by monitorable objects, scheduled in a separate
// thread. The test driver simply verifies that the expected number of
// publications occur, given a known set and type of monitorable object. The
// test driver also demonstrates a sample implementation of the
// 'ntci::MonitorablePublisher' protocol that writes a description of each
// statistic and the object that measured it to standard ouput, in tabular
// form.
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char* s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100)
            ++testStatus;
    }
}

#define ASSERT(X)                                                             \
    {                                                                         \
        aSsErT(!(X), #X, __LINE__);                                           \
    }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I, X)                                                     \
    {                                                                         \
        if (!(X)) {                                                           \
            bsl::cout << #I << ": " << I << "\n";                             \
            aSsErT(1, #X, __LINE__);                                          \
        }                                                                     \
    }

#define LOOP2_ASSERT(I, J, X)                                                 \
    {                                                                         \
        if (!(X)) {                                                           \
            bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";  \
            aSsErT(1, #X, __LINE__);                                          \
        }                                                                     \
    }

#define LOOP3_ASSERT(I, J, K, X)                                              \
    {                                                                         \
        if (!(X)) {                                                           \
            bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"   \
                      << #K << ": " << K << "\n";                             \
            aSsErT(1, #X, __LINE__);                                          \
        }                                                                     \
    }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
#define L_ __LINE__
#define NL "\n"
#define T_() bsl::cout << "    " << bsl::flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose             = 0;
static int veryVerbose         = 0;
static int veryVeryVerbose     = 0;
static int veryVeryVeryVerbose = 0;

//=============================================================================
//                        HELPER FUNCTIONS AND CLASSES
//-----------------------------------------------------------------------------

namespace test {

/// This struct describes a statistic measured by an object in this
/// test driver.
class ObjectStatistic
{
    bsls::SpinLock d_lock;
    bsl::int64_t   d_count;
    bsl::int64_t   d_total;
    bsl::int64_t   d_min;
    bsl::int64_t   d_max;

  public:
    /// Create a new statistic having the default value.
    ObjectStatistic();

    /// Update the static by the specified 'amount'.
    void update(bsl::int64_t amount);

    /// Load into the specified 'count', 'total', 'min' and 'max' the
    /// respective components of the statistic. If the specified 'reset'
    /// flag is true, reset the accumulated values.
    void load(bsl::int64_t* count,
              bsl::int64_t* total,
              bsl::int64_t* min,
              bsl::int64_t* max,
              bool          reset);
};

/// This class implements the 'ntci::Monitorable' interface for use by this
/// test driver.
class Object : public ntci::Monitorable
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex        d_mutex;
    bsls::TimeInterval   d_currentTime;
    int                  d_seed;
    ObjectStatistic      d_statistic;

    static struct StatisticMetadata {
        const char*                      prefix;
        const char*                      name;
        const char*                      description;
        ntci::Monitorable::StatisticType type;
    } STATISTICS[];

  private:
    Object(const Object&) BSLS_KEYWORD_DELETED;
    Object& operator=(const Object&) BSLS_KEYWORD_DELETED;

  public:
    enum StatisticOrdinal {
        // Define the semantics of the statistics measured by this object
        // reported at each ordinal.

        STATISTIC_COUNT = 0,
        STATISTIC_TOTAL = 1,
        STATISTIC_MIN   = 2,
        STATISTIC_MAX   = 3,
        NUM_STATISTICS  = 4
    };

    /// Create a new object.
    Object();

    /// Destroy this object.
    ~Object();

    /// Set the current time observed by this object to the specified
    /// 'currentTime'.
    void setCurrentTime(const bsls::TimeInterval& currentTime);

    /// Execute the responsibility of this object and update its statistics.
    void execute();

    /// Load into the specified 'result' the array of statistics from the
    /// specified 'snapshot' for this object based on the specified
    /// 'operation': if 'operation' is e_CUMULATIVE then the statistics are
    /// for the entire life of this object;  otherwise the statistics are
    /// for the period since the last call to this function. If 'operation'
    /// is e_INTERVAL_WITH_RESET then reset all internal measurements.  Note
    /// that 'result->theArray().length()' is expected to have the same
    /// value each time this function returns.
    virtual void getStats(bdld::ManagedDatum* result);

    /// Return the prefix corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    virtual const char* getFieldPrefix(int ordinal) const;

    /// Return the field name corresponding to the field at the specified
    /// 'ordinal' position, or 0 if no field at the 'ordinal' position
    /// exists.
    virtual const char* getFieldName(int ordinal) const;

    /// Return the field description corresponding to the field at the
    /// specified 'ordinal' position, or 0 if no field at the 'ordinal'
    /// position exists.
    virtual const char* getFieldDescription(int ordinal) const;

    /// Return the type of the statistic at the specified 'ordinal'
    /// position, or e_AVERAGE if no field at the 'ordinal' position exists
    /// or the type is unknown.
    virtual StatisticType getFieldType(int ordinal) const;

    /// Return the flags that indicate which indexes to apply to the
    /// statistics measured by this monitorable object.
    virtual int getFieldTags(int ordinal) const;

    /// Return the ordinal of the specified 'fieldName', or a negative value
    /// if no field identified by 'fieldName' exists.
    virtual int getFieldOrdinal(const char* fieldName) const;

    /// Return the maximum number of elements in a datum resulting from
    /// a call to 'getStats()'.
    virtual int numOrdinals() const;

    /// Return the human-readable name of the monitorable object, or 0 or
    /// the empty string if no such human-readable name has been assigned to
    /// the monitorable object.
    virtual const char* objectName() const;
};

/// Provide a protocol to publish statistcs measured
/// by monitorable objects. Implementations of this protocol must be
/// thread safe.
class Publisher : public ntci::MonitorablePublisher
{
    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex    d_mutex;
    bslmt::Semaphore d_semaphore;
    int              d_numPublications;

  private:
    Publisher(const Publisher&) BSLS_KEYWORD_DELETED;
    Publisher& operator=(const Publisher&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new publisher.
    Publisher();

    /// Destroy this object.
    virtual ~Publisher();

    /// Publish the specified 'statistics' collected from the specified
    /// 'monitorable' object at the specified 'time'. If the specified
    /// 'final' flag is true, these 'statistics' are the final statistics
    /// collected during the same sample at the 'time'.
    virtual void publish(const bsl::shared_ptr<ntci::Monitorable>& monitorable,
                         const bdld::Datum&                        statistics,
                         const bsls::TimeInterval&                 time,
                         bool                                      final);

    /// Wait until the final statistics have been published.
    void waitUntilPublicationCompletes();

    /// Return the number of times 'publish' has been called.
    int numPublications() const;
};

ObjectStatistic::ObjectStatistic()
: d_lock(bsls::SpinLock::s_unlocked)
, d_count(0)
, d_total(0)
, d_min(bsl::numeric_limits<bsl::int64_t>::max())
, d_max(bsl::numeric_limits<bsl::int64_t>::min())
{
}

void ObjectStatistic::update(bsl::int64_t amount)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_count += 1;
    d_total += amount;

    if (d_min > amount) {
        d_min = amount;
    }

    if (d_max < amount) {
        d_max = amount;
    }
}

void ObjectStatistic::load(bsl::int64_t* count,
                           bsl::int64_t* total,
                           bsl::int64_t* min,
                           bsl::int64_t* max,
                           bool          reset)
{
    bsls::SpinLockGuard guard(&d_lock);

    *count = d_count;
    *total = d_total;
    *min   = d_min;
    *max   = d_max;

    if (reset) {
        d_count = 0;
        d_total = 0;
        d_min   = bsl::numeric_limits<bsl::int64_t>::max();
        d_max   = bsl::numeric_limits<bsl::int64_t>::min();
    }
}

Object::StatisticMetadata Object::STATISTICS[] = {
    {"test.object",
     "execute.calls",         "Number of calls to execute",
     ntci::Monitorable::e_SUM    },
    {"test.object",
     "execute.totalTime",               "Total execution time",
     ntci::Monitorable::e_AVERAGE},
    {"test.object",
     "execute.minTime", "Minimum execution time of any call",
     ntci::Monitorable::e_MINIMUM},
    {"test.object",
     "execute.maxTime", "Maximum execution time of any call",
     ntci::Monitorable::e_MAXIMUM}
};

Object::Object()
: d_mutex()
, d_currentTime()
, d_seed(::time(0))
{
}

Object::~Object()
{
}

void Object::setCurrentTime(const bsls::TimeInterval& currentTime)
{
    d_currentTime = currentTime;
}

void Object::execute()
{
    const bool measure = true;

    bsl::int64_t t0 = 0;
    if (measure) {
        t0 = bsls::TimeUtil::getTimer();
    }

    {
        int cost = static_cast<int>(
            (static_cast<double>(bdlb::Random::generate15(&d_seed)) / 32767) *
            100);

        bsls::TimeInterval interval;
        interval.setTotalMilliseconds(cost);
        bslmt::ThreadUtil::sleep(interval);
    }

    bsl::int64_t t1 = 0;
    if (measure) {
        t1 = bsls::TimeUtil::getTimer();
    }

    bsl::int64_t t = t1 - t0;
    if (t > 0) {
        d_statistic.update(t);
    }
}

void Object::getStats(bdld::ManagedDatum* result)
{
    LockGuard guard(&d_mutex);

    bsl::int64_t count, total, min, max;

    d_statistic.load(&count, &total, &min, &max, true);

    bdld::DatumMutableArrayRef array;
    bdld::Datum::createUninitializedArray(&array, 4, result->allocator());

    array.data()[0] = bdld::Datum::createInteger64(count, result->allocator());
    array.data()[1] = bdld::Datum::createInteger64(total, result->allocator());
    array.data()[2] = bdld::Datum::createInteger64(min, result->allocator());
    array.data()[3] = bdld::Datum::createInteger64(max, result->allocator());

    *array.length() = 4;

    result->adopt(bdld::Datum::adoptArray(array));
}

const char* Object::getFieldPrefix(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return Object::STATISTICS[ordinal].prefix;
    }
    else {
        return 0;
    }
}

const char* Object::getFieldName(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return Object::STATISTICS[ordinal].name;
    }
    else {
        return 0;
    }
}

const char* Object::getFieldDescription(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return Object::STATISTICS[ordinal].description;
    }
    else {
        return 0;
    }
}

ntci::Monitorable::StatisticType Object::getFieldType(int ordinal) const
{
    if (ordinal < numOrdinals()) {
        return Object::STATISTICS[ordinal].type;
    }
    else {
        return ntci::Monitorable::e_AVERAGE;
    }
}

int Object::getFieldTags(int ordinal) const
{
    return ntci::Monitorable::e_ANONYMOUS;
}

int Object::getFieldOrdinal(const char* fieldName) const
{
    int result = 0;

    for (int ordinal = 0; ordinal < numOrdinals(); ++ordinal) {
        if (bsl::strcmp(Object::STATISTICS[ordinal].name, fieldName) == 0) {
            result = ordinal;
        }
    }

    return result;
}

int Object::numOrdinals() const
{
    return sizeof Object::STATISTICS / sizeof Object::STATISTICS[0];
}

const char* Object::objectName() const
{
    return 0;
}

Publisher::Publisher()
: d_mutex()
, d_semaphore()
, d_numPublications(0)
{
}

Publisher::~Publisher()
{
}

void Publisher::publish(const bsl::shared_ptr<ntci::Monitorable>& monitorable,
                        const bdld::Datum&                        statistics,
                        const bsls::TimeInterval&                 time,
                        bool                                      final)
{
    LockGuard guard(&d_mutex);

    ASSERT(statistics.isArray());

    for (int ordinal = 0; ordinal < statistics.theArray().length(); ++ordinal)
    {
        ASSERT(statistics.theArray().data()[ordinal].isInteger64());

        ASSERT(monitorable->getFieldPrefix(ordinal));
        ASSERT(monitorable->getFieldName(ordinal));
        ASSERT(monitorable->getFieldDescription(ordinal));

        if (verbose) {
            bsl::string guidLabel;
            {
                bsl::ostringstream ss;
                ss << monitorable->guid();
                guidLabel = ss.str();
            }

            bsl::string objectIdLabel;
            {
                bsl::ostringstream ss;
                ss << monitorable->objectId();
                objectIdLabel = ss.str();
            }

            bsl::cout << bsl::setw(46) << bsl::left << guidLabel << "  "
                      << bsl::setw(12) << bsl::left << objectIdLabel << "  "
                      << bsl::setw(15) << bsl::left
                      << monitorable->getFieldPrefix(ordinal) << "  "
                      << bsl::setw(30) << bsl::left
                      << monitorable->getFieldName(ordinal) << "  "
                      << bsl::setw(22) << bsl::right
                      << statistics.theArray().data()[ordinal].theInteger64()
                      << "  " << bsl::left
                      << monitorable->getFieldDescription(ordinal)
                      << bsl::endl;
        }
    }

    ++d_numPublications;

    if (final) {
        d_semaphore.post();
    }
}

void Publisher::waitUntilPublicationCompletes()
{
    d_semaphore.wait();
}

int Publisher::numPublications() const
{
    return d_numPublications;
}

}  // close namespace 'test'

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose             = (argc > 2);
    veryVerbose         = (argc > 3);
    veryVeryVerbose     = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);
    int verbosity =
        1 + verbose + veryVerbose + veryVeryVerbose + veryVeryVeryVerbose;
    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;
    ;

    switch (test) {
    case 0:  // Zero is always the leading case.
    case 1: {
        // TESTING COLLECTION AND PUBLICATION
        //
        // Concerns:
        //   The statistics measured by registered monitorable objects are
        //   published through publishers registered with a collector.
        //
        // Plan:
        //   First, create the monitorable object registry and set it as the
        //   default monitorable registry. This registery tracks the currently
        //   alive monitorable objects in the process. Second, create a rate
        //   limited measurement control and use it as the default control for
        //   subsequently registered monitorable objects. Without a control
        //   monitorable objects are, contractually speaking, not permitted
        //   to perform an measurements. Third, create a number of monitorable
        //   objects and register them with the default monitorable registry.
        //   Fourth, create a monitorable object statistics collector and a
        //   test publisher and register that publisher with the collector.
        //   Setup for the test is now complete. Now, instruct each test
        //   object to "perform work". During the performance of this work,
        //   the test objects measure statistics about the duration of the
        //   work they perform. Next, collect and publish statistics from these
        //   objects. Ensure that the expected number of publications occur.
        //   Finally, deregister each test object from the default monitorable
        //   object registery and deregister the registry itself.

        ntccfg::TestAllocator ta;
        {
            const int NUM_OBJECTS    = 3;
            const int NUM_ITERATIONS = 3;

            // Create the monitorable object registry and set it as the
            // default monitorable registry.

            bsl::shared_ptr<ntcm::MonitorableRegistry> monitorableRegistry;
            monitorableRegistry.createInplace(&ta, &ta);

            // Create a number of monitorable objects and register them with
            // the default monitorable registry.

            typedef bsl::vector<bsl::shared_ptr<test::Object> > ObjectVector;
            ObjectVector                                        objects;

            for (int i = 0; i < NUM_OBJECTS; ++i) {
                bsl::shared_ptr<test::Object> object;
                object.createInplace(&ta);

                monitorableRegistry->registerMonitorable(object);

                objects.push_back(object);
            }

            // Create a monitorable object statistics collector.

            ntca::MonitorableCollectorConfig collectorConfig;
            collectorConfig.setThreadName("metrics");
            collectorConfig.setPeriod(3);

            ntcm::PeriodicCollector::LoadCallback loadCallback =
                bdlf::MemFnUtil::memFn(
                    &ntcm::MonitorableRegistry::loadRegisteredObjects,
                    monitorableRegistry);

            ntcm::PeriodicCollector collector(collectorConfig,
                                              loadCallback,
                                              &ta);

            // Create and register a test publisher with the collector.

            bsl::shared_ptr<test::Publisher> publisher;
            publisher.createInplace(&ta);

            collector.registerPublisher(publisher);

            // Perform work.

            for (ObjectVector::const_iterator it = objects.begin();
                 it != objects.end();
                 ++it)
            {
                const bsl::shared_ptr<test::Object> object = *it;
                for (int i = 0; i < NUM_ITERATIONS; ++i) {
                    object->execute();
                }
            }

            // Start the periodic collector and wait until metrics have
            // been collected and published.

            collector.start();
            publisher->waitUntilPublicationCompletes();
            collector.stop();

            ASSERT(publisher->numPublications() == NUM_OBJECTS);

            // Deregister all the monitorable objects.

            for (ObjectVector::const_iterator it = objects.begin();
                 it != objects.end();
                 ++it)
            {
                const bsl::shared_ptr<test::Object> object = *it;
                monitorableRegistry->deregisterMonitorable(object);
            }
        }
        ASSERT(0 == ta.numBlocksInUse());
    } break;
    default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
    }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}
