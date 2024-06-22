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

#include <ntci_monitorable.h>

#include <ntccfg_test.h>

#include <bdlb_random.h>
#include <bdld_manageddatum.h>
#include <bdlt_currenttime.h>

#include <bslma_testallocator.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>

#include <bsl_cstdlib.h>
#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test driver ensure that a coherent implementation of the interfaces
// provided by the component under test are possible. The implementations
// of these interfaces are excercised the test the integrity of the design,
// with respect to measurement throttling, querying, snapshots, and reset
// semantics.
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

/// This class implements the 'ntci::MonitorableRegistry' interface for use
/// by this test driver.
class ObjectRegistry : public ntci::MonitorableRegistry
{
    /// Define a type alias for a map of locally-unique object
    /// identifiers to the shared pointers to registered monitorable objects
    /// so identified.
    typedef bsl::map<int, bsl::shared_ptr<ntci::Monitorable> > ObjectMap;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex d_mutex;
    ObjectMap     d_objects;

  private:
    ObjectRegistry(const ObjectRegistry&) BSLS_KEYWORD_DELETED;
    ObjectRegistry& operator=(const ObjectRegistry&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object registry. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit ObjectRegistry(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    virtual ~ObjectRegistry();

    /// Add the specified 'object' to this monitorable object registry.
    virtual void registerMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& object);

    /// Remove the specified 'object' from this monitorable object
    /// registry.
    virtual void deregisterMonitorable(
        const bsl::shared_ptr<ntci::Monitorable>& object);

    /// Load into the specified 'result' each currently registered
    /// monitorable object.
    virtual void loadRegisteredObjects(
        bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result) const;
};

/// Provide utilities for querying statistics from a
/// 'test::Object' and verifying their basic properties.
struct ObjectUtil {
    /// Load into the specified 'result' array the statistics for the
    /// specified 'object', stored in the specified 'snapshot', according to
    /// the specified 'operation'. Use the specified 'allocator' to supply
    /// memory.
    static void getStats(bsl::int64_t      result[4],
                         test::Object*     object,
                         bslma::Allocator* allocator);
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
, d_seed(static_cast<int>(::time(0)))
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
    NTCCFG_WARNING_UNUSED(ordinal);
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

ObjectRegistry::ObjectRegistry(bslma::Allocator* basicAllocator)
: d_mutex()
, d_objects(basicAllocator)
{
}

ObjectRegistry::~ObjectRegistry()
{
}

void ObjectRegistry::registerMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& object)
{
    LockGuard guard(&d_mutex);

    d_objects[static_cast<int>(object->objectId())] = object;
}

void ObjectRegistry::deregisterMonitorable(
    const bsl::shared_ptr<ntci::Monitorable>& object)
{
    LockGuard guard(&d_mutex);

    d_objects.erase(static_cast<int>(object->objectId()));
}

void ObjectRegistry::loadRegisteredObjects(
    bsl::vector<bsl::shared_ptr<ntci::Monitorable> >* result) const
{
    LockGuard guard(&d_mutex);

    result->reserve(result->size() + d_objects.size());

    for (ObjectMap::const_iterator it = d_objects.begin();
         it != d_objects.end();
         ++it)
    {
        result->push_back(it->second);
    }
}

void ObjectUtil::getStats(bsl::int64_t      result[4],
                          test::Object*     object,
                          bslma::Allocator* allocator)
{
    ASSERT(allocator);

    bdld::ManagedDatum stats(allocator);
    object->getStats(&stats);

    bdld::Datum datum(stats.datum());

    if (verbose) {
        bsl::cout << "--" << bsl::endl
                  << "Object " << object->objectId() << " GUID "
                  << object->guid() << bsl::endl
                  << "--" << bsl::endl;

        if (veryVerbose) {
            bsl::cout << "Datum = " << datum << bsl::endl << "--" << bsl::endl;
        }
    }

    ASSERT(datum.isArray());
    ASSERT(datum.theArray().length() == test::Object::NUM_STATISTICS);

    for (int ordinal = 0;
         ordinal < static_cast<int>(datum.theArray().length());
         ++ordinal)
    {
        ASSERT(datum.theArray().data()[ordinal].isInteger64());

        result[ordinal] = datum.theArray().data()[ordinal].theInteger64();

        if (verbose) {
            bsl::cout << "    " << bsl::setw(15) << bsl::left
                      << object->getFieldPrefix(ordinal) << "  "
                      << bsl::setw(30) << bsl::left
                      << object->getFieldName(ordinal) << "  " << bsl::setw(22)
                      << bsl::right
                      << datum.theArray().data()[ordinal].theInteger64()
                      << "  " << bsl::left
                      << object->getFieldDescription(ordinal) << bsl::endl;
        }
    }
}

}  // close test namespace

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
    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;
    ;

    switch (test) {
    case 0:  // Zero is always the leading case.
    case 1: {
        // TESTING STATISTICS MEASUREMENT, THROTTLING, AND QUERYING
        //
        // Concerns:
        //   Monitorable objects measure statistics governed by a control
        //   mechanism that limits the rate at which the measurements may be
        //   made. The measurements are stored simlulatenously in an
        //   "application" and "internal" snapshot. The measurements may be
        //   queried with various "reset" semantics.
        //
        // Plan:
        //   Implement the 'ntci::Monitorable' and 'ntci::MonitorableRegistry'
        //   interfaces suitable for this test driver. The monitorable object
        //   simulates some responsibilty to do work and measures the time it
        //   takes for perform that work. Test the various "reset" semantics
        //   when querying statistics. Test that statistics are maintained in
        //   separate "application" and "internal" snapshots. Test that the
        //   cumulative statistics are never reset.

        ntccfg::TestAllocator ta;
        {
            bsls::TimeInterval currentTime = bdlt::CurrentTime::now();

            // Create the monitorable object registry and set it as the
            // default monitorable registry.

            bsl::shared_ptr<test::ObjectRegistry> objectRegistry;
            objectRegistry.createInplace(&ta, &ta);

            // Create a monitorable object and register it with the default
            // monitorable registry.

            bsl::shared_ptr<test::Object> object;
            object.createInplace(&ta);

            objectRegistry->registerMonitorable(object);

            // Ensure the registry is tracking this monitorable object.

            {
                bsl::vector<bsl::shared_ptr<ntci::Monitorable> >
                    registeredObjects;

                objectRegistry->loadRegisteredObjects(&registeredObjects);

                ASSERT(registeredObjects.size() == 1);
                ASSERT(registeredObjects[0] == object);
            }

            // Test three measurement intervals...

            for (bsl::int64_t currentInterval = 0; currentInterval < 3;
                 ++currentInterval)
            {
                // Advance to the next measurment interval.

                currentTime.addSeconds(1);
                object->setCurrentTime(currentTime);

                // Perform two iterations of work.

                object->execute();
                object->execute();

                // Get the internal snapshot statistics measured by the
                // monitorable object since they were last reset, and reset
                // their values. Ensure the statistics are not zero.

                bsl::int64_t stats1[test::Object::NUM_STATISTICS];
                test::ObjectUtil::getStats(stats1, object.get(), &ta);

                ASSERT(stats1[test::Object::STATISTIC_COUNT] > 0);
                ASSERT(stats1[test::Object::STATISTIC_TOTAL] > 0);

                ASSERT(stats1[test::Object::STATISTIC_MIN] !=
                       bsl::numeric_limits<bsl::int64_t>::max());

                ASSERT(stats1[test::Object::STATISTIC_MAX] !=
                       bsl::numeric_limits<bsl::int64_t>::min());

                // Get the internal snapshot statistics measured by the
                // monitorable object since they were last reset, and reset
                // their values.  Ensure the statistics are zero since there
                // has been no activity since the last query.

                bsl::int64_t stats2[test::Object::NUM_STATISTICS];
                test::ObjectUtil::getStats(stats2, object.get(), &ta);

                ASSERT(stats2[test::Object::STATISTIC_COUNT] == 0);
                ASSERT(stats2[test::Object::STATISTIC_TOTAL] == 0);
                ASSERT(stats2[test::Object::STATISTIC_MIN] ==
                       bsl::numeric_limits<bsl::int64_t>::max());
                ASSERT(stats2[test::Object::STATISTIC_MAX] ==
                       bsl::numeric_limits<bsl::int64_t>::min());
            }

            // Deregister the monitorable object.

            objectRegistry->deregisterMonitorable(object);

            // Ensure the registry is no longer tracking this monitorable
            // object.

            {
                bsl::vector<bsl::shared_ptr<ntci::Monitorable> >
                    registeredObjects;

                objectRegistry->loadRegisteredObjects(&registeredObjects);

                ASSERT(registeredObjects.size() == 0);
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
