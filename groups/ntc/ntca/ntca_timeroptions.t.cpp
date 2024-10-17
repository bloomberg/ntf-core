
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
BSLS_IDENT_RCSID(ntca_timeroptions_t_cpp, "$Id$ $CSID$")

#include <ntca_timeroptions.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntca {

// Provide tests for 'ntca::TimerOptions'.
class TimerOptionsTest
{
  public:
    // Test default constructor.
    static void verifyDefaultConstructor();

    // Test copy constructor.
    static void verifyCopyConstructor();

    // Test move constructor.
    static void verifyMoveConstructor();

    // Test assignment operator.
    static void verifyCopyAssignmentOperator();

    // Test move-assignment operator.
    static void verifyMoveAssignmentOperator();

    // Test 'reset'.
    static void verifyReset();

    // Test 'move'.
    static void verifyMove();

    // Test 'setHandle'.
    static void verifySetHandle();

    // Test 'setId'.
    static void verifySetId();

    // Test 'setOneShot'.
    static void verifySetOneShot();

    // Test 'setDrift'.
    static void verifySetDrift();

    // Test 'showEvent', 'hideEvent', and 'wantEvent'.
    static void verifyShowEvent();

    // Test 'equals'.
    static void verifyEquals();

    // Test 'less'.
    static void verifyLess();

    // Test 'hash'.
    static void verifyHash();

    // Test 'print'.
    static void verifyPrint();

