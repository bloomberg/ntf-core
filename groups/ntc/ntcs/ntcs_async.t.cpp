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
BSLS_IDENT_RCSID(ntcs_async_t_cpp, "$Id$ $CSID$")

#include <ntcs_async.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Async'.
class AsyncTest
{
    // Process the execution of a deferred function.
    static void processFunction(bslmt::Semaphore* semaphore);

    // Process the specified 'event' for the specified 'timer'.
    static void processTimer(bslmt::Semaphore*                   semaphore,
                             const bsl::shared_ptr<ntci::Timer>& timer,
                             const ntca::TimerEvent&             event);

  public:
    // TODO
    static void verifyFunction();

    // TODO
    static void verifyStrand();

    // TODO
    static void verifyTimer();
};

void AsyncTest::processFunction(bslmt::Semaphore* semaphore)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing function" << NTCI_LOG_STREAM_END;

    semaphore->post();
}

void AsyncTest::processTimer(bslmt::Semaphore*                   semaphore,
                             const bsl::shared_ptr<ntci::Timer>& timer,
                             const ntca::TimerEvent&             event)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Processing timer event " << event
                          << NTCI_LOG_STREAM_END;

    semaphore->post();
}

NTSCFG_TEST_FUNCTION(ntcs::AsyncTest::verifyFunction)
{
    bslmt::Semaphore semaphore;

    ntcs::Async::execute(NTCCFG_BIND(&AsyncTest::processFunction, &semaphore));

    semaphore.wait();
}

NTSCFG_TEST_FUNCTION(ntcs::AsyncTest::verifyStrand)
{
    bslmt::Semaphore semaphore;

    bsl::shared_ptr<ntci::Strand> strand = ntcs::Async::createStrand();

    strand->execute(NTCCFG_BIND(&AsyncTest::processFunction, &semaphore));

    semaphore.wait();
}

NTSCFG_TEST_FUNCTION(ntcs::AsyncTest::verifyTimer)
{
    bslmt::Semaphore semaphore;

    ntca::TimerOptions timerOptions;
    timerOptions.setId(1);
    timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
    timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
    timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
    timerOptions.setOneShot(true);

    ntci::TimerCallback timerCallback(NTCCFG_BIND(&AsyncTest::processTimer,
                                                  &semaphore,
                                                  NTCCFG_BIND_PLACEHOLDER_1,
                                                  NTCCFG_BIND_PLACEHOLDER_2));

    bsl::shared_ptr<ntci::Timer> timer =
        ntcs::Async::createTimer(timerOptions, timerCallback);

    timer->schedule(timer->currentTime() + bsls::TimeInterval(1));

    semaphore.wait();
}

}  // close namespace ntcs
}  // close namespace BloombergLP
