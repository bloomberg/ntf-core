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
BSLS_IDENT_RCSID(ntsa_backoff_t_cpp, "$Id$ $CSID$")

#include <ntsa_backoff.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Backoff'.
class BackoffTest
{
  public:
    // Verify no progression with an initial value of 1.
    static void verifyDefault1();

    // Verify no progression with an initial value of 1.
    static void verifyDefault8();

    // Verify an arithmetic progression with an initial value of 1.
    static void verifyArithmetic1();

    // Verify an arithmetic progression with an initial value of 8.
    static void verifyArithmetic8();

    // Verify an geometric progression with an initial value of 1.
    static void verifyGeometric1();

    // Verify an geometric progression with an initial value of 8.
    static void verifyGeometric8();

  private:
    static void log(bsl::size_t step, const bsls::TimeInterval& interval);
};

NTSCFG_TEST_FUNCTION(ntsa::BackoffTest::verifyDefault1)
{
    ntsa::Backoff backoff;

    bsls::TimeInterval interval(1);
    BackoffTest::log(0, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(1));

    interval = backoff.apply(interval);
    BackoffTest::log(1, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(1));

    interval = backoff.apply(interval);
    BackoffTest::log(2, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(1));

    interval = backoff.apply(interval);
    BackoffTest::log(3, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(1));

    interval = backoff.apply(interval);
    BackoffTest::log(4, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(1));
}

NTSCFG_TEST_FUNCTION(ntsa::BackoffTest::verifyDefault8)
{
    ntsa::Backoff backoff;

    bsls::TimeInterval interval(8);
    BackoffTest::log(0, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(8));

    interval = backoff.apply(interval);
    BackoffTest::log(1, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(8));

    interval = backoff.apply(interval);
    BackoffTest::log(2, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(8));

    interval = backoff.apply(interval);
    BackoffTest::log(3, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(8));

    interval = backoff.apply(interval);
    BackoffTest::log(4, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(8));
}

NTSCFG_TEST_FUNCTION(ntsa::BackoffTest::verifyArithmetic1)
{
    ntsa::Backoff backoff;
    backoff.makeArithmetic(bsls::TimeInterval(1));

    bsls::TimeInterval interval(1);
    BackoffTest::log(0, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(1));

    interval = backoff.apply(interval);
    BackoffTest::log(1, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(2));

    interval = backoff.apply(interval);
    BackoffTest::log(2, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(3));

    interval = backoff.apply(interval);
    BackoffTest::log(3, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(4));

    interval = backoff.apply(interval);
    BackoffTest::log(4, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(5));
}

NTSCFG_TEST_FUNCTION(ntsa::BackoffTest::verifyArithmetic8)
{
    ntsa::Backoff backoff;
    backoff.makeArithmetic(bsls::TimeInterval(1));

    bsls::TimeInterval interval(8);
    BackoffTest::log(0, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(8));

    interval = backoff.apply(interval);
    BackoffTest::log(1, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(9));

    interval = backoff.apply(interval);
    BackoffTest::log(2, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(10));

    interval = backoff.apply(interval);
    BackoffTest::log(3, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(11));

    interval = backoff.apply(interval);
    BackoffTest::log(4, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(12));
}

NTSCFG_TEST_FUNCTION(ntsa::BackoffTest::verifyGeometric1)
{
    ntsa::Backoff backoff;
    backoff.makeGeometric(2.0);

    bsls::TimeInterval interval(1);
    BackoffTest::log(0, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(1));

    interval = backoff.apply(interval);
    BackoffTest::log(1, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(2));

    interval = backoff.apply(interval);
    BackoffTest::log(2, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(4));

    interval = backoff.apply(interval);
    BackoffTest::log(3, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(8));

    interval = backoff.apply(interval);
    BackoffTest::log(4, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(16));
}

NTSCFG_TEST_FUNCTION(ntsa::BackoffTest::verifyGeometric8)
{
    ntsa::Backoff backoff;
    backoff.makeGeometric(2.0);

    bsls::TimeInterval interval(8);
    BackoffTest::log(0, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(8));

    interval = backoff.apply(interval);
    BackoffTest::log(1, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(16));

    interval = backoff.apply(interval);
    BackoffTest::log(2, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(32));

    interval = backoff.apply(interval);
    BackoffTest::log(3, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(64));

    interval = backoff.apply(interval);
    BackoffTest::log(4, interval);
    NTSCFG_TEST_EQ(interval, bsls::TimeInterval(128));
}

void BackoffTest::log(bsl::size_t step, const bsls::TimeInterval& interval)
{
    NTSCFG_TEST_LOG_DEBUG << "t[" << step << "] = " << interval
                          << NTSCFG_TEST_LOG_END;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
