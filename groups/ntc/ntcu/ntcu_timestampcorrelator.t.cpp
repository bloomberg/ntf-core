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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcu_timestampcorrelator_t_cpp, "$Id$ $CSID$")

#include <ntcu_timestampcorrelator.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcu {

// Provide tests for 'ntcu::TimestampCorrelator'.
class TimestampCorrelatorTest
{
    static const bsls::TimeInterval k_ONE_SEC;
    static const bsls::TimeInterval k_TWO_SEC;

    // This structure is used for testing purposes. It's a collection of values
    // describing time point before sending a packet and related timestamps.
    struct Element;

    // Generate and return random TimeInterval based on the specified 'seed'.
    static bsls::TimeInterval randomTime(int* seed);

    // Generate and return the specified 'time' increased by the random time
    // value based on the specified 'seed'.
    static bsls::TimeInterval randomTimeIncreased(
        const bsls::TimeInterval& time,
        int*                      seed);

    // Helper function used to multiply the specified 'time' to the specified
    // 'v' For simplicity, the specified 'time' should be an integral number of
    // seconds less than 100. Returns the resulting bsls::TimeInterval.
    static bsls::TimeInterval multiply(const bsls::TimeInterval& ti, int v)
    {
        const bsls::Types::Int64 seconds = ti.seconds();
        BSLS_ASSERT(seconds < 100);
        BSLS_ASSERT(ti.nanoseconds() == 0);
        bsls::TimeInterval res(seconds * v, 0);
        return res;
    }

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();

    // TODO
    static void verifyCase6();
};

const bsls::TimeInterval TimestampCorrelatorTest::k_ONE_SEC =
    bsls::TimeInterval(1, 0);

const bsls::TimeInterval TimestampCorrelatorTest::k_TWO_SEC =
    k_ONE_SEC + k_ONE_SEC;

/// This structure is used for testing purposes. It's a collection of values
/// describing a time point before sending a packet and related timestamps.
struct TimestampCorrelatorTest::Element {
    bsls::TimeInterval d_reference;
    bsl::uint32_t      d_id;
    ntsa::Timestamp    d_timestampScheduled;
    ntsa::Timestamp    d_timestampSent;
    ntsa::Timestamp    d_timestampAcknowledged;

    explicit Element(bsl::uint32_t id);
    Element& operator=(const Element& other);
};

TimestampCorrelatorTest::Element::Element(bsl::uint32_t id)
: d_reference()
, d_id(id)
, d_timestampScheduled()
, d_timestampSent()
, d_timestampAcknowledged()
{
    d_timestampScheduled.setType(ntsa::TimestampType::e_SCHEDULED);
    d_timestampSent.setType(ntsa::TimestampType::e_SENT);
    d_timestampAcknowledged.setType(ntsa::TimestampType::e_ACKNOWLEDGED);

    d_timestampScheduled.setId(id);
    d_timestampSent.setId(id);
    d_timestampAcknowledged.setId(id);
}

TimestampCorrelatorTest::Element& TimestampCorrelatorTest::Element::operator=(
    const Element& other)
{
    d_reference             = other.d_reference;
    d_id                    = other.d_id;
    d_timestampScheduled    = other.d_timestampScheduled;
    d_timestampSent         = other.d_timestampSent;
    d_timestampAcknowledged = other.d_timestampAcknowledged;
    return *this;
}

bsls::TimeInterval TimestampCorrelatorTest::randomTime(int* seed)
{
    BSLS_ASSERT(seed != 0);
    return bsls::TimeInterval(bdlb::Random::generate15(seed) % 1000,
                              bdlb::Random::generate15(seed) % 1000);
}

bsls::TimeInterval TimestampCorrelatorTest::randomTimeIncreased(
    const bsls::TimeInterval& time,
    int*                      seed)
{
    return time + TimestampCorrelatorTest::randomTime(seed);
}

NTSCFG_TEST_FUNCTION(ntcu::TimestampCorrelatorTest::verifyCase1)
{
    // Concern: test basic construction & destruction

    TimestampCorrelator tc1(ntsa::TransportMode::e_DATAGRAM,
                            NTSCFG_TEST_ALLOCATOR);
    TimestampCorrelator tc2(ntsa::TransportMode::e_STREAM,
                            NTSCFG_TEST_ALLOCATOR);
}

NTSCFG_TEST_FUNCTION(ntcu::TimestampCorrelatorTest::verifyCase2)
{
    // Concern: test basic scenario to save timestamp and extract it
    const bsl::uint32_t id = 0;

    TimestampCorrelator tc(ntsa::TransportMode::e_DATAGRAM,
                           NTSCFG_TEST_ALLOCATOR);
    bsls::TimeInterval  ti(100, 200);
    tc.saveTimestampBeforeSend(ti, id);

    ntsa::Timestamp ts;
    ts.setId(id);
    ts.setType(ntsa::TimestampType::e_SCHEDULED);
    ts.setTime(ti + TimestampCorrelatorTest::k_ONE_SEC);

    ts.setId(id);
    bdlb::NullableValue<bsls::TimeInterval> diff = tc.timestampReceived(ts);
    NTSCFG_TEST_TRUE(diff.has_value());
    NTSCFG_TEST_EQ(diff.value(), TimestampCorrelatorTest::k_ONE_SEC);
}

