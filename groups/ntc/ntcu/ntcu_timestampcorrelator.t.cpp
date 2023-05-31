// Copyright 2023 Bloomberg Finance L.P.
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

#include <ntcu_timestampcorrelator.h>

#include <bdlb_random.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

#include <ntccfg_test.h>

using namespace BloombergLP;
using namespace ntcu;

namespace Test {
const bsls::TimeInterval oneSec = bsls::TimeInterval(1, 0);
const bsls::TimeInterval twoSec = oneSec + oneSec;

/// This structure is used for testing purposes. It's a collection of values
/// describing time point before sending a packet and related timestamps.
struct Element {
    bsls::TimeInterval d_reference;
    bsl::uint32_t      d_id;
    ntsa::Timestamp    d_timestampScheduled;
    ntsa::Timestamp    d_timestampSent;
    ntsa::Timestamp    d_timestampAcknowledged;

    /// C-tor.
    Element(bsl::uint32_t id);

    /// Assignment operator.
    Element& operator=(const Element& other);
};

Element::Element(bsl::uint32_t id)
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

Element& Element::operator=(const Element& other)
{
    d_reference             = other.d_reference;
    d_id                    = other.d_id;
    d_timestampScheduled    = other.d_timestampScheduled;
    d_timestampSent         = other.d_timestampSent;
    d_timestampAcknowledged = other.d_timestampAcknowledged;
    return *this;
}

/// Generate and return random TimeInterval based on the specified 'seed'.
bsls::TimeInterval randomTime(int* seed);

/// Generate and return the specified 'time' increased by the random time value
/// based on the specified 'seed'.
bsls::TimeInterval randomTimeIncreased(const bsls::TimeInterval& time,
                                       int*                      seed);

bsls::TimeInterval randomTime(int* seed)
{
    BSLS_ASSERT(seed != 0);
    return bsls::TimeInterval(bdlb::Random::generate15(seed) % 1000,
                              bdlb::Random::generate15(seed) % 1000);
}

bsls::TimeInterval randomTimeIncreased(const bsls::TimeInterval& time,
                                       int*                      seed)
{
    return time + randomTime(seed);
}

}

/// Helper function used to multiply the specified 'time' to the specified 'v'
/// For simplicity, the specified 'time' should be an integral number of
/// seconds less than 100. Returns the resulting bsls::TimeInterval.
bsls::TimeInterval operator*(const bsls::TimeInterval& ti, int v)
{
    const bsls::Types::Int64 seconds = ti.seconds();
    BSLS_ASSERT(seconds < 100);
    BSLS_ASSERT(ti.nanoseconds() == 0);
    bsls::TimeInterval res(seconds * v, 0);
    return res;
}