  private:
    static const void* k_HANDLE;
    static const int   k_ID;
    static const bool  k_ONE_SHOT;
    static const bool  k_DRIFT;
};

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyDefaultConstructor)
{
    ntca::TimerOptions timerOptions;

    NTSCFG_TEST_EQ(timerOptions.handle(),
                   reinterpret_cast<void*>(static_cast<bsl::intptr_t>(0)));
    NTSCFG_TEST_EQ(timerOptions.id(), 0);
    NTSCFG_TEST_EQ(timerOptions.oneShot(), false);
    NTSCFG_TEST_EQ(timerOptions.drift(), false);

    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   true);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   true);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyCopyConstructor)
{
    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle((void*)k_HANDLE);
    otherTimerOptions.setId(k_ID);
    otherTimerOptions.setOneShot(k_ONE_SHOT);
    otherTimerOptions.setDrift(k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTSCFG_TEST_EQ(otherTimerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(otherTimerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(otherTimerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(otherTimerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTSCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Copy-construct a timer options from the other timer options.

    char                buffer[sizeof(ntca::TimerOptions)];
    ntca::TimerOptions* timerOptions =
        new (buffer) ntca::TimerOptions(otherTimerOptions);

    // Ensure the copy-constructed timer options has the expected value.

    NTSCFG_TEST_EQ(timerOptions->handle(), k_HANDLE);
    NTSCFG_TEST_EQ(timerOptions->id(), k_ID);
    NTSCFG_TEST_EQ(timerOptions->oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(timerOptions->drift(), k_DRIFT);

    NTSCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTSCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTSCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Ensure the other time options still has its original value.

    NTSCFG_TEST_EQ(otherTimerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(otherTimerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(otherTimerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(otherTimerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTSCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyMoveConstructor)
{
    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle((void*)k_HANDLE);
    otherTimerOptions.setId(k_ID);
    otherTimerOptions.setOneShot(k_ONE_SHOT);
    otherTimerOptions.setDrift(k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTSCFG_TEST_EQ(otherTimerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(otherTimerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(otherTimerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(otherTimerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTSCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Move-construct a timer options from the other timer options.

    char                buffer[sizeof(ntca::TimerOptions)];
    ntca::TimerOptions* timerOptions = new (buffer)
        ntca::TimerOptions(bslmf::MovableRefUtil::move(otherTimerOptions));

    // Ensure the move-constructed timer options has the expected value.

    NTSCFG_TEST_EQ(timerOptions->handle(), k_HANDLE);
    NTSCFG_TEST_EQ(timerOptions->id(), k_ID);
    NTSCFG_TEST_EQ(timerOptions->oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(timerOptions->drift(), k_DRIFT);

    NTSCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTSCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTSCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Ensure the other time options still has its original value.

    NTSCFG_TEST_EQ(otherTimerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(otherTimerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(otherTimerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(otherTimerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTSCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyCopyAssignmentOperator)
{
    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle((void*)k_HANDLE);
    otherTimerOptions.setId(k_ID);
    otherTimerOptions.setOneShot(k_ONE_SHOT);
    otherTimerOptions.setDrift(k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTSCFG_TEST_EQ(otherTimerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(otherTimerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(otherTimerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(otherTimerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTSCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Assign the other timer options to the timer options.

    ntca::TimerOptions timerOptions;
    timerOptions = otherTimerOptions;

    // Ensure the copy-constructed timer options has the expected value.

    NTSCFG_TEST_EQ(timerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(timerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(timerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(timerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Ensure the other time options still has its original value.

    NTSCFG_TEST_EQ(otherTimerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(otherTimerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(otherTimerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(otherTimerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTSCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyMoveAssignmentOperator)
{
    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle((void*)k_HANDLE);
    otherTimerOptions.setId(k_ID);
    otherTimerOptions.setOneShot(k_ONE_SHOT);
    otherTimerOptions.setDrift(k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTSCFG_TEST_EQ(otherTimerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(otherTimerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(otherTimerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(otherTimerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTSCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Move-assign the other timer options to the timer options.

    ntca::TimerOptions timerOptions;
    timerOptions = bslmf::MovableRefUtil::move(otherTimerOptions);

    // Ensure the move-constructed timer options has the expected value.

    NTSCFG_TEST_EQ(timerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(timerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(timerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(timerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Ensure the other time options still has its original value.

    NTSCFG_TEST_EQ(otherTimerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(otherTimerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(otherTimerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(otherTimerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTSCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTSCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyReset)
{
    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions timerOptions;

    timerOptions.setHandle((void*)k_HANDLE);
    timerOptions.setId(k_ID);
    timerOptions.setOneShot(k_ONE_SHOT);
    timerOptions.setDrift(k_DRIFT);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTSCFG_TEST_EQ(timerOptions.handle(), k_HANDLE);
    NTSCFG_TEST_EQ(timerOptions.id(), k_ID);
    NTSCFG_TEST_EQ(timerOptions.oneShot(), k_ONE_SHOT);
    NTSCFG_TEST_EQ(timerOptions.drift(), k_DRIFT);

    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Reset the value of the timer options to its value upon default
    // construction.

    timerOptions.reset();

    // Ensure the timer options has the expected value.

    NTSCFG_TEST_EQ(timerOptions.handle(),
                   reinterpret_cast<void*>(static_cast<bsl::intptr_t>(0)));
    NTSCFG_TEST_EQ(timerOptions.id(), 0);
    NTSCFG_TEST_EQ(timerOptions.oneShot(), false);
    NTSCFG_TEST_EQ(timerOptions.drift(), false);

    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   true);
    NTSCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   true);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyMove)
{
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifySetHandle)
{
    ntca::TimerOptions timerOptions;

    NTSCFG_TEST_EQ(timerOptions.handle(),
                   reinterpret_cast<void*>(static_cast<bsl::intptr_t>(0)));

    timerOptions.setHandle((void*)k_HANDLE);

    NTSCFG_TEST_EQ(timerOptions.handle(), k_HANDLE);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifySetId)
{
    ntca::TimerOptions timerOptions;

    NTSCFG_TEST_EQ(timerOptions.id(), 0);

    timerOptions.setId(k_ID);

    NTSCFG_TEST_EQ(timerOptions.id(), k_ID);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifySetOneShot)
{
    ntca::TimerOptions timerOptions;

    NTSCFG_TEST_EQ(timerOptions.oneShot(), false);

    timerOptions.setOneShot(k_ONE_SHOT);

    NTSCFG_TEST_EQ(timerOptions.oneShot(), k_ONE_SHOT);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifySetDrift)
{
    ntca::TimerOptions timerOptions;

    NTSCFG_TEST_EQ(timerOptions.drift(), false);

    timerOptions.setDrift(k_DRIFT);

    NTSCFG_TEST_EQ(timerOptions.drift(), k_DRIFT);
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyShowEvent)
{
    ntca::TimerOptions timerOptions;

    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.hideEvent(ntca::TimerEventType::e_DEADLINE);

    NTSCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);

    NTSCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTSCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    NTSCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTSCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTSCFG_TEST_FALSE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTSCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTSCFG_TEST_FALSE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);

    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTSCFG_TEST_FALSE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTSCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyEquals)
{
    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        {
            bool result = timerOptions == otherTimerOptions;
            NTSCFG_TEST_EQ(result, true);
        }

        {
            bool result = timerOptions != otherTimerOptions;
            NTSCFG_TEST_EQ(result, false);
        }
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle((void*)k_HANDLE);
        otherTimerOptions.setId(k_ID);
        otherTimerOptions.setOneShot(k_ONE_SHOT);
        otherTimerOptions.setDrift(k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        {
            bool result = timerOptions == otherTimerOptions;
            NTSCFG_TEST_EQ(result, false);
        }

        {
            bool result = timerOptions != otherTimerOptions;
            NTSCFG_TEST_EQ(result, true);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyLess)
{
    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        {
            bool result = timerOptions < otherTimerOptions;
            NTSCFG_TEST_EQ(result, false);
        }
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle((void*)k_HANDLE);
        otherTimerOptions.setId(k_ID);
        otherTimerOptions.setOneShot(k_ONE_SHOT);
        otherTimerOptions.setDrift(k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        {
            bool result = timerOptions < otherTimerOptions;
            NTSCFG_TEST_EQ(result, true);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyHash)
{
    // Test hash member function with default hash algorithm.

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        bsl::size_t h1 = timerOptions.hash();
        bsl::size_t h2 = otherTimerOptions.hash();

        NTSCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle((void*)k_HANDLE);
        otherTimerOptions.setId(k_ID);
        otherTimerOptions.setOneShot(k_ONE_SHOT);
        otherTimerOptions.setDrift(k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        bsl::size_t h1 = timerOptions.hash();
        bsl::size_t h2 = otherTimerOptions.hash();

        NTSCFG_TEST_NE(h1, h2);
    }

    // Test hash member function with default hash algorithm.

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        bsl::size_t h1 = timerOptions.hash<bslh::DefaultHashAlgorithm>();
        bsl::size_t h2 = otherTimerOptions.hash<bslh::DefaultHashAlgorithm>();

        NTSCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle((void*)k_HANDLE);
        otherTimerOptions.setId(k_ID);
        otherTimerOptions.setOneShot(k_ONE_SHOT);
        otherTimerOptions.setDrift(k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        bsl::size_t h1 = timerOptions.hash<bslh::DefaultHashAlgorithm>();
        bsl::size_t h2 = otherTimerOptions.hash<bslh::DefaultHashAlgorithm>();

        NTSCFG_TEST_NE(h1, h2);
    }

    // Test using bslh::Hash.

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        bsl::size_t h1 = bslh::Hash<>()(timerOptions);
        bsl::size_t h2 = bslh::Hash<>()(otherTimerOptions);

        NTSCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle((void*)k_HANDLE);
        otherTimerOptions.setId(k_ID);
        otherTimerOptions.setOneShot(k_ONE_SHOT);
        otherTimerOptions.setDrift(k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        bsl::size_t h1 = bslh::Hash<>()(timerOptions);
        bsl::size_t h2 = bslh::Hash<>()(otherTimerOptions);

        NTSCFG_TEST_NE(h1, h2);
    }
}

NTSCFG_TEST_FUNCTION(ntca::TimerOptionsTest::verifyPrint)
{
    ntca::TimerOptions timerOptions;

    {
        bsl::stringstream ss;
        timerOptions.print(ss, 0, -1);
        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Timer options = " << ss.str() << bsl::endl;
        }
    }

    {
        bsl::stringstream ss;
        timerOptions.print(ss, 1, 4);
        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Timer options = " << ss.str() << bsl::endl;
        }
    }

    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle((void*)k_HANDLE);
    otherTimerOptions.setId(k_ID);
    otherTimerOptions.setOneShot(k_ONE_SHOT);
    otherTimerOptions.setDrift(k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    {
        bsl::stringstream ss;
        otherTimerOptions.print(ss, 0, -1);
        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Timer options = " << ss.str() << bsl::endl;
        }
    }

    {
        bsl::stringstream ss;
        otherTimerOptions.print(ss, 1, 4);
        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "Timer options = " << ss.str() << bsl::endl;
        }
    }
}

const void* TimerOptionsTest::k_HANDLE =
    reinterpret_cast<void*>(static_cast<bsl::intptr_t>(1));

const int  TimerOptionsTest::k_ID       = 2;
const bool TimerOptionsTest::k_ONE_SHOT = true;
const bool TimerOptionsTest::k_DRIFT    = true;

}  // close namespace ntca
}  // close namespace BloombergLP