NTSCFG_TEST_FUNCTION(ntcu::TimestampCorrelatorTest::verifyCase3)
{
    // Concern: test wrapping of internal ring buffer

    const bsl::size_t rbSize = TimestampCorrelator::k_RING_BUFFER_SIZE;

    TimestampCorrelator tc(ntsa::TransportMode::e_STREAM,
                           NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_FALSE(tc.timestampReceived(ntsa::Timestamp()).has_value());
    // now 'tc'is operational

    {  // Push numTimestamps to the correlator
        bsls::TimeInterval ti = TimestampCorrelatorTest::k_ONE_SEC;
        for (bsl::size_t i = 0; i < rbSize * 2; ++i) {
            tc.saveTimestampBeforeSend(ti, i);
            ti += TimestampCorrelatorTest::k_ONE_SEC;
        }
    }

    // buffer should contain only timestamps starting from
    // TimestampCorrelatorTest::k_ONE_SEC * (rbSize + 1)
    {
        // Extract all timestamps
        ntsa::Timestamp ts;
        ts.setType(ntsa::TimestampType::e_ACKNOWLEDGED);

        bsls::TimeInterval ti = TimestampCorrelatorTest::k_ONE_SEC;
        ti = TimestampCorrelatorTest::multiply(ti, rbSize + 1);
        ti = ti + TimestampCorrelatorTest::k_TWO_SEC;

        int id = rbSize;
        for (bsl::size_t i = 0; i < rbSize; ++i) {
            ts.setId(id);
            ts.setTime(ti);

            ti += TimestampCorrelatorTest::k_ONE_SEC;
            ++id;

            bdlb::NullableValue<bsls::TimeInterval> diff =
                tc.timestampReceived(ts);
            NTSCFG_TEST_TRUE(diff.has_value());
            NTSCFG_TEST_EQ(diff.value(), TimestampCorrelatorTest::k_TWO_SEC);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntcu::TimestampCorrelatorTest::verifyCase4)
{
    // Concern: complex test case with pseudo random timestamps,
    // timestamps are fetched in order of their saving

    int       seed          = 22;
    const int numTimestamps = 10000;

    bsl::unordered_set<bsl::uint32_t> uniqueIds(NTSCFG_TEST_ALLOCATOR);
    bsl::vector<TimestampCorrelatorTest::Element> elements(
        NTSCFG_TEST_ALLOCATOR);
    for (int i = 0; i < numTimestamps;) {
        const bsl::uint32_t id = bdlb::Random::generate15(&seed);
        if (uniqueIds.count(id) > 0) {
            continue;
        }

        bsls::TimeInterval time(TimestampCorrelatorTest::randomTime(&seed));
        TimestampCorrelatorTest::Element el(id);
        el.d_reference = time;
        el.d_timestampScheduled.setTime(
            TimestampCorrelatorTest::randomTimeIncreased(time, &seed));
        el.d_timestampSent.setTime(
            TimestampCorrelatorTest::randomTimeIncreased(time, &seed));
        el.d_timestampScheduled.setTime(
            TimestampCorrelatorTest::randomTimeIncreased(time, &seed));

        elements.push_back(el);
        ++i;
    }

    TimestampCorrelator tc(ntsa::TransportMode::e_STREAM,
                           NTSCFG_TEST_ALLOCATOR);

    for (bsl::size_t i = 0; i < TimestampCorrelator::k_RING_BUFFER_SIZE; ++i) {
        tc.saveTimestampBeforeSend(elements.at(i).d_reference,
                                   elements.at(i).d_id);
    }

    for (bsl::size_t i = 0;
         i < numTimestamps - TimestampCorrelator::k_RING_BUFFER_SIZE;
         ++i)
    {
        const TimestampCorrelatorTest::Element& el = elements.at(i);

        bdlb::NullableValue<bsls::TimeInterval> diff =
            tc.timestampReceived(el.d_timestampScheduled);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampScheduled.time() - el.d_reference);

        diff = tc.timestampReceived(el.d_timestampSent);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampSent.time() - el.d_reference);

        diff = tc.timestampReceived(el.d_timestampAcknowledged);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampAcknowledged.time() - el.d_reference);

        tc.saveTimestampBeforeSend(
            elements.at(i + TimestampCorrelator::k_RING_BUFFER_SIZE)
                .d_reference,
            elements.at(i + TimestampCorrelator::k_RING_BUFFER_SIZE).d_id);
    }

    for (bsl::size_t i =
             numTimestamps - TimestampCorrelator::k_RING_BUFFER_SIZE;
         i < numTimestamps;
         ++i)
    {
        const TimestampCorrelatorTest::Element& el = elements.at(i);

        bdlb::NullableValue<bsls::TimeInterval> diff =
            tc.timestampReceived(el.d_timestampScheduled);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampScheduled.time() - el.d_reference);

        diff = tc.timestampReceived(el.d_timestampSent);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampSent.time() - el.d_reference);

        diff = tc.timestampReceived(el.d_timestampAcknowledged);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampAcknowledged.time() - el.d_reference);
    }
}