NTCCFG_TEST_CASE(1)
{
    // Concern: test basic construction & destruction
    bslma::TestAllocator ta;
    {
        TimestampCorrelator tc1(ntsa::TransportMode::e_DATAGRAM, &ta);
        TimestampCorrelator tc2(ntsa::TransportMode::e_STREAM, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: test basic scenario to save timestamp and extract it
    const bsl::uint32_t  id = 0;
    bslma::TestAllocator ta;
    {
        TimestampCorrelator tc(ntsa::TransportMode::e_DATAGRAM, &ta);
        bsls::TimeInterval  ti(100, 200);
        tc.saveTimestampBeforeSend(ti, id);

        ntsa::Timestamp ts;
        ts.setId(id);
        ts.setType(ntsa::TimestampType::e_SCHEDULED);
        ts.setTime(ti + Test::oneSec);

        bdlb::NullableValue<bsls::TimeInterval> diff =
            tc.timestampReceived(ts);
        NTCCFG_TEST_FALSE(diff.has_value());  // first timestamp is ignored

        tc.saveTimestampBeforeSend(ti, id + 1);
        ts.setId(id + 1);
        diff = tc.timestampReceived(ts);
        NTCCFG_TEST_TRUE(diff.has_value());
        NTCCFG_TEST_EQ(diff.value(), Test::oneSec);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: check that timestamps are not saved until the first one is
    // received
    const int            numTimestamps = 64;
    bslma::TestAllocator ta;
    {
        TimestampCorrelator tc(ntsa::TransportMode::e_STREAM, &ta);

        {  /// Push numTimestamps to the correlator
            bsls::TimeInterval ti = Test::oneSec;
            for (int i = 0; i < numTimestamps; ++i) {
                tc.saveTimestampBeforeSend(ti, i);
                ti += Test::oneSec;
            }
        }

        {
            // Try to extract all these timestamps: it should not work as these
            // timestamps were not saved
            ntsa::Timestamp    ts;
            bsls::TimeInterval ti = Test::oneSec + Test::oneSec;
            ts.setId(0);
            ts.setType(ntsa::TimestampType::e_SENT);
            ts.setTime(ti);

            bdlb::NullableValue<bsls::TimeInterval> diff =
                tc.timestampReceived(ts);
            NTCCFG_TEST_FALSE(diff.has_value());

            ti += Test::oneSec;
            for (int i = 1; i < numTimestamps; ++i) {
                ts.setId(i);
                ts.setTime(ti);
                ti   += Test::oneSec;
                diff = tc.timestampReceived(ts);
                NTCCFG_TEST_FALSE(diff.has_value());
            }
        }

        {
            // Now timestamps can be saved and extracted
            const int id = 500;
            tc.saveTimestampBeforeSend(Test::oneSec, id);
            ntsa::Timestamp ts;
            ts.setId(id);
            ts.setType(ntsa::TimestampType::e_SENT);
            ts.setTime(Test::twoSec + Test::oneSec);
            bdlb::NullableValue<bsls::TimeInterval> diff =
                tc.timestampReceived(ts);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(), Test::twoSec);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: test wrapping of internal ring buffer

    const bsl::size_t    rbSize = TimestampCorrelator::k_RING_BUFFER_SIZE;
    bslma::TestAllocator ta;
    {
        TimestampCorrelator tc(ntsa::TransportMode::e_STREAM, &ta);
        NTCCFG_TEST_FALSE(tc.timestampReceived(ntsa::Timestamp()).has_value());
        // now 'tc'is operational

        {  // Push numTimestamps to the correlator
            bsls::TimeInterval ti = Test::oneSec;
            for (bsl::size_t i = 0; i < rbSize * 2; ++i) {
                tc.saveTimestampBeforeSend(ti, i);
                ti += Test::oneSec;
            }
        }

        // buffer should contain only timestamps starting from
        // Test::oneSec * (rbSize + 1)
        {
            // Extract all timestamps
            ntsa::Timestamp ts;
            ts.setType(ntsa::TimestampType::e_ACKNOWLEDGED);

            bsls::TimeInterval ti = Test::oneSec * (rbSize + 1) + Test::twoSec;
            int                id = rbSize;
            for (bsl::size_t i = 0; i < rbSize; ++i) {
                ts.setId(id);
                ts.setTime(ti);

                ti += Test::oneSec;
                ++id;

                bdlb::NullableValue<bsls::TimeInterval> diff =
                    tc.timestampReceived(ts);
                NTCCFG_TEST_TRUE(diff.has_value());
                NTCCFG_TEST_EQ(diff.value(), Test::twoSec);
            }
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Concern: complex test case with pseudo random timestamps,
    // timestamps are fetched in order of their saving

    int       seed          = 22;
    const int numTimestamps = 10000;

    bslma::TestAllocator ta;
    {
        bsl::unordered_set<bsl::uint32_t> uniqueIds(&ta);
        bsl::vector<Test::Element>        elements(&ta);
        for (int i = 0; i < numTimestamps;) {
            const bsl::uint32_t id = bdlb::Random::generate15(&seed);
            if (uniqueIds.count(id) > 0) {
                continue;
            }

            bsls::TimeInterval time(Test::randomTime(&seed));
            Test::Element      el(id);
            el.d_reference = time;
            el.d_timestampScheduled.setTime(
                Test::randomTimeIncreased(time, &seed));
            el.d_timestampSent.setTime(Test::randomTimeIncreased(time, &seed));
            el.d_timestampScheduled.setTime(
                Test::randomTimeIncreased(time, &seed));

            elements.push_back(el);
            ++i;
        }

        TimestampCorrelator tc(ntsa::TransportMode::e_STREAM, &ta);
        NTCCFG_TEST_FALSE(tc.timestampReceived(ntsa::Timestamp()).has_value());
        // now 'tc'is operational

        for (bsl::size_t i = 0; i < TimestampCorrelator::k_RING_BUFFER_SIZE;
             ++i)
        {
            tc.saveTimestampBeforeSend(elements.at(i).d_reference,
                                       elements.at(i).d_id);
        }

        for (bsl::size_t i = 0;
             i < numTimestamps - TimestampCorrelator::k_RING_BUFFER_SIZE;
             ++i)
        {
            const Test::Element& el = elements.at(i);

            bdlb::NullableValue<bsls::TimeInterval> diff =
                tc.timestampReceived(el.d_timestampScheduled);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
                           el.d_timestampScheduled.time() - el.d_reference);

            diff = tc.timestampReceived(el.d_timestampSent);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
                           el.d_timestampSent.time() - el.d_reference);

            diff = tc.timestampReceived(el.d_timestampAcknowledged);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
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
            const Test::Element& el = elements.at(i);

            bdlb::NullableValue<bsls::TimeInterval> diff =
                tc.timestampReceived(el.d_timestampScheduled);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
                           el.d_timestampScheduled.time() - el.d_reference);

            diff = tc.timestampReceived(el.d_timestampSent);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
                           el.d_timestampSent.time() - el.d_reference);

            diff = tc.timestampReceived(el.d_timestampAcknowledged);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
                           el.d_timestampAcknowledged.time() - el.d_reference);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    // Concern: test that timestamps can be fetched even in random order
    const int numTimestamps = TimestampCorrelator::k_RING_BUFFER_SIZE;
    int       seed          = 123;

    bslma::TestAllocator ta;
    {
        bsl::unordered_set<bsl::uint32_t> uniqueIds(&ta);
        bsl::vector<Test::Element>        elements(&ta);
        while (elements.size() < numTimestamps) {
            const bsl::uint32_t id = bdlb::Random::generate15(&seed);
            if (uniqueIds.count(id) > 0) {
                continue;
            }
            Test::Element      el(id);
            bsls::TimeInterval time(Test::randomTime(&seed));
            el.d_reference = time;
            el.d_timestampScheduled.setTime(
                Test::randomTimeIncreased(time, &seed));
            el.d_timestampSent.setTime(Test::randomTimeIncreased(time, &seed));
            el.d_timestampScheduled.setTime(
                Test::randomTimeIncreased(time, &seed));

            elements.push_back(el);
        }

        // now fill the correlator
        TimestampCorrelator tc(ntsa::TransportMode::e_STREAM, &ta);
        NTCCFG_TEST_FALSE(tc.timestampReceived(ntsa::Timestamp()).has_value());
        // now 'tc'is operational

        for (bsl::size_t i = 0; i < elements.size(); ++i) {
            tc.saveTimestampBeforeSend(elements.at(i).d_reference,
                                       elements.at(i).d_id);
        }

        while (!elements.empty()) {
            int index = bdlb::Random::generate15(&seed) % elements.size();
            const Test::Element& el = elements.at(index);

            bdlb::NullableValue<bsls::TimeInterval> diff =
                tc.timestampReceived(el.d_timestampScheduled);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
                           el.d_timestampScheduled.time() - el.d_reference);
            diff = tc.timestampReceived(el.d_timestampScheduled);
            NTCCFG_TEST_FALSE(diff.has_value());

            diff = tc.timestampReceived(el.d_timestampSent);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
                           el.d_timestampSent.time() - el.d_reference);
            diff = tc.timestampReceived(el.d_timestampSent);
            NTCCFG_TEST_FALSE(diff.has_value());

            diff = tc.timestampReceived(el.d_timestampAcknowledged);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(),
                           el.d_timestampAcknowledged.time() - el.d_reference);
            diff = tc.timestampReceived(el.d_timestampAcknowledged);
            NTCCFG_TEST_FALSE(diff.has_value());

            elements.erase(&el);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(7)
{
    // Concern: test that after reset existing timestamps cannot be fetched

    const int numTimestamps = TimestampCorrelator::k_RING_BUFFER_SIZE;

    bslma::TestAllocator ta;
    {
        TimestampCorrelator tc(ntsa::TransportMode::e_STREAM, &ta);
        NTCCFG_TEST_FALSE(tc.timestampReceived(ntsa::Timestamp()).has_value());
        // now 'tc'is operational

        for (int i = 0; i < numTimestamps; ++i) {
            tc.saveTimestampBeforeSend(bsls::TimeInterval(), i);
        }

        tc.reset();
        for (int i = 0; i < numTimestamps; ++i) {
            ntsa::Timestamp ts;
            ts.setId(i);
            ts.setType(ntsa::TimestampType::e_SCHEDULED);
            NTCCFG_TEST_FALSE(tc.timestampReceived(ts).has_value());

            ts.setType(ntsa::TimestampType::e_SENT);
            NTCCFG_TEST_FALSE(tc.timestampReceived(ts).has_value());

            ts.setType(ntsa::TimestampType::e_ACKNOWLEDGED);
            NTCCFG_TEST_FALSE(tc.timestampReceived(ts).has_value());
        }

        {
            const int randomId = 98;
            tc.saveTimestampBeforeSend(Test::oneSec, randomId);
            ntsa::Timestamp ts;
            ts.setId(randomId);
            ts.setType(ntsa::TimestampType::e_SCHEDULED);
            ts.setTime(Test::twoSec);
            bdlb::NullableValue<bsls::TimeInterval> diff =
                tc.timestampReceived(ts);
            NTCCFG_TEST_TRUE(diff.has_value());
            NTCCFG_TEST_EQ(diff.value(), Test::oneSec);
        }
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
    NTCCFG_TEST_REGISTER(7);
}
NTCCFG_TEST_DRIVER_END;
