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

#include <ntcs_proactordetachcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_roactordetachcontext_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcs {

namespace {

// The socket is attached to its proactor and the user has not initiated a 
// detachment.
//
// 00000000 00000000 00000000 00000000
const unsigned int k_ATTACHED = 0;

// The user has initiated a detachment of the socket from its proactor, but the
// callback has not yet been invoked.
//
// 01000000 00000000 00000000 00000000
const unsigned int k_DETACHING = 1 << 30;

// The user has initiated a detachment of the socket from its proactor and the
// callback as been invoked (or enqueued onto a strand to be invoked
// asynchronously.)
//
// 10000000 00000000 00000000 00000000
const unsigned int k_DETACHED = 1 << 31;

// The mask of the bits to store the number of threads actively working on the
// socket.
//
// 00111111 11111111 11111111 11111111
const unsigned int k_COUNT_MASK = 0x3FFFFFFF;

// The mask of the bits used to store the detched state.
//
// 11000000 00000000 00000000 00000000
const unsigned int k_STATE_MASK = 0xC0000000;

} // close unnamed namespace

ProactorDetachContext::ProactorDetachContext()
: d_value()
{
}

ProactorDetachContext::~ProactorDetachContext()
{
}

bool ProactorDetachContext::incrementAndCheckNoDetach()
{
    unsigned int val = d_value.addAcqRel(1);
    if ((val & k_STATE_MASK) != k_ATTACHED) {
        return false;
    }
    return true;
}

bool ProactorDetachContext::decrementProcessCounterAndCheckDetachPossible()
{
    unsigned int val = d_value.subtractAcqRel(1);
    if ((val & k_COUNT_MASK) == 0 &&
        (val & k_STATE_MASK) == k_DETACHING)
    {
        unsigned int prev = d_value.testAndSwapAcqRel(
            val,
            k_DETACHED);
        return prev == val;
    }
    return false;
}

bool ProactorDetachContext::trySetDetachScheduled()
{
    unsigned int val = d_value.loadAcquire();
    if ((val & k_COUNT_MASK) == 0 &&
        (val & k_STATE_MASK) == k_DETACHING)
    {
        unsigned int prev = d_value.testAndSwapAcqRel(
            val,
            k_DETACHED);
        return prev == val;
    }
    return false;
}

bool ProactorDetachContext::trySetDetachRequired()
{
    while (true) {
        unsigned int current = d_value.loadAcquire();
        if ((current & k_STATE_MASK) != k_ATTACHED) {
            return false;
        }
        unsigned int prev = d_value.testAndSwapAcqRel(
            current,
            current | k_DETACHING);
        if (prev == current) {
            break;
        }
    }
    return true;
}



bool ProactorDetachContext::incrementReference()
{
    while (true) {
        unsigned int currentValue = d_value.load();
        unsigned int currentState = currentValue & k_STATE_MASK;
        unsigned int currentCount = currentValue & k_COUNT_MASK;

        if (currentState == k_ATTACHED) {
            if (update(currentValue, k_ATTACHED | (currentCount + 1))) {
                return true;
            }
        }
        else {
            return false;
        }
    }
}

bool ProactorDetachContext::decrementReference()
{
    while (true) {
        const unsigned int currentValue = d_value.load();
        const unsigned int currentState = currentValue & k_STATE_MASK;
        const unsigned int currentCount = currentValue & k_COUNT_MASK;

        if (currentState == k_ATTACHED) {
            BSLS_ASSERT(currentCount > 0);
            if (update(currentValue, k_ATTACHED | (currentCount - 1))) {
                return false;
            }
        }
        else if (currentState == k_DETACHING) {
            if (currentCount == 1) {
                if (update(currentValue, k_DETACHED)) {
                    return true;
                }
            }
            else {
                BSLS_ASSERT(currentCount > 1);
                if (update(currentValue, k_DETACHING | (currentCount - 1))) {
                    return false;
                }
            }
        }
        else {
            BSLS_ASSERT_OPT(currentState != k_DETACHED);
        }
    }
}

ntsa::Error ProactorDetachContext::detach()
{
    while (true) {
        const unsigned int currentValue = d_value.load();
        const unsigned int currentState = currentValue & k_STATE_MASK;
        const unsigned int currentCount = currentValue & k_COUNT_MASK;

        if (currentState == k_ATTACHED) {
            if (currentCount == 0) {
                if (update(currentValue, k_DETACHED)) {
                    return ntsa::Error();
                }
            }
            else {
                BSLS_ASSERT(currentCount > 0);
                if (update(currentValue, k_DETACHING | currentCount)) {
                    return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
                }
            }
        }
        else if (currentState == k_DETACHING) {
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            BSLS_ASSERT(currentState == k_DETACHED);
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

bool ProactorDetachContext::update(unsigned int oldValue, 
                                   unsigned int newValue)
{
    return d_value.testAndSwap(oldValue, newValue) == oldValue;
}

}  // close package namespace
}  // close enterprise namespace