NTSCFG_TEST_FUNCTION(ntcu::TimestampCorrelatorTest::verifyCase5)
{
    // Concern: test that timestamps can be fetched even in random order
    const int numTimestamps = TimestampCorrelator::k_RING_BUFFER_SIZE;
    int       seed          = 123;

    bsl::unordered_set<bsl::uint32_t> uniqueIds(NTSCFG_TEST_ALLOCATOR);
    bsl::vector<TimestampCorrelatorTest::Element> elements(
        NTSCFG_TEST_ALLOCATOR);
    while (elements.size() < numTimestamps) {
        const bsl::uint32_t id = bdlb::Random::generate15(&seed);
        if (uniqueIds.count(id) > 0) {
            continue;
        }
        TimestampCorrelatorTest::Element el(id);
        bsls::TimeInterval time(TimestampCorrelatorTest::randomTime(&seed));
        el.d_reference = time;
        el.d_timestampScheduled.setTime(
            TimestampCorrelatorTest::randomTimeIncreased(time, &seed));
        el.d_timestampSent.setTime(
            TimestampCorrelatorTest::randomTimeIncreased(time, &seed));
        el.d_timestampScheduled.setTime(
            TimestampCorrelatorTest::randomTimeIncreased(time, &seed));

        elements.push_back(el);
    }

    // now fill the correlator
    TimestampCorrelator tc(ntsa::TransportMode::e_STREAM,
                           NTSCFG_TEST_ALLOCATOR);

    for (bsl::size_t i = 0; i < elements.size(); ++i) {
        tc.saveTimestampBeforeSend(elements.at(i).d_reference,
                                   elements.at(i).d_id);
    }

    while (!elements.empty()) {
        int index = bdlb::Random::generate15(&seed) % elements.size();
        const TimestampCorrelatorTest::Element& el = elements.at(index);

        bdlb::NullableValue<bsls::TimeInterval> diff =
            tc.timestampReceived(el.d_timestampScheduled);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampScheduled.time() - el.d_reference);
        diff = tc.timestampReceived(el.d_timestampScheduled);
        NTSCFG_TEST_FALSE(diff.has_value());

        diff = tc.timestampReceived(el.d_timestampSent);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampSent.time() - el.d_reference);
        diff = tc.timestampReceived(el.d_timestampSent);
        NTSCFG_TEST_FALSE(diff.has_value());

        diff = tc.timestampReceived(el.d_timestampAcknowledged);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(),
                       el.d_timestampAcknowledged.time() - el.d_reference);
        diff = tc.timestampReceived(el.d_timestampAcknowledged);
        NTSCFG_TEST_FALSE(diff.has_value());

        elements.erase(&el);
    }
}

NTSCFG_TEST_FUNCTION(ntcu::TimestampCorrelatorTest::verifyCase6)
{
    // Concern: test that after reset existing timestamps cannot be fetched

    const int numTimestamps = TimestampCorrelator::k_RING_BUFFER_SIZE;

    TimestampCorrelator tc(ntsa::TransportMode::e_STREAM,
                           NTSCFG_TEST_ALLOCATOR);

    for (int i = 0; i < numTimestamps; ++i) {
        tc.saveTimestampBeforeSend(bsls::TimeInterval(), i);
    }

    tc.reset();
    for (int i = 0; i < numTimestamps; ++i) {
        ntsa::Timestamp ts;
        ts.setId(i);
        ts.setType(ntsa::TimestampType::e_SCHEDULED);
        NTSCFG_TEST_FALSE(tc.timestampReceived(ts).has_value());

        ts.setType(ntsa::TimestampType::e_SENT);
        NTSCFG_TEST_FALSE(tc.timestampReceived(ts).has_value());

        ts.setType(ntsa::TimestampType::e_ACKNOWLEDGED);
        NTSCFG_TEST_FALSE(tc.timestampReceived(ts).has_value());
    }

    {
        const int randomId = 98;
        tc.saveTimestampBeforeSend(TimestampCorrelatorTest::k_ONE_SEC,
                                   randomId);
        ntsa::Timestamp ts;
        ts.setId(randomId);
        ts.setType(ntsa::TimestampType::e_SCHEDULED);
        ts.setTime(TimestampCorrelatorTest::k_TWO_SEC);
        bdlb::NullableValue<bsls::TimeInterval> diff =
            tc.timestampReceived(ts);
        NTSCFG_TEST_TRUE(diff.has_value());
        NTSCFG_TEST_EQ(diff.value(), TimestampCorrelatorTest::k_ONE_SEC);
    }
}

}  // close namespace ntcu
}  // close namespace BloombergLP
