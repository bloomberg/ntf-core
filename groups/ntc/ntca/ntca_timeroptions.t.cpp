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

#include <ntca_timeroptions.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

namespace test {

void*      k_HANDLE   = reinterpret_cast<void*>(static_cast<bsl::intptr_t>(1));
const int  k_ID       = 2;
const bool k_ONE_SHOT = true;
const bool k_DRIFT    = true;

};

NTCCFG_TEST_CASE(1)
{
    // Test default constructor.

    ntca::TimerOptions timerOptions;

    NTCCFG_TEST_EQ(timerOptions.handle(),
                   reinterpret_cast<void*>(static_cast<bsl::intptr_t>(0)));
    NTCCFG_TEST_EQ(timerOptions.id(), 0);
    NTCCFG_TEST_EQ(timerOptions.oneShot(), false);
    NTCCFG_TEST_EQ(timerOptions.drift(), false);

    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   true);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   true);
}

NTCCFG_TEST_CASE(2)
{
    // Test copy constructor.

    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle(test::k_HANDLE);
    otherTimerOptions.setId(test::k_ID);
    otherTimerOptions.setOneShot(test::k_ONE_SHOT);
    otherTimerOptions.setDrift(test::k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTCCFG_TEST_EQ(otherTimerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(otherTimerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(otherTimerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(otherTimerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTCCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Copy-construct a timer options from the other timer options.

    char                buffer[sizeof(ntca::TimerOptions)];
    ntca::TimerOptions* timerOptions =
        new (buffer) ntca::TimerOptions(otherTimerOptions);

    // Ensure the copy-constructed timer options has the expected value.

    NTCCFG_TEST_EQ(timerOptions->handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(timerOptions->id(), test::k_ID);
    NTCCFG_TEST_EQ(timerOptions->oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(timerOptions->drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTCCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTCCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Ensure the other time options still has its original value.

    NTCCFG_TEST_EQ(otherTimerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(otherTimerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(otherTimerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(otherTimerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTCCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);
}

NTCCFG_TEST_CASE(3)
{
    // Test move constructor.

    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle(test::k_HANDLE);
    otherTimerOptions.setId(test::k_ID);
    otherTimerOptions.setOneShot(test::k_ONE_SHOT);
    otherTimerOptions.setDrift(test::k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTCCFG_TEST_EQ(otherTimerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(otherTimerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(otherTimerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(otherTimerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTCCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Move-construct a timer options from the other timer options.

    char                buffer[sizeof(ntca::TimerOptions)];
    ntca::TimerOptions* timerOptions = new (buffer)
        ntca::TimerOptions(bslmf::MovableRefUtil::move(otherTimerOptions));

    // Ensure the move-constructed timer options has the expected value.

    NTCCFG_TEST_EQ(timerOptions->handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(timerOptions->id(), test::k_ID);
    NTCCFG_TEST_EQ(timerOptions->oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(timerOptions->drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTCCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTCCFG_TEST_EQ(timerOptions->wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Ensure the other time options still has its original value.

    NTCCFG_TEST_EQ(otherTimerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(otherTimerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(otherTimerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(otherTimerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTCCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);
}

NTCCFG_TEST_CASE(4)
{
    // Test assignment operator.

    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle(test::k_HANDLE);
    otherTimerOptions.setId(test::k_ID);
    otherTimerOptions.setOneShot(test::k_ONE_SHOT);
    otherTimerOptions.setDrift(test::k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTCCFG_TEST_EQ(otherTimerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(otherTimerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(otherTimerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(otherTimerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTCCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Assign the other timer options to the timer options.

    ntca::TimerOptions timerOptions;
    timerOptions = otherTimerOptions;

    // Ensure the copy-constructed timer options has the expected value.

    NTCCFG_TEST_EQ(timerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(timerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(timerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(timerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Ensure the other time options still has its original value.

    NTCCFG_TEST_EQ(otherTimerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(otherTimerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(otherTimerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(otherTimerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTCCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);
}

NTCCFG_TEST_CASE(5)
{
    // Test move-assignment operator.

    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle(test::k_HANDLE);
    otherTimerOptions.setId(test::k_ID);
    otherTimerOptions.setOneShot(test::k_ONE_SHOT);
    otherTimerOptions.setDrift(test::k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTCCFG_TEST_EQ(otherTimerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(otherTimerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(otherTimerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(otherTimerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTCCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Move-assign the other timer options to the timer options.

    ntca::TimerOptions timerOptions;
    timerOptions = bslmf::MovableRefUtil::move(otherTimerOptions);

    // Ensure the move-constructed timer options has the expected value.

    NTCCFG_TEST_EQ(timerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(timerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(timerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(timerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Ensure the other time options still has its original value.

    NTCCFG_TEST_EQ(otherTimerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(otherTimerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(otherTimerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(otherTimerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
        true);
    NTCCFG_TEST_EQ(
        otherTimerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
        false);
    NTCCFG_TEST_EQ(otherTimerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);
}

NTCCFG_TEST_CASE(6)
{
    // Test 'reset'.

    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions timerOptions;

    timerOptions.setHandle(test::k_HANDLE);
    timerOptions.setId(test::k_ID);
    timerOptions.setOneShot(test::k_ONE_SHOT);
    timerOptions.setDrift(test::k_DRIFT);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    // Ensure the other timer options has the expected value.

    NTCCFG_TEST_EQ(timerOptions.handle(), test::k_HANDLE);
    NTCCFG_TEST_EQ(timerOptions.id(), test::k_ID);
    NTCCFG_TEST_EQ(timerOptions.oneShot(), test::k_ONE_SHOT);
    NTCCFG_TEST_EQ(timerOptions.drift(), test::k_DRIFT);

    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   false);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   false);

    // Reset the value of the timer options to its value upon default
    // construction.

    timerOptions.reset();

    // Ensure the timer options has the expected value.

    NTCCFG_TEST_EQ(timerOptions.handle(),
                   reinterpret_cast<void*>(static_cast<bsl::intptr_t>(0)));
    NTCCFG_TEST_EQ(timerOptions.id(), 0);
    NTCCFG_TEST_EQ(timerOptions.oneShot(), false);
    NTCCFG_TEST_EQ(timerOptions.drift(), false);

    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE),
                   true);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED),
                   true);
    NTCCFG_TEST_EQ(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED),
                   true);
}

NTCCFG_TEST_CASE(7)
{
    // Test 'move'.
}

NTCCFG_TEST_CASE(8)
{
    // Test 'setHandle'.

    ntca::TimerOptions timerOptions;

    NTCCFG_TEST_EQ(timerOptions.handle(),
                   reinterpret_cast<void*>(static_cast<bsl::intptr_t>(0)));

    timerOptions.setHandle(test::k_HANDLE);

    NTCCFG_TEST_EQ(timerOptions.handle(), test::k_HANDLE);
}

NTCCFG_TEST_CASE(9)
{
    // Test 'setId'.

    ntca::TimerOptions timerOptions;

    NTCCFG_TEST_EQ(timerOptions.id(), 0);

    timerOptions.setId(test::k_ID);

    NTCCFG_TEST_EQ(timerOptions.id(), test::k_ID);
}

NTCCFG_TEST_CASE(10)
{
    // Test 'setOneShot'.

    ntca::TimerOptions timerOptions;

    NTCCFG_TEST_EQ(timerOptions.oneShot(), false);

    timerOptions.setOneShot(test::k_ONE_SHOT);

    NTCCFG_TEST_EQ(timerOptions.oneShot(), test::k_ONE_SHOT);
}

NTCCFG_TEST_CASE(11)
{
    // Test 'setDrift'.

    ntca::TimerOptions timerOptions;

    NTCCFG_TEST_EQ(timerOptions.drift(), false);

    timerOptions.setDrift(test::k_DRIFT);

    NTCCFG_TEST_EQ(timerOptions.drift(), test::k_DRIFT);
}

NTCCFG_TEST_CASE(12)
{
    // Test 'showEvent', 'hideEvent', and 'wantEvent'.

    ntca::TimerOptions timerOptions;

    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.hideEvent(ntca::TimerEventType::e_DEADLINE);

    NTCCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);

    NTCCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTCCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    NTCCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTCCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTCCFG_TEST_FALSE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);

    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTCCFG_TEST_FALSE(
        timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTCCFG_TEST_FALSE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);

    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTCCFG_TEST_FALSE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));

    timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);

    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_DEADLINE));
    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CANCELED));
    NTCCFG_TEST_TRUE(timerOptions.wantEvent(ntca::TimerEventType::e_CLOSED));
}

NTCCFG_TEST_CASE(13)
{
    // Test 'equals'.

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        {
            bool result = timerOptions == otherTimerOptions;
            NTCCFG_TEST_EQ(result, true);
        }

        {
            bool result = timerOptions != otherTimerOptions;
            NTCCFG_TEST_EQ(result, false);
        }
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle(test::k_HANDLE);
        otherTimerOptions.setId(test::k_ID);
        otherTimerOptions.setOneShot(test::k_ONE_SHOT);
        otherTimerOptions.setDrift(test::k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        {
            bool result = timerOptions == otherTimerOptions;
            NTCCFG_TEST_EQ(result, false);
        }

        {
            bool result = timerOptions != otherTimerOptions;
            NTCCFG_TEST_EQ(result, true);
        }
    }
}

NTCCFG_TEST_CASE(14)
{
    // Test 'less'.

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        {
            bool result = timerOptions < otherTimerOptions;
            NTCCFG_TEST_EQ(result, false);
        }
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle(test::k_HANDLE);
        otherTimerOptions.setId(test::k_ID);
        otherTimerOptions.setOneShot(test::k_ONE_SHOT);
        otherTimerOptions.setDrift(test::k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        {
            bool result = timerOptions < otherTimerOptions;
            NTCCFG_TEST_EQ(result, true);
        }
    }
}

NTCCFG_TEST_CASE(15)
{
    // Test 'hash'.

    // Test hash member function with default hash algorithm.

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        bsl::size_t h1 = timerOptions.hash();
        bsl::size_t h2 = otherTimerOptions.hash();

        NTCCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle(test::k_HANDLE);
        otherTimerOptions.setId(test::k_ID);
        otherTimerOptions.setOneShot(test::k_ONE_SHOT);
        otherTimerOptions.setDrift(test::k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        bsl::size_t h1 = timerOptions.hash();
        bsl::size_t h2 = otherTimerOptions.hash();

        NTCCFG_TEST_NE(h1, h2);
    }

    // Test hash member function with default hash algorithm.

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        bsl::size_t h1 = timerOptions.hash<bslh::DefaultHashAlgorithm>();
        bsl::size_t h2 = otherTimerOptions.hash<bslh::DefaultHashAlgorithm>();

        NTCCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle(test::k_HANDLE);
        otherTimerOptions.setId(test::k_ID);
        otherTimerOptions.setOneShot(test::k_ONE_SHOT);
        otherTimerOptions.setDrift(test::k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        bsl::size_t h1 = timerOptions.hash<bslh::DefaultHashAlgorithm>();
        bsl::size_t h2 = otherTimerOptions.hash<bslh::DefaultHashAlgorithm>();

        NTCCFG_TEST_NE(h1, h2);
    }

    // Test using bslh::Hash.

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        bsl::size_t h1 = bslh::Hash<>()(timerOptions);
        bsl::size_t h2 = bslh::Hash<>()(otherTimerOptions);

        NTCCFG_TEST_EQ(h1, h2);
    }

    {
        ntca::TimerOptions timerOptions;
        ntca::TimerOptions otherTimerOptions;

        otherTimerOptions.setHandle(test::k_HANDLE);
        otherTimerOptions.setId(test::k_ID);
        otherTimerOptions.setOneShot(test::k_ONE_SHOT);
        otherTimerOptions.setDrift(test::k_DRIFT);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
        otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

        bsl::size_t h1 = bslh::Hash<>()(timerOptions);
        bsl::size_t h2 = bslh::Hash<>()(otherTimerOptions);

        NTCCFG_TEST_NE(h1, h2);
    }
}

NTCCFG_TEST_CASE(16)
{
    // Test 'print'.

    ntca::TimerOptions timerOptions;

    {
        bsl::stringstream ss;
        timerOptions.print(ss, 0, -1);
        if (NTCCFG_TEST_VERBOSITY) {
            bsl::cout << "Timer options = " << ss.str() << bsl::endl;
        }
    }

    {
        bsl::stringstream ss;
        timerOptions.print(ss, 1, 4);
        if (NTCCFG_TEST_VERBOSITY) {
            bsl::cout << "Timer options = " << ss.str() << bsl::endl;
        }
    }

    // Define other timer options that does not have the same value as a
    // default-constructed timer options.

    ntca::TimerOptions otherTimerOptions;

    otherTimerOptions.setHandle(test::k_HANDLE);
    otherTimerOptions.setId(test::k_ID);
    otherTimerOptions.setOneShot(test::k_ONE_SHOT);
    otherTimerOptions.setDrift(test::k_DRIFT);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    otherTimerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);

    {
        bsl::stringstream ss;
        otherTimerOptions.print(ss, 0, -1);
        if (NTCCFG_TEST_VERBOSITY) {
            bsl::cout << "Timer options = " << ss.str() << bsl::endl;
        }
    }

    {
        bsl::stringstream ss;
        otherTimerOptions.print(ss, 1, 4);
        if (NTCCFG_TEST_VERBOSITY) {
            bsl::cout << "Timer options = " << ss.str() << bsl::endl;
        }
    }
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);  // Test Default constructor
    NTCCFG_TEST_REGISTER(2);  // Test copy constructor
    NTCCFG_TEST_REGISTER(3);  // Test cove constructor

    NTCCFG_TEST_REGISTER(4);  // Test assignment operator
    NTCCFG_TEST_REGISTER(5);  // Test move-assignment operator

    NTCCFG_TEST_REGISTER(6);  // Test 'reset'
    NTCCFG_TEST_REGISTER(7);  // Test 'move'

    NTCCFG_TEST_REGISTER(8);   // Test 'setHandle'
    NTCCFG_TEST_REGISTER(9);   // Test 'setId'
    NTCCFG_TEST_REGISTER(10);  // Test 'setOneShot'
    NTCCFG_TEST_REGISTER(11);  // Test 'setDrift'

    NTCCFG_TEST_REGISTER(
        12);  // Test 'showEvent', 'hideEvent', and 'wantEvent'

    NTCCFG_TEST_REGISTER(13);  // Test 'equals'
    NTCCFG_TEST_REGISTER(14);  // Test 'less'
    NTCCFG_TEST_REGISTER(15);  // Test 'hash'
    NTCCFG_TEST_REGISTER(16);  // Test 'print'
}
NTCCFG_TEST_DRIVER_END;
